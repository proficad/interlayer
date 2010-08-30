// MyPreviewView.h : header file
//

#include "afxpriv.h"

/////////////////////////////////////////////////////////////////////////////
// CMyPreviewView view
#define MAX_DLLS			50

class CGridCtrl;

class CMyPreviewView : public CPreviewView
{
protected:
	CMyPreviewView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CMyPreviewView)

// Attributes
public:

protected:
// Operations
	CGridCtrl*	m_pGridCtrl;
public:
// Overrides
	void	SetDialogPointer(CGridCtrl* pGrid);
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyPreviewView)
protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMyPreviewView();
#ifdef _DEBUG
	virtual void	AssertValid() const;
	virtual void	Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyPreviewView)
	//}}AFX_MSG
	afx_msg void	OnPreviewPrint();
protected:
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
