#include "ImgProcess.h"
#include "param.h"
//USM Gaussian ker
namespace IMat
{
	int GetGaussKer(float* ker, int r, float sigma, bool isNorm)
	{
		if (ker == NULL) return -1;
		if (r <= 0)      return -2;
		int h = r * 2 + 1;
		int w = h;
		float cof = -1.0f / (2.0f * sigma * sigma);
		for (int y = 0; y < h; ++y)
		{
			for (int x = 0; x < w; ++x)
			{
				int index = y * w + x;
				ker[index] = (float)exp((pow(x - r, 2) + pow(y - r, 2)) * cof);
			}
		}
		//将核压缩到1
		if (isNorm == true)
		{
			int ker_size = w * h;
			float sum = 0.0f;
			for (int i = 0; i < ker_size; ++i)
				sum += ker[i];
			float ratio = 1.0f / sum;
			for (int i = 0; i < ker_size; ++i)
				ker[i] = ker[i] * ratio;
		}
		return 1;
	}


	extern "C" FNNOEXPORT int USM_Gaussian(uint16_t * psrc, uint16_t * pdst, int img_w, int img_h, int r, float lambda, int thresh, float sigma, bool* mask)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		if (img_w <= 0 || img_h <= 0 || r <= 0) return -4;
		int img_size = img_w * img_h;
		int k_w = 2 * r + 1;
		int k_h = 2 * r + 1;
		int ker_size = k_w * k_h;
		float* psrcf = new float[img_size];
		float* pdstf = new float[img_size];
		float* kerf = new float[ker_size];
		GetGaussKer(kerf, r, sigma, false);
		for (int i = 0; i < img_size; ++i)
			psrcf[i] = (float)psrc[i];
		int ret;
		if (mask != NULL)
		{
			ret = ImageConv(psrcf, pdstf, kerf, img_w, img_h, k_w, k_h);
			for (int i = 0; i < img_size; ++i)
			{
				float dif = abs(psrcf[i] - pdstf[i]);
				mask[i] = (dif >= thresh) ? true : false;
			}
		}
		float sumofker = 0.0f;//高斯核必然为1，因为之前已经归一化过了
		for (int i = 0; i < ker_size; ++i)
		{
			sumofker += kerf[i];
			kerf[i] = 1.0f * kerf[i] * lambda;
		}
		float cen_cof = sumofker * lambda + 1.0f;
		ret = ImageConv(psrcf, pdstf, kerf, img_w, img_h, k_w, k_h, mask);
		for (int i = 0; i < img_size; ++i)
		{
			float val = -pdstf[i] + psrcf[i] * cen_cof;
			val = (val < 65535 ? val : 65535);
			val = val >= 0 ? val : 0;
			pdst[i] = (uint16_t)val;
		}
		delete[]psrcf;
		delete[]pdstf;
		delete[]kerf;
		return ret;
	}

	//USM算法，支持16位,
	extern "C" FNNOEXPORT int USM_BOX(uint16_t * psrc, uint16_t * pdst, int w, int h, int r, float lam, bool isNeedNorm)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		float* psrcf = new float[(uint64_t)w * h];
		int imgSize = w * h;
		uint16_t* psrc16 = (uint16_t*)psrc;
		for (int i = 0; i < imgSize; ++i)
			psrcf[i] = (float)psrc16[i];
		float* temp = new float[(uint64_t)w * h];
		memset(temp, 0, sizeof(float) * imgSize);
		float* padd = NULL;
		copyBorder(psrcf, padd, h, w, 0, 0, r, r);
		int paddw = w + 2 * r;
		int paddh = h;
		//先横向滤波
		for (int y = 0; y < h; ++y)
		{
			float sum = 0.0f;
			float* temprow = temp + (uint64_t)y * (uint64_t)w;
			float* paddrow = padd + (uint64_t)y * paddw + r;
			for (int xx = -r; xx <= r; ++xx)
				sum += paddrow[xx];
			temprow[0] = sum;
			for (int x = 1; x < w; ++x)
			{
				sum += (paddrow[x + r] - paddrow[x - r - 1]);
				temprow[x] = sum;
			}
		}
		//再纵向滤波，尺寸未变，直接把padd放进去即可
		delete[]padd;
		padd = NULL;
		copyBorder(temp, padd, h, w, r, r, 0, 0);
		paddw = w;
		paddh = h + 2 * r;
		for (int x = 0; x < w; ++x)
		{
			float sum = 0.0f;
			float* tempcol = temp + x;
			float* paddcol = padd + x + (uint64_t)paddw * r;
			for (int yy = -r; yy <= r; ++yy)
				sum += (paddcol[yy * paddw]);
			tempcol[0] = sum;
			for (int y = 1; y < h; ++y)
			{
				sum += (paddcol[(y + r) * paddw] - paddcol[(y - r - 1) * paddw]);
				tempcol[y * w] = sum;
			}
		}
		float cof_psrcf = (2 * r + 1) * (2 * r + 1) * lam + 1;
		float cof_temp = lam;
		for (int i = 0; i < imgSize; ++i)
			temp[i] = cof_psrcf * psrcf[i] + cof_temp * (-temp[i]);
		//至此基于均值滤波的USM以及完成
		uint16_t* pdst16 = (uint16_t*)pdst;
		if (isNeedNorm == 1)
		{
			normalize_32F(temp, psrcf, w, h, 0, 65535.0f);
			for (int i = 0; i < imgSize; ++i)
				pdst16[i] = (uint16_t)SafeVal(psrcf[i], 0.0f, 65535.0f);
		}
		else
		{
			for (int i = 0; i < imgSize; ++i)
				pdst16[i] = (uint16_t)SafeVal(temp[i], 0.0f, 65535.0f);
		}
		delete[]temp;
		delete[]padd;
		delete[]psrcf;
		return 1;
	}
	//USM算法，支持16位,
	extern "C" FNNOEXPORT int USM_EX(void* psrc, void* pdst, int w, int h, PixelForm type, struUSMParam & m, const char* filePath)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		if (type != PixelForm::U16) return -3;
		if (strlen(filePath))
			m = struUSMParam(filePath);
		return USM_BOX((uint16_t*)psrc, (uint16_t*)pdst, w, h, m.r, m.lam, m.isNeedNorm);
	}
};