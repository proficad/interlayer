// DlgAddCurve.cpp : implementation file
//

#include "stdafx.h"
#include "DlgAddCurve.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CString GetLogComments(const CString &strCurLogName);
/////////////////////////////////////////////////////////////////////////////
// CDlgAddCurve dialog


CDlgAddCurve::CDlgAddCurve(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAddCurve::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAddCurve)
	m_strFormula = _T("");
	m_strCurveName = _T("");
	m_bCustomCurve = FALSE;
	//}}AFX_DATA_INIT
	m_pChartLog = NULL;
	m_bLogCurve = NULL;
}

CDlgAddCurve::~CDlgAddCurve()
{
	if( m_bLogCurve )
		delete []m_bLogCurve;
	m_bLogCurve = NULL;
}


void CDlgAddCurve::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAddCurve)
	DDX_Control(pDX, IDC_LIST_LOG_NAME, m_logName);
	DDX_Text(pDX, IDC_EDIT_CURVE_FORMULA, m_strFormula);
	DDX_Text(pDX, IDC_EDIT_CURVE_NAME, m_strCurveName);
	DDX_Check(pDX, IDC_CHECK_CUSTOM_CURVER, m_bCustomCurve);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgAddCurve, CDialog)
	//{{AFX_MSG_MAP(CDlgAddCurve)
	ON_NOTIFY(NM_CLICK, IDC_LIST_LOG_NAME, OnClickListLogName)
	ON_BN_CLICKED(IDC_CHECK_CUSTOM_CURVER, OnCheckCustomCurver)
	ON_BN_CLICKED(IDOK, OnOk)
	ON_BN_CLICKED(IDC_BTN_ALL_SELECTED, OnBtnAllSelected)
	ON_BN_CLICKED(IDC_BTN_RES_SELECTED, OnBtnResSelected)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAddCurve message handlers

BOOL CDlgAddCurve::OnInitDialog() 
{
	CDialog::OnInitDialog();	

	m_logName.InsertColumn (0, _T("变量名"), LVCFMT_LEFT, 60);
	m_logName.InsertColumn (1, _T("测井曲线"), LVCFMT_LEFT, 80);

	int n = m_pChartLog->m_logDat.GetLogCount();
	m_bLogCurve = new BOOL [n];
	m_bLogCurve[0] = FALSE;
	for (int i=1;i<n;i++)
	{
		CString str;
		str.Format ("Var%02d", i);
		m_logName.InsertItem (i-1, str);
	
		m_logName.SetItemText (i-1, 1, m_pChartLog->m_logDat.GetLogName(i));

		m_bLogCurve[i] = FALSE;
	}
	
	m_logName.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0,
						LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES);

	if( m_bCustomCurve )
	{
		GetDlgItem(IDC_EDIT_CURVE_FORMULA)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_CURVE_NAME)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_CURVE_FORMULA)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_CURVE_NAME)->EnableWindow(FALSE);
	}
	return TRUE;
}

void CDlgAddCurve::OnClickListLogName(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData(TRUE);
	// TODO: Add your control notification handler code here
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
	if( phdn->iItem != -1 && m_bCustomCurve )
	{
		CString str = m_logName.GetItemText(phdn->iItem,0);
		m_strFormula += str;
		
		if( m_strCurveName.IsEmpty() )
		{
			str = m_logName.GetItemText(phdn->iItem,1);
			CString title="";
			title = GetLogComments(str);
			//m_pChartLog->m_mapLogTitle.Lookup(str,title);
			m_strCurveName = title;
		}
		
		UpdateData(FALSE);
	}

	*pResult = 0;
}

void CDlgAddCurve::OnCheckCustomCurver() 
{
	UpdateData(TRUE);

	if( m_bCustomCurve )
	{
		GetDlgItem(IDC_EDIT_CURVE_FORMULA)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_CURVE_NAME)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_CURVE_FORMULA)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_CURVE_NAME)->EnableWindow(FALSE);
	}
}

void CDlgAddCurve::OnOk() 
{
	UpdateData(TRUE);
	int n = m_pChartLog->m_logDat.GetLogCount()-1;
	for (int i=0;i<n;i++)
		m_bLogCurve[i+1] = m_logName.GetCheck(i);
	CDialog::OnOK();
}



void CDlgAddCurve::OnBtnAllSelected() 
{
	// TODO: Add your control notification handler code here
	int n = m_pChartLog->m_logDat.GetLogCount()-1;
	for (int i=0;i<n;i++)
		m_logName.SetCheck(i, TRUE);
}

void CDlgAddCurve::OnBtnResSelected() 
{
	// TODO: Add your control notification handler code here
	int n = m_pChartLog->m_logDat.GetLogCount()-1;
	for (int i=0;i<n;i++)
	{
		m_logName.SetCheck(i, m_logName.GetCheck(i)?FALSE:TRUE);
	}
}
