// GLTrihedron.cpp: implementation of the CGLTrihedron class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GLTrihedron.h"
#include "GLView.h"
#include "GridObject.h"
#include "GLText.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGLTrihedron::CGLTrihedron()
{
	m_strObjName = _T("Trihedron");
	m_glObjType = GLTRIHEDRON;
	m_bLegend = false;
	m_pGLView = NULL;
	m_dDepth = FAR_CLIP;
	csSize = 1.0;
	m_bShowAxis = true;
	m_bShowGrid = true;

	SetMaterial(White);

	m_ListOpenGL = 0;
}

CGLTrihedron::CGLTrihedron(GLfloat size)
{
	m_strObjName = _T("Trihedron");
	m_glObjType = GLTRIHEDRON;
	m_bLegend = false;
	m_pGLView = NULL;
	csSize = size;
	m_bShowAxis = true;
	m_bShowGrid = true;

	ComputeBoundLimits();

	m_dDepth = FAR_CLIP;
	SetMaterial(White);

	m_ListOpenGL = 0;
}

CGLTrihedron::~CGLTrihedron()
{
}

CGLObject* CGLTrihedron::Copy()
{
	CGLTrihedron* T = new CGLTrihedron(csSize);
	T->m_glObjID = m_glObjID;
	T->SetGLView (m_pGLView);
	return T;
}

void CGLTrihedron::DefineDisplay() // 定义显示
{
	GLdouble bgcol[4];	
	
	glGetDoublev(GL_COLOR_CLEAR_VALUE, bgcol);
	GLfloat specref[] =
	{
		1.0f, 1.0f, 1.0f, 1.0f
	};

	// Enable Depth Testing
	//glEnable(GL_DEPTH_TEST);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	// Enable lighting
	glEnable(GL_LIGHTING);

	// Enable color tracking
	glEnable(GL_COLOR_MATERIAL);

	// Set Material properties to follow glColor values
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

// 	GLfloat light_position[] = { 2.0, 2.0, 2.0, 0.0 };
// 
// 	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specref);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128);

	glShadeModel(GL_SMOOTH);

	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	ApplyMaterial();
	
}

void CGLTrihedron::DrawAxis()
{
	glEnable(GL_BLEND);
	glPushAttrib(GL_LIGHTING_BIT);
	
	//Axes' label
	glColor3f(1.0f, 0.0f, 0.0f);
	glRasterPos3d(1.3, -1.0, -1.0);
	DrawText("X");

	glColor3f(0.0f, 1.0f, 0.0f);
	glRasterPos3d(-1.0, 1.3, -1.0);
	DrawText("Y");

	glColor3f(1.0f, -1.0f, 1.0f);
	glRasterPos3d(-1.0, -1.0, 1.3);
	DrawText("Z");

	//X Axis
	glColor3f(0.5f, 0.25f, 0.25f);
	glBegin(GL_LINES);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(1.1f, -1.0f, -1.0f);
	glEnd();

	glBegin(GL_LINES);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glEnd();

	glBegin(GL_LINES);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glEnd();

	glBegin(GL_LINES);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glEnd();

	glPushMatrix();	
	glTranslatef(1.1f, -1.0f, -1.0f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	glutSolidCone(0.027 * csSize, 0.09 * csSize, 10, 10);
	glPopMatrix();

	//Y Axis
	glColor3f(0.25f, 0.5f, 0.25f);

	glBegin(GL_LINES);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, 1.1f, -1.0f);
	glEnd();

	glBegin(GL_LINES);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glEnd();

	glBegin(GL_LINES);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glEnd();

	glBegin(GL_LINES);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glEnd();

	glPushMatrix();	
	glTranslatef(-1.0f, 1.1f, -1.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	glutSolidCone(0.027 * csSize, 0.09 * csSize, 10, 10);
	glPopMatrix();

	//Z Axis
	glColor3f(0.25f, 0.25f, 0.5f);

	glBegin(GL_LINES);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, 1.1f);
	glEnd();

	glBegin(GL_LINES);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glEnd();

	glBegin(GL_LINES);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glEnd();

	glBegin(GL_LINES);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glEnd();

	glPushMatrix();	
	glTranslatef(-1.0f, -1.0f, 1.1f);
	glutSolidCone(0.027 * csSize, 0.09 * csSize, 10, 10);
	glPopMatrix();

	glPopAttrib();
	glDisable(GL_BLEND);
}

