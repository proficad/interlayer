#if !defined(AFX_UNDOBAR_H__F88FEBBF_A4D0_4BEC_80DE_0CC1D1C8B073__INCLUDED_)
#define AFX_UNDOBAR_H__F88FEBBF_A4D0_4BEC_80DE_0CC1D1C8B073__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UndoBar.h : header file
//

#include "UndoListBox.h"

class CUndoButton;

/////////////////////////////////////////////////////////////////////////////
// CUndoBar window

class CUndoBar : public CMFCPopupMenuBar
{
	DECLARE_SERIAL(CUndoBar)

// Construction
public:
	CUndoBar();

	BOOL			m_bUnDo;
	
// Attributes
protected:
	CUndoListBox	m_wndList;
	CRect			m_rectLabel;
	int				m_nLabelHeight;
	CString			m_strLabel;
	

// Operations
public:
	void DoUndo ();
	void SetLabel (const CString& strLabel);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUndoBar)
	//}}AFX_VIRTUAL

	virtual void OnFillBackground (CDC* pDC);
	virtual CSize CalcSize (BOOL bVertDock);

// Implementation
public:
	virtual ~CUndoBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CUndoBar)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CUndoButton* GetParentButton () const;
};

//////////////////////////////////////////////////////////////////////////////
// CUndoMenu

class CUndoMenu : public CMFCPopupMenu
{
	DECLARE_SERIAL(CUndoMenu)

	virtual CMFCPopupMenuBar* GetMenuBar ()
	{
		return &m_wndUndoBar;
	}

	CUndoBar	m_wndUndoBar;

    // Generated message map functions
protected:
    //{{AFX_MSG(CUndoMenu)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////
// CUndoButton

class CUndoButton : public CMFCToolBarMenuButton
{
	friend class CUndoBar;

	DECLARE_SERIAL(CUndoButton)

public:
	CUndoButton()
	{
		
	}

	CUndoButton(UINT uiCmdID, LPCTSTR lpszText) :
		CMFCToolBarMenuButton (uiCmdID, NULL,
			afxCommandManager->GetCmdImage (uiCmdID, FALSE), lpszText)
	{
		m_nSelNum = 0;
	}

	CStringList			m_lstActions;
	int			        m_nSelNum;
	BOOL				m_bUnDo;

	int GetSelNum ()
	{
		return m_nSelNum;
	}

	void ResetSelNum ()
	{
		m_nSelNum = 0;
	}

protected:
	virtual CMFCPopupMenu* CreatePopupMenu ();

	virtual BOOL IsEmptyMenuAllowed () const
	{
		return TRUE;
	}

	virtual void OnChangeParentWnd (CWnd* pWndParent)
	{
		CMFCToolBarMenuButton::OnChangeParentWnd (pWndParent);
		m_bDrawDownArrow = TRUE;
	}

	virtual BOOL IsExclusive () const
	{
		return TRUE;
	}
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UNDOBAR_H__F88FEBBF_A4D0_4BEC_80DE_0CC1D1C8B073__INCLUDED_)
