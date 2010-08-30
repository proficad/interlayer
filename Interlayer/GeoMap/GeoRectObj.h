// GeoRectObj.h: interface for the CGeoRectObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GEORECTOBJ_H__F3B7E493_2F1D_4CCB_A11A_4C3CAF36C097__INCLUDED_)
#define AFX_GEORECTOBJ_H__F3B7E493_2F1D_4CCB_A11A_4C3CAF36C097__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GeoDrawObj.h"
#include "CommonStruct.h"

class CGeoRectObj : public CGeoDrawObj  
{
	DECLARE_SERIAL(CGeoRectObj);
public:
	CGeoRectObj();	
	CGeoRectObj(CDrawLayer* pLayer);
	CGeoRectObj(const CRect& position, CDrawLayer* pLayer);

	virtual ~CGeoRectObj();

	enum PLACE	// 线或填充的放置位置
	{
		INNER,	// 内
			MIDST,	// 中
			OUTER	// 外
	};
	
	enum LINETYPE	// 线型
	{
		SINGLE_LINE,	// 单线
			DUBLE_LINE,		// 双线
			THREE_LINE		// 三线
	};

	CRgn			m_rgn;				// 
public:
	ELEMENTLINE		m_eLine;			// 线属性
	ELEMENTFILL		m_eFill;			// 填充属性
	ELEMENTLINE		m_eLine2;			// 线属性
	int				m_iSpaceWidth;		// 线间宽度
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

	void DrawLineSpace(CDC *pDC, CGeoMapView *pView);
	void SelectClipRgn(CDC *pDC);
	void DrawLine(CDC *pDC, CGeoMapView *pView, PLACE place, BOOL bThin = TRUE);
	void DrawFill(CDC *pDC, CGeoMapView *pView, PLACE place);

	BOOL CreateBrush(CBrush *pBrush, CDC *pDC);
	BOOL CreatePen(CPen *pPen, CDC *pDC, BOOL bThin);
	BOOL CreatePenLine(CPen *pPen, CDC *pDC);
};

#endif // !defined(AFX_GEORECTOBJ_H__F3B7E493_2F1D_4CCB_A11A_4C3CAF36C097__INCLUDED_)
