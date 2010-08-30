#pragma once

#include "Plane.h"

class CFaceTriangles : public CPlane // 三角面
{
private :
	CVertex3D* m_pVertex[6];	  // Vertices (subdivision included)
	CFaceTriangles* m_pFace[3];   // Neighbors
	unsigned int m_IndexMaterial; // Material
	int m_Flag; 				  // Flag (processings)

public :
	// Constructors
	CFaceTriangles();
	CFaceTriangles(CVertex3D* pVertex1, CVertex3D* pVertex2, CVertex3D* pVertex3);
	CFaceTriangles(CFaceTriangles* pFace);
	virtual ~CFaceTriangles()
	{
	}

	// General
	int IsValid();
	void Clear();
	int Equal(CFaceTriangles* pFace);
	void Set(CVertex3D* pVertex1, CVertex3D* pVertex2, CVertex3D* pVertex3);
	void Set(CFaceTriangles* pFace1, CFaceTriangles* pFace2, CFaceTriangles* pFace3);
	void Set(CVertex3D* pVertex1, CVertex3D* pVertex2, CVertex3D* pVertex3, CFaceTriangles* pFace1, CFaceTriangles* pFace2, CFaceTriangles* pFace3);
	void Set(CFaceTriangles* pFace);
	CVertex3D* GetCenter(void);
	CVertex3D* FindNearestVertex(CVertex3D* pVertex);

	// Neighboring
	int BuildArrayNeighbor(CArray3d<CFaceTriangles>* pArrayFace, int depth = 1);

	// Flag
	int GetFlag(void)
	{
		return m_Flag;
	}
	void SetFlag(int flag)
	{
		m_Flag = flag;
	}
	void SetFlagOnVerticesIfDiff(int FlagDiff, int flag);

	// Per vertex
	void v1(CVertex3D* pVertex)
	{
		m_pVertex[0] = pVertex;
	}
	void v2(CVertex3D* pVertex)
	{
		m_pVertex[1] = pVertex;
	}
	void v3(CVertex3D* pVertex)
	{
		m_pVertex[2] = pVertex;
	}
	void v(int index, CVertex3D* pVertex)
	{
		m_pVertex[index % 6] = pVertex;
	}

	int IndexFrom(CVertex3D* pVertex);

	// Per neighboring face
	void f1(CFaceTriangles* pFace)
	{
		m_pFace[0] = pFace;
	}
	void f2(CFaceTriangles* pFace)
	{
		m_pFace[1] = pFace;
	}
	void f3(CFaceTriangles* pFace)
	{
		m_pFace[2] = pFace;
	}
	void f(int index, CFaceTriangles* pFace)
	{
		m_pFace[index % 3] = pFace;
	}

	// Sharp edges
	void ColorSharpEdge(double threshold, CColor& color);
	int GetSharpEdge(double threshold, int* SharpEdge);
	int HasSharpEdge(double threshold);

	// Numbers
	int NbVertex();
	int NbFaceNeighbor();

	// Per vertex
	CVertex3D* v1(void)
	{
		return m_pVertex[0];
	}
	CVertex3D* v2(void)
	{
		return m_pVertex[1];
	}
	CVertex3D* v3(void)
	{
		return m_pVertex[2];
	}
	CVertex3D* v(int index)
	{
		return m_pVertex[index % 6];
	}

	// Per face
	CFaceTriangles* f1(void)
	{
		return m_pFace[0];
	}
	CFaceTriangles* f2(void)
	{
		return m_pFace[1];
	}
	CFaceTriangles* f3(void)
	{
		return m_pFace[2];
	}
	CFaceTriangles* f(int index)
	{
		return m_pFace[index % 3];
	}

	// Find whenever face contain pVertex
	int HasVertex(CVertex3D* pVertex);
	int HasVertex(CVertex3D* pVertex, int* index);
	int HasVertexWithFlag(int flag);

	// Find whenever face contain pFace
	int HasNeighbor(CFaceTriangles* pFace);
	int HasNeighbor(CFaceTriangles* pFace, int* index);
	CFaceTriangles* GetFaceNeighborExclusive(CFaceTriangles* pF0, CFaceTriangles* pF1);
	CFaceTriangles* GetNeighborExclusive(CVertex3D* pVertexHas, CVertex3D* pVertexHasNot);
	CVertex3D* GetVertexExclusive(CVertex3D* pV0, CVertex3D* pV1);
	CVertex3D* GetVertexExclusive(CVertex3D* pV, CFaceTriangles* pFace);
	CVertex3D* GetVertexFaceNeighborExclusive(unsigned int index);
	CVertex3D* GetVertexFaceNeighborExclusive(int index);
	int JointNeighbor(CFaceTriangles** pFace0, CFaceTriangles** pFace1);

	// Vertex sharing
	int Share1Vertex(CFaceTriangles* pFace);
	int Share2Vertex(CFaceTriangles* pFace);
	int Share2Vertex(CFaceTriangles* pFace, int* IndexEdgeThis, int* IndexEdgeOther);
	int Share2Vertex(CFaceTriangles* pFace, CVertex3D** pSharedV1, CVertex3D** pSharedV2);

	// Find whenever face contain pOld, and update it to pNew
	int UpdateVertex(CVertex3D* pOld, CVertex3D* pNew);
	int UpdateNeighbor(CFaceTriangles* pOld, CFaceTriangles* pNew);
	int UpdateVertexRecursive(CVertex3D* pVertexOld, CVertex3D* pVertexNew);

	// Geometric values
	double Area(); 
	double Perimeter();
	double Compacity();

	// 法向量计算
	virtual void CalculateNormal();
};
