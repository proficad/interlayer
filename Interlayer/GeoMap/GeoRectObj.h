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
	ELEMENTFILL		m_eFill;			// �������
	ELEMENTLINE		m_eLine2;			// ������
	int				m_iSpaceWidth;		// �߼���
public:
	// ����ͼ������
	virtual void CopyAttribute(CGeoDrawObj *pSrcObj);
	virtual void Serialize(CArchive& ar);
	virtual CGeoDrawObj* Clone(CDrawLayer *pLayer);
	virtual BOOL OnEditProperties(CGeoMapView* pView = NULL);
	// ��ͼ��
	virtual void Draw(CDC* pDC, CGeoMapView* pView = NULL);

	virtual int HitTestHandle(CPoint point, CGeoMapView* pView, BOOL bSelected);
	
	// �������߷����ı�ʱ
	virtual void OnScaleChanged();
	// ��������תʱ
	virtual void OnAngleChanged();
	// ����������ԭ���иı�ʱ
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
