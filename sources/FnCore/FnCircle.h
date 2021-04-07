// FnMath.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。
#ifndef _FNCIRCLE_H_
#define _FNCIRCLE_H_
#include "FnPoint.h"
//参数方程(x-a)^2+(y-b)^2=r^2
//一般式x^2+y^2+Dx+Ey+F=0  
class FNNOEXPORT FnCircle2D
{
public:
	IMat::FnPoint2f m_center;
	double m_rad;
	double m_D;
	double m_E;
	double m_F;
public:
	FnCircle2D();
	FnCircle2D(const IMat::FnPoint2f& center, double rad);
	FnCircle2D(double D, double E, double F);
	FnCircle2D(const FnCircle2D& other);
	FnCircle2D& operator=(const FnCircle2D& other);
	bool IsPointInArc(const IMat::FnPoint2f& pt);
	bool IsPointInCircle(const IMat::FnPoint2f& pt);
	void SetNormalParam(double D, double E, double F);
	void SetCaliParam(const IMat::FnPoint2f& center, double rad);
private:
	void RefreshCNormalFun(); //刷新一般方程
	void RefreshCaliFun();   //刷新参数方程

};

class FNNOEXPORT FnCircle3D
{
public:
	IMat::FnPoint3f m_center;
	double m_rad;
	double m_D;
	double m_E;
	double m_F;
	double m_G;
public:
	FnCircle3D();
	FnCircle3D(const IMat::FnPoint3f& center, double rad);
	FnCircle3D(double D, double E, double F, double G);
	FnCircle3D(const FnCircle3D& other);
	FnCircle3D& operator=(const FnCircle3D& other);
	bool IsPointInArc(const IMat::FnPoint3f& pt);
	bool IsPointInCircle(const IMat::FnPoint3f& pt);
	void SetNormalParam(double D, double E, double F, double G);
	void SetCaliParam(const IMat::FnPoint3f& center, double rad);
private:
	void RefreshCNormalFun(); //刷新一般方程
	void RefreshCaliFun();   //刷新参数方程

};

#endif // !_FNCIRCLE_H_
