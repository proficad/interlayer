// Line3D.cpp: implementation of the CLine3D class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Line3D.h"
#include <math.h>
#include "MMath.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLine3D::CLine3D()
{
	m_cType = LINE;
}

CLine3D::CLine3D(const CVertex3D& sP, const CVertex3D& eP) : m_startPoint(sP), m_endPoint(eP)
{
	COneAxis Ax(sP, CVector3D(sP, eP));
	m_itsLocation = Ax;
	m_cType = LINE;
}

CLine3D::CLine3D(const CVertex3D& P, const CVector3D& V)
{
	CVector3D dir = V;
	CVector3D sv = dir.Unit() * -INFINITY;
	CVector3D ev = dir.Unit() * INFINITY;
	sv.SetParam(sv.GetX() + P.GetX(), sv.GetY() + P.GetY(), sv.GetZ() + P.GetZ());
	ev.SetParam(ev.GetX() + P.GetX(), ev.GetY() + P.GetY(), ev.GetZ() + P.GetZ());

	m_startPoint.SetParam(sv.GetX(), sv.GetY(), sv.GetZ());
	m_endPoint.SetParam(ev.GetX(), ev.GetY(), ev.GetZ());

	COneAxis Ax(m_startPoint, CVector3D(sv, ev));
	m_itsLocation = Ax;

	m_cType = LINE;
}

CLine3D::~CLine3D()
{
}

void CLine3D::Reverse()
{
	CVertex3D tPnt = m_startPoint;
	m_startPoint = m_endPoint;
	m_endPoint = tPnt;
}

CVertex3D CLine3D::PointAtPara(const double& par)
{
	double u;
	if (par < 0)
		u = 0;
	else if (par > 1)
		u = 1;
	else
		u = par;

	CVertex3D aPnt;
	
	aPnt = m_startPoint + (m_endPoint - m_startPoint) * u;
	
	return aPnt;
}

double CLine3D::FirstParameter() const
{
	return 0;
}

double CLine3D::LastParameter() const
{
	return 1;
}

bool CLine3D::IsClosed() const
{
	return false;
}

bool CLine3D::operator ==(const CLine3D& L) const
{
	bool eq = (m_startPoint == L.StartPoint() && m_endPoint == L.EndPoint());
	return eq;
}

void CLine3D::operator =(const CLine3D& L)
{
	m_itsLocation = L.GetLocation();
	m_startPoint = L.StartPoint();
	m_endPoint = L.EndPoint();
}

CVector3D CLine3D::Direction() const
{
	CVector3D sv(m_startPoint);
	CVector3D ev(m_endPoint);
	CVector3D dir(sv, ev);
	return dir.Unit();
}

COneAxis CLine3D::GetLocation() const
{
	return m_itsLocation;
}

CVector3D CLine3D::NormalThrough(const CVertex3D& P)
{
	CVertex3D P3 = P;
	CVertex3D P1 = m_startPoint;
	double D = Distance(P3);
	D = 1 / D;
	CVector3D N1 = Direction();
	CVector3D PV = CVector3D(P3 - P1);
	double dt = PV.Dot(N1);
	CVector3D N = (CVector3D(P3 - P1) - N1*(dt)) * D;
	return N;
}

double CLine3D::Distance(const CVertex3D& P)
{
	CVertex3D P3 = P;
	CVertex3D P1 = m_startPoint;
	CVertex3D P2 = m_endPoint;
	double x1, y1, z1, x2, y2, z2, x3, y3, z3;
	x1 = P1.GetX(); y1 = P1.GetY(); z1 = P1.GetZ();
	x2 = P2.GetX(); y2 = P2.GetY(); z2 = P2.GetZ();
	x3 = P3.GetX(); y3 = P3.GetY(); z3 = P3.GetZ();
	double L = P1.Distance(P2);
	L = 1 / L;
	double D = L * (sqrt(pow((y3 - y1) * (z2 - z1) - (z3 - z1) * (y2 - y1), 2) + pow((z3 - z1) * (x2 - x1) - (x3 - x1) * (z2 - z1), 2) + pow((x3 - x1) * (y2 - y1) - (y3 - y1) * (x2 - x1), 2)));
	return D;
}

CVertex3D CLine3D::StartPoint() const
{
	return m_startPoint;
}

CVertex3D CLine3D::EndPoint() const
{
	return m_endPoint;
}

void CLine3D::Translate(const COneAxis& Ax, const double& amt)
{
	CGeometry::Translate(Ax, amt);
}

void CLine3D::Translate(double dx, double dy, double dz)
{
	m_startPoint.Translate(dx, dy, dz);
	m_endPoint.Translate(dx, dy, dz);
}

void CLine3D::Translate(const CVector3D& V)
{
	m_startPoint.Translate(V);
	m_endPoint.Translate(V);
}

CCurve* CLine3D::Copy() const
{
	CLine3D* L = new CLine3D(m_startPoint, m_endPoint);
	return L;
}

void CLine3D::Translate(const CVertex3D& P1, const CVertex3D& P2)
{
	m_startPoint.Translate(P1, P2);
	m_endPoint.Translate(P1, P2);
}

void CLine3D::Rotate(const COneAxis& Ax, double ang)
{
	m_startPoint.Rotate(Ax, ang);
	m_endPoint.Rotate(Ax, ang);
}

void CLine3D::Scale(const CVertex3D& P, double fact)
{
	m_startPoint.Scale(P, fact);
	m_endPoint.Scale(P, fact);
}

void CLine3D::Mirror(const CVertex3D& P)
{
	m_startPoint.Mirror(P);
	m_endPoint.Mirror(P);
}

void CLine3D::Mirror(const COneAxis& Ax)
{
	m_startPoint.Mirror(Ax);
	m_endPoint.Mirror(Ax);
}

void CLine3D::Mirror(const CPlane& Pln)
{
	m_startPoint.Mirror(Pln);
	m_endPoint.Mirror(Pln);
}
