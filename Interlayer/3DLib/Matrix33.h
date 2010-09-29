//############################################################################
//
// Matrix33.h: interface for the CMatrix33 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATRIX33_H__C140D9C8_24EC_4702_A10B_F50B007BD7B0__INCLUDED_)
#define AFX_MATRIX33_H__C140D9C8_24EC_4702_A10B_F50B007BD7B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Geometry.h"
#include "Vector3d.h"

class CMatrix33
{
public:
	CMatrix33();
	CMatrix33(double r1[3], double r2[3], double r3[3]);
	~CMatrix33();
	CMatrix33 operator *(const CMatrix33&);
	CVector3D operator *(const CVector3D&);
	CMatrix33 operator *(const double Value);
	void operator*=(const CMatrix33&);
	void operator*=(const double Value);
	CMatrix33 operator/(const double Value);
	void operator/=(const double Value);
	CMatrix33 operator+(const CMatrix33&);
	void operator+=(const CMatrix33&);
	CMatrix33 operator-(const CMatrix33&);
	void operator-=(const CMatrix33&);
	double Determinant();
	void Invert();
	CMatrix33 Inverted() const;
	void Transpose();
	void Value() const;
	bool IsNull() const;
	bool IsDiagonal() const;
	bool IsSymmetrical() const;
	bool IsScalar() const;
	bool IsIdentity() const;
	bool IsEqual(const CMatrix33&) const;
	double operator()(int row, int col) const
	{
		return Row[row][col];
	}
	double& operator()(int row, int col)
	{
		return Row[row][col];
	}
	void SetIdentity();
	void SetRotation(const CVector3D& axis, double ang);
	void SetScale(const CVertex3D&);
	CVertex3D GetDiagonal() const;
private:
	double Row[3][3];
	CMatrix33 Adjoint();
};

#endif // !defined(AFX_MATRIX33_H__C140D9C8_24EC_4702_A10B_F50B007BD7B0__INCLUDED_)
