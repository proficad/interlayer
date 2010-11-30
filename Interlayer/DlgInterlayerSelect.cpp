// DlgInterlayerSelect.cpp : 实现文件
//

#include "stdafx.h"
#include "Interlayer.h"
#include "DlgInterlayerSelect.h"
#include "MainFrm.h"
#include "IntersectSearchManager.h"
#include "3DLib/3DObject.h"
#include "3DLib/PlaneReader.h"
//#include "GridObject.h"
// CDlgInterlayerSelect 对话框

IMPLEMENT_DYNAMIC(CDlgInterlayerSelect, CDialog)

CDlgInterlayerSelect::CDlgInterlayerSelect(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInterlayerSelect::IDD, pParent),m_uiNextPointBarIndex(0)
{

}

CDlgInterlayerSelect::~CDlgInterlayerSelect()
{
}

void CDlgInterlayerSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_ctrFileTree);
	DDX_Control(pDX, IDC_TREE2, m_ctrSearchTree);
	DDX_Control(pDX, IDC_EDIT_GRIDMODEL_NAME, m_ctrGridModel);
}


BEGIN_MESSAGE_MAP(CDlgInterlayerSelect, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADDPOINTBAR, &CDlgInterlayerSelect::OnBnClickedButtonAddpointbar)
	ON_BN_CLICKED(IDC_BUTTON_ADDINTERLAYERS, &CDlgInterlayerSelect::OnBnClickedButtonAddinterlayers)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &CDlgInterlayerSelect::OnBnClickedButtonDel)
	ON_BN_CLICKED(IDOK, &CDlgInterlayerSelect::OnBnClickedOk)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, &CDlgInterlayerSelect::OnNMClickFileTree)
	ON_NOTIFY(NM_CLICK, IDC_TREE2, &CDlgInterlayerSelect::OnNMClickSearchTree)
END_MESSAGE_MAP()


// CDlgInterlayerSelect 消息处理程序

void CDlgInterlayerSelect::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialog::OnOK();
	DestroyWindow();
}

void CDlgInterlayerSelect::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialog::OnCancel();
	DestroyWindow();
}

BOOL CDlgInterlayerSelect::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类

	return CDialog::DestroyWindow();
	delete this;
}

void CDlgInterlayerSelect::InitTree()
{
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	CFileView* fileview = pMF->GetTreeFileView();
	if(fileview)
	{
		CViewTree *filetree = fileview->GetTreeCtrl();
		if(filetree==NULL)
		{
			AfxMessageBox("文件树为空");
		}
		else
		{
			HTREEITEM   hItemhi   =   filetree->GetRootItem();
			
			if   (hItemhi   !=   NULL)
			{
				while(hItemhi!=NULL)
				{
					HTREEITEM parentDest ;

					parentDest = m_ctrFileTree.InsertItem(filetree->GetItemText(hItemhi) );
					CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)filetree->GetItemData(hItemhi);
					m_ctrFileTree.SetItemData(parentDest, (DWORD)lpNodeDat);

					AddChildItems(filetree, hItemhi, parentDest);
					hItemhi = filetree->GetNextSiblingItem(hItemhi);
				}
			}
		}

		//添加一个点坝
		HTREEITEM pointbar = m_ctrSearchTree.InsertItem(_T("点坝1"));
		
		m_uiNextPointBarIndex = 2;
	}
}

void CDlgInterlayerSelect::AddChildItems(CViewTree *srcTree,  HTREEITEM src, HTREEITEM dest )
{
	HTREEITEM point = src;

	HTREEITEM son = srcTree->GetChildItem( point);

	while(son) 
	{
		HTREEITEM destParent = m_ctrFileTree.InsertItem(srcTree->GetItemText(son), dest);
		CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)srcTree->GetItemData(son);
		m_ctrFileTree.SetItemData(destParent, (DWORD)lpNodeDat);
		AddChildItems(srcTree, son, destParent);
		son = srcTree->GetNextSiblingItem(son);
	} 
}
void CDlgInterlayerSelect::OnBnClickedButtonAddpointbar()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strIndex;
	strIndex.Format("%d",m_uiNextPointBarIndex);
	CString strPointbar = "点坝" + strIndex  ;
	HTREEITEM pointbar= m_ctrSearchTree.InsertItem(strPointbar);

	m_uiNextPointBarIndex++;
}

