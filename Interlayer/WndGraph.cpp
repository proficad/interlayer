// WndGraph.cpp : 实现文件
//

#include "stdafx.h"
#include "Interlayer.h"
#include "WndGraph.h"
#include "./Vertical_Model/f2doff.h"
#include "./Vertical_Model/coords.h"
#include "FormGenerateLayer.h"
#include "GenerateLayerDoc.h"
#include "./GeoMap/GeoPolyObj.h"
const double FRulerWidth = 1 / 6.0;    

const double FFontSize = FRulerWidth / 1.13;
const double FBigMarkSize = FRulerWidth / 4;
const double FSmallMarkSize = FRulerWidth / 4;

extern double ptolinesegdist(CPoint2D p, LINESEG l, CPoint2D &np, int &ret);
// CWndGraph

IMPLEMENT_DYNAMIC(CWndGraph, CWnd)

CWndGraph::CWndGraph()
: offscreen(NULL)
{
	RegisterWindowClass();
	offscreen = NULL;
	CurrentCoordsX = NULL;
	CurrentCoordsY = NULL;
	m_dMinX = DBL_MAX;
	m_dMaxX = -DBL_MAX;
	m_dMinY = DBL_MAX;
	m_dMaxY = -DBL_MAX;
	m_dMinZ = DBL_MAX;
	m_dMaxZ = -DBL_MAX;

	m_dDepthScale = 0.5;
	m_iCurVertModel = -1;
}

CWndGraph::~CWndGraph()
{
	if( offscreen)
		delete offscreen;
	if( CurrentCoordsX)
		delete CurrentCoordsX;
	if( CurrentCoordsY)
		delete CurrentCoordsY;
}

CFormGenerateLayer *CWndGraph::GetParentView()
{
	return (CFormGenerateLayer*)GetParent();
}

void CWndGraph::AddVertModel(int idx)
{
	Init(idx);
}

void CWndGraph::Init(int idx)
{
	CFormGenerateLayer *pView = GetParentView();
	if( pView == NULL )
		return;
	CGenerateLayerDoc *pDoc = pView->GetDocument();
	if( pDoc == NULL)
		return;

	switch(m_eType)
	{
	case VERT:
		{
			if( idx > -1 )
			{
				int nSize = pDoc->m_layerModel.m_vertModel.size();

				if( idx >= nSize )
				{
					for (int i = nSize; i<=idx; i++)
					{
						std::vector<CPoint2D> points;
						pDoc->m_layerModel.m_vertModel.push_back(points);
						pDoc->m_layerModel.m_dAngles.push_back(0.0);
						pDoc->m_layerModel.m_vertTitle.Add(_T(""));
						pDoc->m_layerModel.m_vertColor.push_back(0);
					}
				}

				pDoc->m_layerModel.m_vertModel[idx].clear();

				pDoc->m_layerModel.m_dAngles[idx] = 0.0;
				pDoc->m_layerModel.m_vertTitle[idx] = _T("");
				pDoc->m_layerModel.m_vertColor[idx] = 0;

				m_iCurVertModel = -1;
			}
		}
		break;
	case HORZ:
		{
			pDoc->m_layerModel.m_layers.clear();
			pDoc->m_layerModel.m_singleLayers.clear();
			pDoc->m_layerModel.m_normals[0].clear();
			pDoc->m_layerModel.m_normals[1].clear();
		}
		break;
	case TOP_PLANE:
		{
			pDoc->m_layerModel.m_topPlane.clear();
		}
		break;
	case BOTTOM_PLANE:
		{
			pDoc->m_layerModel.m_bottomPlane.clear();
		}
		break;
	}
	
	InitCoords(DBL_MAX,-DBL_MAX,DBL_MAX,-DBL_MAX);
	UpdateGraphWindow(NULL);
}

void CWndGraph::SetMinX(double x)
{
	m_dMinX = x;
}

void CWndGraph::SetMaxX(double x)
{
	m_dMaxX = x;
}

void CWndGraph::SetMinY(double y)
{
	m_dMinY = y;
}

void CWndGraph::SetMaxY(double y)
{
	m_dMaxY = y;
}

void CWndGraph::SetMinZ(double z)
{
	m_dMinZ = z;
}

void CWndGraph::SetMaxZ(double z)
{
	m_dMaxZ = z;
}

void CWndGraph::SetWorldCoords(double _x1, double _x2, double _y1, double _y2, BOOL bRedraw)
{
	InitCoords(_x1, _x2, _y1, _y2);		
	if (bRedraw)
		UpdateGraphWindow(NULL);
}


double CWndGraph::GetMinX()
{
	return m_dMinX;
}

double CWndGraph::GetMaxX()
{
	return m_dMaxX;
}

double CWndGraph::GetMinY()
{
	return m_dMinY;
}

double CWndGraph::GetMaxY()
{
	return m_dMaxY;
}

double CWndGraph::GetMinZ()
{
	return m_dMinZ;
}

double CWndGraph::GetMaxZ()
{
	return m_dMaxZ;
}

void CWndGraph::GetWorldCoords(double* x1, double* x2, double* y1, double* y2)
{
	*x1 = (CurrentCoordsX == NULL ?  0 : CurrentCoordsX->v1());
	*x2 = (CurrentCoordsX == NULL ? 10 : CurrentCoordsX->v2());
	*y1 = (CurrentCoordsY == NULL ?  0 : CurrentCoordsY->v1());
	*y2 = (CurrentCoordsY == NULL ? 10 : CurrentCoordsY->v2());
}

int CWndGraph::GetDatCount() 
{
	CFormGenerateLayer *pView = GetParentView();
	if( pView )
	{
		CGenerateLayerDoc *pDoc = pView->GetDocument();
		if( pDoc )
		{
			switch (m_eType)
			{
			case HORZ:			// 夹层投影
				{
					int num = pDoc->m_layerModel.m_layers.size();
					return num;
				}
				break;
			case VERT:
				{
					int num = pDoc->m_layerModel.m_vertModel.size();
					return num;
				}
				break;
			case TOP_PLANE:
				{
					int num = pDoc->m_layerModel.m_topPlane.size();
					return num;
				}
				break;
			case BOTTOM_PLANE:
				{
					int num = pDoc->m_layerModel.m_bottomPlane.size();
					return num;
				}
				break;
			}
			return 0;
		}
		else
			return 0;
	}
	else
		return 0;
}

BEGIN_MESSAGE_MAP(CWndGraph, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

BOOL CWndGraph::RegisterWindowClass()
{
	WNDCLASS wndcls;
	HINSTANCE hInst = AfxGetInstanceHandle();

	if (!(::GetClassInfo(hInst, _T("WNDGraph"), &wndcls)))
	{
		// otherwise we need to register a new class
		wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
		wndcls.lpfnWndProc      = ::DefWindowProc;
		wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
		wndcls.hInstance        = hInst;
		wndcls.hIcon            = NULL;
		wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		wndcls.hbrBackground    = (HBRUSH) (COLOR_3DFACE + 1);
		wndcls.lpszMenuName     = NULL;
		wndcls.lpszClassName    = GRIDCTRL_CLASSNAME;

		if (!AfxRegisterClass(&wndcls))
		{
			AfxThrowResourceException();
			return FALSE;
		}
	}

	return TRUE;
}


// CWndGraph 消息处理程序


BOOL CWndGraph::Create(const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwStyle)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (!CWnd::Create(NULL, _T("WNDGraph"), dwStyle, rect, pParentWnd, nID))
		return FALSE;
	return TRUE;
}

