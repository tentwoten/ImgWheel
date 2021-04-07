#include"ImgProcess.h"
#include "param.h"
namespace IMat
{

	//获得每一块的受抑制的直方图
	//删除二维数组
	template<typename T>
	void delete2DMemory(T** p, int h)
	{
		if (p == 0) return;
		for (int i = 0; i < h; ++i)
		{
			if (p[i] != 0)
				delete[]p[i];
		}
		delete[]p;
	}
	//获取二维数组
	template<typename T>
	T** get2DMemory(int h, int w, int iniVal = 0)
	{
		T** array = 0;
		try {
			array = new T * [h];
			for (int i = 0; i < h; ++i) array[i] = 0;
			for (int i = 0; i < h; ++i)
			{
				array[i] = new T[w];
				memset(array[i], iniVal, w * sizeof(T));
			}
		}
		catch (const std::bad_alloc& e)
		{
			delete2DMemory<T>(array, h);
			return 0;
		}
		return array;
	}

	template<typename T>
	int clahe_GetEquHists(T* pSrc, float** hists, float** equHists, int imgWidth, int imgHeight, claheParam& m)
	{
		if (pSrc == 0 || hists == 0 || equHists == 0) return 0;
		if (imgWidth <= 0 || imgHeight <= 0) return -1;
		int blockWidth = imgWidth / m.widthBlockNum;
		int blockHeight = imgHeight / m.heightBlockNum;
		int blockNum = m.widthBlockNum * m.heightBlockNum;
		int blockSize = blockWidth * blockHeight;

		for (int x = 0; x < m.widthBlockNum; ++x)
		{
			for (int y = 0; y < m.heightBlockNum; ++y)
			{
				int yStart, yEnd, xStart, xEnd;
				yStart = max(0, min(imgHeight, y * blockHeight));
				yEnd = max(0, min(imgHeight, yStart + blockHeight));
				xStart = max(0, min(imgWidth, x * blockWidth));
				xEnd = max(0, min(imgWidth, xStart + blockWidth));
				int blockIndex = y * m.widthBlockNum + x;
				float* p = hists[blockIndex];
				for (int yy = yStart; yy < yEnd; ++yy)
				{
					for (int xx = xStart; xx < xEnd; ++xx)
					{
						int histIndex = (int)min((double)m.histBin - 1, (double)(*(pSrc + yy * imgWidth + xx) * m.binRatio));   //float型相乘有几率导致数组越界,这里做了一个保护
						hists[blockIndex][histIndex]++;
					}
				}
				//裁剪和增加操作，也就是clahe中的cl部分,多于limit的加起来，然后进行分散到所有直方图上
				int limit = m.peak;
				int steal = 0;
				for (int k = 0; k < m.histBin; ++k)
				{
					if (hists[blockIndex][k] > limit)
					{
						steal += hists[blockIndex][k] - limit;
						hists[blockIndex][k] = limit;
					}
				}
				//将steal平分到每一个直方图上，一个有m.histBin个直方图，所以裁剪总量除以m.histBin		
				int ibegin = 0;
				while (abs(hists[blockIndex][ibegin++]) < 1e-16)
				{
					if (ibegin > m.histBin) break;
				}
				//裁剪后的加到其他通道数上的方式有很多
				//这里把裁剪的分散到 minscope--maxval， 如像素分布在1000-20000,若有65536个通道，那么裁剪后的就分布到1000-65535			
				float eff_num = max<float>(m.histBin - ibegin, 1.0f);
				float bonus = steal / eff_num;
				for (int k = ibegin; k < m.histBin; ++k)
					hists[blockIndex][k] += bonus;
				//计算均衡化后的直方图		
				equHists[blockIndex][ibegin] = 0.0f;
				float temp = 1.0f / (blockSize - hists[blockIndex][ibegin]);
				for (int k = ibegin; k < m.histBin; ++k)
					equHists[blockIndex][k] = equHists[blockIndex][k - 1] + hists[blockIndex][k] * temp;
			}
		}
		return 1;
	}
	//clahe利用直方图进行插值
	template<typename T>
	int clahe_Interpolation(T* pSrc, T* pDst, int imgWidth, int imgHeight, float** equHists, claheParam& m)
	{
		if (pSrc == 0 || pDst == 0 || equHists == 0)
			return 0;
		int widthBlockNum = m.widthBlockNum;
		int heightBlockNum = m.heightBlockNum;
		int blockWidth = imgWidth / widthBlockNum;
		int blockHeight = imgHeight / heightBlockNum;
		float binRatio = m.binRatio;
		float accuracy = 0.0f;
		if (typeid(T) == typeid(uint8_t) || typeid(T) == typeid(uint16_t))   //当T的类型位uint8_t和uint16_t型时 就加上0.5f进行四舍五入，否则就正常
			accuracy = 0.5f;
		for (int i = 0; i < imgWidth; ++i)
		{
			for (int j = 0; j < imgHeight; ++j)
			{
				//计算四个角
				float res = 0.0f;
				int index = j * imgWidth + i;
				int histIndex = (int)min((double)pSrc[index] * binRatio, (double)(m.histBin - 1));
				if (i <= blockWidth / 2 && j <= blockHeight / 2) {
					int num = 0;
					res = equHists[num][histIndex] * m.span;
				}
				else if (i <= blockWidth / 2 && j >= ((heightBlockNum - 1) * blockHeight + blockHeight / 2)) {
					int num = widthBlockNum * (heightBlockNum - 1);
					res = equHists[num][histIndex] * m.span;
				}
				else if (i >= ((widthBlockNum - 1) * blockWidth + blockWidth / 2) && j <= blockHeight / 2) {
					int num = widthBlockNum - 1;
					res = equHists[num][histIndex] * m.span;
				}
				else if (i >= ((widthBlockNum - 1) * blockWidth + blockWidth / 2) && j >= ((heightBlockNum - 1) * blockHeight + blockHeight / 2)) {
					int num = widthBlockNum * heightBlockNum - 1;
					res = equHists[num][histIndex] * m.span;
				}
				//计算四条除了顶角外的边
				else if (i <= blockWidth / 2)                                            //左边
				{
					int num_i = 0;
					int num_j = (j - blockHeight / 2) / blockHeight;
					//int num1=num_j*heightBlockNum;
					int num1 = num_j * widthBlockNum;
					int num2 = num1 + widthBlockNum;
					float p = (j - (num_j * blockHeight + blockHeight / 2)) / (1.0f * blockHeight);
					float q = 1 - p;
					res = (q * equHists[num1][histIndex] + p * equHists[num2][histIndex]) * m.span;
				}
				else if (i >= (widthBlockNum - 1) * blockWidth + blockWidth / 2) {            //右边
					int num_i = widthBlockNum - 1;
					int num_j = (j - blockHeight / 2) / blockHeight;
					int num1 = num_j * widthBlockNum + num_i;
					int num2 = num1 + widthBlockNum;
					float p = (j - (num_j * blockHeight + blockHeight / 2)) / (1.0f * blockHeight);
					float q = 1 - p;
					res = (q * equHists[num1][histIndex] + p * equHists[num2][histIndex]) * m.span;
				}
				else if (j <= blockHeight / 2) {                                        //上边
					int num_i = (i - blockWidth / 2) / blockWidth;
					int num_j = 0;
					int num1 = num_j * widthBlockNum + num_i;
					int num2 = num1 + 1;
					float p = (i - (num_i * blockWidth + blockWidth / 2)) / (1.0f * blockWidth);
					float q = 1 - p;
					res = (q * equHists[num1][histIndex] + p * equHists[num2][histIndex]) * m.span;
				}
				else if (j >= (heightBlockNum - 1) * blockHeight + blockHeight / 2) {         //下边
					int num_i = (i - blockWidth / 2) / blockWidth;
					int num_j = heightBlockNum - 1;
					int num1 = num_j * widthBlockNum + num_i;
					int num2 = num1 + 1;
					float p = (i - (num_i * blockWidth + blockWidth / 2)) / (1.0f * blockWidth);
					float q = 1 - p;
					res = (q * equHists[num1][histIndex] + p * equHists[num2][histIndex]) * m.span;
				}
				//双线性插值
				else
				{
					int num_i = (i - blockWidth / 2) / blockWidth;
					int num_j = (j - blockHeight / 2) / blockHeight;
					int num1 = num_j * widthBlockNum + num_i;
					int num2 = num1 + 1;
					int num3 = num1 + widthBlockNum;
					int num4 = num2 + widthBlockNum;
					float u = (i - (num_i * blockWidth + blockWidth / 2)) / (1.0f * blockWidth);
					float v = (j - (num_j * blockHeight + blockHeight / 2)) / (1.0f * blockHeight);
					res = (u * v * equHists[num4][histIndex] +
						(1 - u) * (1 - v) * equHists[num1][histIndex] +
						u * (1 - v) * equHists[num2][histIndex] +
						(1 - u) * v * equHists[num3][histIndex]) * m.span;
				}
				if (typeid(T) == typeid(uint16_t))
				{
					res = (res + accuracy) <= 65535 ? (res + accuracy) : 65535;
					res = res >= 0 ? res : 0;
					pDst[index] = (T)(res + accuracy);
				}
				else if (typeid(T) == typeid(uint8_t))
				{
					res = (res + accuracy) <= 255 ? (res + accuracy) : 255;
					res = res >= 0 ? res : 0;
					pDst[index] = (T)(res + accuracy);
				}
				else
					pDst[index] = (T)(res + accuracy);
			}
		}
		return 1;
	}
	//64位图clahe
	int clahe_64F(double* pSrc, double* pDst, int imgWidth, int imgHeight, claheParam& m, const char* filePath)
	{
		if (pSrc == 0) return 0;
		bool insituoperation = false;
		if (pDst == pSrc)
		{
			insituoperation = true;
			pDst = new double[imgWidth * imgHeight];
		}
		if (strlen(filePath) != 0)	m = claheParam(filePath);
		if (!(m.widthBlockNum >= 1 && m.widthBlockNum <= imgWidth && m.heightBlockNum >= 1 && m.heightBlockNum <= imgHeight))
			return -1;
		int sign = 1;
		m.histBin = max(min(65536, m.histBin), 1);
		int blockNum = m.widthBlockNum * m.heightBlockNum;
		float** hists = get2DMemory<float>(blockNum, m.histBin, 0);
		float** equHists = get2DMemory<float>(blockNum, m.histBin, 0);
		if (hists == 0 || equHists == 0)
		{
			delete2DMemory<float>(hists, blockNum);
			delete2DMemory<float>(equHists, blockNum);
			return -2;
		}
		double minVal, maxVal;
		sign = getMinMax<double>(pSrc, imgWidth * imgHeight, minVal, maxVal);
		m.span = (float)max(1.0, maxVal - minVal);
		m.binRatio = (m.histBin - 1) / m.span;
		//获得均衡化得直方图
		sign = clahe_GetEquHists<double>(pSrc, hists, equHists, imgWidth, imgHeight, m);
		if (!sign) return -3;
		//利用已知得直方图进行插值
		sign = clahe_Interpolation<double>(pSrc, pDst, imgWidth, imgHeight, equHists, m);
		if (!sign) return -4;
		//删除二维数组
		delete2DMemory<float>(hists, blockNum);
		delete2DMemory<float>(equHists, blockNum);
		if (insituoperation)
		{
			memcpy(pSrc, pDst, sizeof(double) * imgWidth * imgHeight);
			delete[]pDst;
			pDst = pSrc;
		}
		return 1;
	}
	//32位图clahe
	int clahe_32F(float* pSrc, float* pDst, int imgWidth, int imgHeight, claheParam& m, const char* filePath)
	{
		if (pSrc == 0) return 0;
		//是否是原位操作
		bool insituoperation = false;
		if (pDst == pSrc)
		{
			insituoperation = true;
			pDst = new float[imgWidth * imgHeight];
		}
		if (strlen(filePath) != 0)	m = claheParam(filePath);
		if (!(m.widthBlockNum >= 1 && m.widthBlockNum <= imgWidth && m.heightBlockNum >= 1 && m.heightBlockNum <= imgHeight))
			return -1;
		int sign = 1;
		m.histBin = max(min(65536, m.histBin), 1);
		int blockNum = m.widthBlockNum * m.heightBlockNum;
		float** hists = get2DMemory<float>(blockNum, m.histBin, 0);
		float** equHists = get2DMemory<float>(blockNum, m.histBin, 0);
		if (hists == 0 || equHists == 0)
		{
			delete2DMemory<float>(hists, blockNum);
			delete2DMemory<float>(equHists, blockNum);
			return -2;
		}
		float minVal, maxVal;
		sign = getMinMax<float>(pSrc, imgWidth * imgHeight, minVal, maxVal);
		m.span = max<float>(1.0f, maxVal - minVal);
		m.binRatio = (m.histBin - 1) / m.span;
		//获得均衡化得直方图
		sign = clahe_GetEquHists<float>(pSrc, hists, equHists, imgWidth, imgHeight, m);
		if (!sign) return -3;
		//利用已知得直方图进行插值
		sign = clahe_Interpolation<float>(pSrc, pDst, imgWidth, imgHeight, equHists, m);
		if (!sign) return -4;
		//删除二维数组
		delete2DMemory<float>(hists, blockNum);
		delete2DMemory<float>(equHists, blockNum);
		//原位操作
		if (insituoperation)
		{
			memcpy(pSrc, pDst, sizeof(float) * imgWidth * imgHeight);
			delete[]pDst;
			pDst = pSrc;
		}
		return 1;
	}

