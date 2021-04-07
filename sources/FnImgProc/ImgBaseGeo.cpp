#include "ImgBaseGeo.h"
#include<algorithm>
#include<vector>
#include "FnCore/ImgMatAux.hpp"
#include "FnCore/ImgMat.hpp"
using namespace std;
namespace IMat
{
	struct Point_2f
	{
		float x;
		float y;
		Point_2f()
		{
			x = 0.0f;
			y = 0.0f;
		}
		Point_2f(float _x, float _y)
		{
			x = _x;
			y = _y;
		}

		Point_2f& operator=(const Point_2f& other)
		{
			if (this == &other || (&other == NULL))
				return *this;
			this->x = other.x;
			this->y = other.y;
			return *this;
		}

		bool operator==(const Point_2f& other)
		{
			if (&other == NULL)
				return false;
			if (abs(this->x - other.x) < 1e-9 && abs(this->y - other.y) < 1e-9)
			{
				return true;  
			}
			return false;
		}
		Point_2f operator+(const Point_2f& other)
		{
			if (&other == NULL)
				return *this;
			this->x += other.x;
			this->y += other.y;
			return *this;
		}
		float DistanceTo(const Point_2f& other)
		{
			return (float)sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
		}
	};

	extern "C" FNNOEXPORT void GetLine2D(float x1, float y1, float x2, float y2, float& a, float& b, float& c)
	{
		Point_2f pt1(x1, y1);
		Point_2f pt2(x2, y2);
		float mod = pt1.DistanceTo(pt2);
		if (mod < 1e-9)   //两点太近，直接默认无直线
		{
			a = 0.0f;
			b = 0.0f;
			c = 0.0f;
			return;
		}
		/*
			y=[(sin(k)/cos(k))*x+c
			sin(k) *x - cos(k) *y +c=0;
			c=cos(k) *y -sin(k) *x;
		*/
		float k_cos = (pt2.x - pt1.x) / mod;
		float k_sin = (pt2.y - pt1.y) / mod;
		a = k_sin;
		b = -k_cos;
		c = k_cos * pt1.y - k_sin * pt1.x;
	}

	extern "C" FNNOEXPORT int MakeMaskByLine(uint16_t * psrc, uint16_t * pdst, int w, int h, float x1, float y1, float x2, float y2, bool isLargeLine, uint16_t color)
	{
		if (psrc == NULL)
			return -1;
		float a, b, c;
		GetLine2D(x1, y1, x2, y2, a, b, c);
		if (a == 0 && b == 0 && c == 0)  //两个点再一起，那么直接退出，无法生成直线做掩膜
			return 1;
		//寻找直线与图像边缘的交点
		if (isLargeLine == true)
		{
			for (int y = 0; y < h; ++y)
				for (int x = 0; x < w; ++x)
					pdst[y * w + x] = (a * x + b * y + c >= 0) ? color : psrc[y * w + x];
		}
		else
		{
			for (int y = 0; y < h; ++y)
				for (int x = 0; x < w; ++x)
					pdst[y * w + x] = (a * x + b * y + c >= 0) ? psrc[y * w + x] : color;
		}
		return 1;
	}

	void MakeSafeRoi(int& x, int& y, int& width, int& height, int w, int h)
	{
		x = max(0, x);
		y = max(0, y);
		x = min(x, w - 1);
		y = min(y, h - 1);

		width = min(width, w - x);
		height = min(height, h - y);
		width = max(0, width);
		height = max(0, height);
	}

	int GetSafeRoi(void* psrc, void* proi, int img_w, int img_h, PixelForm pt, int& roi_x, int& roi_y, int& roi_width, int& roi_height)
	{
		MakeSafeRoi(roi_x, roi_y, roi_width, roi_height, img_w, img_h);
		int PixelByteSize = GetPixelByteSize(pt);
		if (psrc == NULL)
			return -1;
		if (proi == NULL)
			proi = new uint8_t[roi_width * roi_height * PixelByteSize];
		int begin_y = roi_y;
		int begin_x = roi_x;

		uint8_t* psrc8 = (uint8_t*)psrc;
		uint8_t* proi8 = (uint8_t*)proi;

		for (int y = 0; y < (int)roi_height; ++y)
		{
			int img_y_ind = y + roi_y;
			int img_x_ind = 0 + roi_x;
			uint8_t* p_img_loc = psrc8 + (img_w * img_y_ind + img_x_ind) * PixelByteSize;
			uint8_t* p_roi_loc = proi8 + (roi_width * y) * PixelByteSize;
			memcpy(p_roi_loc, p_img_loc, roi_width * PixelByteSize);
		}
		return 1;
	}

