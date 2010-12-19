#include "StdAfx.h"
#include "GridObject.h"
#include "GLDisplayContext.h"
#include "float.h"
#include "../MainFrm.h"
#include "../IntersectSearchManager.h"

extern void StatusBarMessage(char* fmt, ...);
extern void StatusProgress(int nIndex, int nRange=-1);
extern void StatusSetProgress(int nIndex, int nPos);

GLint faceindexes[24] =
{
	3,2,1,0,								//面0
	5,6,7,4,								//面1
	1,5,4,0,								//面2
	2,6,5,1,								//面3
	3,7,6,2,								//面4
	4,7,3,0								//面5
};
//GLint faceindexes[24] =
//{
//	0,1,2,3,								//面0
//	4,7,6,5,								//面1
//	0,4,5,1,								//面2
//	1,5,6,2,								//面3
//	2,6,7,3,								//面4
//	0,3,7,4								//面5
//};

GLfloat postions[72];
GLfloat normals[72];
GLubyte colors[72];

const CPhyPara& CPhyPara::operator=(const CPhyPara& src)
{
	m_strName = src.m_strName;
	m_bShow = src.m_bShow;
	m_dMinValue = src.m_dMinValue;
	m_dMaxValue = src.m_dMaxValue;
	if( src.m_dValue.size() > 0 )
	{
		m_dValue.resize(src.m_dValue.size());
		std::copy(src.m_dValue.begin(), src.m_dValue.end(), m_dValue.begin());
	}

	return *this;
}

void CPhyPara::SavePara( const std::string& filename )
{
	CFile file(filename.c_str(), CFile::modeWrite|CFile::typeBinary|CFile::modeCreate);
	CArchive ar(&file, CArchive::store);

	//ar << m_strName;
	char *tmp = m_strName.GetBuffer();
	for(int i=0; i<8; i++)
	{
		if(i<=(m_strName.GetLength()-1))
		{
			ar << tmp[i];
		}
		else
		{
			ar << '\0';
		}
	}
	ar << m_bShow;	
	ar << m_dMinValue;
	ar << m_dMaxValue;
	ar << I;
	ar << J;
	ar << K;
	int nSize = m_dValue.size();
	ar << nSize;
	for (int i=0; i<nSize; i++)
	{
		ar << m_dValue[i];
	}

	ar.Close();
}

void CPhyPara::LoadPara( const std::string& filename )
{
	//m_dValue.clear();
	CFile file(filename.c_str(), CFile::modeRead|CFile::typeBinary);
	CArchive ar(&file, CArchive::load);

	//ar << m_strName;
	char tmp[8];
	for(int i=0; i<8; i++)
	{
		ar >> tmp[i];
	}
	m_strName = tmp;
	ar >> m_bShow;	
	ar >> m_dMinValue;
	ar >> m_dMaxValue;
	ar >> I;
	ar >> J;
	ar >> K;

	int nSize;
	ar >> nSize;
	if( m_dValue.capacity()<nSize)
		m_dValue.resize(nSize);
	//m_dMinValue = 1.7e+308;
	//m_dMaxValue = -1.7e+308;
	for (int i=0; i<nSize; i++)
	{
		ar >> m_dValue[i];
		//if(m_dValue[i]>m_dMaxValue)
		//	m_dMaxValue = m_dValue[i];
		//if(m_dValue[i]<m_dMinValue)
		//	m_dMinValue = m_dValue[i];
	}

	ar.Close();
}

//------------------------------------------------------------------------

CGridObject::CGridObject(void)
{
	m_strObjName = _T("网格模型");
	m_ListDone = 0;
	m_Modified = 1;
	m_ColorBinding = COLOR_PER_VERTEX;

	m_glObjType = GLSURFACE;
	m_Color.Set(32,255,0,255);
	m_bColorComputed = false;

	GUID guid;
	if (S_OK == ::CoCreateGuid(&guid))
	{
		m_glObjID = guid.Data1;
	}
	else
		m_glObjID = (int)m_glObjType + CGLObject::GetCount();

	I = 0; 
	J = 0;
	K = 0;
	//m_modelItem = NULL;

	SetMaterial(White);
}

CGridObject::~CGridObject(void)
{
	m_vecPhyPara.FreeExtra();
	m_bShowI.FreeExtra();
	m_bShowJ.FreeExtra();
	m_bShowK.FreeExtra();
	m_bChangeK.FreeExtra();
	m_vecPhyPara.RemoveAll();
	m_bShowI.RemoveAll();
	m_bShowJ.RemoveAll();
	m_bShowK.RemoveAll();
	m_bChangeK.RemoveAll();

}

void CGridObject::Add(LPCTSTR lpszName, double dValue)
{
	int nSize = m_vecPhyPara.GetSize();

	CString str = lpszName;
	str.Trim();
	str.MakeUpper();

	for (int i=0; i<nSize; i++)
	{
		if( m_vecPhyPara[i].m_strName == str )
		{
			m_vecPhyPara[i].Add(dValue);
			return;
		}
	}

	CPhyPara param;
	param.m_strName = str;
	param.I = I;
	param.J = J;
	param.K = K;
	param.Add(dValue);
	m_vecPhyPara.Add(param);
}

void CGridObject::Serialize(CArchive &ar)
{
	CGLObject::Serialize(ar);

	if( ar.IsStoring() )
	{
		ar << I;
		for (int i=0; i<I; i++)
		{
			ar << m_bShowI[i];
		}
		ar << J;
		for (int i=0; i<J; i++)
		{
			ar << m_bShowJ[i];
		}
		ar << K;
		for (int i=0; i<K; i++)
		{
			ar << m_bShowK[i];
		}

		//int size = m_pointList.GetSize();
		ar << m_strGUID;
		int size;
		//ar << size;

		//for (int i=0; i<size; i++)
		//{
		//	ar << m_pointList[i];
		//}
		for(int i=0; i<I; i++)
		{
			for(int j=0; j<J; j++)
			{
				for(int k=0; k<K; k++)
				{
					m_gridCells->m_gridCells[i][j][k].Serialize(ar);
				}
			}
		}

		size = m_vecPhyPara.GetSize();

		ar << size;

		for (int i=0; i<size; i++)
			ar << m_vecPhyPara[i];
	}
	else
	{		
		ar >> I;
		for (int i=0; i<I; i++)
		{
			BOOL b;
			ar >> b;
			m_bShowI.Add(b);
		}

		ar >> J;
		for (int i=0; i<J; i++)
		{
			BOOL b;
			ar >> b;
			m_bShowJ.Add(b);
		}

		ar >> K;
		for (int i=0; i<K; i++)
		{
			BOOL b;
			ar >> b;
			m_bShowK.Add(b);
		}
		ar >> m_strGUID;
		int size;
		//ar >> size;
		//m_pointList.SetSize(size);
		
		//if( m_pointList.capacity()<size )
		//	m_pointList.reserve(size);

		//for (int i=0; i<size; i++)
		//{
		//	CVertex3D point;

		//	ar >> point;

		//	//m_pointList[i] = point;
		//	m_pointList.push_back(point);
		//}
		VECTOR_ARRAY3DPTR grid_cells(new GridCells);
		m_gridCells = grid_cells; 
		m_gridCells->m_gridCells.clear();
		for(int i=0; i<I; i++)
		{
			VECTOR_ARRAY2D gridPlane;
			for(int j=0; j<J; j++)
			{
				VECTOR_ARRAY gridline;
				for(int k=0; k<K; k++)
				{
					tagGridModelCellNew cell;

					gridline.push_back(cell);
				}
				gridPlane.push_back(gridline);
			}
			m_gridCells->m_gridCells.push_back(gridPlane);
		}

		for(int i=0; i<I; i++)
		{
			for(int j=0; j<J; j++)
			{
				for(int k=0; k<K; k++)
				{
					m_gridCells->m_gridCells[i][j][k].Serialize(ar);
				}
			}
		}

		ar >> size;
		m_vecPhyPara.SetSize(size);
		for (int i=0; i<size; i++)
		{
			CPhyPara tmp;
			ar >> tmp;
			m_vecPhyPara[i] = tmp;
		}
	}
}

