// AxisSystem.cpp: implementation of the CAxisSystem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Vector3d.h"
#include "Vertex3D.h"
#include "OneAxis.h"
#include "AxisSystem.h"
#include "GeomException.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//Implementation of class CoordSys (Local Coordinate System)
CAxisSystem::CAxisSystem()
{
	m_geomType = GEO_AXISSYSTEM;
}

CAxisSystem::CAxisSystem(const CVertex3D& Pos, const CVector3D& Dir, const CVector3D& XDir)
{
	m_Position = Pos;
	m_Origin.SetParam(Pos.GetX(), Pos.GetY(), Pos.GetZ());
	m_Direction = Dir;
	m_Direction.Normalize();
	m_XDirection = XDir;
	m_XDirection.Normalize();
	if (!m_Direction.IsNormal(m_XDirection))
	{
		try
		{
			//throw CGeomException(ConstructionFailure);
		}

		catch(...)
		{

		}
	}
	else
		m_YDirection = m_Direction.Crossed(m_XDirection);

	m_geomType = GEO_AXISSYSTEM;
}

CAxisSystem::CAxisSystem(const CVertex3D& Pos, const CVector3D& Dir)
{
	m_Position = Pos;
	m_Origin.SetParam(Pos.GetX(), Pos.GetY(), Pos.GetZ());
	m_Direction = Dir;
	m_Direction.Normalize();

	CVector3D V = m_Direction;
	double lA = V.GetX();
	double lB = V.GetY();
	double lC = V.GetZ();

	double Aabs = lA;
	if (Aabs < 0)
		Aabs = -Aabs;

	double Babs = lB;
	if (Babs < 0)
		Babs = -Babs;

	double Cabs = lC;
	if (Cabs < 0)
		Cabs = -Cabs;

	CVector3D XAxis;

	if (Babs <= Aabs && Babs <= Cabs)
	{
		if (Aabs > Cabs)
			XAxis.SetParam(-lC, 0., lA);
		else
			XAxis.SetParam(lC, 0., -lA);
	}
	else if (Aabs <= Babs && Aabs <= Cabs)
	{
		if (Babs > Cabs)
			XAxis.SetParam(0., -lC, lB);
		else
			XAxis.SetParam(0., lC, -lB);
	}
	else
	{
		if (Aabs > Babs)
			XAxis.SetParam(-lB, lA, 0.);
		else
			XAxis.SetParam(lB, -lA, 0.);
	}
	m_XDirection = XAxis;
	m_XDirection.Normalize();
	if (!m_Direction.IsNormal(m_XDirection))
		throw CGeomException(ConstructionFailure);
	else
		m_YDirection = m_Direction.Crossed(m_XDirection);

	m_geomType = GEO_AXISSYSTEM;
}

CAxisSystem::CAxisSystem(const CAxisSystem& Ax)
{
	m_Position = Ax.GetPosition();
	m_Origin = Ax.GetOrigin();
	m_Direction = Ax.GetDirection();
	m_XDirection = Ax.GetXDirection();
	m_YDirection = m_Direction.Crossed(m_XDirection);

	m_geomType = GEO_AXISSYSTEM;
}

CAxisSystem::~CAxisSystem()
{
}

void CAxisSystem::SetDirection(const CVector3D& D)
{
	m_Direction = D;
	m_Direction.Normalize();
	if (!m_XDirection.IsParallel(m_Direction))
	{
		CVector3D tempY = D.Crossed(m_XDirection);
		m_XDirection = tempY.Crossed(D);
		m_YDirection = m_Direction.Crossed(m_XDirection);
		m_XDirection.Normalize();
		m_YDirection.Normalize();
	}
	else
	{
		CVector3D tempX = m_YDirection.Crossed(D);
		m_YDirection = D.Crossed(tempX);
		m_XDirection = m_Direction.Crossed(m_YDirection);
		m_XDirection.Normalize();
		m_YDirection.Normalize();
	}
	//else
	//	throw CGeomException(ConstructionFailure);
}

void CAxisSystem::SetDirection(double dx, double dy, double dz)
{
	CVector3D D(dx, dy, dz);
	D.Unit();
	SetDirection(D);
}

void CAxisSystem::SetXDirection(const CVector3D& XD)
{
	if (!this->m_Direction.IsParallel(XD))
	{
		CVector3D XDir = XD;
		XDir.Normalize();
		m_XDirection = m_Direction.CrossCrossed(XDir, m_Direction);
		m_YDirection = m_Direction.Crossed(m_XDirection);
	}
	else
		throw CGeomException(ConstructionFailure);
}

