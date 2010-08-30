// LayerListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "LayerListCtrl.h"
#include "DrawLayer.h"
#include "DrawPage.h"
#include "GeoMapDoc.h"
#include "GeoMapView.h"
#include "../Resource.h"
#include "../Command/CmdActiveLayer.h"
#include "../Command/CmdMoveLayerBack.h"
#include "../Command/CmdMoveLayerForward.h"
#include "../MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLayerListCtrl

CLayerListCtrl::CLayerListCtrl()
{
	m_pCurPage = NULL;
	m_strCurPageID = _T("");		// 当前页面ID
	m_strCurLayerID = _T("");	// 当前图层ID
	m_iSelectedRow = -1;
	m_iSelectedCol = -1;
	m_iCurLayerID = -1;
	m_list.Create(MAKEINTRESOURCE(IDB_LAYER_IMAGES), 22, 1, RGB(255,255,255));
	m_pEdit = NULL;
}

CLayerListCtrl::~CLayerListCtrl()
{
	m_list.DeleteImageList();
}


BEGIN_MESSAGE_MAP(CLayerListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CLayerListCtrl)
	ON_WM_MEASUREITEM()
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_DESTROY()
	ON_WM_KILLFOCUS()
	ON_WM_CONTEXTMENU()
	ON_MESSAGE(WM_SETFONT, OnSetFont)
	ON_WM_MEASUREITEM_REFLECT()
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	//}}AFX_MSG_MAP
	ON_MESSAGE_VOID(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLayerListCtrl message handlers

void CLayerListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	CMDIChildWndEx *pWnd =(CMDIChildWndEx *) pMF->MDIGetActive();
	if( !pWnd )
		return;

	CGeoMapView *pView = (CGeoMapView *)pWnd->GetActiveView();
	if(!pView)
		return;

	if(!pView->IsKindOf(RUNTIME_CLASS(CGeoMapView)))
		return;

	int nCols = 5;

	CRect rect;

	CDC* pDC;
	pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	CLayerCell *pCell = (CLayerCell*)GetItemData(lpDrawItemStruct->itemID);

	for ( int nCol=0; nCol< nCols; nCol++)
	{
		if(nCol>0)
			GetSubItemRect(lpDrawItemStruct->itemID,nCol,LVIR_BOUNDS,rect);
		else
			GetItemRect(lpDrawItemStruct->itemID,&rect,LVIR_LABEL);

		pDC->FillSolidRect(rect,RGB(255,255,255));
		rect.DeflateRect(1,1,1,1);		
		pDC->DrawEdge(rect,BDR_RAISEDINNER |BDR_SUNKENOUTER ,BF_RECT);

		if( nCol == 4 )
		{
			pDC->SelectObject(GetStockObject(DEFAULT_GUI_FONT));
			rect.DeflateRect(4,0,0,0);
			
			CDrawLayer *pLayer = m_pCurPage->GetLayer(pCell->m_strLayerID);

			if( pLayer->m_bIsNew )
				pDC->SetTextColor( RGB(0,0,255));
			else
				pDC->SetTextColor( RGB(0,0,0));

			::DrawText(lpDrawItemStruct->hDC, pCell->m_strLayerName, pCell->m_strLayerName.GetLength(), 
				rect, DT_LEFT|DT_VCENTER|DT_SINGLELINE) ;

			pDC->SelectStockObject(SYSTEM_FONT) ;
		}
		else
		{
			int imageIndex = 2*nCol;
		
			if( (m_strCurLayerID != pCell->m_strLayerID && nCol == 0 )
				||(nCol == 1 && !pCell->m_bShow)
				||(nCol == 2 && !pCell->m_bLocked)
				||(nCol == 3 && !pCell->m_bClipping))
				imageIndex += 1;
			
			CPoint pt = rect.CenterPoint();
			pt.x -=11 ; pt.y -= 11;

			m_list.Draw(pDC, imageIndex, pt, ILD_NORMAL);
		}		
	}
}

