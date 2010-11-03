// GLView.cpp : implementation file
//

#include "stdafx.h"
#include "GLView.h"
#include "GeomException.h"
#include "../3DLib/C3DDice.h"
#include "../3DLib/C3DSlice.h"
#include "../3DLib/Plane.h"
#include "../MainFrm.h"
#include "PsRenderer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGLView

CGLView::CGLView(CWnd* pWnd, CGLDisplayContext* ctx) : m_ptrWnd(pWnd), m_pContext(ctx)
{
	//1
	m_colorv[0].amb[0] = 0.0F;	m_colorv[0].amb[1] = 0.0F;	m_colorv[0].amb[2] = 0.0F;
	m_colorv[0].diff[0] = 0.1F;	m_colorv[0].diff[1] = 0.6F;	m_colorv[0].diff[2] = 0.3F;
	m_colorv[0].spec[0] = 1.0F;	m_colorv[0].spec[1] = 1.0F;	m_colorv[0].spec[2] = 1.0F;
	m_colorv[0].ratio = 0.75F;
	m_colorv[0].indexes[0] = 0;	m_colorv[0].indexes[1] = 0;	m_colorv[0].indexes[2] = 0;

	//2
	m_colorv[1].amb[0] = 0.0F;	m_colorv[1].amb[1] = 0.0F;	m_colorv[1].amb[2] = 0.0F;
	m_colorv[1].diff[0] = 0.0F;	m_colorv[1].diff[1] = 0.2F;	m_colorv[1].diff[2] = 0.5F;
	m_colorv[1].spec[0] = 1.0F;	m_colorv[1].spec[1] = 1.0F;	m_colorv[1].spec[2] = 1.0F;
	m_colorv[1].ratio = 0.75F;
	m_colorv[1].indexes[0] = 0;	m_colorv[1].indexes[1] = 0;	m_colorv[1].indexes[2] = 0;

	//3
	m_colorv[2].amb[0] = 0.0F;	m_colorv[2].amb[1] = 0.05F;	m_colorv[2].amb[2] = 0.05F;
	m_colorv[2].diff[0] = 0.6F;	m_colorv[2].diff[1] = 0.0F;	m_colorv[2].diff[2] = 0.8F ;
	m_colorv[2].spec[0] = 1.0F;	m_colorv[2].spec[1] = 1.0F;	m_colorv[2].spec[2] = 1.0F ;
	m_colorv[2].ratio = 0.75F;
	m_colorv[2].indexes[0] = 0;	m_colorv[2].indexes[1] = 0;	m_colorv[2].indexes[2] = 0 ;

	m_ZoomON = false;
	m_PanON = false;
	m_RotON = false;

	m_bSliceZ = false;
	m_dCurrZ = 0.0;
	m_bSliceXY = false;
	m_dCurrX1 = 1.0;					// 当前坐标值
	m_dCurrY1 = 1.0;
	m_dCurrX2 = -1.0;
	m_dCurrY2 = -1.0;

	m_myTrihedron = new CGLTrihedron;
	m_myTrihedron->SetContext(m_pContext);
	m_myTrihedron->SetGLView(this);
	
	InitGL();
}

CGLView::~CGLView()
{
	delete m_myTrihedron;	
	::wglDeleteContext(m_hRC);
	m_ptrWnd->ReleaseDC(m_ptrWnd->GetDC());
}

/////////////////////////////////////////////////////////////////////////////
// CGLView drawing

void CGLView::Draw()
{
	m_pContext->m_dSliceWidth = GetSliceHeight(1);
	m_pContext->m_dSliceHeight = GetSliceHeight(1, false);

	// Make the rendering context current
	::wglMakeCurrent(m_hDC, m_hRC);

	// Call to the rendering function
	RenderScene(GL_RENDER);

	// Swap our scene to the front
	::SwapBuffers(m_hDC);

	// Allow other rendering contexts to co-exist
	::wglMakeCurrent(m_hDC, NULL);
}

/////////////////////////////////////////////////////////////////////////////

void CGLView::ReSize(int cx, int cy)
{
	// TODO: Add your message handler code here
	::wglMakeCurrent(m_hDC, m_hRC);

	GLsizei w, h;
	w = cx;
	h = cy;

	//prevent divide by 0
	if (h == 0)
		h = 1;
	if (w == 0)
		w = 1;

	::glViewport(0, 0, w, h);

	// Reset coordinate system
	::glMatrixMode(GL_PROJECTION);
	::glLoadIdentity();

	gluPerspective(45.0, w/h, 1, 50);
	// Ortho Viewing
	//if (w <= h)
	//	::glOrtho(-m_pContext->m_dRange - m_pContext->m_xTrans, 
	//	m_pContext->m_dRange - m_pContext->m_xTrans, 
	//	-(m_pContext->m_dRange * h / w) - m_pContext->m_yTrans,
	//	(m_pContext->m_dRange * h / w) - m_pContext->m_yTrans, 
	//	-(m_pContext->m_dRange * 5000.0f) - m_pContext->m_zTrans, 
	//	(m_pContext->m_dRange * 5000.0f) - m_pContext->m_zTrans);
	//else
	//	::glOrtho(-(m_pContext->m_dRange * w / h) - m_pContext->m_xTrans, 
	//	(m_pContext->m_dRange * w / h) - m_pContext->m_xTrans,
	//	-m_pContext->m_dRange - m_pContext->m_yTrans,
	//	m_pContext->m_dRange - m_pContext->m_yTrans,
	//	-m_pContext->m_dRange * 5000.0f,
	//	m_pContext->m_dRange * 5000.0f);

	::glMatrixMode(GL_MODELVIEW);
	::glLoadIdentity();
	gluLookAt( 0, 0, m_pContext->m_dRange*5, 0, 0, 0, 0, 1, 0 );

	::wglMakeCurrent(NULL, NULL);	
}

