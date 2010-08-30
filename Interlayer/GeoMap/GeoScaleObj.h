// GeoScaleObj.h: interface for the CGeoScaleObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GEOSCALEOBJ_H__F9FD6253_3F4E_4992_9AE0_081D05127724__INCLUDED_)
#define AFX_GEOSCALEOBJ_H__F9FD6253_3F4E_4992_9AE0_081D05127724__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GeoDrawObj.h"

// 比例尺
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
	LOGFONT		m_lfFont;		// 比例字体
	COLORREF	m_clrFont;		// 字体颜色
	COLORREF	m_clrPen;		// 线颜色
	int			m_nWidth;		// 比例尺宽度
	int			m_nHeight;		// 比例尺高度

public:
	virtual int HitTestHandle(CPoint point, CGeoMapView* pView, BOOL bSelected);

	virtual int GetHandleCount();
	virtual void Serialize(CArchive& ar);
	virtual CGeoDrawObj* Clone(CDrawLayer *pLayer);
	virtual BOOL OnEditProperties(CGeoMapView* pView = NULL);

	// 画图素
	virtual void Draw(CDC* pDC, CGeoMapView* pView = NULL);
	// 画编辑状态
	virtual void DrawEditMode(CGeoMapView* pView);
	// 画移动状态
	virtual void DrawMoveState(CGeoMapView* pView);
	// 画改变大小状态
	virtual void DrawSizeState(CGeoMapView* pView);
	
	// 当比例尺发生改变时
	virtual void OnScaleChanged();
	// 当发生旋转时
	virtual void OnAngleChanged();
	// 当发生坐标原点有改变时
	virtual void OnCoordOrigChanged();
};

#endif // !defined(AFX_GEOSCALEOBJ_H__F9FD6253_3F4E_4992_9AE0_081D05127724__INCLUDED_)
