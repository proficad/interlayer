// GeoWellObj.h: interface for the CGeoWellObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GEOWELLOBJ_H__D974BADC_8005_4E83_895E_B6DA2C9AAE21__INCLUDED_)
#define AFX_GEOWELLOBJ_H__D974BADC_8005_4E83_895E_B6DA2C9AAE21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GeoDrawObj.h"

class CGeoWellObj : public CGeoDrawObj  
{
	DECLARE_SERIAL(CGeoWellObj);
public:
	CGeoWellObj();
	CGeoWellObj(CDrawLayer *pLayer);
	CGeoWellObj(const CPoint &pt, CDrawLayer *pLayer, LPCSTR strWellName = NULL);
	CGeoWellObj(const CPoint2D &pt, CDrawLayer *pLayer, LPCSTR strWellName = NULL);
	virtual ~CGeoWellObj();
public:
	// 井基本信息
	CPoint2D	m_ptSymbolCoord;// 井位坐标
	CPoint		m_ptSymbolWell;	// 井位图形坐标 
	double		m_dElevation;	// 补芯海拔

	CString		m_strWellName;	// 井号或井名
	CString 	m_strWellType;	// 井别代码 ,采油井，注水井，探井
	CString		m_strWellStyle;	// 井型代码 水平井，垂直井
	
	// 井号属性
	CPoint		m_ptWellName;	// 井名绘制位置(相对)
	LOGFONT		m_lfFont;		// 井号字体
	COLORREF	m_clrFont;		// 字体颜色

	// 符号属性
	int			m_iSymbolSize;		// 井符号大小尺寸(0.1mm)
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

#endif // !defined(AFX_GEOWELLOBJ_H__D974BADC_8005_4E83_895E_B6DA2C9AAE21__INCLUDED_)
