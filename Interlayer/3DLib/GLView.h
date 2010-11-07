//############################################################################

#if !defined(AFX_GLVIEW_H__859711F8_BE5E_4EFB_9EAB_84708FB5E343__INCLUDED_)
#define AFX_GLVIEW_H__859711F8_BE5E_4EFB_9EAB_84708FB5E343__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GLView.h : header file
//

#include "GLDisplayContext.h"
#include "GLObject.h"
#include "GLTrihedron.h"

#include "Vector3d.h"
#include "colorlookuptable.h"
#include "background.h"
#include "Vertex3D.h"
#include "GLDisplayContext.h"

#define NUM_COLORS (sizeof(m_colorv) / sizeof(m_colorv[0]))
#define FAR_CLIP 200000.0
/////////////////////////////////////////////////////////////////////////////
// CGLView view

//class CGLDisplayContext;
class CGLView
{
public:
	CGLView(CWnd* pWnd, CGLDisplayContext* aContext);
	~CGLView();

	// Attributes
public:
	HGLRC m_hRC;			// Rendering Context 绘制场景
	HDC m_hDC;				// Device Context 设备场景
	CPalette m_GLPalette;	// Logical Palette
	HPALETTE m_hPalette, m_hPalette2;

	CGLTrihedron* m_myTrihedron;	// 坐标轴
	CGLEditAxis*	m_editAxis;				//编辑坐标轴
private:
	// Color Indexes
	struct colorIndexState
	{
		GLfloat amb[3];		/* ambient color / bottom of ramp */
		GLfloat diff[3];	/* diffuse color / middle of ramp */
		GLfloat spec[3];	/* specular color / top of ramp */
		GLfloat ratio;		/* ratio of diffuse to specular in ramp */
		GLint indexes[3];	/* where ramp was placed in palette */
	};

	colorIndexState m_colorv[3];		// 调色板

	CPoint m_DownPoint;
	bool m_ZoomON;						// 放缩
	bool m_PanON;						// 平移
	bool m_RotON;						// 旋转

	//bool m_gridOn;					// 显示网格
	bool m_bSliceZ;						// 横向切片
	double m_dCurrZ;					// 当前Z值

	bool m_bSliceXY;					// 纵向切片
	double m_dCurrX1;					// 当前坐标值
	double m_dCurrY1;
	double m_dCurrX2;
	double m_dCurrY2;
	
	// Operations
public:
	void InitGL();
	//------------------------------------------------------------------------
	CGLDisplayContext* GetContext() { return m_pContext;}
	BOOL EnableRC(HDC m_hDC, HGLRC m_hRC, BOOL bEnable);
	void RenderScene(GLenum mode, bool bSlice = false);		// 渲染场景
	void InitializePalette(void);

	void ZoomView(CPoint, CPoint);		// 放缩
	void PanView(CPoint, CPoint);		// 平移
	void RotateView(CPoint, CPoint);	// 旋转

	//////////////////////////////////////////////////////////////////////////
	double GetVertical() { return m_pContext->GetVertical(); }
	double GetVerticalOriginal() { return m_pContext->GetVerticalOriginal(); }
	void VerticalStretch() { m_pContext->SetVertical(m_pContext->m_dVertical + 0.1); Refresh(); }
	void VerticalShrink() { m_pContext->SetVertical(m_pContext->m_dVertical - 0.1); Refresh();}
	void VerticalOriginal() { m_pContext->SetVertical(m_pContext->GetVerticalOriginal()); Refresh();}
	//////////////////////////////////////////////////////////////////////////

	void SetSliceZ(bool bFlag){ m_bSliceZ = bFlag; }
	void SetCurrZ(double z) { m_dCurrZ = z; }
	void SetSliceXY(bool bFlag){ m_bSliceXY = bFlag; }
	void SetCurrXY(double x1, double y1, double x2, double y2)
	{ 
		m_dCurrX1 = x1; 
		m_dCurrY1 = y1; 
		m_dCurrX2 = x2; 
		m_dCurrY2 = y2; 
	}

	void GetCurrXY(double &x1, double &y1, double &x2, double &y2)
	{ 
		x1 = m_dCurrX1;
		y1 = m_dCurrY1;
		x2 = m_dCurrX2;
		y2 = m_dCurrY2;
	}

	void StopZooming()
	{
		m_ZoomON = false;
	} // 停止放缩
	void StopPanning()
	{
		m_PanON = false;
	}	// 停止平移
	void StopRotation()
	{
		m_RotON = false;
	}	// 停止旋转

	void TopView();					// 显示顶面
	void BottomView();				// 显示底面
	void FrontView();				// 显示前面
	void BackView();				// 显示背面
	void LeftView();				// 显示左面
	void RightView();				// 显示右面
	void AxonView();				// 轴侧投影

	void FitAll();					// 全部适合
	void ZoomWindow(CRect zcRect);	// 窗口缩放
	void ResetView();				// 重新设定

	void SetSmooth(bool bSmooth);
	bool IsSmooth()
	{
		return m_pContext->m_bSmooth;
	}
	void SetHLRemoved(bool b){m_pContext->SetHLRemoved(b);}; // 消隐线
	bool IsHLRemoved(){return m_pContext->m_bHLRemoved;}

	void PointToScreen(const CVertex3D& P, int& x, int& y);
	double GetSliceHeight(const int& height, bool bVert = true);	// 获取切片的厚度 height 像素点数
	void ScreenToPoint(const int& x, const int& y, CVertex3D& P);
	void FitView();
	void Fit3d(CRect zcRect);

	void ShowLegend(bool bLegend);
	bool IsShowLegend();
	void ShowAxis(bool bShow);
	bool IsShowAxis();
	void ShowGird(bool bShow);
	bool IsShowGird();

	void SetSmoothGround(bool bSmooth);
	bool GetSmoothGround();

	void SetBackgroundColor(GLfloat, GLfloat, GLfloat);
	void GetBackgroundColor(GLfloat&, GLfloat&, GLfloat&);

	void SetDisplayMode(const GLDisplayMode& aMode);
	bool IsShaded() { return m_pContext->GetDisplayMode()==GLSHADED; }

	void SetAntiAliasing(const bool& mode);

	void Select(const int& x, const int& y);		// 单选
	void MultiSelect(const int& x, const int& y);	// 多选
	void SweepSelect(const CRect& swRect);			// 扫视区域选择

	int HitSliceXY(const int& x, const int& y, const int& sensitivity = 20);
	int HitPoint(const int& x, const int& y, const int& sensitivity = 5);
	CGLObject* ProcessSelection(const int& x, const int& y, const int& sensitivity = 5);
	int ProcessSweepSelection(const CRect& rect, unsigned long int* id);
	int ProcessEditAxisSelection(const int& x, const int& y, const int& sensitivity = 5);

	virtual void Draw();			// 绘制
	void ReSize(int cx, int cy);	// 改变图形大小
	void Refresh();					// 刷新

	BOOL QueryNewPalette();			// 查询调色板
	void PaletteChanged();			// 改变调色板

	CWnd *GetWnd(){ return m_ptrWnd;}//窗口

	void ExportFile(LPCTSTR lpszFileName);
	

private:
	CWnd* m_ptrWnd;						// 窗口句柄
	CGLDisplayContext* m_pContext;		// 显示场景	
public:
	CBackground m_Background;
};


#endif // !defined(AFX_GLVIEW_H__859711F8_BE5E_4EFB_9EAB_84708FB5E343__INCLUDED_)
