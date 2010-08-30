// DlgTemplSelect.cpp : implementation file
//

#include "stdafx.h"
#include "displogcurve.h"
#include "DlgTemplSelect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTemplSelect dialog


CDlgTemplSelect::CDlgTemplSelect(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTemplSelect::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTemplSelect)
	m_strTemplName = _T("");
	//}}AFX_DATA_INIT
}


void CDlgTemplSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTemplSelect)
	DDX_Control(pDX, IDC_COMBO_TEMPL, m_wndCombo);
	DDX_CBString(pDX, IDC_COMBO_TEMPL, m_strTemplName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTemplSelect, CDialog)
	//{{AFX_MSG_MAP(CDlgTemplSelect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTemplSelect message handlers

BOOL CDlgTemplSelect::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CString strSQL;
	
	CString strConnection;
	
	CWinAppEx * pApp = (CWinAppEx * )AfxGetApp();
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	
	_splitpath_s(pApp->m_pszHelpFilePath, drive, dir, fname, ext);
	
	CString strFilePath = drive;
	strFilePath += dir;
	
	strConnection = _T("Driver={Microsoft Access Driver (*.mdb)};Dbq=");
	strConnection += strFilePath;
	strConnection += "Legend.mdb;";
	strConnection += "Uid=;Pwd=;";
	
	CADODatabase m_pDb;
	if(m_pDb.Open(strConnection))
	{	
		CString strSQL = _T("select MC from [ÏÔÊ¾Ä£°å] where LB = 1 ");
		
		CADORecordset m_pRs = CADORecordset(&m_pDb);
		if (m_pRs.Open((LPCTSTR)strSQL))
		{
			while (!m_pRs.IsEOF())
			{	
				CString str;
				m_pRs.GetFieldValue(0,str);
				m_wndCombo.AddString(str);
				m_pRs.MoveNext();
			}
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
