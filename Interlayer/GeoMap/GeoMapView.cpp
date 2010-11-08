// GeoMapView.cpp : implementation file
//

#include "stdafx.h"
#include "GeoMapView.h"
#include "../Resource.h"
#include "GeoDrawObj.h"
#include "GeoDrawOleObj.h"
#include "ToolBase.h"
#include "CmdGeoDrawObj.h"
#include "GeoPolyObj.h"
#include "GeoTextObj.h"
#include "../command/CmdMoveLayerBack.h"
#include "../command/CmdMoveLayerForward.h"
#include "ToolSelect.h"
#include "DlgExportPicture.h"
#include <typeinfo>
#include "../MainFrm.h"
#include <afxodlgs.h>
#include "GeoPictrueObj.h"
#include "../DlgAddCtrlPoint.h"
#include "GeoWellObj.h"
#include <math.h>
#include "GeoLineObj.h"
#include "../CoordTrans.h"
#include "../LayerModel.h"

#define PATH_MAX 512

//extern CGPolygon convex_hull_set_points (const list<CGPoint> & points);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//-----------------------------------------------------------------------

CPoint m_gDownPt;
CLIPFORMAT CGeoMapView::m_cfDraw_E = (CLIPFORMAT)::RegisterClipboardFormat(_T("GeoToolMap"));
CLIPFORMAT CGeoMapView::m_cfObjectDescriptor = NULL;
extern CGeoToolSelect selectTool;

extern void StatusBarMessage(char* fmt, ...);
extern void SetStatusXY(double x, double y, LPCSTR lpStr);
extern void SetStatusCenter(double x, double y, LPCSTR lpStr);
extern void StatusProgress(int nIndex, int nRange=-1);
extern void StatusSetProgress(int nIndex, int nPos);

extern CString FindExtension(const CString& name);
extern int FindType(const CString& ext);
extern CString GetFileTypes(BOOL bOpenFileDialog);
//------------------------------------------------------------------------
// 直线与线段相交，如果bLine为true第一个为直线否则为线段，第二个为线段
bool Intersect(CPoint2D *p1, CPoint2D *p2, CPoint2D *p, bool bLine = true)
{
	double A1, B1, C1, A2, B2, C2, D;

	A1 = p1[0].y - p1[1].y;
	B1 = p1[1].x - p1[0].x;
	C1 = p1[1].y*p1[0].x - p1[1].x*p1[0].y;

	A2 = p2[0].y - p2[1].y;
	B2 = p2[1].x - p2[0].x;
	C2 = p2[1].y*p2[0].x - p2[1].x*p2[0].y;

	D = B2*A1 - B1*A2;

	p->x = (B1*C2 - B2*C1) / D;
	p->y = (A2*C1 - A1*C2) / D;

	if( bLine)
	{
		if( (p->x - min(p2[0].x, p2[1].x))>0.00001 
			&& (max(p2[0].x, p2[1].x)-p->x)>0.00001 
			&& (p->y - min(p2[0].y, p2[1].y))>0.00001
			&& ( max(p2[0].y, p2[1].y)-p->y)>0.00001
		)
		return true;
	else
		return false;
	}
	else
	{
		if( p->x > min(p1[0].x, p1[1].x)-0.00001 && p->x< max(p1[0].x, p1[1].x)+0.00001 && 
			p->y > min(p1[0].y, p1[1].y)-0.00001 && p->y < max(p1[0].y, p1[1].y)+0.00001 &&
			p->x > min(p2[0].x, p2[1].x)-0.00001 && p->x< max(p2[0].x, p2[1].x)+0.00001 && 
			p->y > min(p2[0].y, p2[1].y)-0.00001 && p->y < max(p2[0].y, p2[1].y)+0.00001
			)
			return true;
		else
			return false;
	}
}

// 线段延长线
// bFlags 为true 向p1方向延长
// 延长长度dLength = d2-d1
CPoint2D LineExtensionEx(const CPoint2D &p1,const CPoint2D &p2,double dLength,bool bFlags)
{
	CPoint2D p0;
	CPoint2D pt = p1;
	p0.x = (p1.x+p2.x)/2.0;
	p0.y = (p1.y+p2.y)/2.0;

	double dDis = GetPointDistance(p1,p2)/2;

	if( bFlags )
	{
		pt.x = (dLength+dDis)/dDis * (p1.x-p0.x) + p0.x;
		pt.y = (dLength+dDis)/dDis * (p1.y-p0.y) + p0.y;
	}
	else
	{
		pt.x = (dLength+dDis)/dDis * (p2.x-p0.x) + p0.x;
		pt.y = (dLength+dDis)/dDis * (p2.y-p0.y) + p0.y;
	}

	return pt;
}

// 线段延长线
// bFlags 为true 向p1方向
// 长度dLength是距离p1或p2的长度
CPoint2D LineExtension(const CPoint2D &p1,const CPoint2D &p2,double dLength,bool bFlags)
{
	CPoint2D pt;

	double dSacle = dLength/GetPointDistance(p1,p2);
	

	if( bFlags )
	{
		pt.x = p2.x + (p1.x-p2.x)*dSacle;
		pt.y = p2.y + (p1.y-p2.y)*dSacle;
	}
	else
	{
		pt.x = p1.x + (p2.x-p1.x)*dSacle;
		pt.y = p1.y + (p2.y-p1.y)*dSacle;
	}

	return pt;
}

void Bresenham(const std::vector<CPoint2D> &points, std::vector<CPoint2D> &addPoints, int num)
{
	double dDist = 0;

	int nSize = points.size();
	for (int i=1; i<nSize; i++)
		dDist += GetPointDistance(points[i-1],points[i]);

	double dLen = dDist / num;

	double dTmp = dLen;
	for (int i=1; i<nSize; i++)
	{
		dDist = GetPointDistance(points[i-1],points[i]);
		CPoint2D oldPt = points[i-1];
loop:	if( dTmp - dDist > 0.0001)
			dTmp -= dDist;
		else
		{
			CPoint2D pt;
			pt.x = (points[i].x-oldPt.x) * dTmp/dDist + oldPt.x;
			pt.y = (points[i].y-oldPt.y) * dTmp/dDist + oldPt.y;

			if( i==nSize-1) // 最后一点
			{
				if( GetPointDistance(pt,points[i]) > 0.0001 )
				{
					TRACE("a\n");
					addPoints.push_back(pt);
				}
			}
			else
			{
				TRACE("a\n");
				addPoints.push_back(pt);
			}


			oldPt = pt;
			dDist = dDist - dTmp;
			dTmp = dLen;
			goto loop;
		}
	}
}

// 直线夹角公式
//  tanα=|(k1-k2)/(1+k1k2)|绝对值
//
// 曲率计算
// K = dα/ds
//
double CalculateCurvarture(const std::vector<CPoint2D> &points)
{
	int nSize = points.size();
	if(nSize<3)
		return 0.0;

	double dDis = 0;
	for (int i=1;i<nSize; i++)
		dDis += GetPointDistance(points[i-1],points[i]);

	CPoint2D p1[2],p2[2],p;
	p1[0] = points[0];
	p1[1] = points[1];
	p2[0] = points[nSize-1];
	p2[1] = points[nSize-2];
	Intersect(p1,p2,&p);

	Angle::AngleCalculator cal;
	// 直线1
	Angle::Line l1(p.x, p.y, p1[1].x,p1[1].y);
	// 直线2
	Angle::Line l2(p.x, p.y, p2[1].x,p2[1].y);

	// 角度
	double dAngle = PI - cal.getDegree(l1,l2);

	return dAngle / dDis;
}

// bool bFlag :
// --true 从头开始找出最大曲率
// --false 从尾开始找出最大曲率
//
double GetMaxCurvarture(const std::vector<CPoint2D> &points, bool bFlag = true)
{
	std::vector<CPoint2D> pts;

	int nSize = points.size();

	if(nSize<3)
		return 0.0;

	if( bFlag)
	{
		pts.push_back(points[0]);
		pts.push_back(points[1]);
		pts.push_back(points[2]);
	}
	else
	{
		pts.push_back(points[nSize-1]);
		pts.push_back(points[nSize-2]);
		pts.push_back(points[nSize-3]);
	}

	double dCurvarture = CalculateCurvarture(pts);
	
	if( bFlag )
	{
		for (int i=3; i<nSize; i++)
		{
			pts.push_back(points[i]);
			double dCur = CalculateCurvarture(pts);
			if( dCurvarture > dCur )
				break;
			else
				dCurvarture = dCur;
		}
	}
	else
	{
		for (int i=nSize-4; i>=0; i--)
		{
			pts.push_back(points[i]);
			double dCur = CalculateCurvarture(pts);
			if( dCurvarture > dCur )
				break;
			else
				dCurvarture = dCur;
		}
	}

	return dCurvarture;
}

//-----------------------------------------------------------------------

CHandTracker::CHandTracker(CGeoMapView *pView)
{
	ASSERT(pView != NULL);
	m_pView = pView;
	m_sizeMin = CSize(1, 1);
}

void CHandTracker::DrawTrackerRect(LPCRECT lpRect, CWnd* pWndClipTo, CDC* pDC, CWnd* pWnd)
{
	// draw nothing
}

void CHandTracker::OnChangedRect(const CRect& rectOld)
{
	CPoint point;
	// call the view to update scroll position
	point.x = m_rect.Width() - rectOld.Width();
	point.y = m_rect.Height() - rectOld.Height();
	m_pView->UpdateScrollPosition(point);
}

//-----------------------------------------------------------------------

BOOL CCharFormat::operator==(CCharFormat& cf)
{
	return
		dwMask == cf.dwMask
		&& dwEffects == cf.dwEffects
		&& yHeight == cf.yHeight
		&& yOffset == cf.yOffset
		&& crTextColor == cf.crTextColor
		&& bPitchAndFamily == cf.bPitchAndFamily
		&& (lstrcmpA(szFaceName, cf.szFaceName) == 0);
}
//-----------------------------------------------------------------------

CFontDesc::CFontDesc(LPCTSTR lpszName, LPCTSTR lpszScript, BYTE nCharSet,
					 BYTE nPitchAndFamily, DWORD dwFlags)
{
	m_strName = lpszName;
	m_strScript = lpszScript;
	m_nCharSet = nCharSet;
	m_nPitchAndFamily = nPitchAndFamily;
	m_dwFlags = dwFlags;
}
//------------------------------------------------------------------------

int compareInterLayer( const void *arg1, const void *arg2 )
{	
	tagPrCorIndex *pS1, *pS2;
	pS1 = (tagPrCorIndex*)arg1;
	pS2 = (tagPrCorIndex*)arg2;
	if( pS1->dDis < pS2->dDis)
		return -1;
	else if( pS1->dDis > pS2->dDis )
		return 1;
	else
		return 0;
}

int compareNormal( const void *arg1, const void *arg2 )
{	
	tagPrNormalIndex *pS1, *pS2;
	pS1 = (tagPrNormalIndex*)arg1;
	pS2 = (tagPrNormalIndex*)arg2;
	if( pS1->dDis < pS2->dDis )
		return -1;
	else if( pS1->dDis > pS2->dDis )
		return 1;
	else
		return 0;
}
//_______________________________________________________________________
//
#define MM_NONE 0

/////////////////////////////////////////////////////////////////////////////
// CGeoMapView

IMPLEMENT_DYNCREATE(CGeoMapView, CScrollView)

CGeoMapView::CGeoMapView()
{
	m_Offset = CPoint(0, 0);
	m_bActive = FALSE;
	m_eOperation = opNone;
	m_hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	m_posUndoZoom = NULL;
	m_posRedoZoom = NULL;

	m_rectObject.left = 10;
	m_rectObject.right = 100;
	m_rectObject.top = 10;
	m_rectObject.bottom= 100;

	m_lStepZoom = 200;
	m_bCapture = FALSE;

	m_bPageSelectAll = FALSE;
	CToolBase::c_drawShape = CToolBase::selection;

	m_bWeight = FALSE;
	m_bItalic = FALSE;
	m_bUnderline = FALSE;
	::SetCursor(m_hCursor);
	m_nCtrlPointIndex = -1;
}

CGeoMapView::~CGeoMapView()
{
	ResetHisZoom();
}

void CGeoMapView::ResetHisZoom()
{
	POSITION pos = m_listUndoHisZoom.GetHeadPosition();
	while (pos != NULL)
	{
		CZoomCmd *pZoomCmd = (CZoomCmd *)m_listUndoHisZoom.GetNext(pos);
		delete pZoomCmd;
	}
	m_listUndoHisZoom.RemoveAll();

	pos = m_listRedoHisZoom.GetHeadPosition();
	while (pos != NULL)
	{
		CZoomCmd *pZoomCmd = (CZoomCmd *)m_listRedoHisZoom.GetNext(pos);
		delete pZoomCmd;
	}
	m_listRedoHisZoom.RemoveAll();

	m_posUndoZoom = NULL;
	m_posRedoZoom = NULL;
}

void CGeoMapView::UpdateScrollPosition(CPoint point)
{	
	CPoint pointCenter = GetLogClientCenter();
	if (m_nMapMode != MM_TEXT)
	{
		CWindowDC dc(NULL);
		dc.SetMapMode(m_nMapMode);
		dc.DPtoLP(&point);
	}
	pointCenter -= point;
	CenterOnPoint(pointCenter);

	CPoint posPt = GetScrollPosition();

	CGeoMapDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->m_pCurDrawPage->m_ptPosition = posPt;
	pDoc->SetModifiedFlag();
}

CPoint CGeoMapView::GetScrollPosition() const
{
	// return the true scroll position (scroll view + offset)
	CPoint point = CScrollView::GetScrollPosition();
	point += m_Offset;
	return point;
}

void CGeoMapView::ScrollToPosition(POINT Point)
{
	CGeoMapDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->m_pCurDrawPage->m_ptPosition = Point;

	CRect rect;
	GetLogClientRect(&rect);
	Point.x += rect.Width() / 2;
	Point.y += rect.Height() / 2;
	// use center on point to scroll to, new offset will be calculated
	CenterOnPoint(Point);
}

CPoint CGeoMapView::GetLogClientCenter() const
{
	// return the true client center (scroll view + offset)
	CRect rect;
	GetLogClientRect(&rect);
	return rect.CenterPoint();
}

void CGeoMapView::GetLogClientRect(CRect *pRect) const
{
	// return the true client rectangle (scroll view + offset)
	GetClientRect(pRect);
	CPoint point = (m_nMapMode != MM_NONE) ? GetDeviceScrollPosition(): CPoint(0, 0);
	pRect->OffsetRect(point.x, point.y);
	if (m_nMapMode != MM_TEXT)
	{
		ASSERT(m_nMapMode > 0); // must be set
		CWindowDC dc(NULL);
		dc.SetMapMode(m_nMapMode);
		dc.DPtoLP(pRect);
	}
	pRect->OffsetRect(m_Offset);
}

void CGeoMapView::CenterOnPoint(CPoint ptCenter)
{
	// save scroll position & offset
	CPoint savePosition = CScrollView::GetScrollPosition();
	CPoint saveOffset = m_Offset;
	
	CRect rect;
	GetClientRect(&rect);
	CPoint Center = rect.CenterPoint();
	if (m_nMapMode != MM_TEXT)
	{
		ASSERT(m_nMapMode > 0); // must be set
		CWindowDC dc(NULL);
		dc.SetMapMode(m_nMapMode);
		dc.DPtoLP(&Center);
	}
	
	// reverse direction
	if (m_nMapMode != MM_TEXT)
	{
		ptCenter.y = -ptCenter.y;
		Center.y = -Center.y;
	}
	
	CPoint Position(ptCenter.x - Center.x, ptCenter.y - Center.y);
	
	m_Offset.x = 0;
	
	m_Offset.y = 0;
	
	// reverse direction
	if (m_nMapMode != MM_TEXT)
	{
		ptCenter.y = -ptCenter.y;
		Position.y = -Position.y;
		m_Offset.y = -m_Offset.y;
	}
	
	// if scroll position change, scroll view
	if (Position != savePosition)
	{
		CScrollView::ScrollToPosition(Position);
	}
	
	// if offset change, scroll window
	if (saveOffset != m_Offset)
	{
		CSize sizeScroll(saveOffset.x - m_Offset.x, saveOffset.y - m_Offset.y);
		if (m_nMapMode != MM_TEXT)
		{
			ASSERT(m_nMapMode > 0); // must be set
			CWindowDC dc(NULL);
			dc.SetMapMode(m_nMapMode);
			dc.LPtoDP(&sizeScroll);
		}
		ScrollWindow(sizeScroll.cx, -sizeScroll.cy);
	}
}

CGeoMapDoc* CGeoMapView::GetDocument()
{ 
	return (CGeoMapDoc*)m_pDocument; 
}

BEGIN_MESSAGE_MAP(CGeoMapView, CScrollView)
	//{{AFX_MSG_MAP(CGeoMapView)
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
	ON_COMMAND(ID_ZOOM_PAGE_CENTER, OnZoomPageCenter)
	ON_COMMAND(ID_ZOOM_ALL, OnZoomAll)
	ON_COMMAND(ID_ZOOM_MOVE, OnZoomMove)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_MOVE, OnUpdateZoomMove)
	ON_COMMAND(ID_ZOOM_RECT, OnZoomRect)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_RECT, OnUpdateZoomRect)
	ON_COMMAND(ID_ZOOM_IN_POINT, OnZoomInPoint)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_IN_POINT, OnUpdateZoomInPoint)
	ON_COMMAND(ID_ZOOM_OUT_POINT, OnZoomOutPoint)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_OUT_POINT, OnUpdateZoomOutPoint)
	ON_COMMAND(ID_ZOOM_IN, OnZoomIn)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_IN, OnUpdateZoomIn)
	ON_COMMAND(ID_ZOOM_OUT, OnZoomOut)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_OUT, OnUpdateZoomOut)
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_CANCEL_EDIT, OnCancelEdit)
	ON_UPDATE_COMMAND_UI(ID_CANCEL_EDIT, OnUpdateCancelEdit)
	ON_WM_MOUSEWHEEL()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_COMMAND(ID_ZOOM_PREV, OnZoomPrev)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_PREV, OnUpdateZoomPrev)
	ON_COMMAND(ID_ZOOM_NEXT, OnZoomNext)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_NEXT, OnUpdateZoomNext)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_ALL, OnUpdateZoomAll)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_FILE_EXPORT, OnFileExport)
	ON_COMMAND(ID_DISP_PAGEPANEL, OnDispPagepanel)
	ON_UPDATE_COMMAND_UI(ID_DISP_PAGEPANEL, OnUpdateDispPagepanel)
	ON_WM_RBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_CHAR()
	ON_COMMAND(ID_DRAW_SELECT, OnDrawSelect)
	ON_UPDATE_COMMAND_UI(ID_DRAW_SELECT, OnUpdateDrawSelect)
	ON_COMMAND(ID_DRAW_BASE_POLYLINE, OnDrawBasePolyline)
	ON_UPDATE_COMMAND_UI(ID_DRAW_BASE_POLYLINE, OnUpdateDrawBasePolyline)
	ON_COMMAND(ID_DRAW_BASE_PLINE, OnDrawBasePline)
	ON_UPDATE_COMMAND_UI(ID_DRAW_BASE_PLINE, OnUpdateDrawBasePline)
	ON_COMMAND(ID_DRAW_BASE_POLYGON, OnDrawBasePolygon)
	ON_UPDATE_COMMAND_UI(ID_DRAW_BASE_POLYGON, OnUpdateDrawBasePolygon)
	ON_COMMAND(ID_DRAW_BASE_PLINEGON, OnDrawBasePlinegon)
	ON_UPDATE_COMMAND_UI(ID_DRAW_BASE_PLINEGON, OnUpdateDrawBasePlinegon)
	ON_COMMAND(ID_DRAW_BASE_POLYGON_FILL, OnDrawBasePolygonFill)
	ON_UPDATE_COMMAND_UI(ID_DRAW_BASE_POLYGON_FILL, OnUpdateDrawBasePolygonFill)
	ON_COMMAND(ID_DRAW_BASE_PLINEGON_FILL, OnDrawBasePlinegonFill)
	ON_UPDATE_COMMAND_UI(ID_DRAW_BASE_PLINEGON_FILL, OnUpdateDrawBasePlinegonFill)
	ON_COMMAND(ID_DRAW_EDIT_CLEAR, OnEditClear)
	ON_UPDATE_COMMAND_UI(ID_DRAW_EDIT_CLEAR, OnUpdateEditClear)
	ON_COMMAND(ID_PLOY_ADD_PT, OnPloyAddPt)
	ON_UPDATE_COMMAND_UI(ID_PLOY_ADD_PT, OnUpdatePloyAddPt)
	ON_COMMAND(ID_POLY_DEL_PT, OnPolyDelPt)
	ON_UPDATE_COMMAND_UI(ID_POLY_DEL_PT, OnUpdatePolyDelPt)
	ON_COMMAND(ID_POLY_MOVE_PT, OnPolyMovePt)
	ON_UPDATE_COMMAND_UI(ID_POLY_MOVE_PT, OnUpdatePolyMovePt)
	ON_COMMAND(ID_POLY_DIVIDE_LINE, OnPolyDivideLine)
	ON_UPDATE_COMMAND_UI(ID_POLY_DIVIDE_LINE, OnUpdatePolyDivideLine)
	ON_COMMAND(ID_POLY_JOIN_LINE, OnPolyJoinLine)
	ON_UPDATE_COMMAND_UI(ID_POLY_JOIN_LINE, OnUpdatePolyJoinLine)
	ON_COMMAND(ID_OBJECT_MOVETOFRONT, OnObjectMovetofront)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_MOVETOFRONT, OnUpdateObjectMovetofront)
	ON_COMMAND(ID_OBJECT_MOVETOBACK, OnObjectMovetoback)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_MOVETOBACK, OnUpdateObjectMovetoback)
	ON_COMMAND(ID_OBJECT_MOVEFORWARD, OnObjectMoveforward)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_MOVEFORWARD, OnUpdateObjectMoveforward)
	ON_COMMAND(ID_OBJECT_MOVEBACK, OnObjectMoveback)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_MOVEBACK, OnUpdateObjectMoveback)
	ON_WM_TIMER()
	ON_COMMAND(ID_DRAW_POLYGON, OnDrawPolygon)
	ON_UPDATE_COMMAND_UI(ID_DRAW_POLYGON, OnUpdateDrawPolygon)
	ON_COMMAND(ID_DRAW_SMOOTH_FILL, OnDrawSmoothFill)
	ON_UPDATE_COMMAND_UI(ID_DRAW_SMOOTH_FILL, OnUpdateDrawSmoothFill)
	ON_COMMAND(ID_DRAW_BASE_RECT_EMPTY, OnDrawBaseRectEmpty)
	ON_UPDATE_COMMAND_UI(ID_DRAW_BASE_RECT_EMPTY, OnUpdateDrawBaseRectEmpty)
	ON_COMMAND(ID_DRAW_BASE_RECT, OnDrawBaseRect)
	ON_UPDATE_COMMAND_UI(ID_DRAW_BASE_RECT, OnUpdateDrawBaseRect)
	ON_COMMAND(ID_DRAW_RECT, OnDrawRect)
	ON_UPDATE_COMMAND_UI(ID_DRAW_RECT, OnUpdateDrawRect)	
	ON_COMMAND(ID_DRAW_SHOW, OnDrawShow)
	ON_UPDATE_COMMAND_UI(ID_DRAW_SHOW, OnUpdateDrawShow)
	ON_COMMAND(ID_DRAW_LOCK, OnDrawLock)
	ON_UPDATE_COMMAND_UI(ID_DRAW_LOCK, OnUpdateDrawLock)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_DRAW_HIDE, OnDrawHide)
	ON_UPDATE_COMMAND_UI(ID_DRAW_HIDE, OnUpdateDrawHide)
	ON_COMMAND(ID_DRAW_NOLOCK, OnDrawNolock)
	ON_UPDATE_COMMAND_UI(ID_DRAW_NOLOCK, OnUpdateDrawNolock)
	ON_COMMAND(ID_DRAW_CAPTURE, OnDrawCapture)
	ON_UPDATE_COMMAND_UI(ID_DRAW_CAPTURE, OnUpdateDrawCapture)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
	ON_COMMAND(ID_ELEMENT_SELECTALL, OnElementSelectall)
	ON_UPDATE_COMMAND_UI(ID_ELEMENT_SELECTALL, OnUpdateElementSelectall)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_OLE_INSERT_NEW, OnOleInsertNew)
	ON_COMMAND(ID_ARRANGE_LEFT, OnArrangeLeft)
	ON_UPDATE_COMMAND_UI(ID_ARRANGE_LEFT, OnUpdateArrangeLeft)
	ON_COMMAND(ID_ARRANGE_RIGHT, OnArrangeRight)
	ON_UPDATE_COMMAND_UI(ID_ARRANGE_RIGHT, OnUpdateArrangeRight)
	ON_COMMAND(ID_ARRANGE_TOP, OnArrangeTop)
	ON_UPDATE_COMMAND_UI(ID_ARRANGE_TOP, OnUpdateArrangeTop)
	ON_COMMAND(ID_ARRANGE_BOTTOM, OnArrangeBottom)
	ON_UPDATE_COMMAND_UI(ID_ARRANGE_BOTTOM, OnUpdateArrangeBottom)
	ON_COMMAND(ID_ARRANGE_CENTER, OnArrangeCenter)
	ON_UPDATE_COMMAND_UI(ID_ARRANGE_CENTER, OnUpdateArrangeCenter)
	ON_COMMAND(ID_ARRANGE_VCENTER, OnArrangeVcenter)
	ON_UPDATE_COMMAND_UI(ID_ARRANGE_VCENTER, OnUpdateArrangeVcenter)
	ON_COMMAND(ID_EVENLY_HORZ, OnEvenlyHorz)
	ON_UPDATE_COMMAND_UI(ID_EVENLY_HORZ, OnUpdateEvenlyHorz)
	ON_COMMAND(ID_EVENLY_VERT, OnEvenlyVert)
	ON_UPDATE_COMMAND_UI(ID_EVENLY_VERT, OnUpdateEvenlyVert)
	ON_COMMAND(ID_ALIGIN_WIDTH, OnAliginWidth)
	ON_UPDATE_COMMAND_UI(ID_ALIGIN_WIDTH, OnUpdateAliginWidth)
	ON_COMMAND(ID_ALIGIN_HEIGHT, OnAliginHeight)
	ON_UPDATE_COMMAND_UI(ID_ALIGIN_HEIGHT, OnUpdateAliginHeight)
	ON_COMMAND(ID_ALIGIN_SIZE, OnAliginSize)
	ON_UPDATE_COMMAND_UI(ID_ALIGIN_SIZE, OnUpdateAliginSize)
	ON_COMMAND(ID_IMPORT_LINE, OnImportLine)
	ON_COMMAND(ID_DRAW_CLIP, OnDrawClip)
	ON_UPDATE_COMMAND_UI(ID_DRAW_CLIP, OnUpdateDrawClip)
	ON_COMMAND(ID_ATTRIBUTE_BRUSH, OnAttributeBrush)
	ON_UPDATE_COMMAND_UI(ID_ATTRIBUTE_BRUSH, OnUpdateAttributeBrush)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_ZOOM_SET_PAGE_CENTER, OnZoomSetPageCenter)
	ON_WM_CREATE()
	ON_COMMAND(IDC_FONTNAME, OnFontname)
	ON_UPDATE_COMMAND_UI(IDC_FONTNAME, OnUpdateFontname)
	ON_COMMAND(IDC_FONTSIZE, OnFontsize)
	ON_UPDATE_COMMAND_UI(IDC_FONTSIZE, OnUpdateFontsize)
	ON_COMMAND(ID_CHAR_BOLD, OnCharBold)
	ON_UPDATE_COMMAND_UI(ID_CHAR_BOLD, OnUpdateCharBold)
	ON_COMMAND(ID_CHAR_ITALIC, OnCharItalic)
	ON_UPDATE_COMMAND_UI(ID_CHAR_ITALIC, OnUpdateCharItalic)
	ON_COMMAND(ID_CHAR_UNDERLINE, OnCharUnderline)
	ON_UPDATE_COMMAND_UI(ID_CHAR_UNDERLINE, OnUpdateCharUnderline)
	ON_COMMAND(ID_CHAR_COLOR, OnCharColor)
	ON_UPDATE_COMMAND_UI(ID_CHAR_COLOR, OnUpdateCharColor)
	ON_COMMAND(ID_CHAR_WIDTH, OnCharWidth)
	ON_UPDATE_COMMAND_UI(ID_CHAR_WIDTH, OnUpdateCharWidth)
	ON_COMMAND(ID_CHAR_NARROW, OnCharNarrow)
	ON_UPDATE_COMMAND_UI(ID_CHAR_NARROW, OnUpdateCharNarrow)
	ON_COMMAND(ID_PARA_LEFT, OnParaLeft)
	ON_UPDATE_COMMAND_UI(ID_PARA_LEFT, OnUpdateParaLeft)
	ON_COMMAND(ID_PARA_CENTER, OnParaCenter)
	ON_UPDATE_COMMAND_UI(ID_PARA_CENTER, OnUpdateParaCenter)
	ON_COMMAND(ID_PARA_RIGHT, OnParaRight)
	ON_UPDATE_COMMAND_UI(ID_PARA_RIGHT, OnUpdateParaRight)
	ON_COMMAND(ID_DRAW_BASE_TEXT, OnDrawBaseText)
	ON_UPDATE_COMMAND_UI(ID_DRAW_BASE_TEXT, OnUpdateDrawBaseText)
	ON_WM_CANCELMODE()
	ON_WM_CAPTURECHANGED()
	ON_UPDATE_COMMAND_UI(ID_ZOOM_SET_PAGE_CENTER, OnUpdateZoomSetPageCenter)
	ON_COMMAND(ID_ZOOM_PAGE_MOVE, OnZoomPageMove)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_PAGE_MOVE, OnUpdateZoomPageMove)
	ON_COMMAND(ID_DRAW_BASE_LINE, OnDrawBaseLine)
	ON_UPDATE_COMMAND_UI(ID_DRAW_BASE_LINE, OnUpdateDrawBaseLine)
	ON_COMMAND(ID_DRAW_ENABLE_MOVE, OnDrawEnableMove)
	ON_UPDATE_COMMAND_UI(ID_DRAW_ENABLE_MOVE, OnUpdateDrawEnableMove)
	ON_COMMAND(ID_DRAW_EDIT_PROPERTIES, OnDrawEditProperties)
	ON_UPDATE_COMMAND_UI(ID_DRAW_EDIT_PROPERTIES, OnUpdateDrawEditProperties)
	ON_COMMAND(ID_DRAW_BASE_PICTURE, OnDrawBasePicture)
	ON_UPDATE_COMMAND_UI(ID_DRAW_BASE_PICTURE, OnUpdateDrawBasePicture)
	ON_COMMAND(ID_DRAW_SCALE_RULER, OnDrawScaleRuler)
	ON_UPDATE_COMMAND_UI(ID_DRAW_SCALE_RULER, OnUpdateDrawScaleRuler)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_MESSAGE(WM_ENDTEXTEDIT, OnEndTextEdit)
	ON_CBN_SELENDOK(IDC_FONTNAME, OnFontname)
	ON_CBN_SELENDOK(IDC_FONTSIZE, OnFontsize)
	ON_WM_CLOSE()
	ON_COMMAND(ID_CTRL_POINT_SET, &CGeoMapView::OnAddCtrlPoint)
	ON_UPDATE_COMMAND_UI(ID_CTRL_POINT_SET, &CGeoMapView::OnUpdateAddCtrlPoint)
	ON_COMMAND(ID_DELETE_CTRL_POINT, &CGeoMapView::OnDeleteCtrlPoint)
	ON_UPDATE_COMMAND_UI(ID_DELETE_CTRL_POINT, &CGeoMapView::OnUpdateDeleteCtrlPoint)
	ON_COMMAND(ID_EDIT_CTRL_POINT, &CGeoMapView::OnEditCtrlPoint)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CTRL_POINT, &CGeoMapView::OnUpdateEditCtrlPoint)	
	ON_COMMAND(ID_EXPORT_CURVE, &CGeoMapView::OnExportCurve)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_CURVE, &CGeoMapView::OnUpdateExportCurve)
	ON_COMMAND(ID_EXPORT_LINE, &CGeoMapView::OnExportLine)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_LINE, &CGeoMapView::OnUpdateExportLine)
	
	ON_COMMAND(ID_CALE_PARAMETER, &CGeoMapView::OnCaleParameter)
	ON_UPDATE_COMMAND_UI(ID_CALE_PARAMETER, &CGeoMapView::OnUpdateCaleParameter)
	
	ON_COMMAND(ID_DRAW_NORMAL, &CGeoMapView::OnDrawNormal)
	ON_UPDATE_COMMAND_UI(ID_DRAW_NORMAL, &CGeoMapView::OnUpdateDrawNormal)
	ON_COMMAND(ID_DRAW_INTERLAYER, &CGeoMapView::OnDrawInterlayer)
	ON_UPDATE_COMMAND_UI(ID_DRAW_INTERLAYER, &CGeoMapView::OnUpdateDrawInterlayer)
	ON_COMMAND(ID_SINGLE_INTERLAYER, &CGeoMapView::OnSingleInterlayer)
	ON_UPDATE_COMMAND_UI(ID_SINGLE_INTERLAYER, &CGeoMapView::OnUpdateSingleInterlayer)
	ON_COMMAND(ID_SAVE_LAYER_MODEL, &CGeoMapView::OnSaveLayerModel)
	ON_UPDATE_COMMAND_UI(ID_SAVE_LAYER_MODEL, &CGeoMapView::OnUpdateSaveLayerModel)
	ON_COMMAND(ID_SAVE_LAYERMODEL_LINE, &CGeoMapView::OnSaveLayermodelLine)
	ON_UPDATE_COMMAND_UI(ID_SAVE_LAYERMODEL_LINE, &CGeoMapView::OnUpdateSaveLayermodelLine)
	ON_COMMAND(ID_CHANGE_DIRECTION, &CGeoMapView::OnChangeDirection)
	ON_COMMAND(ID_CHANGE_GENERAL, &CGeoMapView::OnChangeGeneral)
	ON_UPDATE_COMMAND_UI(ID_CHANGE_GENERAL, &CGeoMapView::OnUpdateChangeGeneral)
	ON_COMMAND(ID_CHANGE_INTERLAYER, &CGeoMapView::OnChangeInterlayer)
	ON_UPDATE_COMMAND_UI(ID_CHANGE_INTERLAYER, &CGeoMapView::OnUpdateChangeInterlayer)
	ON_COMMAND(ID_CHANGE_NORMAL, &CGeoMapView::OnChangeNormal)
	ON_UPDATE_COMMAND_UI(ID_CHANGE_NORMAL, &CGeoMapView::OnUpdateChangeNormal)
	ON_COMMAND(ID_CHANGE_LINE, &CGeoMapView::OnChangeLine)
	ON_UPDATE_COMMAND_UI(ID_CHANGE_LINE, &CGeoMapView::OnUpdateChangeLine)
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGeoMapView drawing