void CAxisSystem::SetXDirection(double dxx, double dxy, double dxz)
{
	CVector3D XD(dxx, dxy, dxz);
	if (!this->m_Direction.IsParallel(XD))
	{
		SetXDirection(XD.Unit());
	}
	else
		throw CGeomException(ConstructionFailure);
}

void CAxisSystem::SetYDirection(const CVector3D& YD)
{
	if (!this->m_Direction.IsParallel(YD))
	{
		CVector3D YDir = YD;
		YDir.Normalize();
		m_YDirection = m_Direction.CrossCrossed(YDir, m_Direction);
		m_XDirection = m_YDirection.Crossed(m_Direction);
	}
	else
		throw CGeomException(ConstructionFailure);
}

void CAxisSystem::SetYDirection(double dyx, double dyy, double dyz)
{
	CVector3D YD(dyx, dyy, dyz);
	if (!this->m_Direction.IsParallel(YD))
	{
		SetYDirection(YD.Unit());
	}
	else
		throw CGeomException(ConstructionFailure);
}

void CAxisSystem::SetPosition(double x, double y, double z)
{
	m_Position.SetParam(x, y, z);
	m_Origin.SetParam(x, y, z);
}

void CAxisSystem::SetPosition(const CVertex3D& P)
{
	SetPosition(P.GetX(), P.GetY(), P.GetZ());
}

CVertex3D CAxisSystem::GetPosition() const
{
	return m_Position;
}

CVector3D CAxisSystem::GetOrigin() const
{
	return m_Origin;
}

CVector3D CAxisSystem::GetDirection() const
{
	return m_Direction;
}

CVector3D CAxisSystem::GetXDirection() const
{
	return m_XDirection;
}

CVector3D CAxisSystem::GetYDirection() const
{
	return m_YDirection;
}

bool CAxisSystem::operator ==(const CAxisSystem& C) const
{
	return (m_Position == C.GetPosition() 
		&& m_Direction == C.GetDirection() 
		&& m_XDirection == C.GetXDirection() 
		&& m_YDirection == C.GetYDirection());
}

void CAxisSystem::Translate(const COneAxis& Ax, const double& amt)
{
	CGeometry::Translate(Ax, amt);
}

void CAxisSystem::Translate(double dx, double dy, double dz)
{
	m_Position.Translate(dx, dy, dz);
	m_Origin.Translate(dx, dy, dz);
}

void CAxisSystem::Translate(const CVector3D& V)
{
	m_Position.Translate(V);
	m_Origin.Translate(V);
}

void CAxisSystem::Translate(const CVertex3D& P1, const CVertex3D& P2)
{
	m_Position.Translate(P1, P2);
	m_Origin.Translate(P1, P2);
}

void CAxisSystem::Rotate(const COneAxis& Ax, double ang)
{
	COneAxis tAx = Ax;
	tAx.Translate(tAx.GetPosition(), CVertex3D(0, 0, 0));
	CVector3D Dir = GetDirection();
	COneAxis ax(GetPosition(), GetDirection());
	ax.Rotate(tAx, ang);
	m_Origin.Rotate(Ax, ang);
	m_Position = m_Origin.Point();
	SetDirection(ax.GetDirection());
}

void CAxisSystem::Scale(const CVertex3D& P, double fact)
{
	m_Position.Scale(P, fact);
	m_Origin.Scale(P, fact);
	if (fact < 0)
	{
		m_Direction.Reverse();
		m_XDirection.Reverse();
		m_YDirection.Reverse();
	}
}

void CAxisSystem::Mirror(const CVertex3D& P)
{
	m_Position.Mirror(P);
	m_Origin.Mirror(P);
	m_Direction.Reverse();
	m_XDirection.Reverse();
	m_YDirection.Reverse();
}

void CAxisSystem::Mirror(const COneAxis& Ax)
{
	m_Position.Mirror(Ax);
	m_Origin.Mirror(Ax);
	if (m_Direction.IsParallel(Ax.GetDirection()))
		m_Direction.Mirror(Ax.GetDirection());
	else
		m_Direction.Mirror(Ax);
	if (m_XDirection.IsParallel(Ax.GetDirection()))
		m_XDirection.Mirror(Ax.GetDirection());
	else
		m_XDirection.Mirror(Ax);
	if (m_YDirection.IsParallel(Ax.GetDirection()))
		m_YDirection.Mirror(Ax.GetDirection());
	else
		m_YDirection.Mirror(Ax);
	//SetDirection (m_XDirection.Crossed(YDirection));
}

void CAxisSystem::Mirror(const CPlane& Pln)
{
	m_Position.Mirror(Pln);
	m_Origin.Mirror(Pln);
	m_Direction.Mirror(Pln);
	m_XDirection.Mirror(Pln);
	m_YDirection.Mirror(Pln);
}
