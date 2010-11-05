// StlObject.cpp: implementation of the C3DObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "3DObject.h"
#include "Vector3d.h"
#include "Utils3d.h"
#include "GLDisplayContext.h"
#include "C3DDice.h"
#include "C3DSlice.h"
#include "../MainFrm.h"
#include "../IntersectSearchManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
C3DObject::C3DObject()
{
	m_iSelectedPoint = -1;
	m_strObjName = _T("Surface");

	m_ListDone = 0;
	m_Modified = 1;
	m_NormalBinding = NORMAL_PER_FACE;
	m_ColorBinding = COLOR_PER_VERTEX;

	// Texture
	m_IndexTexture = -1;
	m_pTextureCoordinate = NULL;
	m_pTextureCoordinateIndex = NULL;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	m_glObjType = GLSURFACE;	

	GUID guid;
	if (S_OK == ::CoCreateGuid(&guid))
	{
		m_glObjID = guid.Data1;
	}
	else
		m_glObjID = (int)m_glObjType + CGLObject::GetCount();

	SetMaterial(Jade);
}

C3DObject::C3DObject(CReader* r)
{
	m_iSelectedPoint = -1;
	m_strObjName = _T("Surface");

	m_ListDone = 0;
	m_Modified = 1;
	m_NormalBinding = NORMAL_PER_FACE;
	m_ColorBinding = COLOR_PER_VERTEX;

	// Texture
	m_IndexTexture = -1;
	m_pTextureCoordinate = NULL;
	m_pTextureCoordinateIndex = NULL;
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	m_glObjType = GLSURFACE;

	GUID guid;
	if (S_OK == ::CoCreateGuid(&guid))
	{
		m_glObjID = guid.Data1;
	}
	else
		m_glObjID = (int)m_glObjType + CGLObject::GetCount();

	SetMaterial(Jade);

	ComputePoints(r);
	ComputeBoundLimits();
}


C3DObject::~C3DObject()
{	
	Free();
}

CGLObject* C3DObject::Copy()
{
	C3DObject* O = new C3DObject;
	//	O->SetColor(itsRed, itsGreen, itsBlue);
	//	O->SetMaterial(GetMaterial());
	O->SetContext(m_pDisplayContext);
	O->m_glObjID = m_glObjID;
	O->m_strObjName = m_strObjName;
	O->m_glObjType = m_glObjType;

	if( O->GetPointList()->capacity()<m_pointList.size())
		O->GetPointList()->reserve(m_pointList.size());

	O->GetPointList()->resize(m_pointList.size());

	std::copy(m_pointList.begin(), m_pointList.end(), O->GetPointList()->begin());

	//------------------------------------------------------------------------
	if( O->m_indexs.capacity()<m_indexs.size())
		O->m_indexs.reserve(m_indexs.size());

	O->m_indexs.resize(m_indexs.size());

	std::copy(m_indexs.begin(), m_indexs.end(), O->m_indexs.begin());
	//------------------------------------------------------------------------

	//O->ComputeBoundLimits();

	// Vertices
	int Nbvertex = NbVertex();
	O->m_ArrayVertex.SetSize(Nbvertex);
	for (int i = 0; i < Nbvertex; i++)
		O->m_ArrayVertex.SetAt(i, new CVertex3D(O->GetVertex(i)));

	// Faces
	int Nbface = NbFace();
	O->m_ArrayFace.SetSize(Nbface);

	for (int i = 0; i < Nbface; i++)
	{
		CFaceTriangles* pFace = GetFace(i);
		O->m_ArrayFace.SetAt(i, new CFaceTriangles(m_ArrayVertex[IndexFrom(pFace->v1())], m_ArrayVertex[IndexFrom(pFace->v2())], m_ArrayVertex[IndexFrom(pFace->v3())]));
	}
	
	return O;
}


void C3DObject::DefineDisplay()
{
	CWaitCursor wait;

	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	pMF->GetStatusBar().SetPaneText(0, _T("正在渲染图形，请稍等..."));	

	GLdouble bgcol[4];
	glGetDoublev(GL_COLOR_CLEAR_VALUE, bgcol);
	GLfloat specref[] =
	{
		1.0f, 1.0f, 1.0f, 1.0f
	};
	// Enable Depth Testing
	glEnable(GL_DEPTH_TEST);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	// Enable lighting
	glEnable(GL_LIGHTING);

	// Enable color tracking
	glEnable(GL_COLOR_MATERIAL);

	// Set Material properties to follow glColor values
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

//  	GLfloat light_position[] = { 2.0, 2.0, 2.0, 1.0 };
//  	
//  	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specref);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128);

	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	if (m_displayMode == GLSHADED)
	{
		glPushAttrib(GL_LIGHTING_BIT);
		ApplyMaterial();
		DrawShaded();
		glPopAttrib();
	}
	else if (m_displayMode == GLWIREFRAME)
	{
		if( m_pDisplayContext->m_bHLRemoved )
		{
			glPushAttrib(GL_LIGHTING_BIT);
			glDisable(GL_LIGHTING);
			glDisable(GL_COLOR_MATERIAL);
			glEnable(GL_DEPTH_TEST);
			glColor3f(bgcol[0], bgcol[1], bgcol[2]);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1, 1);
			DrawShaded();
			DrawWired();
			glDisable(GL_POLYGON_OFFSET_FILL);
			glPopAttrib();
		}
		else
		{
	 		glPushAttrib(GL_LIGHTING_BIT);
	 		glDisable(GL_LIGHTING);
	 		glDisable(GL_COLOR_MATERIAL);
	 		glEnable(GL_DEPTH_TEST);
	 		DrawWired();
	 		glPopAttrib();
		}
	}

	glDisable(GL_BLEND);

	pMF->GetStatusBar().SetPaneText(0, _T("就绪"));	
}


void C3DObject::Display(const GLDisplayMode& dMode, bool bForce)
{
	m_displayMode = dMode;
	if (!m_Show && !bForce)
		return;

	// Build list at first
	if (!m_ListDone || m_Modified)
	{
		if (!BuildList())
			return;
	}

	// Search for a new list
	if (::glIsList(m_ListOpenGL) == GL_TRUE)
	{
		::glCallList(m_ListOpenGL);
		return;
	}
	else
	{
		TRACE("unable to draw list %d\n", m_ListOpenGL);
		return;
	}
}

bool C3DObject::BuildList()
{
	if (!m_Modified && m_ListDone)
		return false;

	// Erase last list
	::glDeleteLists(m_ListOpenGL, 1);

	// Search for a new list
	m_ListOpenGL = ::glGenLists(1);
	if (m_ListOpenGL == 0)
	{
		TRACE("CGLObject::BuildList : unable to build DrawList\n");
		return false;
	}

	// Start list
	::glNewList(m_ListOpenGL, GL_COMPILE_AND_EXECUTE);

	DefineDisplay(); // 定义显示

	::glEndList();

	// List is done now
	m_ListDone = 1;
	m_Modified = 0;

	return true;
}

void C3DObject::DisplaySelected(const GLSelectedMode& dMode)
{
	Hilight(dMode);
}

void C3DObject::Hilight(const GLSelectedMode& dMode)
{
	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	
	if( dMode == GLEDIT )
		glDisable(GL_DEPTH_TEST);

	glColor3ub(0, 255, 255);
	//glEnable(GL_POLYGON_OFFSET_FILL);
	//glLineWidth(0.1);
	//glPolygonOffset(1, 1);
	//m_itsColor.Set(0, 255, 255);	
	//DrawShaded();
	DrawWired(dMode);
	//glDisable(GL_POLYGON_OFFSET_FILL);
	glPopAttrib();
}

