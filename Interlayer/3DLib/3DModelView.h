#pragma once
#include "GLView.h"
#include "../clientcapture.h"
class C3DModelDoc;

// C3DModelView 视图

class C3DModelView : public CView
{
	DECLARE_DYNCREATE(C3DModelView)

protected:
	C3DModelView();           // 动态创建所使用的受保护的构造函数
	virtual ~C3DModelView();

public:
	virtual void OnDraw(CDC* pDC);      // 重写以绘制该视图
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	C3DModelDoc* GetDocument();
	enum LineStyle { Solid, Dot, ShortDash, LongDash };
	CPen*  m_Pen;
	virtual void DrawRectangle (const bool  Draw =true,
		const LineStyle aLineStyle = Solid  );
	
	enum OPERATE
	{
		NONE,
		SELECT,
		MOVE,
		ROTATE,
		ZOOM,
		WINZOOM,	// 选择放大

		SLICE_Z,	// 横切片
		SLICE_XY,	// 纵切片
		SLICE_X,	// 纵切片
		SLICE_Y,	// 纵切片
		DICE,		// 切块
	};
	CGLView* m_myView;
private:	

	bool m_lbutdown; // 左键
	bool m_mbutdown; // 中键
	bool m_rbutdown; // 右键
	bool m_bPanning;

	CPoint m_lDownPnt;
	CPoint m_mDownPnt;
	CPoint m_rDownPnt;

	bool m_antialiased;
	
	int     m_myXmin;
	int     m_myYmin;  
	int     m_myXmax;
	int     m_myYmax;

	CString coord;

	OPERATE m_eOperate;		// 操作

	int m_posSliceXY;		// 鼠标点击的位置 1,0,2,-1
	unsigned char *SnapClient(CSize *pSize);
protected:
	DECLARE_MESSAGE_MAP()
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
public:
	virtual void OnInitialUpdate();
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	
	afx_msg void OnSmooth();
	afx_msg void OnUpdateSmooth(CCmdUI *pCmdUI);

	afx_msg void OnTopview();
	afx_msg void OnBottomview();
	afx_msg void OnFrontview();
	afx_msg void OnRearview();
	afx_msg void OnLeftview();
	afx_msg void OnRightview();
	afx_msg void OnAxonview();
	afx_msg void OnFitall();
	
	afx_msg void OnReset();
	afx_msg void OnDisplaymode();
	afx_msg void OnUpdateDisplaymode(CCmdUI *pCmdUI);
	afx_msg void OnHlremoved();
	afx_msg void OnUpdateHlremoved(CCmdUI *pCmdUI);
	afx_msg void OnAntial();
	afx_msg void OnUpdateCoordViewer(CCmdUI* pCmdUI); //Coordinates in Status Bar
	
	afx_msg void OnScreenBackgroundcolor();	
	
	afx_msg void OnVerticalStretch();
	afx_msg void OnUpdateVerticalStretch(CCmdUI *pCmdUI);
	afx_msg void OnVerticalOriginal();
	afx_msg void OnUpdateVerticalOriginal(CCmdUI *pCmdUI);
	afx_msg void OnVerticalShrink();
	afx_msg void OnUpdateVerticalShrink(CCmdUI *pCmdUI);

	afx_msg void OnZoomwin();
	afx_msg void OnUpdateZoomwin(CCmdUI *pCmdUI);

	afx_msg void OnSelectObj();
	afx_msg void OnUpdateSelectObj(CCmdUI *pCmdUI);

	afx_msg void OnEditPoint();
	afx_msg void OnUpdateEditPoint(CCmdUI *pCmdUI);

	afx_msg void OnMoveEye();
	afx_msg void OnUpdateMoveEye(CCmdUI *pCmdUI);

	afx_msg void OnRotateEye();
	afx_msg void OnUpdateRotateEye(CCmdUI *pCmdUI);

	afx_msg void OnZoomEye();
	afx_msg void OnUpdateZoomEye(CCmdUI *pCmdUI);

	afx_msg void OnSliceZ();
	afx_msg void OnUpdateSliceZ(CCmdUI *pCmdUI);

	afx_msg void OnSliceX();
	afx_msg void OnUpdateSliceX(CCmdUI *pCmdUI);

	afx_msg void OnSliceY();
	afx_msg void OnUpdateSliceY(CCmdUI *pCmdUI);

	afx_msg void OnSliceXY();
	afx_msg void OnUpdateSliceXY(CCmdUI *pCmdUI);

	afx_msg void OnShowLegend();
	afx_msg void OnUpdateShowLegend(CCmdUI *pCmdUI);

	afx_msg void OnSetLegendColor();	
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSmoothGround();
	afx_msg void OnUpdateSmoothGround(CCmdUI *pCmdUI);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnExport();
	afx_msg void OnUpdateExport(CCmdUI *pCmdUI);
	afx_msg void OnFilePrintPreview();
	CClientCapture m_ClientCapture;
};