CGLObject* CGridObject::Copy()
{
	CGridObject* O = new CGridObject();
	//O->SetContext(m_pDisplayContext);
	//O->m_glObjID = m_glObjID;
	//O->m_strObjName = m_strObjName;
	//O->m_glObjType = m_glObjType;
	//O->I = I;
	//O->J = J;
	//O->K = K;

	////if( m_pointList.GetSize()>0)
	//if( m_pointList.size()>0)
	//{
	//	//O->m_pointList.Copy(m_pointList);
	//	if( O->m_pointList.capacity()<m_pointList.size() )
	//		O->m_pointList.reserve(m_pointList.size());

	//	std::copy(m_pointList.begin(), m_pointList.end(), O->m_pointList.begin());
	//}

	//if( m_vecPhyPara.GetSize()>0)
	//{
	//	O->m_vecPhyPara.Copy(m_vecPhyPara );
	//}

	//O->ComputeBoundLimits();

	return O;
}


void CGridObject::DefineDisplay()
{	
	//CWaitCursor wait;


	StatusSetProgress(1, m_layerIndex );
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
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1, 1);
		ApplyMaterial();
		DrawShaded();
		DrawWired();
		glDisable(GL_POLYGON_OFFSET_FILL);
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
	/*else if (m_pDisplayContext->m_bHLRemoved)
	{
		glPushAttrib(GL_LIGHTING_BIT);
		glDisable(GL_LIGHTING);
		glDisable(GL_COLOR_MATERIAL);
		glEnable(GL_DEPTH_TEST);
		glColor3f(bgcol[0], bgcol[1], bgcol[2]);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(5, 5);
		DrawShaded();
		DrawWired();
		glDisable(GL_POLYGON_OFFSET_FILL);
		glPopAttrib();
	}*/

	glDisable(GL_BLEND);

	//pMF->GetStatusBar().SetPaneText(0, _T("就绪"));	
}

void CGridObject::Display(const GLDisplayMode& dMode, bool bForce)
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
	for(m_layerIndex=0; m_layerIndex<K; m_layerIndex++)
	{
		if (::glIsList(m_ListOpenGL+m_layerIndex) == GL_TRUE)
		{
			::glCallList(m_ListOpenGL+m_layerIndex);
			continue;
		}
		else
		{
			TRACE("unable to draw list %d\n", m_ListOpenGL);
			return;
		}
	}

}

bool CGridObject::BuildList()
{
	if (!m_Modified && m_ListDone)
		return false;

	// Erase last list
	::glDeleteLists(m_ListOpenGL, K);

	// Search for a new list
	m_ListOpenGL = ::glGenLists(K);
	if (m_ListOpenGL == 0)
	{
		TRACE("CGLObject::BuildList : unable to build DrawList\n");
		return false;
	}
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	pMF->GetStatusBar().SetPaneText(0, _T("正在渲染图形，请稍等..."));	
	StatusProgress(1, K-1);
	for(m_layerIndex=0; m_layerIndex<K; m_layerIndex++)
	{
		::glNewList(m_ListOpenGL+m_layerIndex, GL_COMPILE_AND_EXECUTE);
		
		//if(m_bChangeK[m_layerIndex])
		DefineDisplay(); // 定义显示
		//m_bChangeK[m_layerIndex] = FALSE;
		::glEndList();
	}
	StatusProgress(1);
	// List is done now
	m_ListDone = 1;
	m_Modified = 0;
	pMF->GetStatusBar().SetPaneText(0, _T("就绪"));	
	return true;
}

void CGridObject::DisplaySelected(const GLSelectedMode& dMode)
{
	Hilight(dMode);
}

void CGridObject::Hilight(const GLSelectedMode& dMode)
{
}


