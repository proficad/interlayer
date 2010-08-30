// GridCellMask.cpp: implementation of the CGridCellMask class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GridCellMask.h"

#include "../GridCtrl_src/amsEdit.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//IMPLEMENT_DYNCREATE(CGridCellMask, CGridCell)
IMPLEMENT_SERIAL(CGridCellMask, CGridCell, VERSIONABLE_SCHEMA | CURVERSION )

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGridCellMask::CGridCellMask()
{
	m_nType = 8;
	m_strEditMaskedMask = "电话：(###)-########";
}

CGridCellMask::~CGridCellMask()
{

}



// Create a control to do the editing
BOOL CGridCellMask::Edit(int nRow, int nCol, CRect rect, CPoint /* point */, UINT nID, UINT nChar)
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
	
	m_pEditWnd = new CAMSMaskedEdit((CWnd*)pGrid, rect, dwStyle, nID, nRow, nCol, GetText(), nChar);
	
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
	
	CAMSMaskedEdit* p = (CAMSMaskedEdit*)m_pEditWnd;
	p->SetMask(m_strEditMaskedMask);
	if (m_strEditMaskedMask.IsEmpty())
		p->SetText("");
	p->SetTextColor(TextClr);
	p->SetBackgroundColor(TextBkClr);    

    return TRUE;
}

// Cancel the editing.
void CGridCellMask::EndEdit()
{
    if (m_pEditWnd)
	{
		((CAMSMaskedEdit*)m_pEditWnd)->EndEdit();
		m_pEditFont->DeleteObject();
		delete m_pEditFont;
		m_pEditFont = NULL;
	}
}


void CGridCellMask::Serialize(CArchive &ar)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	CGridCell::Serialize(ar);
	ar.SerializeClass(RUNTIME_CLASS(CGridCellMask));
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
			ar >> m_strEditMaskedMask;
			break;
		default:
			// 未知版本			
			break;
		}	
		
	}
	else
	{
		ar << m_strEditMaskedMask;		
	}
}

void CGridCellMask::SetText(LPCTSTR szText)
{
	CString strText = szText;
	CString strNewText;
	int nMaskLen = m_strEditMaskedMask.GetLength();
	
	// If the mask is empty, allow anything
	if (!nMaskLen)
		strNewText = strText;
	else
	{
		// Remove any characters not in the mask
		for (int iPos = 0, iMaskPos = 0, nLen = strText.GetLength(); iPos < nLen; iPos++, iMaskPos++)
		{
			TCHAR c = strText[iPos];
			TCHAR cMask = (iMaskPos < nMaskLen ? m_strEditMaskedMask[iMaskPos] : 0);

			// If we've reached the end of the mask, break
			if (!cMask)
				break;

			// If it's a digit, look for the next digit placeholder (#) in the mask 
			else if (_istdigit(c))
			{
				for (; iMaskPos < nMaskLen; iMaskPos++)
				{
					cMask = m_strEditMaskedMask[iMaskPos];
					if (cMask == '#')
					{
						strNewText += c;
						break;
					}

					strNewText += cMask;
				}
			}

			// If it's the same character as the mask's allow it.
			else if (c == cMask)
				strNewText += c;
			else
				break;
		}
	}

	m_strText = strNewText;	
}


