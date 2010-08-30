#pragma once
#include "geodrawobj.h"

class CGeoPointObj : public CGeoDrawObj  
{
	DECLARE_SERIAL(CGeoPointObj);
public:
	CGeoPointObj();
	CGeoPointObj(CDrawLayer *pLayer);
	CGeoPointObj(const CPoint &pt, CDrawLayer *pLayer, LPCSTR strName = NULL);
	CGeoPointObj(const CPoint2D &pt, CDrawLayer *pLayer, LPCSTR strName = NULL);
	virtual ~CGeoPointObj();
public:
	// 点信息
	CPoint2D	m_ptSymbolCoord;// 点实际坐标
	CPoint		m_ptSymbolPoint;	// 点图形坐标 

	CString		m_strName;	// 点名

	// 井号属性
	CPoint		m_ptName;	// 点绘制位置(相对)
	LOGFONT		m_lfFont;		// 点字体
	COLORREF	m_clrFont;		// 字体颜色

	// 符号属性
	int			m_iSymbolSize;		// 点符号大小尺寸(0.1mm)
	COLORREF	m_clrSymbolPen;		// 线条颜色
	COLORREF	m_clrSymbolFill;	// 填充颜色

public:
	virtual void CopyAttribute(CGeoDrawObj *pSrcObj);
	virtual BOOL RecalcBounds(CGeoMapView* pView = NULL);
	virtual int HitTestHandle(CPoint point, CGeoMapView* pView, BOOL bSelected);
	virtual HCURSOR GetHandleCursor(int nHandle);
	virtual CPoint GetHandle(int nHandle);
	virtual int GetHandleCount();
	virtual void Serialize(CArchive& ar);
	virtual CGeoDrawObj* Clone(CDrawLayer *pLayer);
	virtual BOOL OnEditProperties(CGeoMapView* pView = NULL);
	virtual void MoveTo(const CRect& position, CGeoMapView* pView = NULL);
	// 移动图素大小
	virtual void MoveHandleTo(int nHandle, CPoint point, CGeoMapView* pView = NULL);
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