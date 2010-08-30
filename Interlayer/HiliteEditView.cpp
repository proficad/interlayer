// HiliteEditView.cpp : implementation file
//

#include "stdafx.h"
#include "HiliteEditView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHiliteEditView

IMPLEMENT_DYNCREATE(CHiliteEditView, CEditView)

CHiliteEditView::CHiliteEditView()
{
	m_bCanPaint = TRUE;
}

CHiliteEditView::~CHiliteEditView()
{
}

BEGIN_MESSAGE_MAP(CHiliteEditView, CEditView)
	//{{AFX_MSG_MAP(CHiliteEditView)
	ON_WM_PAINT()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_CHAR()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHiliteEditView diagnostics

#ifdef _DEBUG
void CHiliteEditView::AssertValid() const
{
	CEditView::AssertValid();
}

void CHiliteEditView::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CHiliteEditView functions

int CHiliteEditView::GetCaretLine() const
{
	CPoint ptCaret = GetCaretPos();
	int nLine = HIWORD(GetEditCtrl().CharFromPos(ptCaret));

	// when the top line is not the first line,
	// the result is actually the line before that line,
	// so we add 1 to it
	if ((ptCaret.y == 1) && (nLine > 0)) {
		nLine++;
	}
	return nLine;
}

void CHiliteEditView::GetLineRect(int nLine, LPRECT lpRect) const
{
	// if using CRichEditView, it will be not so complicated

	// EM_POSFROMCHAR will not return correct pos of the last line,
	// so we have to calcluate it
	if (nLine == 0)		// the first line;
	{
		GetEditCtrl().GetRect(lpRect);
		lpRect->bottom = lpRect->top + m_sizeChar.cy;
	}
	else if (nLine == GetEditCtrl().GetLineCount() - 1)		// the last line
	{
		// we get previous line's rect, then offset it by one line height.
		int nLineIndex = GetEditCtrl().LineIndex(nLine - 1);
		CPoint ptPos = GetEditCtrl().PosFromChar(nLineIndex);
		GetEditCtrl().GetRect(lpRect);
		lpRect->top = ptPos.y;
		lpRect->bottom = lpRect->top + m_sizeChar.cy;
		OffsetRect(lpRect, 0, m_sizeChar.cy);
	}
	else	// lines between first and last
	{
		int nLineIndex = GetEditCtrl().LineIndex(nLine);
		CPoint ptPos = GetEditCtrl().PosFromChar(nLineIndex);
		GetEditCtrl().GetRect(lpRect);
		lpRect->top = ptPos.y;
		lpRect->bottom = lpRect->top + m_sizeChar.cy;
	}
}

void CHiliteEditView::DrawCaretLine(BOOL bInPaint)
{
	int nLine = GetCaretLine();
	// for effective we need not redraw when we
	// just move caret in the same line using arrow keys, simply return.
	if (nLine == m_nCaretLine && !bInPaint)
		return;

	CRect rectClip;
	GetEditCtrl().GetRect(rectClip);
	CClientDC dc(this);
	dc.IntersectClipRect(rectClip);

	int nLineFirst = GetEditCtrl().GetFirstVisibleLine();
	int nLineLast = GetEditCtrl().LineFromChar(GetEditCtrl().CharFromPos(rectClip.BottomRight()));

	// hide caret, else it will be ugly.
	HideCaret();

	if (m_nCaretLine >= nLineFirst && m_nCaretLine <= nLineLast)
	{
		// in this section we must not make WM_PAINT a loop
		// so don't let OnPaint() call our DrawCaretLine()
		m_bCanPaint = FALSE;
		CRect rect;
		GetLineRect(m_nCaretLine, rect);
		InvalidateRect(rect, FALSE);
		// update immediately
		UpdateWindow();
		m_bCanPaint = TRUE;
	}

	// we change the caret line color by ROP
	if (nLine >= nLineFirst && nLine <= nLineLast)
	{
		CRect rect;
		GetLineRect(nLine, rect);
		CDC dcMem;
		dcMem.CreateCompatibleDC(&dc);
		CBitmap bmp;
		bmp.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
		CBitmap* pSaveBmp = dcMem.SelectObject(&bmp);
		CBrush br(RGB(0, 0, 255));
		dcMem.FillRect(CRect(0, 0, rect.Width(), rect.Height()), &br);
		// "capture" the line into our memory dc, and "INVERT" it
		dcMem.BitBlt(0, 0, rect.Width(), rect.Height(), &dc, rect.left, rect.top, SRCINVERT);
		// blt it back to origin place, but change colors
		dc.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dcMem, 0, 0, SRCCOPY);
		dcMem.SelectObject(pSaveBmp);
	}

	ShowCaret();
	m_nCaretLine = nLine;
}

void CHiliteEditView::OnInitialUpdate() 
{
	CEditView::OnInitialUpdate();

	// set default font to "Courier New"
	// because it is more clearly and equal-widen,
	CClientDC dc(this);
	m_fontDefault.CreateFont(-MulDiv(10, dc.GetDeviceCaps(LOGPIXELSX), 72), 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, _T("Courier New"));
	SetFont(&m_fontDefault);
	CFont* pSaveFont = dc.SelectObject(GetFont());

	// then get the char size and save for further use.
	m_sizeChar = dc.GetTextExtent(_T("W"));
	dc.SelectObject(pSaveFont);
	m_nCaretLine = GetCaretLine();

	// this is not the right way to set tabstops
	// see MSDN for details
	SetTabStops(16);
}

/////////////////////////////////////////////////////////////////////////////
// CHiliteEditView message handlers

void CHiliteEditView::OnPaint() 
{
	Default();
	if (m_bCanPaint)
		DrawCaretLine(TRUE);
}

void CHiliteEditView::OnKeyDown(UINT /*nChar*/, UINT /*nRepCnt*/, UINT /*nFlags*/) 
{
	Default();
	DrawCaretLine();
}

void CHiliteEditView::OnLButtonDown(UINT /*nFlags*/, CPoint /*point*/) 
{
	Default();
	DrawCaretLine();
}

void CHiliteEditView::OnChar(UINT /*nChar*/, UINT /*nRepCnt*/, UINT /*nFlags*/) 
{
	Default();
	// if using CRichEditView, these steps are not needed.
	int nLine = GetCaretLine();
	if (m_nCaretLine == nLine)
	{
		CRect rect;
		GetLineRect(m_nCaretLine, rect);
		InvalidateRect(rect, FALSE);
		UpdateWindow();
	}
	else
	{
		CRect rect;
		GetLineRect(m_nCaretLine, rect);
		InvalidateRect(rect, FALSE);
		UpdateWindow();
		m_nCaretLine = nLine;
	}
}

void CHiliteEditView::OnMouseMove(UINT nFlags, CPoint /*point*/)
{
	Default();
	if (nFlags & MK_LBUTTON)
	{
	int nLine = GetCaretLine();
//	if (m_nCaretLine != nLine)
	{
		CRect rect;
		GetLineRect(m_nCaretLine, rect);
		InvalidateRect(rect, FALSE);
		UpdateWindow();
		m_nCaretLine = nLine;
	}
/*	else
	{
		CRect rect;
		GetLineRect(m_nCaretLine, rect);
		InvalidateRect(rect, FALSE);
		UpdateWindow();
		m_nCaretLine = nLine;
	}*/	}
}