	int Do_Copy_RightDown_To_LeftUp(void* psrc, void* pdst, int img_w, int img_h, PixelForm pt, int roi_x, int roi_y, int roi_width)
	{
		if (psrc == NULL)
			return -1;
		if (pdst == NULL)
			return -2;
		int channel = GetChannel(pt);
		int PixelByteSize = GetPixelByteSize(pt);
		uint8_t* roiImg = new uint8_t[PixelByteSize * (roi_width) * (roi_width)];

		GetSafeRoi(psrc, roiImg, img_w, img_h, pt, roi_x, roi_y, roi_width, roi_width);
		if (roiImg == NULL)
			return -3;
		if (roi_width <= 1)
			return 1;

		float x1 = (float)(roi_width - 1);
		float y1 = 0.0f;
		float x2 = 0.0f;
		float y2 = (float)(roi_width - 1);
		float a, b, c;
		GetLine2D(x1, y1, x2, y2, a, b, c);
		float c_div_a = c / a;
		float b_div_a = b / a;
		//uint8_t* temp = new uint8_t[PixelByteSize];
		uint8_t* psrc8 = (uint8_t*)psrc;
		uint8_t* roiImg8 = (uint8_t*)roiImg;

		int rect_w = roi_width;
		int rect_h = roi_width;

		for (int y = 0; y < roi_width; ++y)
		{
			int end_x = (int)ceil(-c_div_a - b_div_a * y);
			end_x = min(roi_width - 1, end_x);
			for (int x = 0; x <= end_x; ++x)
			{
				int ind1 = (rect_w * y + x) * PixelByteSize;
				int y1 = rect_w - 1 - x;
				int x1 = rect_h - 1 - y;
				int ind2 = (rect_w * y1 + x1) * PixelByteSize;
				memcpy(roiImg8 + ind1, roiImg8 + ind2, PixelByteSize);
			}
		}
		if (psrc != pdst)
			memcpy(pdst, psrc, img_w * img_h * PixelByteSize);
		AddWaterMask(pdst, pdst, roiImg, img_w, img_h, roi_width, roi_width, roi_x, roi_y, pt, 1);
		//delete[]temp;
		if (roiImg != NULL)
			delete[]roiImg;
		return 1;
	}

	int Do_Copy_LeftDown_To_RightUp(void* psrc, void* pdst, int img_w, int img_h, PixelForm pt, int roi_x, int roi_y, int roi_width)
	{
		if (psrc == NULL)
			return -1;
		if (pdst == NULL)
			return -2;
		int channel = GetChannel(pt);
		int PixelByteSize = GetPixelByteSize(pt);
		uint8_t* roiImg = new uint8_t[PixelByteSize * (roi_width) * (roi_width)];

		GetSafeRoi(psrc, roiImg, img_w, img_h, pt, roi_x, roi_y, roi_width, roi_width);
		if (roiImg == NULL)
			return -3;
		if (roi_width <= 1)
			return 1;

		float x1 = 0;
		float y1 = 0;
		float x2 = (float)(roi_width - 1);
		float y2 = (float)(roi_width - 1);
		float a, b, c;
		GetLine2D(x1, y1, x2, y2, a, b, c);
		float c_div_a = c / a;
		float b_div_a = b / a;
		//uint8_t* temp = new uint8_t[PixelByteSize];
		uint8_t* psrc8 = (uint8_t*)psrc;
		uint8_t* roiImg8 = (uint8_t*)roiImg;

		int rect_w = roi_width;
		int rect_h = roi_width;

		for (int y = 0; y < rect_h; ++y)
		{
			int begin_x = (int)(round(-c_div_a - b_div_a * y));
			begin_x = min(roi_width - 1, begin_x);
			begin_x = max(0, begin_x);
			for (int x = begin_x; x < rect_w; ++x)
			{
				int ind1 = (y * rect_w + x) * PixelByteSize;
				int ind2 = (x * rect_w + y) * PixelByteSize;
				memcpy(roiImg8 + ind1, roiImg8 + ind2, PixelByteSize);
			}
		}
		if (psrc != pdst)
			memcpy(pdst, psrc, img_w * img_h * PixelByteSize);
		AddWaterMask(pdst, pdst, roiImg, img_w, img_h, roi_width, roi_width, roi_x, roi_y, pt, 1);
		//delete[]temp;
		if (roiImg != NULL)
			delete[]roiImg;
		return 1;
	}


