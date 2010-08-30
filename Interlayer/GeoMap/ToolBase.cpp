// ToolBase.cpp: implementation of the CToolBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ToolBase.h"
#include "GeoMapView.h"
#include "../Resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPoint CToolBase::c_down;
UINT CToolBase::c_nDownFlags;
CPoint CToolBase::c_last;
CToolBase::DrawShape CToolBase::c_drawShape = CToolBase::selection;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToolBase::CToolBase(DrawShape drawShape)
{
	m_drawShape = drawShape;
	c_tools.AddTail(this);
}

CToolBase* CToolBase::FindTool(DrawShape drawShape)
{
	POSITION pos = c_tools.GetHeadPosition();
	while (pos != NULL)
	{
		CToolBase* pTool = (CToolBase*)c_tools.GetNext(pos);
		if (pTool->m_drawShape == drawShape)
			return pTool;
	}
	
	return NULL;
}

void CToolBase::OnLButtonDown(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	// deactivate any in-place active item on this view!
	COleClientItem* pActiveItem = pView->GetDocument()->GetInPlaceActiveItem(pView);
	if (pActiveItem != NULL)
	{
		pActiveItem->Close();
		ASSERT(pView->GetDocument()->GetInPlaceActiveItem(pView) == NULL);
	}
	
	pView->SetCapture();
	c_nDownFlags = nFlags;
	c_down = point;
	c_last = point;
}

void CToolBase::OnLButtonDblClk(CGeoMapView* /*pView*/, UINT /*nFlags*/, const CPoint& /*point*/)
{
}

void CToolBase::OnLButtonUp(CGeoMapView* /*pView*/, UINT /*nFlags*/, const CPoint& point)
{
	ReleaseCapture();
	
	if (point == c_down)
		c_drawShape = selection;
}

void CToolBase::OnMouseMove(CGeoMapView* /*pView*/, UINT /*nFlags*/, const CPoint& point)
{
	c_last = point;
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
}

void CToolBase::OnEditProperties(CGeoMapView* /*pView*/)
{
}

void CToolBase::OnCancel(CGeoMapView* pView)
{
	ReleaseCapture();

	pView->Invalidate(FALSE);
	c_drawShape = selection;
}


void CToolBase::OnKeyDown(CGeoMapView* /*pView*/, UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
}

void CToolBase::OnKeyUp(CGeoMapView* /*pView*/, UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
}

void CToolBase::OnChar(CGeoMapView* /*pView*/, UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
}


void CToolBase::OnRButtonDown(CGeoMapView *pView, UINT nFlags, const CPoint &point)
{

}

void CToolBase::OnContextMenu(CGeoMapView* pView, CWnd* pWnd, CPoint point)
{	
	CMenu menu;
	if (menu.LoadMenu(IDR_POPUP_GEOMAP))
	{
		int uMenuID = -1;
		CString strMenuName;
		int nCounts= menu.GetMenuItemCount();
		for (int i=0; i<nCounts; i++)
		{
			menu.GetMenuString(i,strMenuName,MF_BYPOSITION);
			if( strMenuName == _T("йсм╪"))
			{
				uMenuID = i;
				break;
			}
		}

		if( uMenuID == -1 )
			return;

		CMenu* pPopup = menu.GetSubMenu(uMenuID);
		ASSERT(pPopup != NULL);			
		
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;
		pPopupMenu->SetAutoDestroy (TRUE);
		
		pPopupMenu->Create (pView, point.x, point.y, pPopup->m_hMenu, FALSE, TRUE);		
		
		// 			pPopup->TrackPopupMenu(TPM_RIGHTBUTTON | TPM_LEFTALIGN,
		// 				point.x, point.y,
		// 				AfxGetMainWnd()); 
	}
}