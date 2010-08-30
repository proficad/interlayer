// ToolScaleRuler.h: interface for the CToolScaleRuler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLSCALERULER_H__06D87137_05ED_4EBC_A80B_43902077D66A__INCLUDED_)
#define AFX_TOOLSCALERULER_H__06D87137_05ED_4EBC_A80B_43902077D66A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ToolBase.h"

class CGeoToolScaleRuler : public CToolBase  
{
public:
	CGeoToolScaleRuler();
	virtual ~CGeoToolScaleRuler();
public:
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

#endif // !defined(AFX_TOOLSCALERULER_H__06D87137_05ED_4EBC_A80B_43902077D66A__INCLUDED_)