LRESULT CLayerListCtrl::OnSetFont(WPARAM wParam, LPARAM lParam)
{
	CRect rc;
	GetWindowRect(&rc);

	WINDOWPOS wp;
	wp.hwnd = this->m_hWnd;
	wp.cx = rc.Width() ;
	wp.cy = rc.Height() ;
	wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;

	LRESULT lrReturn(Default());

	SendMessage(WM_WINDOWPOSCHANGED, 0, reinterpret_cast<LPARAM> (&wp));

	return lrReturn;
}

void CLayerListCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	// Get the LOGFONT for the current font.

	lpMeasureItemStruct->itemHeight = 24;
}

void CLayerListCtrl::OnInitialUpdate() 
{
	int i ;

	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	CString strHeader[5] = {"活动","显隐","状态","剪切","图层名称"};
	for(i = 0; i<5; i++)
	{
		lvc.iSubItem = i;
		lvc.pszText = (LPSTR)(LPCSTR)strHeader[i];
		
		if( i== 4 )
			lvc.cx = 150;
		else
			lvc.cx = 28;
		lvc.fmt = LVCFMT_LEFT;
		InsertColumn(i,&lvc);
	}

	SetFont(GetFont());
}


void CLayerListCtrl::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	CListCtrl::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

void CLayerListCtrl::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;

	if( m_pEdit && m_pEdit->IsWindowVisible() )
		EndEdit();

	NMITEMACTIVATE* nm=(NMITEMACTIVATE*)pNMHDR;	
	
	if( nm->iItem == -1 || nm->iSubItem == -1 )
		return;

	if( nm->iSubItem == 0 && m_iCurLayerID != nm->iItem)
	{		
		CDrawLayer *pLayer = m_pCurPage->GetLayer(nm->iItem);
		m_pCurPage->m_pDocument->Do(new CCmdActiveLayer(m_pCurPage->m_pDocument,pLayer,this,m_iCurLayerID,nm->iItem));		
	}

	else if(nm->iSubItem > 0 && nm->iItem != -1)
	{
		m_iSelectedRow=nm->iItem;
		m_iSelectedCol=nm->iSubItem;
		
		if( m_iSelectedCol!=4 )
		{
			CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
			CMDIChildWndEx *pWnd =(CMDIChildWndEx *) pMF->MDIGetActive();
			CGeoMapView *pView = (CGeoMapView *)pWnd->GetActiveView();	
			CGeoMapDoc *pDoc = (CGeoMapDoc *)pView ->GetDocument();	

			CLayerCell *pCell = (CLayerCell *)GetItemData(m_iSelectedRow);
			CDrawLayer *pLayer = m_pCurPage->GetLayer(pCell->m_strLayerID);
			if( m_iSelectedCol == 1 )
			{
 				if( pLayer->m_bShow )
 					m_pCurPage->m_pDocument->Do(
 					new CCmdSetLayerShowFlag(m_pCurPage->m_pDocument,pLayer,
 					this,m_iSelectedRow, TRUE, FALSE));
 				else
 					m_pCurPage->m_pDocument->Do(
 					new CCmdSetLayerShowFlag(m_pCurPage->m_pDocument,pLayer,
 					this,m_iSelectedRow, FALSE, TRUE));
			}
			else if( m_iSelectedCol == 2 )
			{
				if( pLayer->m_bLocked )
					m_pCurPage->m_pDocument->Do(
					new CCmdSetLayerLockedFlag(m_pCurPage->m_pDocument,pLayer,
					this,m_iSelectedRow, TRUE, FALSE));
				else
					m_pCurPage->m_pDocument->Do(
					new CCmdSetLayerLockedFlag(m_pCurPage->m_pDocument,pLayer,
					this,m_iSelectedRow, FALSE, TRUE));
			}

			else if( m_iSelectedCol == 3 )
			{
				if( pLayer->m_bClipping )
					m_pCurPage->m_pDocument->Do(
					new CCmdSetLayerClipFlag(m_pCurPage->m_pDocument,pLayer,
					this,m_iSelectedRow, TRUE, FALSE));
				else
					m_pCurPage->m_pDocument->Do(
					new CCmdSetLayerClipFlag(m_pCurPage->m_pDocument,pLayer,
					this,m_iSelectedRow, FALSE, TRUE));
			}
		}

		else
			OnEdit();		
	}	
}

