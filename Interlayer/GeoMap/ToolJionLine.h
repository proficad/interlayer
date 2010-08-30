// ToolJionLine.h: interface for the CToolJionLine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLJIONLINE_H__0CA59B0A_7D77_47CD_A610_8691AEB6E6DF__INCLUDED_)
#define AFX_TOOLJIONLINE_H__0CA59B0A_7D77_47CD_A610_8691AEB6E6DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ToolBase.h"

class CGeoPolyObj;

class CGeoToolJionLine : public CToolBase  
{
public:
	CGeoToolJionLine();
	virtual ~CGeoToolJionLine();
public:
	virtual void OnLButtonDown(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnMouseMove(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonUp(CGeoMapView* pView, UINT nFlags, const CPoint& point);

	CGeoPolyObj *m_pJoinFirstCurve;
	CGeoPolyObj *m_pJoinEndCurve;
	BOOL		m_bIsJoinFirstEnd;
	BOOL		m_bIsJoinLastEnd;
};

#endif // !defined(AFX_TOOLJIONLINE_H__0CA59B0A_7D77_47CD_A610_8691AEB6E6DF__INCLUDED_)
