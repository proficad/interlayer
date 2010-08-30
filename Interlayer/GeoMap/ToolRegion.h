// ToolRegion.h: interface for the CToolRegion class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLREGION_H__0BB081EC_64E8_43C8_AEF7_45E5FEC240B2__INCLUDED_)
#define AFX_TOOLREGION_H__0BB081EC_64E8_43C8_AEF7_45E5FEC240B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ToolBase.h"
#include "GeoPolyObj.h"

class CGeoToolRegion : public CToolBase  
{
public:
	CGeoToolRegion();
	virtual ~CGeoToolRegion();
public:
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

	CGeoDrawObj *m_pFirstObj;
	CGeoDrawObj *m_pLastObj;

	int m_nFirstIndex[2];
	int m_nLastIndex[2];
};

#endif // !defined(AFX_TOOLREGION_H__0BB081EC_64E8_43C8_AEF7_45E5FEC240B2__INCLUDED_)