void CLayerListCtrl::invalidate_grid(int row, int col)
{
	CRect r;
	
	if(col==0)
		GetItemRect(row,&r,LVIR_LABEL);
	else
		GetSubItemRect(row,col,LVIR_BOUNDS,r);
	
	InvalidateRect(&r);
}

void CLayerListCtrl::OnEdit()
{
	CLayerCell *pCell = (CLayerCell*) GetItemData(m_iSelectedRow);
	if( pCell->m_bLocked )
		return;

	if(!m_pEdit) //If Edit box does not exist create one
	{
		m_pEdit = new CEditLayer;
		DWORD dwStyle = WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_TABSTOP|ES_LEFT|ES_AUTOHSCROLL; 
		CRect rect;
		GetSubItemRect(m_iSelectedRow,m_iSelectedCol,LVIR_LABEL,rect);
		rect.DeflateRect(4,4,4,4);
		CString strLayerName = GetItemText(m_iSelectedRow,m_iSelectedCol);
		m_pEdit->Create(dwStyle,rect,this,1001);
		m_pEdit->SetFont(GetFont());
		m_pEdit->ShowWindow(SW_SHOW);
		m_pEdit->SetWindowText(strLayerName);
		m_pEdit->SetSel(0,-1);
		m_pEdit->SetFocus();
	}
	else
	{
		CRect rect;
		GetSubItemRect(m_iSelectedRow,m_iSelectedCol,LVIR_LABEL,rect);
		rect.DeflateRect(4,4,4,4);
		CString strLayerName = GetItemText(m_iSelectedRow,m_iSelectedCol);
		m_pEdit->MoveWindow(rect);
		m_pEdit->ShowWindow(SW_SHOW);
		m_pEdit->SetWindowText(strLayerName);
		m_pEdit->SetSel(0,-1);
		m_pEdit->SetFocus();
	}
}


void CLayerListCtrl::EndEdit()
{
	m_pEdit->ShowWindow(SW_HIDE);
	CString strNewName;
	m_pEdit->GetWindowText(strNewName);
	
	CString strOldName;
	strOldName = GetItemText(m_iSelectedRow,4);
	
	CDrawLayer *pLayer = m_pCurPage->GetLayer(m_iSelectedRow);
	m_pCurPage->m_pDocument->Do(new CCmdRenameLayer(m_pCurPage->m_pDocument,pLayer,this,m_iSelectedRow, strOldName, strNewName));
}


void CLayerListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	if( m_pEdit && m_pEdit->IsWindowVisible() )
		EndEdit();
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CLayerListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	if( m_pEdit && m_pEdit->IsWindowVisible() )
		EndEdit();
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CLayerListCtrl::FillLayerList()
{
	m_strCurLayerID = m_pCurPage->GetCurLayer()->m_strID;
	int nCount = m_pCurPage->GetLayerCount();
	for (int i=0; i<nCount; i++)
	{
		CDrawLayer *pLayer = m_pCurPage->GetLayer(nCount - i - 1);

		CLayerCell *pCell = new CLayerCell(pLayer->m_strLayerName, pLayer->m_strID);

		if( m_strCurLayerID == pLayer->m_strID )
			m_iCurLayerID = i;
		
		pCell->m_bShow = pLayer->m_bShow;
		pCell->m_bLocked = pLayer->m_bLocked;
		pCell->m_bClipping = pLayer->m_bClipping;
	
		InsertItem(i,"");
		SetItemText(i,4,pLayer->m_strLayerName);
		SetItemData(i, (DWORD)pCell);
	}
}

void CLayerListCtrl::DeleteLayerAll()
{
	int nCount = GetItemCount();
	for (int i=0; i<nCount; i++)
	{
		CLayerCell *pCell = (CLayerCell *)GetItemData(i);
		delete pCell;
	}
	
	DeleteAllItems();
}

