#if !defined(AFX_DLGCURVESETUP_H__365840F3_5B2B_4296_8352_5B9798B7902E__INCLUDED_)
#define AFX_DLGCURVESETUP_H__365840F3_5B2B_4296_8352_5B9798B7902E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgCurveSetup.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgCurveSetup dialog
#include "ComboLine.h"

class CDlgCurveSetup : public CDialog
{
// Construction
public:
	CDlgCurveSetup(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgCurveSetup)
	enum { IDD = IDD_DLG_CURVE_SET };
	CComboLine	m_lineType;
	int		m_iFillStyle;
	UINT	m_uPenWidth;
	double	m_dMinVal;
	double	m_dMaxVal;
	double	m_dCoordMin;
	double	m_dCoordMax;
	BOOL	m_bFill;
	UINT	m_iPart;
	UINT	m_iMarkPart;
	BOOL	m_bDirect;
	BOOL	m_bSameCoord;
	int		m_iLineStyle;
	BOOL	m_bLogCoord;
	CString	m_strCurveTitle;
	CString	m_strDepthFormula;
	CString	m_strVarFormula;
	//}}AFX_DATA

	COLORREF m_clrFill;
	COLORREF m_clrLine;

	CMFCColorButton m_FillColor;
	CMFCColorButton m_LineColor;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCurveSetup)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgCurveSetup)
	afx_msg void OnBtnLinecolor();
	afx_msg void OnFillcolor();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCURVESETUP_H__365840F3_5B2B_4296_8352_5B9798B7902E__INCLUDED_)
