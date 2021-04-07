#include "VolMat.h"
#include <iostream>
using namespace std;
using namespace IMat;
VolMat::VolMat()
{
	rows = 0;
	cols = 0;
	pieces = 0;
	RefreshInfo();
	x_ratio = 0.0;
	y_ratio = 0.0;
	z_ratio = 0.0;
	data = nullptr;
	triFaces = vector<vector<FnPoint3f>>();
}

VolMat::VolMat(const VolMat& other)
{
	Set(other.cols, other.rows, other.pieces, other.data);
}

VolMat::VolMat(int _cols, int _rows, int _pieces, unsigned char* _data)
{
	Set(_cols, _rows, _pieces, _data);
}

void VolMat::Set(int _cols, int _rows, int _pieces, unsigned char* _data)
{
	rows = rows;
	cols = cols;
	pieces = pieces;
	RefreshInfo();
	SafeDeleteData();
	if (_data != nullptr)
	{
		data = new unsigned char[rows * cols * pieces];
		memcpy(data, _data, rows * cols * pieces * sizeof(unsigned char));
	}
}

VolMat::~VolMat()
{
	SafeDeleteData();
}

void VolMat::ReadRaw(const char* path, int _cols, int _rows, int _pieces, int offset)
{
	FILE* fp = NULL;
	if (!(fp = fopen(path, "rb")))
	{
		cout << "fail to open file" << endl;
	}
	else
	{
		cols = _cols;
		rows = _rows;
		pieces = _pieces;
		RefreshInfo();
		SafeDeleteData();
		unsigned long long imgByteSize = GetImgByteSize();
		if (offset == 0)
		{
			data = new unsigned char[imgByteSize];
			fread(data, 1, imgByteSize, fp);
		}
		else
		{
			unsigned char* temp = new unsigned char[imgByteSize + offset];
			fread(temp, 1, imgByteSize + offset, fp);
			memcpy(data, temp + offset, imgByteSize);
			delete[]temp;
		}
		fclose(fp);
	}

}

void VolMat::WriteRaw(const char* path)
{
	FILE* fp = NULL;
	if (!(fp = fopen(path, "wb")))
	{
		cout << "fail to open file" << endl;
	}
	else
	{
		if (data != NULL)
		{
			unsigned long long imgByteSize = GetImgByteSize();
			fwrite(data, sizeof(float), imgByteSize, fp);
		}
		fclose(fp);
	}
}

unsigned long long VolMat::GetImgByteSize()
{
	return imgByteSize;
}

unsigned long long VolMat::GetOnePieceSize()
{
	return onePieceByteSize;
}

int VolMat::PutTriface(std::vector<std::vector<IMat::FnPoint3f>>& trifaces, double _x_ratio, double _y_ratio, double _z_ratio)
{
	if (data == nullptr)
		return -1;
	x_ratio = _x_ratio;
	y_ratio = _y_ratio;
	z_ratio = _z_ratio;
	int triNum = (int)trifaces.size();
	vector<vector<FnPoint3f>> extendPts(triNum);
	for (int triId = 0; triId < triNum; ++triId)
	{
		int ptnum = (int)trifaces[triId].size();
		if (ptnum != 3)
			continue;
		extendPts[triId].resize(3);




	}
	return 1;
}

void VolMat::RefreshInfo()
{
	onePieceByteSize = rows * cols * sizeof(unsigned char);
	imgByteSize = onePieceByteSize*pieces;
	vol_center.Set(cols / 2, rows / 2, pieces / 2);
}

void VolMat::SafeDeleteData()
{
	if (data != nullptr)
	{
		delete[]data;
		data = nullptr;
	}
}


