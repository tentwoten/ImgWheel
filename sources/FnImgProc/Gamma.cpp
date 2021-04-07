#include "ImgProcess.h"

namespace IMat
{
	template<typename T>
	int GammaCorrectBegin(T* psrc, T* pdst, int w, int h, float lam)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		int imgSize = w * h;
		double tor_min = 0, tor_max = 0;
		bool isFloat = false;
		GetTorMinMax(psrc, tor_min, tor_max, isFloat);
		if (typeid(T) != typeid(double) && typeid(T) != typeid(float))
		{
			float tor_minf = (float)tor_min;
			float tor_maxf = (float)tor_max;
			float accurate = isFloat == false ? 0.5f : 0;
			if (/*typeid(T) == typeid(uint16_t)*/0)
			{
				uint16_t* tab = new uint16_t[65536];
				for (int i = 0; i < 65536; ++i)
				{
					float val = (float)i / tor_maxf;
					val = pow(val, lam) * tor_maxf + accurate;
					val = val <= tor_maxf ? val : tor_maxf;
					val = val >= tor_minf ? val : tor_minf;
					tab[i] = (T)val;
				}
				for (int i = 0; i < imgSize; ++i)
					pdst[i] = tab[(int)psrc[i]];
				delete[]tab;
			}
			else
			{
				for (int i = 0; i < imgSize; ++i)
				{
					float val = (float)psrc[i] / tor_maxf;
					val = pow(val, lam) * tor_maxf + accurate;
					val = val <= tor_maxf ? val : tor_maxf;
					val = val >= tor_minf ? val : tor_minf;
					pdst[i] = (T)val;
				}
			}
		}
		else
		{
			T minVal = (T)0, maxVal = (T)65535;
			getMinMax(psrc, imgSize, minVal, maxVal);
			tor_max = maxVal;
			tor_min = minVal;
			float scope = (float)(tor_max - tor_min);
			for (int i = 0; i < imgSize; ++i)
			{
				float val = (float)((psrc[i] - tor_min) / scope);
				val = pow(val, lam);
				val = val * scope + tor_min;
				val = val <= (float)tor_max ? val : (float)tor_max;
				val = val >= (float)tor_min ? val : (float)tor_min;
				pdst[i] = (T)val;
			}
		}
		return 1;
	}
	extern "C" FNNOEXPORT int GammaCorrect(void* psrc, void* pdst, int w, int h, float lam, PixelForm Pixel_Type)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		if (Pixel_Type == PixelForm::U8)
			return GammaCorrectBegin((uint8_t*)psrc, (uint8_t*)pdst, w, h, lam);
		else if (Pixel_Type == PixelForm::U16)
			return GammaCorrectBegin((uint16_t*)psrc, (uint16_t*)pdst, w, h, lam);
		else if (Pixel_Type == PixelForm::S16)
			return GammaCorrectBegin((short*)psrc, (short*)pdst, w, h, lam);
		else if (Pixel_Type == PixelForm::F32)
			return GammaCorrectBegin((float*)psrc, (float*)pdst, w, h, lam);
		else
			return -3;
		return 1;
	}

};
