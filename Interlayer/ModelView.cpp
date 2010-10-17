
#include "stdafx.h"
#include "MainFrm.h"
#include "ModelView.h"
#include "Resource.h"
#include "Interlayer.h"
#include "DlgImportModel.h"
#include "MyEditView.h"
#include "GridModel.h"
#include "2DModelDoc.h"
#include "2DModelView.h"
#include <vector>
#include "3DLib/3DModelView.h"
#include "3DLib/3DModelDoc.h"
#include "IntersectSearchManager.h"
using namespace GridModel;

class CModelViewMenuButton : public CMFCToolBarMenuButton
{
	friend class CModelView;

	DECLARE_SERIAL(CModelViewMenuButton)

public:
	CModelViewMenuButton(HMENU hMenu = NULL) : CMFCToolBarMenuButton((UINT)-1, hMenu, -1)
	{
	}

	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE,
		BOOL bCustomizeMode = FALSE, BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE)
	{
		pImages = CMFCToolBar::GetImages();

		CAfxDrawState ds;
		pImages->PrepareDrawImage(ds);

		CMFCToolBarMenuButton::OnDraw(pDC, rect, pImages, bHorz, bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons);

		pImages->EndDrawImage(ds);
	}
};

IMPLEMENT_SERIAL(CModelViewMenuButton, CMFCToolBarMenuButton, 1)

//////////////////////////////////////////////////////////////////////
// 构造/析构
//////////////////////////////////////////////////////////////////////

CModelView::CModelView()
{
	m_wndModelView.m_eTreeType = CViewTree::TREETYPE::modelTree;
	m_nCurrSort = ID_SORTING_GROUPBYTYPE;
}

CModelView::~CModelView()
{
}

BEGIN_MESSAGE_MAP(CModelView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()	
	ON_WM_PAINT()
	ON_WM_SETFOCUS()	
	ON_COMMAND(ID_OPEN, OnFileOpen)	
	ON_UPDATE_COMMAND_UI(ID_OPEN, OnUpdateFileOpen)
	ON_COMMAND(ID_NEW_FOLDER, OnNewFolder)
	ON_COMMAND(ID_ADD_CHILD, &CModelView::OnAddChild)
	ON_UPDATE_COMMAND_UI(ID_ADD_CHILD, &CModelView::OnUpdateAddChild)
	ON_COMMAND(ID_EDIT_RENAME, &CModelView::OnEditRename)
	ON_UPDATE_COMMAND_UI(ID_EDIT_RENAME, &CModelView::OnUpdateEditRename)
	ON_COMMAND(ID_IMPORT_MODEL, &CModelView::OnImportModel)
	ON_UPDATE_COMMAND_UI(ID_IMPORT_MODEL, &CModelView::OnUpdateImportModel)
	ON_COMMAND(ID_IMPORT_FIELD, &CModelView::OnImportField)
	ON_UPDATE_COMMAND_UI(ID_IMPORT_FIELD, &CModelView::OnUpdateImportField)
	ON_COMMAND(IDC_EXPAND_ALL_TREE, &CModelView::OnExpandAllTree)
	ON_COMMAND(ID_COLLAPSE_ALL_ENTIRE_TREE, &CModelView::OnCollapseAllEntireTree)
	ON_COMMAND(ID_EXPORT_MODEL, &CModelView::OnExportModel)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_MODEL, &CModelView::OnUpdateExportModel)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelView 消息处理程序

int CModelView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建视图:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | WS_DISABLED
		| TVS_HASLINES 
		| TVS_LINESATROOT | TVS_HASBUTTONS 
		| WS_CLIPSIBLINGS | WS_CLIPCHILDREN
		| TVS_CHECKBOXES | TVS_TRACKSELECT
		| TVS_EDITLABELS;

	if (!m_wndModelView.Create(dwViewStyle, rectDummy, this, 2))
	{
		TRACE0("未能创建类视图\n");
		return -1;      // 未能创建
	}

	m_wndModelView.pfOpenTree = (CViewTree::OPENTREE_FUNCTION)&CModelView::OpenTree;
	
	// 创建工具条
	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_SORT);
	m_wndToolBar.LoadToolBar(IDR_SORT, 0, 0, TRUE /* 已锁定*/);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// 所有命令将通过此控件路由，而不是通过主框架路由:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	CMenu menuSort;
	menuSort.LoadMenu(IDR_POPUP_SORT);

	m_wndToolBar.ReplaceButton(ID_SORT_MENU, CModelViewMenuButton(menuSort.GetSubMenu(0)->GetSafeHmenu()));

	CModelViewMenuButton* pButton =  DYNAMIC_DOWNCAST(CModelViewMenuButton, m_wndToolBar.GetButton(0));

	if (pButton != NULL)
	{
		pButton->m_bText = FALSE;
		pButton->m_bImage = TRUE;
		pButton->SetImage(GetCmdMgr()->GetCmdImage(m_nCurrSort));
		pButton->SetMessageWnd(this);
	}

	// 填入一些静态树视图数据(此处只需填入虚拟代码，而不是复杂的数据)
	//FillModelView();

	AdjustLayout();

	return 0;
}

bool CModelView::OpenTree(CTreeNodeDat *lpNodeDat, void *lpVoid)
{
	CString strTitle = (LPCSTR)lpVoid;

	switch ( lpNodeDat->m_nType )
	{	
	case GRID_DAT:// 模型
		{	
			CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
			CString strFileName = pMF->GetProjectDatPath();
			strFileName += _T("\\models\\");
			strFileName += ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;
			if( !PathFileExists(strFileName) )
			{
				AfxMessageBox(_T("此文件不存在!"));
			}
			else
			{
				if( pMF->GetTreeGraphView()->GetTreeCtrl()->m_bNew3D ) // 创建3D图形
				{
					CString strPath = pMF->GetProjectDatPath();
					strPath += _T("\\graphics\\");
					CString strNewFileName;
					strNewFileName = newGUID();
					strNewFileName += _T(".3df");
					strPath += strNewFileName;
					CFile file(strPath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
					file.Close();

					HTREEITEM hItem = pMF->GetTreeGraphView()->GetTreeCtrl()->GetChildNode(TVI_ROOT,_T("三维视图"));

					if( hItem == NULL )
						hItem = TVI_ROOT;

					int num = 0;

					CString strTit;
					do 
					{
						if(num>0)
							strTit.Format(_T("3D图形 %d"),num);
						else
							strTit = _T("3D图形");
						num++;
					} while(pMF->GetTreeGraphView()->GetTreeCtrl()->IsExistChildNode(hItem,(LPCSTR)strTit));

					HTREEITEM hSrc = pMF->GetTreeGraphView()->AddTreeItem(strTit, 2, 2, hItem);

					CTreeNodeDat *lpNodeDat = new CTreeNodeDat;
					CFileViewObj *pObj = new CFileViewObj;			
					pObj->m_strFileName = strNewFileName;

					lpNodeDat->m_nImage			= 2;
					lpNodeDat->m_nSelectedImage	= 2;
					lpNodeDat->m_nType			= FILE_3D;
					lpNodeDat->m_pNodeDat		= pObj;
					lpNodeDat->m_uState			= TVIS_EXPANDED;
					strFileName.MakeUpper();
					lpNodeDat->m_strFileName	= strNewFileName;

					pMF->GetTreeGraphView()->GetTreeCtrl()->SetItemData(hSrc, (DWORD)lpNodeDat);

					pMF->GetTreeGraphView()->GetTreeCtrl()->Expand(hItem, TVE_EXPAND);

					C3DModelDoc *pDoc = (C3DModelDoc *)pMF->CreateOrActivateFrame(theApp.m_p3DModelDocTemplate, RUNTIME_CLASS(C3DModelView), strPath);

					if( pDoc != NULL )
					{
						pDoc->AddGridModel(strFileName, strTitle, m_arParamName, m_arFileName);
					}

					pMF->GetTreeGraphView()->GetTreeCtrl()->m_bNew3D = false;
				}
				else
				{
					CDocument *pDoc = pMF->CreateOrActivateFrame(theApp.m_p2DModelDocTemplate, RUNTIME_CLASS(C2DModelView), strFileName);
					if( pDoc ) 
					{
						pDoc->SetPathName(strFileName);
						strTitle += _T(" - 1");
						pDoc->SetTitle(strTitle);
					}

					pMF->GetTreeGraphView()->GetTreeCtrl()->m_bNew3D = false;
				}
			}
		}
		break;
	case FARM_DAT:	// 场数据
		{
			CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
			CString strFileName = pMF->GetProjectDatPath();
			strFileName += _T("\\models\\");
			strFileName += ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;

			if( !PathFileExists(strFileName) )
			{
				AfxMessageBox(_T("此文件不存在!"));
			}
			else
			{
				CDocument *pDoc = pMF->CreateOrActivateFrame(theApp.m_p2DModelDocTemplate, RUNTIME_CLASS(C2DModelView), strFileName);
				if( pDoc ) 
				{
					pDoc->SetPathName(strFileName);
					strTitle += _T(" - 1");
					pDoc->SetTitle(strTitle);
				}
			}
		}
		break;
	case FARM_LAYER:// 场层数据
		{
			CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
			CString strFileName = pMF->GetProjectDatPath();
			strFileName += _T("\\models\\");
			strFileName += ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;

			if( !PathFileExists(strFileName) )
			{
				AfxMessageBox(_T("此文件不存在!"));
			}
			else
			{
				C2DModelDoc *pDoc = (C2DModelDoc*)pMF->CreateOrActivateFrame(theApp.m_p2DModelDocTemplate, RUNTIME_CLASS(C2DModelView), strFileName);
				if( pDoc ) 
				{
					pDoc->SetPathName(strFileName);

					strTitle.Replace("# ","");
					CString strLayerName = strTitle;

					strTitle = lpNodeDat->m_strFileName;

					pDoc->SetTitle(strTitle + " - " + strLayerName);
					int nLayer = atoi(strLayerName);
					pDoc->CreateGrid(nLayer-1);
					pDoc->UpdateAllViews(NULL, HINT_UPDATE_LAYER, NULL);
				}
			}
		}
		break;
	default:
		return false;
	}
	return true;
}


void CModelView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CModelView::FillModelView(LPCTSTR lpszItem, int nImage, int nSelectedImage, 
							   HTREEITEM hParent/* = TVI_ROOT*/, 
							   HTREEITEM hInsertAfter/* = TVI_LAST*/)
{
	HTREEITEM hRoot = AddTreeItem(_T("DEMO 模型"), 0, 0);
	m_wndModelView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);

	HTREEITEM hClass = m_wndModelView.InsertItem(_T("Model_16*16*10"), 3, 3, hRoot);
	AddTreeItem(_T("属性"), 1, 1, hClass);

	m_wndModelView.Expand(hRoot, TVE_EXPAND);
	m_wndModelView.Expand(hClass, TVE_EXPAND);

	hClass = AddTreeItem(_T("NewModel"), 3, 3, hRoot);
	AddTreeItem(_T("属性"), 1, 1, hClass);
	AddTreeItem(_T("夹层"), 1, 1, hClass);
	AddTreeItem(_T("断层"), 1, 1, hClass);
	m_wndModelView.Expand(hClass, TVE_EXPAND);

	hClass = AddTreeItem(_T("Model_22*34*35"), 4, 4, hRoot);
	AddTreeItem(_T("切片"), 1, 1, hClass);
	AddTreeItem(_T("属性"), 1, 1, hClass);
	AddTreeItem(_T("侧积夹层"), 1, 1, hClass);
	m_wndModelView.Expand(hClass, TVE_EXPAND);

	
	hClass = AddTreeItem(_T("新建文件夹"), 2, 2, hRoot);
	AddTreeItem(_T("Model_16*16*10"), 4, 4, hClass);
	m_wndModelView.Expand(hClass, TVE_EXPAND);
}

