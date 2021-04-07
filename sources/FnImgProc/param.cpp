#include "param.h"
paramBase::paramBase() {
	imap_ex = new map<int*, const char*>();
	fmap_ex = new map<float*, const char*>();
	iPtr=new vector<int*>();
	fPtr=new vector<float*>();
	iID = new vector<const char*>();
	fID = new vector<const char*>();
};

paramBase::~paramBase()
{
	delete imap_ex; imap_ex = 0;
	delete fmap_ex; fmap_ex = 0;

	delete iPtr; iPtr = 0;
	delete fPtr; fPtr = 0;
	delete iID; iID = 0;
	delete fID; fID = 0;
}


void paramBase::clearDataToBeSaved()
{
	if (iPtr->size() > 0)
		iPtr->clear();
	if (iID->size() > 0)
		iID->clear();
	if (fPtr->size() > 0)
		fPtr->clear();
	if (fID->size() > 0)
		fID->clear();
	if (imap_ex->size())
		imap_ex->clear();
	if (fmap_ex->size())
		fmap_ex->clear();
}

void paramBase::saveData(const char* app, const char* iniPath)
{
	map<int*, const char*>::iterator iiter_ex;
	map<float*, const char*>::iterator fiter_ex;
	for (iiter_ex = imap_ex->begin(); iiter_ex != imap_ex->end(); ++iiter_ex)
		WriteInt_INI(app, iiter_ex->second, *(iiter_ex->first), iniPath);
	for (fiter_ex = fmap_ex->begin(); fiter_ex != fmap_ex->end(); ++fiter_ex)
		WriteFloat_INI(app, fiter_ex->second, *(fiter_ex->first), iniPath);
}

void paramBase::getData(const char* app, const char* iniPath)
{
	map<int*, const char*>::iterator iiter_ex;
	map<float*, const char*>::iterator fiter_ex;
	for (iiter_ex = imap_ex->begin(); iiter_ex != imap_ex->end(); ++iiter_ex)
		*(iiter_ex->first) = ReadInt_INI(app, iiter_ex->second, iniPath, *(iiter_ex->first));
	for (fiter_ex = fmap_ex->begin(); fiter_ex != fmap_ex->end(); ++fiter_ex)
		*(fiter_ex->first) = ReadFloat_INI(app, fiter_ex->second, iniPath, *(fiter_ex->first));
}

void paramBase::saveData_byVec(const char* app, const char* iniPath)
{
	int len_int =(int) min(iPtr->size(), iID->size());
	int len_float =(int) min(fPtr->size(), fID->size());
	for (int i = len_int - 1; i >= 0; --i)
		WriteInt_INI(app, (*iID)[i], *((*iPtr)[i]), iniPath);
	for (int i = len_float - 1; i >= 0; --i)
		WriteFloat_INI(app, (*fID)[i], *((*fPtr)[i]), iniPath);
}

void paramBase::getData_byVec(const char* app, const char* iniPath)
{
	int len_int =(int) min(iPtr->size(), iID->size());
	int len_float =(int) min(fPtr->size(), fID->size());
	for (int i = 0; i < len_int; ++i)
		*((*iPtr)[i]) = ReadInt_INI(app, (*iID)[i], iniPath, *((*iPtr)[i]));
	for (int i = 0; i < len_float; ++i)
		*((*fPtr)[i]) = ReadFloat_INI(app, (*fID)[i], iniPath, *((*fPtr)[i]));
}

int paramBase::CopyFilePath(char*& pDst, const char* pSrc)
{
	if (pSrc == 0) return -1;
	if (pDst == pSrc) return 1;
	int len = strlen(pSrc) + 1;
	if (pDst != 0)
		delete[]pDst;
	pDst = new char[len];
	strcpy(pDst, pSrc);
	return 1;
}

//////////////////////////////////////
claheParam::claheParam()
{
	widthBlockNum = 8;
	heightBlockNum = 8;
	histBin = 256;
	peak = 200;
	span = 65535;
	binRatio = 0.5f;
	FilePath = 0;
	imap_ex->insert(getIPtrAndName(widthBlockNum));
	imap_ex->insert(getIPtrAndName(heightBlockNum));
	imap_ex->insert(getIPtrAndName(histBin));
	imap_ex->insert(getIPtrAndName(peak));
};

