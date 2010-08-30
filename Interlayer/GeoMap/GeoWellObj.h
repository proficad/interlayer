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
	// ��������Ϣ
	CPoint2D	m_ptSymbolCoord;// ��λ����
	CPoint		m_ptSymbolWell;	// ��λͼ������ 
	double		m_dElevation;	// ��о����

	CString		m_strWellName;	// ���Ż���
	CString 	m_strWellType;	// ������� ,���;���עˮ����̽��
	CString		m_strWellStyle;	// ���ʹ��� ˮƽ������ֱ��
	
	// ��������
	CPoint		m_ptWellName;	// ��������λ��(���)
	LOGFONT		m_lfFont;		// ��������
	COLORREF	m_clrFont;		// ������ɫ

	// ��������
	int			m_iSymbolSize;		// �����Ŵ�С�ߴ�(0.1mm)
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

#endif // !defined(AFX_GEOWELLOBJ_H__D974BADC_8005_4E83_895E_B6DA2C9AAE21__INCLUDED_)
