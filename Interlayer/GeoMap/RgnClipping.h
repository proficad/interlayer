#pragma once
#include "geodrawobj.h"

class CRgnClipping : public CGeoDrawObj  
{
	DECLARE_SERIAL(CRgnClipping);
public:
	CRgnClipping();
	CRgnClipping(CDrawLayer* pLayer);
	CRgnClipping(const CRect& position, CDrawLayer* pLayer);
	virtual ~CRgnClipping();

public:
	int				m_nPoints;			// 控制点数
	int				m_nAllocPoints;		// 最大申请点数
	CPoint*			m_points;			// 控制点图形坐标
	CPoint2D*		m_geoPoints;		// 控制点大地坐标
	BOOL			m_bSmooth;			// 是否光滑
	CRgn			m_rgn;				// 

public:
	virtual int HitTestHandle(CPoint point, CGeoMapView* pView, BOOL bSelected);
	virtual HCURSOR GetHandleCursor(int nHandle);
	virtual CPoint GetHandle(int nHandle);
	virtual int GetHandleCount();
	virtual void Serialize(CArchive& ar);
	virtual CGeoDrawObj* Clone(CDrawLayer *pLayer);
	virtual BOOL OnEditProperties(CGeoMapView* pView = NULL);
	// 移动图素大小
	virtual void MoveHandleTo(int nHandle, CPoint point, CGeoMapView* pView = NULL);
	// 移动图素位置
	virtual void MoveTo(const CRect& position, CGeoMapView* pView = NULL);
	// 画图素
	virtual void Draw(CDC* pDC, CGeoMapView* pView = NULL);
	// 画编辑状态
	virtual void DrawEditMode(CGeoMapView* pView);
	// 画移动状态
	virtual void DrawMoveState(CGeoMapView* pView);
	// 画改变大小状态
	virtual void DrawSizeState(CGeoMapView* pView);

	//
	virtual void DrawTracker(CDC *pDC, CGeoMapView *pView);

	// 当比例尺发生改变时
	virtual void OnScaleChanged();
	// 当发生旋转时
	virtual void OnAngleChanged();
	// 当发生坐标原点有改变时
	virtual void OnCoordOrigChanged();

	//////////////////////////////////////////////////////////////////////////
	void AddPoint(const CPoint& point, CGeoMapView* pView = NULL);
	void AddPoint(const CPoint2D& point, CGeoMapView* pView = NULL);
	void DelPoint(int nIndex, CGeoMapView *pView);
	void DelPoint(const CPoint &point, CGeoMapView *pView);
	void DeletePoint(int nIndex, CGeoMapView *pView);
	void InsertPoint(const CPoint &point, int nIndex, CGeoMapView *pView);
	void InsertPoint(const CPoint &point, CGeoMapView *pView);
	void MovePointTo(const CPoint &point, CGeoMapView *pView);

	virtual CPoint GetNearByIndex(const CPoint &point, int *iIndex);
	virtual int GetPathPoint(LPPOINT &lppt);
	// 是否点中图素边
	virtual BOOL IsHitOnBorder(CPoint point, CGeoMapView *pView);

	virtual BOOL RecalcBounds(CGeoMapView* pView = NULL);

	BOOL GetClipRgn(CDC *pDC, CRgn &rgn);	
	void SelectClipRgn(CDC *pDC);

private:
	BOOL IsPoint(CPoint point, CGeoMapView *pView);	

	void DrawLine(CDC *pDC, CGeoMapView *pView);
};
