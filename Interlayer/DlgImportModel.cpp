// DlgImportModel.cpp : 实现文件
//

#include "stdafx.h"
#include "Interlayer.h"
#include "DlgImportModel.h"


// CDlgImportModel

IMPLEMENT_DYNAMIC(CDlgImportModel, CFileDialog)

CDlgImportModel::CDlgImportModel(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
	m_nFileType = 0;
	m_ofn.lpTemplateName = MAKEINTRESOURCE(IDD_DLG_IMPORT_MODEL);
}

CDlgImportModel::~CDlgImportModel()
{
}


BEGIN_MESSAGE_MAP(CDlgImportModel, CFileDialog)
	ON_WM_PAINT()
	ON_CBN_SELENDOK(IDC_COMBO_TYPE, OnSelendokComboType)
END_MESSAGE_MAP()



// CDlgImportModel 消息处理程序


void CDlgImportModel::DoDataExchange(CDataExchange* pDX)
{
	CFileDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgImportGridModel)
	DDX_Control(pDX, IDC_COMBO_TYPE, m_wndFileType);
	DDX_CBIndex(pDX, IDC_COMBO_TYPE, m_nFileType);
	//}}AFX_DATA_MAP
}

BOOL CDlgImportModel::OnInitDialog() 
{
	CFileDialog::OnInitDialog();

	// TODO: Add extra initialization here
	m_ofn.hInstance = AfxGetInstanceHandle();

	m_wndFileType.AddString("PBRS格式文件");
	m_wndFileType.AddString("VIP格式文件");
	m_wndFileType.AddString("ECLIPSE格式文件");

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgImportModel::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here

	// Do not call CFileDialog::OnPaint() for painting messages
}

void CDlgImportModel::OnSelendokComboType() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

