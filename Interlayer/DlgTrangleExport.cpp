// DlgTrangleExport.cpp : 实现文件
//

#include "stdafx.h"
#include "Interlayer.h"
#include "DlgTrangleExport.h"
#include "DlgImportModel.h"

// CDlgTrangleExport 对话框

IMPLEMENT_DYNAMIC(CDlgTrangleExport, CDialog)

CDlgTrangleExport::CDlgTrangleExport(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTrangleExport::IDD, pParent)
	, m_x(0)
	, m_y(0)
	, m_z(0)
	, m_strFileName(_T(""))
	, m_index(0)
{

}

CDlgTrangleExport::~CDlgTrangleExport()
{
}

void CDlgTrangleExport::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_x);
	DDV_MinMaxDouble(pDX, m_x, 0, 100000);
	DDX_Text(pDX, IDC_EDIT2, m_y);
	DDV_MinMaxDouble(pDX, m_y, 0, 100000);
	DDX_Text(pDX, IDC_EDIT3, m_z);
	DDV_MinMaxDouble(pDX, m_z, 0, 100000);
	DDX_Text(pDX, IDC_EDIT4, m_strFileName);
	DDX_Control(pDX, IDC_EDIT4, m_filenameEditCtr);
	DDX_Text(pDX, IDC_EDIT5, m_index);
}


BEGIN_MESSAGE_MAP(CDlgTrangleExport, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgTrangleExport::OnBnClickedButton1)
END_MESSAGE_MAP()


// CDlgTrangleExport 消息处理程序

void CDlgTrangleExport::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	//CDlgImportModel dlg(FALSE); // 导出模型及场数据

	CString strFilter = ".tri";//GetFileTypes(FALSE);

	CString filename = "";//pDoc->GetTitle();

	CString ext = "";

	if(filename.IsEmpty())
		filename = "default.tri";

	CFileDialog dlg(false, _T("tri"), filename, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT);

	dlg.m_ofn.lpstrFilter = strFilter;

	if(dlg.DoModal() == IDOK)
	{
		ext = dlg.GetFileExt();
		filename = dlg.GetPathName();
		m_strFileName = filename;
		m_filenameEditCtr.SetWindowText(filename);
	}
	//dlg.m_ofn.lpstrTitle = _T("导出模型数据");
	//m_x = 100;
	//m_y = 200;
	//m_z = 300;

	//if( dlg.DoModal() == IDOK )
	//{
	//	CString pathName = dlg.GetPathName();
	//	m_strFileName = dlg.GetFileName();
	//}
}
