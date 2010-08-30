// MyEditView.h : interface of the CMyEditView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYEDITVIEW_H__A8582CE3_46F2_438C_9E22_F0BA333CC5A4__INCLUDED_)
#define AFX_MYEDITVIEW_H__A8582CE3_46F2_438C_9E22_F0BA333CC5A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HiliteEditView.h"
class CMyEditDoc;
class CMyEditView : public CHiliteEditView
{
protected: // create from serialization only
	CMyEditView();
	DECLARE_DYNCREATE(CMyEditView)

// Attributes
public:
	CMyEditDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyEditView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMyEditView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMyEditView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in MyEditView.cpp
inline CMyEditDoc* CMyEditView::GetDocument()
   { return (CMyEditDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYEDITVIEW_H__A8582CE3_46F2_438C_9E22_F0BA333CC5A4__INCLUDED_)
