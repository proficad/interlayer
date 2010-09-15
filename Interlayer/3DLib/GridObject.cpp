#include "StdAfx.h"
#include "GridObject.h"
#include "GLDisplayContext.h"
#include "float.h"
#include "../MainFrm.h"

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

//------------------------------------------------------------------------

CGridObject::CGridObject(void)
{
	m_strObjName = _T("网格模型");
	m_ListDone = 0;
	m_Modified = 1;
	m_ColorBinding = COLOR_PER_VERTEX;

	m_glObjType = GLSURFACE;

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

CGridObject::~CGridObject(void)
{
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
		int size = m_pointList.size();
		ar << size;

		for (int i=0; i<size; i++)
		{
			ar << m_pointList[i];
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

		int size;
		ar >> size;
		//m_pointList.SetSize(size);
		
		if( m_pointList.capacity()<size )
			m_pointList.reserve(size);

		for (int i=0; i<size; i++)
		{
			CVertex3D point;

			ar >> point;

			//m_pointList[i] = point;
			m_pointList.push_back(point);
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
	O->SetContext(m_pDisplayContext);
	O->m_glObjID = m_glObjID;
	O->m_strObjName = m_strObjName;
	O->m_glObjType = m_glObjType;
	O->I = I;
	O->J = J;
	O->K = K;

	//if( m_pointList.GetSize()>0)
	if( m_pointList.size()>0)
	{
		//O->m_pointList.Copy(m_pointList);
		if( O->m_pointList.capacity()<m_pointList.size() )
			O->m_pointList.reserve(m_pointList.size());

		std::copy(m_pointList.begin(), m_pointList.end(), O->m_pointList.begin());
	}

	if( m_vecPhyPara.GetSize()>0)
	{
		O->m_vecPhyPara.Copy(m_vecPhyPara );
	}

	O->ComputeBoundLimits();

	return O;
}


void CGridObject::DefineDisplay()
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

	pMF->GetStatusBar().SetPaneText(0, _T("就绪"));	
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

bool CGridObject::BuildList()
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
					P = m_gridCells[i][j][k].m_cornerPoint[index];
					if( P.x < dMinX )
						dMinX = P.x;
					if( P.y < dMinY )
						dMinY = P.y;
					if( P.z < dMinZ )
						dMinZ = P.z;

					if( P.x > dMaxX )
						dMaxX = P.x;
					if( P.y > dMaxY )
						dMaxY = P.y;
					if( P.z > dMaxZ )
						dMaxZ = P.z;
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
		CVertex3D curP[8];

		int pCnt = 0;
		int nCell = 0;

		//int size = m_pointList.GetSize();
		int size = m_pointList.size();
		for (int i=0; i<size; i++)
		{	
			curP[pCnt % 8] = m_pointList[i];
			pCnt++;
			if( pCnt % 8 == 0 )
			{
				if( pParam && nCell < pParam->m_dValue.size() && m_displayMode != GLSHADED )
				{
					COLORREF clr = GetContext()->GetColorTable()->GetColor( (pParam->m_dValue[nCell]-pParam->m_dMinValue)/( pParam->m_dMaxValue- pParam->m_dMinValue));
					glColor4ub (GetRValue(clr), GetGValue(clr), GetBValue(clr), 255);
				}

				nCell++;

				COLORREF clr[8];
				if( !pParam && m_displayMode != GLSHADED )
				{
					clr[0] = GetContext()->GetColorTable()->GetColor( (curP[0].GetZ()-box.ZMin())/zRange);
					clr[1] = GetContext()->GetColorTable()->GetColor( (curP[1].GetZ()-box.ZMin())/zRange);
					clr[2] = GetContext()->GetColorTable()->GetColor( (curP[2].GetZ()-box.ZMin())/zRange);
					clr[3] = GetContext()->GetColorTable()->GetColor( (curP[3].GetZ()-box.ZMin())/zRange);
					clr[4] = GetContext()->GetColorTable()->GetColor( (curP[4].GetZ()-box.ZMin())/zRange);
					clr[5] = GetContext()->GetColorTable()->GetColor( (curP[5].GetZ()-box.ZMin())/zRange);
					clr[6] = GetContext()->GetColorTable()->GetColor( (curP[6].GetZ()-box.ZMin())/zRange);
					clr[7] = GetContext()->GetColorTable()->GetColor( (curP[7].GetZ()-box.ZMin())/zRange);
				}

				CVector3D nrml;

				glBegin(GL_QUADS);
				nrml = CVector3D(curP[0], curP[1]).CrossProduct(CVector3D(curP[1], curP[2])); 
				nrml.Normalize();
				glNormal3f(nrml.GetX(), nrml.GetY(), nrml.GetZ());				

				if(  !pParam && m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[0]), GetGValue(clr[0]), GetBValue(clr[0]));
				glVertex3d(
					(curP[0].GetX()-xMin)/range*2.0-1.0, 
					(curP[0].GetY()-yMin)/range*2.0-1.0,
					(curP[0].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[1]), GetGValue(clr[0]), GetBValue(clr[0]));
				glVertex3d(
					(curP[1].GetX()-xMin)/range*2.0-1.0, 
					(curP[1].GetY()-yMin)/range*2.0-1.0,
					(curP[1].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[2]), GetGValue(clr[2]), GetBValue(clr[2]));
				glVertex3d(
					(curP[2].GetX()-xMin)/range*2.0-1.0, 
					(curP[2].GetY()-yMin)/range*2.0-1.0,
					(curP[2].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[3]), GetGValue(clr[3]), GetBValue(clr[3]));
				glVertex3d(
					(curP[3].GetX()-xMin)/range*2.0-1.0, 
					(curP[3].GetY()-yMin)/range*2.0-1.0,
					(curP[3].GetZ()-box.ZMin())/zRange*2.0-1.0);
				glEnd();

				glBegin(GL_QUADS);
				nrml = CVector3D(curP[4], curP[7]).CrossProduct(CVector3D(curP[7], curP[6])); 
				nrml.Normalize();
				glNormal3f(nrml.GetX(), nrml.GetY(), nrml.GetZ());

				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[4]), GetGValue(clr[4]), GetBValue(clr[4]));
				glVertex3d(
					(curP[4].GetX()-xMin)/range*2.0-1.0, 
					(curP[4].GetY()-yMin)/range*2.0-1.0,
					(curP[4].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[7]), GetGValue(clr[7]), GetBValue(clr[7]));
				glVertex3d(
					(curP[7].GetX()-xMin)/range*2.0-1.0, 
					(curP[7].GetY()-yMin)/range*2.0-1.0,
					(curP[7].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[6]), GetGValue(clr[6]), GetBValue(clr[6]));
				glVertex3d(
					(curP[6].GetX()-xMin)/range*2.0-1.0, 
					(curP[6].GetY()-yMin)/range*2.0-1.0,
					(curP[6].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[5]), GetGValue(clr[5]), GetBValue(clr[5]));
				glVertex3d(
					(curP[5].GetX()-xMin)/range*2.0-1.0, 
					(curP[5].GetY()-yMin)/range*2.0-1.0,
					(curP[5].GetZ()-box.ZMin())/zRange*2.0-1.0);
				glEnd();


				glBegin(GL_QUADS);
				nrml = CVector3D(curP[0], curP[4]).CrossProduct(CVector3D(curP[4], curP[5])); 
				nrml.Normalize();
				glNormal3f(nrml.GetX(), nrml.GetY(), nrml.GetZ());

				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[0]), GetGValue(clr[0]), GetBValue(clr[0]));
				glVertex3d(
					(curP[0].GetX()-xMin)/range*2.0-1.0, 
					(curP[0].GetY()-yMin)/range*2.0-1.0,
					(curP[0].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[4]), GetGValue(clr[4]), GetBValue(clr[4]));
				glVertex3d(
					(curP[4].GetX()-xMin)/range*2.0-1.0, 
					(curP[4].GetY()-yMin)/range*2.0-1.0,
					(curP[4].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[5]), GetGValue(clr[5]), GetBValue(clr[5]));
				glVertex3d(
					(curP[5].GetX()-xMin)/range*2.0-1.0, 
					(curP[5].GetY()-yMin)/range*2.0-1.0,
					(curP[5].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[1]), GetGValue(clr[1]), GetBValue(clr[1]));
				glVertex3d(
					(curP[1].GetX()-xMin)/range*2.0-1.0, 
					(curP[1].GetY()-yMin)/range*2.0-1.0,
					(curP[1].GetZ()-box.ZMin())/zRange*2.0-1.0);
				glEnd();


				glBegin(GL_QUADS);
				nrml = CVector3D(curP[1], curP[5]).CrossProduct(CVector3D(curP[5], curP[6])); 
				nrml.Normalize();
				glNormal3f(nrml.GetX(), nrml.GetY(), nrml.GetZ());

				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[1]), GetGValue(clr[1]), GetBValue(clr[1]));
				glVertex3d(
					(curP[1].GetX()-xMin)/range*2.0-1.0, 
					(curP[1].GetY()-yMin)/range*2.0-1.0,
					(curP[1].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[5]), GetGValue(clr[5]), GetBValue(clr[5]));
				glVertex3d(
					(curP[5].GetX()-xMin)/range*2.0-1.0, 
					(curP[5].GetY()-yMin)/range*2.0-1.0,
					(curP[5].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[6]), GetGValue(clr[6]), GetBValue(clr[6]));
				glVertex3d(
					(curP[6].GetX()-xMin)/range*2.0-1.0, 
					(curP[6].GetY()-yMin)/range*2.0-1.0,
					(curP[6].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[2]), GetGValue(clr[2]), GetBValue(clr[2]));
				glVertex3d(
					(curP[2].GetX()-xMin)/range*2.0-1.0, 
					(curP[2].GetY()-yMin)/range*2.0-1.0,
					(curP[2].GetZ()-box.ZMin())/zRange*2.0-1.0);
				glEnd();

				glBegin(GL_QUADS);
				nrml = CVector3D(curP[2], curP[6]).CrossProduct(CVector3D(curP[6], curP[7])); 
				nrml.Normalize();
				glNormal3f(nrml.GetX(), nrml.GetY(), nrml.GetZ());

				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[2]), GetGValue(clr[2]), GetBValue(clr[2]));
				glVertex3d(
					(curP[2].GetX()-xMin)/range*2.0-1.0, 
					(curP[2].GetY()-yMin)/range*2.0-1.0,
					(curP[2].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[6]), GetGValue(clr[6]), GetBValue(clr[6]));
				glVertex3d(
					(curP[6].GetX()-xMin)/range*2.0-1.0, 
					(curP[6].GetY()-yMin)/range*2.0-1.0,
					(curP[6].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[7]), GetGValue(clr[7]), GetBValue(clr[7]));
				glVertex3d(
					(curP[7].GetX()-xMin)/range*2.0-1.0, 
					(curP[7].GetY()-yMin)/range*2.0-1.0,
					(curP[7].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[3]), GetGValue(clr[3]), GetBValue(clr[3]));
				glVertex3d(
					(curP[3].GetX()-xMin)/range*2.0-1.0, 
					(curP[3].GetY()-yMin)/range*2.0-1.0,
					(curP[3].GetZ()-box.ZMin())/zRange*2.0-1.0);
				glEnd();

				glBegin(GL_QUADS);
				nrml = CVector3D(curP[0], curP[3]).CrossProduct(CVector3D(curP[3], curP[7])); 
				nrml.Normalize();
				glNormal3f(nrml.GetX(), nrml.GetY(), nrml.GetZ());

				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[0]), GetGValue(clr[0]), GetBValue(clr[0]));
				glVertex3d(
					(curP[0].GetX()-xMin)/range*2.0-1.0, 
					(curP[0].GetY()-yMin)/range*2.0-1.0,
					(curP[0].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[3]), GetGValue(clr[3]), GetBValue(clr[3]));
				glVertex3d(
					(curP[3].GetX()-xMin)/range*2.0-1.0, 
					(curP[3].GetY()-yMin)/range*2.0-1.0,
					(curP[3].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[7]), GetGValue(clr[7]), GetBValue(clr[7]));
				glVertex3d(
					(curP[7].GetX()-xMin)/range*2.0-1.0, 
					(curP[7].GetY()-yMin)/range*2.0-1.0,
					(curP[7].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[4]), GetGValue(clr[4]), GetBValue(clr[4]));
				glVertex3d(
					(curP[4].GetX()-xMin)/range*2.0-1.0, 
					(curP[4].GetY()-yMin)/range*2.0-1.0,
					(curP[4].GetZ()-box.ZMin())/zRange*2.0-1.0);
				glEnd();
			}
		}
	}
	else
	{
		CVertex3D curP[8];

		int pCnt = 0;
		int nCell = 0;

		int i=0;
		int j=0;
		int k=0;

		//int size = m_pointList.GetSize();
		int size = m_pointList.size();

		for (int m=0; m<size; m++)
		{	
			curP[pCnt % 8] = m_pointList[m];
			pCnt++;
			if( pCnt % 8 == 0 )
			{
				nCell++;
				if( !m_bShowI[i] 
				&& !m_bShowJ[j] 
				&& !m_bShowK[k] )
				{
					i++;
					if(i==I)
					{
						i = 0;
						j++;
						if( j==J)
						{
							j = 0;
							k++;
						}
					}
					continue;
				}

				i++;
				if(i==I)
				{
					i = 0;
					j++;
					if( j==J)
					{
						j = 0;
						k++;
					}
				}

				if( pParam && nCell < pParam->m_dValue.size() && m_displayMode != GLSHADED )
				{
					COLORREF clr = GetContext()->GetColorTable()->GetColor( (pParam->m_dValue[nCell]-pParam->m_dMinValue)/( pParam->m_dMaxValue- pParam->m_dMinValue));
					glColor4ub (GetRValue(clr), GetGValue(clr), GetBValue(clr), 255);
				}

				COLORREF clr[8];
				if(!pParam &&  m_displayMode != GLSHADED )
				{
					clr[0] = GetContext()->GetColorTable()->GetColor( (curP[0].GetZ()-box.ZMin())/zRange);
					clr[1] = GetContext()->GetColorTable()->GetColor( (curP[1].GetZ()-box.ZMin())/zRange);
					clr[2] = GetContext()->GetColorTable()->GetColor( (curP[2].GetZ()-box.ZMin())/zRange);
					clr[3] = GetContext()->GetColorTable()->GetColor( (curP[3].GetZ()-box.ZMin())/zRange);
					clr[4] = GetContext()->GetColorTable()->GetColor( (curP[4].GetZ()-box.ZMin())/zRange);
					clr[5] = GetContext()->GetColorTable()->GetColor( (curP[5].GetZ()-box.ZMin())/zRange);
					clr[6] = GetContext()->GetColorTable()->GetColor( (curP[6].GetZ()-box.ZMin())/zRange);
					clr[7] = GetContext()->GetColorTable()->GetColor( (curP[7].GetZ()-box.ZMin())/zRange);
				}

				CVector3D nrml;

				glBegin(GL_QUADS);
				nrml = CVector3D(curP[0], curP[1]).CrossProduct(CVector3D(curP[1], curP[2])); 
				nrml.Normalize();
				glNormal3f(nrml.GetX(), nrml.GetY(), nrml.GetZ());				

				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[0]), GetGValue(clr[0]), GetBValue(clr[0]));
				glVertex3d(
					(curP[0].GetX()-xMin)/range*2.0-1.0, 
					(curP[0].GetY()-yMin)/range*2.0-1.0,
					(curP[0].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[1]), GetGValue(clr[0]), GetBValue(clr[0]));
				glVertex3d(
					(curP[1].GetX()-xMin)/range*2.0-1.0, 
					(curP[1].GetY()-yMin)/range*2.0-1.0,
					(curP[1].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[2]), GetGValue(clr[2]), GetBValue(clr[2]));
				glVertex3d(
					(curP[2].GetX()-xMin)/range*2.0-1.0, 
					(curP[2].GetY()-yMin)/range*2.0-1.0,
					(curP[2].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[3]), GetGValue(clr[3]), GetBValue(clr[3]));
				glVertex3d(
					(curP[3].GetX()-xMin)/range*2.0-1.0, 
					(curP[3].GetY()-yMin)/range*2.0-1.0,
					(curP[3].GetZ()-box.ZMin())/zRange*2.0-1.0);
				glEnd();

				glBegin(GL_QUADS);
				nrml = CVector3D(curP[4], curP[7]).CrossProduct(CVector3D(curP[7], curP[6])); 
				nrml.Normalize();
				glNormal3f(nrml.GetX(), nrml.GetY(), nrml.GetZ());

				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[4]), GetGValue(clr[4]), GetBValue(clr[4]));
				glVertex3d(
					(curP[4].GetX()-xMin)/range*2.0-1.0, 
					(curP[4].GetY()-yMin)/range*2.0-1.0,
					(curP[4].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[7]), GetGValue(clr[7]), GetBValue(clr[7]));
				glVertex3d(
					(curP[7].GetX()-xMin)/range*2.0-1.0, 
					(curP[7].GetY()-yMin)/range*2.0-1.0,
					(curP[7].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[6]), GetGValue(clr[6]), GetBValue(clr[6]));
				glVertex3d(
					(curP[6].GetX()-xMin)/range*2.0-1.0, 
					(curP[6].GetY()-yMin)/range*2.0-1.0,
					(curP[6].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[5]), GetGValue(clr[5]), GetBValue(clr[5]));
				glVertex3d(
					(curP[5].GetX()-xMin)/range*2.0-1.0, 
					(curP[5].GetY()-yMin)/range*2.0-1.0,
					(curP[5].GetZ()-box.ZMin())/zRange*2.0-1.0);
				glEnd();

				glBegin(GL_QUADS);
				nrml = CVector3D(curP[0], curP[4]).CrossProduct(CVector3D(curP[4], curP[5])); 
				nrml.Normalize();
				glNormal3f(nrml.GetX(), nrml.GetY(), nrml.GetZ());

				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[0]), GetGValue(clr[0]), GetBValue(clr[0]));
				glVertex3d(
					(curP[0].GetX()-xMin)/range*2.0-1.0, 
					(curP[0].GetY()-yMin)/range*2.0-1.0,
					(curP[0].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[4]), GetGValue(clr[4]), GetBValue(clr[4]));
				glVertex3d(
					(curP[4].GetX()-xMin)/range*2.0-1.0, 
					(curP[4].GetY()-yMin)/range*2.0-1.0,
					(curP[4].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[5]), GetGValue(clr[5]), GetBValue(clr[5]));
				glVertex3d(
					(curP[5].GetX()-xMin)/range*2.0-1.0, 
					(curP[5].GetY()-yMin)/range*2.0-1.0,
					(curP[5].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[1]), GetGValue(clr[1]), GetBValue(clr[1]));
				glVertex3d(
					(curP[1].GetX()-xMin)/range*2.0-1.0, 
					(curP[1].GetY()-yMin)/range*2.0-1.0,
					(curP[1].GetZ()-box.ZMin())/zRange*2.0-1.0);
				glEnd();


				glBegin(GL_QUADS);
				nrml = CVector3D(curP[1], curP[5]).CrossProduct(CVector3D(curP[5], curP[6])); 
				nrml.Normalize();
				glNormal3f(nrml.GetX(), nrml.GetY(), nrml.GetZ());

				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[1]), GetGValue(clr[1]), GetBValue(clr[1]));
				glVertex3d(
					(curP[1].GetX()-xMin)/range*2.0-1.0, 
					(curP[1].GetY()-yMin)/range*2.0-1.0,
					(curP[1].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[5]), GetGValue(clr[5]), GetBValue(clr[5]));
				glVertex3d(
					(curP[5].GetX()-xMin)/range*2.0-1.0, 
					(curP[5].GetY()-yMin)/range*2.0-1.0,
					(curP[5].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[6]), GetGValue(clr[6]), GetBValue(clr[6]));
				glVertex3d(
					(curP[6].GetX()-xMin)/range*2.0-1.0, 
					(curP[6].GetY()-yMin)/range*2.0-1.0,
					(curP[6].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[2]), GetGValue(clr[2]), GetBValue(clr[2]));
				glVertex3d(
					(curP[2].GetX()-xMin)/range*2.0-1.0, 
					(curP[2].GetY()-yMin)/range*2.0-1.0,
					(curP[2].GetZ()-box.ZMin())/zRange*2.0-1.0);
				glEnd();

				glBegin(GL_QUADS);
				nrml = CVector3D(curP[2], curP[6]).CrossProduct(CVector3D(curP[6], curP[7])); 
				nrml.Normalize();
				glNormal3f(nrml.GetX(), nrml.GetY(), nrml.GetZ());

				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[2]), GetGValue(clr[2]), GetBValue(clr[2]));
				glVertex3d(
					(curP[2].GetX()-xMin)/range*2.0-1.0, 
					(curP[2].GetY()-yMin)/range*2.0-1.0,
					(curP[2].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[6]), GetGValue(clr[6]), GetBValue(clr[6]));
				glVertex3d(
					(curP[6].GetX()-xMin)/range*2.0-1.0, 
					(curP[6].GetY()-yMin)/range*2.0-1.0,
					(curP[6].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[7]), GetGValue(clr[7]), GetBValue(clr[7]));
				glVertex3d(
					(curP[7].GetX()-xMin)/range*2.0-1.0, 
					(curP[7].GetY()-yMin)/range*2.0-1.0,
					(curP[7].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[3]), GetGValue(clr[3]), GetBValue(clr[3]));
				glVertex3d(
					(curP[3].GetX()-xMin)/range*2.0-1.0, 
					(curP[3].GetY()-yMin)/range*2.0-1.0,
					(curP[3].GetZ()-box.ZMin())/zRange*2.0-1.0);
				glEnd();

				glBegin(GL_QUADS);
				nrml = CVector3D(curP[0], curP[3]).CrossProduct(CVector3D(curP[3], curP[7])); 
				nrml.Normalize();
				glNormal3f(nrml.GetX(), nrml.GetY(), nrml.GetZ());

				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[0]), GetGValue(clr[0]), GetBValue(clr[0]));
				glVertex3d(
					(curP[0].GetX()-xMin)/range*2.0-1.0, 
					(curP[0].GetY()-yMin)/range*2.0-1.0,
					(curP[0].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[3]), GetGValue(clr[3]), GetBValue(clr[3]));
				glVertex3d(
					(curP[3].GetX()-xMin)/range*2.0-1.0, 
					(curP[3].GetY()-yMin)/range*2.0-1.0,
					(curP[3].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[7]), GetGValue(clr[7]), GetBValue(clr[7]));
				glVertex3d(
					(curP[7].GetX()-xMin)/range*2.0-1.0, 
					(curP[7].GetY()-yMin)/range*2.0-1.0,
					(curP[7].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam &&  m_displayMode != GLSHADED )
					glColor3ub(GetRValue(clr[4]), GetGValue(clr[4]), GetBValue(clr[4]));
				glVertex3d(
					(curP[4].GetX()-xMin)/range*2.0-1.0, 
					(curP[4].GetY()-yMin)/range*2.0-1.0,
					(curP[4].GetZ()-box.ZMin())/zRange*2.0-1.0);
				glEnd();
			}
		}
	}
}

