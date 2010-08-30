// ToolDivideLine.h: interface for the CToolDivideLine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLDIVIDELINE_H__9494BB97_A303_48FF_B50B_9FDFC581BE41__INCLUDED_)
#define AFX_TOOLDIVIDELINE_H__9494BB97_A303_48FF_B50B_9FDFC581BE41__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ToolBase.h"


class CGeoToolDivideLine : public CToolBase  
{
public:
	CGeoToolDivideLine();
	virtual ~CGeoToolDivideLine();
public:
	virtual void OnLButtonDown(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnMouseMove(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonUp(CGeoMapView* pView, UINT nFlags, const CPoint& point);
};

#endif // !defined(AFX_TOOLDIVIDELINE_H__9494BB97_A303_48FF_B50B_9FDFC581BE41__INCLUDED_)
