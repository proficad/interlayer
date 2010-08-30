// ToolSelect.h: interface for the CToolSelect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLSELECT_H__B0FF545E_4235_4CE1_931C_84D1AF4D0035__INCLUDED_)
#define AFX_TOOLSELECT_H__B0FF545E_4235_4CE1_931C_84D1AF4D0035__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ToolBase.h"

class CGeoToolSelect : public CToolBase  
{
public:
	CGeoToolSelect();
	virtual ~CGeoToolSelect();
public:
	BOOL m_bEnableMove;			//  «∑Ò‘ –Ì“∆∂Ø
public:
	virtual void OnKeyDown(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnKeyUp(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnChar(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags);
	
	virtual void OnLButtonDown(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonDblClk(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonUp(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnMouseMove(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnEditProperties(CGeoMapView* pView);

	virtual void OnContextMenu(CGeoMapView* pView, CWnd* pWnd, CPoint point);
	
};
#endif // !defined(AFX_TOOLSELECT_H__B0FF545E_4235_4CE1_931C_84D1AF4D0035__INCLUDED_)
