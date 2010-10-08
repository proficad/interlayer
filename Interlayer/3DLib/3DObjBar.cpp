// 3dLib\3DObjBar.cpp : 实现文件
//

#include "stdafx.h"
#include "3DObjBar.h"
#include "../Resource.h"
#include "../Interlayer.h"
#include "../IntersectSearchManager.h"
#include "../MainFrm.h"

// C3DObjBar

IMPLEMENT_DYNAMIC(C3DObjBar, CDockablePane)

C3DObjBar::C3DObjBar()
{

}

C3DObjBar::~C3DObjBar()
{
}


BEGIN_MESSAGE_MAP(C3DObjBar, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()

	ON_COMMAND(ID_NEW_3D, OnNew)
	ON_UPDATE_COMMAND_UI(ID_NEW_3D, OnUpdateNew)

	ON_COMMAND(ID_EDIT_RENAME, OnEditRename)
	ON_UPDATE_COMMAND_UI(ID_EDIT_RENAME, OnUpdateEditRename)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
	ON_COMMAND(ID_MATERIAL_SET, OnSetMaterial)
	ON_UPDATE_COMMAND_UI(ID_MATERIAL_SET, OnUpdateSetMaterial)
	ON_COMMAND(ID_COLOR_SET, OnSetColor)
	ON_UPDATE_COMMAND_UI(ID_COLOR_SET, OnUpdateSetColor)

END_MESSAGE_MAP()



// C3DObjBar 消息处理程序
int C3DObjBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建视图:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE
		| TVS_HASLINES 
		| TVS_LINESATROOT | TVS_HASBUTTONS 
		| TVS_CHECKBOXES | TVS_TRACKSELECT
		| TVS_EDITLABELS;

	if (!m_wndTree.Create(dwViewStyle, rectDummy, this, 4))
	{
		TRACE0("未能创建2D视图\n");
		return -1;      // 未能创建
	}
	
	m_TreeStateImages.Create(IDB_3D_OBJ_STATE, 16, 0, RGB(255, 255, 255));
	m_wndTree.SetImageList(&m_TreeStateImages, TVSIL_STATE);
	// 加载视图图像:
	m_Images.Create(IDB_3D_OBJ, 16, 0, RGB(255, 0, 0));
	m_wndTree.SetImageList(&m_Images, TVSIL_NORMAL);
	
	OnChangeVisualStyle();
	
	AdjustLayout();

	return 0;
}

HTREEITEM C3DObjBar::AddTreeItem(LPCTSTR lpszItem, int nImage, int nSelectedImage, 
								 HTREEITEM hParent, 
								 HTREEITEM hInsertAfter)
{
	HTREEITEM hItem = m_wndTree.InsertItem(lpszItem, nImage, nSelectedImage, hParent, hInsertAfter);
	return hItem;
}


void C3DObjBar::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}


void C3DObjBar::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);
	m_wndTree.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + 1, rectClient.Width() - 2, rectClient.Height() - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}


void C3DObjBar::OnChangeVisualStyle()
{
	m_Images.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_3D_OBJ_24 : IDB_3D_OBJ;

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

	m_Images.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_Images.Add(&bmp, RGB(255, 0, 0));

	m_wndTree.SetImageList(&m_Images, TVSIL_NORMAL);
	{	// 装入树状态图标
		m_TreeStateImages.DeleteImageList();	

		UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_3D_OBJ_STATE_24 : IDB_3D_OBJ_STATE;

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

		m_wndTree.SetImageList(&m_TreeStateImages, TVSIL_STATE);
	}
}

void C3DObjBar::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndTree.SetFocus();
}

void C3DObjBar::OnPaint()
{
	CPaintDC dc(this); // 用于绘制的设备上下文

	CRect rectTree;
	m_wndTree.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void C3DObjBar::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*) &m_wndTree;
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
		if( strMenuName == "_三维对象_" )
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