void CGeoMapView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CGeoMapDoc* pDoc = GetDocument();
	//pDoc->m_bAutoDelete = TRUE;
	
	CSize sizeTotal,sizePage(100,100),sizeLine(10,10);
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 32760;
	
	SetScrollSizes(MM_TEXT, sizeTotal,sizePage,sizeLine);	
	
	ScrollToPosition(pDoc->m_pCurDrawPage->m_ptPosition);

	CRect rect(0, 0, 1, 1);
	m_LeftEdit.Create(WS_CHILD  | WS_VISIBLE | ES_MULTILINE | ES_WANTRETURN |ES_LEFT| ES_AUTOVSCROLL | 
		ES_AUTOHSCROLL  , rect, this, 0);
	m_LeftEdit.ShowWindow(SW_HIDE);
	
	m_CenterEdit.Create(WS_CHILD  | WS_VISIBLE | ES_MULTILINE | ES_WANTRETURN |ES_CENTER| ES_AUTOVSCROLL | 
		ES_AUTOHSCROLL  , rect, this, 0);
	m_CenterEdit.ShowWindow(SW_HIDE);
	
	m_RightEdit.Create(WS_CHILD  | WS_VISIBLE | ES_MULTILINE | ES_WANTRETURN |ES_RIGHT| ES_AUTOVSCROLL | 
		ES_AUTOHSCROLL  , rect, this, 0);
	m_RightEdit.ShowWindow(SW_HIDE);
}

void CGeoMapView::OnDraw(CDC* pDC)
{
	CGeoMapDoc* pDoc = GetDocument();
	if( pDoc->m_pCurDrawPage == NULL )
		return;
	
	// TODO: add draw code here
	CDC dc;
	CDC* pDrawDC = pDC;
	CBitmap bitmap;
	CBitmap* pOldBitmap;
	
	// only paint the rect that needs repainting
	CRect client;
		
	int nSaved = pDC->SaveDC();

	pDC->GetClipBox(client);
	
	client.NormalizeRect();

	if( !pDC->IsPrinting() )
	{
		CSize size(8,8);
		pDC->DPtoLP(&size);
		client.DeflateRect(-size.cx,-size.cy,-size.cx,-size.cy); // 消除白线
	}
	
	CRect rect = client;
	
	DocToClient(rect);
	if (!pDC->IsPrinting())
	{
		// draw to offscreen bitmap for fast looking repaints
		if (dc.CreateCompatibleDC(pDC))
		{
			if (bitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height()))
	//		if (bitmap.CreateBitmap(rect.Width(), rect.Height(),1,32,NULL))
			{
				OnPrepareDC(&dc, NULL);
				pDrawDC = &dc;
				
				// offset origin more because bitmap is just piece of the whole drawing
				dc.OffsetViewportOrg(-rect.left, -rect.top);
				pOldBitmap = dc.SelectObject(&bitmap);
				dc.SetBrushOrg(rect.left % 8, rect.top % 8);
				
				// might as well clip to the same rectangle
				dc.IntersectClipRect(client);
			}
		}
	}
	
	// paint background
	CBrush brush;
	brush.CreateSolidBrush(RGB(255, 255, 255));
	
	brush.UnrealizeObject();
	
	if(!pDC->IsPrinting() )
	{
		pDrawDC->FillRect(client, &brush);
		if( pDoc->m_pCurDrawPage != NULL 
			&& pDoc->m_pCurDrawPage->m_bPagePanel )
			DrawPage(pDrawDC);
	}
	
	pDoc->Draw(pDrawDC, this);

	if (pDrawDC != pDC)
	{
		pDC->SetViewportOrg(0, 0);
		pDC->SetWindowOrg(0, 0);
		pDC->SetMapMode(MM_TEXT);
		dc.SetViewportOrg(0, 0);
		dc.SetWindowOrg(0, 0);
		dc.SetMapMode(MM_TEXT);
		pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(),
			&dc, 0, 0, SRCCOPY);
		dc.SelectObject(pOldBitmap);
	}

	pDC->RestoreDC(nSaved);
}

/////////////////////////////////////////////////////////////////////////////
// CGeoMapView diagnostics

#ifdef _DEBUG
void CGeoMapView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CGeoMapView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGeoMapView message handlers

void CGeoMapView::DrawPage(CDC *pDC)
{	
	CGeoMapDoc* pDoc = GetDocument();

	if( pDoc->m_pCurDrawPage == NULL )
		return;

	CRect rect;
	CSize sizePage = pDoc->GetPageSize();
	
	int nSavedDC = pDC->SaveDC();

	sizePage.cx *= pDoc->m_pCurDrawPage->m_nHorzPages;
	sizePage.cy *= pDoc->m_pCurDrawPage->m_nVertPages;
	
	rect.left = -sizePage.cx/2;
	rect.top = -sizePage.cy/2;
	rect.right = rect.left + sizePage.cx;
	rect.bottom = rect.top + sizePage.cy;

	sizePage = pDoc->GetPageSize();

	LOGBRUSH LogBrush;		
	LogBrush.lbColor = RGB(240,200,191);		
	LogBrush.lbStyle = PS_SOLID;		
	CPen pen;
	pen.CreatePen( PS_COSMETIC | PS_ALTERNATE , 1, &LogBrush, 0, NULL );
	CPen *pOldPen = pDC->SelectObject(&pen);
	int i;
	for ( i =0;i < pDoc->m_pCurDrawPage->m_nHorzPages; i++)
	{
		if(i==0) 
		{
			pDC->MoveTo(rect.left, rect.top);
			pDC->LineTo(rect.right, rect.top);

			pDC->MoveTo(rect.left, (i+1) * sizePage.cy + rect.top);
			pDC->LineTo(rect.right, (i+1) * sizePage.cy + rect.top);
		}		
		else
		{
			pDC->MoveTo(rect.left, (i+1) * sizePage.cy + rect.top);
			pDC->LineTo(rect.right, (i+1) * sizePage.cy + rect.top);
		}
	}

	for ( i =0;i < pDoc->m_pCurDrawPage->m_nVertPages; i++)
	{
		if(i==0) 
		{
			pDC->MoveTo(rect.left, rect.top);
			pDC->LineTo(rect.left, rect.bottom);
			pDC->MoveTo(rect.left + sizePage.cx * (i+1), rect.top);
			pDC->LineTo(rect.left + sizePage.cx * (i+1), rect.bottom);
		}
		else
		{
			pDC->MoveTo(rect.left + sizePage.cx * (i+1), rect.top);
			pDC->LineTo(rect.left + sizePage.cx * (i+1), rect.bottom);
		}
	}
	
	pDC->SelectObject(pOldPen);

	CFont font;
	font.CreateFont(0,0,0,0,0,0,0,0,0,0,0,0,0,"Arial");
	CFont *pOldFont = pDC->SelectObject(&font);
	CString str;
	pDC->SetTextColor(RGB(240,200,191));
	for ( i =0;i < pDoc->m_pCurDrawPage->m_nHorzPages; i++)
	{
		for ( int j =0; j < pDoc->m_pCurDrawPage->m_nVertPages; j++)
		{
			str.Format("第%d页",j*pDoc->m_pCurDrawPage->m_nHorzPages+i+1);
			pDC->TextOut(rect.left + sizePage.cx * i + 20,rect.bottom - sizePage.cy * j - 20,str);
		}
	}

	pDC->SelectObject(pOldFont);

	CSize size(3,3);
	
	pDC->DPtoLP(&size);
	CSize border(1,1);
	pDC->DPtoLP(&border);
	
	CRect rectRight;
	rectRight.left = rect.right + border.cx;
	rectRight.bottom = rect.bottom - size.cy + border.cy;
	rectRight.top = rect.top - size.cy;
	rectRight.right = rectRight.left + size.cx;
	CBrush B(RGB(128,128,128));
	pDC->FillRect(rectRight, &B);
	
	CRect rectBottom;
	rectBottom.left = rect.left + size.cx;
	rectBottom.top = rect.top - border.cy;
	rectBottom.right = rect.right + border.cx + size.cx;
	rectBottom.bottom = rectBottom.top - size.cy;
	pDC->FillRect(rectBottom, &B);

	pDC->MoveTo(rect.left, rect.top);
	pDC->LineTo(rect.right, rect.top);
	pDC->LineTo(rect.right, rect.bottom);
	pDC->LineTo(rect.left, rect.bottom);
	pDC->LineTo(rect.left, rect.top);	

	pDC->RestoreDC(nSavedDC);
}


//////////////////////////////////////////////////////////////////////////
// 逻辑坐标到设备坐标的一组互换函数
// 
void CGeoMapView::ClientToDoc(CSize& size)
{
	CClientDC dc(this);
	OnPrepareDC(&dc, NULL);
	dc.DPtoLP(&size);
}

void CGeoMapView::ClientToDoc(CPoint& point)
{
	CClientDC dc(this);
	OnPrepareDC(&dc, NULL);
	dc.DPtoLP(&point);
}

void CGeoMapView::ClientToDoc(CRect& rect)
{
	CClientDC dc(this);
	OnPrepareDC(&dc, NULL);
	dc.DPtoLP(rect);
}

void CGeoMapView::DocToClient(CSize& size)
{
	CClientDC dc(this);
	OnPrepareDC(&dc, NULL);
	dc.LPtoDP(&size);
}


void CGeoMapView::DocToClient(CPoint& point)
{
	CClientDC dc(this);
	OnPrepareDC(&dc, NULL);
	dc.LPtoDP(&point);
}

void CGeoMapView::DocToClient(CRect& rect)
{
	CClientDC dc(this);
	OnPrepareDC(&dc, NULL);
	dc.LPtoDP(rect);
	rect.NormalizeRect();
}

void CGeoMapView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	if( m_nMapMode == MM_NONE )
		return;
	
	CScrollView::OnPrepareDC(pDC, pInfo);
	
	CGeoMapDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	if (pDC->IsPrinting())
	{		
		pDC->SetMapMode(MM_LOMETRIC);
		CSize sizePage = pDoc->GetPageSize();
			
		CSize sizePrint = sizePage;
		sizePrint.cx -= pDoc->m_pCurDrawPage->m_iMarginWidth;
		sizePrint.cy -= pDoc->m_pCurDrawPage->m_iMarginWidth;

		CSize size;
		size.cx = sizePage.cx * pDoc->m_pCurDrawPage->m_nHorzPages;
		size.cy = sizePage.cy * pDoc->m_pCurDrawPage->m_nVertPages;

		size.cx /= 2;
		size.cy /= 2;

		pDC->SetWindowOrg(-size.cx,size.cy);

 		if (pInfo)
 		{
			int hPage = (pInfo->m_nCurPage - 1) % pDoc->m_pCurDrawPage->m_nHorzPages;
			int vPage = (pInfo->m_nCurPage - 1) / pDoc->m_pCurDrawPage->m_nHorzPages;
			
 			pDC->OffsetWindowOrg(
 				hPage * sizePrint.cx, 
 				-sizePrint.cy * vPage
 				);
		}
	}
	else
	{	
		SIZE size;
		size.cx = pDoc->m_pCurDrawPage->m_lZoom;
		size.cy = -pDoc->m_pCurDrawPage->m_lZoom;
		pDC->SetMapMode(MM_ANISOTROPIC);
		pDC->SetWindowExt(5000, 5000);
		pDC->SetViewportExt(size);		
		pDC->OffsetViewportOrg(32760/2,32760/2);
	}
}

BOOL CGeoMapView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	// TODO: call DoPreparePrinting to invoke the Print dialog box
	CGeoMapDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	pInfo->SetMaxPage(pDoc->m_pCurDrawPage->m_nVertPages * pDoc->m_pCurDrawPage->m_nHorzPages);
	
	return DoPreparePrinting(pInfo);
}

void CGeoMapView::OnSize(UINT nType, int cx, int cy) 
{
	CScrollView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here	
}

void CGeoMapView::OnFilePrintPreview() 
{
	AFXPrintPreview (this);
}

// 滚动到页面中心
void CGeoMapView::OnZoomPageCenter() 
{
	CGeoMapDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);	

	if( m_posUndoZoom == NULL 
		|| m_posUndoZoom != m_listUndoHisZoom.GetTailPosition() )
		ResetHisZoom();

	if( m_listUndoHisZoom.GetCount() == 256 )
	{
		POSITION pos = m_listUndoHisZoom.GetHeadPosition();
		CZoomCmd *p = (CZoomCmd *)m_listUndoHisZoom.GetHead();
		delete p;
		m_listUndoHisZoom.RemoveAt(pos);
	}

	CZoomCmd *pZoomCmd = new CZoomCmd;

	pZoomCmd->m_lZoom = pDoc->m_pCurDrawPage->m_lZoom;
	pZoomCmd->m_ptPos = pDoc->m_pCurDrawPage->m_ptPosition;

	m_listUndoHisZoom.AddTail(pZoomCmd);
	m_posUndoZoom = m_listUndoHisZoom.GetTailPosition();

	CRect rect;
	GetClientRect(rect);
	ScrollToPosition(CPoint(32760/2-rect.Width()/2,32760/2-rect.Height()/2));	

	if( m_listRedoHisZoom.GetCount() == 256 )
	{
		POSITION pos = m_listRedoHisZoom.GetHeadPosition();
		CZoomCmd *p = (CZoomCmd *)m_listRedoHisZoom.GetHead();
		delete p;
		m_listRedoHisZoom.RemoveAt(pos);
	}

	pZoomCmd = new CZoomCmd;
	
	pZoomCmd->m_lZoom = pDoc->m_pCurDrawPage->m_lZoom;
	pZoomCmd->m_ptPos = pDoc->m_pCurDrawPage->m_ptPosition;
	
	m_listRedoHisZoom.AddTail(pZoomCmd);
	m_posRedoZoom = m_listRedoHisZoom.GetTailPosition();

	pDoc->SetModifiedFlag();
}

// 显示整个图形
void CGeoMapView::OnZoomAll() 
{
	CGeoMapDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	//////////////////////////////////////////////////////////////////////////
	// 保存历史记录
	if( m_posUndoZoom == NULL 
		|| m_posUndoZoom != m_listUndoHisZoom.GetTailPosition() )
		ResetHisZoom();
	
	if( m_listUndoHisZoom.GetCount() == 256 )
	{
		POSITION pos = m_listUndoHisZoom.GetHeadPosition();
		CZoomCmd *p = (CZoomCmd *)m_listUndoHisZoom.GetHead();
		delete p;
		m_listUndoHisZoom.RemoveAt(pos);
	}
	
	CZoomCmd *pZoomCmd = new CZoomCmd;
	
	pZoomCmd->m_lZoom = pDoc->m_pCurDrawPage->m_lZoom;
	pZoomCmd->m_ptPos = pDoc->m_pCurDrawPage->m_ptPosition;
	
	m_listUndoHisZoom.AddTail(pZoomCmd);
	m_posUndoZoom = m_listUndoHisZoom.GetTailPosition();
   //////////////////////////////////////////////////////////////////////////

	CRect rectPage = pDoc->m_pCurDrawPage->GetPageRect();
	DocToClient(rectPage);
	CRect cliRect;
	GetClientRect(cliRect);
	double dScale = max((double)rectPage.Width()/(double)cliRect.Width(),(double)rectPage.Height()/(double)cliRect.Height());

	pDoc->m_pCurDrawPage->m_lZoom = pDoc->m_pCurDrawPage->m_lZoom/dScale;

	if( pDoc->m_pCurDrawPage->m_lZoom < 100 )
		pDoc->m_pCurDrawPage->m_lZoom = 100;


	Invalidate(FALSE);
	
	rectPage = pDoc->m_pCurDrawPage->GetPageRect();
	DocToClient(rectPage);
	
	CPoint centerPage = rectPage.CenterPoint();	
	CPoint cneterCli = cliRect.CenterPoint();
	UpdateScrollPosition(cneterCli-centerPage);	

	CPoint posPt = GetScrollPosition();
	pDoc->m_pCurDrawPage->m_ptPosition = posPt;

	//////////////////////////////////////////////////////////////////////////
	// 
	if( m_listRedoHisZoom.GetCount() == 256 )
	{
		POSITION pos = m_listRedoHisZoom.GetHeadPosition();
		CZoomCmd *p = (CZoomCmd *)m_listRedoHisZoom.GetHead();
		delete p;
		m_listRedoHisZoom.RemoveAt(pos);
	}
	
	pZoomCmd = new CZoomCmd;
	
	pZoomCmd->m_lZoom = pDoc->m_pCurDrawPage->m_lZoom;
	pZoomCmd->m_ptPos = pDoc->m_pCurDrawPage->m_ptPosition;
	
	m_listRedoHisZoom.AddTail(pZoomCmd);
	m_posRedoZoom = m_listRedoHisZoom.GetTailPosition();
}


void CGeoMapView::OnUpdateZoomAll(CCmdUI* pCmdUI) 
{
	CGeoMapDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	CRect rectPage = pDoc->m_pCurDrawPage->GetPageRect();
	if( rectPage.IsRectEmpty() )
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);	
}

// 平移图形
void CGeoMapView::OnZoomMove() 
{
	HINSTANCE hInstOld = AfxGetResourceHandle();
	m_eOperation = opMoveView;
	m_hCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_HAND_1);
	SetCursor(m_hCursor);
}

void CGeoMapView::OnUpdateZoomMove(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(m_eOperation == opMoveView);
}

//////////////////////////////////////////////////////////////////////////
// 选取显示
void CGeoMapView::OnZoomRect() 
{
	m_eOperation = opZoomRect;
	m_hCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_ZOOM_RECT);
	SetCursor(m_hCursor);
}

void CGeoMapView::OnUpdateZoomRect(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio( m_eOperation == opZoomRect );
}

//////////////////////////////////////////////////////////////////////////
// 中心放大
void CGeoMapView::OnZoomInPoint() 
{
	m_eOperation = opZoomIn;
	m_hCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_ZOOM_IN);
	SetCursor(m_hCursor);
}

void CGeoMapView::OnUpdateZoomInPoint(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio( m_eOperation == opZoomIn );
}

//////////////////////////////////////////////////////////////////////////
// 中心缩小
void CGeoMapView::OnZoomOutPoint() 
{
	m_eOperation = opZoomOut;
	m_hCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_ZOOM_OUT);
	SetCursor(m_hCursor);
}

void CGeoMapView::OnUpdateZoomOutPoint(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio( m_eOperation == opZoomOut );
}

//////////////////////////////////////////////////////////////////////////
// 放大显示
void CGeoMapView::OnZoomIn() 
{	
	CGeoMapDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CRect cliRect;
	GetClientRect(cliRect);
	CPoint local = cliRect.CenterPoint();

	ClientToDoc(local);

	if( m_posUndoZoom == NULL 
		|| m_posUndoZoom != m_listUndoHisZoom.GetTailPosition() )
		ResetHisZoom();
	
	if( m_listUndoHisZoom.GetCount() == 256 )
	{
		POSITION pos = m_listUndoHisZoom.GetHeadPosition();
		CZoomCmd *p = (CZoomCmd *)m_listUndoHisZoom.GetHead();
		delete p;
		m_listUndoHisZoom.RemoveAt(pos);
	}
	
	CZoomCmd *pZoomCmd = new CZoomCmd;
	
	pZoomCmd->m_lZoom = pDoc->m_pCurDrawPage->m_lZoom;
	pZoomCmd->m_ptPos = pDoc->m_pCurDrawPage->m_ptPosition;
	
	m_listUndoHisZoom.AddTail(pZoomCmd);
	m_posUndoZoom = m_listUndoHisZoom.GetTailPosition();

	pDoc->m_pCurDrawPage->m_lZoom += 200;
	if( pDoc->m_pCurDrawPage->m_lZoom > 500000 )
		pDoc->m_pCurDrawPage->m_lZoom = 500000;

	Invalidate(FALSE);
	
	DocToClient(local);
	CPoint pt = cliRect.CenterPoint();
	UpdateScrollPosition(pt-local);

	pDoc->SetModifiedFlag();

	if( m_listRedoHisZoom.GetCount() == 256 )
	{
		POSITION pos = m_listRedoHisZoom.GetHeadPosition();
		CZoomCmd *p = (CZoomCmd *)m_listRedoHisZoom.GetHead();
		delete p;
		m_listRedoHisZoom.RemoveAt(pos);
	}
	
	pZoomCmd = new CZoomCmd;
	
	pZoomCmd->m_lZoom = pDoc->m_pCurDrawPage->m_lZoom;
	pZoomCmd->m_ptPos = pDoc->m_pCurDrawPage->m_ptPosition;
	
	m_listRedoHisZoom.AddTail(pZoomCmd);
	m_posRedoZoom = m_listRedoHisZoom.GetTailPosition();

}

void CGeoMapView::OnUpdateZoomIn(CCmdUI* pCmdUI) 
{
	CGeoMapDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable(pDoc->m_pCurDrawPage->m_lZoom<500000);
}

//////////////////////////////////////////////////////////////////////////
// 缩小显示
void CGeoMapView::OnZoomOut() 
{
	CGeoMapDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CRect cliRect;
	GetClientRect(cliRect);
	CPoint local = cliRect.CenterPoint();
	
	ClientToDoc(local);

	if( m_posUndoZoom == NULL 
		|| m_posUndoZoom != m_listUndoHisZoom.GetTailPosition() )
		ResetHisZoom();
	
	if( m_listUndoHisZoom.GetCount() == 256 )
	{
		POSITION pos = m_listUndoHisZoom.GetHeadPosition();
		CZoomCmd *p = (CZoomCmd *)m_listUndoHisZoom.GetHead();
		delete p;
		m_listUndoHisZoom.RemoveAt(pos);
	}
	
	CZoomCmd *pZoomCmd = new CZoomCmd;
	
	pZoomCmd->m_lZoom = pDoc->m_pCurDrawPage->m_lZoom;
	pZoomCmd->m_ptPos = pDoc->m_pCurDrawPage->m_ptPosition;
	
	m_listUndoHisZoom.AddTail(pZoomCmd);
	m_posUndoZoom = m_listUndoHisZoom.GetTailPosition();

	pDoc->m_pCurDrawPage->m_lZoom -= 200;
	if( pDoc->m_pCurDrawPage->m_lZoom < 100 )
		pDoc->m_pCurDrawPage->m_lZoom = 100;

	Invalidate(FALSE);

	pDoc->SetModifiedFlag();

	DocToClient(local);
	CPoint pt = cliRect.CenterPoint();
	UpdateScrollPosition(pt-local);

	if( m_listRedoHisZoom.GetCount() == 256 )
	{
		POSITION pos = m_listRedoHisZoom.GetHeadPosition();
		CZoomCmd *p = (CZoomCmd *)m_listRedoHisZoom.GetHead();
		delete p;
		m_listRedoHisZoom.RemoveAt(pos);
	}
	
	pZoomCmd = new CZoomCmd;
	
	pZoomCmd->m_lZoom = pDoc->m_pCurDrawPage->m_lZoom;
	pZoomCmd->m_ptPos = pDoc->m_pCurDrawPage->m_ptPosition;
	
	m_listRedoHisZoom.AddTail(pZoomCmd);
	m_posRedoZoom = m_listRedoHisZoom.GetTailPosition();

}

void CGeoMapView::OnUpdateZoomOut(CCmdUI* pCmdUI) 
{
	CGeoMapDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable(pDoc->m_pCurDrawPage->m_lZoom>200);
}

BOOL CGeoMapView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: Add your message handler code here and/or call default
	if (m_eOperation != opNone && nHitTest == HTCLIENT)
	{
		::SetCursor(m_hCursor);
		return TRUE;
	}

	return TRUE;
}

