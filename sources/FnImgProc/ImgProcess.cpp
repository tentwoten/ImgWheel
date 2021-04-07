// FinnoImgProcess.cpp : 定义 DLL 应用程序的导出函数。
#include <iostream>
#include "ImgProcess.h"
using namespace std;
namespace IMat
{
	//反色,只支持16位，需要再添加，支持原位操作
	extern "C" FNNOEXPORT int Inverse(void* psrc, void* pdst, int w, int h, PixelForm type)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		if (type == PixelForm::U16) return -3;
		uint16_t* psrc16 = (uint16_t*)psrc;
		uint16_t* pdst16 = (uint16_t*)pdst;
		int imgSize = w * h;
		for (int i = 0; i < imgSize; ++i)
			pdst16[i] = 65535 - psrc16[i];
		return 1;
	}

	// 计算增益,用之前转成float型,可以进行原位操作
	extern "C" FNNOEXPORT int ImgGainCorrect(float* psrc, float* pdst, int w, int h)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		float* psrcf = nullptr;
		float* pdstf = nullptr;
		int imgSize = w * h;
		double sum = 0.0;
		int num = 0;
		for (int i = 0; i < imgSize; ++i)
		{
			float val = psrc[i];
			num++;
			sum += (double)val;
		}
		float aver = (float)(sum / num);
		for (int i = 0; i < imgSize; ++i)
		{
			float val = psrc[i];
			val = max(0.001f, val);
			pdst[i] = aver / val;
		}
		return 1;
	}
};