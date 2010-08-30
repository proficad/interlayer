// GeoScaleObj.h: interface for the CGeoScaleObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GEOSCALEOBJ_H__F9FD6253_3F4E_4992_9AE0_081D05127724__INCLUDED_)
#define AFX_GEOSCALEOBJ_H__F9FD6253_3F4E_4992_9AE0_081D05127724__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GeoDrawObj.h"

// ������
class CGeoScaleObj : public CGeoDrawObj  
{
	DECLARE_SERIAL(CGeoScaleObj);
public:
	enum ScaleType
	{
		ST_SINGLELINE,
		ST_SINGLELINE2,
		ST_SINGLELINE3,
		ST_FILLRECT,
		ST_FILLRECT2,
		ST_STEPPEDLINE,
		ST_LINERECT
	};

	CGeoScaleObj();
	CGeoScaleObj(CDrawLayer *pLayer);
	CGeoScaleObj(const CPoint &pt, CDrawLayer *pLayer);
	virtual ~CGeoScaleObj();
public:

	ScaleType	m_ScaleType;
	LOGFONT		m_lfFont;		// ��������
	COLORREF	m_clrFont;		// ������ɫ
	COLORREF	m_clrPen;		// ����ɫ
	int			m_nWidth;		// �����߿��
	int			m_nHeight;		// �����߸߶�

public:
	virtual int HitTestHandle(CPoint point, CGeoMapView* pView, BOOL bSelected);

	virtual int GetHandleCount();
	virtual void Serialize(CArchive& ar);
	virtual CGeoDrawObj* Clone(CDrawLayer *pLayer);
	virtual BOOL OnEditProperties(CGeoMapView* pView = NULL);

	// ��ͼ��
	virtual void Draw(CDC* pDC, CGeoMapView* pView = NULL);
	// ���༭״̬
	virtual void DrawEditMode(CGeoMapView* pView);
	// ���ƶ�״̬
	virtual void DrawMoveState(CGeoMapView* pView);
	// ���ı��С״̬
	virtual void DrawSizeState(CGeoMapView* pView);
	
	// �������߷����ı�ʱ
	virtual void OnScaleChanged();
	// ��������תʱ
	virtual void OnAngleChanged();
	// ����������ԭ���иı�ʱ
	virtual void OnCoordOrigChanged();
};

#endif // !defined(AFX_GEOSCALEOBJ_H__F9FD6253_3F4E_4992_9AE0_081D05127724__INCLUDED_)
