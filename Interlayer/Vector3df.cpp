// Vector3d.cpp: implementation of the CVector3DF class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Vector3df.h"
#include "3DLib/Vertex3D.h"
#include "3DLib/Matrix33.h"
#include "3DLib/OneAxis.h"
#include "3DLib/Plane.h"
#include "3DLib/MMath.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//Implementation of class vector
CVector3DF::CVector3DF()
{
	x = 0;
	y = 0;
	z = 0;
}

CVector3DF::CVector3DF(double x, double y, double z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

CVector3DF::CVector3DF(const CVertex3D& P)
{
	this->x = P.GetX();
	this->y = P.GetY();
	this->z = P.GetZ();
}

CVector3DF::CVector3DF(const CVector3DF& V1, const CVector3DF& V2)
{
	CVector3DF VC1, VC2, VC3;
	VC1 = V1;
	VC2 = V2;
	VC3 = VC2 - VC1;
	this->x = VC3.GetX();
	this->y = VC3.GetY();
	this->z = VC3.GetZ();
}

CVector3DF::CVector3DF(const CVertex3D& V1, const CVertex3D& V2)
{
	CVertex3D VC1, VC2;
	VC1 = V1;
	VC2 = V2;
	this->x = VC2.GetX() - VC1.GetX();
	this->y = VC2.GetY() - VC1.GetY();
	this->z = VC2.GetZ() - VC1.GetZ();
}

CVector3DF::CVector3DF(CVertex3D* pVertex1, CVertex3D* pVertex2)
{
	this->x = pVertex2->GetX() - pVertex1->GetX();
	this->y = pVertex2->GetY() - pVertex1->GetY();
	this->z = pVertex2->GetZ() - pVertex1->GetZ();
}

CVector3DF::~CVector3DF()
{
}

double CVector3DF::Dot(const CVector3DF& V1) const
{
	double result;
	result = V1.GetX() * GetX() + V1.GetY() * GetY() + V1.GetZ() * GetZ();
	return result;
}

double CVector3DF::DotCross(const CVector3DF& V1, const CVector3DF& V2)
{
	CVector3DF A = (*this), B = V1, C = V2;
	CVector3DF cross = B.Crossed(C);
	double Res = A.Dot(cross);
	return Res;
}

void CVector3DF::Cross(const CVector3DF& V1)
{
	SetX(GetY() * V1.GetZ() - GetZ() * V1.GetY());
	SetY(GetZ() * V1.GetX() - GetX() * V1.GetZ());
	SetZ(GetX() * V1.GetY() - GetY() * V1.GetX());
}

CVector3DF CVector3DF::Crossed(const CVector3DF& V1) const
{
	CVector3DF result;
	result.SetX(GetY() * V1.GetZ() - GetZ() * V1.GetY());
	result.SetY(GetZ() * V1.GetX() - GetX() * V1.GetZ());
	result.SetZ(GetX() * V1.GetY() - GetY() * V1.GetX());
	return result;
}

void CVector3DF::CrossCross(const CVector3DF& V1, const CVector3DF& V2)
{
	CVector3DF A = (*this), B = V1, C = V2;
	CVector3DF Res = B*((C.Dot(A))) - C*(A.Dot(B));
	(*this) = Res;
}

CVector3DF CVector3DF::CrossCrossed(const CVector3DF& V1, const CVector3DF& V2)
{
	CVector3DF V = (*this);
	V.CrossCross(V1, V2);
	return V;
}

void CVector3DF::Reverse()
{
	this->SetX(-GetX());
	this->SetY(-GetY());
	this->SetZ(-GetZ());
}

CVector3DF CVector3DF::Reversed()
{
	CVector3DF result;
	result.Reverse();
	return result;
}

CVector3DF CVector3DF::operator +(const CVector3DF& V1)
{
	CVector3DF result;
	result.SetX(GetX() + V1.GetX());
	result.SetY(GetY() + V1.GetY());
	result.SetZ(GetZ() + V1.GetZ());
	return result;
}

CVector3DF CVector3DF::operator=(const CVector3DF& V1)
{
	this->SetParam(V1);
	return (*this);
// 	CVector3DF v;
// 	v.SetParam(V1);
// 	return v;
}

void CVector3DF::operator +=(const CVector3DF& V1)
{
	SetX(GetX() + V1.GetX());
	SetY(GetY() + V1.GetY());
	SetZ(GetZ() + V1.GetZ());
}

void CVector3DF::operator +=(CVector3DF* pVector)
{
	this->x += pVector->GetX();
	this->y += pVector->GetY();
	this->z += pVector->GetZ();
}


CVector3DF CVector3DF::operator -(const CVector3DF& V1)
{
	CVector3DF result;
	result.SetX(GetX() - V1.GetX());
	result.SetY(GetY() - V1.GetY());
	result.SetZ(GetZ() - V1.GetZ());
	return result;
}

void CVector3DF::operator -=(const CVector3DF& V1)
{
	SetX(GetX() - V1.GetX());
	SetY(GetY() - V1.GetY());
	SetZ(GetZ() - V1.GetZ());
}

CVector3DF CVector3DF::operator *(const double& scalar)
{
	CVector3DF result;
	result.SetX(GetX() * scalar);
	result.SetY(GetY() * scalar);
	result.SetZ(GetZ() * scalar);
	return result;
}

CVector3DF CVector3DF::operator *(const CMatrix33& M)
{
	CVector3DF V;
	V.SetX(M(0, 0) * GetX() + M(0, 1) * GetY() + M(0, 2) * GetZ());
	V.SetY(M(1, 0) * GetX() + M(1, 1) * GetY() + M(1, 2) * GetZ());
	V.SetZ(M(2, 0) * GetX() + M(2, 1) * GetY() + M(2, 2) * GetZ());
	return V;
}

void CVector3DF::operator *=(const double& scalar)
{
	SetX(GetX() * scalar);
	SetY(GetY() * scalar);
	SetZ(GetZ() * scalar);
}

void CVector3DF::operator *=(const CMatrix33& M)
{
	SetX(M(0, 0) * GetX() + M(0, 1) * GetY() + M(0, 2) * GetZ());
	SetY(M(1, 0) * GetX() + M(1, 1) * GetY() + M(1, 2) * GetZ());
	SetZ(M(2, 0) * GetX() + M(2, 1) * GetY() + M(2, 2) * GetZ());
}

CVector3DF CVector3DF::operator /(const double& scalar)
{
	CVector3DF result;
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

void CVector3DF::operator /=(const double& scalar)
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

CVector3DF CVector3DF::operator ^(const CVector3DF& V1)
{
	return this->Crossed(V1);
}

void CVector3DF::operator ^=(const CVector3DF& V1)
{
	this->Cross(V1);
}

bool CVector3DF::operator ==(const CVector3DF& V) const
{
	return(this->x == V.GetX() && this->y == V.GetY() && this->z == V.GetZ());
}

bool CVector3DF::operator !=(const CVector3DF& V) const
{
	return(this->x != V.GetX() || this->y != V.GetY() || this->z != V.GetZ());
}


CVector3DF::operator CVertex3D()
{
	CVertex3D P(this->x, this->y, this->z);
	return P;
}

double CVector3DF::Magnitude() const
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

double CVector3DF::SqrMagnitude() const
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

double CVector3DF::CrossMagnitude(const CVector3DF& V)
{
	if (!IsNull() && !V.IsNull())
	{
		CVector3DF V1 = (*this);
		CVector3DF V2 = V;
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

double CVector3DF::CrossSqrMagnitude(const CVector3DF& V)
{
	if (!IsNull() && !V.IsNull())
	{
		CVector3DF V1 = (*this);
		CVector3DF V2 = V;
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

double CVector3DF::Modulus()
{
	return sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
}

CVector3DF CVector3DF::Unit()
{
	CVector3DF result;
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

CVertex3D CVector3DF::Point()
{
	CVertex3D aPnt;
	aPnt.SetParam(GetX(), GetY(), GetZ());
	return aPnt;
}

double CVector3DF::Angle(const CVector3DF& Other) const
{
	double theta, a, b, dot;
	CVector3DF tempV = Other;
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

double CVector3DF::Angle(const CVector3DF& Other, const CVector3DF& Dir) const
{
	double theta;
	CVector3DF tempV = Other;
	CVector3DF D = Dir;
	CVector3DF N = this->Crossed(tempV);
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

bool CVector3DF::IsNull() const
{
	if (this->x == 0 && this->y == 0 && this->z == 0)
		return true;
	else
		return false;
}

bool CVector3DF::IsParallel(const CVector3DF& V) const
{
	/*double ang=0;
	CVector3DF tempV = V;
	ang = this->Angle(tempV);
	if(ang==0 || ang==2*PI)
		return true;
	else
		return false;*/
	CVector3DF N = this->Crossed(V);
	return (N.IsNull());
}

bool CVector3DF::IsOpposite(const CVector3DF& V) const
{
	double ang = 0;
	CVector3DF tempV = V;
	ang = this->Angle(tempV);
	if (fabs(ang - PI) <= 0.0001)
		return true;
	else
		return false;
}

bool CVector3DF::IsNormal(const CVector3DF& V) const
{
	double ang = 0;
	CVector3DF tempV = V;
	ang = this->Angle(tempV);
	if (fabs(ang - PI / 2) <= 0.0001)
		return true;
	else
		return false;
}

void CVector3DF::Normalize()
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

void CVector3DF::Translate(const COneAxis& Ax, const double& amt)
{
	//CGeometry::Translate(Ax, amt);
}

void CVector3DF::Translate(double dx, double dy, double dz)
{
	CVertex3D P = (*this);
	P.Translate(dx, dy, dz);
	CVector3DF V = P;
	(*this) = V;
}

void CVector3DF::Translate(const CVector3DF& dV)
{
	CVertex3D P = (*this);
	P.Translate(dV);
	CVector3DF V = P;
	(*this) = V;
}

void CVector3DF::Translate(const CVertex3D& P1, const CVertex3D& P2)
{
	CVertex3D P = (*this);
	P.Translate(P1, P2);
	CVector3DF V = P;
	(*this) = V;
}

void CVector3DF::Rotate(const COneAxis& Ax, double ang)
{
	COneAxis ax = Ax;
	CVector3DF P1 = ax.GetOrigin();
	CVector3DF V = ax.GetDirection();
	V.Normalize();
	CMatrix33 M; CVector3DF PV(*this);
	M.SetRotation(V, ang);
	CVector3DF RV = M*(PV - P1);
	RV = RV + P1;
	*this = RV;
}

void CVector3DF::Scale(const CVertex3D& P1, double fact)
{
	CVertex3D P = (*this);
	P.Scale(P1, fact);
	CVector3DF V = P;
	(*this) = V;
}

void CVector3DF::Mirror(const CVertex3D& P1)
{
	CVertex3D P = (*this);
	P.Mirror(P1);
	CVector3DF V = P;
	(*this) = V;
}

void CVector3DF::Mirror(const COneAxis& Ax)
{
	CVertex3D P = (*this);
	P.Mirror(Ax);
	CVector3DF V = P;
	(*this) = V;
}

void CVector3DF::Mirror(const CPlane& Pln)
{
	CVertex3D P = (*this);
	P.Mirror(Pln);
	CVector3DF V = P;
	(*this) = V;
}

CVector3DF CVector3DF::Origin()
{
	return CVector3DF(0, 0, 0);
}


//********************************************
// Inner
//********************************************
void CVector3DF::Inner(CVector3DF& v)
{
	double x = this->y* v.GetZ() - this->z* v.GetY();
	double y = this->z* v.GetX() - this->x* v.GetZ();
	double z = this->x* v.GetY() - this->y* v.GetX();
	SetParam(x, y, z);
}

//********************************************
// Inner
//********************************************
CVector3DF CVector3DF::Inner(CVector3DF* u, CVector3DF* v)
{
	// w = u ^ v
	CVector3DF w;
	w.SetParam(u->GetY() * v->GetZ() - u->GetZ() * v->GetY(), u->GetZ() * v->GetX() - u->GetX() * v->GetZ(), u->GetX() * v->GetY() - u->GetY() * v->GetX());

	return w;
}

CVector3DF CVector3DF::Inner(const CVector3DF& u, const CVector3DF& v)
{
	// w = u ^ v
	CVector3DF w;
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
void CVector3DF::NormalizeL2(void)
{
	double norm = GetNormL2();
	if (norm != 0.0f)
	{
		this->x = (this->x / norm);
		this->y = (this->y / norm);
		this->z = (this->z / norm);
	}
	//TRACE("norm : %g\n",GetNormL2());
}

//********************************************
// Normalize
//********************************************
void CVector3DF::NormalizeL2(double v)
{
	double norm = GetNormL2();
	if (norm != 0.0f)
	{
		this->x *= (v / norm);
		this->y *= (v / norm);
		this->z *= (v / norm);
	}
	//TRACE("norm : %g (wanted : %g)\n",GetNormL2(),value);
}

//********************************************
// GetNormSquare
//********************************************
double CVector3DF::GetNormL2Square(void) const
{
	return (this->x * this->x + this->y * this->y + this->z * this->z);
}

//********************************************
// GetNorm
//********************************************
double CVector3DF::GetNormL2(void) const
{
	return sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
}

//********************************************
// Collinear
//********************************************
int CVector3DF::Collinear(CVector3DF* pVector)
{
	double x = pVector->GetX() / this->x;
	double y = pVector->GetY() / this->y;
	double z = pVector->GetZ() / this->z;
	return ((x == y) && (y == z));
}

//********************************************
// Collinear
//********************************************
int CVector3DF::Collinear(CVector3DF& v)
{
	double x = v.GetX() / this->x;
	double y = v.GetY() / this->y;
	double z = v.GetZ() / this->z;
	return ((x == y) && (y == z));
}

istream& operator>>(istream& input, CVector3DF& p)
{
	input >> p.x >> p.y >> p.z;
	return input;
}

ostream& operator<<(ostream& output, CVector3DF& p)
{
	output << p.x << p.y << p.z;
	return output;
}