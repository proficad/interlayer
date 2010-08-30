// LayerBar.cpp : implementation file
//

#include "stdafx.h"
#include "LayerBar.h"
#include "../Resource.h"
#include "GeoMapDoc.h"
#include "GeoMapView.h"
#include "DrawPage.h"
#include "DrawLayer.h"
#include "../MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDC_INPLACE_EDIT 102

/////////////////////////////////////////////////////////////////////////////
// CLayerBar

CLayerBar::CLayerBar()
{
}

CLayerBar::~CLayerBar()
{
}


BEGIN_MESSAGE_MAP(CLayerBar, CDockablePane)
	//{{AFX_MSG_MAP(CLayerBar)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_LAYER_MOVETOBACK, OnLayerMovetoback)
	ON_COMMAND(ID_LAYER_MOVEBACK, OnLayerMoveback)
	ON_UPDATE_COMMAND_UI(ID_LAYER_MOVEFORWARD, OnUpdateLayerMoveforward)
	ON_COMMAND(ID_LAYER_MOVETOFRONT, OnLayerMovetofront)
	ON_COMMAND(ID_LAYER_MOVEFORWARD, OnLayerMoveforward)
	ON_WM_DESTROY()
	ON_COMMAND(ID_DEL_LAYER, OnDelLayer)
	ON_COMMAND(ID_ADD_LAYER, OnAddLayer)
	ON_UPDATE_COMMAND_UI(ID_LAYER_MOVETOBACK, OnUpdateLayerMovetoback)
	ON_UPDATE_COMMAND_UI(ID_LAYER_MOVEBACK, OnUpdateLayerMoveback)
	ON_UPDATE_COMMAND_UI(ID_LAYER_MOVETOFRONT, OnUpdateLayerMovetofront)
	ON_UPDATE_COMMAND_UI(ID_DEL_LAYER, OnUpdateDelLayer)
	ON_COMMAND(ID_LAYER_SHOW, OnLayerShow)
	ON_COMMAND(ID_LAYER_HIDE, OnLayerHide)
	ON_COMMAND(ID_LAYER_LOCK, OnLayerLock)
	ON_COMMAND(ID_LAYER_UNLOCK, OnLayerUnlock)
	ON_COMMAND(ID_LAYER_CLIP, OnLayerClip)
	ON_COMMAND(ID_LAYER_NOCLIP, OnLayerNoclip)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CLayerBar message handlers

//
int CLayerBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rectDummy;
	rectDummy.SetRectEmpty ();
	
	// Create view:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN |
		WS_CLIPSIBLINGS| LVS_REPORT | LVS_SINGLESEL
		| LVS_OWNERDRAWFIXED | WS_TABSTOP ;
	
	if (!m_wndListCtrl.Create (dwViewStyle, rectDummy, this, 4))
	{
		TRACE0("Failed to create solution explorer\n");
		return -1;      // fail to create
	}

	m_Font.CreateStockObject (DEFAULT_GUI_FONT);
	m_wndListCtrl.SetFont(&m_Font);
	
	DWORD dExStyle=m_wndListCtrl.GetExtendedStyle();
	dExStyle |=LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT;
	m_wndListCtrl.SetExtendedStyle(dExStyle);
	
	m_wndListCtrl.ModifyStyle(0,LVS_SHOWSELALWAYS);

	m_wndToolBar.Create (this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_TOOLBAR_LAYER);
	m_wndToolBar.LoadToolBar (IDR_TOOLBAR_LAYER, 0, 0, TRUE /* Is locked */);
	
	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);
	m_wndToolBar.SetLockedSizes(CSize(17,17),CSize(13,13));

	// 所有命令将通过此控件路由，而不是通过主框架路由:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	// 填入一些静态树视图数据(此处只需填入虚拟代码，而不是复杂的数据)
	m_wndListCtrl.OnInitialUpdate();
	AdjustLayout();

	return 0;
}

void CLayerBar::OnChangeVisualStyle()
{
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(IDR_TOOLBAR_LAYER, 0, 0, TRUE /* 锁定 */);
}


// 调整工具栏
void CLayerBar::AdjustLayout()
{	
	if (GetSafeHwnd () == NULL)
	{
		return;
	}
	
	CRect rectClient;
	GetClientRect (rectClient);
	
	int cyTlb = m_wndToolBar.CalcFixedLayout (FALSE, TRUE).cy;
	
	m_wndToolBar.SetWindowPos (NULL, rectClient.left, rectClient.top, 
		rectClient.Width (), cyTlb,
		SWP_NOACTIVATE | SWP_NOZORDER);

	m_wndListCtrl.SetWindowPos (NULL, rectClient.left + 1, rectClient.top + cyTlb + 1,
								rectClient.Width () - 2, rectClient.Height () - cyTlb - 2,
								SWP_NOACTIVATE | SWP_NOZORDER);
}


void CLayerBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CRect rectTree;
	m_wndListCtrl.GetWindowRect (rectTree);
	ScreenToClient (rectTree);
	
	rectTree.InflateRect (1, 1);
	dc.Draw3dRect (rectTree, ::GetSysColor (COLOR_3DSHADOW), ::GetSysColor (COLOR_3DSHADOW));	
}

void CLayerBar::OnSize(UINT nType, int cx, int cy) 
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout ();
}

void CLayerBar::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndListCtrl.SetFocus();
}

void CLayerBar::OnAddLayer() 
{
	m_wndListCtrl.AddLayer();
}

void CLayerBar::OnDelLayer() 
{
	m_wndListCtrl.DelLayer();
}

void CLayerBar::OnUpdateDelLayer(CCmdUI* pCmdUI) 
{
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	CMDIChildWndEx *pWnd =(CMDIChildWndEx *) pMF->MDIGetActive();
	if( pWnd )
	{
		CGeoMapView*pView = (CGeoMapView*)pWnd->GetActiveView();
		if( pView && pView->IsKindOf(RUNTIME_CLASS(CGeoMapView)))
		{
			CGeoMapDoc*pDoc =(CGeoMapDoc*)pView->GetDocument();
			if( pDoc )
				pCmdUI->Enable(!pDoc->m_pCurDrawLayer->m_bLocked);
			else
				pCmdUI->Enable(FALSE);
		}
		else
			pCmdUI->Enable(FALSE);
	}
	else
		pCmdUI->Enable(FALSE);	
}

void CLayerBar::OnLayerMovetoback() 
{
	m_wndListCtrl.LayerToBack();

}
void CLayerBar::OnUpdateLayerMovetoback(CCmdUI* pCmdUI) 
{
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	CMDIChildWndEx *pWnd =(CMDIChildWndEx *) pMF->MDIGetActive();
	if( pWnd )
	{
		CGeoMapView*pView = (CGeoMapView*)pWnd->GetActiveView();
		if( pView && pView->IsKindOf(RUNTIME_CLASS(CGeoMapView)))
		{
			CGeoMapDoc*pDoc =(CGeoMapDoc*)pView->GetDocument();
			if( pDoc )
				pCmdUI->Enable(pDoc->m_pCurDrawLayer != pDoc->m_pCurDrawPage->m_DrawLayers.GetHead());
			else
				pCmdUI->Enable(FALSE);
		}
		else
			pCmdUI->Enable(FALSE);
	}
	else
		pCmdUI->Enable(FALSE);
}


void CLayerBar::OnLayerMoveback() 
{
	m_wndListCtrl.LayerNext();
	
}
void CLayerBar::OnUpdateLayerMoveback(CCmdUI* pCmdUI) 
{
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	CMDIChildWndEx *pWnd =(CMDIChildWndEx *) pMF->MDIGetActive();
	if( pWnd )
	{
		CGeoMapView*pView = (CGeoMapView*)pWnd->GetActiveView();
		if( pView && pView->IsKindOf(RUNTIME_CLASS(CGeoMapView)))
		{
			CGeoMapDoc*pDoc =(CGeoMapDoc*)pView->GetDocument();
			if( pDoc )
				pCmdUI->Enable(pDoc->m_pCurDrawLayer != pDoc->m_pCurDrawPage->m_DrawLayers.GetHead());
			else
				pCmdUI->Enable(FALSE);
		}
		else
			pCmdUI->Enable(FALSE);
	}
	else
		pCmdUI->Enable(FALSE);

}

void CLayerBar::OnLayerMoveforward() 
{
	m_wndListCtrl.LayerPrev();
}

void CLayerBar::OnUpdateLayerMoveforward(CCmdUI* pCmdUI) 
{
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	CMDIChildWndEx *pWnd =(CMDIChildWndEx *) pMF->MDIGetActive();
	if( pWnd )
	{
		CGeoMapView*pView = (CGeoMapView*)pWnd->GetActiveView();
		if( pView && pView->IsKindOf(RUNTIME_CLASS(CGeoMapView)))
		{
			CGeoMapDoc*pDoc =(CGeoMapDoc*)pView->GetDocument();
			if( pDoc )
				pCmdUI->Enable(pDoc->m_pCurDrawLayer != pDoc->m_pCurDrawPage->m_DrawLayers.GetTail());
			else
				pCmdUI->Enable(FALSE);
		}
		else
			pCmdUI->Enable(FALSE);
	}
	else
		pCmdUI->Enable(FALSE);
}


