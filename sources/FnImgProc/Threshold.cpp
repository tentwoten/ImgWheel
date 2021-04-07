#include "ImgProcess.h"
namespace IMat
{
	//利用Otsu获取适合的分割阈值
	double CalOtsuThresh(int* histo, double* histoRatio, int len, double integralsum, int pixelcount)
	{
		if (histo == NULL || histoRatio == NULL)
			return -1.0f;
		double spiltRes = 0.0;
		double maxSqu = 0.0;

		double sum0 = 0; //splitGray=0时   splitGray * histo[(int)splitGray]=0;
		double sum1 = integralsum - sum0;
		double w0 = histoRatio[0];
		double w1 = 1.0 - w0;
		for (int splitGray = 1; splitGray < len - 1; ++splitGray)
		{
			sum0 += (double)splitGray * histo[(int)splitGray];
			sum1 = integralsum - sum0;
			w0 += histoRatio[(int)splitGray];
			w1 = 1 - w0;
			double aver0 = sum0 / (w0 * pixelcount);
			double aver1 = sum1 / (w1 * pixelcount);
			double g = w0 * w1 * pow((aver0 - aver1), 2);
			if (g > maxSqu)
			{
				maxSqu = g;
				spiltRes = splitGray;
			}
		}
		return spiltRes;
	}

	FNNOEXPORT double GetOtsuThresh(void* psrc, uint32_t pixelcount, PixelForm pt)
	{
		if (psrc == NULL)
			return -1.0;
		if (pt == PixelForm::U8)
		{
			uint8_t* psrc8 = (uint8_t*)psrc;
			uint32_t len = 256;
			int* histo = new int[len];
			double* histoRatio = new double[len];
			memset(histo, 0, sizeof(int) * len);
			memset(histoRatio, 0, sizeof(double) * len);
			for (uint32_t i = 0; i < pixelcount; ++i)
				histo[(int)psrc8[i]]++;
			double integralsum = 0.0;
			double histoval;
			for (uint32_t i = 0; i < len; ++i)
			{
				histoval = (double)histo[i];
				histoRatio[i] = histoval / (double)pixelcount;
				integralsum += histoval * (double)i;
			}
			double spiltRes = CalOtsuThresh(histo, histoRatio, len, integralsum, pixelcount);
			delete[]histo;
			delete[]histoRatio;
			return spiltRes;
		}
		else if (pt == PixelForm::U16)
		{
			uint16_t* psrc16 = (uint16_t*)psrc;
			uint32_t len = 65536;
			int* histo = new int[len];
			double* histoRatio = new double[len];
			memset(histo, 0, sizeof(int) * len);
			memset(histoRatio, 0, sizeof(double) * len);
			for (uint32_t i = 0; i < pixelcount; ++i)
			{
				histo[(int)psrc16[i]]++;
			}
			double integralsum = 0.0;
			double histoval;
			for (uint32_t i = 0; i < len; ++i)
			{
				histoval = (double)histo[i];
				histoRatio[i] = histoval / (double)pixelcount;
				integralsum += histoval * (double)i;
			}
			double spiltRes = CalOtsuThresh(histo, histoRatio, len, integralsum, pixelcount);
			delete[]histo;
			delete[]histoRatio;
			return spiltRes;
		}
		return -1.0;

	}

	template<typename T>
	int DoThreshold(T* psrc, T* pdst, int w, int h, double thresh, int type)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		int imgSize = w * h;
		double tor_min, tor_max;
		bool isFloat;
		GetTorMinMax(psrc, tor_min, tor_max, isFloat);

		if (type == 0)
		{
			for (int i = 0; i < imgSize; ++i)
				pdst[i] = (T)((double)psrc[i] > thresh ? tor_max : 0);
		}
		else if (type == 1)
		{
			for (int i = 0; i < imgSize; ++i)
				pdst[i] = (T)((double)psrc[i] > thresh ? 0 : tor_max);
		}
		else if (type == 2)
		{
			for (int i = 0; i < imgSize; ++i)
				pdst[i] = (T)((double)psrc[i] > thresh ? psrc[i] : 0);
		}
		else if (type == 3)
		{
			for (int i = 0; i < imgSize; ++i)
				pdst[i] = (T)((double)psrc[i] > thresh ? 0 : psrc[i]);
		}
		return 1;
	}

	//thresh_type=0; val>thresh?val:0;
	//thresh_type=1; val>thresh?0:val;
	//thresh_type=2; val>thresh?val:0;
	//thresh_type=3; val>thresh?0:val;
	extern "C" FNNOEXPORT int ThresholdBinary(void* psrc, void* pdst, PixelForm pt, int w, int h, double thresh, int thresh_type)
	{
		if (psrc == NULL) return-1;
		if (pdst == NULL) return -2;

		if (pt == PixelForm::U16)
			return DoThreshold((uint16_t*)psrc, (uint16_t*)pdst, w, h, thresh, thresh_type);
		if (pt == PixelForm::U8)
			return DoThreshold((uint8_t*)psrc, (uint8_t*)pdst, w, h, thresh, thresh_type);
		return 1;
	}
};