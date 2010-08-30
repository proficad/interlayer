// ToolBase.h: interface for the CToolBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLBASE_H__7B8D4E16_F0D9_447F_B725_6AB2B2D247B8__INCLUDED_)
#define AFX_TOOLBASE_H__7B8D4E16_F0D9_447F_B725_6AB2B2D247B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGeoMapView;

class CToolBase  
{
public:
	enum DrawShape
	{
		selection,				// 选择对象工具
			
			rectempty,			// 空心矩形工具
			roundrectempty,		// 空心圆角矩形工具
			ellipseempty,		// 空心椭圆工具
			rect,				// 矩形工具
			roundrect,			// 圆角矩形工具
			ellipse,			// 椭圆工具
			solidrect,			// 矩形工具
			solidroundrect,		// 圆角矩形工具
			solidellipse,		// 椭圆工具

			line,				// 直线
			polyline,			// 不封闭多边形
			pline,				// 光滑不封闭多边形
			polygonempty,		// 空心封闭多边形	
			plinepolygonempty,	// 空心光滑封闭多边形
			polygon,			// 封闭多边形	
			plinepolygon,		// 光滑封闭多边形
			poly,				// 非光滑区域
			smoothfill,			// 光滑区域

			movePoint,			// 移动节点
			delPoint,			// 删除节点
			addPoint,			// 添加世点
			dividePoly,			// 分割线段
			joinPoly,			// 拚接线段

			region,				// 组合区域
			newFill,			// 创建新的区域
			PaintBox,			// 油漆桶

			clippingRgn,		// 设置剪切区域

			horn_lable,			// 矩形标注
			line_lable,			// 线性标注
			text,				// 文本

			pictrue,			// 图片

			well,				// 建立新井
			moveWellpoleFD,		// 移动井柱 -- 栅状图
			linkWellpoleFD,		// 设置连接 -- 栅状图
			scaleRuler,			// 比例尺

			normal,				// 法线
			interlayer,			// 夹层线

			pageMove,			// 移动整页
			layerMove,			// 移动整个层

			attriBrush			// 格式刷
	};
	
	enum SelectMode				// 选择模式
	{
		none,					// 无
			netSelect,			// 网选对象
			moveObj,			// 移动对象
			sizeObj,			// 改变对象尺寸大小
			editObj, 			// 编辑对象
	};
public:
	CToolBase(DrawShape nDrawShape);
	
	// Overridables
	virtual void OnKeyDown(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnKeyUp(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnChar(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnContextMenu(CGeoMapView* pView, CWnd* pWnd, CPoint point);
	virtual void OnRButtonDown(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonDown(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonDblClk(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonUp(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnMouseMove(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnEditProperties(CGeoMapView* pView);
	virtual void OnCancel(CGeoMapView* pView);

public:	
	// Attributes
	DrawShape m_drawShape;
	
	static CToolBase* FindTool(DrawShape drawShape);
	static CPtrList c_tools;

	static CPoint c_down;
	static UINT c_nDownFlags;
	static CPoint c_last;
	static DrawShape c_drawShape;
};

#endif // !defined(AFX_TOOLBASE_H__7B8D4E16_F0D9_447F_B725_6AB2B2D247B8__INCLUDED_)
