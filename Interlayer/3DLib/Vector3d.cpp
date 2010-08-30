// Vector3d.cpp: implementation of the CVector3d class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Vector3d.h"
#include "Vector3d.h"
#include "Matrix33.h"
#include "OneAxis.h"
#include "Plane.h"
#include "MMath.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//Implementation of class vector
CVector3D::CVector3D()
{
	m_x = 0;
	m_y = 0;
	m_z = 0;
	m_geomType = GEO_VECTOR;
}

CVector3D::CVector3D(double x, double y, double z)
{
	m_x = x;
	m_y = y;
	m_z = z;
	m_geomType = GEO_VECTOR;
}

CVector3D::CVector3D(const CVertex3D& P)
{
	m_x = P.GetX();
	m_y = P.GetY();
	m_z = P.GetZ();
	m_geomType = GEO_VECTOR;
}

CVector3D::CVector3D(const CVector3D& V1, const CVector3D& V2)
{
	CVector3D VC1, VC2, VC3;
	VC1 = V1;
	VC2 = V2;
	VC3 = VC2 - VC1;
	m_x = VC3.GetX();
	m_y = VC3.GetY();
	m_z = VC3.GetZ();
	m_geomType = GEO_VECTOR;
}

CVector3D::CVector3D(const CVertex3D& V1, const CVertex3D& V2)
{
	CVertex3D VC1, VC2;
	VC1 = V1;
	VC2 = V2;
	m_x = VC2.GetX() - VC1.GetX();
	m_y = VC2.GetY() - VC1.GetY();
	m_z = VC2.GetZ() - VC1.GetZ();
	m_geomType = GEO_VECTOR;
}

CVector3D::CVector3D(CVertex3D* pVertex1, CVertex3D* pVertex2)
{
	m_x = pVertex2->GetX() - pVertex1->GetX();
	m_y = pVertex2->GetY() - pVertex1->GetY();
	m_z = pVertex2->GetZ() - pVertex1->GetZ();
	m_geomType = GEO_VECTOR;
}

CVector3D::~CVector3D()
{
}

double CVector3D::Dot(const CVector3D& V1) const
{
	double result;
	result = V1.GetX() * GetX() + V1.GetY() * GetY() + V1.GetZ() * GetZ();
	return result;
}

double CVector3D::DotCross(const CVector3D& V1, const CVector3D& V2)
{
	CVector3D A = (*this), B = V1, C = V2;
	CVector3D cross = B.Crossed(C);
	double Res = A.Dot(cross);
	return Res;
}

void CVector3D::Cross(const CVector3D& V1)
{
	SetX(GetY() * V1.GetZ() - GetZ() * V1.GetY());
	SetY(GetZ() * V1.GetX() - GetX() * V1.GetZ());
	SetZ(GetX() * V1.GetY() - GetY() * V1.GetX());
}

CVector3D CVector3D::Crossed(const CVector3D& V1) const
{
	CVector3D result;
	result.SetX(GetY() * V1.GetZ() - GetZ() * V1.GetY());
	result.SetY(GetZ() * V1.GetX() - GetX() * V1.GetZ());
	result.SetZ(GetX() * V1.GetY() - GetY() * V1.GetX());
	return result;
}

void CVector3D::CrossCross(const CVector3D& V1, const CVector3D& V2)
{
	CVector3D A = (*this), B = V1, C = V2;
	CVector3D Res = B*((C.Dot(A))) - C*(A.Dot(B));
	(*this) = Res;
}

CVector3D CVector3D::CrossCrossed(const CVector3D& V1, const CVector3D& V2)
{
	CVector3D V = (*this);
	V.CrossCross(V1, V2);
	return V;
}

void CVector3D::Reverse()
{
	this->SetX(-GetX());
	this->SetY(-GetY());
	this->SetZ(-GetZ());
}

CVector3D CVector3D::Reversed()
{
	CVector3D result;
	result.Reverse();
	return result;
}

CVector3D CVector3D::operator +(const CVector3D& V1)
{
	CVector3D result;
	result.SetX(GetX() + V1.GetX());
	result.SetY(GetY() + V1.GetY());
	result.SetZ(GetZ() + V1.GetZ());
	return result;
}