void CDlgInterlayerSelect::OnBnClickedButtonAddinterlayers()
{
	// TODO: 在此添加控件通知处理程序代码
	HTREEITEM pointbar = m_ctrSearchTree.GetSelectedItem();
	if(!pointbar)
	{
		AfxMessageBox("请选择点坝");
		return;
	}
	DWORD lpData = m_ctrSearchTree.GetItemData(pointbar);
	while(lpData)
	{
		pointbar = m_ctrSearchTree.GetParentItem(pointbar);
		lpData = m_ctrSearchTree.GetItemData(pointbar);
	}
	if(pointbar)
	{
		AddSearchCheckedItem(pointbar);
		m_ctrSearchTree.Expand(pointbar, TVIS_EXPANDED); 
		m_ctrSearchTree.RedrawWindow();
		//m_ctrSearchTree.SetItemState(pointbar,  TVIS_BOLD, TVIS_BOLD);	
	}
}

void CDlgInterlayerSelect::OnBnClickedButtonDel()
{
	// TODO: 在此添加控件通知处理程序代码
	HTREEITEM hItem = m_ctrSearchTree.GetSelectedItem();
	DeleteAllChildren(hItem);
}

void CDlgInterlayerSelect::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	ShowWindow(SW_HIDE);
	SearchInterlayerGrid();
	OnOK();
}

void CDlgInterlayerSelect::SearchInterlayerGrid()
{
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();

	CString strFileName = pMF->GetProjectDatPath();
	strFileName += _T("\\models\\");
	CIntersectSearchManager::Instance()->SetGridModelName(m_strGridModelName.GetBuffer(), m_strGuid.GetBuffer(), strFileName.GetBuffer());
	HTREEITEM pointbar = m_ctrSearchTree.GetRootItem();

	while(pointbar)
	{
		HTREEITEM son = m_ctrSearchTree.GetChildItem(pointbar);
		while(son)
		{
			CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)m_ctrFileTree.GetItemData(son);
			CString strSourcePathName = pMF->GetProjectDatPath();
			CString strFileName = ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;
			strSourcePathName += _T("\\files\\");
			strSourcePathName += strFileName;
			C3DObject* pPlaneObj;
			if(lpNodeDat->m_strFileName2.IsEmpty())
			{
				CPlaneReader* pReader = new CPlaneReader(strSourcePathName);
				pPlaneObj = new C3DObject(pReader);
				//pPlaneObj->m_strGUID = guid;

				delete pReader;
			}
			else
			{
				pPlaneObj = new C3DObject();
				//pPlaneObj->m_strGUID = guid;
				pPlaneObj->m_first = false;
				CString strFileName1 = pMF->GetProjectDatPath();
				strFileName1 += _T("\\models\\");
				strFileName1 += lpNodeDat->m_strFileName2;
				CFile file(strFileName1.GetBuffer(), CFile::modeRead|CFile::typeBinary);
				CArchive art(&file, CArchive::load);
				//pPlaneObj->SetGLObjType(GLPLANE);
				//pPlaneObj->SetContext(GetContext());
				pPlaneObj->Serialize(art);
				//pPlaneObj->ReversePoints();
				art.Close();
				file.Close();
			}
			CString strText = m_ctrSearchTree.GetItemText(son);
			
			pPlaneObj->SetGLObjType(GLPLANE);
			pPlaneObj->SetObjName(strSourcePathName);

			CIntersectSearchManager::Instance()->AddLayerModel(pPlaneObj);
			
			son = m_ctrSearchTree.GetNextSiblingItem(son);
		}
		//AfxBeginThread(MyThreadProc, pMF->GetTreeModelView());
		CIntersectSearchManager::Instance()->SearchInterSect(pMF->GetTreeModelView());

		pointbar = m_ctrSearchTree.GetNextSiblingItem(pointbar);
	}
}
void CDlgInterlayerSelect::OnNMClickFileTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	CPoint  point; 
	GetCursorPos(&point); 
	m_ctrFileTree.ScreenToClient(&point); 
	UINT  flag =  TVHT_ONITEM; 
	HTREEITEM  hItem  =  m_ctrFileTree.HitTest(point,&flag);
	if (hItem != NULL)
	{
		CString m_sText;
		m_ctrFileTree.SelectItem(hItem);

		if(flag & TVHT_ONITEMSTATEICON)
		{
			bool check = m_ctrFileTree.GetCheck(hItem);
			SetAllChildrenCheck(&m_ctrFileTree, hItem, !check);
		}
	}
}