void CGLTrihedron::DrawGrid()
{	
	glEnable(GL_BLEND);
	glPushAttrib(GL_LIGHTING_BIT);
	glLineWidth(0.1);
	//X Axis
	glColor3f(0.25f, 0.25f, 0.25f);
	

	for (int i=1;i<40; i++)
	{
		glBegin(GL_LINES);
		glVertex3f(-2.0f, -2.0f+i*0.1f, -1.0f);
		glVertex3f(2.0f, -2.0f+i*0.1f, -1.0f);
		glEnd();
	}

	//Y Axis	
	for (int i=1;i<40; i++)
	{
		glBegin(GL_LINES);
		glVertex3f(-2.0f+i*0.1f, -2.0f, -1.0f);
		glVertex3f(-2.0f+i*0.1f, 2.0f, -1.0f);
		glEnd();
	}

	glPopAttrib();
	glDisable(GL_BLEND);
}

void CGLTrihedron::Display(const GLDisplayMode& dMode, bool bForce)
{
	glLoadName(reinterpret_cast<GLuint>(this));
	if (!m_Show)
		return;
	
	// Build list at first
	if (!m_ListDone || m_Modified)
		if (!BuildList())
			return;

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

bool CGLTrihedron::BuildList()
{
	//TRACE(" Start building list ...\n");

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

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if( m_bLegend )
	{
		DrawLegend();
		DrawMark();
	}

	DefineDisplay(); // 定义显示

	if( m_bShowAxis )
		DrawAxis();
	if( m_bShowGrid)
		DrawGrid();

	::glEndList();

	// List is done now
	m_ListDone = 1;
	m_Modified = 0;

	return true;
}

void CGLTrihedron::DisplaySelected(const GLSelectedMode& dMode)
{
	Hilight(dMode);
}

void CGLTrihedron::Hilight(const GLSelectedMode& dMode)
{
}

void CGLTrihedron::SetSize(GLfloat size)
{
	if( csSize != size )
		m_Modified = 1;
	csSize = size;
}

void CGLTrihedron::ComputeBoundLimits()
{
	m_itsBox.SetLimits(-csSize, csSize, -csSize, csSize, -csSize, csSize);
}

void CGLTrihedron::SetColor(const GLubyte& red, const GLubyte& green, const GLubyte& blue)
{
	
}

void CGLTrihedron::SetColor(CColor& color)
{
	
}

void CGLTrihedron::GetColor(GLubyte* colVect) const
{	
}

CColor* CGLTrihedron::GetColor(void)
{
	return NULL;
}

// 画图例
void CGLTrihedron::ShowLegend(bool bLegend)
{
	if( m_bLegend != bLegend )
	{
		m_bLegend = bLegend;
		m_Modified = 1;
	}
}

bool CGLTrihedron::IsShowLegend()
{
	return m_bLegend;
}

void CGLTrihedron::SetGLView(CGLView* pGLView)
{
	m_pGLView = pGLView;
	m_Modified = 1;
}

void CGLTrihedron::DrawLegend()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 1, 0, 1, 0, -m_dDepth);

	glPushAttrib(GL_ENABLE_BIT| GL_TEXTURE_BIT | GL_POLYGON_BIT);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);  
	glDisable(GL_LIGHTING);

	CGLView *pGLView = m_pGLView;
	if( pGLView == NULL )
		return;
	CColorLookUpTable * pColorTable = GetContext()->GetColorTable();
	int size = pColorTable->GetColorTableCount();
	CRect rect;
	pGLView->GetWnd()->GetClientRect(rect);

	double width = rect.Width();
	double height = rect.Height();

	double w = 24.0;
	double h = 200.0;

	double right = 1.0-64.0/width;
	double left = right - w/width;
	double bottom = 60.0/width;
	double top = bottom + h/height;

	double y, dMax, dMin;

	pColorTable->GetValueRange(dMin, dMax);

	glBegin (GL_QUAD_STRIP);

	for (int i=0;i<size; i++)
	{
		double val = pColorTable->GetValue(i)/(dMax - dMin);
		COLORREF clr = pColorTable->GetColor(val);

		y = bottom - (bottom-top) * val;

		glColor3d (GetRValue(clr)/255.0, GetGValue(clr)/255.0, GetBValue(clr)/255.0);
		glVertex2f (left, y);

		glColor3d (GetRValue(clr)/255.0, GetGValue(clr)/255.0, GetBValue(clr)/255.0);
		glVertex2f (right, y);
	}
	glEnd();

	glBegin (GL_LINES);
	for (int i=0;i<11;i++)
	{
		double val = i*0.1;
		COLORREF clr = pColorTable->GetColor(val);
		glColor3d (GetRValue(clr)/255.0, GetGValue(clr)/255.0, GetBValue(clr)/255.0);
		y = bottom - (bottom-top) * val;
		glVertex2f (left, y);
		glVertex2f (right + 8.0/width, y);
	}
	glEnd();

	glPopAttrib();
	glPopMatrix();	
		
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void CGLTrihedron::DrawMark()
{
	CGLView *pGLView = m_pGLView;
	if( pGLView == NULL )
		return;

	//CColorLookUpTable * pColorTable = pGLView->GetColorTable();
	
	GLdouble bgcol[4];	

	glGetDoublev(GL_COLOR_CLEAR_VALUE, bgcol);
	GLfloat specref[] =
	{
		1.0f, 1.0f, 1.0f, 1.0f
	};

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	//glOrtho(0, 1, 0, 1, 0, -20000.0);
	glOrtho(0, 1, 0, 1, 0, -m_dDepth);

	glPushAttrib(GL_ENABLE_BIT| GL_TEXTURE_BIT | GL_POLYGON_BIT);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);  
	glDisable(GL_LIGHTING);

	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specref);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128);

	glShadeModel(GL_SMOOTH);

	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glPushAttrib(GL_LIGHTING_BIT);	
	
	ApplyMaterial();

	CRect rect;
	pGLView->GetWnd()->GetClientRect(rect);

	double width = rect.Width();
	double height = rect.Height();

	double w = 24.0;
	double h = 200.0;

	double right = 1.0-64.0/width;
	double left = right - w/width;
	double bottom = 60.0/width;
	double top = bottom + h/height;

	
	std::vector<CGLObject*>::iterator listIter;
	CGridObject *pGridObj = NULL;

	if (!GetContext()->m_listDisplay->empty())
	{
		for (listIter = GetContext()->m_listDisplay->begin(); listIter != GetContext()->m_listDisplay->end(); listIter++)
		{
			CGLObject *pObj = (*listIter);
			if( pObj->GetGLObjType() == GLSURFACE )
			{
				pGridObj = (CGridObject *)pObj;
				break;
			}
		}
	}	

	CColorLookUpTable * pColorTable = GetContext()->GetColorTable();
	CString str;
	CPhyPara *pParam = NULL;
	if( pGridObj )
	{
		int nSize = pGridObj->m_vecPhyPara.GetSize();
		for (int i=0; i<nSize; i++)
		{
			if( pGridObj->m_vecPhyPara[i].m_bShow )
			{
				pParam = &pGridObj->m_vecPhyPara[i];
				break;
			}
		}
	}

	for (int i=0;i<11;i++)
	{
		double val = i*0.1;

		double y = bottom - (bottom-top) * val;

 		COLORREF clr = pColorTable->GetColor(val);
 		glColor3d (GetRValue(clr)/255.0, GetGValue(clr)/255.0, GetBValue(clr)/255.0);
//		glColor3d (0.0, 0.0, 0.0);
		glRasterPos2d(right + 8.0/width, y);
		
		if( pParam && pParam->m_dValue.size()>0 )
			val = pParam->m_dMinValue + (i*0.1)*(pParam->m_dMaxValue-pParam->m_dMinValue);
		else if(pGridObj)
		{
			CBoundingBox box = m_pDisplayContext->GetBoundingBox();
			double range = box.ZMax() - box.ZMin();
			val = box.ZMin() + (i*0.1) * range;
		}

		str.Format(_T("%.2lf"),val);
		DrawText(str);
	}
	
	glPopAttrib();
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

