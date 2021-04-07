#ifndef _IMGBASEGEO_H_
#define _IMGBASEGEO_H_

#include "ImgProcess.h"
namespace IMat
{
	struct TranInfo
	{
		int v00;
		int v01;
		int v02;
		int v10;
		int v11;
		int v12;
		int v20;
		int v21;
		int v22;

		int w;
		int h;
	};

	struct FnLine
	{
		double dx;
		double dy;
		double dz;
		//FnPoint3f pos;
	};

	extern "C" FNNOEXPORT void GetLine2D(float x1, float y1, float x2, float y2, float& a, float& b, float& c);

	extern "C" FNNOEXPORT int MakeMaskByLine(uint16_t * psrc, uint16_t * pdst, int w, int h, float x1, float y1, float x2, float y2, bool isLargeLine, uint16_t color);

	//对图像的左上角进行镜像;
	extern "C" FNNOEXPORT int Copy_RightDown_To_LeftUp(void* psrc, void* pdst, int img_w, int img_h, PixelForm pt, int x, int y, int roi_width);
	//对图像的右上角进行镜像
	extern "C" FNNOEXPORT int Copy_LeftDown_To_RightUp(void* psrc, void* pdst, int img_w, int img_h, PixelForm pt, int x, int y, int roi_width);
	//旋转180度，支持
	extern "C" FNNOEXPORT int Rotate180(uint8_t * psrc, uint8_t * pdst, int w, int h);
	//旋转矩阵
	extern "C" FNNOEXPORT int CalRotateMat_3X3(bool IsRotate90_InCW, TranInfo * input, TranInfo * output);
	//镜像矩阵
	extern "C" FNNOEXPORT int CalMirrorMat_3X3(bool Is_X_Mirror, TranInfo * input, TranInfo * output);
	
	extern "C" FNNOEXPORT int MatTransfor(void* psrc, unsigned int old_w, unsigned int old_h, void* pdst, unsigned int& new_w, unsigned int& new_h, PixelForm pt, TranInfo & tranMat);
	//计算图像的重心
	extern "C" FNNOEXPORT int CalBlockGravity(int* pt_x, int* pt_y, uint64_t ptnum, double& gx, double& gy);
}

#endif