void CDlgInterlayerSelect::OnNMClickSearchTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	CPoint  point; 
	GetCursorPos(&point); 
	m_ctrSearchTree.ScreenToClient(&point); 
	UINT  flag =  TVHT_ONITEM; 
	HTREEITEM  hItem  =  m_ctrSearchTree.HitTest(point,&flag);
	if (hItem != NULL)
	{
		CString m_sText;
		m_ctrSearchTree.SelectItem(hItem);

		//if(flag & TVHT_ONITEMSTATEICON)
		//{
		//	bool check = m_ctrSearchTree.GetCheck(hItem);
		//	SetAllChildrenCheck(&m_ctrSearchTree, hItem, !check);
		//}
	}
}

void CDlgInterlayerSelect::DeleteAllChildren( HTREEITEM hItem )
{
	//if(!m_ctrSearchTree.ItemHasChildren(hItem))
	//	return ;

	HTREEITEM hChild = m_ctrSearchTree.GetChildItem(hItem);   
	while(hChild != NULL)   
	{   
		HTREEITEM hSiblingItem =m_ctrSearchTree.GetNextSiblingItem(hChild);

		if(m_ctrSearchTree.ItemHasChildren(hChild))
			DeleteAllChildren(hChild);
		else
		{
			m_ctrSearchTree.DeleteItem(hChild);
		}

		hChild = hSiblingItem;
	}

	m_ctrSearchTree.DeleteItem(hItem);
}

void CDlgInterlayerSelect::SetAllChildrenCheck( CTreeCtrl* tree, HTREEITEM hItem, bool check )
{
	HTREEITEM hChild = tree->GetChildItem(hItem);   
	while(hChild != NULL)   
	{   
		HTREEITEM hSiblingItem =tree->GetNextSiblingItem(hChild);

		if(tree->ItemHasChildren(hChild))
			SetAllChildrenCheck(tree, hChild, check);
		
		tree->SetCheck(hChild, check);

		hChild = hSiblingItem;
	}
}

void CDlgInterlayerSelect::AddSearchCheckedItem(HTREEITEM dest)
{
	HTREEITEM   hItemhi   =   m_ctrFileTree.GetRootItem();

	if   (hItemhi   !=   NULL)
	{
		while(hItemhi!=NULL)
		{
			HTREEITEM newItem ;
			CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)m_ctrFileTree.GetItemData(hItemhi);
			if(m_ctrFileTree.GetCheck(hItemhi)&&lpNodeDat->m_nType==FILE_PLANE)
			{
				newItem = m_ctrFileTree.InsertItem(m_ctrFileTree.GetItemText(hItemhi), dest );
				m_ctrFileTree.SetItemData(newItem, (DWORD)lpNodeDat);
			}

			AddSearchCheckedChildren( dest,  hItemhi);
			hItemhi = m_ctrFileTree.GetNextSiblingItem(hItemhi);
		}
	}
}

void CDlgInterlayerSelect::AddSearchCheckedChildren( HTREEITEM dest, HTREEITEM hItem )
{
	HTREEITEM point = hItem;

	HTREEITEM son = m_ctrFileTree.GetChildItem( point);

	while(son) 
	{
		CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)m_ctrFileTree.GetItemData(son);
		if(m_ctrFileTree.GetCheck(son)&&lpNodeDat->m_nType==FILE_PLANE)
		{
			HTREEITEM destParent = m_ctrFileTree.InsertItem(m_ctrFileTree.GetItemText(son), dest);
			m_ctrFileTree.SetItemData(destParent, (DWORD)lpNodeDat);
		}

		AddSearchCheckedChildren(dest, son);
		son = m_ctrFileTree.GetNextSiblingItem(son);
	} 
}