CVector3D CVector3D::operator=(const CVector3D& V1)
{
	this->SetParam(V1);
	return (*this);
// 	CVector3D v;
// 	v.SetParam(V1);
// 	return v;
}

void CVector3D::operator +=(const CVector3D& V1)
{
	SetX(GetX() + V1.GetX());
	SetY(GetY() + V1.GetY());
	SetZ(GetZ() + V1.GetZ());
}

void CVector3D::operator +=(CVector3D* pVector)
{
	m_x += pVector->GetX();
	m_y += pVector->GetY();
	m_z += pVector->GetZ();
}


CVector3D CVector3D::operator -(const CVector3D& V1)
{
	CVector3D result;
	result.SetX(GetX() - V1.GetX());
	result.SetY(GetY() - V1.GetY());
	result.SetZ(GetZ() - V1.GetZ());
	return result;
}

void CVector3D::operator -=(const CVector3D& V1)
{
	SetX(GetX() - V1.GetX());
	SetY(GetY() - V1.GetY());
	SetZ(GetZ() - V1.GetZ());
}

CVector3D CVector3D::operator *(const double& scalar)
{
	CVector3D result;
	result.SetX(GetX() * scalar);
	result.SetY(GetY() * scalar);
	result.SetZ(GetZ() * scalar);
	return result;
}

CVector3D CVector3D::operator *(const CMatrix33& M)
{
	CVector3D V;
	V.SetX(M(0, 0) * GetX() + M(0, 1) * GetY() + M(0, 2) * GetZ());
	V.SetY(M(1, 0) * GetX() + M(1, 1) * GetY() + M(1, 2) * GetZ());
	V.SetZ(M(2, 0) * GetX() + M(2, 1) * GetY() + M(2, 2) * GetZ());
	return V;
}

void CVector3D::operator *=(const double& scalar)
{
	SetX(GetX() * scalar);
	SetY(GetY() * scalar);
	SetZ(GetZ() * scalar);
}

void CVector3D::operator *=(const CMatrix33& M)
{
	SetX(M(0, 0) * GetX() + M(0, 1) * GetY() + M(0, 2) * GetZ());
	SetY(M(1, 0) * GetX() + M(1, 1) * GetY() + M(1, 2) * GetZ());
	SetZ(M(2, 0) * GetX() + M(2, 1) * GetY() + M(2, 2) * GetZ());
}

CVector3D CVector3D::operator /(const double& scalar)
{
	CVector3D result;
	if (!IsNull())
	{
		result.SetX(GetX() / scalar);
		result.SetY(GetY() / scalar);
		result.SetZ(GetZ() / scalar);
	}
	else
	{
		result.SetX(0);
		result.SetY(0);
		result.SetZ(0);
	}
	return result;
}

void CVector3D::operator /=(const double& scalar)
{
	if (!IsNull())
	{
		SetX(GetX() / scalar);
		SetY(GetY() / scalar);
		SetZ(GetZ() / scalar);
	}
	else
	{
		SetX(0);
		SetY(0);
		SetZ(0);
	}
}

CVector3D CVector3D::operator ^(const CVector3D& V1)
{
	return this->Crossed(V1);
}

void CVector3D::operator ^=(const CVector3D& V1)
{
	this->Cross(V1);
}

bool CVector3D::operator ==(const CVector3D& V) const
{
	return(m_x == V.GetX() && m_y == V.GetY() && m_z == V.GetZ());
}

bool CVector3D::operator !=(const CVector3D& V) const
{
	return(m_x != V.GetX() || m_y != V.GetY() || m_z != V.GetZ());
}


CVector3D::operator CVertex3D()
{
	CVertex3D P(m_x, m_y, m_z);
	return P;
}

double CVector3D::Magnitude() const
{
	if (!IsNull())
	{
		double result;
		result = sqrt(pow(GetX(), 2) + pow(GetY(), 2) + pow(GetZ(), 2));
		return result;
	}
	else
	{
		return 0;
	}
}

double CVector3D::SqrMagnitude() const
{
	if (!IsNull())
	{
		double result;
		result = pow(GetX(), 2) + pow(GetY(), 2) + pow(GetZ(), 2);
		return result;
	}
	else
		return 0;
}

