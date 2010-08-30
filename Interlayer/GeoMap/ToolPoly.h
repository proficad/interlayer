// ToolPoly.h: interface for the CToolPoly class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLPOLY_H__AC1076C7_7ED7_4522_BE43_A55743DFCE4C__INCLUDED_)
#define AFX_TOOLPOLY_H__AC1076C7_7ED7_4522_BE43_A55743DFCE4C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ToolBase.h"
#include "GeoPolyObj.h"

class CGeoToolPoly : public CToolBase  
{
public:
	CGeoToolPoly(DrawShape drawShape);
	virtual ~CGeoToolPoly();
	
	//²Ù×÷
	virtual void OnKeyDown(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnKeyUp(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnChar(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags);
	
	virtual void OnLButtonDown(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonDblClk(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonUp(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnMouseMove(CGeoMapView* pView, UINT nFlags, const CPoint& point);

	virtual void OnCancel(CGeoMapView* pView);

public:
	CGeoPolyObj *m_pPolyObj;
};

#endif // !defined(AFX_TOOLPOLY_H__AC1076C7_7ED7_4522_BE43_A55743DFCE4C__INCLUDED_)
