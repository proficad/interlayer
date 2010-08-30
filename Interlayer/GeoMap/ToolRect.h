// ToolRect.h: interface for the CToolRect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLRECT_H__B3E717B2_ACBB_4406_AEC8_BDF14C8D0A9B__INCLUDED_)
#define AFX_TOOLRECT_H__B3E717B2_ACBB_4406_AEC8_BDF14C8D0A9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ToolBase.h"

class CGeoToolRect : public CToolBase  
{
public:
	CGeoToolRect(DrawShape drawShape);
	virtual ~CGeoToolRect();
public:
	//²Ù×÷
	virtual void OnKeyDown(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnKeyUp(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnChar(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags);
	
	virtual void OnLButtonDown(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonDblClk(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonUp(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnMouseMove(CGeoMapView* pView, UINT nFlags, const CPoint& point);
};

#endif // !defined(AFX_TOOLRECT_H__B3E717B2_ACBB_4406_AEC8_BDF14C8D0A9B__INCLUDED_)
