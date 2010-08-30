#if !defined(AFX_DLGWELLSET_H__DC07C4DC_63A1_408F_B5CC_DF203646451F__INCLUDED_)
#define AFX_DLGWELLSET_H__DC07C4DC_63A1_408F_B5CC_DF203646451F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgWellSet.h : header file
//
#include "../Resource.h"
#include "GeoWellObj.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgWellSet dialog

class CDlgWellSet : public CDialog
{
// Construction
public:
	CDlgWellSet(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgWellSet)
	enum { IDD = IDD_DLG_WELL_SET };
	CMFCColorButton	m_wndPenColor;
	CMFCColorButton	m_wndFillColor;
	CComboBox	m_wndSymbol;
	CMFCColorButton	m_wndFontColor;
	CMFCFontComboBox	m_wndFont;
	UINT	m_nFontSize;
	UINT	m_nSymbolSize;
	int		m_iApplyWell;
	CString	m_strWellName;
	double	m_dZZBX;
	double	m_dHZBY;
	double	m_dBXHB;
	int		m_iWellStyle;
	int		m_iWellType;
	int		m_iApply;
	//}}AFX_DATA
	
	COLORREF m_clrFont;
	COLORREF m_clrPen;
	COLORREF m_clrFill;
	LOGFONT m_lfFont;
	CGeoWellObj *m_pWellObj;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgWellSet)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgWellSet)
	afx_msg void OnBtnFontColor();
	afx_msg void OnBtnPenColor();
	afx_msg void OnBtnFillColor();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGWELLSET_H__DC07C4DC_63A1_408F_B5CC_DF203646451F__INCLUDED_)
