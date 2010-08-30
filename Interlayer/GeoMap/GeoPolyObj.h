// GeoPolyObj.h: interface for the CGeoPolyObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GEOPOLYOBJ_H__12299C8B_D96F_4199_A0DD_BBF39F3433EF__INCLUDED_)
#define AFX_GEOPOLYOBJ_H__12299C8B_D96F_4199_A0DD_BBF39F3433EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GeoDrawObj.h"
#include "CommonStruct.h"

/* �������νṹ */

struct LINESEG
{
	CPoint2D s;
	CPoint2D e; 
	LINESEG(CPoint2D a, CPoint2D b) { s=a; e=b;}
	LINESEG() { }
};

struct LINE           // ֱ�ߵĽ������� a*x+b*y+c=0  Ϊͳһ��ʾ��Լ�� a >= 0
{
	double a;
	double b;
	double c; LINE(double d1=1, double d2=-1, double d3=0) {a=d1; b=d2; c=d3;}
};

class CGeoPolyObj : public CGeoDrawObj  
{
	DECLARE_SERIAL(CGeoPolyObj);
public:
	CGeoPolyObj();
	CGeoPolyObj(CDrawLayer* pLayer);
	CGeoPolyObj(const CRect& position, CDrawLayer* pLayer);
	virtual ~CGeoPolyObj();
	  
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

	enum POLYTYPE		// ��������
	{
		generic = 0,			// ��ͨ����
		interlayers,			// ���в���
		singlelayer,			// ���в���
		railway,				// ��·
		road,					// ��·
		national_boundaries,	// ����
		province_boundaries,	// ʡ��
		city_boundaries,		// ������н�
		county_boundaries,		// �ؽ�
		village_boundaries,		// ���
		text,					// �ı�����  -A---B---C
		label,					// ��ע���� ---1234---------1234--------
		faultage,				// �ϲ�
		faultage_up,			// �ϲ�����
		faultage_down,			// �ϲ�����
		contour,				// ��ֵ��
		contour_fill,			// ��ֵ���
	};
public:
	int				m_nPoints;			// ���Ƶ���
	int				m_nAllocPoints;		// ����������
	CPoint*			m_points;			// ���Ƶ�ͼ������
	CPoint2D*		m_geoPoints;		// ���Ƶ�������

	ELEMENTLINE		m_eLine;			// ϸ������
	ELEMENTFILL		m_eFill;			// �������

	ELEMENTLINE		m_eLine2;			// ��������
	int				m_iSpaceWidth;		// �߼���

	CDashLine		DashLine;			// ����
	CRgn			m_rgn;				// 
	
	int				m_nHandleMoving;	// �����ƶ����ֱ�

	POLYTYPE		m_ePolyType;		// �������

	CString			m_strText;			// ��ע�ı�
	LOGFONT			m_lfFont;			// ��ע���弰��С
	COLORREF		m_clrText;			// ��ɫ
	BOOL			m_bShowText;		// �Ƿ���ʾ

	double			GetValue(){return atof(m_strText);}
	void			SetValue(double z, int iDeci) // iDeci С��λ
	{ 
		CString strFmt;
		strFmt.Format(_T("%%.%dlf"),iDeci);
		m_strText.Format(strFmt,z);
	}

public:
	// ����ͼ������
	virtual void CopyAttribute(CGeoDrawObj *pSrcObj);
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

	// ������Ĵ���
	void SwitchOrderPoint();

private:
	BOOL IsPoint(CPoint point, CGeoMapView *pView);	
	
	void CalcArrowRgn(CPoint *pt, BOOL bEnd = TRUE);
	void DrawArrow(CDC *pDC, CGeoMapView *pView);
	void DrawLineSpace(CDC *pDC, CGeoMapView *pView);
	void SelectClipRgn(CDC *pDC);
	void DrawLine(CDC *pDC, CGeoMapView *pView, PLACE place, BOOL bThin = TRUE);
	void DrawFill(CDC *pDC, CGeoMapView *pView, PLACE place);

	BOOL CreateBrush(CBrush *pBrush, CDC *pDC);
	BOOL CreatePen(CPen *pPen, CDC *pDC, BOOL bThin);
	BOOL CreatePenLine(CPen *pPen, CDC *pDC);
};

#endif // !defined(AFX_GEOPOLYOBJ_H__12299C8B_D96F_4199_A0DD_BBF39F3433EF__INCLUDED_)