void C3DObject::ComputeBoundLimits()
{
	double dMinX = DBL_MAX,dMinY = DBL_MAX,dMinZ = DBL_MAX, dMaxX = -DBL_MAX, dMaxY = -DBL_MAX, dMaxZ = -DBL_MAX;
	
	CVertex3D P;

	int pCnt = 0;
	std::vector<CVertex3D>::iterator myListIter = m_pointList.begin();
	for (; myListIter!=m_pointList.end(); myListIter++)
	{
		if (pCnt % 4 != 0)
		{
			P = (*myListIter);

			if( P.x < dMinX )
				dMinX = P.x;
			if( (-P.y) < dMinY )
				dMinY = -P.y;
			if( (-P.z) < dMinZ )
				dMinZ = -P.z;

			if( P.x > dMaxX )
				dMaxX = P.x;
			if( (-P.y) > dMaxY )
				dMaxY = -P.y;
			if( (-P.z) > dMaxZ )
				dMaxZ = -P.z;
		}

		pCnt++;
	}

	m_itsBox.SetLimits(dMinX, dMaxX, dMinY, dMaxY, dMinZ, dMaxZ);
}

void C3DObject::DrawWired(const GLSelectedMode& dMode)
{
	unsigned int NbVertex = (unsigned int)m_ArrayVertex.GetSize();
	unsigned int NbFace = (unsigned int)m_ArrayFace.GetSize();

	if(!NbVertex)
		return;
	if(!NbFace)
		return;

	int pCnt = 0;
	CVertex3D curP;
	
	std::vector<CVertex3D>::const_iterator myListIter = m_pointList.begin();	

	if( dMode == GLNOSELECTED )
		glColor3ub(GetColor()->GetR(), GetColor()->GetG(), GetColor()->GetB());

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	

	CBoundingBox box = m_pDisplayContext->GetBoundingBox();
	double xRange = box.XMax()-box.XMin();
	double yRange = box.YMax()-box.YMin();
	double zRange = box.ZMax()-box.ZMin();

	double range = (xRange>yRange?xRange:yRange);

	double xMin, xMax, yMin, yMax;
	xMin = (box.XMin()+box.XMax())/2.0-range/2;
	xMax = (box.XMin()+box.XMax())/2.0+range/2;
	yMin = (box.YMin()+box.YMax())/2.0-range/2;
	yMax = (box.YMin()+box.YMax())/2.0+range/2;
	if( dMode != GLEDIT )	
	{
		glBegin(GL_TRIANGLES);
		CFaceTriangles *pFace;
		for(unsigned int i=0;i<NbFace;i++)
		{

			pFace = m_ArrayFace[i];
			ASSERT(pFace != NULL);
			
			for(int j=0;j<3;j++)
			{				
				::glVertex3f(
					(pFace->v(j)->x-xMin)/range*2.0-1.0,
					(pFace->v(j)->y-yMin)/range*2.0-1.0,
					(pFace->v(j)->z-box.ZMin())/zRange*2.0-1.0);
			}
		}
		glEnd();
	}


	if( dMode == GLEDIT )
	{
		
		for(int i=0; i<NbVertex; i++)
		{
			if( m_iSelectedPoint == i)
			{
				glPointSize(20);
				glColor3ub(255, 0, 0);
			}
			else
			{
				glPointSize(10);
				glColor3ub(0, 255, 255);
			}

			glLoadName(i);
			glEnable(GL_POINT_SMOOTH); 
			glBegin(GL_POINTS);
			glVertex3d(
				(m_ArrayVertex.GetAt(i)->x-xMin)/range*2.0-1.0, 
				(m_ArrayVertex.GetAt(i)->y-yMin)/range*2.0-1.0,
				(m_ArrayVertex.GetAt(i)->z-box.ZMin())/zRange*2.0-1.0);
			glEnd();
			glDisable(GL_POINT_SMOOTH); 
		}
	}
}

void C3DObject::DrawShaded()
{
	unsigned int NbVertex = (unsigned int)m_ArrayVertex.GetSize();
	unsigned int NbFace = (unsigned int)m_ArrayFace.GetSize();

	if(!NbVertex)
		return;
	if(!NbFace)
		return;


	if (m_displayMode == GLSHADED)	// 亮度
		glColor3ub(155, 155, 155);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	CBoundingBox box = m_pDisplayContext->GetBoundingBox();
	double xRange = box.XMax()-box.XMin();
	double yRange = box.YMax()-box.YMin();
	double zRange = box.ZMax()-box.ZMin();

	double range = (xRange>yRange?xRange:yRange);

	double xMin, xMax, yMin, yMax;
	xMin = (box.XMin()+box.XMax())/2.0-range/2;
	xMax = (box.XMin()+box.XMax())/2.0+range/2;
	yMin = (box.YMin()+box.YMax())/2.0-range/2;
	yMax = (box.YMin()+box.YMax())/2.0+range/2;
	
	CFaceTriangles *pFace;
	CVector3D *pVector;	
	
	::glBegin(GL_TRIANGLES);
	for(unsigned int i=0;i<NbFace;i++)
	{

		pFace = m_ArrayFace[i];
		ASSERT(pFace != NULL);
		
		for(int j=0;j<3;j++)
		{
			// Normal
			//if(m_NormalBinding == NORMAL_PER_VERTEX)
			{
				pVector = pFace->v(j)->GetNormal();
				::glNormal3f(pVector->GetX(),pVector->GetY(),pVector->GetZ());
			}

			// Vertex
			::glVertex3f(
				(pFace->v(j)->x-xMin)/range*2.0-1.0,
				(pFace->v(j)->y-yMin)/range*2.0-1.0,
				(pFace->v(j)->z-box.ZMin())/zRange*2.0-1.0);
		}
	}

	::glEnd();

}

void C3DObject::ComputePoints(CReader* r)
{
	if( m_pointList.capacity()<r->m_pointList.size())
		m_pointList.reserve(r->m_pointList.size());

	m_pointList.resize(r->m_pointList.size());
	
	std::copy(r->m_pointList.begin(), r->m_pointList.end(), m_pointList.begin());

//------------------------------------------------------------------------
	if( m_indexs.capacity()<r->m_indexs.size())
		m_indexs.reserve(r->m_indexs.size());

	m_indexs.resize(r->m_indexs.size());

	std::copy(r->m_indexs.begin(), r->m_indexs.end(), m_indexs.begin());

	//------------------------------------------------------------------------

	int size = m_pointList.size();
	for (int i=0;i<size; i++)
	{
		CVertex3D *pVert = new CVertex3D;
		pVert->x = m_pointList[i].x;
		pVert->y = m_pointList[i].y;
		pVert->z = m_pointList[i].z;

		m_pointList[i].y = -m_pointList[i].y;
		m_pointList[i].z = -m_pointList[i].z;
		m_ArrayVertex.Add(pVert);
	}
	
	size = m_indexs.size()/3;
	for (int i=0; i<size; i++)
	{
		CFaceTriangles *face = new CFaceTriangles(m_ArrayVertex.GetAt(m_indexs[i*3]),m_ArrayVertex.GetAt(m_indexs[i*3+1]),m_ArrayVertex.GetAt(m_indexs[i*3+2]));
		m_ArrayFace.Add(face);
	}

	BuildAdjacency();
	CalculateNormalPerFace();
	CalculateNormalPerVertex();
}

//-----------------------------------------------------------------------

//********************************************
// IsValid
//********************************************
int C3DObject::IsValid()
{
	int NbFace = m_ArrayFace.GetSize();
	for (int i = 0; i < NbFace; i++)
		if (!m_ArrayFace[i]->IsValid())
			return 0;
	return 1;
}


//********************************************
// DeleteVertex
//********************************************
int C3DObject::DeleteVertex(CVertex3D* pVertex)
{
	int size = m_ArrayVertex.GetSize();
	for (int i = 0; i < size; i++)
	{
		CVertex3D* pV = m_ArrayVertex[i];
		if (pV == pVertex)
		{
			m_ArrayVertex.RemoveAt(i);
			delete pVertex;
			return 1;
		}
	}
	return 0;
}

//********************************************
// DeleteVertex
//********************************************
int C3DObject::DeleteVertex(int index)
{
	if (index < m_ArrayVertex.GetSize())
	{
		CVertex3D* pVertex = (CVertex3D*)m_ArrayVertex[index];
		m_ArrayVertex.RemoveAt(index);
		delete pVertex;
		return 1;
	}
	return 0;
}

