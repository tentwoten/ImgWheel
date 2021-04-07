#include"ImgProcess.h"
namespace IMat
{
	template<typename T>
	int DoFastGaussFilter(T* psrc, T* pdst, int w, int h, double sigma)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;

		double q;
		if (sigma >= 0.5 && sigma <= 2.5)
			q = 3.97156 - 4.14544 * sqrt(1 - 0.26891 * sigma);
		else
			q = 0.98711 * sigma - 0.96330;
		float b0 = (float)(1.57825 + (2.44413 * q) + (1.4281 * q * q) + 0.422205 * q * q * q);
		float b1 = (float)((2.44413 * q) + (2.85619 * q * q) + (1.26661 * q * q * q));
		float b2 = (float)(-(1.4281 * q * q + 1.26661 * q * q * q));
		float b3 = (float)(0.422205 * q * q * q);
		float B = (float)(1.0 - (b1 + b2 + b3) / b0);
		int nw = w + 3;
		//先横向滤波
		float* w1 = new float[nw];
		float* w2 = new float[nw];
		float b1b0 = b1 / b0;
		float b2b0 = b2 / b0;
		float b3b0 = b3 / b0;
		for (int y = 0; y < h; ++y)
		{
			T* psrc_row = psrc + y * w;
			T* pdst_row = pdst + y * w;
			w1[0] = (float)psrc_row[2];
			w1[1] = (float)psrc_row[1];
			w1[2] = (float)psrc_row[0];
			for (int x = 3; x < nw; ++x)
				w1[x] = B * psrc_row[x - 3] + b1b0 * w1[x - 1] + b2b0 * w1[x - 2] + b3b0 * w1[x - 3]; //w1[x] = B * psrc_row[x - 3] + (b1 * w1[x - 1] + b2 * w1[x - 2] + b3 * w1[x - 3]) / b0;
			w2[nw - 3] = w1[nw - 3];
			w2[nw - 2] = w1[nw - 2];
			w2[nw - 1] = w1[nw - 1];
			for (int x = w - 1; x >= 0; --x)
			{
				w2[x] = B * w1[x] + b1b0 * w2[x + 1] + b2b0 * w2[x + 2] + b3b0 * w2[x + 3];//w2[x] = B * w1[x] + (b1 * w2[x + 1] + b2 * w2[x + 2] + b3 * w2[x + 3]) / b0;
				pdst_row[x] = w2[x + 3];
			}
		}
		delete[]w1;
		delete[]w2;

		int nh = h + 3;
		w1 = new  float[nh];
		w2 = new  float[nh];
		//纵向滤波
		for (int x = 0; x < w; ++x)
		{
			T* pdst_col = pdst + x;
			w1[0] = (float)pdst_col[2 * w];
			w1[1] = (float)pdst_col[w];
			w1[2] = (float)pdst_col[0];
			for (int y = 3; y < nh; ++y)
				w1[y] = B * pdst_col[(y - 3) * w] + b1b0 * w1[y - 1] + b2b0 * w1[y - 2] + b3b0 * w1[y - 3];//w1[y] = B * pdst_col[(y - 3) * w] + (b1 * w1[y - 1] + b2 * w1[y - 2] + b3 * w1[y - 3]) / b0;
			w2[nh - 3] = w1[nh - 3];
			w2[nh - 2] = w1[nh - 2];
			w2[nh - 1] = w1[nh - 1];
			for (int y = h - 1; y >= 0; --y)
			{
				w2[y] = B * w1[y] + b1b0 * w2[y + 1] + b2b0 * w2[y + 2] + b3b0 * w2[y + 3];//w2[y] = B * w1[y] + (b1 * w2[y + 1] + b2 * w2[y + 2] + b3 * w2[y + 3]) / b0;
				pdst_col[y * w] = w2[y + 3];
			}
		}
		delete[]w1;
		delete[]w2;
		return 1;
	}

	//基于论文 Recuirsive implementation of the Gaussian filter
	FNNOEXPORT int FastGaussFilter(void* psrc, void* pdst, int w, int h, PixelForm type, double sigma)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		if (type == PixelForm::U8)
			return DoFastGaussFilter((uint8_t*)psrc, (uint8_t*)pdst, w, h, sigma);
		if (type == PixelForm::U16)
			return DoFastGaussFilter((uint16_t*)psrc, (uint16_t*)pdst, w, h, sigma);
		if (type == PixelForm::U32)
			return DoFastGaussFilter((uint32_t*)psrc, (uint32_t*)pdst, w, h, sigma);
		if (type == PixelForm::F32)
			return DoFastGaussFilter((float*)psrc, (float*)pdst, w, h, sigma);
		if (type == PixelForm::F64)
			return DoFastGaussFilter((double*)psrc, (double*)pdst, w, h, sigma);
		return -3;
	}

	FNNOEXPORT int FastGaussFilter(Mat& src, Mat& dst, double sigma)
	{
		uint8_t* psrc = src.data;
		uint8_t* pdst = dst.data;
		if (psrc == nullptr)
		{
			PrintInfo("FastGaussFilter: src is nullptr");
			return -1;
		}
		if (pdst == nullptr)
		{
			PrintInfo("FastGaussFilter: dst is nullptr");
			return -2;
		}
		return FastGaussFilter(psrc, pdst, src.cols, src.rows, src.m_Format, sigma);
	}
};