void CWndGraph::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类

	CWnd::PreSubclassWindow();

	Initialise();
}

BOOL CWndGraph::Initialise()
{
	CRect rect;
	GetWindowRect(rect);
	CWnd* pParent = GetParent();
	if (pParent != NULL)
		pParent->ScreenToClient(rect);
	rect.InflateRect(1,1);	MoveWindow(rect);
	rect.DeflateRect(1,1);  MoveWindow(rect);

	return TRUE;
}

void CWndGraph::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CWnd::OnPaint()

	PAINTSTRUCT* ps = &dc.m_ps;
	CRect rect(ps->rcPaint);

	DoRedraw(&dc, rect);
}

BOOL CWndGraph::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return FALSE;
}

void CWndGraph::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

// 	if( m_eType == VERT )
// 	{	
// 		if( cx>cy)
// 			SetWorldCoords(-0.1-(double)(cx-cy)/(double)cx,1.1+(double)(cx-cy)/(double)cx,-0.1,1.1,TRUE);
// 		else
// 			SetWorldCoords(-0.1,1.1,-0.1-(double)(cy-cx)/(double)cy,1.1+(double)(cy-cx)/(double)cy,TRUE);
// 	}
// 	else
		if( m_eType == HORZ )
	{
		double dWidth = max(GetMaxX()-GetMinX(), GetMaxY()-GetMinY());
		
		double x1 = (GetMinX() + GetMaxX())/2.0 - dWidth/2.0;
		double x2 = x1 + dWidth;
		double y1 = (GetMinY() + GetMaxY())/2.0 - dWidth/2.0;
		double y2 = y1 + dWidth;

		if( cx>cy)
			SetWorldCoords(x1-(double)(cx-cy)/(double)cx/2.0*dWidth,x2+(double)(cx-cy)/(double)cx/2.0*dWidth,y1,y2,TRUE);
		else
			SetWorldCoords(x1,x2,y1-(double)(cy-cx)/(double)cy/2.0*dWidth,y2+(double)(cy-cx)/(double)cy/2.0*dWidth,TRUE);
	}
	else
		InitCoords(GetMinX(), GetMaxX(), GetMinY(), GetMaxY());
	

	// TODO: 在此处添加消息处理程序代码
	if (offscreen)
	{
		// update offscreen 
		CRect r;	
		GetClientRect(&r);
		CSize s = r.Size();
		offscreen->ResizeDib(s.cx, s.cy);
	}
}

void CWndGraph::InitCoords(double x1, double x2, double y1, double y2)
{
	if (CurrentCoordsX != NULL)
	{
		delete CurrentCoordsX;
		CurrentCoordsX = NULL;
	}
	if (CurrentCoordsY != NULL)
	{
		delete CurrentCoordsY;
		CurrentCoordsY = NULL;
	}

	CRect r;
	GetClientRect(&r);
	CurrentCoordsX = new CCoordinates(r.left, r.right, x1, x2);
	CurrentCoordsY = new CCoordinates(r.bottom, r.top, y1, y2);
}

int CWndGraph::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	CRect r;
	GetClientRect(&r);
	offscreen = new FOffScreen(r.Width(), r.Height());

	return 0;
}

void CWndGraph::DoRedraw(CDC* dc, LPCRECT r)
{
	if (!offscreen)
	{
		return;
	}

	CRect rect;
	if (r == NULL)
	{
		GetClientRect(&rect);
	}
	else
	{
		rect.CopyRect(r);
	}

	if (rect.IsRectEmpty())
		return;

	if (!offscreen->IsDIBReady())
	{
		UpdateGraphWindow(NULL);
	}

	offscreen->DrawDib(dc->m_hDC, &rect);
}

void CWndGraph::UpdateGraphWindow(LPCRECT rect)
{
	if (!offscreen)
	{
		return;
	}

	CRect r;
	if (rect == NULL)
		GetClientRect(&r);
	else
		r.CopyRect(rect);

	//do paint here
	CDC* dc = offscreen->GetDibCDC();
	if (dc != NULL)
	{
		CRect rect_to_draw;
		GetClientRect(rect_to_draw);
		DrawToDC(dc, rect_to_draw);
		offscreen->SetReady(TRUE);
	}

	InvalidateRect(r,FALSE);
}

void CWndGraph::DrawToDC(CDC* dc_to_draw, CRect& rect_to_draw)
{
	int saved_dc = dc_to_draw->SaveDC();
	
	//fill background
	CBrush bkbrush(RGB(230, 230, 230));
	CBrush* oldbrush = (CBrush*)dc_to_draw->SelectObject(&bkbrush);

	CRect bkrect(rect_to_draw);
	bkrect.bottom += 1; bkrect.right += 1; bkrect.left -= 1; bkrect.top -= 1;
	dc_to_draw->Rectangle(bkrect);
	dc_to_draw->SelectObject(oldbrush);
	
	if( m_eType == VERT )// 纵向模型
	{
		CCoordinates coords_x(rect_to_draw.left, rect_to_draw.right, CurrentCoordsX->v1(), CurrentCoordsX->v2());
		CCoordinates coords_y(rect_to_draw.bottom, rect_to_draw.top, CurrentCoordsY->v1(), CurrentCoordsY->v2());
// 		
// 		CCoordinates coords_x(rect_to_draw.left, rect_to_draw.right, GetMinX(), GetMaxX());
// 		CCoordinates coords_y(rect_to_draw.bottom, rect_to_draw.top, GetMinY(), GetMaxY());


		int x1, y1,  x2, y2;
		if( m_iCurVertModel > -1 )
		{
			x1 = (int)coords_x.WtoX(GetMinX());
			x2 = (int)coords_x.WtoX(GetMaxX());
			y1 = (int)coords_y.WtoX(GetMinY());
			y2 = (int)coords_y.WtoX(GetMaxY());
		}
		else
		{
			x1 = (int)coords_x.WtoX(0);
			x2 = (int)coords_x.WtoX(1);
			y1 = (int)coords_y.WtoX(0);
			y2 = (int)coords_y.WtoX(1);
		}

		CRect rect(x1, y1, x2, y2);
		DrawAxis(dc_to_draw, rect);		
		
		CPen pen(PS_SOLID, 1, RGB(255, 255, 255));
		CPen *pOldPen = dc_to_draw->SelectObject(&pen);

		dc_to_draw->MoveTo(x1, y1);
		dc_to_draw->LineTo(x1, y2);
		dc_to_draw->MoveTo(x1, y2);
		dc_to_draw->LineTo(x2, y2);
		dc_to_draw->SelectObject(pOldPen);

		CPen pen1(PS_SOLID, 1, RGB(192, 192, 192));
		pOldPen = dc_to_draw->SelectObject(&pen1);

		dc_to_draw->MoveTo(x2, y1);
		dc_to_draw->LineTo(x2, y2);
		dc_to_draw->MoveTo(x1, y1);
		dc_to_draw->LineTo(x2, y1);

		dc_to_draw->SelectObject(pOldPen);
	}
	else
		DrawAxis(dc_to_draw, rect_to_draw);
	
	OnDraw(dc_to_draw, rect_to_draw);

	dc_to_draw->RestoreDC(saved_dc);
}

