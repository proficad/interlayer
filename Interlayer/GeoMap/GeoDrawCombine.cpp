// GeoDrawCombine.cpp: implementation of the CGeoDrawCombine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GeoDrawCombine.h"
#include "GeoMapView.h"
#include "CmdGeoDrawObj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


IMPLEMENT_SERIAL(CGeoDrawCombine, CGeoDrawObj, VERSIONABLE_SCHEMA|0)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeoDrawCombine::CGeoDrawCombine()
{

}

CGeoDrawCombine::CGeoDrawCombine(CDrawLayer* pLayer)
:CGeoDrawObj(pLayer)
{
}

CGeoDrawCombine::~CGeoDrawCombine()
{
}

CGeoDrawObj* CGeoDrawCombine::Clone(CDrawLayer *pLayer)
{
	ASSERT_VALID(this);

	CGeoDrawCombine* pClone = new CGeoDrawCombine(pLayer);

	if (pClone != NULL)
	{
		m_pDocument->Do(new CCmdAddDrawObj(m_pDocument, pClone), TRUE);		
	}
	
	return pClone;
}

int CGeoDrawCombine::GetHandleCount()
{
	ASSERT_VALID(this);
	return 0;
}

int CGeoDrawCombine::HitTestHandle(CPoint point, CGeoMapView *pView, BOOL bSelected)
{
	ASSERT_VALID(this);
	ASSERT(pView != NULL);
	
	if (bSelected) // 如果该图素已经被选中了
	{
		int nHandleCount = GetHandleCount(); // 手柄总数
		for (int nHandle = 1; nHandle <= nHandleCount; nHandle += 1)
		{
			// 返回某手柄的逻辑范围
			CRect rc = GetHandleRect(nHandle,pView);
			rc.NormalizeRect();
			if (point.x >= rc.left && point.x <= rc.right &&
				point.y >= rc.top && point.y <= rc.bottom)
				return nHandle;
		}
	}
	else // 否则, 测试是否落在图素范围内了
	{
		m_pSelectedObj = NULL;
		
		CRect rt = m_rtPosition;
		
		rt.NormalizeRect();
		
		if( m_pParentLayer->m_rgnClipping.m_nPoints > 2
			&& m_pParentLayer->m_bClipping)
			rt = rt&m_pParentLayer->m_rgnClipping.m_rtPosition;
		
		if (point.x >= rt.left && point.x <= rt.right &&
			point.y >= rt.top && point.y <= rt.bottom)
			return 1;		
	}
	
	return 0;
}
