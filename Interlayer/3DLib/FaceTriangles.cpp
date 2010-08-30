#include "stdafx.h"
#include "FaceTriangles.h"
#include "Utils3d.h"
#include "MMath.h"

//////////////////////////////////////////////
// CONSTRUCTORS
//////////////////////////////////////////////

//********************************************
// Constructor
//********************************************
CFaceTriangles::CFaceTriangles()
{
	for (int i = 0; i < 3; i++)
		m_pFace[i] = NULL;
	for (int i = 0; i < 6; i++)
		m_pVertex[i] = NULL;
	m_Flag = 0;
	m_geomType = GEO_TRIANGLE;
}

//********************************************
// Constructor
//********************************************
CFaceTriangles::CFaceTriangles(CVertex3D* pVertex1, CVertex3D* pVertex2, CVertex3D* pVertex3) : CPlane(pVertex1, pVertex2, pVertex3)
{
	// Face
	for (int i = 0; i < 3; i++)
		m_pFace[i] = NULL;

	// Vertices
	Set(pVertex1, pVertex2, pVertex3);

	for (int i = 3; i < 6; i++)
		m_pVertex[i] = NULL;

	m_Flag = 0;
	m_geomType = GEO_TRIANGLE;
}


//********************************************
// Constructor
//********************************************
CFaceTriangles::CFaceTriangles(CFaceTriangles* pFace)
{
	Set(pFace);

	m_Flag = 0;
	m_geomType = GEO_TRIANGLE;
}


//////////////////////////////////////////////
// DATAS
//////////////////////////////////////////////

//********************************************
// Set
//********************************************
inline void CFaceTriangles::Clear()
{
	for (int i = 0; i < 3; i++)
		m_pFace[i] = NULL;

	for (int i = 0; i < 6; i++)
		m_pVertex[i] = NULL;

	m_Flag = 0;
}

//********************************************
// Equal
//********************************************
int CFaceTriangles::Equal(CFaceTriangles* pFace)
{
	return (HasVertex(pFace->v1()) && HasVertex(pFace->v2()) && HasVertex(pFace->v3()));
}

//********************************************
// IsValid
//********************************************
int CFaceTriangles::IsValid()
{
	int success = 1;

	success &= (m_pFace[0] != this);
	success &= (m_pFace[1] != this);
	success &= (m_pFace[2] != this);

	// Different neighbors, if there are
	success &= ((m_pFace[0] != m_pFace[1]) || (m_pFace[0] == NULL));
	success &= ((m_pFace[0] != m_pFace[2]) || (m_pFace[0] == NULL));
	success &= ((m_pFace[1] != m_pFace[2]) || (m_pFace[1] == NULL));

#ifdef _DEBUG
	if (!success)
	{
		TRACE("Face [%x] has the same neighbors\n", this);
	}
#endif

	// Different vertices
	success &= (m_pVertex[0] != m_pVertex[1]);
	success &= (m_pVertex[1] != m_pVertex[2]);
	success &= (m_pVertex[0] != m_pVertex[2]);

	/*
	#ifdef _DEBUG
	if(NbFaceNeighbor() != 3)
	{
	TRACE("Face [%x] has %d neighbors\n",NbFaceNeighbor());
	}
	#endif
	*/

#ifdef _DEBUG
	if (!success)
	{
		TRACE("Face [%x] has the same vertices \n", this);
	}
#endif

	// Reciproc. neighboring
	for (int i = 0; i < 3; i++)
	{
		CFaceTriangles* pFace = f(i);
		if (pFace != NULL)
		{
			if (!pFace->HasNeighbor(this))
			{
				TRACE("Face [%x] has invalid reciproc. neighboring \n", this);
				success = 0;
			}
		}
	}

	return success;
}

//********************************************
// Set
//********************************************
inline void CFaceTriangles::Set(CVertex3D* pVertex1, CVertex3D* pVertex2, CVertex3D* pVertex3)
{
	m_pVertex[0] = pVertex1;
	m_pVertex[1] = pVertex2;
	m_pVertex[2] = pVertex3;
}

//********************************************
// Set
//********************************************
inline void CFaceTriangles::Set(CFaceTriangles* pFace1, CFaceTriangles* pFace2, CFaceTriangles* pFace3)
{
	m_pFace[0] = pFace1;
	m_pFace[1] = pFace2;
	m_pFace[2] = pFace3;
}

//********************************************
// Set
//********************************************
inline void CFaceTriangles::Set(CVertex3D* pVertex1, CVertex3D* pVertex2, CVertex3D* pVertex3, CFaceTriangles* pFace1, CFaceTriangles* pFace2, CFaceTriangles* pFace3)
{
	m_pVertex[0] = pVertex1;
	m_pVertex[1] = pVertex2;
	m_pVertex[2] = pVertex3;
	m_pFace[0] = pFace1;
	m_pFace[1] = pFace2;
	m_pFace[2] = pFace3;
}

