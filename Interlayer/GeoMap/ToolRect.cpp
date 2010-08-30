// ToolRect.cpp: implementation of the CToolRect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ToolRect.h"
#include "GeoRectObj.h"
#include "GeoLineObj.h"
#include "ToolSelect.h"
#include "GeoMapView.h"
#include "GeoMapDoc.h"
#include "CmdGeoDrawObj.h"
#include "GeoPictrueObj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern int g_nMoveLine;
extern int g_nMovePage;

extern CGeoToolSelect selectTool;
extern CToolBase::SelectMode selectMode;
extern int g_nDragHandle;
extern CPoint lastPoint; // 最后一点坐标

CGeoToolRect linetool(CToolBase::line);
CGeoToolRect normaltool(CToolBase::normal);
CGeoToolRect emptyRecttool(CToolBase::rectempty);
CGeoToolRect rectTool(CToolBase::rect);
CGeoToolRect solidrectTool(CToolBase::solidrect);
CGeoToolRect pictrueToool(CToolBase::pictrue);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeoToolRect::CGeoToolRect(DrawShape drawShape)
: CToolBase(drawShape)
{

}

CGeoToolRect::~CGeoToolRect()
{

}

void CGeoToolRect::OnLButtonDown(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	CToolBase::OnLButtonDown(pView, nFlags, point);
	
	CPoint local = point;
	pView->ClientToDoc(local);	

	CGeoMapDoc *pDoc = pView->GetDocument();
	
	switch (c_drawShape)
	{
	default:
		ASSERT(FALSE); // unsuported shape!
		
	case pictrue:
		{
			CGeoPictrueObj* pObj = new CGeoPictrueObj(CRect(local, CSize(0, 0)),pDoc->m_pCurDrawLayer);
			pObj->m_bEditing = TRUE;
			pView->Select(pObj);
		}
		break;
	case line:
		{
			CGeoLineObj* pObj = new CGeoLineObj(CRect(local, CSize(0, 0)),pDoc->m_pCurDrawLayer);
			pObj->m_bEditing = TRUE;
			pView->Select(pObj);

			pObj->m_points[0] = local;
			pObj->m_points[1] = local;
			pObj->m_geoPoints[0] = pObj->CoordinateTransfer(local);
			pObj->m_geoPoints[1] = pObj->m_geoPoints[0];
		}
		break;
	case normal:
		{
			CGeoLineObj* pObj = new CGeoLineObj(CRect(local, CSize(0, 0)),pDoc->m_pCurDrawLayer);
			pObj->m_bEditing = TRUE;
			pObj->SetObjName(_T("法线"));
			pObj->m_eLine.s_LineStyle.m_PatternFill.m_crColourFore = RGB(255, 0, 0);
			pView->Select(pObj);

			pObj->m_points[0] = local;
			pObj->m_points[1] = local;
			pObj->m_geoPoints[0] = pObj->CoordinateTransfer(local);
			pObj->m_geoPoints[1] = pObj->m_geoPoints[0];

			pObj->m_eLine.m_uBeginPointMode = 6;			
		}
		break;
	case rectempty:
		{
			CGeoRectObj* pObj = new CGeoRectObj(CRect(local, CSize(0, 0)),pDoc->m_pCurDrawLayer);
			pObj->m_bEditing = TRUE;
			pObj->m_eFill.m_bShow = FALSE;
			pView->Select(pObj);
		}
		break;
		
	case rect:
		{
			CGeoRectObj* pObj = new CGeoRectObj(CRect(local, CSize(0, 0)),pDoc->m_pCurDrawLayer);
			pObj->m_bEditing = TRUE;
			pObj->m_eFill.m_bShow = TRUE;
			pView->Select(pObj);
		}
		break;
		
	case solidrect:
		{
			CGeoRectObj* pObj = new CGeoRectObj(CRect(local, CSize(0, 0)),pDoc->m_pCurDrawLayer);
			pObj->m_bEditing = TRUE;
			pObj->m_eLine.m_nPenStyle = 6;
			pObj->m_eFill.m_bShow = TRUE;
			pView->Select(pObj);
		}
		break;
	}	
	
	selectMode = sizeObj;
	lastPoint = local;

	g_nDragHandle = 1;	
}

void CGeoToolRect::OnLButtonDblClk(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	CToolBase::OnLButtonDblClk(pView, nFlags, point);
}

void CGeoToolRect::OnLButtonUp(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	if (point == c_down)
	{
		// Don't create empty objects...
		CGeoDrawObj *pObj = pView->m_selection.GetTail();
		CGeoMapDoc *pDoc = pView->GetDocument();
		pDoc->Remove(pObj);
		pView->Select(NULL);
		c_drawShape = selection;		
	}
	else
	{
		CGeoMapDoc *pDoc = pView->GetDocument();
		CGeoDrawObj *pObj = pView->m_selection.GetTail();
		pDoc->Do(new CCmdAddDrawObj(pDoc, pObj), TRUE);	// Do add stroke command

		pObj->m_bEditing = FALSE;
		pObj->m_bSizeChanging = FALSE;
		pObj->m_bMoving = FALSE;

		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPictrueObj)))
		{
			pObj->OnEditProperties(pView);
		}
	}

	CToolBase::OnLButtonUp(pView, nFlags, point);
}

void CGeoToolRect::OnMouseMove(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
	selectTool.OnMouseMove(pView, nFlags, point);
}


void CGeoToolRect::OnKeyDown(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// 滚动屏幕
	
	CPoint delta(0,0);
	switch(nChar)
	{
	case VK_LEFT:
		delta.x = g_nMoveLine;
		break;
	case VK_RIGHT:
		delta.x = -g_nMoveLine;
		break;
	case VK_UP:
		delta.y = g_nMoveLine;
		break;
	case VK_DOWN:
		delta.y = -g_nMoveLine;
		break;
	case VK_HOME:
		delta.x = g_nMovePage;
		break;
	case VK_END:
		delta.x = -g_nMovePage;
		break;
	case VK_PRIOR:
		delta.y = g_nMovePage;
		break;
	case VK_NEXT:
		delta.y = -g_nMovePage;
		break;
	case VK_ADD:
	case '+':
		g_nMoveLine ++;
		break;
	case VK_SUBTRACT:
	case '-':
		g_nMoveLine --;
		if(g_nMoveLine < 1)
			g_nMoveLine = 1;
		break;
	default:
		break;
	}

	pView->UpdateScrollPosition(delta);
}
void CGeoToolRect::OnKeyUp(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags)
{

}
void CGeoToolRect::OnChar(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	
}