void CGLView::Refresh()
{
	m_ptrWnd->Invalidate(FALSE);
}

void CGLView::InitGL()
{
	int nPixelFormat;					// Pixel format index
	m_hDC = m_ptrWnd->GetDC()->m_hDC;			// Get the Device context

	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),	// Size of this structure
		1,								// Version of this structure	
		PFD_DRAW_TO_WINDOW |			// Draw to Window (not to bitmap)
		PFD_SUPPORT_OPENGL |			// Support OpenGL calls in window
		PFD_DOUBLEBUFFER,				// Double buffered mode
		PFD_TYPE_RGBA,					// RGBA Color mode
		24,								// Want 24bit color 
		0, 0, 0, 0, 0, 0,					// Not used to select mode
		0, 0,							// Not used to select mode
		0, 0, 0, 0, 0,						// Not used to select mode
		32,								// Size of depth buffer
		0,								// Not used to select mode
		0,								// Not used to select mode
		PFD_MAIN_PLANE,					// Draw in main plane
		0,								// Not used to select mode
		0, 0, 0
	};						// Not used to select mode

	pfd.cStencilBits = 8;	// 模板缓冲区
	// Choose a pixel format that best matches that described in pfd
	nPixelFormat = ChoosePixelFormat(m_hDC, &pfd);

	// Set the pixel format for the device context
	VERIFY(SetPixelFormat(m_hDC, nPixelFormat, &pfd));

	// Create the rendering context
	m_hRC = wglCreateContext(m_hDC);

	// Make the rendering context current, perform initialization, then
	// deselect it
	VERIFY(wglMakeCurrent(m_hDC, m_hRC));

	wglMakeCurrent(NULL, NULL);

	// Create the palette if needed
	InitializePalette();	
}


