// DlgGridPropSetup.cpp : 实现文件
//

#include "stdafx.h"
#include "Interlayer.h"
#include "DlgGridPropSetup.h"


// CDlgGridPropSetup 对话框

IMPLEMENT_DYNAMIC(CDlgGridPropSetup, CDialog)

CDlgGridPropSetup::CDlgGridPropSetup(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgGridPropSetup::IDD, pParent)
{
	m_dMinValue = 0.0;
	m_dMaxValue = 0.0;
	m_dDispMinValue = 0.0;
	m_dDispMaxValue = 0.0;
	m_strGridInfo = _T("");
}

CDlgGridPropSetup::~CDlgGridPropSetup()
{
}

void CDlgGridPropSetup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MINV, m_dMinValue);
	DDX_Text(pDX, IDC_EDIT_MAXV, m_dMaxValue);
	DDX_Text(pDX, IDC_EDIT_MINDV, m_dDispMinValue);
	DDX_Text(pDX, IDC_EDIT_MAXDV, m_dDispMaxValue);
	DDX_Text(pDX, IDC_EDIT_GRID, m_strGridInfo);
	DDX_Control(pDX, IDC_BTN_MINC, m_wndMinColor);
	DDX_Control(pDX, IDC_BTN_MAXC, m_wndMaxColor);
}


BEGIN_MESSAGE_MAP(CDlgGridPropSetup, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgGridPropSetup::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_MINC, &CDlgGridPropSetup::OnBnClickedBtnMinc)
	ON_BN_CLICKED(IDC_BTN_MAXC, &CDlgGridPropSetup::OnBnClickedBtnMaxc)
END_MESSAGE_MAP()


// CDlgGridPropSetup 消息处理程序

void CDlgGridPropSetup::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}

void CDlgGridPropSetup::OnBnClickedBtnMinc()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_clrMinColor = m_wndMinColor.GetColor();
}

void CDlgGridPropSetup::OnBnClickedBtnMaxc()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_clrMaxColor = m_wndMaxColor.GetColor();
}

BOOL CDlgGridPropSetup::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	m_wndMinColor.EnableAutomaticButton (_T("自动"), RGB (0, 0, 0));
	m_wndMinColor.EnableOtherButton (_T("其它"));
	m_wndMinColor.SetColor (m_clrMinColor);
	m_wndMinColor.SetColumnsNumber (10);

	m_wndMaxColor.EnableAutomaticButton (_T("自动"), RGB (0, 0, 0));
	m_wndMaxColor.EnableOtherButton (_T("其它"));
	m_wndMaxColor.SetColor (m_clrMaxColor);
	m_wndMaxColor.SetColumnsNumber (10);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
