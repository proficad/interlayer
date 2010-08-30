// GridCellNumeric.cpp: implementation of the CGridCellNumeric class.
//
// Written by Andrew Truckle [ajtruckle@wsatkins.co.uk]
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GridCellNumeric.h"
#include "../GridCtrl_src/inplaceedit.h"
#include "../GridCtrl_src/GridCtrl.h"
#include "../GridCtrl_src/amsEdit.h"

//IMPLEMENT_DYNCREATE(CGridCellNumeric, CGridCell)
IMPLEMENT_SERIAL(CGridCellNumeric, CGridCell, VERSIONABLE_SCHEMA | CURVERSION )

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGridCellNumeric::CGridCellNumeric()
{
	m_nType = 5;
	m_bSigned = TRUE;
}

CGridCellNumeric::~CGridCellNumeric()
{

}
// Create a control to do the editing
BOOL CGridCellNumeric::Edit(int nRow, int nCol, CRect rect, CPoint /* point */, UINT nID, UINT nChar)
{
    DWORD dwStyle = ES_LEFT;
	if (GetFormat() & DT_RIGHT) 
		dwStyle = ES_RIGHT;
	else if (GetFormat() & DT_CENTER) 
		dwStyle = ES_CENTER;
	
	m_bEditing = TRUE;
	
	// InPlaceEdit auto-deletes itself
	CGridCtrl* pGrid = GetGrid();
	DWORD nFormat = GetFormat();
	
	if( nFormat & DT_WORDBREAK )
		dwStyle |=ES_MULTILINE|ES_WANTRETURN;
	
	m_pEditWnd = new CAMSIntegerEdit(pGrid, rect, dwStyle, nID, nRow, nCol, GetText(), nChar);
	
	m_pEditFont = new CFont;
	m_pEditFont->CreateFontIndirect(GetFont());
	ASSERT(m_pEditFont);
	if (m_pEditFont)
		m_pEditWnd->SetFont(m_pEditFont);
	
	CGridDefaultCell *pDefaultCell = (CGridDefaultCell*) GetDefaultCell();
	if (!pDefaultCell)
        return FALSE;
	
	// Set up text and background colours
	COLORREF TextClr, TextBkClr;
	
	TextClr = (GetTextClr() == CLR_DEFAULT)? pDefaultCell->GetTextClr() : GetTextClr();
	if (GetBackClr() == CLR_DEFAULT)
		TextBkClr = pDefaultCell->GetBackClr();
	else
		TextBkClr = GetBackClr();
	
	CAMSIntegerEdit* p = (CAMSIntegerEdit*)m_pEditWnd;
	p->SetTextColor(TextClr);
	p->SetBackgroundColor(TextBkClr);
	p->AllowNegative(m_bSigned);
	
    return TRUE;
}

// Cancel the editing.
void CGridCellNumeric::EndEdit()
{
    if (m_pEditWnd)
	{
		CAMSIntegerEdit*p = (CAMSIntegerEdit*)m_pEditWnd;
		m_nValue = p->GetTextAsLong();
        ((CAMSIntegerEdit*)m_pEditWnd)->EndEdit();
	}
}

long CGridCellNumeric::GetTextAsLong()
{
	m_nValue = _ttol(m_strText);
	return m_nValue;
}

void CGridCellNumeric::SetText(LPCTSTR szText)
{
	if(strlen(szText) == 0 )
	{
		m_strText="";
		return;
	}
	m_nValue = _ttol(szText);
	m_strText.Format("%ld", m_nValue);
}

void CGridCellNumeric::SetCellLong(long value)
{
	m_nValue = value;
	m_strText.Format("%ld", m_nValue);
}

void CGridCellNumeric::Serialize(CArchive &ar)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	CGridCell::Serialize(ar);
	ar.SerializeClass(RUNTIME_CLASS(CGridCellNumeric));
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
			ar >> m_nValue >> m_bSigned;
			break;
		default:
			// 未知版本			
			break;
		}	
		
	}
	else
	{
		ar << m_nValue << m_bSigned;
	}
}
