#include "post_process_chain.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
namespace IMat
{
	string GetSubStr(const string& str, char begin, char end)
	{
		int begin_ind = (int)str.find(begin) + 1;
		int end_ind = (int)str.find(end, begin_ind);
		string sub_str = "";
		if (begin_ind >= 0 && end_ind > begin_ind)
			sub_str = str.substr(begin_ind, end_ind - begin_ind);
		return sub_str;
	}

	int GetIntSubStr(const string& src, char begin, char end)
	{
		string temp = GetSubStr(src, begin, end);
		return stoi(temp);
	}

	float GetFloatSubStr(const string& src, char begin, char end)
	{
		string temp = GetSubStr(src, begin, end);
		return stof(temp);
	}

	int Do_CLAHE(uint16_t* psrc, uint16_t* pdst, int w, int h, vector<string>& contents)
	{
		int len = (int)contents.size();
		if (len != 5) return -11;
		int i = 1;
		claheParam m;
		m.peak = GetIntSubStr(contents[i++], '=', ';');
		m.histBin = GetIntSubStr(contents[i++], '=', ';');
		m.heightBlockNum = GetIntSubStr(contents[i++], '=', ';');
		m.widthBlockNum = GetIntSubStr(contents[i++], '=', ';');
		return clahe_16U(psrc, pdst, w, h, m);
	}

	int Do_Gamma(uint16_t* psrc, uint16_t* pdst, int w, int h, vector<string>& contents)
	{
		int len = (int)contents.size();
		if (len != 1) return -11;
		int i = 1;
		float gamma_cof = GetFloatSubStr(contents[i++], '=', ';');
		return GammaCorrect(psrc, pdst, w, h, gamma_cof, PixelForm::U16);
	}
	//
	int Do_BC(uint16_t* psrc, uint16_t* pdst, int w, int h, vector<string>& contents)
	{
		int len = (int)contents.size();
		if (len != 3) return -11;
		int i = 1;
		int b = GetIntSubStr(contents[i++], '=', ';');
		float c = GetFloatSubStr(contents[i++], '=', ';');
		return BrightAndContrast(psrc, pdst, w, h, PixelForm::U16, (float)b, c);
	}

	int Do_USM(uint16_t* psrc, uint16_t* pdst, int w, int h, vector<string>& contents)
	{
		int len = (int)contents.size();
		if (len != 4) return -11;
		int i = 1;
		struUSMParam m;
		m.r = GetIntSubStr(contents[i++], '=', ';');
		m.lam = GetFloatSubStr(contents[i++], '=', ';');
		m.isNeedNorm = GetIntSubStr(contents[i++], '=', ';');
		return USM_EX(psrc, pdst, w, h, PixelForm::U16, m);
	}

	int Do_WLN(uint16_t* psrc, uint16_t* pdst, int w, int h, vector<string>& contents)
	{
		int len = (int)contents.size();
		if (len != 5) return  -11;
		int i = 1;
		struWLNParam m;
		m.wl = (float)GetIntSubStr(contents[i++], '=', ';');
		m.wr = (float)GetIntSubStr(contents[i++], '=', ';');
		m.specifymin = (float)GetIntSubStr(contents[i++], '=', ';');
		m.specifymax = (float)GetIntSubStr(contents[i++], '=', ';');
		return WLN(psrc, pdst, w * h, PixelForm::U16, m);
	}

	int Do_ACE(uint16_t* psrc, uint16_t* pdst, int w, int h, vector<string>& contents)
	{
		//解析ACE参数
		int len = (int)contents.size();
		string temp;
		if (len != 7) return -11;
		struAceGammaParam m;
		int i = 1;
		m.ace_r = GetIntSubStr(contents[i++], '=', ';');
		m.ace_amount = GetFloatSubStr(contents[i++], '=', ';');
		m.ace_gainlimit = GetFloatSubStr(contents[i++], '=', ';');
		m.gamma_cof = GetFloatSubStr(contents[i++], '=', ';');
		m.algorithm_type = GetIntSubStr(contents[i++], '=', ';');
		m.isNeedEquHists = GetIntSubStr(contents[i++], '=', ';');
		return Ace_Gamma(psrc, pdst, w, h, PixelForm::U16, m);
	}

