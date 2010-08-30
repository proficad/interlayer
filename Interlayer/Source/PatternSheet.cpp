// PatternSheet.cpp : implementation file
//

#include "stdafx.h"
#include "../Resource.h"
#include "PatternSheet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPatternSheet

IMPLEMENT_DYNAMIC(CPatternSheet, CPropertySheet)

CPatternSheet::CPatternSheet(LPCTSTR pszCaption, CWnd* pWndParent)
	 : CPropertySheet(pszCaption, pWndParent)
{
	// Add all of the property pages here.  Note that
	// the order that they appear in here will be
	// the order they appear in on screen.  By default,
	// the first page of the set is the active one.
	// One way to make a different property page the 
	// active one is to call SetActivePage().

	//AddPage(&m_Page1);
	//AddPage(&m_Page2);
	AddPage(&m_Page3);
	//AddPage(&m_Page4);	
}

CPatternSheet::~CPatternSheet()
{
}


BEGIN_MESSAGE_MAP(CPatternSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CPatternSheet)
	ON_WM_NCCREATE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPatternSheet message handlers

BOOL CPatternSheet::OnInitDialog()
{
	BOOL bResult = CPropertySheet::OnInitDialog();

	m_Page3.nCol = patternStyle.m_PatternFill.m_nCol;
	m_Page3.nRow = patternStyle.m_PatternFill.m_nRow;
	m_Page3.m_crColourBack = patternStyle.m_PatternFill.m_crColourBack;
	m_Page3.m_crColourFore = patternStyle.m_PatternFill.m_crColourFore;

	// add a preview window to the property sheet.
	CWnd *pWnd = GetDlgItem(ID_APPLY_NOW);
	pWnd->ShowWindow(SW_HIDE);
	pWnd = GetDlgItem(IDHELP);
	pWnd->ShowWindow(SW_HIDE);

	CRect rectWnd;
	GetWindowRect(rectWnd);
	SetWindowPos(NULL, 0, 0,
		rectWnd.Width() + 100,
		rectWnd.Height() - 25,
		SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	CRect rectPreview(rectWnd.Width() + 5, rectWnd.Height() - 155,
		rectWnd.Width() + 80, rectWnd.Height() - 80);

	pWnd = GetDlgItem(IDOK);
	pWnd->SetWindowPos(NULL, rectWnd.Width()+4, 8, 0, 0, SWP_NOSIZE);

	pWnd = GetDlgItem(IDCANCEL);
	pWnd->SetWindowPos(NULL, rectWnd.Width()+4, 36, 0, 0, SWP_NOSIZE);

	m_wndPreview.Create(NULL, NULL, WS_CHILD|WS_VISIBLE,
		rectPreview, this, 0x1000);

	CenterWindow();
	return bResult;
}



BOOL CPatternSheet::OnNcCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (!CPropertySheet::OnNcCreate(lpCreateStruct))
		return FALSE;
	
	ModifyStyleEx(0, WS_EX_CONTEXTHELP);
	
	return TRUE;
}

void CPatternSheet::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	dc.SetBkMode(1);

	CRect rect;
	GetClientRect(rect);
	CFont font;
	font.CreateFont(-12, 0, 0, 0, 0, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, "ËÎÌå");
	CFont *pOldFont = dc.SelectObject(&font);
	dc.TextOut(rect.right - 90, rect.bottom - 125, "Ê¾Àý:");
	dc.SelectObject(pOldFont);
	font.DeleteObject();
}
