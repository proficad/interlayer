//############################################################################
//
// Vector3d.h: interface for the CVector3d class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VECTOR3D_H__DD3ABF1E_D4ED_4D0B_A32B_C5AD8CAFA517__INCLUDED_)
#define AFX_VECTOR3D_H__DD3ABF1E_D4ED_4D0B_A32B_C5AD8CAFA517__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Geometry.h"

class CVertex3D;
class CMatrix33;
class COneAxis;
class CPlane;

class CVector3D : public CGeometry  // 向量
{
public:
	CVector3D();
	CVector3D(double x, double y, double z);
	CVector3D(const CVertex3D&);
	CVector3D(const CVector3D&, const CVector3D&);
	CVector3D(const CVertex3D&, const CVertex3D&);
	CVector3D(CVertex3D* pVertex1, CVertex3D* pVertex2);
	~CVector3D();

	void SetX(double x)
	{
		m_x = x;
	} //Set The X Component Value
	void SetY(double y)
	{
		m_y = y;
	} //Set The Y Component Value
	void SetZ(double z)
	{
		m_z = z;
	} //Set The Z Component Value
	void SetParam(double x, double y, double z)
	{
		m_x = x; m_y = y;m_z = z;
	} //Set The X Y and Z Values
	void SetParam(const CVector3D& vec)
	{
		m_x = vec.GetX(); m_y = vec.GetY();m_z = vec.GetZ();
	} //Set The X Y and Z Values
	double GetX() const
	{
		return m_x;
	}// Returns The X Component
	double GetY() const
	{
		return m_y;
	}// Returns The Y Component
	double GetZ() const
	{
		return m_z;
	}// Returns The Z Component

	// cross product
	const CVector3D CrossProduct(const CVector3D &vec) const
	{
		return CVector3D(m_y*vec.m_z - m_z*vec.m_y, m_z*vec.m_x - m_x*vec.m_z, m_x*vec.m_y - m_y*vec.m_x);
	}

	// dot product
	const float DotProduct(const CVector3D &vec) const
	{
		return m_x*vec.m_x + m_y*vec.m_y + m_z*vec.m_z;
	}	
	
	double Dot(const CVector3D& V1) const;//Calculate Dot Product 点积
	double DotCross(const CVector3D& V1, const CVector3D& V2);//Calculate Scalar Triple Product 纯量矢量三重积
	void Cross(const CVector3D& V1);//Calculate Cross Product Of' this' And Other 叉积
	CVector3D Crossed(const CVector3D& V1) const;//Return The Cross Product Vector 矢积
	void CrossCross(const CVector3D& V1, const CVector3D& V2);//Calculate The Vector Triple Product 矢量三重积
	CVector3D CrossCrossed(const CVector3D& V1, const CVector3D& V2);//Return Vector Triple Product 矢量三重积
	void Reverse();//Reverse The Direction Of 'this' 相反矢量
	CVector3D Reversed();//Return The Reversed Vector Of 'this'相反矢量
	
	CVector3D operator +(const CVector3D& V1);//Return Addition Of 'this' And Other 相加	
	CVector3D operator -(const CVector3D& V1);//Return Subtraction Of 'this' And Other
	CVector3D operator *(const double& scalar);//Scalar Multiple With Other Vector
	CVector3D operator *(const CMatrix33& M);//Multiply With Matrix
	CVector3D operator /(const double& scalar);//Scalar Division

	CVector3D operator ^(const CVector3D& V1);//Return The Cross Product

	CVector3D operator=(const CVector3D& V1); // Setting

	void operator +=(const CVector3D& V1);//Add Other To 'this'
	void operator +=(CVector3D* pVector);
	
	void operator -=(const CVector3D& V1);//Subtract Other From 'this'
	
	void operator *=(const double& scalar);//Multiply 'this' With Other
	void operator *=(const CMatrix33& M);//Multiply 'this With Matrix
	
	void operator /=(const double& scalar);//Divide 'this' By Scalar
	
	void operator ^=(const CVector3D& V1);//Calculate Cross Product Of' this' And Other
	bool operator ==(const CVector3D& V) const;//Check Equality
	bool operator !=(const CVector3D& V) const;//Check Equality

	double Magnitude() const;// Calculate 'this' Vector's Magnitude
	double SqrMagnitude() const;// Calculate 'this' Vector's Square Magnitude
	double CrossMagnitude(const CVector3D& V);// Calculate 'this' Vector's Cross Magnitude
	double CrossSqrMagnitude(const CVector3D& V1);// Calculate 'this' Vector's Cross Square Magnitude
	double Modulus();// Return Modulus
	CVector3D Unit();//Return Normalized Vector Of 'this'
	CVertex3D Point();//Return Component Triplet As Point
	double Angle(const CVector3D&) const;//Angle Bet'n 'this' And Other
	double Angle(const CVector3D&, const CVector3D&) const;//Angle Bet'n 'this' And Other WRT Direction
	bool IsNull() const;//Check For Null Vector
	bool IsParallel(const CVector3D&) const;//Check If Parallel To Other
	bool IsOpposite(const CVector3D&) const;//Check If Opposite Direction To Other
	bool IsNormal(const CVector3D&) const;//Check If Normal To Other	
	operator CVertex3D();//Convertion Cast To Point;	
	void Normalize();//Normalize 'this' Vector
	virtual void Translate(const COneAxis&, const double&);
	virtual void Translate(double dx, double dy, double dz);
	virtual void Translate(const CVector3D&);
	virtual void Translate(const CVertex3D&, const CVertex3D&);
	virtual void Rotate(const COneAxis&, double);
	virtual void Scale(const CVertex3D&, double);
	virtual void Mirror(const CVertex3D&);
	virtual void Mirror(const COneAxis&);
	virtual void Mirror(const CPlane&);
	static CVector3D Origin();

	//-----------------------------------------------------------------------
	void Inner(CVector3D& v);
	static CVector3D Inner(CVector3D* u, CVector3D* v); // Stupid c++
	static CVector3D Inner(const CVector3D& u, const CVector3D& v); // Stupid c++

	// Misc
	void NormalizeL2(void);
	void NormalizeL2(double val);
	double GetNormL2(void) const;
	double GetNormL2Square(void) const;
	int Collinear(CVector3D* pVector);
	int Collinear(CVector3D& v);
private:
	double m_x;
	double m_y;
	double m_z;
};

#endif // !defined(AFX_VECTOR3D_H__DD3ABF1E_D4ED_4D0B_A32B_C5AD8CAFA517__INCLUDED_)
