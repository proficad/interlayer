//############################################################################
//
// Line3D.h: interface for the CLine3D class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LINE3D_H__5EAE9E77_1D1A_426B_A0A5_BF82B65CA45B__INCLUDED_)
#define AFX_LINE3D_H__5EAE9E77_1D1A_426B_A0A5_BF82B65CA45B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Curve.h"
#include "Vector3d.h"
#include "Vertex3D.h"
#include "OneAxis.h"


class CLine3D : public CCurve
{
public:
	CLine3D();
	CLine3D(const CVertex3D&, const CVertex3D&);
	CLine3D(const CVertex3D&, const CVector3D&);
	virtual ~CLine3D();
	virtual void Reverse();
	virtual CVertex3D PointAtPara(const double& par);
	virtual double FirstParameter() const;
	virtual double LastParameter() const;
	virtual bool IsClosed() const;
	bool operator ==(const CLine3D& L) const;
	void operator =(const CLine3D& L);
	CVector3D Direction() const;
	COneAxis GetLocation() const;
	CVector3D NormalThrough(const CVertex3D&);
	double Distance(const CVertex3D&);
	CVertex3D StartPoint() const;
	CVertex3D EndPoint() const;
	virtual CCurve* Copy() const;
	virtual void Translate(const COneAxis&, const double&);
	virtual void Translate(double dx, double dy, double dz);
	virtual void Translate(const CVector3D&);
	virtual void Translate(const CVertex3D&, const CVertex3D&);
	virtual void Rotate(const COneAxis&, double);
	virtual void Scale(const CVertex3D&, double);
	virtual void Mirror(const CVertex3D&);
	virtual void Mirror(const COneAxis&);
	virtual void Mirror(const CPlane&);
private:
	CVertex3D m_startPoint;
	CVertex3D m_endPoint;
	COneAxis m_itsLocation;
};

#endif // !defined(AFX_LINE3D_H__5EAE9E77_1D1A_426B_A0A5_BF82B65CA45B__INCLUDED_)
