// ToolAttriBush.h: interface for the CToolAttriBush class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLATTRIBUSH_H__F0232B11_D8BC_479E_ACEE_D9209B11D76A__INCLUDED_)
#define AFX_TOOLATTRIBUSH_H__F0232B11_D8BC_479E_ACEE_D9209B11D76A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ToolBase.h"

class CGeoToolAttriBush : public CToolBase  
{
public:
	CGeoToolAttriBush();
	virtual ~CGeoToolAttriBush();
	virtual void OnLButtonDown(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonUp(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnMouseMove(CGeoMapView* pView, UINT nFlags, const CPoint& point);
};

#endif // !defined(AFX_TOOLATTRIBUSH_H__F0232B11_D8BC_479E_ACEE_D9209B11D76A__INCLUDED_)
