// GeoDrawCombine.h: interface for the CGeoDrawCombine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GEODRAWCOMBINE_H__59A27139_640B_438D_87A6_F2206878C510__INCLUDED_)
#define AFX_GEODRAWCOMBINE_H__59A27139_640B_438D_87A6_F2206878C510__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GeoDrawObj.h"

// Í¼ËØ×éºÏ
class CGeoDrawCombine : public CGeoDrawObj  
{
	DECLARE_SERIAL(CGeoDrawCombine);
public:
	CGeoDrawCombine();
	CGeoDrawCombine(CDrawLayer *pLayer);
	virtual ~CGeoDrawCombine();
public:
	virtual CGeoDrawObj* Clone(CDrawLayer *pLayer);	
	virtual int GetHandleCount();
	virtual int HitTestHandle(CPoint point, CGeoMapView* pView, BOOL bSelected);
};

#endif // !defined(AFX_GEODRAWCOMBINE_H__59A27139_640B_438D_87A6_F2206878C510__INCLUDED_)
