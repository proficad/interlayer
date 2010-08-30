#if !defined(AFX_DISPLOGCURVEVIEW_H__F454A24E_D088_49EA_8178_2AFCC8578825__INCLUDED_)
#define AFX_DISPLOGCURVEVIEW_H__F454A24E_D088_49EA_8178_2AFCC8578825__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DispLogCurveView.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CDispLogCurveView view

#define MAX_TIP_COUNT 4
#define WM_CREATE_FILE_LOG WM_USER+103

class CDispLogCurveDoc;

class CDispLogCurveView : public CScrollView
{
protected:
	CDispLogCurveView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDispLogCurveView)

// Attributes
public:

	BOOL m_bLMouseDown;

	CPoint m_StartPoint;
	CPoint m_EndPoint;

	double m_dFirstDepth;
	double m_dEndDepth;
	
	int tipCount;
	CToolTipCtrl tooltip;
	float m_fZoomScale;			// 缩放比例
// Operations
public:

	//////////////////////////////////////////////////////////////////////////
	// Drag
	CImageList* CreateDragImage(BOOL bLane, int iLaneIndex, int iIndex);
	CImageList *m_pDragImageList;
	void OnBeginDrag(BOOL bLane, int iLaneIndex, int iIndex, CPoint point);
	//////////////////////////////////////////////////////////////////////////
	
	BOOL VertScrollBy(CSize sizeScroll, BOOL bDoScroll);
	BOOL VertScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll);

	void ApplyTemplate(LPCTSTR lpszTempl, CDispLogCurveDoc* pDoc); // 应用曲线模板
	//
	// 是否为716格式
	// 是否按实际曲线条数保存的
	// 判断文件是否为文本文件
	//
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDispLogCurveView)
	public:
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL DoMouseWheel(UINT fFlags, short zDelta, CPoint point);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	afx_msg void OnContextMenu(CWnd*, CPoint point);
	virtual ~CDispLogCurveView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CDispLogCurveView)
	afx_msg void OnViewScale();
	afx_msg void OnExportDisplogcurve();
	afx_msg void OnUpdateExportDisplogcurve(CCmdUI* pCmdUI);
	afx_msg void OnZoomIn();
	afx_msg void OnUpdateZoomIn(CCmdUI* pCmdUI);
	afx_msg void OnZoom11();
	afx_msg void OnUpdateZoom11(CCmdUI* pCmdUI);
	afx_msg void OnZoomOut();
	afx_msg void OnUpdateZoomOut(CCmdUI* pCmdUI);
	afx_msg void OnDepthSegment();
	afx_msg void OnUpdateDepthSegment(CCmdUI* pCmdUI);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnViewDispcaption();
	afx_msg void OnUpdateViewDispcaption(CCmdUI* pCmdUI);
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnViewLaneProp();
	afx_msg void OnInsertLane();
	afx_msg void OnAppendLane();
	afx_msg void OnAddCurve();
	afx_msg void OnUpdateInsertLane(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAppendLane(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAddCurve(CCmdUI* pCmdUI);

	afx_msg void OnDelCurve();
	afx_msg void OnUpdateDelCurve(CCmdUI* pCmdUI);

	afx_msg void OnDelLane();
	afx_msg void OnUpdateDelLane(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnImportDat();
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateImportDat(CCmdUI* pCmdUI);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDispTempl();
	afx_msg void OnUpdateDispTempl(CCmdUI* pCmdUI);
	afx_msg void OnEditDispTempl();
	afx_msg void OnUpdateEditDispTempl(CCmdUI* pCmdUI);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnEditCutInterLayer();
	afx_msg void OnUpdateEditCutInterLayer(CCmdUI* pCmdUI);
	afx_msg void OnEditCopyInterLayer();
	afx_msg void OnUpdateEditCopyInterLayer(CCmdUI* pCmdUI);
	afx_msg void OnEditPasteInterLayer();
	afx_msg void OnUpdateEditPasteInterLayer(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg UINT OnGetDlgCode();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISPLOGCURVEVIEW_H__F454A24E_D088_49EA_8178_2AFCC8578825__INCLUDED_)
