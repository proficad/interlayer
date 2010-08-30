#pragma once

#include "3DLib/ColorLookUpTable.h"	// Added by ClassView
#include "Resource.h"
#include "3DLib/ColorTableList.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgColorTableSet dialog

class CDlgColorTableSet : public CDialog
{
	// Construction
public:
	CColorLookUpTable m_ColorTable;
	CDlgColorTableSet(CWnd* pParent = NULL);   // standard constructor

	// Dialog Data
	//{{AFX_DATA(CDlgColorTableSet)
	enum { IDD = IDD_DLG_COLOR_TABLE_SET };
	CColorTableList	m_wndListColor;
	CMFCColorButton	m_wndSelColor;
	CMFCColorButton	m_wndNewColor;
	double	m_NewPegPosition;
	double	m_SelPegPosition;
	//}}AFX_DATA


	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgColorTableSet)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgColorTableSet)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnLoad();
	afx_msg void OnBtnSave();
	afx_msg void OnDelPeg();
	afx_msg void OnAddPeg();
	afx_msg void OnSelpegColour();
	afx_msg void OnPaint();
	afx_msg void OnSelchangeListColor();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};