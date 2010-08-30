// ToolAttriBush.cpp: implementation of the CToolAttriBush class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ToolAttriBush.h"
#include "ToolSelect.h"
#include "GeoMapView.h"
#include "GeoMapDoc.h"
#include "CmdGeoDrawObj.h"
#include "../Resource.h"

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

CPtrList CToolBase::c_tools;

CGeoToolAttriBush attriBushTool;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeoToolAttriBush::CGeoToolAttriBush()
: CToolBase(CToolBase::attriBrush)
{

}

CGeoToolAttriBush::~CGeoToolAttriBush()
{

}

void CGeoToolAttriBush::OnLButtonDown(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	if( pView->m_selection.GetCount() != 1)
		return;

	CPoint local = point;
	pView->ClientToDoc(local);
	
	CGeoDrawObj* pDrawObj = pView->m_selection.GetHead();
	CGeoDrawObj* pObj = pView->GetDocument()->ObjectAt(local,pView);
	if (pObj != NULL 
		&& pDrawObj != pObj)
	{
		CString strClassName1 = pDrawObj->GetRuntimeClass()->m_lpszClassName;
		CString strClassName2 = pObj->GetRuntimeClass()->m_lpszClassName;
		if( strClassName1 == strClassName2 )
		{
			SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_ATTRI_BRUSH));
			pObj->CopyAttribute(pDrawObj);
			pView->GetDocument()->SetModifiedFlag();
			pObj->Invalidate(pView);
		}
	}

	lastPoint = local;
	
	CToolBase::OnLButtonDown(pView, nFlags, point);
}

void CGeoToolAttriBush::OnLButtonUp(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{	
	if( pView->m_selection.GetCount() != 1)
		return;
	ReleaseCapture();
}

void CGeoToolAttriBush::OnMouseMove(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	if( pView->m_selection.GetCount() != 1)
		return;

	CPoint local = point;
	pView->ClientToDoc(local);
	
	CGeoDrawObj* pDrawObj = pView->m_selection.GetHead();
	CGeoDrawObj* pObj = pView->GetDocument()->ObjectAt(local,pView);
	if (pObj != NULL 
		&& pDrawObj != pObj)
	{
		CString strClassName1 = pDrawObj->GetRuntimeClass()->m_lpszClassName;
		CString strClassName2 = pObj->GetRuntimeClass()->m_lpszClassName;
		if( strClassName1 == strClassName2 )
		{
			SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_ATTRI_BRUSH));
			return;
		}
	}
	
	CToolBase::OnMouseMove(pView, nFlags, point);
}
