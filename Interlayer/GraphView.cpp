// 2DView.cpp : 实现文件
//

#include "stdafx.h"
#include "Interlayer.h"
#include "GraphView.h"
#include "MainFrm.h"
#include "GeoMap/GeoMapView.h"
#include "3DLib/3DModelView.h"
// CGraphView

CGraphView::CGraphView()
{
	m_wndGraphView.m_eTreeType = CViewTree::TREETYPE::graphTree;
}

CGraphView::~CGraphView()
{
}

BEGIN_MESSAGE_MAP(CGraphView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_OPEN, OnFileOpen)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_COMMAND(IDC_EXPAND_ALL_TREE, OnExpandAllTree)
	ON_COMMAND(ID_COLLAPSE_ALL_ENTIRE_TREE, OnCollapseAllEntireTree)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, OnUpdateFileNew)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_RENAME, OnEditRename)
	ON_UPDATE_COMMAND_UI(ID_EDIT_RENAME, OnUpdateEditRename)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar 消息处理程序

int CGraphView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建视图:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | WS_DISABLED
		| TVS_HASLINES | TVS_LINESATROOT 
		| TVS_HASBUTTONS | TVS_TRACKSELECT | TVS_EDITLABELS;

	if (!m_wndGraphView.Create(dwViewStyle, rectDummy, this, 4))
	{
		TRACE0("未能创建2D视图\n");
		return -1;      // 未能创建
	}

	m_wndGraphView.pfOpenTree = (CViewTree::OPENTREE_FUNCTION)&CGraphView::OpenTree;

	// 加载视图图像:
	m_GraphViewImages.Create(IDB_GRAPH_VIEW, 16, 0, RGB(255, 0, 255));
	m_wndGraphView.SetImageList(&m_GraphViewImages, TVSIL_NORMAL);
	
	OnChangeVisualStyle();
	
	FillGraphView();
	AdjustLayout();

	return 0;
}

bool CGraphView::OpenTree(CTreeNodeDat *lpNodeDat, void *lpVoid)
{
	CString strTitle = (LPCSTR)lpVoid;

	switch ( lpNodeDat->m_nType )
	{
	case FILE_2D:
		{
			CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
			CString strFileName = pMF->GetProjectDatPath();
			strFileName += _T("\\graphics\\");
			strFileName += ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;
			if( !PathFileExists(strFileName) )
			{
				AfxMessageBox(_T("此文件不存在!"));
			}
			else
			{
				CDocument *pDoc = pMF->CreateOrActivateFrame(theApp.m_pGeoMapDocTemplate, RUNTIME_CLASS(CGeoMapView), strFileName);
				if( pDoc ) 
				{
					pDoc->SetPathName(strFileName);
					pDoc->SetTitle(strTitle);
				}
			}
		}
		break;
	case FILE_3D:
		{
			CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
			CString strFileName = pMF->GetProjectDatPath();
			strFileName += _T("\\graphics\\");
			strFileName += ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;
			if( !PathFileExists(strFileName) )
			{
				AfxMessageBox(_T("此文件不存在!"));
			}
			else
			{
				CDocument *pDoc = pMF->CreateOrActivateFrame(theApp.m_p3DModelDocTemplate, RUNTIME_CLASS(C3DModelView), strFileName);
				if( pDoc ) 
				{
					pDoc->SetPathName(strFileName);
					pDoc->SetTitle(strTitle);
				}
			}
		}
		break;
	case FILE_LOG:
		{
			CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
			CString strFileName = pMF->GetProjectDatPath();
			strFileName += _T("\\graphics\\");
			strFileName += ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;
			if( !PathFileExists(strFileName) )
			{
				AfxMessageBox(_T("此文件不存在!"));
			}
			else
			{
				CDocument *pDoc = pMF->CreateOrActivateFrame(theApp.m_pLoggingDocTemplate, NULL, strFileName);
				if( pDoc ) 
				{
					pDoc->SetPathName(strFileName);
					pDoc->SetTitle(strTitle);
				}
			}
		}
		break;
	default:
		return false;
	}
	return true;
}


void CGraphView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CGraphView::FillGraphView(LPCTSTR lpszItem, int nImage, int nSelectedImage, 
						 HTREEITEM hParent/* = TVI_ROOT*/, 
						 HTREEITEM hInsertAfter/* = TVI_LAST*/)
{	
	
	HTREEITEM hSrc = m_wndGraphView.InsertItem(_T("二维视图"), 0, 0, TVI_ROOT, TVI_LAST);
	CTreeNodeDat *lpNodeDat = new CTreeNodeDat;

	lpNodeDat->m_nImage = 0;
	lpNodeDat->m_nSelectedImage = 0;
	lpNodeDat->m_nType = GRAPHICS_2DS;
	lpNodeDat->m_pNodeDat = NULL;
	
	m_wndGraphView.SetItemData(hSrc, (DWORD)lpNodeDat);

	HTREEITEM hInc = m_wndGraphView.InsertItem(_T("三维视图"), 0, 0, TVI_ROOT, TVI_LAST);

	lpNodeDat = new CTreeNodeDat;

	lpNodeDat->m_nImage = 0;
	lpNodeDat->m_nSelectedImage = 0;
	lpNodeDat->m_nType = GRAPHICS_3DS;
	lpNodeDat->m_pNodeDat = NULL;

	m_wndGraphView.SetItemData(hInc, (DWORD)lpNodeDat);

	hInc = m_wndGraphView.InsertItem(_T("测井曲线视图"), 0, 0, TVI_ROOT, TVI_LAST);

	lpNodeDat = new CTreeNodeDat;

	lpNodeDat->m_nImage = 0;
	lpNodeDat->m_nSelectedImage = 0;
	lpNodeDat->m_nType = GRAPHICS_LOGS;
	lpNodeDat->m_pNodeDat = NULL;

	m_wndGraphView.SetItemData(hInc, (DWORD)lpNodeDat);
	
	m_wndGraphView.Expand(hSrc, TVE_EXPAND);
	m_wndGraphView.Expand(hInc, TVE_EXPAND);
}