void CGridObject::ComputeBoundLimits()
{
	double dMinX = DBL_MAX,dMinY = DBL_MAX,dMinZ = DBL_MAX, dMaxX = -DBL_MAX, dMaxY = -DBL_MAX, dMaxZ = -DBL_MAX;

	CVertex3D P;

	////int size = m_pointList.GetSize();
	//int size = m_pointList.size();

	//for(int i=0; i<size; i++)
	//{
	//	P = m_pointList[i];

	//	if( P.x < dMinX )
	//		dMinX = P.x;
	//	if( P.y < dMinY )
	//		dMinY = P.y;
	//	if( P.z < dMinZ )
	//		dMinZ = P.z;

	//	if( P.x > dMaxX )
	//		dMaxX = P.x;
	//	if( P.y > dMaxY )
	//		dMaxY = P.y;
	//	if( P.z > dMaxZ )
	//		dMaxZ = P.z;
	//}
	for(int i=0; i<I; i++)
	{
		for(int j=0; j<J; j++)
		{
			for(int k=0; k<K; k++)
			{
				for(int index=0; index<8; index++)
				{
					P = m_gridCells->m_gridCells[i][j][k].m_cornerPoint[index];
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
			}
		}
	}
	m_itsBox.SetLimits(dMinX, dMaxX, dMinY, dMaxY, dMinZ, dMaxZ);
}

void CGridObject::DrawWired()
{
	if( m_displayMode == GLSHADED )
		glColor3ub(GetColor()->GetR(), GetColor()->GetG(), GetColor()->GetB());

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_CULL_FACE);
	glCullFace( GL_BACK ); 

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

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

	CPhyPara *pParam = NULL;
	int nSize = m_vecPhyPara.GetSize();
	for (int i=0; i<nSize; i++)
	{
		if( m_vecPhyPara[i].m_bShow )
		{
			pParam = &m_vecPhyPara[i];
			break;
		}
	}

	if( !pParam && m_displayMode != GLSHADED )
	{
		glEnableClientState(GL_COLOR_ARRAY);
	}

	// 判断是否为全显
	bool bFlag = false;
	int nI, nJ, nK;
	int n = 0;
	for (int i=0; i<I; i++)
	{
		if( m_bShowI[i])
			n++;
	}

	nI = n;
	if( n==I)
		bFlag = true;
	else
	{
		n = 0;
		for (int i=0; i<J; i++)
		{
			if( m_bShowJ[i])
				n++;
		}
		nJ = n;
		if( n==J)
			bFlag = true;
		else
		{
			n = 0;
			for (int i=0; i<K; i++)
			{
				if( m_bShowK[i])
					n++;
			}
			nK = n;
			if( n==K)
				bFlag = true;
		}
	}

	if( !bFlag 
		&& nI == 0 
		&& nJ == 0 
		&& nK == 0 )
		bFlag = true;
	//------------------------------------------------------------------------

	if( bFlag )
	{	
		for(int i=0; i<I; i++)
		{
			for(int j=0; j<J; j++)
			{
				int k =m_layerIndex;
				if(pParam)
				{
					double phypara = pParam->m_dValue[k*I*J + j*I + i];
					double phymin = CIntersectSearchManager::Instance()->GetPhyParaMin();
					double phymax = CIntersectSearchManager::Instance()->GetPhyParaMax();
					if((phypara>phymax)||(phypara<phymin))
						continue;
				}

				//for(int k=0; k<K; k++)
				//{

					for(int index=0; index<24; index++)
					{
						postions[index*3] = m_gridCells->m_gridCells[i][j][k].m_cornerPoint[faceindexes[index]].GetX();
						postions[index*3+1] = -m_gridCells->m_gridCells[i][j][k].m_cornerPoint[faceindexes[index]].GetY();
						postions[index*3+2] = -m_gridCells->m_gridCells[i][j][k].m_cornerPoint[faceindexes[index]].GetZ();

						postions[index*3] = (postions[index*3]-xMin)/range*2.0-1.0;
						postions[index*3+1] = (postions[index*3+1]-yMin)/range*2.0-1.0;
						postions[index*3+2] = (postions[index*3+2]-box.ZMin())/zRange*2.0-1.0;

						normals[index*3] = m_gridCells->m_gridCells[i][j][k].m_faceNormals[index>>2].GetX();		// index>>2为面号
						normals[index*3+1] = -m_gridCells->m_gridCells[i][j][k].m_faceNormals[index>>2].GetY();
						normals[index*3+2] = -m_gridCells->m_gridCells[i][j][k].m_faceNormals[index>>2].GetZ();
						if( !pParam && m_displayMode != GLSHADED )
						{
							colors[index*3] = GetRValue(m_gridCells->m_gridCells[i][j][k].m_itsColor[faceindexes[index]]);
							colors[index*3+1] = GetGValue(m_gridCells->m_gridCells[i][j][k].m_itsColor[faceindexes[index]]);
							colors[index*3+2] = GetBValue(m_gridCells->m_gridCells[i][j][k].m_itsColor[faceindexes[index]]);
						}
					}

					glVertexPointer(3,GL_FLOAT,0,postions);
					glNormalPointer(GL_FLOAT,0,normals);

					glDrawArrays(GL_QUADS,0,24);
				//}
			}
		}
	}
	else
	{
		for(int i=0; i<I; i++)
		{
			for(int j=0; j<J; j++)
			{
				int k = m_layerIndex;
				if(pParam)
				{			
					double phypara = pParam->m_dValue[k*I*J + j*I + i];
					double phymin = CIntersectSearchManager::Instance()->GetPhyParaMin();
					double phymax = CIntersectSearchManager::Instance()->GetPhyParaMax();
					if((phypara>phymax)||(phypara<phymin))
						continue;
				}
				
				//for(int k=0; k<K; k++)
				//{
					if( !m_bShowI[i] 
					&& !m_bShowJ[j] 
					&& !m_bShowK[k] )
						continue;

				if( !m_gridCells->m_gridCells[i][j][k].m_bIsGridRefinement)
				{
					for(int index=0; index<24; index++)
					{
						postions[index*3] = m_gridCells->m_gridCells[i][j][k].m_cornerPoint[faceindexes[index]].GetX();
						postions[index*3+1] = -m_gridCells->m_gridCells[i][j][k].m_cornerPoint[faceindexes[index]].GetY();
						postions[index*3+2] = -m_gridCells->m_gridCells[i][j][k].m_cornerPoint[faceindexes[index]].GetZ();

						postions[index*3] = (postions[index*3]-xMin)/range*2.0-1.0;
						postions[index*3+1] = (postions[index*3+1]-yMin)/range*2.0-1.0;
						postions[index*3+2] = (postions[index*3+2]-box.ZMin())/zRange*2.0-1.0;

						normals[index*3] = m_gridCells->m_gridCells[i][j][k].m_faceNormals[index>>2].GetX();		// index>>2为面号
						normals[index*3+1] = -m_gridCells->m_gridCells[i][j][k].m_faceNormals[index>>2].GetY();
						normals[index*3+2] = -m_gridCells->m_gridCells[i][j][k].m_faceNormals[index>>2].GetZ();
						if( !pParam && m_displayMode != GLSHADED )
						{
							colors[index*3] = GetRValue(m_gridCells->m_gridCells[i][j][k].m_itsColor[faceindexes[index]]);
							colors[index*3+1] = GetGValue(m_gridCells->m_gridCells[i][j][k].m_itsColor[faceindexes[index]]);
							colors[index*3+2] = GetBValue(m_gridCells->m_gridCells[i][j][k].m_itsColor[faceindexes[index]]);
						}
					}

					glVertexPointer(3,GL_FLOAT,0,postions);
					glNormalPointer(GL_FLOAT,0,normals);

					glDrawArrays(GL_QUADS,0,24);
				}
				else
				{
					for(vector<tagGridModelCellNew>::iterator it=m_gridCells->m_gridCells[i][j][k].m_subCells.begin(); it!=m_gridCells->m_gridCells[i][j][k].m_subCells.end(); ++it )
					{
						tagGridModelCellNew cell = (*it);
						for(int index=0; index<24; index++)
						{
							postions[index*3] = cell.m_cornerPoint[faceindexes[index]].GetX();
							postions[index*3+1] = -cell.m_cornerPoint[faceindexes[index]].GetY();
							postions[index*3+2] = -cell.m_cornerPoint[faceindexes[index]].GetZ();

							postions[index*3] = (postions[index*3]-xMin)/range*2.0-1.0;
							postions[index*3+1] = (postions[index*3+1]-yMin)/range*2.0-1.0;
							postions[index*3+2] = (postions[index*3+2]-box.ZMin())/zRange*2.0-1.0;

							normals[index*3] = cell.m_faceNormals[index>>2].GetX();		// index>>2为面号
							normals[index*3+1] = -cell.m_faceNormals[index>>2].GetY();
							normals[index*3+2] = -cell.m_faceNormals[index>>2].GetZ();
							if( !pParam && m_displayMode != GLSHADED )
							{
								colors[index*3] = GetRValue(cell.m_itsColor[faceindexes[index]]);
								colors[index*3+1] = GetGValue(cell.m_itsColor[faceindexes[index]]);
								colors[index*3+2] = GetBValue(cell.m_itsColor[faceindexes[index]]);
							}
						}

						glVertexPointer(3,GL_FLOAT,0,postions);
						glNormalPointer(GL_FLOAT,0,normals);

						glDrawArrays(GL_QUADS,0,24);
					}
				}
				//}
			}
		}
	}
	glEnd();
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	if( !pParam && m_displayMode != GLSHADED )
	{
		glDisableClientState(GL_COLOR_ARRAY);
	}
	glDisable(GL_CULL_FACE);
}

