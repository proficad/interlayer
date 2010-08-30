// DlgZSlice.cpp : 实现文件
//

#include "stdafx.h"
#include "Interlayer.h"
#include "DlgZSlice.h"


// CDlgZSlice 对话框

IMPLEMENT_DYNAMIC(CDlgZSlice, CDialog)

CDlgZSlice::CDlgZSlice(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgZSlice::IDD, pParent)
	, m_dZ(0)
	, m_bSpecified(FALSE)
	, m_dHeight(1.0)
	, m_bShow(TRUE)
	, m_strTitle(_T(""))
	, m_bShowSlicePlane(TRUE)
{
	m_dMinZ = -1000.0;
	m_dMaxZ = 10000.0;
}

CDlgZSlice::~CDlgZSlice()
{
}

void CDlgZSlice::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_Z, m_dZ);
	DDX_Text(pDX, IDC_EDIT_MAX, m_dMaxZ);
	DDX_Text(pDX, IDC_EDIT_MIN, m_dMinZ);
	DDV_MinMaxDouble(pDX, m_dZ, m_dMinZ, m_dMaxZ);
	DDX_Check(pDX, IDC_CHECK_AUTO, m_bSpecified);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_dHeight);
	DDV_MinMaxDouble(pDX, m_dHeight, 0.0001, 10000);
	DDX_Check(pDX, IDC_CHECK_SHOW, m_bShow);
	DDX_Check(pDX, IDC_CHECK_PLANE, m_bShowSlicePlane);
}


BEGIN_MESSAGE_MAP(CDlgZSlice, CDialog)
END_MESSAGE_MAP()


// CDlgZSlice 消息处理程序

BOOL CDlgZSlice::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	if( !m_strTitle.IsEmpty() )
		SetWindowText(m_strTitle);

	if( m_dZ < m_dMinZ)
		m_dZ = m_dMinZ;
	if( m_dZ>m_dMaxZ)
		m_dZ = m_dMaxZ;

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
