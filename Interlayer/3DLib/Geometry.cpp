// Geometry.cpp: implementation of the CGeometry class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Geometry.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#include "Vector3d.h"
#include "Vertex3D.h"
#include "OneAxis.h"
#include "AxisSystem.h"
#include "Line3D.h"
#include "Plane.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int CGeometry::m_refCount = 0;

CGeometry::CGeometry()
{
	m_refCount++;
	AddMaterial(DEFAULT);
}

CGeometry::~CGeometry()
{
	m_refCount--;
}

void CGeometry::Translate(const COneAxis& Ax, const double& amt)
{
	CVector3D D = Ax.GetDirection();
	double dx, dy, dz;
	dx = D.GetX(); dy = D.GetY(); dz = D.GetZ();
	dx *= amt; dy *= amt; dz *= amt;
	Translate(dx, dy, dz);
}

void CGeometry::SetMaterial(const CMaterial& Material)
{
	m_itsMaterial = Material;
}

void CGeometry::SetMaterial(const CMaterial* pMaterial)
{
	m_itsMaterial = (*pMaterial);
}

void CGeometry::AddMaterial(float* matArray)
{
	m_itsMaterial.SetMaterial(matArray);
}

void CGeometry::AddMaterial(const GLMaterial& mat)
{
	switch (mat)
	{
		//Default Setting
	case DEFAULT:
		AddMaterial(DefaultM);
		break;
		//Non Metals
	case EMERALD:
		AddMaterial(Emerald);
		break;
	case JADE:
		AddMaterial(Jade);
		break;
	case OBSIDIAN:
		AddMaterial(Obsidian);
		break;
	case PEARL:
		AddMaterial(Pearl);
		break;
	case PEWTER:
		AddMaterial(Pewter);
		break;
	case RUBY:
		AddMaterial(Ruby);
		break;
	case TURQUOISE:
		AddMaterial(Turquoise);
		break;
	case PLASTIC:
		AddMaterial(Plastic);
		break;
	case RUBBER:
		AddMaterial(Rubber);
		break;
		//Metals
	case ALUMINIUM:
		AddMaterial(Aluminium);
		break;
	case BRASS:
		AddMaterial(Brass);
		break;
	case BRONZE:
		AddMaterial(Bronze);
		break;
	case P_BRONZE:
		AddMaterial(Polished_Bronze);
		break;
	case CHROME:
		AddMaterial(Chrome);
		break;
	case COPPER:
		AddMaterial(Copper);
		break;
	case P_COPPER:
		AddMaterial(Polished_Copper);
		break;
	case GOLD:
		AddMaterial(Gold);
		break;
	case P_GOLD:
		AddMaterial(Polished_Gold);
		break;
	case SILVER:
		AddMaterial(Silver);
		break;
	case P_SILVER:
		AddMaterial(Polished_Silver);
		break;
	case STEEL:
		AddMaterial(Steel);
		break;
	case NONE:
		AddMaterial(None);
		break;
	default:
		AddMaterial(DefaultM);
		break;
	}
}

void CGeometry::ApplyMaterial(const GLMaterial& mat)
{
	switch (mat)
	{
		//Default Setting
	case DEFAULT:
		AddMaterial(DefaultM);
		break;
		//Non Metals
	case EMERALD:
		AddMaterial(Emerald);
		break;
	case JADE:
		AddMaterial(Jade);
		break;
	case OBSIDIAN:
		AddMaterial(Obsidian);
		break;
	case PEARL:
		AddMaterial(Pearl);
		break;
	case PEWTER:
		AddMaterial(Pewter);
		break;
	case RUBY:
		AddMaterial(Ruby);
		break;
	case TURQUOISE:
		AddMaterial(Turquoise);
		break;
	case PLASTIC:
		AddMaterial(Plastic);
		break;
	case RUBBER:
		AddMaterial(Rubber);
		break;
		//Metals
	case ALUMINIUM:
		AddMaterial(Aluminium);
		break;
	case BRASS:
		AddMaterial(Brass);
		break;
	case BRONZE:
		AddMaterial(Bronze);
		break;
	case P_BRONZE:
		AddMaterial(Polished_Bronze);
		break;
	case CHROME:
		AddMaterial(Chrome);
		break;
	case COPPER:
		AddMaterial(Copper);
		break;
	case P_COPPER:
		AddMaterial(Polished_Copper);
		break;
	case GOLD:
		AddMaterial(Gold);
		break;
	case P_GOLD:
		AddMaterial(Polished_Gold);
		break;
	case SILVER:
		AddMaterial(Silver);
		break;
	case P_SILVER:
		AddMaterial(Polished_Silver);
		break;
	case STEEL:
		AddMaterial(Steel);
		break;
	case NONE:
		AddMaterial(None);
		break;
	default:
		AddMaterial(DefaultM);
		break;
	}
}

void CGeometry::ApplyMaterial(const CMaterial* pMaterial)
{
	m_itsMaterial = (*pMaterial);
}

void CGeometry::ApplyMaterial(const CMaterial& Material)
{
	m_itsMaterial = Material;
}

CMaterial CGeometry::GetMaterial() const
{
	return m_itsMaterial;
}
