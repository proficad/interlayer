#pragma once

#include "DrawLayer.h"

class CDrawPage : public CRefObject  
{
public:
	CDrawPage(CGeoMapDoc* pDocument);
	virtual ~CDrawPage();
	CDrawPage();

	//����
public:
	// ����
	// �������� x * y ҳ
	int					m_nHorzPages;			// ������ҳ������ 
	int					m_nVertPages;			// ������ҳ������

	BOOL				m_bLocked;				// ����
	CString				m_strPageName;			// ҳ������
	CString				m_strMark;				// ��ע˵��
	CString				m_strID;				// ҳ��ID��ʶ;
	BOOL				m_bPagePanel;			// �Ƿ���ʾ����


	// ����ԭ������ x�ϱ� y���� m_dValue����������н�--˳ʱ��Ϊ��(��)
	CPoint2D			m_ptCoordOrig;
	double				m_lScale;				// ������������
	double				m_ground2screen[6];
	double				m_screen2ground[6];
	BOOL				m_bParameter;			// �Ƿ��Ѿ�����ƽ���Ĳ���
	//------------------------------------------------------------------------
	// ƽ���Ĳ���
	//
	// KΪ�߶����ӣ���Ϊ��ת�Ƕȣ�Lx\Ly���Ӧ��ƽ�Ʋ���
	//
	// X=Lx + X1*Kcos��- Y1*Ksin��
	// Y=LY + Y1*Kcos��+ X1*Ksin��
	//
	// ת��Ϊ
	//
	// X=a+X1*c-Y1*d
	// Y=b+Y1*c+X1*d
	//
	// ����
	// a = Lx, b = Ly, c = Kcos��, d = Ksin��;
	//
	// K = 1/m_lScale
	// ��= m_ptCoordOrig.m_dValue*PI/180.0;
	// Lx = m_ptCoordOrig.x
	// Ly = m_ptCoordOrig.y
	//	

	int 				m_lZoom;				// ��Ļ��������
	CPoint				m_ptPosition;			// ��ǰ��Ļ����λ��			

	CDrawLayerList		m_DrawLayers;			// ͼ���б�	
	CString				m_strCurLayerID;		// ��ǰͼ��ID
	int					m_iMarginWidth;			// �ַ���ӡʱ�غϿ��

	//------------------------------------------------------------------------
	// ������
	CDrawLayer *m_pCurDrawLayer;				// ��ǰͼ��
	CGeoMapDoc *m_pDocument;					// ��ǰ�ĵ�	

public:
	// ��ȡ����������н�--˳ʱ��Ϊ��(��)
	double GetAngle() { return m_ptCoordOrig.m_dValue; }

	// ��������������н�--˳ʱ��Ϊ��(��)
	void SetAngle(double dAngle) { m_ptCoordOrig.m_dValue = dAngle; }

private:

	//����
public:	
	// ��������ȡͼ��ָ��
	CDrawLayer* GetLayer(const int& iIndex);
	CDrawLayer* GetLayer(const CString &strID);

	// ��ȡͼ��ID
	CString GetLayerID(const int& iIndex);
	CString GetLayerID(const CDrawLayer* pLayer);

	// ��ȡ��ҳ�������ж�����ռ�����С
	CSize GetPageSize();
	// ��ȡ��ҳ�������ж�����ռ����Χ
	CRect GetPageRect();

	int GetLayerCount() { return m_DrawLayers.GetCount();} // ͼ������
	void SetCurLayer(CDrawLayer *pLayer);			// ���õ�ǰͼ��	
	CDrawLayer *GetCurLayer();						// ��ȡ��ǰͼ��

	BOOL Add(CDrawLayer *pObj);						// ����ͼ��
	BOOL Remove(CDrawLayer *pObj);					// ɾ��ͼ��
	void RemoveAll();								// ɾ������ͼ��

	void Draw(CDC *pDC, CGeoMapView *pView);		// ��ʾͼ��
	virtual void Serialize(CArchive& ar);			// ���л�����

	// ͼ������
	BOOL MoveBack(CDrawLayer *pLayer);
	BOOL MoveForward(CDrawLayer *pLayer);
	BOOL MoveToBack(CDrawLayer *pLayer);
	BOOL MoveToFront(CDrawLayer *pLayer);

	// 
	void OnEditProperties(CGeoMapView* pView = NULL);
	void OnScaleChanged();

	// ����Ǵ������ϵ����ת���ɴ������(geodetic coordinates)
	// �������������ϵ����Ϊͼ����������
	CPoint2D CoordinateTransfer(const CPoint &pt);
	CPoint CoordinateTransfer(const CPoint2D &pt);
	CRectEx CoordinateTransfer(const CRect &rect);
	CRect CoordinateTransfer(const CRectEx &rect);

	CPoint2D CoordinateTransferNew(const CPoint &pt);
	CPoint CoordinateTransferNew(const CPoint2D &pt);
	CRectEx CoordinateTransferNew(const CRect &rect);
	CRect CoordinateTransferNew(const CRectEx &rect);

	// ���캢��
	void ClaimChildren();

protected:
	DECLARE_SERIAL(CDrawPage);
private:

};