//********************************************
// DeleteFace
//********************************************
int C3DObject::DeleteFaceNbNeighbors(int NbNeighbor)
{
	int deleted = 0;
	for (int i = 0; i < m_ArrayFace.GetSize(); i++)
	{
		CFaceTriangles* pFace = m_ArrayFace[i];
		if (pFace->NbFaceNeighbor() == NbNeighbor)
		{
			m_ArrayFace.RemoveAt(i);
			delete pFace;
			i--;
			deleted++;
		}
	}
	
	return deleted;
}


//////////////////////////////////////////////
// RANGE
//////////////////////////////////////////////

//********************************************
// Range
//********************************************
void C3DObject::Range(int coord, double* min, double* max)
{
	ASSERT(coord >= 0 && coord <= 2);
	int NbVertex = m_ArrayVertex.GetSize();
	double Min = m_ArrayVertex[0]->GetParam(coord);
	double Max = Min;
	for (int i = 1; i < NbVertex; i++)
	{
		double value = m_ArrayVertex[i]->GetParam(coord);
		if (value < Min)
			Min = value;
		if (value > Max)
			Max = value;
	}
	*min = Min;
	*max = Max;
}

//********************************************
// Range (apply)
//********************************************
void C3DObject::Range(int coord, double min, double max)
{
	double Min, Max;
	Range(coord, &Min, &Max);
	Offset(coord, -Min);
	Scale(coord, (max - min) / (Max - Min));
	Offset(coord, min);
}

//********************************************
// Scale
//********************************************
void C3DObject::Scale(int coord, double scale)
{
	int NbVertex = m_ArrayVertex.GetSize();
	for (int i = 0; i < NbVertex; i++)
		m_ArrayVertex[i]->SetParam(coord, m_ArrayVertex[i]->GetParam(coord) * scale);
	m_Modified = 1;
}

//********************************************
// Offset
//********************************************
void C3DObject::Offset(int coord, double offset)
{
	int NbVertex = m_ArrayVertex.GetSize();
	for (int i = 0; i < NbVertex; i++)
		m_ArrayVertex[i]->SetParam(coord, m_ArrayVertex[i]->GetParam(coord) + offset);
	m_Modified = 1;
}


//////////////////////////////////////////////
// PROCESSING
//////////////////////////////////////////////

//********************************************
// BuildAdjacency
// From VertexArray and FaceArray, build
// neighboring vertices and faces, using
// edge and vertex sharing
//********************************************
int C3DObject::BuildAdjacency()
{
	// Check for valid sizes
	int NbVertex = m_ArrayVertex.GetSize();
	int NbFace = m_ArrayFace.GetSize();
	if (NbVertex < 3 || NbFace <= 0)
	{
		TRACE("  invalid array sizes\n");
		return 0;
	}

	for (int i = 0; i < NbFace; i++)
		for (int k = 0; k < 6; k++)
			m_ArrayFace[i]->f(k, NULL);

	for (int i = 0; i < NbVertex; i++)
	{
		m_ArrayVertex[i]->RemoveAllFaceNeighbor();
		m_ArrayVertex[i]->RemoveAllVertexNeighbor();
	}

	//*********************************************
	// For each face, set face neighboring vertices 
	//*********************************************
	//TRACE("  set face neighboring vertices\n");
	for (int i = 0; i < NbFace; i++)
	{
		CFaceTriangles* pFaceCurrent = m_ArrayFace[i];
		for (int j = 0; j < 3; j++)
			pFaceCurrent->v(j)->AddNeighbor(pFaceCurrent);
	}

	//*********************************************
	// For each vertex, set vertex neighboring,
	// just look on neighboring faces 
	//*********************************************
	//TRACE("  set vertices neighboring vertices\n");
	for (int i = 0; i < NbVertex; i++)
	{
		CVertex3D* pVertexCurrent = m_ArrayVertex[i];
		int NbFaceNeighbor = pVertexCurrent->NbFaceNeighbor();
		for (int j = 0; j < NbFaceNeighbor; j++)
		{
			CFaceTriangles* pFace = pVertexCurrent->GetFaceNeighbor(j);
			for (int k = 0; k < 3; k++)
				pVertexCurrent->AddNeighbor(pFace->v(k));
		}
	}

	//*********************************************
	// For each face, set face neighboring,
	// just look on faces neighboring vertices
	//*********************************************
	//TRACE("  set faces neighboring faces\n");
	for (int i = 0; i < NbFace; i++)
	{
		CFaceTriangles* pFaceCurrent = m_ArrayFace[i];
		// For each edge
		for (int j = 0; j < 3; j++)
		{
			CVertex3D* pVertex = pFaceCurrent->v(j);
			CVertex3D* pNextVertex = pFaceCurrent->v((j + 1) % 3);
			int NbFaceNeighbor = pVertex->NbFaceNeighbor();
			for (int k = 0; k < NbFaceNeighbor; k++)
			{
				// This face contain pVertex
				CFaceTriangles* pFace = pVertex->GetFaceNeighbor(k);
				if (pFace != pFaceCurrent)
				{
					if (pFaceCurrent->f(j) == NULL)
					{
						if (pFace->HasVertex(pVertex))
						{
							if (pFace->HasVertex(pNextVertex))
								pFaceCurrent->f(j, pFace);
						}
					}
				}
			}
		}
	}

	return TRUE;
}

//********************************************
// Rebuild
//********************************************
void C3DObject::Rebuild()
{
	BuildAdjacency();
	CalculateNormalPerFace();
	CalculateNormalPerVertex();
	SetModified(1);
}

//********************************************
// IndexFrom
// Return -1 if failed
//********************************************
int C3DObject::IndexFrom(CFaceTriangles* pFace)
{
	ASSERT(pFace != NULL);
	int NbFace = m_ArrayFace.GetSize();
	for (int i = 0; i < NbFace; i++)
		if (m_ArrayFace[i] == pFace)
			return i;
	return -1;
}

//********************************************
// IndexFrom
// Return -1 if failed
//********************************************
int C3DObject::IndexFrom(CVertex3D* pVertex)
{
	int NbVertex = m_ArrayVertex.GetSize();
	for (int i = 0; i < NbVertex; i++)
		if (m_ArrayVertex[i] == pVertex)
			return i;
	return -1;
}

//********************************************
// Move
//********************************************
void C3DObject::Move(double dx, double dy, double dz)
{
	int NbVertex = m_ArrayVertex.GetSize();
	for (int i = 0; i < NbVertex; i++)
		m_ArrayVertex[i]->Move(dx, dy, dz);
	m_Modified = 1;
}


//********************************************
// FindVertexInFaces
//********************************************
int C3DObject::FindVertex(CVertex3D* pVertex)
{
	int find = 0;

	int NbFace = m_ArrayFace.GetSize();
	for (int j = 0; j < NbFace; j++)
	{
		if (m_ArrayFace[j]->HasVertex(pVertex))
		{
			find = 1;
			TRACE("Find vertex [%x] in face %d\n", pVertex, j);
		}
	}

	int NbVertex = m_ArrayVertex.GetSize();
	for (int j = 0; j < NbVertex; j++)
	{
		if (m_ArrayVertex[j] == pVertex)
		{
			find = 1;
			TRACE("Find vertex [%x] at position %d\n", pVertex, j);
		}
	}

	for (int j = 0; j < NbVertex; j++)
	{
		CVertex3D* pV = m_ArrayVertex[j];
		if (pV->HasNeighbor(pVertex))
		{
			find = 1;
			TRACE("Find vertex [%x] in neighbors of vertex %d\n", pVertex, j);
		}
	}

	return find;
}