claheParam& claheParam:: operator=(const claheParam& other)
{
	if (this == &other)
		return *this;
	CopyFilePath(this->FilePath, other.FilePath);
	this->widthBlockNum = other.widthBlockNum;
	this->heightBlockNum = other.heightBlockNum;
	this->histBin = other.histBin;
	this->peak = other.peak;
	this->binRatio = other.binRatio;
	this->span = other.span;
	return *this;
}

claheParam::claheParam(const char* fileName)
{
	FilePath = 0;
	CopyFilePath(FilePath, fileName);
	loadParam();
}

void claheParam::loadParam()
{
	widthBlockNum = 8;
	heightBlockNum = 8;
	histBin = 256;
	peak = 600;
	imap_ex->insert(getIPtrAndName(peak));
	imap_ex->insert(getIPtrAndName(histBin));
	imap_ex->insert(getIPtrAndName(widthBlockNum));
	imap_ex->insert(getIPtrAndName(heightBlockNum));		
}

void claheParam::saveParam()
{
	saveData("claheParam", FilePath);
}

claheParam::~claheParam()
{
	if (FilePath)
		delete FilePath;
}

//////////////////////////////////////////////////////////
clipParam::clipParam()
{
	getDefaultData();
}

clipParam::clipParam(const char* filePath)
{
	getDefaultData();
	FilePath = 0;
	CopyFilePath(FilePath, filePath);
	loadParam();
}

clipParam& clipParam::operator=(const clipParam& other)
{
	if (this == &other)
		return *this;
	CopyFilePath(this->FilePath, other.FilePath);
	this->up = other.up;
	this->down = other.down;
	this->left = other.left;
	this->right = other.right;
	return *this;
}

void clipParam::loadParam()
{
	getData("clipParam", FilePath);
}

void clipParam::saveParam()
{
	saveData("clipParam", FilePath);
}

void clipParam::getDefaultData()
{
	this->up = 20;
	this->down = 20;
	this->left = 200;
	this->right = 200;
	imap_ex->insert(getIPtrAndName(up));
	imap_ex->insert(getIPtrAndName(down));
	imap_ex->insert(getIPtrAndName(left));
	imap_ex->insert(getIPtrAndName(right));
}

clipParam::~clipParam()
{
	if (FilePath)
		delete[]FilePath;
}

///////////////////////////////////////////////////
struBilatralParam::struBilatralParam()
{
	getDefaultData();
}

struBilatralParam::struBilatralParam(const char* filePath)
{
	getDefaultData();
	FilePath = 0;
	CopyFilePath(FilePath, filePath);
	loadParam();
}

struBilatralParam& struBilatralParam::operator= (const struBilatralParam& other)
{
	if (this == &other)
		return *this;
	CopyFilePath(this->FilePath, other.FilePath);
	this->k_w = other.k_w;
	this->k_h = other.k_h;
	this->sigColor_x = this->sigColor_x;
	this->sigColor_y = this->sigColor_y;
	this->sigSpace_x = this->sigSpace_x;
	this->sigSpace_y = this->sigSpace_y;
	return *this;
}

void struBilatralParam::getDefaultData()
{
	k_w = 5;
	k_h = 5;
	sigColor_x = 0.5f;
	sigColor_y = 0.5f;
	sigSpace_x = 0.5f;
	sigSpace_y = 0.5f;
}

void struBilatralParam::loadParam()
{
	imap_ex->insert(getIPtrAndName(k_w));
	imap_ex->insert(getIPtrAndName(k_h));
	fmap_ex->insert(getFPtrAndName(sigColor_x));
	fmap_ex->insert(getFPtrAndName(sigColor_y));
	fmap_ex->insert(getFPtrAndName(sigSpace_x));
	fmap_ex->insert(getFPtrAndName(sigSpace_y));
	getData("struBilatralParam", FilePath);
}

void struBilatralParam::saveParam()
{
	saveData("struBilatralParam", FilePath);
}

struBilatralParam::~struBilatralParam()
{
	if (FilePath)
		delete[]FilePath;
}

////////////struAceGammaParam参数////////////
struAceGammaParam::struAceGammaParam()
{
	this->index = 0;
	getDefaultData();
	loadParam();
}
struAceGammaParam::struAceGammaParam(int ind)
{
	this->index = ind;
	getDefaultData();
	loadParam();
}