CGLEditAxis::CGLEditAxis()
{
	m_strObjName = _T("EditAxis");
	m_glObjType = GLEDITAXIS;
	//m_bLegend = false;
	m_pGLView = NULL;

	m_bShowAxis = true;
	//m_bShowGrid = true;
	m_size = 1.0;
	SetMaterial(White);
	Show(0);
	m_ListOpenGL = 0;
}

CGLEditAxis::CGLEditAxis( GLfloat size )
{
	m_strObjName = _T("Trihedron");
	m_glObjType = GLTRIHEDRON;
	//m_bLegend = false;
	m_pGLView = NULL;
	m_size = size;
	m_bShowAxis = true;
	//m_bShowGrid = true;

	SetMaterial(White);
	Show(0);
	m_ListOpenGL = 0;
}

CGLEditAxis::~CGLEditAxis()
{

}

CGLObject* CGLEditAxis::Copy()
{
	CGLEditAxis* T = new CGLEditAxis(m_size);
	T->m_glObjID = m_glObjID;
	T->SetGLView (m_pGLView);
	return T;
}

bool CGLEditAxis::BuildList()
{
	//TRACE(" Start building list ...\n");

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

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	DefineDisplay(); // 定义显示

	::glEndList();

	// List is done now
	m_ListDone = 1;
	m_Modified = 0;

	return true;
}