void CLayerBar::OnLayerMovetofront() 
{
	m_wndListCtrl.LayerToFront();
}

void CLayerBar::OnUpdateLayerMovetofront(CCmdUI* pCmdUI) 
{
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	CMDIChildWndEx *pWnd =(CMDIChildWndEx *) pMF->MDIGetActive();
	if( pWnd )
	{
		CGeoMapView*pView = (CGeoMapView*)pWnd->GetActiveView();
		if( pView && pView->IsKindOf(RUNTIME_CLASS(CGeoMapView)))
		{
			CGeoMapDoc*pDoc =(CGeoMapDoc*)pView->GetDocument();
			if( pDoc )
				pCmdUI->Enable(pDoc->m_pCurDrawLayer != pDoc->m_pCurDrawPage->m_DrawLayers.GetTail());
			else
				pCmdUI->Enable(FALSE);
		}
		else
			pCmdUI->Enable(FALSE);
	}
	else
		pCmdUI->Enable(FALSE);
	
}

void CLayerBar::OnDestroy() 
{
	CDockablePane::OnDestroy();	
}


void CLayerBar::OnLayerShow() 
{
	// TODO: Add your command handler code here
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	CMDIChildWndEx *pWnd =(CMDIChildWndEx *) pMF->MDIGetActive();
	
	if( !pWnd )
		return;
	
	CGeoMapView*pView = (CGeoMapView*)pWnd->GetActiveView();
	
	if( !pView || !pView->IsKindOf(RUNTIME_CLASS(CGeoMapView)))
		return;
	
	CGeoMapDoc*pDoc =(CGeoMapDoc*)pView->GetDocument();
	
	if( !pDoc )
		return;

	if( !pDoc->m_pCurDrawPage )
		return;

	POSITION pos = pDoc->m_pCurDrawPage->m_DrawLayers.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawLayer* pLayer = pDoc->m_pCurDrawPage->m_DrawLayers.GetNext(pos);
		pLayer->m_bShow = TRUE;	
		//pView->InvalLayer(pLayer);
	}

	int n = m_wndListCtrl.GetItemCount();
	for (int i=0;i<n; i++)
	{
		CLayerCell *pCell = (CLayerCell *)m_wndListCtrl.GetItemData(i);
		pCell->m_bShow = TRUE;
	}
	m_wndListCtrl.Invalidate();

	pDoc->SetModifiedFlag();

	pView->Invalidate(FALSE);
}

void CLayerBar::OnLayerHide() 
{
	// TODO: Add your command handler code here
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	CMDIChildWndEx *pWnd =(CMDIChildWndEx *) pMF->MDIGetActive();
	
	if( !pWnd )
		return;
	
	CGeoMapView*pView = (CGeoMapView*)pWnd->GetActiveView();
	
	if( !pView || !pView->IsKindOf(RUNTIME_CLASS(CGeoMapView)))
		return;
	
	CGeoMapDoc*pDoc =(CGeoMapDoc*)pView->GetDocument();
	
	if( !pDoc )
		return;
	
	if( !pDoc->m_pCurDrawPage )
		return;
	
	POSITION pos = pDoc->m_pCurDrawPage->m_DrawLayers.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawLayer* pLayer = pDoc->m_pCurDrawPage->m_DrawLayers.GetNext(pos);
		pLayer->m_bShow = FALSE;	
		//pView->InvalLayer(pLayer);
	}
	
	int n = m_wndListCtrl.GetItemCount();
	for (int i=0;i<n; i++)
	{
		CLayerCell *pCell = (CLayerCell *)m_wndListCtrl.GetItemData(i);
		pCell->m_bShow = FALSE;
	}
	m_wndListCtrl.Invalidate();

	pDoc->SetModifiedFlag();
	pView->Invalidate(FALSE);
}

void CLayerBar::OnLayerLock() 
{
	// TODO: Add your command handler code here
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	CMDIChildWndEx *pWnd =(CMDIChildWndEx *) pMF->MDIGetActive();
	
	if( !pWnd )
		return;
	
	CGeoMapView*pView = (CGeoMapView*)pWnd->GetActiveView();
	
	if( !pView || !pView->IsKindOf(RUNTIME_CLASS(CGeoMapView)))
		return;
	
	CGeoMapDoc*pDoc =(CGeoMapDoc*)pView->GetDocument();
	
	if( !pDoc )
		return;
	
	if( !pDoc->m_pCurDrawPage )
		return;
	
	POSITION pos = pDoc->m_pCurDrawPage->m_DrawLayers.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawLayer* pLayer = pDoc->m_pCurDrawPage->m_DrawLayers.GetNext(pos);
		pLayer->m_bLocked = TRUE;	
	//	pView->InvalLayer(pLayer);
	}
	
	int n = m_wndListCtrl.GetItemCount();
	for (int i=0;i<n; i++)
	{
		CLayerCell *pCell = (CLayerCell *)m_wndListCtrl.GetItemData(i);
		pCell->m_bLocked = TRUE;
	}
	m_wndListCtrl.Invalidate();

	pDoc->SetModifiedFlag();
	pView->Invalidate(FALSE);
}

