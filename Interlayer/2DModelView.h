#pragma once
#include <afxtempl.h>
#include "GridModelDoc.h"
#include "./VFC/BigView.h"
#include "FieldCell.h"
#include "./VFC/InverseDist.h"

// C2DModelView 视图

#define ZOOM_ONE 1000

class C2DModelDoc;

class C2DModelView : public CBigScrollView
{
protected:
	C2DModelView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(C2DModelView)

	enum Operation
	{
		opNone,
		opMoveView,		// 移动视窗
		opZoomRect,		// 区域放大
		opSelectCell,	// 选择网格
	};
	// Attributes
public:

	// Operations
public:

	BOOL m_bDispTop;
	CSize m_sizeDoc;	// document size
	long  m_x0, m_y0;	// document origin

	void  OnZoom(long Zoom);
	long  m_Zoom, m_ZoomMin, m_ZoomMax;

	long  m_Scale;		// zoom for drawing function
	long  m_xo, m_yo;	// origine for drawing function

	Operation m_operation;	// 操作
	CPoint m_ptDown;
	CPoint m_ptLast;

	CPoint m_ptCurPosition;

	CClipRegion m_clipReg;		// clipping region

	CFieldCell* HitCell(const CPoint &point);
	void UpdateDisplayInfo() const;
	void UpdateScrollPosition(CPoint point);
	HCURSOR m_hCursor;

	CObList m_selection; // 目前选择的网格
	void InvalCell(CFieldCell *pCell);
	BOOL IsSelected(CObject* pCell) const;
	void DeSelect(CFieldCell *pCell);
	void SelectWithinRect(CRect rect, BOOL bAdd = FALSE);
	void Select(CFieldCell *pCell, BOOL bAdd = FALSE);

	LOGFONT	m_lgFont;
	BOOL	m_bDrawFill;
	BOOL	m_bGradientFill;
	BOOL	m_bDrawBorder;
	BOOL	m_bDrawText;

	CFieldCell *m_pCurCell;

	//////////////////////////////////////////////////////////////////////////
	void DocToClient(CRect& rect);
	void DocToClient(CPoint& point);
	void ClientToDoc(CPoint& point);
	void DocToClient(CSize& size);
	void ClientToDoc(CSize& size);
	void ClientToDoc(CRect& rect);

	C2DModelDoc *GetDocument();
	void Init();
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(C2DModelView)
public:
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

	// Implementation
protected:
	virtual ~C2DModelView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(C2DModelView)
	afx_msg void OnViewZoomIn();
	afx_msg void OnViewZoomOut();
	afx_msg void OnViewZoomDefault();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnViewSetScaleToFit();
	afx_msg void OnViewResizeParentToFit();
	afx_msg void OnViewUpperLeft();	
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnTrackMode();
	afx_msg void OnUpdateTrackMode(CCmdUI* pCmdUI);
	afx_msg void OnZoomArea();
	afx_msg void OnUpdateZoomArea(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);	
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);	
	afx_msg void OnFilePrintPreview();
	afx_msg void OnDispFill();
	afx_msg void OnUpdateDispFill(CCmdUI* pCmdUI);
	afx_msg void OnDispGradfill();
	afx_msg void OnUpdateDispGradfill(CCmdUI* pCmdUI);
	afx_msg void OnDispGrid();
	afx_msg void OnUpdateDispGrid(CCmdUI* pCmdUI);
	afx_msg void OnDispValues();
	afx_msg void OnUpdateDispValues(CCmdUI* pCmdUI);
	afx_msg void OnDrawSelect();
	afx_msg void OnUpdateDrawSelect(CCmdUI* pCmdUI);
	afx_msg void OnEditCell();
	afx_msg void OnUpdateEditCell(CCmdUI* pCmdUI);
	afx_msg void OnFuntion();
	afx_msg void OnUpdateFuntion(CCmdUI* pCmdUI);
	afx_msg void OnDispTop();
	afx_msg void OnUpdateDispTop(CCmdUI* pCmdUI);
	afx_msg void OnDispBottom();
	afx_msg void OnUpdateDispBottom(CCmdUI* pCmdUI);
	afx_msg void OnExportGraph();
	afx_msg void OnExportDat();
	afx_msg void OnPropSet();
	afx_msg void OnUpLayer();
	afx_msg void OnUpdateUpLayer(CCmdUI* pCmdUI);
	afx_msg void OnDownLayer();
	afx_msg void OnUpdateDownLayer(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
public:
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
};

namespace _2DView
{
	class CHandTracker :public CRectTracker 
	{
	public :
		C2DModelView * m_pView;
		CHandTracker(C2DModelView * pView);
		virtual void DrawTrackerRect(LPCRECT lpRect, CWnd * pWndClipTo, CDC * pDC, CWnd * pWnd);
		virtual void OnChangedRect(const CRect & rectOld);
	};
}