//********************************************
// FindFace
//********************************************
int C3DObject::FindFace(CFaceTriangles* pFace)
{
	int find = 0;

	int NbFace = m_ArrayFace.GetSize();
	for (int j = 0; j < NbFace; j++)
	{
		CFaceTriangles* pF = m_ArrayFace[j];
		if (pF == pFace)
		{
			find = 1;
			TRACE("Find face [%x] in mesh (index : %d)\n", pFace, j);
		}

		for (int i = 0; i < 3; i++)
		{
			if (pF->f(i) == pFace)
			{
				find = 1;
				TRACE("Find face [%x] in neighbors %d of face %d\n", pFace, i, j);
			}
		}
	}

	return find;
}



//////////////////////////////////////////////
// NORMALS
//////////////////////////////////////////////


//********************************************
// CalculateNormalPerVertex
//********************************************
int C3DObject::CalculateNormalPerVertex(void)
{
	int NbVertex = m_ArrayVertex.GetSize();
	int NbFace = m_ArrayFace.GetSize();

	for (int i = 0; i < NbVertex; i++)
	{
		CVertex3D* pVertex = m_ArrayVertex[i];
		int NbNormal = 0;
		CVector3D vector;
		int NbFaceNeighbor = pVertex->NbFaceNeighbor();
		for (int j = 0; j < NbFaceNeighbor; j++)
		{
			CFaceTriangles* pFace = pVertex->GetFaceNeighbor(j);
			NbNormal += 1;
			vector += pFace->GetNormal();
		}
		if (NbNormal >= 1)
		{
			vector.NormalizeL2();
			pVertex->SetNormal(vector);
		}
	}

	m_Modified = 1;

	return 1;
}

//********************************************
// CalculateNormalPerFace
//********************************************
int C3DObject::CalculateNormalPerFace(void)
{
	int size = m_ArrayFace.GetSize();

	for (int i = 0; i < size; i++)
	{
		CFaceTriangles* pFace = m_ArrayFace[i];
		if (pFace->IsValid())
			pFace->CalculateNormal();
	}
	m_Modified = 1;

	return 1;
}




//********************************************
// ColorSharpEdges
//********************************************
int C3DObject::ColorSharpEdge(double threshold, CColor& color)
{
	int NbFace = m_ArrayFace.GetSize();

	for (int i = 0; i < NbFace; i++)
		m_ArrayFace[i]->ColorSharpEdge(threshold, color);

	return 1;
}


//********************************************
// SetNormalBinding
//********************************************
void C3DObject::SetNormalBinding(NORMALBINDING type)
{
	m_NormalBinding = type;
	m_Modified = 1;
}

//********************************************
// GetNormalBinding
//********************************************
NORMALBINDING C3DObject::GetNormalBinding(void)
{
	return m_NormalBinding;
}

// ********************************************
// SetColor
// ********************************************
// void C3DObject::SetColor(CColor& color)
// {
// 	int size = m_ArrayFace.GetSize();
// 	//	for (int i = 0; i < size; i++)
// 	//		m_ArrayFace[i]->SetColor(color);
// 	m_Modified = 1;
// }
// 
// ********************************************
// SetColor
// ********************************************
// void C3DObject::SetColor(const GLubyte& r, const GLubyte& g, const GLubyte& b)
// {
// 	//	for (int i = 0; i < m_ArrayFace.GetSize(); i++)
// 	//		m_ArrayFace[i]->SetColor(r, g, b);
// 	//	for (int i = 0; i < m_ArrayVertex.GetSize(); i++)
// 	//		m_ArrayVertex[i]->SetColor(r, g, b);
// 	m_Modified = 1;
// }
// 
// void C3DObject::GetColor(GLubyte* colVect) const
// {
// 	/*colVect = new GLubyte[3];
// 	colVect[0] = m_itsColor.GetR();
// 	colVect[1] = m_itsColor.GetG();
// 	colVect[2] = m_itsColor.GetB();
// 	*/
// }
// 
// CColor* C3DObject::GetColor(void)
// {
// 	return CGLObject::GetColor();
// }


//********************************************
// SetFlagOnFaces
//********************************************
void C3DObject::SetFlagOnFaces(int flag)
{
	int size = m_ArrayFace.GetSize();
	for (int i = 0; i < size; i++)
		m_ArrayFace[i]->SetFlag(flag);
}

//********************************************
// GetMaxFlagOnFaces
//********************************************
int C3DObject::GetMaxFlagOnFaces()
{
	int size = m_ArrayFace.GetSize();
	int max = 0;
	for (int i = 0; i < size; i++)
	{
		int tmp = m_ArrayFace[i]->GetFlag();
		max = (tmp > max) ? tmp : max;
	}
	return max;
}

//********************************************
// SetFlagOnVertices
//********************************************
void C3DObject::SetFlagOnVertices(int flag)
{
	int size = m_ArrayVertex.GetSize();
	for (int i = 0; i < size; i++)
		m_ArrayVertex[i]->SetFlag((char)flag);
}



//////////////////////////////////////////////
// SUBDIVISION
//////////////////////////////////////////////

//********************************************
// Alpha
// From Piecewise smooth reconstruction (Hoppe)
//********************************************
double C3DObject::Alpha(int n)
{
	double tmp = 3.0f + 2.0f * (double)cos(2.0f * 3.14159265359f / (double)n);
	double a = 5.0f / 8.0f - (tmp* tmp) / 64.0f;
	return (n * (1 - a) / a);
}


