#ifndef _VOLMAT_H_
#define _VOLMAT_H_
#include<vector>
#include "FnPoint.h"
class VolMat
{
public:
	int cols;
	int rows;
	int pieces;
	unsigned char* data;
	unsigned long long onePieceByteSize;
	unsigned long long imgByteSize;
	double x_ratio;
	double y_ratio;
	double z_ratio;
	std::vector<std::vector<IMat::FnPoint3f>> triFaces;
	//std::vector<std::vector<IMat::FnPoint3f>> extendtriFaces;
	IMat::FnPoint3f vol_center;
public:
	VolMat();

	VolMat(const VolMat& other);

	VolMat(int _cols, int _rows, int _pieces, unsigned char* data);
	
	void Set(int _cols, int _rows, int _pieces, unsigned char* data);

	~VolMat();
	
	void ReadRaw(const char* path, int _cols, int _rows, int _pieces, int offset = 0);

	void WriteRaw(const char* path);

	unsigned long long GetImgByteSize();

	unsigned long long GetOnePieceSize();

	int PutTriface(std::vector<std::vector<IMat::FnPoint3f>>& trifaces, double x_ratio,double y_ratio,double z_ratio);

	inline unsigned char GetValue(int x, int y, int z)
	{
		return data[onePieceByteSize * z + y * cols + x];
	}



private:

	void RefreshInfo();

	void SafeDeleteData();

};

#endif // !_VOLMAT_H_
