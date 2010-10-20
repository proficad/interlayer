
#pragma once

#include "ViewTree.h"

class CModelToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CModelView : public CDockablePane
{
public:
	CModelView();
	virtual ~CModelView();

	void AdjustLayout();
	void OnChangeVisualStyle();

protected:
	CModelToolBar m_wndToolBar;
	CViewTree m_wndModelView;
	CImageList m_ModelViewImages;
	CImageList m_TreeStateImages;
	UINT m_nCurrSort;

	CStringArray m_arParamName;
	CStringArray m_arFileName;

public:
	void FillModelView(LPCTSTR lpszItem = NULL, int nImage = 0, int nSelectedImage = 0, 
		HTREEITEM hParent = TVI_ROOT, 
		HTREEITEM hInsertAfter = TVI_LAST);

	HTREEITEM AddTreeItem(LPCTSTR lpszItem, int nImage = 0, int nSelectedImage = 0, 
		HTREEITEM hParent = TVI_ROOT, 
		HTREEITEM hInsertAfter = TVI_LAST);

	void LoadTree(CArchive& ar);
	void SaveTree(CArchive& ar);
	void EmptyTree();

	CViewTree * GetTreeCtrl() { return &m_wndModelView; }
	bool OpenTree(CTreeNodeDat *lpNodeDat, void *lpVoid);

	bool ReadPBRSModel(LPCSTR lpszFileName, LPCSTR lpszModelFileName);
	bool ReadEclipseModel(LPCSTR lpszFileName, LPCSTR lpszModelFileName);

	bool ReadGridPBRS(LPCSTR lpszGridFileName, LPCSTR lpszModelFileName, HTREEITEM hItem);
	bool ReadGridEclipse(LPCSTR lpszGridFileName, LPCSTR lpszModelFileName, HTREEITEM hItem);

	bool OnImportInterlayer( LPCSTR filename, LPCSTR name );
// ÖØÐ´
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnNewFolder();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnChangeActiveTab(WPARAM, LPARAM);
	afx_msg void OnFileOpen();
	afx_msg void OnUpdateFileOpen(CCmdUI *pCmdUI);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnAddChild();
	afx_msg void OnUpdateAddChild(CCmdUI *pCmdUI);
	afx_msg void OnEditRename();
	afx_msg void OnUpdateEditRename(CCmdUI *pCmdUI);
	afx_msg void OnImportModel();
	afx_msg void OnUpdateImportModel(CCmdUI *pCmdUI);

	afx_msg void OnSearchModel();
	afx_msg void OnUpdateSearchModel(CCmdUI *pCmdUI);

	afx_msg void OnImportField();
	afx_msg void OnUpdateImportField(CCmdUI *pCmdUI);
	afx_msg void OnExpandAllTree();
	afx_msg void OnCollapseAllEntireTree();
	afx_msg void OnExportModel();
	afx_msg void OnUpdateExportModel(CCmdUI *pCmdUI);
	afx_msg void OnEditClear();
	afx_msg void OnUpdateEditClear(CCmdUI *pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI *pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI *pCmdUI);
};

