// DlgInterlayerParaRange.cpp : 实现文件
//

#include "stdafx.h"
#include "Interlayer.h"
#include "DlgInterlayerParaRange.h"


// CDlgInterlayerParaRange 对话框

IMPLEMENT_DYNAMIC(CDlgInterlayerParaRange, CDialog)

CDlgInterlayerParaRange::CDlgInterlayerParaRange(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInterlayerParaRange::IDD, pParent)
	, m_rangeMin(0)
	, m_rangeMax(0)
{

}

CDlgInterlayerParaRange::~CDlgInterlayerParaRange()
{
}

void CDlgInterlayerParaRange::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_rangeMin);
	DDX_Text(pDX, IDC_EDIT2, m_rangeMax);
}


BEGIN_MESSAGE_MAP(CDlgInterlayerParaRange, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgInterlayerParaRange::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgInterlayerParaRange 消息处理程序

void CDlgInterlayerParaRange::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}
