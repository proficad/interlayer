// DlgAddCtrlPoint.cpp : 实现文件
//

#include "stdafx.h"
#include "Interlayer.h"
#include "DlgAddCtrlPoint.h"
#include "MainFrm.h"

// CDlgAddCtrlPoint 对话框

IMPLEMENT_DYNAMIC(CDlgAddCtrlPoint, CDialog)

CDlgAddCtrlPoint::CDlgAddCtrlPoint(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAddCtrlPoint::IDD, pParent)
	, m_x(0)
	, m_y(0)
	, m_strWellName(_T(""))
{

}

CDlgAddCtrlPoint::~CDlgAddCtrlPoint()
{
}

void CDlgAddCtrlPoint::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_X, m_x);
	DDX_Text(pDX, IDC_EDIT_Y, m_y);
	DDX_Control(pDX, IDC_COMBO_WELLNAME, m_comboWellName);
	DDX_CBString(pDX, IDC_COMBO_WELLNAME, m_strWellName);
}


BEGIN_MESSAGE_MAP(CDlgAddCtrlPoint, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgAddCtrlPoint::OnBnClickedOk)
	ON_CBN_SELENDOK(IDC_COMBO_WELLNAME, &CDlgAddCtrlPoint::OnCbnSelendokComboWellname)
END_MESSAGE_MAP()


// CDlgAddCtrlPoint 消息处理程序

BOOL CDlgAddCtrlPoint::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();

	HTREEITEM hRoot = pMF->GetTreeFileView()->GetTreeCtrl()->GetChildNode(TVI_ROOT, _T("单井数据"));
	HTREEITEM hItem = pMF->GetTreeFileView()->GetTreeCtrl()->GetChildNode(hRoot, _T("井基础数据"));

	HTREEITEM hWellItem = pMF->GetTreeFileView()->GetTreeCtrl()->GetChildItem(hItem);
	while( hWellItem != NULL )
	{
		CString strWellName = pMF->GetTreeFileView()->GetTreeCtrl()->GetItemText(hWellItem);
		m_comboWellName.AddString(strWellName);

		hWellItem = pMF->GetTreeFileView()->GetTreeCtrl()->GetNextSiblingItem(hWellItem);
	}	

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgAddCtrlPoint::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();

	HTREEITEM hRoot = pMF->GetTreeFileView()->GetTreeCtrl()->GetChildNode(TVI_ROOT, _T("单井数据"));
	HTREEITEM hItem = pMF->GetTreeFileView()->GetTreeCtrl()->GetChildNode(hRoot, _T("井基础数据"));

	m_strWellName.Trim();

	HTREEITEM hWellItem = pMF->GetTreeFileView()->GetTreeCtrl()->GetChildNode(hItem, m_strWellName, FALSE);

	if( hWellItem != NULL )
	{
		CTreeNodeDat *pNodeDat = (CTreeNodeDat *)pMF->GetTreeFileView()->GetTreeCtrl()->GetItemData(hWellItem);
		m_x = ((CWellInfo*)pNodeDat->m_pNodeDat)->m_x;
		m_y = ((CWellInfo*)pNodeDat->m_pNodeDat)->m_y;
	}
	else
	{
		m_x = 0.0;
		m_y = 0.0;
	}
	OnOK();
}

void CDlgAddCtrlPoint::OnCbnSelendokComboWellname()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();

	HTREEITEM hRoot = pMF->GetTreeFileView()->GetTreeCtrl()->GetChildNode(TVI_ROOT, _T("单井数据"));
	HTREEITEM hItem = pMF->GetTreeFileView()->GetTreeCtrl()->GetChildNode(hRoot, _T("井基础数据"));

	m_strWellName.Trim();

	HTREEITEM hWellItem = pMF->GetTreeFileView()->GetTreeCtrl()->GetChildNode(hItem, m_strWellName, FALSE);

	if( hWellItem != NULL )
	{
		CTreeNodeDat *pNodeDat = (CTreeNodeDat *)pMF->GetTreeFileView()->GetTreeCtrl()->GetItemData(hWellItem);
		m_x = ((CWellInfo*)pNodeDat->m_pNodeDat)->m_x;
		m_y = ((CWellInfo*)pNodeDat->m_pNodeDat)->m_y;		
	}
	else
	{
		m_x = 0.0;
		m_y = 0.0;
	}

	UpdateData(FALSE);
}
