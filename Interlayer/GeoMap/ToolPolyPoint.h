// ToolPolyPoint.h: interface for the CToolPolyPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLPOLYPOINT_H__A620F3F6_C40A_4D98_A1EA_175900B236E2__INCLUDED_)
#define AFX_TOOLPOLYPOINT_H__A620F3F6_C40A_4D98_A1EA_175900B236E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ToolBase.h"

class CGeoToolPolyPoint : public CToolBase  
{
public:
	CGeoToolPolyPoint(DrawShape drawShape);
	virtual ~CGeoToolPolyPoint();
	virtual void OnLButtonDown(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnMouseMove(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonUp(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnContextMenu(CGeoMapView* pView, CWnd* pWnd, CPoint point);
};

#endif // !defined(AFX_TOOLPOLYPOINT_H__A620F3F6_C40A_4D98_A1EA_175900B236E2__INCLUDED_)