double CVector3D::CrossMagnitude(const CVector3D& V)
{
	if (!IsNull() && !V.IsNull())
	{
		CVector3D V1 = (*this);
		CVector3D V2 = V;
		V1.Cross(V2);
		double result;
		result = sqrt(pow(V1.GetX(), 2) + pow(V1.GetY(), 2) + pow(V1.GetZ(), 2));
		return result;
	}
	else
	{
		return 0;
	}
}

double CVector3D::CrossSqrMagnitude(const CVector3D& V)
{
	if (!IsNull() && !V.IsNull())
	{
		CVector3D V1 = (*this);
		CVector3D V2 = V;
		V1.Cross(V2);
		double result;
		result = pow(V1.GetX(), 2) + pow(V1.GetY(), 2) + pow(V1.GetZ(), 2);
		return result;
	}
	else
	{
		return 0;
	}
}

double CVector3D::Modulus()
{
	return sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
}

CVector3D CVector3D::Unit()
{
	CVector3D result;
	double x, y, z;
	if (!IsNull())
	{
		x = GetX() / Magnitude();
		y = GetY() / Magnitude();
		z = GetZ() / Magnitude();
		result.SetX(x);
		result.SetY(y);
		result.SetZ(z);
		return result;
	}
	else
		return *this;
}

CVertex3D CVector3D::Point()
{
	CVertex3D aPnt;
	aPnt.SetParam(GetX(), GetY(), GetZ());
	return aPnt;
}

double CVector3D::Angle(const CVector3D& Other) const
{
	double theta, a, b, dot;
	CVector3D tempV = Other;
	if (!this->IsNull() && !tempV.IsNull())
	{
		dot = this->Dot(Other);
		a = this->Magnitude();
		b = tempV.Magnitude();
		theta = acos(dot / (a * b));
		return theta;
	}
	else
		return 0;
}

double CVector3D::Angle(const CVector3D& Other, const CVector3D& Dir) const
{
	double theta;
	CVector3D tempV = Other;
	CVector3D D = Dir;
	CVector3D N = this->Crossed(tempV);
	if (!this->IsNull() && !tempV.IsNull())
	{
		theta = Angle(Other);

		if (IsOpposite(tempV))
		{
			return PI;
		}
		else
		{
			if (N.Angle(D) <= 0.0001)
				return theta;
			else
				return -theta;
		}
	}
	else
		return 0;
}

bool CVector3D::IsNull() const
{
	if (m_x == 0 && m_y == 0 && m_z == 0)
		return true;
	else
		return false;
}

bool CVector3D::IsParallel(const CVector3D& V) const
{
	/*double ang=0;
	CVector3D tempV = V;
	ang = this->Angle(tempV);
	if(ang==0 || ang==2*PI)
		return true;
	else
		return false;*/
	CVector3D N = this->Crossed(V);
	return (N.IsNull());
}

bool CVector3D::IsOpposite(const CVector3D& V) const
{
	double ang = 0;
	CVector3D tempV = V;
	ang = this->Angle(tempV);
	if (fabs(ang - PI) <= 0.0001)
		return true;
	else
		return false;
}

bool CVector3D::IsNormal(const CVector3D& V) const
{
	double ang = 0;
	CVector3D tempV = V;
	ang = this->Angle(tempV);
	if (fabs(ang - PI / 2) <= 0.0001)
		return true;
	else
		return false;
}

void CVector3D::Normalize()
{
	double x, y, z;
	if (!IsNull())
	{
		x = GetX() / Magnitude();
		y = GetY() / Magnitude();
		z = GetZ() / Magnitude();
		SetX(x);
		SetY(y);
		SetZ(z);
	}
	else
		return;
}

void CVector3D::Translate(const COneAxis& Ax, const double& amt)
{
	CGeometry::Translate(Ax, amt);
}

void CVector3D::Translate(double dx, double dy, double dz)
{
	CVertex3D P = (*this);
	P.Translate(dx, dy, dz);
	CVector3D V = P;
	(*this) = V;
}

void CVector3D::Translate(const CVector3D& dV)
{
	CVertex3D P = (*this);
	P.Translate(dV);
	CVector3D V = P;
	(*this) = V;
}

void CVector3D::Translate(const CVertex3D& P1, const CVertex3D& P2)
{
	CVertex3D P = (*this);
	P.Translate(P1, P2);
	CVector3D V = P;
	(*this) = V;
}