//********************************************
// SubdivisionLoop
// From Piecewise smooth reconstruction (Hoppe)
//********************************************
// (SIGGRAPH 94) -> Charles Loop 
//********************************************
int C3DObject::SubdivisionLoop()
{
	// We assume adjacency is built
	int NbVertex = m_ArrayVertex.GetSize();
	SetFlagOnVertices(0); // for boundaries

	// Create subdivision info (edge vertices)
	int NbFace = m_ArrayFace.GetSize();

	// For each face
	for (int i = 0; i < NbFace; i++)
	{
		CFaceTriangles* pFace = m_ArrayFace[i];

		// Check valid neighboring
		if (!pFace->IsValid())
			continue;

		// On each edge
		for (int IndexEdge = 0; IndexEdge < 3; IndexEdge++)
		{
			// Get IndexEdge on neighbor
			int IndexCurrent, IndexNeighbor;
			CFaceTriangles* pFaceNeighbor = pFace->f(IndexEdge);

			// No neighbor on this edge, it is a boundary edge
			if (pFaceNeighbor == NULL)
			{
				// Two vertices involved
				CVertex3D* pVertex[2];
				pVertex[0] = pFace->v(IndexEdge);
				pVertex[1] = pFace->v((IndexEdge + 1) % 3);
				double x = 0.5f * (pVertex[0]->GetX() + pVertex[1]->GetX());
				double y = 0.5f * (pVertex[0]->GetY() + pVertex[1]->GetY());
				double z = 0.5f * (pVertex[0]->GetZ() + pVertex[1]->GetZ());
				CVertex3D* pNewVertex = new CVertex3D(x, y, z);
				m_ArrayVertex.Add(pNewVertex);
				pFace->v(3 + IndexEdge, pNewVertex);
				// Boundary vertices
				pVertex[0]->SetFlag(1);
				pVertex[1]->SetFlag(1);
			}
			else // std case
			{
				VERIFY(pFace->Share2Vertex(pFaceNeighbor, &IndexCurrent, &IndexNeighbor));
				ASSERT(IndexCurrent == IndexEdge);

				// If neighboring face has been treated, then get vertex
				// and go to next step
				if (pFaceNeighbor->GetFlag())
				{
					CVertex3D* pVertex = pFaceNeighbor->v(3 + (IndexNeighbor % 3));
					pFace->v(3 + (IndexEdge % 3), pVertex);
					continue;
				}

				// Vertex weighting
				// 0 & 1 : weight : 3, 2 & 3 : weight : 1
				CVertex3D* pVertex[4];

				// Weight : 3
				pVertex[0] = pFace->v(IndexEdge);
				pVertex[1] = pFace->v((IndexEdge + 1) % 3);
				// Weight : 1
				pVertex[2] = pFace->v((IndexEdge + 2) % 3);
				pVertex[3] = pFaceNeighbor->v((IndexNeighbor + 2) % 3);

				ASSERT(pVertex[0] != NULL && pVertex[1] != NULL && pVertex[2] != NULL && pVertex[3] != NULL);

				// For each composant
				double x = (3.0f * (pVertex[0]->GetX() + pVertex[1]->GetX()) + pVertex[2]->GetX() + pVertex[3]->GetX()) / 8.0f;
				double y = (3.0f * (pVertex[0]->GetY() + pVertex[1]->GetY()) + pVertex[2]->GetY() + pVertex[3]->GetY()) / 8.0f;
				double z = (3.0f * (pVertex[0]->GetZ() + pVertex[1]->GetZ()) + pVertex[2]->GetZ() + pVertex[3]->GetZ()) / 8.0f;

				// Add vertex to global mesh array, and set face's vertex
				CVertex3D* pNewVertex = new CVertex3D(x, y, z);
				m_ArrayVertex.Add(pNewVertex);
				pFace->v(3 + IndexCurrent, pNewVertex);
			}
		}

		// Set flag
		pFace->SetFlag(1);
	}

	//*****************************
	// Create faces
	//*****************************
	TRACE("  creating faces (%d faces)\n", NbFace);
	// For each valid face
	for (int i = 0; i < NbFace; i++)
	{
		CFaceTriangles* pFace = m_ArrayFace[i];
		pFace->SetFlag(0);

		// Valid face
		int NbVertex = pFace->NbVertex();
		CFaceTriangles* pNewFace;

		switch (NbVertex)
		{
		case 4:
			// Create one face

			// On edge 0
			if (pFace->v(3) != NULL)
			{
				pNewFace = new CFaceTriangles(pFace->v(3), pFace->v(1), pFace->v(2));
				m_ArrayFace.Add(pNewFace);

				// Move current face
				pFace->v(1, pFace->v(3));
			}
			else
				// On edge 1
			if (pFace->v(4) != NULL)
			{
				pNewFace = new CFaceTriangles(pFace->v(0), pFace->v(4), pFace->v(2));
				m_ArrayFace.Add(pNewFace);

				// Move current face
				pFace->v(2, pFace->v(4));
			}
			else
				// On edge 2
			if (pFace->v(5) != NULL)
			{
				pNewFace = new CFaceTriangles(pFace->v(5), pFace->v(1), pFace->v(2));
				m_ArrayFace.Add(pNewFace);

				// Move current face
				pFace->v(2, pFace->v(5));
			}
			break;
		case 5:
			// Create two faces

			// On edge 0 & 2
			if (pFace->v(3) != NULL && pFace->v(5) != NULL)
			{
				pNewFace = new CFaceTriangles(pFace->v(0), pFace->v(3), pFace->v(5));
				m_ArrayFace.Add(pNewFace);
				pNewFace = new CFaceTriangles(pFace->v(5), pFace->v(3), pFace->v(2));
				m_ArrayFace.Add(pNewFace);

				// Move current face
				pFace->v(0, pFace->v(3));
			}
			else
				// On edge 0 & 1
			if (pFace->v(3) != NULL && pFace->v(4) != NULL)
			{
				pNewFace = new CFaceTriangles(pFace->v(3), pFace->v(1), pFace->v(4));
				m_ArrayFace.Add(pNewFace);
				pNewFace = new CFaceTriangles(pFace->v(0), pFace->v(3), pFace->v(4));
				m_ArrayFace.Add(pNewFace);

				// Move current face
				pFace->v(1, pFace->v(4));
			}
			else
				// On edge 1 & 2
			if (pFace->v(4) != NULL && pFace->v(5) != NULL)
			{
				pNewFace = new CFaceTriangles(pFace->v(1), pFace->v(4), pFace->v(5));
				m_ArrayFace.Add(pNewFace);
				pNewFace = new CFaceTriangles(pFace->v(4), pFace->v(2), pFace->v(5));
				m_ArrayFace.Add(pNewFace);

				// Move current face
				pFace->v(2, pFace->v(5));
			}
			break;
		case 6:
			// Create three faces

			// First (v3,v1,v4)
			pNewFace = new CFaceTriangles(pFace->v(3), pFace->v(1), pFace->v(4));
			m_ArrayFace.Add(pNewFace);
			// Second (v3,v4,v5)
			pNewFace = new CFaceTriangles(pFace->v(3), pFace->v(4), pFace->v(5));
			m_ArrayFace.Add(pNewFace);
			// Third (v5,v4,v2)
			pNewFace = new CFaceTriangles(pFace->v(5), pFace->v(4), pFace->v(2));
			m_ArrayFace.Add(pNewFace);

			// Move current face
			pFace->v(1, pFace->v(3));
			pFace->v(2, pFace->v(5));

			break;
		}
		// Remove subdivision info
		for (int k = 3; k < 6; k++)
			pFace->v(k, NULL);
	}
	TRACE("  end creating faces (%d faces)\n", m_ArrayFace.GetSize());


	//*****************************
	// Move original vertices
	//*****************************

	// Copy
	TRACE("  copy\n");
	CArray3d<CVertex3D> ArrayVertex;
	ArrayVertex.SetSize(NbVertex);
	for (int i = 0; i < NbVertex; i++)
		ArrayVertex.SetAt(i, new CVertex3D);

	// For each vertex (at least 2 neighbors)
	for (int i = 0; i < NbVertex; i++)
	{
		CVertex3D* pVertex = m_ArrayVertex[i];
		int n = pVertex->NbVertexNeighbor();

		// Spline on boundaries, we need two vertices
		// remind that the adjacency has not been 
		// updated yet
		// weighting: 1 6 1
		if (pVertex->GetFlag())
		{
			CVertex3D* pv[100];
			int index = 0;
			for (int k = 0; k < n; k++)
			{
				CVertex3D* pNVertex = pVertex->GetVertexNeighbor(k);
				if (pNVertex->GetFlag())
					pv[index++] = pNVertex;
			}
			//ASSERT(index == 2);
			if( index >= 2 )
				ArrayVertex[i]->SetParam(0.125f * (6.0f * pVertex->GetX() + pv[0]->GetX() + pv[1]->GetX()),
				0.125f * (6.0f * pVertex->GetY() + pv[0]->GetY() + pv[1]->GetY()),
				0.125f * (6.0f * pVertex->GetZ() + pv[0]->GetZ() + pv[1]->GetZ()));
		}
		else
		{
			double alpha = Alpha(n);
			double tmp = alpha + (double)n;

			// For each componant
			for (unsigned int j = 0; j < 3; j++)
			{
				double val = alpha* pVertex->GetParam(j);
				for (int k = 0; k < n; k++)
					val += pVertex->GetVertexNeighbor(k)->GetParam(j);
				val /= tmp;
				ArrayVertex[i]->SetParam(j, val);
			}
		}
	}

	// Restore
	for (int i = 0; i < NbVertex; i++)
		for (unsigned int j = 0; j < 3; j++)
			m_ArrayVertex[i]->SetParam(j, ArrayVertex[i]->GetParam(j));

	ArrayVertex.Free();

	// Rebuild adjacency and normals
	BuildAdjacency();
	CalculateNormalPerFace();
	CalculateNormalPerVertex();
	m_Modified = 1;

	return 1;
}


