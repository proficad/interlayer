#pragma once

#include "ViewTree.h"

class CGraphToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};


// CGraphView

class CGraphView : public CDockablePane
{
	// 构造
public:
	CGraphView();

	void AdjustLayout();
	void OnChangeVisualStyle();

	// 属性
protected:

	CViewTree m_wndGraphView;
	CImageList m_GraphViewImages;
//	CGraphToolBar m_wndToolBar;

public:
	void FillGraphView(LPCTSTR lpszItem = NULL, int nImage = 0, int nSelectedImage = 0, 
		HTREEITEM hParent = TVI_ROOT, 
		HTREEITEM hInsertAfter = TVI_LAST);

	HTREEITEM AddTreeItem(LPCTSTR lpszItem, int nImage = 0, int nSelectedImage = 0, 
		HTREEITEM hParent = TVI_ROOT, 
		HTREEITEM hInsertAfter = TVI_LAST);

	void LoadTree(CArchive& ar);
	void SaveTree(CArchive& ar);
	void EmptyTree();
	CViewTree * GetTreeCtrl() { return &m_wndGraphView; }
	bool OpenTree(CTreeNodeDat *lpNodeDat, void *lpVoid);
	// 实现
public:
	virtual ~CGraphView();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);	
	afx_msg void OnFileOpen();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnExpandAllTree();
	afx_msg void OnCollapseAllEntireTree();
	afx_msg void OnFileNew();
	afx_msg void OnUpdateFileNew(CCmdUI *pCmdUI);
	afx_msg void OnEditRename();
	afx_msg void OnUpdateEditRename(CCmdUI *pCmdUI);
	afx_msg void OnEditClear();
	afx_msg void OnUpdateEditClear(CCmdUI *pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI *pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI *pCmdUI);
};