void CLayerListCtrl::OnIdleUpdateCmdUI()
{
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	CMDIChildWndEx *pWnd =(CMDIChildWndEx *) pMF->MDIGetActive();
	if( !pWnd )
	{
		DeleteLayerAll();
		m_pCurPage = NULL;
		return;
	}

	CGeoMapView *pView = (CGeoMapView *)pWnd->GetActiveView();
	if(!pView)
	{
		DeleteLayerAll();
		m_pCurPage = NULL;
		return;
	}
	
	if(!pView->IsKindOf(RUNTIME_CLASS(CGeoMapView)))
	{
		DeleteLayerAll();
		m_pCurPage = NULL;
		return;
	}
	
	CGeoMapDoc *pDoc = (CGeoMapDoc *)pView ->GetDocument();
	
	if(m_strCurPageID != pDoc->m_strCurPageID 
		|| pDoc->m_pCurDrawPage != m_pCurPage)
	{
		DeleteLayerAll();
		m_strCurPageID = pDoc->m_strCurPageID;
		m_pCurPage = pDoc->m_pCurDrawPage;
		FillLayerList();
	}
}

void CLayerListCtrl::Refresh()
{
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	CMDIChildWndEx *pWnd =(CMDIChildWndEx *) pMF->MDIGetActive();
	if( !pWnd )
	{
		DeleteLayerAll();
		m_pCurPage = NULL;
		return;
	}

	CGeoMapView *pView = (CGeoMapView *)pWnd->GetActiveView();
	if(!pView)
	{
		DeleteLayerAll();
		m_pCurPage = NULL;
		return;
	}

	if(!pView->IsKindOf(RUNTIME_CLASS(CGeoMapView)))
	{
		DeleteLayerAll();
		m_pCurPage = NULL;
		return;
	}

	CGeoMapDoc *pDoc = (CGeoMapDoc *)pView ->GetDocument();

	DeleteLayerAll();
	m_strCurPageID = pDoc->m_strCurPageID;
	m_pCurPage = pDoc->m_pCurDrawPage;
	FillLayerList();
}

void CLayerListCtrl::AddLayer()
{
	CDrawLayer *pLayer = new CDrawLayer(m_pCurPage->m_pDocument);
	pLayer->m_strLayerName.Format("图层 %d",m_pCurPage->GetLayerCount()+1);
	m_pCurPage->m_pDocument->Do(new CCmdAddLayer(m_pCurPage->m_pDocument,pLayer, this));
}

BOOL CLayerListCtrl::AddLayer(CDrawLayer *pLayer)
{
	if(m_pCurPage == NULL )
	{
		DeleteLayerAll();
		m_strCurPageID = pLayer->m_pDocument->m_strCurPageID;
		m_pCurPage = pLayer->m_pDocument->m_pCurDrawPage;
		FillLayerList();
	}
	m_pCurPage->Add(pLayer);
	m_pCurPage->m_pDocument->m_pCurDrawLayer = m_pCurPage->GetCurLayer();
	m_strCurLayerID = pLayer->m_strID;
	
	CLayerCell *pCell = new CLayerCell(pLayer->m_strLayerName, pLayer->m_strID);
	
	m_iCurLayerID = 0;
	
	pCell->m_bShow = pLayer->m_bShow;
	pCell->m_bLocked = pLayer->m_bLocked;
	pCell->m_bClipping = pLayer->m_bClipping;
	
	InsertItem(0,"");
	SetItemText(0,4,pLayer->m_strLayerName);
	SetItemData(0, (DWORD)pCell);
	
	return TRUE;
}


void CLayerListCtrl::DelLayer()
{
	CDrawLayer *pLayer = m_pCurPage->GetLayer(m_strCurLayerID);
	m_pCurPage->m_pDocument->Do(new CCmdDelLayer(m_pCurPage->m_pDocument,pLayer, this));
}

