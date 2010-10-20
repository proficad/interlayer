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

	std::vector<CString> m_PhyParaName;
	std::vector<CString> m_PhyParaNamefilename;
	HTREEITEM AddTreeItem(LPCTSTR lpszItem, int nImage = 0, int nSelectedImage = 0, 
		HTREEITEM hParent = TVI_ROOT, 
		HTREEITEM hInsertAfter = TVI_LAST);
protected:
	void ReadEclipseGrid(LPCTSTR filename);
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
	afx_msg void OnAddPhyPara();
	afx_msg void OnUpdateAddPhyPara(CCmdUI *pCmdUI);
	afx_msg void OnCalcPhyPara();
	afx_msg void OnUpdateCalcPhyPara(CCmdUI *pCmdUI);
	afx_msg void OnAdjPhyPara();
	afx_msg void OnUpdateAdjPhyPara(CCmdUI *pCmdUI);
	DECLARE_MESSAGE_MAP()
};

class CLayerIntersectSearch : public CDockablePane
{
	DECLARE_DYNAMIC(CLayerIntersectSearch)
public:
	CLayerIntersectSearch();
	virtual ~CLayerIntersectSearch();
	void AdjustLayout();
	void OnChangeVisualStyle();	

	void SetGrid(const std::string& gridname);
	CIntersectSearchTree m_wndTree;
	CImageList m_Images;
	CImageList m_TreeStateImages;
	CButton	  m_wndButton;

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

	void OnClickButton();
	void OnUpdateClickMyButton(CCmdUI* pCmdUI); 
	DECLARE_MESSAGE_MAP()
};
