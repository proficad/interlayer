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
	CString m_strFileName;	// 图片文件名
	CxImage *pImage;
	BOOL	m_bLockScale;	// 锁定比例
	CArray<CPoint, CPoint&> m_ptCtrl;
	CArray<CPoint2D,CPoint2D&> m_ptGeoCtrl;
	CStringArray m_vecWellName;

	double m_dAngle;	// 图片旋转角度

	BOOL m_bEditPoint; // 是否正在编辑控制点
public:

	void LoadPictrue();
	
	virtual void Serialize(CArchive& ar);
	virtual CGeoDrawObj* Clone(CDrawLayer *pLayer);
	virtual BOOL OnEditProperties(CGeoMapView* pView = NULL);
	// 画图素
	virtual void Draw(CDC* pDC, CGeoMapView* pView = NULL);
	// 移动图素大小
	//virtual void MoveHandleTo(int nHandle, CPoint point, CGeoMapView* pView = NULL);
	// 移动图素位置
	virtual void MoveTo(const CRect& position, CGeoMapView* pView = NULL);
	// 当比例尺发生改变时
	virtual void OnScaleChanged();
	// 当发生旋转时
	virtual void OnAngleChanged();
	// 当发生坐标原点有改变时
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
