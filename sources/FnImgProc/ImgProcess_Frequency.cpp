#include "ImgProcess_Frequency.h"
#include "ImgProcess.h"
namespace IMat
{
	/*
			  0
	F(u)=1/N sum  f(x)*exp(-j*2*Pi*u*x/N)   N是指一维数据的长度，x是只在一维数据上的坐标
			 N-1

	exp(i*x)=cos(x)+i*sin(x)
	*/
#define PI 3.1415926535
	int DoDft1D(float* psrc, float* preal, float* pimag, int N)
	{
		if (psrc == NULL) return -1;
		if (preal == NULL) return -2;
		if (pimag == NULL) return -3;

		for (int u = 0; u < N; ++u)
		{
			float real = 0.0f;
			float imag = 0.0f;
			for (int x = 0; x < N; ++x)
			{
				float val = psrc[x];
				float cof = (float)(2.0 * PI * u * x / (float)N);
				real += val * cos(cof);
				imag += val * sin(cof);
			}
			preal[u] = real / N;
			pimag[u] = imag / N;
		}
		return 1;
	}

	/*
					0   0
	F(u)=1/(M*N)   sum sum  f(x)*exp[-2*j*Pi*(u*x/M+v*x/N)]   M和N是是图像的长和宽
				   M-1 N-1
	*/

	//DFT处理
	int DoDft2D(float* psrc, float* preal, float* pimag, int w, int h)
	{
		if (psrc == NULL) return -1;
		if (preal == NULL) return -2;
		if (pimag == NULL) return -3;
		int M = w;
		int N = h;
		for (int v = 0; v < N; ++v)
		{
			for (int u = 0; u < M; ++u)
			{
				float real = 0.0f;
				float imag = 0.0f;
				for (int y = 0; y < h; ++y)
				{
					for (int x = 0; x < w; ++x)
					{
						float val = psrc[y * w + h];
						float cof =(float) (2.0 * PI * (u * x / (float)M + v * y / (float)N));
						real += val * cos(cof);
						imag += val * sin(cof);
					}
				}
				preal[v * w + u] = real / (M * N);
				pimag[v * w + u] = imag / (M * N);
			}
		}
		return 1;
	}
	//
	extern "C" FNNOEXPORT int Dft1D(float* psrc, float* real, float* imag, int w)
	{
		if (psrc == NULL) return -1;
		if (real == NULL) return -2;
		if (imag == NULL) return -3;
		return DoDft1D(psrc, real, imag, w);
	}

	extern "C" FNNOEXPORT int Dft2D(float* psrc, float* real, float* imag, int w, int h)
	{
		if (psrc == NULL) return -1;
		if (real == NULL) return -2;
		if (imag == NULL) return -3;
		return DoDft2D(psrc, real, imag, w, h);
	}
	/*
				 N-1
	f(x)=1/N sum F(u)*exp(2*Pi*x*u/N);
				  0
	*/
	extern "C" FNNOEXPORT int Idft1D(float* psrc, float* preal, float* pimag, int N)
	{
		if (psrc == NULL) return -1;
		if (preal == NULL) return -2;
		if (pimag == NULL) return -3;

		for (int x = 0; x < N; ++x)
		{
			float p1 = 0.0f;
			float p2 = 0.0f;
			for (int u = 0; u < N; ++u)
			{
				float v1 = preal[u];
				float v2 = pimag[u];
				float coe =(float) (2.0 * PI * x * u / N);
				float v3 = cos(coe);
				float v4 = sin(coe);
				p1 = v1 * v3 - v2 * v4;
				p2 = v2 * v3 + v1 * v4;
			}
			psrc[x] = p1 / N;
		}
		return 1;
	}

	/*
				   N-1
	f(x,y)=1/(M*N) sum F(u,v)*exp(2*Pi*(u*x/M+v*y/N));
					0
	*/
	extern "C" FNNOEXPORT int Idft2D(float* psrc, float* preal, float* pimag, int w, int h)
	{
		if (psrc == NULL) return -1;
		if (preal == NULL) return -2;
		if (pimag == NULL) return -3;
		int M = w;
		int N = h;
		for (int y = 0; y < N; ++y)
		{
			for (int x = 0; x < M; ++x)
			{
				float p1 = 0.0f;
				float p2 = 0.0f;
				for (int v = 0; v < N; ++v)
				{
					for (int u = 0; u < M; ++u)
					{
						float v1 = preal[v * N + u];
						float v2 = pimag[v * N + u];
						float coe =(float)exp(2.0 * PI * (u * x / (float)M + v * y / (float)N));
						float v3 = cos(coe);
						float v4 = sin(coe);
						p1 = v1 * v3 - v2 * v4;
						p2 = v2 * v3 + v1 * v4;
					}
				}
				psrc[y * N + x] = p1 / (M * N);
			}
		}
		return 1;
	}
};