void CGridObject::DrawShaded()
{
	if (m_displayMode == GLWIREFRAME)
		glColor3ub(155, 155, 155);
	//glColor4d(0.135000, 0.222500, 0.157500, 0.950000);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	glCullFace( GL_BACK ); 

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

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

	ComputePoints(box);

	CPhyPara *pParam = NULL;
	int nSize = m_vecPhyPara.GetSize();
	for (int i=0; i<nSize; i++)
	{
		if( m_vecPhyPara[i].m_bShow )
		{
			pParam = &m_vecPhyPara[i];
			break;
		}
	}

	if( /*!pParam &&*/ m_displayMode == GLSHADED )
	{
		glEnableClientState(GL_COLOR_ARRAY);
	}

	// 判断是否为全显
	bool bFlag = false;
	int n = 0;
	int nI = 0, nJ = 0, nK = 0;

	for (int i=0; i<I; i++)
	{
		if( m_bShowI[i])
			n++;
	}

	nI = n;
	if( n==I)
		bFlag = true;
	else
	{
		n = 0;
		for (int i=0; i<J; i++)
		{
			if( m_bShowJ[i])
				n++;
		}
		nJ = n;
		if( n==J)
			bFlag = true;
		else
		{
			n = 0;
			for (int i=0; i<K; i++)
			{
				if( m_bShowK[i])
					n++;
			}
			nK = n;
			if( n==K)
				bFlag = true;
		}
	}

	if( !bFlag 
		&& nI == 0 
		&& nJ == 0 
		&& nK == 0 )
		bFlag = true;
	//------------------------------------------------------------------------

	if( bFlag )
	{	
		for(int i=0; i<I; i++)
		{
			for(int j=0; j<J; j++)
			{
				//for(int k=0; k<K; k++)
				//{		
				int k = m_layerIndex;
				if(pParam)
				{
					double phypara = pParam->m_dValue[k*I*J + j*I + i];
					double phymin = CIntersectSearchManager::Instance()->GetPhyParaMin();
					double phymax = CIntersectSearchManager::Instance()->GetPhyParaMax();
					if((phypara>phymax)||(phypara<phymin))
						continue;
				}

				for(int index=0; index<24; index++)
				{
					postions[index*3] = m_gridCells->m_gridCells[i][j][k].m_cornerPoint[faceindexes[index]].GetX();
					postions[index*3+1] = -m_gridCells->m_gridCells[i][j][k].m_cornerPoint[faceindexes[index]].GetY();
					postions[index*3+2] = -m_gridCells->m_gridCells[i][j][k].m_cornerPoint[faceindexes[index]].GetZ();

					postions[index*3] = (postions[index*3]-xMin)/range*2.0-1.0;
					postions[index*3+1] = (postions[index*3+1]-yMin)/range*2.0-1.0;
					postions[index*3+2] = (postions[index*3+2]-box.ZMin())/zRange*2.0-1.0;

					normals[index*3] = m_gridCells->m_gridCells[i][j][k].m_faceNormals[index>>2].GetX();		// index>>2为面号
					normals[index*3+1] = -m_gridCells->m_gridCells[i][j][k].m_faceNormals[index>>2].GetY();
					normals[index*3+2] = -m_gridCells->m_gridCells[i][j][k].m_faceNormals[index>>2].GetZ();
					if( /*!pParam &&*/ m_displayMode == GLSHADED )
					{
						if(pParam)
						{
							COLORREF clr = GetContext()->GetColorTable()->GetColor( (pParam->m_dValue[k*I*J + j*I + i]-pParam->m_dMinValue)/( pParam->m_dMaxValue- pParam->m_dMinValue));
							colors[index*3] = GetRValue(clr);
							colors[index*3+1] = GetGValue(clr);
							colors[index*3+2] = GetBValue(clr);
						}
						else
						{
							colors[index*3] = GetRValue(m_gridCells->m_gridCells[i][j][k].m_itsColor[faceindexes[index]]);
							colors[index*3+1] = GetGValue(m_gridCells->m_gridCells[i][j][k].m_itsColor[faceindexes[index]]);
							colors[index*3+2] = GetBValue(m_gridCells->m_gridCells[i][j][k].m_itsColor[faceindexes[index]]);
						}
					}
				}

				glVertexPointer(3,GL_FLOAT,0,postions);
				glNormalPointer(GL_FLOAT,0,normals);
				if( /*!pParam &&*/ m_displayMode == GLSHADED )
				{
					glColorPointer(3,GL_UNSIGNED_BYTE,0,colors);
				}

				glDrawArrays(GL_QUADS,0,24);
			//}
			}
		}

	}
	else
	{	
		for(int i=0; i<I; i++)
		{
			for(int j=0; j<J; j++)
			{
				int k = m_layerIndex;
				if(pParam)
				{
					double phypara = pParam->m_dValue[k*I*J + j*I + i];
					double phymin = CIntersectSearchManager::Instance()->GetPhyParaMin();
					double phymax = CIntersectSearchManager::Instance()->GetPhyParaMax();
					if((phypara>phymax)||(phypara<phymin))
						continue;
				}
				
				//for(int k=0; k<K; k++)
				//{
					if( !m_bShowI[i] 
					&& !m_bShowJ[j] 
					&& !m_bShowK[k] )
						continue;

					if( !m_gridCells->m_gridCells[i][j][k].m_bIsGridRefinement)
					{
						for(int index=0; index<24; index++)
						{
							postions[index*3] = m_gridCells->m_gridCells[i][j][k].m_cornerPoint[faceindexes[index]].GetX();
							postions[index*3+1] = -m_gridCells->m_gridCells[i][j][k].m_cornerPoint[faceindexes[index]].GetY();
							postions[index*3+2] = -m_gridCells->m_gridCells[i][j][k].m_cornerPoint[faceindexes[index]].GetZ();

							postions[index*3] = (postions[index*3]-xMin)/range*2.0-1.0;
							postions[index*3+1] = (postions[index*3+1]-yMin)/range*2.0-1.0;
							postions[index*3+2] = (postions[index*3+2]-box.ZMin())/zRange*2.0-1.0;

							normals[index*3] = m_gridCells->m_gridCells[i][j][k].m_faceNormals[index>>2].GetX();		// index>>2为面号
							normals[index*3+1] = -m_gridCells->m_gridCells[i][j][k].m_faceNormals[index>>2].GetY();
							normals[index*3+2] = -m_gridCells->m_gridCells[i][j][k].m_faceNormals[index>>2].GetZ();
							if(/* !pParam &&*/ m_displayMode == GLSHADED )
							{
								if(pParam)
								{
									COLORREF clr = GetContext()->GetColorTable()->GetColor( (pParam->m_dValue[k*I*J + j*I + i]-pParam->m_dMinValue)/( pParam->m_dMaxValue- pParam->m_dMinValue));
									colors[index*3] = GetRValue(clr);
									colors[index*3+1] = GetGValue(clr);
									colors[index*3+2] = GetBValue(clr);
								}
								else
								{
									colors[index*3] = GetRValue(m_gridCells->m_gridCells[i][j][k].m_itsColor[faceindexes[index]]);
									colors[index*3+1] = GetGValue(m_gridCells->m_gridCells[i][j][k].m_itsColor[faceindexes[index]]);
									colors[index*3+2] = GetBValue(m_gridCells->m_gridCells[i][j][k].m_itsColor[faceindexes[index]]);
								}
							}
						}

						glVertexPointer(3,GL_FLOAT,0,postions);
						glNormalPointer(GL_FLOAT,0,normals);
						if( /*!pParam && */m_displayMode == GLSHADED )
						{
							glColorPointer(3,GL_UNSIGNED_BYTE,0,colors);
						}
						glDrawArrays(GL_QUADS,0,24);
					}
					else
					{
						for(vector<tagGridModelCellNew>::iterator it=m_gridCells->m_gridCells[i][j][k].m_subCells.begin(); it!=m_gridCells->m_gridCells[i][j][k].m_subCells.end(); ++it )
						{
							tagGridModelCellNew cell = (*it);
							for(int index=0; index<24; index++)
							{
								postions[index*3] = cell.m_cornerPoint[faceindexes[index]].GetX();
								postions[index*3+1] = -cell.m_cornerPoint[faceindexes[index]].GetY();
								postions[index*3+2] = -cell.m_cornerPoint[faceindexes[index]].GetZ();

								postions[index*3] = (postions[index*3]-xMin)/range*2.0-1.0;
								postions[index*3+1] = (postions[index*3+1]-yMin)/range*2.0-1.0;
								postions[index*3+2] = (postions[index*3+2]-box.ZMin())/zRange*2.0-1.0;

								normals[index*3] = cell.m_faceNormals[index>>2].GetX();		// index>>2为面号
								normals[index*3+1] = -cell.m_faceNormals[index>>2].GetY();
								normals[index*3+2] = -cell.m_faceNormals[index>>2].GetZ();
								if( /*!pParam && */m_displayMode == GLSHADED )
								{
									if(pParam)
									{
										COLORREF clr = GetContext()->GetColorTable()->GetColor( (pParam->m_dValue[k*I*J + j*I + i]-pParam->m_dMinValue)/( pParam->m_dMaxValue- pParam->m_dMinValue));
										colors[index*3] = GetRValue(clr);
										colors[index*3+1] = GetGValue(clr);
										colors[index*3+2] = GetBValue(clr);
									}
									else
									{
										colors[index*3] = GetRValue(m_gridCells->m_gridCells[i][j][k].m_itsColor[faceindexes[index]]);
										colors[index*3+1] = GetGValue(m_gridCells->m_gridCells[i][j][k].m_itsColor[faceindexes[index]]);
										colors[index*3+2] = GetBValue(m_gridCells->m_gridCells[i][j][k].m_itsColor[faceindexes[index]]);
									}
								}
							}

							glVertexPointer(3,GL_FLOAT,0,postions);
							glNormalPointer(GL_FLOAT,0,normals);
							if( /*!pParam &&*/ m_displayMode == GLSHADED )
							{
								glColorPointer(3,GL_UNSIGNED_BYTE,0,colors);
							}
							glDrawArrays(GL_QUADS,0,24);
						}
					}
				//}
			}
		}
		
	}
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	if( /*!pParam && */m_displayMode == GLSHADED )
	{
		glDisableClientState(GL_COLOR_ARRAY);
	}
	glDisable(GL_CULL_FACE);
}

