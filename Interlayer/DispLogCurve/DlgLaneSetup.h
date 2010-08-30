#if !defined(AFX_DLGLANESETUP_H__AC0B3701_EF7E_41C1_8A15_2AB7E331278F__INCLUDED_)
#define AFX_DLGLANESETUP_H__AC0B3701_EF7E_41C1_8A15_2AB7E331278F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgLaneSetup.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgLaneSetup dialog

class CDlgLaneSetup : public CDialog
{
// Construction
public:
	
	CDlgLaneSetup(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgLaneSetup)
	enum { IDD = IDD_DLG_LANE_SET };
	BOOL	m_bFill;
	BOOL	m_bBorder;
	BOOL	m_bHorzLine;
	BOOL	m_bVertLine;
	double	m_dWidth;
	BOOL	m_bApplyAll;
	CString	m_strTitle;
	//}}AFX_DATA

	COLORREF m_clrFill;
	CMFCColorButton m_ColorPicker;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgLaneSetup)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgLaneSetup)
	afx_msg void OnBtnFillcolor();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGLANESETUP_H__AC0B3701_EF7E_41C1_8A15_2AB7E331278F__INCLUDED_)
