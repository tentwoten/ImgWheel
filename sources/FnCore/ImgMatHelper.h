#ifndef _IMGMATHELPER_H_
#define _IMGMATHELPER_H_

#include "FnGlobalEnum.h"
#include "FnGlobalHead.h"
//根据像素类型，获得该像素的可能的最大值
FNNOEXPORT int GetTorMinMax(PixelForm pt, double& tor_min, double& tor_max, bool& isFloat);
//每个像素的字节大小
FNNOEXPORT int GetPixelByteSize(PixelForm pt);
//像素通道数量
FNNOEXPORT int GetChannel(PixelForm pt);
//返回指针类型
FNNOEXPORT PtrType GetPtrType(PixelForm pt);

#endif // !_IMGMATHELPER_H_