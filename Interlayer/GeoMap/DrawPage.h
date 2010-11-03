#pragma once

#include "DrawLayer.h"

class CDrawPage : public CRefObject  
{
public:
	CDrawPage(CGeoMapDoc* pDocument);
	virtual ~CDrawPage();
	CDrawPage();

	//属性
public:
	// 保存
	// 板面排列 x * y 页
	int					m_nHorzPages;			// 横向排页面总数 
	int					m_nVertPages;			// 纵向排页面总数

	BOOL				m_bLocked;				// 锁定
	CString				m_strPageName;			// 页面名称
	CString				m_strMark;				// 备注说明
	CString				m_strID;				// 页面ID标识;
	BOOL				m_bPagePanel;			// 是否显示版面


	// 坐标原点坐标 x南北 y东西 m_dValue与正北方向夹角--顺时针为正(度)
	CPoint2D			m_ptCoordOrig;
	double				m_lScale;				// 大地坐标比例尺
	double				m_ground2screen[6];
	double				m_screen2ground[6];
	BOOL				m_bParameter;			// 是否已经赋予平差四参数
	//------------------------------------------------------------------------
	// 平差四参数
	//
	// K为尺度因子，ａ为旋转角度，Lx\Ly相对应的平移参数
	//
	// X=Lx + X1*Kcosａ- Y1*Ksinａ
	// Y=LY + Y1*Kcosａ+ X1*Ksinａ
	//
	// 转化为
	//
	// X=a+X1*c-Y1*d
	// Y=b+Y1*c+X1*d
	//
	// 其中
	// a = Lx, b = Ly, c = Kcosａ, d = Ksinａ;
	//
	// K = 1/m_lScale
	// ａ= m_ptCoordOrig.m_dValue*PI/180.0;
	// Lx = m_ptCoordOrig.x
	// Ly = m_ptCoordOrig.y
	//	

	int 				m_lZoom;				// 屏幕放缩因子
	CPoint				m_ptPosition;			// 当前屏幕滚动位置			

	CDrawLayerList		m_DrawLayers;			// 图层列表	
	CString				m_strCurLayerID;		// 当前图层ID
	int					m_iMarginWidth;			// 分幅排印时重合宽度

	//------------------------------------------------------------------------
	// 不保存
	CDrawLayer *m_pCurDrawLayer;				// 当前图层
	CGeoMapDoc *m_pDocument;					// 当前文档	

public:
	// 获取与正北方向夹角--顺时针为正(度)
	double GetAngle() { return m_ptCoordOrig.m_dValue; }

	// 设置与正北方向夹角--顺时针为正(度)
	void SetAngle(double dAngle) { m_ptCoordOrig.m_dValue = dAngle; }

private:

	//操作
public:	
	// 按索引获取图层指针
	CDrawLayer* GetLayer(const int& iIndex);
	CDrawLayer* GetLayer(const CString &strID);

	// 获取图层ID
	CString GetLayerID(const int& iIndex);
	CString GetLayerID(const CDrawLayer* pLayer);

	// 获取该页面中所有对象所占区域大小
	CSize GetPageSize();
	// 获取该页央中所有对象所占区域范围
	CRect GetPageRect();

	int GetLayerCount() { return m_DrawLayers.GetCount();} // 图层总数
	void SetCurLayer(CDrawLayer *pLayer);			// 设置当前图层	
	CDrawLayer *GetCurLayer();						// 获取当前图层

	BOOL Add(CDrawLayer *pObj);						// 增加图层
	BOOL Remove(CDrawLayer *pObj);					// 删除图层
	void RemoveAll();								// 删除所有图层

	void Draw(CDC *pDC, CGeoMapView *pView);		// 显示图层
	virtual void Serialize(CArchive& ar);			// 串行化对象

	// 图层排序
	BOOL MoveBack(CDrawLayer *pLayer);
	BOOL MoveForward(CDrawLayer *pLayer);
	BOOL MoveToBack(CDrawLayer *pLayer);
	BOOL MoveToFront(CDrawLayer *pLayer);

	// 
	void OnEditProperties(CGeoMapView* pView = NULL);
	void OnScaleChanged();

	// 如果是大地坐标系，则转换成大地坐标(geodetic coordinates)
	// 如果是数据坐标系，则为图形中心坐标
	CPoint2D CoordinateTransfer(const CPoint &pt);
	CPoint CoordinateTransfer(const CPoint2D &pt);
	CRectEx CoordinateTransfer(const CRect &rect);
	CRect CoordinateTransfer(const CRectEx &rect);

	CPoint2D CoordinateTransferNew(const CPoint &pt);
	CPoint CoordinateTransferNew(const CPoint2D &pt);
	CRectEx CoordinateTransferNew(const CRect &rect);
	CRect CoordinateTransferNew(const CRectEx &rect);

	// 认领孩子
	void ClaimChildren();

protected:
	DECLARE_SERIAL(CDrawPage);
private:

};

