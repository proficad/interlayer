#include "stdafx.h"
#include "Edge3D.h"
#include "Vertex3D.h"
#include "Plane.h"
#include "FaceTriangles.h"
#include "Utils3d.h"

//********************************************
// Constructor
//********************************************
CEdge3D::CEdge3D()
{
	m_pVertex1 = NULL;
	m_pVertex2 = NULL;
	m_pFace1 = NULL;
	m_pFace2 = NULL;
	m_Flag = 0;
	m_geomType = GEO_EDGE;
}


//********************************************
// Constructor
//********************************************
CEdge3D::CEdge3D(CVertex3D *pVertex1,
								 CVertex3D *pVertex2)
{
	Set(pVertex1,pVertex2);
	m_pFace1 = NULL;
	m_pFace2 = NULL;
	m_Flag = 0;
	m_geomType = GEO_EDGE;
}

//********************************************
// Constructor
//********************************************
CEdge3D::CEdge3D(CVertex3D *pVertex1,
								 CVertex3D *pVertex2,
								 CPlane *pFace1,
								 CPlane *pFace2)
{
	Set(pVertex1,pVertex2);
	Set(pFace1,pFace2);
	m_Flag = 0;
	m_geomType = GEO_EDGE;
}


//********************************************
// Constructor
//********************************************
CEdge3D::CEdge3D(CEdge3D *pEdge)
{
	Set(pEdge);
	m_Flag = 0;
	m_geomType = GEO_EDGE;
}



//////////////////////////////////////////////
//////////////////////////////////////////////
// DATAS
//////////////////////////////////////////////
//////////////////////////////////////////////

//********************************************
// Set
//********************************************
void CEdge3D::Set(CVertex3D *pVertex1,
									CVertex3D *pVertex2)
{
	m_pVertex1 = pVertex1;
	m_pVertex2 = pVertex2;
}

//********************************************
// Set
//********************************************
void CEdge3D::Set(CPlane *pFace1,
									CPlane *pFace2)
{
	m_pFace1 = pFace1;
	m_pFace2 = pFace2;
}


//********************************************
// Set
//********************************************
void CEdge3D::Set(CEdge3D *pEdge)
{
	Set(pEdge->v1(),pEdge->v2());
	Set(pEdge->f1(),pEdge->f2());
}

//////////////////////////////////////////////
//////////////////////////////////////////////
// PROCESSING
//////////////////////////////////////////////
//////////////////////////////////////////////

//********************************************
// sharp
// threshold : sin(angle)
//********************************************
int CEdge3D::sharp(float threshold)
{
	// Check
	if(m_pFace1 == NULL || 
	   m_pFace2 == NULL)
		return 0;

	return (SinAngle(m_pFace1,m_pFace2) >= threshold);
}

//********************************************
// Equal (order independant)
//********************************************
int CEdge3D::Equal(CEdge3D *pEdge)
{
	int success = 0;
	success += m_pVertex1 == pEdge->v1();
	success += m_pVertex1 == pEdge->v2();
	success += m_pVertex2 == pEdge->v1();
	success += m_pVertex2 == pEdge->v2();
	return (success >= 2);
}


//********************************************
// GetVertexDiff
//********************************************
CVertex3D *CEdge3D::GetVertexDiff(CVertex3D *pVertexDiff)
{
	if(m_pVertex1 != pVertexDiff)
		return m_pVertex1;
	else
	{
		ASSERT(m_pVertex2 != pVertexDiff);
		return m_pVertex2;
	}
}
