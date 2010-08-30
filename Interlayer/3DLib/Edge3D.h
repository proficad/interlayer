#pragma once
#include "geometry.h"

class CVertex3D;
class CPlane;

/*
         \   /
          \ /
-----------* (1)
			\      面(2)
			 \     
			  \
    面(1)  (2) *------------
			  / \
             /   \
*/

class CEdge3D :	public CGeometry // 边棱
{
private :

	// Geometry
	CVertex3D *m_pVertex1;		// 边棱的两个点
	CVertex3D *m_pVertex2;

	CPlane *m_pFace1;			// 边的两面
	CPlane *m_pFace2;

	// Flag
	char m_Flag;

public :

	CEdge3D();
	virtual ~CEdge3D() {}

	// Constructors
	CEdge3D(CVertex3D *pVertex1,CVertex3D *pVertex2);
	CEdge3D(CVertex3D *pVertex1,CVertex3D *pVertex2,CPlane *pFace1,CPlane *pFace2);
	CEdge3D(CEdge3D *pEdge);

	// Copy Edge
	void Set(CEdge3D *pEdge);

	// Vertices
	void Set(CVertex3D *pVertex1,CVertex3D *pVertex2);
	void v1(CVertex3D *pVertex) { m_pVertex1 = pVertex; }
	void v2(CVertex3D *pVertex) { m_pVertex2 = pVertex; }

	void v(int index,CVertex3D *pVertex)
	{
		if(index == 0)
			m_pVertex1 = pVertex;
		else
			m_pVertex2 = pVertex;
	}

	// Faces
	void Set(CPlane *pFace1,CPlane *pFace2);
	void f1(CPlane *pFace) { m_pFace1 = pFace; }
	void f2(CPlane *pFace) { m_pFace2 = pFace; }
	void f(int index,CPlane *pFace)
	{
		if(index == 0)
			m_pFace1 = pFace;
		else
			m_pFace2 = pFace;
	}

	// Vertices
	CVertex3D *v1(void) { return m_pVertex1; }
	CVertex3D *v2(void) { return m_pVertex2; }
	CVertex3D *v(int index) { return ((index == 0) ? m_pVertex1 : m_pVertex2); }
	CVertex3D *GetVertexDiff(CVertex3D *pVertexDiff);

	// Faces
	CPlane *f1(void) { return m_pFace1; }
	CPlane *f2(void) { return m_pFace2; }
	CPlane *f(int index) { return ((index == 0) ? m_pFace1 : m_pFace2); }

	// Flag
	void SetFlag(char flag) { m_Flag = flag; }
	char GetFlag() { return m_Flag; }

	// Processing
	int Equal(CEdge3D *pEdge);
	int sharp(float threshold); // threshold : sin(angle)
};
