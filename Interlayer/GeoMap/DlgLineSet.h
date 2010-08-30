#if !defined(AFX_DLGLINESET_H__1FD07F0C_ACC8_4463_A0B5_3182B4809D28__INCLUDED_)
#define AFX_DLGLINESET_H__1FD07F0C_ACC8_4463_A0B5_3182B4809D28__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgLineSet.h : header file
//
#include "CommonStruct.h"
#include "../Resource.h"
#include "../Source/ArrowPicker.h"
#include "../Source/ColourPicker.h"
#include "../Source/ComboLine.h"
#include "../Source/ComboLineStyle.h"
#include "../Source/ComboFill.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgLineSet dialog

class CDlgLineSet : public CDialog
{
// Construction
public:
	CDlgLineSet(CWnd* pParent = NULL);   // standard constructor

	int nPenStyle;
	int nFillStyle;
// Dialog Data
	//{{AFX_DATA(CDlgLineSet)
	enum { IDD = IDD_DLG_LINE_SET };
	CColourPicker	m_PenPicker;//画笔颜色按钮
	CArrowPicker	m_BeginArrowPicker;//始端箭头按钮
	CArrowPicker	m_EndArrowPicker;//末端箭头按钮
	//}}AFX_DATA

	ELEMENTLINE		eLine;			// 线属性

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgLineSet)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgLineSet)
	afx_msg void OnCheckFill();
	afx_msg void OnCheckPolygon();
	virtual BOOL OnInitDialog();
	afx_msg void OnOk();
	afx_msg LONG OnSelEndOK(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSelendokComboPenstyle();
	afx_msg void OnCheckLinefill();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CComboLine line;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGLINESET_H__1FD07F0C_ACC8_4463_A0B5_3182B4809D28__INCLUDED_)
