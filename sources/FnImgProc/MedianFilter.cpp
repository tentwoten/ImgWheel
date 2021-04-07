#include "ImgProcess.h"
#include <future>
using namespace std;
namespace IMat
{
#define _USE_MULTTHREAD_
	//仿函数
	class OPF
	{
	public:
		template<typename T>
		inline void operator()(T& a, T& b)
		{
			if (a < b)
			{
				T t = a;
				a = b;
				b = t;
			}
		}
	};

	template<typename T>
	int partition(T* array, int left, int right)
	{
		int i = left;
		int j = right;
		T temp = array[i];
		while (i < j)
		{
			while (i < j && array[j] >= temp) j--;
			if (i < j)
				array[i] = array[j];
			while (i < j && array[i] <= temp) i++;
			if (i < j)
				array[j] = array[i];
		}
		array[i] = temp;
		return i;
	}
	//根据快排原始代码进行优化中值滤波速度，比原始版本提高了一倍的速度,不过比opencv差远了
	template<typename T>
	void FindMid(T* array, int left, int right)
	{
		int dp = 0;
		dp = partition(array, left, right);
		int half = (right - left + 1) / 2;
		while (dp != half)
		{
			if (dp < half)
			{
				left = dp + 1;
				dp = partition(array, left, right);
			}
			else
			{
				right = dp - 1;
				dp = partition(array, left, right);
			}
		}
	}
	template<typename T>
	void QuickSort(T* array, int left, int right)
	{
		if (left < right)
		{
			int dp = partition(array, left, right);
			QuickSort(array, dp + 1, right);
			QuickSort(array, left, dp - 1);
		}
	}

	template<typename T>
	inline  void op(T& a, T& b)
	{
		if ((a) < (b))
		{
			T t = a;
			a = b;
			b = t;
		}
	}

	template<typename T>
	void MedianBlurKernel(T* temp_p, T* pdst, int temp_w, int temp_h, int w, int h, int begin_y, int end_y, int r)
	{
		for (int y = begin_y; y <= end_y; ++y)
		{
			for (int x = 0; x < w; ++x)
			{
				int by = y - r;
				int bx = x - r;
				T* p = temp_p + by * temp_w + bx;
				T a0 = p[0], a1 = p[1], a2 = p[2], a3 = p[3], a4 = p[4];
				T a5 = p[0 + temp_w], a6 = p[1 + temp_w], a7 = p[2 + temp_w], a8 = p[3 + temp_w], a9 = p[4 + temp_w];
				T a10 = p[0 + 2 * temp_w], a11 = p[1 + 2 * temp_w], a12 = p[2 + 2 * temp_w], a13 = p[3 + 2 * temp_w], a14 = p[4 + 2 * temp_w];
				T a15 = p[0 + 3 * temp_w], a16 = p[1 + 3 * temp_w], a17 = p[2 + 3 * temp_w], a18 = p[3 + 3 * temp_w], a19 = p[4 + 3 * temp_w];
				T a20 = p[0 + 4 * temp_w], a21 = p[1 + 4 * temp_w], a22 = p[2 + 4 * temp_w], a23 = p[3 + 4 * temp_w], a24 = p[4 + 4 * temp_w];
				OPF opf;
				opf(a1, a2);   opf(a0, a1);    opf(a1, a2);   opf(a4, a5);   opf(a3, a4);
				opf(a4, a5);   opf(a0, a3);    opf(a2, a5);   opf(a2, a3);   opf(a1, a4);
				opf(a1, a2);   opf(a3, a4);    opf(a7, a8);   opf(a6, a7);   opf(a7, a8);
				opf(a10, a11); opf(a9, a10);   opf(a10, a11); opf(a6, a9);   opf(a8, a11);
				opf(a8, a9);   opf(a7, a10);   opf(a7, a8);   opf(a9, a10);  opf(a0, a6);
				opf(a4, a10);  opf(a4, a6);    opf(a2, a8);   opf(a2, a4);   opf(a6, a8);
				opf(a1, a7);   opf(a5, a11);   opf(a5, a7);   opf(a3, a9); opf(a3, a5);
				opf(a7, a9);   opf(a1, a2);    opf(a3, a4);   opf(a5, a6); opf(a7, a8);
				opf(a9, a10);  opf(a13, a14);  opf(a12, a13); opf(a13, a14); opf(a16, a17);
				opf(a15, a16); opf(a16, a17);  opf(a12, a15); opf(a14, a17); opf(a14, a15);
				opf(a13, a16); opf(a13, a14);  opf(a15, a16); opf(a19, a20); opf(a18, a19);
				opf(a19, a20); opf(a21, a22);  opf(a23, a24); opf(a21, a23); opf(a22, a24);
				opf(a22, a23); opf(a18, a21);  opf(a20, a23); opf(a20, a21); opf(a19, a22);
				opf(a22, a24); opf(a19, a20);  opf(a21, a22); opf(a23, a24); opf(a12, a18);
				opf(a16, a22); opf(a16, a18);  opf(a14, a20); opf(a20, a24); opf(a14, a16);
				opf(a18, a20); opf(a22, a24);  opf(a13, a19); opf(a17, a23); opf(a17, a19);
				opf(a15, a21); opf(a15, a17);  opf(a19, a21); opf(a13, a14); opf(a15, a16);
				opf(a17, a18); opf(a19, a20);  opf(a21, a22); opf(a23, a24); opf(a0, a12);
				opf(a8, a20);  opf(a8, a12);   opf(a4, a16);  opf(a16, a24); opf(a12, a16);
				opf(a2, a14);  opf(a10, a22);  opf(a10, a14); opf(a6, a18);  opf(a6, a10);
				opf(a10, a12); opf(a1, a13);   opf(a9, a21);  opf(a9, a13);  opf(a5, a17);
				opf(a13, a17); opf(a3, a15);   opf(a11, a23); opf(a11, a15); opf(a7, a19);
				opf(a7, a11);  opf(a11, a13);  opf(a11, a12);
				pdst[y * w + x] = a12;
			}
		}
	}

