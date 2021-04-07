#include "ImgProcess.h"
#include "param.h"
namespace IMat
{
	template<typename T>
	int  ImgSqu_NoTrunc_Begin(T* psrc, double* pdst, int w, int h)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		int imgSize = w * h;
		for (int i = 0; i < imgSize; ++i)
			pdst[i] = pow(psrc[i], 2);
		return 1;
	}

	//获得整幅图像的平均值
	template <typename T>
	double GetImageGlobalMean(T* psrc, int imgSize)
	{
		if (psrc == NULL) return 0.0;
		double sum = 0.0;
		for (int i = 0; i < imgSize; ++i)
			sum += psrc[i];
		return sum / (double)imgSize;
	}
	//获得整幅图像的标准差值
	template <typename T>
	double GetImageGlobalStd(T* psrc, int imgSize, double* globalMean = NULL)
	{
		if (psrc == NULL) return 0.0;
		double globalmeanval;
		globalmeanval = globalMean == NULL ? GetImageGlobalMean(psrc, imgSize) : *globalMean;

		double variance = 0.0;
		for (int i = 0; i < imgSize; ++i)
		{
			double val = psrc[i] - globalmeanval;
			val = val * val;
			variance += val;
		}
		variance = variance / imgSize;
		return sqrt(variance);
	}


	//支持原位操作
	int AdaGainEnhanceDouble(double* psrc, double* pdst, int w, int h, int r, float amount, float gainlimit)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		int imgSize = w * h;
		//求整幅图的平均值
		double globalMean = GetImageGlobalMean(psrc, imgSize);
		//对原图进行半径为r的均值卷积,获得局部均值
		double* localMeanMat = new double[imgSize];
		BoxBlur(psrc, localMeanMat, w, h, PixelForm::F64, r);
		//计算整幅图的每个像素的平方
		double* squ = new double[imgSize];
		ImgSqu_NoTrunc_Begin(psrc, squ, w, h);
		//对平方图进行半径为r的均值卷积，获得平方图的局部均值
		double* localMeanSquMat = new double[imgSize];;
		BoxBlur(squ, localMeanSquMat, w, h, PixelForm::F64, r);
		//均方差（标准差）等于 （平方的平均数-平均数的平方)然后开根号
		for (int i = 0; i < imgSize; ++i)
		{
			double mean = localMeanMat[i];
			double difSqu = sqrt(localMeanSquMat[i] - pow(mean, 2));
			if (difSqu > 0.0)
			{
				double lam = sqrt(localMeanSquMat[i] - pow(mean, 2));  //
				double cof = amount * (globalMean / lam);
				cof = cof < (double)gainlimit ? cof : (double)gainlimit;
				double res = mean + cof * ((double)psrc[i] - mean);
				pdst[i] = res;
			}
			else
				pdst[i] = mean;
		}
		if (squ)             delete[] squ;
		if (localMeanMat)    delete[] localMeanMat;
		if (localMeanSquMat) delete[] localMeanSquMat;
		return 1;
	}
	//自动色彩均衡算法 
	//out(x,y)=mean(x,y)+amount*(D/lam(x,y))*(in(x,y)-mean(x,y));  其中 mean(x,y)是再半径为r的局部均值，lam(x,y)为半径为r的局部标准差，D是常数，可以是整幅图的平均值或者是均方差值
	//amount为增益系数，提高对比度；gainlimit是增益amount*(D/lam(x,y))不能超过gainlimit;
	//支持原位操作
	int AdaGainEnhance(void* psrc, void* pdst, int w, int h, PixelForm pt, int r, float amount, float gainlimit)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		int ret = 1;
		double* temp = NULL;
		int imgSize = w * h;
		if (pt == PixelForm::U16)
		{
			temp = new double[imgSize];
			uint16_t* p = (uint16_t*)psrc;
			for (int i = 0; i < imgSize; ++i)
				temp[i] = (double)p[i];
			ret = AdaGainEnhanceDouble(temp, temp, w, h, r, amount, gainlimit);
			p = (uint16_t*)pdst;
			for (int i = 0; i < imgSize; ++i)
			{
				double res = temp[i];
				res = res <= 65535 ? res : 65535;
				res = res >= 0 ? res : 0;
				p[i] = (uint16_t)res;
			}
		}
		else if (pt == PixelForm::U8)
		{
			temp = new double[imgSize];
			uint8_t* p = (uint8_t*)psrc;
			for (int i = 0; i < imgSize; ++i)
				temp[i] = (double)p[i];
			ret = AdaGainEnhanceDouble(temp, temp, w, h, r, amount, gainlimit);
			p = (uint8_t*)pdst;
			for (int i = 0; i < imgSize; ++i)
			{
				double res = temp[i];
				res = res <= 255 ? res : 255;
				res = res >= 0 ? res : 0;
				p[i] = (uint8_t)res;
			}
		}
		else if (pt == PixelForm::S16)
		{
			temp = new double[imgSize];
			short* p = (short*)psrc;
			for (int i = 0; i < imgSize; ++i)
				temp[i] = (double)p[i];
			ret = AdaGainEnhanceDouble(temp, temp, w, h, r, amount, gainlimit);
			p = (short*)pdst;
			for (int i = 0; i < imgSize; ++i)
			{
				double res = temp[i];
				res = res <= SHRT_MAX ? res : SHRT_MAX;
				res = res >= SHRT_MIN ? res : SHRT_MIN;
				p[i] = (short)res;
			}
		}
		if (temp) delete[] temp;
		return 1;
	}
	//float型
	////支持原位操作
	int AdaGainEnhanceFloat(float* psrc, float* pdst, int w, int h, int r, float amount, float gainlimit)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		int imgSize = w * h;
		//求整幅图的平均值
		//float D1 = (float)GetImageGlobalMean(psrc, imgSize);
		float D = (float)GetImageGlobalStd(psrc, imgSize);
		//对原图进行半径为r的均值卷积,获得局部均值
		float* localMeanMat = new float[imgSize];
		FastBoxBlur(psrc, localMeanMat, w, h, r);
		//计算整幅图的每个像素的平方
		float* squ = new float[imgSize];
		for (int i = 0; i < imgSize; ++i)
		{
			float val = psrc[i];
			squ[i] = val * val;
		}
		//对平方图进行半径为r的均值卷积，获得平方图的局部均值
		float* localMeanSquMat = new float[imgSize];
		FastBoxBlur(squ, localMeanSquMat, w, h, r);

		//均方差（标准差）等于 （平方的平均数-平均数的平方)然后开根号
		for (int i = 0; i < imgSize; ++i)
		{
			float mean = localMeanMat[i];
			float squdif = localMeanSquMat[i] - pow(mean, 2);   //squdif是方差，理论上必定大于0，但是float型不精确，有时候会减少到小于0，lam变成无穷大，会变成噪声点
			if (squdif > 0.0f)
			{
				float cof = D / sqrt(squdif);
				cof = amount * (cof < gainlimit ? cof : gainlimit);
				float res = mean + cof * (psrc[i] - mean);
				pdst[i] = res;
			}
			else
				pdst[i] = mean;
		}
		if (squ) delete[] squ;
		if (localMeanMat) delete[] localMeanMat;
		if (localMeanSquMat) delete[]localMeanSquMat;
		return 1;
	}
	//自动色彩均衡算法 
	//out(x,y)=mean(x,y)+amount*(D/lam(x,y))*(in(x,y)-mean(x,y));  其中 mean(x,y)是再半径为r的局部均值，lam(x,y)为半径为r的局部标准差，D是常数，可以是整幅图的平均值或者是均方差值
	//amount为增益系数，提高对比度；gainlimit是增益amount*(D/lam(x,y))不能超过gainlimit;
	//支持原位操作
	int AdaGainEnhanceQuick(void* psrc, void* pdst, int w, int h, PixelForm pt, int r, float amount, float gainlimit)
	{
		if (psrc == NULL) return -1;
		if (pdst == NULL) return -2;
		int ret = 1;
		float* temp = NULL;
		int imgSize = w * h;
		float ker_size = (float)((2 * r + 1) * (2 * r + 1));
		float cof = 1.0f / ((2.0f * r + 1.0f) * (2.0f * r + 1.0f));
		if (pt == PixelForm::U16)
		{
			temp = new float[imgSize];
			uint16_t* p = (uint16_t*)psrc;
			for (int i = 0; i < imgSize; ++i)
				temp[i] = p[i] * cof;
			ret = AdaGainEnhanceFloat(temp, temp, w, h, r, amount, gainlimit);
			p = (uint16_t*)pdst;
			for (int i = 0; i < imgSize; ++i)
			{
				float res = temp[i] * ker_size;
				res = res <= 65535 ? res : 65535;
				res = res >= 0 ? res : 0;
				p[i] = (uint16_t)res;
			}
		}
		else if (pt == PixelForm::U8)
		{
			temp = new float[imgSize];
			uint8_t* p = (uint8_t*)psrc;
			for (int i = 0; i < imgSize; ++i)
				temp[i] = (float)p[i] * cof;
			ret = AdaGainEnhanceFloat(temp, temp, w, h, r, amount, gainlimit);
			p = (uint8_t*)pdst;
			for (int i = 0; i < imgSize; ++i)
			{
				float res = temp[i] * ker_size;
				res = res <= 255 ? res : 255;
				res = res >= 0 ? res : 0;
				p[i] = (uint8_t)res;
			}
		}
		else if (pt == PixelForm::S16)
		{
			temp = new float[imgSize];
			short* p = (short*)psrc;
			for (int i = 0; i < imgSize; ++i)
				temp[i] = (float)p[i] * cof;
			ret = AdaGainEnhanceFloat(temp, temp, w, h, r, amount, gainlimit);
			p = (short*)pdst;
			for (int i = 0; i < imgSize; ++i)
			{
				float res = temp[i] * ker_size;
				res = res <= SHRT_MAX ? res : SHRT_MAX;
				res = res >= SHRT_MIN ? res : SHRT_MIN;
				p[i] = (short)res;
			}
		}
		else if (pt == PixelForm::F32)
		{
			temp = new float[imgSize];
			float* p = (float*)psrc;
			for (int i = 0; i < imgSize; ++i)
				temp[i] = p[i] * cof;
			ret = AdaGainEnhanceFloat(temp, temp, w, h, r, amount, gainlimit);
			p = (float*)pdst;
			for (int i = 0; i < imgSize; ++i)
				p[i] = temp[i] * ker_size;
		}
		if (temp != NULL)
			delete[] temp;
		return 1;
	}

	extern "C" FNNOEXPORT int Ace_Gamma(void* psrc, void* pdst, int w, int h, PixelForm pt, struAceGammaParam & m, const char* iniPath)
	{
		if (psrc == nullptr) return -1;
		if (pdst == nullptr) return -2;
		int bytesize = 8;
		if (pt == PixelForm::U16 || pt == PixelForm::S16)
			bytesize = 2;
		else if (pt == PixelForm::U8)
			bytesize = 1;
		else if (pt == PixelForm::F32)
			bytesize = 4;
		bool isSituOperation = false;
		uint8_t* p_backup = nullptr;
		if (psrc == pdst)  //如果原位操作，要重新开辟一块内存
		{
			p_backup = new uint8_t[(uint64_t)w * h * bytesize];
			pdst = p_backup;
			isSituOperation = true;
		}
		if (strlen(iniPath) > 0)
			m = struAceGammaParam(iniPath);
		//先进行ACE处理
		int   ace_r = m.ace_r;
		float ace_amount = m.ace_amount;
		float ace_gainlimit = m.ace_gainlimit;
		int algorithm_type = m.algorithm_type;
		if (algorithm_type == 0)
			AdaGainEnhanceQuick(psrc, pdst, w, h, pt, ace_r, ace_amount, ace_gainlimit);
		else
			AdaGainEnhance(psrc, pdst, w, h, pt, ace_r, ace_amount, ace_gainlimit);
		//进行gamma变换操作
		float gamma_coef = m.gamma_cof;
		if (gamma_coef != 1.0f)
			GammaCorrect(pdst, pdst, w, h, gamma_coef, pt);
		if (m.isNeedEquHists == 1 && (pt == PixelForm::U16 || pt == PixelForm::U8))
			EquHist(pdst, pdst, w * h, pt);
		if (isSituOperation == true&& pdst!=nullptr)
		{
			memcpy(psrc, pdst, (uint64_t)bytesize * w * h);
			delete[] p_backup;
			pdst = psrc;
		}
		return 1;
	}

};