	int Do_Smooth_Med(uint16_t* psrc, uint16_t* pdst, int w, int h, vector<string>& contents)
	{
		int len = (int)contents.size();
		if (len != 2) return -11;
		int i = 1;
		int r = GetIntSubStr(contents[i++], '=', ';');
		return Median_Blur(psrc, pdst, w, h, r, PixelForm::U16);
	}

	int Do_Smooth_Box(uint16_t* psrc, uint16_t* pdst, int w, int h, vector<string>& contents)
	{
		int len = (int)contents.size();
		if (len != 2) return -11;
		int i = 1;
		int r = GetIntSubStr(contents[i++], '=', ';');
		return BoxBlur(psrc, pdst, w, h, PixelForm::U16, r);
	}

	int Do_FastNLM(uint16_t* psrc, uint16_t* pdst, int w, int h, vector<string>& contents)
	{
		int len = (int)contents.size();
		if (len != 4) return -11;
		int i = 1;
		struFastNLMParam m;
		m.rd = GetIntSubStr(contents[i++], '=', ';');
		m.rD = GetIntSubStr(contents[i++], '=', ';');
		m.h = GetFloatSubStr(contents[i++], '=', ';');
		if (psrc == NULL)
			return -1;
		if (pdst == NULL)
			return -2;
		uint8_t* psrc8 = new uint8_t[w * h];

		int imgSize = w * h;
		for (int i = 0; i < imgSize; ++i)
			psrc8[i] = (uint8_t)(psrc[i] / 257.0f + 0.5f);
		int ret = FastNLM(psrc8, psrc8, w, h, m.h, m.rd, m.rD);

		for (int i = 0; i < imgSize; ++i)
			psrc[i] = (uint16_t)(psrc8[i] * 257.0f);
		delete[]psrc8;
		return ret;
	}

	int Do_Inverse(uint16_t* psrc, uint16_t* pdst, int w, int h, vector<string>& contents)
	{
		int len = (int)contents.size();
		if (len != 1) return -11;
		return Inverse(psrc, pdst, w, h, PixelForm::U16);
	}

	extern "C"
		FNNOEXPORT int Img_Post_Process_Chain(uint16_t * psrc, uint16_t * pdst, int w, int h, const char* filePath, string & PostInfo)
	{
		string word;
		vector<vector<string>> paramBlocks;
		PostInfo = "";
		ifstream inf(filePath); //单词文件bai
		if (!inf)
		{
			cout << "NO Post Process Chain File" << endl;
			return -1;
		}
		vector<string> contents;
		while (inf >> word)
		{
			PostInfo.append(word);
			int index = (int)word.find("end");
			if (word.find("end") == 0)
			{
				paramBlocks.push_back(contents);
				contents.clear();
			}
			else
				contents.push_back(word);
		}
		inf.close();

		int len = (int)paramBlocks.size();
		if (len == 0)
		{
			cout << "Do Not Find Param" << endl;
			return -2;
		}

		memcpy(pdst, psrc, sizeof(uint16_t) * w * h);
		for (int i = 0; i < len; ++i)
		{
			string head = paramBlocks[i][0];
			string key = GetSubStr(paramBlocks[i][0], '[', ']');
			if (key == "ACE")
				Do_ACE(pdst, pdst, w, h, paramBlocks[i]);
			else if (key == "CLAHE")
				Do_CLAHE(pdst, pdst, w, h, paramBlocks[i]);
			else if (key == "WLN")
				Do_WLN(pdst, pdst, w, h, paramBlocks[i]);
			else if (key == "USM")
				Do_USM(pdst, pdst, w, h, paramBlocks[i]);
			else if (key == "BC")
				Do_BC(pdst, pdst, w, h, paramBlocks[i]);
			else if (key == "Smooth_Med")
				Do_Smooth_Med(pdst, pdst, w, h, paramBlocks[i]);
			else if (key == "Smooth_Box")
				Do_Smooth_Box(pdst, pdst, w, h, paramBlocks[i]);
			else if (key == "FastNLM")
				Do_FastNLM(pdst, pdst, w, h, paramBlocks[i]);
			else if (key == "Inverse")
				Do_Inverse(pdst, pdst, w, h, paramBlocks[i]);
			else
				break;
		}
		return 0;
	}
};