void CGeoMapView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if( !m_bActive )
		return;
	
	CGeoMapDoc* pDoc = GetDocument();

	if( m_eOperation !=  opNone )
	{
		switch(m_eOperation)
		{
		case opEditCtrlPoint:
			{
				m_nCtrlPointIndex = -1;

				CPoint local = point;
				ClientToDoc(local);

				CRect rt;
				CGeoPictrueObj *pPictrue = (CGeoPictrueObj*) m_selection.GetTail();
				CSize size(10, 10);
				ClientToDoc(size);
				int n = pPictrue->m_ptCtrl.GetSize();
				int i=0;
				for ( i=0;i<n;i++)
				{
					rt.SetRect(pPictrue->m_ptCtrl[i].x - size.cx,
						pPictrue->m_ptCtrl[i].y - size.cy,
						pPictrue->m_ptCtrl[i].x + size.cx,
						pPictrue->m_ptCtrl[i].y + size.cy);

					if(rt.PtInRect(local))
						break;
				}

				if( i>=n)
					return;

				m_nCtrlPointIndex = i;
				this->SetCapture();
				m_ptDown = point;

				CClientDC dc(this);

				dc.SetROP2(R2_NOT);

				dc.SelectStockObject( NULL_BRUSH );

				dc.MoveTo(point.x-10,point.y-10);
				dc.LineTo(point.x+10,point.y+10);

				dc.MoveTo(point.x+10,point.y-10);
				dc.LineTo(point.x-10,point.y+10);

				dc.Ellipse(point.x-15,point.y-15,point.x+15,point.y+15);

				return;
			}
			break;
		case opAddCtrlPoint:
			{
				CDlgAddCtrlPoint dlg;
				if (dlg.DoModal() != IDOK )
					break;

				CPoint local = point;
				CPoint2D pt;
				CString strWellName;

				pt.x = dlg.m_x;
				pt.y = dlg.m_y;
				strWellName = dlg.m_strWellName;

				ClientToDoc(local);

				CGeoPictrueObj *pPictrue = (CGeoPictrueObj*) m_selection.GetTail();
				pPictrue->m_ptCtrl.Add(local);			//位图
				pPictrue->m_ptGeoCtrl.Add(pt);		//适量图
				pPictrue->m_vecWellName.Add(strWellName);				

				pPictrue->Invalidate(this);
				pDoc->SetModifiedFlag();

				if( CaleParameter(pPictrue) ) // 计算四参数
				{
					ParamterChanged();
				}
			}
			break;
		case opDelCtrlPoint:
			{
				CPoint local = point;
				ClientToDoc(local);

				CRect rt;
				CGeoPictrueObj *pPictrue = (CGeoPictrueObj*) m_selection.GetTail();
				CSize size(10, 10);
				ClientToDoc(size);
				int n = pPictrue->m_ptCtrl.GetSize();
				for ( int i=n-1;i>=0;i--)
				{
					rt.SetRect(pPictrue->m_ptCtrl[i].x - size.cx,
						pPictrue->m_ptCtrl[i].y - size.cy,
						pPictrue->m_ptCtrl[i].x + size.cx,
						pPictrue->m_ptCtrl[i].y + size.cy);

					if(rt.PtInRect(local))
					{
						pPictrue->m_ptCtrl.RemoveAt(i);
						pPictrue->m_ptGeoCtrl.RemoveAt(i);
						pPictrue->m_vecWellName.RemoveAt(i);
						pPictrue->Invalidate(this);
						pDoc->SetModifiedFlag();

						break;
					}
				}

				if( pPictrue->m_vecWellName.GetSize()<1)
					m_eOperation = opNone;
			}
			break;
		case opMoveView:
			{
				m_hCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_HAND_2);
				SetCursor(m_hCursor);
				CHandTracker trackHand(this);
				trackHand.TrackRubberBand(this, point);
				Invalidate(FALSE);
			}
			break;
		case opZoomOut:
			{
				CPoint local = point;
				ClientToDoc(local);
				
				m_gDownPt = local;

				if( m_posUndoZoom == NULL 
					|| m_posUndoZoom != m_listUndoHisZoom.GetTailPosition() )
					ResetHisZoom();
				
				if( m_listUndoHisZoom.GetCount() == 256 )
				{
					POSITION pos = m_listUndoHisZoom.GetHeadPosition();
					CZoomCmd *p = (CZoomCmd *)m_listUndoHisZoom.GetHead();
					delete p;
					m_listUndoHisZoom.RemoveAt(pos);
				}
				
				CZoomCmd *pZoomCmd = new CZoomCmd;
				
				pZoomCmd->m_lZoom = pDoc->m_pCurDrawPage->m_lZoom;
				pZoomCmd->m_ptPos = pDoc->m_pCurDrawPage->m_ptPosition;
				
				m_listUndoHisZoom.AddTail(pZoomCmd);
				m_posUndoZoom = m_listUndoHisZoom.GetTailPosition();
				
				pDoc->m_pCurDrawPage->m_lZoom -= 200;
				if( pDoc->m_pCurDrawPage->m_lZoom < 100 )
					pDoc->m_pCurDrawPage->m_lZoom = 100;
				
				Invalidate(FALSE);
				
				pDoc->SetModifiedFlag();
				
				DocToClient(local);
				CRect rect;
				GetClientRect(rect);
				CPoint pt = rect.CenterPoint();
				UpdateScrollPosition(pt-local);
				
				CPoint posPt = GetScrollPosition();
				pDoc->m_pCurDrawPage->m_ptPosition = posPt;	
				
				if( m_listRedoHisZoom.GetCount() == 256 )
				{
					POSITION pos = m_listRedoHisZoom.GetHeadPosition();
					CZoomCmd *p = (CZoomCmd *)m_listRedoHisZoom.GetHead();
					delete p;
					m_listRedoHisZoom.RemoveAt(pos);
				}
				
				pZoomCmd = new CZoomCmd;
				
				pZoomCmd->m_lZoom = pDoc->m_pCurDrawPage->m_lZoom;
				pZoomCmd->m_ptPos = pDoc->m_pCurDrawPage->m_ptPosition;
				
				m_listRedoHisZoom.AddTail(pZoomCmd);
				m_posRedoZoom = m_listRedoHisZoom.GetTailPosition();
			}
			break;
		case opZoomIn:
			{
				CPoint local = point;
				ClientToDoc(local);
				
				m_gDownPt = local;

				if( m_posUndoZoom == NULL 
					|| m_posUndoZoom != m_listUndoHisZoom.GetTailPosition() )
					ResetHisZoom();
				
				if( m_listUndoHisZoom.GetCount() == 256 )
				{
					POSITION pos = m_listUndoHisZoom.GetHeadPosition();
					CZoomCmd *p = (CZoomCmd *)m_listUndoHisZoom.GetHead();
					delete p;
					m_listUndoHisZoom.RemoveAt(pos);
				}
				
				CZoomCmd *pZoomCmd = new CZoomCmd;
				
				pZoomCmd->m_lZoom = pDoc->m_pCurDrawPage->m_lZoom;
				pZoomCmd->m_ptPos = pDoc->m_pCurDrawPage->m_ptPosition;
				
				m_listUndoHisZoom.AddTail(pZoomCmd);
				m_posUndoZoom = m_listUndoHisZoom.GetTailPosition();
				
				pDoc->m_pCurDrawPage->m_lZoom += 200;
				
				if( pDoc->m_pCurDrawPage->m_lZoom > 500000 )
					pDoc->m_pCurDrawPage->m_lZoom = 500000;
				
				Invalidate(FALSE);
				
				pDoc->SetModifiedFlag();
				
				DocToClient(local);
				CRect rect;
				GetClientRect(rect);				
				CPoint pt = rect.CenterPoint();
				UpdateScrollPosition(pt-local);

				CPoint posPt = GetScrollPosition();
				pDoc->m_pCurDrawPage->m_ptPosition = posPt;	
				
				if( m_listRedoHisZoom.GetCount() == 256 )
				{
					POSITION pos = m_listRedoHisZoom.GetHeadPosition();
					CZoomCmd *p = (CZoomCmd *)m_listRedoHisZoom.GetHead();
					delete p;
					m_listRedoHisZoom.RemoveAt(pos);
				}
				
				pZoomCmd = new CZoomCmd;
				
				pZoomCmd->m_lZoom = pDoc->m_pCurDrawPage->m_lZoom;
				pZoomCmd->m_ptPos = pDoc->m_pCurDrawPage->m_ptPosition;
				
				m_listRedoHisZoom.AddTail(pZoomCmd);
				m_posRedoZoom = m_listRedoHisZoom.GetTailPosition();
			}
			break;
		case opZoomRect:
			{
				CClientDC dc(this);
				CRect rect(point.x, point.y, point.x, point.y);
				rect.NormalizeRect();
				dc.DrawFocusRect(rect);
				m_ptDown = point;
				m_ptLast = point;
			}
			break;
		case opChalk:
		    break;
		default:
		    break;
		}
		return;
	}

	if( pDoc->m_pCurDrawPage->m_bLocked || pDoc->m_pCurDrawLayer->m_bLocked )
		return;

	CToolBase* pTool = CToolBase::FindTool(CToolBase::c_drawShape);
	if (pTool != NULL)
		pTool->OnLButtonDown(this, nFlags, point);
}

void CGeoMapView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if( !m_bActive )
		return;
	
	CGeoMapDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if( m_eOperation !=  opNone )
	{
		switch(m_eOperation)
		{
		case opEditCtrlPoint:
			{
				if( m_nCtrlPointIndex > -1)//调整控制点
				{
					CPoint local = point;

					ClientToDoc(local);

					ReleaseCapture();
					CClientDC dc(this);

					dc.SetROP2(R2_NOT);

					dc.SelectStockObject( NULL_BRUSH );

					dc.MoveTo(point.x-10,point.y-10);
					dc.LineTo(point.x+10,point.y+10);

					dc.MoveTo(point.x+10,point.y-10);
					dc.LineTo(point.x-10,point.y+10);

					dc.Ellipse(point.x-15,point.y-15,point.x+15,point.y+15);

					CGeoPictrueObj *pPictrue = (CGeoPictrueObj*) m_selection.GetTail();

					pPictrue->m_ptCtrl[m_nCtrlPointIndex].x = local.x; 
					pPictrue->m_ptCtrl[m_nCtrlPointIndex].y = local.y;

					m_nCtrlPointIndex = -1;

					if( CaleParameter(pPictrue) ) // 计算四参数
					{
						ParamterChanged();
					}

					pPictrue->Invalidate(this);
					pDoc->SetModifiedFlag();
					return;
				}				
			}
			break;
		case opZoomIn:
		case opZoomOut:
			{
							
			}
			break;
		case opZoomRect:
			{
				CRect rect(m_ptDown,point);
				rect.NormalizeRect();				
				CRect cliRect;
				GetClientRect(cliRect);
				
				double lScale = max((double)cliRect.Width() / (double)rect.Width(),(double)cliRect.Height() / (double)rect.Height());
				
				ClientToDoc(rect);
				
				if( m_posUndoZoom == NULL || m_posUndoZoom != m_listUndoHisZoom.GetTailPosition() )
					ResetHisZoom();
				
				if( m_listUndoHisZoom.GetCount() == 256 )
				{
					POSITION pos = m_listUndoHisZoom.GetHeadPosition();
					CZoomCmd *p = (CZoomCmd *)m_listUndoHisZoom.GetHead();
					delete p;
					m_listUndoHisZoom.RemoveAt(pos);
				}
				
				CZoomCmd *pZoomCmd = new CZoomCmd;
				
				pZoomCmd->m_lZoom = pDoc->m_pCurDrawPage->m_lZoom;
				pZoomCmd->m_ptPos = pDoc->m_pCurDrawPage->m_ptPosition;
				
				m_listUndoHisZoom.AddTail(pZoomCmd);
				m_posUndoZoom = m_listUndoHisZoom.GetTailPosition();

				pDoc->m_pCurDrawPage->m_lZoom *= lScale;	
				
				if(pDoc->m_pCurDrawPage->m_lZoom < 100 )
					pDoc->m_pCurDrawPage->m_lZoom = 100;
				if( pDoc->m_pCurDrawPage->m_lZoom > 500000 )
					pDoc->m_pCurDrawPage->m_lZoom = 500000;
				
				Invalidate(FALSE);				
				
				DocToClient(rect);
				CPoint centerPt = rect.CenterPoint();
				CPoint cneterCli = cliRect.CenterPoint();

				UpdateScrollPosition(cneterCli-centerPt);

				CPoint posPt = GetScrollPosition();
				pDoc->m_pCurDrawPage->m_ptPosition = posPt;

				pDoc->SetModifiedFlag();

				if( m_listRedoHisZoom.GetCount() == 256 )
				{
					POSITION pos = m_listRedoHisZoom.GetHeadPosition();
					CZoomCmd *p = (CZoomCmd *)m_listRedoHisZoom.GetHead();
					delete p;
					m_listRedoHisZoom.RemoveAt(pos);
				}
				
				pZoomCmd = new CZoomCmd;
				
				pZoomCmd->m_lZoom = pDoc->m_pCurDrawPage->m_lZoom;
				pZoomCmd->m_ptPos = pDoc->m_pCurDrawPage->m_ptPosition;
				
				m_listRedoHisZoom.AddTail(pZoomCmd);
				m_posRedoZoom = m_listRedoHisZoom.GetTailPosition();

			}
			break;
		default:
			break;
		}
		return;
	}
	
	if( pDoc->m_pCurDrawPage->m_bLocked || pDoc->m_pCurDrawLayer->m_bLocked )
		return;

	CToolBase* pTool = CToolBase::FindTool(CToolBase::c_drawShape);
	if (pTool != NULL)
		pTool->OnLButtonUp(this, nFlags, point);
}

void CGeoMapView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if( !m_bActive )
		return;

	CPoint local = point;
	ClientToDoc(local);
	CGeoMapDoc *pDoc = GetDocument();
	CPoint2D pt = pDoc->m_pCurDrawPage->CoordinateTransfer(local);
	SetStatusXY(pt.x,pt.y, "鼠标位置: ");

	if( m_eOperation !=  opNone )
	{
		switch(m_eOperation)
		{
		case opEditCtrlPoint:
			{
				if( nFlags&MK_LBUTTON  && m_nCtrlPointIndex > -1)//调整控制点
				{
					CClientDC dc(this);
					dc.SelectStockObject( NULL_BRUSH );
					dc.SetROP2(R2_NOT);

					dc.MoveTo(m_ptDown.x-10,m_ptDown.y-10);
					dc.LineTo(m_ptDown.x+10,m_ptDown.y+10);

					dc.MoveTo(m_ptDown.x+10,m_ptDown.y-10);
					dc.LineTo(m_ptDown.x-10,m_ptDown.y+10);

					dc.Ellipse(m_ptDown.x-15,m_ptDown.y-15,m_ptDown.x+15,m_ptDown.y+15);

					m_ptDown = point;

					dc.MoveTo(point.x-10,point.y-10);
					dc.LineTo(point.x+10,point.y+10);

					dc.MoveTo(point.x+10,point.y-10);
					dc.LineTo(point.x-10,point.y+10);

					dc.Ellipse(point.x-15,point.y-15,point.x+15,point.y+15);
					return;
				}
				else
				{
					CPoint local = point;
					ClientToDoc(local);

					CRect rt;
					CGeoPictrueObj *pPictrue = (CGeoPictrueObj*) m_selection.GetTail();
					CSize size(10, 10);
					ClientToDoc(size);
					int n = pPictrue->m_ptCtrl.GetSize();
					int i=0;
					for ( i=0;i<n;i++)
					{
						rt.SetRect(pPictrue->m_ptCtrl[i].x - size.cx,
							pPictrue->m_ptCtrl[i].y - size.cy,
							pPictrue->m_ptCtrl[i].x + size.cx,
							pPictrue->m_ptCtrl[i].y + size.cy);

						if(rt.PtInRect(local))
							break;
					}

					if( i>=n)
						return;

					m_nCtrlPointIndex = i;
					pPictrue->Invalidate(this);
				}
			}
			break;
		case opMoveView:
			{
				if( nFlags&MK_LBUTTON)
					m_hCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_HAND_2);
				else
					m_hCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_HAND_1);

				SetCursor(m_hCursor);
			}
			break;
		case opChalk:
			break;
		case opZoomRect:
			if(nFlags & MK_LBUTTON )
			{
				CClientDC dc(this);
				CRect rect(m_ptDown.x, m_ptDown.y, m_ptLast.x, m_ptLast.y);
				rect.NormalizeRect();
				dc.DrawFocusRect(rect);
				rect.SetRect(m_ptDown.x, m_ptDown.y, point.x, point.y);
				rect.NormalizeRect();
				dc.DrawFocusRect(rect);
				m_ptLast = point;
				
			}
			break;
		default:
			break;
		}

		return;
	}

	if( pDoc->m_pCurDrawPage->m_bLocked || pDoc->m_pCurDrawLayer->m_bLocked )
		return;

	CToolBase* pTool = CToolBase::FindTool(CToolBase::c_drawShape);
	if (pTool != NULL)
		pTool->OnMouseMove(this, nFlags, point);
}

void CGeoMapView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if (!m_bActive)
		return;
	
	CGeoMapDoc* pDoc = GetDocument();

	if( m_eOperation != opNone )
	{	
		switch(m_eOperation)
		{
		case opEditCtrlPoint:
			{		
				CPoint local = point;
				ClientToDoc(local);

				CRect rt;
				CGeoPictrueObj *pPictrue = (CGeoPictrueObj*) m_selection.GetTail();
				CSize size(10, 10);
				ClientToDoc(size);
				int n = pPictrue->m_ptCtrl.GetSize();
				int i=0;
				for ( i=0;i<n;i++)
				{
					rt.SetRect(pPictrue->m_ptCtrl[i].x - size.cx,
						pPictrue->m_ptCtrl[i].y - size.cy,
						pPictrue->m_ptCtrl[i].x + size.cx,
						pPictrue->m_ptCtrl[i].y + size.cy);

					if(rt.PtInRect(local))
						break;
				}

				if( i<n)
				{
					CDlgAddCtrlPoint dlg;
					dlg.m_x = pPictrue->m_ptGeoCtrl[i].x;
					dlg.m_y = pPictrue->m_ptGeoCtrl[i].y;
					dlg.m_strWellName = pPictrue->m_vecWellName[i];

					if( dlg.DoModal() == IDOK )
					{
						pPictrue->m_ptGeoCtrl[i].x = dlg.m_x;
						pPictrue->m_ptGeoCtrl[i].y = dlg.m_y;
						pPictrue->m_vecWellName[i] = dlg.m_strWellName;

						pPictrue->Invalidate(this);
						pDoc->SetModifiedFlag();

						if( CaleParameter(pPictrue) ) // 计算四参数
						{
							ParamterChanged();
						}
					}
				}
			}
			break;
		default:
			break;
		}
		return;
	}
	
	if( pDoc->m_pCurDrawPage->m_bLocked || pDoc->m_pCurDrawLayer->m_bLocked )
		return;

	CToolBase* pTool = CToolBase::FindTool(CToolBase::c_drawShape);
	if (pTool != NULL)
		pTool->OnLButtonDblClk(this, nFlags, point);
}

void CGeoMapView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	// TODO: Add your specialized code here and/or call the base class
	if( bActivate )
	{
		CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
		pFrame->ActiveView(pActivateView);
	}
	else
	{
		CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
		pFrame->DeactiveView(pActivateView);
	}

	CScrollView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	if (m_bActive != bActivate)
	{
		if (bActivate)  // if becoming active update as if active
			m_bActive = bActivate;
		if (!m_selection.IsEmpty())
			OnUpdate(NULL, HINT_UPDATE_SELECTION, NULL);
		m_bActive = bActivate;
	}

	CGeoMapDoc* pDoc = GetDocument();
	if( pDoc->m_pCurDrawPage->m_ptPosition.x == -1 
		&& pDoc->m_pCurDrawPage->m_ptPosition.y == -1)
	{
		CRect rect;
		GetClientRect(rect);
		CSize sizePage = pDoc->GetPageSize();
		DocToClient(sizePage);
		ScrollToPosition(CPoint(32760/2-rect.Width()/2,32760/2-sizePage.cy/2-8));
	}
}

void CGeoMapView::OnCancelEdit() 
{
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL)
	{
		// if we found one, deactivate it
		pActiveItem->Close();
	}
	
	ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
	
	ReleaseCapture();

	// escape also brings us back into select mode
	if( m_eOperation != opNone )
		m_eOperation = opNone;
	else
	{
		CToolBase* pTool = CToolBase::FindTool(CToolBase::c_drawShape);
		if (pTool != NULL)
			pTool->OnCancel(this);
	}	
}

void CGeoMapView::OnUpdateCancelEdit(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}

BOOL CGeoMapView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{	
	BOOL ret = TRUE;
	//if( m_eOperation == opMoveView )
	{
		CGeoMapDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		
		CRect cliRect;
		GetClientRect(cliRect);
		CPoint local = cliRect.CenterPoint();
		
		ClientToDoc(local);
		
		if( zDelta < 0 )
			pDoc->m_pCurDrawPage->m_lZoom += m_lStepZoom;
		else
			pDoc->m_pCurDrawPage->m_lZoom -= m_lStepZoom;

		if( pDoc->m_pCurDrawPage->m_lZoom > 500000 )
			pDoc->m_pCurDrawPage->m_lZoom = 500000;
		if( pDoc->m_pCurDrawPage->m_lZoom < 100 )
			pDoc->m_pCurDrawPage->m_lZoom = 100;
		
		Invalidate(FALSE);
		DocToClient(local);
		CPoint pt = cliRect.CenterPoint();
		UpdateScrollPosition(pt-local);
		
		pDoc->SetModifiedFlag();
	}
	/*else
	{
		ret = CScrollView::OnMouseWheel(nFlags, zDelta, pt);
		
		CPoint posPt = GetScrollPosition();
		
		CGeoMapDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pDoc->m_pCurDrawPage->m_ptPosition = posPt;
		pDoc->SetModifiedFlag();
	}	
*/
	return ret;
}

void CGeoMapView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	
	CScrollView::OnHScroll(nSBCode, nPos, pScrollBar);

	CPoint posPt = GetScrollPosition();
	
	CGeoMapDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->m_pCurDrawPage->m_ptPosition = posPt;
	pDoc->SetModifiedFlag();
}

void CGeoMapView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	
	CScrollView::OnVScroll(nSBCode, nPos, pScrollBar);

	CPoint posPt = GetScrollPosition();
	
	CGeoMapDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->m_pCurDrawPage->m_ptPosition = posPt;
	pDoc->SetModifiedFlag();
}

void CGeoMapView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch (lHint)
	{
	case HINT_UPDATE_WINDOW:    // 重画所有窗口
		Invalidate(FALSE);
		break;
		
	case HINT_UPDATE_DRAWOBJ:   // 某图素发生改变（包括状态、大小、位置)
		InvalObj((CGeoDrawObj*)pHint);
		break;
		
	case HINT_UPDATE_SELECTION: // 所有选中的图素发生改变（包括状态、大小、位置)
		{
			CGeoDrawObjList* pList = pHint != NULL ?
				(CGeoDrawObjList*)pHint : &m_selection;
			POSITION pos = pList->GetHeadPosition();
			while (pos != NULL)
				InvalObj(pList->GetNext(pos));
		}
		break;
	case HINT_UPDATE_OLE_ITEMS:
		{
 			CGeoMapDoc* pDoc = GetDocument();
 			POSITION pos = pDoc->m_pCurDrawLayer->m_DrawObjects.GetHeadPosition();
 			while (pos != NULL)
 			{
 				CGeoDrawObj* pObj = pDoc->m_pCurDrawLayer->m_DrawObjects.GetNext(pos);
 				if (pObj->IsKindOf(RUNTIME_CLASS(CGeoDrawOleObj)))
 					InvalObj(pObj);
 			}
		}
		break;
		
	case HINT_DELETE_SELECTION: // 移走所有选中的图素
 		if (pHint != &m_selection)
 		{
 			CGeoDrawObjList* pList = (CGeoDrawObjList*)pHint;
 			POSITION pos = pList->GetHeadPosition();
 			while (pos != NULL)
 			{
 				CGeoDrawObj* pObj = pList->GetNext(pos);
 				InvalObj(pObj);
 				Remove(pObj);   // 从选中图素集中移走
 			}
 		}
		break;
	case HINT_UPDATE_LAYERBAR:
		{
			CGeoMapDoc* pDoc = GetDocument();
				
			CDrawLayer* pLayer = pHint != NULL ?
				(CDrawLayer*)pHint : pDoc->m_pCurDrawLayer;
			pLayer->Invalidate(this);
			
			CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
			pFrame->GetLayerToolbar()->Invalidate();
		}
		break;
	case HINT_UPDATE_PAGE:
		{
			ResetHisZoom();
			Invalidate(FALSE);
			CGeoMapDoc* pDoc = GetDocument();
			if( pDoc->m_pCurDrawPage->m_ptPosition.x == -1 
				&& pDoc->m_pCurDrawPage->m_ptPosition.y == -1)
			{
				CRect rect;
				GetClientRect(rect);
				CSize sizePage = pDoc->GetPageSize();
				DocToClient(sizePage);
				ScrollToPosition(CPoint(32760/2-rect.Width()/2,32760/2-sizePage.cy/2-8));
			}
			else
				ScrollToPosition(pDoc->m_pCurDrawPage->m_ptPosition);
			
			Invalidate(TRUE);
		}
		break;
		
	default:
		
		break;
	}	
}

void CGeoMapView::OnZoomPrev() 
{
	if (m_posUndoZoom != NULL)
	{
		CZoomCmd *pZoomCmd = (CZoomCmd *)m_listUndoHisZoom.GetPrev(m_posUndoZoom);
		CGeoMapDoc* pDoc = GetDocument();
		pDoc->m_pCurDrawPage->m_lZoom = pZoomCmd->m_lZoom;
		pDoc->m_pCurDrawPage->m_ptPosition = pZoomCmd->m_ptPos;
		ScrollToPosition(pZoomCmd->m_ptPos);
		Invalidate(FALSE);
		m_listRedoHisZoom.GetPrev(m_posRedoZoom);
	}
}

void CGeoMapView::OnUpdateZoomPrev(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(
		m_listUndoHisZoom.GetCount()>0
		&& m_posUndoZoom != NULL);
}

void CGeoMapView::OnZoomNext()
{
	if (m_posRedoZoom == NULL)
	{
		m_posRedoZoom = m_listRedoHisZoom.GetHeadPosition();
		CZoomCmd *pZoomCmd = (CZoomCmd *)m_listRedoHisZoom.GetHead();
		CGeoMapDoc* pDoc = GetDocument();
		pDoc->m_pCurDrawPage->m_lZoom = pZoomCmd->m_lZoom;
		pDoc->m_pCurDrawPage->m_ptPosition = pZoomCmd->m_ptPos;
		ScrollToPosition(pZoomCmd->m_ptPos);
		Invalidate(FALSE);
		m_posUndoZoom = m_listUndoHisZoom.GetHeadPosition();
	}
	else
	{
		m_listUndoHisZoom.GetNext(m_posUndoZoom);
		m_listRedoHisZoom.GetNext(m_posRedoZoom);
		CZoomCmd *pZoomCmd = (CZoomCmd *) m_listRedoHisZoom.GetAt(m_posRedoZoom);
		CGeoMapDoc* pDoc = GetDocument();
		pDoc->m_pCurDrawPage->m_lZoom = pZoomCmd->m_lZoom;
		pDoc->m_pCurDrawPage->m_ptPosition = pZoomCmd->m_ptPos;
		ScrollToPosition(pZoomCmd->m_ptPos);
		Invalidate(FALSE);				
	}
}

void CGeoMapView::OnUpdateZoomNext(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(
		m_listRedoHisZoom.GetCount()>0
		&& m_posRedoZoom != m_listRedoHisZoom.GetTailPosition());
}

//功能:从选中列表中删除一个图素
//参数:
//返回:
void CGeoMapView::Remove(CGeoDrawObj *pObj)
{
	POSITION pos = m_selection.Find(pObj);
	if (pos != NULL)
		m_selection.RemoveAt(pos);
}


//功能:判断图素是否在选中列表中
//参数:
//返回:
BOOL CGeoMapView::IsSelected(const CObject* pDocItem) const
{
	CGeoDrawObj* pDrawObj = (CGeoDrawObj*)pDocItem;
	if (pDocItem->IsKindOf(RUNTIME_CLASS(CGeoDrawItem)))
		pDrawObj = (CGeoDrawObj* )((CGeoDrawItem*)pDocItem)->m_pDrawObj;
	return m_selection.Find(pDrawObj) != NULL;	
}

//功能:把图素加入到选中列表中
//参数:如果 pObj == NULL则清空列表, 如果bAdd = FALSE清空列表后加入图素,否则加入图素
//返回:
void CGeoMapView::Select(CGeoDrawObj *pObj, BOOL bAdd)
{
	if (!bAdd)
	{
		OnUpdate(NULL, HINT_UPDATE_SELECTION, NULL);

		POSITION pos = m_selection.GetHeadPosition();
		while (pos != NULL)//清空时使每个图素处于非编辑状态
		{
			CGeoDrawObj* pObj = m_selection.GetNext(pos);
			pObj->m_trackerState = CGeoDrawObj::normal;	
		}			
		m_selection.RemoveAll();

		UpdateFormatBar();
	}

	if( pObj != NULL )
		SetStatusCenter(pObj->m_geoCenterPoint.x, pObj->m_geoCenterPoint.y, "图素中心: ");
	else
		SetStatusCenter(0, 0, "");

	if (pObj == NULL || IsSelected(pObj))
		return;
	
	m_selection.AddTail(pObj);
	pObj->m_trackerState = CGeoDrawObj::selected;
	SetSelectActive(pObj);
	InvalObj(pObj);

	if( pObj->IsKindOf(RUNTIME_CLASS(CGeoTextObj)) )
		UpdateFormatBar();
}

//功能:刷新图素
//参数:
//返回:

void CGeoMapView::InvalObj(CGeoDrawObj *pObj)
{
	CRect rect = pObj->m_rtPosition; // 逻辑坐标
	DocToClient(rect); // 转换为设备坐标
	if (m_bActive && IsSelected(pObj))
	{
		rect.left -= 4;
		rect.top -= 5;
		rect.right += 5;
		rect.bottom += 4;
	}
	rect.InflateRect(1, 1); // handles CDrawOleObj objects
	
	InvalidateRect(rect, FALSE);
}

//功能:刷新图层
//参数:
//返回:

void CGeoMapView::InvalLayer(CDrawLayer* pLayer)
{
	CRect rect = pLayer->GetLayerRect(); // 逻辑坐标
	DocToClient(rect); // 转换为设备坐标
	if (m_bActive )
	{
		rect.left -= 4;
		rect.top -= 5;
		rect.right += 5;
		rect.bottom += 4;
	}
	rect.InflateRect(1, 1); // handles CDrawOleObj objects
	
	InvalidateRect(rect, FALSE);
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoMapView::CloneSelection
// 返回  : void
// 说明  : 克隆选择对象收集器中的对象
//
void CGeoMapView::CloneSelection()
{
 	CGeoMapDoc *pDocument = GetDocument();
 	POSITION pos = m_selection.GetHeadPosition();
 	while (pos != NULL)
 	{
 		CGeoDrawObj* pObj = m_selection.GetNext(pos);
 		pObj->Clone(pDocument->m_pCurDrawLayer);
 	}
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoMapView::SelectWithinRect
// 返回  : void
// 参数  : CRect rect
// 参数  : BOOL bAdd FALSE-- 移去原所有选中的图素后添加该图素到图素集中 
//					 TRUE -- 不移去原所有选中的图素并添加该图素到图素集中
//					缺省值是FALSE
// 说明  : 圈选图素
//
void CGeoMapView::SelectWithinRect(CRect rect, BOOL bAdd)
{
	if (!bAdd)
		Select(NULL);
	
	if( rect.IsRectEmpty() )
		return;

	this->ClientToDoc(rect);

	CGeoMapDoc *pDocument = GetDocument();

	CGeoDrawObjList* pObList = &(pDocument->m_pCurDrawLayer->m_DrawObjects);
	POSITION posObj = pObList->GetHeadPosition();
	while (posObj != NULL)
	{
		CGeoDrawObj* pObj = pObList->GetNext(posObj);
		if (pObj->Intersects(rect))
		{
			Select(pObj, TRUE);
		}
	}
}


//功能:取消选中图素
//参数:
//返回:
void CGeoMapView::DeSelect(CGeoDrawObj *pObj)
{
	POSITION pos = m_selection.Find(pObj);
	if (pos != NULL)
	{
		InvalObj(pObj);
		m_selection.RemoveAt(pos);
	}
}


//功能:
//参数:
//返回:
void CGeoMapView::UpdateActiveItem()
{
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL &&
		pActiveItem->GetItemState() == COleClientItem::activeUIState)
	{
		// this will update the item rectangles by calling
		//  OnGetPosRect & OnGetClipRect.
		pActiveItem->SetItemRects();
	}
}

BOOL CGeoMapView::OnScrollBy(CSize sizeScroll, BOOL bDoScroll) 
{
	if (!CScrollView::OnScrollBy(sizeScroll, bDoScroll))
		return FALSE;
	
	// update the position of any in-place active item
	if (bDoScroll)
	{
		UpdateActiveItem();
		UpdateWindow();
	}
	return TRUE;
}

//功能:当鼠标第一次进入下放目标窗口的非滚动区域时，框架就调用这个函数。
//参数:
//返回:
DROPEFFECT CGeoMapView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	ASSERT(m_prevDropEffect == DROPEFFECT_NONE);
	m_bDragDataAcceptable = FALSE;
	if (!COleClientItem::CanCreateFromData(pDataObject))
		return DROPEFFECT_NONE;

	m_bDragDataAcceptable = TRUE;
	GetObjectInfo(pDataObject, &m_dragSize, &m_dragOffset);
	CClientDC dc(NULL);
	dc.HIMETRICtoDP(&m_dragSize);
	dc.HIMETRICtoDP(&m_dragOffset);

	return OnDragOver(pDataObject, dwKeyState, point);
}