HTREEITEM CModelView::AddTreeItem(LPCTSTR lpszItem, int nImage, int nSelectedImage, 
								 HTREEITEM hParent, 
								 HTREEITEM hInsertAfter)
{
	HTREEITEM hItem = m_wndModelView.InsertItem(lpszItem, nImage, nSelectedImage, hParent, hInsertAfter);
	m_wndModelView.SetModifiedFlag(true);
	return hItem;
}

void CModelView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_wndModelView;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// 选择已单击的项:
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			pWndTree->SelectItem(hTreeItem);
		}
	}

	pWndTree->SetFocus();
	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_POPUP_TREE));
	int nCount = menu.GetMenuItemCount();
	CMenu* pPopup = NULL;
	//
	//	GetMenuString
	CString strMenuName;
	for (int i=0;i<nCount;i++)
	{
		menu.GetMenuString(i,strMenuName,MF_BYPOSITION);
		if( strMenuName == "_模型管理器_" )
		{
			pPopup = menu.GetSubMenu(i);
			break;
		}
	}

	if( pPopup == NULL )
		return;

	ASSERT(pPopup != NULL);

	theApp.GetContextMenuManager()->ShowPopupMenu(pPopup->m_hMenu, point.x, point.y, this, TRUE);
}

void CModelView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndModelView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

BOOL CModelView::PreTranslateMessage(MSG* pMsg)
{
	return CDockablePane::PreTranslateMessage(pMsg);
}

void CModelView::OnNewFolder()
{
	m_wndModelView.AddSiblingFolder();
}

void CModelView::OnPaint()
{
	CPaintDC dc(this); // 用于绘制的设备上下文

	CRect rectTree;
	m_wndModelView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CModelView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndModelView.SetFocus();
}

void CModelView::OnChangeVisualStyle()
{
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_SORT_24 : IDR_SORT, 0, 0, TRUE /* 锁定*/);

	{// 装入树图标
		m_ModelViewImages.DeleteImageList();

		UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_MODEL_VIEW_24 : IDB_MODEL_VIEW;

		CBitmap bmp;
		if (!bmp.LoadBitmap(uiBmpId))
		{
			TRACE(_T("无法加载位图: %x\n"), uiBmpId);
			ASSERT(FALSE);
			return;
		}

		BITMAP bmpObj;
		bmp.GetBitmap(&bmpObj);

		UINT nFlags = ILC_MASK;

		nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

		m_ModelViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
		m_ModelViewImages.Add(&bmp, RGB(255, 0, 0));

		m_wndModelView.SetImageList(&m_ModelViewImages, TVSIL_NORMAL);
	}
	{	// 装入树状态图标
		m_TreeStateImages.DeleteImageList();	

		UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_TREE_STATE_24 : IDB_TREE_STATE;

		CBitmap bmp;
		if (!bmp.LoadBitmap(uiBmpId))
		{
			TRACE(_T("无法加载位图: %x\n"), uiBmpId);
			ASSERT(FALSE);
			return;
		}

		BITMAP bmpObj;
		bmp.GetBitmap(&bmpObj);

		UINT nFlags = ILC_MASK;

		nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

		m_TreeStateImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
		m_TreeStateImages.Add(&bmp, RGB(255, 255, 255));

		m_wndModelView.SetImageList(&m_TreeStateImages, TVSIL_STATE);
	}
}

void CModelView::LoadTree(CArchive& ar)
{
	m_wndModelView.Serialize(ar);
}

void CModelView::SaveTree(CArchive& ar)
{
	m_wndModelView.Serialize(ar);

	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	CString strDirectory = pMF->GetProjectDatPath();
	CString strTmp = strDirectory;
	strTmp += _T("\\models");
	m_wndModelView.DeleteUnusedFile(strTmp);
}

void CModelView::EmptyTree()
{
	m_wndModelView.DeleteItemM(TVI_ROOT);
}

void CModelView::OnAddChild()
{
	// TODO: 在此添加命令处理程序代码
	m_wndModelView.AddChildFolder();
}

void CModelView::OnUpdateAddChild(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
}

void CModelView::OnEditRename()
{
	// TODO: 在此添加命令处理程序代码
	m_wndModelView.OnRename();
}

void CModelView::OnUpdateEditRename(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	m_wndModelView.OnUpdateRename(pCmdUI);
}

void CModelView::OnImportModel()
{
	// TODO: 在此添加命令处理程序代码
	CDlgImportModel dlg(TRUE,0,0,4|2|64,_T("所有文件 (*.*)|*.*||")); // 导入模型及场数据
	dlg.m_ofn.lpstrTitle = _T("导入模型数据");
	if( dlg.DoModal() == IDOK )
	{	
		CString strSourcePathName = dlg.GetPathName();
		
		CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
		CString strTargePathName = pMF->GetProjectDatPath();
		strTargePathName += _T("\\models\\");

		CString strFileName = newGUID();
		strFileName += _T(".mod");

		strTargePathName += strFileName;

		switch( dlg.m_nFileType )
		{
		case 2:			// PBRS文件格式
		case 1:
			if( !ReadPBRSModel(strSourcePathName, strTargePathName) )
				return;
			else
				break;
		case 0:
			if( !ReadEclipseModel(strSourcePathName, strTargePathName) )
				return;
			else
				break;
		default:
			return;
		}

		CString strFileTitle = dlg.GetFileTitle();
		
		HTREEITEM hItem = m_wndModelView.GetSelectedItem();
		if( hItem == NULL )
			hItem = TVI_ROOT;

		HTREEITEM h = AddTreeItem(strFileTitle, 1, 1, hItem);

		CTreeNodeDat *lpNodeDat = new CTreeNodeDat;

		CFileViewObj *pObj = new CFileViewObj;
		pObj->m_strFileName = strFileName;

		lpNodeDat->m_nImage			= 1;
		lpNodeDat->m_nSelectedImage	= 1;
		lpNodeDat->m_nType			= GRID_DAT;
		lpNodeDat->m_pNodeDat		= pObj;
		lpNodeDat->m_uState			= TVIS_EXPANDED;
		strFileName.MakeUpper();
		lpNodeDat->m_strFileName = strFileName;

		m_wndModelView.SetItemData(h, (DWORD)lpNodeDat);

		m_wndModelView.Expand(hItem, TVE_EXPAND);

		m_wndModelView.EditLabel(h);
	}
}

void CModelView::OnUpdateImportModel(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();

	if( pMF->m_strPrjFileName.IsEmpty() )
		pCmdUI->Enable(FALSE);
	else
	{
		HTREEITEM hItem = m_wndModelView.GetSelectedItem();

		if( hItem != NULL)
		{
			CTreeNodeDat *pNote = (CTreeNodeDat *)m_wndModelView.GetItemData(hItem);
			if(pNote->m_nType == FOLDER)
			{
				pCmdUI->Enable(TRUE);
			}
			else
				pCmdUI->Enable(FALSE);
		}
		else
			pCmdUI->Enable(TRUE);
	}
}