void CVector3D::Rotate(const COneAxis& Ax, double ang)
{
	COneAxis ax = Ax;
	CVector3D P1 = ax.GetOrigin();
	CVector3D V = ax.GetDirection();
	V.Normalize();
	CMatrix33 M; CVector3D PV(*this);
	M.SetRotation(V, ang);
	CVector3D RV = M*(PV - P1);
	RV = RV + P1;
	*this = RV;
}

void CVector3D::Scale(const CVertex3D& P1, double fact)
{
	CVertex3D P = (*this);
	P.Scale(P1, fact);
	CVector3D V = P;
	(*this) = V;
}

void CVector3D::Mirror(const CVertex3D& P1)
{
	CVertex3D P = (*this);
	P.Mirror(P1);
	CVector3D V = P;
	(*this) = V;
}

void CVector3D::Mirror(const COneAxis& Ax)
{
	CVertex3D P = (*this);
	P.Mirror(Ax);
	CVector3D V = P;
	(*this) = V;
}

void CVector3D::Mirror(const CPlane& Pln)
{
	CVertex3D P = (*this);
	P.Mirror(Pln);
	CVector3D V = P;
	(*this) = V;
}

CVector3D CVector3D::Origin()
{
	return CVector3D(0, 0, 0);
}


//********************************************
// Inner
//********************************************
void CVector3D::Inner(CVector3D& v)
{
	double x = m_y* v.GetZ() - m_z* v.GetY();
	double y = m_z* v.GetX() - m_x* v.GetZ();
	double z = m_x* v.GetY() - m_y* v.GetX();
	SetParam(x, y, z);
}

//********************************************
// Inner
//********************************************
CVector3D CVector3D::Inner(CVector3D* u, CVector3D* v)
{
	// w = u ^ v
	CVector3D w;
	w.SetParam(u->GetY() * v->GetZ() - u->GetZ() * v->GetY(), u->GetZ() * v->GetX() - u->GetX() * v->GetZ(), u->GetX() * v->GetY() - u->GetY() * v->GetX());

	return w;
}

CVector3D CVector3D::Inner(const CVector3D& u, const CVector3D& v)
{
	// w = u ^ v
	CVector3D w;
	w.SetParam(u.GetY() * v.GetZ() - u.GetZ() * v.GetY(), u.GetZ() * v.GetX() - u.GetX() * v.GetZ(), u.GetX() * v.GetY() - u.GetY() * v.GetX());

	return w;
}



//////////////////////////////////////////////
//////////////////////////////////////////////
// PROCESSING
//////////////////////////////////////////////
//////////////////////////////////////////////

//********************************************
// Normalize
//********************************************
void CVector3D::NormalizeL2(void)
{
	double norm = GetNormL2();
	if (norm != 0.0f)
	{
		m_x = (m_x / norm);
		m_y = (m_y / norm);
		m_z = (m_z / norm);
	}
	//TRACE("norm : %g\n",GetNormL2());
}

//********************************************
// Normalize
//********************************************
void CVector3D::NormalizeL2(double v)
{
	double norm = GetNormL2();
	if (norm != 0.0f)
	{
		m_x *= (v / norm);
		m_y *= (v / norm);
		m_z *= (v / norm);
	}
	//TRACE("norm : %g (wanted : %g)\n",GetNormL2(),value);
}

//********************************************
// GetNormSquare
//********************************************
double CVector3D::GetNormL2Square(void) const
{
	return (m_x * m_x + m_y * m_y + m_z * m_z);
}

//********************************************
// GetNorm
//********************************************
double CVector3D::GetNormL2(void) const
{
	return sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
}

//********************************************
// Collinear
//********************************************
int CVector3D::Collinear(CVector3D* pVector)
{
	double x = pVector->GetX() / m_x;
	double y = pVector->GetY() / m_y;
	double z = pVector->GetZ() / m_z;
	return ((x == y) && (y == z));
}

//********************************************
// Collinear
//********************************************
int CVector3D::Collinear(CVector3D& v)
{
	double x = v.GetX() / m_x;
	double y = v.GetY() / m_y;
	double z = v.GetZ() / m_z;
	return ((x == y) && (y == z));
}

