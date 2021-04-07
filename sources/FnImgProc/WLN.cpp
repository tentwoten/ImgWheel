#include "ImgProcess.h"
#include "param.h"
//可以进行原位操作,支持原位操作
namespace IMat
{
	template<typename T>
	int doWLN(T* psrc, T* pdst, unsigned int pixelcount, float wl, float wr, float sl, float sr)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		if (typeid(T) == typeid(uint8_t) || typeid(T) == typeid(uint16_t))
		{
			int tabnum = typeid(T) == typeid(uint8_t) ? 256 : 65536;
			int tormin = 0;
			int tormax = typeid(T) == typeid(uint8_t) ? 255 : 65535;

			int* tab = new int[tabnum];
			float dy = sr - sl;
			float dx = wr - wl;
			if (abs(dx) < 1e-6)
			{
				for (int i = 0; i < tabnum; ++i)
					tab[i] = SafeVal((int)(sl + 0.5f), tormin, tormax);
			}
			else
			{
				float k = dy / dx;
				float b = sl - k * wl;
				float val;
				for (int i = 0; i < tabnum; ++i)
				{
					if (i <= wl)
					{
						tab[i] = SafeVal((int)(sl + 0.5f), tormin, tormax);
						//tab[i] = 0;
					}
					else if (i > wl && i < wr)
					{
						val = k * i + b;
						val = SafeVal((int)(val + 0.5f), tormin, tormax);
						tab[i] = (int)val;
					}
					else
						tab[i] = tormax;
				}
			}
			for (uint32_t i = 0; i < pixelcount; ++i)
				pdst[i] = (T)tab[(int)psrc[i]];
			delete[]tab;
		}
		else if (typeid(T) == typeid(float))
		{
			normalize_32F((float*)psrc, (float*)pdst, (int)pixelcount, 1, sl, sr);
		}
		return 1;

	}

	//窗宽窗位调节
	extern "C" FNNOEXPORT int WLN(void* psrc, void* pdst, unsigned int pixelcount, PixelForm pt, struWLNParam & m, const char* filePath)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		if (filePath)
			m = struWLNParam(filePath);
		float wl = m.wl;
		float wr = m.wr;
		float sl = m.specifymin;
		float sr = m.specifymax;
		if (wl > wr)
			swap(wl, wr);
		if (sl > sr)
			swap(sl, wr);
		if (pt == PixelForm::U16)
			return doWLN((uint16_t*)psrc, (uint16_t*)pdst, pixelcount, wl, wr, sl, sr);
		if (pt == PixelForm::U8)
			return doWLN((uint8_t*)psrc, (uint8_t*)pdst, pixelcount, wl, wr, sl, sr);
		if (pt == PixelForm::F32)
			return doWLN((float*)psrc, (float*)pdst, pixelcount, wl, wr, sl, sr);
		return 1;
	}


	template<typename T>
	int DoMulScaledWLN(T* psrc, T* pdst, unsigned int pixelcount, int scalednum, struWLNParam& m)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		double wl = m.wl, wr = m.wr, yl = m.specifymin, yr = m.specifymax;
		T nmin, nmax;
		getMinMax(psrc, pixelcount, nmin, nmax);
		double maxval = (double)nmax;
		double minval = (double)nmin;
		double tormin = 0, tormax = 65535;
		bool isFloat = false;
		GetTorMinMax(psrc, tormin, tormax, isFloat);
		scalednum = max(1, scalednum);

		if (abs(wl - yl) < 1e-6 && abs(wr - yr) < 1e-6)
		{
			if (pdst != psrc)
				memcpy(pdst, psrc, sizeof(T) * pixelcount);
			return 1;
		}

		if (typeid(T) == typeid(uint16_t) || typeid(T) == typeid(uint8_t))
		{
			int tabnum = typeid(T) == typeid(uint16_t) ? 65536 : 256;
			double* tab = new double[tabnum];
			memset(tab, 0, sizeof(double) * tabnum);
			double xs_bs = (double)(wl - minval) / (double)scalednum;
			double xe_bs = (double)(wr - maxval) / (double)scalednum;

			double val;
			for (int iter = 1; iter <= scalednum; ++iter)
			{
				double xl = minval + xs_bs * iter;
				double xr = maxval + xe_bs * iter;

				if (abs(xl - xr) < 1e-6)
				{
					val = (yl + yr) / 2.0;
					val = SafeVal(val, tormin, tormax);
					for (int i = 0; i < tabnum; ++i)
						tab[i] = val / (double)scalednum;
					continue;
				}
				double k = (yr - yl) / (xr - xl);
				double b = yl - k * xl;
				for (int i = 0; i < tabnum; ++i)
				{
					if (i < xl)
					{
						val = yl;
					}
					else if (i >= xl && i <= xr)
					{
						val = k * i + b;
					}
					else
					{
						val = yr;
					}
					tab[i] += val / (double)scalednum;
				}
			}
			T* tabT = new T[tabnum];
			for (int i = 0; i < tabnum; ++i)
				tabT[i] = SafeVal<T>((T)tab[i], (T)tormin, (T)tormax);

			for (uint32_t i = 0; i < pixelcount; ++i)
				pdst[i] = tabT[(int)psrc[i]];
			delete[]tab;
			delete[]tabT;
		}
		return 1;
	}

	template<typename T>
	int DoMulScaledWLN(T* psrc, T* pdst, unsigned int pixelcount, int scalednum, double wl, double wr, double yl, double yr)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		T nmin, nmax;
		getMinMax(psrc, pixelcount, nmin, nmax);
		double maxval = (double)nmax;
		double minval = (double)nmin;
		scalednum = max(1, scalednum);

		if (abs(wl - yl) < 1e-6 && abs(wr - yr) < 1e-6)
		{
			if (pdst != psrc)
				memcpy(pdst, psrc, sizeof(T) * pixelcount);
			return 1;
		}

		if (typeid(T) == typeid(uint16_t) || typeid(T) == typeid(uint8_t))
		{
			int tabnum = typeid(T) == typeid(uint16_t) ? 65536 : 256;
			double tormin = 0;
			double tormax = typeid(T) == typeid(uint16_t) ? 65535.0 : 255.0;
			double* tab = new double[tabnum];
			memset(tab, 0, sizeof(double) * tabnum);
			double xs_bs = (double)(wl - minval) / (double)scalednum;
			double xe_bs = (double)(wr - maxval) / (double)scalednum;

			double val;
			for (int iter = 1; iter <= scalednum; ++iter)
			{
				double xl = minval + xs_bs * iter;
				double xr = maxval + xe_bs * iter;

				if (abs(xl - xr) < 1e-6)
				{
					val = (yl + yr) / 2.0;
					val = SafeVal(val, tormin, tormax);
					for (int i = 0; i < tabnum; ++i)
						tab[i] = val / (double)scalednum;
					continue;
				}
				double k = (yr - yl) / (xr - xl);
				double b = yl - k * xl;
				for (int i = 0; i < tabnum; ++i)
				{
					if (i < xl)
					{
						val = yl;
					}
					else if (i >= xl && i <= xr)
					{
						val = k * i + b;
					}
					else
					{
						val = yr;
					}
					tab[i] += val / (double)scalednum;
				}
			}
			T* tabT = new T[tabnum];
			for (int i = 0; i < tabnum; ++i)
			{
				double tabval = tab[i];
				tabval = tabval > tormin ? tabval : tormin;
				tabval = tabval < tormax ? tabval : tormax;
				tabT[i] = tabval;
			}
			for (uint32_t i = 0; i < pixelcount; ++i)
				pdst[i] = tabT[(int)psrc[i]];
			delete[]tab;
			delete[]tabT;
		}
		return 1;
	}


	FNNOEXPORT int MulScaledWLN(void* psrc, void* pdst, unsigned int pixelcount, PixelForm pt, int scalednum, struWLNParam& m, const char* filePath)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		if (filePath != NULL && strlen(filePath) > 0)
			m = struWLNParam(filePath);
		if (pt == PixelForm::U8)
			return DoMulScaledWLN((uint8_t*)psrc, (uint8_t*)pdst, pixelcount, scalednum, m);
		if (pt == PixelForm::U16)
			return DoMulScaledWLN((uint16_t*)psrc, (uint16_t*)pdst, pixelcount, scalednum, m);
		return 1;
	}
};