//功能:当鼠标从下放目标窗口上方经过时，框架窗口调用这个函数
//参数:
//返回:
DROPEFFECT CGeoMapView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	if(m_bDragDataAcceptable == FALSE)
		return DROPEFFECT_NONE;

	point -= m_dragOffset;  // adjust target rect by original cursor offset

	// check for point outside logical area -- i.e. in hatched region
	// GetTotalSize() returns the size passed to SetScrollSizes
	CRect rectScroll(CPoint(0, 0), GetTotalSize());

	CRect rectItem(point,m_dragSize);
	rectItem.OffsetRect(GetDeviceScrollPosition());

	DROPEFFECT de = DROPEFFECT_NONE;
	CRect rectTemp;
	if (rectTemp.IntersectRect(rectScroll, rectItem))
	{
		// check for force link
		if ((dwKeyState & (MK_CONTROL|MK_SHIFT)) == (MK_CONTROL|MK_SHIFT))
			de = DROPEFFECT_NONE; // DRAWCLI isn't a linking container
		// check for force copy
		else if ((dwKeyState & MK_CONTROL) == MK_CONTROL)
			de = DROPEFFECT_COPY;
		// check for force move
		else if ((dwKeyState & MK_ALT) == MK_ALT)
			de = DROPEFFECT_MOVE;
		// default -- recommended action is move
		else
			de = DROPEFFECT_MOVE;
	}

	if (point == m_dragPoint)
		return de;

	// otherwise, cursor has moved -- need to update the drag feedback
	CClientDC dc(this);
	if (m_prevDropEffect != DROPEFFECT_NONE)
	{
		// erase previous focus rect
		dc.DrawFocusRect(CRect(m_dragPoint, m_dragSize));
	}
	m_prevDropEffect = de;
	if (m_prevDropEffect != DROPEFFECT_NONE)
	{
		m_dragPoint = point;
		dc.DrawFocusRect(CRect(point, m_dragSize));
	}
	return de;
}

//功能:当用户在有效的下放目标上方放开一个数据对象时，框架调用这个函数。
//参数:
//	pDataObject  指向将要被放入下放目标的COleDataObject对象。
//			可以通过pDataObject来检查数据对象的剪贴板数据格式和指定点的下放数据。
//  dropEffect   用户要求的下放效果。
//		1.DROPEFFECT_COPY  创建被下放的数据对象的一个拷贝。
//		2.DROPEFFECT_MOVE  将数据对象移动到当前鼠标位置。
//		3.DROPEFFECT_LINK  在数据对象和它的服务器之间创建连接。
//返回: 如果成功地下放，则返回非零值，否则返回0。
BOOL CGeoMapView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
	ASSERT_VALID(this);

	// clean up focus rect
	OnDragLeave();

	// offset point as appropriate for dragging
	GetObjectInfo(pDataObject, &m_dragSize, &m_dragOffset);
	CClientDC dc(NULL);
	dc.HIMETRICtoDP(&m_dragSize);
	dc.HIMETRICtoDP(&m_dragOffset);
	point -= m_dragOffset;

	// invalidate current selection since it will be deselected
	OnUpdate(NULL, HINT_UPDATE_SELECTION, NULL);
	m_selection.RemoveAll();
	if (m_bDragDataAcceptable)
		PasteEmbedded(*pDataObject, point);

	GetDocument()->UpdateAllViews(NULL, 0, NULL);      // including this view

	return TRUE;
}


//功能:当鼠标移出窗口的有效下放位置时，框架调用这个函数。
//参数:
//返回:
void CGeoMapView::OnDragLeave() 
{
	CClientDC dc(this);
	if (m_prevDropEffect != DROPEFFECT_NONE)
	{
		dc.DrawFocusRect(CRect(m_dragPoint,m_dragSize)); // erase previous focus rect
		m_prevDropEffect = DROPEFFECT_NONE;
	}
}

//功能:获取对象信息
//参数:
//返回:
BOOL CGeoMapView::GetObjectInfo(COleDataObject *pDataObject, CSize *pSize, CSize *pOffset)
{
	ASSERT(pSize != NULL);

	// get object descriptor data
	HGLOBAL hObjDesc = pDataObject->GetGlobalData(m_cfObjectDescriptor);
	if (hObjDesc == NULL)
	{
		if (pOffset != NULL)
			*pOffset = CSize(0, 0); // fill in defaults instead
		*pSize = CSize(0, 0);
		return FALSE;
	}
	ASSERT(hObjDesc != NULL);

	// otherwise, got CF_OBJECTDESCRIPTOR ok.  Lock it down and extract size.
	LPOBJECTDESCRIPTOR pObjDesc = (LPOBJECTDESCRIPTOR)GlobalLock(hObjDesc);
	ASSERT(pObjDesc != NULL);
	pSize->cx = (int)pObjDesc->sizel.cx;
	pSize->cy = (int)pObjDesc->sizel.cy;
	if (pOffset != NULL)
	{
		pOffset->cx = (int)pObjDesc->pointl.x;
		pOffset->cy = (int)pObjDesc->pointl.y;
	}
	GlobalUnlock(hObjDesc);
	GlobalFree(hObjDesc);

	// successfully retrieved pSize & pOffset info
	return TRUE;
}

//功能:粘贴对象
//参数:
//返回:
void CGeoMapView::PasteEmbedded(COleDataObject &dataObject, CPoint point)
{
	BeginWaitCursor();
	
	CGeoMapDoc *pDoc = GetDocument();

	// paste embedded
	CGeoDrawOleObj* pObj = new CGeoDrawOleObj(GetInitialPosition(), pDoc->m_pCurDrawLayer);
	ASSERT_VALID(pObj);
	CGeoDrawItem* pItem = new CGeoDrawItem(GetDocument(), pObj);
	ASSERT_VALID(pItem);
	pObj->m_pClientItem = pItem;
	
	TRY
	{
		if (!pItem->CreateFromData(&dataObject) &&
			!pItem->CreateStaticFromData(&dataObject))
		{
			AfxThrowMemoryException();      // any exception will do
		}
		
		// add the object to the document
		GetDocument()->Add(pObj);
		m_selection.AddTail(pObj);

		pObj->MoveTo( CRect( point, pObj->m_extent ), this );
		
		// try to get initial presentation data
		pItem->UpdateLink();
		pItem->UpdateExtent();
	}
	CATCH_ALL(e)
	{
		// clean up item
		pItem->Delete();
		pObj->m_pClientItem = NULL;
		GetDocument()->Remove(pObj);
		pObj->Remove();
		
		AfxMessageBox(_T("建立对象失败。请确认该对象已被注册入系统。"));
	}
	END_CATCH_ALL
		
	EndWaitCursor();
}


//功能:对象初始化位置
//参数:
//返回:
CRect CGeoMapView::GetInitialPosition()
{
	CRect rect = m_rectObject;	
	rect.NormalizeRect();
	ClientToDoc(rect);
	return rect;
}

//////////////////////////////////////////////////////////////////////////
// 复制
void CGeoMapView::OnEditCopy() 
{
	if( m_selection.IsEmpty() ) //是空的
	{
		CMetaFileDC file;
		CClientDC dc(this);
		dc.m_bPrinting = TRUE;
		dc.SetMapMode(MM_LOMETRIC);
		
		file.CreateEnhanced(&dc, NULL, NULL , "ImageName\0Copy\0\0");
		file.SetAttribDC(dc.m_hDC);
		file.SetMapMode(MM_LOMETRIC);
		file.m_bPrinting = TRUE;
		
		CGeoMapDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		
		pDoc->Draw(&file, this);
		
		HENHMETAFILE hemf = file.CloseEnhanced();
		OpenClipboard();
		EmptyClipboard();
		::SetClipboardData(CF_ENHMETAFILE,hemf);
		CloseClipboard();
		file.ReleaseAttribDC();
		
		::DeleteEnhMetaFile(hemf);
		
		return;
	}
	else
	{
		ASSERT_VALID(this);
		ASSERT(m_cfDraw_E != NULL);
		
		// Create a shared file and associate a CArchive with it
		CSharedFile file;
		CArchive ar(&file, CArchive::store);
		
		// Serialize selected objects to the archive
		m_selection.Serialize(ar);
		ar.Close();
		
		COleDataSource* pDataSource = NULL;
		TRY
		{
			pDataSource = new COleDataSource;
			// put on local format instead of or in addation to
			pDataSource->CacheGlobalData(m_cfDraw_E, file.Detach());
			
			// if only one item and it is a COleClientItem then also
			// paste in that format
			CGeoDrawObj* pDrawObj = m_selection.GetHead();
			if (m_selection.GetCount() == 1 &&
				pDrawObj->IsKindOf(RUNTIME_CLASS(CGeoDrawOleObj)))
			{
				CGeoDrawOleObj* pDrawOle = (CGeoDrawOleObj*)pDrawObj;
				pDrawOle->m_pClientItem->GetClipboardData(pDataSource, FALSE);
			}
			pDataSource->SetClipboard();
		}
		CATCH_ALL(e)
		{
			delete pDataSource;
			THROW_LAST();
		}
		END_CATCH_ALL
	}
}

//////////////////////////////////////////////////////////////////////////
// 导出图像
void CGeoMapView::OnFileExport() 
{	
	CGeoMapDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CString strFileName;
	
	CString strTitle = pDoc->GetTitle();
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	
	_splitpath_s( strTitle, drive, dir, fname, ext );

	CDlgExportPicture dlg(FALSE,"WMF",fname,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_ENABLETEMPLATE);
	//------用来检测目前的操作系统的版本信息	
	
	dlg.m_ofn.lpstrTitle = _T("导出图像");
	CString strFilter = GetFileTypes(FALSE);
	dlg.m_ofn.lpstrFilter = strFilter;
	if (dlg.DoModal() != IDOK)
	{
		return;
	}

	strFileName = dlg.GetPathName();
	
	int iQuality = dlg.m_iQuality; // 输出质量
	int iExport = dlg.m_iExportType;

	CString strExt = FindExtension(strFileName);
	strExt.MakeLower();
	if (strExt != "")
	{		
		int type = FindType(strExt);
		if( CXIMAGE_FORMAT_WMF == type ) // 如果是图元文件
		{
			if( iExport== 0) // 当前页
			{
				CMetaFileDC file;
				CClientDC dc(this);
				dc.SetMapMode(MM_LOMETRIC);
				dc.m_bPrinting = TRUE;
				
				file.CreateEnhanced(&dc, strFileName, NULL , NULL);
				file.SetAttribDC(dc.m_hDC);
				file.SetMapMode(MM_LOMETRIC);
				file.m_bPrinting = TRUE;			
				
				pDoc->Draw(&file, this);
				
				HENHMETAFILE hemf = file.CloseEnhanced();
				file.ReleaseAttribDC();
				::DeleteEnhMetaFile(hemf);
			}
			else if( iExport == 1)	// 当前层
			{
				CMetaFileDC file;
				CClientDC dc(this);
				dc.SetMapMode(MM_LOMETRIC);
				dc.m_bPrinting = TRUE;
				
				file.CreateEnhanced(&dc, strFileName, NULL , NULL);
				file.SetAttribDC(dc.m_hDC);
				file.SetMapMode(MM_LOMETRIC);
				file.m_bPrinting = TRUE;			
				
				pDoc->m_pCurDrawLayer->Draw(&file, this);
				
				HENHMETAFILE hemf = file.CloseEnhanced();
				file.ReleaseAttribDC();
				::DeleteEnhMetaFile(hemf);
			}
			else//选择的图素
			{
				if(m_selection.GetCount()>0)
				{
					CMetaFileDC file;
					CClientDC dc(this);
					dc.SetMapMode(MM_LOMETRIC);
					dc.m_bPrinting = TRUE;
					
					file.CreateEnhanced(&dc, strFileName, NULL , NULL);
					file.SetAttribDC(dc.m_hDC);
					file.SetMapMode(MM_LOMETRIC);
					file.m_bPrinting = TRUE;			
					
					POSITION pos = m_selection.GetHeadPosition();
					while (pos!=NULL)
					{
						CGeoDrawObj *pObj = m_selection.GetNext(pos);
						pObj->Draw(&file, this);
					}				
					
					HENHMETAFILE hemf = file.CloseEnhanced();
					file.ReleaseAttribDC();
					::DeleteEnhMetaFile(hemf);
				}
				else
					::MessageBox(NULL, "没有选择图素！","提示信息",MB_OK|MB_ICONWARNING);
			}
		}
		else // 位图文件
		{
			if(iExport == 0 ) // 当前页
			{
				CClientDC dc(this);

				dc.SetMapMode(MM_ANISOTROPIC);
				dc.SetViewportExt(iQuality,iQuality);
				dc.SetWindowExt(100,-100);
								
				CRect client = pDoc->m_pCurDrawPage->GetPageRect(); 
				CDC* pDrawDC = &dc;
				CBitmap bitmap;
				CDC memDC;
				CBitmap* pOldBitmap;
				
				CRect rect = client;	
				dc.LPtoDP(&rect);
				rect.NormalizeRect();
				
				if (memDC.CreateCompatibleDC(&dc))
				{
					if (bitmap.CreateCompatibleBitmap(pDrawDC, rect.Width(), rect.Height()))
					{
						memDC.m_bPrinting = TRUE;

						memDC.SetMapMode(MM_ANISOTROPIC);
						memDC.SetViewportExt(iQuality,iQuality);
						memDC.SetWindowExt(100,-100);						
						
						pDrawDC = &memDC;
						memDC.OffsetViewportOrg(-rect.left, -rect.top);
						pOldBitmap = memDC.SelectObject(&bitmap);
						memDC.SetBrushOrg(rect.left % 8, rect.top % 8);
						
						memDC.GetClipBox(client);
						pDrawDC->FillSolidRect(client, RGB(255,255,255));
						pDoc->Draw(pDrawDC, this);
						
						CxImage *pImage = new CxImage;
						pImage->CreateFromHBITMAP((HBITMAP)bitmap);
						
						if (!pImage->IsValid())
							AfxMessageBox(pImage->GetLastError());
						else
							pImage->Save(strFileName, type);
						delete pImage;
						pImage = NULL;
						memDC.SelectObject(pOldBitmap);
					}
					else
					{
						::MessageBox(NULL, "超出内存，导出图像失败。","提示信息",MB_OK|MB_ICONWARNING);
					}
				}
			}
			else if( iExport == 1 ) // 当前层
			{
				CClientDC dc(this);
				dc.SetMapMode(MM_ANISOTROPIC);
				dc.SetViewportExt(iQuality,iQuality);
				dc.SetWindowExt(100,-100);
				
				CRect client = pDoc->m_pCurDrawLayer->GetLayerRect(); 
				CDC* pDrawDC = &dc;
				CBitmap bitmap;
				CDC memDC;
				CBitmap* pOldBitmap;
				
				CRect rect = client;	
				dc.LPtoDP(&rect);
				rect.NormalizeRect();
				
				if (memDC.CreateCompatibleDC(&dc))
				{
					if (bitmap.CreateCompatibleBitmap(pDrawDC, rect.Width(), rect.Height()))
					{
						memDC.m_bPrinting = TRUE;
						memDC.SetMapMode(MM_ANISOTROPIC);
						memDC.SetViewportExt(iQuality,iQuality);
						memDC.SetWindowExt(100,-100);
						
						pDrawDC = &memDC;
						memDC.OffsetViewportOrg(-rect.left, -rect.top);
						pOldBitmap = memDC.SelectObject(&bitmap);
						memDC.SetBrushOrg(rect.left % 8, rect.top % 8);
						
						memDC.GetClipBox(client);
						pDrawDC->FillSolidRect(client, RGB(255,255,255));
						
						pDoc->m_pCurDrawLayer->Draw(pDrawDC, this);
						
						CxImage *pImage = new CxImage;
						pImage->CreateFromHBITMAP((HBITMAP)bitmap);
						
						if (!pImage->IsValid())
							AfxMessageBox(pImage->GetLastError());
						else
							pImage->Save(strFileName, type);
						delete pImage;
						pImage = NULL;
						memDC.SelectObject(pOldBitmap);
					}
					else
					{
						::MessageBox(NULL, "超出内存，导出图像失败。","提示信息",MB_OK|MB_ICONWARNING);
					}
				}
			}
			else	// 所有选择的图素
			{
				if( m_selection.GetCount() > 0 )
				{					
					CClientDC dc(this);
					dc.SetMapMode(MM_ANISOTROPIC);
					dc.SetViewportExt(iQuality,iQuality);
					dc.SetWindowExt(100,-100);

					CRect client(0,0,0,0);
					POSITION pos = m_selection.GetHeadPosition();
					while (pos!=NULL)
					{
						CGeoDrawObj *pObj = m_selection.GetNext(pos);
						CRect rt = pObj->m_rtPosition;
						rt.NormalizeRect();
						client |= rt;
					}
					
					CDC* pDrawDC = &dc;
					CBitmap bitmap;
					CDC memDC;
					CBitmap* pOldBitmap;
					
					CRect rect = client;	
					dc.LPtoDP(&rect);
					rect.NormalizeRect();
					
					if (memDC.CreateCompatibleDC(&dc))
					{
						if (bitmap.CreateCompatibleBitmap(pDrawDC, rect.Width(), rect.Height()))
						{
							memDC.m_bPrinting = TRUE;
							memDC.SetMapMode(MM_ANISOTROPIC);
							memDC.SetViewportExt(iQuality,iQuality);
							memDC.SetWindowExt(100,-100);
							
							pDrawDC = &memDC;
							memDC.OffsetViewportOrg(-rect.left, -rect.top);
							pOldBitmap = memDC.SelectObject(&bitmap);
							memDC.SetBrushOrg(rect.left % 8, rect.top % 8);
							
							memDC.GetClipBox(client);
							pDrawDC->FillSolidRect(client, RGB(255,255,255));
							
							POSITION pos = m_selection.GetHeadPosition();
							while (pos!=NULL)
							{
								CGeoDrawObj *pObj = m_selection.GetNext(pos);
								pObj->Draw(pDrawDC, this);
							}
							
							CxImage *pImage = new CxImage;
							pImage->CreateFromHBITMAP((HBITMAP)bitmap);
							
							if (!pImage->IsValid())
								AfxMessageBox(pImage->GetLastError());
							else
								pImage->Save(strFileName, type);
							delete pImage;
							pImage = NULL;
							memDC.SelectObject(pOldBitmap);
						}
						else
						{
							::MessageBox(NULL, "超出内存，导出图像失败。","提示信息",MB_OK|MB_ICONWARNING);
						}
					}
				}
				else
					::MessageBox(NULL, "没有选择图素！","提示信息",MB_OK|MB_ICONWARNING);
			}
			
		}		
 		
	}
	else // 没有后缀时导出为图元文件
	{
		CMetaFileDC file;
		CClientDC dc(this);
		dc.SetMapMode(MM_LOMETRIC);
		dc.m_bPrinting = TRUE;
		
		file.CreateEnhanced(&dc, strFileName, NULL , NULL);
		file.SetAttribDC(dc.m_hDC);
		file.SetMapMode(MM_LOMETRIC);
		file.m_bPrinting = TRUE;
		
		pDoc->Draw(&file, this);
		
		HENHMETAFILE hemf = file.CloseEnhanced();
		file.ReleaseAttribDC();
		::DeleteEnhMetaFile(hemf);
	}	
}


void CGeoMapView::OnDispPagepanel() 
{
	CGeoMapDoc *pDoc = GetDocument();
	if( pDoc->m_pCurDrawPage->m_bPagePanel )
		pDoc->m_pCurDrawPage->m_bPagePanel = FALSE;
	else
		pDoc->m_pCurDrawPage->m_bPagePanel = TRUE;

	Invalidate(FALSE);

	pDoc->SetModifiedFlag();
}

void CGeoMapView::OnUpdateDispPagepanel(CCmdUI* pCmdUI) 
{
	CGeoMapDoc *pDoc = GetDocument();

	pCmdUI->SetRadio(pDoc->m_pCurDrawPage->m_bPagePanel);
}

void CGeoMapView::SetSelectActive(CGeoDrawObj* pDrawObj)
{
	POSITION pos = m_selection.GetHeadPosition();
	while (pos != NULL)
	{
		CGeoDrawObj* pObj = m_selection.GetNext(pos);
		
		if (pObj->m_trackerState == CGeoDrawObj::active )
		{
			pObj->m_trackerState = CGeoDrawObj::selected;
			InvalObj(pObj);
		}
	}
	
	pDrawObj->m_trackerState = CGeoDrawObj::active;
	
	InvalObj(pDrawObj);
}

void CGeoMapView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if (!m_bActive)
		return;
	
	CToolBase* pTool = CToolBase::FindTool(CToolBase::c_drawShape);
	if (pTool != NULL)
		pTool->OnRButtonDown(this, nFlags, point);
}

void CGeoMapView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (!m_bActive)
		return;
		
	CToolBase* pTool = CToolBase::FindTool(CToolBase::c_drawShape);
	if (pTool != NULL)
		pTool->OnKeyDown(this, nChar, nRepCnt, nFlags);
	CScrollView::OnKeyDown(nChar,  nRepCnt, nFlags);
}

void CGeoMapView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (!m_bActive)
		return;
	
	CToolBase* pTool = CToolBase::FindTool(CToolBase::c_drawShape);
	if (pTool != NULL)
		pTool->OnKeyUp(this, nChar, nRepCnt, nFlags);

	CScrollView::OnKeyUp(nChar,  nRepCnt, nFlags);
}

void CGeoMapView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (!m_bActive)
		return;
	
	CToolBase* pTool = CToolBase::FindTool(CToolBase::c_drawShape);
	if (pTool != NULL)
		pTool->OnChar(this, nChar, nRepCnt, nFlags);
	CScrollView::OnChar(nChar,  nRepCnt, nFlags);
}

void CGeoMapView::OnDrawSelect() 
{
	if( CToolBase::c_drawShape == CToolBase::clippingRgn )
		Invalidate(FALSE);

	CToolBase::c_drawShape = CToolBase::selection;	
}

void CGeoMapView::OnUpdateDrawSelect(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(CToolBase::c_drawShape == CToolBase::selection);
}

void CGeoMapView::OnDrawBasePolyline() 
{
	CToolBase::c_drawShape = CToolBase::polyline;	
}

void CGeoMapView::OnUpdateDrawBasePolyline(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(CToolBase::c_drawShape == CToolBase::polyline);
}

void CGeoMapView::OnDrawBasePline() 
{
	CToolBase::c_drawShape = CToolBase::pline;
}

void CGeoMapView::OnUpdateDrawBasePline(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(CToolBase::c_drawShape == CToolBase::pline);
}


void CGeoMapView::OnDrawInterlayer()
{
	// TODO: 在此添加命令处理程序代码
	CToolBase::c_drawShape = CToolBase::interlayer;
}

void CGeoMapView::OnUpdateDrawInterlayer(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetRadio(CToolBase::c_drawShape == CToolBase::interlayer);
}

void CGeoMapView::OnDrawBasePolygon() 
{
	CToolBase::c_drawShape = CToolBase::polygonempty;
}

void CGeoMapView::OnUpdateDrawBasePolygon(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(CToolBase::c_drawShape == CToolBase::polygonempty);
}

void CGeoMapView::OnDrawBasePlinegon() 
{
	CToolBase::c_drawShape = CToolBase::plinepolygonempty;	
}

void CGeoMapView::OnUpdateDrawBasePlinegon(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(CToolBase::c_drawShape == CToolBase::plinepolygonempty);	
}

void CGeoMapView::OnDrawBasePolygonFill() 
{
	CToolBase::c_drawShape = CToolBase::polygon;
}

void CGeoMapView::OnUpdateDrawBasePolygonFill(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(CToolBase::c_drawShape == CToolBase::polygon);
}

void CGeoMapView::OnDrawBasePlinegonFill() 
{
	CToolBase::c_drawShape = CToolBase::plinepolygon;
}

void CGeoMapView::OnUpdateDrawBasePlinegonFill(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(CToolBase::c_drawShape == CToolBase::plinepolygon);
	
}

//功能:删除命令
//参数:
//返回:
void CGeoMapView::OnEditClear() 
{
	if( CToolBase::c_drawShape == CToolBase::newFill )
		return;

 	CWnd *pWnd = GetFocus();
 	
 	if( pWnd )//如果文字图素正在编辑则发送删除键
 	{
 		if(pWnd->m_hWnd == m_LeftEdit.m_hWnd)
 		{
 			::SendMessage(m_LeftEdit.m_hWnd, WM_KEYDOWN, VK_DELETE, 0);
 			return;
 		}
 		if(pWnd->m_hWnd == m_CenterEdit.m_hWnd)
 		{
 			::SendMessage(m_CenterEdit.m_hWnd, WM_KEYDOWN, VK_DELETE, 0);
 			return;
 		}
 		if(pWnd->m_hWnd == m_RightEdit.m_hWnd)
 		{
 			::SendMessage(m_RightEdit.m_hWnd, WM_KEYDOWN, VK_DELETE, 0);
 			return;
 		}
 	}
	
	//否则删除图素
	// update all the views before the selection goes away
	GetDocument()->UpdateAllViews(NULL, HINT_DELETE_SELECTION, &m_selection);
	OnUpdate(NULL, HINT_UPDATE_SELECTION, NULL);
	
	// now remove the selection from the document
	CGeoMapDoc *pDoc = GetDocument();
	
	if(pDoc->m_pCurDrawPage->m_bLocked || pDoc->m_pCurDrawLayer->m_bLocked)
		return;

	if(m_selection.GetCount() == 1)
	{		
		POSITION pos = m_selection.GetHeadPosition();
		while (pos != NULL)
		{
			CGeoDrawObj* pObj = m_selection.GetNext(pos);
			
			pDoc->Do(new CCmdRemoveDrawObj(pDoc, pObj), TRUE);		// @@ added line			
		}
	}
	else
		pDoc->Do(new CCmdRemoveDrawObjs(pDoc, m_selection), TRUE);		// @@ added line
}

void CGeoMapView::OnUpdateEditClear(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_selection.IsEmpty());

}

//////////////////////////////////////////////////////////////////////////
// 曲线操作
void CGeoMapView::OnPloyAddPt() 
{
	CToolBase::c_drawShape = CToolBase::addPoint;
}

void CGeoMapView::OnUpdatePloyAddPt(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);

	if(m_selection.GetCount() == 1)
	{
		CGeoDrawObj *pObj = m_selection.GetHead();
		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPolyObj)))
		{
			pCmdUI->Enable(TRUE);
		}
	}
	
	pCmdUI->SetCheck(CToolBase::c_drawShape == CToolBase::addPoint);
}

void CGeoMapView::OnPolyDelPt() 
{
	CToolBase::c_drawShape = CToolBase::delPoint;
}

void CGeoMapView::OnUpdatePolyDelPt(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);
	
	if(m_selection.GetCount() == 1)
	{
		CGeoDrawObj *pObj = m_selection.GetHead();
		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPolyObj)))
		{
			pCmdUI->Enable(TRUE);
		}
	}
	pCmdUI->SetCheck(CToolBase::c_drawShape == CToolBase::delPoint);
}

void CGeoMapView::OnPolyMovePt() 
{
	CToolBase::c_drawShape = CToolBase::movePoint;
}

void CGeoMapView::OnUpdatePolyMovePt(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);
	
	if(m_selection.GetCount() == 1)
	{
		CGeoDrawObj *pObj = m_selection.GetHead();
		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPolyObj)))
		{
			pCmdUI->Enable(TRUE);
		}
	}
	pCmdUI->SetCheck(CToolBase::c_drawShape == CToolBase::movePoint);
}

void CGeoMapView::OnPolyDivideLine() 
{
	CToolBase::c_drawShape = CToolBase::dividePoly;
}

void CGeoMapView::OnUpdatePolyDivideLine(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(CToolBase::c_drawShape == CToolBase::dividePoly);
}

void CGeoMapView::OnPolyJoinLine() 
{
	CToolBase::c_drawShape = CToolBase::joinPoly;
}

void CGeoMapView::OnUpdatePolyJoinLine(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(CToolBase::c_drawShape == CToolBase::joinPoly);	
}

//////////////////////////////////////////////////////////////////////////
// 排序

//功能:将选中图素置于最上方
//参数:
//返回:
void CGeoMapView::OnObjectMovetofront() 
{
	CGeoMapDoc* pDoc = GetDocument();
	CGeoDrawObj* pObj = m_selection.GetHead();
	pDoc->Do(new CCmdMoveToFront(pDoc, pObj), TRUE);
	InvalObj(pObj);	
}

void CGeoMapView::OnUpdateObjectMovetofront(CCmdUI* pCmdUI) 
{
	if(m_selection.GetCount() == 1)
	{
		pCmdUI->Enable(TRUE);
	}
	else
		pCmdUI->Enable(FALSE);
}

//功能:将选中图素置于最下方
//参数:
//返回:
void CGeoMapView::OnObjectMovetoback() 
{
	CGeoMapDoc* pDoc = GetDocument();
	CGeoDrawObj* pObj = m_selection.GetHead();
	pDoc->Do(new CCmdMoveToBack(pDoc, pObj), TRUE);
	InvalObj(pObj);
}

