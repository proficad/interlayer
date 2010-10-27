// DlgInterlayerParaRange.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Interlayer.h"
#include "DlgInterlayerParaRange.h"


// CDlgInterlayerParaRange �Ի���

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


// CDlgInterlayerParaRange ��Ϣ�������

void CDlgInterlayerParaRange::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnOK();
}