void CModelView::OnImportField()
{
	// TODO: 在此添加命令处理程序代码
	HTREEITEM hItem = m_wndModelView.GetSelectedItem();

	CTreeNodeDat *pNote = (CTreeNodeDat *)m_wndModelView.GetItemData(hItem);
	
	CString strModelFileName = pNote->m_strFileName;

	CDlgImportModel dlg(TRUE,0,0,4|2|64,_T("所有文件 (*.*)|*.*||")); // 导入模型及场数据
	dlg.m_ofn.lpstrTitle = _T("导入场数据");
	if( dlg.DoModal() == IDOK )
	{
		CString strFileName = dlg.GetPathName();
		switch( dlg.m_nFileType )
		{
		case 0:			// PBRS文件格式
		case 1:
			ReadGridPBRS(strFileName, strModelFileName, hItem);
			break;
		case 2:
			ReadGridEclipse(strFileName, strModelFileName, hItem);
			break;
		default:
			break;
		}
	}
}

void CModelView::OnUpdateImportField(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();

	if( pMF->m_strPrjFileName.IsEmpty() )
		pCmdUI->Enable(FALSE);
	else
	{
		HTREEITEM hItem = m_wndModelView.GetSelectedItem();

		if( hItem != NULL)
		{
			CTreeNodeDat *pNote = (CTreeNodeDat *)m_wndModelView.GetItemData(hItem);
			if(pNote->m_nType == GRID_DAT)
			{
				pCmdUI->Enable(TRUE);
			}
			else
				pCmdUI->Enable(FALSE);
		}
		else
			pCmdUI->Enable(FALSE);
	}
}

void CModelView::OnExpandAllTree()
{
	// TODO: 在此添加命令处理程序代码
	m_wndModelView.OnExpandAllTree();
}

void CModelView::OnCollapseAllEntireTree()
{
	// TODO: 在此添加命令处理程序代码
	m_wndModelView.OnCollapseAllEntireTree();
}

void CModelView::OnExportModel()
{
	// TODO: 在此添加命令处理程序代码

	CDlgImportModel dlg(FALSE); // 导出模型及场数据
	dlg.m_ofn.lpstrTitle = _T("导出模型数据");
	if( dlg.DoModal() == IDOK )
	{

	}
}

void CModelView::OnFileOpen()
{
	// TODO: 在此处添加命令处理程序代码

	HTREEITEM hItem = m_wndModelView.GetSelectedItem();

	if( hItem == NULL )
		return;

	CString strTitle = m_wndModelView.GetItemText(hItem);
	CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)m_wndModelView.GetItemData(hItem);

	if( lpNodeDat->m_nType == GRID_DAT)
	{	
		CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
		pMF->GetTreeGraphView()->GetTreeCtrl()->m_bNew3D = true;

		CString strPath = pMF->GetProjectDatPath();
		strPath += _T("\\models\\");

		m_arParamName.RemoveAll();
		m_arFileName.RemoveAll();

		HTREEITEM hChildItem = m_wndModelView.GetChildItem(hItem);
		while(hChildItem != NULL )
		{
			CTreeNodeDat *lpNode = (CTreeNodeDat*)m_wndModelView.GetItemData(hChildItem);
			CString str = m_wndModelView.GetItemText(hChildItem);
			m_arParamName.Add(str);

			str = strPath;
			str += ((CFileViewObj*)lpNode->m_pNodeDat)->m_strFileName;

			m_arFileName.Add(str);

			hChildItem = m_wndModelView.GetNextSiblingItem(hChildItem);
		}
	}
	
	OpenTree(lpNodeDat, (LPVOID)(LPCSTR)strTitle);
}

void CModelView::OnUpdateFileOpen(CCmdUI *pCmdUI)
{
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();

	if( pMF->m_strPrjFileName.IsEmpty() )
		pCmdUI->Enable(FALSE);
	else
	{
		HTREEITEM hItem = m_wndModelView.GetSelectedItem();

		if( hItem != NULL)
		{
			CTreeNodeDat *pNote = (CTreeNodeDat *)m_wndModelView.GetItemData(hItem);
			if(pNote->m_nType == GRID_DAT)
			{
				pCmdUI->SetText(_T("新建3D图形"));
				pCmdUI->Enable(TRUE);
			}
		}
		else
			pCmdUI->Enable(FALSE);
	}
}

void CModelView::OnUpdateExportModel(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();

	if( pMF->m_strPrjFileName.IsEmpty() )
		pCmdUI->Enable(FALSE);
	else
	{
		HTREEITEM hItem = m_wndModelView.GetSelectedItem();

		if( hItem != NULL)
		{
			CTreeNodeDat *pNote = (CTreeNodeDat *)m_wndModelView.GetItemData(hItem);
			if(pNote->m_nType == GRID_DAT)
			{
				pCmdUI->Enable(TRUE);
			}
			else
				pCmdUI->Enable(FALSE);
		}
		else
			pCmdUI->Enable(FALSE);
	}
}

