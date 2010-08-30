// ToolPageMove.h: interface for the CToolPageMove class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLPAGEMOVE_H__47FDA3DD_D7DB_49F6_B06B_F89C9F2A3049__INCLUDED_)
#define AFX_TOOLPAGEMOVE_H__47FDA3DD_D7DB_49F6_B06B_F89C9F2A3049__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ToolBase.h"

class CGeoToolPageMove : public CToolBase  
{
public:
	CGeoToolPageMove();
	virtual ~CGeoToolPageMove();

	virtual void OnLButtonDown(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonDblClk(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonUp(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnMouseMove(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnContextMenu(CGeoMapView* pView, CWnd* pWnd, CPoint point);
	
public:
	BOOL		m_bDown;
	
	CPoint2D	m_ptFirst;
};

#endif // !defined(AFX_TOOLPAGEMOVE_H__47FDA3DD_D7DB_49F6_B06B_F89C9F2A3049__INCLUDED_)
