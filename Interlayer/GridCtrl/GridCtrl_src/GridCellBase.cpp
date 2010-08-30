// GridCellBase.cpp : implementation file
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GridCtrl.h"
#include "GridCellBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CGridCellBase, CObject)

/////////////////////////////////////////////////////////////////////////////
// GridCellBase

CGridCellBase::CGridCellBase()
{
    Reset();
}

CGridCellBase::~CGridCellBase()
{
}

/////////////////////////////////////////////////////////////////////////////
// GridCellBase Operations

void CGridCellBase::Reset()
{
	m_Hide=false;
    m_nState  = 0;
	m_MergeRange.Set();
	m_IsMergeWithOthers=false;
	m_MergeCellID.row=-1;
	m_MergeCellID.col=-1;
	m_nType = -1;

	// zj 2004.6.21 add
	m_nBorderLine = CBL_NONE;
}
//Used for merge cells,remove const
//by Huang Wei

void CGridCellBase::operator=( CGridCellBase& cell)
{
	if (this == &cell) return;

    SetGrid(cell.GetGrid());    // do first in case of dependencies

    SetText(cell.GetText());
    SetImage(cell.GetImage());
    SetData(cell.GetData());
    SetState(cell.GetState());
    SetFormat(cell.GetFormat());
    SetTextClr(cell.GetTextClr());
    SetBackClr(cell.GetBackClr());
    SetFont(cell.IsDefaultFont()? NULL : cell.GetFont());
    SetMargin(cell.GetMargin());
//Used for merge cells
//by Huang Wei
	SetMergeCellID(cell.GetMergeCellID());
	SetMergeRange(cell.GetMergeRange());
	Show(cell.IsShow());
	
	// zj 2004.6.20 add
	SetCellBorderLine(cell.GetCellBorderLine());
}

/////////////////////////////////////////////////////////////////////////////
// CGridCellBase Attributes