void CGridObject::ComputePoints( const CBoundingBox& box )
{
	if(m_bColorComputed)
		return;
	m_bColorComputed = true;
	//CBoundingBox box = m_itsBox;
	double xRange = box.XMax()-box.XMin();
	double yRange = box.YMax()-box.YMin();
	double zRange = box.ZMax()-box.ZMin();

	double range = (xRange>yRange?xRange:yRange);

	double xMin, xMax, yMin, yMax;
	xMin = (box.XMin()+box.XMax())/2.0-range/2;
	xMax = (box.XMin()+box.XMax())/2.0+range/2;
	yMin = (box.YMin()+box.YMax())/2.0-range/2;
	yMax = (box.YMin()+box.YMax())/2.0+range/2;
	for(int i=0; i<I; i++)
	{
		for(int j=0; j<J; j++)
		{
			for(int k=0; k<K; k++)
			{
				for(int index=0; index<8; index++)
				{
					m_gridCells->m_gridCells[i][j][k].m_itsColor[index] = GetContext()->GetColorTable()->GetColor( (-m_gridCells->m_gridCells[i][j][k].m_cornerPoint[index].GetZ()-box.ZMin())/zRange);
				}
			}
		}
	}
	
}

//////////////////////////////////////////////////////////////////////////
//			夹层cell
InterLayerGridObject::InterLayerGridObject( void )
{
	m_strObjName = _T("夹层相交网格");
	m_ListDone = 0;
	m_Modified = 1;
	m_ColorBinding = COLOR_PER_VERTEX;

	m_glObjType = GLINTERLAYERCELL;
	m_Color.Set(32,255,0,255);
	m_bColorComputed = false;

	m_singleColor = 0;
	

	GUID guid;
	if (S_OK == ::CoCreateGuid(&guid))
	{
		m_glObjID = guid.Data1;
	}
	else
		m_glObjID = (int)m_glObjType + CGLObject::GetCount();

	I = 0; 
	J = 0;
	K = 0;

	SetMaterial(White);
}

InterLayerGridObject::~InterLayerGridObject( void )
{

}

CGLObject* InterLayerGridObject::Copy()
{
	CGridObject* O = new CGridObject();
	//O->SetContext(m_pDisplayContext);
	//O->m_glObjID = m_glObjID;
	//O->m_strObjName = m_strObjName;
	//O->m_glObjType = m_glObjType;
	//O->I = I;
	//O->J = J;
	//O->K = K;

	//if( m_pointList.size()>0)
	//{
	//	//O->m_pointList.Copy(m_pointList);
	//	if( O->m_pointList.capacity()<m_pointList.size() )
	//		O->m_pointList.reserve(m_pointList.size());

	//	std::copy(m_pointList.begin(), m_pointList.end(), O->m_pointList.begin());
	//}

	//if( m_vecPhyPara.GetSize()>0)
	//{
	//	O->m_vecPhyPara.Copy(m_vecPhyPara );
	//}

	//O->ComputeBoundLimits();

	return O;
}

void InterLayerGridObject::DefineDisplay()
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
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1, 1);
		ApplyMaterial();
		DrawShaded();
		DrawWired();
		glDisable(GL_POLYGON_OFFSET_FILL);
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

void InterLayerGridObject::Display( const GLDisplayMode& dMode /*= GLWIREFRAME*/, bool bForce /*= false*/ )
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

void InterLayerGridObject::DisplaySelected( const GLSelectedMode&mode /*= GLSELECTED*/ )
{
	Hilight(mode);
}

void InterLayerGridObject::Hilight( const GLSelectedMode& )
{

}

bool InterLayerGridObject::BuildList()
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

