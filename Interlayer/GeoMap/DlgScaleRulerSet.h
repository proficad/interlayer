#if !defined(AFX_DLGSCALERULERSET_H__182D9A8D_A5C6_4AF4_A216_43F196C0324F__INCLUDED_)
#define AFX_DLGSCALERULERSET_H__182D9A8D_A5C6_4AF4_A216_43F196C0324F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgScaleRulerSet.h : header file
//
#include "../Resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgScaleRulerSet dialog

class CDlgScaleRulerSet : public CDialog
{
// Construction
public:
	CDlgScaleRulerSet(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgScaleRulerSet)
	enum { IDD = IDD_DLG_SCALERULER_SET };
	CMFCFontComboBox	m_wndFont;
	CMFCColorButton	m_ColorFont;
	CMFCColorButton	m_ColorPen;
	UINT	m_uFontSize;
	UINT	m_uHeight;
	UINT	m_uWidth;
	int		m_ScaleType;
	//}}AFX_DATA

	LOGFONT m_lfFont;
	COLORREF m_clrFont;
	COLORREF m_clrPen;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgScaleRulerSet)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgScaleRulerSet)
	afx_msg void OnBtnFontColor();
	afx_msg void OnBtnColorPen();
	afx_msg void OnOk();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSCALERULERSET_H__182D9A8D_A5C6_4AF4_A216_43F196C0324F__INCLUDED_)
