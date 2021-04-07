
#ifndef IMGMATAUX_HPP_H
#define IMGMATAUX_HPP_H


#include <iostream>
#include "Log_xx.h"
#include "FnGlobalEnum.h"
#include "FnGlobalHead.h"
#include <cmath>
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))


template<typename T>
int CopyVal(T* p1, T val, uint64_t count)
{
	if(p1 == nullptr)
	{
		PrintInfo("CopyVal:p1 = nullptr");
		return -1;
	}
	for (uint64_t i = 0; i < count; ++i)
	{
		p1[i] = val;
	}
	return 1;
}

template<typename T>
int IM_Add(T* p1, T* p2, uint64_t count, double tormin, double tormax)
{
	if (p1 == nullptr)
	{
		PrintInfo("IM_Add:p1=nullptr");
		return -1;
	}
	if (p2 == nullptr)
	{
		PrintInfo("IM_Add:p2=nullptr");
		return -2;
	}
	if (sizeof(T) < 4)
	{
		for (uint64_t i = 0; i < count; ++i)
		{
			p1[i] = (T)min(max((double)p1[i] + (double)p2[i], tormin), tormax);
		}
	}
	else
	{
		for (uint64_t i = 0; i < count; ++i)
		{
			p1[i] = p1[i] + p2[i];
		}
	}
	return 1;
}

template<typename T>
int IM_Add(T* p1, T* p2, T *p3,uint64_t count, double tormin, double tormax)
{
	if (p1 == nullptr)
	{
		PrintInfo("IM_Add:p1=nullptr");
		return -1;
	}
	if (p2 == nullptr)
	{
		PrintInfo("IM_Add:p2=nullptr");
		return -2;
	}
	if (p3 == nullptr)
	{
		PrintInfo("IM_Add:p3=nullptr");
		return -3;
	}
	if (sizeof(T) < 4)
	{
		for (uint64_t i = 0; i < count; ++i)
		{
			p3[i] = (T)min(max((double)p1[i] + (double)p2[i], tormin), tormax);
		}
	}
	else
	{
		for (uint64_t i = 0; i < count; ++i)
		{
			p3[i] = p1[i] + p2[i];
		}
	}
	return 1;
}

template<typename T>
int IM_Sub(T* p1, T* p2, uint64_t count, double tormin, double tormax)
{
	if (p1 == nullptr)
	{
		PrintInfo("IM_Sub:p1=nullptr");
		return -1;
	}
	if (p2 == nullptr)
	{
		PrintInfo("IM_Sub:p2=nullptr");
		return -2;
	}
	if (sizeof(T) < 4)
	{
		for (uint64_t i = 0; i < count; ++i)
		{
			p1[i] = (T)min(max((double)p1[i] - (double)p2[i], tormin), tormax);
		}
	}
	else
	{
		for (uint64_t i = 0; i < count; ++i)
		{
			p1[i] =(T) (p1[i] - p2[i]);
		}
	}
	return 1;
}

template<typename T>
int IM_Sub(T* p1, T* p2, T *p3,uint64_t count, double tormin, double tormax)
{
	if (p1 == nullptr)
	{
		PrintInfo("IM_Sub:p1=nullptr");		
		return -1;
	}
	if (p2 == nullptr)
	{		
		PrintInfo("IM_Sub:p2=nullptr");
		return -2;
	}
	if (p3 == nullptr)
	{
		PrintInfo("IM_Sub:p3=nullptr");
		return -3;
	}
	if (sizeof(T) < 4)
	{
		for (uint64_t i = 0; i < count; ++i)
		{
			p3[i] = (T)min(max((double)p1[i] - (double)p2[i], tormin), tormax);
		}
	}
	else
	{
		for (uint64_t i = 0; i < count; ++i)
		{
			p3[i] = p1[i] - p2[i];
		}
	}
	return 1;
}

template<typename T>
int IM_DotMul(T* p1, T* p2, double* p3, uint64_t count, double tormin, double tormax)
{
	if (p1 == nullptr)
	{
		PrintInfo("Im_DotMul:p1=nullptr");
		return -1;
	}
	if (p2 == nullptr)
	{
		PrintInfo("Im_DotMul:p2=nullptr");
		return -2;
	}
	if (p3 == nullptr)
	{
		PrintInfo("Im_DotMul:p3=nullptr");
		return -3;
	}
	if (sizeof(T) < 4)
	{
		for (uint64_t i = 0; i < count; ++i)
		{
			p3[i] = (T)min(max((double)p1[i] * (double)p2[i], tormin), tormax);
		}
	}
	else
	{
		for (uint64_t i = 0; i < count; ++i)
		{
			p3[i] = p1[i] * p2[i];
		}
	}
	return 1;
}
//计算点击
template<typename T1,typename T2>
double IM_DotProduct(T1* p1, T2* p2, uint64_t count)
{
	if (p1 == nullptr)
	{
		PrintInfo("IM_DotProduct:p1=nullptr");
		return -1;
	}
	if (p2 == nullptr)
	{
		PrintInfo("IM_DotProduct:p2=nullptr");
		return -2;
	}
	double doProductVal = 0.0;
	for (uint64_t i = 0; i < count; ++i)
	{
		doProductVal += (double)p1[i] * (double)p2[i];
	}
	return doProductVal;
}

