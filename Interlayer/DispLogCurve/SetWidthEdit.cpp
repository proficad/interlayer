// SetWidthEdit.cpp : implementation file
//

#include "stdafx.h"
#include "SetWidthEdit.h"

#define WM_SETWIDTH WM_USER+1

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetWidthEdit

CSetWidthEdit::CSetWidthEdit()
{
}

CSetWidthEdit::~CSetWidthEdit()
{
}


BEGIN_MESSAGE_MAP(CSetWidthEdit, CEdit)
	//{{AFX_MSG_MAP(CSetWidthEdit)
	ON_WM_GETDLGCODE()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetWidthEdit message handlers

UINT CSetWidthEdit::OnGetDlgCode() 
{
	// TODO: Add your message handler code here and/or call default
	
	return DLGC_WANTALLKEYS;
}

void CSetWidthEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(nChar == 0x0d || nChar == ' ')
	{
		int s,e;
		GetSel(s,e);
		CWnd *pWnd = GetParent();
		pWnd->SendMessage(WM_SETWIDTH,0,(LPARAM)e);
	}
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}