void CLayerBar::OnLayerUnlock() 
{
	// TODO: Add your command handler code here
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	CMDIChildWndEx *pWnd =(CMDIChildWndEx *) pMF->MDIGetActive();
	
	if( !pWnd )
		return;
	
	CGeoMapView*pView = (CGeoMapView*)pWnd->GetActiveView();
	
	if( !pView || !pView->IsKindOf(RUNTIME_CLASS(CGeoMapView)))
		return;
	
	CGeoMapDoc*pDoc =(CGeoMapDoc*)pView->GetDocument();
	
	if( !pDoc )
		return;
	
	if( !pDoc->m_pCurDrawPage )
		return;
	
	POSITION pos = pDoc->m_pCurDrawPage->m_DrawLayers.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawLayer* pLayer = pDoc->m_pCurDrawPage->m_DrawLayers.GetNext(pos);
		pLayer->m_bLocked = FALSE;	
	//	pView->InvalLayer(pLayer);
	}
	
	int n = m_wndListCtrl.GetItemCount();
	for (int i=0;i<n; i++)
	{
		CLayerCell *pCell = (CLayerCell *)m_wndListCtrl.GetItemData(i);
		pCell->m_bLocked = FALSE;
	}
	m_wndListCtrl.Invalidate();

	pDoc->SetModifiedFlag();
	pView->Invalidate(FALSE);
}

void CLayerBar::OnLayerClip() 
{
	// TODO: Add your command handler code here
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	CMDIChildWndEx *pWnd =(CMDIChildWndEx *) pMF->MDIGetActive();
	
	if( !pWnd )
		return;
	
	CGeoMapView*pView = (CGeoMapView*)pWnd->GetActiveView();
	
	if( !pView || !pView->IsKindOf(RUNTIME_CLASS(CGeoMapView)))
		return;
	
	CGeoMapDoc*pDoc =(CGeoMapDoc*)pView->GetDocument();
	
	if( !pDoc )
		return;
	
	if( !pDoc->m_pCurDrawPage )
		return;
	
	POSITION pos = pDoc->m_pCurDrawPage->m_DrawLayers.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawLayer* pLayer = pDoc->m_pCurDrawPage->m_DrawLayers.GetNext(pos);
		pLayer->m_bClipping = TRUE;	
		//	pView->InvalLayer(pLayer);
	}
	
	int n = m_wndListCtrl.GetItemCount();
	for (int i=0;i<n; i++)
	{
		CLayerCell *pCell = (CLayerCell *)m_wndListCtrl.GetItemData(i);
		pCell->m_bClipping = TRUE;
	}
	m_wndListCtrl.Invalidate();
	
	pDoc->SetModifiedFlag();
	pView->Invalidate(FALSE);
}

void CLayerBar::OnLayerNoclip() 
{
	// TODO: Add your command handler code here
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	CMDIChildWndEx *pWnd =(CMDIChildWndEx *) pMF->MDIGetActive();
	
	if( !pWnd )
		return;
	
	CGeoMapView*pView = (CGeoMapView*)pWnd->GetActiveView();
	
	if( !pView || !pView->IsKindOf(RUNTIME_CLASS(CGeoMapView)))
		return;
	
	CGeoMapDoc*pDoc =(CGeoMapDoc*)pView->GetDocument();
	
	if( !pDoc )
		return;
	
	if( !pDoc->m_pCurDrawPage )
		return;
	
	POSITION pos = pDoc->m_pCurDrawPage->m_DrawLayers.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawLayer* pLayer = pDoc->m_pCurDrawPage->m_DrawLayers.GetNext(pos);
		pLayer->m_bClipping = FALSE;	
		//	pView->InvalLayer(pLayer);
	}
	
	int n = m_wndListCtrl.GetItemCount();
	for (int i=0;i<n; i++)
	{
		CLayerCell *pCell = (CLayerCell *)m_wndListCtrl.GetItemData(i);
		pCell->m_bClipping = FALSE;
	}
	m_wndListCtrl.Invalidate();
	
	pDoc->SetModifiedFlag();
	pView->Invalidate(FALSE);
}