int CWndGraph::GetWidth(CDC* dest_dc)
{
	return ConvertLogPixToRealPix(dest_dc, FRulerWidth, FALSE);
}

int CWndGraph::ConvertLogPixToRealPix(CDC* dest_dc, double log_pix_num, BOOL b_x_axis)
{
	int index = LOGPIXELSY;
	if (b_x_axis)
	{
		index = LOGPIXELSX;
	};
	int log_pix = GetDeviceCaps(dest_dc->m_hAttribDC, index);
	int res = (int)(log_pix_num* log_pix);
	if (res == 0)
		res = 1;
	return res;
}

int CWndGraph::GetNMax(CDC* dc_to_draw, CRect rect_to_draw)
{
	int MinShift = GetWidth(dc_to_draw);
	int MaxShift = 0;

	CString s;
	s = _T("9.99 ");

	LOGFONT MainFontRec;
	MainFontRec.lfHeight = 14;
	MainFontRec.lfWidth = 8;
	MainFontRec.lfEscapement = 0;
	MainFontRec.lfOrientation = 0;
	MainFontRec.lfWeight = 0;
	MainFontRec.lfItalic = 0;
	MainFontRec.lfUnderline = 0;
	MainFontRec.lfStrikeOut = 0;
	MainFontRec.lfCharSet = ANSI_CHARSET;
	MainFontRec.lfOutPrecision = OUT_TT_PRECIS;
	MainFontRec.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	MainFontRec.lfQuality = PROOF_QUALITY;
	MainFontRec.lfPitchAndFamily = FF_ROMAN | VARIABLE_PITCH;
	strcpy_s(MainFontRec.lfFaceName, _T(""));
	
	TEXTMETRIC TM;
	int Nmax;
	tagSIZE Sz;
	CFont* newfont;
	newfont = new CFont();
	MainFontRec.lfEscapement = 0;
	MainFontRec.lfOrientation = 0;
	MainFontRec.lfHeight = ConvertLogPixToRealPix(dc_to_draw, FFontSize, FALSE);
	MainFontRec.lfWidth = 0;
	newfont->CreateFontIndirect(&MainFontRec);

	CFont* OldFont = (CFont*)dc_to_draw->SelectObject(newfont);
	dc_to_draw->GetTextMetrics(&TM);
	GetTextExtentPoint32(dc_to_draw->m_hDC, LPCTSTR(s), s.GetLength(), &Sz);

	double p;
	p = (double)(rect_to_draw.right - rect_to_draw.left - MinShift - MaxShift) / Sz.cx;
	if (p == (int)p)
		Nmax = (int)p;
	else
		Nmax = (int)p + 1;

	dc_to_draw->SelectObject(OldFont);

	delete newfont;
	return Nmax;
}

int CWndGraph::GetMMax(CDC* dc_to_draw, CRect rect_to_draw)
{
	int MinShift = GetWidth(dc_to_draw);
	int MaxShift = 0;

	CString s;
	s = _T("9.99 ");

	LOGFONT MainFontRec;
	MainFontRec.lfHeight = 14;
	MainFontRec.lfWidth = 8;
	MainFontRec.lfEscapement = 0;
	MainFontRec.lfOrientation = 0;
	MainFontRec.lfWeight = 0;
	MainFontRec.lfItalic = 0;
	MainFontRec.lfUnderline = 0;
	MainFontRec.lfStrikeOut = 0;
	MainFontRec.lfCharSet = ANSI_CHARSET;
	MainFontRec.lfOutPrecision = OUT_TT_PRECIS;
	MainFontRec.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	MainFontRec.lfQuality = PROOF_QUALITY;
	MainFontRec.lfPitchAndFamily = FF_ROMAN | VARIABLE_PITCH;
	strcpy_s(MainFontRec.lfFaceName, _T(""));

	TEXTMETRIC TM;
	int Nmax;
	tagSIZE Sz;
	CFont* newfont;
	newfont = new CFont();
	MainFontRec.lfEscapement = 900;
	MainFontRec.lfOrientation = 900;
	MainFontRec.lfHeight = ConvertLogPixToRealPix(dc_to_draw, FFontSize, TRUE);
	MainFontRec.lfWidth = 0;
	newfont->CreateFontIndirect(&MainFontRec);

	CFont* OldFont = (CFont*)dc_to_draw->SelectObject(newfont);
	dc_to_draw->GetTextMetrics(&TM);
	GetTextExtentPoint32(dc_to_draw->m_hDC, LPCTSTR(s), s.GetLength(), &Sz);

	double p;
	p = (double)(rect_to_draw.bottom - rect_to_draw.top - MinShift - MaxShift) / Sz.cx;
	if (p == (int)p)
		Nmax = (int)p;
	else
		Nmax = (int)p + 1;

	dc_to_draw->SelectObject(OldFont);

	delete newfont;

	return Nmax;
}

void CWndGraph::DrawAxis(CDC* pDC, CRect& rect_to_draw)
{
	if (CurrentCoordsX == NULL || CurrentCoordsY == NULL)
		return;

	int saved_dc = pDC->SaveDC();
	int i;

	//create new set of CCoordinates
	double xMin, xMax, yMin, yMax;
	
	if( m_eType == VERT )
	{
		xMin = 0;
		xMax = 100;
		yMin = 0;
		yMax = 1;
	}
	else
	{
		xMin = GetMinX();
		xMax = GetMaxX();
		yMin = GetMinY();
		yMax = GetMaxY();
	}

	CCoordinates coords_x(rect_to_draw.left, rect_to_draw.right, xMin, xMax);
	CCoordinates coords_y(rect_to_draw.bottom, rect_to_draw.top, yMin, yMax);

	//draw axis and zero line first
	CPen BlackDashPen(PS_DOT, 1, RGB(192, 128, 128));

	CPen* OldPen = (CPen*)pDC->SelectObject(&BlackDashPen);
	
	//horizontal
	double Delta;
	double* wp;
	double* sp;
	int count, maxexp, NMax;
	NMax = GetNMax(pDC, rect_to_draw);
	coords_x.DivideInterval(NMax, &maxexp, &Delta, &count, &sp, &wp);
	for (i = 0; i < count; i++)
	{
		int x = (int)sp[i];
		pDC->MoveTo(x, rect_to_draw.bottom);
		pDC->LineTo(x, rect_to_draw.top);
	}
	if (sp != NULL)
		delete sp;
	if (wp != NULL)
		delete wp;
	//vertical
	NMax = GetMMax(pDC, rect_to_draw);
	coords_y.DivideInterval(NMax, &maxexp, &Delta, &count, &sp, &wp);
	for (i = 0; i < count; i++)
	{
		int x = (int)sp[i];
		pDC->MoveTo(rect_to_draw.right, x);
		pDC->LineTo(rect_to_draw.left, x);
	};
	if (sp != NULL)
		delete sp;
	if (wp != NULL)
		delete wp;

	pDC->SelectObject(OldPen);

	pDC->RestoreDC(saved_dc);
}

