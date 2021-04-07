#include "ImgProcess.h"
#include "FnCore/Log_xx.h"
namespace IMat
{
	//卷积
#if defined(WIN32)
#include "intrin.h"
	template<typename T>
	int blockFilter(T* pTemp, T* pDst, __m256* avx_Ker, int img_h_start, int img_h_end, int img_w, int up, int down, int left, int right, int k_w, int kerSize_n)
	{
		int tempw = left + right + img_w;
		T* pTempF = pTemp + tempw * up + left;
		T* col = new T[kerSize_n]();    //长度做到8字节的倍数
		float accury = (typeid(T) == typeid(float) || typeid(T) == typeid(double)) ? 0 : 0.5f;
		if (typeid(T) == typeid(float))
		{
			for (int y = img_h_start; y < img_h_end; ++y)
			{
				for (int x = 0; x < img_w; ++x)
				{
					for (int yy = y - up, k_ind = 0; yy <= y + down; ++yy, k_ind++)
						memcpy(col + k_ind * k_w, pTempF + yy * tempw + x - left, k_w * sizeof(T));
					__m256 avx_sum = _mm256_setzero_ps();
					for (int i = 0, len = kerSize_n / 8; i < len; ++i)
					{
						__m256 t = _mm256_loadu_ps(col + 8 * i);
						avx_sum = _mm256_add_ps(avx_sum, _mm256_mul_ps(t, avx_Ker[i]));
					}
					T* p = (T*)&avx_sum;
					pDst[y * img_w + x] = p[0] + p[1] + p[2] + p[3] + p[4] + p[5] + p[6] + p[7] + accury;
				}
			}
		}
		delete[]col;
		return 1;
	}

