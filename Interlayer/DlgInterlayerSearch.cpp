// DlgInterlayerSearch.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Interlayer.h"
#include "DlgInterlayerSearch.h"


// DlgInterlayerSearch �Ի���

IMPLEMENT_DYNAMIC(DlgInterlayerSearch, CDialog)

DlgInterlayerSearch::DlgInterlayerSearch(CWnd* pParent /*=NULL*/)
	: CDialog(DlgInterlayerSearch::IDD, pParent)
{

}

DlgInterlayerSearch::~DlgInterlayerSearch()
{
}

void DlgInterlayerSearch::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgInterlayerSearch, CDialog)
	ON_BN_CLICKED(ID_DLG_INTERSECT_OK, &DlgInterlayerSearch::OnBnClickedDlgIntersectOk)
END_MESSAGE_MAP()


// DlgInterlayerSearch ��Ϣ�������
void DlgInterlayerSearch::OnBnClickedDlgIntersectOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}
