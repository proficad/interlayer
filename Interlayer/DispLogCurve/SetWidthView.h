#if !defined(AFX_SETWIDTHVIEW_H__D6DF042D_5F10_4F4C_8BE5_4E85BB186158__INCLUDED_)
#define AFX_SETWIDTHVIEW_H__D6DF042D_5F10_4F4C_8BE5_4E85BB186158__INCLUDED_


#include "SetWidthEdit.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetWidthView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetWidthView view

class CSetWidthView : public CScrollView
{
	
	
protected:
	CSetWidthView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CSetWidthView)

// Attributes
public:

	// Operations
public:
	LRESULT OnSetWidth(WPARAM wp, LPARAM lp);
	void UpdateWidth();
	void AddStr(CString str);
	void GetSeqWidth(int*iSeqsWidth);
	int GetSeqCounts();
	CFont m_font;
	CArray <int,int> m_arWidth;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetWidthView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSetWidthView();
#ifdef _DEBUG
#endif

	// Generated message map functions
	//{{AFX_MSG(CSetWidthView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CSetWidthEdit m_Edit;
	CStringArray m_arrStr;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETWIDTHVIEW_H__D6DF042D_5F10_4F4C_8BE5_4E85BB186158__INCLUDED_)
