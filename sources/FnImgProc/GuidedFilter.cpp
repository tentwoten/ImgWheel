#include "ImgProcess.h"
#include "FnCore/Log_xx.h"
//导向滤波
/*
q[i]=a[k]I[i]+b[k]   i属于wk
q[i]=p[i]-n[i]
p为输入图像，I为导向图，q为输出图像。在这里我们认为输出图像可以看成导向图I 的一个局部线性变换
其中k是局部化的窗口的中点，都可以用导向图对应的pixel通过（ak,bk）的系数进行变换计算出来
接下来就是解出这样的系数a和b，使得p和q的差别尽量小;
				  w
令E(a(k),b(k))=sum sum (( a[k]I[i]+b[k]-p[i])^2 + eps((a[k])^2))
				  k


matlab代码
function q = guidedfilter(I, p, r, eps)

%   - guidance image: I (should be a gray-scale/single channel image)
%   - filtering input image: p (should be a gray-scale/single channel image)
%   - local window radius: r
%   - regularization parameter: eps

[hei, wid] = size(I);
N = boxfilter(ones(hei, wid), r);

mean_I = boxfilter(I, r) ./ N;
mean_p = boxfilter(p, r) ./ N;
mean_Ip = boxfilter(I.*p, r) ./ N;
% this is the covariance of (I, p) in each local patch.
cov_Ip = mean_Ip - mean_I .* mean_p;

mean_II = boxfilter(I.*I, r) ./ N;
var_I = mean_II - mean_I .* mean_I;

a = cov_Ip ./ (var_I + eps);
b = mean_p - a .* mean_I;

mean_a = boxfilter(a, r) ./ N;
mean_b = boxfilter(b, r) ./ N;

q = mean_a .* I + mean_b;
end

*/
namespace IMat
{
#define V1_0
#ifdef V1_0
	FNNOEXPORT int GuidedFilter_D(void* psrc, void* pdst, void* guide, PixelForm pt, int w, int h, int r, double eps)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		if (guide == NULL) guide = psrc;
		if (pt == PixelForm::U16)
		{
			uint32_t pixelcount = w * h;
			uint16_t* psrc16 = (uint16_t*)psrc;
			uint16_t* pdst16 = (uint16_t*)pdst;
			uint16_t* guide16 = (uint16_t*)guide;

			double* p = new double[pixelcount];
			double* I = new double[pixelcount];
			double* IP = new double[pixelcount];
			for (uint32_t i = 0; i < pixelcount; ++i)
			{
				p[i] = (double)psrc16[i] / 65535.0;
				I[i] = (double)guide16[i] / 65535.0;
			}
			double* mean_p = new double[pixelcount];
			double* mean_i = new double[pixelcount];
			double* mean_ip = IP;
			//step1
			BoxBlur(p, mean_p, w, h, PixelForm::F64, r);
			//step2
			BoxBlur(I, mean_i, w, h, PixelForm::F64, r);
			//step3
			for (uint32_t i = 0; i < pixelcount; ++i)
				IP[i] = I[i] * p[i];
			BoxBlur(IP, mean_ip, w, h, PixelForm::F64, r);
			//step4
			double* cov_Ip = mean_ip;
			for (uint32_t i = 0; i < pixelcount; ++i)
				cov_Ip[i] = mean_ip[i] - mean_i[i] * mean_p[i];
			//step5
			double* II = new double[pixelcount];
			for (uint32_t i = 0; i < pixelcount; ++i)
				II[i] = I[i] * I[i];
			double* mean_II = II;
			BoxBlur(II, mean_II, w, h, PixelForm::F64, r);
			//step6
			double* var_I = mean_II;
			for (uint32_t i = 0; i < pixelcount; ++i)
				var_I[i] = mean_II[i] - mean_i[i] * mean_i[i];
			//step7
			double* a = cov_Ip;
			for (uint32_t i = 0; i < pixelcount; ++i)
				a[i] = cov_Ip[i] / (var_I[i] + eps);
			double* b = new double[pixelcount];
			for (uint32_t i = 0; i < pixelcount; ++i)
				b[i] = mean_p[i] - a[i] * mean_i[i];
			BoxBlur(a, a, w, h, PixelForm::F64, r);
			BoxBlur(b, b, w, h, PixelForm::F64, r);
			double* mean_a = a, * mean_b = b;
			for (uint32_t i = 0; i < pixelcount; ++i)
			{
				double q = (mean_a[i] * I[i] + mean_b[i]) * 65535.0 + 0.5;
				q = min(max(q, 0.0), 65535.0);
				pdst16[i] = (uint16_t)q;
			}
			delete[]p;
			delete[]I;
			delete[]II;
			delete[]IP;
			delete[]b;
			delete[]mean_p;
			delete[]mean_i;
		}
		return 1;
	}


