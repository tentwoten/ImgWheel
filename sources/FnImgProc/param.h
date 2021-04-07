#ifndef _Param_H_
#define _Param_H_
#include <vector>
#include "FnCore/FnGlobalEnum.h"
#include "FnCore/FnGlobalHead.h"
#include "iniFile.h"
#include <map>
#include <algorithm>
using namespace std;
#ifndef getIPtrAndName
#define getIPtrAndName(N) make_pair<int*,const char*>(&(this->N),const_cast<const char*>(#N))
#endif
#ifndef getFPtrAndName
#define getFPtrAndName(N) make_pair<float*,const char*>(&(this->N),const_cast<const char*>(#N))
#endif

struct FNNOEXPORT paramBase
{
public:
	paramBase();
	//paramBase &operator=(const paramBase& other);
	virtual ~paramBase();
	void clearDataToBeSaved();
protected:
	map<int*, const char*> *imap_ex;
	map<float*, const char*>*fmap_ex;

	vector<int*> *iPtr;
	vector<float*> *fPtr;
	vector<const char*> *iID;
	vector<const char*> *fID;

	void saveData(const char* app, const char* iniPath);
	void getData(const char* app, const char* iniPath);
	void saveData_byVec(const char* app, const char* iniPath);
	void getData_byVec(const char* app, const char* iniPath);
	int CopyFilePath(char*& pDst, const char* pSrc);
};

struct FNNOEXPORT claheParam :public paramBase
{
public:  //必须初始化的量
	int widthBlockNum;
	int heightBlockNum;
	int histBin;
	int peak;
public:
	float binRatio;    //比宁率，直方图数量除以像素跨度
	float span;        //像素跨度，最大像素减去最小像素的值
	char* FilePath;
public:
	claheParam();
	~claheParam();
	claheParam(const char* fileName);
	claheParam& operator=(const claheParam& other);
	void loadParam();
	void saveParam();
};


struct FNNOEXPORT clipParam :public paramBase
{
	int up;
	int down;
	int left;
	int right;
	char* FilePath;
	clipParam();
	~clipParam();
	clipParam(const char* filePath);
	clipParam& operator=(const clipParam& other);
	void loadParam();
	void saveParam();
private:
	void getDefaultData();
};
//Params for ACE and Gamma channge 
struct FNNOEXPORT struAceGammaParam :public paramBase
{
public:
	int ace_r;          //方差计算半径
	float ace_amount;     //增益倍数
	float ace_gainlimit;  //限制增益系数，增益系数不可超过该值
	float gamma_cof;     //gamma参数
	int algorithm_type;  //算法方法，algorithm_type=0采用float型进行计算，速度快，但是精度有限；algorithm_type=1，采用double型进行计算，速度慢，精度高。当然，肉眼看上去差不多
	int isNeedEquHists;  //是否进行直方图均衡化
	int index;
	char* FilePath;
	struAceGammaParam();
	struAceGammaParam(int ind);
	~struAceGammaParam();
	struAceGammaParam(const char* filePath, int ind = 0);
	struAceGammaParam& operator=(const struAceGammaParam& other);
	void loadParam();
	void saveParam();
private:
	void getDefaultData();
};

struct FNNOEXPORT struBilatralParam :public paramBase
{
public:
	int k_w;
	int k_h;
	float sigColor_x;
	float sigColor_y;
	float sigSpace_x;
	float sigSpace_y;
	char* FilePath;
public:
	struBilatralParam();
	~struBilatralParam();
	struBilatralParam(const char* filePath);
	struBilatralParam& operator=(const struBilatralParam& other);
	void getDefaultData();
	void loadParam();
	void saveParam();
};


struct FNNOEXPORT struWLNParam :public paramBase
{
public:
	float wl;
	float wr;
	float specifymin;
	float specifymax;
	char* FilePath;
public:
	struWLNParam();
	~struWLNParam();
	struWLNParam(const char* filePath);
	struWLNParam& operator=(const struWLNParam& other);
	void getDefaultData();
	void loadParam();
	void saveParam();
};

struct FNNOEXPORT struImageBaseParam
{
public:
	int brightness;
	float constrast;
	float gamma_cof;
	int med_r;
	int box_r;
	struImageBaseParam()
	{
		brightness = 0;
		constrast = 1.0f;
		gamma_cof = 1.0f;
		med_r = 0;
		box_r = 0;
	}

//public:
//	struBaseParam();
//	~struBaseParam();
//	struBaseParam(const char* filePath);
//	struBaseParam& operator=(const struBaseParam& other);
//	void getDefaultData();
//	void loadParam();
//	void saveParam();
};

struct FNNOEXPORT struFastNLMParam:public paramBase
{
public:
	int rd;
	int rD;
	float h;
	char* FilePath;
public:
	struFastNLMParam();
	~struFastNLMParam();
	struFastNLMParam(const char* filePath);
	struFastNLMParam& operator=(const struFastNLMParam& other);
	void getDefaultData();
	void loadParam();
	void saveParam();
};

struct FNNOEXPORT struUSMParam :public paramBase
{
public:
	int r;
	int isNeedNorm;
	float lam;
	char* FilePath;
public:
	struUSMParam();
	~struUSMParam();
	struUSMParam(const char* filePath);
	struUSMParam& operator=(const struUSMParam& other);
	void getDefaultData();
	void loadParam();
	void saveParam();
};


struct FNNOEXPORT struClaheParam :public paramBase
{
public:  //必须初始化的量
	int widthBlockNum;
	int heightBlockNum;
	int histBin;
	int peak;
public:
	float binRatio;    //比宁率，直方图数量除以像素跨度
	float span;        //像素跨度，最大像素减去最小像素的值
	char* FilePath;
public:
	struClaheParam();
	~struClaheParam();
	struClaheParam(const char* fileName,int index=0);
	struClaheParam& operator=(const struClaheParam& other);
	void loadParam();
	void saveParam();
private:
	int param_index;
};

#endif