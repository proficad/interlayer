// DlgExportGridData.cpp : 实现文件
//

#include "stdafx.h"
#include "Interlayer.h"
#include "DlgExportGridData.h"
#include "MainFrm.h"
#include "IntersectSearchManager.h"
// CDlgExportGridData 对话框

IMPLEMENT_DYNAMIC(CDlgExportGridData, CDialog)

CDlgExportGridData::CDlgExportGridData(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgExportGridData::IDD, pParent)
	, m_strFilePath(_T(""))
	, m_changeValue(0)
{
	m_bCoordinate = true;
	m__bRedefine = m_bTransRate = m_bRedefineTransRate =
		m_bCenterPoint = m_bProperty = m_bRedefineProperty = m_bChangeProperty = m_bRedefineChangeProperty = false;
	m_dChangePropertyValue = 0.0;
	m_csChangePropertyName = "";
}

CDlgExportGridData::~CDlgExportGridData()
{
}

void CDlgExportGridData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strFilePath);
	DDX_Control(pDX, IDC_TREE1, m_ctrTree);
	DDX_Control(pDX, IDC_EDIT1, m_filenameEditCtr);
	DDX_Control(pDX, IDC_COMBO1, m_Ctr_pro_comb);
	DDX_Control(pDX, IDC_CHECK5, m_ctr_check5);
	DDX_Text(pDX, IDC_EDIT2, m_changeValue);
}


BEGIN_MESSAGE_MAP(CDlgExportGridData, CDialog)
	ON_BN_CLICKED(IDC_CHECK1, &CDlgExportGridData::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_CHECK2, &CDlgExportGridData::OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_CHECK3, &CDlgExportGridData::OnBnClickedCheck3)
	ON_BN_CLICKED(IDC_CHECK4, &CDlgExportGridData::OnBnClickedCheck4)
	ON_BN_CLICKED(IDC_CHECK6, &CDlgExportGridData::OnBnClickedCheck6)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, &CDlgExportGridData::OnNMClickTree1)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgExportGridData::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_CHECK5, &CDlgExportGridData::OnBnClickedCheck5)
	ON_BN_CLICKED(IDOK, &CDlgExportGridData::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK7, &CDlgExportGridData::OnBnClickedCheck7)
	ON_BN_CLICKED(IDC_RADIO1, &CDlgExportGridData::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CDlgExportGridData::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &CDlgExportGridData::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO4, &CDlgExportGridData::OnBnClickedRadio4)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CDlgExportGridData::OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


// CDlgExportGridData 消息处理程序

void CDlgExportGridData::OnBnClickedCheck1()
{
	// TODO: 在此添加控件通知处理程序代码
	if ( BST_CHECKED == IsDlgButtonChecked( IDC_CHECK1 ) )
	{
		// 勾选
		m_bCoordinate = true;
	}
	else
	{
		m_bCoordinate = false;
	}
}

void CDlgExportGridData::OnBnClickedCheck2()
{
	// TODO: 在此添加控件通知处理程序代码
	if ( BST_CHECKED == IsDlgButtonChecked( IDC_CHECK2 ) )
	{
		// 勾选
		m__bRedefine = true;
		if(BST_CHECKED == IsDlgButtonChecked(IDC_CHECK6))
			GetDlgItem(IDC_CHECK5)->EnableWindow(TRUE);
		else
			GetDlgItem(IDC_CHECK5)->EnableWindow(FALSE);
		if(BST_CHECKED == IsDlgButtonChecked(IDC_CHECK7))
			GetDlgItem(IDC_RADIO2)->EnableWindow(TRUE);
		else
			GetDlgItem(IDC_RADIO2)->EnableWindow(FALSE);
		if(BST_CHECKED == IsDlgButtonChecked(IDC_CHECK3))
			GetDlgItem(IDC_RADIO4)->EnableWindow(TRUE);
		else
			GetDlgItem(IDC_RADIO4)->EnableWindow(FALSE);
	}
	else
	{
		m__bRedefine = false;
		GetDlgItem(IDC_RADIO2)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO4)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK5)->EnableWindow(FALSE);
	}
}

void CDlgExportGridData::OnBnClickedCheck3()
{
	// TODO: 在此添加控件通知处理程序代码
	if ( BST_CHECKED == IsDlgButtonChecked( IDC_CHECK3 ) )
	{
		// 勾选
		m_bTransRate = true;
		GetDlgItem(IDC_RADIO3)->EnableWindow(TRUE);
		if(BST_CHECKED == IsDlgButtonChecked(IDC_CHECK2 ))
			GetDlgItem(IDC_RADIO4)->EnableWindow(TRUE);
		else
			GetDlgItem(IDC_RADIO4)->EnableWindow(FALSE);
	}
	else
	{
		m_bTransRate = false;
		GetDlgItem(IDC_RADIO3)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO4)->EnableWindow(FALSE);
	}
}

