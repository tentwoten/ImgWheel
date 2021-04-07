// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 FINNOIMGPROCESS_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// IMGPROCESS_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。

#ifndef  _IMGPROCESS_H_
#define _IMGPROCESS_H_
#include "param.h"
#include <string>
#include "FnCore/FnPoint.h"
#include "FnCore/ImgMat.hpp"
#include "FnCore/ImgMatHelper.h"
#include "FnCore/ImgMatAux.hpp"


namespace IMat
{
	//64位double型
	extern "C" FNNOEXPORT int clahe_64F(double* pSrc, double* pDst, int imgWidth, int imgHeight, claheParam & m, const char* filePath = "");
	//32位float型
	extern "C" FNNOEXPORT int clahe_32F(float* pSrc, float* pDst, int imgWidth, int imgHeight, claheParam & m, const char* filePath = "");
	//16位uint16_t型
	extern "C" FNNOEXPORT int clahe_16U(uint16_t * pSrc, uint16_t * pDst, int imgWidth, int imgHeight, claheParam & m, const char* filePath = "");
	//8位uint8_t型
	extern "C" FNNOEXPORT int clahe_8U(uint8_t * pSrc, uint8_t * pDst, int imgWidth, int imgHeight, claheParam & m, const char* filePath = "");
	//64位double型  手动赋值
	extern "C" FNNOEXPORT int clahe_64F_Manual(double* pSrc, double* pDst, int imgWidth, int imgHeight, int widthBlockNum, int heightBlockNum, int histBin, int peak, const char* filePath = "");
	//32位float型   手动赋值
	extern "C" FNNOEXPORT int clahe_32F_Manual(float* pSrc, float* pDst, int imgWidth, int imgHeight, int widthBlockNum, int heightBlockNum, int histBin, int peak, const char* filePath = "");
	//16位uint16_t型  手动赋值
	extern "C" FNNOEXPORT int clahe_16U_Manual(uint16_t * pSrc, uint16_t * pDst, int imgWidth, int imgHeight, int widthBlockNum, int heightBlockNum, int histBin, int peak, const char* filePath = "");
	//8位uint8_t型  手动赋值
	extern "C" FNNOEXPORT int clahe_8U_Manual(uint8_t * pSrc, uint8_t * pDst, int imgWidth, int imgHeight, int widthBlockNum, int heightBlockNum, int histBin, int peak, const char* filePath = "");

	extern "C" FNNOEXPORT int edgeClip_32F(float* pSrc, float* pDst, int img_h, int img_w, clipParam m, std::string filePath = "");

	extern "C" FNNOEXPORT int edgeClip_16U(uint16_t * pSrc, uint16_t * pDst, int img_h, int img_w, clipParam m, std::string filePath = "");

	extern "C" FNNOEXPORT int edgeClip_8U(uint8_t * pSrc, uint8_t * pDst, int img_h, int img_w, clipParam m, std::string filePath = "");

	extern "C" FNNOEXPORT int normalize_16U(uint16_t * pSrc, uint16_t * pDst, int w, int h, uint16_t minVal_normalized, uint16_t maxVal_normalized);

	extern "C" FNNOEXPORT int normalize_8U(uint8_t * pSrc, uint8_t * pDst, int w, int h, uint8_t minVal_normalized, uint8_t maxVal_normalized);

	extern "C" FNNOEXPORT int normalize_32F(float* pSrc, float* pDst, int w, int h, float minVal_normalized, float maxVal_normalized);

