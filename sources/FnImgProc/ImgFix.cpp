#include "ImgProcess.h"
namespace IMat
{
	template<typename T>
	T GetImgPixel(T* psrc, int w, int x, int y)
	{
		return psrc[y * w + x];
	}
	//修复坏点,可以原位操作,该算法不可以修复团块，只能修复点
	extern "C" FNNOEXPORT int FixBadPixel(float* psrc, float* pdst, float* mask, int w, int h)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		if (mask == NULL)
			memcpy(pdst, psrc, sizeof(float) * w * h);
		float* temp = pdst;
		//原位操作
		bool isSituOperation = false;
		if (psrc == pdst)
		{
			temp = new float[(uint64_t)w * h];
			isSituOperation = true;
		}
		int cols = w;
		int rows = h;
		for (int y = 0; y < rows; ++y)
		{
			for (int x = 0; x < cols; ++x)
			{
				float val = psrc[y * cols + x];
				float ptype = mask[y * cols + x];
				if (ptype < 0.01f)
				{
					temp[y * cols + x] = val;
					continue;
				}
				int yb = max(y - 1, 0);
				int ye = min(y + 1, rows - 1);
				int xb = max(x - 1, 0);
				int xe = min(x + 1, cols - 1);
				float sum = 0.0f;
				int num = 0;
				for (int yy = yb; yy <= ye; ++yy)
				{
					for (int xx = xb; xx <= xe; ++xx)
					{
						float val = psrc[yy * cols + xx];
						float ptype = mask[yy * cols + xx];
						if (ptype < 0.01f)
						{
							sum += val;
							num++;
						}
					}
				}
				temp[y * cols + x] = num > 0 ? sum / (float)num : 0.0f;
			}
		}
		if (isSituOperation == true)
		{
			memcpy(pdst, temp, sizeof(float) * w * h);
			delete[]temp;
		}
		return 1;
	}

	//图像修复,只支持32位float型，可以原位操作，该算法可能修复团块状缺陷
	extern "C" FNNOEXPORT int ImgFix(void* psrc, void* pdst, float* mask, int w, int h, int r, PixelForm pixelType, int iternum)
	{
		if (psrc == nullptr) return -1;
		if (pdst == nullptr) return -2;
		if (mask == nullptr) return -3;
		if (pixelType != PixelForm::F32) return -4;
		int imgSize = w * h;
		uint8_t* mask_temp = new uint8_t[imgSize];
		for (int i = 0; i < imgSize; ++i)
		{
			if (abs(mask[i]) < 1e-6f)
				mask_temp[i] = 0;
			else
			{
				float val = mask[i];
				val = max(val, 0.0f);
				val = min(val, 255.0f);
				mask_temp[i] = (uint8_t)val;
			}
		}
		if (pdst != psrc)
			memcpy(pdst, psrc, sizeof(float) * imgSize);
		struct Point
		{
			int x;
			int y;
		};
		vector<Point> badPts;
		float* psrcf = (float*)psrc;
		float* pdstf = (float*)pdst;
		for (int y = 0; y < h; ++y)
		{
			for (int x = 0; x < w; ++x)
			{
				if (mask_temp[y * w + x] == 0)
					continue;
				badPts.push_back({ x,y });
			}
		}
		int iter = 0;
		iternum = max(iternum, 1);
		while (badPts.size() > 0 && iter++ < iternum)
		{
			vector<Point> remain_badPts;
			vector<Point> beFixedPts;
			for (auto& pt : badPts)
			{
				Point up{ pt.x,max(pt.y - 1, 0) };
				Point down{ pt.x,min(pt.y + 1,w - 1) };
				Point left{ max(pt.x - 1,0),pt.y };
				Point right{ min(pt.x + 1,0),pt.y };
				//说明点在里面
				if (GetImgPixel(mask_temp, w, up.x, up.y) != 0 && GetImgPixel(mask_temp, w, down.x, down.y) != 0
					&& GetImgPixel(mask_temp, w, left.x, left.y) != 0 && GetImgPixel(mask_temp, w, right.x, right.y) != 0)
				{
					remain_badPts.push_back(pt);
					continue;
				}
				int begin_x = max(pt.x - r, 0);
				int end_x = min(pt.x + r, w - 1);
				int begin_y = max(pt.y - r, 0);
				int end_y = min(pt.y + r, h - 1);
				float sum = 0;
				int num = 0;
				for (int yy = begin_y; yy <= end_y; ++yy)
				{
					for (int xx = begin_x; xx <= end_x; ++xx)
					{
						int ind = yy * w + xx;
						if (mask_temp[ind] == (uint8_t)0)
						{
							num++;
							sum += pdstf[ind];
						}
					}
				}
				if (num > 0)
				{
					pdstf[pt.y * w + pt.x] = sum / (float)num;
					beFixedPts.push_back(pt);
				}
				else
				{
					remain_badPts.push_back(pt);
				}
			}
			for (auto& pt : beFixedPts)
			{
				mask_temp[pt.y * w + pt.x] = 0;
			}
			badPts = remain_badPts;
		}
		delete[] mask_temp;
		return 1;
	}
};