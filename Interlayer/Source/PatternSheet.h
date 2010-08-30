// PatternSheet.h : header file
//
// This class defines custom modal property sheet 
// CPatternSheet.
 // CPatternSheet has been customized to include
// a preview window.
 
#ifndef __PATTERNSHEET_H__
#define __PATTERNSHEET_H__

#include "PatternPage.h"
#include "PatternPreviewWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CPatternSheet

class CPatternSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CPatternSheet)

// Construction
public:
	CPatternSheet(LPCTSTR pszCaption, CWnd* pWndParent = NULL);

// Attributes
public:
	CTransitionPage m_Page1;
	CTexturePage m_Page2;
	CPatternPage m_Page3;
	CPicturePage m_Page4;
	CPatternPreviewWnd m_wndPreview;

	PATTERNSTYLE patternStyle;
	
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPatternSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPatternSheet();
		 virtual BOOL OnInitDialog();

// Generated message map functions
protected:
	//{{AFX_MSG(CPatternSheet)
	afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __PATTERNSHEET_H__
