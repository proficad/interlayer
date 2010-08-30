// DlgGridPropSetup.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Interlayer.h"
#include "DlgGridPropSetup.h"


// CDlgGridPropSetup �Ի���

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


// CDlgGridPropSetup ��Ϣ�������

void CDlgGridPropSetup::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnOK();
}

void CDlgGridPropSetup::OnBnClickedBtnMinc()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	m_clrMinColor = m_wndMinColor.GetColor();
}

void CDlgGridPropSetup::OnBnClickedBtnMaxc()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	m_clrMaxColor = m_wndMaxColor.GetColor();
}

BOOL CDlgGridPropSetup::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	m_wndMinColor.EnableAutomaticButton (_T("�Զ�"), RGB (0, 0, 0));
	m_wndMinColor.EnableOtherButton (_T("����"));
	m_wndMinColor.SetColor (m_clrMinColor);
	m_wndMinColor.SetColumnsNumber (10);

	m_wndMaxColor.EnableAutomaticButton (_T("�Զ�"), RGB (0, 0, 0));
	m_wndMaxColor.EnableOtherButton (_T("����"));
	m_wndMaxColor.SetColor (m_clrMaxColor);
	m_wndMaxColor.SetColumnsNumber (10);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
