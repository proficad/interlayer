// GeoJionPolyObj.cpp: implementation of the CGeoJionPolyObj class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GeoJionPolyObj.h"
#include "GeoPolyObj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CGeoJionPolyObj, CGeoDrawObj, VERSIONABLE_SCHEMA|0)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeoJionPolyObj::CGeoJionPolyObj()
{
	m_pJoinFirstCurve = NULL;
	m_pJoinEndCurve = NULL;
	m_bIsJoinFirstEnd = TRUE;
	m_bIsJoinLastEnd = TRUE;
}

CGeoJionPolyObj::CGeoJionPolyObj(CDrawLayer* pLayer)
:CGeoDrawObj(pLayer)
{
	m_pJoinFirstCurve = NULL;
	m_pJoinEndCurve = NULL;
	m_bIsJoinFirstEnd = TRUE;
	m_bIsJoinLastEnd = TRUE;
}

CGeoJionPolyObj::CGeoJionPolyObj(const CRect& position, CDrawLayer* pLayer)
:CGeoDrawObj(position,pLayer)
{
	m_pJoinFirstCurve = NULL;
	m_pJoinEndCurve = NULL;
	m_bIsJoinFirstEnd = TRUE;
	m_bIsJoinLastEnd = TRUE;
}

CGeoJionPolyObj::~CGeoJionPolyObj()
{

}
