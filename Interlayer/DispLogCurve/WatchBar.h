// WatchBar.h: interface for the CWatchBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WATCHBAR_H__F5AFD609_5A68_4BAA_9BF2_7F4D9E95517A__INCLUDED_)
#define AFX_WATCHBAR_H__F5AFD609_5A68_4BAA_9BF2_7F4D9E95517A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWatchBar : public CDockablePane
{
	// Construction
public:
	CWatchBar();
	
	// Attributes
protected:
	CFont			m_Font;
public:
	CMFCListCtrl	m_wndWatch;
	
	// Operations
public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWatchBar)
	//}}AFX_VIRTUAL
	
	// Implementation
public:
	virtual ~CWatchBar();
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CWatchBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_WATCHBAR_H__F5AFD609_5A68_4BAA_9BF2_7F4D9E95517A__INCLUDED_)
