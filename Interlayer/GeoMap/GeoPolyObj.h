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

/* 基本几何结构 */

struct LINESEG
{
	CPoint2D s;
	CPoint2D e; 
	LINESEG(CPoint2D a, CPoint2D b) { s=a; e=b;}
	LINESEG() { }
};

struct LINE           // 直线的解析方程 a*x+b*y+c=0  为统一表示，约定 a >= 0
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
	  
	enum PLACE	// 线或填充的放置位置
	{
		INNER,	// 内
		MIDST,	// 中
		OUTER	// 外
	};
	
	enum LINETYPE	// 线型
	{
		SINGLE_LINE,	// 单线
		DUBLE_LINE,		// 双线
		THREE_LINE		// 三线
	};

	enum POLYTYPE		// 曲线类型
	{
		generic = 0,			// 普通曲线
		interlayers,			// 复夹层线
		singlelayer,			// 单夹层线
		railway,				// 铁路
		road,					// 公路
		national_boundaries,	// 国界
		province_boundaries,	// 省界
		city_boundaries,		// 区界或市界
		county_boundaries,		// 县界
		village_boundaries,		// 乡界
		text,					// 文本曲线  -A---B---C
		label,					// 标注曲线 ---1234---------1234--------
		faultage,				// 断层
		faultage_up,			// 断层上盘
		faultage_down,			// 断层上盘
		contour,				// 等值线
		contour_fill,			// 等值填充
	};
public:
	int				m_nPoints;			// 控制点数
	int				m_nAllocPoints;		// 最大申请点数
	CPoint*			m_points;			// 控制点图形坐标
	CPoint2D*		m_geoPoints;		// 控制点大地坐标

	ELEMENTLINE		m_eLine;			// 细线属性
	ELEMENTFILL		m_eFill;			// 填充属性

	ELEMENTLINE		m_eLine2;			// 粗线属性
	int				m_iSpaceWidth;		// 线间宽度

	CDashLine		DashLine;			// 线型
	CRgn			m_rgn;				// 
	
	int				m_nHandleMoving;	// 正在移动的手柄

	POLYTYPE		m_ePolyType;		// 曲线类别

	CString			m_strText;			// 标注文本
	LOGFONT			m_lfFont;			// 标注字体及大小
	COLORREF		m_clrText;			// 颜色
	BOOL			m_bShowText;		// 是否显示

	double			GetValue(){return atof(m_strText);}
	void			SetValue(double z, int iDeci) // iDeci 小数位
	{ 
		CString strFmt;
		strFmt.Format(_T("%%.%dlf"),iDeci);
		m_strText.Format(strFmt,z);
	}

public:
	// 拷贝图素属性
	virtual void CopyAttribute(CGeoDrawObj *pSrcObj);
	virtual int HitTestHandle(CPoint point, CGeoMapView* pView, BOOL bSelected);
	virtual HCURSOR GetHandleCursor(int nHandle);
	virtual CPoint GetHandle(int nHandle);
	virtual int GetHandleCount();
	virtual void Serialize(CArchive& ar);
	virtual CGeoDrawObj* Clone(CDrawLayer *pLayer);
	virtual BOOL OnEditProperties(CGeoMapView* pView = NULL);
	// 移动图素大小
	virtual void MoveHandleTo(int nHandle, CPoint point, CGeoMapView* pView = NULL);
	// 移动图素位置
	virtual void MoveTo(const CRect& position, CGeoMapView* pView = NULL);
	// 画图素
	virtual void Draw(CDC* pDC, CGeoMapView* pView = NULL);
	// 画编辑状态
	virtual void DrawEditMode(CGeoMapView* pView);
	// 画移动状态
	virtual void DrawMoveState(CGeoMapView* pView);
	// 画改变大小状态
	virtual void DrawSizeState(CGeoMapView* pView);
	
	// 当比例尺发生改变时
	virtual void OnScaleChanged();
	// 当发生旋转时
	virtual void OnAngleChanged();
	// 当发生坐标原点有改变时
	virtual void OnCoordOrigChanged();
	// 转换参数改变时
	virtual void OnConvertParamChanged();
	// 逆序点集
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
	// 是否点中图素边
	virtual BOOL IsHitOnBorder(CPoint point, CGeoMapView *pView);

	virtual BOOL RecalcBounds(CGeoMapView* pView = NULL);

	// 交换点的次序
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