void CGeoMapView::OnUpdateObjectMovetoback(CCmdUI* pCmdUI) 
{
	if(m_selection.GetCount() == 1)
	{
		pCmdUI->Enable(TRUE);
	}
	else
		pCmdUI->Enable(FALSE);
}

//功能:将选中图素向上移动一层
//参数:
//返回:
void CGeoMapView::OnObjectMoveforward() 
{	
	CGeoMapDoc* pDoc = GetDocument();
	CGeoDrawObj* pObj = m_selection.GetHead();
	pDoc->Do(new CCmdMoveForward(pDoc, pObj), TRUE);
	InvalObj(pObj);	
}

void CGeoMapView::OnUpdateObjectMoveforward(CCmdUI* pCmdUI) 
{
	if(m_selection.GetCount() == 1)
	{
		pCmdUI->Enable(TRUE);
	}
	else
		pCmdUI->Enable(FALSE);
}

//功能:将选中图素向下移动一层
//参数:
//返回:
void CGeoMapView::OnObjectMoveback() 
{
	CGeoMapDoc* pDoc = GetDocument();
	CGeoDrawObj* pObj = m_selection.GetHead();
	pDoc->Do(new CCmdMoveBack(pDoc, pObj), TRUE);
	InvalObj(pObj);
}

void CGeoMapView::OnUpdateObjectMoveback(CCmdUI* pCmdUI) 
{
	if(m_selection.GetCount() == 1)
	{
		pCmdUI->Enable(TRUE);
	}
	else
		pCmdUI->Enable(FALSE);
}

void CGeoMapView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if( nIDEvent == 100 )
	{
		
		return;
	}
	CScrollView::OnTimer(nIDEvent);
}

void CGeoMapView::OnDrawPolygon() 
{
	CToolBase::c_drawShape = CToolBase::poly;
}

void CGeoMapView::OnUpdateDrawPolygon(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(CToolBase::c_drawShape == CToolBase::poly);
}

void CGeoMapView::OnDrawSmoothFill() 
{
	CToolBase::c_drawShape = CToolBase::smoothfill;
}

void CGeoMapView::OnUpdateDrawSmoothFill(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(CToolBase::c_drawShape == CToolBase::smoothfill);
}

void CGeoMapView::OnDrawBaseRectEmpty() 
{
	CToolBase::c_drawShape = CToolBase::rectempty;
}

void CGeoMapView::OnUpdateDrawBaseRectEmpty(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(CToolBase::c_drawShape == CToolBase::rectempty);
}


void CGeoMapView::OnDrawBaseRect() 
{
	CToolBase::c_drawShape = CToolBase::rect;
}

void CGeoMapView::OnUpdateDrawBaseRect(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(CToolBase::c_drawShape == CToolBase::rect);
}


void CGeoMapView::OnDrawRect() 
{
	CToolBase::c_drawShape = CToolBase::solidrect;
}

void CGeoMapView::OnUpdateDrawRect(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(CToolBase::c_drawShape == CToolBase::solidrect);
}


void CGeoMapView::OnDrawShow() 
{
	POSITION pos = m_selection.GetHeadPosition();
	while (pos != NULL)//清空时使每个图素处于非编辑状态
	{
		CGeoDrawObj* pObj = m_selection.GetNext(pos);
		pObj->m_bShow = TRUE;
		InvalObj(pObj);
	}
	CGeoMapDoc* pDoc = GetDocument();
	pDoc->SetModifiedFlag();
}

void CGeoMapView::OnUpdateDrawShow(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_selection.GetCount()>0);
}

void CGeoMapView::OnDrawLock() 
{
	POSITION pos = m_selection.GetHeadPosition();
	while (pos != NULL)//清空时使每个图素处于非编辑状态
	{
		CGeoDrawObj* pObj = m_selection.GetNext(pos);
		pObj->m_bLocked = TRUE;
		InvalObj(pObj);
	}
	CGeoMapDoc* pDoc = GetDocument();
	pDoc->SetModifiedFlag();
}

void CGeoMapView::OnUpdateDrawLock(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_selection.GetCount()>0);
}

void CGeoMapView::OnDrawHide() 
{
	POSITION pos = m_selection.GetHeadPosition();
	while (pos != NULL)//清空时使每个图素处于非编辑状态
	{
		CGeoDrawObj* pObj = m_selection.GetNext(pos);
		pObj->m_bShow = FALSE;
		InvalObj(pObj);
	}
	CGeoMapDoc* pDoc = GetDocument();
	pDoc->SetModifiedFlag();
}

void CGeoMapView::OnUpdateDrawHide(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_selection.GetCount()>0);
}

void CGeoMapView::OnDrawNolock() 
{
	POSITION pos = m_selection.GetHeadPosition();
	while (pos != NULL)//清空时使每个图素处于非编辑状态
	{
		CGeoDrawObj* pObj = m_selection.GetNext(pos);
		pObj->m_bLocked = FALSE;
		InvalObj(pObj);
	}
	CGeoMapDoc* pDoc = GetDocument();
	pDoc->SetModifiedFlag();
}

void CGeoMapView::OnUpdateDrawNolock(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_selection.GetCount()>0);
}

void CGeoMapView::OnEditPaste() 
{
	COleDataObject dataObject;
	dataObject.AttachClipboard();
	
	// invalidate current selection since it will be deselected
	OnUpdate(NULL, HINT_UPDATE_SELECTION, NULL);
	m_selection.RemoveAll();
	if (dataObject.IsDataAvailable(m_cfDraw_E))
	{
		PasteNative(dataObject);
		
		CGeoMapDoc *pDoc = GetDocument();
		pDoc->Do(new CCmdAddDrawObjs(pDoc, m_selection), TRUE);
		POSITION  pos = m_selection.GetHeadPosition();
		while(pos!=NULL)
		{
			CGeoDrawObj *pObj = m_selection.GetNext(pos);
			pObj->ClaimChildren();
		}
	}
	else
		PasteEmbedded(dataObject, GetInitialPosition().TopLeft());
	
	GetDocument()->SetModifiedFlag();
	
	// invalidate new pasted stuff
	GetDocument()->UpdateAllViews(NULL, HINT_UPDATE_SELECTION, &m_selection);
}

void CGeoMapView::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	COleDataObject dataObject;
	BOOL bEnable = dataObject.AttachClipboard() &&
		(dataObject.IsDataAvailable(m_cfDraw_E) ||
		COleClientItem::CanCreateFromData(&dataObject));
	
	// enable command based on availability
	pCmdUI->Enable(bEnable);	
}


void CGeoMapView::PasteNative(COleDataObject& dataObject)
{
	// get file refering to clipboard data
	CFile* pFile = dataObject.GetFileData(m_cfDraw_E);
	if (pFile == NULL)
		return;
	
	// connect the file to the archive
	CArchive ar(pFile, CArchive::load);
	TRY
	{
		ar.m_pDocument = GetDocument(); // set back-pointer in archive
		
		// read the selection
		m_selection.Serialize(ar);
	}
	CATCH_ALL(e)
	{
		ar.Close();
		delete pFile;
		THROW_LAST();
	}
	END_CATCH_ALL
		
		ar.Close();
	delete pFile;
}

void CGeoMapView::OnDrawCapture() 
{
	if( m_bCapture )
		m_bCapture = FALSE;
	else
		m_bCapture = TRUE;
}

void CGeoMapView::OnUpdateDrawCapture(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(m_bCapture);
}

void CGeoMapView::OnEditSelectAll() 
{
	m_selection.RemoveAll();

	if( !m_bPageSelectAll )
	{
		CGeoDrawObjList* pObList = &(GetDocument()->m_pCurDrawLayer->m_DrawObjects);
		POSITION pos = pObList->GetHeadPosition();
		while (pos != NULL)
			Select(pObList->GetNext(pos), TRUE);
	}
	else
	{
		CDrawLayerList * pLayerList = &(GetDocument()->m_pCurDrawPage->m_DrawLayers);
		POSITION pos = pLayerList->GetHeadPosition();
		while( pos != NULL)
		{			
			CGeoDrawObjList* pObList = &(pLayerList->GetNext(pos)->m_DrawObjects);
			POSITION pos1 = pObList->GetHeadPosition();
			while (pos1 != NULL)
				Select(pObList->GetNext(pos1), TRUE);
		}
	}
}

void CGeoMapView::OnUpdateEditSelectAll(CCmdUI* pCmdUI) 
{
 	if( !m_bPageSelectAll )
 	{
 		CGeoDrawObjList* pObList = &(GetDocument()->m_pCurDrawLayer->m_DrawObjects);
 		pCmdUI->Enable(pObList->GetCount() != 0);
 	}
 	else
 	{
 		CDrawLayerList * pLayerList = &(GetDocument()->m_pCurDrawPage->m_DrawLayers);
 		POSITION pos = pLayerList->GetHeadPosition();
 		while( pos != NULL)
 		{
 			CGeoDrawObjList* pObList = &(pLayerList->GetNext(pos)->m_DrawObjects);
 			pCmdUI->Enable(pObList->GetCount() != 0);
 		}
 	}
}

void CGeoMapView::OnElementSelectall() 
{
	if( m_bPageSelectAll )
		m_bPageSelectAll = FALSE;
	else
		m_bPageSelectAll = TRUE;
}

void CGeoMapView::OnUpdateElementSelectall(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(m_bPageSelectAll);
}

//功能:剪切命令
//参数:
//返回:
void CGeoMapView::OnEditCut() 
{
	OnEditCopy();
	OnEditClear();
}

void CGeoMapView::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_selection.GetCount()>0);
}

void CGeoMapView::OnOleInsertNew() 
{
	COleInsertDialog dlg;
	if (dlg.DoModal() != IDOK)
		return;
	
	BeginWaitCursor();
	
	CGeoMapDoc *pDoc = GetDocument();
	// First create the C++ object
	CGeoDrawOleObj* pObj = new CGeoDrawOleObj(GetInitialPosition(), pDoc->m_pCurDrawLayer);
	ASSERT_VALID(pObj);
	CGeoDrawItem* pItem = new CGeoDrawItem(GetDocument(), pObj);
	ASSERT_VALID(pItem);
	pObj->m_pClientItem = pItem;
	
	// Now create the OLE object/item
	TRY
	{
		if (!dlg.CreateItem(pObj->m_pClientItem))
			AfxThrowMemoryException();
		
		// add the object to the document
		GetDocument()->Add(pObj);
		
		// try to get initial presentation data
		pItem->UpdateLink();
		pItem->UpdateExtent();
		
		// if insert new object -- initially show the object
		if (dlg.GetSelectionType() == COleInsertDialog::createNewItem)
			pItem->DoVerb(OLEIVERB_SHOW, this);
	}
	CATCH_ALL(e)
	{
		// clean up item
		pItem->Delete();
		pObj->m_pClientItem = NULL;
		GetDocument()->Remove(pObj);
		pObj->Remove();
		
		AfxMessageBox(_T("建立对象失败。请确认该对象已被注册入系统。"));
	}
	END_CATCH_ALL
		
	EndWaitCursor();
}

// 左对齐
void CGeoMapView::OnArrangeLeft() 
{
	CGeoDrawObj *pDrawObj;
	CRect rect;

	CGeoMapDoc *pDoc = GetDocument();

	POSITION pos = m_selection.GetHeadPosition();
	while (pos != NULL)
	{
		CGeoDrawObj* pObj = m_selection.GetNext(pos);
		if(pObj->m_trackerState == CGeoDrawObj::active)
		{
			pDrawObj = pObj;
			rect = pObj->m_rtPosition;
			rect.NormalizeRect();
			break;
		}
	}

	pos = m_selection.GetHeadPosition();
	while (pos != NULL)
	{
		CGeoDrawObj* pObj = m_selection.GetNext(pos);
		if(pObj != pDrawObj)
		{
			CRect oldRT = pObj->m_rtPosition;
			CRect newRT = oldRT;

			oldRT.NormalizeRect();
			CPoint pt;
			pt.x = rect.left - oldRT.left;
			pt.y = 0;
			newRT += pt;

			pDoc->Do(new CCmdObjMove(pDoc,pObj,newRT, oldRT,this), TRUE);
		}
	}
}

void CGeoMapView::OnUpdateArrangeLeft(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_selection.GetCount()>1);
}

// 右对齐
void CGeoMapView::OnArrangeRight() 
{
	CGeoDrawObj *pDrawObj;
	CRect rect;
	
	CGeoMapDoc *pDoc = GetDocument();
	
	POSITION pos = m_selection.GetHeadPosition();
	while (pos != NULL)
	{
		CGeoDrawObj* pObj = m_selection.GetNext(pos);
		if(pObj->m_trackerState == CGeoDrawObj::active)
		{
			pDrawObj = pObj;
			rect = pObj->m_rtPosition;
			rect.NormalizeRect();
			break;
		}
	}
	
	pos = m_selection.GetHeadPosition();
	while (pos != NULL)
	{
		CGeoDrawObj* pObj = m_selection.GetNext(pos);
		if(pObj != pDrawObj)
		{
			CRect oldRT = pObj->m_rtPosition;
			CRect newRT = oldRT;
			
			oldRT.NormalizeRect();
			CPoint pt;
			pt.x = rect.right - oldRT.right;
			pt.y = 0;
			newRT += pt;
			
			pDoc->Do(new CCmdObjMove(pDoc,pObj,newRT, oldRT,this), TRUE);
		}
	}

}

void CGeoMapView::OnUpdateArrangeRight(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_selection.GetCount()>1);
}

// 上对齐
void CGeoMapView::OnArrangeTop() 
{
	CGeoDrawObj *pDrawObj;
	CRect rect;
	
	CGeoMapDoc *pDoc = GetDocument();
	
	POSITION pos = m_selection.GetHeadPosition();
	while (pos != NULL)
	{
		CGeoDrawObj* pObj = m_selection.GetNext(pos);
		if(pObj->m_trackerState == CGeoDrawObj::active)
		{
			pDrawObj = pObj;
			rect = pObj->m_rtPosition;
			rect.NormalizeRect();
			break;
		}
	}
	
	pos = m_selection.GetHeadPosition();
	while (pos != NULL)
	{
		CGeoDrawObj* pObj = m_selection.GetNext(pos);
		if(pObj != pDrawObj)
		{
			CRect oldRT = pObj->m_rtPosition;
			CRect newRT = oldRT;
			
			oldRT.NormalizeRect();
			CPoint pt;
			pt.x = 0;
			pt.y = rect.bottom - oldRT.bottom;
			newRT += pt;
			
			pDoc->Do(new CCmdObjMove(pDoc,pObj,newRT, oldRT,this), TRUE);
		}
	}
}

void CGeoMapView::OnUpdateArrangeTop(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_selection.GetCount()>1);
}

// 下对齐
void CGeoMapView::OnArrangeBottom() 
{
	CGeoDrawObj *pDrawObj;
	CRect rect;
	
	CGeoMapDoc *pDoc = GetDocument();
	
	POSITION pos = m_selection.GetHeadPosition();
	while (pos != NULL)
	{
		CGeoDrawObj* pObj = m_selection.GetNext(pos);
		if(pObj->m_trackerState == CGeoDrawObj::active)
		{
			pDrawObj = pObj;
			rect = pObj->m_rtPosition;
			rect.NormalizeRect();
			break;
		}
	}
	
	pos = m_selection.GetHeadPosition();
	while (pos != NULL)
	{
		CGeoDrawObj* pObj = m_selection.GetNext(pos);
		if(pObj != pDrawObj)
		{
			CRect oldRT = pObj->m_rtPosition;
			CRect newRT = oldRT;
			
			oldRT.NormalizeRect();
			CPoint pt;
			pt.x = 0;
			pt.y = rect.top - oldRT.top;
			newRT += pt;
			
			pDoc->Do(new CCmdObjMove(pDoc,pObj,newRT, oldRT,this), TRUE);
		}
	}
}

void CGeoMapView::OnUpdateArrangeBottom(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_selection.GetCount()>1);
}

// 水平居中
void CGeoMapView::OnArrangeCenter() 
{
	CGeoDrawObj *pDrawObj;
	CRect rect;
	CPoint point;
	
	CGeoMapDoc *pDoc = GetDocument();
	
	POSITION pos = m_selection.GetHeadPosition();
	while (pos != NULL)
	{
		CGeoDrawObj* pObj = m_selection.GetNext(pos);
		if(pObj->m_trackerState == CGeoDrawObj::active)
		{
			pDrawObj = pObj;
			rect = pObj->m_rtPosition;
			rect.NormalizeRect();
			point = rect.CenterPoint();
			break;
		}
	}
	
	pos = m_selection.GetHeadPosition();
	while (pos != NULL)
	{
		CGeoDrawObj* pObj = m_selection.GetNext(pos);
		if(pObj != pDrawObj)
		{
			CRect oldRT = pObj->m_rtPosition;
			CRect newRT = oldRT;
			
			oldRT.NormalizeRect();
			CPoint pt = oldRT.CenterPoint();
			pt.x = 0;
			pt.y = point.y - pt.y;			
			newRT += pt;
			
			pDoc->Do(new CCmdObjMove(pDoc,pObj,newRT, oldRT,this), TRUE);
		}
	}
}

void CGeoMapView::OnUpdateArrangeCenter(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_selection.GetCount()>1);
}

// 垂直居中
void CGeoMapView::OnArrangeVcenter() 
{
	CGeoDrawObj *pDrawObj;
	CRect rect;
	CPoint point;
	
	CGeoMapDoc *pDoc = GetDocument();
	
	POSITION pos = m_selection.GetHeadPosition();
	while (pos != NULL)
	{
		CGeoDrawObj* pObj = m_selection.GetNext(pos);
		if(pObj->m_trackerState == CGeoDrawObj::active)
		{
			pDrawObj = pObj;
			rect = pObj->m_rtPosition;
			rect.NormalizeRect();
			point = rect.CenterPoint();
			break;
		}
	}
	
	pos = m_selection.GetHeadPosition();
	while (pos != NULL)
	{
		CGeoDrawObj* pObj = m_selection.GetNext(pos);
		if(pObj != pDrawObj)
		{
			CRect oldRT = pObj->m_rtPosition;
			CRect newRT = oldRT;
			
			oldRT.NormalizeRect();
			CPoint pt = oldRT.CenterPoint();
			pt.x = point.x - pt.x;
			pt.y = 0;
			newRT += pt;
			
			pDoc->Do(new CCmdObjMove(pDoc,pObj,newRT, oldRT,this), TRUE);
		}
	}
}

void CGeoMapView::OnUpdateArrangeVcenter(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_selection.GetCount()>1);
}

// 横向平均
void CGeoMapView::OnEvenlyHorz() 
{
	CGeoMapDoc *pDoc = GetDocument();

	CGeoDrawObj *pLeftObj, *pRightObj;
	
	pLeftObj = pRightObj = m_selection.GetTail();
	
	POSITION pos = m_selection.GetHeadPosition();
	while (pos != NULL)
	{
		CGeoDrawObj* pDrawObj = m_selection.GetNext(pos);
		
		if (pDrawObj->m_rtPosition.right < pLeftObj->m_rtPosition.right )
			pLeftObj = pDrawObj;
		if (pDrawObj->m_rtPosition.left > pRightObj->m_rtPosition.left )
			pRightObj = pDrawObj;
	}
	
	int nWidth = 0;
	pos = m_selection.GetHeadPosition();
	while (pos != NULL)
	{
		CGeoDrawObj* pDrawObj = m_selection.GetNext(pos);
		
		if (pDrawObj != pLeftObj && pDrawObj != pRightObj )
			nWidth += pDrawObj->m_rtPosition.Width();
	}
	
	int n = m_selection.GetCount();
	
	int nStep = (pRightObj->m_rtPosition.left - pLeftObj->m_rtPosition.right - nWidth ) / ( n - 1);
	
	int nLeft = pLeftObj->m_rtPosition.right + nStep;
	
	pos = m_selection.GetHeadPosition();
	while (pos != NULL)
	{
		CGeoDrawObj* pDrawObj = m_selection.GetNext(pos);
		
		if (pDrawObj != pLeftObj && pDrawObj != pRightObj )
		{
			CPoint pt(0,0);
			CRect rect = pDrawObj->m_rtPosition;
			CRect rt = pDrawObj->m_rtPosition;
			pt.x = -rect.left + nLeft;
			rect += pt;
			pDoc->Do(new CCmdObjMove(pDoc,pDrawObj,rect,rt,this), TRUE);
			nLeft += rect.Width() + nStep;
		}
	}
}

void CGeoMapView::OnUpdateEvenlyHorz(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_selection.GetCount()>1);
}

// 纵向平均
void CGeoMapView::OnEvenlyVert() 
{
	CGeoDrawObj *pTopObj, *pBottomObj;
	
	CGeoMapDoc *pDoc = GetDocument();

	pTopObj = pBottomObj = m_selection.GetTail();
	
	POSITION pos = m_selection.GetHeadPosition();
	while (pos != NULL)
	{
		CGeoDrawObj* pDrawObj = m_selection.GetNext(pos);
		
		if (pDrawObj->m_rtPosition.bottom < pTopObj->m_rtPosition.bottom )
			pTopObj = pDrawObj;
		if (pDrawObj->m_rtPosition.top > pBottomObj->m_rtPosition.top )
			pBottomObj = pDrawObj;
	}
	
	int nHeight = 0;
	pos = m_selection.GetHeadPosition();
	while (pos != NULL)
	{
		CGeoDrawObj* pDrawObj = m_selection.GetNext(pos);
		
		if (pDrawObj != pTopObj && pDrawObj != pBottomObj )
			nHeight += pDrawObj->m_rtPosition.Height();
	}
	
	int n = m_selection.GetCount();
	
	int nStep = (pBottomObj->m_rtPosition.top - pTopObj->m_rtPosition.bottom - nHeight ) / ( n - 1);
	
	int nTop = pTopObj->m_rtPosition.bottom + nStep;
	
	pos = m_selection.GetHeadPosition();
	while (pos != NULL)
	{
		CGeoDrawObj* pDrawObj = m_selection.GetNext(pos);
		
		if (pDrawObj != pTopObj && pDrawObj != pBottomObj )
		{
			CPoint pt(0,0);
			CRect rect = pDrawObj->m_rtPosition;
			CRect rt = pDrawObj->m_rtPosition;
			pt.y = -rect.top + nTop;
			rect += pt;

			pDoc->Do(new CCmdObjMove(pDoc,pDrawObj,rect,rt,this), TRUE);
			nTop += rect.Height() + nStep;
		}
	}
}

void CGeoMapView::OnUpdateEvenlyVert(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_selection.GetCount()>1);
}

// 相同宽度
void CGeoMapView::OnAliginWidth() 
{
	CGeoDrawObj *pDrawObj;
	int nWidth;
//	int nHeight;
	CPoint point;
	
	CGeoMapDoc *pDoc = GetDocument();
	
	POSITION pos = m_selection.GetHeadPosition();
	while (pos != NULL)
	{
		CGeoDrawObj* pObj = m_selection.GetNext(pos);
		if(pObj->m_trackerState == CGeoDrawObj::active)
		{
			pDrawObj = pObj;
			CRect rect = pObj->m_rtPosition;
			
			nWidth = rect.Width();
//			nHeight = rect.Height();

			break;
		}
	}
	
	pos = m_selection.GetHeadPosition();
	while (pos != NULL)
	{
		CGeoDrawObj* pObj = m_selection.GetNext(pos);
		if(pObj != pDrawObj)
		{
			CRect oldRT = pObj->m_rtPosition;
			CRect newRT = oldRT;
			
			oldRT.NormalizeRect();

			CPoint point = oldRT.CenterPoint();
			newRT.left = point.x - nWidth/2;
			newRT.right = newRT.left + nWidth;			
			
			pDoc->Do(new CCmdObjMove(pDoc,pObj,newRT, oldRT,this), TRUE);
		}
	}
}

void CGeoMapView::OnUpdateAliginWidth(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_selection.GetCount()>1);
}

// 相同高度
void CGeoMapView::OnAliginHeight() 
{
	CGeoDrawObj *pDrawObj;
	// int nWidth;
	int nHeight;
	CPoint point;
	
	CGeoMapDoc *pDoc = GetDocument();
	
	POSITION pos = m_selection.GetHeadPosition();
	while (pos != NULL)
	{
		CGeoDrawObj* pObj = m_selection.GetNext(pos);
		if(pObj->m_trackerState == CGeoDrawObj::active)
		{
			pDrawObj = pObj;
			CRect rect = pObj->m_rtPosition;
			
			// nWidth = rect.Width();
			nHeight = rect.Height();
			
			break;
		}
	}
	
	pos = m_selection.GetHeadPosition();
	while (pos != NULL)
	{
		CGeoDrawObj* pObj = m_selection.GetNext(pos);
		if(pObj != pDrawObj)
		{
			CRect oldRT = pObj->m_rtPosition;
			CRect newRT = oldRT;
			
			oldRT.NormalizeRect();
			
			CPoint point = oldRT.CenterPoint();
			
			newRT.top = point.y - nHeight/2;
			newRT.bottom = newRT.top + nHeight;			
			
			pDoc->Do(new CCmdObjMove(pDoc,pObj,newRT, oldRT,this), TRUE);
		}
	}
}

void CGeoMapView::OnUpdateAliginHeight(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_selection.GetCount()>1);
}

// 相同尺寸
void CGeoMapView::OnAliginSize() 
{
	CGeoDrawObj *pDrawObj;
	int nWidth;
	int nHeight;
	CPoint point;
	
	CGeoMapDoc *pDoc = GetDocument();
	
	POSITION pos = m_selection.GetHeadPosition();
	while (pos != NULL)
	{
		CGeoDrawObj* pObj = m_selection.GetNext(pos);
		if(pObj->m_trackerState == CGeoDrawObj::active)
		{
			pDrawObj = pObj;
			CRect rect = pObj->m_rtPosition;
			
			nWidth = rect.Width();
			nHeight = rect.Height();
			
			break;
		}
	}
	
	pos = m_selection.GetHeadPosition();
	while (pos != NULL)
	{
		CGeoDrawObj* pObj = m_selection.GetNext(pos);
		if(pObj != pDrawObj)
		{
			CRect oldRT = pObj->m_rtPosition;
			CRect newRT = oldRT;
			
			oldRT.NormalizeRect();
			
			CPoint point = oldRT.CenterPoint();

			newRT.left = point.x - nWidth/2;
			newRT.right = newRT.left + nWidth;
			
			newRT.top = point.y - nHeight/2;
			newRT.bottom = newRT.top + nHeight;			
			
			pDoc->Do(new CCmdObjMove(pDoc,pObj,newRT, oldRT,this), TRUE);
		}
	}
}

void CGeoMapView::OnUpdateAliginSize(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_selection.GetCount()>1);
}

void CGeoMapView::OnImportLine() 
{
//	static char szFilter[] = "增强图元文件 (*.EMF)|*.EMF||"; 
	CString str;
	
	//CFileDialog dlg(FALSE, "*.WMF", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, NULL);
	
	CFileDialog dlg(TRUE);

	//------用来检测目前的操作系统的版本信息	
	int nStructSize = 0;	
	DWORD dwVersion, dwWindowsMajorVersion, dwWindwosMinorVersion;	
	dwVersion = GetVersion();	
	dwWindowsMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));	
	dwWindwosMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));	
	if (dwVersion < 0x80000000)
	{		
        nStructSize = 88; // 2k,xp下显示新的版本		
	}	
	else		
	{		
        nStructSize = 76; // 98下显示老版本		
	}	
	dlg.m_ofn.lStructSize = nStructSize; //------用来检测目前的操作系统的版本信息
	if (dlg.DoModal() == IDOK)
	{
		CString strFileName = dlg.GetPathName();

		CStdioFile file;
		if( !file.Open( strFileName, CFile::modeRead | CFile::typeText ) ) 
		{
			::MessageBox(NULL,"打开文件失败!","信息提示",MB_OK|MB_ICONERROR);
			return;
		}

		CString buff;
		CGeoPolyObj *pObj;
		CGeoMapDoc *pDoc = GetDocument();
		while (file.ReadString(buff))
		{
			buff.TrimLeft();
			buff.TrimRight();
			if (buff.IsEmpty())
			{
				if(pObj!=NULL)
				{
					pObj->RecalcBounds(this);
					pDoc->Add(pObj);
					pObj = NULL;
				}
				continue;
			}
			buff.MakeUpper();
			if(buff.Left(5) == _T("PLINE"))
			{
				pObj = new CGeoPolyObj(pDoc->m_pCurDrawLayer);
				continue;
			}
			
			if(pObj==NULL)
				continue;

			CPoint2D point;
			if( sscanf_s((LPCSTR)buff,"%lf,%lf",&point.x, &point.y)!=2)
				continue;
			pObj->AddPoint(point,this);
		}

		if(pObj!=NULL)
		{
			pObj->RecalcBounds(this);
			pDoc->Add(pObj);
			pObj = NULL;
		}

		file.Close();
	}
}

void CGeoMapView::OnDrawClip() 
{
	CGeoMapDoc *pDoc = GetDocument();
	CGeoDrawObj *pObj = &pDoc->m_pCurDrawLayer->m_rgnClipping;
	CToolBase::c_drawShape = CToolBase::clippingRgn;
	InvalObj(pObj);
}

void CGeoMapView::OnUpdateDrawClip(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(CToolBase::c_drawShape == CToolBase::clippingRgn);
}

void CGeoMapView::OnAttributeBrush() 
{
	CToolBase::c_drawShape = CToolBase::attriBrush;
}

void CGeoMapView::OnUpdateAttributeBrush(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_selection.GetCount()==1);
	pCmdUI->SetRadio(CToolBase::c_drawShape == CToolBase::attriBrush);

}

void CGeoMapView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CToolBase* pTool = CToolBase::FindTool(CToolBase::c_drawShape);
	if (pTool != NULL)
	{
		pTool->OnContextMenu(this, pWnd, point);
		return;
	}	
}

int CGeoMapView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}