// 读取PBRS网格模型
bool CModelView::ReadPBRSModel(LPCSTR lpszFileName, LPCSTR lpszModelFileName)
{
	CStdioFile file;
	if( !file.Open(lpszFileName, CFile::modeRead | CFile::typeText) )
		return false;
	if( file.GetLength() < 1 ) // 如果文件长度为0
	{
		file.Close();
		::MessageBox(AfxGetMainWnd()->m_hWnd, _T("文件长度为0!"),_T("提示信息"), MB_OK | MB_ICONWARNING);
		return false;
	}
	file.Close();

	CWaitCursor wait;

	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();

	CString strMsg;
	strMsg = _T("正在分析网格模型数据文件，请稍等...");
	pMF->GetStatusBar().SetPaneText(0, strMsg);

	CString strTmp;

	// 0-x ,  1-y ,  2-z
	std::vector<double> xSteps;
	std::vector<double> ySteps;
	std::vector<double> zValues;
	std::vector<double> depthLayer;

	int iDatType = -1;			// 数据判刑
	int iArrayType = -1;		// 数组选件

	CParsePbrsFile pbrsFile;
	pbrsFile.OpenFile(lpszFileName);
	CSimuToken token;

	do{
		CPbrsCommandItem commItem;
		token = pbrsFile.GetToken(commItem);
		if( token.token_type == CSimuToken::COMMAND )
		{
			switch(commItem.m_nCommID)
			{
			case CPbrsCommandItem::DX:
				iDatType = 0;
				break;
			case CPbrsCommandItem::DY:
				iDatType = 1;
				break;
			case CPbrsCommandItem::DZ:
				iDatType = 2;
				break;
			case CPbrsCommandItem::DEPTH:
				iDatType = 3;
				break;
			case CPbrsCommandItem::MOD:
				iDatType = 4;
				break;
			case CPbrsCommandItem::VMOD:
				iDatType = 5;
				break;
			case CPbrsCommandItem::CON:
				iArrayType = 0;
				break;
			case CPbrsCommandItem::XVAR:
				iArrayType = 1;
				break;
			case CPbrsCommandItem::YVAR:
				iArrayType = 2;
				break;
			case CPbrsCommandItem::ZVAR:
				iArrayType = 3;
				break;
			case CPbrsCommandItem::VALUE:
				iArrayType = 4;
				break;
			case CPbrsCommandItem::LAYER:
				iArrayType = 5;
				break;
			case CPbrsCommandItem::MULT:
				iArrayType = 6;
				break;
			case CPbrsCommandItem::VMULT:
				iArrayType = 7;
				break;
			default:
				iDatType = -1;
				iArrayType = -1;
				break;
			}
		}		
		else if( token.token_type == CSimuToken::NUMBER )
		{
			CString strTmp = token.m_strToken;
			int index = strTmp.Find("*");

			switch(iDatType)
			{
			case 0:				// DX
				if( iArrayType == 1 ) // XVAR
				{
					if(index > -1) // 是 2*0.5 格式
					{
						int n = atoi(strTmp.Left(index));
						double v = atof(strTmp.Right(strTmp.GetLength()-index+1));
						for (int i=0;i<n;i++)
							xSteps.push_back(v);
					}
					else
					{
						double v = atof(strTmp);
						xSteps.push_back(v);
					}
				}
				break;
			case 1:
				if( iArrayType == 2 ) // YVAR
				{
					if(index > -1) // 是 2*0.5 格式
					{
						int n = atoi(strTmp.Left(index));
						double v = atof(strTmp.Right(strTmp.GetLength()-index+1));
						for (int i=0;i<n;i++)
							ySteps.push_back(v);
					}
					else
					{
						double v = atof(strTmp);
						ySteps.push_back(v);
					}
				}
				break;
			case 2:
				if( iArrayType == 3 ) // ZVAR
				{
					if(index > -1) // 是 2*0.5 格式
					{
						int n = atoi(strTmp.Left(index));
						double v = atof(strTmp.Right(strTmp.GetLength()-index+1));
						for (int i=0;i<n;i++)
							zValues.push_back(v);
					}
					else
					{
						double v = atof(strTmp);
						zValues.push_back(v);
					}
				}
				if( iArrayType == 4 ) // ZVAR
				{
					if(index > -1) // 是 2*0.5 格式
					{
						int n = atoi(strTmp.Left(index));
						double v = atof(strTmp.Right(strTmp.GetLength()-index+1));
						for (int i=0;i<n;i++)
							zValues.push_back(v);
					}
					else
					{
						double v = atof(strTmp);
						zValues.push_back(v);
					}
				}
				break;
			case 3:
				if( iArrayType == 5 ) // LAYER
				{
					if(index > -1) // 是 2*0.5 格式
					{
						int n = atoi(strTmp.Left(index));
						double v = atof(strTmp.Right(strTmp.GetLength()-index+1));
						for (int i=0;i<n;i++)
							depthLayer.push_back(v);
					}
					else
					{
						double v = atof(strTmp);
						depthLayer.push_back(v);
					}
				}
				break;
			default:
				break;
			}			
		}
		else
		{
			iDatType = -1;
		}

	}while(token.token_type != CSimuToken::END);

	int nX = xSteps.size();
	int nY = ySteps.size();
	if( nX<1 || nY < 1)
	{
		::MessageBox(AfxGetMainWnd()->m_hWnd, _T("不是PBRS数据文件格式!"), _T("提示信息"), MB_OK | MB_ICONWARNING);
		return false;
	}

	int nSize = depthLayer.size();
	if( nSize != (nX * nY) ) 
	{
		::MessageBox(AfxGetMainWnd()->m_hWnd, _T("不是PBRS数据文件格式!"), _T("提示信息"), MB_OK | MB_ICONWARNING);
		return false;
	}

	nSize = zValues.size();

	if( nSize % (nX*nY) )
	{
		::MessageBox(AfxGetMainWnd()->m_hWnd, _T("不是PBRS数据文件格式!"), _T("提示信息"), MB_OK | MB_ICONWARNING);
		return false;
	}

	CGridModel gridModel;
	// 先清除内存
	gridModel.m_coordGrid.erase(gridModel.m_coordGrid.begin(), gridModel.m_coordGrid.end());
	gridModel.m_zcornGrid.erase(gridModel.m_zcornGrid.begin(), gridModel.m_zcornGrid.end());

	gridModel.m_gridDX.erase(gridModel.m_gridDX.begin(), gridModel.m_gridDX.end());
	gridModel.m_gridDY.erase(gridModel.m_gridDY.begin(), gridModel.m_gridDY.end());
	gridModel.m_gridDZ.erase(gridModel.m_gridDZ.begin(), gridModel.m_gridDZ.end());
	gridModel.m_gridTops.erase(gridModel.m_gridTops.begin(), gridModel.m_gridTops.end());

	int nZ = nSize/(nX*nY);

	gridModel.m_bCornerPointGrid = FALSE;
	gridModel.m_nGridX = nX;
	gridModel.m_nGridY = nY;
	gridModel.m_nGridZ = nZ;	

	for (int i=0;i<nZ; i++)
	{
		for (int j=0;j<nY;j++)
		{
			double v = ySteps[j];
			for (int k=0;k<nX;k++)
			{
				gridModel.m_gridDX.push_back(xSteps[k]);
				gridModel.m_gridDY.push_back(ySteps[k]);
			}
		}
	}
	gridModel.m_gridDZ = zValues;
	gridModel.m_gridTops = depthLayer;


	gridModel.m_dMinX = DBL_MAX;
	gridModel.m_dMaxX = -DBL_MAX;

	gridModel.m_dMinY = DBL_MAX;
	gridModel.m_dMaxY = -DBL_MAX;

	gridModel.m_dMinZ = DBL_MAX;
	gridModel.m_dMaxZ = -DBL_MAX;

	strMsg = _T("正在计算网格模型数据文件，请稍等...");
	pMF->GetStatusBar().SetPaneText(0, strMsg);

	pMF->GetStatusBar().EnablePaneProgressBar (1, nZ, TRUE);
	for ( int i=0;i<nZ; i++)
	{
		for (int j=0;j<nY;j++)
		{
			for (int k=0;k<nX;k++)
			{
				for (int m=0;m<8;m++)
				{
					CVertex3D point = gridModel.GetCornerPoint(k,j,i,m);

					if( point.x > gridModel.m_dMaxX )
						gridModel.m_dMaxX = point.x;
					if( point.x < gridModel.m_dMinX )
						gridModel.m_dMinX = point.x;

					if( point.y > gridModel.m_dMaxY )
						gridModel.m_dMaxY = point.y;
					if( point.y < gridModel.m_dMinY )
						gridModel.m_dMinY = point.y;

					if( point.z > gridModel.m_dMaxZ )
						gridModel.m_dMaxZ = point.z;
					if( point.z < gridModel.m_dMinZ )
						gridModel.m_dMinZ = point.z;
				}
			}
		}

		pMF->GetStatusBar().SetPaneProgress (1, i);
	}

	pMF->GetStatusBar().EnablePaneProgressBar (1, -1);

	strMsg = _T("正在导入网格模型数据文件，请稍等...");
	pMF->GetStatusBar().SetPaneText(0, strMsg);

	CFile gridfile;
	if( gridfile.Open(lpszModelFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary ) )
	{
		CArchive ar(&gridfile,CArchive::store);
		gridModel.Serialize(ar);
		ar.Close();
		gridfile.Close();
	}

	strMsg = _T("导入网格模型数据文件完毕!");
	
	pMF->GetStatusBar().SetPaneText(0, strMsg);

	return true;
}

// 读取Eclipse网格模型
bool CModelView::ReadEclipseModel(LPCSTR lpszFileName, LPCSTR lpszModelFileName)
{
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	CString strMsg;
	strMsg = _T("正在解析网格数据文件，请稍等...");
	pMF->GetStatusBar().SetPaneText(0, strMsg);
	bool rl= CIntersectSearchManager::Instance()->LoadGridModel(lpszFileName,lpszModelFileName);
	strMsg = _T("就绪");
	pMF->GetStatusBar().SetPaneText(0, strMsg);
	return rl;
}