//********************************************
// Set
//********************************************
inline void CFaceTriangles::Set(CFaceTriangles* pFace)
{
	Set(pFace->v1(), pFace->v2(), pFace->v3());
	Set(pFace->f1(), pFace->f2(), pFace->f3());
}

//********************************************
// SetFlagOnVerticesIfDiff
//********************************************
void CFaceTriangles::SetFlagOnVerticesIfDiff(int FlagDiff, int flag)
{
	for (int i = 0; i < 3; i++)
	{
		CVertex3D* pVertex = v(i);
		if (pVertex->GetFlag() != FlagDiff)
			pVertex->SetFlag((char)flag);
	}
}


//********************************************
// IndexFrom
//********************************************
int CFaceTriangles::IndexFrom(CVertex3D* pVertex)
{
	ASSERT(HasVertex(pVertex));
	for (int i = 0; i < 3; i++)
		if (m_pVertex[i] == pVertex)
			return i;
	return 0;
}


//********************************************
// GetCenter
// Allocate on the heap
//********************************************
CVertex3D* CFaceTriangles::GetCenter(void)
{
	CVertex3D* pVertex = new CVertex3D;
	pVertex->SetX((m_pVertex[0]->GetX() + m_pVertex[1]->GetX() + m_pVertex[2]->GetX()) / 3.0f);
	pVertex->SetY((m_pVertex[0]->GetY() + m_pVertex[1]->GetY() + m_pVertex[2]->GetY()) / 3.0f);
	pVertex->SetZ((m_pVertex[0]->GetZ() + m_pVertex[1]->GetZ() + m_pVertex[2]->GetZ()) / 3.0f);
	return pVertex;
}

//********************************************
// FindNearestVertex
//********************************************
CVertex3D* CFaceTriangles::FindNearestVertex(CVertex3D* pVertex)
{
	CVertex3D* pV = v(0);
	double MinDistance = DistanceSquare(pVertex, v(0));
	for (int i = 1; i < 3; i++)
	{
		double tmp = DistanceSquare(pVertex, v(i));
		if (tmp < MinDistance)
		{
			MinDistance = tmp;
			pV = v(i);
		}
	}
	return pV;
}


//********************************************
// ColorSharpEdge
//********************************************
void CFaceTriangles::ColorSharpEdge(double threshold, CColor& color)
{
	for (int i = 0; i < 3; i++)
	{
		if (SinAngle(this, f(i)) >= threshold)
		{
			v(i)->SetColor(color);
			v((i+1)%3)->SetColor(color);
		}
	}
}

//********************************************
// ColorSharpEdge
//********************************************
int CFaceTriangles::GetSharpEdge(double threshold, int* SharpEdge)
{
	int success = 0;
	for (int i = 0; i < 3; i++)
	{
		if (f(i) != NULL)
		{
			if (SinAngle(this, f(i)) >= threshold)
			{
				SharpEdge[i] = 1;			
				success = 1;
			}
		}
	}
	return success;
}

//********************************************
// HasSharpEdge
//********************************************
int CFaceTriangles::HasSharpEdge(double threshold)
{
	for (int i = 0; i < 3; i++)
	{
		double sinus = SinAngle(this, f(i));
		//TRACE("SinAngle : %g\n",sinus);
		if (sinus >= threshold)
			return 1;
	}
	return 0;
}


//////////////////////////////////////////////
// DATA ACCESS
//////////////////////////////////////////////

//********************************************
// NbVertex
//********************************************
int CFaceTriangles::NbVertex()
{
	int NbVertex = 0;
	for (int i = 0; i < 6; i++)
		NbVertex += (m_pVertex[i] != NULL);
	return NbVertex;
}

//********************************************
// NbFaceNeighbor
//********************************************
int CFaceTriangles::NbFaceNeighbor()
{
	int NbFace = 0;
	NbFace += (m_pFace[0] != NULL);
	NbFace += (m_pFace[1] != NULL);
	NbFace += (m_pFace[2] != NULL);
	return NbFace;
}


//////////////////////////////////////////////
// MISC
//////////////////////////////////////////////

//********************************************
// HasVertex
//********************************************
int CFaceTriangles::HasVertex(CVertex3D* pVertex)
{
	return (m_pVertex[0] == pVertex || m_pVertex[1] == pVertex || m_pVertex[2] == pVertex);
}

//********************************************
// HasVertexWithFlag
//********************************************
int CFaceTriangles::HasVertexWithFlag(int flag)
{
	return (m_pVertex[0]->GetFlag() == flag || m_pVertex[1]->GetFlag() == flag || m_pVertex[2]->GetFlag() == flag);
}

