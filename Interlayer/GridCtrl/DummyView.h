#if !defined(AFX_DUMMYVIEW_H__9AB25DD8_06B1_11D7_ADB6_00B0D0652E95__INCLUDED_)
#define AFX_DUMMYVIEW_H__9AB25DD8_06B1_11D7_ADB6_00B0D0652E95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DummyView.h : header file
//
#include <afxstat_.h>
/////////////////////////////////////////////////////////////////////////////
// CDummyView view
class CGridCtrl;

class CDummyView : public CView
{
public:
	CDummyView();
	virtual ~CDummyView();
protected:
	DECLARE_DYNCREATE(CDummyView)

// Attributes
public:
	void		SetPrintDialog(CGridCtrl* pGrid);
	void		OnDoPrintPreview();
	void		OnDoFilePrint();
	CFrameWnd*	m_pOldFrame;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDummyView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView);
	//}}AFX_VIRTUAL
// Implementation
protected:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CGridCtrl*	m_pGridCtrl;
	// Generated message map functions
protected:
	//{{AFX_MSG(CDummyView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	friend class CMyPreviewView;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DUMMYVIEW_H__9AB25DD8_06B1_11D7_ADB6_00B0D0652E95__INCLUDED_)