// Initializes the CPalette object 初始化调色板
void CGLView::InitializePalette(void)
{
	PIXELFORMATDESCRIPTOR pfd;
	LOGPALETTE* pPal;
	int pixelFormat = GetPixelFormat(m_hDC);
	int paletteSize;

	DescribePixelFormat(m_hDC, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	/*
	** Determine if a palette is needed and if so what size.
	*/
	if (pfd.dwFlags & PFD_NEED_PALETTE)
	{
		paletteSize = 1 << pfd.cColorBits;
	}
	else if (pfd.iPixelType == PFD_TYPE_COLORINDEX)
	{
		paletteSize = 4096;
	}
	else
	{
		return;
	}

	pPal = (LOGPALETTE *)malloc(sizeof(LOGPALETTE) + paletteSize * sizeof(PALETTEENTRY));
	pPal->palVersion = 0x300;
	pPal->palNumEntries = paletteSize;

	if (pfd.iPixelType == PFD_TYPE_RGBA)
	{
		/*
				** Fill the logical paletee with RGB color ramps
				*/
		int redMask = (1 << pfd.cRedBits) - 1;
		int greenMask = (1 << pfd.cGreenBits) - 1;
		int blueMask = (1 << pfd.cBlueBits) - 1;
		int i;

		for (i = 0; i < paletteSize; ++i)
		{
			pPal->palPalEntry[i].peRed = (((i >> pfd.cRedShift) & redMask) * 255) / redMask;
			pPal->palPalEntry[i].peGreen = (((i >> pfd.cGreenShift) & greenMask) * 255) / greenMask;
			pPal->palPalEntry[i].peBlue = (((i >> pfd.cBlueShift) & blueMask) * 255) / blueMask;
			pPal->palPalEntry[i].peFlags = 0;
		}
	}
	else
	{
		int numRamps = NUM_COLORS;
		int rampSize = (paletteSize - 20) / numRamps;
		int extra = (paletteSize - 20) - (numRamps* rampSize);
		int i, r;

		GetSystemPaletteEntries(m_hDC, 0, paletteSize, &pPal->palPalEntry[0]);

		for (r = 0; r < numRamps; ++r)
		{
			int rampBase = r* rampSize + 10;
			PALETTEENTRY* pe = &pPal->palPalEntry[rampBase];
			int diffSize = (int)(rampSize* m_colorv[r].ratio);
			int specSize = rampSize - diffSize;

			for (i = 0; i < rampSize; ++i)
			{
				GLfloat* c0, * c1;
				GLint a;

				if (i < diffSize)
				{
					c0 = m_colorv[r].amb;
					c1 = m_colorv[r].diff;
					a = (i * 255) / (diffSize - 1);
				}
				else
				{
					c0 = m_colorv[r].diff;
					c1 = m_colorv[r].spec;
					a = ((i - diffSize) * 255) / (specSize - 1);
				}

				pe[i].peRed = (BYTE)(a * (c1[0] - c0[0]) + 255 * c0[0]);
				pe[i].peGreen = (BYTE)(a * (c1[1] - c0[1]) + 255 * c0[1]);
				pe[i].peBlue = (BYTE)(a * (c1[2] - c0[2]) + 255 * c0[2]);
				pe[i].peFlags = PC_NOCOLLAPSE;
			}

			m_colorv[r].indexes[0] = rampBase;
			m_colorv[r].indexes[1] = rampBase + (diffSize - 1);
			m_colorv[r].indexes[2] = rampBase + (rampSize - 1);
		}

		for (i = 0; i < extra; ++i)
		{
			int index = numRamps* rampSize + 10 + i;
			PALETTEENTRY* pe = &pPal->palPalEntry[index];

			pe->peRed = (BYTE)0;
			pe->peGreen = (BYTE)0;
			pe->peBlue = (BYTE)0;
			pe->peFlags = PC_NOCOLLAPSE;
		}
	}

	m_hPalette2 = CreatePalette(pPal);
	free(pPal);

	if (m_hPalette2)
	{
		SelectPalette(m_hDC, m_hPalette2, FALSE);
		RealizePalette(m_hDC);
	}
}

BOOL CGLView::EnableRC(HDC m_hDC, HGLRC m_hRC, BOOL bEnable)
{
	if (bEnable)
	{
		if (!wglMakeCurrent(m_hDC, m_hRC))
		{
			CString err("CGLView::EnableRC - wglMakeCurrent failed\n");
			MessageBox(0, err, "OpenGL ERROR", MB_OK);
			return FALSE;
		}
	}
	else
		wglMakeCurrent(NULL, NULL);

	return TRUE;
}



BOOL CGLView::QueryNewPalette()
{
	int nRet;
	// If the palette was created.
	if ((HPALETTE)m_GLPalette)
	{
		// Selects the palette into the current device context
		SelectPalette(m_hDC, (HPALETTE)m_GLPalette, FALSE);

		// Map entries from the currently selected palette to
		// the system palette.  The return value is the number 
		// of palette entries modified.
		nRet = RealizePalette(m_hDC);

		// Repaint, forces remap of palette in current window
		m_ptrWnd->InvalidateRect(NULL, FALSE);
	}
	return nRet;
}

void CGLView::PaletteChanged()
{
	// Select the palette into the device context
	SelectPalette(m_hDC, (HPALETTE)m_GLPalette, FALSE);

	// Map entries to system palette
	RealizePalette(m_hDC);

	// Remap the current colors to the newly realized palette
	UpdateColors(m_hDC);
}

void CGLView::RenderScene(GLenum mode, bool bSlice)
{
	// Clear the window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (m_pContext->m_antialias)
	{
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glLineWidth(1);
		glDepthFunc(GL_LESS);
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_LINE_SMOOTH);
		glLineWidth(1);
		glDisable(GL_BLEND);
	}

	m_Background.SetModified(1);
	m_Background.Display();

	if (m_pContext->m_bSmooth)
		::glShadeModel(GL_SMOOTH);
	else
		::glShadeModel(GL_FLAT);	

	glPushMatrix();

	//Scene and View Rotations
	glRotatef(m_pContext->m_xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(m_pContext->m_yRot, 0.0f, 1.0f, 0.0f);
	glRotatef(m_pContext->m_zRot, 0.0f, 0.0f, 1.0f);

	glScalef(1.0f, 1.0f, m_pContext->m_dVertical);	

	m_pContext->Display(m_pContext->m_displayMode);
	if( m_pContext->m_selectedMode != GLNONE)
		m_pContext->DisplaySelected(m_pContext->m_selectedMode);
	
	//------------------------------------------------------------------------
	// 画坐标框
	m_myTrihedron->SetSize(m_pContext->m_dRange / 2);
	m_myTrihedron->Display();

	//------------------------------------------------------------------------
	// 画切片框
	if( m_bSliceZ ) // 横切片
	{
		if(m_pContext->m_selectedMode != GLEDIT)
			glLoadName(100);
		glBegin(GL_LINE_STRIP);
		glColor3ub(0,255,0);
		glLineWidth(3);
		glVertex3d(1.0,1.0,m_dCurrZ);
		glVertex3d(-1.0,1.0,m_dCurrZ);
		glVertex3d(-1,-1.0,m_dCurrZ);
		glVertex3d(1.0,-1.0,m_dCurrZ);
		glVertex3d(1.0,1.0,m_dCurrZ);
		glEnd();
	}

	if( m_bSliceXY)
	{
		if(m_pContext->m_selectedMode != GLEDIT)
			glLoadName(101);
		glBegin(GL_LINE_STRIP);
		glColor3ub(0,255,0);
		glLineWidth(3);
		glVertex3d(m_dCurrX1, m_dCurrY1, 1.0);
		glVertex3d(m_dCurrX2, m_dCurrY2, 1.0);
		glVertex3d(m_dCurrX2, m_dCurrY2, -1.0);
		glVertex3d(m_dCurrX1, m_dCurrY1,-1.0);
		glVertex3d(m_dCurrX1, m_dCurrY1,1.0);
		glEnd();
		
		double x = (m_dCurrX1+m_dCurrX2)/2;
		double y = (m_dCurrY1+m_dCurrY2)/2;

		glColor3ub(255,0,0);
		glPointSize(9);

		if(m_pContext->m_selectedMode != GLEDIT)
			glLoadName(102);
		glBegin(GL_POINTS);
		glVertex3d(x, y, 1.0);		
		glEnd();

		if(m_pContext->m_selectedMode != GLEDIT)
			glLoadName(103);
		glBegin(GL_POINTS);
		glVertex3d(m_dCurrX1, m_dCurrY1, 1.0);
		glEnd();

		if(m_pContext->m_selectedMode != GLEDIT)
			glLoadName(104);
		glBegin(GL_POINTS);
		glVertex3d(m_dCurrX2, m_dCurrY2, 1.0);
		glEnd();
	}

	glPopMatrix();
	
	glFlush();

	glDisable(GL_LIGHT0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void CGLView::ZoomView(CPoint dPoint, CPoint mPoint)
{
	if (!m_ZoomON)
		m_DownPoint = dPoint;

	m_ZoomON = true;

	m_pContext->SetRange(m_pContext->m_dRange + m_pContext->m_dRange * (m_DownPoint.y - mPoint.y)*0.01);

	m_DownPoint = mPoint;

	CRect rect;
	m_ptrWnd->GetClientRect(&rect);

	int w = rect.Width();
	int h = rect.Height();

	ReSize(w, h);
}

void CGLView::PanView(CPoint dPoint, CPoint mPoint)
{
	if (!m_PanON)
		m_DownPoint = dPoint;
	m_PanON = true;
	CSize pan = m_DownPoint - mPoint;
	m_DownPoint = mPoint;

	m_pContext->TransformXY(
		m_pContext->m_xTrans - 0.005 * m_pContext->m_dRange / 2 * pan.cx,
		m_pContext->m_yTrans + 0.005 * m_pContext->m_dRange / 2 * pan.cy);

	m_DownPoint = mPoint;

	CRect rect;
	m_ptrWnd->GetClientRect(&rect);

	int w = rect.Width();
	int h = rect.Height();

	ReSize(w, h);
}

void CGLView::RotateView(CPoint dPoint, CPoint mPoint)
{
	if (!m_RotON)
		m_DownPoint = dPoint;
	m_RotON = true;
	CSize rotate = m_DownPoint - mPoint;

	m_pContext->RotateXZ(
		m_pContext->m_xRot - rotate.cy / 3.0,
		m_pContext->m_zRot - rotate.cx / 3.0);
	
	m_DownPoint = mPoint;
}


void CGLView::TopView()
{
	// TODO: Add your command handler code here
	m_pContext->RotateXYZ(0,0,0);

	Refresh();
}

void CGLView::BottomView()
{
	// TODO: Add your command handler code here
	m_pContext->RotateXYZ(-180,0,0);
	Refresh();
}

void CGLView::FrontView()
{
	// TODO: Add your command handler code here
	m_pContext->RotateXYZ(-90,0,0);
	Refresh();
}

void CGLView::BackView()
{
	// TODO: Add your command handler code here
	m_pContext->RotateXYZ(-90,0,-180);
	Refresh();
}

void CGLView::LeftView()
{
	// TODO: Add your command handler code here
	m_pContext->RotateXYZ(-90,0,-90);
	Refresh();
}

void CGLView::RightView()
{
	// TODO: Add your command handler code here
	m_pContext->RotateXYZ(-90,0,90);
	Refresh();
}

void CGLView::AxonView()
{
	// TODO: Add your command handler code here
	m_pContext->RotateXYZ(-45,0,-45);
	Refresh();
}

void CGLView::FitAll()
{
	// TODO: Add your command handler code here
	if (m_pContext->IsEmpty())
		return;
	
	FitView();

	CRect rect;
	m_ptrWnd->GetClientRect(&rect);

	int w = rect.Width();
	int h = rect.Height();

	ReSize(w, h);

	Refresh();
}

void CGLView::ZoomWindow(CRect zcRect)
{
	CRect rcRect;
	m_ptrWnd->GetClientRect(&rcRect);

	int rcWidth, rcHeight, zcWidth, zcHeight;
	CPoint rcCentre, zcCentre;
	int dx, dy, dw, dh;
	GLdouble wRatio, hRatio;
	GLdouble dRange;

	GLint viewport[4];
	GLdouble mvmatrix[16], projmatrix[16];
	GLint realy;

	GLdouble cx, cy, cz;
	GLdouble mx, my, mz;

	rcWidth = rcRect.Width();
	rcHeight = rcRect.Height();

	zcWidth = zcRect.Width();
	zcHeight = zcRect.Height();

	rcCentre = rcRect.CenterPoint();
	zcCentre = zcRect.CenterPoint();

	wglMakeCurrent(m_hDC, m_hRC);
	glPushMatrix();
	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);

	realy = viewport[3] - (GLint)zcCentre.y - 1;
	gluUnProject((GLdouble)zcCentre.x, (GLdouble)realy, 0.5, mvmatrix, projmatrix, viewport, &mx, &my, &mz);

	realy = viewport[3] - (GLint)rcCentre.y - 1;
	gluUnProject((GLdouble)rcCentre.x, (GLdouble)realy, 0.5, mvmatrix, projmatrix, viewport, &cx, &cy, &cz);

	glPopMatrix();

	dx = cx - mx;
	dy = cy - my;

	dw = rcWidth - zcWidth;
	dh = rcHeight - zcHeight;

	wRatio = (GLdouble)zcWidth / rcWidth;
	hRatio = (GLdouble)zcHeight / rcHeight;

	dRange = (wRatio < hRatio) ? hRatio : wRatio;
	
	m_pContext->SetRange(m_pContext->m_dRange * dRange / (double)0.8);
	m_pContext->TransformXY(m_pContext->m_xTrans + dx, m_pContext->m_yTrans - dy);

	wglMakeCurrent(NULL, NULL);

	ReSize(rcWidth, rcHeight);

	Refresh();
}

void CGLView::ResetView()
{
	// TODO: Add your command handler code here

	m_pContext->RotateXYZ(-75, 0, -45);
	m_pContext->TransformXYZ(0, 0, 0);
	m_pContext->SetRange(2);

	CRect rect;
	m_ptrWnd->GetClientRect(&rect);

	int w = rect.Width();
	int h = rect.Height();

	ReSize(w, h);

	Refresh();
}

void CGLView::GetBackgroundColor(GLfloat& r, GLfloat& g, GLfloat& b)
{
	m_pContext->GetBackgroundColor(r, g, b);
}

void CGLView::SetBackgroundColor(GLfloat r, GLfloat g, GLfloat b)
{
	m_pContext->SetBackgroundColor(r, g, b);

	m_Background.SetBackgroundColor(r, g, b);
	m_Background.SetModified(1);
	
	m_ptrWnd->InvalidateRect(NULL, FALSE);
}

void CGLView::SetSmoothGround(bool bSmooth)
{
	m_pContext->SetSmoothGround(bSmooth);
	m_Background.SetSmoothGround(bSmooth);
	m_Background.SetModified(1);

	m_ptrWnd->InvalidateRect(NULL, FALSE);
}

bool CGLView::GetSmoothGround()
{
	return m_pContext->GetSmoothGround();
}

void CGLView::ShowLegend(bool bLegend)
{
	m_pContext->ShowLegend(bLegend);
	m_myTrihedron->ShowLegend(bLegend);
	m_myTrihedron->SetModified(1);

	m_ptrWnd->InvalidateRect(NULL, FALSE);
}

bool CGLView::IsShowLegend()
{
	return m_pContext->IsShowLegend();
}

void CGLView::ShowAxis(bool bShow)
{
	m_pContext->ShowAxis(bShow);
	m_myTrihedron->ShowAxis(bShow);
	m_myTrihedron->SetModified(1);

	m_ptrWnd->InvalidateRect(NULL, FALSE);
}

bool CGLView::IsShowAxis()
{
	return m_pContext->IsShowAxis();
}

void CGLView::ShowGird(bool bShow)
{
	m_pContext->ShowGird(bShow);
	m_myTrihedron->ShowGird(bShow);
	m_myTrihedron->SetModified(1);

	m_ptrWnd->InvalidateRect(NULL, FALSE);
}
bool CGLView::IsShowGird()
{
	return m_pContext->IsShowGird();
}

void CGLView::SetDisplayMode(const GLDisplayMode& aMode)
{
	m_pContext->SetDisplayMode(aMode);	
	m_ptrWnd->InvalidateRect(NULL, FALSE);
}

void CGLView::SetSmooth(bool bSmooth)
{
	m_pContext->SetSmooth(bSmooth);
	m_ptrWnd->InvalidateRect(NULL, FALSE);
}

void CGLView::SetAntiAliasing(const bool& mode)
{
	m_pContext->SetAntiAliasing(mode);
	m_ptrWnd->InvalidateRect(NULL, FALSE);
}

void CGLView::PointToScreen(const CVertex3D& P, int& x, int& y)
{
	CVertex3D p = P;
	GLint viewport[4];
	GLdouble mvmatrix[16], projmatrix[16];
	GLdouble wx, wy, wz;
	::wglMakeCurrent(m_hDC, m_hRC);
	glPushMatrix();
	glRotatef(m_pContext->m_xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(m_pContext->m_yRot, 0.0f, 1.0f, 0.0f);
	glRotatef(m_pContext->m_zRot, 0.0f, 0.0f, 1.0f);
	
	glScalef(1.0f, 1.0f, m_pContext->m_dVertical);
	
	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);
	gluProject((GLdouble)p.GetX(), p.GetY(), p.GetZ(), mvmatrix, projmatrix, viewport, &wx, &wy, &wz);
	glPopMatrix();
	::wglMakeCurrent(NULL, NULL);

	x = (int)wx;
	y = (int)wy;
}

double CGLView::GetSliceHeight(const int& width, bool bVert)
{	
	if( bVert ) // 纵切时
	{
		CVertex3D C1;
		CVertex3D C2;
		ScreenToPoint(1, 0, C1);
		ScreenToPoint(1+width, 0, C2);
		return fabs(C2.x-C1.x);
	}
	else
	{
		CVertex3D C1;
		CVertex3D C2;
		ScreenToPoint(1, 0, C1);
		ScreenToPoint(1+width, 0, C2);
		return fabs(C2.x-C1.x)/m_pContext->GetVertical();
	}
}

void CGLView::ScreenToPoint(const int& x, const int& y, CVertex3D& P)
{
	CPoint point(x, y);
	GLint viewport[4];
	GLdouble mvmatrix[16], projmatrix[16];
	GLint realy;
	GLdouble wx, wy, wz;
	::wglMakeCurrent(m_hDC, m_hRC);
	glPushMatrix();
	
	glRotatef(m_pContext->m_xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(m_pContext->m_yRot, 0.0f, 1.0f, 0.0f);
	glRotatef(m_pContext->m_zRot, 0.0f, 0.0f, 1.0f);
	
	glScalef(1.0f, 1.0f, m_pContext->m_dVertical);
	
	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);
	realy = viewport[3] - (GLint)point.y - 1;

// 	GLfloat winZ;
// 	glReadPixels(point.x, realy, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ); 

	gluUnProject((GLdouble)point.x, (GLdouble)realy, (GLdouble)0.5, mvmatrix, projmatrix, viewport, &wx, &wy, &wz);
	
	glPopMatrix();
	::wglMakeCurrent(NULL, NULL);

	P.SetParam(wx, wy, wz);
}

void CGLView::FitView()
{
	GLint viewport[4];
	GLdouble mvmatrix[16], projmatrix[16];
	CRect volRect;
	GLdouble cx, cy, cz;
	GLdouble mx, my, mz;
	GLdouble lx, ly, lz, sx, sy, sz;

	sx = - 1.0;
	lx = 1.0;
	sy = - 1.0;
	ly = 1.0;

	sz = - 1.0;
	lz = 1.0;
	
	wglMakeCurrent(m_hDC, m_hRC);
	glPushMatrix();

	glRotatef(m_pContext->m_xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(m_pContext->m_yRot, 0.0f, 1.0f, 0.0f);
	glRotatef(m_pContext->m_zRot, 0.0f, 0.0f, 1.0f);

	glScalef(1.0f, 1.0f, m_pContext->m_dVertical);

	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);

	gluProject(lx, ly, lz, mvmatrix, projmatrix, viewport, &mx, &my, &mz);

	gluProject(sx, sy, sz, mvmatrix, projmatrix, viewport, &cx, &cy, &cz);

	volRect.SetRect(cx, cy, mx, my);
	volRect.NormalizeRect();

	glPopMatrix();
	::wglMakeCurrent(NULL, NULL);

	Fit3d(volRect);	
}

void CGLView::Fit3d(CRect rect)
{
	CRect zcRect = rect;
	CRect rcRect;
	m_ptrWnd->GetClientRect(&rcRect);

	int rcWidth, rcHeight, zcWidth, zcHeight;
	CPoint rcCentre, zcCentre;
	int dx, dy, dw, dh;
	GLdouble wRatio, hRatio;
	GLdouble dRange;

	GLint viewport[4];
	GLdouble mvmatrix[16], projmatrix[16];
	GLint realy;

	GLdouble cx, cy, cz;
	GLdouble mx, my, mz;

	rcWidth = rcRect.Width();
	rcHeight = rcRect.Height();

	zcWidth = zcRect.Width();
	zcHeight = zcRect.Height();

	rcCentre = rcRect.CenterPoint();
	zcCentre = zcRect.CenterPoint();

	::wglMakeCurrent(m_hDC, m_hRC);

	glPushMatrix();

	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);

	realy = viewport[3] - (GLint)zcCentre.y - 1;
	gluUnProject((GLdouble)zcCentre.x, (GLdouble)realy, 0.5, mvmatrix, projmatrix, viewport, &mx, &my, &mz);

	realy = viewport[3] - (GLint)rcCentre.y - 1;
	gluUnProject((GLdouble)rcCentre.x, (GLdouble)realy, 0.5, mvmatrix, projmatrix, viewport, &cx, &cy, &cz);

	glPopMatrix();

	::wglMakeCurrent(NULL, NULL);

	dx = cx - mx;
	dy = cy - my;

	dw = rcWidth - zcWidth;
	dh = rcHeight - zcHeight;

	wRatio = (GLdouble)zcWidth / rcWidth;
	hRatio = (GLdouble)zcHeight / rcHeight;

	dRange = (wRatio < hRatio) ? hRatio : wRatio;

	m_pContext->SetRange(m_pContext->m_dRange * dRange / (double)0.8);
	m_pContext->TransformXY(m_pContext->m_xTrans + dx, m_pContext->m_yTrans - dy);
}

void CGLView::Select(const int& x, const int& y)
{
	m_pContext->Select(this, x, y);
}

void CGLView::MultiSelect(const int& x, const int& y)
{
	m_pContext->MultiSelect(this, x, y);
}

void CGLView::SweepSelect(const CRect& swRect)
{
	m_pContext->SweepSelect(this, swRect);
}

int CGLView::HitSliceXY(const int& xPos, const int& yPos, const int& sensitivity)
{
	int id = -1;

	::wglMakeCurrent(m_hDC, m_hRC);

	// Space for selection buffer
	GLuint selectBuff[512]={0};

	// Hit counter and viewport storeage
	GLint hits, viewport[4];

	// Setup selection buffer
	glSelectBuffer(512, selectBuff);

	// Get the viewport
	glGetIntegerv(GL_VIEWPORT, viewport);

	// Switch to projection and save the matrix
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	// Change render mode
	glRenderMode(GL_SELECT);

	// Establish new clipping volume to be unit cube around
	// mouse cursor point (xPos, yPos) and extending n pixels
	// in the vertical and horzontal direction
	int n = sensitivity;
	glLoadIdentity();
	gluPickMatrix(xPos, viewport[3] - yPos, n, n, viewport);

	CRect rect;
	m_ptrWnd->GetClientRect(&rect);

	int w = rect.Width();
	int h = rect.Height();

	// Ortho Viewing
	if (w <= h)
		glOrtho(-m_pContext->m_dRange - m_pContext->m_xTrans, 
		m_pContext->m_dRange - m_pContext->m_xTrans,
		-(m_pContext->m_dRange * h / w) - m_pContext->m_yTrans,
		(m_pContext->m_dRange * h / w) - m_pContext->m_yTrans,
		-(m_pContext->m_dRange * 5000.0f) - m_pContext->m_zTrans,
		(m_pContext->m_dRange * 5000.0f) - m_pContext->m_zTrans);
	else
		glOrtho(-(m_pContext->m_dRange * w / h) - m_pContext->m_xTrans,
		(m_pContext->m_dRange * w / h) - m_pContext->m_xTrans,
		-m_pContext->m_dRange - m_pContext->m_yTrans,
		m_pContext->m_dRange - m_pContext->m_yTrans,
		-m_pContext->m_dRange * 5000.0f - m_pContext->m_zTrans,
		m_pContext->m_dRange * 5000.0f - m_pContext->m_zTrans);

	// Draw the scene
	RenderScene(GL_SELECT, true);

	// Collect the hits
	hits = glRenderMode(GL_RENDER);

	// Restore the projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	// Go back to modelview for normal rendering
	glMatrixMode(GL_MODELVIEW);

	// If a single hit occured, display the info.
	if (hits)
	{
		int		  name_background = -1;
		const int pick_maxz = 0xffffffff;
		GLint     hit = name_background;
		GLuint    minz = 0;
		GLint     i,j;
		GLint	  nnames;

		minz = pick_maxz;
		for (i = j = 0; j < hits; j++) 
		{
			nnames = selectBuff[i];
			i++;
			//if (selectBuff[i] < minz) 
			{
				minz = selectBuff[i];
				hit = (GLint)selectBuff[i + 1 + nnames];
				TRACE(_T("asdasd\n"));
// 				if (hit>0 && hit<200)
// 				{
// 					id = hit;
// 					return id;
// 				}
				TRACE(_T("%d\n"),hit);
			} 
			i++;
			i += nnames + 1;

			TRACE(_T("%d %d\n"),nnames,hit);
		}

		if (hit>0 && hit<200)
		{
			id = hit;
		}
	}

	::wglMakeCurrent(NULL, NULL);

	return id;
}


int CGLView::HitPoint(const int& xPos, const int& yPos, const int& sensitivity)
{
	int id = -1;

	::wglMakeCurrent(m_hDC, m_hRC);

	// Space for selection buffer
	GLuint selectBuff[3096]={0};

	// Hit counter and viewport storeage
	GLint hits, viewport[4];

	// Setup selection buffer
	glSelectBuffer(3096, selectBuff);

	// Get the viewport
	glGetIntegerv(GL_VIEWPORT, viewport);

	// Switch to projection and save the matrix
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	// Change render mode
	glRenderMode(GL_SELECT);

	// Establish new clipping volume to be unit cube around
	// mouse cursor point (xPos, yPos) and extending n pixels
	// in the vertical and horzontal direction
	int n = sensitivity;
	glLoadIdentity();
	gluPickMatrix(xPos, viewport[3] - yPos, n, n, viewport);

	CRect rect;
	m_ptrWnd->GetClientRect(&rect);

	int w = rect.Width();
	int h = rect.Height();

	// Ortho Viewing
	if (w <= h)
		glOrtho(-m_pContext->m_dRange - m_pContext->m_xTrans, 
		m_pContext->m_dRange - m_pContext->m_xTrans,
		-(m_pContext->m_dRange * h / w) - m_pContext->m_yTrans,
		(m_pContext->m_dRange * h / w) - m_pContext->m_yTrans,
		-(m_pContext->m_dRange * 5000.0f) - m_pContext->m_zTrans,
		(m_pContext->m_dRange * 5000.0f) - m_pContext->m_zTrans);
	else
		glOrtho(-(m_pContext->m_dRange * w / h) - m_pContext->m_xTrans,
		(m_pContext->m_dRange * w / h) - m_pContext->m_xTrans,
		-m_pContext->m_dRange - m_pContext->m_yTrans,
		m_pContext->m_dRange - m_pContext->m_yTrans,
		-m_pContext->m_dRange * 5000.0f - m_pContext->m_zTrans,
		m_pContext->m_dRange * 5000.0f - m_pContext->m_zTrans);

	// Draw the scene
	RenderScene(GL_SELECT, true);

	// Collect the hits
	hits = glRenderMode(GL_RENDER);

	// Restore the projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	// Go back to modelview for normal rendering
	glMatrixMode(GL_MODELVIEW);

	// If a single hit occured, display the info.
	if (hits>=3)
	{
		int		  name_background = -1;
		const int pick_maxz = 0xffffffff;
		GLint     hit = name_background;
		GLuint    minz = 0;
		GLint     i,j;
		//GLint	  nnames;

		GLuint names, *ptr;   
		TRACE("hits = %d\n", hits);   
		ptr = (GLuint *) selectBuff;   
		for (i = 0; i < hits; i++)
		{     
			names = *ptr;   
			TRACE(" number of names for hit = %d\n", names); ptr++;   
			TRACE("  z1 is %g;", (float) *ptr/0x7fffffff); ptr++;   
			TRACE(" z2 is %g\n", (float) *ptr/0x7fffffff); ptr++;   
			TRACE("   the name is ");   
			for (j = 0; j < names; j++)
			{     
				id = (*ptr);
				TRACE("%d ", *ptr);
				ptr++;   
			}  
			if( names < 1 )
				TRACE("no\n");  
			TRACE("\n");   

			if( id%4 && id>0)
			{
				break;
			}
		}  

		/*minz = pick_maxz;
		for (i = j = 0; j < hits; j++) 
		{
			nnames = selectBuff[i];
			i++;
			if (selectBuff[i] < minz) 
			{
				minz = selectBuff[i];
				hit = (GLint)selectBuff[i + 1 + nnames];				
			} 
			i++;
			i += nnames + 1;
			
			if (hit>-1)
				break;

		}

		if (hit>-1)
		{
			id = hit;
		}
		*/
	}

	::wglMakeCurrent(NULL, NULL);

	return id;
}


CGLObject* CGLView::ProcessSelection(const int& xPos, const int& yPos, const int& sensitivity)
{	
	CGLObject* pObj = NULL;

	::wglMakeCurrent(m_hDC, m_hRC);

	// Space for selection buffer
	GLuint selectBuff[512]={0};

	// Hit counter and viewport storeage
	GLint hits, viewport[4];

	// Setup selection buffer
	glSelectBuffer(512, selectBuff);

	// Get the viewport
	glGetIntegerv(GL_VIEWPORT, viewport);

	// Switch to projection and save the matrix
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	// Change render mode
	glRenderMode(GL_SELECT);

	// Establish new clipping volume to be unit cube around
	// mouse cursor point (xPos, yPos) and extending n pixels
	// in the vertical and horzontal direction
	int n = sensitivity;
	glLoadIdentity();
	gluPickMatrix(xPos, viewport[3] - yPos, n, n, viewport);

	CRect rect;
	m_ptrWnd->GetClientRect(&rect);

	int w = rect.Width();
	int h = rect.Height();

	// Ortho Viewing
	if (w <= h)
		glOrtho(-m_pContext->m_dRange - m_pContext->m_xTrans, 
		m_pContext->m_dRange - m_pContext->m_xTrans,
		-(m_pContext->m_dRange * h / w) - m_pContext->m_yTrans,
		(m_pContext->m_dRange * h / w) - m_pContext->m_yTrans,
		-(m_pContext->m_dRange * 5000.0f) - m_pContext->m_zTrans,
		(m_pContext->m_dRange * 5000.0f) - m_pContext->m_zTrans);
	else
		glOrtho(-(m_pContext->m_dRange * w / h) - m_pContext->m_xTrans,
		(m_pContext->m_dRange * w / h) - m_pContext->m_xTrans,
		-m_pContext->m_dRange - m_pContext->m_yTrans,
		m_pContext->m_dRange - m_pContext->m_yTrans,
		-m_pContext->m_dRange * 5000.0f - m_pContext->m_zTrans,
		m_pContext->m_dRange * 5000.0f - m_pContext->m_zTrans);

	// Draw the scene
	RenderScene(GL_SELECT);

	// Collect the hits
	hits = glRenderMode(GL_RENDER);

	// Restore the projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	// Go back to modelview for normal rendering
	glMatrixMode(GL_MODELVIEW);

	// If a single hit occured, display the info.
	if (hits)
	{
		int		  name_background = -1;
		const int pick_maxz = 0xffffffff;
		GLint     hit = name_background;
		GLuint    minz;
		GLint     i,j;
		GLint	  nnames;
		
		minz = pick_maxz;
		for (i = j = 0; j < hits; j++) 
		{
			nnames = selectBuff[i];
			i++;
			if (selectBuff[i] < minz) 
			{
				minz = selectBuff[i + nnames];
				if(minz>0)
				{
					hit = (GLint)selectBuff[i + 1 + nnames];
					if (hit>200)
					{
						pObj = reinterpret_cast<CGLObject*>(hit);
						if( pObj->isShow())
							break;
					}
				}
			} 
			i++;
			i += nnames + 1;
		}

		if (hit>200)
		{
			pObj = reinterpret_cast<CGLObject*>(hit);

			if( !pObj->isShow())
				pObj = NULL;
		}
	}

	::wglMakeCurrent(NULL, NULL);

	return pObj;
}

int CGLView::ProcessSweepSelection(const CRect& swRect, unsigned long int* id)
{
	CGLObject* pObj = NULL;

	GLint i;
	GLuint j;
	::wglMakeCurrent(m_hDC, m_hRC);

	// Space for selection buffer
	GLuint selectBuff[512];

	// Hit counter and viewport storeage
	GLint hits = 0, viewport[4];

	// Setup selection buffer
	glSelectBuffer(512, selectBuff);

	// Get the viewport
	glGetIntegerv(GL_VIEWPORT, viewport);

	// Switch to projection and save the matrix
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	// Change render mode
	glRenderMode(GL_SELECT);

	// Establish new clipping volume to be cube around
	// center point of sweeping rectangle and extending pixels
	// falling within the height and width of the sweep rectangle
	// in the vertical and horzontal direction
	glLoadIdentity();
	gluPickMatrix(swRect.CenterPoint().x, viewport[3] - swRect.CenterPoint().y, swRect.Width(), swRect.Height(), viewport);

	CRect rect;
	m_ptrWnd->GetClientRect(&rect);

	int w = rect.Width();
	int h = rect.Height();

	// Ortho Viewing
	if (w <= h)
		glOrtho(-m_pContext->m_dRange - m_pContext->m_xTrans, m_pContext->m_dRange - m_pContext->m_xTrans, 
		-(m_pContext->m_dRange * h / w) - m_pContext->m_yTrans, 
		(m_pContext->m_dRange * h / w) - m_pContext->m_yTrans, 
		-(m_pContext->m_dRange * 5000.0f) - m_pContext->m_zTrans, 
		(m_pContext->m_dRange * 5000.0f) - m_pContext->m_zTrans);
	else
		glOrtho(-(m_pContext->m_dRange * w / h) - m_pContext->m_xTrans, (m_pContext->m_dRange * w / h) - m_pContext->m_xTrans, 
		-m_pContext->m_dRange - m_pContext->m_yTrans, 
		m_pContext->m_dRange - m_pContext->m_yTrans, 
		-m_pContext->m_dRange * 5000.0f - m_pContext->m_yTrans, 
		m_pContext->m_dRange * 5000.0f - m_pContext->m_yTrans);

	// Draw the scene
	RenderScene(GL_SELECT);

	// Collect the hits
	hits = glRenderMode(GL_RENDER);

	// Restore the projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	// Go back to modelview for normal rendering
	glMatrixMode(GL_MODELVIEW);

	// If hits occured, display the info.
	if (hits)
	{
		GLuint* ptr, names;
		ptr = (GLuint *)selectBuff;
		for (i = 0; i < hits; i++)
		{
			names = *ptr;
			ptr++; ptr++; ptr++;
			for (j = 0; j < names; j++)
			{
				id[i] = *ptr;
				ptr++;
			}
		}
	}

	::wglMakeCurrent(NULL, NULL);

	return hits;
}

void CGLView::ExportFile(LPCTSTR lpszFileName)
{
	// Alloc
	const int size = (int)6e6; // dirty code
	GLfloat *pFeedbackBuffer = new GLfloat[size];
	ASSERT(pFeedbackBuffer);
	::wglMakeCurrent(m_hDC, m_hRC);
	glPushMatrix();
	glRotatef(m_pContext->m_xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(m_pContext->m_yRot, 0.0f, 1.0f, 0.0f);
	glRotatef(m_pContext->m_zRot, 0.0f, 0.0f, 1.0f);

	glScalef(1.0f, 1.0f, m_pContext->m_dVertical);


	// Set feedback mode
	::glFeedbackBuffer(size,GL_3D_COLOR,pFeedbackBuffer);
	::glRenderMode(GL_FEEDBACK);

	// Call to the rendering function
	RenderScene(GL_RENDER);

	// Swap our scene to the front
	::SwapBuffers(m_hDC);

	int NbValues = glRenderMode(GL_RENDER);

	// The stuff here
	CPsRenderer PsRenderer;
	PsRenderer.Run(lpszFileName,pFeedbackBuffer,NbValues,TRUE);

	// Cleanup
	delete [] pFeedbackBuffer;	
	glPopMatrix();
	::wglMakeCurrent(NULL, NULL);
}