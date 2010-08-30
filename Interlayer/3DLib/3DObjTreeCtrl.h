#pragma once
#include "3DModelDoc.h"

// C3DObjTreeCtrl

class C3DObjTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(C3DObjTreeCtrl)

public:
	C3DObjTreeCtrl();
	virtual ~C3DObjTreeCtrl();
	BOOL SetCheck(HTREEITEM hItem, UINT uCheck);
	int GetCheck(HTREEITEM hItem);
	void SetItemAndChildren(HTREEITEM hItem, UINT uCheck);
	int IsAllChildHide(HTREEITEM hItem);
	int IsAllChildShow(HTREEITEM hItem);
	BOOL IsExistChildNode(HTREEITEM hti, LPCSTR lpszNodeName, BOOL bCase = TRUE);

	C3DModelDoc* m_pDoc;
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	void FillTreeCtrl();

	void OnDeleteItem();
	void AddObj(CGLObject *pObj);

	HTREEITEM GetChildNode(HTREEITEM hti, LPCSTR lpszNodeName, BOOL bCase = TRUE);

	void OnNew();
	void OnUpdateNew(CCmdUI *pCmdUI);

	void OnEditClear();
	void OnUpdateEditClear(CCmdUI *pCmdUI);
	void OnRename();
	void OnUpdateRename(CCmdUI *pCmdUI);
	void OnSetMaterial();
	void OnUpdateSetMaterial(CCmdUI *pCmdUI);
	void OnSetColor();
	void OnUpdateSetColor(CCmdUI *pCmdUI);
protected:
	afx_msg void OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnIdleUpdateCmdUI();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
};


