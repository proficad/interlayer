#if !defined(AFX_LOGCURVETABFRAME_H__D5CF2AA2_F118_42DD_98BF_C0EFEEFEBF31__INCLUDED_)
#define AFX_LOGCURVETABFRAME_H__D5CF2AA2_F118_42DD_98BF_C0EFEEFEBF31__INCLUDED_

#include "WatchBar.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LogCurveTabFrame.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDispLogCurveFrame frame

class CDispLogCurveFrame : public CMDIChildWndEx
{
	DECLARE_DYNCREATE(CDispLogCurveFrame)
protected:
	CDispLogCurveFrame();           // protected constructor used by dynamic creation

// Attributes
public:

	CMFCToolBar  		m_wndToolBar;
	CMenu       m_menu;         // menu to give to the the MDI Frame

// Operations
public:
	CWatchBar m_wndWatchBar;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDispLogCurveFrame)
	public:
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDispLogCurveFrame();

	// Generated message map functions
	//{{AFX_MSG(CDispLogCurveFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewWatchBar();
	afx_msg void OnUpdateViewWatchBar(CCmdUI* pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg LRESULT OnToolbarReset(WPARAM wp,LPARAM);
	afx_msg LRESULT OnToolbarContextMenu(WPARAM,LPARAM lp);
	
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGCURVETABFRAME_H__D5CF2AA2_F118_42DD_98BF_C0EFEEFEBF31__INCLUDED_)