//********************************************
// Subdivision
// Simple : 1->4
//********************************************
int C3DObject::Subdivision(void)
{
	// We assume adjacency is built
	int NbVertex = m_ArrayVertex.GetSize();

	// Create subdivision info (edge vertices)
	int NbFace = m_ArrayFace.GetSize();

	// For each face
	for (int i = 0; i < NbFace; i++)
	{
		CFaceTriangles* pFace = m_ArrayFace[i];

		// On each edge
		for (int IndexEdge = 0; IndexEdge < 3; IndexEdge++)
		{
			// Get IndexEdge on neighbor
			int IndexCurrent, IndexNeighbor;
			CFaceTriangles* pFaceNeighbor = pFace->f(IndexEdge);

			// No neighbor on this edge, go to next
			if (pFaceNeighbor != NULL)
			{
				VERIFY(pFace->Share2Vertex(pFaceNeighbor, &IndexCurrent, &IndexNeighbor));
				ASSERT(IndexCurrent == IndexEdge);

				// If neighboring face has been treated, then get vertex
				// and go to next step
				if (pFaceNeighbor->GetFlag())
				{
					CVertex3D* pVertex = pFaceNeighbor->v(3 + (IndexNeighbor % 3));
					pFace->v(3 + (IndexEdge % 3), pVertex);
					continue;
				}
			}

			// Vertex weighting
			// 0 & 1 : weight : 1
			CVertex3D* pVertex[2];

			// Weight : 1
			pVertex[0] = pFace->v(IndexEdge);
			pVertex[1] = pFace->v((IndexEdge + 1) % 3);

			ASSERT(pVertex[0] != NULL && pVertex[1] != NULL);

			// For each composant
			double coord[3];
			for (int k = 0; k < 3; k++)
				coord[k] = (pVertex[0]->GetParam(k) + pVertex[1]->GetParam(k)) / 2.0f;

			// Add vertex to global mesh array, and set face's vertex
			CVertex3D* pNewVertex = new CVertex3D(coord[0], coord[1], coord[2]);
			m_ArrayVertex.Add(pNewVertex);

			pFace->v(3 + IndexEdge, pNewVertex);
		}

		// Set flag
		pFace->SetFlag(1);
	}

	//*****************************
	// Create faces
	//*****************************
	TRACE("  creating faces (%d faces)\n", NbFace);
	// For each valid face
	for (int i = 0; i < NbFace; i++)
	{
		CFaceTriangles* pFace = m_ArrayFace[i];
		pFace->SetFlag(0);

		// Valid face
		int NbVertex = pFace->NbVertex();
		CFaceTriangles* pNewFace;

		switch (NbVertex)
		{
		case 4:
			// Create one face

			// On edge 0
			if (pFace->v(3) != NULL)
			{
				pNewFace = new CFaceTriangles(pFace->v(3), pFace->v(1), pFace->v(2));
				m_ArrayFace.Add(pNewFace);

				// Move current face
				pFace->v(1, pFace->v(3));
			}
			else
				// On edge 1
			if (pFace->v(4) != NULL)
			{
				pNewFace = new CFaceTriangles(pFace->v(0), pFace->v(4), pFace->v(2));
				m_ArrayFace.Add(pNewFace);

				// Move current face
				pFace->v(2, pFace->v(4));
			}
			else
				// On edge 2
			if (pFace->v(5) != NULL)
			{
				pNewFace = new CFaceTriangles(pFace->v(5), pFace->v(1), pFace->v(2));
				m_ArrayFace.Add(pNewFace);

				// Move current face
				pFace->v(2, pFace->v(5));
			}
			break;
		case 5:
			// Create two faces

			// On edge 0 & 2
			if (pFace->v(3) != NULL && pFace->v(5) != NULL)
			{
				pNewFace = new CFaceTriangles(pFace->v(0), pFace->v(3), pFace->v(5));
				m_ArrayFace.Add(pNewFace);
				pNewFace = new CFaceTriangles(pFace->v(5), pFace->v(3), pFace->v(2));
				m_ArrayFace.Add(pNewFace);

				// Move current face
				pFace->v(0, pFace->v(3));
			}
			else
				// On edge 0 & 1
			if (pFace->v(3) != NULL && pFace->v(4) != NULL)
			{
				pNewFace = new CFaceTriangles(pFace->v(3), pFace->v(1), pFace->v(4));
				m_ArrayFace.Add(pNewFace);
				pNewFace = new CFaceTriangles(pFace->v(0), pFace->v(3), pFace->v(4));
				m_ArrayFace.Add(pNewFace);

				// Move current face
				pFace->v(1, pFace->v(4));
			}
			else
				// On edge 1 & 2
			if (pFace->v(4) != NULL && pFace->v(5) != NULL)
			{
				pNewFace = new CFaceTriangles(pFace->v(1), pFace->v(4), pFace->v(5));
				m_ArrayFace.Add(pNewFace);
				pNewFace = new CFaceTriangles(pFace->v(4), pFace->v(2), pFace->v(5));
				m_ArrayFace.Add(pNewFace);

				// Move current face
				pFace->v(2, pFace->v(5));
			}
			break;
		case 6:
			// Create three faces

			// First (v3,v1,v4)
			pNewFace = new CFaceTriangles(pFace->v(3), pFace->v(1), pFace->v(4));
			m_ArrayFace.Add(pNewFace);
			// Second (v3,v4,v5)
			pNewFace = new CFaceTriangles(pFace->v(3), pFace->v(4), pFace->v(5));
			m_ArrayFace.Add(pNewFace);
			// Third (v5,v4,v2)
			pNewFace = new CFaceTriangles(pFace->v(5), pFace->v(4), pFace->v(2));
			m_ArrayFace.Add(pNewFace);

			// Move current face
			pFace->v(1, pFace->v(3));
			pFace->v(2, pFace->v(5));

			break;
		}
		// Remove subdivision info
		for (int k = 3; k < 6; k++)
			pFace->v(k, NULL);
	}

	// Rebuild adjacency and normals
	BuildAdjacency();
	CalculateNormalPerFace();
	CalculateNormalPerVertex();
	m_Modified = 1;

	return 1;
}

//********************************************
// Smooth
// 30/09/98
//********************************************
int C3DObject::Smooth(int MoveOnBundary /* = 1 */)
{
	// We assume adjacency is built
	int NbVertex = m_ArrayVertex.GetSize();

	// Create subdivision info (edge vertices)
	int NbFace = m_ArrayFace.GetSize();

	//*****************************
	// Move original vertices
	//*****************************

	// Copy
	CArray3d<CVertex3D> ArrayVertex;
	ArrayVertex.SetSize(NbVertex);
	for (int i = 0; i < NbVertex; i++)
		ArrayVertex.SetAt(i, new CVertex3D);

	// For each vertex (at least 3 neighbors)
	for (int i = 0; i < NbVertex; i++)
	{
		CVertex3D* pVertex = m_ArrayVertex[i];

		if (!MoveOnBundary)
			if (pVertex->IsOnBoundary())
			{
				ArrayVertex[i]->SetParam(pVertex);	
				continue;
			}

		int n = pVertex->NbVertexNeighbor();
		double alpha = Alpha(n);
		double tmp = alpha + (double)n;
		// For each composant
		for (unsigned int j = 0; j < 3; j++)
		{
			double val = alpha* pVertex->GetParam(j);
			for (int k = 0; k < n; k++)
				val += pVertex->GetVertexNeighbor(k)->GetParam(j);
			val /= tmp;
			ArrayVertex[i]->SetParam(j, val);
		}
	}

	// Restore
	for (int i = 0; i < NbVertex; i++)
		for (unsigned int j = 0; j < 3; j++)
			m_ArrayVertex[i]->SetParam(j, ArrayVertex[i]->GetParam(j));

	// Cleanup
	ArrayVertex.Free();

	// Rebuild adjacency and normals
	BuildAdjacency();
	CalculateNormalPerFace();
	CalculateNormalPerVertex();
	m_Modified = 1;

	return 1;
}

//////////////////////////////////////////////
// PREDEFINED
//////////////////////////////////////////////

