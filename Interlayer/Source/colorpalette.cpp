#include "stdafx.h"
#include "colorpalette.h"
#include "afxpriv.h"
#include "../Resource.h"
#include "../GeoMap/GeoMapView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CColorPalette::CColorPalette()
{                    
	m_ForeColor = RGB(0, 0, 0);
	m_BackColor = RGB(191, 191, 191);
}

CColorPalette::~CColorPalette()
{
}


BEGIN_MESSAGE_MAP(CColorPalette, CDialogBar)
//{{AFX_MSG_MAP(CColorPalette)
ON_WM_LBUTTONDOWN()
ON_WM_PAINT()
ON_WM_LBUTTONUP()
//}}AFX_MSG_MAP
ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdate)
END_MESSAGE_MAP()


LPARAM CColorPalette::OnIdleUpdate(WPARAM, LPARAM)
{
	CMDIFrameWnd * p = (CMDIFrameWnd *)AfxGetMainWnd();
	if(!p) return 0L;
	CFrameWnd * pChild = p->GetActiveFrame();
	if(!pChild) return 0L;
	CView* pWnd = (CView*) pChild->GetActiveView();
	if(!pWnd) return 0L;
	

	if(!pWnd->IsKindOf(RUNTIME_CLASS(CGeoMapView)))
		return 0L;

	CGeoMapView *pView = (CGeoMapView*) pWnd;
	
	CWnd* pColorFrame = GetDlgItem(IDC_COLOR_FRAME);
	CWnd* pRed = GetDlgItem(IDC_RED);
	CWnd* pGreen = GetDlgItem(IDC_GREEN);
	CWnd* pBlue = GetDlgItem(IDC_BLUE);
	CWnd* pIntensity = GetDlgItem(IDC_INTENSITY);
	CWnd* pX = GetDlgItem(IDC_X);
	CWnd* pY = GetDlgItem(IDC_Y);
	
	ASSERT(pColorFrame);
	ASSERT(pRed);
	ASSERT(pGreen);
	ASSERT(pBlue);
	ASSERT(pIntensity);
	
	CRect rect;
	pColorFrame->GetClientRect(rect);
	pColorFrame->ClientToScreen(rect);
	ScreenToClient(rect);
	rect.DeflateRect(2, 2);
	CBrush brush;
	
	CString sRed(_T("..."));
	CString sGreen(_T("..."));
	CString sBlue(_T("..."));
	CString sIntensity(_T("..."));
	CString sX(_T("..."));
	CString sY(_T("..."));
	
	int x;
	int y;
	COLORREF c;
	if (pView && pView->GetColorAndPositionAtCursor(c, x, y))
	{
		brush.CreateSolidBrush(c);
		sRed.Format(_T("%d"), GetRValue(c));
		sGreen.Format(_T("%d"), GetGValue(c));
		sBlue.Format(_T("%d"), GetBValue(c));
		sIntensity.Format(_T("%d"), (GetRValue(c) + GetGValue(c) + GetBValue(c))/3);
		sX.Format(_T("%d"), x);
		sY.Format(_T("%d"), y);
	}
	else
		brush.CreateHatchBrush(HS_DIAGCROSS, RGB(0, 0, 0));
	
	CClientDC dc(this);
	dc.FillRect(rect, &brush);
	
	pRed->SetWindowText(sRed);
	pGreen->SetWindowText(sGreen);
	pBlue->SetWindowText(sBlue);
	pIntensity->SetWindowText(sIntensity);
	pX->SetWindowText(sX);
	pY->SetWindowText(sY);
	
	return 0L;
}

void CColorPalette::OnLButtonDown(UINT /*nFlags*/, CPoint point) 
{
	CWnd* pFore = GetDlgItem(IDC_FORECOLOR);
	CWnd* pBack = GetDlgItem(IDC_BACKCOLOR);
	ASSERT(pFore);
	ASSERT(pBack);
	
	CRect ForeRect;
	CRect BackRect;
	pFore->GetClientRect(ForeRect);
	pFore->ClientToScreen(ForeRect);  
	ScreenToClient(ForeRect);
	pBack->GetClientRect(BackRect);
	pBack->ClientToScreen(BackRect);  
	ScreenToClient(BackRect);
	
	CBCGColorDialog dlg;
	if (ForeRect.PtInRect(point))
	{
		dlg.SetCurrentColor(m_ForeColor);
		if (dlg.DoModal() == IDOK)
		{
			m_ForeColor = dlg.GetColor();
			
			CMDIFrameWnd * p = (CMDIFrameWnd *)AfxGetMainWnd();
			if(!p) return ;
			CFrameWnd * pChild = p->GetActiveFrame();
			if(!pChild) return ;
			CView* pWnd = (CView*) pChild->GetActiveView();
			if(!pWnd) return ;
			
			
			if(!pWnd->IsKindOf(RUNTIME_CLASS(CGeoMapView)))
				return ;
			
			CGeoMapView *pView = (CGeoMapView*) pWnd;

			pView->m_clrFore = m_ForeColor;

			POSITION pos = pView->m_selection.GetHeadPosition();
			CDrawObj *pObj;
			while(pos!=NULL)
			{
				pObj = pView->m_selection.GetNext(pos);
				if(pObj)
				{
					pObj->m_clrBack = pView->m_clrBack;
					pObj->m_clrFore = pView->m_clrFore;
					pObj->Invalidate();
				}
			}
			
			Invalidate();
		}
	}
	else if (BackRect.PtInRect(point))
	{
		dlg.SetCurrentColor( m_BackColor );
		if (dlg.DoModal() == IDOK)
		{
			m_BackColor = dlg.GetColor();
			CMDIFrameWnd * p = (CMDIFrameWnd *)AfxGetMainWnd();
			if(!p) return ;
			CFrameWnd * pChild = p->GetActiveFrame();
			if(!pChild) return;
			CView* pWnd = (CView*) pChild->GetActiveView();
			if(!pWnd) return ;
			
			
			if(!pWnd->IsKindOf(RUNTIME_CLASS(CGeoMapView)))
				return;
			
			CGeoMapView *pView = (CGeoMapView*) pWnd;

			pView->m_clrBack = m_BackColor;

			POSITION pos = pView->m_selection.GetHeadPosition();
			CDrawObj *pObj;
			while(pos!=NULL)
			{
				pObj = pView->m_selection.GetNext(pos);
				if(pObj)
				{
					pObj->m_clrBack = pView->m_clrBack;
					pObj->m_clrFore = pView->m_clrFore;
					pObj->Invalidate();
				}
			}

			Invalidate();
		}
	}
}

void CColorPalette::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CWnd* pFore = GetDlgItem(IDC_FORECOLOR);
	CWnd* pBack = GetDlgItem(IDC_BACKCOLOR);
	ASSERT(pFore);
	ASSERT(pBack);
	
	CBrush Forebrush;
	CBrush Backbrush;
	Forebrush.CreateSolidBrush(m_ForeColor);
	Backbrush.CreateSolidBrush(m_BackColor);
	
	CRect ForeRect;
	CRect BackRect;
	pFore->GetClientRect(ForeRect);
	pFore->ClientToScreen(ForeRect);  
	pBack->GetClientRect(BackRect);
	pBack->ClientToScreen(BackRect);  
	ScreenToClient(ForeRect);
	ScreenToClient(BackRect);
	dc.FillRect(BackRect, &Backbrush);
	dc.FillRect(ForeRect, &Forebrush);
}

void CColorPalette::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialogBar::OnLButtonUp(nFlags, point);
}