void CWndGraph::DrawPlane(CDC* dc, CRect& rect_to_draw, bool bTop)
{
	CFormGenerateLayer *pView = GetParentView();
	if( pView == NULL )
		return;
	CGenerateLayerDoc *pDoc = pView->GetDocument();
	if( pDoc == NULL)
		return;

	int saved_dc = dc->SaveDC();

	//set new clipping region
	dc->IntersectClipRect(rect_to_draw);

	//create new set of CCoordinates
	CCoordinates coords_x(rect_to_draw.left, rect_to_draw.right, CurrentCoordsX->v1(), CurrentCoordsX->v2());
	CCoordinates coords_y(rect_to_draw.bottom, rect_to_draw.top, CurrentCoordsY->v1(), CurrentCoordsY->v2());

	if( bTop )
	{
		int nSize = pDoc->m_layerModel.m_topPlane.size();
		for (int i=0; i< nSize; i++)
		{
			int num = pDoc->m_layerModel.m_topPlane[i].m_indexs.size()/3;
			for (int j=0; j<num; j++)
			{
				CPoint point[3];
				CPoint2D pt = pDoc->m_layerModel.m_topPlane[i].m_points[pDoc->m_layerModel.m_topPlane[i].m_indexs[j*3+0]];
				
				point[0].x = (int)coords_x.WtoX(pt.x);
				point[0].y = (int)coords_y.WtoX(pt.y);

				pt = pDoc->m_layerModel.m_topPlane[i].m_points[pDoc->m_layerModel.m_topPlane[i].m_indexs[j*3+1]];
				point[1].x = (int)coords_x.WtoX(pt.x);
				point[1].y = (int)coords_y.WtoX(pt.y);

				pt = pDoc->m_layerModel.m_topPlane[i].m_points[pDoc->m_layerModel.m_topPlane[i].m_indexs[j*3+2]];
				point[2].x = (int)coords_x.WtoX(pt.x);
				point[2].y = (int)coords_y.WtoX(pt.y);

				dc->Polygon(point, 3);
			}
		}
	}
	else
	{
		int nSize = pDoc->m_layerModel.m_bottomPlane.size();
		for (int i=0; i< nSize; i++)
		{
			int num = pDoc->m_layerModel.m_bottomPlane[i].m_indexs.size()/3;
			for (int j=0; j<num; j++)
			{
				CPoint point[3];
				CPoint2D pt = pDoc->m_layerModel.m_bottomPlane[i].m_points[pDoc->m_layerModel.m_bottomPlane[i].m_indexs[j*3+0]];

				point[0].x = (int)coords_x.WtoX(pt.x);
				point[0].y = (int)coords_y.WtoX(pt.y);

				pt = pDoc->m_layerModel.m_bottomPlane[i].m_points[pDoc->m_layerModel.m_bottomPlane[i].m_indexs[j*3+1]];
				point[1].x = (int)coords_x.WtoX(pt.x);
				point[1].y = (int)coords_y.WtoX(pt.y);

				pt = pDoc->m_layerModel.m_bottomPlane[i].m_points[pDoc->m_layerModel.m_bottomPlane[i].m_indexs[j*3+2]];
				point[2].x = (int)coords_x.WtoX(pt.x);
				point[2].y = (int)coords_y.WtoX(pt.y);

				dc->Polygon(point, 3);
			}
		}
	}

	dc->RestoreDC(saved_dc);
}


void CWndGraph::DrawVertModel(CDC* dc, CRect& rect_to_draw)
{
	CFormGenerateLayer *pView = GetParentView();
	if( pView == NULL )
		return;
	CGenerateLayerDoc *pDoc = pView->GetDocument();
	if( pDoc == NULL)
		return;

	int nSize = pDoc->m_layerModel.m_vertModel.size();

	if( m_iCurVertModel >= nSize )
	{
		for (int i = nSize; i<=m_iCurVertModel; i++)
		{
			std::vector<CPoint2D> points;
			pDoc->m_layerModel.m_vertModel.push_back(points);
		}

		return;
	}

	if( m_iCurVertModel < 0 )
		return;

	int num = pDoc->m_layerModel.m_vertModel[m_iCurVertModel].size();
	
	int saved_dc = dc->SaveDC();

	//set new clipping region
	dc->IntersectClipRect(rect_to_draw);

	//create new set of CCoordinates
	CCoordinates coords_x(rect_to_draw.left, rect_to_draw.right, CurrentCoordsX->v1(), CurrentCoordsX->v2());
	CCoordinates coords_y(rect_to_draw.bottom, rect_to_draw.top, CurrentCoordsY->v1(), CurrentCoordsY->v2());

	CPen pen(PS_SOLID, 0, pDoc->m_layerModel.m_vertColor[m_iCurVertModel]);
	CPen* oldpen = dc->SelectObject(&pen);

	CArray<POINT> pts;
	POINT p;
	for (int j=0; j<num; j++)
	{
		CPoint2D pt;
		pt = pDoc->m_layerModel.m_vertModel[m_iCurVertModel][j];
		int x = (int)coords_x.WtoX(pt.x);
		int y = (int)coords_y.WtoX(pt.y);
		POINT pt1;
		pt1.x = x;
		pt1.y = y;
		if( j!= 0 )
		{
			if( pt1.x == p.x && pt1.y == p.y)
				continue;
		}
		p.x =pt1.x;
		p.y = pt1.y;
		pts.Add(pt1);
	}
	if (pts.GetSize() > 2)
	{
		//PolySpline3(dc->m_hDC, pts.GetData(), pts.GetSize());
		SPLINE::CSpline sp(pts.GetData(), pts.GetSize());
		sp.Generate();
		sp.draw(dc->m_hDC);
	}
	else
	{
		dc->Polyline(pts.GetData(), pts.GetSize());
	}

	dc->SelectObject(oldpen);

	// 画模型深度线
	{
		CPen pen(PS_SOLID, 0, RGB(255, 0, 0));
		oldpen = dc->SelectObject(&pen);
		int y1 = (int)coords_y.WtoX(m_dDepthScale);
		
		dc->MoveTo(rect_to_draw.left, y1);
		dc->LineTo(rect_to_draw.right, y1);

		for (int j=1; j<num; j++)
		{
			CPoint2D pt1, pt2;
			pt1 = pDoc->m_layerModel.m_vertModel[m_iCurVertModel][j-1];
			pt2 = pDoc->m_layerModel.m_vertModel[m_iCurVertModel][j];

			if( m_dDepthScale>=pt2.y && m_dDepthScale<pt1.y )
			{
				
				double x = (m_dDepthScale-pt1.y)/(pt2.y-pt1.y)*(pt2.x-pt1.x)+pt1.x;

				int x1 = (int)coords_x.WtoX(x);
				
				m_dXYScale = x;				

				CRect rect(x1-5,y1-5, x1+5,y1+5);
				dc->Ellipse(rect);

				break;
			}
		}

		dc->SelectObject(oldpen);
	}

	dc->SetTextColor(pDoc->m_layerModel.m_vertColor[m_iCurVertModel]);

	dc->SetBkMode(TRANSPARENT);

	CFont font;
	font.CreateFont(13,0,0,0,0,0,0,0,GB2312_CHARSET,0,0,0,0,_T("宋体"));
	CFont *pOldFont = dc->SelectObject(&font);
	CString str = _T("名称:");
	str += pDoc->m_layerModel.m_vertTitle[m_iCurVertModel];
	dc->TextOut(4,4,str);
	dc->SelectObject(pOldFont);

	dc->RestoreDC(saved_dc);
}

