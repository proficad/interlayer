// ClrFileDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ClrFileDialog.h"
#include "../Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClrFileDialog

IMPLEMENT_DYNAMIC(CClrFileDialog, CFileDialog)

CClrFileDialog::CClrFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
		m_ofn.lpTemplateName = MAKEINTRESOURCE(IDD_FILECOLORTMP_COLOR_TABLE);
}


BEGIN_MESSAGE_MAP(CClrFileDialog, CFileDialog)
	//{{AFX_MSG_MAP(CClrFileDialog)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CClrFileDialog::OnInitDialog()
{
	CFileDialog::OnInitDialog();
		
	m_ofn.hInstance = AfxGetInstanceHandle();
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CClrFileDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	DrawColorSpectrum();
}

void CClrFileDialog::DrawColorSpectrum()
{
	CWnd *pWnd=GetDlgItem(IDC_STATIC_PREVIEW);
	CDC *pDC=pWnd->GetDC();
	CRect rect;
	pWnd->GetClientRect(&rect);
	
	m_ColorTable.DrawColorSpectrum(pDC,rect);
	
	pWnd->ReleaseDC(pDC);	
}

void CClrFileDialog::OnFileNameChange()
{
	CString strFileName = this->GetFileName();
	if( !strFileName.IsEmpty() )
	{
		strFileName = GetPathName();
		m_ColorTable.LoadColorTable(strFileName);
		DrawColorSpectrum();
	}
	
	CFileDialog::OnFileNameChange();	
}