	//16位图clahe,支持原位操作
	int clahe_16U(uint16_t* pSrc, uint16_t* pDst, int imgWidth, int imgHeight, claheParam& m, const char* filePath)
	{
		if (pSrc == 0) return 0;
		bool insituoperation = false;
		//是否是原位操作
		if (pDst == pSrc)
		{
			insituoperation = true;
			pDst = new uint16_t[imgWidth * imgHeight];
		}

		if (strlen(filePath) != 0) m = claheParam(filePath);
		if (!(m.widthBlockNum >= 1 && m.widthBlockNum <= imgWidth && m.heightBlockNum >= 1 && m.heightBlockNum <= imgHeight))
			return -1;
		int sign = 1;
		m.histBin = max(min(65536, m.histBin), 1);
		int blockNum = m.widthBlockNum * m.heightBlockNum;
		float** hists = get2DMemory<float>(blockNum, m.histBin, 0);
		float** equHists = get2DMemory<float>(blockNum, m.histBin, 0);
		if (hists == 0 || equHists == 0)
		{
			delete2DMemory<float>(hists, blockNum);
			delete2DMemory<float>(equHists, blockNum);
			return -2;
		}
		//uint16_t minVal,maxVal;
		//sign=getMinMax<uint16_t>(pSrc,imgWidth*imgHeight,minVal,maxVal);
		//m.span=maxVal-minVal;
		m.span = 65535;                                 //16位图，所以像素跨度最大为65536
		m.binRatio = (m.histBin - 1) / m.span;
		//获得均衡化得直方图
		sign = clahe_GetEquHists<uint16_t>(pSrc, hists, equHists, imgWidth, imgHeight, m);
		if (!sign) return -3;
		//利用已知得直方图进行插值
		sign = clahe_Interpolation<uint16_t>(pSrc, pDst, imgWidth, imgHeight, equHists, m);
		if (!sign) return -4;
		//删除二维数组
		delete2DMemory<float>(hists, blockNum);
		delete2DMemory<float>(equHists, blockNum);

		if (insituoperation == true)
		{
			memcpy(pSrc, pDst, sizeof(uint16_t) * imgHeight * imgWidth);
			delete[]pDst;
			pDst = pSrc;
		}

		return 1;
	}
	//8位图clahe
	int clahe_8U(uint8_t* pSrc, uint8_t* pDst, int imgWidth, int imgHeight, claheParam& m, const char* filePath)
	{
		if (pSrc == 0) return 0;
		bool insituoperation = false;
		if (pDst == pSrc)
		{
			insituoperation = true;
			pDst = new uint8_t[imgWidth * imgHeight];
		}

		if (strlen(filePath) != 0) m = claheParam(filePath);
		if (!(m.widthBlockNum >= 1 && m.widthBlockNum <= imgWidth && m.heightBlockNum >= 1 && m.heightBlockNum <= imgHeight))
			return -1;
		int sign = 1;
		m.histBin = max(min(256, m.histBin), 1);
		int blockNum = m.widthBlockNum * m.heightBlockNum;
		float** hists = get2DMemory<float>(blockNum, m.histBin);
		float** equHists = get2DMemory<float>(blockNum, m.histBin);
		if (hists == 0 || equHists == 0)
		{
			delete2DMemory<float>(hists, blockNum);
			delete2DMemory<float>(equHists, blockNum);
			return -2;
		}
		m.span = 255;                                  //8位图，所以像素差设为255
		m.binRatio = (m.histBin - 1) / m.span;
		//获得均衡化得直方图
		sign = clahe_GetEquHists<uint8_t>(pSrc, hists, equHists, imgWidth, imgHeight, m);
		if (!sign) return -3;
		//利用已知得直方图进行插值
		sign = clahe_Interpolation<uint8_t>(pSrc, pDst, imgWidth, imgHeight, equHists, m);
		if (!sign) return -4;
		//删除二维数组
		delete2DMemory<float>(hists, blockNum);
		delete2DMemory<float>(equHists, blockNum);
		if (insituoperation == true)
		{
			memcpy(pSrc, pDst, sizeof(uint8_t) * imgHeight * imgWidth);
			delete[]pDst;
			pDst = pSrc;
		}
		return 1;
	}
	extern "C"
		int clahe_64F_Manual(double* pSrc, double* pDst, int imgWidth, int imgHeight, int widthBlockNum, int heightBlockNum, int histBin, int peak, const char* filePath)
	{
		claheParam m;
		m.widthBlockNum = widthBlockNum;
		m.heightBlockNum = heightBlockNum;
		m.histBin = histBin;
		m.peak = peak;
		return clahe_64F(pSrc, pDst, imgWidth, imgHeight, m, filePath);
	}
	//32位float型   手动赋值
	extern "C"
		int clahe_32F_Manual(float* pSrc, float* pDst, int imgWidth, int imgHeight, int widthBlockNum, int heightBlockNum, int histBin, int peak, const char* filePath)
	{
		claheParam m;
		m.widthBlockNum = widthBlockNum;
		m.heightBlockNum = heightBlockNum;
		m.histBin = histBin;
		m.peak = peak;
		return clahe_32F(pSrc, pDst, imgWidth, imgHeight, m, filePath);
	}
	//16位uint16_t型  手动赋值
	extern "C"
		int clahe_16U_Manual(uint16_t * pSrc, uint16_t * pDst, int imgWidth, int imgHeight, int widthBlockNum, int heightBlockNum, int histBin, int peak, const char* filePath)
	{
		claheParam m;
		m.widthBlockNum = widthBlockNum;
		m.heightBlockNum = heightBlockNum;
		m.histBin = histBin;
		m.peak = peak;
		return clahe_16U(pSrc, pDst, imgWidth, imgHeight, m, filePath);
	}
	//8位uint8_t型  手动赋值
	extern "C"
		int clahe_8U_Manual(uint8_t * pSrc, uint8_t * pDst, int imgWidth, int imgHeight, int widthBlockNum, int heightBlockNum, int histBin, int peak, const char* filePath)
	{
		claheParam m;
		m.widthBlockNum = widthBlockNum;
		m.heightBlockNum = heightBlockNum;
		m.histBin = histBin;
		m.peak = peak;
		return clahe_8U(pSrc, pDst, imgWidth, imgHeight, m, filePath);
	}
};