void CGridObject::DrawShaded2()
{
	if (m_displayMode == GLWIREFRAME)
		glColor3ub(155, 155, 155);
		//glColor4d(0.135000, 0.222500, 0.157500, 0.950000);

	glEnable(GL_CULL_FACE);
	glCullFace( GL_BACK ); 
	
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
		CVertex3D curP[8];

		int pCnt = 0;
		int nCell = 0;

		//int size = m_pointList.GetSize();
		int size = m_pointList.size();
		for (int i=0; i<size; i++)
		{	
			curP[pCnt % 8] = m_pointList[i];
			pCnt++;
			if( pCnt % 8 == 0 )
			{
				if( pParam && nCell < pParam->m_dValue.size() && m_displayMode == GLSHADED )
				{
					COLORREF clr = GetContext()->GetColorTable()->GetColor( (pParam->m_dValue[nCell]-pParam->m_dMinValue)/( pParam->m_dMaxValue- pParam->m_dMinValue));
					glColor4ub (GetRValue(clr), GetGValue(clr), GetBValue(clr), 255);
				}

				nCell++;

				COLORREF clr[8];
				if( !pParam  && m_displayMode == GLSHADED)
				{
					clr[0] = GetContext()->GetColorTable()->GetColor( (curP[0].GetZ()-box.ZMin())/zRange);
					clr[1] = GetContext()->GetColorTable()->GetColor( (curP[1].GetZ()-box.ZMin())/zRange);
					clr[2] = GetContext()->GetColorTable()->GetColor( (curP[2].GetZ()-box.ZMin())/zRange);
					clr[3] = GetContext()->GetColorTable()->GetColor( (curP[3].GetZ()-box.ZMin())/zRange);
					clr[4] = GetContext()->GetColorTable()->GetColor( (curP[4].GetZ()-box.ZMin())/zRange);
					clr[5] = GetContext()->GetColorTable()->GetColor( (curP[5].GetZ()-box.ZMin())/zRange);
					clr[6] = GetContext()->GetColorTable()->GetColor( (curP[6].GetZ()-box.ZMin())/zRange);
					clr[7] = GetContext()->GetColorTable()->GetColor( (curP[7].GetZ()-box.ZMin())/zRange);
				}

				CVector3D nrml;

				glBegin(GL_QUADS);
				nrml = CVector3D(curP[0], curP[1]).CrossProduct(CVector3D(curP[1], curP[2])); 
				nrml.Normalize();
				glNormal3f(nrml.GetX(), nrml.GetY(), nrml.GetZ());				

				if( !pParam  && m_displayMode == GLSHADED)
					glColor3ub(GetRValue(clr[0]), GetGValue(clr[0]), GetBValue(clr[0]));
				glVertex3d(
					(curP[0].GetX()-xMin)/range*2.0-1.0, 
					(curP[0].GetY()-yMin)/range*2.0-1.0,
					(curP[0].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam  && m_displayMode == GLSHADED)
					glColor3ub(GetRValue(clr[1]), GetGValue(clr[0]), GetBValue(clr[0]));
				glVertex3d(
					(curP[1].GetX()-xMin)/range*2.0-1.0, 
					(curP[1].GetY()-yMin)/range*2.0-1.0,
					(curP[1].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam  && m_displayMode == GLSHADED)
					glColor3ub(GetRValue(clr[2]), GetGValue(clr[2]), GetBValue(clr[2]));
				glVertex3d(
					(curP[2].GetX()-xMin)/range*2.0-1.0, 
					(curP[2].GetY()-yMin)/range*2.0-1.0,
					(curP[2].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam  && m_displayMode == GLSHADED)
					glColor3ub(GetRValue(clr[3]), GetGValue(clr[3]), GetBValue(clr[3]));
				glVertex3d(
					(curP[3].GetX()-xMin)/range*2.0-1.0, 
					(curP[3].GetY()-yMin)/range*2.0-1.0,
					(curP[3].GetZ()-box.ZMin())/zRange*2.0-1.0);
				glEnd();

				glBegin(GL_QUADS);
				nrml = CVector3D(curP[4], curP[7]).CrossProduct(CVector3D(curP[7], curP[6])); 
				nrml.Normalize();
				glNormal3f(nrml.GetX(), nrml.GetY(), nrml.GetZ());

				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[4]), GetGValue(clr[4]), GetBValue(clr[4]));
				glVertex3d(
					(curP[4].GetX()-xMin)/range*2.0-1.0, 
					(curP[4].GetY()-yMin)/range*2.0-1.0,
					(curP[4].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[7]), GetGValue(clr[7]), GetBValue(clr[7]));
				glVertex3d(
					(curP[7].GetX()-xMin)/range*2.0-1.0, 
					(curP[7].GetY()-yMin)/range*2.0-1.0,
					(curP[7].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[6]), GetGValue(clr[6]), GetBValue(clr[6]));
				glVertex3d(
					(curP[6].GetX()-xMin)/range*2.0-1.0, 
					(curP[6].GetY()-yMin)/range*2.0-1.0,
					(curP[6].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[5]), GetGValue(clr[5]), GetBValue(clr[5]));
				glVertex3d(
					(curP[5].GetX()-xMin)/range*2.0-1.0, 
					(curP[5].GetY()-yMin)/range*2.0-1.0,
					(curP[5].GetZ()-box.ZMin())/zRange*2.0-1.0);
				glEnd();


				glBegin(GL_QUADS);
				nrml = CVector3D(curP[0], curP[4]).CrossProduct(CVector3D(curP[4], curP[5])); 
				nrml.Normalize();
				glNormal3f(nrml.GetX(), nrml.GetY(), nrml.GetZ());

				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[0]), GetGValue(clr[0]), GetBValue(clr[0]));
				glVertex3d(
					(curP[0].GetX()-xMin)/range*2.0-1.0, 
					(curP[0].GetY()-yMin)/range*2.0-1.0,
					(curP[0].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[4]), GetGValue(clr[4]), GetBValue(clr[4]));
				glVertex3d(
					(curP[4].GetX()-xMin)/range*2.0-1.0, 
					(curP[4].GetY()-yMin)/range*2.0-1.0,
					(curP[4].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[5]), GetGValue(clr[5]), GetBValue(clr[5]));
				glVertex3d(
					(curP[5].GetX()-xMin)/range*2.0-1.0, 
					(curP[5].GetY()-yMin)/range*2.0-1.0,
					(curP[5].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[1]), GetGValue(clr[1]), GetBValue(clr[1]));
				glVertex3d(
					(curP[1].GetX()-xMin)/range*2.0-1.0, 
					(curP[1].GetY()-yMin)/range*2.0-1.0,
					(curP[1].GetZ()-box.ZMin())/zRange*2.0-1.0);
				glEnd();


				glBegin(GL_QUADS);
				nrml = CVector3D(curP[1], curP[5]).CrossProduct(CVector3D(curP[5], curP[6])); 
				nrml.Normalize();
				glNormal3f(nrml.GetX(), nrml.GetY(), nrml.GetZ());

				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[1]), GetGValue(clr[1]), GetBValue(clr[1]));
				glVertex3d(
					(curP[1].GetX()-xMin)/range*2.0-1.0, 
					(curP[1].GetY()-yMin)/range*2.0-1.0,
					(curP[1].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[5]), GetGValue(clr[5]), GetBValue(clr[5]));
				glVertex3d(
					(curP[5].GetX()-xMin)/range*2.0-1.0, 
					(curP[5].GetY()-yMin)/range*2.0-1.0,
					(curP[5].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[6]), GetGValue(clr[6]), GetBValue(clr[6]));
				glVertex3d(
					(curP[6].GetX()-xMin)/range*2.0-1.0, 
					(curP[6].GetY()-yMin)/range*2.0-1.0,
					(curP[6].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[2]), GetGValue(clr[2]), GetBValue(clr[2]));
				glVertex3d(
					(curP[2].GetX()-xMin)/range*2.0-1.0, 
					(curP[2].GetY()-yMin)/range*2.0-1.0,
					(curP[2].GetZ()-box.ZMin())/zRange*2.0-1.0);
				glEnd();

				glBegin(GL_QUADS);
				nrml = CVector3D(curP[2], curP[6]).CrossProduct(CVector3D(curP[6], curP[7])); 
				nrml.Normalize();
				glNormal3f(nrml.GetX(), nrml.GetY(), nrml.GetZ());

				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[2]), GetGValue(clr[2]), GetBValue(clr[2]));
				glVertex3d(
					(curP[2].GetX()-xMin)/range*2.0-1.0, 
					(curP[2].GetY()-yMin)/range*2.0-1.0,
					(curP[2].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[6]), GetGValue(clr[6]), GetBValue(clr[6]));
				glVertex3d(
					(curP[6].GetX()-xMin)/range*2.0-1.0, 
					(curP[6].GetY()-yMin)/range*2.0-1.0,
					(curP[6].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[7]), GetGValue(clr[7]), GetBValue(clr[7]));
				glVertex3d(
					(curP[7].GetX()-xMin)/range*2.0-1.0, 
					(curP[7].GetY()-yMin)/range*2.0-1.0,
					(curP[7].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[3]), GetGValue(clr[3]), GetBValue(clr[3]));
				glVertex3d(
					(curP[3].GetX()-xMin)/range*2.0-1.0, 
					(curP[3].GetY()-yMin)/range*2.0-1.0,
					(curP[3].GetZ()-box.ZMin())/zRange*2.0-1.0);
				glEnd();

				glBegin(GL_QUADS);
				nrml = CVector3D(curP[0], curP[3]).CrossProduct(CVector3D(curP[3], curP[7])); 
				nrml.Normalize();
				glNormal3f(nrml.GetX(), nrml.GetY(), nrml.GetZ());

				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[0]), GetGValue(clr[0]), GetBValue(clr[0]));
				glVertex3d(
					(curP[0].GetX()-xMin)/range*2.0-1.0, 
					(curP[0].GetY()-yMin)/range*2.0-1.0,
					(curP[0].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[3]), GetGValue(clr[3]), GetBValue(clr[3]));
				glVertex3d(
					(curP[3].GetX()-xMin)/range*2.0-1.0, 
					(curP[3].GetY()-yMin)/range*2.0-1.0,
					(curP[3].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[7]), GetGValue(clr[7]), GetBValue(clr[7]));
				glVertex3d(
					(curP[7].GetX()-xMin)/range*2.0-1.0, 
					(curP[7].GetY()-yMin)/range*2.0-1.0,
					(curP[7].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[4]), GetGValue(clr[4]), GetBValue(clr[4]));
				glVertex3d(
					(curP[4].GetX()-xMin)/range*2.0-1.0, 
					(curP[4].GetY()-yMin)/range*2.0-1.0,
					(curP[4].GetZ()-box.ZMin())/zRange*2.0-1.0);
				glEnd();
			}
		}
	}
	else
	{	
		CVertex3D curP[8];
		
		int pCnt = 0;
		int nCell = 0;

		int i=0;
		int j=0;
		int k=0;

		//int size = m_pointList.GetSize();
		int size = m_pointList.size();
		for (int m=0; m<size; m++)
		{	
			curP[pCnt % 8] = m_pointList[m];
			pCnt++;
			if( pCnt % 8 == 0 )
			{
				nCell++;
				if( !m_bShowI[i] 
				&& !m_bShowJ[j] 
				&& !m_bShowK[k] )
				{
					i++;
					if(i==I)
					{
						i = 0;
						j++;
						if( j==J)
						{
							j = 0;
							k++;
						}
					}
					continue;
				}

				i++;
				if(i==I)
				{
					i = 0;
					j++;
					if( j==J)
					{
						j = 0;
						k++;
					}
				}

				if( pParam && nCell <= pParam->m_dValue.size() && m_displayMode == GLSHADED )
				{
					COLORREF clr = GetContext()->GetColorTable()->GetColor( (pParam->m_dValue[nCell]-pParam->m_dMinValue)/( pParam->m_dMaxValue- pParam->m_dMinValue));
					glColor4ub (GetRValue(clr), GetGValue(clr), GetBValue(clr), 255);
				}

				COLORREF clr[8];
				if( !pParam && m_displayMode == GLSHADED )
				{
					clr[0] = GetContext()->GetColorTable()->GetColor( (curP[0].GetZ()-box.ZMin())/zRange);
					clr[1] = GetContext()->GetColorTable()->GetColor( (curP[1].GetZ()-box.ZMin())/zRange);
					clr[2] = GetContext()->GetColorTable()->GetColor( (curP[2].GetZ()-box.ZMin())/zRange);
					clr[3] = GetContext()->GetColorTable()->GetColor( (curP[3].GetZ()-box.ZMin())/zRange);
					clr[4] = GetContext()->GetColorTable()->GetColor( (curP[4].GetZ()-box.ZMin())/zRange);
					clr[5] = GetContext()->GetColorTable()->GetColor( (curP[5].GetZ()-box.ZMin())/zRange);
					clr[6] = GetContext()->GetColorTable()->GetColor( (curP[6].GetZ()-box.ZMin())/zRange);
					clr[7] = GetContext()->GetColorTable()->GetColor( (curP[7].GetZ()-box.ZMin())/zRange);
				}

				CVector3D nrml;

				glBegin(GL_QUADS);
				nrml = CVector3D(curP[0], curP[1]).CrossProduct(CVector3D(curP[1], curP[2])); 
				nrml.Normalize();
				glNormal3f(nrml.GetX(), nrml.GetY(), nrml.GetZ());				

				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[0]), GetGValue(clr[0]), GetBValue(clr[0]));
				glVertex3d(
					(curP[0].GetX()-xMin)/range*2.0-1.0, 
					(curP[0].GetY()-yMin)/range*2.0-1.0,
					(curP[0].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[1]), GetGValue(clr[0]), GetBValue(clr[0]));
				glVertex3d(
					(curP[1].GetX()-xMin)/range*2.0-1.0, 
					(curP[1].GetY()-yMin)/range*2.0-1.0,
					(curP[1].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[2]), GetGValue(clr[2]), GetBValue(clr[2]));
				glVertex3d(
					(curP[2].GetX()-xMin)/range*2.0-1.0, 
					(curP[2].GetY()-yMin)/range*2.0-1.0,
					(curP[2].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[3]), GetGValue(clr[3]), GetBValue(clr[3]));
				glVertex3d(
					(curP[3].GetX()-xMin)/range*2.0-1.0, 
					(curP[3].GetY()-yMin)/range*2.0-1.0,
					(curP[3].GetZ()-box.ZMin())/zRange*2.0-1.0);
				glEnd();

				glBegin(GL_QUADS);
				nrml = CVector3D(curP[4], curP[7]).CrossProduct(CVector3D(curP[7], curP[6])); 
				nrml.Normalize();
				glNormal3f(nrml.GetX(), nrml.GetY(), nrml.GetZ());

				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[4]), GetGValue(clr[4]), GetBValue(clr[4]));
				glVertex3d(
					(curP[4].GetX()-xMin)/range*2.0-1.0, 
					(curP[4].GetY()-yMin)/range*2.0-1.0,
					(curP[4].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[7]), GetGValue(clr[7]), GetBValue(clr[7]));
				glVertex3d(
					(curP[7].GetX()-xMin)/range*2.0-1.0, 
					(curP[7].GetY()-yMin)/range*2.0-1.0,
					(curP[7].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[6]), GetGValue(clr[6]), GetBValue(clr[6]));
				glVertex3d(
					(curP[6].GetX()-xMin)/range*2.0-1.0, 
					(curP[6].GetY()-yMin)/range*2.0-1.0,
					(curP[6].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[5]), GetGValue(clr[5]), GetBValue(clr[5]));
				glVertex3d(
					(curP[5].GetX()-xMin)/range*2.0-1.0, 
					(curP[5].GetY()-yMin)/range*2.0-1.0,
					(curP[5].GetZ()-box.ZMin())/zRange*2.0-1.0);
				glEnd();


				glBegin(GL_QUADS);
				nrml = CVector3D(curP[0], curP[4]).CrossProduct(CVector3D(curP[4], curP[5])); 
				nrml.Normalize();
				glNormal3f(nrml.GetX(), nrml.GetY(), nrml.GetZ());

				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[0]), GetGValue(clr[0]), GetBValue(clr[0]));
				glVertex3d(
					(curP[0].GetX()-xMin)/range*2.0-1.0, 
					(curP[0].GetY()-yMin)/range*2.0-1.0,
					(curP[0].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[4]), GetGValue(clr[4]), GetBValue(clr[4]));
				glVertex3d(
					(curP[4].GetX()-xMin)/range*2.0-1.0, 
					(curP[4].GetY()-yMin)/range*2.0-1.0,
					(curP[4].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[5]), GetGValue(clr[5]), GetBValue(clr[5]));
				glVertex3d(
					(curP[5].GetX()-xMin)/range*2.0-1.0, 
					(curP[5].GetY()-yMin)/range*2.0-1.0,
					(curP[5].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[1]), GetGValue(clr[1]), GetBValue(clr[1]));
				glVertex3d(
					(curP[1].GetX()-xMin)/range*2.0-1.0, 
					(curP[1].GetY()-yMin)/range*2.0-1.0,
					(curP[1].GetZ()-box.ZMin())/zRange*2.0-1.0);
				glEnd();


				glBegin(GL_QUADS);
				nrml = CVector3D(curP[1], curP[5]).CrossProduct(CVector3D(curP[5], curP[6])); 
				nrml.Normalize();
				glNormal3f(nrml.GetX(), nrml.GetY(), nrml.GetZ());

				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[1]), GetGValue(clr[1]), GetBValue(clr[1]));
				glVertex3d(
					(curP[1].GetX()-xMin)/range*2.0-1.0, 
					(curP[1].GetY()-yMin)/range*2.0-1.0,
					(curP[1].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[5]), GetGValue(clr[5]), GetBValue(clr[5]));
				glVertex3d(
					(curP[5].GetX()-xMin)/range*2.0-1.0, 
					(curP[5].GetY()-yMin)/range*2.0-1.0,
					(curP[5].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[6]), GetGValue(clr[6]), GetBValue(clr[6]));
				glVertex3d(
					(curP[6].GetX()-xMin)/range*2.0-1.0, 
					(curP[6].GetY()-yMin)/range*2.0-1.0,
					(curP[6].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[2]), GetGValue(clr[2]), GetBValue(clr[2]));
				glVertex3d(
					(curP[2].GetX()-xMin)/range*2.0-1.0, 
					(curP[2].GetY()-yMin)/range*2.0-1.0,
					(curP[2].GetZ()-box.ZMin())/zRange*2.0-1.0);
				glEnd();

				glBegin(GL_QUADS);
				nrml = CVector3D(curP[2], curP[6]).CrossProduct(CVector3D(curP[6], curP[7])); 
				nrml.Normalize();
				glNormal3f(nrml.GetX(), nrml.GetY(), nrml.GetZ());

				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[2]), GetGValue(clr[2]), GetBValue(clr[2]));
				glVertex3d(
					(curP[2].GetX()-xMin)/range*2.0-1.0, 
					(curP[2].GetY()-yMin)/range*2.0-1.0,
					(curP[2].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[6]), GetGValue(clr[6]), GetBValue(clr[6]));
				glVertex3d(
					(curP[6].GetX()-xMin)/range*2.0-1.0, 
					(curP[6].GetY()-yMin)/range*2.0-1.0,
					(curP[6].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[7]), GetGValue(clr[7]), GetBValue(clr[7]));
				glVertex3d(
					(curP[7].GetX()-xMin)/range*2.0-1.0, 
					(curP[7].GetY()-yMin)/range*2.0-1.0,
					(curP[7].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[3]), GetGValue(clr[3]), GetBValue(clr[3]));
				glVertex3d(
					(curP[3].GetX()-xMin)/range*2.0-1.0, 
					(curP[3].GetY()-yMin)/range*2.0-1.0,
					(curP[3].GetZ()-box.ZMin())/zRange*2.0-1.0);
				glEnd();

				glBegin(GL_QUADS);
				nrml = CVector3D(curP[0], curP[3]).CrossProduct(CVector3D(curP[3], curP[7])); 
				nrml.Normalize();
				glNormal3f(nrml.GetX(), nrml.GetY(), nrml.GetZ());

				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[0]), GetGValue(clr[0]), GetBValue(clr[0]));
				glVertex3d(
					(curP[0].GetX()-xMin)/range*2.0-1.0, 
					(curP[0].GetY()-yMin)/range*2.0-1.0,
					(curP[0].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[3]), GetGValue(clr[3]), GetBValue(clr[3]));
				glVertex3d(
					(curP[3].GetX()-xMin)/range*2.0-1.0, 
					(curP[3].GetY()-yMin)/range*2.0-1.0,
					(curP[3].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[7]), GetGValue(clr[7]), GetBValue(clr[7]));
				glVertex3d(
					(curP[7].GetX()-xMin)/range*2.0-1.0, 
					(curP[7].GetY()-yMin)/range*2.0-1.0,
					(curP[7].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[4]), GetGValue(clr[4]), GetBValue(clr[4]));
				glVertex3d(
					(curP[4].GetX()-xMin)/range*2.0-1.0, 
					(curP[4].GetY()-yMin)/range*2.0-1.0,
					(curP[4].GetZ()-box.ZMin())/zRange*2.0-1.0);
				glEnd();
			}
		}
	}

	glDisable(GL_CULL_FACE);
}

