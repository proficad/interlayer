//############################################################################
//
// Curve.h: interface for the CCurve class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CURVE_H__14D97D93_F8D7_4434_AC43_2056A73CF2C2__INCLUDED_)
#define AFX_CURVE_H__14D97D93_F8D7_4434_AC43_2056A73CF2C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Geometry.h"

class CVertex3D;

enum CurveType
{
	BEZIER,
	BSPLINE,
	CIRCLE,
	ELLIPSE,
	HYPERBOLA,
	LINE,
	PARABOLA
};

//Abstract Base Class
class CCurve : public CGeometry
{
public:
	CCurve();
	virtual ~CCurve();
	virtual void Reverse() = 0;
	virtual CVertex3D PointAtPara(const double& para) = 0;
	virtual double FirstParameter() const = 0;
	virtual double LastParameter() const = 0;
	virtual bool IsClosed() const = 0;
	CurveType Type() const;
	bool IsOfType(const CurveType&) const;
	virtual CCurve* Copy() const = 0;
	virtual void Translate(double dx, double dy, double dz) = 0;
	virtual void Translate(const CVector3D&) = 0;
	virtual void Translate(const CVertex3D&, const CVertex3D&) = 0;
	virtual void Rotate(const COneAxis&, double) = 0;
	virtual void Scale(const CVertex3D&, double) = 0;
	virtual void Mirror(const CVertex3D&) = 0;
	virtual void Mirror(const COneAxis&) = 0;
	virtual void Mirror(const CPlane&) = 0;
protected:
	CurveType m_cType;		// 曲线类别
	bool m_bReversed;		// 反向的
};

#endif // !defined(AFX_CURVE_H__14D97D93_F8D7_4434_AC43_2056A73CF2C2__INCLUDED_)