void C3DObjBar::OnNew()
{
	m_wndTree.OnNew();
}

void C3DObjBar::OnUpdateNew(CCmdUI *pCmdUI)
{
	m_wndTree.OnUpdateNew(pCmdUI);
}

void C3DObjBar::OnEditRename()
{
	// TODO: 在此添加命令处理程序代码
	m_wndTree.OnRename();
}

void C3DObjBar::OnUpdateEditRename(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	m_wndTree.OnUpdateRename(pCmdUI);
}

//________________________________________________________________________
// 删除节点
void C3DObjBar::OnEditClear()
{
	// TODO: 在此添加命令处理程序代码
	m_wndTree.OnEditClear();
}

void C3DObjBar::OnUpdateEditClear(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	m_wndTree.OnUpdateEditClear(pCmdUI);
}

void C3DObjBar::OnSetMaterial()
{
	m_wndTree.OnSetMaterial();
	
}

void C3DObjBar::OnUpdateSetMaterial(CCmdUI *pCmdUI)
{
	m_wndTree.OnUpdateSetMaterial(pCmdUI);
}

void C3DObjBar::OnSetColor()
{
	m_wndTree.OnSetColor();
}

void C3DObjBar::OnUpdateSetColor(CCmdUI *pCmdUI)
{
	m_wndTree.OnUpdateSetColor(pCmdUI);
}

IMPLEMENT_DYNAMIC(CLayerIntersectSearch, CDockablePane)

CLayerIntersectSearch::CLayerIntersectSearch()
{

}

CLayerIntersectSearch::~CLayerIntersectSearch()
{
}


BEGIN_MESSAGE_MAP(CLayerIntersectSearch, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()

	ON_COMMAND(ID_NEW_3D, OnNew)
	ON_UPDATE_COMMAND_UI(ID_NEW_3D, OnUpdateNew)

	ON_COMMAND(ID_EDIT_RENAME, OnEditRename)
	ON_UPDATE_COMMAND_UI(ID_EDIT_RENAME, OnUpdateEditRename)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
	ON_COMMAND(ID_MATERIAL_SET, OnSetMaterial)
	ON_UPDATE_COMMAND_UI(ID_MATERIAL_SET, OnUpdateSetMaterial)
	ON_COMMAND(ID_COLOR_SET, OnSetColor)
	ON_UPDATE_COMMAND_UI(ID_COLOR_SET, OnUpdateSetColor)
	ON_BN_CLICKED(IDC_BUTTON_INTERSECTSEARCH, OnClickButton)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_INTERSECTSEARCH, OnUpdateClickMyButton) 
END_MESSAGE_MAP()



// CLayerIntersectSearch 消息处理程序
int CLayerIntersectSearch::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建视图:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE
		| TVS_HASLINES 
		| TVS_LINESATROOT | TVS_HASBUTTONS 
		| TVS_CHECKBOXES | TVS_TRACKSELECT
		| TVS_EDITLABELS;

	if (!m_wndTree.Create(dwViewStyle, rectDummy, this, 4))
	{
		TRACE0("未能创建2D视图\n");
		return -1;      // 未能创建
	}

	if(!m_wndButton.Create(_T("查找相交模型网格"),WS_CHILD | WS_VISIBLE,rectDummy,this,IDC_BUTTON_INTERSECTSEARCH))
	{
		TRACE0("未能创建2D视图\n");
		return -1;      // 未能创建
	}
	m_TreeStateImages.Create(IDB_3D_OBJ_STATE, 16, 0, RGB(255, 255, 255));
	m_wndTree.SetImageList(&m_TreeStateImages, TVSIL_STATE);
	// 加载视图图像:
	m_Images.Create(IDB_3D_OBJ, 16, 0, RGB(255, 0, 0));
	m_wndTree.SetImageList(&m_Images, TVSIL_NORMAL);

	OnChangeVisualStyle();

	AdjustLayout();

	return 0;
}

