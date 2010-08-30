#if !defined(AFX_DLGADDCURVE_H__D669D989_02BF_4154_B0B1_B7C4DD8746B9__INCLUDED_)
#define AFX_DLGADDCURVE_H__D669D989_02BF_4154_B0B1_B7C4DD8746B9__INCLUDED_

#include "LogCurveDatStruct.h"
#include "resource.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAddCurve.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgAddCurve dialog

class CDlgAddCurve : public CDialog
{
// Construction
public:
	CDlgAddCurve(CWnd* pParent = NULL);   // standard constructor
	~CDlgAddCurve();

// Dialog Data
	//{{AFX_DATA(CDlgAddCurve)
	enum { IDD = IDD_DLG_ADD_CURVE };
	CListCtrl	m_logName;
	CString	m_strFormula;
	CString	m_strCurveName;
	BOOL	m_bCustomCurve;
	//}}AFX_DATA

	CLogChart *m_pChartLog;

	BOOL *m_bLogCurve;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAddCurve)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgAddCurve)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickListLogName(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCheckCustomCurver();
	afx_msg void OnOk();
	afx_msg void OnCheckAll();
	afx_msg void OnBtnAllSelected();
	afx_msg void OnBtnResSelected();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGADDCURVE_H__D669D989_02BF_4154_B0B1_B7C4DD8746B9__INCLUDED_)
