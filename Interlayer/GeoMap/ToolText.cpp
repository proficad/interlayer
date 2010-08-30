// ToolText.cpp: implementation of the CToolText class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ToolText.h"
#include "GeoTextObj.h"
#include "ToolSelect.h"
#include "GeoMapView.h"
#include "GeoMapDoc.h"
#include "CmdGeoDrawObj.h"

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

CGeoToolText textTool;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeoToolText::CGeoToolText()
: CToolBase(CToolBase::text)
{

}

CGeoToolText::~CGeoToolText()
{

}

void CGeoToolText::OnLButtonDown(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	CToolBase::OnLButtonDown(pView, nFlags, point);
	
	CPoint local = point;
	pView->ClientToDoc(local);	

	CGeoMapDoc *pDoc = pView->GetDocument();
	
	CGeoTextObj* pObj = new CGeoTextObj(CRect(local, CSize(0, 0)),pDoc->m_pCurDrawLayer);
	pObj->m_bEditing = TRUE;
		
	pView->Select(pObj);
	
	selectMode = sizeObj;
	lastPoint = local;

	g_nDragHandle = 1;
}

void CGeoToolText::OnLButtonDblClk(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	CToolBase::OnLButtonDblClk(pView, nFlags, point);
}

void CGeoToolText::OnLButtonUp(CGeoMapView* pView, UINT nFlags, const CPoint& point)
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
	}

	CToolBase::OnLButtonUp(pView, nFlags, point);
}

void CGeoToolText::OnMouseMove(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
	selectTool.OnMouseMove(pView, nFlags, point);
}

void CGeoToolText::OnKeyDown(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags)
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

void CGeoToolText::OnKeyUp(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags)
{

}
void CGeoToolText::OnChar(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	
}