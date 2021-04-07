#include "ImgProcess.h"
//亮度，对比度调节,支持原位操作
namespace IMat
{
	extern "C" FNNOEXPORT int BrightAndContrast(void* psrc, void* pdst, int w, int h, PixelForm type, float brightness, float contrast)
	{
		if (psrc == NULL)
			return -1;
		if (pdst == NULL)
			return -2;
		if (type != PixelForm::U16)
			return -3;
		uint16_t* psrc16 = (uint16_t*)psrc;
		uint16_t* pdst16 = (uint16_t*)pdst;
		int imgSize = w * h;
		for (int i = 0; i < imgSize; ++i)
		{
			float val = (float)psrc16[i];
			val = val * contrast + brightness;
			val = max(min(val, 65535.0f), 0.0f);
			pdst16[i] = (uint16_t)val;
		}
		return 1;
	}
	extern "C" FNNOEXPORT int BrightAndContrastEx(uint16_t * psrc, uint16_t * pdst, int w, int h, int low, int high)
	{
		if (psrc == NULL)
			return -1;
		if (pdst == NULL)
			return -2;
		int imgSize = w * h;

		float left_x = (float)low;
		float right_x = (float)high;
		float left_y = 0.0f;
		float right_y = 65535.0f;
		right_x = max(right_x, left_x + 1e-6f);
		float k = (right_y - left_y) / (right_x - left_x);
		float b = -k * left_x;

		float* tab = new float[65536];
		for (int i = 0; i < 65536; ++i)
		{
			if (i <= left_x)
				tab[i] = 0.0f;
			else if (i > left_x && i < right_x)
				tab[i] = max(min(k * i + b + 0.5f, 65535.0f), 0.0f);
			else
				tab[i] = 65535.0f;
		}
		for (int i = 0; i < imgSize; ++i)
			pdst[i] = (uint16_t)tab[psrc[i]];
		delete[]tab;
		return 1;
	}

	//根据重心进行AutoBC
	//找到直方图的重心
#pragma region AutoBC
	int FindHistBarycenter(int* histgram, int hist_begin, int hist_end, int& totalNum, int& baryCenter)
	{
		if (histgram == NULL) return -1;
		if (hist_begin > hist_end || hist_begin < 0 || hist_end < 0) return -2;
		uint64_t moment = 0;

		for (int x = hist_begin; x <= hist_end; ++x)
		{
			int val = histgram[x];
			totalNum += val;
			moment += (uint64_t)x * val;
		}
		if (totalNum == 0) return (hist_end - hist_begin) / 2;
		int Barycenter = (int)((double)moment / (double)totalNum + 0.5) + hist_begin;
		return Barycenter;
	}

	extern "C" FNNOEXPORT int AutoBC(void* psrc, void* pdst, int w, int h, PixelForm type, int& low, int& high, float Pixel_Remain_ratio)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		if (type == PixelForm::U16)
		{
			uint16_t* psrc16 = (uint16_t*)psrc;
			uint16_t* pdst16 = (uint16_t*)pdst;
			if (low >= high - 1)
			{
				uint16_t minVal = 0, maxVal = 0;
				getMinMax<uint16_t>(psrc16, w * h, minVal, maxVal);
				low = minVal;
				high = maxVal;
			}
			int histNum = 65536;
			int* histgram = new int[histNum];
			memset(histgram, 0, histNum * sizeof(int));
			int imgSize = w * h;
			for (int i = 0; i < imgSize; ++i)
				histgram[psrc16[i]] ++;
			int effectivePixelNum = 0, baryCenter = 0;
			FindHistBarycenter(histgram, low, high, effectivePixelNum, baryCenter);
			if (baryCenter < 0) return -3;
			int PixelNum_Threshold = (int)(effectivePixelNum - (1.0 - Pixel_Remain_ratio) * imgSize);
			int reamin_PixelNum = effectivePixelNum;
			int SumFromLeft = 0, SumFromRight = 0;
			for (int i = 0; i < low; ++i)
				SumFromLeft += histgram[i];
			for (int i = high + 1; i < 65536; ++i)
				SumFromRight += histgram[i];

			while (reamin_PixelNum >= PixelNum_Threshold)
			{
				SumFromLeft += histgram[low];
				SumFromRight += histgram[high];
				if (SumFromLeft < SumFromRight)
				{
					reamin_PixelNum -= histgram[low++];
					SumFromRight -= histgram[high];
				}
				else
				{
					reamin_PixelNum -= histgram[high--];
					SumFromLeft -= histgram[low];
				}
				if (low >= high)
					break;
			}
			delete[]histgram;
			if (low == high)
				memset(pdst, 32767, sizeof(uint16_t) * imgSize);
			else
				return BrightAndContrastEx(psrc16, pdst16, w, h, low, high);
		}
		return 1;
	}

};