// 读取静态场数据并返回新的静态场文件名
bool CModelView::ReadGridPBRS(LPCSTR lpszGridFileName, LPCSTR lpszModelFileName, HTREEITEM hItem)
{
	CStdioFile fileOld;
	if( !fileOld.Open(lpszGridFileName, CFile::modeRead | CFile::typeText) )
		return false;
	if( fileOld.GetLength() < 1 ) // 如果文件长度为0 返回空的文件名
	{
		fileOld.Close();
		return false;
	}

	fileOld.Close();

	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();

	CWaitCursor wait;
	
	CString strModeFileName = pMF->GetProjectDatPath();
	strModeFileName += _T("\\models\\");
	strModeFileName += lpszModelFileName;

	int nGridX, nGridY, nGridZ;
	BOOL bCornerPointGrid;
	CFile gridfile;
	if( gridfile.Open(lpszModelFileName, CFile::modeRead | CFile::typeBinary ) )
	{
		CArchive ar(&gridfile,CArchive::load);
		CObject::Serialize(ar);
		ar.SerializeClass(RUNTIME_CLASS(CGridModel));
		ar >> bCornerPointGrid;
		ar >> nGridX;
		ar >> nGridY;
		ar >> nGridZ;
		ar.Close();
		gridfile.Close();
	}	

	CString strMsg;
	strMsg = _T("正在分析静态场数据文件，请稍等...");
	pMF->GetStatusBar().SetPaneText(0, strMsg);

	CString strTmp;

	std::vector<double> values;

	CPbrsCommandItem	vmultItem;	// 被乘数组选件
	std::vector<double> vmult;		// 乘数 nx*ny*nz 适用于VMULT
	double dMult;					// 乘数 适用于MULT

	int i1, i2, j1, j2, k1, k2;		// 适用于MOD VMOD
	std::vector<double> vmod;
	int index = 0;

	CParsePbrsFile pbrsFile;
	pbrsFile.OpenFile(lpszGridFileName);
	CSimuToken token;
	CPbrsCommandItem commType; // 数据类型
	CPbrsCommandItem dateType; // 数组选件

	do
	{
		CPbrsCommandItem commItem;
		token = pbrsFile.GetToken(commItem);
		if( token.token_type == CSimuToken::COMMAND )
		{
			switch(commItem.m_nCommID)
			{
			case CPbrsCommandItem::DZNET:		// 有效厚度
			case CPbrsCommandItem::KX:			// x方向渗透率
			case CPbrsCommandItem::KY:
			case CPbrsCommandItem::KZ:
			case CPbrsCommandItem::KXF:
			case CPbrsCommandItem::KYF:
			case CPbrsCommandItem::KZF:
			case CPbrsCommandItem::POR:			// 孔隙度
			case CPbrsCommandItem::SWC:			// 归一化束缚水饱和度
			case CPbrsCommandItem::SGC:			// 归一化临界气饱和度
			case CPbrsCommandItem::SW:			// 含水饱和度
			case CPbrsCommandItem::SO:			// 含油饱和度
			case CPbrsCommandItem::SG:			// 含气饱和度
			case CPbrsCommandItem::P:			// 每个网格单元的平均油相压力
			case CPbrsCommandItem::BP:			// 每个网格单元的泡点压力
				if(dateType.m_nCommID == CPbrsCommandItem::VMULT 
					&& vmultItem.m_nCommID == CPbrsCommandItem::UNKNOWN )
				{
					vmultItem = commItem;
				}
				else if( dateType.m_nCommID == CPbrsCommandItem::MULT )
				{
					// *mult;
					HTREEITEM htiSon = m_wndModelView.GetChildNode(hItem, vmultItem.m_strCommand);
					if( htiSon != NULL )
					{
						CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)m_wndModelView.GetItemData(htiSon);

						CString strName = pMF->GetProjectDatPath();
						strName += _T("\\models\\");
						strName += ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;

						values.erase(values.begin(), values.end());
						CFile tmpfile;
						if( tmpfile.Open(strName, CFile::modeRead | CFile::typeBinary ) )
						{
							CArchive ar(&tmpfile,CArchive::load);
							for (int i=0;i<nGridX*nGridY*nGridZ; i++)
							{
								double v;
								ar >> v;
								values.push_back(v);
							}

							ar.Close();

							tmpfile.Close();

							for ( int i=0;i<nGridX*nGridY*nGridZ; i++)
								values[i] *= dMult;
						}							
					}
					dateType.m_nCommID = CPbrsCommandItem::UNKNOWN;
				}
				else
				{
					if( dateType.m_nCommID == CPbrsCommandItem::VMULT
						&& vmultItem.m_nCommID != CPbrsCommandItem::UNKNOWN)
					{
						HTREEITEM htiSon = m_wndModelView.GetChildNode(hItem, vmultItem.m_strCommand);
						if( htiSon != NULL )
						{
							CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)m_wndModelView.GetItemData(htiSon);

							CString strName = pMF->GetProjectDatPath();
							strName += _T("\\models\\");
							strName += ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;

							values.erase(values.begin(), values.end());
							CFile tmpfile;
							if( tmpfile.Open(strName, CFile::modeRead | CFile::typeBinary ) )
							{
								CArchive ar(&tmpfile,CArchive::load);
								for (int i=0;i<nGridX*nGridY*nGridZ; i++)
								{
									double v;
									ar >> v;
									values.push_back(v);
								}

								ar.Close();

								tmpfile.Close();

								for ( int i=0;i<nGridX*nGridY*nGridZ; i++)
									values[i] *= vmult[i];
							}
						}
					}
					vmultItem.m_nCommID = CPbrsCommandItem::UNKNOWN;

					if( values.size()> 0 )
					{
						if( dateType.m_nCommID == CPbrsCommandItem::VMOD )
						{
							int r = i2-i1+1;
							int c = j2-j1+1;
							for(int k=k1, kk=0;k<=k2;k++, kk++)
							{
								for (int j=j1, jj=0;j<=j2;j++, j++)
								{
									for (int i=i1, ii=0;i<=i2; i++, ii++)
									{
										values[k*(nGridX*nGridY)+j*nGridX + i] = vmod[kk*c*r+jj*r+ii];
									}
								}
							}
						}
						strMsg = _T("正在保存静态场数据文件，请稍等...");
						pMF->GetStatusBar().SetPaneText(0, strMsg);

						CString strName = newGUID();
						strName += _T(".grd");

						CString strNewFileName = pMF->GetProjectDatPath();
						strNewFileName += _T("\\models\\");
						strNewFileName += strName;

						CFile fileNew;
						if( fileNew.Open(strNewFileName, CFile::modeCreate | CFile::modeWrite ) )
						{
							CArchive archive(&fileNew, CArchive::store);
							CString strModelFileName = lpszModelFileName;
							
							archive << strModelFileName;

							int nSize = values.size();
							
							archive << nSize;

							for ( int i=0; i<nSize; i++)
							{
								archive << values[i];
							}
							archive << RGB(255,0,0);
							archive << RGB(255,255,255);
							archive << (double)0;
							archive << (double)-1;
							archive.Close();
							fileNew.Close();
						}

						HTREEITEM htiSon = m_wndModelView.GetChildNode(hItem,commType.m_strCommand);
						if( htiSon == NULL )
						{
							HTREEITEM hSrc = AddTreeItem(commType.m_strCommand, 2, 2, hItem);

							CTreeNodeDat *lpNodeDat = new CTreeNodeDat;

							CFileViewObj *pObj = new CFileViewObj;
							pObj->m_strFileName = strName;

							lpNodeDat->m_nImage			= 2;
							lpNodeDat->m_nSelectedImage	= 2;
							lpNodeDat->m_nType			= FARM_DAT;
							lpNodeDat->m_pNodeDat		= pObj;
							lpNodeDat->m_uState			= TVIS_EXPANDED;
							strName.MakeUpper();
							lpNodeDat->m_strFileName = strName;

							m_wndModelView.SetItemData(hSrc, (DWORD)lpNodeDat);

							m_wndModelView.Expand(hItem, TVE_EXPAND);
							
							for (int i=0;i<nGridZ; i++)
							{
								CString str;
								str.Format("# %d", i+1);

								HTREEITEM h = AddTreeItem(str, 3, 3, hSrc);

								CTreeNodeDat *lpNodeDat = new CTreeNodeDat;

								CFileViewObj *pObj = new CFileViewObj;
								pObj->m_strFileName = strName;

								lpNodeDat->m_nImage			= 3;
								lpNodeDat->m_nSelectedImage	= 3;
								lpNodeDat->m_nType			= FARM_LAYER;
								lpNodeDat->m_pNodeDat		= pObj;
								lpNodeDat->m_uState			= TVIS_EXPANDED;
								lpNodeDat->m_strFileName	= commType.m_strCommand;

								m_wndModelView.SetItemData(h, (DWORD)lpNodeDat);
							}

							//m_wndModelView.Expand(hSrc, TVE_EXPAND);

						}
						else
						{
							CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)m_wndModelView.GetItemData(htiSon);
							
							if( lpNodeDat != NULL )
							{
								delete lpNodeDat;
								lpNodeDat = NULL;
							}

							lpNodeDat = new CTreeNodeDat;

							CFileViewObj *pObj = new CFileViewObj;
							pObj->m_strFileName = strName;

							lpNodeDat->m_nImage			= 2;
							lpNodeDat->m_nSelectedImage	= 2;
							lpNodeDat->m_nType			= FARM_DAT;
							lpNodeDat->m_pNodeDat		= pObj;
							lpNodeDat->m_uState			= TVIS_EXPANDED;
							strName.MakeUpper();
							lpNodeDat->m_strFileName = strName;

							m_wndModelView.SetItemData(htiSon, (DWORD)lpNodeDat);
						}					

						values.erase(values.begin(), values.end());

						strMsg = _T("正在分析静态场数据文件，请稍等...");
					}

					commType = commItem;
				}
				break;
			case CPbrsCommandItem::VALUE:

			case CPbrsCommandItem::MULT:			// KZ MULT
				// 0.1 KX         -- KZ=0.1*KX

			case CPbrsCommandItem::VMULT:			// DZNET VMULT
				// DZ 
				// Vi ........ V nx*ny*nz个 -- DZNET = Vi*DZi

			case CPbrsCommandItem::MOD:				// MOD
				// 2 4 2 2 1 1 *1.5
				// 2 3 3 3 1 1 =0.22

			case CPbrsCommandItem::VMOD:			// VMOD
				// 2 4 2 3 1 1
				// 0.25 0.26 2*0.29 2*0.3
			case CPbrsCommandItem::CON:				// 常数
				dateType = commItem;
				break;
			default:				
				if(values.size()>0)
				{
					if( dateType.m_nCommID == CPbrsCommandItem::VMOD )
					{
						int r = i2-i1+1;
						int c = j2-j1+1;
						for(int k=k1, kk=0;k<=k2;k++, kk++)
						{
							for (int j=j1, jj=0;j<=j2;j++, j++)
							{
								for (int i=i1, ii=0;i<=i2; i++, ii++)
								{
									values[k*(nGridX*nGridY)+j*nGridX + i] = vmod[kk*c*r+jj*r+ii];
								}
							}
						}
					}
					commType.m_nCommID = CPbrsCommandItem::UNKNOWN;
					dateType.m_nCommID = CPbrsCommandItem::UNKNOWN;
					strMsg = _T("正在保存静态场数据文件，请稍等...");
					pMF->GetStatusBar().SetPaneText(0, strMsg);

					CString strName = newGUID();
					strName += _T(".grd");

					CString strNewFileName = pMF->GetProjectDatPath();
					strNewFileName += _T("\\models\\");
					strNewFileName += strName;

					CFile fileNew;
					if( fileNew.Open(strNewFileName, CFile::modeCreate | CFile::modeWrite ) )
					{
						CArchive archive(&fileNew, CArchive::store);

						CString strModelFileName = lpszModelFileName;

						archive << strModelFileName;

						int nSize = values.size();
						
						archive << nSize;

						for ( int i=0; i<nSize; i++)
						{
							archive << values[i];
						}
						archive << RGB(255,0,0);
						archive << RGB(255,255,255);
						archive << (double)0;
						archive << (double)-1;
						archive.Close();
						fileNew.Close();
					}

					HTREEITEM htiSon = m_wndModelView.GetChildNode(hItem,commType.m_strCommand);
					if( htiSon == NULL )
					{
						HTREEITEM hSrc = AddTreeItem(commType.m_strCommand, 2, 2, hItem);

						CTreeNodeDat *lpNodeDat = new CTreeNodeDat;

						CFileViewObj *pObj = new CFileViewObj;
						pObj->m_strFileName = strName;

						lpNodeDat->m_nImage			= 2;
						lpNodeDat->m_nSelectedImage	= 2;
						lpNodeDat->m_nType			= FARM_DAT;
						lpNodeDat->m_pNodeDat		= pObj;
						lpNodeDat->m_uState			= TVIS_EXPANDED;
						strName.MakeUpper();
						lpNodeDat->m_strFileName = strName;

						m_wndModelView.SetItemData(hSrc, (DWORD)lpNodeDat);

						m_wndModelView.Expand(hItem, TVE_EXPAND);

						for (int i=0;i<nGridZ; i++)
						{
							CString str;
							str.Format("# %d", i+1);

							HTREEITEM h = AddTreeItem(str, 3, 3, hSrc);

							CTreeNodeDat *lpNodeDat = new CTreeNodeDat;

							CFileViewObj *pObj = new CFileViewObj;
							pObj->m_strFileName = strName;

							lpNodeDat->m_nImage			= 3;
							lpNodeDat->m_nSelectedImage	= 3;
							lpNodeDat->m_nType			= FARM_LAYER;
							lpNodeDat->m_pNodeDat		= pObj;
							lpNodeDat->m_uState			= TVIS_EXPANDED;
							lpNodeDat->m_strFileName	= commType.m_strCommand;

							m_wndModelView.SetItemData(h, (DWORD)lpNodeDat);
						}

						//m_wndModelView.Expand(hSrc, TVE_EXPAND);
					}
					else
					{
						CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)m_wndModelView.GetItemData(htiSon);

						if( lpNodeDat != NULL )
						{
							delete lpNodeDat;
							lpNodeDat = NULL;
						}

						lpNodeDat = new CTreeNodeDat;

						CFileViewObj *pObj = new CFileViewObj;
						pObj->m_strFileName = strName;

						lpNodeDat->m_nImage			= 2;
						lpNodeDat->m_nSelectedImage	= 2;
						lpNodeDat->m_nType			= FARM_DAT;
						lpNodeDat->m_pNodeDat		= pObj;
						lpNodeDat->m_uState			= TVIS_EXPANDED;
						strName.MakeUpper();
						lpNodeDat->m_strFileName = strName;

						m_wndModelView.SetItemData(htiSon, (DWORD)lpNodeDat);
					}					

					values.erase(values.begin(), values.end());

					strMsg = _T("正在分析静态场数据文件，请稍等...");
				}
				break;
			}
		}		
		else if( token.token_type == CSimuToken::NUMBER )
		{
			CString strTmp = token.m_strToken;
			int index = strTmp.Find("*");

			switch(commType.m_nCommID)
			{
			case CPbrsCommandItem::DZNET:		// 有效厚度
			case CPbrsCommandItem::KX:			// x方向渗透率
			case CPbrsCommandItem::KY:
			case CPbrsCommandItem::KZ:
			case CPbrsCommandItem::KXF:
			case CPbrsCommandItem::KYF:
			case CPbrsCommandItem::KZF:
			case CPbrsCommandItem::POR:			// 孔隙度
			case CPbrsCommandItem::SWC:			// 归一化束缚水饱和度
			case CPbrsCommandItem::SGC:			// 归一化临界气饱和度
			case CPbrsCommandItem::SW:			// 含水饱和度
			case CPbrsCommandItem::SO:			// 含油饱和度
			case CPbrsCommandItem::SG:			// 含气饱和度
			case CPbrsCommandItem::P:			// 每个网格单元的平均油相压力
			case CPbrsCommandItem::BP:			// 每个网格单元的泡点压力
				{
					switch(dateType.m_nCommID)
					{
					case CPbrsCommandItem::VALUE:
						{
							if(index > -1) // 是 2*0.5 格式
							{
								int n = atoi(strTmp.Left(index));
								double v = atof(strTmp.Right(strTmp.GetLength()-index+1));
								for (int i=0;i<n;i++)
									values.push_back(v);
							}
							else
							{
								double v = atof(strTmp);
								values.push_back(v);
							}
						}
						break;
					case CPbrsCommandItem::MULT:			// KZ MULT	
						{
							dMult=atof(strTmp);
						}
						break;
					case CPbrsCommandItem::VMULT:			// DZNET VMULT
						{
							if(index > -1) // 是 2*0.5 格式
							{
								int n = atoi(strTmp.Left(index));
								double v = atof(strTmp.Right(strTmp.GetLength()-index+1));
								for (int i=0;i<n;i++)
									vmult.push_back(v);
							}
							else
							{
								double v = atof(strTmp);
								vmult.push_back(v);
							}
						}
						break;
					case CPbrsCommandItem::MOD:				// MOD
						{
							if( index == 0 )
								i1 = atoi(strTmp);
							if( index == 1 )
								i2 = atoi(strTmp);
							if( index == 2 )
								j1 = atoi(strTmp);
							if( index == 3 )
								j2 = atoi(strTmp);
							if( index == 4 )
								k1 = atoi(strTmp);
							if( index == 5 )
								k2 = atoi(strTmp);

							index ++;
						}
						break;
					case CPbrsCommandItem::VMOD:			// VMOD
						{
							if( index == 0 )
								i1 = atoi(strTmp);
							if( index == 1 )
								i2 = atoi(strTmp);
							if( index == 2 )
								j1 = atoi(strTmp);
							if( index == 3 )
								j2 = atoi(strTmp);
							if( index == 4 )
								k1 = atoi(strTmp);
							if( index == 5 )
								k2 = atoi(strTmp);
							if( index == 6 )
							{
								if(index > -1) // 是 2*0.5 格式
								{
									int n = atoi(strTmp.Left(index));
									double v = atof(strTmp.Right(strTmp.GetLength()-index+1));
									for (int i=0;i<n;i++)
										vmod.push_back(v);
								}
								else
								{
									double v = atof(strTmp);
									vmod.push_back(v);
								}
							}
							else
								index ++;
						}
						break;
					case CPbrsCommandItem::CON:				//
						{
							double v = atof(strTmp);
							for (int i=0;i<nGridX*nGridY*nGridZ;i++)
							{
								values.push_back(v);
							}
						}
						break;
					default:
						break;
					}
				}
				break;
			default:
				break;
			}			
		}
		else if( token.token_type == CSimuToken::STRING )
		{
			if( dateType.m_nCommID == CPbrsCommandItem::MOD 
				&& commType.m_nCommID != CPbrsCommandItem::UNKNOWN
				&& index == 6)				// MOD
			{
				if(token.m_strToken[0] == '=' 
					|| token.m_strToken[0] == '+' 
					|| token.m_strToken[0] == '-' 
					|| token.m_strToken[0] == '*' 
					|| token.m_strToken[0] == '/' )
				{
					index = 0;
					CString strTmp = token.m_strToken.Right(token.m_strToken.GetLength()-1);
					double v = atof(strTmp);

					for (int k=k1; k<=k2; k++)
					{
						for (int j=j1; j<=j2; j++)
						{
							for (int i=i1; i<=i2; i++)
							{
								values[k*(nGridX*nGridY)+j*nGridX + i]=v;
							}
						}
					}
				}
			}
		}
		else
		{
			commType.m_nCommID = CPbrsCommandItem::UNKNOWN;
		}

	}while(token.token_type != CSimuToken::END);


	if( values.size() > 0 )
	{
		if( dateType.m_nCommID == CPbrsCommandItem::VMOD )
		{
			int r = i2-i1+1;
			int c = j2-j1+1;
			for(int k=k1, kk=0;k<=k2;k++, kk++)
			{
				for (int j=j1, jj=0;j<=j2;j++, j++)
				{
					for (int i=i1, ii=0;i<=i2; i++, ii++)
					{
						values[k*(nGridX*nGridY)+j*nGridX + i] = vmod[kk*c*r+jj*r+ii];
					}
				}
			}
		}
		strMsg = _T("正在保存静态场数据文件，请稍等...");
		pMF->GetStatusBar().SetPaneText(0, strMsg);

		CString strName = newGUID();
		strName += _T(".grd");

		CString strNewFileName = pMF->GetProjectDatPath();
		strNewFileName += _T("\\models\\");
		strNewFileName += strName;

		CFile fileNew;
		if( fileNew.Open(strNewFileName, CFile::modeCreate | CFile::modeWrite ) )
		{
			CArchive archive(&fileNew, CArchive::store);

			CString strModelFileName = lpszModelFileName;

			archive << strModelFileName;

			int nSize = values.size();
			
			archive << nSize;

			for ( int i=0; i<nSize; i++)
			{
				archive << values[i];
			}
			archive << RGB(255,0,0);
			archive << RGB(255,255,255);
			archive << (double)0;
			archive << (double)-1;
			archive.Close();
			fileNew.Close();
		}

		HTREEITEM htiSon = m_wndModelView.GetChildNode(hItem,commType.m_strCommand);
		if( htiSon == NULL )
		{
			HTREEITEM hSrc = AddTreeItem(commType.m_strCommand, 2, 2, hItem);

			CTreeNodeDat *lpNodeDat = new CTreeNodeDat;

			CFileViewObj *pObj = new CFileViewObj;
			pObj->m_strFileName = strName;

			lpNodeDat->m_nImage			= 2;
			lpNodeDat->m_nSelectedImage	= 2;
			lpNodeDat->m_nType			= FARM_DAT;
			lpNodeDat->m_pNodeDat		= pObj;
			lpNodeDat->m_uState			= TVIS_EXPANDED;
			strName.MakeUpper();
			lpNodeDat->m_strFileName = strName;

			m_wndModelView.SetItemData(hSrc, (DWORD)lpNodeDat);

			m_wndModelView.Expand(hItem, TVE_EXPAND);

			for (int i=0;i<nGridZ; i++)
			{
				CString str;
				str.Format("# %d", i+1);

				HTREEITEM h = AddTreeItem(str, 3, 3, hSrc);

				CTreeNodeDat *lpNodeDat = new CTreeNodeDat;

				CFileViewObj *pObj = new CFileViewObj;
				pObj->m_strFileName = strName;

				lpNodeDat->m_nImage			= 3;
				lpNodeDat->m_nSelectedImage	= 3;
				lpNodeDat->m_nType			= FARM_LAYER;
				lpNodeDat->m_pNodeDat		= pObj;
				lpNodeDat->m_uState			= TVIS_EXPANDED;
				lpNodeDat->m_strFileName	= commType.m_strCommand;

				m_wndModelView.SetItemData(h, (DWORD)lpNodeDat);			
			}

			//m_wndModelView.Expand(hSrc, TVE_EXPAND);
		}
		else
		{
			CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)m_wndModelView.GetItemData(htiSon);

			if( lpNodeDat != NULL )
			{
				delete lpNodeDat;
				lpNodeDat = NULL;
			}

			lpNodeDat = new CTreeNodeDat;

			CFileViewObj *pObj = new CFileViewObj;
			pObj->m_strFileName = strName;

			lpNodeDat->m_nImage			= 2;
			lpNodeDat->m_nSelectedImage	= 2;
			lpNodeDat->m_nType			= FARM_DAT;
			lpNodeDat->m_pNodeDat		= pObj;
			lpNodeDat->m_uState			= TVIS_EXPANDED;
			strName.MakeUpper();
			lpNodeDat->m_strFileName = strName;

			m_wndModelView.SetItemData(htiSon, (DWORD)lpNodeDat);
		}
	}

	return true;
}


