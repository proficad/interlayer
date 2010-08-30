// ToolClip.h: interface for the CGeoToolClip class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLCLIP_H__AF28941D_E109_45E9_BC6F_139AFB9D1A5D__INCLUDED_)
#define AFX_TOOLCLIP_H__AF28941D_E109_45E9_BC6F_139AFB9D1A5D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ToolBase.h"

class CGeoToolClip : public CToolBase  
{
public:
	CGeoToolClip();
	virtual ~CGeoToolClip();

	virtual void OnKeyDown(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnKeyUp(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnChar(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags);
	
	virtual void OnLButtonDown(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonDblClk(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonUp(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnMouseMove(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnEditProperties(CGeoMapView* pView);
	virtual void OnRButtonDown(CGeoMapView* pView, UINT nFlags, const CPoint& point);

};

#endif // !defined(AFX_TOOLCLIP_H__AF28941D_E109_45E9_BC6F_139AFB9D1A5D__INCLUDED_)
