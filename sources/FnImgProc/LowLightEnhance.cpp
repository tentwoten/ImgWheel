#include "ImgProcess.h"
namespace IMat
{
	template <typename T>
	void AdjustDown(T* a, int parent, int size)
	{
		int child = 2 * parent + 1;
		while (child < size)
		{
			if (child + 1 < size && a[child] > a[child + 1])
				child++;
			if (a[parent] > a[child])
			{
				swap(a[parent], a[child]);
				parent = child;
				child = 2 * parent + 1;
			}
			else
				break;
		}
	}
	template <typename T>
	T Find_Kth_Maxnum_ByHeap(T* a, int N, int K)
	{
		K = min(max(0, K), N - 1);
		if (K == 0)
		{
			T minVal = 0, maxVal = 0;
			getMinMax<T>(a, N, minVal, maxVal);
			return maxVal;
		}
		T* arr = new T[K];
		memcpy(arr, a, sizeof(T) * K);
		for (int i = (K - 2) / 2; i >= 0; i--)
			AdjustDown(arr, i, K);
		for (int i = K; i < N; i++)
		{
			if (arr[0] < a[i])
			{
				arr[0] = a[i];
				AdjustDown(arr, 0, K);
			}
		}
		T minVal = 0, maxVal = 0;
		getMinMax<T>(arr, K, minVal, maxVal);
		delete[]arr;
		return minVal;
	}

	template<typename T>
	T Find_Kth_Maxnum(T* img, int N, int K)
	{
		K = min(max(1, K), N - 1);
		if (typeid(T) != typeid(uint16_t) && typeid(T) != typeid(uint8_t))
			return Find_Kth_Maxnum_ByHeap(img, N, K);
		int* histo = new int[65536];
		memset(histo, 0, sizeof(int) * 65536);
		for (int i = 0; i < N; ++i)
			histo[(int)img[i]]++;
		int count = 0;
		T result = 0;
		for (int i = 65535; i >= 0; --i)
		{
			count += histo[i];
			if (count >= K)
			{
				result = (T)i;
				break;
			}
		}
		delete[]histo;
		return result;
	}

	/*
	Adaptive Local Tone Mapping Based on Retinex for High Dynamic Range Images
	Lg(x,y)=   log(Lw(x,y)/aver(Lw)+1)   /    log(Lwmax/(aver(Lw)+1));

	Lg是待输出图像,

	Lw是输入图

	aver(Lw)=exp(1/N *sum ( theta+Lw(x,y))) theta是一个很小的值,防止亮度为0的清情况


	*/
	template<typename T>
	int DoLowLightEnhance(T* psrc, T* pdst, int w, int h, float chosenBrightRatio)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		//求出Lwmax，论文里是找最大值，这里是找到第百分之chosenBrightRatio大的数，用于防止噪声干扰
		T kthMax = Find_Kth_Maxnum(psrc, w * h, chosenBrightRatio * w * h);
		float Lwmax = (float)kthMax;
		int pixelnum = w * h;
		double Lw_Sum = 0.0f;
		if (typeid(T) == typeid(uint8_t))
		{
			double* logTab = new double[65536];
			logTab[0] = log(0.001);
			for (int i = 1; i < 65536; ++i)
				logTab[i] = log(0.001 + i / 255.0);
			//求出Lw_aver
			for (int i = 0; i < pixelnum; ++i)
				Lw_Sum += logTab[(int)psrc[i]];
			float Lw_aver = (float)exp(((Lw_Sum) / (double)pixelnum));
			Lwmax /= 255.0f;

			for (int i = 0; i < 256; ++i)
				logTab[i] = 255.0 * (log(i / 255.0f / Lw_aver + 1) / log(Lwmax / Lw_aver + 1));

			for (int i = 0; i < pixelnum; ++i)
				pdst[i] = (T)logTab[(int)psrc[i]];
			delete[]logTab;
		}
		else if (typeid(T) == typeid(uint16_t))
		{
			double* logTab = new double[65536];
			logTab[0] = log(0.001);
			for (int i = 1; i < 65536; ++i)
				logTab[i] = log(0.001 + i / 65535.0);
			//求出Lw_aver
			for (int i = 0; i < pixelnum; ++i)
				Lw_Sum += logTab[(int)psrc[i]];
			float Lw_aver = (float)exp(((Lw_Sum) / (double)pixelnum));
			Lwmax /= 65535.0;
			for (int i = 0; i < 256; ++i)
				logTab[i] = 65535 * (log(i / 65535.0 / Lw_aver + 1) / log(Lwmax / Lw_aver + 1));

			for (int i = 0; i < pixelnum; ++i)
				pdst[i] = (T)logTab[(int)psrc[i]];
			delete[]logTab;
		}
		else
		{
			double Lw_sum = 0.0f;
			for (int i = 0; i < pixelnum; ++i)
				Lw_Sum += log(0.001 + psrc[i]);
			float Lw_aver = (float)exp(((Lw_Sum) / (double)pixelnum));
			for (int i = 0; i < pixelnum; ++i)
			{
				float gain = log(psrc[i] / (Lw_aver + 1)) / log(Lwmax / (Lw_aver + 1));
				pdst[i] = (T)gain * psrc[i];
			}
		}
		return 1;
	}

	//测了一下，感觉效果一般
	FNNOEXPORT int LowLightEnhance(void* psrc, void* pdst, int w, int h, PixelForm pt, float chosenBrightRatio)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		if (pt == PixelForm::U8)
			return DoLowLightEnhance<uint8_t>((uint8_t*)psrc, (uint8_t*)pdst, w, h, chosenBrightRatio);
		if (pt == PixelForm::U16)
			return DoLowLightEnhance<uint16_t>((uint16_t*)psrc, (uint16_t*)pdst, w, h, chosenBrightRatio);
		if (pt == PixelForm::F32)
			return DoLowLightEnhance<float>((float*)psrc, (float*)pdst, w, h, chosenBrightRatio);
		return 1;
	}

};