// 读取静态场数据并返回新的静态场文件名
bool CModelView::ReadGridEclipse(LPCSTR lpszGridFileName, LPCSTR lpszModelFileName, HTREEITEM hItem)
{
	CStdioFile fileOld;
	if( !fileOld.Open(lpszGridFileName, CFile::modeRead | CFile::typeText) )
		return false;
	if( fileOld.GetLength() < 1 ) // 如果文件长度为0 返回空的文件名
	{
		fileOld.Close();
		return false;
	}

	fileOld.Close();

	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();

	CWaitCursor wait;

	CString strModeFileName = pMF->GetProjectDatPath();
	strModeFileName += _T("\\models\\");
	strModeFileName += lpszModelFileName;

	
	int nGridX, nGridY, nGridZ;
	BOOL bCornerPointGrid;
	CFile gridfile;
	if( gridfile.Open(strModeFileName, CFile::modeRead | CFile::typeBinary ) )
	{
		CArchive ar(&gridfile,CArchive::load);
		CObject::Serialize(ar);
		ar.SerializeClass(RUNTIME_CLASS(CGridModel));
		ar >> bCornerPointGrid;
		ar >> nGridX;
		ar >> nGridY;
		ar >> nGridZ;
		ar.Close();
		gridfile.Close();
	}	

	CString strMsg;
	strMsg = _T("正在分析静态场数据文件，请稍等...");
	pMF->GetStatusBar().SetPaneText(0, strMsg);

	CString strTmp;

	std::vector<double> values;

	CParseEclipseFile eclipseFile;
	eclipseFile.OpenFile(lpszGridFileName);
	CSimuToken token;
	CEclipseCommandItem commType; // 数据类型

	do{
		CEclipseCommandItem commItem;
		token = eclipseFile.GetToken(commItem);
		if( token.token_type == CSimuToken::COMMAND )
		{
			switch(commItem.m_nCommID)
			{
			case CEclipseCommandItem::PORO:		// 有效厚度
			case CEclipseCommandItem::GENERAL:			// x方向渗透率
			case CEclipseCommandItem::NTG:
			case CEclipseCommandItem::PERMX:
			case CEclipseCommandItem::PERMY:
			case CEclipseCommandItem::SWAT:				
				if( values.size()> 0 )
				{
					strMsg = _T("正在保存静态场数据文件，请稍等...");
					pMF->GetStatusBar().SetPaneText(0, strMsg);

					CString strName = newGUID();
					strName += _T(".grd");

					CString strNewFileName = pMF->GetProjectDatPath();
					strNewFileName += _T("\\models\\");
					strNewFileName += strName;

					CFile fileNew;
					if( fileNew.Open(strNewFileName, CFile::modeCreate | CFile::modeWrite ) )
					{
						CArchive archive(&fileNew, CArchive::store);

						CString strModelFileName = lpszModelFileName;

						archive << strModelFileName;

						int nSize = values.size();

						archive << nSize;

						for ( int i=0; i<nSize; i++)
						{
							archive << values[i];
						}
						archive << RGB(255,0,0);
						archive << RGB(255,255,255);
						archive << (double)0;
						archive << (double)-1;
						archive.Close();
						fileNew.Close();
					}

					HTREEITEM htiSon = m_wndModelView.GetChildNode(hItem,commType.m_strCommand);
					if( htiSon == NULL )
					{
						htiSon = AddTreeItem(commType.m_strCommand, 2, 2, hItem);

						CTreeNodeDat *lpNodeDat = new CTreeNodeDat;

						CFileViewObj *pObj = new CFileViewObj;
						pObj->m_strFileName = strName;

						lpNodeDat->m_nImage			= 2;
						lpNodeDat->m_nSelectedImage	= 2;
						lpNodeDat->m_nType			= FARM_DAT;
						lpNodeDat->m_pNodeDat		= pObj;
						lpNodeDat->m_uState			= TVIS_EXPANDED;
						strName.MakeUpper();
						lpNodeDat->m_strFileName = strName;

						m_wndModelView.SetItemData(htiSon, (DWORD)lpNodeDat);

						m_wndModelView.Expand(hItem, TVE_EXPAND);

						for (int i=0;i<nGridZ; i++)
						{
							CString str;
							str.Format("# %d", i+1);

							HTREEITEM h = AddTreeItem(str, 3, 3, htiSon);

							CTreeNodeDat *lpNodeDat = new CTreeNodeDat;

							CFileViewObj *pObj = new CFileViewObj;
							pObj->m_strFileName = strName;

							lpNodeDat->m_nImage			= 3;
							lpNodeDat->m_nSelectedImage	= 3;
							lpNodeDat->m_nType			= FARM_LAYER;
							lpNodeDat->m_pNodeDat		= pObj;
							lpNodeDat->m_uState			= TVIS_EXPANDED;
							lpNodeDat->m_strFileName	= commType.m_strCommand;

							m_wndModelView.SetItemData(h, (DWORD)lpNodeDat);
						}

						//m_wndModelView.Expand(hSrc, TVE_EXPAND);
					}
					else
					{
						CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)m_wndModelView.GetItemData(htiSon);

						if( lpNodeDat != NULL )
						{
							delete lpNodeDat;
							lpNodeDat = NULL;
						}

						lpNodeDat = new CTreeNodeDat;

						CFileViewObj *pObj = new CFileViewObj;
						pObj->m_strFileName = strName;

						lpNodeDat->m_nImage			= 2;
						lpNodeDat->m_nSelectedImage	= 2;
						lpNodeDat->m_nType			= FARM_DAT;
						lpNodeDat->m_pNodeDat		= pObj;
						lpNodeDat->m_uState			= TVIS_EXPANDED;
						strName.MakeUpper();
						lpNodeDat->m_strFileName = strName;

						m_wndModelView.SetItemData(htiSon, (DWORD)lpNodeDat);
					}					

					values.erase(values.begin(), values.end());

					strMsg = _T("正在分析静态场数据文件，请稍等...");
				}

				commType = commItem;
				break;

			default:				
				if(values.size()>0)
				{					
					commType.m_nCommID = CEclipseCommandItem::UNKNOWN;
					strMsg = _T("正在保存静态场数据文件，请稍等...");
					pMF->GetStatusBar().SetPaneText(0, strMsg);

					CString strName = newGUID();
					strName += _T(".grd");

					CString strNewFileName = pMF->GetProjectDatPath();
					strNewFileName += _T("\\models\\");
					strNewFileName += strName;

					CFile fileNew;
					if( fileNew.Open(strNewFileName, CFile::modeCreate | CFile::modeWrite ) )
					{
						CArchive archive(&fileNew, CArchive::store);

						CString strModelFileName = lpszModelFileName;

						archive << strModelFileName;

						int nSize = values.size();
						
						archive << nSize;
						
						for ( int i=0; i<nSize; i++)
						{
							archive << values[i];
						}

						archive << RGB(255,0,0);
						archive << RGB(255,255,255);
						archive << (double)0;
						archive << (double)-1;
						archive.Close();
						fileNew.Close();
					}

					HTREEITEM htiSon = m_wndModelView.GetChildNode(hItem,commType.m_strCommand);
					if( htiSon == NULL )
					{
						htiSon = AddTreeItem(commType.m_strCommand, 2, 2, hItem);

						CTreeNodeDat *lpNodeDat = new CTreeNodeDat;

						CFileViewObj *pObj = new CFileViewObj;
						pObj->m_strFileName = strName;

						lpNodeDat->m_nImage			= 2;
						lpNodeDat->m_nSelectedImage	= 2;
						lpNodeDat->m_nType			= FARM_DAT;
						lpNodeDat->m_pNodeDat		= pObj;
						lpNodeDat->m_uState			= TVIS_EXPANDED;
						strName.MakeUpper();
						lpNodeDat->m_strFileName = strName;

						m_wndModelView.SetItemData(htiSon, (DWORD)lpNodeDat);

						m_wndModelView.Expand(hItem, TVE_EXPAND);

						for (int i=0;i<nGridZ; i++)
						{
							CString str;
							str.Format("# %d", i+1);

							HTREEITEM h = AddTreeItem(str, 3, 3, htiSon);

							CTreeNodeDat *lpNodeDat = new CTreeNodeDat;

							CFileViewObj *pObj = new CFileViewObj;
							pObj->m_strFileName = strName;

							lpNodeDat->m_nImage			= 3;
							lpNodeDat->m_nSelectedImage	= 3;
							lpNodeDat->m_nType			= FARM_LAYER;
							lpNodeDat->m_pNodeDat		= pObj;
							lpNodeDat->m_uState			= TVIS_EXPANDED;
							lpNodeDat->m_strFileName	= commType.m_strCommand;

							m_wndModelView.SetItemData(h, (DWORD)lpNodeDat);
						}

						//m_wndModelView.Expand(hSrc, TVE_EXPAND);
					}
					else
					{
						CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)m_wndModelView.GetItemData(htiSon);

						if( lpNodeDat != NULL )
						{
							delete lpNodeDat;
							lpNodeDat = NULL;
						}

						lpNodeDat = new CTreeNodeDat;

						CFileViewObj *pObj = new CFileViewObj;
						pObj->m_strFileName = strName;

						lpNodeDat->m_nImage			= 2;
						lpNodeDat->m_nSelectedImage	= 2;
						lpNodeDat->m_nType			= FARM_DAT;
						lpNodeDat->m_pNodeDat		= pObj;
						lpNodeDat->m_uState			= TVIS_EXPANDED;
						strName.MakeUpper();
						lpNodeDat->m_strFileName = strName;

						m_wndModelView.SetItemData(htiSon, (DWORD)lpNodeDat);
					}					

					values.erase(values.begin(), values.end());

					strMsg = "正在分析静态场数据文件，请稍等...";
				}
				break;
			}
		}		
		else if( token.token_type == CSimuToken::NUMBER )
		{
			CString strTmp = token.m_strToken;
			int index = strTmp.Find("*");

			switch(commType.m_nCommID)
			{
			case CEclipseCommandItem::PORO:		// 有效厚度
			case CEclipseCommandItem::GENERAL:	// x方向渗透率
			case CEclipseCommandItem::NTG:
			case CEclipseCommandItem::PERMX:
			case CEclipseCommandItem::PERMY:
			case CEclipseCommandItem::SWAT:	
				{					
					if(index > -1) // 是 2*0.5 格式
					{
						int n = atoi(strTmp.Left(index));
						double v = atof(strTmp.Right(strTmp.GetLength()-index+1));
						for (int i=0;i<n;i++)
							values.push_back(v);
					}
					else
					{
						double v = atof(strTmp);
						values.push_back(v);
					}
				}
				break;
			default:
				break;
			}			
		}
		else if( token.token_type == CSimuToken::STRING )
		{
			commType.m_nCommID = CEclipseCommandItem::UNKNOWN;
		}
		else
		{
			commType.m_nCommID = CEclipseCommandItem::UNKNOWN;
		}

	}while(token.token_type != CSimuToken::END);


	if( values.size() > 0 )
	{
		strMsg = _T("正在保存静态场数据文件，请稍等...");
		pMF->GetStatusBar().SetPaneText(0, strMsg);

		CString strName = newGUID();
		strName += _T(".grd");

		CString strNewFileName = pMF->GetProjectDatPath();
		strNewFileName += _T("\\models\\");
		strNewFileName += strName;

		CFile fileNew;
		if( fileNew.Open(strNewFileName, CFile::modeCreate | CFile::modeWrite ) )
		{
			CArchive archive(&fileNew, CArchive::store);

			CString strModelFileName = lpszModelFileName;

			archive << strModelFileName;

			int nSize = values.size();
			
			archive << nSize;

			for ( int i=0; i<nSize; i++)
			{
				archive << values[i];
			}
			archive << RGB(255,0,0);
			archive << RGB(255,255,255);
			archive << (double)0;
			archive << (double)-1;
			archive.Close();
			fileNew.Close();
		}

		HTREEITEM htiSon = m_wndModelView.GetChildNode(hItem,commType.m_strCommand);
		if( htiSon == NULL )
		{
			htiSon = AddTreeItem(commType.m_strCommand, 2, 2, hItem);

			CTreeNodeDat *lpNodeDat = new CTreeNodeDat;

			CFileViewObj *pObj = new CFileViewObj;
			pObj->m_strFileName = strName;

			lpNodeDat->m_nImage			= 2;
			lpNodeDat->m_nSelectedImage	= 2;
			lpNodeDat->m_nType			= FARM_DAT;
			lpNodeDat->m_pNodeDat		= pObj;
			lpNodeDat->m_uState			= TVIS_EXPANDED;
			strName.MakeUpper();
			lpNodeDat->m_strFileName = strName;

			m_wndModelView.SetItemData(htiSon, (DWORD)lpNodeDat);

			m_wndModelView.Expand(hItem, TVE_EXPAND);

			for (int i=0;i<nGridZ; i++)
			{
				CString str;
				str.Format("# %d", i+1);

				HTREEITEM h = AddTreeItem(str, 3, 3, htiSon);

				CTreeNodeDat *lpNodeDat = new CTreeNodeDat;
				
				CFileViewObj *pObj = new CFileViewObj;
				pObj->m_strFileName = strName;

				lpNodeDat->m_nImage			= 3;
				lpNodeDat->m_nSelectedImage	= 3;
				lpNodeDat->m_nType			= FARM_LAYER;
				lpNodeDat->m_pNodeDat		= pObj;
				lpNodeDat->m_uState			= TVIS_EXPANDED;
				lpNodeDat->m_strFileName	= commType.m_strCommand;

				m_wndModelView.SetItemData(h, (DWORD)lpNodeDat);			
			}

			//m_wndModelView.Expand(hSrc, TVE_EXPAND);
		}
		else
		{
			CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)m_wndModelView.GetItemData(htiSon);

			if( lpNodeDat != NULL )
			{
				delete lpNodeDat;
				lpNodeDat = NULL;
			}

			lpNodeDat = new CTreeNodeDat;

			CFileViewObj *pObj = new CFileViewObj;
			pObj->m_strFileName = strName;

			lpNodeDat->m_nImage			= 2;
			lpNodeDat->m_nSelectedImage	= 2;
			lpNodeDat->m_nType			= FARM_DAT;
			lpNodeDat->m_pNodeDat		= pObj;
			lpNodeDat->m_uState			= TVIS_EXPANDED;
			strName.MakeUpper();
			lpNodeDat->m_strFileName = strName;

			m_wndModelView.SetItemData(htiSon, (DWORD)lpNodeDat);
		}
	}

	return true;
}

