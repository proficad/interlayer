#if !defined(AFX_DLGPAGESET_H__DC8E0670_29C9_4991_AE98_5550BC5F1FC7__INCLUDED_)
#define AFX_DLGPAGESET_H__DC8E0670_29C9_4991_AE98_5550BC5F1FC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgPageSet.h : header file
//
#include "../Resource.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgPageSet dialog

class CDlgPageSet : public CDialog
{
// Construction
public:
	CDlgPageSet(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgPageSet)
	enum { IDD = IDD_DLG_PAGE_SET };
	CString	m_strPageName;
	UINT	m_nHorzPages;
	UINT	m_nVertPages;
	double	m_lScale;
	double	m_dAngle;
	CString	m_strMark;
	int		m_iMarginWidth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPageSet)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgPageSet)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	double m_x;
	double m_y;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPAGESET_H__DC8E0670_29C9_4991_AE98_5550BC5F1FC7__INCLUDED_)
