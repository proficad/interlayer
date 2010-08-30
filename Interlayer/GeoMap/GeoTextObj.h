#pragma once
#include "GeoDrawObj.h"

class CGeoTextObj :
	public CGeoDrawObj
{
	DECLARE_SERIAL(CGeoTextObj);
public:
	CGeoTextObj(void);
	CGeoTextObj(CDrawLayer* pLayer);
	CGeoTextObj(const CRect& position, CDrawLayer* pLayer);
public:
	virtual ~CGeoTextObj(void);
public:
	CRgn			m_rgn;				// 

	CString			m_strText;			// 文本
	COLORREF		m_clrText;			// 字体颜色
	int				m_nFormat;			// 格式
	LOGFONT			m_lfFont;			// 字体
public:
	// 拷贝图素属性
	virtual void CopyAttribute(CGeoDrawObj *pSrcObj);
	virtual void Serialize(CArchive& ar);
	virtual CGeoDrawObj* Clone(CDrawLayer *pLayer);
	virtual BOOL OnEditProperties(CGeoMapView* pView = NULL);
	// 画图素
	virtual void Draw(CDC* pDC, CGeoMapView* pView = NULL);

	virtual int HitTestHandle(CPoint point, CGeoMapView* pView, BOOL bSelected);

	// 当比例尺发生改变时
	virtual void OnScaleChanged();
	// 当发生旋转时
	virtual void OnAngleChanged();
	// 当发生坐标原点有改变时
	virtual void OnCoordOrigChanged();

	BOOL IsPoint(CPoint point, CGeoMapView *pView);
	void SelectClipRgn(CDC *pDC);
};
