// GeoJionPolyObj.h: interface for the CGeoJionPolyObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GEOJIONPOLYOBJ_H__750E5844_0521_4E4E_BBAC_AD04156EA23B__INCLUDED_)
#define AFX_GEOJIONPOLYOBJ_H__750E5844_0521_4E4E_BBAC_AD04156EA23B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GeoDrawObj.h"

class CGeoPolyObj;

// ﬁ’Ω”œﬂ∂Œ¿‡
class CGeoJionPolyObj : public CGeoDrawObj  
{
	DECLARE_SERIAL(CGeoJionPolyObj);
public:
	CGeoJionPolyObj();
	CGeoJionPolyObj(CDrawLayer* pLayer);
	CGeoJionPolyObj(const CRect& position, CDrawLayer* pLayer);
	virtual ~CGeoJionPolyObj();

public:
	CGeoPolyObj *m_pJoinFirstCurve;
	CGeoPolyObj *m_pJoinEndCurve;
	BOOL		m_bIsJoinFirstEnd;
	BOOL		m_bIsJoinLastEnd;
};

#endif // !defined(AFX_GEOJIONPOLYOBJ_H__750E5844_0521_4E4E_BBAC_AD04156EA23B__INCLUDED_)
