
#ifndef _IMGMAT_HPP_
#define _IMGMAT_HPP_

#include <iostream>
#include <memory>
#include"FnGlobalEnum.h"
#include "FnGlobalHead.h"

namespace IMat  
{ 	
	class FNNOEXPORT Rect
	{
	public:
		int x;
		int y;
		int width;
		int height;
		Rect() { x = 0; y = 0; width = 0; height = 0; }
		Rect(int _x,int _y,int _width,int _height) { x = _x; y = _y; width = _width; height = _height; }
		Rect(const Rect& rect) { x = rect.x; y = rect.y; width = rect.width, height = rect.height; }
		Rect& operator=(const Rect& other)
		{
			if (this != &other)
			{
				x = other.x;
				y = other.y;
				width = other.width;
				height = other.height;
			}
			return *this;
		}
	};

	class FNNOEXPORT Vec3b
	{
	public:
		int x;
		int y;
		int z;
	};

	class FNNOEXPORT Vec4i
	{
	public:
		int x;
		int y;
		int z;
		int a;
	};

	class FNNOEXPORT Vec4l
	{
	public:
		Vec4l()
		{
			left  = 0;
			up    = 0;
			right = 0;
			down  = 0;
		}
		Vec4l(int64_t x0,int64_t x1,int64_t x2,int64_t x3)
		{
			left  = x0;
			up    = x1;
			right = x2;
			down  = x3;
		}
	public:
		union
		{
			struct
			{
				long long left;
				long long up;
				long long right;
				long long down;
			};
			struct
			{
				long long x;
				long long y;
				long long z;
				long long a;
			};
		};
	};

	class FNNOEXPORT Point2f
	{
	public:
		int x;
		int y;
		Point2f() {
			x = 0;
			y = 0;
		}
		Point2f(int _x, int _y)
		{
			x = _x;
			y = _y;
		}
		Point2f& operator=(const Point2f& other)
		{
			if (this == &other)
				return *this;
			x = other.x;
			y = other.y;
			return *this;
		}

		Point2f& operator+(const Point2f& other)
		{
			x += other.x;
			y += other.y;
			return *this;
		}

		Point2f& operator-(const Point2f& other)
		{
			x -= other.x;
			y -= other.y;
			return *this;
		}
	};
	

	//自定义智能指针，加锁
	 class FNNOEXPORT Mat
	{
		public:
			int64_t rows;
			int64_t cols;
			int channels;
			int m_PByteSize;
			int64_t step;
			uint64_t imgByteSize;
			uint8_t* data;
			uint8_t* dataend;
			PixelForm m_Format;

		public:
			Mat();
			Mat(int64_t h, int64_t w, PixelForm pt = PixelForm::U8);
			Mat(void* data, int64_t h, int64_t w, PixelForm pt);
			Mat(const Mat& other);
			~Mat();
			Mat& operator=(const Mat& other);   //light copy		
			//建立一个内存
			void Create(int64_t h, int64_t w, PixelForm pt);
			//解绑当前内存
			void Release();   
			//读取Raw文件
			bool ReadRawFile(const char* path, int h, int w, PixelForm pt, int byte_offset = 0);
			//
			void Transpose();	
			//
			inline double* operator[](const int& h) const
			{
				return ((double*)data) + (int64_t)cols * h;
			}
		public: 
			//深拷贝，克隆图片
			Mat  Clone();
			//判断图片是否相同
			bool IsSameSize(const Mat &other);
			//判断是否位float型图片
			bool IsFloat();
			//将图片转换位某种像素格式
			void ConvertTo(Mat& dst, PixelForm type);
		private:
			uint8_t* m_ptr;   //头+data  第一位是引用次数
			static uint64_t m_ByteOffset;
		private:
			//获取该图片的引用次数
			int GetRefCount();
			//刷新内部参数
			void CalAdvanceData();
			//刷新指针
			void RefreshPtr();
			//增加指针引用计数
			void AddRefCount();
			//减少指针引用计数
			void SubRefCount();
		public:
		template<typename T>
		inline T& Get(int x, int y, int c=0)
		{
			uint64_t index =  (int64_t)y * cols + x;
			T* res = (T*)(data + (index) * m_PByteSize);
			return *(res + c);
		}

		template<typename T>
		inline  void Set(T val,int x, int y, int c = 0)
		{
			int64_t index =(int64_t)y * cols + x;
			*(T*)(data + (index) * m_PByteSize)=val;
		}
	public:
		//矩阵减法
		FNNOEXPORT friend Mat operator+(const Mat& m1, const Mat& m2);
		//矩阵加法
		FNNOEXPORT friend Mat operator-(const Mat& m1, const Mat& m2);
		//矩阵乘法
		FNNOEXPORT friend Mat operator*(const Mat& m1, const Mat& m2);
		 //点乘
		FNNOEXPORT friend Mat DotMul(const Mat& m1, const Mat& m2);
		//点积
		FNNOEXPORT friend double DotProduct(const Mat& m1, const Mat& m2);
		//矩阵求逆
		FNNOEXPORT friend Mat Inverse(const Mat& m);
		//判断矩阵是否相同
		FNNOEXPORT friend bool IsSameSize(const Mat& m1, const Mat& m2);
		//
		Mat operator=(int8_t val);
		Mat operator=(uint8_t val);	
		Mat operator=(int16_t val);
		Mat operator=(uint16_t val);
		Mat operator=(int val);
		Mat operator=(uint32_t val);	
		Mat operator=(int64_t val);
		Mat operator=(uint64_t val);
		Mat operator=(float val);
		Mat operator=(double val);

	public:
		static Mat Zeros(int rows, int cols, PixelForm pt);
		static Mat Ones(int rows, int cols, PixelForm pt);
		static Mat Eyes(int rows, int cols, PixelForm pt);
		static Mat Random(int rows, int cols, double min, double max);
	};


};

#endif // !_IMGMAT_HPP_