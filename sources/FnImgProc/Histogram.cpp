#include "ImgProcess.h"
namespace IMat
{
	//直方图均衡化
	template<typename T>
	int EquHistBegin(T* psrc, T* pdst, int imgSize, int histNum)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		if (histNum <= 0) return -3;
		int* hists = new int[histNum];
		memset(hists, 0, sizeof(int) * histNum);
		for (int i = 0; i < imgSize; ++i)
			hists[psrc[i]]++;
		float* histsRatio = new float[histNum];
		memset(histsRatio, 0, sizeof(float) * histNum);
		histsRatio[0] = (float)hists[0] / (float)imgSize;
		for (int i = 1; i < histNum; ++i)
			histsRatio[i] = histsRatio[i - 1] + (float)hists[i] / (float)imgSize;
		bool isFloat = false;
		double tor_min = 0.0, tor_max = 65535.0;
		GetTorMinMax(psrc, tor_min, tor_max, isFloat);
		float accuracy = isFloat ? 0.0f : 0.5f;
		for (int i = 0; i < imgSize; ++i)
		{
			T val = psrc[i];
			float res = (float)val * histsRatio[val] + 0.5f;
			res = (float)min<double>((double)res, tor_max);
			res = (float)max<double>((double)res, tor_min);
			pdst[i] = (T)res;
		}
		delete[]hists;
		delete[]histsRatio;
		return 1;
	}
	//只支持使用uint16_t型和uint8_t型
	extern "C" FNNOEXPORT int EquHist(void* psrc, void* pdst, int imgSize, PixelForm pt)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		uint8_t* temp = (uint8_t*)pdst;
		bool isSituOperator = false;
		int pixelByteSize = 8;
		if (pt == PixelForm::U16)
			pixelByteSize = 2;
		else if (pt == PixelForm::U8)
			pixelByteSize = 1;
		else
			return -3;
		if (psrc == pdst)
		{
			temp = new uint8_t[(uint64_t)imgSize * pixelByteSize];
			isSituOperator = true;
		}
		if (pt == PixelForm::U16)
			return EquHistBegin((uint16_t*)psrc, (uint16_t*)pdst, imgSize, 65536);
		else if (pt == PixelForm::U8)
			return EquHistBegin((uint8_t*)psrc, (uint8_t*)pdst, imgSize, 256);
		if (isSituOperator == true && temp != NULL)
		{
			memcpy(psrc, temp, (uint64_t)imgSize * pixelByteSize);
			pdst = psrc;//这句话没用，暂时放着
			delete[]temp;
		}
		return 1;
	}

	//获取图像的直方图
	extern "C" FNNOEXPORT int GetHistBin(void* psrc, int img_w, int img_h, PixelForm pt, int* histBin, unsigned int histBinNum)
	{
		if (psrc == NULL)
			return -1;
		if (histBin == NULL)
			return -2;
		int imgSize = img_w * img_h;
		memset(histBin, 0, sizeof(int) * histBinNum);
		if (pt == PixelForm::U8)
		{
			uint8_t* p = (uint8_t*)psrc;
			for (int i = 0; i < imgSize; ++i)
			{
				int img_val = (int)p[i];
				histBin[img_val]++;
			}
		}
		else if (pt == PixelForm::U16)
		{
			uint16_t* p = (uint16_t*)psrc;
			uint16_t minval, maxval;
			getMinMax(p, imgSize, minval, maxval);
			if (maxval - minval == 0)
				histBin[0] = imgSize;
			else
			{
				float span = (float)(maxval - minval + 1);
				float ratio = (float)histBinNum / span;
				for (int i = 0; i < imgSize; ++i)
				{
					float img_val = (float)p[i];
					int histInd = (int)((img_val - (float)minval) * ratio);
					histInd = histInd > 0 ? histInd : 0;
					histInd = histInd < ((int)histBinNum - 1) ? histInd : ((int)histBinNum - 1);
					histBin[histInd]++;
				}
			}
		}
		else if (pt == PixelForm::F32)
		{
			float* p = (float*)psrc;
			float minval, maxval;
			getMinMax(p, imgSize, minval, maxval);
			float span = maxval - minval;
			if (span == 0.0f)
				histBin[0] = imgSize;
			else
			{
				float ratio = (float)histBinNum / span;
				for (int i = 0; i < imgSize; ++i)
				{
					float img_val = p[i];
					int histInd = (int)((img_val - minval) * ratio);
					histInd = histInd > 0 ? histInd : 0;
					histInd = histInd < ((int)histBinNum - 1) ? histInd : ((int)histBinNum - 1);
					histBin[histInd]++;
				}
			}
		}
		return 1;
	}
};