BOOL CLayerListCtrl::DelLayer(CDrawLayer *pLayer)
{
	if( m_strCurLayerID == pLayer->m_strID )
	{
		if( !m_pCurPage->Remove(pLayer) )
			return FALSE;
		
		int nCount = GetItemCount();
		
		for (int i = 0; i<nCount; i++)
		{
			CLayerCell *pCell = (CLayerCell*)GetItemData(i);
			if( m_strCurLayerID == pCell->m_strLayerID )
			{
				delete pCell;
				DeleteItem(i);
				
				if( nCount == 1 )
				{
					CDrawLayer *pLayer = m_pCurPage->GetCurLayer();
					m_iCurLayerID = i;
					CLayerCell *pCell = new CLayerCell(pLayer->m_strLayerName, pLayer->m_strID);
					m_strCurLayerID = pLayer->m_strID;
					pCell->m_bShow = pLayer->m_bShow;
					pCell->m_bLocked = pLayer->m_bLocked;
					pCell->m_bClipping = pLayer->m_bClipping;
					InsertItem(m_iCurLayerID,"");
					SetItemText(m_iCurLayerID,4,pLayer->m_strLayerName);
					SetItemData(m_iCurLayerID, (DWORD)pCell);
				}
				else if( i == 0)
				{
					m_iCurLayerID = i;
					pCell = (CLayerCell*)GetItemData(m_iCurLayerID);
					m_strCurLayerID = pCell->m_strLayerID;
					Invalidate(FALSE);
				}
				else
				{
					m_iCurLayerID = i-1;
					pCell = (CLayerCell*)GetItemData(m_iCurLayerID);
					m_strCurLayerID = pCell->m_strLayerID;
					Invalidate(FALSE);
				}
				
				break;
			}
		}
	}
	else
		return FALSE;

	return TRUE;
}


// 移到最前面
void CLayerListCtrl::LayerToFront()
{
	CGeoMapDoc*pDoc = m_pCurPage->m_pDocument;
	CDrawLayer *pLayer = m_pCurPage->GetLayer(m_strCurLayerID);
	pDoc->Do(new CCmdMoveLayerToFront(pDoc,pLayer,this));
}

BOOL CLayerListCtrl::LayerToFront(CDrawLayer *pLayer)
{
	if( !m_pCurPage->MoveToFront(pLayer) )
		return FALSE;
	
	m_pCurPage->SetCurLayer(pLayer);

	int nCount = GetItemCount();
	
	for (int i = 0; i<nCount; i++)
	{
		CLayerCell *pCell = (CLayerCell*)GetItemData(i);
		if( m_strCurLayerID == pCell->m_strLayerID )
		{			
			DeleteItem(i);
			
			m_iCurLayerID = 0;
			
			InsertItem(m_iCurLayerID,"");
			SetItemText(m_iCurLayerID,4,pLayer->m_strLayerName);
			SetItemData(m_iCurLayerID, (DWORD)pCell);
			m_strCurLayerID = pLayer->m_strID;
			m_pCurPage->m_pDocument->m_pCurDrawLayer = pLayer;
			return TRUE;
		}
	}
	
	return FALSE;
}


void CLayerListCtrl::LayerToBack()
{
	CGeoMapDoc*pDoc = m_pCurPage->m_pDocument;
	CDrawLayer *pLayer = m_pCurPage->GetLayer(m_strCurLayerID);
	pDoc->Do(new CCmdMoveLayerToBack(pDoc,pLayer,this));
}

BOOL CLayerListCtrl::LayerToBack(CDrawLayer *pLayer)
{
	if( !m_pCurPage->MoveToBack(pLayer) )
		return FALSE;
	m_pCurPage->SetCurLayer(pLayer);

	int nCount = GetItemCount();
	
	for (int i = 0; i<nCount; i++)
	{
		CLayerCell *pCell = (CLayerCell*)GetItemData(i);
		if( m_strCurLayerID == pCell->m_strLayerID )
		{			
			DeleteItem(i);
			
			m_iCurLayerID = GetItemCount();
			
			InsertItem(m_iCurLayerID,"");
			SetItemText(m_iCurLayerID,4,pLayer->m_strLayerName);
			SetItemData(m_iCurLayerID, (DWORD)pCell);
			m_strCurLayerID = pLayer->m_strID;
			m_pCurPage->m_pDocument->m_pCurDrawLayer = pLayer;
			return TRUE;
		}
	}

	return FALSE;
}


