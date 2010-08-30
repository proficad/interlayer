// GridCellCheck.cpp : implementation file
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../GridCtrl_src/GridCell.h"
#include "../GridCtrl_src/GridCtrl.h"

#include "GridCellCheck.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//IMPLEMENT_DYNCREATE(CGridCellCheck, CGridCell)
IMPLEMENT_SERIAL(CGridCellCheck, CGridCell, VERSIONABLE_SCHEMA | CURVERSION )

CGridCellCheck::CGridCellCheck() : CGridCell()
{
    m_bChecked = FALSE;
    //m_Rect.IsRectNull();
	m_nType = 2;
}

CSize CGridCellCheck::GetCellExtent(CDC* pDC)
{
    // Using SM_CXHSCROLL as a guide to the size of the checkbox
	int nWidth = GetSystemMetrics(SM_CXHSCROLL) + 2*GetMargin();
	return CGridCell::GetCellExtent(pDC) + CSize(nWidth, nWidth);
}

// i/o:  i=dims of cell rect; o=dims of text rect
BOOL CGridCellCheck::GetTextRect( LPRECT pRect)
{
    BOOL bResult = CGridCell::GetTextRect(pRect);
    if (bResult)
    {
        int nWidth = GetSystemMetrics(SM_CXHSCROLL) + 2*GetMargin();
        pRect->left += nWidth;
        if (pRect->left > pRect->right)
            pRect->left = pRect->right;
    }
    return bResult;
}

// Override draw so that when the cell is selected, a drop arrow is shown in the RHS.
BOOL CGridCellCheck::Draw(CDC* pDC, int nRow, int nCol, CRect rect,  BOOL bEraseBkgnd /*=TRUE*/)
{
    BOOL bResult = CGridCell::Draw(pDC, nRow, nCol, rect,  bEraseBkgnd);

#ifndef _WIN32_WCE
    // Store the cell's dimensions for later
    m_Rect = rect;

    CRect CheckRect = GetCheckPlacement();
    rect.left = CheckRect.right;

    // enough room to draw?
    // if (CheckRect.Width() < rect.Width() && CheckRect.Height() < rect.Height()) {

    // Do the draw 
    pDC->DrawFrameControl(GetCheckPlacement(), DFC_BUTTON, 
	    (m_bChecked)? DFCS_BUTTONCHECK | DFCS_CHECKED : DFCS_BUTTONCHECK);

    // }
#endif
    return bResult;
}

void CGridCellCheck::OnClick(CPoint PointCellRelative)
{
	// PointCellRelative is relative to the topleft of the cell. Convert to client coords
	PointCellRelative += m_Rect.TopLeft();

	// GetCheckPlacement returns the checkbox dimensions in client coords. Only check/
	// uncheck if the user clicked in the box

	//  张军修改于 [2005-10-10 19:46]
	int nState = GetState();

	if(nState & GVIS_READONLY )
		return;
	//////////////////////////////////////////////////////////////////////////
	
	if (GetCheckPlacement().PtInRect(PointCellRelative))
	{		
		m_bChecked = !m_bChecked;
		GetGrid()->InvalidateRect(m_Rect);
		GetGrid()->SetModified();
	}
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL CGridCellCheck::SetCheck(BOOL bChecked /*=TRUE*/)
{
	BOOL bTemp = m_bChecked;
	m_bChecked = bChecked;
	if (!m_Rect.IsRectEmpty() )
	{
		CGridCtrl *p = GetGrid();
		if( p->m_hWnd != NULL )
			p->InvalidateRect(m_Rect);	
	}

	return bTemp;
}

BOOL CGridCellCheck::GetCheck()
{
	return m_bChecked;
}

//////////////////////////////////////////////////////////////////////
// Protected implementation
//////////////////////////////////////////////////////////////////////

// Returns the dimensions and placement of the checkbox in client coords.
CRect CGridCellCheck::GetCheckPlacement()
{
	int nWidth = GetSystemMetrics(SM_CXHSCROLL);
	CRect place = m_Rect;// + CSize(GetMargin(), GetMargin());
	
	place.top +=(place.Height() - nWidth)/2;
    place.right = place.left + nWidth;
    place.bottom = place.top + nWidth;

	/* for centering
	int nDiff = (place.Width() - nWidth)/2;
	if (nDiff > 0)
	{
		place.left += nDiff;
		place.right = place.left + nWidth;
	}
	nDiff = (place.Height() - nWidth)/2;
	if (nDiff > 0)
	{
		place.top += nDiff;
		place.bottom = place.top + nWidth;
	}
    */

	return place;
}

void CGridCellCheck::Serialize(CArchive &ar)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	CGridCell::Serialize(ar);
	ar.SerializeClass(RUNTIME_CLASS(CGridCellCheck));
	if( ar.IsLoading() )	
	{
		int nVersion = ar.GetObjectSchema();

		switch(nVersion)
		{
		case 0:
			// 读以前版本
			
			break;
		case 100:
			// 读当前版本			
			// get the document back pointer from the archive
			ar >> m_bChecked;		
			ar >> m_Rect;
			break;
		default:
			// 未知版本			
			break;
		}	
		
	}
	else
	{
		ar << m_bChecked;		
		ar << m_Rect;
	}
}