//////////////////////////////////////////////////////////////////////////
//  文本格式
// 
void CGeoMapView::OnFontname() 
{
	USES_CONVERSION;
	
	
	CMFCToolBarFontComboBox* pSrcCombo = 
		(CMFCToolBarFontComboBox*) CMFCToolBarComboBoxButton::GetByCmd (IDC_FONTNAME, TRUE);
	if (pSrcCombo == NULL)
	{		
		return;
	}
	
	
	CCharFormat cf;
	cf.szFaceName[0] = NULL;
	cf.dwMask = CFM_FACE | CFM_CHARSET;
	
	int nn = pSrcCombo->GetCurSel();
	if( nn == -1 )
		return;
	
	const CMFCFontInfo* pDesc = pSrcCombo->GetFontDesc ();
	
	ASSERT_VALID (pDesc);
	ASSERT(pDesc->m_strName.GetLength() < LF_FACESIZE);
	
#if _MSC_VER >= 1300
	lstrcpyn(cf.szFaceName, pDesc->m_strName, LF_FACESIZE);
#else
	lstrcpynA(cf.szFaceName,
		T2A((LPTSTR) (LPCTSTR) pDesc->m_strName), LF_FACESIZE);
#endif
	cf.bCharSet = pDesc->m_nCharSet;
	cf.bPitchAndFamily = pDesc->m_nPitchAndFamily;
	
	CMFCToolBarFontSizeComboBox* pSizeCombo =
		DYNAMIC_DOWNCAST (CMFCToolBarFontSizeComboBox, CMFCToolBarFontSizeComboBox::GetByCmd (IDC_FONTSIZE));
	if (pSizeCombo != NULL)
	{
		int nSize = pSizeCombo->GetTwipSize();
		if (nSize == -2) // error
		{
			//			AfxMessageBox(IDS_INVALID_NUMBER, MB_OK|MB_ICONINFORMATION);
			nSize = pSizeCombo->GetTwipSize ();
		}
		else if ((nSize >= 0 && nSize < 20) || nSize > 32760)
		{
			//			AfxMessageBox(IDS_INVALID_FONTSIZE, MB_OK|MB_ICONINFORMATION);
			nSize = pSizeCombo->GetTwipSize ();
		}
		
		pSizeCombo->RebuildFontSizes (pDesc->m_strName);
		pSizeCombo->SetTwipSize (nSize);
	}
	
	SetCharFormat (cf);
	SetFocus ();
}

void CGeoMapView::OnUpdateFontname(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}

void CGeoMapView::OnFontsize() 
{
	CMFCToolBarFontSizeComboBox* pSrcCombo = 
		(CMFCToolBarFontSizeComboBox*) CMFCToolBarComboBoxButton::GetByCmd (IDC_FONTSIZE, TRUE);
	
	if (pSrcCombo == NULL)
	{
		return;
	}
	
	int nSize = pSrcCombo->GetTwipSize();
	if (nSize == -2)
	{
		//		AfxMessageBox(IDS_INVALID_NUMBER, MB_OK|MB_ICONINFORMATION);
	}
	else if ((nSize >= 0 && nSize < 20) || nSize > 32760)
	{
		//		AfxMessageBox(IDS_INVALID_FONTSIZE, MB_OK|MB_ICONINFORMATION);
	}
	else if (nSize > 0)
	{
		CCharFormat cf;
		cf.dwMask = CFM_SIZE;
		cf.yHeight = nSize;
		
		SetCharFormat (cf);
		SetFocus ();
	}
}

void CGeoMapView::OnUpdateFontsize(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}

void CGeoMapView::OnCharBold() 
{
	if (m_selection.GetCount() == 0)
		return;
	
	POSITION pos = m_selection.GetHeadPosition();
	CGeoDrawObj *pObj;
	while (pos != NULL)
	{	
		pObj = m_selection.GetNext(pos);
		if (pObj)
		{
			if (pObj->IsKindOf(RUNTIME_CLASS(CGeoTextObj)))
			{
				CGeoTextObj *pDrawText = (CGeoTextObj*)pObj;
				if (m_bWeight)
					pDrawText->m_lfFont.lfWeight = FW_NORMAL;
				else 
					pDrawText->m_lfFont.lfWeight = FW_BOLD;
				pDrawText->Invalidate(this);

				GetDocument()->SetModifiedFlag();
			}
		}
	}
}

void CGeoMapView::OnUpdateCharBold(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);
	m_bWeight = FALSE;
	pCmdUI->SetRadio(FALSE);
	
	if (m_selection.GetCount() > 1)
	{
		BOOL b = FALSE;
		POSITION pos = m_selection.GetHeadPosition();
		CGeoDrawObj *pObj;
		while (pos != NULL)
		{	
			pObj = m_selection.GetNext(pos);
			if (pObj)
			{
				if (pObj->IsKindOf(RUNTIME_CLASS(CGeoTextObj)))
				{
					CGeoTextObj *pDrawText = (CGeoTextObj*)pObj;
					if (pDrawText->m_lfFont.lfWeight >= 700)
						b = TRUE;
					else
						b = FALSE;
					pCmdUI->Enable(TRUE);
				}
			}
		}
		m_bWeight = b;
		pCmdUI->SetRadio(b);
	}
	
	if (m_selection.GetCount() == 1)
	{
		CGeoDrawObj *pObj = m_selection.GetTail();
		if (pObj->IsKindOf(RUNTIME_CLASS(CGeoTextObj)))
		{
			CGeoTextObj *pDrawText = (CGeoTextObj*)pObj;
			m_bWeight = (pDrawText->m_lfFont.lfWeight >= 700);
			pCmdUI->SetRadio(pDrawText->m_lfFont.lfWeight >= 700);// FW_BOLD
			pCmdUI->Enable(TRUE);
		}
		else
			pCmdUI->SetRadio(FALSE);
	}
}

void CGeoMapView::OnCharItalic() 
{
	if (m_selection.GetCount() == 0)
		return;
	
	POSITION pos = m_selection.GetHeadPosition();
	CGeoDrawObj *pObj;
	while (pos != NULL)
	{	
		pObj = m_selection.GetNext(pos);
		if (pObj)
		{
			if (pObj->IsKindOf(RUNTIME_CLASS(CGeoTextObj)))
			{
				CGeoTextObj *pDrawText = (CGeoTextObj*)pObj;
				if (m_bItalic)
					pDrawText->m_lfFont.lfItalic = FALSE;
				else 
					pDrawText->m_lfFont.lfItalic = TRUE;				
				pObj->Invalidate(this);

				GetDocument()->SetModifiedFlag();
			}
		}
	}
}

void CGeoMapView::OnUpdateCharItalic(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);
	m_bItalic = FALSE;
	pCmdUI->SetRadio(FALSE);
	
	if (m_selection.GetCount() > 1)
	{
		BOOL b = FALSE;
		POSITION pos = m_selection.GetHeadPosition();
		CGeoDrawObj *pObj;
		while (pos != NULL)
		{	
			pObj = m_selection.GetNext(pos);
			if (pObj)
			{
				if (pObj->IsKindOf(RUNTIME_CLASS(CGeoTextObj)))
				{
					CGeoTextObj *pDrawText = (CGeoTextObj*)pObj;
					if (pDrawText->m_lfFont.lfItalic)
						b = TRUE;
					else
						b = FALSE;
					pCmdUI->Enable(TRUE);
				}
			}
		}
		m_bItalic = b;
		pCmdUI->SetRadio(b);
	}
	
	if (m_selection.GetCount() == 1)
	{
		CGeoDrawObj *pObj = m_selection.GetTail();
		if (pObj->IsKindOf(RUNTIME_CLASS(CGeoTextObj)))
		{
			CGeoTextObj *pDrawText = (CGeoTextObj*)pObj;
			m_bItalic = (pDrawText->m_lfFont.lfItalic);
			pCmdUI->SetRadio(pDrawText->m_lfFont.lfItalic);//
			pCmdUI->Enable(TRUE);
		}
		else
			pCmdUI->SetRadio(FALSE);
	}
}

void CGeoMapView::OnCharUnderline() 
{
	if (m_selection.GetCount() == 0)
		return;
	
	POSITION pos = m_selection.GetHeadPosition();
	CGeoDrawObj *pObj;
	while (pos != NULL)
	{	
		pObj = m_selection.GetNext(pos);
		if (pObj)
		{
			if (pObj->IsKindOf(RUNTIME_CLASS(CGeoTextObj)))
			{
				CGeoTextObj *pDrawText = (CGeoTextObj*)pObj;
				if (m_bUnderline)
					pDrawText->m_lfFont.lfUnderline = FALSE;
				else 
					pDrawText->m_lfFont.lfUnderline = TRUE;				
				pObj->Invalidate(this);
				GetDocument()->SetModifiedFlag();
			}
		}
	}
}

void CGeoMapView::OnUpdateCharUnderline(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);
	m_bUnderline = FALSE;
	pCmdUI->SetRadio(FALSE);
	
	if (m_selection.GetCount() > 1)
	{
		BOOL b = FALSE;
		POSITION pos = m_selection.GetHeadPosition();
		CGeoDrawObj *pObj;
		while (pos != NULL)
		{	
			pObj = m_selection.GetNext(pos);
			if (pObj)
			{
				if (pObj->IsKindOf(RUNTIME_CLASS(CGeoTextObj)))
				{
					CGeoTextObj *pDrawText = (CGeoTextObj*)pObj;
					if (pDrawText->m_lfFont.lfUnderline)
						b = TRUE;
					else
						b = FALSE;
					pCmdUI->Enable(TRUE);
				}
			}
		}
		m_bUnderline = b;
		pCmdUI->SetRadio(b);
	}
	
	if (m_selection.GetCount() == 1)
	{
		CGeoDrawObj *pObj = m_selection.GetTail();
		if (pObj->IsKindOf(RUNTIME_CLASS(CGeoTextObj)))
		{
			CGeoTextObj *pDrawText = (CGeoTextObj*)pObj;
			m_bUnderline = (pDrawText->m_lfFont.lfUnderline);
			pCmdUI->SetRadio(pDrawText->m_lfFont.lfUnderline);//
			pCmdUI->Enable(TRUE);
		}
		else
			pCmdUI->SetRadio(FALSE);
	}
}

void CGeoMapView::OnCharColor() 
{
	if (m_selection.GetCount() == 0)
		return;
	
	COLORREF color = CMFCColorMenuButton::GetColorByCmdID (ID_CHAR_COLOR);
	
	POSITION pos = m_selection.GetHeadPosition();
	CGeoDrawObj *pObj;
	while (pos != NULL)
	{	
		pObj = m_selection.GetNext(pos);
		if (pObj)
		{
			if (pObj->IsKindOf(RUNTIME_CLASS(CGeoTextObj)))
			{
				CGeoTextObj *pDrawText = (CGeoTextObj*)pObj;
				if (pDrawText->m_clrText != color)
				{
					pDrawText->m_clrText = color;
					
					InvalObj(pDrawText);

					GetDocument()->SetModifiedFlag();
				}
			}
		}
	}
}

void CGeoMapView::OnUpdateCharColor(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);
	POSITION pos = m_selection.GetHeadPosition();
	CGeoDrawObj *pObj;
	while (pos != NULL)
	{	
		pObj = m_selection.GetNext(pos);
		if (pObj)
		{
			if (pObj->IsKindOf(RUNTIME_CLASS(CGeoTextObj)))
			{
				pCmdUI->Enable(TRUE);
			}
		}
	}
}

void CGeoMapView::OnCharWidth() 
{
	if (m_selection.GetCount() == 0)
		return;
	
	POSITION pos = m_selection.GetHeadPosition();
	CGeoDrawObj *pObj;
	while (pos != NULL)
	{	
		pObj = m_selection.GetNext(pos);
		if (pObj)
		{
			if (pObj->IsKindOf(RUNTIME_CLASS(CGeoTextObj)))
			{
				CGeoTextObj *pDrawText = (CGeoTextObj*)pObj;
				if (!pDrawText->m_lfFont.lfWidth)
					pDrawText->m_lfFont.lfWidth  = pDrawText->m_lfFont.lfHeight / 2;
				pDrawText->m_lfFont.lfWidth += 5;
				pDrawText->Invalidate(this);
				
				GetDocument()->SetModifiedFlag();
			}
		}
	}
}

void CGeoMapView::OnUpdateCharWidth(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);
	POSITION pos = m_selection.GetHeadPosition();
	CGeoDrawObj *pObj;
	while (pos != NULL)
	{	
		pObj = m_selection.GetNext(pos);
		if (pObj)
		{
			if (pObj->IsKindOf(RUNTIME_CLASS(CGeoTextObj)))
			{
				pCmdUI->Enable(TRUE);
			}
		}
	}
}

void CGeoMapView::OnCharNarrow() 
{
	if (m_selection.GetCount() == 0)
		return;
	
	POSITION pos = m_selection.GetHeadPosition();
	CGeoDrawObj *pObj;
	while (pos != NULL)
	{	
		pObj = m_selection.GetNext(pos);
		if (pObj)
		{
			if (pObj->IsKindOf(RUNTIME_CLASS(CGeoTextObj)))
			{
				CGeoTextObj *pDrawText = (CGeoTextObj*)pObj;
				if (!pDrawText->m_lfFont.lfWidth)
					pDrawText->m_lfFont.lfWidth  = pDrawText->m_lfFont.lfHeight / 2;
				pDrawText->m_lfFont.lfWidth -= 5;
				if (pDrawText->m_lfFont.lfWidth <= 0)
					pDrawText->m_lfFont.lfWidth += 5;
				pDrawText->Invalidate(this);
				
				GetDocument()->SetModifiedFlag();
			}
		}
	}
}

void CGeoMapView::OnUpdateCharNarrow(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);
	POSITION pos = m_selection.GetHeadPosition();
	CGeoDrawObj *pObj;
	while (pos != NULL)
	{	
		pObj = m_selection.GetNext(pos);
		if (pObj)
		{
			if (pObj->IsKindOf(RUNTIME_CLASS(CGeoTextObj)))
			{
				pCmdUI->Enable(TRUE);
			}
		}
	}
}

void CGeoMapView::OnParaLeft() 
{
	if (m_selection.GetCount() == 0)
		return;
	
	POSITION pos = m_selection.GetHeadPosition();
	CGeoDrawObj *pObj;
	while (pos != NULL)
	{	
		pObj = m_selection.GetNext(pos);
		if (pObj)
		{
			if (pObj->IsKindOf(RUNTIME_CLASS(CGeoTextObj)))
			{
				CGeoTextObj *pDrawText = (CGeoTextObj*)pObj;
				pDrawText->m_nFormat = DT_LEFT;
				pDrawText->Invalidate(this);
				
				GetDocument()->SetModifiedFlag();
			}
		}
	}
}

void CGeoMapView::OnUpdateParaLeft(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);
	pCmdUI->SetRadio(FALSE);
	POSITION pos = m_selection.GetHeadPosition();
	CGeoDrawObj *pObj;	
	while (pos != NULL)
	{	
		pObj = m_selection.GetNext(pos);
		if (pObj)
		{
			if (pObj->IsKindOf(RUNTIME_CLASS(CGeoTextObj)))
			{
				pCmdUI->Enable(TRUE);
				CGeoTextObj *pDrawText = (CGeoTextObj*)pObj;				
				
				if (pDrawText->m_nFormat != DT_LEFT)
				{
					pCmdUI->SetRadio(FALSE);
					break;
				}
				else
					pCmdUI->SetRadio(TRUE);
			}
		}
	}
}

void CGeoMapView::OnParaCenter() 
{
	if (m_selection.GetCount() == 0)
		return;
	
	POSITION pos = m_selection.GetHeadPosition();
	CGeoDrawObj *pObj;
	while (pos != NULL)
	{	
		pObj = m_selection.GetNext(pos);
		if (pObj)
		{
			if (pObj->IsKindOf(RUNTIME_CLASS(CGeoTextObj)))
			{
				CGeoTextObj *pDrawText = (CGeoTextObj*)pObj;
				pDrawText->m_nFormat = DT_CENTER;
				pDrawText->Invalidate(this);
				
				GetDocument()->SetModifiedFlag();
			}
		}
	}
}

void CGeoMapView::OnUpdateParaCenter(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);
	pCmdUI->SetRadio(FALSE);
	POSITION pos = m_selection.GetHeadPosition();
	CGeoDrawObj *pObj;
	while (pos != NULL)
	{	
		pObj = m_selection.GetNext(pos);
		if (pObj)
		{
			if (pObj->IsKindOf(RUNTIME_CLASS(CGeoTextObj)))
			{
				pCmdUI->Enable(TRUE);
				
				CGeoTextObj *pDrawText = (CGeoTextObj*)pObj;
				if (pDrawText->m_nFormat != DT_CENTER)
				{
					pCmdUI->SetRadio(FALSE);
					break;
				}
				else
					pCmdUI->SetRadio(TRUE);
			}
		}
	}
}

void CGeoMapView::OnParaRight() 
{
	if (m_selection.GetCount() == 0)
		return;
	
	POSITION pos = m_selection.GetHeadPosition();
	CGeoDrawObj *pObj;
	while (pos != NULL)
	{	
		pObj = m_selection.GetNext(pos);
		if (pObj)
		{
			if (pObj->IsKindOf(RUNTIME_CLASS(CGeoTextObj)))
			{
				CGeoTextObj *pDrawText = (CGeoTextObj*)pObj;
				pDrawText->m_nFormat = DT_RIGHT;
				pDrawText->Invalidate(this);
				
				GetDocument()->SetModifiedFlag();
			}
		}
	}
}

void CGeoMapView::OnUpdateParaRight(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);
	pCmdUI->SetRadio(FALSE);
	POSITION pos = m_selection.GetHeadPosition();
	CGeoDrawObj *pObj;
	while (pos != NULL)
	{	
		pObj = m_selection.GetNext(pos);
		if (pObj)
		{
			if (pObj->IsKindOf(RUNTIME_CLASS(CGeoTextObj)))
			{
				pCmdUI->Enable(TRUE);
				
				CGeoTextObj *pDrawText = (CGeoTextObj*)pObj;
				if (pDrawText->m_nFormat != DT_RIGHT)
				{
					pCmdUI->SetRadio(FALSE);
					break;
				}
				else
					pCmdUI->SetRadio(TRUE);
			}
		}
	}
}


void CGeoMapView::SetCharFormat(CCharFormat& cf)
{
	POSITION pos =m_selection.GetHeadPosition();
	CGeoDrawObj *pObj;
	while(pos!=NULL)
	{			
		pObj = m_selection.GetNext(pos);
		
		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoTextObj)) )
		{
			CGeoTextObj *pDrawText = (CGeoTextObj*)pObj;
			if(cf.dwMask & CFM_SIZE)
			{
				pDrawText->m_lfFont.lfHeight = long(cf.yHeight / 1440.0 * 254);
			}
			else
			{
				pDrawText->m_lfFont.lfCharSet = cf.bCharSet;
				strcpy_s(pDrawText->m_lfFont.lfFaceName, cf.szFaceName);
			}
			InvalObj(pDrawText);
			
			GetDocument()->SetModifiedFlag();
		}
		
	}
}

void CGeoMapView::UpdateFormatBar()
{
	CGeoTextObj *pDrawText = NULL;
	
	if( m_selection.GetCount() == 1 )
	{
		CGeoDrawObj *pObj = m_selection.GetTail();
		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoTextObj)) )
			pDrawText = (CGeoTextObj*)pObj;
	}	
	
	CString strFontName="";
	
	// get the current font from the view and update
	//	WPD_CHARFORMAT cf = pView->GetCharFormatSelection ();
	
	CObList listButtons;
	if (CMFCToolBar::GetCommandButtons (IDC_FONTNAME, listButtons) > 0)
	{
		for (POSITION posCombo = listButtons.GetHeadPosition (); posCombo != NULL;)
		{
			CMFCToolBarFontComboBox* pCombo = 
				DYNAMIC_DOWNCAST (CMFCToolBarFontComboBox, listButtons.GetNext (posCombo));
			
			if (pCombo != NULL && !pCombo->HasFocus ())
			{
				if( pDrawText )
				{
					
#if _MSC_VER >= 1300
					strFontName = pDrawText->m_lfFont.lfFaceName;
#else
					strFontName = A2T(pDrawText->m_lfFont.lfFaceName);
#endif
					pCombo->SetFont (strFontName, DEFAULT_CHARSET, TRUE);
					
				}
				else
					pCombo->SetText(_T(""));
			}
		}
	}
	
	if (CMFCToolBar::GetCommandButtons (IDC_FONTSIZE, listButtons) > 0)
	{
		for (POSITION posCombo = listButtons.GetHeadPosition (); posCombo != NULL;)
		{
			CMFCToolBarFontSizeComboBox* pCombo = 
				DYNAMIC_DOWNCAST (CMFCToolBarFontSizeComboBox, listButtons.GetNext (posCombo));
			
			if (pCombo != NULL && !pCombo->HasFocus ())
			{
				if (!strFontName.IsEmpty () && pCombo->GetCount () == 0)
				{
					pCombo->RebuildFontSizes (strFontName);
				}
				
				if( pDrawText )
					pCombo->SetTwipSize( labs(pDrawText->m_lfFont.lfHeight) / 100.0 * 567 );
				else
					pCombo->SetTwipSize(-1);
			}
		}
	}
	
	return;	
}

// 文本工具
void CGeoMapView::OnDrawBaseText() 
{
	CToolBase::c_drawShape = CToolBase::text;
}

void CGeoMapView::OnUpdateDrawBaseText(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(CToolBase::c_drawShape == CToolBase::text);
}


/*********************************************************************
* 函数名称:CLayerContrastView::OnEndTextEdit
* 说明:结束文字编辑
* 入口参数:
* 返回值:
* void  -- 无
* 作者: 张军(ZhangJun) 
* 时间 : 2001 -08 -23 09:17:26 
*********************************************************************/
LRESULT CGeoMapView::OnEndTextEdit(WPARAM wp, LPARAM lp)
{
	m_pDrawTextObj->m_bEditing = FALSE;
	CWnd *pWnd = (CWnd*) wp;
	pWnd->GetWindowText(m_pDrawTextObj->m_strText);
	
	InvalObj(m_pDrawTextObj);

	GetDocument()->SetModifiedFlag();
	return 0;
}

void CGeoMapView::OnCancelMode() 
{
	CScrollView::OnCancelMode();
	
	// TODO: Add your message handler code here
	
}

void CGeoMapView::OnCaptureChanged(CWnd *pWnd) 
{
	// TODO: Add your message handler code here
	
	CScrollView::OnCaptureChanged(pWnd);
}

void CGeoMapView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	CScrollView::OnPrint(pDC, pInfo);
}

void CGeoMapView::OnZoomSetPageCenter() 
{
	CGeoMapDoc *pDoc = GetDocument();
	
	CRect rect = pDoc->m_pCurDrawPage->GetPageRect();
	
	CRectEx rt = pDoc->m_pCurDrawPage->CoordinateTransfer(rect);

	CPoint2D point;
	point.x = (rt.left + rt.right)/2.0;
	point.y = (rt.top + rt.bottom)/2.0;

	pDoc->m_pCurDrawPage->m_ptCoordOrig.x = point.x;
	pDoc->m_pCurDrawPage->m_ptCoordOrig.y = point.y;

	pDoc->m_pCurDrawPage->OnScaleChanged();

	Invalidate(FALSE);
}

void CGeoMapView::OnUpdateZoomSetPageCenter(CCmdUI* pCmdUI) 
{
	CGeoMapDoc *pDoc = GetDocument();
	
	CRect rect = pDoc->m_pCurDrawPage->GetPageRect();
	
	if( rect.IsRectEmpty() )
		pCmdUI->Enable( FALSE );
	else
		pCmdUI->Enable( TRUE );
}

void CGeoMapView::OnZoomPageMove() 
{
	CToolBase::c_drawShape = CToolBase::pageMove;
}

void CGeoMapView::OnUpdateZoomPageMove(CCmdUI* pCmdUI) 
{
	CGeoMapDoc *pDoc = GetDocument();

	CRect rect = pDoc->m_pCurDrawPage->GetPageRect();

	if( rect.IsRectEmpty() )
		pCmdUI->Enable( FALSE );
	else
		pCmdUI->Enable( TRUE );

	pCmdUI->SetRadio(CToolBase::c_drawShape == CToolBase::pageMove);
}

void CGeoMapView::OnDrawBaseLine() 
{
	CToolBase::c_drawShape = CToolBase::line;
}

void CGeoMapView::OnUpdateDrawBaseLine(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(CToolBase::c_drawShape == CToolBase::normal);
}


void CGeoMapView::OnDrawNormal()
{
	// TODO: 在此添加命令处理程序代码
	CToolBase::c_drawShape = CToolBase::normal;
}

void CGeoMapView::OnUpdateDrawNormal(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetRadio(CToolBase::c_drawShape == CToolBase::normal);
}

void CGeoMapView::OnDrawEnableMove() 
{
	if( selectTool.m_bEnableMove )
		selectTool.m_bEnableMove = FALSE;
	else
		selectTool.m_bEnableMove = TRUE;
}

void CGeoMapView::OnUpdateDrawEnableMove(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(selectTool.m_bEnableMove );
}

void CGeoMapView::OnDrawEditProperties() 
{
	if (m_selection.GetCount() == 1 
		&& CToolBase::c_drawShape == CToolBase::selection)
	{
		CToolBase* pTool = CToolBase::FindTool(CToolBase::c_drawShape);
		ASSERT(pTool != NULL);
		pTool->OnEditProperties(this);
	}
}

void CGeoMapView::OnUpdateDrawEditProperties(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_selection.GetCount() == 1 &&
				   CToolBase::c_drawShape == CToolBase::selection);
}

//////////////////////////////////////////////////////////////////////////

void CGeoMapView::OnDrawBasePicture() 
{
	// TODO: Add your command handler code here
	CToolBase::c_drawShape = CToolBase::pictrue;
}

void CGeoMapView::OnUpdateDrawBasePicture(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(CToolBase::c_drawShape == CToolBase::pictrue);
}

void CGeoMapView::OnDrawScaleRuler() 
{
	CToolBase::c_drawShape = CToolBase::scaleRuler;
}

void CGeoMapView::OnUpdateDrawScaleRuler(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(CToolBase::c_drawShape == CToolBase::scaleRuler);
}

BOOL CGeoMapView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	return CScrollView::OnEraseBkgnd(pDC);
}


void CGeoMapView::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CScrollView::OnClose();
}

// 添加坐标校正点
void CGeoMapView::OnAddCtrlPoint()
{
	// TODO: 在此添加命令处理程序代码
	
	CGeoPictrueObj *pPictrue = (CGeoPictrueObj*) m_selection.GetTail();
	if( m_eOperation == opAddCtrlPoint)
	{
		m_eOperation = opNone;
		pPictrue->m_bEditPoint = FALSE;
	}
	else
	{
		m_eOperation = opAddCtrlPoint;
		m_hCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_ADD_CTRL_POINT);
		SetCursor(m_hCursor);		
		pPictrue->m_bEditPoint = TRUE;		
	}

	pPictrue->Invalidate(this);
}

void CGeoMapView::OnUpdateAddCtrlPoint(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if( m_selection.GetCount() < 1 )
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	CGeoDrawObj *pObj = (CGeoDrawObj*) m_selection.GetTail();	

	if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPictrueObj)))
	{
		CGeoPictrueObj *pPictrue = (CGeoPictrueObj *)pObj;
		if( pPictrue->m_bLocked )
			pCmdUI->Enable( FALSE );
		else
			pCmdUI->Enable( TRUE );
		pCmdUI->SetRadio(m_eOperation == opAddCtrlPoint);
	}
	else
		pCmdUI->Enable(FALSE);
}

void CGeoMapView::OnDeleteCtrlPoint()
{
	// TODO: 在此添加命令处理程序代码
	CGeoPictrueObj *pPictrue = (CGeoPictrueObj*) m_selection.GetTail();
	if( m_eOperation == opDelCtrlPoint)
	{
		m_eOperation = opNone;
		pPictrue->m_bEditPoint = FALSE;
	}
	else
	{
		m_eOperation = opDelCtrlPoint;
		m_hCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_DEL_CTRL_POINT);
		SetCursor(m_hCursor);		
		pPictrue->m_bEditPoint = TRUE;		
	}

	pPictrue->Invalidate(this);
}

void CGeoMapView::OnUpdateDeleteCtrlPoint(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if( m_selection.GetCount() < 1 )
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	CGeoDrawObj *pObj = (CGeoDrawObj*) m_selection.GetTail();
	
	if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPictrueObj)))
	{
		CGeoPictrueObj *pPictrue = (CGeoPictrueObj *)pObj;
		if( pPictrue->m_ptCtrl.GetSize() < 1 )
			pCmdUI->Enable( FALSE );
		else
		{
			if( pPictrue->m_bLocked )
				pCmdUI->Enable( FALSE );
			else
				pCmdUI->Enable( TRUE );
			pCmdUI->SetRadio(m_eOperation == opDelCtrlPoint);
		}
	}
	else
		pCmdUI->Enable(FALSE);
}

void CGeoMapView::OnEditCtrlPoint()
{
	// TODO: 在此添加命令处理程序代码
	CGeoPictrueObj *pPictrue = (CGeoPictrueObj*) m_selection.GetTail();
	if( m_eOperation == opEditCtrlPoint)
	{
		m_eOperation = opNone;
		pPictrue->m_bEditPoint = FALSE;
	}
	else
	{
		m_eOperation = opEditCtrlPoint;
		m_hCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_EDIT_CTRL_POINT);
		SetCursor(m_hCursor);		
		pPictrue->m_bEditPoint = TRUE;		
	}

	pPictrue->Invalidate(this);
}

void CGeoMapView::OnUpdateEditCtrlPoint(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码

	if( m_selection.GetCount() < 1 )
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	CGeoDrawObj *pObj = (CGeoDrawObj*) m_selection.GetTail();
	
	if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPictrueObj)))
	{
		CGeoPictrueObj *pPictrue = (CGeoPictrueObj *)pObj;
		if( pPictrue->m_ptCtrl.GetSize() < 1 )
			pCmdUI->Enable( FALSE );
		else
		{
			if( pPictrue->m_bLocked )
				pCmdUI->Enable( FALSE );
			else
				pCmdUI->Enable( TRUE );
			pCmdUI->SetRadio(m_eOperation == opEditCtrlPoint);
		}
	}
	else
		pCmdUI->Enable(FALSE);
}

// 导出曲线
void CGeoMapView::OnExportCurve()
{
	// TODO: 在此添加命令处理程序代码
	
	CFileDialog dlg(FALSE,_T("csv"),0,4|2,_T("逗号分隔符文件 (*.csv)|*.csv||"));
	CString strFileName;
	if( dlg.DoModal() != IDOK )
		return;
	
	strFileName = dlg.GetPathName();

	CStdioFile file(strFileName, CFile::modeCreate|CFile::modeWrite|CFile::typeText);

	POSITION pos = m_selection.GetHeadPosition();
	while ( pos!=NULL )
	{
		CGeoDrawObj *pObj = m_selection.GetNext(pos);
		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPolyObj)))
		{
			CGeoPolyObj* pPoly = (CGeoPolyObj*)pObj;
			
			for (int i=0; i<pPoly->m_nPoints;i++)
			{
				CPoint2D geoPoint = pPoly->CoordinateTransfer(pPoly->m_points[i]);
				CString strTmp;
				strTmp.Format(_T("%g,%g\n"),geoPoint.x, geoPoint.y);

				file.WriteString(strTmp);
			}

			file.WriteString(_T("EOM\n"));
		}
	}

	file.Close();
}

