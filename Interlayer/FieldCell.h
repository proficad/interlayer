#pragma once
#include "./3Dlib/Vertex3D.h"

class C2DModelView;

// CFieldCell 命令目标

class CFieldCell : public CObject
{
	DECLARE_SERIAL(CFieldCell)

	CFieldCell();           // protected constructor used by dynamic creation

	virtual ~CFieldCell();
	// Attributes
public:	
	CVertex3D m_cornerPoint[8];	// 角点实际坐标
	double	 m_dValue;			// 网格参数数据
	int      m_i, m_j, m_k;		// 当前网格索引
	int		 m_nX, m_nY, m_nZ;	// 网格最大索引数

	BOOL	m_bDispTop;			// 显示顶面

	// 不保存 重新计算
	CPoint m_point[8];			// 角点屏幕坐标
	CVertex3D m_centerPoint[2];	// 网格中心坐标()四边形重心坐标
	COLORREF m_clrCenter;		// 中心点颜色
	COLORREF m_clrCorner[8];	// 角点颜色

	// Operations
public:
	CRect GetRect ();
	CVertex3D PointOfIntersection(); // 四边形对角线交点
	void GradientFill(CDC *pDC, C2DModelView *pView);
	void DrawFill(CDC *pDC, C2DModelView *pView);
	void DrawBorder(CDC *pDC, C2DModelView *pView);
	void DrawText(CDC *pDC, C2DModelView *pView);
	void Draw(CDC *pDC,  C2DModelView *pView);
	void DrawStateSelected(CDC *pDC, C2DModelView *pView);
	void DrawCurCell(CDC *pDC, C2DModelView *pView);

	virtual void Serialize(CArchive& ar);
	BOOL Intersects(CRect &rect);
	BOOL PtInRegion(const CPoint &point);

	CPoint2D intersect(CPoint2D p1[2], CPoint2D p2[2]);
};


