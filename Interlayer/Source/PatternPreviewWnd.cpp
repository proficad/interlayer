// PatternPreviewWnd.cpp : implementation file
//

#include "stdafx.h"
#include "../Resource.h"
#include "PatternPreviewWnd.h"
#include "../GeoMap/commonstruct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPatternPreviewWnd

CPatternPreviewWnd::CPatternPreviewWnd()
{
	nRow = -1;
	nCol = -1;
	m_crColourBack = RGB(255, 255, 255);
	m_crColourFore = RGB(0, 0, 0);
}

CPatternPreviewWnd::~CPatternPreviewWnd()
{
}


BEGIN_MESSAGE_MAP(CPatternPreviewWnd, CWnd)
	//{{AFX_MSG_MAP(CPatternPreviewWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPatternPreviewWnd message handlers

void CPatternPreviewWnd::OnPaint() 
{
	// TODO: Add your custom preview paint code.
	// For an example, we are drawing a blue ellipse.

	CPaintDC dc(this); // device context for painting
	CRect rect;
	GetClientRect(rect);	

	if( nRow != -1 && nCol != -1)
	{
		PATTERNFILL patternFill;
		patternFill.m_nCol = nCol;
		patternFill.m_nRow = nRow;
		patternFill.m_crColourBack = m_crColourBack;
		patternFill.m_crColourFore = m_crColourFore;

		CBrush brush;
		if(! patternFill.CreateBrush(&brush, &dc))
			return ;

		CBrush *pOldBrush = dc.SelectObject(&brush);

		dc.Rectangle(rect);

		dc.SelectObject(pOldBrush);		
		brush.DeleteObject();
	}

	else
		dc.Rectangle(rect);	
}

BOOL CPatternPreviewWnd::OnEraseBkgnd(CDC* pDC) 
{
	// Use the same background color as that of the dialog
	//  (property sheet).

	CWnd* pParentWnd = GetParent();
	HBRUSH hBrush = (HBRUSH)pParentWnd->SendMessage(WM_CTLCOLORDLG,
		(WPARAM)pDC->m_hDC, (LPARAM)pParentWnd->m_hWnd);
	CRect rect;
	GetClientRect(rect);
	pDC->FillRect(&rect, CBrush::FromHandle(hBrush));
	return TRUE;
}
