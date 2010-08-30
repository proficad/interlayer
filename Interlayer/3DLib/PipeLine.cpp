#include "StdAfx.h"
#include "PipeLine.h"
#include "GLDisplayContext.h"
#include "float.h"
#include "../MainFrm.h"

extern double Angle(const CVector3D& U, const CVector3D& V);
//------------------------------------------------------------------------

CPipeLine::CPipeLine(void)
{
	m_strObjName = _T("三维管子");
	m_ListDone = 0;
	m_Modified = 1;
	m_ColorBinding = COLOR_PER_VERTEX;

	m_glObjType = GLPIPELINE;

	GUID guid;
	if (S_OK == ::CoCreateGuid(&guid))
	{
		m_glObjID = guid.Data1;
	}
	else
		m_glObjID = (int)m_glObjType + CGLObject::GetCount();
	
	m_dRadius = 10;

	SetMaterial(White);
}

CPipeLine::~CPipeLine(void)
{
}


void CPipeLine::Serialize(CArchive &ar)
{
	CGLObject::Serialize(ar);

	if( ar.IsStoring() )
	{		
		ar << m_dRadius;
		ar << m_Pipeline;
	}
	else
	{
		ar >> m_dRadius;
		ar >> m_Pipeline;

		ComputeBoundLimits();
	}
}

CGLObject* CPipeLine::Copy()
{
	CPipeLine* O = new CPipeLine();
	O->SetContext(m_pDisplayContext);
	O->m_glObjID = m_glObjID;
	O->m_strObjName = m_strObjName;
	O->m_glObjType = m_glObjType;

	O->m_dRadius = m_dRadius;
	O->m_Pipeline = m_Pipeline;

	O->ComputeBoundLimits();

	return O;
}


void CPipeLine::DefineDisplay()
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
		glPushAttrib(GL_LIGHTING_BIT);
		glDisable(GL_LIGHTING);
		glDisable(GL_COLOR_MATERIAL);
		glEnable(GL_DEPTH_TEST);
		DrawWired();
		glPopAttrib();
	}
	
	glDisable(GL_BLEND);

	pMF->GetStatusBar().SetPaneText(0, _T("就绪"));	
}

void CPipeLine::Display(const GLDisplayMode& dMode, bool bForce)
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

bool CPipeLine::BuildList()
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

void CPipeLine::DisplaySelected(const GLSelectedMode& dMode)
{
	Hilight(dMode);
}

void CPipeLine::Hilight(const GLSelectedMode& dMode)
{
}


void CPipeLine::ComputeBoundLimits()
{
	double dMinX = DBL_MAX,dMinY = DBL_MAX,dMinZ = DBL_MAX, dMaxX = -DBL_MAX, dMaxY = -DBL_MAX, dMaxZ = -DBL_MAX;

	CVertex3D P;

	int nSize = m_Pipeline.m_arrPoint.GetSize();

	for (int i=0; i<nSize; i++)
	{
		P = m_Pipeline.m_arrPoint[i];

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

	m_itsBox.SetLimits(dMinX, dMaxX, dMinY, dMaxY, dMinZ, dMaxZ);
}

void CPipeLine::DrawWired()
{
	glColor3ub(GetColor()->GetR(), GetColor()->GetG(), GetColor()->GetB());
	
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

	glBegin(GL_LINE_STRIP);

	int nSize = m_Pipeline.m_arrPoint.GetSize();

	for(int i = 0; i < nSize; i++)
	{
		glVertex3d((m_Pipeline.m_arrPoint[i].x-xMin)/range*2.0-1.0,
			(m_Pipeline.m_arrPoint[i].y-yMin)/range*2.0-1.0,
			(m_Pipeline.m_arrPoint[i].z-box.ZMin())/zRange*2.0-1.0);
	}
	glEnd();

	DrawTitle();
}

void CPipeLine::DrawShaded()
{
	if (m_displayMode == GLSHADED)	// 亮度
		glColor3ub(155, 155, 155);
	//glColor4d(0.135000, 0.222500, 0.157500, 0.950000);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	DrawPipe();
	DrawTitle();
}

void CPipeLine::DrawTitle()
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

	glRasterPos3d((m_Pipeline.m_arrPoint[0].x-xMin)/range*2.0-1.0,
		(m_Pipeline.m_arrPoint[0].y-yMin)/range*2.0-1.0, 
		(m_Pipeline.m_arrPoint[0].z-box.ZMin())/zRange*2.0-1.0+0.05f);
	DrawText(m_Pipeline.m_strTitle);

	glPushMatrix();
	glTranslatef((m_Pipeline.m_arrPoint[0].x-xMin)/range*2.0-1.0,
		(m_Pipeline.m_arrPoint[0].y-yMin)/range*2.0-1.0, 
		(m_Pipeline.m_arrPoint[0].z-box.ZMin())/zRange*2.0-1.0);
	glScalef(1.0f, 1.0f, 1.0/GetContext()->m_dVertical);
	glutSolidSphere(0.01,40,40);
	glPopMatrix();
	
}

