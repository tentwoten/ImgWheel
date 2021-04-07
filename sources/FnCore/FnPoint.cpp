#include "FnPoint.h"
using namespace IMat;
FnPoint2i::FnPoint2i()
{
	x = 0;
	y = 0;
}
FnPoint2i::FnPoint2i(int _x, int _y)
{
	x = _x;
	y = _y;
}

IMat::FnPoint2i::FnPoint2i(const FnPoint2i& other)
{
	x = other.x;
	y = other.y;
}

void IMat::FnPoint2i::Set(int _x, int _y)
{
	x = _x;
	y = _y;
}

FnPoint2i& FnPoint2i::operator=(const FnPoint2i& other)
{
	if (this == &other)
		return *this;
	x = other.x;
	y = other.y;
	return *this;
}

FnPoint2i& FnPoint2i::operator+(const FnPoint2i& other)
{
	x += other.x;
	y += other.y;
	return *this;
}

FnPoint2i& FnPoint2i::operator-(const FnPoint2i& other)
{
	x -= other.x;
	y -= other.y;
	return *this;
}