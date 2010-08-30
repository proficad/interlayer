// DlgSetWidth.cpp : implementation file
//

#include "stdafx.h"
#include "DlgSetWidth.h"
#include "SetWidthView.h"
#include "EmptyDoc.h"
#include "../afc/src/DFVCtrl.h"
#include "../afc/src/DFVFrameWnd.h"
#include "../afc/src/DFVDocTemplate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSetWidth dialog


CDlgSetWidth::CDlgSetWidth(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSetWidth::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSetWidth)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pView = NULL;
	m_strTitleWidth = "";
}


void CDlgSetWidth::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSetWidth)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSetWidth, CDialog)
	//{{AFX_MSG_MAP(CDlgSetWidth)
	ON_BN_CLICKED(IDOK, OnOk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSetWidth message handlers

BOOL CDlgSetWidth::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_DFVCtrl.Create(this, IDC_STATIC_1,
		IDR_POPUP_DISPLOGCURVE,
		RUNTIME_CLASS(CEmptyDoc),
		RUNTIME_CLASS(CDFVFrameWnd),       // main SDI frame window
		RUNTIME_CLASS(CSetWidthView),
		WS_CHILD | WS_BORDER | WS_VISIBLE, 0L);

	if( m_pView )
	{
		CString tmp;

		tmp = pList->GetItemText(m_uTitleRow-1, 1);
		tmp.Replace("	"," ");
		m_pView->AddStr(tmp);
		int nRows = pList->GetItemCount();
		
		int row = m_iFirstRow-1;
		for (int i = 0; (i < 10 || row < nRows); i++)
		{		
			tmp = pList->GetItemText(row, 1);
			tmp.Replace("	"," ");
			m_pView->AddStr(tmp);
			row++;
		}	
	}
	m_pView->UpdateWidth();
	char *tok = NULL;
	char *next_token = NULL;
	tok = strtok_s((LPSTR)(LPCSTR)m_strTitleWidth, ",",&next_token);
	int w =0 ;
	while (tok != NULL)
	{		
		w += atoi(tok);
		m_pView->m_arWidth.Add(w);
		tok = strtok_s(NULL, ",",&next_token);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSetWidth::OnOk() 
{
	m_strTitleWidth = "";
	int n = m_pView->GetSeqCounts();
	if( n > 0 )
	{
		int * p = new int[n];
		m_pView->GetSeqWidth(p);
		CString tmp;
		for (int i=0;i<n;i++)
		{
			tmp.Format("%d,",i==0?p[i]:p[i]-p[i-1]);
			m_strTitleWidth += tmp;
		}
		delete []p;
	}
	CDialog::OnOK();
}