template<typename T>
int IM_MatMul(T* p1, T* p2, double* p3, uint64_t cols_1,uint64_t rows_1,uint64_t cols_2,uint64_t rows_2)
{
	if (p1 == nullptr)
	{
		PrintInfo("IM_MatMul:p1=nullptr");
		return -1;
	}
	if (p2 == nullptr)
	{
		PrintInfo("IM_MatMul:p2=nullptr");
		return -2;
	}
	if (p3 == nullptr)
	{
		PrintInfo("IM_MatMul:p3=nullptr");
		return -3;
	}
	if (cols_1 != rows_2)
		return -4;
	int rows_3 = (int)rows_1;
	int cols_3 = (int)cols_2;
	memset(p3, 0, sizeof(double) * rows_3 * cols_3);

	for (int y1 = 0; y1 < (int)rows_1; ++y1)
	{
		for (uint64_t x1 = 0; x1 < cols_1; ++x1)
		{
			double r =(double)p1[y1 * cols_1 + x1];
			for (uint64_t k2 = 0; k2 < cols_2; ++k2)
			{
				p3[(uint64_t)y1 * cols_3 + k2] += r * (double)p2[x1 * cols_2 + k2];
			}
		}
	}
	return 1;
}

//模板函数
//获得安全值
template<typename T>
inline  T SafeVal(T val, T min, T max)
{
	val = val < max ? val : max;
	val = val > min ? val : min;
	return val;
}
//获取一张图的最大值和最小值
template<typename T>
int getMinMax(T* pSrc, int length, T& minVal, T& maxVal)
{
	if (pSrc == 0) return 0;
	minVal = maxVal = pSrc[0];
	for (int i = 1; i < length; ++i)
	{
		if (pSrc[i] > maxVal)
			maxVal = pSrc[i];
		else if (pSrc[i] < minVal)
			minVal = pSrc[i];
	}
	return 1;
}

//扩展边界
template<typename T>
int copyBorder(T* pImg, T*& pDst, int img_h, int img_w, int up, int down, int left, int right)    //该方法参照opencv卷积前的边界复制
{
	if (pImg == 0)
		return -1;
	up = min(up, img_h);    down = min(down, img_h);
	left = min(left, img_w); right = min(right, img_w);
	int dst_h = img_h + up + down;
	int dst_w = img_w + left + right;
	if (pDst == 0)
		pDst = new T[dst_h * dst_w];
	int dstInd, imgInd;
	for (int i = 1; i <= up; ++i)     //上方扩展up个镜像
	{
		dstInd = (up - i) * dst_w + left;
		imgInd = i * img_w;
		memcpy(pDst + dstInd, pImg + imgInd, img_w * sizeof(T));
	}
	for (int i = up, len = up + img_h; i < len; ++i) //本体复制
	{
		dstInd = i * dst_w + left;
		imgInd = (i - up) * img_w;
		memcpy(pDst + dstInd, pImg + imgInd, img_w * sizeof(T));
	}

	for (int i = 1; i <= down; ++i)    //下方拓展down个镜像
	{
		dstInd = (up + img_h - 1 + i) * dst_w + left;
		imgInd = (img_h - 1 - i) * img_w;
		memcpy(pDst + dstInd, pImg + imgInd, img_w * sizeof(T));
	}
	for (int y = 0; y < dst_h; ++y)   //左右方向分别拓展left和right个镜像
	{
		dstInd = y * dst_w + left;
		for (int x = 1; x <= left; ++x)
			pDst[dstInd - x] = pDst[dstInd + x];
		dstInd = y * dst_w + dst_w - 1 - right;
		for (int x = 1; x <= right; ++x)
			pDst[dstInd + x] = pDst[dstInd - x];
	}
	return 1;
}