void CGeoMapView::OnUpdateExportCurve(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(FALSE);

	if(m_selection.GetCount() >= 1)
	{
		POSITION pos = m_selection.GetHeadPosition();
		while ( pos!=NULL )
		{
			CGeoDrawObj *pObj = m_selection.GetNext(pos);
			if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPolyObj)))
			{
				pCmdUI->Enable(TRUE);
				return;
			}
		}		
	}
	else
		pCmdUI->Enable(FALSE);
}

void CGeoMapView::OnExportLine()
{
	// TODO: 在此添加命令处理程序代码

	CFileDialog dlg(FALSE,_T("csv"),0,4|2,_T("逗号分隔符文件 (*.csv)|*.csv||"));
	CString strFileName;
	if( dlg.DoModal() != IDOK )
		return;

	strFileName = dlg.GetPathName();

	CStdioFile file(strFileName, CFile::modeCreate|CFile::modeWrite|CFile::typeText);

	POSITION pos = m_selection.GetHeadPosition();
	while ( pos!=NULL )
	{
		CGeoDrawObj *pObj = m_selection.GetNext(pos);
		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoLineObj)))
		{
			CGeoLineObj* pLine = (CGeoLineObj*)pObj;

			for (int i=0; i<2;i++)
			{
				CPoint2D geoPoint = pLine->CoordinateTransfer(pLine->m_points[i]);
				CString strTmp;
				strTmp.Format(_T("%g,%g\n"),geoPoint.x, geoPoint.y);

				file.WriteString(strTmp);
			}

			file.WriteString(_T("EOM\n"));
		}
	}

	file.Close();
}

void CGeoMapView::OnUpdateExportLine(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码

	if(m_selection.GetCount() >= 1)
	{
		POSITION pos = m_selection.GetHeadPosition();
		while ( pos!=NULL )
		{
			CGeoDrawObj *pObj = m_selection.GetNext(pos);
			if( pObj->IsKindOf(RUNTIME_CLASS(CGeoLineObj)))
			{
				pCmdUI->Enable(TRUE);
				return;
			}
		}
	}
	else
		pCmdUI->Enable(FALSE);
}

// 排序夹层线
void CGeoMapView::SortInterLayerLine(CGeoDrawObjList &polys, CGeoLineObj *pLine)
{	
	CArray<tagPrCorIndex, tagPrCorIndex&> pts;
	POSITION pos = polys.GetHeadPosition();
	while(pos != NULL)
	{
		CGeoPolyObj *pPoly = (CGeoPolyObj *)polys.GetNext(pos);
		
		for (int i=1;i<pPoly->m_nPoints; i++)
		{	
			CPoint2D p2[2];

			p2[0] = pPoly->m_geoPoints[i-1];
			p2[1] = pPoly->m_geoPoints[i];

			CPoint2D p;
			if( Intersect(pLine->m_geoPoints, p2, &p, false) )
			{
				tagPrCorIndex idx;
				idx.dDis = GetPointDistance(pLine->m_geoPoints[0], p);
				idx.pPoly = pPoly;
				pts.Add(idx);
				break;
			}
		}
	}

	
	qsort(pts.GetData(),pts.GetSize(),sizeof(tagPrCorIndex),compareInterLayer);

	CGeoDrawObjList select;

	int nSize = pts.GetSize();
	for (int i=0; i<nSize; i++)
		select.AddTail(pts[i].pPoly);

	polys.RemoveAll();

	pos = select.GetHeadPosition();
	while(pos!=NULL)
	{
		CGeoDrawObj *pObj = select.GetNext(pos);
		polys.AddTail(pObj);
	}
}

// 排序法线
void CGeoMapView::SortNormalLine(CGeoDrawObjList &lines, CGeoPolyObj *pPoly)
{	
	CGeoDrawObjList select;

	for (int i=1;i<pPoly->m_nPoints; i++)
	{	
		CPoint2D p2[2];

		p2[0] = pPoly->m_geoPoints[i-1];
		p2[1] = pPoly->m_geoPoints[i];
		
		CArray<tagPrNormalIndex, tagPrNormalIndex&> ls;
		
		POSITION pos = lines.GetHeadPosition();	
		while(pos != NULL)
		{
			CGeoLineObj *pLine = (CGeoLineObj *)lines.GetNext(pos);

			CPoint2D p;
			if( Intersect(pLine->m_geoPoints, p2, &p, false) )
			{
				tagPrNormalIndex idx;
				idx.pLine = pLine;
				idx.dDis = GetPointDistance(p2[0],p);
				ls.Add(idx);
			}
		}	

		if( ls.GetSize()==1)
		{
			select.AddTail(ls[0].pLine);
		}
		if( ls.GetSize()>1)
		{
			qsort(ls.GetData(), ls.GetSize(),sizeof(tagPrNormalIndex),compareNormal);
			for (int j=0;j<ls.GetSize(); j++)
			{
				select.AddTail(ls[j].pLine);
			}
		}
	}
	
	lines.RemoveAll();

	POSITION pos = select.GetHeadPosition();
	while(pos!=NULL)
	{
		CGeoDrawObj *pObj = select.GetNext(pos);
		lines.AddTail(pObj);
	}
}

// 删除没有相交的法线
void CGeoMapView::DeleteNoCorssNormal(CGeoDrawObjList &polys, CGeoDrawObjList &Lines)
{
	CGeoDrawObjList select;

	POSITION pos = Lines.GetHeadPosition();
	while(pos != NULL)
	{
		CGeoLineObj *pLine = (CGeoLineObj *)Lines.GetNext(pos);
		
		int num = 0; // 相交层数
		POSITION pos2 = polys.GetHeadPosition();
		while(pos2!=NULL)
		{
			CGeoPolyObj *pPoly = (CGeoPolyObj*)polys.GetNext(pos2);
			for (int i=1;i<pPoly->m_nPoints; i++)
			{	
				CPoint2D p2[2];

				p2[0] = pPoly->m_geoPoints[i-1];
				p2[1] = pPoly->m_geoPoints[i];

				CPoint2D p;
				if( Intersect(pLine->m_geoPoints, p2, &p) )
				{
					num ++;
					break;
				}
			}
		}

		if( num == polys.GetCount() ) //  相交层数等于总层数，则添加
			select.AddTail(pLine);
	}

	Lines.RemoveAll();
	
	pos = select.GetHeadPosition();
	while(pos!=NULL)
	{
		CGeoDrawObj *pObj = select.GetNext(pos);
		Lines.AddTail(pObj);
	}
}

// 获取穿过夹层线的首条法线
CGeoLineObj * CGeoMapView::GetCrossFirstNormal(CGeoDrawObjList &Lines, CGeoPolyObj *pPoly)
{	
	CGeoDrawObjList select;

	for (int i=1;i<pPoly->m_nPoints; i++)
	{	
		CPoint2D p2[2];

		p2[0] = pPoly->m_geoPoints[i-1];
		p2[1] = pPoly->m_geoPoints[i];

		POSITION pos = Lines.GetHeadPosition();

		while(pos != NULL)
		{
			CGeoLineObj *pLine = (CGeoLineObj *)Lines.GetNext(pos);

			CPoint2D p;
			if( Intersect(pLine->m_geoPoints, p2, &p) )
			{
				return pLine;
			}
		}
	}

	return NULL;
}


int CGeoMapView::GetPolyDirection(CGeoDrawObjList &Lines, CGeoPolyObj *pPoly)
{	
	CGeoDrawObjList select;

	int num = 0;
	BOOL bFlag = TRUE;
	int nFirst;
	for (int i=1;i<pPoly->m_nPoints; i++)
	{	
		CPoint2D p2[2];

		p2[0] = pPoly->m_geoPoints[i-1];
		p2[1] = pPoly->m_geoPoints[i];

		int k = 0;
		POSITION pos = Lines.GetHeadPosition();
		while(pos != NULL)
		{
			k++;
			CGeoLineObj *pLine = (CGeoLineObj *)Lines.GetNext(pos);
			CPoint2D p;
			if( Intersect(pLine->m_geoPoints, p2, &p) )
			{
				if( bFlag )
					nFirst = k;
				else
				{
					num += (k-nFirst);
					nFirst = k;
				}

				bFlag = FALSE;
				break;
			}
		}
	}

	return num;
}

// 获取穿过法线的首条夹层线
CGeoPolyObj* CGeoMapView::GetCrossFirstLayer(CGeoDrawObjList &polys, CGeoLineObj *pLine)
{
	CArray<tagPrCorIndex, tagPrCorIndex&> pts;
	POSITION pos = polys.GetHeadPosition();
	while(pos != NULL)
	{
		CGeoPolyObj *pPoly = (CGeoPolyObj *)polys.GetNext(pos);

		for (int i=1;i<pPoly->m_nPoints; i++)
		{	
			CPoint2D p2[2];

			p2[0] = pPoly->m_geoPoints[i-1];
			p2[1] = pPoly->m_geoPoints[i];

			CPoint2D p;
			if( Intersect(pLine->m_geoPoints, p2, &p, false) )
			{
				tagPrCorIndex idx;
				idx.dDis = GetPointDistance(pLine->m_geoPoints[0], p);
				idx.pPoly = pPoly;
				pts.Add(idx);

				TRACE(_T("%lf\n"),idx.dDis);
				break;
			}
		}
	}

	double dDis = DBL_MAX;

	int nSize = pts.GetSize();

	int idx = 0;

	for (int i=0; i<nSize; i++)
	{
		double d = pts[i].dDis;
		if( d < dDis)
		{
			dDis = d;
			idx = i;
		}
	}

	return pts[idx].pPoly;
}

// 生成侧积夹层横向切线
//void CGeoMapView::OnGenerateInterlayer()
//{
	// TODO: 在此添加命令处理程序代码

	/*
	CGeoMapDoc * pDoc = GetDocument();

	CGeoDrawObjList polys;		// 曲线
	CGeoDrawObjList lines;		// 法线

	
	POSITION pos = m_selection.GetHeadPosition();
	while(pos!=NULL)
	{
		CGeoDrawObj *pObj = m_selection.GetNext(pos);

		CDrawLayer *pLayer = pObj->GetParentLayer();
		if( pLayer->m_eLayerType != CDrawLayer::interlayer )
			continue;

		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPolyObj)))
			polys.AddTail(pObj);
		else if( pObj->IsKindOf(RUNTIME_CLASS(CGeoLineObj)))
			lines.AddTail(pObj);
	}

	//////////////////////////////////////////////////////////////////////////
	
	if( polys.GetCount() < 2 )
	{
		AfxMessageBox(_T("夹层线少于2条!"));
		return;
	}

	if( lines.GetCount() < 1 )
	{
		AfxMessageBox(_T("主法线不能少于1条!"));
		return;
	}

	TRACE(_T("%d\n"),lines.GetCount());
	// 删除没有相交的法线
	DeleteNoCorssNormal(polys, lines);
	TRACE(_T("%d\n"),lines.GetCount());
	//////////////////////////////////////////////////////////////////////////
	// 夹层线排序
	//	
	CGeoLineObj *pLine = (CGeoLineObj *)lines.GetHead();
	SortInterLayerLine(polys, pLine);
	TRACE(_T("%d\n"),lines.GetCount());

	//////////////////////////////////////////////////////////////////////////
	// 法线排序

	CGeoPolyObj *pPoly = (CGeoPolyObj *)polys.GetHead();
	SortNormalLine(lines, pPoly);
	TRACE(_T("%d\n"),lines.GetCount());

	//////////////////////////////////////////////////////////////////////////
	// 检查夹层线方向
	pLine = GetCrossFirstNormal(lines, pPoly);
	pos = polys.GetHeadPosition();
	while (pos!=NULL)
	{
		pPoly = (CGeoPolyObj *)polys.GetNext(pos);
		CGeoLineObj *p = GetCrossFirstNormal(lines, pPoly);
		if( p != pLine ) // 如果不相等，调整夹层线点顺序
			pPoly->ReversePoints();
	}


	//////////////////////////////////////////////////////////////////////////
	// 先求首尾夹层线与法线的交点
	//
	// 首尾夹层线与法线交点集
	std::vector<CPoint2D> *insPt = new std::vector<CPoint2D> [polys.GetCount()];
	double *dFirstCurvarture = new double[polys.GetCount()];	// 每条夹层线开始段的曲率
	double *dEndCurvarture = new double[polys.GetCount()];		// 每条夹层线最后段的曲率

	// 首条法线为夹层线首点相连
	// 尾条法线为夹层线尾点相连

	// 求每条法线
	for(int i=0; i<polys.GetCount(); i++)
	{
		CGeoPolyObj *pPoly = (CGeoPolyObj *)polys.GetAt(polys.FindIndex(i));
		insPt[i].push_back(pPoly->m_geoPoints[0]);	// 加首条法线

		long nSize = GetSpline3Point(pPoly->m_geoPoints,pPoly->m_nPoints);

		if( nSize > 0 )
		{
			CPoint2D *pt = new CPoint2D[nSize];
			GetSpline3Point(pPoly->m_geoPoints,pPoly->m_nPoints,FALSE,pt);
			
			int iSeek = 1;

			for (int j=0; j<lines.GetCount();j++)
			{
				CGeoLineObj *pLine = (CGeoLineObj *)lines.GetAt(lines.FindIndex(j));
				std::vector<CPoint2D> pts;

				int n = insPt[i].size();
				pts.push_back(insPt[i][n-1]);

				for (long k=iSeek;k<nSize; k++)
				{
					CPoint2D p2[2];

					p2[0] = pt[k-1];
					p2[1] = pt[k];

					pts.push_back(p2[0]);

					CPoint2D p;

					if( Intersect(pLine->m_geoPoints, p2, &p) )
					{	
						pts.push_back(p);
						iSeek = k+1;

						if( j==0 )
							dFirstCurvarture[i] = GetMaxCurvarture(pts,true);

						Bresenham(pts, insPt[i], m_iNormals);
						
						insPt[i].push_back(p);

						// 求末段曲率
						if( j==lines.GetCount()-1)
						{
							pts.erase(pts.begin(), pts.end());
							
							pts.push_back(p);
							
							for (;k<nSize; k++)
								pts.push_back(pt[k]);

							dEndCurvarture[i] = GetMaxCurvarture(pts,false);

							Bresenham(pts, insPt[i], m_iNormals);

							insPt[i].push_back(pt[nSize-1]); // 加末条法线
						}
						break;
					}
				}
			}
			delete []pt;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	std::vector<CPoint2D> *insPt1 = new std::vector<CPoint2D> [(polys.GetCount()-1)*2];
	for (int i=0;i<polys.GetCount();i++)
	{
		if( i==0 )
		{
			insPt1[i].resize(insPt[0].size());
			std::copy(insPt[i].begin(),insPt[i].end(), insPt1[i].begin());
		}
		else if(i == polys.GetCount()-1 )
		{
			insPt1[(polys.GetCount()-1)*2-1].resize(insPt[0].size());
			std::copy(insPt[i].begin(),insPt[i].end(), insPt1[(polys.GetCount()-1)*2-1].begin());
		}
		else
		{
			insPt1[2*i-1].resize(insPt[0].size());
			std::copy(insPt[i].begin(),insPt[i].end(), insPt1[2*i-1].begin());

			insPt1[2*i].resize(insPt[0].size());
			std::copy(insPt[i].begin(),insPt[i].end(), insPt1[2*i].begin());			
		}
	}

	delete[]insPt;

	// 判断首末法相交情况
	for(int i=0; i<polys.GetCount()-1; i++)
	{
		CGeoPolyObj *pPoly1 = (CGeoPolyObj *)polys.GetAt(polys.FindIndex(i));
		CGeoPolyObj *pPoly2 = (CGeoPolyObj *)polys.GetAt(polys.FindIndex(i+1));		
		//首
		CPoint2D p1[2];
		p1[0] = pPoly1->m_geoPoints[0];
		p1[1] = pPoly2->m_geoPoints[0];
		int j;
		for ( j=1;j<m_iNormals; j++)
		{
			CPoint2D p2[2];
			p2[0] = insPt1[2*i][j];
			p2[1] = insPt1[2*i+1][j];

			CPoint2D p;
			if( Intersect(p1, p2, &p, false))//是否相交
			{
				if( dFirstCurvarture[i] < dFirstCurvarture[i+1])
					insPt1[2*i][j] = pPoly1->m_geoPoints[0];
				else
					insPt1[2*i+1][j] = pPoly2->m_geoPoints[0];
			}
			else
				break;
		}

		int k=j;
		for (;j<m_iNormals; j++)
		{
			if( dFirstCurvarture[i] < dFirstCurvarture[i+1])
			{
				insPt1[2*i][j].x = pPoly1->m_geoPoints[0].x+(-pPoly1->m_geoPoints[0].x+insPt1[2*i][m_iNormals].x)/(m_iNormals-k+1)*(j-k+1);
				insPt1[2*i][j].y = pPoly1->m_geoPoints[0].y+(-pPoly1->m_geoPoints[0].y+insPt1[2*i][m_iNormals].y)/(m_iNormals-k+1)*(j-k+1);
			}
			else
			{
				insPt1[2*i+1][j].x = pPoly2->m_geoPoints[0].x+(-pPoly2->m_geoPoints[0].x+insPt1[2*i+1][m_iNormals].x)/(m_iNormals-k+1)*(j-k+1);
				insPt1[2*i+1][j].y = pPoly2->m_geoPoints[0].y+(-pPoly2->m_geoPoints[0].y+insPt1[2*i+1][m_iNormals].y)/(m_iNormals-k+1)*(j-k+1);
			}
		}

		// 末

		p1[0] = pPoly1->m_geoPoints[pPoly1->m_nPoints-1];
		p1[1] = pPoly2->m_geoPoints[pPoly2->m_nPoints-1];
		int n = insPt1[0].size();
		for (j=1;j<m_iNormals; j++)
		{
			CPoint2D p2[2];
			p2[0] = insPt1[2*i][n-1-j];
			p2[1] = insPt1[2*i+1][n-1-j];

			CPoint2D p;
			if( Intersect(p1, p2, &p, false))//是否相交
			{
				if( dEndCurvarture[i] < dEndCurvarture[i+1])
					insPt1[2*i][n-1-j] = pPoly1->m_geoPoints[pPoly1->m_nPoints-1];
				else
					insPt1[2*i+1][n-1-j] = pPoly2->m_geoPoints[pPoly2->m_nPoints-1];
			}
			else
				break;
		}

		k=j;
		for (;j<m_iNormals; j++)
		{
			if( dEndCurvarture[i] < dEndCurvarture[i+1])
			{
				insPt1[2*i][n-1-j].x = pPoly1->m_geoPoints[pPoly1->m_nPoints-1].x+(-pPoly1->m_geoPoints[pPoly1->m_nPoints-1].x+insPt1[2*i][n-1-m_iNormals].x)/(m_iNormals-k+1)*(j-k+1);
				insPt1[2*i][n-1-j].y = pPoly1->m_geoPoints[pPoly1->m_nPoints-1].y+(-pPoly1->m_geoPoints[pPoly1->m_nPoints-1].y+insPt1[2*i][n-1-m_iNormals].y)/(m_iNormals-k+1)*(j-k+1);
			}
			else
			{
				insPt1[2*i+1][n-1-j].x = pPoly2->m_geoPoints[pPoly2->m_nPoints-1].x+(-pPoly2->m_geoPoints[pPoly2->m_nPoints-1].x+insPt1[2*i+1][n-1-m_iNormals].x)/(m_iNormals-k+1)*(j-k+1);
				insPt1[2*i+1][n-1-j].y = pPoly2->m_geoPoints[pPoly2->m_nPoints-1].y+(-pPoly2->m_geoPoints[pPoly2->m_nPoints-1].y+insPt1[2*i+1][n-1-m_iNormals].y)/(m_iNormals-k+1)*(j-k+1);
			}
		}
	}
	
	delete []dFirstCurvarture;
	delete []dEndCurvarture;
	
	//////////////////////////////////////////////////////////////////////////
	// 生成交点
	
	std::vector<CPoint2D> *insPt2 = new std::vector<CPoint2D> [2*(polys.GetCount()-1)];//交点集

	int n = insPt1[0].size();
	for (int j=0; j<n; j++)
	{
		for (int i=0; i < polys.GetCount()-1; i++)
		{
			CPoint2D p1[2];

			p1[0] = insPt1[2*i][j];
			p1[1] = insPt1[2*i+1][j];

			double dDis = sqrt((p1[0].x-p1[1].x)*(p1[0].x-p1[1].x)+(p1[0].y-p1[1].y)*(p1[0].y-p1[1].y));
			p1[0] = LineExtension(p1[0],p1[1],dDis, true);
			p1[1] = LineExtension(p1[0],p1[1],dDis, false);

			CGeoPolyObj *pPoly1 = (CGeoPolyObj *)polys.GetAt(polys.FindIndex(i));
			
			
			long nSize = GetSpline3Point(pPoly1->m_geoPoints,pPoly1->m_nPoints);

			if( nSize > 0 )
			{
				CPoint2D *pt = new CPoint2D[nSize+2];
				GetSpline3Point(pPoly1->m_geoPoints,pPoly1->m_nPoints,FALSE,pt);
				memmove(&pt[1], &pt[0], nSize);

				pt[0] = pPoly1->m_geoPoints[0];
				pt[nSize+1] = pPoly1->m_geoPoints[pPoly1->m_nPoints-1];
				
				for (long k=1;k<nSize; k++)
				{	
					CPoint2D p2[2];

					p2[0] = pt[k-1];
					p2[1] = pt[k];

					CPoint2D p;

					if( Intersect(p1, p2, &p, false) )
					{
						insPt2[2*i].push_back(p);
						break;
					}
				}

				delete []pt;
			}

			CGeoPolyObj *pPoly2 = (CGeoPolyObj *)polys.GetAt(polys.FindIndex(i+1));

			nSize = GetSpline3Point(pPoly2->m_geoPoints,pPoly2->m_nPoints);

			if( nSize > 0 )
			{
				CPoint2D *pt = new CPoint2D[nSize+2];
				GetSpline3Point(pPoly2->m_geoPoints,pPoly2->m_nPoints,FALSE,pt);
				memmove(&pt[1], &pt[0], nSize);

				pt[0] = pPoly2->m_geoPoints[0];
				pt[nSize+1] = pPoly2->m_geoPoints[pPoly2->m_nPoints-1];

				for (long k=1;k<nSize; k++)
				{	
					CPoint2D p2[2];

					p2[0] = pt[k-1];
					p2[1] = pt[k];

					CPoint2D p;

					if( Intersect(p1, p2, &p, false) )
					{
						insPt2[2*i+1].push_back(p);
						break;
					}
				}

				delete []pt;
			}

		}
	}
	
	delete []insPt1;


	//////////////////////////////////////////////////////////////////////////
	// 写法线
	{
		CString strFileName = newGUID();
		strFileName += _T(".dat");

		CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
		CString strTargetName = pMF->GetProjectDatPath();
		strTargetName += _T("\\files\\");
		strTargetName += strFileName;

		CFile file(strTargetName, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
		CArchive ar(&file, CArchive::store);

		int nRows = (3*(polys.GetCount()-1))*insPt2[0].size();
		int nCols = 4;

		ar << nRows;
		ar << nCols;
		CString strTmp;
		int n = insPt2[0].size();
		for (int j=0; j<n; j++)
		{
			for (int i=0;i<polys.GetCount()-1; i++)
			{
				strTmp.Format(_T("%g"), insPt2[2*i][j].x);
				ar << strTmp;
				strTmp.Format(_T("%g"), insPt2[2*i][j].y);
				ar << strTmp;
				strTmp.Format(_T("%g"), -99999.0);
				ar << strTmp;
				strTmp = _T("");
				ar << strTmp;

				strTmp.Format(_T("%g"), insPt2[2*i+1][j].x);
				ar << strTmp;
				strTmp.Format(_T("%g"), insPt2[2*i+1][j].y);
				ar << strTmp;
				strTmp.Format(_T("%g"), -99999.0);
				ar << strTmp;
				strTmp = _T("");
				ar << strTmp;

				strTmp = _T("EOM");
				ar << strTmp;
				strTmp = _T("");
				ar << strTmp;
				strTmp = _T("");
				ar << strTmp;
				strTmp = _T("");
				ar << strTmp;
			}			
		}

		ar.Close();
		file.Close();

		//////////////////////////////////////////////////////////////////////////
		CFileView *pTreeView = pMF->GetTreeFileView();
		CViewTree *pTree = pTreeView->GetTreeCtrl();

		HTREEITEM hItem = pTree->GetSelectedItem();
		if( hItem == NULL )
			hItem = TVI_ROOT;

		int nImage = pTree->m_nCountImages - 17;

		HTREEITEM h = pTreeView->AddTreeItem(_T("夹层法线"), nImage + 12, nImage + 12, hItem);

		CTreeNodeDat *lpNodeDat = new CTreeNodeDat;

		CFileViewObj *pObj = new CFileViewObj;
		pObj->m_strFileName = strFileName;

		lpNodeDat->m_nImage			= nImage + 12;
		lpNodeDat->m_nSelectedImage	= nImage + 12;
		lpNodeDat->m_nType			= FILE_NORMAL;
		lpNodeDat->m_pNodeDat		= pObj;
		lpNodeDat->m_uState			= TVIS_EXPANDED;
		strFileName.MakeUpper();
		lpNodeDat->m_strFileName = strFileName;

		pTree->SetItemData(h, (DWORD)lpNodeDat);

		pTree->Expand(hItem, TVE_EXPAND);
	}

	//////////////////////////////////////////////////////////////////////////
	// 写夹层线
	{
		CString strFileName = newGUID();
		strFileName += _T(".dat");

		CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
		CString strTargetName = pMF->GetProjectDatPath();
		strTargetName += _T("\\files\\");
		strTargetName += strFileName;

		CFile file(strTargetName, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
		CArchive ar(&file, CArchive::store);

		int nRows = (polys.GetCount()+(polys.GetCount()-1)*m_iLayers)*(insPt2[0].size()+1);
		int nCols = 4;

		ar << nRows;
		ar << nCols;
		CString strTmp;
		for (int i=0; i<polys.GetCount()-1;i++)
		{
			int n = insPt2[0].size();
			for (int j=0; j<n; j++)
			{
				strTmp.Format(_T("%g"), insPt2[2*i][j].x);
				ar << strTmp;
				strTmp.Format(_T("%g"), insPt2[2*i][j].y);
				ar << strTmp;
				strTmp.Format(_T("%g"), -99999.0);
				ar << strTmp;
				strTmp = _T("");
				ar << strTmp;
			}
			strTmp = _T("EOM");
			ar << strTmp;
			strTmp = _T("");
			ar << strTmp;
			strTmp = _T("");
			ar << strTmp;
			strTmp = _T("");
			ar << strTmp;

			for (int j=0; j<m_iLayers;j++)
			{
				int n = insPt2[0].size();
				for (int k=0; k<n; k++)
				{			
					CPoint2D pt;
					pt.x = insPt2[2*i][k].x + (insPt2[2*i+1][k].x-insPt2[2*i][k].x)/(m_iLayers+1)*(j+1);
					pt.y = insPt2[2*i][k].y + (insPt2[2*i+1][k].y-insPt2[2*i][k].y)/(m_iLayers+1)*(j+1);
					strTmp.Format(_T("%g"), pt.x);
					ar << strTmp;
					strTmp.Format(_T("%g"), pt.y);
					ar << strTmp;
					strTmp.Format(_T("%g"), -99999.0);
					ar << strTmp;
					strTmp = _T("");
					ar << strTmp;
				}
				strTmp = _T("EOM");
				ar << strTmp;
				strTmp = _T("");
				ar << strTmp;
				strTmp = _T("");
				ar << strTmp;
				strTmp = _T("");
				ar << strTmp;
			}

			for (int j=0; j<n; j++)
			{
				strTmp.Format(_T("%g"), insPt2[2*i+1][j].x);
				ar << strTmp;
				strTmp.Format(_T("%g"), insPt2[2*i+1][j].y);
				ar << strTmp;
				strTmp.Format(_T("%g"), -99999.0);
				ar << strTmp;
				strTmp = _T("");
				ar << strTmp;
			}
			strTmp = _T("EOM");
			ar << strTmp;
			strTmp = _T("");
			ar << strTmp;
			strTmp = _T("");
			ar << strTmp;
			strTmp = _T("");
			ar << strTmp;
		}

// 		int n = insPt2[0].size();
// 		for (int j=0; j<n; j++)
// 		{
// 			strTmp.Format(_T("%g"), insPt2[2*(polys.GetCount()-1)+1][j].x);
// 			ar << strTmp;
// 			strTmp.Format(_T("%g"), insPt2[2*(polys.GetCount()-1)+1][j].y);
// 			ar << strTmp;
// 			strTmp.Format(_T("%g"), -99999.0);
// 			ar << strTmp;				
// 			strTmp = _T("");
// 			ar << strTmp;
// 		}
// 		strTmp = _T("EOM");
// 		ar << strTmp;
// 		strTmp = _T("");
// 		ar << strTmp;
// 		strTmp = _T("");
// 		ar << strTmp;
// 		strTmp = _T("");
// 		ar << strTmp;			

		delete []insPt2;

		ar.Close();
		file.Close();

		//////////////////////////////////////////////////////////////////////////
		CFileView *pTreeView = pMF->GetTreeFileView();
		CViewTree *pTree = pTreeView->GetTreeCtrl();

		HTREEITEM hItem = pTree->GetSelectedItem();
		if( hItem == NULL )
			hItem = TVI_ROOT;

		int nImage = pTree->m_nCountImages - 17;

		HTREEITEM h = pTreeView->AddTreeItem(_T("侧积夹层"), nImage + 13, nImage + 13, hItem);

		CTreeNodeDat *lpNodeDat = new CTreeNodeDat;

		CFileViewObj *pObj = new CFileViewObj;
		pObj->m_strFileName = strFileName;

		lpNodeDat->m_nImage			= nImage + 13;
		lpNodeDat->m_nSelectedImage	= nImage + 13;
		lpNodeDat->m_nType			= FILE_LAYER;
		lpNodeDat->m_pNodeDat		= pObj;
		lpNodeDat->m_uState			= TVIS_EXPANDED;
		strFileName.MakeUpper();
		lpNodeDat->m_strFileName = strFileName;

		pTree->SetItemData(h, (DWORD)lpNodeDat);

		pTree->Expand(hItem, TVE_EXPAND);
	}
	*/
