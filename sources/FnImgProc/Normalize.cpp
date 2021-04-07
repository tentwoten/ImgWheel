#include "ImgProcess.h"
namespace IMat
{
	//归一化
	template<typename T>
	int normalize_M(T* pSrc, T* pDst, int w, int h, T minVal_normalized, T maxVal_normalized)
	{
		if (pSrc == 0 || pDst == 0) return -1;
		int imgSize = w * h;
		if (minVal_normalized > maxVal_normalized)
			swap(minVal_normalized, maxVal_normalized);
		T scope_normalized = maxVal_normalized - minVal_normalized;
		T minVal, maxVal;
		getMinMax<T>(pSrc, imgSize, minVal, maxVal);
		if (abs(maxVal - minVal) < 1e-16 || ((abs(minVal_normalized - minVal) < 1e-9) && (abs(maxVal_normalized - maxVal) < 1e-9)))
			return 1;
		T scope = maxVal - minVal;
		float rscope = 1.0f / (float)scope;
		float accuracy = (typeid(T) == typeid(float) || typeid(T) == typeid(double)) ? 0.0f : 0.5f;
		for (int i = 0; i < imgSize; ++i)
		{
			double val = ((pSrc[i] - minVal) * rscope * (float)scope_normalized + (float)minVal_normalized) + accuracy;
			val = val <= maxVal_normalized ? val : maxVal_normalized;
			val = val >= minVal_normalized ? val : minVal_normalized;
			pDst[i] = (T)val;
		}
		return 1;
	}

	extern "C" int normalize_16U(uint16_t * pSrc, uint16_t * pDst, int w, int h, uint16_t minVal_normalized, uint16_t maxVal_normalized)
	{
		return normalize_M<uint16_t>(pSrc, pDst, w, h, minVal_normalized, maxVal_normalized);
	}
	//
	extern "C" int normalize_8U(uint8_t * pSrc, uint8_t * pDst, int w, int h, uint8_t minVal_normalized, uint8_t maxVal_normalized)
	{
		return normalize_M<uint8_t>(pSrc, pDst, w, h, minVal_normalized, maxVal_normalized);
	}

	extern "C" int normalize_32F(float* pSrc, float* pDst, int w, int h, float minVal_normalized, float maxVal_normalized)
	{
		return normalize_M<float>(pSrc, pDst, w, h, minVal_normalized, maxVal_normalized);
	}

	extern "C" int normalize_64F(double* pSrc, double* pDst, int w, int h, double minVal_normalized, double maxVal_normalized)
	{
		return normalize_M<double>(pSrc, pDst, w, h, minVal_normalized, maxVal_normalized);
	}

};