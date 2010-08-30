#if !defined(AFX_DLGTEMPLSELECT_H__CDB89E8E_C4F0_4740_916D_76EECBFEF548__INCLUDED_)
#define AFX_DLGTEMPLSELECT_H__CDB89E8E_C4F0_4740_916D_76EECBFEF548__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTemplSelect.h : header file
//
#include "Resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgTemplSelect dialog

class CDlgTemplSelect : public CDialog
{
// Construction
public:
	CDlgTemplSelect(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgTemplSelect)
	enum { IDD = IDD_DLG_SELECT_TEMPL };
	CComboBox	m_wndCombo;
	CString	m_strTemplName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTemplSelect)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTemplSelect)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTEMPLSELECT_H__CDB89E8E_C4F0_4740_916D_76EECBFEF548__INCLUDED_)