void CLayerListCtrl::LayerNext()
{
	CGeoMapDoc*pDoc = m_pCurPage->m_pDocument;
	CDrawLayer *pLayer = m_pCurPage->GetLayer(m_strCurLayerID);
	pDoc->Do(new CCmdMoveLayerBack(pDoc,pLayer,this));
}

BOOL CLayerListCtrl::LayerNext(CDrawLayer *pLayer)
{
	if( !m_pCurPage->MoveBack(pLayer) )
		return FALSE;
	m_pCurPage->SetCurLayer(pLayer);

	int nCount = GetItemCount();
	
	for (int i = 0; i<nCount; i++)
	{
		CLayerCell *pCell = (CLayerCell*)GetItemData(i);
		if( m_strCurLayerID == pCell->m_strLayerID )
		{			
			DeleteItem(i);
			
			m_iCurLayerID = i+1;
			
			InsertItem(m_iCurLayerID,"");
			SetItemText(m_iCurLayerID,4,pLayer->m_strLayerName);
			SetItemData(m_iCurLayerID, (DWORD)pCell);
			break;
		}
	}
	
	m_strCurLayerID = pLayer->m_strID;
	m_pCurPage->m_pDocument->m_pCurDrawLayer = pLayer;

	return TRUE;
}


void CLayerListCtrl::LayerPrev()
{
	CGeoMapDoc*pDoc = m_pCurPage->m_pDocument;
	CDrawLayer *pLayer = m_pCurPage->GetLayer(m_strCurLayerID);
	pDoc->Do(new CCmdMoveLayerForward(pDoc,pLayer,this));
}


BOOL CLayerListCtrl::LayerPrev(CDrawLayer *pLayer)
{
	if( !m_pCurPage->MoveForward(pLayer) )
		return FALSE;
	
	m_pCurPage->SetCurLayer(pLayer);
	int nCount = GetItemCount();
	
	for (int i = 0; i<nCount; i++)
	{
		CLayerCell *pCell = (CLayerCell*)GetItemData(i);
		if( m_strCurLayerID == pCell->m_strLayerID )
		{			
			DeleteItem(i);
			
			m_iCurLayerID = i-1;
			
			InsertItem(m_iCurLayerID,"");
			SetItemText(m_iCurLayerID,4,pLayer->m_strLayerName);
			SetItemData(m_iCurLayerID, (DWORD)pCell);
			break;
		}
	}

	m_strCurLayerID = pLayer->m_strID;
	m_pCurPage->m_pDocument->m_pCurDrawLayer = pLayer;

	return TRUE;
}


void CLayerListCtrl::OnDestroy() 
{
	if( m_pEdit && m_pEdit->IsWindowVisible() )
		EndEdit();

	DeleteLayerAll();
	CListCtrl::OnDestroy();
	
}
/////////////////////////////////////////////////////////////////////////////
// CEditLayer

CEditLayer::CEditLayer()
{
}

CEditLayer::~CEditLayer()
{
}


BEGIN_MESSAGE_MAP(CEditLayer, CEdit)
	//{{AFX_MSG_MAP(CEditLayer)
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditLayer message handlers

void CEditLayer::OnKillFocus(CWnd* pNewWnd) 
{	
	CLayerListCtrl *pWnd = (CLayerListCtrl *)GetParent();
	if( pWnd!= NULL && pWnd->m_hWnd != NULL ) 
		pWnd->EndEdit();

	CEdit::OnKillFocus(pNewWnd);
}

BOOL CLayerListCtrl::ActiveLayer(const int &nActiveLayerID)
{
	CLayerCell *pCell = (CLayerCell *)GetItemData(nActiveLayerID);
	CDrawLayer *pLayer = m_pCurPage->GetLayer(pCell->m_strLayerID);
	m_pCurPage->SetCurLayer(pLayer);
	m_strCurLayerID = pCell->m_strLayerID;
	m_pCurPage->m_pDocument->m_pCurDrawLayer = pLayer;
	
	invalidate_grid(m_iCurLayerID,0);
	
	m_iCurLayerID=nActiveLayerID;
	
	invalidate_grid(m_iCurLayerID,0);
	
	return TRUE;
}