void CDlgExportGridData::OnBnClickedCheck4()
{
	// TODO: 在此添加控件通知处理程序代码
	if ( BST_CHECKED == IsDlgButtonChecked( IDC_CHECK4 ) )
	{
		// 勾选
		m_bCenterPoint = true;
	}
	else
	{
		m_bCenterPoint = false;
	}
}

void CDlgExportGridData::OnBnClickedCheck5()
{
	// TODO: 在此添加控件通知处理程序代码
	// TODO: 在此添加控件通知处理程序代码
	if ( BST_CHECKED == IsDlgButtonChecked( IDC_CHECK5 ) )
	{
		// 勾选
		m_bRedefineProperty = true;
	}
	else
	{
		m_bRedefineProperty = false;
	}
}

void CDlgExportGridData::OnBnClickedCheck6()
{
	// TODO: 在此添加控件通知处理程序代码
	if ( BST_CHECKED == IsDlgButtonChecked( IDC_CHECK6 ) )
	{
		// 勾选
		m_bProperty = true;
		m_ctrTree.EnableWindow(TRUE);
		if(BST_CHECKED == IsDlgButtonChecked( IDC_CHECK2))
			m_ctr_check5.EnableWindow(TRUE);
		else
			m_ctr_check5.EnableWindow(FALSE);
	}
	else
	{
		m_bProperty = false;
		m_ctrTree.EnableWindow(FALSE);
		m_ctr_check5.EnableWindow(FALSE);
	}
}

BOOL CDlgExportGridData::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_ctrTree.EnableWindow(FALSE);
	m_bCoordinate = TRUE;
	((CButton *)GetDlgItem(IDC_CHECK1))->SetCheck(TRUE);
	((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
	((CButton *)GetDlgItem(IDC_RADIO3))->SetCheck(TRUE);
	((CButton *)GetDlgItem(IDC_CHECK1))->EnableWindow(FALSE);

	FillProperty();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

bool CDlgExportGridData::FillProperty()
{
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	CTreeCtrl* pTree = pMF->GetTreeModelView()->GetTreeCtrl();
	CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)(pTree->GetItemData)(m_ModelItem);

	CString strSourcePathName = pMF->GetProjectDatPath();
	CString strFileName = ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;
	strSourcePathName += _T("\\models\\");
	strSourcePathName += strFileName;
	CExportManager::Instance()->SetExportGridFileName(strSourcePathName.GetBuffer());

	HTREEITEM son = pTree->GetChildItem(m_ModelItem);
	if(son==NULL)
		return false;
	int index = 0;
	do 
	{
		CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)(pTree->GetItemData)(son);
		if(lpNodeDat->m_nType==FARM_DAT)
		{
			CString strFileName2 = ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;
			HTREEITEM hTreeItem = m_ctrTree.InsertItem(pTree->GetItemText(son));
			m_ctrTree.SetItemData(hTreeItem, (DWORD)(this->m_strProperty.size()));
			m_ctrTree.SetCheck(hTreeItem, 1);
			m_ctrTree.Expand(hTreeItem, TVE_EXPAND);

			m_strProperty.push_back(strFileName2);
			m_bPropertyExport.push_back(FALSE);

			m_Ctr_pro_comb.InsertString(index++, pTree->GetItemText(son));
		}
		else if(lpNodeDat->m_nType==GRID_LAYER)
		{
			CString strFileName3 = ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;
			m_strGridLayer.push_back(strFileName3);
		}
		son = pTree->GetNextSiblingItem(son);
	} while (son!=NULL);
	if(m_strProperty.size()>0)
		m_Ctr_pro_comb.SetCurSel(0);
	GetDlgItem(IDC_EDIT2)->SetWindowText("0.0");
	return true;
}

void CDlgExportGridData::SetModelItem( HTREEITEM model )
{
	m_ModelItem = model;
}

void CDlgExportGridData::OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	CPoint  point; 
	GetCursorPos(&point); 
	m_ctrTree.ScreenToClient(&point); 
	UINT  flag =  TVHT_ONITEM; 
	HTREEITEM  hItem  =  m_ctrTree.HitTest(point,&flag);
	if (hItem != NULL)
	{
		CString m_sText;
		m_ctrTree.SelectItem(hItem);
		bool check = m_ctrTree.GetCheck(hItem);
		if(!(flag & TVHT_ONITEMSTATEICON))
		{
			check = !check;
		}
		m_ctrTree.SetCheck(hItem, check);
		m_sText  =  m_ctrTree.GetItemText(hItem);
		DWORD index = (DWORD)m_ctrTree.GetItemData(hItem);
		if(flag & TVHT_ONITEMSTATEICON)
			check = !check;
		m_bPropertyExport[index] = check;
		//if(check)
		//	AfxMessageBox(m_strProperty[index]);
	}
}