	extern "C" FNNOEXPORT int Copy_RightDown_To_LeftUp(void* psrc, void* pdst, int img_w, int img_h, PixelForm pt, int roi_x, int roi_y, int roi_width)
	{
		if (psrc == NULL)
			return -1;
		if (pdst == NULL)
			return -2;
		int channel = GetChannel(pt);
		int PixelByteSize = GetPixelByteSize(pt);
		uint8_t* roiImg = new uint8_t[PixelByteSize * (roi_width) * (roi_width)];

		GetSafeRoi(psrc, roiImg, img_w, img_h, pt, roi_x, roi_y, roi_width, roi_width);
		if (roiImg == NULL)
			return -3;
		if (roi_width <= 1)
			return 1;

		float x1 = (float)(roi_width - 1);
		float y1 = 0.0f;
		float x2 = 0.0f;
		float y2 = (float)(roi_width - 1);
		float a, b, c;
		GetLine2D(x1, y1, x2, y2, a, b, c);
		float c_div_a = c / a;
		float b_div_a = b / a;
		//uint8_t* temp = new uint8_t[PixelByteSize];
		uint8_t* psrc8 = (uint8_t*)psrc;
		uint8_t* roiImg8 = (uint8_t*)roiImg;

		int rect_w = roi_width;
		int rect_h = roi_width;

		for (int y = 0; y < roi_width; ++y)
		{
			int end_x =(int)ceil(-c_div_a - b_div_a * y);
			end_x = min(roi_width - 1, end_x);
			for (int x = 0; x <= end_x; ++x)
			{
				int ind1 = (rect_w * y + x) * PixelByteSize;
				int y1 = rect_w - 1 - x;
				int x1 = rect_h - 1 - y;
				int ind2 = (rect_w * y1 + x1) * PixelByteSize;
				memcpy(roiImg8 + ind1, roiImg8 + ind2, PixelByteSize);
			}
		}
		if (psrc != pdst)
			memcpy(pdst, psrc, img_w * img_h * PixelByteSize);
		CopyRoiTo(pdst, pdst, roiImg, pt, img_w, img_h, roi_width, roi_width, roi_x, roi_y);

		//AddWaterMask(pdst, pdst, roiImg, img_w, img_h, roi_width, roi_width, roi_x, roi_y, pt, 1);
		//delete[]temp;
		if (roiImg != NULL)
			delete[]roiImg;
		return 1;
	}

	//左下考到右上
	extern "C" FNNOEXPORT int Copy_LeftDown_To_RightUp(void* psrc, void* pdst, int img_w, int img_h, PixelForm pt, int roi_x, int roi_y, int roi_width)
	{
		if (psrc == NULL)
			return -1;
		if (pdst == NULL)
			return -2;
		int channel = GetChannel(pt);
		int PixelByteSize = GetPixelByteSize(pt);
		uint8_t* roiImg = new uint8_t[PixelByteSize * (roi_width) * (roi_width)];

		GetSafeRoi(psrc, roiImg, img_w, img_h, pt, roi_x, roi_y, roi_width, roi_width);
		if (roiImg == NULL)
			return -3;
		if (roi_width <= 1)
			return 1;

		float x1 = 0;
		float y1 = 0;
		float x2 = (float)(roi_width - 1);
		float y2 = (float)(roi_width - 1);
		float a, b, c;
		GetLine2D(x1, y1, x2, y2, a, b, c);
		float c_div_a = c / a;
		float b_div_a = b / a;
		//uint8_t* temp = new uint8_t[PixelByteSize];
		uint8_t* psrc8 = (uint8_t*)psrc;
		uint8_t* roiImg8 = (uint8_t*)roiImg;

		int rect_w = roi_width;
		int rect_h = roi_width;

		for (int y = 0; y < rect_h; ++y)
		{
			int begin_x = (int)(round(-c_div_a - b_div_a * y));
			begin_x = min(roi_width - 1, begin_x);
			begin_x = max(0, begin_x);
			for (int x = begin_x; x < rect_w; ++x)
			{
				int ind1 = (y * rect_w + x) * PixelByteSize;
				int ind2 = (x * rect_w + y) * PixelByteSize;
				memcpy(roiImg8 + ind1, roiImg8 + ind2, PixelByteSize);
			}
		}
		if (psrc != pdst)
			memcpy(pdst, psrc, img_w * img_h * PixelByteSize);

		CopyRoiTo(pdst, pdst, roiImg, pt, img_w, img_h, roi_width, roi_width, roi_x, roi_y);
		//AddWaterMask(pdst, pdst, roiImg, img_w, img_h, roi_width, roi_width, roi_x, roi_y, pt, 1);
		//delete[]temp;
		if (roiImg != NULL)
			delete[]roiImg;
		return 1;
	}

