#if !defined(AFX_DLGADDLANE_H__FCB0EFB4_5CF1_4163_8071_9990662571EB__INCLUDED_)
#define AFX_DLGADDLANE_H__FCB0EFB4_5CF1_4163_8071_9990662571EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgAddLane.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgAddLane dialog

class CDlgAddLane : public CDialog
{
	// Construction
public:
	CDlgAddLane(CWnd* pParent = NULL);   // standard constructor
	
	~CDlgAddLane();
	// Dialog Data
	//{{AFX_DATA(CDlgAddLane)
	enum { IDD = IDD_DLG_LANE_ADD };
	CListCtrl	m_listLane;
	//}}AFX_DATA
	
	
	CImageList *m_pImageList;
	
	int m_iSelected;
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgAddLane)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgAddLane)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickListLane(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGADDLANE_H__FCB0EFB4_5CF1_4163_8071_9990662571EB__INCLUDED_)