//********************************************
// GenerateBox
//********************************************
int C3DObject::GenerateBox(double dx, double dy, double dz)
{
	TRACE("Generate box...");
	CVertex3D* pVertex;

	pVertex = new CVertex3D(-dx / 2, -dy / 2, -dz / 2);
	m_ArrayVertex.Add(pVertex);

	pVertex = new CVertex3D(-dx / 2, +dy / 2, -dz / 2);
	m_ArrayVertex.Add(pVertex);

	pVertex = new CVertex3D(+dx / 2, +dy / 2, -dz / 2);
	m_ArrayVertex.Add(pVertex);

	pVertex = new CVertex3D(+dx / 2, -dy / 2, -dz / 2);
	m_ArrayVertex.Add(pVertex);

	pVertex = new CVertex3D(-dx / 2, -dy / 2, +dz / 2);
	m_ArrayVertex.Add(pVertex);

	pVertex = new CVertex3D(-dx / 2, +dy / 2, +dz / 2);
	m_ArrayVertex.Add(pVertex);

	pVertex = new CVertex3D(+dx / 2, +dy / 2, +dz / 2);
	m_ArrayVertex.Add(pVertex);

	pVertex = new CVertex3D(+dx / 2, -dy / 2, +dz / 2);
	m_ArrayVertex.Add(pVertex);

	CFaceTriangles* pFace;
	pFace = new CFaceTriangles(m_ArrayVertex[0], m_ArrayVertex[1], m_ArrayVertex[3]);
	pFace->SetNormal(0.0f, 0.0f, -1.0f);
	m_ArrayFace.Add(pFace);

	pFace = new CFaceTriangles(m_ArrayVertex[3], m_ArrayVertex[1], m_ArrayVertex[2]);
	pFace->SetNormal(0.0f, 0.0f, -1.0f);
	m_ArrayFace.Add(pFace);

	pFace = new CFaceTriangles(m_ArrayVertex[0], m_ArrayVertex[4], m_ArrayVertex[1]);
	pFace->SetNormal(-1.0f, 0.0f, 0.0f);
	m_ArrayFace.Add(pFace);

	pFace = new CFaceTriangles(m_ArrayVertex[1], m_ArrayVertex[4], m_ArrayVertex[5]);
	pFace->SetNormal(-1.0f, 0.0f, 0.0f);
	m_ArrayFace.Add(pFace);

	pFace = new CFaceTriangles(m_ArrayVertex[3], m_ArrayVertex[2], m_ArrayVertex[7]);
	m_ArrayFace.Add(pFace);
	pFace->SetNormal(1.0f, 0.0f, 0.0f);

	pFace = new CFaceTriangles(m_ArrayVertex[7], m_ArrayVertex[2], m_ArrayVertex[6]);
	m_ArrayFace.Add(pFace);
	pFace->SetNormal(1.0f, 0.0f, 0.0f);

	pFace = new CFaceTriangles(m_ArrayVertex[4], m_ArrayVertex[0], m_ArrayVertex[3]);
	m_ArrayFace.Add(pFace);
	pFace->SetNormal(0.0f, -1.0f, 0.0f);

	pFace = new CFaceTriangles(m_ArrayVertex[7], m_ArrayVertex[4], m_ArrayVertex[3]);
	m_ArrayFace.Add(pFace);
	pFace->SetNormal(0.0f, -1.0f, 0.0f);

	pFace = new CFaceTriangles(m_ArrayVertex[6], m_ArrayVertex[4], m_ArrayVertex[7]);
	m_ArrayFace.Add(pFace);
	pFace->SetNormal(0.0f, 0.0f, 1.0f);

	pFace = new CFaceTriangles(m_ArrayVertex[6], m_ArrayVertex[5], m_ArrayVertex[4]);
	m_ArrayFace.Add(pFace);
	pFace->SetNormal(0.0f, 0.0f, 1.0f);

	pFace = new CFaceTriangles(m_ArrayVertex[1], m_ArrayVertex[5], m_ArrayVertex[6]);
	m_ArrayFace.Add(pFace);
	pFace->SetNormal(0.0f, 1.0f, 0.0f);

	pFace = new CFaceTriangles(m_ArrayVertex[2], m_ArrayVertex[1], m_ArrayVertex[6]);
	m_ArrayFace.Add(pFace);
	pFace->SetNormal(0.0f, 1.0f, 0.0f);
	
	return 1;
}

//********************************************
// GenerateMap
//********************************************
int C3DObject::GenerateMap(int line, int col, double min, double max)
{
	double x, y, z;
	int i, j;

	// Set vertices
	for (i = 0; i < col; i++)
	{
		for (j = 0; j < line; j++)
		{
			x = min + ((double)i / (double)line) * (max - min);
			z = min + ((double)j / (double)line) * (max - min);
			y = (double)(cos(x) * cos(z));
			m_ArrayVertex.Add(new CVertex3D(x, y, z));
		}
	}

	// Set faces
	for (i = 0; i < col - 1; i++)
	{
		for (j = 0; j < line - 1; j++)
		{
			CVertex3D* pVertex1 = m_ArrayVertex[line* i + j];
			CVertex3D* pVertex2 = m_ArrayVertex[line* i + j + 1];
			CVertex3D* pVertex3 = m_ArrayVertex[line*(i + 1) + j + 1];
			CVertex3D* pVertex4 = m_ArrayVertex[line*(i + 1) + j];

			m_ArrayFace.Add(new CFaceTriangles(pVertex1, pVertex2, pVertex3));
			m_ArrayFace.Add(new CFaceTriangles(pVertex1, pVertex3, pVertex4));
		}
	}

	return 1;
}

//********************************************
// Free
//********************************************
void C3DObject::Free()
{
	m_ArrayVertex.Free();
	m_ArrayFace.Free();

	if (m_pTextureCoordinate != NULL)
		delete[] m_pTextureCoordinate;
	if (m_pTextureCoordinateIndex != NULL)
		delete[] m_pTextureCoordinateIndex;
}

//********************************************
// GenerateMapFromImage
//********************************************
int C3DObject::GenerateMap(CTexture* pTexture, int width, int height, int FlagColor /* = 1 */)
{
	// Cleanup
	Free();

	int WidthImage = pTexture->GetWidth();
	int HeightImage = pTexture->GetHeight();
	unsigned char red, green, blue;

	// Vertices
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			int xImage = (int)((double)i / (double)width*(double)WidthImage);
			int yImage = (int)((double)j / (double)height*(double)HeightImage);
			int index = m_ArrayVertex.Add(new CVertex3D((double)i, (double)pTexture->Grey(xImage, yImage), (double)j));
			if (FlagColor)
			{
				pTexture->Color(xImage, yImage, &red, &green, &blue);
				//				m_ArrayVertex[index]->SetColor(red, green, blue);
			}
		}
	}

	// Faces
	for (int j = 0; j < height - 1; j++)
	{
		for (int i = 0; i < width - 1; i++)
		{
			int index = m_ArrayFace.Add(new CFaceTriangles(m_ArrayVertex[j* width + i + 1], m_ArrayVertex[j* width + i], m_ArrayVertex[(j + 1) * width + i + 1]));
			m_ArrayFace.Add(new CFaceTriangles(m_ArrayVertex[(j + 1) * width + i + 1], m_ArrayVertex[j * width + i], m_ArrayVertex[(j + 1) * width + i]));
			if (FlagColor)
			{
				//				m_ArrayFace[index]->SetColor(*m_ArrayVertex[j * width + i]->GetColor());
				//				m_ArrayFace[index + 1]->SetColor(*m_ArrayVertex[j * width + i + 1]->GetColor());
			}
		}
	}

	// Rebuild
	BuildAdjacency();
	CalculateNormalPerFace();
	CalculateNormalPerVertex();

	return 1;
}

/////////////////////////////////////////////
// INTERSECTION
/////////////////////////////////////////////

//********************************************
// NearestIntersectionWithLine
// Non-optimized
// Nearest -> distance from pV0 to pVertexResult
//********************************************
int C3DObject::NearestIntersectionWithLine(CVertex3D* pV0, CVertex3D* pV1, CVertex3D* pVertexResult, CFaceTriangles** ppFaceResult, int* pNbFaceVisited)
{
	return ::NearestIntersectionWithLine(&m_ArrayFace, pV0, pV1, pVertexResult, ppFaceResult, pNbFaceVisited);
}