HTREEITEM CLayerIntersectSearch::AddTreeItem(LPCTSTR lpszItem, int nImage, int nSelectedImage, 
								 HTREEITEM hParent, 
								 HTREEITEM hInsertAfter)
{
	HTREEITEM hItem = m_wndTree.InsertItem(lpszItem, nImage, nSelectedImage, hParent, hInsertAfter);
	return hItem;
}


void CLayerIntersectSearch::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}


void CLayerIntersectSearch::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);
	m_wndButton.SetWindowPos(NULL, rectClient.left + 1, 0, rectClient.Width() - 2, 50, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndTree.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + 51, rectClient.Width() - 2, rectClient.Height() - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}


void CLayerIntersectSearch::OnChangeVisualStyle()
{
	m_Images.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_3D_OBJ_24 : IDB_3D_OBJ;

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

	m_Images.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_Images.Add(&bmp, RGB(255, 0, 0));

	m_wndTree.SetImageList(&m_Images, TVSIL_NORMAL);
	{	// 装入树状态图标
		m_TreeStateImages.DeleteImageList();	

		UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_3D_OBJ_STATE_24 : IDB_3D_OBJ_STATE;

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

		m_wndTree.SetImageList(&m_TreeStateImages, TVSIL_STATE);
	}
}

void CLayerIntersectSearch::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndTree.SetFocus();
}

void CLayerIntersectSearch::OnPaint()
{
	CPaintDC dc(this); // 用于绘制的设备上下文

	CRect rectTree;
	m_wndTree.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CLayerIntersectSearch::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*) &m_wndTree;
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
		if( strMenuName == "_三维对象_" )
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

void CLayerIntersectSearch::OnNew()
{
	m_wndTree.OnNew();
}

void CLayerIntersectSearch::OnUpdateNew(CCmdUI *pCmdUI)
{
	m_wndTree.OnUpdateNew(pCmdUI);
}

void CLayerIntersectSearch::OnEditRename()
{
	// TODO: 在此添加命令处理程序代码
	m_wndTree.OnRename();
}

void CLayerIntersectSearch::OnUpdateEditRename(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	m_wndTree.OnUpdateRename(pCmdUI);
}

//________________________________________________________________________
// 删除节点
void CLayerIntersectSearch::OnEditClear()
{
	// TODO: 在此添加命令处理程序代码
	m_wndTree.OnEditClear();
}

void CLayerIntersectSearch::OnUpdateEditClear(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	m_wndTree.OnUpdateEditClear(pCmdUI);
}

void CLayerIntersectSearch::OnSetMaterial()
{
	m_wndTree.OnSetMaterial();

}

void CLayerIntersectSearch::OnUpdateSetMaterial(CCmdUI *pCmdUI)
{
	m_wndTree.OnUpdateSetMaterial(pCmdUI);
}

void CLayerIntersectSearch::OnSetColor()
{
	m_wndTree.OnSetColor();
}

void CLayerIntersectSearch::OnUpdateSetColor(CCmdUI *pCmdUI)
{
	m_wndTree.OnUpdateSetColor(pCmdUI);
}

void CLayerIntersectSearch::OnUpdateClickMyButton( CCmdUI* pCmdUI )
{
	pCmdUI->Enable();
}

void CLayerIntersectSearch::OnClickButton()
{
	CMainFrame *pMF = (CMainFrame *)AfxGetMainWnd();

	C3DObjBar*pBar = pMF->Get3DBar();
	CLayerIntersectSearch *pSearchbar = pMF->GetSearchBar();
	//CIntersectSearchManager::Instance()->ReleaseAll();
	//遍历管理器把模型和夹层保存并调用C#追夹层
	pBar->m_wndTree.DeleteAllItems();
	pBar->m_wndTree.FillTreeCtrl();
	pSearchbar->m_wndTree.DeleteAllItems();
	pSearchbar->m_wndTree.FillTreeCtrl();
}
