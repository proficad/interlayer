// ToolText.h: interface for the CToolText class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLTEXT_H__28AD42A5_2317_49D1_884F_25A7866F7244__INCLUDED_)
#define AFX_TOOLTEXT_H__28AD42A5_2317_49D1_884F_25A7866F7244__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ToolBase.h"

class CGeoToolText : public CToolBase  
{
public:
	CGeoToolText();
	virtual ~CGeoToolText();
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

#endif // !defined(AFX_TOOLTEXT_H__28AD42A5_2317_49D1_884F_25A7866F7244__INCLUDED_)