struAceGammaParam::struAceGammaParam(const char* filePath, int ind)
{
	this->index = ind;
	getDefaultData();
	CopyFilePath(this->FilePath, filePath);
	loadParam();
}

void struAceGammaParam::getDefaultData()
{
	gamma_cof = 0.8f;
	ace_r = 20;
	ace_amount = 1.0f;
	ace_gainlimit = 3.0f;
	algorithm_type = 0;
	isNeedEquHists = 0;
	FilePath = NULL;;
}

struAceGammaParam::~struAceGammaParam()
{
	if (FilePath)
		delete FilePath;
}

void struAceGammaParam::loadParam()
{

	/*fmap_ex->insert(make_pair<float*, const char*>(&gamma_cof, const_cast<const char*>("param1")));
	fmap_ex->insert(make_pair<float*, const char*>(&ace_r, const_cast<const char*>("param2")));
	fmap_ex->insert(make_pair<float*, const char*>(&ace_amount, const_cast<const char*>("param3")));
	fmap_ex->insert(make_pair<float*, const char*>(&ace_gainlimit, const_cast<const char*>("param4")));
	imap_ex->insert(make_pair<int*, const char*>(&algorithm_type, const_cast<const char*>("param5")));*/
	iPtr->push_back(&ace_r);
	iID->push_back(const_cast<const char*>("param1"));
	fPtr->push_back(&ace_amount);
	fID->push_back(const_cast<const char*>("param2"));
	fPtr->push_back(&ace_gainlimit);
	fID->push_back(const_cast<const char*>("param3"));
	fPtr->push_back(&gamma_cof);
	fID->push_back(const_cast<const char*>("param4"));
	iPtr->push_back(&algorithm_type);
	iID->push_back(const_cast<const char*>("param5"));
	iPtr->push_back(&isNeedEquHists);
	iID->push_back(const_cast<const char*>("param6"));

	char str[64];
	sprintf(str, "Post_Process_Param_%d", this->index);
	if (FilePath)
		getData_byVec(str, FilePath);

}

void struAceGammaParam::saveParam()
{
	char str[64];
	sprintf(str, "Post_Process_Param_%d", this->index);
	saveData_byVec(str, FilePath);
	//saveData("struAceGammaParam", FilePath);
}

struAceGammaParam& struAceGammaParam::operator=(const struAceGammaParam& other)
{
	if (this == &other)
		return *this;
	CopyFilePath(this->FilePath, other.FilePath);
	this->ace_r = other.ace_r;
	this->ace_amount = other.ace_amount;
	this->ace_gainlimit = other.ace_gainlimit;
	this->gamma_cof = other.gamma_cof;
	this->algorithm_type = other.algorithm_type;
	this->index = other.index;
	this->isNeedEquHists = other.isNeedEquHists;
	return *this;
}

/////////////////WLN参数//////////////////////////
struWLNParam::struWLNParam()
{
	getDefaultData();
}

struWLNParam::struWLNParam(const char* filePath)
{
	getDefaultData();
	CopyFilePath(FilePath, filePath);
	loadParam();
}


void struWLNParam::loadParam()
{
	getData("WLNParam", FilePath);
}

void struWLNParam::saveParam()
{
	saveData("WLNParam", FilePath);
}

struWLNParam::~struWLNParam()
{
	if (FilePath)
		delete[]FilePath;
}

struWLNParam& struWLNParam::operator=(const struWLNParam& other)
{
	if (this == &other)
		return *this;
	wl = other.wl;
	wr = other.wr;
	specifymin = other.specifymin;
	specifymax = other.specifymax;
	CopyFilePath(FilePath, other.FilePath);
	return *this;
}

void struWLNParam::getDefaultData()
{
	wl = 0.0f;
	wr = 65535.0f;
	specifymin = 0.0f;
	specifymax = 65535.0f;
	FilePath = 0;
	fmap_ex->insert(getFPtrAndName(wl));
	fmap_ex->insert(getFPtrAndName(wr));
	fmap_ex->insert(getFPtrAndName(specifymin));
	fmap_ex->insert(getFPtrAndName(specifymax));
}

