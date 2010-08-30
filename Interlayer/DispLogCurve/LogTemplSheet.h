// LogTemplSheet.h : header file
//
// This class defines custom modal property sheet 
// CLogTemplSheet.
 
#ifndef __LOGTEMPLSHEET_H__
#define __LOGTEMPLSHEET_H__

#include "LogTemplPage.h"

//#define  CPropertySheet CBCGPPropertySheet
/////////////////////////////////////////////////////////////////////////////
// CLogTemplSheet

class CLogTemplSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CLogTemplSheet)

// Construction
public:
	CLogTemplSheet(CWnd* pWndParent = NULL);

// Attributes
public:
	CLogTemplPage0 m_Page0;
	CLogTemplPage1 m_Page1;
//	CLogTemplPage2 m_Page2;

	CString m_strPath;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogTemplSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLogTemplSheet();

// Generated message map functions
protected:
	//{{AFX_MSG(CLogTemplSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __LOGTEMPLSHEET_H__
