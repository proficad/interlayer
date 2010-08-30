// DlgLayerSet.cpp : 实现文件
//

#include "stdafx.h"
#include "Interlayer.h"
#include "DlgLayerSet.h"


// CDlgLayerSet 对话框

IMPLEMENT_DYNAMIC(CDlgLayerSet, CDialog)

CDlgLayerSet::CDlgLayerSet(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLayerSet::IDD, pParent)
	, m_iType(0)
	, m_strLayerName(_T(""))
	, m_bShow(FALSE)
	, m_bLocked(FALSE)
	, m_bClipping(FALSE)
{

}

CDlgLayerSet::~CDlgLayerSet()
{
}

void CDlgLayerSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_COMBO_TYPE, m_iType);
	DDX_Text(pDX, IDC_EDIT_NAME, m_strLayerName);
	DDX_Check(pDX, IDC_CHECK_SHOW, m_bShow);
	DDX_Check(pDX, IDC_CHECK_LOCK, m_bLocked);
	DDX_Check(pDX, IDC_CHECK_CLIP, m_bClipping);
}


BEGIN_MESSAGE_MAP(CDlgLayerSet, CDialog)
END_MESSAGE_MAP()


// CDlgLayerSet 消息处理程序
