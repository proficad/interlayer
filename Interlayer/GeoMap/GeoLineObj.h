// GeoLineObj.h: interface for the CGeoLineObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GEOLINEOBJ_H__31A7D0ED_A41C_482B_AC3E_1BC94D072F9E__INCLUDED_)
#define AFX_GEOLINEOBJ_H__31A7D0ED_A41C_482B_AC3E_1BC94D072F9E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GeoDrawObj.h"
#include "CommonStruct.h"

class CGeoLineObj : public CGeoDrawObj  
{
	DECLARE_SERIAL(CGeoLineObj);
public:
	CGeoLineObj();	
	CGeoLineObj(CDrawLayer* pLayer);
	CGeoLineObj(const CRect& position, CDrawLayer* pLayer);
	
	virtual ~CGeoLineObj();
	
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

	CPoint			m_points[2];		// 控制点图形坐标
	CPoint2D		m_geoPoints[2];		// 控制点大地坐标
public:
	// 拷贝图素属性
	virtual void CopyAttribute(CGeoDrawObj *pSrcObj);
	virtual void Serialize(CArchive& ar);
	virtual CGeoDrawObj* Clone(CDrawLayer *pLayer);
	virtual BOOL OnEditProperties(CGeoMapView* pView = NULL);
	// 画图素
	virtual void Draw(CDC* pDC, CGeoMapView* pView = NULL);
	void CalcArrowRgn(CPoint *pt, BOOL bEnd = TRUE);
	void DrawArrow(CDC *pDC, CGeoMapView *pView);
	// 画编辑状态
	virtual void DrawEditMode(CGeoMapView* pView);
	// 画移动状态
	virtual void DrawMoveState(CGeoMapView* pView);
	// 画改变大小状态
	virtual void DrawSizeState(CGeoMapView* pView);
	virtual void MoveHandleTo(int nHandle, CPoint point, CGeoMapView* pView = NULL);
	// 移动图素位置
	virtual void MoveTo(const CRect& position, CGeoMapView* pView = NULL);
	virtual CPoint GetHandle(int nHandle);
	virtual HCURSOR GetHandleCursor(int nHandle);
	virtual BOOL Intersects(const CRect& rect);
	virtual int HitTestHandle(CPoint point, CGeoMapView* pView, BOOL bSelected);
	
	virtual BOOL RecalcBounds(CGeoMapView* pView = NULL);
	
	// 当比例尺发生改变时
	virtual void OnScaleChanged();
	// 当发生旋转时
	virtual void OnAngleChanged();
	// 当发生坐标原点有改变时
	virtual void OnCoordOrigChanged();
	// 转换参数改变时
	virtual void OnConvertParamChanged();
	// 逆序点集
	virtual void ReversePoints();
	
	BOOL IsPoint(CPoint point, CGeoMapView *pView);
	
	void DrawLine(CDC *pDC, CGeoMapView *pView, PLACE place, BOOL bThin = TRUE);

	BOOL CreatePen(CPen *pPen, CDC *pDC, BOOL bThin);

	virtual int GetHandleCount();
};


#endif // !defined(AFX_GEOLINEOBJ_H__31A7D0ED_A41C_482B_AC3E_1BC94D072F9E__INCLUDED_)
