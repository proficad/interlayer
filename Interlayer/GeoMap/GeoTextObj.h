#pragma once
#include "GeoDrawObj.h"

class CGeoTextObj :
	public CGeoDrawObj
{
	DECLARE_SERIAL(CGeoTextObj);
public:
	CGeoTextObj(void);
	CGeoTextObj(CDrawLayer* pLayer);
	CGeoTextObj(const CRect& position, CDrawLayer* pLayer);
public:
	virtual ~CGeoTextObj(void);
public:
	CRgn			m_rgn;				// 

	CString			m_strText;			// �ı�
	COLORREF		m_clrText;			// ������ɫ
	int				m_nFormat;			// ��ʽ
	LOGFONT			m_lfFont;			// ����
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
	void SelectClipRgn(CDC *pDC);
};
