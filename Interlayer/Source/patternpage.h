// PatternPage.h : header file
//

#ifndef __PATTERNPAGE_H__
#define __PATTERNPAGE_H__
#include "../Resource.h"
/////////////////////////////////////////////////////////////////////////////
// CTransitionPage dialog
#include "ColourPicker.h"

class CColourPicker;

class CTransitionPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CTransitionPage)

// Construction
public:
	CTransitionPage();
	~CTransitionPage();

// Dialog Data
	//{{AFX_DATA(CTransitionPage)
	enum { IDD = IDD_PROPPAGE1 };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTransitionPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTransitionPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////
// CTexturePage dialog

class CTexturePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CTexturePage)

// Construction
public:
	CTexturePage();
	~CTexturePage();

// Dialog Data
	//{{AFX_DATA(CTexturePage)
	enum { IDD = IDD_PROPPAGE2 };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTexturePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTexturePage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////
// CPatternPage dialog

class CPatternPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CPatternPage)

// Construction
public:
	
	COLORREF m_crColourBack;
	COLORREF m_crColourFore;

	int nCol;
	int nRow;
	CString m_strPatternName[48];
	CPatternPage();
	~CPatternPage();

// Dialog Data
	//{{AFX_DATA(CPatternPage)
	enum { IDD = IDD_PROPPAGE3 };
		CColourPicker    m_ColourFore;
		CColourPicker    m_ColourBack;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPatternPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPatternPage)
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg LONG OnSelEndOK(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CPicturePage dialog

class CPicturePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CPicturePage)

// Construction
public:
	CPicturePage();
	~CPicturePage();

// Dialog Data
	//{{AFX_DATA(CPicturePage)
	enum { IDD = IDD_PROPPAGE4 };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPicturePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPicturePage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};



#endif // __PATTERNPAGE_H__
