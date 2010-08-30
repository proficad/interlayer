#if !defined(AFX_DLGRECTSET_H__B8545031_0C60_450C_85A4_AA609677014A__INCLUDED_)
#define AFX_DLGRECTSET_H__B8545031_0C60_450C_85A4_AA609677014A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgRectSet.h : header file
//

#include "CommonStruct.h"
#include "../Resource.h"
#include "../Source/ArrowPicker.h"
#include "../Source/ColourPicker.h"
#include "../Source/ComboLine.h"
#include "../Source/ComboLineStyle.h"
#include "../Source/ComboFill.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgRectSet dialog

class CDlgRectSet : public CDialog
{
// Construction
public:
	CDlgRectSet(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgRectSet)
	enum { IDD = IDD_DLG_RECT_SET };
	CSliderCtrl	m_Trans;
	CSpinButtonCtrl	m_spin;
	int		m_nTrans;
	int		m_iSpaceWidth;
	//}}AFX_DATA

	ELEMENTFILL eFill;//填充结构
	ELEMENTLINE eLine;//线结构
	ELEMENTLINE eLine2;//线结构
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgRectSet)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgRectSet)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckFill();
	afx_msg LONG OnSelEndOK(WPARAM wParam, LPARAM lParam);
	afx_msg void OnOk();
	afx_msg void OnCheckLinefill();
	afx_msg void OnSelendokComboLinetype();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:	
	CColourPicker m_LineColourPicker;
	CColourPicker m_LineColourPicker2;
	CComboLine penStyle;
	CComboLine penStyle2;
	CComboLineStyle lineStyle;
	CColourPicker m_FillColourPicker;
	CColourPicker	m_ColourPickerLineFill;//线间颜色按钮
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGRECTSET_H__B8545031_0C60_450C_85A4_AA609677014A__INCLUDED_)
