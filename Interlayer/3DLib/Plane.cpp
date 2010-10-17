// Plane.cpp: implementation of the CPlane class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Plane.h"
#include "Vector3d.h"
#include "Vertex3D.h"
#include "OneAxis.h"
#include "GeomException.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPlane::CPlane(const CVertex3D& P1, const CVertex3D& P2, const CVertex3D& P3)
{
	CVector3D V1(P1, P2);
	CVector3D V2(P1, P3);
	CVector3D Dir = V1 ^ V2;

	CAxisSystem ax(P1, Dir, V1);
	m_itsLocation = ax;

	UpdateEquation();

	double A = m_Equation[0];
	double B = m_Equation[1];
	double C = m_Equation[2];
	double D = m_Equation[3];

	double x = m_itsLocation.GetPosition().GetX();
	double y = m_itsLocation.GetPosition().GetY();
	double z = m_itsLocation.GetPosition().GetZ();

	bool Done = (A* x + B* y + C* z + D == 0);
	if (!Done)
		throw CGeomException(ConstructionFailure);

	m_geomType = GEO_PLANE;
}

CPlane::CPlane(CVertex3D* pVertex1, CVertex3D* pVertex2, CVertex3D* pVertex3)
{
	CVertex3D P1, P2, P3;

	P1.SetParam(pVertex1);
	P2.SetParam(pVertex2);
	P3.SetParam(pVertex3);

	CVector3D V1(P1, P2);
	CVector3D V2(P1, P3);
	if(V1.IsNull())
		V1=CVector3D(1,0,0);
	if(V2.IsNull())
		V2=CVector3D(0,1,0);
	CVector3D Dir = V1 ^ V2;

	CAxisSystem ax(P1, Dir, V1);
	m_itsLocation = ax;

	UpdateEquation();

	double A = m_Equation[0];
	double B = m_Equation[1];
	double C = m_Equation[2];
	double D = m_Equation[3];

	double x = m_itsLocation.GetPosition().GetX();
	double y = m_itsLocation.GetPosition().GetY();
	double z = m_itsLocation.GetPosition().GetZ();

	bool Done = (A* x + B* y + C* z + D == 0);
	if (!Done)
		throw CGeomException(ConstructionFailure);

	m_geomType = GEO_PLANE;
}

CPlane::CPlane(const CVertex3D& P1, const CVector3D& V1, const CVector3D& V2)
{
	CVector3D v1 = V1, v2 = V2;
	CVector3D Dir = v1 ^ v2;

	CAxisSystem ax(P1, Dir, v1);
	m_itsLocation = ax;

	UpdateEquation();

	double A = m_Equation[0];
	double B = m_Equation[1];
	double C = m_Equation[2];
	double D = m_Equation[3];

	double x = m_itsLocation.GetPosition().GetX();
	double y = m_itsLocation.GetPosition().GetY();
	double z = m_itsLocation.GetPosition().GetZ();

	bool Done = (A* x + B* y + C* z + D == 0);
	if (!Done)
		throw CGeomException(ConstructionFailure);

	m_geomType = GEO_PLANE;
}

CPlane::CPlane(const double& A, const double& B, const double& C, const double& D)
{
	m_Equation[0] = A;
	m_Equation[1] = B;
	m_Equation[2] = C;
	m_Equation[3] = D;

	CVertex3D Pos(A, B, C);
	CVector3D Dir(A, B, C);

	CAxisSystem location(Pos, Dir);

	double d = location.GetDirection().Magnitude();
	if (d > 0.0)
	{
		d = 1.0 / d;
		CVector3D O = location.GetDirection() * (-d)*m_Equation[3];
		location.SetPosition(O.Point());
	}

	m_itsLocation = location;

	double x = m_itsLocation.GetPosition().GetX();
	double y = m_itsLocation.GetPosition().GetY();
	double z = m_itsLocation.GetPosition().GetZ();	

	bool Done = (A* x + B* y + C* z + D == 0);
	if (!Done)
		throw CGeomException(ConstructionFailure);

	m_geomType = GEO_PLANE;
}

