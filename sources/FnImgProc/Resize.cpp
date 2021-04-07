#include "ImgProcess.h"
//////双线性插值
namespace IMat
{
	template<typename T>
	int resize_bilinear_interp(T* pSrc, T* pDst, int old_w, int old_h, int new_w, int new_h, int channels)
	{
		if (pSrc == 0) return -1;
		if (pSrc == 0) return -2;
		if (pSrc == pDst) return -3;//哪里申请，哪里释放，pDst传入为空时或者pDst==pSrc时，就不对pDst进行处理了
		if (old_w <= 0 || old_h <= 0 || new_w <= 0 || new_h <= 0) return -4;
		if (new_w == old_w && new_h == old_h)
		{
			memcpy(pDst, pSrc, sizeof(T) * old_h * old_w * channels);
			return 1;
		}
		float w_ratio = (float)old_w / (float)new_w;
		float h_ratio = (float)old_h / (float)new_h;
		for (int y = 0; y < new_h; ++y)
		{
			float sy = y * h_ratio;
			int y0 = (int)floor(sy);
			y0 = y0 >= 0 ? y0 : 0;
			int y1 = y0 + 1;
			y1 = y1 < old_h ? y1 : old_h - 1;
			float y_g = sy - (float)y0;
			for (int x = 0; x < new_w; ++x)
			{
				float sx = x * w_ratio;
				int x0 = (int)floor(sx);
				int x1 = x0 + 1;
				x0 = x0 >= 0 ? x0 : 0;
				x1 = x1 < old_w ? x1 : old_w - 1;
				float x_g = sx - (float)x0;
				//每个通道数进行处理
				for (int c = 0; c < channels; ++c)
				{
					T p00 = pSrc[(y0 * old_w + x0) * channels + c];
					T p01 = pSrc[(y0 * old_w + x1) * channels + c];
					T p10 = pSrc[(y1 * old_w + x0) * channels + c];
					T p11 = pSrc[(y1 * old_w + x1) * channels + c];
					T val = (1.0f - y_g) * (1.0f - x_g) * p00
						+ (1.0f - y_g) * x_g * p01
						+ y_g * (1.0f - x_g) * p10
						+ x_g * y_g * p11;
					pDst[(y * new_w + x) * channels + c] = val;
				}
			}
		}
		return 1;
	}

	extern "C" FNNOEXPORT int Resize2D(void* psrc, void* pdst, int src_w, int src_h, int dst_w, int dst_h, PixelForm pt)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		if (psrc == pdst) return -3;
		int PixelByteSize = GetPixelByteSize(pt);
		int channelnum = GetChannel(pt);
		if (src_w == dst_w || src_h == dst_h)
		{
			memcpy(pdst, psrc, src_w * src_h * PixelByteSize);
			return 1;
		}
		if (pt == PixelForm::U16 || pt == PixelForm::U16_RGB)
			return resize_bilinear_interp((uint16_t*)psrc, (uint16_t*)pdst, src_w, src_h, dst_w, dst_h, channelnum);
		if (pt == PixelForm::U8 || pt == PixelForm::U8_RGB || pt == PixelForm::U8_BGRA || pt == PixelForm::U8_RGBA)
			return resize_bilinear_interp((uint8_t*)psrc, (uint8_t*)pdst, src_w, src_h, dst_w, dst_h, channelnum);
		if (pt == PixelForm::F32)
			return resize_bilinear_interp((float*)psrc, (float*)pdst, src_w, src_h, dst_w, dst_h, channelnum);
		return 1;
	}
};