//________________________________________________________________________
// 删除节点
void CModelView::OnEditClear()
{
	// TODO: 在此添加命令处理程序代码
	m_wndModelView.OnEditClear();
}

void CModelView::OnUpdateEditClear(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	m_wndModelView.OnUpdateEditClear(pCmdUI);
}

// 复制节点

void CModelView::OnEditCopy()
{
	// TODO: 在此添加命令处理程序代码
	m_wndModelView.OnEditCopy();
}

void CModelView::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	m_wndModelView.OnUpdateEditCopy(pCmdUI);
}

void CModelView::OnEditCut()
{
	// TODO: 在此添加命令处理程序代码
	m_wndModelView.OnEditCut();
}

void CModelView::OnUpdateEditCut(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	m_wndModelView.OnUpdateEditCut(pCmdUI);
}

void CModelView::OnEditPaste()
{
	// TODO: 在此添加命令处理程序代码
	m_wndModelView.OnEditPaste();
}

void CModelView::OnUpdateEditPaste(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	m_wndModelView.OnUpdateEditPaste(pCmdUI);
}

bool CModelView::OnImportInterlayer( LPCSTR strFileName, LPCSTR strFileTitle)
{
	HTREEITEM hItem = m_wndModelView.GetSelectedItem();
	if( hItem == NULL )
		hItem = TVI_ROOT;

	HTREEITEM h = AddTreeItem(strFileTitle, 1, 1, hItem);

	CTreeNodeDat *lpNodeDat = new CTreeNodeDat;

	CFileViewObj *pObj = new CFileViewObj;
	pObj->m_strFileName = strFileName;

	lpNodeDat->m_nImage			= 10;
	lpNodeDat->m_nSelectedImage	= 10;
	lpNodeDat->m_nType			= GRID_LAYER;
	lpNodeDat->m_pNodeDat		= pObj;
	lpNodeDat->m_uState			= TVIS_EXPANDED;
	CString tmp = strFileName;
	tmp.MakeUpper();
	strFileName = tmp;
	lpNodeDat->m_strFileName = strFileName;

	m_wndModelView.SetItemData(h, (DWORD)lpNodeDat);

	m_wndModelView.Expand(hItem, TVE_EXPAND);

	m_wndModelView.EditLabel(h);

	return true;
}