void CWndGraph::DrawHorzModel(CDC* dc, CRect& rect_to_draw)
{
	CFormGenerateLayer *pView = GetParentView();
	if( pView == NULL )
		return;
	CGenerateLayerDoc *pDoc = pView->GetDocument();
	if( pDoc == NULL)
		return;

	int saved_dc = dc->SaveDC();

	//set new clipping region
	dc->IntersectClipRect(rect_to_draw);

	//create new set of CCoordinates
	CCoordinates coords_x(rect_to_draw.left, rect_to_draw.right, CurrentCoordsX->v1(), CurrentCoordsX->v2());
	CCoordinates coords_y(rect_to_draw.bottom, rect_to_draw.top, CurrentCoordsY->v1(), CurrentCoordsY->v2());

	{
		CPen pen1(PS_SOLID, 0, RGB(0,0,0));
		CPen pen2(PS_SOLID, 0, RGB(128,0,0));

		CPen* oldpen = dc->SelectObject(&pen1);

		int nSize = pDoc->m_layerModel.m_layers.size();
		for (int i=0; i<nSize; i++)
		{
			CArray<POINT> pts;
			int num = pDoc->m_layerModel.m_layers[i].size();
			for (int j=0; j<num; j++)
			{
				CPoint2D pt;
				pt = pDoc->m_layerModel.m_layers[i][j];
				int x = (int)coords_x.WtoX(pt.x);
				int y = (int)coords_y.WtoX(pt.y);
				POINT pt1;
				pt1.x = x;
				pt1.y = y;
				pts.Add(pt1);
			}
			if (pts.GetSize() > 2 )
			{
				PolySpline3(dc->m_hDC, pts.GetData(), pts.GetSize());
			}
			else
			{
				dc->Polyline(pts.GetData(), pts.GetSize());
			}
		}

		dc->SelectObject(oldpen);

		oldpen = dc->SelectObject(&pen2);

		nSize = pDoc->m_layerModel.m_singleLayers.size();
		for (int i=0; i<nSize; i++)
		{
			CArray<POINT> pts;
			int num = pDoc->m_layerModel.m_singleLayers[i].size();
			for (int j=0; j<num; j++)
			{
				CPoint2D pt;
				pt = pDoc->m_layerModel.m_singleLayers[i][j];
				int x = (int)coords_x.WtoX(pt.x);
				int y = (int)coords_y.WtoX(pt.y);
				POINT pt1;
				pt1.x = x;
				pt1.y = y;
				pts.Add(pt1);
			}
			if (pts.GetSize() > 2 )
			{
				PolySpline3(dc->m_hDC, pts.GetData(), pts.GetSize());
			}
			else
			{
				dc->Polyline(pts.GetData(), pts.GetSize());
			}
		}

		dc->SelectObject(oldpen);	
	}
	
	// 画法线	
	{
		int nSize = pDoc->m_layerModel.m_normals[0].size();
		for (int i=0; i<nSize; i++)
		{
			COLORREF clr1, clr2;
			int num = pDoc->m_layerModel.m_vertModel[i].size();
			if( num > 0 )
			{
				clr1 = pDoc->m_layerModel.m_vertColor[i];
				clr2 = pDoc->m_layerModel.m_vertColor[i];
			}
			else
			{
				clr1 = RGB(255, 0, 0);
				clr2 = RGB(255, 255, 255);
			}
			
			CPen pen(PS_SOLID, 0, clr1);
			CPen* oldpen = dc->SelectObject(&pen);			

			CPoint2D pt;
			pt = pDoc->m_layerModel.m_normals[1][i];
			int x = (int)coords_x.WtoX(pt.x);
			int y = (int)coords_y.WtoX(pt.y);
			dc->MoveTo(x, y);
			pt = pDoc->m_layerModel.m_normals[0][i];
			x = (int)coords_x.WtoX(pt.x);
			y = (int)coords_y.WtoX(pt.y);
			dc->LineTo(x, y);

			CPoint pp[4];
			CalcArrowRgn(pp,i);			
			CPoint p[3];
			p[0].x = x;
			p[0].y = y;
			p[1] = pp[2];
			p[2] = pp[3];
			CRgn rgn;
			rgn.CreatePolygonRgn(p, 3, ALTERNATE);
			
			CBrush b(clr2);
			
			dc->FillRgn(&rgn, &b);
			dc->MoveTo(p[0]);
			dc->LineTo(p[1]);
			dc->LineTo(p[2]);
			dc->LineTo(p[0]);

			dc->SelectObject(oldpen);
			if( num > 0 )
			{
				CFont font;
				if(pDoc->m_layerModel.m_normals[1][i].x>pDoc->m_layerModel.m_normals[0][i].x)
					dc->SetTextAlign(TA_RIGHT|TA_TOP);
				else
					dc->SetTextAlign(TA_LEFT|TA_TOP);
				dc->SetBkMode(TRANSPARENT);
				font.CreateFont(13, 0,0,0,0,0,0,0,GB2312_CHARSET,0,0,0,0,_T("宋体"));
				CFont *pOldFont = dc->SelectObject(&font);
				dc->SetTextColor(clr1);
				if(pDoc->m_layerModel.m_normals[1][i].x>pDoc->m_layerModel.m_normals[0][i].x)
					dc->TextOut(p[0].x-8, p[0].y-8, pDoc->m_layerModel.m_vertTitle[i]);
				else
					dc->TextOut(p[0].x+8, p[0].y-8, pDoc->m_layerModel.m_vertTitle[i]);
				dc->SelectObject(pOldFont);
			}
		}		

		nSize = m_selectVertModel.size();
		for (int i=0;i<nSize;i++)
		{
			if( m_selectVertModel[i] != -1)
			{
				CPen pen(PS_SOLID, 2, RGB(0, 255, 255));
				CPen* oldpen = dc->SelectObject(&pen);

				CPoint2D pt;
				pt = pDoc->m_layerModel.m_normals[1][m_selectVertModel[i]];
				int x = (int)coords_x.WtoX(pt.x);
				int y = (int)coords_y.WtoX(pt.y);
				dc->MoveTo(x, y);
				pt = pDoc->m_layerModel.m_normals[0][m_selectVertModel[i]];
				x = (int)coords_x.WtoX(pt.x);
				y = (int)coords_y.WtoX(pt.y);
				dc->LineTo(x, y);

				CPoint pp[4];
				CalcArrowRgn(pp,m_selectVertModel[i]);			
				CPoint p[3];
				p[0].x = x;
				p[0].y = y;
				p[1] = pp[2];
				p[2] = pp[3];
				CRgn rgn;
				rgn.CreatePolygonRgn(p, 3, ALTERNATE);
				CBrush b;
				int num = pDoc->m_layerModel.m_vertModel[m_selectVertModel[i]].size();
				if(num>0)
					b.CreateSolidBrush(RGB(0, 255, 255));
				else
					b.CreateSolidBrush(RGB(255, 255, 255));
				dc->FillRgn(&rgn, &b);
				dc->MoveTo(p[0]);
				dc->LineTo(p[1]);
				dc->LineTo(p[2]);
				dc->LineTo(p[0]);

				dc->SelectObject(oldpen);
			}
		}
	}

	dc->RestoreDC(saved_dc);
}

