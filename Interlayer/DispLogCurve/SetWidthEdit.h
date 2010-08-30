#if !defined(AFX_SETWIDTHEDIT_H__B3BBC2E9_1DB3_45BE_99DE_5503B325EDE0__INCLUDED_)
#define AFX_SETWIDTHEDIT_H__B3BBC2E9_1DB3_45BE_99DE_5503B325EDE0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetWidthEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetWidthEdit window

class CSetWidthEdit : public CEdit
{
// Construction
public:
	CSetWidthEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetWidthEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSetWidthEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSetWidthEdit)
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETWIDTHEDIT_H__B3BBC2E9_1DB3_45BE_99DE_5503B325EDE0__INCLUDED_)
