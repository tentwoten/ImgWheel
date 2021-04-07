#include "ImgProcess.h"
#include <iostream>
namespace IMat
{
	//仅限float型指针，可原位操作，速度快于BoxBlur, 精度不如BoxBlur
	extern "C" FNNOEXPORT int FastBoxBlur(float* psrc, float* pdst, int w, int h, int r)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		if (r < 1) return -3;
		r = r > (w - 1) / 2 ? (w - 1) / 2 : r;
		r = r > (h - 1) / 2 ? (h - 1) / 2 : r;

		float* temp = new float[w * h];
		//先横向
		int d = 2 * r + 1;
		for (int y = 0; y < h; ++y)
		{
			float sum = psrc[y * w];
			for (int xx = 1; xx <= r; ++xx)
				sum += 2 * psrc[y * w + xx];  // |1 2 3 4 5| ==> 3 2|1 2 3 4 5|   
			//计算第一个像素	 
			temp[y * w] = sum;
			//从第二个像素开始计算
			int x = 1;
			for (; x <= r; ++x)
			{
				sum += psrc[y * w + x + r] - psrc[y * w + r - x + 1];
				temp[y * w + x] = sum;
			}
			for (; x < w - r; ++x)
			{
				sum += psrc[y * w + x + r] - psrc[y * w + x - r - 1];
				temp[y * w + x] = sum;
			}
			for (; x < w; ++x)
			{
				sum -= (psrc[y * w + x - r - 1] - psrc[y * w + w * 2 - x - r - 2]);  // 注意    (w - 1) - (x + r - (w - 1)) => 2*w-x-r-2
				temp[y * w + x] = sum;
			}
		}
		////纵向计算
		float* sum_array = new float[w];
		memcpy(sum_array, temp, sizeof(float) * w);  //先拷贝第一行
		for (int yy = 1; yy <= r; ++yy)
			for (int x = 0; x < w; ++x)
				sum_array[x] += 2 * temp[(int)yy * w + x];
		float coe = 1.0f / (d * d);
		//先计算第0行
		for (int x = 0; x < w; ++x)
			pdst[x] = sum_array[x] * coe;
		//计算第1行
		int y = 1;
		for (; y <= r; ++y)
		{
			for (int x = 0; x < w; ++x)
			{
				sum_array[x] += temp[(y + r) * w + x] - temp[(r - y + 1) * w + x];
				pdst[y * w + x] = sum_array[x] * coe;
			}
		}
		for (; y < h - r; ++y)
		{
			for (int x = 0; x < w; ++x)
			{
				sum_array[x] += temp[(y + r) * w + x] - temp[(y - r - 1) * w + x];
				pdst[y * w + x] = sum_array[x] * coe;
			}
		}
		for (; y < h; ++y)
		{
			for (int x = 0; x < w; ++x)
			{
				sum_array[x] -= temp[(y - r - 1) * w + x] - temp[(h * 2 - y - r - 2) * w + x];
				pdst[y * w + x] = sum_array[x] * coe;
			}
		}
		delete[] sum_array;
		delete[] temp;
		return 1;
	}

	template<typename T>
	int DoFastBoxBlur(T* psrc, T* pdst, int w, int h, int r)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		if (r < 1) return -3;
		r = r > w / 2 ? w / 2 : r;
		r = r > h / 2 ? h / 2 : r;

		double* temp = new double[w * h];
		//先横向
		int d = 2 * r + 1;
		double coe = 1.0 / (d * d);
		for (int y = 0; y < h; ++y)
		{
			double sum = psrc[y * w];
			for (int xx = 1; xx <= r; ++xx)
				sum += 2 * psrc[y * w + xx];  // |1 2 3 4 5| ==> 3 2|1 2 3 4 5|   
			//计算第一个像素	 
			temp[y * w] = sum;
			//从第二个像素开始计算
			int x = 1;
			for (; x <= r; ++x)
			{
				sum += psrc[y * w + x + r] - psrc[y * w + r - x + 1];
				temp[y * w + x] = sum;
			}
			for (; x < w - r; ++x)
			{
				sum += psrc[y * w + x + r] - psrc[y * w + x - r - 1];
				temp[y * w + x] = sum;
			}
			for (; x < w; ++x)
			{
				sum -= (psrc[y * w + x - r - 1] - psrc[y * w + w * 2 - x - r - 2]);  // 注意    (w - 1) - (x + r - (w - 1)) => 2*w-x-r-2
				temp[y * w + x] = sum;
			}
		}
		////纵向计算
		double* sum_array = new double[w];
		for (int i = 0; i < w; ++i)
			sum_array[i] = temp[i];
		for (int yy = 1; yy <= r; ++yy)
			for (int x = 0; x < w; ++x)
				sum_array[x] += 2 * temp[yy * w + x];
		//先计算第0行
		double tor_min = 0.0, tor_max = 65535.0;
		bool isFloat = 0;
		GetTorMinMax(psrc, tor_min, tor_max, isFloat);
		double accurate = isFloat == true ? 0 : 0.5;

		if (typeid(T) == typeid(float) || typeid(T) == typeid(double))
		{
			for (int x = 0; x < w; ++x)
				pdst[x] = (T)sum_array[x] * coe;
			int y = 1;
			for (; y <= r; ++y)
			{
				for (int x = 0; x < w; ++x)
				{
					sum_array[x] += temp[(y + r) * w + x] - temp[(r - y + 1) * w + x];
					pdst[y * w + x] = (T)(sum_array[x] * coe);
				}
			}
			for (; y < h - r; ++y)
			{
				for (int x = 0; x < w; ++x)
				{
					sum_array[x] += temp[(y + r) * w + x] - temp[(y - r - 1) * w + x];
					pdst[y * w + x] = (T)(sum_array[x] * coe);
				}
			}
			for (; y < h; ++y)
			{
				for (int x = 0; x < w; ++x)
				{
					sum_array[x] -= temp[(y - r - 1) * w + x] - temp[(h * 2 - y - r - 2) * w + x];
					pdst[y * w + x] = (T)(sum_array[x] * coe);
				}
			}
		}
		else
		{
			for (int x = 0; x < w; ++x)
				pdst[x] = (T)min(max(sum_array[x] * coe + 0.5, tor_min), tor_max);
			int y = 1;
			for (; y <= r; ++y)
			{
				for (int x = 0; x < w; ++x)
				{
					sum_array[x] += temp[(y + r) * w + x] - temp[(r - y + 1) * w + x];
					pdst[y * w + x] = (T)min(max(sum_array[x] * coe + 0.5, tor_min), tor_max);
				}
			}
			for (; y < h - r; ++y)
			{
				for (int x = 0; x < w; ++x)
				{
					sum_array[x] += temp[(y + r) * w + x] - temp[(y - r - 1) * w + x];
					pdst[y * w + x] = (T)min(max(sum_array[x] * coe + 0.5, tor_min), tor_max);
				}
			}
			for (; y < h; ++y)
			{
				for (int x = 0; x < w; ++x)
				{
					sum_array[x] -= temp[(y - r - 1) * w + x] - temp[(h * 2 - y - r - 2) * w + x];
					pdst[y * w + x] = (T)min(max(sum_array[x] * coe + 0.5, tor_min), tor_max);
				}
			}
		}
		delete[] sum_array;
		delete[] temp;
		return 1;
	}
	//仅限单通道，指针类型任意,可原位操作,精度高于FastBoxBlur,速度不如FastBoxBlur
	FNNOEXPORT int BoxBlur(void* psrc, void* pdst, int w, int h, PixelForm type, int r)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		if (type == PixelForm::U8)
			return DoFastBoxBlur((uint8_t*)psrc, (uint8_t*)pdst, w, h, r);
		if (type == PixelForm::U16)
			return DoFastBoxBlur((uint16_t*)psrc, (uint16_t*)pdst, w, h, r);
		if (type == PixelForm::U32)
			return DoFastBoxBlur((uint32_t*)psrc, (uint32_t*)pdst, w, h, r);
		if (type == PixelForm::F32)
			return DoFastBoxBlur((float*)psrc, (float*)pdst, w, h, r);
		if (type == PixelForm::F64)
			return DoFastBoxBlur((double*)psrc, (double*)pdst, w, h, r);
		return -3;
	}
};