void InterLayerGridObject::ComputeBoundLimits()
{
	double dMinX = DBL_MAX,dMinY = DBL_MAX,dMinZ = DBL_MAX, dMaxX = -DBL_MAX, dMaxY = -DBL_MAX, dMaxZ = -DBL_MAX;

	CVertex3D P;

	for(int i=0; i<m_gridCells.size(); i++)
	{
		for(int j=0; j<8; j++)
		{
			if(m_gridCells[i].m_cornerPoint[j].x<dMinX)
				dMinX = m_gridCells[i].m_cornerPoint[j].x;
			if(m_gridCells[i].m_cornerPoint[j].x>dMaxX)
				dMaxX = m_gridCells[i].m_cornerPoint[j].x;

			if( ( -m_gridCells[i].m_cornerPoint[j].y) < dMinY )
				dMinY = -m_gridCells[i].m_cornerPoint[j].y;
			if( (-m_gridCells[i].m_cornerPoint[j].y) > dMaxY)
				dMaxY = -m_gridCells[i].m_cornerPoint[j].y;

			if( (-m_gridCells[i].m_cornerPoint[j].z) < dMinZ )
				dMinZ = -m_gridCells[i].m_cornerPoint[j].z;
			if( (-m_gridCells[i].m_cornerPoint[j].z) > dMaxZ)
				dMaxZ = -m_gridCells[i].m_cornerPoint[j].z;
		}	
	}
	m_itsBox.SetLimits(dMinX, dMaxX, dMinY, dMaxY, dMinZ, dMaxZ);
}

void InterLayerGridObject::Serialize( CArchive& ar )
{
	CGLObject::Serialize(ar);

	if( ar.IsStoring() )
	{
		ar << m_strGUID;
		ar << m_strModelGUID;
		ar << I;
		ar << J;
		ar << K;

		ar <<m_gridCells.size();

		for (int i = 0; i < m_gridCells.size(); i++)
		{
			m_gridCells[i].Serialize(ar);
		}

		int size = m_vecPhyPara.GetSize();

		ar << size;

		for (int i=0; i<size; i++)
			ar << m_vecPhyPara[i];
	}
	else
	{		
		ar >> m_strGUID;
		ar >> m_strModelGUID;
		ar >> I;
		ar >> J;
		ar >> K;

		int size;
		ar >> size;

		for (int i = 0; i < size; i++)
		{
			tagGridModelCellNew cell;
			cell.Serialize(ar);
			m_gridCells.push_back(cell);
		}

		ar >> size;
		m_vecPhyPara.SetSize(size);
		for (int i=0; i<size; i++)
		{
			CPhyPara tmp;
			ar >> tmp;
			m_vecPhyPara[i] = tmp;
		}
	}
}

void InterLayerGridObject::DrawWired()
{
	if( m_displayMode == GLSHADED )
		glColor3ub(GetColor()->GetR(), GetColor()->GetG(), GetColor()->GetB());
	//glColor4d(0.135000, 0.222500, 0.157500, 0.950000);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_CULL_FACE);
	glCullFace( GL_BACK ); 

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

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

	ComputePoints(box);


	if( m_displayMode != GLSHADED )
	{
		glEnableClientState(GL_COLOR_ARRAY);
	}
	CPhyPara *pParam = NULL;
	int nSize = m_vecPhyPara.GetSize();
	for (int i=0; i<nSize; i++)
	{
		if( m_vecPhyPara[i].m_bShow )
		{
			pParam = &m_vecPhyPara[i];
			break;
		}
	}
	//------------------------------------------------------------------------

	for(int i=0; i<m_gridCells.size(); i++)
	{
		if(pParam)
		{
			int _i = m_gridCells[i]._x;
			int _j = m_gridCells[i]._y;
			int _k = m_gridCells[i]._z;
			double phypara = pParam->m_dValue[_k*I*J + _j*I + _i];
			double phymin = CIntersectSearchManager::Instance()->GetPhyParaMin();
			double phymax = CIntersectSearchManager::Instance()->GetPhyParaMax();
			if((phypara>phymax)||(phypara<phymin))
				break;
		}
	
		if( !m_gridCells[i].m_bIsGridRefinement)
		{
			for(int index=0; index<24; index++)
			{
				postions[index*3] = m_gridCells[i].m_cornerPoint[faceindexes[index]].GetX();
				postions[index*3+1] = -m_gridCells[i].m_cornerPoint[faceindexes[index]].GetY();
				postions[index*3+2] = -m_gridCells[i].m_cornerPoint[faceindexes[index]].GetZ();

				postions[index*3] = (postions[index*3]-xMin)/range*2.0-1.0;
				postions[index*3+1] = (postions[index*3+1]-yMin)/range*2.0-1.0;
				postions[index*3+2] = (postions[index*3+2]-box.ZMin())/zRange*2.0-1.0;

				normals[index*3] = m_gridCells[i].m_faceNormals[index>>2].GetX();		// index>>2为面号
				normals[index*3+1] = -m_gridCells[i].m_faceNormals[index>>2].GetY();
				normals[index*3+2] = -m_gridCells[i].m_faceNormals[index>>2].GetZ();
				if( m_displayMode != GLSHADED )
				{
					colors[index*3] = GetRValue(m_gridCells[i].m_itsColor[faceindexes[index]]);
					colors[index*3+1] = GetGValue(m_gridCells[i].m_itsColor[faceindexes[index]]);
					colors[index*3+2] = GetBValue(m_gridCells[i].m_itsColor[faceindexes[index]]);
				}
			}

			glVertexPointer(3,GL_FLOAT,0,postions);
			glNormalPointer(GL_FLOAT,0,normals);

			glDrawArrays(GL_QUADS,0,24);
		}
		else
		{
			for(vector<tagGridModelCellNew>::iterator it=m_gridCells[i].m_subCells.begin(); it!=m_gridCells[i].m_subCells.end(); ++it )
			{
				tagGridModelCellNew cell = (*it);
				for(int index=0; index<24; index++)
				{
					postions[index*3] = cell.m_cornerPoint[faceindexes[index]].GetX();
					postions[index*3+1] = -cell.m_cornerPoint[faceindexes[index]].GetY();
					postions[index*3+2] = -cell.m_cornerPoint[faceindexes[index]].GetZ();

					postions[index*3] = (postions[index*3]-xMin)/range*2.0-1.0;
					postions[index*3+1] = (postions[index*3+1]-yMin)/range*2.0-1.0;
					postions[index*3+2] = (postions[index*3+2]-box.ZMin())/zRange*2.0-1.0;

					normals[index*3] = cell.m_faceNormals[index>>2].GetX();		// index>>2为面号
					normals[index*3+1] = -cell.m_faceNormals[index>>2].GetY();
					normals[index*3+2] = -cell.m_faceNormals[index>>2].GetZ();
					if( m_displayMode != GLSHADED )
					{
						colors[index*3] = GetRValue(cell.m_itsColor[faceindexes[index]]);
						colors[index*3+1] = GetGValue(cell.m_itsColor[faceindexes[index]]);
						colors[index*3+2] = GetBValue(cell.m_itsColor[faceindexes[index]]);
					}
				}

				glVertexPointer(3,GL_FLOAT,0,postions);
				glNormalPointer(GL_FLOAT,0,normals);

				glDrawArrays(GL_QUADS,0,24);
			}
		}
		//for(int index=0; index<24; index++)
		//{
		//	postions[index*3] = m_gridCells[i].m_cornerPoint[faceindexes[index]].GetX();
		//	postions[index*3+1] = -m_gridCells[i].m_cornerPoint[faceindexes[index]].GetY();
		//	postions[index*3+2] = -m_gridCells[i].m_cornerPoint[faceindexes[index]].GetZ();

		//	postions[index*3] = (postions[index*3]-xMin)/range*2.0-1.0;
		//	postions[index*3+1] = (postions[index*3+1]-yMin)/range*2.0-1.0;
		//	postions[index*3+2] = (postions[index*3+2]-box.ZMin())/zRange*2.0-1.0;

		//	normals[index*3] = m_gridCells[i].m_faceNormals[index>>2].GetX();		// index>>2为面号
		//	normals[index*3+1] = -m_gridCells[i].m_faceNormals[index>>2].GetY();
		//	normals[index*3+2] = -m_gridCells[i].m_faceNormals[index>>2].GetZ();
		//	if( m_displayMode == GLSHADED )
		//	{
		//		colors[index*3] = GetRValue(m_gridCells[i].m_itsColor[faceindexes[index]]);
		//		colors[index*3+1] = GetGValue(m_gridCells[i].m_itsColor[faceindexes[index]]);
		//		colors[index*3+2] = GetBValue(m_gridCells[i].m_itsColor[faceindexes[index]]);
		//	}
		//}

		//glVertexPointer(3,GL_FLOAT,0,postions);
		//glNormalPointer(GL_FLOAT,0,normals);
		//if( m_displayMode != GLSHADED )
		//{
		//	glColorPointer(3,GL_UNSIGNED_BYTE,0,colors);
		//}

		//glDrawArrays(GL_QUADS,0,24);
	}


	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	if( m_displayMode != GLSHADED )
	{
		glDisableClientState(GL_COLOR_ARRAY);
	}
	glDisable(GL_CULL_FACE);
}

