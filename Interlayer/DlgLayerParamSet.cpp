// DlgLayerParamSet.cpp : 实现文件
//

#include "stdafx.h"
#include "Interlayer.h"
#include "DlgLayerParamSet.h"


// CDlgLayerParamSet 对话框

IMPLEMENT_DYNAMIC(CDlgLayerParamSet, CDialogEx)

CDlgLayerParamSet::CDlgLayerParamSet(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgLayerParamSet::IDD, pParent)
	, m_dLayers(0.5)
	, m_dNormals(0.5)
	, m_dVerts(0.5)
{

}

CDlgLayerParamSet::~CDlgLayerParamSet()
{
}

void CDlgLayerParamSet::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_LAYERS, m_dLayers);
	DDV_MinMaxDouble(pDX, m_dLayers, 0.1, 10000);
	DDX_Text(pDX, IDC_EDIT_NORMALS, m_dNormals);
	DDV_MinMaxDouble(pDX, m_dNormals, 0.1, 10000);
	DDX_Text(pDX, IDC_EDIT_VERTS, m_dVerts);
	DDV_MinMaxDouble(pDX, m_dVerts, 0.1, 10000);
}


BEGIN_MESSAGE_MAP(CDlgLayerParamSet, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgLayerParamSet::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgLayerParamSet 消息处理程序

void CDlgLayerParamSet::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();

	CString strTmp;
	strTmp.Format(_T("%g"), m_dLayers);
	AfxGetApp()->WriteProfileString(_T("Settings"),_T("Layers"),strTmp);

	strTmp.Format(_T("%g"), m_dNormals);
	AfxGetApp()->WriteProfileString(_T("Settings"),_T("Normals"),strTmp);

	strTmp.Format(_T("%g"), m_dVerts);
	AfxGetApp()->WriteProfileString(_T("Settings"),_T("Verts"),strTmp);

	OnOK();
}

BOOL CDlgLayerParamSet::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	CString strTmp;
	strTmp.Format(_T("%g"), m_dLayers);
	strTmp = AfxGetApp()->GetProfileString(_T("Settings"),_T("Layers"),strTmp);
	m_dLayers = atof(strTmp);

	strTmp.Format(_T("%g"), m_dNormals);
	strTmp = AfxGetApp()->GetProfileString(_T("Settings"),_T("Normals"),strTmp);
	m_dNormals = atof(strTmp);
	
	strTmp.Format(_T("%g"), m_dVerts);
	strTmp = AfxGetApp()->GetProfileString(_T("Settings"),_T("Verts"),strTmp);
	m_dVerts = atof(strTmp);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