//根据指针类型获得该指针下的最小和最大值，以及是否位float型
template <typename T>
int GetTorMinMax(T* ptr, double& tor_min, double& tor_max, bool& isFloat)
{
	isFloat = false;
	if (typeid(T) == typeid(unsigned char))
	{
		tor_min = 0;
		tor_max = 255;
	}
	else if (typeid(T) == typeid(unsigned short))
	{
		tor_min = 0;
		tor_max = 65535;
	}
	else if (typeid(T) == typeid(char))
	{
		tor_min = -128;
		tor_max = 127;
	}
	else if (typeid(T) == typeid(short))
	{
		tor_min = -32768;
		tor_max = 32767;
	}
	else if (typeid(T) == typeid(int))
	{
		tor_min = (double)INT_MIN;
		tor_max = (double)INT_MAX;
	}
	else if (typeid(T) == typeid(unsigned int))
	{
		tor_min = 0;
		tor_max = double((double)INT_MAX * 2);
	}
	else if (typeid(T) == typeid(float))
	{
		tor_min = FLT_MIN;
		tor_max = FLT_MAX;
		isFloat = true;
	}
	else if (typeid(T) == typeid(double))
	{
		tor_min = DBL_MIN;
		tor_max = DBL_MAX;
		isFloat = true;
	}
	return 1;
}

//具体的convertTo
template<typename T1, typename T2>
int ConvertToBegin(T1* psrc, T2* pdst, int64_t pixelcount, int c1, int c2, double tor_min = 0.0, double tor_max = 65535.0, bool isFloat = false)
{
	if (psrc == nullptr)
		return -1;
	if (pdst == nullptr)
		return -2;
	if (typeid(T1) == typeid(T2))
		return -3;
	const double offset = 0.5;
	if (c1 == c2)
	{
		if (c2 != 4)
		{
			int64_t totalSize = pixelcount * c1;
			for (int64_t i = 0; i < totalSize; ++i)
			{
				double val = (double)psrc[i] + offset;
				val = val <= tor_max ? val : tor_max;
				val = val >= tor_min ? val : tor_min;
				pdst[i] = (T2)val;
			}
		}
		else if (c2 == 4)
		{
			for (int64_t i = 0; i < pixelcount; ++i)
			{
				for (int c = 0; c < 3; ++c)
				{
					double val = (double)psrc[i * 4 + c] + offset;
					val = val <= tor_max ? val : tor_max;
					val = val >= tor_min ? val : tor_min;
					pdst[i] = (T2)val;
				}
				pdst[i] = (T2)(isFloat == true ? 1.0 : tor_max);
			}
		}
	}
	else if (c1 == 1)
	{
		if (c2 == 3)
		{
			for (int64_t i = 0; i < pixelcount; ++i)
			{
				double val = psrc[i] + offset;
				val = val <= tor_max ? val : tor_max;
				val = val >= tor_min ? val : tor_min;
				pdst[i * 3] = (T2)val;
				pdst[i * 3 + 1] = (T2)val;
				pdst[i * 3 + 2] = (T2)val;
			}
		}
		else if (c2 == 4)
		{
			for (int64_t i = 0; i < pixelcount; ++i)
			{
				double val = psrc[i] + offset;
				val = val <= tor_max ? val : tor_max;
				val = val >= tor_min ? val : tor_min;
				pdst[i * 4] = (T2)val;
				pdst[i * 4 + 1] = (T2)val;
				pdst[i * 4 + 2] = (T2)val;
				pdst[i * 4 + 3] = (T2)(isFloat == true ? 1.0f : tor_max);
			}
		}
	}
	else if (c1 == 3)
	{
		if (c2 == 1)
		{
			for (int64_t i = 0; i < pixelcount; ++i)
			{
				double val = ((double)psrc[i * 3] + (double)psrc[i * 3 + 1] + (double)psrc[i * 3 + 2]) / 3.0+ offset;
				val = val <= tor_max ? val : tor_max;
				val = val >= tor_min ? val : tor_min;
				pdst[i] = (T2)val;
			}
		}
		else if (c2 == 4)
		{
			for (int64_t i = 0; i < pixelcount; ++i)
			{
				for (int c = 0; c < 3; ++c)
				{
					double val = (double)psrc[i * 3 + c] + offset;
					val = val <= tor_max ? val : tor_max;
					val = val >= tor_min ? val : tor_min;
					pdst[i * 4 + c] = (T2)val;
				}
				pdst[i * 4 + 3] = (T2)(isFloat == true ? 1.0 : tor_max);
			}
		}
	}
	else if (c1 == 4)
	{
		if (c2 == 1)
		{
			for (int64_t i = 0; i < pixelcount; ++i)
			{
				double val =((double)psrc[i * 4] + (double)psrc[i * 4 + 1]+ (double)psrc[i * 4+2])/3.0 + offset;
				val = val <= tor_max ? val : tor_max;
				val = val >= tor_min ? val : tor_min;
				pdst[i] = (T2)val;
			}
		}
		else if (c2 == 3)
		{
			for (int64_t i = 0; i < pixelcount; ++i)
			{
				for (int c = 0; c < 3; ++c)
				{
					double val =(double) psrc[i * 4 + c] + offset;
					val = val <= tor_max ? val : tor_max;
					val = val >= tor_min ? val : tor_min;
					pdst[i * 3 + c] = (T2)val;
				}
			}
		}
	}
	return 1;
}

#undef max
#undef min
#endif // IMGMATAUX_HPP_H