//********************************************
// HasVertex
//********************************************
int CFaceTriangles::HasVertex(CVertex3D* pVertex, int* index)
{
	for (int i = 0; i < 3; i++)
		if (m_pVertex[i] == pVertex)
		{
			*index = i;
			return 1;
		}
	return 0;
}

//********************************************
// HasVertex
//********************************************
int CFaceTriangles::HasNeighbor(CFaceTriangles* pFace)
{
	return (m_pFace[0] == pFace || m_pFace[1] == pFace || m_pFace[2] == pFace);
}


//********************************************
// HasVertex
//********************************************
int CFaceTriangles::HasNeighbor(CFaceTriangles* pFace, int* index)
{
	for (int i = 0; i < 3; i++)
		if (m_pFace[i] == pFace)
		{
			*index = i;
			return 1;
		}
	return 0;
}


//********************************************
// GetNeighborExclusive
// Get neighboring face wich has pVertexHas
// and has not pVertexHasNot
//********************************************
CFaceTriangles* CFaceTriangles::GetNeighborExclusive(CVertex3D* pVertexHas, CVertex3D* pVertexHasNot)
{
	for (int i = 0; i < 3; i++)
	{
		CFaceTriangles* pFace = f(i);
		if (pFace != NULL)
			if (pFace->HasVertex(pVertexHas) && !pFace->HasVertex(pVertexHasNot))
				return pFace;
	}	
	return NULL;
}

//********************************************
// GetVertexExclusive
//********************************************
CVertex3D* CFaceTriangles::GetVertexExclusive(CVertex3D* pV0, CVertex3D* pV1)
{
	for (int i = 0; i < 3; i++)
		if (v(i) != pV0 && v(i) != pV1)
			return v(i);
	return NULL;
}


//********************************************
// GetVertexExclusive
// Return vertex common to this and pFace,
// but pV
//********************************************
CVertex3D* CFaceTriangles::GetVertexExclusive(CVertex3D* pV, CFaceTriangles* pFace)
{
	for (int i = 0; i < 3; i++)
		if (v(i) != pV && pFace->HasVertex(v(i)))
			return v(i);
	return NULL;
}


//********************************************
// GetFaceNeighborExclusive
//********************************************
CFaceTriangles* CFaceTriangles::GetFaceNeighborExclusive(CFaceTriangles* pF0, CFaceTriangles* pF1)
{
	for (int i = 0; i < 3; i++)
		if (f(i) != pF0 && f(i) != pF1)
			return f(i);
	return NULL;
}


//********************************************
// GetVertexFaceNeighborExclusive
// Get vertex on neighboring face, which is not 
// on "this".
// index : neighboring face
//********************************************
CVertex3D* CFaceTriangles::GetVertexFaceNeighborExclusive(unsigned int index)
{
	CFaceTriangles* pFace = m_pFace[index % 3];
	for (int i = 0; i < 3; i++)
		if (!this->HasVertex(pFace->v(i)))
			return pFace->v(i);
	return NULL;
}


//********************************************
// JointNeighbor
//********************************************
int CFaceTriangles::JointNeighbor(CFaceTriangles** pFace0, CFaceTriangles** pFace1)
{
	ASSERT(NbFaceNeighbor() == 2);

	if (NbFaceNeighbor() != 2)
		return 0;

	// Find 2 neighbors
	CFaceTriangles* pFaceNeighbor[2];
	int k = 0;
	for (int i = 0; i < 3; i++)
	{
		if (m_pFace[i] != NULL)
			pFaceNeighbor[k++] = m_pFace[i];
	}
	ASSERT(k == 2);
	if (k != 2)
		return 0;

	ASSERT(pFaceNeighbor[0]->HasNeighbor(this));
	ASSERT(pFaceNeighbor[1]->HasNeighbor(this));
	ASSERT(pFaceNeighbor[0] != this);
	ASSERT(pFaceNeighbor[1] != this);

	*pFace0 = pFaceNeighbor[0];
	*pFace1 = pFaceNeighbor[1];

	pFaceNeighbor[0]->UpdateNeighbor(this, pFaceNeighbor[1]);
	pFaceNeighbor[1]->UpdateNeighbor(this, pFaceNeighbor[0]);

	return 1;
}



//********************************************
// UpdateVertex
//********************************************
int CFaceTriangles::UpdateVertex(CVertex3D* pOld, CVertex3D* pNew)
{
	int index;
	if (HasVertex(pOld, &index))
	{
		v(index, pNew);
		CalculateNormal(); // Update normal
		return 1;
	}
	return 0;
}