BOOL CLayerListCtrl::SetLayerLockedFlag(const int &nLayerID, const BOOL &bFlag)
{
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	CMDIChildWndEx *pWnd =(CMDIChildWndEx *) pMF->MDIGetActive();

	CGeoMapView *pView = (CGeoMapView *)pWnd->GetActiveView();
	CGeoMapDoc *pDoc = (CGeoMapDoc *)pView ->GetDocument();	
	
	CLayerCell *pCell = (CLayerCell *)GetItemData(nLayerID);
	CDrawLayer *pLayer = m_pCurPage->GetLayer(pCell->m_strLayerID);
	pLayer->m_bLocked = bFlag;
	pCell->m_bLocked = pLayer->m_bLocked;
	
	pDoc->SetModifiedFlag();
	
	invalidate_grid(nLayerID,2);
	return TRUE;
}

BOOL CLayerListCtrl::SetLayerShowFlag(const int &nLayerID, const BOOL &bFlag)
{
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	CMDIChildWndEx *pWnd =(CMDIChildWndEx *) pMF->MDIGetActive();
	CGeoMapView *pView = (CGeoMapView *)pWnd->GetActiveView();
	CGeoMapDoc *pDoc = (CGeoMapDoc *)pView ->GetDocument();	
	
	CLayerCell *pCell = (CLayerCell *)GetItemData(nLayerID);
	CDrawLayer *pLayer = m_pCurPage->GetLayer(pCell->m_strLayerID);
	pLayer->m_bShow = bFlag;
	pCell->m_bShow = pLayer->m_bShow;
	
	pDoc->SetModifiedFlag();
	
	invalidate_grid(nLayerID,1);
	pDoc->UpdateAllViews(NULL);
	return TRUE;
}

BOOL CLayerListCtrl::SetLayerClipFlag(const int &nLayerID, const BOOL &bFlag)
{
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	CMDIChildWndEx *pWnd =(CMDIChildWndEx *) pMF->MDIGetActive();
	CGeoMapView *pView = (CGeoMapView *)pWnd->GetActiveView();
	CGeoMapDoc *pDoc = (CGeoMapDoc *)pView ->GetDocument();	
	
	CLayerCell *pCell = (CLayerCell *)GetItemData(nLayerID);
	CDrawLayer *pLayer = m_pCurPage->GetLayer(pCell->m_strLayerID);
	pLayer->m_bClipping = bFlag;
	pCell->m_bClipping = pLayer->m_bClipping;
	
	pDoc->SetModifiedFlag();
	
	invalidate_grid(nLayerID,3);
	pDoc->UpdateAllViews(NULL);
	return TRUE;
}


BOOL CLayerListCtrl::SetLayerName(const int &nLayerID, const CString &strLayerName)
{
	SetItemText(nLayerID,4,strLayerName);
	
	invalidate_grid(nLayerID,4);	
	
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	CMDIChildWndEx *pWnd =(CMDIChildWndEx *) pMF->MDIGetActive();
	CGeoMapView *pView = (CGeoMapView *)pWnd->GetActiveView();
	CGeoMapDoc *pDoc = (CGeoMapDoc *)pView ->GetDocument();	
	
	CLayerCell *pCell = (CLayerCell *)GetItemData(nLayerID);
	CDrawLayer *pLayer = m_pCurPage->GetLayer(pCell->m_strLayerID);
	pLayer->m_strLayerName = strLayerName;
	pCell->m_strLayerName = strLayerName;

	return TRUE;
}

void CLayerListCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	CListCtrl::OnKillFocus(pNewWnd);
}

void CLayerListCtrl::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	
}

void CLayerListCtrl::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
		
	NMITEMACTIVATE* nm=(NMITEMACTIVATE*)pNMHDR;	
	
	if( nm->iItem == -1 || nm->iSubItem == -1 )
	{
		m_pCurPage->OnEditProperties();
		return;
	}
	
	if(nm->iSubItem ==0 && nm->iItem != -1)
	{
		CDrawLayer *pLayer = m_pCurPage->GetLayer(GetItemCount()-nm->iItem-1);
		pLayer->OnEditProperties();
	}
		

	*pResult = 0;
}
