// GeoPictrueObj.h: interface for the CGeoPictrueObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GEOPICTRUEOBJ_H__B9E6D452_28DC_4F63_B276_364F2C7B2275__INCLUDED_)
#define AFX_GEOPICTRUEOBJ_H__B9E6D452_28DC_4F63_B276_364F2C7B2275__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GeoDrawObj.h"

class CGeoPictrueObj : public CGeoDrawObj  
{
	DECLARE_SERIAL(CGeoPictrueObj);
public:
	CGeoPictrueObj();	
	CGeoPictrueObj(CDrawLayer* pLayer);
	CGeoPictrueObj(const CRect& position, CDrawLayer* pLayer);
	
	virtual ~CGeoPictrueObj();
public:
	CString m_strFileName;	// ͼƬ�ļ���
	CxImage *pImage;
	BOOL	m_bLockScale;	// ��������
	CArray<CPoint, CPoint&> m_ptCtrl;
	CArray<CPoint2D,CPoint2D&> m_ptGeoCtrl;
	CStringArray m_vecWellName;

	double m_dAngle;	// ͼƬ��ת�Ƕ�

	BOOL m_bEditPoint; // �Ƿ����ڱ༭���Ƶ�
public:

	void LoadPictrue();
	
	virtual void Serialize(CArchive& ar);
	virtual CGeoDrawObj* Clone(CDrawLayer *pLayer);
	virtual BOOL OnEditProperties(CGeoMapView* pView = NULL);
	// ��ͼ��
	virtual void Draw(CDC* pDC, CGeoMapView* pView = NULL);
	// �ƶ�ͼ�ش�С
	//virtual void MoveHandleTo(int nHandle, CPoint point, CGeoMapView* pView = NULL);
	// �ƶ�ͼ��λ��
	virtual void MoveTo(const CRect& position, CGeoMapView* pView = NULL);
	// �������߷����ı�ʱ
	virtual void OnScaleChanged();
	// ��������תʱ
	virtual void OnAngleChanged();
	// ����������ԭ���иı�ʱ
	virtual void OnCoordOrigChanged();
	
};

struct DocType
{
public:
	int nID;
	BOOL bRead;
	BOOL bWrite;
	const char* description;
	const char* ext;
};

#endif // !defined(AFX_GEOPICTRUEOBJ_H__B9E6D452_28DC_4F63_B276_364F2C7B2275__INCLUDED_)