void CDlgExportGridData::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码

	CString strFilter = "";//GetFileTypes(FALSE);

	CString filename = "";//pDoc->GetTitle();

	CString ext = "";

	if(filename.IsEmpty())
		filename = "default";

	CFileDialog dlg(false, _T(""), filename, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT);

	dlg.m_ofn.lpstrFilter = strFilter;


	if(dlg.DoModal() == IDOK)
	{
		ext = dlg.GetFileExt();
		filename = dlg.GetPathName();
		m_strFilePath = filename;
		m_filenameEditCtr.SetWindowText(filename);
	}
}

void CDlgExportGridData::SaveExport()
{
	CExportManager::Instance()->ClearProNames();
	CExportManager::Instance()->ClearTrackNames();

	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	CString strFileName = pMF->GetProjectDatPath();
	strFileName += _T("\\models\\");

	CString outfile = m_strFilePath;
	if(m_bCoordinate)
	{
		CExportManager::Instance()->SetWriteProperty(m_bProperty);
		CExportManager::Instance()->SetRedefine(m__bRedefine);
		CExportManager::Instance()->SetRedefineProperty(m_bProperty);
		CExportManager::Instance()->SetTransRate(m_bTransRate);

		CExportManager::Instance()->SetWriteChangeProperty(m_bChangeProperty);
		CExportManager::Instance()->SetWriteRedefineChangeProperty(m_bRedefineChangeProperty);
		CExportManager::Instance()->SetWriteRedefineMult(m_bRedefineTransRate);

		if(m_bChangeProperty)
		{
			CExportManager::Instance()->SetChangePropertyValue(m_changeValue);
			CExportManager::Instance()->SetChangePropertyName(m_csChangePropertyName.GetBuffer());
		}

		if(m_bProperty)
		{
			for(int i=0; i<m_strProperty.size(); i++)
			{
				if(m_bPropertyExport[i])
					CExportManager::Instance()->AddProName(m_strProperty[i].GetBuffer());
			}
		}

		for (int i=0; i<m_strGridLayer.size(); i++)
		{
			CString tmp = strFileName + m_strGridLayer[i].GetBuffer();
			CExportManager::Instance()->AddTrackName(tmp.GetBuffer());
		}

		CExportManager::Instance()->WriteExport(outfile.GetBuffer());
	}
	if(m_bCenterPoint)
	{
		for (int i=0; i<m_strGridLayer.size(); i++)
		{
			CString newFileName;
			newFileName.Format("%d",i);
			newFileName = outfile + newFileName;

			CString tmpold = strFileName + m_strGridLayer[i].GetBuffer();

			CExportManager::Instance()->WriteCenterPoints(tmpold.GetBuffer(), newFileName.GetBuffer(), m__bRedefine, i);
		}
	}
	CExportManager::Instance()->ClearProNames();
	CExportManager::Instance()->ClearTrackNames();
}


void CDlgExportGridData::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	OnOK();
}

void CDlgExportGridData::OnBnClickedCheck7()
{
	// TODO: 在此添加控件通知处理程序代码
	if ( BST_CHECKED == IsDlgButtonChecked( IDC_CHECK7 ) )
	{
		GetDlgItem(IDC_COMBO1)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO1)->EnableWindow(TRUE);
		if(BST_CHECKED==IsDlgButtonChecked(IDC_CHECK2))
			GetDlgItem(IDC_RADIO2)->EnableWindow(TRUE);
		else
			GetDlgItem(IDC_RADIO2)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT2)->EnableWindow(TRUE);

		int i = ((CComboBox*)GetDlgItem(IDC_COMBO1))->GetCurSel();
		m_csChangePropertyName = m_strProperty[i];

		m_bChangeProperty = true;
	}
	else
	{
		GetDlgItem(IDC_COMBO1)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO1)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO2)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT2)->EnableWindow(FALSE);

		m_bChangeProperty = false;
	}
}

void CDlgExportGridData::OnBnClickedRadio1()
{
	// TODO: 在此添加控件通知处理程序代码
	m_bRedefineChangeProperty = false;
}

void CDlgExportGridData::OnBnClickedRadio2()
{
	// TODO: 在此添加控件通知处理程序代码
	m_bRedefineChangeProperty = true;
}

void CDlgExportGridData::OnBnClickedRadio3()
{
	// TODO: 在此添加控件通知处理程序代码
	m_bRedefineTransRate = false;
}

void CDlgExportGridData::OnBnClickedRadio4()
{
	// TODO: 在此添加控件通知处理程序代码
	m_bRedefineTransRate = true;
}

void CDlgExportGridData::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
	int i = ((CComboBox*)GetDlgItem(IDC_COMBO1))->GetCurSel();
	m_csChangePropertyName = m_strProperty[i];
}