HTREEITEM CGraphView::AddTreeItem(LPCTSTR lpszItem, int nImage, int nSelectedImage, 
								 HTREEITEM hParent, 
								 HTREEITEM hInsertAfter)
{
	HTREEITEM hItem = m_wndGraphView.InsertItem(lpszItem, nImage, nSelectedImage, hParent, hInsertAfter);
	m_wndGraphView.SetModifiedFlag(true);
	return hItem;
}

void CGraphView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*) &m_wndGraphView;
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
		if( strMenuName == "_图形窗口管理器_" )
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

void CGraphView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	//int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	//m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	//m_wndGraphView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);

	m_wndGraphView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + 1, rectClient.Width() - 2, rectClient.Height() - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CGraphView::OnFileOpen()
{
	// TODO: 在此处添加命令处理程序代码

	HTREEITEM hItem = m_wndGraphView.GetSelectedItem();

	if( hItem == NULL )
		return;

	CString strTitle = m_wndGraphView.GetItemText(hItem);
	CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)m_wndGraphView.GetItemData(hItem);
	OpenTree(lpNodeDat, (LPVOID)(LPCSTR)strTitle);
}

void CGraphView::OnPaint()
{
	CPaintDC dc(this); // 用于绘制的设备上下文

	CRect rectTree;
	m_wndGraphView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CGraphView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndGraphView.SetFocus();
}

void CGraphView::OnChangeVisualStyle()
{
	//m_wndToolBar.CleanUpLockedImages();
	//m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_EXPLORER_24 : IDR_EXPLORER, 0, 0, TRUE /* 锁定 */);

	m_GraphViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_GRAPH_VIEW_24 : IDB_GRAPH_VIEW;

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

	m_GraphViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_GraphViewImages.Add(&bmp, RGB(255, 0, 255));

	m_wndGraphView.SetImageList(&m_GraphViewImages, TVSIL_NORMAL);
}

void CGraphView::LoadTree(CArchive& ar)
{
	m_wndGraphView.Serialize(ar);
}

void CGraphView::SaveTree(CArchive& ar)
{
	m_wndGraphView.Serialize(ar);

	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	CString strDirectory = pMF->GetProjectDatPath();
	CString strTmp = strDirectory;
	strTmp += _T("\\graphics");
	m_wndGraphView.DeleteUnusedFile(strTmp);
}

void CGraphView::EmptyTree()
{
	m_wndGraphView.DeleteItemM(TVI_ROOT);
}

void CGraphView::OnExpandAllTree()
{
	// TODO: 在此添加命令处理程序代码

	m_wndGraphView.OnExpandAllTree();
}

void CGraphView::OnCollapseAllEntireTree()
{
	// TODO: 在此添加命令处理程序代码
	m_wndGraphView.OnCollapseAllEntireTree();
}

void CGraphView::OnFileNew()
{
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	pMF->OnFileNew();
}

void CGraphView::OnUpdateFileNew(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码

	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	pMF->OnUpdateFileNew(pCmdUI);
}

//________________________________________________________________________
void CGraphView::OnEditRename()
{
	// TODO: 在此添加命令处理程序代码
	m_wndGraphView.OnRename();
}

void CGraphView::OnUpdateEditRename(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	m_wndGraphView.OnUpdateRename(pCmdUI);
}

// 删除节点
void CGraphView::OnEditClear()
{
	// TODO: 在此添加命令处理程序代码
	m_wndGraphView.OnEditClear();
}

void CGraphView::OnUpdateEditClear(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	m_wndGraphView.OnUpdateEditClear(pCmdUI);
}

// 复制节点

void CGraphView::OnEditCopy()
{
	// TODO: 在此添加命令处理程序代码
	m_wndGraphView.OnEditCopy();
}

void CGraphView::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	m_wndGraphView.OnUpdateEditCopy(pCmdUI);
}

void CGraphView::OnEditCut()
{
	// TODO: 在此添加命令处理程序代码
	m_wndGraphView.OnEditCut();
}

void CGraphView::OnUpdateEditCut(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	m_wndGraphView.OnUpdateEditCut(pCmdUI);
}

void CGraphView::OnEditPaste()
{
	// TODO: 在此添加命令处理程序代码
	m_wndGraphView.OnEditPaste();
}

void CGraphView::OnUpdateEditPaste(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	m_wndGraphView.OnUpdateEditPaste(pCmdUI);
}
