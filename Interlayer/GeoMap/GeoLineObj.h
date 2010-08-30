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
	
	enum PLACE	// �߻����ķ���λ��
	{
		INNER,	// ��
			MIDST,	// ��
			OUTER	// ��
	};
	
	enum LINETYPE	// ����
	{
		SINGLE_LINE,	// ����
			DUBLE_LINE,		// ˫��
			THREE_LINE		// ����
	};
	
	CRgn			m_rgn;				// 
public:
	ELEMENTLINE		m_eLine;			// ������

	CPoint			m_points[2];		// ���Ƶ�ͼ������
	CPoint2D		m_geoPoints[2];		// ���Ƶ�������
public:
	// ����ͼ������
	virtual void CopyAttribute(CGeoDrawObj *pSrcObj);
	virtual void Serialize(CArchive& ar);
	virtual CGeoDrawObj* Clone(CDrawLayer *pLayer);
	virtual BOOL OnEditProperties(CGeoMapView* pView = NULL);
	// ��ͼ��
	virtual void Draw(CDC* pDC, CGeoMapView* pView = NULL);
	void CalcArrowRgn(CPoint *pt, BOOL bEnd = TRUE);
	void DrawArrow(CDC *pDC, CGeoMapView *pView);
	// ���༭״̬
	virtual void DrawEditMode(CGeoMapView* pView);
	// ���ƶ�״̬
	virtual void DrawMoveState(CGeoMapView* pView);
	// ���ı��С״̬
	virtual void DrawSizeState(CGeoMapView* pView);
	virtual void MoveHandleTo(int nHandle, CPoint point, CGeoMapView* pView = NULL);
	// �ƶ�ͼ��λ��
	virtual void MoveTo(const CRect& position, CGeoMapView* pView = NULL);
	virtual CPoint GetHandle(int nHandle);
	virtual HCURSOR GetHandleCursor(int nHandle);
	virtual BOOL Intersects(const CRect& rect);
	virtual int HitTestHandle(CPoint point, CGeoMapView* pView, BOOL bSelected);
	
	virtual BOOL RecalcBounds(CGeoMapView* pView = NULL);
	
	// �������߷����ı�ʱ
	virtual void OnScaleChanged();
	// ��������תʱ
	virtual void OnAngleChanged();
	// ����������ԭ���иı�ʱ
	virtual void OnCoordOrigChanged();
	// ת�������ı�ʱ
	virtual void OnConvertParamChanged();
	// ����㼯
	virtual void ReversePoints();
	
	BOOL IsPoint(CPoint point, CGeoMapView *pView);
	
	void DrawLine(CDC *pDC, CGeoMapView *pView, PLACE place, BOOL bThin = TRUE);

	BOOL CreatePen(CPen *pPen, CDC *pDC, BOOL bThin);

	virtual int GetHandleCount();
};


#endif // !defined(AFX_GEOLINEOBJ_H__31A7D0ED_A41C_482B_AC3E_1BC94D072F9E__INCLUDED_)
