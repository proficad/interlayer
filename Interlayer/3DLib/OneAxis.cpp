// OneAxis.cpp: implementation of the COneAxis class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OneAxis.h"
#include "Vector3d.h"
#include "Vertex3D.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//Implementation of class OneAxis 
//(Used for construction of revolutions and as axis of symmetries)
COneAxis::COneAxis()
{
	m_Position = CVertex3D(0, 0, 0);
	m_Direction = CVector3D(0, 0, 1);
	m_Direction.Normalize();
	m_Origin.SetParam(0, 0, 0);
	m_geomType = GEO_ONEAXIS;
}

COneAxis::COneAxis(const CVertex3D& Pos, const CVector3D& Dir) : m_Position(Pos), m_Direction(Dir)
{
	m_Direction.Normalize();
	m_Origin.SetParam(Pos.GetX(), Pos.GetY(), Pos.GetZ());
	m_geomType = GEO_ONEAXIS;
}

COneAxis::~COneAxis()
{
}

void COneAxis::SetPosition(const CVertex3D& P)
{
	m_Position = P;
	m_Origin = P;
}

void COneAxis::SetPosition(double x, double y, double z)
{
	m_Position.SetParam(x, y, z);
	m_Origin.SetParam(x, y, z);
}

void COneAxis::SetDirection(const CVector3D& D)
{
	m_Direction = D;
	m_Direction.Normalize();
}

void COneAxis::SetDirection(double dx, double dy, double dz)
{
	m_Direction.SetParam(dx, dy, dz);
	m_Direction.Normalize();
}

CVertex3D COneAxis::GetPosition() const
{
	return m_Position;
}

CVector3D COneAxis::GetOrigin() const
{
	return m_Origin;
}

CVector3D COneAxis::GetDirection() const
{
	return m_Direction;
}

bool COneAxis::operator ==(const COneAxis& rhAx) const
{
	bool eq = (m_Position == rhAx.GetPosition() && m_Direction == rhAx.GetDirection() && m_Origin == rhAx.GetOrigin());
	return eq;
}

void COneAxis::Translate(const COneAxis& Ax, const double& amt)
{
	CGeometry::Translate(Ax, amt);
}

void COneAxis::Translate(double dx, double dy, double dz)
{
	CVertex3D P = m_Position;
	P.Translate(dx, dy, dz);
	SetPosition(P);
	m_Origin.SetParam(P.GetX(), P.GetY(), P.GetZ());
}

void COneAxis::Translate(const CVector3D& V)
{
	CVertex3D P = m_Position;
	P.Translate(V);
	SetPosition(P);
	m_Origin.SetParam(P.GetX(), P.GetY(), P.GetZ());
}

void COneAxis::Translate(const CVertex3D& P1, const CVertex3D& P2)
{
	CVertex3D P = m_Position;
	P.Translate(P1, P2);
	SetPosition(P);
	m_Origin.SetParam(P.GetX(), P.GetY(), P.GetZ());
}

void COneAxis::Rotate(const COneAxis& Ax, double ang)
{
	CVertex3D P = m_Position;
	CVector3D D = m_Direction;
	P.Rotate(Ax, ang);
	D.Rotate(Ax, ang);
	SetPosition(P);
	SetDirection(D);
	m_Origin.SetParam(P.GetX(), P.GetY(), P.GetZ());
}

void COneAxis::Scale(const CVertex3D& Pnt, double fact)
{
	CVertex3D P = m_Position;
	CVector3D D = m_Direction;
	P.Scale(Pnt, fact);
	D.Scale(Pnt, fact);
	SetPosition(P);
	SetDirection(D);
	m_Origin.SetParam(P.GetX(), P.GetY(), P.GetZ());
}

void COneAxis::Mirror(const CVertex3D& Pnt)
{
	CVertex3D P = m_Position;
	CVector3D D = m_Direction;
	P.Mirror(Pnt);
	D.Mirror(Pnt);
	SetPosition(P);
	SetDirection(D);
	m_Origin.SetParam(P.GetX(), P.GetY(), P.GetZ());
}

void COneAxis::Mirror(const COneAxis& Ax)
{
	CVertex3D P = m_Position;
	CVector3D D = m_Direction;
	/*CString str;
	str.Format(" Now Direction X %0.3lf Y %0.3lf Z %0.3lf", D.GetX(),D.GetY(),D.GetZ());
	MessageBox(0, str, "From Axis", MB_OK);*/
	P.Mirror(Ax);
	CVector3D dir = Ax.GetDirection();
	if (dir.IsParallel(D))
		D.Mirror(Ax.GetDirection());
	else
		D.Mirror(Ax);
	/*str.Format(" Now Direction X %0.3lf Y %0.3lf Z %0.3lf", D.GetX(),D.GetY(),D.GetZ());
	MessageBox(0, str, "From Axis", MB_OK);*/
	SetPosition(P);
	SetDirection(D);
	m_Origin.SetParam(P.GetX(), P.GetY(), P.GetZ());
}

void COneAxis::Mirror(const CPlane& Pln)
{
	CVertex3D P = m_Position;
	CVector3D D = m_Direction;
	P.Mirror(Pln);
	D.Mirror(Pln);
	SetPosition(P);
	SetDirection(D);
	m_Origin.SetParam(P.GetX(), P.GetY(), P.GetZ());
}

COneAxis COneAxis::OX()
{
	COneAxis Ax(CVertex3D(0, 0, 0), CVector3D(1, 0, 0));
	return Ax;
}

COneAxis COneAxis::OY()
{
	COneAxis Ax(CVertex3D(0, 0, 0), CVector3D(0, 1, 0));
	return Ax;
}

COneAxis COneAxis::OZ()
{
	COneAxis Ax(CVertex3D(0, 0, 0), CVector3D(0, 0, 1));
	return Ax;
}