#if !defined(AFX_EMPTYDOC_H__38F50941_23B8_4922_8C7B_99CC6433B043__INCLUDED_)
#define AFX_EMPTYDOC_H__38F50941_23B8_4922_8C7B_99CC6433B043__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EmptyDoc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEmptyDoc document

class CEmptyDoc : public CDocument
{
protected:
	CEmptyDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CEmptyDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEmptyDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEmptyDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CEmptyDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EMPTYDOC_H__38F50941_23B8_4922_8C7B_99CC6433B043__INCLUDED_)