//}

//void CGeoMapView::OnUpdateGenerateInterlayer(CCmdUI *pCmdUI)
//{
	// TODO: 在此添加命令更新用户界面处理程序代码

//	if( m_selection.GetCount() > 0 )
//		pCmdUI->Enable(TRUE);
//	else
//		pCmdUI->Enable(FALSE);
//}

// 计算坐标转换平差四参数
void CGeoMapView::OnCaleParameter()
{
	// TODO: 在此添加命令处理程序代码

	CGeoMapDoc * pDoc = GetDocument();

	if(pDoc->m_pCurDrawPage->m_bParameter)
	{
		if( AfxMessageBox(_T("坐标转换参数存在，是否重新计算？"),MB_YESNO|MB_ICONQUESTION) == IDNO)
			return;
	}

	CGeoDrawObj *pObj = (CGeoDrawObj*) m_selection.GetTail();

	CGeoPictrueObj *pPictrue = NULL;
	if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPictrueObj)))
	{
		pPictrue = (CGeoPictrueObj *)pObj;
		if( pPictrue->m_ptCtrl.GetSize() < 3 )
		{
			AfxMessageBox(_T("底图重合点坐标最少需要3个!"));
			return;
		}
	}		

	CCoordConverterNew converter; // 坐标转换器

	int n = pPictrue->m_ptCtrl.GetSize();
	for (int i=0; i<n; i++)
	{
		if( pPictrue->m_vecWellName[i].IsEmpty() )
			continue;
		CPoint2D pt1, pt2;
		pt1 = pPictrue->m_ptGeoCtrl[i];
		if( pt1.x < 1)
			continue;

		pt2.x = pPictrue->m_ptCtrl[i].x;
		pt2.y = pPictrue->m_ptCtrl[i].y;

		converter.AddCoincidence(pt1, pt2);
	}

	if( !converter.Prepare() )
	{
		AfxMessageBox(_T("底图重合点坐标最少需要3个!"));
		return;
	}

	pPictrue->m_bLocked = TRUE; // 锁定底图

	//double a = converter.GetParameter(0);
	//double b = converter.GetParameter(1);
	//double c = converter.GetParameter(2);
	//double d = converter.GetParameter(3);

	//double Lx = a;
	//double Ly = b;

	//double K = sqrt(c*c+d*d);
	//double alph = acos(c/K)/PI*180.0;
	//if( d<0)
	//	alph = alph*(-1);

	//pDoc->m_pCurDrawPage->m_lScale = 1.0/K;
	//pDoc->m_pCurDrawPage->m_ptCoordOrig.x = a;
	//pDoc->m_pCurDrawPage->m_ptCoordOrig.y = b;
	//pDoc->m_pCurDrawPage->m_ptCoordOrig.m_dValue = alph;

	for(int i=0; i<6; i++)
	{
		pDoc->m_pCurDrawPage->m_ground2screen[i] = converter.GetG2SParameter(i);
		pDoc->m_pCurDrawPage->m_screen2ground[i] = converter.GetS2GParameter(i);
	}

	pDoc->m_pCurDrawPage->m_bParameter = TRUE;

	pDoc->SetModifiedFlag();
}

void CGeoMapView::OnUpdateCaleParameter(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码

	if( m_selection.GetCount()<1)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	CGeoDrawObj *pObj = (CGeoDrawObj*) m_selection.GetTail();

	if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPictrueObj)))
	{
		CDrawLayer *pLayer = pObj->GetParentLayer();
		if( pLayer->m_eLayerType == CDrawLayer::basepictrue)
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
	else
		pCmdUI->Enable(FALSE);
}

bool CGeoMapView::CaleParameter(CGeoPictrueObj*pPictrue)
{
	CGeoMapDoc * pDoc = GetDocument();

	if( pPictrue->m_ptCtrl.GetSize() < 3 )
		return false;

	//CCoordConverter converter; // 坐标转换器
	CCoordConverterNew converter;
	
	int n = pPictrue->m_ptCtrl.GetSize();
	for (int i=0; i<n; i++)
	{
		if( pPictrue->m_vecWellName[i].IsEmpty() )
			continue;
		CPoint2D pt1, pt2;
		pt1 = pPictrue->m_ptGeoCtrl[i];
		if( pt1.x < 1)
			continue;

		pt2.x = pPictrue->m_ptCtrl[i].x;
		pt2.y = pPictrue->m_ptCtrl[i].y;

		converter.AddCoincidence(pt1, pt2);
	}

	if( !converter.Prepare() )
		return false;

	//double a = converter.GetParameter(0);
	//double b = converter.GetParameter(1);
	//double c = converter.GetParameter(2);
	//double d = converter.GetParameter(3);

	//double Lx = a;
	//double Ly = b;

	//double K = sqrt(c*c+d*d);
	//double alph = acos(c/K)/PI*180.0;
	//if( d<0)
	//	alph = alph*(-1);

	//pDoc->m_pCurDrawPage->m_lScale = 1.0/K;
	//pDoc->m_pCurDrawPage->m_ptCoordOrig.x = a;
	//pDoc->m_pCurDrawPage->m_ptCoordOrig.y = b;
	//pDoc->m_pCurDrawPage->m_ptCoordOrig.m_dValue = alph;

	for(int i=0; i<6; i++)
	{
		pDoc->m_pCurDrawPage->m_ground2screen[i] = converter.GetG2SParameter(i);
		pDoc->m_pCurDrawPage->m_screen2ground[i] = converter.GetS2GParameter(i);
	}


	pDoc->m_pCurDrawPage->m_bParameter = TRUE;
	
	return true;
}

// 转换四参数改变了
void CGeoMapView::ParamterChanged()
{
	CGeoMapDoc * pDoc = GetDocument();
	POSITION pos = pDoc->m_pCurDrawPage->m_DrawLayers.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawLayer* pLayer = pDoc->m_pCurDrawPage->m_DrawLayers.GetNext(pos);
		POSITION pos1 = pLayer->m_DrawObjects.GetHeadPosition();
		while (pos1 != NULL)
		{
			CGeoDrawObj* pObj = pLayer->m_DrawObjects.GetNext(pos1);
			if( pObj->IsKindOf(RUNTIME_CLASS(CGeoWellObj)) )
			{
				CGeoWellObj *pWell = (CGeoWellObj*)pObj;
				pWell->OnScaleChanged();
			}
		}
	}	
}
// 单复夹层线转换
void CGeoMapView::OnSingleInterlayer()
{
	// TODO: 在此添加命令处理程序代码

	POSITION pos = m_selection.GetHeadPosition();
	while(pos != NULL)
	{
		CGeoDrawObj *pObj = m_selection.GetNext(pos);
		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPolyObj)))
		{
			CGeoPolyObj *pPoly = (CGeoPolyObj*)pObj;
			if( pPoly->m_strObjName == _T("夹层线"))
			{
				if(pPoly->m_ePolyType == CGeoPolyObj::interlayers )
				{
					pPoly->m_ePolyType = CGeoPolyObj::singlelayer;
					pPoly->m_eLine.s_LineStyle.m_PatternFill.m_crColourFore = RGB(0, 192, 0);
					pPoly->Invalidate(this);
				}
				else
				{
					pPoly->m_ePolyType = CGeoPolyObj::interlayers;
					pPoly->m_eLine.s_LineStyle.m_PatternFill.m_crColourFore = RGB(0, 0, 192);
					pPoly->Invalidate(this);
				}
			}
		}
	}
}

void CGeoMapView::OnUpdateSingleInterlayer(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if( m_selection.GetCount()>0)
	{
		POSITION pos = m_selection.GetHeadPosition();
		while(pos != NULL)
		{
			CGeoDrawObj *pObj = m_selection.GetNext(pos);
			if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPolyObj)))
			{
				CGeoPolyObj *pPoly = (CGeoPolyObj*)pObj;
				if( pPoly->m_strObjName == _T("夹层线"))
				{
					pCmdUI->Enable(TRUE);
					if(pPoly->m_ePolyType == CGeoPolyObj::interlayers )
						pCmdUI->SetCheck(0);
					else
						pCmdUI->SetCheck(1);
				}
			}
		}
	}
	else
		pCmdUI->Enable(FALSE);
}

// 保存为夹层模型
void CGeoMapView::OnSaveLayerModel()
{
	// TODO: 在此添加命令处理程序代码
	CGeoDrawObjList polys;		// 曲线
	CGeoDrawObjList lines;		// 法线

	POSITION pos = m_selection.GetHeadPosition();
	while(pos!=NULL)
	{
		CGeoDrawObj *pObj = m_selection.GetNext(pos);

		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPolyObj)))
		{
			CGeoPolyObj*pPoly = (CGeoPolyObj*)pObj;
			if( pPoly->m_strObjName == _T("夹层线"))
				polys.AddTail(pObj);
		}
		else if( pObj->IsKindOf(RUNTIME_CLASS(CGeoLineObj)))
		{
			CGeoLineObj*pLine = (CGeoLineObj*)pObj;
			if( pLine->m_strObjName == _T("法线"))
				lines.AddTail(pObj);
		}
	}

	//////////////////////////////////////////////////////////////////////////

	if( polys.GetCount() < 2 )
	{
		AfxMessageBox(_T("夹层线少于2条!"));
		return;
	}

	if( lines.GetCount() < 1 )
	{
		AfxMessageBox(_T("主法线不能少于1条!"));
		return;
	}

	TRACE(_T("%d\n"),lines.GetCount());
	// 删除没有相交的法线
	DeleteNoCorssNormal(polys, lines);
	TRACE(_T("%d\n"),lines.GetCount());

	//////////////////////////////////////////////////////////////////////////
	// 法线排序
	CGeoPolyObj *pPoly = (CGeoPolyObj *)polys.GetHead();
	SortNormalLine(lines, pPoly);
	TRACE(_T("%d\n"),lines.GetCount());

	//////////////////////////////////////////////////////////////////////////
	// 夹层线排序
	//	
	CGeoLineObj *pLine = (CGeoLineObj *)lines.GetHead();
	SortInterLayerLine(polys, pLine);
	TRACE(_T("%d\n"),lines.GetCount());

	//////////////////////////////////////////////////////////////////////////
	// 检查法线方向
	{
		BOOL bFlags = FALSE;
		CGeoLineObj *pL = (CGeoLineObj *)lines.GetHead();
		int n = lines.GetCount();
		CGeoPolyObj *pPoly = GetCrossFirstLayer(polys, pL);

		for (int i=0; i<n; i++)
		{
			CGeoLineObj *pLine = (CGeoLineObj *)lines.GetAt(lines.FindIndex(i));
			CGeoPolyObj *p = GetCrossFirstLayer(polys, pLine);
			if( p!=pPoly)
			{
				AfxMessageBox(_T("法线方向不一致，请检查并转换为同一方向!"));
				return;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 写夹层模型
	{
		CLayerModel layerModel;

		CGeoPolyObj *pP = (CGeoPolyObj *)polys.GetHead();
		BOOL bDirection = (GetPolyDirection(lines, pP)>0);
		int nSize = polys.GetCount();
		for (int i=0; i<nSize;i++)
		{
			CGeoPolyObj *pPoly = (CGeoPolyObj *)polys.GetAt(polys.FindIndex(i));
			BOOL bDir = (GetPolyDirection(lines, pPoly)>0);
			
			std::vector<CPoint2D> points;

			// 检查夹层线方向
			if( bDirection != bDir ) // 如果不相等，调整夹层线点顺序
			{
				if(pPoly->m_ePolyType == CGeoPolyObj::interlayers )
				{
					for (int j=pPoly->m_nPoints-1; j>=0; j--)
					{
						CPoint2D pt;
						pt = pPoly->m_geoPoints[j];
						points.push_back(pt);
					}
					layerModel.m_layers.push_back(points);
				}
				else
				{
					for (int j=pPoly->m_nPoints-1; j>=0; j--)
					{
						CPoint2D pt;
						pt = pPoly->m_geoPoints[j];
						points.push_back(pt);
					}
					layerModel.m_singleLayers.push_back(points);					
				}
			}
			else
			{
				if(pPoly->m_ePolyType == CGeoPolyObj::interlayers )
				{	
					for (int j= 0; j<pPoly->m_nPoints; j++)
					{
						CPoint2D pt;
						pt = pPoly->m_geoPoints[j];
						points.push_back(pt);
					}
					layerModel.m_layers.push_back(points);
				}
				else
				{
					for (int j= 0; j<pPoly->m_nPoints; j++)
					{
						CPoint2D pt;
						pt = pPoly->m_geoPoints[j];
						points.push_back(pt);
					}
					layerModel.m_singleLayers.push_back(points);
				}
			}
		}

		nSize = lines.GetCount();
		for (int i=0; i<nSize;i++)
		{
			CGeoLineObj *pLine = (CGeoLineObj *)lines.GetAt(lines.FindIndex(i));
			
			layerModel.m_normals[0].push_back(pLine->m_geoPoints[0]);
			layerModel.m_normals[1].push_back(pLine->m_geoPoints[1]);

			std::vector<CPoint2D> pts;
			layerModel.m_vertModel.push_back(pts);
			layerModel.m_dAngles.push_back(0.0);
			layerModel.m_vertTitle.Add(_T(""));
			layerModel.m_vertColor.push_back(0);
		}

		CString strFileName = newGUID();
		strFileName += _T(".dat");

		CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
		CString strTargetName = pMF->GetProjectDatPath();
		strTargetName += _T("\\files\\");
		strTargetName += strFileName;

		CFile file(strTargetName, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
		CArchive ar(&file, CArchive::store);

		layerModel.Serialize(ar);

		ar.Close();
		file.Close();

		//////////////////////////////////////////////////////////////////////////
		CFileView *pTreeView = pMF->GetTreeFileView();
		CViewTree *pTree = pTreeView->GetTreeCtrl();

		HTREEITEM hItem = pTree->GetSelectedItem();
		if( hItem == NULL )
			hItem = TVI_ROOT;
		else
		{
			CTreeNodeDat *lpNode = (CTreeNodeDat*)pTree->GetItemData(hItem);
			if( lpNode->m_nType != FOLDER)
				hItem = TVI_ROOT;
		}

		int nImage = pTree->m_nCountImages - 19;

		HTREEITEM h = pTreeView->AddTreeItem(_T("夹层模型"), nImage + 18, nImage + 18, hItem);

		CTreeNodeDat *lpNodeDat = new CTreeNodeDat;

		CFileViewObj *pObj = new CFileViewObj;
		pObj->m_strFileName = strFileName;

		lpNodeDat->m_nImage			= nImage + 18;
		lpNodeDat->m_nSelectedImage	= nImage + 18;
		lpNodeDat->m_nType			= FILE_LAYER_MODEL;
		lpNodeDat->m_pNodeDat		= pObj;
		lpNodeDat->m_uState			= TVIS_EXPANDED;
		strFileName.MakeUpper();
		lpNodeDat->m_strFileName = strFileName;

		pTree->SetItemData(h, (DWORD)lpNodeDat);
		pTree->Expand(hItem, TVE_EXPAND);

		pTree->EditLabel(h);
	}
}

void CGeoMapView::OnUpdateSaveLayerModel(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if( m_selection.GetCount() > 0 )
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

// 保存为夹层线及法线
void CGeoMapView::OnSaveLayermodelLine()
{
	// TODO: 在此添加命令处理程序代码

	CGeoDrawObjList polys;		// 曲线
	CGeoDrawObjList lines;		// 法线

	POSITION pos = m_selection.GetHeadPosition();
	while(pos!=NULL)
	{
		CGeoDrawObj *pObj = m_selection.GetNext(pos);

		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPolyObj)))
		{
			CGeoPolyObj*pPoly = (CGeoPolyObj*)pObj;
			if( pPoly->m_strObjName == _T("夹层线"))
				polys.AddTail(pObj);
		}
		else if( pObj->IsKindOf(RUNTIME_CLASS(CGeoLineObj)))
		{
			CGeoLineObj*pLine = (CGeoLineObj*)pObj;
			if( pLine->m_strObjName == _T("法线"))
				lines.AddTail(pObj);
		}
	}

	//////////////////////////////////////////////////////////////////////////

	if( polys.GetCount() < 2 )
	{
		AfxMessageBox(_T("夹层线少于2条!"));
		return;
	}

	if( lines.GetCount() < 1 )
	{
		AfxMessageBox(_T("主法线不能少于1条!"));
		return;
	}

	TRACE(_T("%d\n"),lines.GetCount());
	// 删除没有相交的法线
	DeleteNoCorssNormal(polys, lines);
	TRACE(_T("%d\n"),lines.GetCount());
	//////////////////////////////////////////////////////////////////////////
	// 夹层线排序
	//	
	CGeoLineObj *pLine = (CGeoLineObj *)lines.GetHead();
	SortInterLayerLine(polys, pLine);
	TRACE(_T("%d\n"),lines.GetCount());

	//////////////////////////////////////////////////////////////////////////
	// 法线排序

	CGeoPolyObj *pPoly = (CGeoPolyObj *)polys.GetHead();
	SortNormalLine(lines, pPoly);
	TRACE(_T("%d\n"),lines.GetCount());
	
	//////////////////////////////////////////////////////////////////////////
	// 写法线
	{
		BOOL bFlags = FALSE;
		CGeoLineObj *pL = (CGeoLineObj *)lines.GetHead();
		int n = lines.GetCount();
		CGeoPolyObj *pPoly = GetCrossFirstLayer(polys, pL);

		for (int i=0; i<n; i++)
		{
			CGeoLineObj *pLine = (CGeoLineObj *)lines.GetAt(lines.FindIndex(i));
			CGeoPolyObj *p = GetCrossFirstLayer(polys, pLine);
			if( p!=pPoly)
			{
				int iRet = AfxMessageBox(_T("法线方向不一致，是否转换为同一方向?"),MB_YESNOCANCEL|MB_ICONQUESTION);
				if(iRet == IDYES)
				{
					bFlags = TRUE;
					break;
				}
				else if( iRet == IDCANCEL )
					return;
			}
		}

		CString strFileName = newGUID();
		strFileName += _T(".dat");

		CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
		CString strTargetName = pMF->GetProjectDatPath();
		strTargetName += _T("\\files\\");
		strTargetName += strFileName;

		CFile file(strTargetName, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
		CArchive ar(&file, CArchive::store);

		int nRows = (3*lines.GetCount());
		int nCols = 4;

		ar << nRows;
		ar << nCols;
		CString strTmp;
				
		for (int i=0; i<n; i++)
		{
			CGeoLineObj *pLine = (CGeoLineObj *)lines.GetAt(lines.FindIndex(i));
			CGeoPolyObj *p = GetCrossFirstLayer(polys, pLine);
			if( p!=pPoly && bFlags)
			{
				strTmp.Format(_T("%g"), pLine->m_geoPoints[1].x);
				ar << strTmp;
				strTmp.Format(_T("%g"), pLine->m_geoPoints[1].y);
				ar << strTmp;
				strTmp.Format(_T("%g"), -99999.0);
				ar << strTmp;
				strTmp = _T("");
				ar << strTmp;

				strTmp.Format(_T("%g"), pLine->m_geoPoints[0].x);
				ar << strTmp;
				strTmp.Format(_T("%g"), pLine->m_geoPoints[0].y);
				ar << strTmp;
				strTmp.Format(_T("%g"), -99999.0);
				ar << strTmp;
				strTmp = _T("");
				ar << strTmp;
			}
			else
			{
				strTmp.Format(_T("%g"), pLine->m_geoPoints[0].x);
				ar << strTmp;
				strTmp.Format(_T("%g"), pLine->m_geoPoints[0].y);
				ar << strTmp;
				strTmp.Format(_T("%g"), -99999.0);
				ar << strTmp;
				strTmp = _T("");
				ar << strTmp;

				strTmp.Format(_T("%g"), pLine->m_geoPoints[1].x);
				ar << strTmp;
				strTmp.Format(_T("%g"), pLine->m_geoPoints[1].y);
				ar << strTmp;
				strTmp.Format(_T("%g"), -99999.0);
				ar << strTmp;
				strTmp = _T("");
				ar << strTmp;
			}

			strTmp = _T("EOM");
			ar << strTmp;
			strTmp = _T("");
			ar << strTmp;
			strTmp = _T("");
			ar << strTmp;
			strTmp = _T("");
			ar << strTmp;
		}

		ar.Close();
		file.Close();

		//////////////////////////////////////////////////////////////////////////
		CFileView *pTreeView = pMF->GetTreeFileView();
		CViewTree *pTree = pTreeView->GetTreeCtrl();

		HTREEITEM hItem = pTree->GetSelectedItem();
		if( hItem == NULL )
			hItem = TVI_ROOT;
		else
		{
			CTreeNodeDat *lpNode = (CTreeNodeDat*)pTree->GetItemData(hItem);
			if( lpNode->m_nType != FOLDER)
				hItem = TVI_ROOT;
		}

		int nImage = pTree->m_nCountImages - 19;

		HTREEITEM h = pTreeView->AddTreeItem(_T("夹层法线"), nImage + 14, nImage + 14, hItem);

		CTreeNodeDat *lpNodeDat = new CTreeNodeDat;

		CFileViewObj *pObj = new CFileViewObj;
		pObj->m_strFileName = strFileName;

		lpNodeDat->m_nImage			= nImage + 14;
		lpNodeDat->m_nSelectedImage	= nImage + 14;
		lpNodeDat->m_nType			= FILE_NORMAL;
		lpNodeDat->m_pNodeDat		= pObj;
		lpNodeDat->m_uState			= TVIS_EXPANDED;
		strFileName.MakeUpper();
		lpNodeDat->m_strFileName = strFileName;

		pTree->SetItemData(h, (DWORD)lpNodeDat);

		pTree->Expand(hItem, TVE_EXPAND);
	}

	//////////////////////////////////////////////////////////////////////////
	// 写夹层线
	{
		CString strFileName = newGUID();
		strFileName += _T(".dat");

		CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
		CString strTargetName = pMF->GetProjectDatPath();
		strTargetName += _T("\\files\\");
		strTargetName += strFileName;

		CFile file(strTargetName, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
		CArchive ar(&file, CArchive::store);

		int nSize = polys.GetCount();
		int nRows= 0;
		for (int i=0;i<nSize; i++)
		{
			CGeoPolyObj *pPoly = (CGeoPolyObj *)polys.GetAt(polys.FindIndex(i));
			nRows += pPoly->m_nPoints;
			nRows += 1;
		}

		int nCols = 4;

		ar << nRows;
		ar << nCols;

		CString strTmp;

		CGeoPolyObj *pP = (CGeoPolyObj *)polys.GetHead();
		CGeoLineObj *pLine = GetCrossFirstNormal(lines, pP);
		for (int i=0; i<nSize;i++)
		{
			CGeoPolyObj *pPoly = (CGeoPolyObj *)polys.GetAt(polys.FindIndex(i));
			CGeoLineObj *p = GetCrossFirstNormal(lines, pPoly);
			 // 检查夹层线方向
			if( p != pLine ) // 如果不相等，调整夹层线点顺序
			{
				for (int j=pPoly->m_nPoints-1; j>=0; j--)
				{
					strTmp.Format(_T("%g"), pPoly->m_geoPoints[j].x);
					ar << strTmp;
					strTmp.Format(_T("%g"), pPoly->m_geoPoints[j].y);
					ar << strTmp;
					strTmp.Format(_T("%g"), -99999.0);
					ar << strTmp;
					strTmp = _T("");
					ar << strTmp;
				}
			}
			else
			{
				for (int j=0; j<pPoly->m_nPoints; j++)
				{
					strTmp.Format(_T("%g"), pPoly->m_geoPoints[j].x);
					ar << strTmp;
					strTmp.Format(_T("%g"), pPoly->m_geoPoints[j].y);
					ar << strTmp;
					strTmp.Format(_T("%g"), -99999.0);
					ar << strTmp;
					strTmp = _T("");
					ar << strTmp;
				}
			}
			strTmp = _T("EOM");
			ar << strTmp;
			strTmp = _T("");
			ar << strTmp;
			strTmp = _T("");
			ar << strTmp;
			strTmp = _T("");
			ar << strTmp;
		}

		ar.Close();
		file.Close();

		//////////////////////////////////////////////////////////////////////////
		CFileView *pTreeView = pMF->GetTreeFileView();
		CViewTree *pTree = pTreeView->GetTreeCtrl();

		HTREEITEM hItem = pTree->GetSelectedItem();
		if( hItem == NULL )
			hItem = TVI_ROOT;
		else
		{
			CTreeNodeDat *lpNode = (CTreeNodeDat*)pTree->GetItemData(hItem);
			if( lpNode->m_nType != FOLDER)
				hItem = TVI_ROOT;
		}

		int nImage = pTree->m_nCountImages - 19;

		HTREEITEM h = pTreeView->AddTreeItem(_T("侧积夹层"), nImage + 15, nImage + 15, hItem);

		CTreeNodeDat *lpNodeDat = new CTreeNodeDat;

		CFileViewObj *pObj = new CFileViewObj;
		pObj->m_strFileName = strFileName;

		lpNodeDat->m_nImage			= nImage + 15;
		lpNodeDat->m_nSelectedImage	= nImage + 15;
		lpNodeDat->m_nType			= FILE_LAYER;
		lpNodeDat->m_pNodeDat		= pObj;
		lpNodeDat->m_uState			= TVIS_EXPANDED;
		strFileName.MakeUpper();
		lpNodeDat->m_strFileName = strFileName;

		pTree->SetItemData(h, (DWORD)lpNodeDat);

		pTree->Expand(hItem, TVE_EXPAND);
	}
}

void CGeoMapView::OnUpdateSaveLayermodelLine(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if( m_selection.GetCount() > 0 )
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CGeoMapView::OnChangeDirection()
{
	// TODO: 在此添加命令处理程序代码

	POSITION pos = m_selection.GetHeadPosition();

	while(pos != NULL)
	{
		CGeoDrawObj *pObj = m_selection.GetNext(pos);
		if( pObj->m_strObjName == _T("法线"))
		{
			CGeoLineObj *pLine = (CGeoLineObj*)pObj;
			
			CPoint2D pt1;			
			pt1 = pLine->m_geoPoints[0];
			pLine->m_geoPoints[0] = pLine->m_geoPoints[1];
			pLine->m_geoPoints[1] = pt1;

			CPoint pt2;
			pt2 = pLine->m_points[0];
			pLine->m_points[0] = pLine->m_points[1];
			pLine->m_points[1] = pt2;
		}
		if( pObj->m_strObjName == _T("夹层线"))
		{
			CGeoPolyObj *pPoly = (CGeoPolyObj*)pObj;
			pPoly->ReversePoints();
		}
	}
}

// 转为普通线
void CGeoMapView::OnChangeGeneral()
{
	// TODO: 在此添加命令处理程序代码

	POSITION pos = m_selection.GetHeadPosition();

	while(pos != NULL )
	{
		CGeoDrawObj *pObj = m_selection.GetNext(pos);
		if( pObj->m_strObjName == _T("夹层线") )
		{
			CGeoPolyObj *pPoly = (CGeoPolyObj *)pObj;
			pPoly->m_strObjName = _T("曲线");
			pPoly->m_ePolyType = CGeoPolyObj::generic;
			pPoly->m_eLine.s_LineStyle.m_PatternFill.m_crColourFore = RGB(0,0,0);
			pPoly->Invalidate(this);
		}
	}
}

void CGeoMapView::OnUpdateChangeGeneral(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if( m_selection.GetCount()>0)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

// 转为夹层线
void CGeoMapView::OnChangeInterlayer()
{
	// TODO: 在此添加命令处理程序代码

	POSITION pos = m_selection.GetHeadPosition();

	while(pos != NULL )
	{
		CGeoDrawObj *pObj = m_selection.GetNext(pos);
		if( pObj->m_strObjName == _T("曲线") )
		{
			CGeoPolyObj *pPoly = (CGeoPolyObj *)pObj;
			pPoly->m_strObjName = _T("夹层线");
			pPoly->m_ePolyType = CGeoPolyObj::interlayers;
			pPoly->m_eLine.s_LineStyle.m_PatternFill.m_crColourFore = RGB(0,0,192);
			pPoly->Invalidate(this);
		}
	}
}

void CGeoMapView::OnUpdateChangeInterlayer(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if( m_selection.GetCount()>0)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

// 转为法线
void CGeoMapView::OnChangeNormal()
{
	// TODO: 在此添加命令处理程序代码

	POSITION pos = m_selection.GetHeadPosition();

	while(pos != NULL )
	{
		CGeoDrawObj *pObj = m_selection.GetNext(pos);
		if( pObj->m_strObjName == _T("直线") )
		{
			CGeoLineObj *pPoly = (CGeoLineObj *)pObj;
			pPoly->m_strObjName = _T("法线");
			pPoly->m_eLine.s_LineStyle.m_PatternFill.m_crColourFore = RGB(255,0,0);
			pPoly->Invalidate(this);
		}
	}
}

void CGeoMapView::OnUpdateChangeNormal(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if( m_selection.GetCount()>0)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

// 转为直线
void CGeoMapView::OnChangeLine()
{
	// TODO: 在此添加命令处理程序代码

	POSITION pos = m_selection.GetHeadPosition();

	while(pos != NULL )
	{
		CGeoDrawObj *pObj = m_selection.GetNext(pos);
		if( pObj->m_strObjName == _T("法线") )
		{
			CGeoLineObj *pPoly = (CGeoLineObj *)pObj;
			pPoly->m_strObjName = _T("直线");
			pPoly->m_eLine.s_LineStyle.m_PatternFill.m_crColourFore = RGB(0,0,0);
			pPoly->Invalidate(this);
		}
	}
}

void CGeoMapView::OnUpdateChangeLine(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if( m_selection.GetCount()>0)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CGeoMapView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值	
	if( (m_eOperation!=opAddCtrlPoint)&&(m_eOperation!=opDelCtrlPoint)&&(m_eOperation!=opEditCtrlPoint))
	{
		m_hCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_HAND_2);
		SetCursor(m_hCursor);
	}

	CHandTracker trackHand(this);
	trackHand.TrackRubberBand(this, point);
	Invalidate(FALSE);
	CScrollView::OnMButtonDown(nFlags, point);
}

void CGeoMapView::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	ReleaseCapture();
	CScrollView::OnMButtonUp(nFlags, point);
}