void CGLEditAxis::DefineDisplay()
{
	GLdouble bgcol[4];	

	glGetDoublev(GL_COLOR_CLEAR_VALUE, bgcol);
	GLfloat specref[] =
	{
		1.0f, 1.0f, 1.0f, 1.0f
	};

	// Enable Depth Testing
	//glEnable(GL_DEPTH_TEST);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	// Enable lighting
	glEnable(GL_LIGHTING);

	// Enable color tracking
	glEnable(GL_COLOR_MATERIAL);

	// Set Material properties to follow glColor values
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	// 	GLfloat light_position[] = { 2.0, 2.0, 2.0, 0.0 };
	// 
	// 	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specref);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128);

	glShadeModel(GL_SMOOTH);

	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ApplyMaterial();

	DrawShaded();
}

void CGLEditAxis::Display( const GLDisplayMode& /*= GLWIREFRAME*/, bool bForce /*= false*/ )
{
	//glLoadName(reinterpret_cast<GLuint>(this));
	if (!m_Show)
		return;

	// Build list at first
	if (!m_ListDone || m_Modified)
		if (!BuildList())
			return;

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
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//DefineDisplay();
	//DrawShaded();
}

void CGLEditAxis::DisplaySelected( const GLSelectedMode& /*= GLSELECTED*/ )
{

}

void CGLEditAxis::Hilight( const GLSelectedMode& )
{

}

void CGLEditAxis::SetSize( GLfloat size )
{
	if( m_size != size )
		m_Modified = 1;
	m_size = size;
}

void CGLEditAxis::SetColor( const GLubyte&, const GLubyte&, const GLubyte& )
{

}

void CGLEditAxis::SetColor( CColor& color )
{

}

void CGLEditAxis::GetColor( GLubyte* ) const
{

}

CColor* CGLEditAxis::GetColor( void )
{
	return NULL;
}

void CGLEditAxis::SetPosition( const CVector3DF& position )
{
	m_v3Position = position;
}

CVector3DF CGLEditAxis::GetPosition()
{
	return m_v3Position;
}

void CGLEditAxis::DrawShaded()
{
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

	glLineWidth(4);
	glEnable(GL_BLEND);
	glPushAttrib(GL_LIGHTING_BIT);
	CVector3DF position;
	position.SetX((m_v3Position.GetX()-xMin)/range*2.0-1.0);
	position.SetY((m_v3Position.GetY()-yMin)/range*2.0-1.0);
	position.SetZ( m_pDisplayContext->m_dVertical*((m_v3Position.GetZ()-box.ZMin())/zRange*2.0-1.0) );

	//Axes' label
	//glLoadName(reinterpret_cast<GLuint>(this));
	glColor3f(0.0f, 0.0f, 0.0f);
	glRasterPos3d(position.GetX()+m_size, position.GetY(), position.GetZ());
	DrawText("X");

	glColor3f(0.0f, 1.0f, 0.0f);
	glRasterPos3d(position.GetX(), position.GetY()+m_size, position.GetZ());
	DrawText("Y");

	glColor3f(0.0f, 1.0f, 1.0f);
	glRasterPos3d(position.GetX(), position.GetY(), position.GetZ()+m_size);
	DrawText("Z");

	//X Axis
	glLoadName(101);
	glColor3f(0.5f, 0.25f, 0.25f);

	glBegin(GL_LINES);
	glVertex3f(position.GetX(), position.GetY(), position.GetZ());
	glVertex3f(position.GetX()+m_size, position.GetY(), position.GetZ());
	glEnd();

	glPushMatrix();	
	glTranslatef(position.GetX()+m_size, position.GetY(), position.GetZ());
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	glutSolidCone(0.027*m_size*2 , 0.09*m_size*2 , 10, 10);
	glPopMatrix();

	//Y Axis
	glLoadName(102);
	glColor3f(0.25f, 0.5f, 0.25f);

	glBegin(GL_LINES);
	glVertex3f(position.GetX(), position.GetY(), position.GetZ());
	glVertex3f(position.GetX(), position.GetY()+m_size, position.GetZ());
	glEnd();


	glPushMatrix();	
	glTranslatef(position.GetX(), position.GetY()+m_size, position.GetZ());
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	glutSolidCone(0.027*m_size*2 , 0.09*m_size*2 , 10, 10);
	glPopMatrix();

	//Z Axis
	glLoadName(103);
	glColor3f(0.25f, 0.25f, 0.5f);

	glBegin(GL_LINES);
	glVertex3f(position.GetX(), position.GetY(), position.GetZ());
	glVertex3f(position.GetX(), position.GetY(), position.GetZ()+m_size);
	glEnd();

	glPushMatrix();	
	glTranslatef(position.GetX(), position.GetY(), position.GetZ()+m_size);
	glutSolidCone(0.027*m_size*2 , 0.09*m_size*2 , 10, 10);
	glPopMatrix();

	glPopAttrib();
	glDisable(GL_BLEND);
	glLineWidth(1);
}

void CGLEditAxis::SetGLView( CGLView* pGLView )
{
	m_pGLView = pGLView;
	m_Modified = 1;
}