void CWndGraph::OnDraw(CDC* dc, CRect& rect_to_draw)
{
	switch(m_eType)
	{
	case VERT:
		DrawVertModel(dc, rect_to_draw);
		break;
	case HORZ:
		DrawHorzModel(dc, rect_to_draw);
		break;
	case TOP_PLANE:
		DrawPlane(dc, rect_to_draw, true);
		break;
	case BOTTOM_PLANE:
		DrawPlane(dc, rect_to_draw, false);
		break;
	default:
		break;
	}
}

void CWndGraph::SetMinMax(const CPoint2D &pt)
{
	if (pt.x < GetMinX())
		SetMinX(pt.x);

	if (pt.x > GetMaxX())
		SetMaxX(pt.x);

	if (pt.y < GetMinY())
		SetMinY(pt.y);

	if (pt.y >GetMaxY())
		SetMaxY(pt.y);

	if (pt.m_dValue < GetMinZ())
		SetMinZ(pt.m_dValue);

	if (pt.m_dValue >GetMaxZ())
		SetMaxZ(pt.m_dValue);
}

void CWndGraph::AddPoint(int index, const CPoint2D &pt)
{
	CFormGenerateLayer *pView = GetParentView();
	if( pView == NULL )
		return;
	CGenerateLayerDoc *pDoc = pView->GetDocument();
	if( pDoc == NULL)
		return;

	SetMinMax(pt);

	if( m_eType == HORZ ) // 夹层投影模型
	{
		
	}

	if( m_eType == VERT ) // 纵向模型
	{
		if( m_iCurVertModel < 0 )
			return;

		int nSize = pDoc->m_layerModel.m_vertModel.size();

		if( index >= nSize )
			return;

		pDoc->m_layerModel.m_vertModel[index].push_back(pt);
	}

	if( m_eType == TOP_PLANE )
	{
		if( (index + 1) > pDoc->m_layerModel.m_topPlane.size() )
		{
			tagPlane plane;
			pDoc->m_layerModel.m_topPlane.push_back(plane);
		}
		pDoc->m_layerModel.m_topPlane[index].m_points.push_back(pt);
	}

	if( m_eType == BOTTOM_PLANE )
	{
		if( (index + 1) > pDoc->m_layerModel.m_bottomPlane.size() )
		{
			tagPlane plane;
			pDoc->m_layerModel.m_bottomPlane.push_back(plane);
		}
		pDoc->m_layerModel.m_bottomPlane[index].m_points.push_back(pt);
	}
}

void CWndGraph::AddIndex(int index, const int &vertexIndex)
{
	CFormGenerateLayer *pView = GetParentView();
	if( pView == NULL )
		return;
	CGenerateLayerDoc *pDoc = pView->GetDocument();
	if( pDoc == NULL)
		return;

	if( m_eType == TOP_PLANE )
	{
		if( (index + 1) > pDoc->m_layerModel.m_topPlane.size() )
		{
			tagPlane plane;
			pDoc->m_layerModel.m_topPlane.push_back(plane);
		}
		pDoc->m_layerModel.m_topPlane[index].m_indexs.push_back(vertexIndex);	
	}

	if( m_eType == BOTTOM_PLANE )
	{
		if( (index + 1) > pDoc->m_layerModel.m_bottomPlane.size() )
		{
			tagPlane plane;
			pDoc->m_layerModel.m_bottomPlane.push_back(plane);
		}
		pDoc->m_layerModel.m_bottomPlane[index].m_indexs.push_back(vertexIndex);	
	}
}

void CWndGraph::AddNormals(int index, const CPoint2D &pt)
{
	CFormGenerateLayer *pView = GetParentView();
	if( pView == NULL )
		return;
	CGenerateLayerDoc *pDoc = pView->GetDocument();
	if( pDoc == NULL)
		return;

	SetMinMax(pt);

	pDoc->m_layerModel.m_normals[index].push_back(pt);
}

LRESULT CWndGraph::SendMessageToParent(int nMessage) const
{
	if (!IsWindow(m_hWnd))
		return 0;

	tagWGNMHDR wg;
	
	wg.hdr.hwndFrom = m_hWnd;
	wg.hdr.idFrom   = GetDlgCtrlID();
	wg.hdr.code     = nMessage;
	
	switch(nMessage)
	{
	case GVN_DATACHANGED:
		{
			wg.m_dDepthScale = m_dDepthScale;
		}
		break;
	case GVN_NORMALCHANGING:
		{
			if( m_selectVertModel.size()>0)
				wg.m_iNormal = m_selectVertModel[0];
			else
				wg.m_iNormal = -1;
		}
		break;
	default:
		break;
	}	

	CWnd *pOwner = GetOwner();
	if (pOwner && IsWindow(pOwner->m_hWnd))
		return pOwner->SendMessage(WM_NOTIFY, wg.hdr.idFrom, (LPARAM)&wg);
	else
		return 0;
}


void CWndGraph::AddNormals(LPCTSTR lpszFileName)	// 加法线
{	
	InitCoords(DBL_MAX,-DBL_MAX,DBL_MAX,-DBL_MAX);

	CFile file(lpszFileName, CFile::modeRead|CFile::typeBinary);
	CArchive ar(&file, CArchive::load);

	int nRows;
	int nCols;

	ar >> nRows;
	ar >> nCols;

	CString strTmp[4];
	int index = 0;
	for (int i=0;i<nRows;i++)
	{
		ar >> strTmp[0];
		ar >> strTmp[1];
		ar >> strTmp[2];
		ar >> strTmp[3];
		if( strTmp[0] == _T("EOM"))
		{
			index = 0;
		}
		else
		{	
			CPoint2D point;
			point.x = atof(strTmp[0]);
			point.y = atof(strTmp[1]);
			AddNormals(index, point);
			index ++;
		}
	}
	
	ar.Close();
	file.Close();	
	SendMessageToParent(GVN_DATACHANGED);

	double width;
	width = max(GetMaxX()-GetMinX(), GetMaxY()-GetMinY());

	double x1, x2, y1, y2;

	x1 = GetMinX() - (width-(GetMaxX()-GetMinX()))/2.0;
	x2 = x1 + width;

	y1 = GetMinY() - (width-(GetMaxY()-GetMinY()))/2.0;
	y2 = y1 + width;

	SetWorldCoords(x1, x2, y1, y2, TRUE);
}

