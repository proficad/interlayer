// Curve.cpp: implementation of the CCurve class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Curve.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCurve::CCurve()
{
	m_bReversed = false;
	m_geomType = GEO_CURVE;
}

CCurve::~CCurve()
{
}

CurveType CCurve::Type() const
{
	return m_cType;
}

bool CCurve::IsOfType(const CurveType& type) const
{
	CurveType typ = type;
	bool b = (m_cType == typ);
	return b;
}

