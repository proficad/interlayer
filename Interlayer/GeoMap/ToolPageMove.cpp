// ToolPageMove.cpp: implementation of the CToolPageMove class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ToolPageMove.h"
#include "GeoMapView.h"
#include "GeoMapDoc.h"
#include "ToolSelect.h"
#include "../Resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


extern CGeoToolSelect selectTool;

CGeoToolPageMove pageMove;

extern CToolBase::SelectMode selectMode;

extern CPoint lastPoint; // 最后一点坐标
extern CPoint firstDownPoint;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeoToolPageMove::CGeoToolPageMove()
:CToolBase(CToolBase::pageMove)
{
	
}

CGeoToolPageMove::~CGeoToolPageMove()
{

}

void CGeoToolPageMove::OnLButtonDown(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	CGeoMapDoc *pDoc = pView->GetDocument();

	CRect rect = pDoc->m_pCurDrawPage->GetPageRect();

	if( rect.IsRectEmpty() )
		return;	
	
	CPoint local = point;
	pView->ClientToDoc(local);	
	
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	
	lastPoint = point;
	firstDownPoint = point;
	
	CClientDC dc(pView);
	CPen pen(PS_SOLID, 0, RGB(255,0,0));
	CPen *pOldPen = dc.SelectObject(&pen);
	dc.SetROP2(R2_NOTXORPEN);
	dc.MoveTo(firstDownPoint);
	dc.LineTo(lastPoint);
	dc.MoveTo(firstDownPoint);
	dc.LineTo(lastPoint);	
	
	pView->DocToClient(rect);
	dc.Rectangle(rect);
	
	dc.SelectObject(pOldPen);		
	
	m_bDown = TRUE;
		
	m_ptFirst = pDoc->m_pCurDrawPage->CoordinateTransfer(local);

	pView->SetCapture();
}

void CGeoToolPageMove::OnLButtonDblClk(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	
	CGeoMapDoc *pDoc = pView->GetDocument();

	pDoc->m_pCurDrawPage->OnEditProperties(pView);	
}

void CGeoToolPageMove::OnLButtonUp(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	CGeoMapDoc *pDoc = pView->GetDocument();
	
	CRect rect = pDoc->m_pCurDrawPage->GetPageRect();
	
	if( rect.IsRectEmpty() )
		return;	

	m_bDown = FALSE;

	CPoint local = point;
	pView->ClientToDoc(local);

	CPoint2D delta = pDoc->m_pCurDrawPage->CoordinateTransfer(local);
	delta.x = delta.x - m_ptFirst.x;
	delta.y = delta.y - m_ptFirst.y;

	pDoc->m_pCurDrawPage->m_ptCoordOrig.x -= delta.x;
	pDoc->m_pCurDrawPage->m_ptCoordOrig.y -= delta.y;

	pDoc->m_pCurDrawPage->OnScaleChanged();

	pView->Invalidate(FALSE);
	
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));
	
	ReleaseCapture();
}

void CGeoToolPageMove::OnMouseMove(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	CGeoMapDoc *pDoc = pView->GetDocument();
	
	CRect rect = pDoc->m_pCurDrawPage->GetPageRect();
	
	if( rect.IsRectEmpty() )
		return;	
	
	if( m_bDown )
	{
		CClientDC dc(pView);
		CPen pen(PS_SOLID, 0, RGB(255, 0, 0));
		CPen *pOldPen = dc.SelectObject(&pen);
		
		dc.SetROP2(R2_NOTXORPEN);
		
		dc.MoveTo(firstDownPoint);
		dc.LineTo(lastPoint);
		
		dc.MoveTo(firstDownPoint);
		dc.LineTo(point);

		pView->DocToClient(rect);
		CRect rt = rect;
		rt += (lastPoint-firstDownPoint);
		dc.Rectangle(rt);
		rect += (point-firstDownPoint);
		dc.Rectangle(rect);
		dc.SelectObject(pOldPen);
		
		lastPoint = point;		
		
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	}
	else
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));
}

void CGeoToolPageMove::OnContextMenu(CGeoMapView* pView, CWnd* pWnd, CPoint point)
{
	selectTool.OnContextMenu(pView, pWnd, point);
}