// Returns a pointer to a cell that holds default values for this particular type of cell
CGridCellBase* CGridCellBase::GetDefaultCell() const
{
    if (GetGrid())
        return GetGrid()->GetDefaultCell(IsFixedRow(), IsFixedCol());
    return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CGridCellBase Operations

// EFW - Various changes to make it draw cells better when using alternate
// color schemes.  Also removed printing references as that's now done
// by PrintCell() and fixed the sort marker so that it doesn't draw out
// of bounds.
BOOL CGridCellBase::Draw(CDC* pDC, int nRow, int nCol, CRect rect,  BOOL bEraseBkgnd /*=TRUE*/)
{
    // Note - all through this function we totally brutalise 'rect'. Do not
    // depend on it's value being that which was passed in.

	//Used for merge cells
	//by Huang Wei
	if(	m_Hide && !IsMerged())
	{
		return TRUE;
	}
	
	// zj 2004.6.21 add
	CRect cellRect = rect;
	//******************


    CGridCtrl* pGrid = GetGrid();
    ASSERT(pGrid);

    if (!pGrid || !pDC)
        return FALSE;

    if( rect.Width() <= 0 || rect.Height() <= 0)  // prevents imagelist item from drawing even
        return FALSE;                             //  though cell is hidden

    //TRACE3("Drawing %scell %d, %d\n", IsFixed()? _T("Fixed ") : _T(""), nRow, nCol);

    int nSavedDC = pDC->SaveDC();
    pDC->SetBkMode(TRANSPARENT);

    // Get the default cell implementation for this kind of cell. We use it if this cell
    // has anything marked as "default"
    CGridDefaultCell *pDefaultCell = (CGridDefaultCell*) GetDefaultCell();
    if (!pDefaultCell)
        return FALSE;

    // Set up text and background colours
    COLORREF TextClr, TextBkClr;

    TextClr = (GetTextClr() == CLR_DEFAULT)? pDefaultCell->GetTextClr() : GetTextClr();
    if (GetBackClr() == CLR_DEFAULT)
        TextBkClr = pDefaultCell->GetBackClr();
    else
    {
        bEraseBkgnd = TRUE;
        TextBkClr = GetBackClr();
    }

    // Draw cell background and highlighting (if necessary)
    if ( IsFocused() || IsDropHighlighted() )
    {
        // Always draw even in list mode so that we can tell where the
        // cursor is at.  Use the highlight colors though.
        if(GetState() & GVIS_SELECTED)
        {
            TextBkClr = ::GetSysColor(COLOR_HIGHLIGHT);
            TextClr = ::GetSysColor(COLOR_HIGHLIGHTTEXT);			
            bEraseBkgnd = TRUE;
        }

  //      rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
        if (bEraseBkgnd)
        {
            TRY 
            {
                CBrush brush(TextBkClr);
				
                pDC->FillRect(rect, &brush);
            } 
            CATCH(CResourceException, e)
            {
                //e->ReportError();
            }
            END_CATCH
        }

        // Don't adjust frame rect if no grid lines so that the
        // whole cell is enclosed.
        if(pGrid->GetGridLines() != GVL_NONE)
        {
            rect.right--;
            rect.bottom--;
        }

        if (pGrid->GetFrameFocusCell())
        {
                // Use same color as text to outline the cell so that it shows
                // up if the background is black.
            TRY 
            {
                CBrush brush(TextClr);
                pDC->FrameRect(rect, &brush);
            }
            CATCH(CResourceException, e)
            {
                //e->ReportError();
            }
            END_CATCH
        }
        pDC->SetTextColor(TextClr);

        // Adjust rect after frame draw if no grid lines
        if(pGrid->GetGridLines() == GVL_NONE)
        {
            rect.right--;
            rect.bottom--;
        }

		rect.DeflateRect(0,1,1,1); 
    }
    else if ((GetState() & GVIS_SELECTED))
    {
        //rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom  //²»»­Ïß
        pDC->FillSolidRect(rect, ::GetSysColor(COLOR_HIGHLIGHT));
        //rect.right--; rect.bottom--;
        pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
    }
    else
    {
        if (bEraseBkgnd)
        {
//            rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
            CBrush brush(TextBkClr);
            pDC->FillRect(rect, &brush);
  //          rect.right--; rect.bottom--;
        }
        pDC->SetTextColor(TextClr);
    }

    // Draw lines only when wanted
    
	// zj 2004.6.21 modified
	if (IsFixed() )
	//if (IsFixed() && pGrid->GetGridLines() != GVL_NONE)	
    {
		if(pGrid->GetReport())
		{
			CBrush brush(pGrid->GetTextBkColor());
			CRect rt = rect;
			rt.DeflateRect(-1,-1,-1,-1);
            pDC->FillRect(rt, &brush);
		}
		else
		{
			CCellID FocusCell = pGrid->GetFocusCell();
			
			// As above, always show current location even in list mode so
			// that we know where the cursor is at.
			BOOL bHiliteFixed = pGrid->GetTrackFocusCell() && pGrid->IsValid(FocusCell) &&
				(FocusCell.row == nRow || FocusCell.col == nCol);
			
			// If this fixed cell is on the same row/col as the focus cell,
			// highlight it.
			if (bHiliteFixed)
			{
				rect.right++; rect.bottom++;
				pDC->DrawEdge(rect, BDR_SUNKENINNER /*EDGE_RAISED*/, BF_RECT);
				rect.DeflateRect(1,1);
			}
			else
			{
				CPen lightpen(PS_SOLID, 1,  ::GetSysColor(COLOR_3DHIGHLIGHT)),
					darkpen(PS_SOLID,  1, ::GetSysColor(COLOR_3DDKSHADOW)),
					*pOldPen = pDC->GetCurrentPen();
				
				pDC->SelectObject(&lightpen);
				pDC->MoveTo(rect.right, rect.top);
				pDC->LineTo(rect.left, rect.top);
				pDC->LineTo(rect.left, rect.bottom);
				
				pDC->SelectObject(&darkpen);
				pDC->MoveTo(rect.right, rect.top);
				pDC->LineTo(rect.right, rect.bottom);
				pDC->LineTo(rect.left, rect.bottom);
				
				pDC->SelectObject(pOldPen);
				rect.DeflateRect(1,1);
			}
		}
    }

    // Draw Text and image
#if !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)
    if (!pDC->m_bPrinting)
#endif
    {
        CFont *pFont = GetFontObject();
		ASSERT(pFont);
        if (pFont)
            pDC->SelectObject(pFont);
    }

    rect.DeflateRect(GetMargin(), 0);

    if (pGrid->GetImageList() && GetImage() >= 0)
    {
        IMAGEINFO Info;
        if (pGrid->GetImageList()->GetImageInfo(GetImage(), &Info))
        {
            //  would like to use a clipping region but seems to have issue
            //  working with CMemDC directly.  Instead, don't display image
            //  if any part of it cut-off
            //
            // CRgn rgn;
            // rgn.CreateRectRgnIndirect(rect);
            // pDC->SelectClipRgn(&rgn);
            // rgn.DeleteObject();

            int nImageWidth = Info.rcImage.right-Info.rcImage.left+1;
            int nImageHeight = Info.rcImage.bottom-Info.rcImage.top+1;

            if( nImageWidth + rect.left <= rect.right + (int)(2*GetMargin())
                && nImageHeight + rect.top <= rect.bottom + (int)(2*GetMargin())  )
            {
                pGrid->GetImageList()->Draw(pDC, GetImage(), rect.TopLeft(), ILD_NORMAL);
            }

            //rect.left += nImageWidth+GetMargin();
        }
    }

    // Draw sort arrow
    if (pGrid->GetSortColumn() == nCol && nRow == 0)
    {
        CSize size = pDC->GetTextExtent(_T("M"));
        int nOffset = 2;

        // Base the size of the triangle on the smaller of the column
        // height or text height with a slight offset top and bottom.
        // Otherwise, it can get drawn outside the bounds of the cell.
        size.cy -= (nOffset * 2);

        if (size.cy >= rect.Height())
            size.cy = rect.Height() - (nOffset * 2);

        size.cx = size.cy;      // Make the dimensions square

        // Kludge for vertical text
        BOOL bVertical = (GetFont()->lfEscapement == 900);

        // Only draw if it'll fit!
        if (size.cx + rect.left < rect.right + (int)(2*GetMargin()))
        {
            int nTriangleBase = rect.bottom - nOffset - size.cy;    // Triangle bottom right
            //int nTriangleBase = (rect.top + rect.bottom - size.cy)/2; // Triangle middle right
            //int nTriangleBase = rect.top + nOffset;                 // Triangle top right

            //int nTriangleLeft = rect.right - size.cx;                 // Triangle RHS
            //int nTriangleLeft = (rect.right + rect.left - size.cx)/2; // Triangle middle
            //int nTriangleLeft = rect.left;                            // Triangle LHS

            int nTriangleLeft;
            if (bVertical)
                nTriangleLeft = (rect.right + rect.left - size.cx)/2; // Triangle middle
            else
                nTriangleLeft = rect.right - size.cx;               // Triangle RHS

            CPen penShadow(PS_SOLID, 0, ::GetSysColor(COLOR_3DSHADOW));
            CPen penLight(PS_SOLID, 0, ::GetSysColor(COLOR_3DHILIGHT));
            if (pGrid->GetSortAscending())
            {
                // Draw triangle pointing upwards
                CPen *pOldPen = (CPen*) pDC->SelectObject(&penLight);
                pDC->MoveTo( nTriangleLeft + 1, nTriangleBase + size.cy + 1);
                pDC->LineTo( nTriangleLeft + (size.cx / 2) + 1, nTriangleBase + 1 );
                pDC->LineTo( nTriangleLeft + size.cx + 1, nTriangleBase + size.cy + 1);
                pDC->LineTo( nTriangleLeft + 1, nTriangleBase + size.cy + 1);

                pDC->SelectObject(&penShadow);
                pDC->MoveTo( nTriangleLeft, nTriangleBase + size.cy );
                pDC->LineTo( nTriangleLeft + (size.cx / 2), nTriangleBase );
                pDC->LineTo( nTriangleLeft + size.cx, nTriangleBase + size.cy );
                pDC->LineTo( nTriangleLeft, nTriangleBase + size.cy );
                pDC->SelectObject(pOldPen);
            }
            else
            {
                // Draw triangle pointing downwards
                CPen *pOldPen = (CPen*) pDC->SelectObject(&penLight);
                pDC->MoveTo( nTriangleLeft + 1, nTriangleBase + 1 );
                pDC->LineTo( nTriangleLeft + (size.cx / 2) + 1, nTriangleBase + size.cy + 1 );
                pDC->LineTo( nTriangleLeft + size.cx + 1, nTriangleBase + 1 );
                pDC->LineTo( nTriangleLeft + 1, nTriangleBase + 1 );
    
                pDC->SelectObject(&penShadow);
                pDC->MoveTo( nTriangleLeft, nTriangleBase );
                pDC->LineTo( nTriangleLeft + (size.cx / 2), nTriangleBase + size.cy );
                pDC->LineTo( nTriangleLeft + size.cx, nTriangleBase );
                pDC->LineTo( nTriangleLeft, nTriangleBase );
                pDC->SelectObject(pOldPen);
            }
            
            if (!bVertical)
                rect.right -= size.cy;
        }
    }

    // We want to see '&' characters so use DT_NOPREFIX
    // zj 2004.9.14 add
    GetTextRect(rect);
	CRect textRect(0,0,1,1);
	DWORD fmt = GetFormat();
	CString ttt = GetText();	
	pDC->DrawText(ttt,textRect,DT_CALCRECT);
	
	CRect rt = rect;

	if( fmt & DT_LEFT )
	{
		rt.left = rect.left;
		rt.right = rect.left + textRect.Width();
	}
	if(fmt & DT_RIGHT)
	{
		rt.right = rect.right;
		rt.left = rect.right - textRect.Width();
	}
	
	if(fmt & DT_CENTER)
	{
		rt.left = rect.left;
		rt.right = rect.right;
	}	

	if(fmt & DT_TOP)
	{
		rt.top = rect.top;
		rt.bottom = rect.top + textRect.Height();
	}

	if(fmt & DT_BOTTOM)
	{
		rt.top = rect.bottom - textRect.Height();;
		rt.bottom = rect.bottom;
	}
	if(fmt & DT_VCENTER)
	{
		rt.top = rect.top + (rect.Height()- textRect.Height())/2;
		rt.bottom = rt.top + textRect.Height();
	}

	DrawText(pDC->m_hDC, GetText(), -1, rt, fmt | DT_NOPREFIX);

	// zj 2004.6.20 add
	if(!pGrid->GetReport())
	{		
		if(m_nBorderLine != CBL_NONE && (!IsFixed()) )
		{
			CPen pen(PS_SOLID, 0, RGB(0,0,0));
			cellRect.DeflateRect(-1,-1,0,0);
			CPen *pOldPen = (CPen*) pDC->SelectObject(&pen);
			if(m_nBorderLine & CBL_LEFT)
			{
				pDC->MoveTo(cellRect.left, cellRect.top);
				pDC->LineTo(cellRect.left, cellRect.bottom);
			}
			if(m_nBorderLine & CBL_TOP)
			{
				pDC->MoveTo(cellRect.left, cellRect.top);
				pDC->LineTo(cellRect.right, cellRect.top);
			}
			if(m_nBorderLine & CBL_RIGHT)
			{
				pDC->MoveTo(cellRect.right, cellRect.top);
				pDC->LineTo(cellRect.right, cellRect.bottom);
			}
			if(m_nBorderLine & CBL_BOTTOM)
			{
				pDC->MoveTo(cellRect.left, cellRect.bottom);
				pDC->LineTo(cellRect.right, cellRect.bottom);
			}
			pDC->SelectObject(pOldPen);
		}
	}
	else
	{
		if(m_nBorderLine != CBL_NONE)
		{
			CPen pen(PS_SOLID, 0, RGB(0,0,0));
			cellRect.DeflateRect(-1,-1,0,0);
			CPen *pOldPen = (CPen*) pDC->SelectObject(&pen);
			if(m_nBorderLine & CBL_LEFT)
			{
				pDC->MoveTo(cellRect.left, cellRect.top);
				pDC->LineTo(cellRect.left, cellRect.bottom);
			}
			if(m_nBorderLine & CBL_TOP)
			{
				pDC->MoveTo(cellRect.left, cellRect.top);
				pDC->LineTo(cellRect.right, cellRect.top);
			}
			if(m_nBorderLine & CBL_RIGHT)
			{
				pDC->MoveTo(cellRect.right, cellRect.top);
				pDC->LineTo(cellRect.right, cellRect.bottom);
			}
			if(m_nBorderLine & CBL_BOTTOM)
			{
				pDC->MoveTo(cellRect.left, cellRect.bottom);
				pDC->LineTo(cellRect.right, cellRect.bottom);
			}
			pDC->SelectObject(pOldPen);
		}
	}
	//***************************//
	
    pDC->RestoreDC(nSavedDC);

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CGridCellBase Mouse and Cursor events

// Not yet implemented
void CGridCellBase::OnMouseEnter()
{
    TRACE0("Mouse entered cell\n");
}

void CGridCellBase::OnMouseOver()
{
    //TRACE0("Mouse over cell\n");
}

// Not Yet Implemented
void CGridCellBase::OnMouseLeave()
{
    TRACE0("Mouse left cell\n");
}

void CGridCellBase::OnClick( CPoint PointCellRelative)
{
    UNUSED_ALWAYS(PointCellRelative);
    TRACE2("Mouse Left btn up in cell at x=%i y=%i\n", PointCellRelative.x, PointCellRelative.y);
}

void CGridCellBase::OnClickDown( CPoint PointCellRelative)
{
    UNUSED_ALWAYS(PointCellRelative);
    TRACE2("Mouse Left btn down in cell at x=%i y=%i\n", PointCellRelative.x, PointCellRelative.y);
}

void CGridCellBase::OnRClick( CPoint PointCellRelative)
{
    UNUSED_ALWAYS(PointCellRelative);
    TRACE2("Mouse right-clicked in cell at x=%i y=%i\n", PointCellRelative.x, PointCellRelative.y);
}

void CGridCellBase::OnDblClick( CPoint PointCellRelative)
{
    UNUSED_ALWAYS(PointCellRelative);
    TRACE2("Mouse double-clicked in cell at x=%i y=%i\n", PointCellRelative.x, PointCellRelative.y);
}

// Return TRUE if you set the cursor
BOOL CGridCellBase::OnSetCursor()
{
#ifndef _WIN32_WCE_NO_CURSOR
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
#endif
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CGridCellBase editing

void CGridCellBase::OnEndEdit() 
{
	ASSERT( FALSE); 
}

BOOL CGridCellBase::ValidateEdit(LPCTSTR str)
{
    UNUSED_ALWAYS(str);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CGridCellBase Sizing

BOOL CGridCellBase::GetTextRect( LPRECT pRect)  // i/o:  i=dims of cell rect; o=dims of text rect
{
    if (GetImage() >= 0)
    {
        IMAGEINFO Info;

        CGridCtrl* pGrid = GetGrid();
        CImageList* pImageList = pGrid->GetImageList();
        
        if (pImageList && pImageList->GetImageInfo( GetImage(), &Info))
        {
            int nImageWidth = Info.rcImage.right-Info.rcImage.left+1;
            pRect->left += nImageWidth + GetMargin();
        }
    }

    return TRUE;
}

// By default this uses the selected font (which is a bigger font)
CSize CGridCellBase::GetTextExtent(LPCTSTR szText, CDC* pDC /*= NULL*/)
{
    CGridCtrl* pGrid = GetGrid();
    ASSERT(pGrid);

    BOOL bReleaseDC = FALSE;
    if (pDC == NULL || szText == NULL)
    {
        if (szText)
			pDC = pGrid->GetDC();
        if (pDC == NULL || szText == NULL) 
        {
            CGridDefaultCell* pDefCell = (CGridDefaultCell*) GetDefaultCell();
            ASSERT(pDefCell);
            return CSize(pDefCell->GetWidth(), pDefCell->GetHeight());
        }
        bReleaseDC = TRUE;
    }

    CFont *pOldFont = NULL,
          *pFont = GetFontObject();
    if (pFont)
        pOldFont = pDC->SelectObject(pFont);

    CSize size;
    int nFormat = GetFormat();

    // If the cell is a multiline cell, then use the width of the cell
    // to get the height
    if ((nFormat & DT_WORDBREAK) && !(nFormat & DT_SINGLELINE))
    {
        CString str = szText;
        int nMaxWidth = 0;
        while (TRUE)
        {
            int nPos = str.Find(_T('\n'));
            CString TempStr = (nPos < 0)? str : str.Left(nPos);
            int nTempWidth = pDC->GetTextExtent(TempStr).cx;
            if (nTempWidth > nMaxWidth)
                nMaxWidth = nTempWidth;

            if (nPos < 0)
                break;
            str = str.Mid(nPos + 1);    // Bug fix by Thomas Steinborn
        }
        
        CRect rect;
        rect.SetRect(0,0, nMaxWidth+1, 0);
        pDC->DrawText(szText, -1, rect, nFormat | DT_CALCRECT);
        size = rect.Size();
    }
    else
        size = pDC->GetTextExtent(szText, _tcslen(szText));

    TEXTMETRIC tm;
    pDC->GetTextMetrics(&tm);
    size.cx += (tm.tmOverhang);

    if (pOldFont)
        pDC->SelectObject(pOldFont);
    
    size += CSize(4*GetMargin(), 2*GetMargin());

    // Kludge for vertical text
    LOGFONT *pLF = GetFont();
    if (pLF->lfEscapement == 900 || pLF->lfEscapement == -900)
    {
        int nTemp = size.cx;
        size.cx = size.cy;
        size.cy = nTemp;
        size += CSize(0, 4*GetMargin());
    }
    
    if (bReleaseDC)
        pGrid->ReleaseDC(pDC);

    return size;
}

CSize CGridCellBase::GetCellExtent(CDC* pDC)
{
    CSize size = GetTextExtent(GetText(), pDC);
    CSize ImageSize(0,0);

    int nImage = GetImage();
    if (nImage >= 0) 
    {
        CGridCtrl* pGrid = GetGrid();
        ASSERT(pGrid);

        if (pGrid->GetImageList()) 
        {
            IMAGEINFO Info;
            if (pGrid->GetImageList()->GetImageInfo(nImage, &Info))
                ImageSize = CSize(Info.rcImage.right-Info.rcImage.left+1, 
                                  Info.rcImage.bottom-Info.rcImage.top+1);
        }
    }
    
    return CSize(size.cx + ImageSize.cx, max(size.cy, ImageSize.cy));
}

// EFW - Added to print cells so that grids that use different colors are
// printed correctly.
BOOL CGridCellBase::PrintCell(CDC* pDC, int /*nRow*/, int /*nCol*/, CRect rect)
{
	//Used for merge cells
	//by Huang Wei
	if(	m_Hide && !IsMerged())
	{
		return TRUE;
	}

	// zj 2004.6.21 add
	CRect cellRect = rect;
	//*********************//
	
#if defined(_WIN32_WCE_NO_PRINTING) || defined(GRIDCONTROL_NO_PRINTING)
    return FALSE;
#else
    COLORREF crFG, crBG;
    GV_ITEM Item;

    CGridCtrl* pGrid = GetGrid();
    if (!pGrid || !pDC)
        return FALSE;

    if( rect.Width() <= 0
        || rect.Height() <= 0)  // prevents imagelist item from drawing even
        return FALSE;           //  though cell is hidden

    int nSavedDC = pDC->SaveDC();

    pDC->SetBkMode(TRANSPARENT);
	//Used for merge cells
	//by Huang Wei
	rect.InflateRect(1,1);
	
	// zj 2004.6.21 modified
	//pDC->Rectangle(rect);
	//*******************//

	rect.DeflateRect(1,1);

    if (pGrid->GetShadedPrintOut())
    {
        // Get the default cell implementation for this kind of cell. We use it if this cell
        // has anything marked as "default"
        CGridDefaultCell *pDefaultCell = (CGridDefaultCell*) GetDefaultCell();
        if (!pDefaultCell)
            return FALSE;

        // Use custom color if it doesn't match the default color and the
        // default grid background color.  If not, leave it alone.
        if(IsFixed())
            crBG = (GetBackClr() != CLR_DEFAULT) ? GetBackClr() : pDefaultCell->GetBackClr();
        else
            crBG = (GetBackClr() != CLR_DEFAULT && GetBackClr() != pDefaultCell->GetBackClr()) ?
                GetBackClr() : CLR_DEFAULT;

        // Use custom color if the background is different or if it doesn't
        // match the default color and the default grid text color.  
        if(IsFixed())
            crFG = (GetTextClr() != CLR_DEFAULT) ? GetTextClr() : pDefaultCell->GetTextClr();
        else
            crFG = (GetTextClr() != CLR_DEFAULT) ? GetTextClr() : pDefaultCell->GetTextClr();

        // If not printing on a color printer, adjust the foreground color
        // to a gray scale if the background color isn't used so that all
        // colors will be visible.  If not, some colors turn to solid black
        // or white when printed and may not show up.  This may be caused by
        // coarse dithering by the printer driver too (see image note below).
        if(pDC->GetDeviceCaps(NUMCOLORS) == 2 && crBG == CLR_DEFAULT)
            crFG = RGB(GetRValue(crFG) * 0.30, GetGValue(crFG) * 0.59,
                GetBValue(crFG) * 0.11);

        // Only erase the background if the color is not the default
        // grid background color.
        if(crBG != CLR_DEFAULT)
        {
            CBrush brush(crBG);
//            rect.right++; rect.bottom++;
            pDC->FillRect(rect, &brush);
//            rect.right--; rect.bottom--;
        }
    }
    else
    {
        crBG = CLR_DEFAULT;
        crFG = RGB(0, 0, 0);
    }

    pDC->SetTextColor(crFG);

    CFont *pFont = GetFontObject();
    if (pFont)
        pDC->SelectObject(pFont);

    /*
    // ***************************************************
    // Disabled - if you need this functionality then you'll need to rewrite.
    // Create the appropriate font and select into DC.
    CFont Font;
    // Bold the fixed cells if not shading the print out.  Use italic
    // font it it is enabled.
    const LOGFONT* plfFont = GetFont();
    if(IsFixed() && !pGrid->GetShadedPrintOut())
    {
        Font.CreateFont(plfFont->lfHeight, 0, 0, 0, FW_BOLD, plfFont->lfItalic, 0, 0,
            ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
#ifndef _WIN32_WCE
            PROOF_QUALITY,
#else
            DEFAULT_QUALITY,
#endif
            VARIABLE_PITCH | FF_SWISS, plfFont->lfFaceName);
    }
    else
        Font.CreateFontIndirect(plfFont);

    pDC->SelectObject(&Font);
    // ***************************************************
    */

    // Draw lines only when wanted on fixed cells.  Normal cell grid lines
    // are handled in OnPrint.

	//zj 2004.6.21 modified
	if(IsFixed())
    //if(pGrid->GetGridLines() != GVL_NONE && IsFixed())
    {
		if(pGrid->GetReport())
		{
			CBrush brush(RGB(255,255,255));
			CRect rt = rect;
			rt.DeflateRect(-1,-1,-1,-1);
            pDC->FillRect(rt, &brush);
		}
		else
		{
			CPen lightpen(PS_SOLID, 1,  ::GetSysColor(COLOR_3DHIGHLIGHT)),
				darkpen(PS_SOLID,  1, ::GetSysColor(COLOR_3DDKSHADOW)),
				*pOldPen = pDC->GetCurrentPen();
			
			pDC->SelectObject(&lightpen);
			pDC->MoveTo(rect.right, rect.top);
			pDC->LineTo(rect.left, rect.top);
			pDC->LineTo(rect.left, rect.bottom);
			
			pDC->SelectObject(&darkpen);
			pDC->MoveTo(rect.right, rect.top);
			pDC->LineTo(rect.right, rect.bottom);
			pDC->LineTo(rect.left, rect.bottom);
			
			rect.DeflateRect(1,1);
			pDC->SelectObject(pOldPen);
		}
    }
	else
	{
//		pDC->Rectangle(rect);

	}

    rect.DeflateRect(GetMargin(), 0);

    if(pGrid->GetImageList() && GetImage() >= 0)
    {
        // NOTE: If your printed images look like fuzzy garbage, check the
        //       settings on your printer driver.  If it's using coarse
        //       dithering and/or vector graphics, they may print wrong.
        //       Changing to fine dithering and raster graphics makes them
        //       print properly.  My HP 4L had that problem.

        IMAGEINFO Info;
        if(pGrid->GetImageList()->GetImageInfo(GetImage(), &Info))
        {
            int nImageWidth = Info.rcImage.right-Info.rcImage.left;
            pGrid->GetImageList()->Draw(pDC, GetImage(), rect.TopLeft(), ILD_NORMAL);
            rect.left += nImageWidth+GetMargin();
        }
    }

    // Draw without clipping so as not to lose text when printed for real
	// DT_NOCLIP removed 01.01.01. Slower, but who cares - we are printing!

   //zj 2004.9.14 add
	CRect textRect(0,0,1,1);
	DWORD fmt = GetFormat();
	CString ttt = GetText();	
	pDC->DrawText(ttt,textRect,DT_CALCRECT);
	
	CRect rt = rect;

	if( fmt & DT_LEFT )
	{
		rt.left = rect.left;
		rt.right = rect.left + textRect.Width();
	}
	if(fmt & DT_RIGHT)
	{
		rt.right = rect.right;
		rt.left = rect.right - textRect.Width();
	}
	
	if(fmt & DT_CENTER)
	{
		rt.left = rect.left;
		rt.right = rect.right;
	}	

	if(fmt & DT_TOP)
	{
		rt.top = rect.top;
		rt.bottom = rect.top + textRect.Height();
	}

	if(fmt & DT_BOTTOM)
	{
		rt.top = rect.bottom - textRect.Height();;
		rt.bottom = rect.bottom;
	}
	if(fmt & DT_VCENTER)
	{
		rt.top = rect.top + (rect.Height()- textRect.Height())/2;
		rt.bottom = rt.top + textRect.Height();
	}

	DrawText(pDC->m_hDC, GetText(), -1, rt, fmt | DT_NOPREFIX);

    //DrawText(pDC->m_hDC, GetText(), -1, rect,
      //  GetFormat() | /*DT_NOCLIP | */ DT_NOPREFIX);

	//**************************************************

	// zj 2004.6.20 add
	if(!pGrid->GetReport())
	{
		if(m_nBorderLine != CBL_NONE && (!IsFixed()))
		{
			CPen pen(PS_SOLID, 0, RGB(0,0,0));
			cellRect.DeflateRect(-1,-1,0,0);
			CPen *pOldPen = (CPen*) pDC->SelectObject(&pen);
			if(m_nBorderLine & CBL_LEFT)
			{
				pDC->MoveTo(cellRect.left, cellRect.top);
				pDC->LineTo(cellRect.left, cellRect.bottom);
			}
			if(m_nBorderLine & CBL_TOP)
			{
				pDC->MoveTo(cellRect.left, cellRect.top);
				pDC->LineTo(cellRect.right, cellRect.top);
			}
			if(m_nBorderLine & CBL_RIGHT)
			{
				pDC->MoveTo(cellRect.right, cellRect.top);
				pDC->LineTo(cellRect.right, cellRect.bottom);
			}
			if(m_nBorderLine & CBL_BOTTOM)
			{
				pDC->MoveTo(cellRect.left, cellRect.bottom);
				pDC->LineTo(cellRect.right, cellRect.bottom);
			}
			pDC->SelectObject(pOldPen);
		}
	}
	else
	{
		if(m_nBorderLine != CBL_NONE)
		{
			CPen pen(PS_SOLID, 0, RGB(0,0,0));
			cellRect.DeflateRect(-1,-1,0,0);
			CPen *pOldPen = (CPen*) pDC->SelectObject(&pen);
			if(m_nBorderLine & CBL_LEFT)
			{
				pDC->MoveTo(cellRect.left, cellRect.top);
				pDC->LineTo(cellRect.left, cellRect.bottom);
			}
			if(m_nBorderLine & CBL_TOP)
			{
				pDC->MoveTo(cellRect.left, cellRect.top);
				pDC->LineTo(cellRect.right, cellRect.top);
			}
			if(m_nBorderLine & CBL_RIGHT)
			{
				pDC->MoveTo(cellRect.right, cellRect.top);
				pDC->LineTo(cellRect.right, cellRect.bottom);
			}
			if(m_nBorderLine & CBL_BOTTOM)
			{
				pDC->MoveTo(cellRect.left, cellRect.bottom);
				pDC->LineTo(cellRect.right, cellRect.bottom);
			}
			pDC->SelectObject(pOldPen);
		}
	}
	//***************************//

    pDC->RestoreDC(nSavedDC);

    return TRUE;
#endif
}

/*****************************************************************************
Callable by derived classes, only
*****************************************************************************/
LRESULT CGridCellBase::SendMessageToParent(int nRow, int nCol, int nMessage)
{
    CGridCtrl* pGrid = GetGrid();
    if( pGrid)
        return pGrid->SendMessageToParent(nRow, nCol, nMessage);
    else
        return 0;
}
//Used for merge cells
//by Huang Wei
void CGridCellBase::Show(BOOL IsShow)
{
	m_Hide=!IsShow;
}
//Used for merge cells
//by Huang Wei
void CGridCellBase::SetMergeRange(CCellRange range)
{
	m_MergeRange=range;
}
//Used for merge cells
//by Huang Wei
CCellRange CGridCellBase::GetMergeRange()
{
	return m_MergeRange;
}
//Used for merge cells
//by Huang Wei
BOOL CGridCellBase::IsMerged()
{
	return m_MergeRange.Count()>0;
}
//Used for merge cells
//by Huang Wei
void CGridCellBase::SetMergeCellID(CCellID cell)
{
	m_MergeCellID=cell;
	if(cell.row!=-1)
		m_IsMergeWithOthers=true;
	else
		m_IsMergeWithOthers=false;

}
//Used for merge cells
//by Huang Wei
CCellID CGridCellBase::GetMergeCellID()
{
	return m_MergeCellID;
}
//Used for merge cells
//by Huang Wei
BOOL CGridCellBase::IsMergeWithOthers()
{
	return m_IsMergeWithOthers;
}
//Used for merge cells
//by Huang Wei
BOOL CGridCellBase::IsShow()
{
	return !m_Hide;
}
//Used for merge cells
//by Huang Wei
void CGridCellBase::UnMerge()
{
	m_Hide=false;
	m_MergeRange.Set();
	m_IsMergeWithOthers=false;
	m_MergeCellID.row=-1;
	m_MergeCellID.col=-1;
}

void CGridCellBase::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);	

	m_MergeRange.Serialize(ar);
	m_MergeCellID.Serialize(ar);
	
	if(ar.IsLoading())
	{
		ar >> m_IsMergeWithOthers >> m_Hide >> m_nState >> m_nType;
		// zj 2004.6.21 add
		ar >> m_nBorderLine;
		//*********************//
	}
	else
	{
		ar << m_IsMergeWithOthers << m_Hide << ( m_nState & ~GVIS_SELECTED ) << m_nType;
		// zj 2004.6.21 add
		ar << m_nBorderLine;
		//*********************//
	}
}