void CWndGraph::RefreshCoord()
{
	CFormGenerateLayer *pView = GetParentView();
	if( pView == NULL )
		return;
	CGenerateLayerDoc *pDoc = pView->GetDocument();
	if( pDoc == NULL)
		return;

	InitCoords(DBL_MAX,-DBL_MAX,DBL_MAX,-DBL_MAX);
	
	switch(m_eType)
	{
	case TOP_PLANE:
		{
			int nSize = pDoc->m_layerModel.m_topPlane.size();
			for (int i=0; i< nSize; i++)
			{
				int num = pDoc->m_layerModel.m_topPlane[i].m_points.size();
				for (int j=0; j<num; j++)
				{
					CPoint2D pt = pDoc->m_layerModel.m_topPlane[i].m_points[j];
					SetMinMax(pt);
				}
			}

			double dWidth = max(GetMaxX()-GetMinX(), GetMaxY()-GetMinY());

			double x1 = (GetMinX() + GetMaxX())/2.0 - dWidth/2.0;
			double x2 = x1 + dWidth;
			double y1 = (GetMinY() + GetMaxY())/2.0 - dWidth/2.0;
			double y2 = y1 + dWidth;

			CRect rect;
			GetClientRect(rect);
			int cx = rect.Width();
			int cy = rect.Height();

			if( cx>cy)
				SetWorldCoords(x1-(double)(cx-cy)/(double)cx/2.0*dWidth,x2+(double)(cx-cy)/(double)cx/2.0*dWidth,y1,y2,TRUE);
			else
				SetWorldCoords(x1,x2,y1-(double)(cy-cx)/(double)cy/2.0*dWidth,y2+(double)(cy-cx)/(double)cy/2.0*dWidth,TRUE);
		}
		break;
	case BOTTOM_PLANE:
		{
			int nSize = pDoc->m_layerModel.m_bottomPlane.size();
			for (int i=0; i< nSize; i++)
			{
				int num = pDoc->m_layerModel.m_bottomPlane[i].m_points.size();
				for (int j=0; j<num; j++)
				{
					CPoint2D pt = pDoc->m_layerModel.m_bottomPlane[i].m_points[j];
					SetMinMax(pt);
				}
			}

			double dWidth = max(GetMaxX()-GetMinX(), GetMaxY()-GetMinY());

			double x1 = (GetMinX() + GetMaxX())/2.0 - dWidth/2.0;
			double x2 = x1 + dWidth;
			double y1 = (GetMinY() + GetMaxY())/2.0 - dWidth/2.0;
			double y2 = y1 + dWidth;

			CRect rect;
			GetClientRect(rect);
			int cx = rect.Width();
			int cy = rect.Height();

			if( cx>cy)
				SetWorldCoords(x1-(double)(cx-cy)/(double)cx/2.0*dWidth,x2+(double)(cx-cy)/(double)cx/2.0*dWidth,y1,y2,TRUE);
			else
				SetWorldCoords(x1,x2,y1-(double)(cy-cx)/(double)cy/2.0*dWidth,y2+(double)(cy-cx)/(double)cy/2.0*dWidth,TRUE);
		}
		break;
	case HORZ:
		{
			int nSize = pDoc->m_layerModel.m_layers.size();
			for (int i=0; i< nSize; i++)
			{
				int num = pDoc->m_layerModel.m_layers[i].size();
				for (int j=0; j<num; j++)
				{
					CPoint2D pt = pDoc->m_layerModel.m_layers[i][j];
					SetMinMax(pt);
				}
			}

			nSize = pDoc->m_layerModel.m_singleLayers.size();
			for (int i=0; i< nSize; i++)
			{
				int num = pDoc->m_layerModel.m_singleLayers[i].size();
				for (int j=0; j<num; j++)
				{
					CPoint2D pt = pDoc->m_layerModel.m_singleLayers[i][j];
					SetMinMax(pt);
				}
			}

			nSize = pDoc->m_layerModel.m_normals[0].size();
			for (int i=0; i< nSize; i++)
			{
				for (int j=0; j<2; j++)
				{
					CPoint2D pt = pDoc->m_layerModel.m_normals[j][i];
					SetMinMax(pt);
				}
			}

			double dWidth = max(GetMaxX()-GetMinX(), GetMaxY()-GetMinY());

			double x1 = (GetMinX() + GetMaxX())/2.0 - dWidth/2.0;
			double x2 = x1 + dWidth;
			double y1 = (GetMinY() + GetMaxY())/2.0 - dWidth/2.0;
			double y2 = y1 + dWidth;

			CRect rect;
			GetClientRect(rect);
			int cx = rect.Width();
			int cy = rect.Height();

			if( cx>cy)
				SetWorldCoords(x1-(double)(cx-cy)/(double)cx/2.0*dWidth,x2+(double)(cx-cy)/(double)cx/2.0*dWidth,y1,y2,TRUE);
			else
				SetWorldCoords(x1,x2,y1-(double)(cy-cx)/(double)cy/2.0*dWidth,y2+(double)(cy-cx)/(double)cy/2.0*dWidth,TRUE);
		}
		break;
	}
}

void CWndGraph::AddLayers(LPCTSTR lpszFileName)		// 加夹层
{
	Init();

	InitCoords(DBL_MAX,-DBL_MAX,DBL_MAX,-DBL_MAX);

	CFile file(lpszFileName, CFile::modeRead|CFile::typeBinary);
	CArchive ar(&file, CArchive::load);

	int nRows;
	int nCols;

	ar >> nRows;
	ar >> nCols;

	CString strTmp[4];
	int index = 0;
	for (int i=0;i<nRows;i++)
	{
		ar >> strTmp[0];
		ar >> strTmp[1];
		ar >> strTmp[2];
		ar >> strTmp[3];
		if( strTmp[0] == _T("EOM"))
		{
			index++;
		}
		else
		{
			CPoint2D point;
			point.x = atof(strTmp[0]);
			point.y = atof(strTmp[1]);
			point.m_dValue = atof(strTmp[2]);
			AddPoint(index, point);
		}
	}

	ar.Close();
	file.Close();	
	SendMessageToParent(GVN_DATACHANGED);
	
	double width;
	width = max(GetMaxX()-GetMinX(), GetMaxY()-GetMinY());

	double x1, x2, y1, y2;

	x1 = GetMinX() - (width-(GetMaxX()-GetMinX()))/2.0;
	x2 = x1 + width;

	y1 = GetMinY() - (width-(GetMaxY()-GetMinY()))/2.0;
	y2 = y1 + width;
	
	SetWorldCoords(x1, x2, y1, y2, TRUE);
}

void CWndGraph::AddPlanes(LPCTSTR lpszFileName, bool bTop)		// 加面
{
	Init();

	CFile file(lpszFileName, CFile::modeRead|CFile::typeBinary);
	CArchive ar(&file, CArchive::load);

	int nRows;
	int nCols;

	ar >> nRows;
	ar >> nCols;

	CString strTmp[4];

	BOOL bIsVertex = TRUE;

	int index1 = -1, index2 = -1;

	for (int i=0;i<nRows;i++)
	{
		ar >> strTmp[0];
		ar >> strTmp[1];
		ar >> strTmp[2];
		ar >> strTmp[4];
		if( strTmp[0] == _T("Coordinate"))
		{
			index1 ++;
			bIsVertex = TRUE;
			continue;
		}

		if( strTmp[0] == _T("CoordIndex"))
		{
			bIsVertex = FALSE;
			index2 ++;
			continue;
		}

		if( bIsVertex )
		{
			CPoint2D point;
			point.x = atof(strTmp[0]);
			point.y = atof(strTmp[1]);
			point.m_dValue = atof(strTmp[2]);

			AddPoint(index1, point);
		}
		else
		{
			AddIndex(index2, atoi(strTmp[0]));
			AddIndex(index2, atoi(strTmp[1]));
			AddIndex(index2, atoi(strTmp[2]));
		}
	}
	
	ar.Close();
	file.Close();
	SendMessageToParent(GVN_DATACHANGED);

	double width;
	width = max(GetMaxX()-GetMinX(), GetMaxY()-GetMinY());

	double x1, x2, y1, y2;

	x1 = GetMinX() - (width-(GetMaxX()-GetMinX()))/2.0;
	x2 = x1 + width;

	y1 = GetMinY() - (width-(GetMaxY()-GetMinY()))/2.0;
	y2 = y1 + width;

	SetWorldCoords(x1, x2, y1, y2, TRUE);
}

