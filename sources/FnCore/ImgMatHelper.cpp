#include "ImgMatHelper.h"
#include <iostream>
//根据像素类型，获得该像素的可能的最大值 
int GetTorMinMax(PixelForm pt, double& tor_min, double& tor_max, bool& isFloat)
{
	isFloat = false;
	if (pt == PixelForm::U8)
	{
		tor_min = 0;
		tor_max = 255;
	}
	else if (pt == PixelForm::U16)
	{
		tor_min = 0;
		tor_max = 65535;
	}
	else if (pt == PixelForm::S8)
	{
		tor_min = -128;
		tor_max = 127;
	}
	else if (pt == PixelForm::S16)
	{
		tor_min = -32768;
		tor_max = 32767;
	}
	else if (pt == PixelForm::S32)
	{
		tor_min = (double)INT_MIN;
		tor_max = (double)INT_MAX;
	}
	else if (pt == PixelForm::U32)
	{
		tor_min = 0;
		tor_max = double((double)INT_MAX * 2);
	}
	else if (pt == PixelForm::F32)
	{
		tor_min = FLT_MIN;
		tor_max = FLT_MAX;
		isFloat = true;
	}
	else if (pt == PixelForm::F64)
	{
		tor_min = DBL_MIN;
		tor_max = DBL_MAX;
		isFloat = true;
	}
	return 1;
}
//每个像素的字节大小
int GetPixelByteSize(PixelForm pt)
{
	if (pt == PixelForm::U8 || pt == PixelForm::S8) return 1;
	if (pt == PixelForm::U16 || pt == PixelForm::S16) return 2;
	if (pt == PixelForm::U8_RGB) return 3;
	if (pt == PixelForm::F32 || pt == PixelForm::U32 || pt == PixelForm::S32
		|| pt == PixelForm::U8_BGRA || pt == PixelForm::U8_RGBA)
		return 4;
	if (pt == PixelForm::U16_RGB) return 6;
	return 8;
}
//像素通道数量
int GetChannel(PixelForm pt)
{
	if (pt == PixelForm::U8_RGB || pt == PixelForm::U16_RGB)   return 3;
	if (pt == PixelForm::U8_RGBA || pt == PixelForm::U8_BGRA) return 4;
	return 1;
}
//返回指针类型
PtrType GetPtrType(PixelForm pt)
{
	if (pt == PixelForm::S8 || pt == PixelForm::S8_RGB || pt == PixelForm::S8_RGBA || pt == PixelForm::S8_BGRA)
		return PtrType::S8;
	if (pt == PixelForm::U8 || pt == PixelForm::U8_RGB || pt == PixelForm::U8_RGBA || pt == PixelForm::U8_BGRA)
		return PtrType::U8;
	if (pt == PixelForm::S16 || pt == PixelForm::S16_RGB)
		return PtrType::S16;
	if (pt == PixelForm::U16 || pt == PixelForm::U16_RGB)
		return PtrType::U16;
	if (pt == PixelForm::S32 || pt == PixelForm::S32_RGB)
		return PtrType::S32;
	if (pt == PixelForm::U32 || pt == PixelForm::U32_RGB)
		return PtrType::U32;
	if (pt == PixelForm::F32)
		return PtrType::F32;
	if (pt == PixelForm::F64)
		return PtrType::F64;
	return PtrType::U8;
}
