// ToolPolyPoint.cpp: implementation of the CToolPolyPoint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ToolPolyPoint.h"
#include "GeoPolyObj.h"
#include "GeoMapView.h"
#include "../Resource.h"
#include "ToolSelect.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CGeoToolSelect selectTool;

CGeoToolPolyPoint delpointTool(CToolBase::delPoint);
CGeoToolPolyPoint addpointTool(CToolBase::addPoint);
CGeoToolPolyPoint movepointTool(CToolBase::movePoint);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeoToolPolyPoint::CGeoToolPolyPoint(DrawShape drawShape)
: CToolBase(drawShape)
{

}

CGeoToolPolyPoint::~CGeoToolPolyPoint()
{

}

void CGeoToolPolyPoint::OnLButtonDown(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	if( pView->m_selection.GetCount() != 1 )
		return;
	
	CPoint local = point;
	pView->ClientToDoc(local);	
	
	CGeoPolyObj *pObj = (CGeoPolyObj *)pView->m_selection.GetHead();
	
	int nDragHandle = pObj->HitTestHandle(local, pView, TRUE); // 返回某选中对象的手柄
	if (nDragHandle != 0)		// 如果在某一手柄上，
	{
		if( c_drawShape == delPoint )
		{
			pObj->DelPoint(nDragHandle, pView);
			return;
		}
	}
	else if( c_drawShape == addPoint )
	{
		pObj->InsertPoint(local, pView);
		return;
	}

	else if( c_drawShape == movePoint )
	{
		pObj->MovePointTo(local, pView);
		return;
	}
	else if( c_drawShape == delPoint )
	{
		pObj->DelPoint(local, pView);
		return;
	}

	//CToolBase::OnLButtonDown(pView, nFlags, point);
}

void CGeoToolPolyPoint::OnLButtonUp(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	//CToolBase::OnLButtonUp(pView, nFlags, point);
}

void CGeoToolPolyPoint::OnMouseMove(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	if( pView->m_selection.GetCount() != 1 )
		return;
	
	if( c_drawShape == addPoint )
		SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_ADD_PT));
	else if( c_drawShape == movePoint)
		SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_MOVE_PT));
	else if( c_drawShape == delPoint )
		SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_DEL_PT));
	
	return;	
}


void CGeoToolPolyPoint::OnContextMenu(CGeoMapView* pView, CWnd* pWnd, CPoint point)
{
	selectTool.OnContextMenu(pView, pWnd, point);
}