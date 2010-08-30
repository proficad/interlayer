// ColorTableList.cpp : implementation file
//

#include "stdafx.h"
#include "ColorTableList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorTableList

CColorTableList::CColorTableList()
{
}

CColorTableList::~CColorTableList()
{
}


BEGIN_MESSAGE_MAP(CColorTableList, CListBox)
	//{{AFX_MSG_MAP(CColorTableList)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorTableList message handlers

void CColorTableList::DrawItem(LPDRAWITEMSTRUCT dis) 
{
	// TODO: Add your code to draw the specified item
	CRect r = dis->rcItem;
    CDC * dc = CDC::FromHandle(dis->hDC);
    CBrush br;
    COLORREF txcolor;
    COLORREF bkcolor;
	
    int saved = dc->SaveDC();
	
    CString temp(_T("123,456   "));
    CSize sz = dc->GetTextExtent(temp);
    
    if(dis->itemState & ODS_SELECTED)
	{ /* selected */
		if(::GetFocus() == m_hWnd)
		{ /* has focus */
			bkcolor = ::GetSysColor(COLOR_HIGHLIGHT);
			txcolor = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
		} /* has focus */	    
		else
		{ /* no focus */
			bkcolor = ::GetSysColor(COLOR_3DFACE);
			txcolor = ::GetSysColor(COLOR_WINDOWTEXT);
		} /* no focus */
	} /* selected */
    else
	{ /* unselected */
		if(dis->itemState & (ODS_DISABLED | ODS_GRAYED))
			txcolor = ::GetSysColor(COLOR_GRAYTEXT);
		else
			txcolor = ::GetSysColor(COLOR_WINDOWTEXT);
		
		bkcolor = ::GetSysColor(COLOR_WINDOW);
	} /* unselected */
	
    dc->SetBkColor(bkcolor);
    dc->SetTextColor(txcolor);
	
    CRect text = r;
    text.right = sz.cx;
    dc->FillSolidRect(&text, bkcolor);
	
    if(dis->itemID != (UINT)-1 
		&& (dis->itemState & (ODS_DISABLED | ODS_GRAYED)) == 0)
	{ /* has item */
		double v = m_pColorTable->GetValue(dis->itemID);
		COLORREF c = m_pColorTable->LookUpColor(v);
		CRect color = r;
		color.left = sz.cx;
		dc->FillSolidRect(&color, c);
		
		CString s;
		s.Format(_T("%.2lf"), v);
		dc->SetBkMode(TRANSPARENT);
		
		dc->TextOut(r.left, r.top, s);
		
	} /* has item */
	
    if(dis->itemState & ODS_FOCUS && dis->itemAction != ODA_SELECT)
		dc->DrawFocusRect(&r);
	
    dc->RestoreDC(saved);
}