CPlane::CPlane(const COneAxis& Ax)
{
	COneAxis ax = Ax;

	CVertex3D P = Ax.GetPosition();
	CVector3D V = Ax.GetDirection();

	CAxisSystem location(P, V);

	m_itsLocation = location;

	UpdateEquation();

	double A = m_Equation[0];
	double B = m_Equation[1];
	double C = m_Equation[2];
	double D = m_Equation[3];

	double x = m_itsLocation.GetPosition().GetX();
	double y = m_itsLocation.GetPosition().GetY();
	double z = m_itsLocation.GetPosition().GetZ();

	bool Done = (A* x + B* y + C* z + D == 0);
	if (!Done)
		throw CGeomException(ConstructionFailure);

	m_geomType = GEO_PLANE;
}

CPlane::~CPlane()
{
}

CVertex3D CPlane::PointAtPara(const double uPar, const double vPar)
{
	CVector3D V;
	V = m_itsLocation.GetOrigin() + m_itsLocation.GetXDirection() * uPar + m_itsLocation.GetYDirection() * vPar;
	/*CString str;
	str.Format("YDirection %0.3lf, %0.3lf, %0.3lf",m_itsLocation.GetYDirection().GetX(),m_itsLocation.GetYDirection().GetY(),m_itsLocation.GetYDirection().GetZ());
	MessageBox(0, str, "From Plane", MB_OK);*/
	return V.Point();
}

CVertex3D CPlane::GetPosition() const
{
	return m_itsLocation.GetPosition();
}

CVector3D CPlane::GetDirection() const
{
	return m_itsLocation.GetDirection();
}

void CPlane::EquationAt(const CVertex3D& P, double* e)
{
	double x = P.GetX();
	double y = P.GetY();
	double z = P.GetZ();

	e[0] = m_Equation[0] * x;
	e[1] = m_Equation[1] * y;
	e[2] = m_Equation[2] * z;
	e[3] = m_Equation[3];
}

double CPlane::GetConstant() const
{
	return m_Equation[3];
}

void CPlane::Translate(const COneAxis& Ax, const double& amt)
{
	CGeometry::Translate(Ax, amt);
}

void CPlane::Translate(double dx, double dy, double dz)
{
	m_itsLocation.Translate(dx, dy, dz);
}

void CPlane::Translate(const CVector3D& V)
{
	m_itsLocation.Translate(V);
}

void CPlane::Translate(const CVertex3D& P1, const CVertex3D& P2)
{
	m_itsLocation.Translate(P1, P2);
}

void CPlane::Rotate(const COneAxis& Ax, double ang)
{
	m_itsLocation.Rotate(Ax, ang);
}

void CPlane::Scale(const CVertex3D& P, double fact)
{
	m_itsLocation.Scale(P, fact);
}

void CPlane::Mirror(const CVertex3D& P)
{
	m_itsLocation.Mirror(P);
}

void CPlane::Mirror(const COneAxis& Ax)
{
	m_itsLocation.Mirror(Ax);
}

void CPlane::Mirror(const CPlane& Pln)
{
	m_itsLocation.Mirror(Pln);
}

void CPlane::UpdateEquation()
{
	CVector3D Z = m_itsLocation.GetDirection();
	m_Equation[0] = Z.GetX();
	m_Equation[1] = Z.GetY();
	m_Equation[2] = Z.GetZ();
	m_Equation[3] = -(m_itsLocation.GetOrigin().Dot(Z));
}

CPlane CPlane::XOY()
{
	CPlane P(COneAxis(CVertex3D(0, 0, 0), CVector3D(0, 0, 1)));
	return P;
}

CPlane CPlane::YOZ()
{
	CPlane P(COneAxis(CVertex3D(0, 0, 0), CVector3D(1, 0, 0)));
	return P;
}

CPlane CPlane::ZOX()
{
	CPlane P(COneAxis(CVertex3D(0, 0, 0), CVector3D(0, 1, 0)));
	return P;
}