void CWndGraph::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if( m_eType == VERT && (nFlags&MK_LBUTTON))
	{
		CRect rect_to_draw;
		GetClientRect(rect_to_draw);

		//create new set of CCoordinates
		CCoordinates coords_x(rect_to_draw.left, rect_to_draw.right, CurrentCoordsX->v1(), CurrentCoordsX->v2());
		CCoordinates coords_y(rect_to_draw.bottom, rect_to_draw.top, CurrentCoordsY->v1(), CurrentCoordsY->v2());

		m_dDepthScale = coords_y.XtoW(point.y);

		SendMessageToParent(GVN_DATACHANGED);
		UpdateGraphWindow(NULL);
	}
	CWnd::OnMouseMove(nFlags, point);
}

bool CWndGraph::CalcArrowRgn(CPoint *pt, int index)
{
	CFormGenerateLayer *pView = GetParentView();
	if( pView == NULL )
		return false;
	CGenerateLayerDoc *pDoc = pView->GetDocument();
	if( pDoc == NULL)
		return false;

	CRect rect_to_draw;
	GetClientRect(rect_to_draw);

	CCoordinates coords_x(rect_to_draw.left, rect_to_draw.right, CurrentCoordsX->v1(), CurrentCoordsX->v2());
	CCoordinates coords_y(rect_to_draw.bottom, rect_to_draw.top, CurrentCoordsY->v1(), CurrentCoordsY->v2());
	
	double Len = 11.0;
	CPoint point[3];

	point[0].x = (int)coords_x.WtoX(pDoc->m_layerModel.m_normals[0][index].x);
	point[0].y = (int)coords_y.WtoX(pDoc->m_layerModel.m_normals[0][index].y);
	point[1].x = (int)coords_x.WtoX(pDoc->m_layerModel.m_normals[1][index].x);
	point[1].y = (int)coords_y.WtoX(pDoc->m_layerModel.m_normals[1][index].y);


	double A1, B1, C1, A2, B2, C2;			
	double A, B, C;	


	A = point[1].y - point[0].y ;
	B = point[0].x - point[1].x;
	C = - (  A * point[0].x + B * point[0].y );				

	A2 = A;
	B2 = B;

	A1 = -B2;
	B1 = A2;
	C1 = - A1 * point[0].x - B1 * point[0].y;//?

	C2 = Len/2 * sqrt(A*A + B*B ) + C;

	pt[0].x = ( B1 *C2 - B2 * C1 ) / ( A1 * B2 - A2 * B1 );
	pt[0].y = ( C1 *A2 - C2 * A1 ) / ( A1 * B2 - A2 * B1 );			

	C2 = C - Len/2 * sqrt(A*A + B*B );

	pt[1].x = ( B1 *C2 - B2 * C1 ) / ( A1 * B2 - A2 * B1 );
	pt[1].y = ( C1 *A2 - C2 * A1 ) / ( A1 * B2 - A2 * B1 );

	double lamta;

	double L = sqrt( (double)(point[0].x - point[1].x) * (point[0].x - point[1].x) + (point[0].y - point[1].y) * (point[0].y - point[1].y) );

	lamta = Len / ( L - Len );

	point[2].x = (int)( ( point[0].x + point[1].x * lamta )/ ( 1+lamta) );
	point[2].y = (int)( ( point[0].y + point[1].y * lamta )/ ( 1+lamta) );

	C1 = - A1 * point[2].x - B1 * point[2].y;//?

	C2 = Len / 2 * sqrt(A*A + B*B ) + C;

	pt[3].x = ( B1 *C2 - B2 * C1 ) / ( A1 * B2 - A2 * B1 );
	pt[3].y = ( C1 *A2 - C2 * A1 ) / ( A1 * B2 - A2 * B1 );			

	C2 = C - Len / 2 * sqrt(A*A + B*B );

	pt[2].x = ( B1 *C2 - B2 * C1 ) / ( A1 * B2 - A2 * B1 );
	pt[2].y = ( C1 *A2 - C2 * A1 ) / ( A1 * B2 - A2 * B1 );

	return true;
}

// 法线改变了
void CWndGraph::NormalChanged(int index)
{
	m_iCurVertModel = index;
// 	CRect rect;
// 	GetClientRect( rect );
// 	int cx = rect.Width();
// 	int cy = rect.Height();
// 
// 	if( cx>cy)
// 		SetWorldCoords(-0.1-(double)(cx-cy)/(double)cx,1.1+(double)(cx-cy)/(double)cx,-0.1,1.1,TRUE);
// 	else
// 		SetWorldCoords(-0.1,1.1,-0.1-(double)(cy-cx)/(double)cy,1.1+(double)(cy-cx)/(double)cy,TRUE);
	UpdateGraphWindow(NULL);
}

void CWndGraph::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CWnd::OnLButtonDown(nFlags, point);

	CFormGenerateLayer *pView = GetParentView();
	if( pView == NULL )
		return;
	CGenerateLayerDoc *pDoc = pView->GetDocument();
	if( pDoc == NULL)
		return;

	CRect rect_to_draw;
	GetClientRect(rect_to_draw);

	//create new set of CCoordinates
	CCoordinates coords_x(rect_to_draw.left, rect_to_draw.right, CurrentCoordsX->v1(), CurrentCoordsX->v2());
	CCoordinates coords_y(rect_to_draw.bottom, rect_to_draw.top, CurrentCoordsY->v1(), CurrentCoordsY->v2());
	
	if(m_eType == HORZ)
	{	
		CPoint2D p;
		int ret;
		LINESEG l;
		CPoint2D pt;
		pt.x = point.x;
		pt.y = point.y;

		int iCur = -1;	
		int nSize = pDoc->m_layerModel.m_normals[0].size();
		for (int i=0; i<nSize; i++)
		{
			CPoint2D pp = pDoc->m_layerModel.m_normals[0][i];
			l.s.x = coords_x.WtoX(pp.x);
			l.s.y = coords_y.WtoX(pp.y);

			pp = pDoc->m_layerModel.m_normals[1][i];
			l.e.x = coords_x.WtoX(pp.x);
			l.e.y = coords_y.WtoX(pp.y);

			if( ptolinesegdist(pt, l, p, ret) <= 8.0 )
			{
				if(nFlags&MK_CONTROL)
				{
					int num = m_selectVertModel.size();
					int j;
					for ( j=0;j<num; j++)
					{
						if( i==m_selectVertModel[j])
						{
							m_selectVertModel.erase(m_selectVertModel.begin()+i);
							break;
						}
					}
					if( j==num)
						m_selectVertModel.push_back(i);
				}
				else
				{
					m_selectVertModel.clear();
					m_selectVertModel.push_back(i);
				}
				iCur = i;
				break;
			}
		}

		if( iCur == -1 && !(nFlags&MK_CONTROL))
			m_selectVertModel.clear();
		
		SendMessageToParent(GVN_NORMALCHANGING);

		UpdateGraphWindow(NULL);		
	}
}
