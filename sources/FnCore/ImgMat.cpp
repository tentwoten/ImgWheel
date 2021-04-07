#include "ImgMat.hpp"
#include "ImgMatAux.hpp"
#include "ImgMatHelper.h"
#include <mutex>
using namespace std;

mutex mx;

namespace IMat {
	uint64_t Mat::m_ByteOffset = 4;
	Mat Mat::operator=(int8_t val)
	{
		Create(rows, cols, PixelForm::S8);
		CopyVal((int8_t*)data, val, imgByteSize / m_PByteSize);
		return *this;
	}
	Mat Mat::operator=(uint8_t val)
	{
		Create(rows, cols, PixelForm::U8);
		CopyVal((uint8_t*)data, val, imgByteSize / m_PByteSize);
		return *this;
	}
	Mat Mat::operator=(int16_t val)
	{
		Create(rows, cols, PixelForm::S16);
		CopyVal((int16_t*)data, val, imgByteSize/m_PByteSize);
		return *this;
	}
	Mat Mat::operator=(uint16_t val)
	{
		Create(rows, cols, PixelForm::U16);
		CopyVal((uint16_t*)data, val, imgByteSize / m_PByteSize);
		return *this;
	}
	Mat Mat::operator=(int val)
	{
		Create(rows, cols, PixelForm::S32);
		CopyVal((int*)data, val, imgByteSize / m_PByteSize);
		return *this;
	}
	Mat Mat::operator=(uint32_t val)
	{
		Create(rows, cols, PixelForm::U32);
		CopyVal((uint32_t*)data, val, imgByteSize / m_PByteSize);
		return *this;
	}
	Mat Mat::operator=(int64_t val)
	{
		Create(rows, cols, PixelForm::S64);
		CopyVal((int64_t*)data, val, imgByteSize / m_PByteSize);
		return *this;
	}
	Mat Mat::operator=(uint64_t val)
	{
		Create(rows, cols, PixelForm::U64);
		CopyVal((uint64_t*)data, val, imgByteSize / m_PByteSize);
		return *this;
	}
	Mat Mat::operator=(float val)
	{
		Create(rows, cols, PixelForm::F32);
		CopyVal((float*)data, val, imgByteSize / m_PByteSize);
		return *this;
	}
	Mat Mat::operator=(double val)
	{
		Create(rows, cols, PixelForm::F64);
		CopyVal((double*)data, val, imgByteSize / m_PByteSize);
		return *this;
	}
};
namespace IMat
{
	Mat::Mat()
	{
		rows = 0;
		cols = 0;
		m_Format = PixelForm::U8;
		m_ptr = nullptr;
		data = nullptr;	
		dataend = nullptr;
		step = 0;
		imgByteSize = 0;
		m_PByteSize = 0;
		channels = 1;
		//CalAdvanceData();
	}

	Mat::Mat(int64_t h, int64_t w, PixelForm pt)
	{
		rows = h;
		cols = w;
		m_Format = pt;
		m_ptr = nullptr;
		data = nullptr;
		Create(h,w,pt);
	}

	Mat::Mat(void* _data, int64_t h, int64_t w, PixelForm pt)
	{
		rows = h;
		cols = w;		
		m_Format = pt;
		m_ptr = nullptr;
		CalAdvanceData();
		if (_data != nullptr)
		{
			m_ptr = new uint8_t[imgByteSize + m_ByteOffset];
			RefreshPtr();
			if(data!=nullptr)
				memcpy(data, _data, imgByteSize);
			((int*)dataend)[0] = 1;
		}
		else
			RefreshPtr();
	}

	Mat::Mat(const Mat& other)
	{
		rows = other.rows;
		cols = other.cols;
		m_Format = other.m_Format;
		CalAdvanceData();
		m_ptr = other.m_ptr;	
		RefreshPtr();
		AddRefCount();	
	}

	Mat::~Mat()
	{
		Release();
	}

	Mat& Mat::operator=(const Mat& other)
	{
		if (this != &other)
		{
			if (m_ptr!=other.m_ptr)   //如果数据指针相同，那么不能先消除现在的指针
				Release();
			rows = other.rows;
			cols = other.cols;
			m_Format = other.m_Format;
			CalAdvanceData();
			m_ptr = other.m_ptr;
			RefreshPtr();
			AddRefCount();
		}
		return *this;
	}


	void Mat::Create(int64_t h,int64_t w,PixelForm pt)
	{
		Release();      //先解绑一下之前的内存
		rows = (int)h;
		cols = (int)w;
		m_Format = pt;
		CalAdvanceData();
		int64_t byteSize =imgByteSize+m_ByteOffset;
		if (imgByteSize > 0)
		{
			m_ptr = new uint8_t[byteSize];
		}
		RefreshPtr();
		mx.lock();
		((int*)dataend)[0] = 1;
		mx.unlock();
	}

