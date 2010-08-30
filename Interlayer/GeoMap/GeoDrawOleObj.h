// GeoDrawOleObj.h: interface for the CGeoDrawOleObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GEODRAWOLEOBJ_H__7BDEE4CB_5273_42A2_AD87_BAACD6EC1230__INCLUDED_)
#define AFX_GEODRAWOLEOBJ_H__7BDEE4CB_5273_42A2_AD87_BAACD6EC1230__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GeoDrawObj.h"
#include "DrawItem.h"

class CGeoDrawOleObj : public CGeoDrawObj
{
protected:
	DECLARE_SERIAL(CGeoDrawOleObj);
	CGeoDrawOleObj();
	
public:
	CGeoDrawOleObj(const CRect& position, CDrawLayer *pLayer);
	
	// Implementation
public:
	virtual void Serialize(CArchive& ar);
	virtual void Draw(CDC* pDC, CGeoMapView* pView = NULL);
	virtual CGeoDrawObj* Clone(CDrawLayer *pLayer);
	virtual void OnOpen(CGeoMapView* pView);
	virtual void MoveTo(const CRect& positon, CGeoMapView* pView = NULL);
	virtual BOOL OnEditProperties(CGeoMapView* pView);
	virtual void Remove();
	// �������߷����ı�ʱ
	virtual void OnScaleChanged();
	// ��������תʱ
	virtual void OnAngleChanged();
	// ����������ԭ���иı�ʱ
	virtual void OnCoordOrigChanged();
	
	virtual ~CGeoDrawOleObj();
	
	CGeoDrawItem* m_pClientItem;
	CSize m_extent; // current extent is tracked separate from scaled position
};

#endif // !defined(AFX_GEODRAWOLEOBJ_H__7BDEE4CB_5273_42A2_AD87_BAACD6EC1230__INCLUDED_)