//********************************************
// GetMinArea
//********************************************
double C3DObject::GetMinArea(CFaceTriangles** ppFace /* NULL */)
{
	double min = MAX_DOUBLE;
	int size = m_ArrayFace.GetSize();
	for (int i = 0; i < size; i++)
	{
		double area = m_ArrayFace[i]->Area();
		if (area < min)
		{
			min = area;
			if (ppFace != NULL)
				*ppFace = m_ArrayFace[i];
		}
	}
	return min;
}

//********************************************
// GetMeanArea
//********************************************
double C3DObject::GetMeanArea()
{
	return ::GetMeanArea(&m_ArrayFace);
}

//********************************************
// GetFirstVertexWithFlag
//********************************************
double C3DObject::GetMeanLengthEdge()
{
	int size = m_ArrayVertex.GetSize();
	double sum = 0;
	for(int i=0;i<size;i++)
		sum += m_ArrayVertex[i]->GetMeanLengthEdgeAround();
	if(size)
		return (float)sum/size;
	else
		return 0.0f;
}


// Faces
int C3DObject::NbFace()
{
	return m_ArrayFace.GetSize();
}

CFaceTriangles* C3DObject::GetFace(int index)
{
	return m_ArrayFace[index];
}

void C3DObject::AddFace(CFaceTriangles* pFace)
{
	m_ArrayFace.Add(pFace);
}

void C3DObject::SetNbFace(int NbFace)
{
	m_ArrayFace.SetSize(NbFace);
}

CArray3d<CFaceTriangles>* C3DObject::GetArrayFace()
{
	return &m_ArrayFace;
}

int C3DObject::Has(CFaceTriangles* pFace)
{
	return m_ArrayFace.Has(pFace);
}

// Vertices
int C3DObject::NbVertex()
{
	return m_ArrayVertex.GetSize();
}

void C3DObject::SetNbVertex(int NbVertex)
{
	m_ArrayVertex.SetSize(NbVertex);
}

CArray3d<CVertex3D>* C3DObject::GetArrayVertex()
{
	return &m_ArrayVertex;
}

void C3DObject::AddVertex(CVertex3D* pVertex)
{
	m_ArrayVertex.Add(pVertex);
}

CVertex3D* C3DObject::GetVertex(int index)
{
	return m_ArrayVertex[index];
}

int C3DObject::Has(CVertex3D* pVertex)
{
	return m_ArrayVertex.Has(pVertex);
}

// Modif
void C3DObject::SetModified(int flag)
{
	m_Modified = flag;
}

int C3DObject::GetModified()
{
	return m_Modified;
}

// Texture
int C3DObject::GetTextureIndex()
{
	return m_IndexTexture;
}

void C3DObject::Serialize(CArchive& ar)
{
	CGLObject::Serialize(ar);

	if( ar.IsStoring() )
	{
		int size = m_pointList.size();
		ar << size;

		for (int i= 0; i < size; i++)
			ar << m_pointList[i];

		size = m_indexs.size();
		ar << size;

		for (int i=0; i<size; i++)
			ar << m_indexs[i];
	}
	else
	{
		m_pointList.clear();
		m_indexs.clear();

		int size;
		ar >> size;

		for (int i=0; i<size; i++)
		{
			CVertex3D point;
			ar >> point;
			m_pointList.push_back(CVertex3D(point.x, -point.y, -point.z));
		}

		ar >> size;

		for (int i=0; i<size; i++)
		{
			int index;
			ar >> index;
			m_indexs.push_back(index);
		}
		
		ComputeBoundLimits();

		//------------------------------------------------------------------------
		size = m_pointList.size();
		for (int i=0;i<size; i++)
		{
			CVertex3D *pVert = new CVertex3D;
			pVert->x = m_pointList[i].x;
			pVert->y = m_pointList[i].y;
			pVert->z = m_pointList[i].z;

			m_ArrayVertex.Add(pVert);
		}

		size = m_indexs.size()/3;
		for (int i=0; i<size; i++)
		{
			CFaceTriangles *face = new CFaceTriangles(m_ArrayVertex.GetAt(m_indexs[i*3]),m_ArrayVertex.GetAt(m_indexs[i*3+1]),m_ArrayVertex.GetAt(m_indexs[i*3+2]));
			m_ArrayFace.Add(face);
		}

		BuildAdjacency();
		CalculateNormalPerFace();
		CalculateNormalPerVertex();
	}
}

void C3DObject::ReversePoints()
{
	CFaceTriangles *pFace;
	CVector3D *pVector;	

	unsigned int NbVertex = (unsigned int)m_ArrayVertex.GetSize();
	unsigned int NbFace = (unsigned int)m_ArrayFace.GetSize();

	//if(!NbVertex)
	//	return;
	//if(!NbFace)
	//	return;

	for(unsigned int i=0;i<NbFace;i++)
	{

		pFace = m_ArrayFace[i];
		ASSERT(pFace != NULL);

		for(int j=0;j<3;j++)
		{
			pFace->v(j)->y = -pFace->v(j)->y;
			pFace->v(j)->z = -pFace->v(j)->z;
		}
	}

	int size = m_pointList.size();

	for (int i= 0; i < size; i++)
	{
		m_pointList[i].y = -m_pointList[i].y;
		m_pointList[i].z = -m_pointList[i].z;
	}
}

void C3DObject::SaveSurface( const std::string& filename )
{
	CFile file(filename.c_str(), CFile::modeWrite|CFile::typeBinary|CFile::modeCreate);
	CArchive art(&file, CArchive::store);
	int size = m_pointList.size();
	art << size;

	for (int i= 0; i < size; i++)
		art << m_pointList[i];

	size = m_indexs.size();
	art << size;
	for (int i=0; i<size; i++)
		art << m_indexs[i];
	art.Close();
}

void C3DObject::SaveDivideSurface( const std::string& filename, const std::string& newfilename, const CVector3D& size, int index )
{
	if(size.GetNormL2()<0.00001)
	{
		AfxMessageBox("三角形过细");
	}
	if(filename.empty())
	{
		AfxMessageBox("文件名为空");
	}
	double x_min = MAX_DOUBLE;
	double y_min = MAX_DOUBLE;
	double z_min = MAX_DOUBLE;
	for(int i=0; i<(m_indexs.size()/3); i++)
	{
		double dx1 = abs( m_pointList[ m_indexs[i*3] ].x - m_pointList[ m_indexs[i*3+1] ].x);
		double dx2 = abs( m_pointList[ m_indexs[i*3+1] ].x - m_pointList[ m_indexs[i*3+2] ].x);
		double dx3 = abs( m_pointList[ m_indexs[i*3+2] ].x - m_pointList[ m_indexs[i*3] ].x);

		double dy1 = abs( m_pointList[ m_indexs[i*3] ].y - m_pointList[ m_indexs[i*3+1] ].y);
		double dy2 = abs( m_pointList[ m_indexs[i*3+1] ].y - m_pointList[ m_indexs[i*3+2] ].y);
		double dy3 = abs( m_pointList[ m_indexs[i*3+2] ].y - m_pointList[ m_indexs[i*3] ].y);

		double dz1 = abs( m_pointList[ m_indexs[i*3] ].z - m_pointList[ m_indexs[i*3+1] ].z);
		double dz2 = abs( m_pointList[ m_indexs[i*3+1] ].z - m_pointList[ m_indexs[i*3+2] ].z);
		double dz3 = abs( m_pointList[ m_indexs[i*3+2] ].z - m_pointList[ m_indexs[i*3] ].z);

		double dx = (dx1+dx2+dx3)/3;
		double dy = (dy1+dy2+dy3)/3;
		double dz = (dz1+dz2+dz3)/3;
		if(dx < x_min)
			x_min = dx;
		if(dy < y_min)
			y_min = dy;
		if(dz < z_min)
			z_min = dz;
	}
	int dx = x_min/size.GetX();
	int dy = y_min/size.GetY();
	int dz = z_min/size.GetZ();
	int divide = max( max(dx, dy), dz );
	if(divide<=0)
		divide = 1;
	CIntersectSearchManager::Instance()->ExportTriangle(filename, newfilename, divide, index);
}
