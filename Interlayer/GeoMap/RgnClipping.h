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
	int				m_nPoints;			// ���Ƶ���
	int				m_nAllocPoints;		// ����������
	CPoint*			m_points;			// ���Ƶ�ͼ������
	CPoint2D*		m_geoPoints;		// ���Ƶ�������
	BOOL			m_bSmooth;			// �Ƿ�⻬
	CRgn			m_rgn;				// 

public:
	virtual int HitTestHandle(CPoint point, CGeoMapView* pView, BOOL bSelected);
	virtual HCURSOR GetHandleCursor(int nHandle);
	virtual CPoint GetHandle(int nHandle);
	virtual int GetHandleCount();
	virtual void Serialize(CArchive& ar);
	virtual CGeoDrawObj* Clone(CDrawLayer *pLayer);
	virtual BOOL OnEditProperties(CGeoMapView* pView = NULL);
	// �ƶ�ͼ�ش�С
	virtual void MoveHandleTo(int nHandle, CPoint point, CGeoMapView* pView = NULL);
	// �ƶ�ͼ��λ��
	virtual void MoveTo(const CRect& position, CGeoMapView* pView = NULL);
	// ��ͼ��
	virtual void Draw(CDC* pDC, CGeoMapView* pView = NULL);
	// ���༭״̬
	virtual void DrawEditMode(CGeoMapView* pView);
	// ���ƶ�״̬
	virtual void DrawMoveState(CGeoMapView* pView);
	// ���ı��С״̬
	virtual void DrawSizeState(CGeoMapView* pView);

	//
	virtual void DrawTracker(CDC *pDC, CGeoMapView *pView);

	// �������߷����ı�ʱ
	virtual void OnScaleChanged();
	// ��������תʱ
	virtual void OnAngleChanged();
	// ����������ԭ���иı�ʱ
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
	// �Ƿ����ͼ�ر�
	virtual BOOL IsHitOnBorder(CPoint point, CGeoMapView *pView);

	virtual BOOL RecalcBounds(CGeoMapView* pView = NULL);

	BOOL GetClipRgn(CDC *pDC, CRgn &rgn);	
	void SelectClipRgn(CDC *pDC);

private:
	BOOL IsPoint(CPoint point, CGeoMapView *pView);	

	void DrawLine(CDC *pDC, CGeoMapView *pView);
};
