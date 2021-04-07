#include<iostream>
#include <string>
#include "iniFile.h"
#include <cstdlib>
#include "post_process_chain.h"
using namespace std;

void initest()
{
	string str = "abc";
	char str1[54] = {0};
	while (1)
	{
		int num = rand() % 100+1;
		char* temp = new char[num];
		for (int i = 0; i < num-1; i++)
		{
			int index = rand() % 3;
			if (index == 0)
				temp[i] = rand() % 25 + 97;
			else if (index == 1)
				temp[i] = rand() % 25 + 65;
			else if (index == 2)
				temp[i] = rand() % 10 + '0';
		}
		temp[num - 1] = 0;
		WriteString_INI("Configuration", "Language ", temp, "C:/Users/zhangqi/Desktop/ImageProcess_CFG.INI");
		int len =strlen(temp)+1;
		char* res = new char[len];
		ReadString_INI("Configuration", "Language ", res, len,"C:/Users/zhangqi/Desktop/ImageProcess_CFG.INI");

		cout << temp << endl;
		cout << res << endl;

		if (strcmp(temp, res))
		{
			cout << "检测到不一致！请按任意键继续" << endl;
			cin >> str;
		}
		cout << endl << endl; 
		delete []temp;
	}
}

int main()
{
	
	string postInfo;
	//Img_Post_Process_Chain(nullptr, nullptr, 0, 0, "C:/Users/Administrator/Desktop/test/test.txt", postInfo);
	//int xx = 0;
}