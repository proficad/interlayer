#if !defined(AFX_DLGSETPOLY_H__817783E9_DC12_4343_BBEC_1556D68B0706__INCLUDED_)
#define AFX_DLGSETPOLY_H__817783E9_DC12_4343_BBEC_1556D68B0706__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSetPoly.h : header file
//
#include "CommonStruct.h"
#include "../Resource.h"
#include "../Source/ArrowPicker.h"
#include "../Source/ColourPicker.h"
#include "../Source/ComboLine.h"
#include "../Source/ComboLineStyle.h"
#include "../Source/ComboFill.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgPolySet dialog

class CDlgPolySet : public CDialog
{
// Construction
public:
	CDlgPolySet(CWnd* pParent = NULL);   // standard constructor

	int nPenStyle;
	int nFillStyle;
	
// Dialog Data
	//{{AFX_DATA(CDlgPolySet)
	enum { IDD = IDD_DLG_POLY_SET };
	
	CSliderCtrl	m_CtrlTrans;//透明度
	CColourPicker	m_PenPicker;//画笔颜色按钮
	CColourPicker	m_PenPicker2;//画笔颜色按钮
	CColourPicker	m_FillPicker;//填充颜色按钮
	CColourPicker	m_ColourPickerLineFill;//线间颜色按钮
	CArrowPicker	m_BeginArrowPicker;//始端箭头按钮
	CArrowPicker	m_EndArrowPicker;//末端箭头按钮
	int		m_nTrans;//透明度
	int		m_iSpaceWidth;		// 线间宽度
	
	//}}AFX_DATA

	ELEMENTLINE		eLine;			// 线属性
	ELEMENTFILL		eFill;			// 填充属性
	ELEMENTLINE		eLine2;			// 线属性

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPolySet)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgPolySet)
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
	CComboLineStyle lineStyle;
	CComboLine line2;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSETPOLY_H__817783E9_DC12_4343_BBEC_1556D68B0706__INCLUDED_)
