// FnMath.cpp: 定义应用程序的入口点。
//

#include "FnCircle.h"
using namespace IMat;
FnCircle2D::FnCircle2D() :m_center(FnPoint2f()), m_rad(0.0), m_D(0.0), m_E(0.0), m_F(0.0)
{}

FnCircle2D::FnCircle2D(const FnPoint2f& center, double r) : m_center(center), m_rad(r)
{
	RefreshCNormalFun();
}

FnCircle2D::FnCircle2D(double D, double E, double F) : m_D(D), m_E(E), m_F(F)
{
	RefreshCaliFun();
}

FnCircle2D::FnCircle2D(const FnCircle2D& other)
{
	m_center = other.m_center;
	m_rad = other.m_rad;
	m_D = other.m_D;
	m_E = other.m_E;
	m_F = other.m_F;
}

FnCircle2D& FnCircle2D::operator=(const FnCircle2D& other)
{
	if (this == &other)
		return *this;
	m_center = other.m_center;
	m_rad = other.m_rad;
	m_D = other.m_D;
	m_E = other.m_E;
	m_F = other.m_F;
	return *this;
}

void FnCircle2D::SetNormalParam(double D, double E, double F)
{
	m_D = D;
	m_E = E;
	m_F = F;
	RefreshCaliFun();
}

void FnCircle2D::SetCaliParam(const IMat::FnPoint2f& center, double rad)
{
	m_center = center;
	m_rad = rad;
	RefreshCNormalFun();
}

bool FnCircle2D::IsPointInArc(const FnPoint2f& pt)
{
	double dis = DistanceP2P_2D(m_center, pt);
	return abs(dis - m_rad) < 1e-7;
}

bool FnCircle2D::IsPointInCircle(const IMat::FnPoint2f& pt)
{
	double dis = DistanceP2P_2D(m_center, pt);
	return (m_rad - dis) >= 1e-7;
}

void FnCircle2D::RefreshCNormalFun()
{
	m_D = -2.0 * m_center.x;
	m_E = -2.0 * m_center.y;
	m_F = pow(m_center.x, 2.0) + pow(m_center.y, 2.0) - pow(m_rad, 2.0);
}

void FnCircle2D::RefreshCaliFun()
{
	double Rad2 = pow(m_D, 2.0) + pow(m_E, 2.0) - 4.0 * m_F;
	if (Rad2 >= 0.0)
	{
		m_center.x = -m_D / 2.0;
		m_center.y = -m_E / 2.0;
		m_rad = sqrt(Rad2);
	}
	else
	{
		m_center.x = 0.0;
		m_center.y = 0.0;
		m_rad = 0.0;
	}
}

FnCircle3D::FnCircle3D() :m_center(FnPoint3f()), m_rad(0.0), m_D(0.0), m_E(0.0), m_F(0.0), m_G(0.0)
{}

FnCircle3D::FnCircle3D(const IMat::FnPoint3f& center, double rad)
{
	m_center = center;
	m_rad = rad;
	RefreshCNormalFun();
}

FnCircle3D::FnCircle3D(double D, double E, double F, double G)
{
	m_D = D;
	m_E = E;
	m_F = F;
	m_G = G;
	RefreshCaliFun();
}

FnCircle3D::FnCircle3D(const FnCircle3D& other)
{
	m_center = other.m_center;
	m_rad = other.m_rad;
	m_D = other.m_D;
	m_E = other.m_E;
	m_F = other.m_F;
	m_G = other.m_G;
}

FnCircle3D& FnCircle3D::operator=(const FnCircle3D& other)
{
	if (&other == this)
		return *this;
	m_center = other.m_center;
	m_rad = other.m_rad;
	m_D = other.m_D;
	m_E = other.m_E;
	m_F = other.m_F;
	m_G = other.m_G;
	return *this;
}

bool FnCircle3D::IsPointInArc(const IMat::FnPoint3f& pt)
{
	double dis = DistanceP2P_3D(m_center, pt);
	return dis < 1e-6;
}

bool FnCircle3D::IsPointInCircle(const IMat::FnPoint3f& pt)
{
	double dis = DistanceP2P_3D(m_center, pt);
	return (m_rad - dis) >= 1e-7;
}

void FnCircle3D::SetNormalParam(double D, double E, double F, double G)
{
	m_D = D;
	m_E = E;
	m_F = F;
	m_G = G;
	RefreshCaliFun();
}

void FnCircle3D::SetCaliParam(const IMat::FnPoint3f& center, double rad)
{
	m_center = center;
	m_rad = rad;
	RefreshCNormalFun();
}

void FnCircle3D::RefreshCNormalFun()
{
	m_D = -2.0 * m_center.x;
	m_E = -2.0 * m_center.y;
	m_F = -2.0 * m_center.z;
	m_G = pow(m_center.x, 2.0) + pow(m_center.y, 2.0) + pow(m_center.z, 2.0) - pow(m_rad, 2.0);
}

void FnCircle3D::RefreshCaliFun()
{
	double Rad2 = pow(m_D, 2.0) + pow(m_E, 2.0) + pow(m_F, 2.0) - 4.0 * m_G;
	if (Rad2 >= 0.0)
	{
		m_center.x = -m_D / 2.0;
		m_center.y = -m_E / 2.0;
		m_center.z = -m_F / 2.0;
		m_rad = sqrt(Rad2);
	}
	else
	{
		m_center.x = 0.0;
		m_center.y = 0.0;
		m_center.z = 0.0;
		m_rad = 0.0;
	}
}
