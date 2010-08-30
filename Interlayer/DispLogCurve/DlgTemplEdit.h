#if !defined(AFX_DLGTEMPLEDIT_H__96A07BD3_4027_45D7_843F_8C57AD3AD46A__INCLUDED_)
#define AFX_DLGTEMPLEDIT_H__96A07BD3_4027_45D7_843F_8C57AD3AD46A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTemplEdit.h : header file
//
#include "Resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgTemplEdit dialog

class CDlgTemplEdit : public CDialog
{
// Construction
public:
	CDlgTemplEdit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgTemplEdit)
	enum { IDD = IDD_DLG_EDIT_TEMPL };
	CComboBox	m_wndCombo;
	CString	m_strTemplName;
	//}}AFX_DATA

	BOOL m_bIsExist;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTemplEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTemplEdit)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnDel();
	afx_msg void OnOk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTEMPLEDIT_H__96A07BD3_4027_45D7_843F_8C57AD3AD46A__INCLUDED_)
