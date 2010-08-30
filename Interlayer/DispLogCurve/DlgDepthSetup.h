#if !defined(AFX_DLGDEPTHSETUP_H__B0E2AB71_1D51_4876_9337_014D45A731AC__INCLUDED_)
#define AFX_DLGDEPTHSETUP_H__B0E2AB71_1D51_4876_9337_014D45A731AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgDepthSetup.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgDepthSetup dialog

class CDlgDepthSetup : public CDialog
{
// Construction
public:
	CDlgDepthSetup(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgDepthSetup)
	enum { IDD = IDD_DLG_DISPDEPTH_SET };
	double	m_dMaxDepth;
	double	m_dMaxVal;
	double	m_dMinDepth;
	double	m_dMinVal;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDepthSetup)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgDepthSetup)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDEPTHSETUP_H__B0E2AB71_1D51_4876_9337_014D45A731AC__INCLUDED_)
