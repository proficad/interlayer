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
	// ����Ϣ
	CPoint2D	m_ptSymbolCoord;// ��ʵ������
	CPoint		m_ptSymbolPoint;	// ��ͼ������ 

	CString		m_strName;	// ����

	// ��������
	CPoint		m_ptName;	// �����λ��(���)
	LOGFONT		m_lfFont;		// ������
	COLORREF	m_clrFont;		// ������ɫ

	// ��������
	int			m_iSymbolSize;		// ����Ŵ�С�ߴ�(0.1mm)
	COLORREF	m_clrSymbolPen;		// ������ɫ
	COLORREF	m_clrSymbolFill;	// �����ɫ

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
	// �ƶ�ͼ�ش�С
	virtual void MoveHandleTo(int nHandle, CPoint point, CGeoMapView* pView = NULL);
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