	template<typename T>
	int filter_2D(T* pSrc, T* pDst, int img_w, int img_h, T* pKer, int k_w, int k_h)
	{
		using namespace std;
		if (pSrc == 0 || pDst == 0) return -1;
		int up = k_h / 2;
		int down = k_h - up - 1;
		int left = k_w / 2;
		int right = k_w - left - 1;
		//开辟一块大内存,便于复制
		int temph = up + down + img_h;
		int tempw = left + right + img_w;
		T* pTemp = 0;
		copyBorder<T>(pSrc, pTemp, img_h, img_w, up, down, left, right);

		T* pTempF = pTemp + tempw * up + left;
		int kerSize = k_h * k_w;

		int kerSize_n = kerSize % 8 == 0 ? kerSize : kerSize / 8 * 8 + 8;//kerSize大于0时，kerSize_n是kerSize向上取整到最小的8的倍数,	
		T* pKer_n = (T*)_aligned_malloc(kerSize_n * sizeof(float), 32);
		memcpy(pKer_n, pKer, sizeof(T) * kerSize);
		__m256* avx_Ker = (__m256*) _aligned_malloc(kerSize_n * sizeof(float), 32);
		for (int i = 0; i < kerSize_n / 8; ++i)
			avx_Ker[i] = _mm256_load_ps(pKer_n + 8 * i);
		int nblocks = min(omp_get_num_procs(), img_h);   //设置分块数，便于设置线程
#pragma omp parallel for
		for (int i = 0; i < nblocks; ++i)
			blockFilter<T>(pTemp, pDst, avx_Ker, img_h * i / nblocks, img_h * (i + 1) / nblocks, img_w, up, down, left, right, k_w, kerSize_n);
		_mm_free(pKer_n);
		_mm_free(avx_Ker);
		delete pTemp;
		return 1;
	}
#endif // 

	
	//判断矩阵的秩是否为1,如果矩阵的秩为1，则可以分解成n*1竖矩阵和1*n的横矩阵
	int SeparatingMatrix(float* mat, int w, int h, float*& vmat, float*& hmat)
	{
		if (mat == NULL)
		{
			if (vmat != NULL)
				delete[] vmat;
			if (hmat != NULL)
				delete[] hmat;
			vmat = NULL;
			hmat = NULL;
			return 0;
		}
		if (vmat) delete[] vmat;
		if (hmat) delete[]hmat;
		vmat = NULL;
		hmat = NULL;
		bool rankIsNotOne = false;
		do
		{
			//先找第一组不为0的基；
			int y = 0;
			bool hasFindBaseVec = false;
			for (; y < h; ++y)
			{
				float* pRow = mat + y * w;
				for (int x = 0; x < w; ++x)
				{
					if (pRow[0] != 0)
					{
						hasFindBaseVec = true;
						break;
					}
				}
				if (hasFindBaseVec == true)
					break;
			}
			//矩阵所有元素都是0，那么矩阵秩为0
			if (y == w)
			{
				rankIsNotOne = true;
				break;
			}
			vmat = new float[h];
			hmat = new float[w];
			//竖矩阵 0,0,0,...1,?,?,?;  横矩阵: 第一组基(x1,x2,x3,....xw)),下面求解"？"处参数
			memset(vmat, 0, sizeof(float) * h);
			vmat[y] = 1.0f;
			memcpy(hmat, mat + y * w, sizeof(float) * w);
			y++;

			for (; y < w; ++y)
			{
				float* pRow = mat + y * w;
				float cof = 0.0f;
				int noz = 0;
				for (; noz < w; ++noz)
				{
					if (pRow[noz] != 0)
						break;
				}
				for (int x = noz; x < w; ++x)
				{
					//对应元素都为0，则判断下一个元素
					if (pRow[x] == 0 && hmat[x] == 0)
						continue;
					//对应元素一个为0，一个不为0，则为两个基，那么矩阵秩不为1
					else if (pRow[x] == 0 && hmat[x] != 0 || pRow[x] != 0 && hmat[x] == 0)
					{
						rankIsNotOne = true;
						break;
					}
					//对应元素不为0，两个基是否线性无关，线性无关的话，那么矩阵的秩不为1
					else
					{
						float lambda = pRow[x] / hmat[x];  //这里排除为0的情况了
						if (cof == 0.0f)
							cof = lambda;
						else
						{
							if (abs((cof - lambda) / lambda) > 1e-5)
							{
								rankIsNotOne = true;
								break;
							}
						}
					}
				}
				vmat[y] = cof;
				if (rankIsNotOne == true)
					break;
			}
		} while (false);
		if (rankIsNotOne == true)
		{
			if (vmat)
				delete[] vmat;
			if (hmat)
				delete[]hmat;
			vmat = NULL;
			hmat = NULL;
		}
		return !rankIsNotOne;
	}

	//常规卷积，最原始的方案，还未做相关优化
	extern "C" FNNOEXPORT int ImageConv(float* psrc, float* pdst, float* kernel, int img_w, int img_h, int k_w, int k_h, bool* mask)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		if (kernel == NULL) return -3;
		if (img_w <= 0 || img_h <= 0 || k_w <= 0 || k_h <= 0) return -4;