	//小核的中值滤波直接计算即可
	template<typename T>
	int Median_Blur_Begin(T* psrc, T* pdst, int w, int h, int r)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		T* temp = NULL;
		copyBorder(psrc, temp, h, w, r, r, r, r);
		if (temp == NULL) return -3;
		int d = 2 * r + 1;
		T* array = new T[d * d];
		do
		{
			int temp_w = w + 2 * r;
			int temp_h = w + 2 * r;
			T* temp_p = temp + temp_w * r + r;
			int ker_w = 2 * r + 1;
			int ker_size = ker_w * ker_w;
			if (r > 2)
			{
				for (int y = 0; y < h; ++y)
				{
					for (int x = 0; x < w; ++x)
					{
						int by = y - r;
						int ey = y + r;
						int bx = x - r;
						for (int yy = by; yy <= ey; ++yy)
						{
							int temp_ind = yy * temp_w + bx;
							int array_ind = (yy - by) * ker_w;
							memcpy(array + array_ind, temp_p + temp_ind, ker_w * sizeof(T));
						}
						FindMid(array, 0, ker_size - 1);
						pdst[y * w + x] = array[ker_size / 2];
					}
				}
			}
			else if (r == 1)
			{
				for (int y = 0; y < h; ++y)
				{
					for (int x = 0; x < w; ++x)
					{
						int by = y - 1;
						int bx = x - 1;
						T* p1 = temp_p + by * temp_w + bx;
						T* p2 = p1 + temp_w;
						T* p3 = p2 + temp_w;
						T a0 = *p1, a1 = *(p1 + 1), a2 = *(p1 + 2);
						T a3 = *p2, a4 = *(p2 + 1), a5 = *(p2 + 2);
						T a6 = *p3, a7 = *(p3 + 1), a8 = *(p3 + 2);
						op(a1, a2); op(a4, a5); op(a7, a8); op(a0, a1);
						op(a3, a4); op(a6, a7); op(a1, a2); op(a4, a5);
						op(a7, a8); op(a0, a3); op(a5, a8); op(a4, a7);
						op(a3, a6); op(a1, a4); op(a2, a5); op(a4, a7);
						op(a4, a2); op(a6, a4); op(a4, a2);
						pdst[y * w + x] = a4;
					}
				}
			}
			else if (r == 2)
			{
#ifdef _USE_MULTTHREAD_
				future<void> f1 = std::async(MedianBlurKernel<T>, temp_p, pdst, temp_w, temp_h, w, h, 0, h / 2, r);
				future<void> f2 = std::async(MedianBlurKernel<T>, temp_p, pdst, temp_w, temp_h, w, h, h / 2 + 1, h - 1, r);
				f1.wait();
				f2.wait();
#else
				for (int y = 0; y < h; ++y)
				{
					for (int x = 0; x < w; ++x)
					{
						int by = y - r;
						int bx = x - r;
						T* p = temp_p + by * temp_w + bx;
						T a0 = p[0], a1 = p[1], a2 = p[2], a3 = p[3], a4 = p[4];
						T a5 = p[0 + temp_w], a6 = p[1 + temp_w], a7 = p[2 + temp_w], a8 = p[3 + temp_w], a9 = p[4 + temp_w];
						T a10 = p[0 + 2 * temp_w], a11 = p[1 + 2 * temp_w], a12 = p[2 + 2 * temp_w], a13 = p[3 + 2 * temp_w], a14 = p[4 + 2 * temp_w];
						T a15 = p[0 + 3 * temp_w], a16 = p[1 + 3 * temp_w], a17 = p[2 + 3 * temp_w], a18 = p[3 + 3 * temp_w], a19 = p[4 + 3 * temp_w];
						T a20 = p[0 + 4 * temp_w], a21 = p[1 + 4 * temp_w], a22 = p[2 + 4 * temp_w], a23 = p[3 + 4 * temp_w], a24 = p[4 + 4 * temp_w];

						op1(a1, a2); op1(a0, a1); op1(a1, a2); op1(a4, a5); op1(a3, a4);
						op1(a4, a5); op1(a0, a3); op1(a2, a5); op1(a2, a3); op1(a1, a4);
						op1(a1, a2); op1(a3, a4); op1(a7, a8); op1(a6, a7); op1(a7, a8);
						op1(a10, a11); op1(a9, a10); op1(a10, a11); op1(a6, a9); op1(a8, a11);
						op1(a8, a9); op1(a7, a10); op1(a7, a8); op1(a9, a10); op1(a0, a6);
						op1(a4, a10); op1(a4, a6); op1(a2, a8); op1(a2, a4); op1(a6, a8);
						op1(a1, a7); op1(a5, a11); op1(a5, a7); op1(a3, a9); op1(a3, a5);
						op1(a7, a9); op1(a1, a2); op1(a3, a4); op1(a5, a6); op1(a7, a8);
						op1(a9, a10); op1(a13, a14); op1(a12, a13); op1(a13, a14); op1(a16, a17);
						op1(a15, a16); op1(a16, a17); op1(a12, a15); op1(a14, a17); op1(a14, a15);
						op1(a13, a16); op1(a13, a14); op1(a15, a16); op1(a19, a20); op1(a18, a19);
						op1(a19, a20); op1(a21, a22); op1(a23, a24); op1(a21, a23); op1(a22, a24);
						op1(a22, a23); op1(a18, a21); op1(a20, a23); op1(a20, a21); op1(a19, a22);
						op1(a22, a24); op1(a19, a20); op1(a21, a22); op1(a23, a24); op1(a12, a18);
						op1(a16, a22); op1(a16, a18); op1(a14, a20); op1(a20, a24); op1(a14, a16);
						op1(a18, a20); op1(a22, a24); op1(a13, a19); op1(a17, a23); op1(a17, a19);
						op1(a15, a21); op1(a15, a17); op1(a19, a21); op1(a13, a14); op1(a15, a16);
						op1(a17, a18); op1(a19, a20); op1(a21, a22); op1(a23, a24); op1(a0, a12);
						op1(a8, a20); op1(a8, a12); op1(a4, a16); op1(a16, a24); op1(a12, a16);
						op1(a2, a14); op1(a10, a22); op1(a10, a14); op1(a6, a18); op1(a6, a10);
						op1(a10, a12); op1(a1, a13); op1(a9, a21); op1(a9, a13); op1(a5, a17);
						op1(a13, a17); op1(a3, a15); op1(a11, a23); op1(a11, a15); op1(a7, a19);
						op1(a7, a11); op1(a11, a13); op1(a11, a12);
						pdst[y * w + x] = a12;
					}
				}
#endif // _USE_MULTTHREAD_
			}
		} while (false);
		delete[]temp;
		delete[]array;
		return 1;
	}

	//中值滤波，支持原位操作
	extern "C" FNNOEXPORT int Median_Blur(void* psrc, void* pdst, int w, int h, int r, PixelForm pixelType)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		if (pixelType == PixelForm::U16)
			return Median_Blur_Begin((uint16_t*)psrc, (uint16_t*)pdst, w, h, r);
		else if (pixelType == PixelForm::S16)
			return Median_Blur_Begin((short*)psrc, (short*)pdst, w, h, r);
		else if (pixelType == PixelForm::U8)
			return Median_Blur_Begin((uint8_t*)psrc, (uint8_t*)pdst, w, h, r);
		else if (pixelType == PixelForm::F32)
			return Median_Blur_Begin((float*)psrc, (float*)pdst, w, h, r);
		else
			return -3;
		return 1;
	}
};