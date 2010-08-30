// LogTemplSheet.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "LogTemplSheet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLogTemplSheet

IMPLEMENT_DYNAMIC(CLogTemplSheet, CPropertySheet)

CLogTemplSheet::CLogTemplSheet(CWnd* pWndParent)
	 : CPropertySheet(IDS_PROPSHT_CAPTION, pWndParent)
{
	// Add all of the property pages here.  Note that
	// the order that they appear in here will be
	// the order they appear in on screen.  By default,
	// the first page of the set is the active one.
	// One way to make a different property page the 
	// active one is to call SetActivePage().

	m_strPath = _T("c:\\B0000022.list");
	//m_strPath = _T("c:\\LAp111-501h2.txt");

	AddPage(&m_Page0);
	AddPage(&m_Page1);
//	AddPage(&m_Page2);

	SetWizardMode();	
}

CLogTemplSheet::~CLogTemplSheet()
{
}


BEGIN_MESSAGE_MAP(CLogTemplSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CLogTemplSheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CLogTemplSheet message handlers


