#include "ImgProcess.h"
#include "FnCore/Log_xx.h"
//瘦身算法,只实用于二值化图像
using namespace IMat;
namespace IMat
{
	inline bool bet(int x, int min, int max)
	{
		return (x > min && x < max) ? true : false;
	}

	void VThin(Mat& src, bool* judgeTab, int arraynum)
	{
		if (src.data == nullptr || judgeTab == nullptr) return;
		int h = src.rows;
		int w = src.cols;
		int next = 1;
		uint8_t* psrc = src.data;

		for (int y = 0; y < h - 1; ++y)
		{
			for (int x = 1; x < w - 1; ++x)
			{
				if (next == 0)
					next = 1;
				else
				{
					int M = psrc[y * w + x - 1] + psrc[y * w + x] + psrc[y * w + x + 1];
					int a[9] = { 0 };
					if (psrc[y * w + x] == 0 && M != 0)
					{
						for (int k = 0; k < 3; ++k)
						{
							for (int l = 0; l < 3; ++l)
							{
								if (bet(y - 1 + k, -1, h) && bet(x - 1 + k, -1, w) && psrc[(y - 1 + k) * w + x - 1 + l] == 255)
									a[k * 3 + l] = 1;
							}
						}
						int sum = a[0] * 1 + a[1] * 2 + a[2] * 4 + a[3] * 8 + a[5] * 16 + a[6] * 32 + a[7] * 64 + a[8] * 128;
						psrc[y * w + x] = judgeTab[sum] == true ? 255 : 0;
						if (judgeTab[sum] == true)
							next = 0;
					}
				}
			}
		}
	}

	void HThin(Mat& src, bool* judgeTab, int arraynum)
	{
		if (src.data == nullptr || judgeTab == nullptr) return;
		int h = src.rows;
		int w = src.cols;
		int next = 1;
		uint8_t* psrc = src.data;

		for (int x = 0; x < w - 1; ++x)
		{
			for (int y = 1; y < h - 1; ++y)
			{
				if (next == 0)
					next = 1;
				else
				{
					int M = psrc[(y - 1) * w + x] + psrc[y * w + x] + psrc[(y + 1) * w + x];
					if (psrc[y * w + x] == 0 && M != 0)
					{
						int a[9] = { 0 };
						for (int k = 0; k < 3; ++k)
						{
							for (int l = 0; l < 3; ++l)
							{
								if (bet(y - 1 + k, -1, h) && bet(x - 1 + l, -1, w) && psrc[(y - 1 + k) * w + x - 1 + l] == 255)
									a[k * 3 + l] = 1;
							}
						}
						int sum = a[0] * 1 + a[1] * 2 + a[2] * 4 + a[3] * 8 + a[5] * 16 + a[6] * 32 + a[7] * 64 + a[8] * 128;
						psrc[y * w + x] = judgeTab[sum] == true ? 255 : 0;

						if (judgeTab[sum] == true)
							next = 0;
					}
				}
			}
		}
	}

	extern "C" FNNOEXPORT int ThinBinImg(IMat::Mat & src, IMat::Mat & dst, int iternum)
	{
		if (src.m_Format != PixelForm::U8)
		{
			PrintInfo("ThinBinImg:src must be uint8!");
		}
		if (src.data == nullptr)
		{
			PrintInfo("ThinBinImg:src is nullptr!");
			return true;
		}
		Mat temp;
		if (src.data == dst.data || src.IsSameSize(dst) == false)
		{
			temp = src.Clone();
		}
		bool JudgeTab[256] = {
							   false, false, true , true , false, false, true , true , true , true , false, true , true , true , false, true ,
							   true , true , false, false, true , true , true , true , false, false, false, false, false, false, false, true ,
							   false, false, true , true , false, false, true , true , true , true , false, true , true , true , false, true ,
							   true , true , false, false, true , true , true , true , false, false, false, false, false, false, false, true ,
							   true , true , false, false, true , true , false, false, false, false, false, false, false, false, false, false,
							   false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
							   true , true , false, false, true , true , false, false, true , true , false, true , true , true , false, true ,
							   false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
							   false, false, true , true , false, false, true , true , true , true , false, true , true , true , false, true ,
							   true , true , false, false, true , true , true , true , false, false, false, false, false, false, false, true ,
							   false, false, true , true , false, false, true , true , true , true , false, true , true , true , false, true ,
							   true , true , false, false, true , true , true , true , false, false, false, false, false, false, false, false,
							   true , true , false, false, true , true , false, false, false, false, false, false, false, false, false, false,
							   true , true , false, false, true , true , true , true , false, false, false, false, false, false, false, false,
							   true , true , false, false, true , true , false, false, true , true , false, true , true , true , false, false,
							   true , true , false, false, true , true , true , false, true , true , false, false, true , false, false, false
		};

		iternum = max(1, iternum);
		for (int i = 0; i < iternum; ++i)
		{
			VThin(temp, JudgeTab, 256);
			HThin(temp, JudgeTab, 256);
		}
		dst = temp;
		return 1;
	}
};