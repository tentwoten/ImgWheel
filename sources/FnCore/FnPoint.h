#ifndef _FNPOINT_H_
#define _FNPOINT_H_

#include "FnGlobalEnum.h"
#include "FnGlobalHead.h"
#include <algorithm>
#include <iostream>


namespace IMat
{
	class FNNOEXPORT FnPoint2i
	{
	public:
		int x;
		int y;
	public:
		FnPoint2i();

		FnPoint2i(int _x, int _y);

		FnPoint2i(const FnPoint2i& other);

		void Set(int _x, int _y);

		FnPoint2i& operator=(const FnPoint2i& other);

		FnPoint2i& operator+(const FnPoint2i& other);

		FnPoint2i& operator-(const FnPoint2i& other);
	};

	class FNNOEXPORT FnPoint2f
	{
	public:
		double x;
		double y;
	public:
		FnPoint2f() :x(0.0), y(0.0) {}
		FnPoint2f(double _x, double _y) :x(_x), y(_y) {}
		FnPoint2f& operator=(const FnPoint2f& other)
		{
			if (&other == this)
				return *this;
			x = other.x;
			y = other.y;
			return *this;
		}

		friend FnPoint2f operator+(const FnPoint2f& p1, const FnPoint2f& p2)
		{
			return FnPoint2f(p1.x + p2.x, p1.y + p2.y);
		}

		friend FnPoint2f operator-(const FnPoint2f& p1, const FnPoint2f& p2)
		{
			return FnPoint2f(p1.x - p2.x, p1.y - p2.y);
		}

		friend double DotMul(const FnPoint2f& p1, const FnPoint2f& p2)
		{
			return p1.x * p2.x + p1.y * p2.y;
		}

		friend double DistanceP2P_2D(const FnPoint2f& p1, const FnPoint2f& p2)
		{
			return sqrt((double)(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2)));
		}
	};

	class FNNOEXPORT FnPoint3f
	{
	public:
		double x;
		double y;
		double z;
	public:
		FnPoint3f()
		{
			x = 0;
			y = 0;
			z = 0;
		}

		FnPoint3f(double _x, double _y, double _z)
		{
			x = _x;
			y = _y;
			z = _z;
		}

		FnPoint3f(const FnPoint3f& other)
		{
			x = other.x;
			y = other.y;
			z = other.z;
		}

		void Set(double _x, double _y, double _z)
		{
			x = _x;
			y = _y;
			z = _z;
		}

		FnPoint3f& operator=(const FnPoint3f& other)
		{
			if (&other == this)
				return *this;
			x = other.x;
			y = other.y;
			z = other.z;
			return *this;
		}

		friend FnPoint3f operator+(const FnPoint3f& p1, const FnPoint3f& p2)
		{
			return FnPoint3f(p1.x + p2.x, p1.y + p2.y, p1.z + p2.z);
		}

		friend FnPoint3f operator-(const FnPoint3f& p1, const FnPoint3f& p2)
		{
			return FnPoint3f(p1.x - p2.x, p1.y - p2.y, p1.z - p2.z);
		}

		friend double DotMul(const FnPoint3f& p1, const FnPoint3f& p2)
		{
			return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
		}

		friend double DistanceP2P_3D(const FnPoint3f& p1, const FnPoint3f& p2)
		{
			return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2) + pow(p1.z - p2.z, 2));
		}
	};
};

#endif // !_FNPOINT_H_