void CPipeLine::DrawPipe()
{
	glColor3ub(GetColor()->GetR(), GetColor()->GetG(), GetColor()->GetB());
	
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

	// 可以将曲线的每个点都画成球体，在曲线的每两个点之间画一个圆柱体

	if( m_Pipeline.m_arrPoint.GetSize() == 2 )
	{
		glRasterPos3d((m_Pipeline.m_arrPoint[m_Pipeline.m_arrPoint.GetSize()-1].x-xMin)/range*2.0-1.0,
			(m_Pipeline.m_arrPoint[m_Pipeline.m_arrPoint.GetSize()-1].y-yMin)/range*2.0-1.0, 
			(m_Pipeline.m_arrPoint[m_Pipeline.m_arrPoint.GetSize()-1].z-box.ZMin())/zRange*2.0-1.0);

		glPushMatrix();
		glTranslatef((m_Pipeline.m_arrPoint[m_Pipeline.m_arrPoint.GetSize()-1].x-xMin)/range*2.0-1.0,
			(m_Pipeline.m_arrPoint[m_Pipeline.m_arrPoint.GetSize()-1].y-yMin)/range*2.0-1.0, 
			(m_Pipeline.m_arrPoint[m_Pipeline.m_arrPoint.GetSize()-1].z-box.ZMin())/zRange*2.0-1.0);
		GLUquadricObj *quadratic; 
		quadratic=gluNewQuadric(); // 创建二次几何体 
		gluQuadricDrawStyle(quadratic, GL_FILL);
		gluQuadricNormals(quadratic, GLU_SMOOTH); // 使用平滑法线 
		gluQuadricTexture(quadratic, GL_TRUE); // 使用纹理
		
		double x2, x1, y2, y1, z2, z1;
		x2 = (m_Pipeline.m_arrPoint[m_Pipeline.m_arrPoint.GetSize()-1].x-xMin)/range*2.0-1.0;
		x1 = (m_Pipeline.m_arrPoint[m_Pipeline.m_arrPoint.GetSize()-2].x-xMin)/range*2.0-1.0;
		y2 = (m_Pipeline.m_arrPoint[m_Pipeline.m_arrPoint.GetSize()-1].y-yMin)/range*2.0-1.0;
		y1 = (m_Pipeline.m_arrPoint[m_Pipeline.m_arrPoint.GetSize()-2].y-yMin)/range*2.0-1.0;
		z2 = (m_Pipeline.m_arrPoint[m_Pipeline.m_arrPoint.GetSize()-1].z-box.ZMin())/zRange*2.0-1.0;
		z1 = (m_Pipeline.m_arrPoint[m_Pipeline.m_arrPoint.GetSize()-2].z-box.ZMin())/zRange*2.0-1.0;

		double height = sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)+(z2-z1)*(z2-z1));

		gluCylinder(quadratic,0.003f,0.003f,height,10,10);
		gluDeleteQuadric(quadratic);
		glPopMatrix();		
	}
	else
	{
		for(int i = 1; i < m_Pipeline.m_arrPoint.GetSize(); i++)
		{
			glRasterPos3d((m_Pipeline.m_arrPoint[i].x-xMin)/range*2.0-1.0,
				(m_Pipeline.m_arrPoint[i].y-yMin)/range*2.0-1.0, 
				(m_Pipeline.m_arrPoint[i].z-box.ZMin())/zRange*2.0-1.0);

			glPushMatrix();
			
			glTranslatef((m_Pipeline.m_arrPoint[i].x-xMin)/range*2.0-1.0,
				(m_Pipeline.m_arrPoint[i].y-yMin)/range*2.0-1.0, 
				(m_Pipeline.m_arrPoint[i].z-box.ZMin())/zRange*2.0-1.0);
			
			glScalef(1.0f, 1.0f, 1.0f);
			
			if( i<m_Pipeline.m_arrPoint.GetSize()-1)
				glutSolidSphere(0.003, 10, 10); // 球		

			GLUquadricObj *quadratic; 
			quadratic=gluNewQuadric(); // 创建二次几何体 
			gluQuadricDrawStyle(quadratic, GL_FILL);
			gluQuadricNormals(quadratic, GLU_SMOOTH); // 使用平滑法线 
			gluQuadricTexture(quadratic, GL_TRUE); // 使用纹理

			double x2, x1, y2, y1, z2, z1;
			x2 = (m_Pipeline.m_arrPoint[i].x-xMin)/range*2.0-1.0;
			x1 = (m_Pipeline.m_arrPoint[i-1].x-xMin)/range*2.0-1.0;
			y2 = (m_Pipeline.m_arrPoint[i].y-yMin)/range*2.0-1.0;
			y1 = (m_Pipeline.m_arrPoint[i-1].y-yMin)/range*2.0-1.0;
			z2 = (m_Pipeline.m_arrPoint[i].z-box.ZMin())/zRange*2.0-1.0;
			z1 = (m_Pipeline.m_arrPoint[i-1].z-box.ZMin())/zRange*2.0-1.0;

			double height = sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)+(z2-z1)*(z2-z1));

			if( height > 1e-10) 
			{
				double angle;
				if( fabs(x2-x1) < 1e-10 )
					angle = 90;
				else
					angle = atan((y2-y1)/(x2-x1))*180.0/PI;

				glRotatef(angle, 0.0, 0.0, 1.0);
				angle = acos((z2-z1)/height)*180.0/PI;
				if( angle>90)
					angle = 180 - angle;
				if( angle < 0.00001 )
					angle = 0.0;

				if( x2>x1)
					angle = -angle;
				glRotatef(angle, 0.0, 1.0, 0.0);
				gluCylinder(quadratic,0.003f,0.003f,height,10,10);
			}
			gluDeleteQuadric(quadratic);
			
			glPopMatrix();
		}
	}
}