void InterLayerGridObject::ComputePoints( const CBoundingBox& box )
{
	if(m_bColorComputed)
		return;
	m_bColorComputed = true;
	//CBoundingBox box = m_itsBox;
	double xRange = box.XMax()-box.XMin();
	double yRange = box.YMax()-box.YMin();
	double zRange = box.ZMax()-box.ZMin();

	double range = (xRange>yRange?xRange:yRange);

	double xMin, xMax, yMin, yMax;
	xMin = (box.XMin()+box.XMax())/2.0-range/2;
	xMax = (box.XMin()+box.XMax())/2.0+range/2;
	yMin = (box.YMin()+box.YMax())/2.0-range/2;
	yMax = (box.YMin()+box.YMax())/2.0+range/2;
	//int r_color = rand()%255;
	//int g_color = rand()%255;
	int b_color = rand()%m_gridCells.size();

	//m_singleColor = r_color+g_color<<8 + b_color<<16;
	for(int i=0; i<m_gridCells.size(); i++)
	{
		for(int index=0; index<8; index++)
		{
			m_gridCells[i].m_itsColor[index] = GetContext()->GetColorTable()->GetColor( (-m_gridCells[b_color].m_cornerPoint[0].GetZ()-box.ZMin())/zRange);
		}
	}

}

