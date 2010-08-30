#if !defined(AFX_RESIZABLEFORMVIEW_H__INCLUDED_)
#define AFX_RESIZABLEFORMVIEW_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ResizableFormView.h : header file
//

#include "ResizableLayout.h"

/////////////////////////////////////////////////////////////////////////////
// CResizableFormView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class AFX_EXT_CLASS CResizableFormView : public CFormView, public CResizableLayout
{
	DECLARE_DYNAMIC(CResizableFormView)

// Construction
protected:      // must derive your own class
	CResizableFormView(UINT nIDTemplate);
	CResizableFormView(LPCTSTR lpszTemplateName);
	virtual ~CResizableFormView();

private:
	void Construct();
	// internal status

	BOOL m_bInitDone;			// if all internal vars initialized


// called from base class
protected:

	virtual void GetTotalClientRect(LPRECT lpRect);

	virtual CWnd* GetResizableWnd()
	{
		// make the layout know its parent window
		return this;
	};


// Attributes
public:

// Operations
public:

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResizableFormView)
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

// Implementation
protected:

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CResizableFormView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESIZABLEFORMVIEW_H__INCLUDED_)