	void Mat::Release()
	{
		if (m_ptr != nullptr)
		{
			int refcount = GetRefCount();
			if (refcount <= 1)   //引用次数小于等于1了，代表只有这里引用了,就可以删除了内存了
			{
				mx.lock();
				delete[]m_ptr;
				m_ptr = nullptr;
				RefreshPtr();
				mx.unlock();
			}
			else
			{
				SubRefCount();
			}	
		}
	}

	bool Mat::ReadRawFile(const char* path, int h, int w, PixelForm pt, int byte_offset)
	{
		FILE* fp = NULL;
		if (!(fp = fopen(path, "rb")))
		{
			cout << "fail to open file" << endl;
			//fclose(fp);
			return false;
		}
		else
		{
			Create(h, w, pt);
			if (byte_offset == 0)
			{
				fread(data, 1, imgByteSize, fp);
			}
			else
			{
				uint8_t* temp = new uint8_t[imgByteSize + byte_offset];
				fread(temp, 1, imgByteSize + byte_offset, fp);
				memcpy(data, temp + byte_offset, imgByteSize);
				delete[]temp;
			}
		}
		fclose(fp);
		return true;
	}

	Mat Mat::Clone() 
	{
		Mat res(rows,cols,m_Format);
		if (this->data != nullptr)
		{
			uint8_t* pdst = res.data;
			uint8_t* psrc = this->data;
			memcpy(pdst, psrc, res.imgByteSize);
		}
		return res;
	}
	
	void Mat::Transpose()
	{

	}

	bool Mat::IsSameSize(const Mat& other)
	{
		if (other.rows == this->rows && other.cols == this->cols && other.m_Format == this->m_Format)
			return true;
		return false;
	}

	int Mat::GetRefCount()
	{
		if (m_ptr == nullptr)
			return 0;
		return *((int*)(m_ptr+imgByteSize));
	}

	bool Mat::IsFloat()
	{
		if (m_Format == PixelForm::F32 || m_Format == PixelForm::F32_RGB
			|| m_Format == PixelForm::F64 || m_Format == PixelForm::F64_RGB)
			return true;
		return false;
	}