void InterLayerGridObject::DrawShaded()
{
	if (m_displayMode == GLWIREFRAME)
		glColor3ub(155, 155, 155);
	//glColor4d(0.135000, 0.222500, 0.157500, 0.950000);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	glCullFace( GL_BACK ); 

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

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

	ComputePoints(box);

	CPhyPara *pParam = NULL;
	int nSize = m_vecPhyPara.GetSize();
	for (int i=0; i<nSize; i++)
	{
		if( m_vecPhyPara[i].m_bShow )
		{
			pParam = &m_vecPhyPara[i];
			break;
		}
	}

	if( m_displayMode == GLSHADED )
	{
		glEnableClientState(GL_COLOR_ARRAY);
	}

	//------------------------------------------------------------------------

	for(int i=0; i<m_gridCells.size(); i++)
	{
		if(pParam)
		{
			int _i = m_gridCells[i]._x;
			int _j = m_gridCells[i]._y;
			int _k = m_gridCells[i]._z;
			double phypara = pParam->m_dValue[_k*I*J + _j*I + _i];
			double phymin = CIntersectSearchManager::Instance()->GetPhyParaMin();
			double phymax = CIntersectSearchManager::Instance()->GetPhyParaMax();
			if((phypara>phymax)||(phypara<phymin))
				continue;
		}
	
		if( !m_gridCells[i].m_bIsGridRefinement)
		{
			for(int index=0; index<24; index++)
			{
				postions[index*3] = m_gridCells[i].m_cornerPoint[faceindexes[index]].GetX();
				postions[index*3+1] = -m_gridCells[i].m_cornerPoint[faceindexes[index]].GetY();
				postions[index*3+2] = -m_gridCells[i].m_cornerPoint[faceindexes[index]].GetZ();

				postions[index*3] = (postions[index*3]-xMin)/range*2.0-1.0;
				postions[index*3+1] = (postions[index*3+1]-yMin)/range*2.0-1.0;
				postions[index*3+2] = (postions[index*3+2]-box.ZMin())/zRange*2.0-1.0;

				normals[index*3] = m_gridCells[i].m_faceNormals[index>>2].GetX();		// index>>2为面号
				normals[index*3+1] = -m_gridCells[i].m_faceNormals[index>>2].GetY();
				normals[index*3+2] = -m_gridCells[i].m_faceNormals[index>>2].GetZ();
				if(  m_displayMode == GLSHADED )
				{
					if(pParam)
					{
						int x = m_gridCells[i]._x;		
						int y = m_gridCells[i]._y;
						int z = m_gridCells[i]._z;
						COLORREF clr = GetContext()->GetColorTable()->GetColor( (pParam->m_dValue[z*I*J + y*I + x]-pParam->m_dMinValue)/( pParam->m_dMaxValue- pParam->m_dMinValue));
						colors[index*3] = GetRValue(clr);
						colors[index*3+1] = GetGValue(clr);
						colors[index*3+2] = GetBValue(clr);
					}
					else
					{
						colors[index*3] = GetRValue(m_gridCells[i].m_itsColor[faceindexes[index]]);
						colors[index*3+1] = GetGValue(m_gridCells[i].m_itsColor[faceindexes[index]]);
						colors[index*3+2] = GetBValue(m_gridCells[i].m_itsColor[faceindexes[index]]);
					}
					//colors[index*3] = GetRValue(m_gridCells[i].m_itsColor[faceindexes[index]]);
					//colors[index*3+1] = GetGValue(m_gridCells[i].m_itsColor[faceindexes[index]]);
					//colors[index*3+2] = GetBValue(m_gridCells[i].m_itsColor[faceindexes[index]]);
				}
			}

			glVertexPointer(3,GL_FLOAT,0,postions);
			glNormalPointer(GL_FLOAT,0,normals);
			if( m_displayMode == GLSHADED )
			{
				glColorPointer(3,GL_UNSIGNED_BYTE,0,colors);
			}
			glDrawArrays(GL_QUADS,0,24);
		}
		else
		{
			for(vector<tagGridModelCellNew>::iterator it=m_gridCells[i].m_subCells.begin(); it!=m_gridCells[i].m_subCells.end(); ++it )
			{
				if(pParam)
				{
					int _i = m_gridCells[i]._x;
					int _j = m_gridCells[i]._y;
					int _k = m_gridCells[i]._z;
					double phypara = pParam->m_dValue[_k*I*J + _j*I + _i];
					double phymin = CIntersectSearchManager::Instance()->GetPhyParaMin();
					double phymax = CIntersectSearchManager::Instance()->GetPhyParaMax();
					if((phypara>phymax)||(phypara<phymin))
						continue;
				}

				tagGridModelCellNew cell = (*it);
				for(int index=0; index<24; index++)
				{
					postions[index*3] = cell.m_cornerPoint[faceindexes[index]].GetX();
					postions[index*3+1] = -cell.m_cornerPoint[faceindexes[index]].GetY();
					postions[index*3+2] = -cell.m_cornerPoint[faceindexes[index]].GetZ();

					postions[index*3] = (postions[index*3]-xMin)/range*2.0-1.0;
					postions[index*3+1] = (postions[index*3+1]-yMin)/range*2.0-1.0;
					postions[index*3+2] = (postions[index*3+2]-box.ZMin())/zRange*2.0-1.0;

					normals[index*3] = cell.m_faceNormals[index>>2].GetX();		// index>>2为面号
					normals[index*3+1] = -cell.m_faceNormals[index>>2].GetY();
					normals[index*3+2] = -cell.m_faceNormals[index>>2].GetZ();
					if(  m_displayMode == GLSHADED )
					{
						if(pParam)
						{
							int x = m_gridCells[i]._x;		
							int y = m_gridCells[i]._y;
							int z = m_gridCells[i]._z;
							COLORREF clr = GetContext()->GetColorTable()->GetColor( (pParam->m_dValue[z*I*J + y*I + x]-pParam->m_dMinValue)/( pParam->m_dMaxValue- pParam->m_dMinValue));
							colors[index*3] = GetRValue(clr);
							colors[index*3+1] = GetGValue(clr);
							colors[index*3+2] = GetBValue(clr);
						}
						else
						{
							colors[index*3] = GetRValue(m_gridCells[i].m_itsColor[faceindexes[index]]);
							colors[index*3+1] = GetGValue(m_gridCells[i].m_itsColor[faceindexes[index]]);
							colors[index*3+2] = GetBValue(m_gridCells[i].m_itsColor[faceindexes[index]]);
						}
					}
				}

				glVertexPointer(3,GL_FLOAT,0,postions);
				glNormalPointer(GL_FLOAT,0,normals);
				if( m_displayMode == GLSHADED )
				{
					glColorPointer(3,GL_UNSIGNED_BYTE,0,colors);
				}
				glDrawArrays(GL_QUADS,0,24);
			}
		}
		//for(int index=0; index<24; index++)
		//{
		//	postions[index*3] = m_gridCells[i].m_cornerPoint[faceindexes[index]].GetX();
		//	postions[index*3+1] = -m_gridCells[i].m_cornerPoint[faceindexes[index]].GetY();
		//	postions[index*3+2] = -m_gridCells[i].m_cornerPoint[faceindexes[index]].GetZ();

		//	postions[index*3] = (postions[index*3]-xMin)/range*2.0-1.0;
		//	postions[index*3+1] = (postions[index*3+1]-yMin)/range*2.0-1.0;
		//	postions[index*3+2] = (postions[index*3+2]-box.ZMin())/zRange*2.0-1.0;

		//	normals[index*3] = m_gridCells[i].m_faceNormals[index>>2].GetX();		// index>>2为面号
		//	normals[index*3+1] = -m_gridCells[i].m_faceNormals[index>>2].GetY();
		//	normals[index*3+2] = -m_gridCells[i].m_faceNormals[index>>2].GetZ();
		//	if( m_displayMode == GLSHADED )
		//	{
		//		colors[index*3] = GetRValue(m_gridCells[i].m_itsColor[faceindexes[index]]);
		//		colors[index*3+1] = GetGValue(m_gridCells[i].m_itsColor[faceindexes[index]]);
		//		colors[index*3+2] = GetBValue(m_gridCells[i].m_itsColor[faceindexes[index]]);
		//	}
		//}

		//glVertexPointer(3,GL_FLOAT,0,postions);
		//glNormalPointer(GL_FLOAT,0,normals);
		//if( m_displayMode == GLSHADED )
		//{
		//	glColorPointer(3,GL_UNSIGNED_BYTE,0,colors);
		//}

		//glDrawArrays(GL_QUADS,0,24);
	}


	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	if( m_displayMode == GLSHADED )
	{
		glDisableClientState(GL_COLOR_ARRAY);
	}
	glDisable(GL_CULL_FACE);
}

void InterLayerGridObject::LoadLayer( const std::string& filename )
{
	CFile file(filename.c_str(), CFile::modeRead|CFile::typeBinary|CFile::modeRead);
	CArchive art(&file, CArchive::load);
	art >> I;
	art >> J;
	art >> K;
	int size;
	art >> size;
	for (int i = 0; i < size; i++)
	{
		tagGridModelCellNew cell;
		//I,J,K
		art >> cell._x;
		art >> cell._y;
		art >> cell._z;

		//八个点网格坐标
		for(int j=0;j<8;j++)
		{
			art >> cell.m_cornerPoint[j].x;
			art >> cell.m_cornerPoint[j].y;
			art >> cell.m_cornerPoint[j].z;
		}
		cell.CalcNormals();
		int size_t;
		art >> size_t;
		if(size_t>0)
			cell.m_bIsGridRefinement = true;
		for (int j = 0; j < size_t; j++)
		{
			int tmpSize;
			art >> tmpSize;

			tagGridModelCellNew subcell;
			
			for (int k = 0; k < 8; k++)
			{
				art >> subcell.m_cornerPoint[k].x;
				art >> subcell.m_cornerPoint[k].y;
				art >> subcell.m_cornerPoint[k].z;	
			}
			subcell.CalcNormals();
			cell.m_subCells.push_back(subcell);
		}
		m_gridCells.push_back(cell);
	}

	art.Close();
}

void InterLayerGridObject::Add( LPCTSTR lpszName, double dValue )
{
	int nSize = m_vecPhyPara.GetSize();

	CString str = lpszName;
	str.Trim();
	str.MakeUpper();

	for (int i=0; i<nSize; i++)
	{
		if( m_vecPhyPara[i].m_strName == str )
		{
			m_vecPhyPara[i].Add(dValue);
			return;
		}
	}

	CPhyPara param;
	param.m_strName = str;
	param.I = I;
	param.J = J;
	param.K = K;
	param.Add(dValue);
	m_vecPhyPara.Add(param);
}
