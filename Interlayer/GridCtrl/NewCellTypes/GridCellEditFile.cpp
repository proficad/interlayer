// GridCellEditFile.cpp: implementation of the CGridCellEditFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GridCellEditFile.h"
//#include "GridCtrl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(CGridCellEditFile, CGridCell, VERSIONABLE_SCHEMA | CURVERSION )

CGridCellEditFile::CGridCellEditFile() : CGridCell()
{
	m_bDown = FALSE;
	m_nType = 9;
}

CSize CGridCellEditFile::GetCellExtent(CDC* pDC)
{
    // Using SM_CXHSCROLL as a guide to the size of the checkbox
	int nWidth = GetSystemMetrics(SM_CXHSCROLL) + 2*GetMargin();
	return CGridCell::GetCellExtent(pDC) + CSize(nWidth, nWidth);
}

// i/o:  i=dims of cell rect; o=dims of text rect
BOOL CGridCellEditFile::GetTextRect( LPRECT pRect)
{
    BOOL bResult = CGridCell::GetTextRect(pRect);
    if (bResult)
    {
        int nWidth = GetSystemMetrics(SM_CXHSCROLL) + 2*GetMargin();
        pRect->right -= nWidth;
        
		if( pRect->right < pRect->left )
			pRect->right = pRect->left;
    }
    return bResult;
}

// Override draw so that when the cell is selected, a drop arrow is shown in the RHS.
BOOL CGridCellEditFile::Draw(CDC* pDC, int nRow, int nCol, CRect rect,  BOOL bEraseBkgnd /*=TRUE*/)
{
    BOOL bResult = CGridCell::Draw(pDC, nRow, nCol, rect,  bEraseBkgnd);

#ifndef _WIN32_WCE
    // Store the cell's dimensions for later
    m_Rect = rect;

    CRect ButtonRect = GetButtonPlacement();
    rect.left = ButtonRect.right;

    // enough room to draw?
    // if (CheckRect.Width() < rect.Width() && CheckRect.Height() < rect.Height()) {

    // Do the draw 
    pDC->DrawFrameControl(GetButtonPlacement(), DFC_BUTTON, 
	    (m_bDown)? DFCS_PUSHED | DFCS_BUTTONPUSH : DFCS_BUTTONPUSH);

    // }
#endif
    return bResult;
}

void CGridCellEditFile::OnClick(CPoint PointCellRelative)
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
	
	m_bDown = FALSE;
	GetGrid()->InvalidateRect(m_Rect);

	if (GetButtonPlacement().PtInRect(PointCellRelative))
	{
		static char szFilter[] = "所有文件 (*.*)|*.*||"; 
		CString strFileName = GetText();
		
		CFileDialog dlg(TRUE, NULL, strFileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, NULL);
		//------用来检测目前的操作系统的版本信息	
		int nStructSize = 0;	
		DWORD dwVersion, dwWindowsMajorVersion, dwWindwosMinorVersion;	
		dwVersion = GetVersion();	
		dwWindowsMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));	
		dwWindwosMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));	
		if (dwVersion < 0x80000000)
		{		
			nStructSize = 88; // 2k,xp下显示新的版本		
		}	
		else		
		{		
			nStructSize = 76; // 98下显示老版本		
		}	
		dlg.m_ofn.lStructSize = nStructSize; //------用来检测目前的操作系统的版本信息
		
		if (dlg.DoModal() == IDOK)
		{
			strFileName = dlg.GetPathName();
			SetText(strFileName);
			GetGrid()->SetModified();			
		}
	}
}

void CGridCellEditFile::OnClickDown(CPoint PointCellRelative)
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
	
	if (GetButtonPlacement().PtInRect(PointCellRelative))
	{		
		m_bDown = TRUE;
		GetGrid()->InvalidateRect(m_Rect);
	}
}


//////////////////////////////////////////////////////////////////////
// Protected implementation
//////////////////////////////////////////////////////////////////////

// Returns the dimensions and placement of the checkbox in client coords.
CRect CGridCellEditFile::GetButtonPlacement()
{
	int nWidth = GetSystemMetrics(SM_CXHSCROLL);
	CRect place = m_Rect;// + CSize(GetMargin(), GetMargin());
	
	place.top +=(place.Height() - nWidth)/2;
    place.left = place.right - nWidth;
    place.bottom = place.top + nWidth;

	return place;
}

void CGridCellEditFile::Serialize(CArchive &ar)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	CGridCell::Serialize(ar);
	ar.SerializeClass(RUNTIME_CLASS(CGridCellEditFile));
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
			ar >> m_Rect;
			break;
		default:
			// 未知版本			
			break;
		}		
	}
	else
	{
		ar << m_Rect;
	}
}
