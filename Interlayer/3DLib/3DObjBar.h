#pragma once
#include "3DObjTreeCtrl.h"

// C3DObjBar

class C3DObjBar : public CDockablePane
{
	DECLARE_DYNAMIC(C3DObjBar)

public:
	C3DObjBar();
	virtual ~C3DObjBar();
	void AdjustLayout();
	void OnChangeVisualStyle();	

	C3DObjTreeCtrl m_wndTree;
	CImageList m_Images;
	CImageList m_TreeStateImages;

	HTREEITEM AddTreeItem(LPCTSTR lpszItem, int nImage = 0, int nSelectedImage = 0, 
		HTREEITEM hParent = TVI_ROOT, 
		HTREEITEM hInsertAfter = TVI_LAST);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	afx_msg void OnNew();
	afx_msg void OnUpdateNew(CCmdUI *pCmdUI);

	afx_msg void OnEditRename();
	afx_msg void OnUpdateEditRename(CCmdUI *pCmdUI);
	afx_msg void OnEditClear();
	afx_msg void OnUpdateEditClear(CCmdUI *pCmdUI);
	afx_msg void OnSetMaterial();
	afx_msg void OnUpdateSetMaterial(CCmdUI *pCmdUI);
	afx_msg void OnSetColor();
	afx_msg void OnUpdateSetColor(CCmdUI *pCmdUI);
	
	DECLARE_MESSAGE_MAP()
};


