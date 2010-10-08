// DlgInterlayerSearch.cpp : 实现文件
//

#include "stdafx.h"
#include "Interlayer.h"
#include "DlgInterlayerSearch.h"


// DlgInterlayerSearch 对话框

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


// DlgInterlayerSearch 消息处理程序
void DlgInterlayerSearch::OnBnClickedDlgIntersectOk()
{
	// TODO: 在此添加控件通知处理程序代码
}
