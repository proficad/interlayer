#if !defined(AFX_LAYERLISTCTRL_H__A6D8AF3D_2054_4518_9C8E_C463088CCECE__INCLUDED_)
#define AFX_LAYERLISTCTRL_H__A6D8AF3D_2054_4518_9C8E_C463088CCECE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LayerListCtrl.h : header file
//
#include "DrawLayer.h"
class CLayerCell : public CObject
{
public:
	CLayerCell(CString &str, const CString &strID) 
	{
		m_bShow = FALSE;		// 是否显示该层
		m_bLocked = FALSE;		// 是否锁定
		m_bClipping = FALSE;	// 是否按剪切区域显示
		m_strLayerName = str;
		m_strLayerID = strID;
	}
	BOOL m_bShow; //层的属性:显示 锁定 
	BOOL m_bLocked;
	BOOL m_bClipping;
	CString	m_strLayerName;//层的名称 	
	CString m_strLayerID;//层的索引
}; 

/////////////////////////////////////////////////////////////////////////////
// CLayerListCtrl window

class CEditLayer;

class CDrawPage;
//#define CListCtrl CBCGPListCtrl
	
class CLayerListCtrl : public CListCtrl
{
	// Construction
public:
	CLayerListCtrl();
	
	// Attributes
public:
	
	CDrawPage *m_pCurPage;
	CString m_strCurPageID;		// 当前页面ID
	CString m_strCurLayerID;	// 当前图层ID

	int m_iSelectedRow;
	int m_iSelectedCol;
	
	int m_iCurLayerID; // 当前处理编辑的图层
	
	CImageList	m_list;
	void invalidate_grid(int row, int col);
	
	void OnInitialUpdate() ;
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) ;
	void Refresh();
	
	afx_msg LRESULT	OnSetFont(WPARAM wParam, LPARAM lParam);
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	
	// Operations
public:
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLayerListCtrl)
	//}}AFX_VIRTUAL
	
	// Implementation
public:
	BOOL SetLayerName(const int &nLayerID, const CString &strLayerName);
	BOOL SetLayerShowFlag(const int &nLayerID, const BOOL &bFlag);
	BOOL SetLayerLockedFlag(const int &nLayerID, const BOOL &bFlag);
	BOOL SetLayerClipFlag(const int &nLayerID, const BOOL &bFlag);
	BOOL ActiveLayer(const int &nActiveLayerID);
	void EndEdit();
	void DeleteLayerAll();
	void LayerPrev();
	BOOL LayerPrev(CDrawLayer *pLayer);
	void LayerNext();
	BOOL LayerNext(CDrawLayer *pLayer);
	void LayerToBack();
	BOOL LayerToBack(CDrawLayer *pLayer);
	void LayerToFront();
	BOOL LayerToFront(CDrawLayer *pLayer);
	void DelLayer();
	BOOL DelLayer(CDrawLayer *pLayer);
	void AddLayer();
	BOOL AddLayer(CDrawLayer *pLayer);
	void FillLayerList();
	void OnEdit();
	CEditLayer *m_pEdit;
	virtual ~CLayerListCtrl();
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CLayerListCtrl)
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDestroy();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()

protected:
	void OnIdleUpdateCmdUI();
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CEditLayer window

class CEditLayer : public CEdit
{
// Construction
public:
	CEditLayer();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditLayer)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditLayer();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditLayer)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LAYERLISTCTRL_H__A6D8AF3D_2054_4518_9C8E_C463088CCECE__INCLUDED_)
