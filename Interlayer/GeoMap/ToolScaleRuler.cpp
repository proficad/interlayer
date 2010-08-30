// ToolScaleRuler.cpp: implementation of the CToolScaleRuler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ToolScaleRuler.h"
#include "GeoScaleObj.h"
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

CGeoToolScaleRuler scaleRulerTool;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeoToolScaleRuler::CGeoToolScaleRuler()
: CToolBase(CToolBase::scaleRuler)
{

}

CGeoToolScaleRuler::~CGeoToolScaleRuler()
{

}

void CGeoToolScaleRuler::OnLButtonDown(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	CToolBase::OnLButtonDown(pView, nFlags, point);
	
	CPoint local = point;
	pView->ClientToDoc(local);	

	CGeoMapDoc *pDoc = pView->GetDocument();
	
	CGeoScaleObj* pObj = new CGeoScaleObj(local, pDoc->m_pCurDrawLayer);
	pObj->m_bEditing = TRUE;
		
	pView->Select(pObj);
}

void CGeoToolScaleRuler::OnLButtonDblClk(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	CToolBase::OnLButtonDblClk(pView, nFlags, point);
}

void CGeoToolScaleRuler::OnLButtonUp(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	
	CGeoMapDoc *pDoc = pView->GetDocument();
	CGeoDrawObj *pObj = pView->m_selection.GetTail();
	pDoc->Do(new CCmdAddDrawObj(pDoc, pObj), TRUE);	// Do add stroke command
	
	pObj->m_bEditing = FALSE;
	pObj->m_bSizeChanging = FALSE;
	pObj->m_bMoving = FALSE;

	CToolBase::OnLButtonUp(pView, nFlags, point);
}

void CGeoToolScaleRuler::OnMouseMove(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
}

void CGeoToolScaleRuler::OnKeyDown(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// ¹ö¶¯ÆÁÄ»
	
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

void CGeoToolScaleRuler::OnKeyUp(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags)
{

}
void CGeoToolScaleRuler::OnChar(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	
}
