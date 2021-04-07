#include "ImgProcess.h"
namespace IMat
{
	//图像剪切32位
	template<typename T>
	int edgeClip(T* pSrc, T* pDst, int img_h, int img_w, clipParam m)
	{
		int up = max(m.up, 0);
		int down = max(m.down, 0);
		int left = max(m.left, 0);
		int right = max(m.right, 0);
		int new_h = img_h - up - down;
		int new_w = img_w - left - right;
		for (int y = 0; y < new_h; ++y)
		{
			int pSrcIndex = (y + up) * img_w + left;
			int pDstIndex = y * new_w;
			//memcpy(pDst+pDstIndex,pSrc+pSrcIndex,new_w*sizeof(T));
			for (int x = 0; x < new_w; ++x)
				pDst[pDstIndex + x] = pSrc[pSrcIndex + x];
		}
		return 1;
	}
	/* 参数1：原图，参数2：输出图，参数3：原图高度，参数4：原图宽度，参数5：边缘裁剪结构体，参数6：结构体读取文档*/
	int edgeClip_32F(float* pSrc, float* pDst, int img_h, int img_w, clipParam m, std::string filePath)
	{
		if (filePath.size() != 0)
			m = clipParam(filePath.c_str());
		if (pSrc == 0 || pDst == 0)
			return -1;
		edgeClip<float>(pSrc, pDst, img_h, img_w, m);
		return 1;
	}

	//图像剪切16位
	int edgeClip_16U(uint16_t* pSrc, uint16_t* pDst, int img_h, int img_w, clipParam m, std::string filePath)
	{
		if (filePath.size() != 0)
			m = clipParam(filePath.c_str());
		if (pSrc == 0 || pDst == 0)
			return -1;
		pDst[0] = 255;
		edgeClip<uint16_t>(pSrc, pDst, img_h, img_w, m);
		return 1;
	}
	//图像剪切8位
	int edgeClip_8U(uint8_t* pSrc, uint8_t* pDst, int img_h, int img_w, clipParam m, std::string filePath)
	{
		if (filePath.size() != 0)
			m = clipParam(filePath.c_str());
		if (pSrc == 0 || pDst == 0)
			return -1;
		edgeClip<uint8_t>(pSrc, pDst, img_h, img_w, m);
		return 1;
	}
};