void CGridObject::DrawShaded()
{
	if (m_displayMode == GLWIREFRAME)
		glColor3ub(155, 155, 155);
	//glColor4d(0.135000, 0.222500, 0.157500, 0.950000);

	glEnable(GL_CULL_FACE);
	glCullFace( GL_BACK ); 

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
		GLint faceindexes[24] =
		{
			0,1,2,3,								//面0
			4,7,6,5,								//面1
			0,4,5,1,								//面2
			1,5,6,2,								//面3
			2,6,7,3,								//面4
			0,3,7,4								//面5
		};
		
		for(int i=0; i<I; i++)
		{
			for(int j=0; j<J; j++)
			{
				for(int k=0; k<K; k++)
				{
					GLfloat postions[72];
					GLfloat normals[72];
					GLubyte colors[72];

					for(int index=0; index<24; index++)
					{
						postions[index*3] = m_gridCells[i][j][k].m_cornerPoint[faceindexes[index]].GetX();
						postions[index*3+1] = m_gridCells[i][j][k].m_cornerPoint[faceindexes[index]].GetY();
						postions[index*3+2] = m_gridCells[i][j][k].m_cornerPoint[faceindexes[index]].GetZ();

						normals[index*3] = m_gridCells[i][j][k].m_faceNormals[index>>2].GetX();		// index>>2为面号
						normals[index*3+1] = m_gridCells[i][j][k].m_faceNormals[index>>2].GetY();
						normals[index*3+2] = m_gridCells[i][j][k].m_faceNormals[index>>2].GetZ();
						if( !pParam && m_displayMode == GLSHADED )
						{
							colors[index*3] = m_gridCells[i][j][k].m_cornerPoint[faceindexes[index]].m_itsColor.GetR();
							colors[index*3+1] = m_gridCells[i][j][k].m_cornerPoint[faceindexes[index]].m_itsColor.GetG();
							colors[index*3+2] = m_gridCells[i][j][k].m_cornerPoint[faceindexes[index]].m_itsColor.GetB();
						}
					}
					glEnableClientState(GL_VERTEX_ARRAY);
					glEnableClientState(GL_NORMAL_ARRAY);
					glVertexPointer(3,GL_FLOAT,0,postions);
					glNormalPointer(GL_FLOAT,0,normals);
					if( !pParam && m_displayMode == GLSHADED )
					{
						glEnableClientState(GL_COLOR_ARRAY);
						glColorPointer(3,GL_UNSIGNED_BYTE,0,colors);
					}
					glBegin(GL_QUADS);
					for(int index =0; index<24; index++)
					{
						glArrayElement(index);
					}
					glEnd();
				}
			}
		}

	}
	else
	{	
		CVertex3D curP[8];

		int pCnt = 0;
		int nCell = 0;

		int i=0;
		int j=0;
		int k=0;

		//int size = m_pointList.GetSize();
		int size = m_pointList.size();
		for (int m=0; m<size; m++)
		{	
			curP[pCnt % 8] = m_pointList[m];
			pCnt++;
			if( pCnt % 8 == 0 )
			{
				nCell++;
				if( !m_bShowI[i] 
				&& !m_bShowJ[j] 
				&& !m_bShowK[k] )
				{
					i++;
					if(i==I)
					{
						i = 0;
						j++;
						if( j==J)
						{
							j = 0;
							k++;
						}
					}
					continue;
				}

				i++;
				if(i==I)
				{
					i = 0;
					j++;
					if( j==J)
					{
						j = 0;
						k++;
					}
				}

				if( pParam && nCell <= pParam->m_dValue.size() && m_displayMode == GLSHADED )
				{
					COLORREF clr = GetContext()->GetColorTable()->GetColor( (pParam->m_dValue[nCell]-pParam->m_dMinValue)/( pParam->m_dMaxValue- pParam->m_dMinValue));
					glColor4ub (GetRValue(clr), GetGValue(clr), GetBValue(clr), 255);
				}

				COLORREF clr[8];
				if( !pParam && m_displayMode == GLSHADED )
				{
					clr[0] = GetContext()->GetColorTable()->GetColor( (curP[0].GetZ()-box.ZMin())/zRange);
					clr[1] = GetContext()->GetColorTable()->GetColor( (curP[1].GetZ()-box.ZMin())/zRange);
					clr[2] = GetContext()->GetColorTable()->GetColor( (curP[2].GetZ()-box.ZMin())/zRange);
					clr[3] = GetContext()->GetColorTable()->GetColor( (curP[3].GetZ()-box.ZMin())/zRange);
					clr[4] = GetContext()->GetColorTable()->GetColor( (curP[4].GetZ()-box.ZMin())/zRange);
					clr[5] = GetContext()->GetColorTable()->GetColor( (curP[5].GetZ()-box.ZMin())/zRange);
					clr[6] = GetContext()->GetColorTable()->GetColor( (curP[6].GetZ()-box.ZMin())/zRange);
					clr[7] = GetContext()->GetColorTable()->GetColor( (curP[7].GetZ()-box.ZMin())/zRange);
				}

				CVector3D nrml;

				glBegin(GL_QUADS);
				nrml = CVector3D(curP[0], curP[1]).CrossProduct(CVector3D(curP[1], curP[2])); 
				nrml.Normalize();
				glNormal3f(nrml.GetX(), nrml.GetY(), nrml.GetZ());				

				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[0]), GetGValue(clr[0]), GetBValue(clr[0]));
				glVertex3d(
					(curP[0].GetX()-xMin)/range*2.0-1.0, 
					(curP[0].GetY()-yMin)/range*2.0-1.0,
					(curP[0].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[1]), GetGValue(clr[0]), GetBValue(clr[0]));
				glVertex3d(
					(curP[1].GetX()-xMin)/range*2.0-1.0, 
					(curP[1].GetY()-yMin)/range*2.0-1.0,
					(curP[1].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[2]), GetGValue(clr[2]), GetBValue(clr[2]));
				glVertex3d(
					(curP[2].GetX()-xMin)/range*2.0-1.0, 
					(curP[2].GetY()-yMin)/range*2.0-1.0,
					(curP[2].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[3]), GetGValue(clr[3]), GetBValue(clr[3]));
				glVertex3d(
					(curP[3].GetX()-xMin)/range*2.0-1.0, 
					(curP[3].GetY()-yMin)/range*2.0-1.0,
					(curP[3].GetZ()-box.ZMin())/zRange*2.0-1.0);
				glEnd();

				glBegin(GL_QUADS);
				nrml = CVector3D(curP[4], curP[7]).CrossProduct(CVector3D(curP[7], curP[6])); 
				nrml.Normalize();
				glNormal3f(nrml.GetX(), nrml.GetY(), nrml.GetZ());

				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[4]), GetGValue(clr[4]), GetBValue(clr[4]));
				glVertex3d(
					(curP[4].GetX()-xMin)/range*2.0-1.0, 
					(curP[4].GetY()-yMin)/range*2.0-1.0,
					(curP[4].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[7]), GetGValue(clr[7]), GetBValue(clr[7]));
				glVertex3d(
					(curP[7].GetX()-xMin)/range*2.0-1.0, 
					(curP[7].GetY()-yMin)/range*2.0-1.0,
					(curP[7].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[6]), GetGValue(clr[6]), GetBValue(clr[6]));
				glVertex3d(
					(curP[6].GetX()-xMin)/range*2.0-1.0, 
					(curP[6].GetY()-yMin)/range*2.0-1.0,
					(curP[6].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[5]), GetGValue(clr[5]), GetBValue(clr[5]));
				glVertex3d(
					(curP[5].GetX()-xMin)/range*2.0-1.0, 
					(curP[5].GetY()-yMin)/range*2.0-1.0,
					(curP[5].GetZ()-box.ZMin())/zRange*2.0-1.0);
				glEnd();


				glBegin(GL_QUADS);
				nrml = CVector3D(curP[0], curP[4]).CrossProduct(CVector3D(curP[4], curP[5])); 
				nrml.Normalize();
				glNormal3f(nrml.GetX(), nrml.GetY(), nrml.GetZ());

				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[0]), GetGValue(clr[0]), GetBValue(clr[0]));
				glVertex3d(
					(curP[0].GetX()-xMin)/range*2.0-1.0, 
					(curP[0].GetY()-yMin)/range*2.0-1.0,
					(curP[0].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[4]), GetGValue(clr[4]), GetBValue(clr[4]));
				glVertex3d(
					(curP[4].GetX()-xMin)/range*2.0-1.0, 
					(curP[4].GetY()-yMin)/range*2.0-1.0,
					(curP[4].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[5]), GetGValue(clr[5]), GetBValue(clr[5]));
				glVertex3d(
					(curP[5].GetX()-xMin)/range*2.0-1.0, 
					(curP[5].GetY()-yMin)/range*2.0-1.0,
					(curP[5].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[1]), GetGValue(clr[1]), GetBValue(clr[1]));
				glVertex3d(
					(curP[1].GetX()-xMin)/range*2.0-1.0, 
					(curP[1].GetY()-yMin)/range*2.0-1.0,
					(curP[1].GetZ()-box.ZMin())/zRange*2.0-1.0);
				glEnd();


				glBegin(GL_QUADS);
				nrml = CVector3D(curP[1], curP[5]).CrossProduct(CVector3D(curP[5], curP[6])); 
				nrml.Normalize();
				glNormal3f(nrml.GetX(), nrml.GetY(), nrml.GetZ());

				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[1]), GetGValue(clr[1]), GetBValue(clr[1]));
				glVertex3d(
					(curP[1].GetX()-xMin)/range*2.0-1.0, 
					(curP[1].GetY()-yMin)/range*2.0-1.0,
					(curP[1].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[5]), GetGValue(clr[5]), GetBValue(clr[5]));
				glVertex3d(
					(curP[5].GetX()-xMin)/range*2.0-1.0, 
					(curP[5].GetY()-yMin)/range*2.0-1.0,
					(curP[5].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[6]), GetGValue(clr[6]), GetBValue(clr[6]));
				glVertex3d(
					(curP[6].GetX()-xMin)/range*2.0-1.0, 
					(curP[6].GetY()-yMin)/range*2.0-1.0,
					(curP[6].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[2]), GetGValue(clr[2]), GetBValue(clr[2]));
				glVertex3d(
					(curP[2].GetX()-xMin)/range*2.0-1.0, 
					(curP[2].GetY()-yMin)/range*2.0-1.0,
					(curP[2].GetZ()-box.ZMin())/zRange*2.0-1.0);
				glEnd();

				glBegin(GL_QUADS);
				nrml = CVector3D(curP[2], curP[6]).CrossProduct(CVector3D(curP[6], curP[7])); 
				nrml.Normalize();
				glNormal3f(nrml.GetX(), nrml.GetY(), nrml.GetZ());

				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[2]), GetGValue(clr[2]), GetBValue(clr[2]));
				glVertex3d(
					(curP[2].GetX()-xMin)/range*2.0-1.0, 
					(curP[2].GetY()-yMin)/range*2.0-1.0,
					(curP[2].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[6]), GetGValue(clr[6]), GetBValue(clr[6]));
				glVertex3d(
					(curP[6].GetX()-xMin)/range*2.0-1.0, 
					(curP[6].GetY()-yMin)/range*2.0-1.0,
					(curP[6].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[7]), GetGValue(clr[7]), GetBValue(clr[7]));
				glVertex3d(
					(curP[7].GetX()-xMin)/range*2.0-1.0, 
					(curP[7].GetY()-yMin)/range*2.0-1.0,
					(curP[7].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[3]), GetGValue(clr[3]), GetBValue(clr[3]));
				glVertex3d(
					(curP[3].GetX()-xMin)/range*2.0-1.0, 
					(curP[3].GetY()-yMin)/range*2.0-1.0,
					(curP[3].GetZ()-box.ZMin())/zRange*2.0-1.0);
				glEnd();

				glBegin(GL_QUADS);
				nrml = CVector3D(curP[0], curP[3]).CrossProduct(CVector3D(curP[3], curP[7])); 
				nrml.Normalize();
				glNormal3f(nrml.GetX(), nrml.GetY(), nrml.GetZ());

				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[0]), GetGValue(clr[0]), GetBValue(clr[0]));
				glVertex3d(
					(curP[0].GetX()-xMin)/range*2.0-1.0, 
					(curP[0].GetY()-yMin)/range*2.0-1.0,
					(curP[0].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[3]), GetGValue(clr[3]), GetBValue(clr[3]));
				glVertex3d(
					(curP[3].GetX()-xMin)/range*2.0-1.0, 
					(curP[3].GetY()-yMin)/range*2.0-1.0,
					(curP[3].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[7]), GetGValue(clr[7]), GetBValue(clr[7]));
				glVertex3d(
					(curP[7].GetX()-xMin)/range*2.0-1.0, 
					(curP[7].GetY()-yMin)/range*2.0-1.0,
					(curP[7].GetZ()-box.ZMin())/zRange*2.0-1.0);
				if( !pParam && m_displayMode == GLSHADED )
					glColor3ub(GetRValue(clr[4]), GetGValue(clr[4]), GetBValue(clr[4]));
				glVertex3d(
					(curP[4].GetX()-xMin)/range*2.0-1.0, 
					(curP[4].GetY()-yMin)/range*2.0-1.0,
					(curP[4].GetZ()-box.ZMin())/zRange*2.0-1.0);
				glEnd();
			}
		}
	}

	glDisable(GL_CULL_FACE);
}

void CGridObject::ComputePoints()
{
	CBoundingBox box = m_itsBox;
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
					m_gridCells[i][j][k].m_cornerPoint[index].SetX( (m_gridCells[i][j][k].m_cornerPoint[index].GetX()-xMin)/range*2.0-1.0);
					m_gridCells[i][j][k].m_cornerPoint[index].SetY( (m_gridCells[i][j][k].m_cornerPoint[index].GetY()-yMin)/range*2.0-1.0);
					m_gridCells[i][j][k].m_cornerPoint[index].SetZ( (m_gridCells[i][j][k].m_cornerPoint[index].GetZ()-box.ZMin())/zRange*2.0-1.0);
					
					m_gridCells[i][j][k].m_cornerPoint[index].m_itsColor = GetContext()->GetColorTable()->GetColor( (m_gridCells[i][j][k].m_cornerPoint[index].GetZ()-box.ZMin())/zRange);
				}
			}
		}
	}
}