#endif
	template<typename T>
	int DoGuidedFilter(double* p, T* q, double* I, int w, int h, int r, double eps)
	{
		if (p == NULL) return -1;
		if (q == NULL) return -2;
		if (I == NULL) return -3;
		uint32_t pixelcount = (uint32_t)w * (uint32_t)h;

		double* IP = new double[pixelcount];
		double* mean_p = new double[pixelcount];
		double* mean_i = new double[pixelcount];

		//step1
		BoxBlur(p, mean_p, w, h, PixelForm::F64, r);
		//step2
		BoxBlur(I, mean_i, w, h, PixelForm::F64, r);
		//step3
		for (uint32_t i = 0; i < pixelcount; ++i)
			IP[i] = I[i] * p[i];
		double* mean_ip = IP;
		BoxBlur(IP, mean_ip, w, h, PixelForm::F64, r);
		//step4
		double* cov_Ip = mean_ip;
		for (uint32_t i = 0; i < pixelcount; ++i)
			cov_Ip[i] = mean_ip[i] - mean_i[i] * mean_p[i];
		//step5
		double* II = new double[pixelcount];
		for (uint32_t i = 0; i < pixelcount; ++i)
			II[i] = I[i] * I[i];
		double* mean_II = II;
		BoxBlur(II, mean_II, w, h, PixelForm::F64, r);
		//step6
		double* var_I = mean_II;
		for (uint32_t i = 0; i < pixelcount; ++i)
			var_I[i] = mean_II[i] - mean_i[i] * mean_i[i];
		//step7
		double* a = cov_Ip;
		for (uint32_t i = 0; i < pixelcount; ++i)
			a[i] = cov_Ip[i] / (var_I[i] + eps);
		double* b = new double[pixelcount];
		for (uint32_t i = 0; i < pixelcount; ++i)
			b[i] = mean_p[i] - a[i] * mean_i[i];
		BoxBlur(a, a, w, h, PixelForm::F64, r);
		BoxBlur(b, b, w, h, PixelForm::F64, r);
		double* mean_a = a, * mean_b = b;

		double tor_min = 0, tor_max = 65535;
		bool isFloat = false;
		GetTorMinMax(q, tor_min, tor_max, isFloat);

		for (uint32_t i = 0; i < pixelcount; ++i)
		{
			double temp = (mean_a[i] * I[i] + mean_b[i]) * 65535.0 + 0.5;
			temp = min(max(temp, tor_min), tor_max);
			q[i] = (uint16_t)temp;
		}
		delete[]IP;
		delete[]mean_p;
		delete[]mean_i;
		delete[]II;
		delete[]b;
	}

#ifdef V1_1
	FNNOEXPORT int GuidedFilter(void* psrc, void* pdst, void* guide, Pixel_Type pt, int w, int h, int r, double eps)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		if (guide == NULL) guide = psrc;

		uint pixelcount = w * h;
		double* p = new double[pixelcount];
		double* I = new double[pixelcount];
		if (pt == Pixel_Type::Grayscale8)
		{
			uint8_t* psrc8 = (uint8_t*)psrc;
			uint8_t* guide8 = (uint8_t*)guide;
			for (uint i = 0; i < pixelcount; ++i)
			{
				p[i] = (double)psrc8[i] / 255.0;
				I[i] = (double)guide8[i] / 255.0;
			}
			DoGuidedFilter(p, (uint8_t*)pdst, I, w, h, r, eps);
		}
		else if (pt == Pixel_Type::Grayscale16)
		{
			uint16_t* psrc16 = (uint16_t*)psrc;
			uint16_t* guide16 = (uint16_t*)guide;
			for (uint i = 0; i < pixelcount; ++i)
			{
				p[i] = (double)psrc16[i] / 65535.0;
				I[i] = (double)guide16[i] / 65535.0;
			}
			DoGuidedFilter(p, (uint16_t*)pdst, I, w, h, r, eps);
		}
		else if (pt == Pixel_Type::Float32)
		{
			float* psrcf = (float*)psrc;
			float* guidef = (float*)pdst;
			for (uint i = 0; i < pixelcount; ++i)
			{
				p[i] = psrcf[i];
				I[i] = guidef[i];
			}
			DoGuidedFilter(p, (float*)pdst, I, w, h, r, eps);
		}
		delete[]p;
		delete[]I;
		return 1;
	}
#endif



	int GuidedFilter(Mat& src, Mat& dst, Mat& guide, int r, double eps)
	{
		bool isSituOperation = false;
		if (src.data == nullptr)
		{
			PrintInfo("GuidedFilter: src is nullptr");
			return -1;
		}
		else if (dst.data == nullptr||src.IsSameSize(dst)==false)
		{
			dst = src.Clone();
		}
		else if (dst.data == src.data)
		{
			dst = src.Clone();
			isSituOperation = true;
		}

		if (src.m_Format == PixelForm::U16)
		{
			GuidedFilter_D((uint16_t*)src.data, (uint16_t*)dst.data, (uint16_t*)guide.data, src.m_Format, src.cols, src.rows, r, eps);
		}
		else
		{
			PrintInfo("GuidedFilter: The format must be U16");
			return -3;
		}
		if (isSituOperation == true)
		{
			memcpy(src.data, dst.data, GetChannel(dst.m_Format) * dst.rows * dst.cols);
		}
		return 1;
	}

};