		bool isSituOperation = false;
		float* temp = NULL;
		float* vmat = NULL, * hmat = NULL;
		int ret = 1;
		do
		{
			//先判断卷积核是否秩为1，如果为1，可以对卷积核进行拆分	
			if (k_w > 1 && k_h > 1)
			{
				int split_res = SeparatingMatrix(kernel, k_w, k_h, vmat, hmat);
				if (split_res == 1)
				{
					ImageConv(psrc, pdst, vmat, img_w, img_h, 1, k_h);
					ImageConv(pdst, pdst, hmat, img_w, img_h, k_w, 1);
					break;
				}
			}
			int up_expand = k_h / 2;
			int down_expand = k_h - up_expand - 1;
			int left_expand = k_w / 2;
			int right_expand = k_w - left_expand - 1;
			/*注意这里，先传图像高，再传图像宽*/
			ret = copyBorder(psrc, temp, img_h, img_w, up_expand, down_expand, left_expand, right_expand);
			if (ret != 1) break;
			int t_width = img_w + k_w - 1;
			float* pTemp = temp + t_width * up_expand + left_expand;
			if (mask == NULL)
			{
				for (int y = 0; y < img_h; ++y)
				{
					for (int x = 0; x < img_w; ++x)
					{
						int begin_y = y - up_expand;
						int end_y = y + down_expand;
						int begin_x = x - left_expand;
						int end_x = x + right_expand;
						int k_ind = 0;
						float val = 0.0f;
						for (int yy = begin_y; yy <= end_y; ++yy)
						{
							for (int xx = begin_x; xx <= end_x; ++xx)
							{
								int t_ind = yy * t_width + xx;
								val += (kernel[k_ind] * pTemp[t_ind]);
								k_ind++;
							}
						}
						int ind = y * img_w + x;
						pdst[ind] = val;
					}
				}
			}
			else
			{
				for (int y = 0; y < img_h; ++y)
				{
					for (int x = 0; x < img_w; ++x)
					{
						int ind = y * img_w + x;
						if (mask[ind] == 0)
						{
							pdst[ind] = psrc[ind];
							continue;
						}
						int begin_y = y - up_expand;
						int end_y = y + down_expand;
						int begin_x = x - left_expand;
						int end_x = x + right_expand;
						int k_ind = 0;
						float val = 0.0f;
						for (int yy = begin_y; yy <= end_y; ++yy)
						{
							for (int xx = begin_x; xx <= end_x; ++xx)
							{
								int t_ind = yy * t_width + xx;
								val += (kernel[k_ind] * pTemp[t_ind]);
								k_ind++;
							}
						}
						pdst[ind] = val;
					}
				}
			}
		} while (false);
		if (temp != NULL)
			delete[] temp;
		if (vmat != NULL)
			delete[]vmat;
		if (hmat != NULL)
			delete[]hmat;
		return ret;
	}

	//图像边缘拓展
	extern "C" FNNOEXPORT int MatPad(Mat & src, Mat & dst, int up, int down, int left, int right)
	{
		if (src.data == nullptr)
			return -1;
		PtrType pt = GetPtrType(src.m_Format);
		int img_w = src.cols;
		int img_h = src.rows;
		int padw = src.cols + left + right;
		int padh = src.rows + up + down;
		dst = Mat(padh, padw, src.m_Format);
		if (pt == PtrType::S8)
		{
			char* psrc = (char*)src.data;
			char* pdst = (char*)dst.data;
			return copyBorder(psrc, pdst, img_h, img_w, up, down, left, right);
		}
		else if (pt == PtrType::U8)
		{
			uint8_t* psrc = (uint8_t*)src.data;
			uint8_t* pdst = (uint8_t*)dst.data;
			return copyBorder(psrc, pdst, img_h, img_w, up, down, left, right);
		}
		else if (pt == PtrType::S16)
		{
			short* psrc = (short*)src.data;
			short* pdst = (short*)dst.data;
			return copyBorder(psrc, pdst, img_h, img_w, up, down, left, right);
		}
		else if (pt == PtrType::U16)
		{
			uint16_t* psrc = (uint16_t*)src.data;
			uint16_t* pdst = (uint16_t*)dst.data;
			return copyBorder(psrc, pdst, img_h, img_w, up, down, left, right);
		}
		else if (pt == PtrType::S32)
		{
			int* psrc = (int*)src.data;
			int* pdst = (int*)dst.data;
			return copyBorder(psrc, pdst, img_h, img_w, up, down, left, right);
		}
		else if (pt == PtrType::U32)
		{
			uint32_t* psrc = (uint32_t*)src.data;
			uint32_t* pdst = (uint32_t*)dst.data;
			return copyBorder(psrc, pdst, img_h, img_w, up, down, left, right);
		}
		else if (pt == PtrType::S64)
		{
			int64_t* psrc = (int64_t*)src.data;
			int64_t* pdst = (int64_t*)dst.data;
			return copyBorder(psrc, pdst, img_h, img_w, up, down, left, right);
		}
		else if (pt == PtrType::U64)
		{
			uint64_t* psrc = (uint64_t*)src.data;
			uint64_t* pdst = (uint64_t*)dst.data;
			return copyBorder(psrc, pdst, img_h, img_w, up, down, left, right);
		}
		else if (pt == PtrType::F32)
		{
			float* psrc = (float*)src.data;
			float* pdst = (float*)dst.data;
			return copyBorder(psrc, pdst, img_h, img_w, up, down, left, right);
		}
		else if (pt == PtrType::F64)
		{
			double* psrc = (double*)src.data;
			double* pdst = (double*)dst.data;
			return copyBorder(psrc, pdst, img_h, img_w, up, down, left, right);
		}
		return 1;
	}

	template<typename T>
	int DoConv2D_Int(T* padd, int64_t* pdst, int64_t* ker, int64_t padw, int64_t padh, int64_t dstw, int64_t dsth,
		Vec4l vc)
	{
		if (padd == nullptr)
		{
			PrintInfo("DoConv2D_Int: padd is nullptr");
			return -1;
		}
		if (pdst == nullptr)
		{
			PrintInfo("DoConv2D_Int: pdst is nullptr");
			return -2;
		}

		if (ker == nullptr)
		{
			PrintInfo("DoConv2D_Int: ker is nullptr");
			return -2;
		}

		memset(pdst, 0, sizeof(int64_t) * dstw * dsth);

		int64_t left = vc.left;
		int64_t up = vc.up;
		int64_t right = vc.right;
		int64_t down = vc.down;

		T* conv_start = padd + up * padw + left;  //边缘拓展后的卷积对应点其实点，在第up行，第left列
		int64_t ker_w = left + right + 1;
		int64_t ker_h = up + down + 1;
		int64_t* pAnchor = ker + up * ker_w + left;
		int64_t img_w = dstw;
		int64_t img_h = dsth;
		for (int64_t ky = -up; ky <= down; ++ky)
		{
			for (int64_t kx = -left; kx <= right; ++kx)
			{
				int64_t pad_offset = ky * padw + kx;
				int64_t ker_offset = ky * ker_w + kx;
				int64_t kerval = pAnchor[ker_offset];
				for (int64_t iy = 0; iy < img_h; ++iy)
				{
					int64_t* pdst_row = pdst + iy * img_w;
					T* padd_raw = conv_start + (iy + ky) * padw + kx;
					for (int64_t ix = 0; ix < img_w; ++ix)
					{
						pdst_row[ix] += (int64_t)padd_raw[ix] * kerval;
					}
				}
			}
		}
		return 1;
	}

	template<typename T>
	int DoConv2D_Float(T* padd, double* pdst, double* ker, int64_t padw, int64_t padh, int64_t dstw, int64_t dsth,
		Vec4l vc)
	{
		if (padd == nullptr)
		{
			PrintInfo("DoConv2D_Float: padd is nullptr");
			return -1;
		}
		if (pdst == nullptr)
		{
			PrintInfo("DoConv2D_Float: pdst is nullptr");
			return -2;
		}

		if (ker == nullptr)
		{
			PrintInfo("DoConv2D_Float: ker is nullptr");
			return -2;
		}

		memset(pdst, 0, sizeof(int64_t) * dstw * dsth);

		int64_t left = vc.left;
		int64_t up = vc.up;
		int64_t right = vc.right;
		int64_t down = vc.down;

		T* conv_start = padd + up * padw + left;  //边缘拓展后的卷积对应点其实点，在第up行，第left列
		int64_t ker_w = left + right + 1;
		int64_t ker_h = up + down + 1;
		double* pAnchor = ker + up * ker_w + left;
		int64_t img_w = dstw;
		int64_t img_h = dsth;
		for (int64_t ky = -up; ky <= down; ++ky)
		{
			for (int64_t kx = -left; kx <= right; ++kx)
			{
				int64_t pad_offset = ky * padw + kx;
				int64_t ker_offset = ky * ker_w + kx;
				double kerval = pAnchor[ker_offset];
				for (int64_t iy = 0; iy < img_h; ++iy)
				{
					double* pdst_row = pdst + iy * img_w;
					T* padd_raw = conv_start + (iy + ky) * padw + kx;
					for (int64_t ix = 0; ix < img_w; ++ix)
					{
						pdst_row[ix] += (double)padd_raw[ix] * kerval;
					}
				}
			}
		}
		return 1;
	}



	extern "C" FNNOEXPORT int Conv2D(IMat::Mat & src, IMat::Mat & dst, IMat::Mat & ker, FnPoint2i anchorpt, uint32_t stride, PaddType pad)
	{
		if (src.data == nullptr)
		{
			PrintInfo("Conv2D:: src is nullptr");
			return -1;
		}

		if (ker.data == nullptr)
		{
			PrintInfo("Conv2D: ker is nullptr");
			return -3;
		}

		if (pad == PaddType::SAME)
		{
			Mat padd;
			int left = ker.cols / 2, up = ker.cols / 2, right = ker.rows / 2, down = ker.rows / 2;
			if (!(anchorpt.x < 0 || anchorpt.y < 0))
			{
				left = anchorpt.x;
				right = ker.cols - 1 - left;
				up = anchorpt.y;
				down = ker.rows - 1 - up;
			}
			MatPad(src, padd, up, down, left, right);
			int img_w = src.cols;
			int img_h = src.rows;
			int dst_w = src.cols;
			int dst_h = src.rows;
			int padd_w = padd.cols;
			int padd_h = padd.rows;
			Vec4l vc(left, up, right, down);

			if (src.IsFloat() == false)
			{
				Mat dst_temp = Mat(src.rows, src.cols, PixelForm::S64);
				PixelForm src_format = src.m_Format;
				int ret = 0;
				if (src_format == PixelForm::S8)
					ret = DoConv2D_Int<char>((char*)(padd.data), (int64_t*)dst_temp.data, (int64_t*)ker.data, padd_w, padd_h, dst_w, dst_h, vc);
				else if (src_format == PixelForm::U8)
					ret = DoConv2D_Int<uint8_t>((uint8_t*)(padd.data), (int64_t*)dst_temp.data, (int64_t*)ker.data, padd_w, padd_h, dst_w, dst_h, vc);
				else if (src_format == PixelForm::S16)
					ret = DoConv2D_Int<short>((short*)(padd.data), (int64_t*)dst_temp.data, (int64_t*)ker.data, padd_w, padd_h, dst_w, dst_h, vc);
				else if (src_format == PixelForm::U16)
					ret = DoConv2D_Int<uint16_t>((uint16_t*)(padd.data), (int64_t*)dst_temp.data, (int64_t*)ker.data, padd_w, padd_h, dst_w, dst_h, vc);
				else if (src_format == PixelForm::S32)
					ret = DoConv2D_Int<int>((int*)(padd.data), (int64_t*)dst_temp.data, (int64_t*)ker.data, padd_w, padd_h, dst_w, dst_h, vc);
				else if (src_format == PixelForm::U32)
					ret = DoConv2D_Int<uint32_t>((uint32_t*)(padd.data), (int64_t*)dst_temp.data, (int64_t*)ker.data, padd_w, padd_h, dst_w, dst_h, vc);
				else if (src_format == PixelForm::S64)
					ret = DoConv2D_Int<int64_t>((int64_t*)(padd.data), (int64_t*)dst_temp.data, (int64_t*)ker.data, padd_w, padd_h, dst_w, dst_h, vc);
				else if (src_format == PixelForm::U64)
					ret = DoConv2D_Int<uint64_t>((uint64_t*)(padd.data), (int64_t*)dst_temp.data, (int64_t*)ker.data, padd_w, padd_h, dst_w, dst_h, vc);
				if (src.data != dst.data)
					dst = dst_temp.Clone();
				else
					memcpy(src.data, dst_temp.data, src.imgByteSize);
			}


		}
		return 1;
	}
};