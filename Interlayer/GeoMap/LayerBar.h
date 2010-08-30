#if !defined(AFX_LAYERBAR_H__921A53FA_F8B0_42A3_A977_9EFC25F8F131__INCLUDED_)
#define AFX_LAYERBAR_H__921A53FA_F8B0_42A3_A977_9EFC25F8F131__INCLUDED_

#include "LayerListCtrl.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LayerBar.h : header file
//

class CLayerToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)	
	{		
		CMFCToolBar::OnUpdateCmdUI ((CFrameWnd*) GetOwner (), bDisableIfNoHndler);
	}
	virtual BOOL AllowShowOnList () const		{	return FALSE;	}
};


class CDrawPage;
class CGeoMapView;


/////////////////////////////////////////////////////////////////////////////
// CLayerBar window

class CLayerBar : public CDockablePane
{
// Construction
public:
	CLayerBar();
	virtual ~CLayerBar();

	void AdjustLayout();
	void OnChangeVisualStyle();

// Attributes
public:
	
// Operations
public:

	CFont m_Font;
	CLayerToolBar	m_wndToolBar;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLayerBar)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	CLayerListCtrl m_wndListCtrl;

	void RefreshListCtrl(){m_wndListCtrl.Refresh();}
	// Generated message map functions
protected:
	//{{AFX_MSG(CLayerBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLayerMovetoback();
	afx_msg void OnLayerMoveback();
	afx_msg void OnUpdateLayerMoveforward(CCmdUI* pCmdUI);
	afx_msg void OnLayerMovetofront();
	afx_msg void OnLayerMoveforward();
	afx_msg void OnDestroy();
	afx_msg void OnDelLayer();
	afx_msg void OnAddLayer();
	afx_msg void OnUpdateLayerMovetoback(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLayerMoveback(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLayerMovetofront(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDelLayer(CCmdUI* pCmdUI);
	afx_msg void OnLayerShow();
	afx_msg void OnLayerHide();
	afx_msg void OnLayerLock();
	afx_msg void OnLayerUnlock();
	afx_msg void OnLayerClip();
	afx_msg void OnLayerNoclip();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LAYERBAR_H__921A53FA_F8B0_42A3_A977_9EFC25F8F131__INCLUDED_)
