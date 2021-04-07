#include "ImgProcess.h"
//将ROI拷贝到图中
namespace IMat
{
	template<typename T>
	int DoRoiWatermark(T* psrc, T* pdst, T* pMask, int src_w, int src_h, int mask_w, int mask_h, int begin_x, int begin_y, int channel, float weight = 0.5f)
	{
		if (psrc == NULL)
			return -1;
		if (pdst == NULL)
			return -2;
		if (pMask == NULL)
			return -3;
		if (pdst != psrc)
			memcpy(pdst, psrc, sizeof(T) * src_w * src_h * channel);
		begin_x = max(0, begin_x);
		begin_y = max(0, begin_y);
		int end_x = min(src_w - 1, begin_x + mask_w - 1);
		int end_y = min(src_h - 1, begin_y + mask_h - 1);
		int fillWidth = end_x - begin_x;
		int fillHeight = end_y - begin_y;
		double tor_min = 0, tor_max = 0;
		bool isFloat = 0;
		GetTorMinMax(psrc, tor_min, tor_max, isFloat);
		double a = (double)weight;
		double b = 1.0 - a;
		if (channel == 1)
		{
			for (int my = 0; my <= fillHeight; ++my)
			{
				for (int mx = 0; mx <= fillWidth; ++mx)
				{
					int mindex = mask_w * channel * my + mx;
					int py = begin_y + my;
					int px = begin_x + mx;
					int pindex = src_w * channel * py + px;
					T mask_val = pMask[mindex];
					if (mask_val != 0)
					{
						double res = pdst[pindex] * b + mask_val * a;
						res = min(tor_max, res);
						res = max(tor_min, res);
						pdst[pindex] = (T)res;
					}
				}
			}
		}
		else if (channel == 3 || channel == 4)
		{
			for (int my = 0; my <= fillHeight; ++my)
			{
				for (int mx = 0; mx <= fillWidth; ++mx)
				{
					int mindex = (my * mask_w + mx) * channel;
					int py = begin_y + my;
					int px = begin_x + mx;
					int pindex = (src_w * py + px) * channel;
					T val1 = pMask[mindex];
					T val2 = pMask[mindex + 1];
					T val3 = pMask[mindex + 2];
					if (val1 == 0 && val2 == 0 && val3 == 0)
						continue;
					for (int c = 0; c < 3; ++c)   //channel==3或者channel==4都只算前3个通道就行了
					{
						double res = pdst[pindex + c] * b + pMask[mindex + c] * a;
						res = min(tor_max, res);
						res = max(tor_min, res);
						pdst[pindex + c] = (T)res;
					}
				}
			}
		}
		return 1;
	}
	extern "C" FNNOEXPORT int AddWaterMask(void* psrc, void* pdst, void* pMask, int src_w, int src_h, int mask_w, int mask_h, int begin_x, int begin_y, PixelForm pt, float weight)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		if (pMask == NULL) return -3;
		int channel = GetChannel(pt);
		if (pt == PixelForm::U16 || pt == PixelForm::U16_RGB)
			return DoRoiWatermark((uint16_t*)psrc, (uint16_t*)pdst, (uint16_t*)pMask, src_w, src_h, mask_w, mask_h, begin_x, begin_y, channel, weight);
		if (pt == PixelForm::U8 || pt == PixelForm::U8_RGB || pt == PixelForm::U8_RGBA || pt == PixelForm::U8_BGRA)
			return DoRoiWatermark((uint8_t*)psrc, (uint8_t*)pdst, (uint8_t*)pMask, src_w, src_h, mask_w, mask_h, begin_x, begin_y, channel, weight);
		if (pt == PixelForm::F32)
			return DoRoiWatermark((float*)psrc, (float*)pdst, (float*)pMask, src_w, src_h, mask_w, mask_h, begin_x, begin_y, channel, weight);
		return 1;
	}

	//将Roi拷贝到相应的位置中
	extern "C" FNNOEXPORT int CopyRoiTo(void* psrc, void* pdst, void* mask, PixelForm pt, int img_w, int img_h, int mask_w, int mask_h, int begin_x, int begin_y)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		if (mask == NULL) return -3;
		int PixelByteSize = GetPixelByteSize(pt);
		if (pdst != psrc)
			memcpy(pdst, psrc, PixelByteSize * img_w * img_h);
		begin_x = max(0, begin_x);
		begin_y = max(0, begin_y);
		int end_x = min(img_w - 1, begin_x + mask_w - 1);
		int end_y = min(img_h - 1, begin_y + mask_h - 1);
		int fillWidth = end_x - begin_x + 1;
		int fillHeight = end_y - begin_y + 1;

		uint8_t* pdst8 = (uint8_t*)pdst;
		uint8_t* pmask8 = (uint8_t*)mask;
		for (int y = begin_y; y < begin_y + fillHeight; ++y)
		{
			int pdst8_ind = (y * img_w + begin_x) * PixelByteSize;
			int mask_ind = (y - begin_y) * fillWidth * PixelByteSize;
			memcpy(pdst8 + pdst8_ind, pmask8 + mask_ind, fillWidth * PixelByteSize);
		}
		return 1;
	}
};