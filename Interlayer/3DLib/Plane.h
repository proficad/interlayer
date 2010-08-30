// Plane.h: interface for the CPlane class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLANE_H__10484064_B0F8_46EC_8C2E_DDC8A0EA6299__INCLUDED_)
#define AFX_PLANE_H__10484064_B0F8_46EC_8C2E_DDC8A0EA6299__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Geometry.h"
#include "Vector3d.h"
#include "Vertex3D.h"
#include "OneAxis.h"
#include "AxisSystem.h"
#include "Material.h"

class CPlane : public CGeometry
{
public:
	CPlane()
	{
	}
	CPlane(const CVertex3D&, const CVertex3D&, const CVertex3D&);
	CPlane(CVertex3D* pVertex1, CVertex3D* pVertex2, CVertex3D* pVertex3);
	CPlane(const CVertex3D&, const CVector3D&, const CVector3D&);
	CPlane(const double&, const double&, const double&, const double&);
	CPlane(const COneAxis&);
	virtual ~CPlane();

	virtual CVertex3D PointAtPara(const double uPar, const double vPar);
	virtual CVertex3D GetPosition() const;
	virtual CVector3D GetDirection() const;	

	virtual void EquationAt(const CVertex3D&, double*);				// 
	virtual double GetConstant() const;								// 常量 d
	virtual void Translate(const COneAxis&, const double&);
	virtual void Translate(double dx, double dy, double dz);
	virtual void Translate(const CVector3D&);
	virtual void Translate(const CVertex3D&, const CVertex3D&);
	virtual void Rotate(const COneAxis&, double);
	virtual void Scale(const CVertex3D&, double);
	virtual void Mirror(const CVertex3D&);
	virtual void Mirror(const COneAxis&);
	virtual void Mirror(const CPlane&);
	static CPlane XOY();
	static CPlane YOZ();
	static CPlane ZOX();

	//-----------------------------------------------------------------------
	void GetPlaneParam(double *a, double *b, double *c, double *d)
	{
		*a = m_Equation[0];
		*b = m_Equation[1];
		*c = m_Equation[2];
		*d = m_Equation[3];
	}

	void GetPlaneParam(double *eq)
	{
		eq[0] = m_Equation[0];
		eq[1] = m_Equation[1];
		eq[2] = m_Equation[2];
		eq[3] = m_Equation[3];
	}
	//-----------------------------------------------------------------------
	virtual void SetNormal(double x, double y, double z)
	{
		m_Equation[0] = x;
		m_Equation[1] = y;
		m_Equation[2] = z;
	}

	virtual CVector3D GetNormal() const
	{
		CVector3D Normal;
		Normal.SetParam(m_Equation[0], m_Equation[1], m_Equation[2]);
		return Normal;
	}
	virtual bool HasNormal()
	{
		CVector3D Normal;
		Normal.SetParam(m_Equation[0], m_Equation[1], m_Equation[2]);
		return (Normal.GetNormL2Square() != 0);
	}

	// 法向量计算
	virtual void CalculateNormal(){};
	//////////////////////////////////////////////////////////////////////////
	CAxisSystem m_itsLocation;
	virtual void UpdateEquation();

private:
	//Plane Equation 平面方程 ax + by + cz + d = 0
	double m_Equation[4];				// Equation[0] -> a; Equation[1] -> b; Equation[2] -> c; Equation[3] -> d
};

#endif // !defined(AFX_PLANE_H__10484064_B0F8_46EC_8C2E_DDC8A0EA6299__INCLUDED_)