	extern "C" FNNOEXPORT int normalize_64F(double* pSrc, double* pDst, int w, int h, double minVal_normalized, double maxVal_normalized);//获得高斯核
	//gamma变换
	extern "C" FNNOEXPORT int GammaCorrect(void* psrc, void* pdst, int w, int h, float lam, PixelForm Pixel_Type);
	//后处理，ACE算法配合gamma亮度校正
	extern "C" FNNOEXPORT int Ace_Gamma(void* psrc, void* pdst, int w, int h, PixelForm Pixel_Type, struAceGammaParam & m, const char* iniPath = "");
	//直方图均衡化 可以原位操作,psrc和pdst不可为空
	extern "C" FNNOEXPORT int EquHist(void* psrc, void* pdst, int imgSize, PixelForm pt);
	//修复坏点 
	extern "C" FNNOEXPORT int FixBadPixel(float* psrc, float* pdst, float* mask, int w, int h);
	//窗宽_窗位，再进行normalize ,可以进行原位操作
	extern "C" FNNOEXPORT int WLN(void* psrc, void* pdst, unsigned int pixelcount, PixelForm pt, struWLNParam & m, const char* filePath = NULL);
	//多尺度框宽窗位调节，可以进行原位操作
	extern "C" FNNOEXPORT int MulScaledWLN(void* psrc, void* pdst, unsigned int pixelcount, PixelForm pt, int scalednum, struWLNParam & m, const char* filePath = NULL);
	//FastNLM算法，可进行原位操作
	extern "C" FNNOEXPORT int FastNLM(uint8_t * psrc, uint8_t * pdst, int img_w, int img_h, float h, int rd, int rD);
	//FastNLM16算法，可进行原位操作,支持16位图
	extern "C" FNNOEXPORT int FastNLM16(uint16_t * psrc, uint16_t * pdst, int img_w, int img_h, float h, int rd, int rD);
	//最原始的卷积
	extern "C" FNNOEXPORT int ImageConv(float* psrc, float* pdst, float* kernel, int img_w, int img_h, int k_w, int k_h, bool* mask = NULL);
	//USM 只支持16位，有需要再添加，支持原位操作
	extern "C" FNNOEXPORT int USM_BOX(uint16_t * psrc, uint16_t * pdst, int w, int h, int r, float lam, bool isNeedNorm = false);
	//USM算法，只支持16位，有需要再添加，支持原位操作
	extern "C" FNNOEXPORT int USM_EX(void* psrc, void* pdst, int w, int h, PixelForm type, struUSMParam & m, const char* filePath = "");
	//USM 平滑的时候使用高斯滤波
	extern "C" FNNOEXPORT int USM_Gaussian(uint16_t * psrc, uint16_t * pdst, int img_w, int img_h, int r, float lambda = 0.5f, int thresh = 0, float sigma = 1.0f, bool* mask = NULL);
	//调节对比度和亮度，只支持16位，有需要再添加，支持原为操作。
	extern "C" FNNOEXPORT int BrightAndContrast(void* psrc, void* pdst, int w, int h, PixelForm type, float brightness = 0, float contrast = 1);
	//亮度，对比度调节
	extern "C" FNNOEXPORT int BrightAndContrastEx(uint16_t * psrc, uint16_t * pdst, int w, int h, int low, int high);
	//反色,只支持16位，需要再添加，支持原位操作
	extern "C" FNNOEXPORT int Inverse(void* psrc, void* pdst, int w, int h, PixelForm type);
	//中值滤波 ,支持原位操作
	extern "C" FNNOEXPORT int Median_Blur(void* psrc, void* pdst, int w, int h, int r, PixelForm pixelType);
	//图像修复,只支持16位，可以原位操作,掩膜必须位32位float型
	extern "C" FNNOEXPORT int ImgFix(void* psrc, void* pdst, float* mask, int w, int h, int r, PixelForm pixelType, int iternum = 200);
	//图像增益校准,支持16位uint16_t，32位Float，支持原位操作
	extern "C" FNNOEXPORT int ImgGainCorrect(float* psrc, float* pdst, int w, int h);
	//增加水印,只支持uint8_t,uint16_t和float型
	extern "C" FNNOEXPORT int AddWaterMask(void* psrc, void* pdst, void* mask, int src_w, int src_h, int mask_w, int mask_h, int begin_x, int begin_y, PixelForm pixelType, float weight = 0.5f);
	//获得大津法OSTU的值，仅适合16位图和8位图
	extern "C" FNNOEXPORT double GetOtsuThresh(void* psrc, uint32_t pixelcount, PixelForm pt);
	//普通二值化，
	//thresh_type=0; val>thresh?val:0;
	//thresh_type=1; val>thresh?0:val;
	//thresh_type=2; val>thresh?val:0;
	//thresh_type=3; val>thresh?0:val;
	extern "C" FNNOEXPORT int ThresholdBinary(void* psrc, void* pdst, PixelForm pt, int w, int h, double thresh, int thresh_type);
	//+log和-log变换
	extern "C" FNNOEXPORT int Image_Log(float* psrc, float* pdst, int w, int h, int isNegative = false);
	//将Roi拷贝到相应的位置中
	extern "C" FNNOEXPORT int CopyRoiTo(void* psrc, void* pdst, void* mask, PixelForm pt, int img_w, int img_h, int mask_w, int mask_h, int begin_x, int begin_y);
	//获取图像的直方图,仅限uint8_t,uint16_t,float;
	extern "C" FNNOEXPORT int GetHistBin(void* psrc, int img_w, int img_h, PixelForm pt, int* histBin, unsigned int histBinNum);
	//Resize
	extern "C" FNNOEXPORT int Resize2D(void* psrc, void* pdst, int src_w, int src_h, int dst_w, int dst_h, PixelForm pt);
	//自动阈值分割
	extern "C" FNNOEXPORT int AutoBC(void* psrc, void* pdst, int w, int h, PixelForm type, int& low, int& high, float PixelRemain_Ratio = 0.99f);
	//均值滤波
	extern "C" FNNOEXPORT int BoxBlur(void* psrc, void* pdst, int w, int h, PixelForm type, int r);
	//快速均值滤波,仅限float型
	extern "C" FNNOEXPORT int FastBoxBlur(float* psrc, float* pdst, int w, int h, int r);
	//快速近似高斯滤波 //论文Recursive implementation of the Gaussian filter
	extern "C" FNNOEXPORT int FastGaussFilter(void* psrc, void* pdst, int w, int h, PixelForm type, double sigma);
	//低照度增强
	extern "C" FNNOEXPORT int LowLightEnhance(void* psrc, void* pdst, int w, int h, PixelForm type, float chosenBrightRatio = 0.001f);
	//伪彩 可以原位操作
	extern "C" FNNOEXPORT int PseudoColor(IMat::Mat & src, IMat::Mat & dst_8U,ColorMapStyle style = ColorMapStyle::COMMON, int RGBOrder = 0);
	//导向滤波
	//extern "C" IMGPROCESS_API int GuidedFilter_D(void* psrc, void* pdst, void* guide, PixelForm pt, int w, int h, int r, double eps);
	extern "C" FNNOEXPORT int GuidedFilter(IMat::Mat & src, IMat::Mat & dst, IMat::Mat & guide, int r, double eps);
	//sobel算子，返回dst为double型
	extern "C" FNNOEXPORT int Sobel2D(IMat::Mat & src, IMat::Mat & dst, int r, bool is_dir_x);
	//边缘扩展
	extern "C" FNNOEXPORT int MatPad(IMat::Mat & src, IMat::Mat & dst, int up, int down, int left, int right);
	//二维卷积
	extern "C" FNNOEXPORT int Conv2D(IMat::Mat & src, IMat::Mat & dst, IMat::Mat & ker, IMat::FnPoint2i anchor = IMat::FnPoint2i(-1, -1), uint32_t stride = 1, PaddType pad = PaddType::SAME);
	//细化图像
	extern "C" FNNOEXPORT int ThinBinImg(IMat::Mat & src, IMat::Mat & dst, int iternum);
	//
	extern "C" FNNOEXPORT int Get4BoundDomain(vector<vector<IMat::FnPoint2i>> & domains, uint8_t * binImg, int w, int h);

	extern "C" FNNOEXPORT void FindCountour(vector<vector<IMat::FnPoint2i>> & countours, uint8_t * binImg, int w, int h, int method = 0);

};
#endif