//********************************************
// UpdateNeighbor
//********************************************
int CFaceTriangles::UpdateNeighbor(CFaceTriangles* pOld, CFaceTriangles* pNew)
{
	int index;
	if (HasNeighbor(pOld, &index))
	{
		f(index, pNew);
		return 1;
	}
	return 0;
}

//********************************************
// Share2Vertex (exactly)
//********************************************
int CFaceTriangles::Share2Vertex(CFaceTriangles* pFace)
{
	if (pFace == NULL)
		return 0;
	int NbSharedVertex = 0;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			NbSharedVertex += (pFace->v(i) == m_pVertex[j]);
	return (NbSharedVertex == 2);
}

//********************************************
// Share2Vertex (exactly)
// Get sharing edge index info
//********************************************
int CFaceTriangles::Share2Vertex(CFaceTriangles* pFace, int* IndexEdgeThis, int* IndexEdgeOther)
{
	if (!Share2Vertex(pFace))
		return 0;

	int IndexThis[3] =
	{
		0, 0, 0
	};
	int IndexOther[3] =
	{
		0, 0, 0
	};

	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			if (pFace->v(i) == m_pVertex[j])
			{
				IndexThis[j] = 1;
				IndexOther[i] = 1;
			}

	// Set IndexEdges
	*IndexEdgeThis = (IndexThis[0] && IndexThis[1]) ? 0 : (IndexThis[1] && IndexThis[2]) ? 1 : 2;
	*IndexEdgeOther = (IndexOther[0] && IndexOther[1]) ? 0 : (IndexOther[1] && IndexOther[2]) ? 1 : 2;

	// ** DEBUG **
	if (*IndexEdgeThis == 2)
	{
		ASSERT(IndexThis[0] && IndexThis[2]);
	}
	if (*IndexEdgeOther == 2)
	{
		ASSERT(IndexOther[0] && IndexOther[2]);
	}

	return 1;
}

//********************************************
// Share2Vertex (exactly)
// Get sharing edge index info
//********************************************
int CFaceTriangles::Share2Vertex(CFaceTriangles* pFace, CVertex3D** pSharedV1, CVertex3D** pSharedV2)
{
	int EdgeThis;
	int EdgeOther;
	if (!Share2Vertex(pFace))
	{
		*pSharedV1 = NULL;
		*pSharedV2 = NULL;
		return 0;
	}
	Share2Vertex(pFace, &EdgeThis, &EdgeOther);
	*pSharedV1 = v(EdgeThis);
	*pSharedV2 = v((EdgeThis + 1) % 3);
	return 1;
}

//********************************************
// Share1Vertex (exactly)
//********************************************
int CFaceTriangles::Share1Vertex(CFaceTriangles* pFace)
{
	int NbSharedVertex = 0;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			NbSharedVertex += (pFace->v(i) == m_pVertex[j]);
	return (NbSharedVertex == 1);
}


//********************************************
// UpdateVertexRecursive
//********************************************
int CFaceTriangles::UpdateVertexRecursive(CVertex3D* pVertexOld, CVertex3D* pVertexNew)
{
	if (pVertexOld == pVertexNew)
		return 0;
	//TRACE("Update vertex %x in face %x\n",pVertexOld,this);
	this->UpdateVertex(pVertexOld, pVertexNew);
	for (int i = 0; i < 3; i++)
	{
		CFaceTriangles* pFace = f(i);
		if (pFace != NULL)
			if (pFace->HasVertex(pVertexOld))
				pFace->UpdateVertexRecursive(pVertexOld, pVertexNew);
	}
	return 1;
}

//********************************************
// Area
//********************************************
double CFaceTriangles::Area()
{
	return ::Area(m_pVertex[0], m_pVertex[1], m_pVertex[2]);
}

//********************************************
// Perimeter
//********************************************
double CFaceTriangles::Perimeter()
{
	return (::Distance(m_pVertex[0], m_pVertex[1]) + ::Distance(m_pVertex[1], m_pVertex[2]) + ::Distance(m_pVertex[2], m_pVertex[0]));
}

//********************************************
// Compacity
//********************************************
double CFaceTriangles::Compacity()
{
	double perimeter = Perimeter();
	if (perimeter == 0.0f)
	{
		TRACE("CFaceTriangles::Compacity : null triangle\n");
		return 0.0;
	}
	return (4.0 * PI * Area() / (perimeter * perimeter));
}

void CFaceTriangles::CalculateNormal()
{
	CVector3D V1(m_pVertex[0], m_pVertex[1]);
	CVector3D V2(m_pVertex[0], m_pVertex[2]);

	CVector3D Dir = V1 ^ V2;

	CAxisSystem ax(m_pVertex[0], Dir, V1);

	m_itsLocation = ax;

	UpdateEquation();
}