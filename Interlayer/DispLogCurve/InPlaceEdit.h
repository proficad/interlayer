#if !defined(AFX_INPLACEEDIT_H__FB28D94F_A3E1_44B6_9C65_E8FBE89EBEE2__INCLUDED_)
#define AFX_INPLACEEDIT_H__FB28D94F_A3E1_44B6_9C65_E8FBE89EBEE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InPlaceEdit.h : header file
//
namespace dispLogCurve
{

/////////////////////////////////////////////////////////////////////////////
// CInPlaceEdit window

class CInPlaceEdit : public CEdit
{
// Construction
public:
	CInPlaceEdit(CWnd* pParent, CRect &rect, DWORD dwStyle, UINT nID,
                 int nIndex, CString sInitText, UINT nFirstChar);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
public:
	int     m_nIndex;
    CString m_sInitText;
	void EndEdit();
	virtual ~CInPlaceEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CInPlaceEdit)
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
    
    UINT    m_nLastChar;
    BOOL    m_bExitOnArrows;
    CRect   m_Rect;
};
}
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPLACEEDIT_H__FB28D94F_A3E1_44B6_9C65_E8FBE89EBEE2__INCLUDED_)
