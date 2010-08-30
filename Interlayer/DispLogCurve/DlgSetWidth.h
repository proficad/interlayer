#if !defined(AFX_DLGSETWIDTH_H__6674CB06_9095_496F_93DB_359B313D60FC__INCLUDED_)
#define AFX_DLGSETWIDTH_H__6674CB06_9095_496F_93DB_359B313D60FC__INCLUDED_

#include "../afc/src/DFVCtrl.h"
#include "SetWidthView.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSetWidth.h : header file
//
#include "Resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgSetWidth dialog

class CDlgSetWidth : public CDialog
{
// Construction
public:
	CString m_strTitleWidth;
	CSetWidthView *m_pView;
	CDFVCtrl m_DFVCtrl;
	CDlgSetWidth(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSetWidth)
	enum { IDD = IDD_DLG_SETWIDTH };
	//}}AFX_DATA

	CListCtrl *pList;
	UINT	m_iFirstRow;
	UINT	m_uTitleRow;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSetWidth)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSetWidth)
	virtual BOOL OnInitDialog();
	afx_msg void OnOk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSETWIDTH_H__6674CB06_9095_496F_93DB_359B313D60FC__INCLUDED_)