	FNNOEXPORT int Rotate180(uint8_t* psrc, uint8_t* pdst, int w, int h)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		int imgSize = w * h;
		if (pdst != psrc)//不是原位操作
		{
			for (int i = 0; i < imgSize; ++i)
				memcpy(pdst + imgSize - i, psrc + i, sizeof(uint8_t));
		}
		else//是原位操作
		{
			int low = 0;
			int high = imgSize - 1;
			uint8_t temp;
			while (low < high)
			{
				temp = psrc[low];
				psrc[low] = pdst[high];
				pdst[high] = temp;
				low++;
				high--;
			}
		}
		return 1;
	}
	//绕（0,0）逆时针旋转90度
	/*
		  0   -1   h
	mat=  1    0   0
		  0    0   1

	*/
	//绕(0,0) 顺时针旋转90度
	/*
		  0    1   0
	mat=  -1   0   w
		  0    0   1
	*/

	FNNOEXPORT int CalRotateMat_3X3(bool IsRotate90_InCW, TranInfo* input, TranInfo* output)
	{
		//如果顺时针旋转90度
		int m[3][3] = { 0 };
		if (IsRotate90_InCW == false)
		{
			m[0][0] = 0;
			m[0][1] = -1;
			m[0][2] = input->h;
			m[1][0] = 1;
			m[1][1] = 0;
			m[1][2] = 0;
			m[2][0] = 0;
			m[2][1] = 0;
			m[2][2] = 1;
		}
		//如果逆时针旋转90度
		else
		{
			m[0][0] = 0;
			m[0][1] = 1;
			m[0][2] = 0;
			m[1][0] = -1;
			m[1][1] = 0;
			m[1][2] = input->w;
			m[2][0] = 0;
			m[2][1] = 0;
			m[2][2] = 1;
		}

		int nv00 = m[0][0] * input->v00 + m[0][1] * input->v10 + m[0][2] * input->v20;
		int nv01 = m[0][0] * input->v01 + m[0][1] * input->v11 + m[0][2] * input->v21;
		int nv02 = m[0][0] * input->v02 + m[0][1] * input->v12 + m[0][2] * input->v22;

		int nv10 = m[1][0] * input->v00 + m[1][1] * input->v10 + m[1][2] * input->v20;
		int nv11 = m[1][0] * input->v01 + m[1][1] * input->v11 + m[1][2] * input->v21;
		int nv12 = m[1][0] * input->v02 + m[1][1] * input->v12 + m[1][2] * input->v22;

		int nv20 = m[2][0] * input->v00 + m[2][1] * input->v10 + m[2][2] * input->v20;
		int nv21 = m[2][0] * input->v01 + m[2][1] * input->v11 + m[2][2] * input->v21;
		int nv22 = m[2][0] * input->v02 + m[2][1] * input->v12 + m[2][2] * input->v22;

		output->v00 = nv00;
		output->v01 = nv01;
		output->v02 = nv02;

		output->v10 = nv10;
		output->v11 = nv11;
		output->v12 = nv12;

		output->v20 = nv20;
		output->v21 = nv21;
		output->v22 = nv22;

		int new_width = m[0][0] * (int)input->w + m[0][1] * (int)input->h;
		int new_height = m[1][0] * (int)input->w + m[1][1] * (int)input->h;

		output->w = abs(new_width);
		output->h = abs(new_height);

		return 1;
	}


	//X方向上的镜像矩阵
	/*
		-1   0   w
	mat= 0   1   0
		 0   0   1
	*/
	//y方向上的镜像矩阵
	/*
		 1 0  0
	mat= 0 -1 h
		 0 0  1
	*/

	FNNOEXPORT int CalMirrorMat_3X3(bool Is_X_Mirror, TranInfo* input, TranInfo* output)
	{
		int m[3][3] = { 0 };
		if (Is_X_Mirror == true)
		{
			m[0][0] = -1;
			m[0][1] = 0;
			m[0][2] = input->w;
			m[1][0] = 0;
			m[1][1] = 1;
			m[1][2] = 0;
			m[2][0] = 0;
			m[2][1] = 0;
			m[2][2] = 1;
		}
		//如果逆时针旋转90度
		else
		{
			m[0][0] = 1;
			m[0][1] = 0;
			m[0][2] = 0;
			m[1][0] = 0;
			m[1][1] = -1;
			m[1][2] = input->h;
			m[2][0] = 0;
			m[2][1] = 0;
			m[2][2] = 1;
		}

		int nv00 = m[0][0] * input->v00 + m[0][1] * input->v10 + m[0][2] * input->v20;
		int nv01 = m[0][0] * input->v01 + m[0][1] * input->v11 + m[0][2] * input->v21;
		int nv02 = m[0][0] * input->v02 + m[0][1] * input->v12 + m[0][2] * input->v22;

		int nv10 = m[1][0] * input->v00 + m[1][1] * input->v10 + m[1][2] * input->v20;
		int nv11 = m[1][0] * input->v01 + m[1][1] * input->v11 + m[1][2] * input->v21;
		int nv12 = m[1][0] * input->v02 + m[1][1] * input->v12 + m[1][2] * input->v22;

		int nv20 = m[2][0] * input->v00 + m[2][1] * input->v10 + m[2][2] * input->v20;
		int nv21 = m[2][0] * input->v01 + m[2][1] * input->v11 + m[2][2] * input->v21;
		int nv22 = m[2][0] * input->v02 + m[2][1] * input->v12 + m[2][2] * input->v22;

		output->v00 = nv00;
		output->v01 = nv01;
		output->v02 = nv02;

		output->v10 = nv10;
		output->v11 = nv11;
		output->v12 = nv12;

		output->v20 = nv20;
		output->v21 = nv21;
		output->v22 = nv22;

		int new_width = m[0][0] * (int)input->w + m[0][1] * (int)input->h;
		int new_height = m[1][0] * (int)input->w + m[1][1] * (int)input->h;

		output->w = abs(new_width);
		output->h = abs(new_height);

		return 1;
	}

	FNNOEXPORT int MatTransfor(void* psrc, unsigned int old_w, unsigned int old_h, void* pdst, unsigned int& new_w, unsigned int& new_h, PixelForm pt, TranInfo& tranMat)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		uint8_t* psrc8 = (uint8_t*)psrc;
		int PPByteSize = GetPixelByteSize(pt);
		bool isSituOperation = (psrc == pdst) ? true : false;
		uint8_t* pdst8 = (psrc == pdst) ? new uint8_t[old_w * old_h * PPByteSize] : (uint8_t*)pdst;

		int new_width = tranMat.v00 * (int)old_w + tranMat.v01 * (int)old_h;
		int new_height = tranMat.v10 * (int)old_w + tranMat.v11 * (int)old_h;
		new_w = abs(new_width);
		new_h = abs(new_height);
		int imgSize = old_w * old_h;
		for (int y = 0; y < (int)old_h; ++y)
		{
			for (int x = 0; x < (int)old_w; ++x)
			{
				int nx = tranMat.v00 * (int)x + tranMat.v01 * (int)y + tranMat.v02 * 1;
				int ny = tranMat.v10 * (int)x + tranMat.v11 * (int)y + tranMat.v12 * 1;
				int ind1 = (y * old_w + x) * PPByteSize;
				int ind2 = (ny * new_w + nx) * PPByteSize;
				if (ind2 >= 0 && ind2 < imgSize)
					memcpy(pdst8 + ind2, psrc8 + ind1, PPByteSize);
			}
		}

		if (isSituOperation)
		{
			memcpy(psrc, pdst8, old_w * old_h * PPByteSize);
			delete[]pdst8;
		}
		return 1;
	}

	FNNOEXPORT int CalBlockGravity(int* pt_x, int* pt_y, uint64_t ptnum, double& gx, double& gy)
	{
		if (pt_x == nullptr || pt_y == nullptr)
			return -1;
		double sum_x = 0;
		double sum_y = 0;
		return 1;
	}
};