	void Mat::ConvertTo(Mat& dst, PixelForm type)
	{
		 void* psrc =(void*) this->data;
		 void* pdst = dst.data;
		 //原图图片为空，直接退出
		 if (psrc == nullptr)
		 {
			 dst = *this;
			 return;
		 }
		 PixelForm format_src = this->m_Format;
		 PixelForm format_dst = dst.m_Format;
		 int64_t w_src = this->cols;
		 int64_t h_src = this->rows;
		 if (psrc == pdst)
		 {
			 if (format_src == format_dst)
				 return;
		 }
		 int64_t goalByteSize =(int64_t) GetPixelByteSize(format_dst) * (int64_t)h_src * (int64_t)w_src;
		 uint8_t* temp = nullptr;
		 bool isSamePtr = (dst.data == this->data);
		 if (isSamePtr == true)
		 {
			 temp = new uint8_t[goalByteSize];
		 }
		 else if(IsSameSize(dst))
		 {
			 temp = dst.data;
		 }
		 else
		 {
			 dst.Create(h_src, w_src, type);
			 temp = dst.data;
		 }

		 int c_src = GetChannel(format_src);
		 int c_dst = GetChannel(format_dst);
		 PtrType pt_src = GetPtrType(format_src);
		 PtrType pt_dst = GetPtrType(format_dst);
		 double tor_min, tor_max;
		 bool isFloat;
		 GetTorMinMax(format_dst, tor_min, tor_max, isFloat);
		 int64_t pixelNum =(int64_t)(w_src) * h_src;
		 
		 switch (pt_src)
		 {
			case PtrType::S8:
			{
				switch (pt_dst)
				{
				case PtrType::S8: ConvertToBegin((char*)psrc, (char*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U8: ConvertToBegin((char*)psrc, (uint8_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::S16: ConvertToBegin((char*)psrc, (short*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U16: ConvertToBegin((char*)psrc, (uint16_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::S32: ConvertToBegin((char*)psrc, (int*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U32: ConvertToBegin((char*)psrc, (uint32_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::F32: ConvertToBegin((char*)psrc, (float*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::F64: ConvertToBegin((char*)psrc, (double*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::S64: ConvertToBegin((char*)psrc, (int64_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U64: ConvertToBegin((char*)psrc, (uint64_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				}
				break;
			}
			case PtrType::U8:
			{
				switch (pt_dst)
				{
				case PtrType::S8: ConvertToBegin((uint8_t*)psrc, (char*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U8: ConvertToBegin((uint8_t*)psrc, (uint8_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::S16: ConvertToBegin((uint8_t*)psrc, (short*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U16: ConvertToBegin((uint8_t*)psrc, (uint16_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::S32: ConvertToBegin((uint8_t*)psrc, (int*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U32: ConvertToBegin((uint8_t*)psrc, (uint32_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::F32: ConvertToBegin((uint8_t*)psrc, (float*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::F64: ConvertToBegin((uint8_t*)psrc, (double*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::S64: ConvertToBegin((uint8_t*)psrc, (int64_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U64: ConvertToBegin((uint8_t*)psrc, (uint64_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				}
				break;
			}
			case PtrType::S16:
			{
				switch (pt_dst)
				{
				case PtrType::S8: ConvertToBegin((short*)psrc, (char*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U8: ConvertToBegin((short*)psrc, (uint8_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::S16: ConvertToBegin((short*)psrc, (short*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U16: ConvertToBegin((short*)psrc, (uint16_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::S32: ConvertToBegin((short*)psrc, (int*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U32: ConvertToBegin((short*)psrc, (uint32_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::F32: ConvertToBegin((short*)psrc, (float*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::F64: ConvertToBegin((short*)psrc, (double*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::S64: ConvertToBegin((short*)psrc, (int64_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U64: ConvertToBegin((short*)psrc, (uint64_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				}
				break;
			}
			case PtrType::U16:
			{
				switch (pt_dst)
				{
				case PtrType::S8: ConvertToBegin((uint16_t*)psrc, (char*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U8: ConvertToBegin((uint16_t*)psrc, (uint8_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::S16: ConvertToBegin((uint16_t*)psrc, (short*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U16: ConvertToBegin((uint16_t*)psrc, (uint16_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::S32: ConvertToBegin((uint16_t*)psrc, (int*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U32:  ConvertToBegin((uint16_t*)psrc, (uint32_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::F32: ConvertToBegin((uint16_t*)psrc, (float*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::F64: ConvertToBegin((uint16_t*)psrc, (double*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::S64: ConvertToBegin((uint16_t*)psrc, (int64_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U64: ConvertToBegin((uint16_t*)psrc, (uint64_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				}
				break;
			}
			case PtrType::S32:
			{
				switch (pt_dst)
				{
				case PtrType::S8: ConvertToBegin((int*)psrc, (char*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U8: ConvertToBegin((int*)psrc, (uint8_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::S16: ConvertToBegin((int*)psrc, (short*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U16: ConvertToBegin((int*)psrc, (uint16_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::S32: ConvertToBegin((int*)psrc, (int*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U32: ConvertToBegin((int*)psrc, (uint32_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::F32: ConvertToBegin((int*)psrc, (float*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::F64: ConvertToBegin((int*)psrc, (double*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::S64: ConvertToBegin((int*)psrc, (int64_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U64: ConvertToBegin((int*)psrc, (uint64_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				}
				break;
			}
			case PtrType::U32:
			{
				switch (pt_dst)
				{
				case PtrType::S8: ConvertToBegin((uint32_t*)psrc, (char*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U8: ConvertToBegin((uint32_t*)psrc, (uint8_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::S16: ConvertToBegin((uint32_t*)psrc, (short*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U16: ConvertToBegin((uint32_t*)psrc, (uint16_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::S32: ConvertToBegin((uint32_t*)psrc, (int*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U32: ConvertToBegin((uint32_t*)psrc, (uint32_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::F32: ConvertToBegin((uint32_t*)psrc, (float*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::F64: ConvertToBegin((uint32_t*)psrc, (double*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::S64: ConvertToBegin((uint32_t*)psrc, (int64_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U64: ConvertToBegin((uint32_t*)psrc, (uint64_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				}
				break;
			}
			case PtrType::F32:
			{
				switch (pt_dst)
				{
				case PtrType::S8: ConvertToBegin((float*)psrc, (char*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U8: ConvertToBegin((float*)psrc, (uint8_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::S16: ConvertToBegin((float*)psrc, (short*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U16: ConvertToBegin((float*)psrc, (uint16_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::S32: ConvertToBegin((float*)psrc, (int*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U32: ConvertToBegin((float*)psrc, (uint32_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::F32: ConvertToBegin((float*)psrc, (float*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::F64: ConvertToBegin((float*)psrc, (double*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::S64: ConvertToBegin((float*)psrc, (int64_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U64: ConvertToBegin((float*)psrc, (uint64_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				}
				break;
			}
			case PtrType::F64:
			{
				switch (pt_dst)
				{
				case PtrType::S8: ConvertToBegin((double*)psrc, (char*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U8: ConvertToBegin((double*)psrc, (uint8_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::S16: ConvertToBegin((double*)psrc, (short*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U16: ConvertToBegin((double*)psrc, (uint16_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::S32: ConvertToBegin((double*)psrc, (int*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U32: ConvertToBegin((double*)psrc, (uint32_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::F32: ConvertToBegin((double*)psrc, (float*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::F64: ConvertToBegin((double*)psrc, (double*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::S64: ConvertToBegin((double*)psrc, (int64_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U64: ConvertToBegin((double*)psrc, (uint64_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				}
				break;
			}
			case PtrType::S64:
			{
				switch (pt_dst)
				{
				case PtrType::S8: ConvertToBegin((int64_t*)psrc, (char*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U8: ConvertToBegin((int64_t*)psrc, (uint8_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::S16: ConvertToBegin((int64_t*)psrc, (short*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U16: ConvertToBegin((int64_t*)psrc, (uint16_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::S32: ConvertToBegin((int64_t*)psrc, (int*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U32: ConvertToBegin((int64_t*)psrc, (uint32_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::F32: ConvertToBegin((int64_t*)psrc, (float*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::F64: ConvertToBegin((int64_t*)psrc, (double*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::S64: ConvertToBegin((int64_t*)psrc, (int64_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U64: ConvertToBegin((int64_t*)psrc, (uint64_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				}
				break;
			}


			case PtrType::U64:
			{
				switch (pt_dst)
				{
				case PtrType::S8: ConvertToBegin((uint64_t*)psrc, (char*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U8: ConvertToBegin((uint64_t*)psrc, (uint8_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::S16: ConvertToBegin((uint64_t*)psrc, (short*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U16: ConvertToBegin((uint64_t*)psrc, (uint16_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::S32: ConvertToBegin((uint64_t*)psrc, (int*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U32: ConvertToBegin((uint64_t*)psrc, (uint32_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::F32: ConvertToBegin((uint64_t*)psrc, (float*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::F64: ConvertToBegin((uint64_t*)psrc, (double*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::S64: ConvertToBegin((uint64_t*)psrc, (int64_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				case PtrType::U64: ConvertToBegin((uint64_t*)psrc, (uint64_t*)temp, pixelNum, c_src, c_dst, tor_min, tor_max, isFloat); break;
				}
				break;
			}
		 }	
		 return;
	}

	void Mat::CalAdvanceData()
	{
		channels = GetChannel(m_Format);
		m_PByteSize = GetPixelByteSize(m_Format);
		step =(int64_t)cols * channels;
		imgByteSize =(uint64_t) step * rows * m_PByteSize;
	}
	void Mat::RefreshPtr()
	{
		if (m_ptr == nullptr)
		{
			data = m_ptr;
			dataend = nullptr;
		}
		else
		{
			data = m_ptr;
			dataend = m_ptr + imgByteSize;
		}
	}
	void Mat::AddRefCount()
	{
		mx.lock();
		if (dataend)
			((int*)dataend)[0]++;
		mx.unlock();
	}
	void Mat::SubRefCount()
	{
		mx.lock();
		if(dataend)
			((int*)dataend)[0]--;
		mx.unlock();
	}

	Mat Mat::Zeros(int rows, int cols, PixelForm pt)
	{
		Mat res(rows, cols, pt);
		if(res.data)
			memset(res.data, 0, res.imgByteSize);
		return res;
	}

	Mat Mat::Ones(int rows, int cols, PixelForm pt)
	{
		Mat res(rows, cols, pt);
		if (res.data)
		{
			
		}
		return res;
	}

	Mat Mat::Eyes(int rows, int cols, PixelForm pt)
	{
		return Mat();
	}

	Mat Mat::Random(int rows, int cols,double min, double max)
	{
		Mat temp(rows, cols, PixelForm::F64);
		int64_t pNum = (int64_t)rows * cols;
		double* pData = (double*)temp.data;
		for (int i = 0; i < pNum; ++i)
		{
			double ratio = (double)rand() / (double)RAND_MAX;
			pData[i] = (max - min) * ratio + min;
		}
		return temp;
	}
}
//友元函数
namespace IMat {

	Mat operator+(const Mat& m1, const Mat& m2)
	{	
		Mat res = const_cast<Mat*>(&m1)->Clone();
		if (IsSameSize(m1,m2) == false)
			return res;
		uint64_t count = (uint64_t)m1.rows*m1.cols*m1.channels;
		double tormin, tormax;
		bool isFloat;
		PixelForm m_Format = m1.m_Format;
		GetTorMinMax(m_Format, tormin, tormax, isFloat);
		if (GetPtrType(m_Format) == PtrType::S8)
			IM_Add((char*)res.data, (char*)m2.data, count, tormin, tormax);
		else if (GetPtrType(m_Format) == PtrType::U8)
			IM_Add((uint8_t*)res.data, (uint8_t*)m2.data, count, tormin, tormax);
		else if (GetPtrType(m_Format) == PtrType::S16)
			IM_Add((short*)res.data, (short*)m2.data, count, tormin, tormax);
		else if (GetPtrType(m_Format) == PtrType::U16)
			IM_Add((uint16_t*)res.data, (uint16_t*)m2.data, count, tormin, tormax);
		else if (GetPtrType(m_Format) == PtrType::S32)
			IM_Add((int*)res.data, (int*)m2.data, count, tormin, tormax);
		else if (GetPtrType(m_Format) == PtrType::U32)
			IM_Add((uint32_t*)res.data, (uint32_t*)m2.data, count, tormin, tormax);
		else if (GetPtrType(m_Format) == PtrType::S64)
			IM_Add((int64_t*)res.data, (int64_t*)m2.data, count, tormin, tormax);
		else if (GetPtrType(m_Format) == PtrType::U64)
			IM_Add((uint64_t*)res.data, (uint64_t*)m2.data, count, tormin, tormax);
		else if (GetPtrType(m_Format) == PtrType::F32)
			IM_Add((float*)res.data, (float*)m2.data, count, tormin, tormax);
		else if (GetPtrType(m_Format) == PtrType::F64)
			IM_Add((double*)res.data, (double*)m2.data, count, tormin, tormax);
		return res;
		
	}
	Mat operator-(const Mat& m1, const Mat& m2)
	{
		Mat res = const_cast<Mat*>(&m1)->Clone();
		if (IsSameSize(m1, m2) == false)
			return res;
		uint64_t count =(uint64_t)(m1.rows) * m1.cols * m1.channels;
		double tormin, tormax;
		bool isFloat;
		PixelForm m_Format = m1.m_Format;
		GetTorMinMax(m_Format, tormin, tormax, isFloat);
		if (GetPtrType(m_Format) == PtrType::S8)
			IM_Sub((char*)res.data, (char*)m2.data, count, tormin, tormax);
		else if (GetPtrType(m_Format) == PtrType::U8)
			IM_Sub((uint8_t*)res.data, (uint8_t*)m2.data, count, tormin, tormax);
		else if (GetPtrType(m_Format) == PtrType::S16)
			IM_Sub((short*)res.data, (short*)m2.data, count, tormin, tormax);
		else if (GetPtrType(m_Format) == PtrType::U16)
			IM_Sub((uint16_t*)res.data, (uint16_t*)m2.data, count, tormin, tormax);
		else if (GetPtrType(m_Format) == PtrType::S32)
			IM_Sub((int*)res.data, (int*)m2.data, count, tormin, tormax);
		else if (GetPtrType(m_Format) == PtrType::U32)
			IM_Sub((uint32_t*)res.data, (uint32_t*)m2.data, count, tormin, tormax);
		else if (GetPtrType(m_Format) == PtrType::S64)
			IM_Sub((int64_t*)res.data, (int64_t*)m2.data, count, tormin, tormax);
		else if (GetPtrType(m_Format) == PtrType::U64)
			IM_Sub((uint64_t*)res.data, (uint64_t*)m2.data, count, tormin, tormax);
		else if (GetPtrType(m_Format) == PtrType::F32)
			IM_Sub((float*)res.data, (float*)m2.data, count, tormin, tormax);
		else if (GetPtrType(m_Format) == PtrType::F64)
			IM_Sub((double*)res.data, (double*)m2.data, count, tormin, tormax);
		return res;
	}

	Mat operator*(const Mat& m1, const Mat& m2)
	{
		if (m1.cols != m2.rows || m1.m_Format != m2.m_Format || m1.channels != 1)
		{
			return Mat();
		}
		PixelForm m_Format = m1.m_Format;
		Mat m3(m1.rows, m2.cols, PixelForm::F64);
		if (GetPtrType(m_Format) == PtrType::S8)
			IM_MatMul((char*)m1.data, (char*)m2.data, (double*)m3.data, m1.cols,m1.rows,m2.cols,m2.rows);
		else if (GetPtrType(m_Format) == PtrType::U8)
			IM_MatMul((uint8_t*)m1.data, (uint8_t*)m2.data, (double*)m3.data, m1.cols, m1.rows, m2.cols, m2.rows);
		else if (GetPtrType(m_Format) == PtrType::S16)
			IM_MatMul((short*)m1.data, (short*)m2.data, (double*)m3.data, m1.cols, m1.rows, m2.cols, m2.rows);
		else if (GetPtrType(m_Format) == PtrType::U16)
			IM_MatMul((uint16_t*)m1.data, (uint16_t*)m2.data, (double*)m3.data, m1.cols, m1.rows, m2.cols, m2.rows);  
		else if (GetPtrType(m_Format) == PtrType::S32)
			IM_MatMul((int*)m1.data, (int*)m2.data, (double*)m3.data, m1.cols, m1.rows, m2.cols, m2.rows);
		else if (GetPtrType(m_Format) == PtrType::U32)
			IM_MatMul((uint32_t*)m1.data, (uint32_t*)m2.data, (double*)m3.data, m1.cols, m1.rows, m2.cols, m2.rows);
		else if (GetPtrType(m_Format) == PtrType::S64)
			IM_MatMul((int64_t*)m1.data, (int64_t*)m2.data, (double*)m3.data, m1.cols, m1.rows, m2.cols, m2.rows);
		else if (GetPtrType(m_Format) == PtrType::U64)
			IM_MatMul((uint64_t*)m1.data, (uint64_t*)m2.data, (double*)m3.data, m1.cols, m1.rows, m2.cols, m2.rows);
		else if (GetPtrType(m_Format) == PtrType::F32)
			IM_MatMul((float*)m1.data, (float*)m2.data, (double*)m3.data, m1.cols, m1.rows, m2.cols, m2.rows);
		else if (GetPtrType(m_Format) == PtrType::F64)
			IM_MatMul((double*)m1.data, (double*)m2.data, (double*)m3.data, m1.cols, m1.rows, m2.cols, m2.rows);
		return m3;
	}
	//点积
	Mat DotMul(const Mat& m1, const Mat& m2)
	{
		return Mat();
	}

	double DotProduct(const Mat& m1, const Mat& m2)
	{
		if (m1.data == nullptr)
		{
			PrintInfo("DotProduct:m1 is null");
			return 0.0;
		}
		if (m2.data == nullptr)
		{
			PrintInfo("DotProduct:m2 is null");
			return 0.0;
		}
		if (IsSameSize(m1, m2) == false)
		{
			PrintInfo("DotProduct:m1 and m2 have different size");
			return 0.0;
		}

		uint64_t count = (uint64_t)m1.rows*m1.cols;
		if (m1.m_Format == PixelForm::S8)
		{
			if (m2.m_Format == PixelForm::S8)
				return IM_DotProduct((char*)m1.data, (char*)m2.data, count);
			if (m2.m_Format == PixelForm::U8)
				return IM_DotProduct((char*)m1.data, (uint8_t*)m2.data, count);
			if (m2.m_Format == PixelForm::S16)
				return IM_DotProduct((char*)m1.data, (short*)m2.data, count);
			if (m2.m_Format == PixelForm::U16)
				return IM_DotProduct((char*)m1.data, (uint16_t*)m2.data, count);
			if (m2.m_Format == PixelForm::S32)
				return IM_DotProduct((char*)m1.data, (int*)m2.data, count);
			if (m2.m_Format == PixelForm::U32)
				return IM_DotProduct((char*)m1.data, (uint32_t*)m2.data, count);
			if (m2.m_Format == PixelForm::S64)
				return IM_DotProduct((char*)m1.data, (int64_t*)m2.data, count);
			if (m2.m_Format == PixelForm::U64)
				return IM_DotProduct((char*)m1.data, (uint64_t*)m2.data, count);
			if (m2.m_Format == PixelForm::F32)
				return IM_DotProduct((char*)m1.data, (float*)m2.data, count);
			if (m2.m_Format == PixelForm::F64)
				return IM_DotProduct((char*)m1.data, (double*)m2.data, count);
		}
		else if (m1.m_Format == PixelForm::U8)
		{
			if (m2.m_Format == PixelForm::S8)
				return IM_DotProduct((uint8_t*)m1.data, (char*)m2.data, count);
			if (m2.m_Format == PixelForm::U8)
				return IM_DotProduct((uint8_t*)m1.data, (uint8_t*)m2.data, count);
			if (m2.m_Format == PixelForm::S16)
				return IM_DotProduct((uint8_t*)m1.data, (short*)m2.data, count);
			if (m2.m_Format == PixelForm::U16)
				return IM_DotProduct((uint8_t*)m1.data, (uint16_t*)m2.data, count);
			if (m2.m_Format == PixelForm::S32)
				return IM_DotProduct((uint8_t*)m1.data, (int*)m2.data, count);
			if (m2.m_Format == PixelForm::U32)
				return IM_DotProduct((uint8_t*)m1.data, (uint32_t*)m2.data, count);
			if (m2.m_Format == PixelForm::S64)
				return IM_DotProduct((uint8_t*)m1.data, (int64_t*)m2.data, count);
			if (m2.m_Format == PixelForm::U64)
				return IM_DotProduct((uint8_t*)m1.data, (uint64_t*)m2.data, count);
			if (m2.m_Format == PixelForm::F32)
				return IM_DotProduct((uint8_t*)m1.data, (float*)m2.data, count);
			if (m2.m_Format == PixelForm::F64)
				return IM_DotProduct((uint8_t*)m1.data, (double*)m2.data, count);
		}
		else if (m1.m_Format == PixelForm::S16)
		{
			if (m2.m_Format == PixelForm::S8)
				return IM_DotProduct((short*)m1.data, (char*)m2.data, count);
			if (m2.m_Format == PixelForm::U8)
				return IM_DotProduct((short*)m1.data, (uint8_t*)m2.data, count);
			if (m2.m_Format == PixelForm::S16)
				return IM_DotProduct((short*)m1.data, (short*)m2.data, count);
			if (m2.m_Format == PixelForm::U16)
				return IM_DotProduct((short*)m1.data, (uint16_t*)m2.data, count);
			if (m2.m_Format == PixelForm::S32)
				return IM_DotProduct((short*)m1.data, (int*)m2.data, count);
			if (m2.m_Format == PixelForm::U32)
				return IM_DotProduct((short*)m1.data, (uint32_t*)m2.data, count);
			if (m2.m_Format == PixelForm::S64)
				return IM_DotProduct((short*)m1.data, (int64_t*)m2.data, count);
			if (m2.m_Format == PixelForm::U64)
				return IM_DotProduct((short*)m1.data, (uint64_t*)m2.data, count);
			if (m2.m_Format == PixelForm::F32)
				return IM_DotProduct((short*)m1.data, (float*)m2.data, count);
			if (m2.m_Format == PixelForm::F64)
				return IM_DotProduct((short*)m1.data, (double*)m2.data, count);
		}
		else if (m1.m_Format == PixelForm::U16)
		{
			if (m2.m_Format == PixelForm::S8)
				return IM_DotProduct((uint16_t*)m1.data, (char*)m2.data, count);
			if (m2.m_Format == PixelForm::U8)
				return IM_DotProduct((uint16_t*)m1.data, (uint8_t*)m2.data, count);
			if (m2.m_Format == PixelForm::S16)
				return IM_DotProduct((uint16_t*)m1.data, (short*)m2.data, count);
			if (m2.m_Format == PixelForm::U16)
				return IM_DotProduct((uint16_t*)m1.data, (uint16_t*)m2.data, count);
			if (m2.m_Format == PixelForm::S32)
				return IM_DotProduct((uint16_t*)m1.data, (int*)m2.data, count);
			if (m2.m_Format == PixelForm::U32)
				return IM_DotProduct((uint16_t*)m1.data, (uint32_t*)m2.data, count);
			if (m2.m_Format == PixelForm::S64)
				return IM_DotProduct((uint16_t*)m1.data, (int64_t*)m2.data, count);
			if (m2.m_Format == PixelForm::U64)
				return IM_DotProduct((uint16_t*)m1.data, (uint64_t*)m2.data, count);
			if (m2.m_Format == PixelForm::F32)
				return IM_DotProduct((uint16_t*)m1.data, (float*)m2.data, count);
			if (m2.m_Format == PixelForm::F64)
				return IM_DotProduct((uint16_t*)m1.data, (double*)m2.data, count);
		}
		else if (m1.m_Format == PixelForm::S32)
		{
			if (m2.m_Format == PixelForm::S8)
				return IM_DotProduct((int*)m1.data, (char*)m2.data, count);
			if (m2.m_Format == PixelForm::U8)
				return IM_DotProduct((int*)m1.data, (uint8_t*)m2.data, count);
			if (m2.m_Format == PixelForm::S16)
				return IM_DotProduct((int*)m1.data, (short*)m2.data, count);
			if (m2.m_Format == PixelForm::U16)
				return IM_DotProduct((int*)m1.data, (uint16_t*)m2.data, count);
			if (m2.m_Format == PixelForm::S32)
				return IM_DotProduct((int*)m1.data, (int*)m2.data, count);
			if (m2.m_Format == PixelForm::U32)
				return IM_DotProduct((int*)m1.data, (uint32_t*)m2.data, count);
			if (m2.m_Format == PixelForm::S64)
				return IM_DotProduct((int*)m1.data, (int64_t*)m2.data, count);
			if (m2.m_Format == PixelForm::U64)
				return IM_DotProduct((int*)m1.data, (uint64_t*)m2.data, count);
			if (m2.m_Format == PixelForm::F32)
				return IM_DotProduct((int*)m1.data, (float*)m2.data, count);
			if (m2.m_Format == PixelForm::F64)
				return IM_DotProduct((int*)m1.data, (double*)m2.data, count);
		}
		else if (m1.m_Format == PixelForm::U32)
		{
			if (m2.m_Format == PixelForm::S8)
				return IM_DotProduct((uint32_t*)m1.data, (char*)m2.data, count);
			if (m2.m_Format == PixelForm::U8)
				return IM_DotProduct((uint32_t*)m1.data, (uint8_t*)m2.data, count);
			if (m2.m_Format == PixelForm::S16)
				return IM_DotProduct((uint32_t*)m1.data, (short*)m2.data, count);
			if (m2.m_Format == PixelForm::U16)
				return IM_DotProduct((uint32_t*)m1.data, (uint16_t*)m2.data, count);
			if (m2.m_Format == PixelForm::S32)
				return IM_DotProduct((uint32_t*)m1.data, (int*)m2.data, count);
			if (m2.m_Format == PixelForm::U32)
				return IM_DotProduct((uint32_t*)m1.data, (uint32_t*)m2.data, count);
			if (m2.m_Format == PixelForm::S64)
				return IM_DotProduct((uint32_t*)m1.data, (int64_t*)m2.data, count);
			if (m2.m_Format == PixelForm::U64)
				return IM_DotProduct((uint32_t*)m1.data, (uint64_t*)m2.data, count);
			if (m2.m_Format == PixelForm::F32)
				return IM_DotProduct((uint32_t*)m1.data, (float*)m2.data, count);
			if (m2.m_Format == PixelForm::F64)
				return IM_DotProduct((uint32_t*)m1.data, (double*)m2.data, count);
		}
		else if (m1.m_Format == PixelForm::F32)
		{
			if (m2.m_Format == PixelForm::S8)
				return IM_DotProduct((float*)m1.data, (char*)m2.data, count);
			if (m2.m_Format == PixelForm::U8)
				return IM_DotProduct((float*)m1.data, (uint8_t*)m2.data, count);
			if (m2.m_Format == PixelForm::S16)
				return IM_DotProduct((float*)m1.data, (short*)m2.data, count);
			if (m2.m_Format == PixelForm::U16)
				return IM_DotProduct((float*)m1.data, (uint16_t*)m2.data, count);
			if (m2.m_Format == PixelForm::S32)
				return IM_DotProduct((float*)m1.data, (int*)m2.data, count);
			if (m2.m_Format == PixelForm::U32)
				return IM_DotProduct((float*)m1.data, (uint32_t*)m2.data, count);
			if (m2.m_Format == PixelForm::S64)
				return IM_DotProduct((float*)m1.data, (int64_t*)m2.data, count);
			if (m2.m_Format == PixelForm::U64)
				return IM_DotProduct((float*)m1.data, (uint64_t*)m2.data, count);
			if (m2.m_Format == PixelForm::F32)
				return IM_DotProduct((float*)m1.data, (float*)m2.data, count);
			if (m2.m_Format == PixelForm::F64)
				return IM_DotProduct((float*)m1.data, (double*)m2.data, count);
		}
		else if (m1.m_Format == PixelForm::F64)
		{
			if (m2.m_Format == PixelForm::S8)
				return IM_DotProduct((double*)m1.data, (char*)m2.data, count);
			if (m2.m_Format == PixelForm::U8)
				return IM_DotProduct((double*)m1.data, (uint8_t*)m2.data, count);
			if (m2.m_Format == PixelForm::S16)
				return IM_DotProduct((double*)m1.data, (short*)m2.data, count);
			if (m2.m_Format == PixelForm::U16)
				return IM_DotProduct((double*)m1.data, (uint16_t*)m2.data, count);
			if (m2.m_Format == PixelForm::S32)
				return IM_DotProduct((double*)m1.data, (int*)m2.data, count);
			if (m2.m_Format == PixelForm::U32)
				return IM_DotProduct((double*)m1.data, (uint32_t*)m2.data, count);
			if (m2.m_Format == PixelForm::S64)
				return IM_DotProduct((double*)m1.data, (int64_t*)m2.data, count);
			if (m2.m_Format == PixelForm::U64)
				return IM_DotProduct((double*)m1.data, (uint64_t*)m2.data, count);
			if (m2.m_Format == PixelForm::F32)
				return IM_DotProduct((double*)m1.data, (float*)m2.data, count);
			if (m2.m_Format == PixelForm::F64)
				return IM_DotProduct((double*)m1.data, (double*)m2.data, count);
		}
		return 0.0;
	}

	Mat Inverse(const Mat& m)
	{
		return Mat();
	}

	bool IsSameSize(const Mat& m1, const Mat& m2)
	{
		if (m1.rows == m2.rows && m1.cols == m2.cols && m1.m_Format == m2.m_Format)
			return true;
		return false;
	}
};