//struFastNLMParam参数列表
struFastNLMParam::struFastNLMParam()
{
	getDefaultData();
	imap_ex->insert(getIPtrAndName(rd));
	imap_ex->insert(getIPtrAndName(rD));
	fmap_ex->insert(getFPtrAndName(h));
}

struFastNLMParam::struFastNLMParam(const char *filePath)
{
	getDefaultData();
	CopyFilePath(FilePath, filePath);
	loadParam();
}

struFastNLMParam::~struFastNLMParam()
{
	if (FilePath)
		delete[]FilePath;
}

void struFastNLMParam::getDefaultData()
{
	rd = 1;
	rD = 3;
	h = 3.0f;
	FilePath = NULL;
}

void struFastNLMParam::loadParam()
{
	getData("struFastNLMParam", FilePath);
}

void struFastNLMParam::saveParam()
{
	saveData("struFastNLMParam", FilePath);
}

struFastNLMParam& struFastNLMParam::operator=(const struFastNLMParam& other)
{
	if (this == &other)
		return *this;
	rd = other.rd;
	rD = other.rD;
	h = other.h;
	CopyFilePath(FilePath, other.FilePath);
	return *this;
}

//struUSMParam参数列表
struUSMParam::struUSMParam()
{
	getDefaultData();
	imap_ex->insert(getIPtrAndName(r));
	imap_ex->insert(getIPtrAndName(isNeedNorm));
	fmap_ex->insert(getFPtrAndName(lam));
}

struUSMParam::struUSMParam(const char* filePath)
{
	getDefaultData();
	CopyFilePath(FilePath, filePath);
	loadParam();
}

struUSMParam::~struUSMParam()
{
	if (FilePath)
		delete[]FilePath;
}

void struUSMParam::getDefaultData()
{
	r = 1;
	lam = 0.5f;
	isNeedNorm = 0;
	FilePath = NULL;
}

void struUSMParam::loadParam()
{
	getData("struFastNLMParam", FilePath);
}

void struUSMParam::saveParam()
{
	saveData("struFastNLMParam", FilePath);
}

struUSMParam& struUSMParam::operator=(const struUSMParam& other)
{
	if (this == &other)
		return *this;
	r = other.r;
	lam = other.lam;
	CopyFilePath(FilePath, other.FilePath);
	return *this;
}

//////////////////////////////////////
struClaheParam::struClaheParam()
{
	widthBlockNum = 8;
	heightBlockNum = 8;
	histBin = 256;
	peak = 200;
	span = 65535;
	binRatio = 0.5f;
	FilePath = 0;
	param_index = 0;
	imap_ex->insert(getIPtrAndName(widthBlockNum));
	imap_ex->insert(getIPtrAndName(heightBlockNum));
	imap_ex->insert(getIPtrAndName(histBin));
	imap_ex->insert(getIPtrAndName(peak));
};

struClaheParam& struClaheParam:: operator=(const struClaheParam& other)
{
	if (this == &other)
		return *this;
	CopyFilePath(this->FilePath, other.FilePath);
	this->widthBlockNum = other.widthBlockNum;
	this->heightBlockNum = other.heightBlockNum;
	this->histBin = other.histBin;
	this->peak = other.peak;
	this->binRatio = other.binRatio;
	this->span = other.span;
	this->param_index = other.param_index;
	return *this;
}

struClaheParam::struClaheParam(const char* fileName,int index)
{
	param_index = index;
	FilePath = 0;
	CopyFilePath(FilePath, fileName);
	loadParam();
}

void struClaheParam::loadParam()
{
	widthBlockNum = 8;
	heightBlockNum = 8;
	histBin = 256;
	peak = 600;

	iPtr->push_back(&peak);
	iID->push_back(const_cast<const char*>("param1"));
	iPtr->push_back(&histBin);
	iID->push_back(const_cast<const char*>("param2"));
	iPtr->push_back(&widthBlockNum);
	iID->push_back(const_cast<const char*>("param3"));
	iPtr->push_back(&heightBlockNum);
	iID->push_back(const_cast<const char*>("param4"));

	char str[64];
	sprintf(str, "Post_Process_Param_%d", param_index);
	if (FilePath)
		getData_byVec(str, FilePath);

}

void struClaheParam::saveParam()
{
	saveData("struClaheParam", FilePath);
}

struClaheParam::~struClaheParam()
{
	if (FilePath)
		delete FilePath;
}

