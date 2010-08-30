// DlgTemplEdit.cpp : implementation file
//

#include "stdafx.h"
#include "displogcurve.h"
#include "DlgTemplEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTemplEdit dialog


CDlgTemplEdit::CDlgTemplEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTemplEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTemplEdit)
	m_strTemplName = _T("");
	//}}AFX_DATA_INIT
	m_bIsExist = FALSE;
}


void CDlgTemplEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTemplEdit)
	DDX_Control(pDX, IDC_COMBO_TEMPL, m_wndCombo);
	DDX_CBString(pDX, IDC_COMBO_TEMPL, m_strTemplName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTemplEdit, CDialog)
	//{{AFX_MSG_MAP(CDlgTemplEdit)
	ON_BN_CLICKED(IDC_BTN_DEL, OnBtnDel)
	ON_BN_CLICKED(IDOK, OnOk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTemplEdit message handlers

BOOL CDlgTemplEdit::OnInitDialog() 
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
		CString strSQL = _T("select MC from [显示模板] where LB = 1 ");
		
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
		m_pRs.Close();

		m_pRs.Close();
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTemplEdit::OnBtnDel() 
{
	UpdateData(TRUE);
	if( ::MessageBox(NULL, "您确定要删除此模板吗？","提示信息",MB_YESNO | MB_ICONQUESTION) == IDYES )
	{
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
			strSQL = "delete from [显示模板] where LB = 1 and mc ='";
			strSQL += m_strTemplName;
			strSQL += "'";
			if( m_pDb.Execute(strSQL) )
				m_wndCombo.DeleteString(m_wndCombo.FindString(0,m_strTemplName));
		}
		m_pDb.Close();

		
		m_wndCombo.DeleteString(m_wndCombo.GetCurSel());
	}
}

void CDlgTemplEdit::OnOk() 
{
	UpdateData(TRUE);
	int nCount = m_wndCombo.GetCount();
	for (int i=0; i<nCount; i++)
	{
		m_bIsExist = FALSE;
		CString str;
		m_wndCombo.GetLBText(i,str);
		m_strTemplName.TrimRight();
		m_strTemplName.TrimLeft();

		if( m_strTemplName.IsEmpty() )
		{
			::MessageBox(NULL, "该显示模板名不能为空", "提示信息" ,MB_OK|MB_ICONINFORMATION);
			return;
		}

		if( m_strTemplName == str )
		{
			if( ::MessageBox(NULL, "该显示模板名已经存在，是否覆盖？", "提示信息" ,MB_YESNO|MB_ICONQUESTION) == IDYES )
			{
				m_bIsExist = TRUE;
				break;
			}
			else
				return;
		}		
	}
	
	CDialog::OnOK();
}
