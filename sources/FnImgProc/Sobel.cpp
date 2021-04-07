#include "ImgProcess.h"
namespace IMat
{
	FNNOEXPORT int Sobel2D(void* psrc, void* pdst, int w, int h, int r, bool x_or_y, PixelForm pt)
	{
		if (psrc == nullptr)
			return -1;
		if (pdst == nullptr)
			return -1;

		int d = 2 * r + 1;
		int kerSize = d * d;
		float* sobelKer = nullptr;

		sobelKer = new float[d];
		if (r == 1)
		{
			sobelKer[0] = 1, sobelKer[1] = 2, sobelKer[2] = 1;
			sobelKer[3] = 0, sobelKer[4] = 0, sobelKer[5] = 0;
			sobelKer[3] = -1, sobelKer[4] = -2, sobelKer[5] = -1;
		}
		for (int y = 0; y < h; ++y)
		{
			for (int x = 0; x < w; ++x)
			{

			}
		}



		if (sobelKer)
			delete[]sobelKer;
		return 1;
	}
	//获得sobel核
	Mat GetSobelBinCoef(int r, bool is_dir_x)
	{
		r = max(1, r);
		int d = 2 * r + 1;
		int n = d;
		Mat res = Mat::Zeros(d, d, PixelForm::F64);
		double* pd = (double*)res.data;
		//y方向
		if (is_dir_x == false)
		{
			pd[0] = 1.0;
			for (int i = 1; i < r + 1; ++i)
			{
				double nextcof = (d + 1.0 - (double)i) / (double)i;
				pd[i] = pd[i - 1] * nextcof;
			}
			for (int i = r + 2; i < d; ++i)
			{
				pd[i] = pd[2 * r + 2 - i];
			}
			double* bottom_ptr = pd + res.step * (res.rows - 1);
			for (int x = 0; x < res.cols; ++x)
			{
				bottom_ptr[x] = -pd[x];
			}
		}
		//x方向
		if (is_dir_x == false)
		{
			int w = res.cols;
			int h = res.rows;
			pd[0] = -1.0;
			for (int i = 1; i < r + 1; ++i)
			{
				double nextcof = (d + 1.0 - (double)i) / (double)i;
				pd[i * w] = pd[(i - 1) * w] * nextcof;
			}
			for (int i = r + 2; i < d; ++i)
			{
				pd[i * w] = pd[(2 * r + 2 - i) * w];
			}
			double* right_ptr = pd + w - 1;
			for (int y = 0; y < h; ++y)
			{
				right_ptr[y * w] = -pd[y * w];
			}
		}
		return res;
	}



	extern "C" FNNOEXPORT int Sobel2D(IMat::Mat & src, IMat::Mat & dst, int r, bool is_dirX)
	{
		if (src.data == nullptr || dst.data == nullptr)
			return -1;
		Mat res = Mat(src.rows, src.cols, PixelForm::F64);

		Mat ker = GetSobelBinCoef(r, is_dirX);
		//y方向 [ 1 2 1 ]   [1 0 -1]'
		if (is_dirX == false)
		{
			//Conv2D(src, dst, ker, 1);




		}
		else//x方向[-1 -2 -1]'   [1 2 1]'
		{
			Mat temp_pad;
			MatPad(src, temp_pad, r, r, r, r);



		}
		return 1;
	}
};