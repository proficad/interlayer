// GridCellDouble.cpp: implementation of the CGridCellDouble class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GridCellDouble.h"
#include "../GridCtrl_src/amsEdit.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//IMPLEMENT_DYNCREATE(CGridCellDouble, CGridCell)
IMPLEMENT_SERIAL(CGridCellDouble, CGridCell, VERSIONABLE_SCHEMA | CURVERSION )

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGridCellDouble::CGridCellDouble()
{
	m_nType = 6;
	m_nEditNumericLeftDigits = 20;
	m_nEditNumericRightDigits = 2;
	m_bSigned = TRUE;

}

CGridCellDouble::~CGridCellDouble()
{

}


// Create a control to do the editing
BOOL CGridCellDouble::Edit(int nRow, int nCol, CRect rect, CPoint /* point */, UINT nID, UINT nChar)
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
	
	m_pEditWnd = new CAMSNumericEdit((CWnd*)pGrid, rect, dwStyle, nID, nRow, nCol, GetText(), nChar);
	
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
	
	CAMSNumericEdit* p = (CAMSNumericEdit*)m_pEditWnd;
	p->SetMaxWholeDigits(m_nEditNumericLeftDigits);
	p->SetMaxDecimalPlaces(m_nEditNumericRightDigits);
	p->SetTextColor(TextClr);
	p->SetBackgroundColor(TextBkClr);
    p->AllowNegative(m_bSigned);

    return TRUE;
}

// Cancel the editing.
void CGridCellDouble::EndEdit()
{
    if (m_pEditWnd)
	{
		CAMSNumericEdit * p = (CAMSNumericEdit*) m_pEditWnd;
		m_dValue = p->GetTextAsDouble();		
        ((CAMSCurrencyEdit*)m_pEditWnd)->EndEdit();

		if(m_pEditFont!=NULL)
		{
			m_pEditFont->DeleteObject();
			delete m_pEditFont;
			m_pEditFont = NULL;
		}
	}
}


void CGridCellDouble::Serialize(CArchive &ar)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	CGridCell::Serialize(ar);
	ar.SerializeClass(RUNTIME_CLASS(CGridCellDouble));
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
			ar >> m_dValue >> m_bSigned;
			ar >> m_nEditNumericLeftDigits;
			ar >> m_nEditNumericRightDigits;
			break;
		default:
			// 未知版本			
			break;
		}	
		
	}
	else
	{
		ar << m_dValue << m_bSigned;
		ar << m_nEditNumericLeftDigits;
		ar << m_nEditNumericRightDigits;
	}
}

double CGridCellDouble::GetTextAsDouble()
{
	m_dValue = _tcstod(m_strText, NULL);	
	return m_dValue;
}

void CGridCellDouble::SetText(LPCTSTR szText)
{
	if(strlen(szText) == 0 )
	{
		m_strText="";
		return;
	}
	m_dValue = _tcstod(szText, NULL);
	CString format;
	format.Format("%%.%dlf", m_nEditNumericRightDigits);
	m_strText.Format(format, m_dValue);	
}

void CGridCellDouble::SetCellValue(double Value)
{
	m_dValue = Value;
	CString format;
	format.Format("%%.%dlf", m_nEditNumericRightDigits);
	m_strText.Format(format, Value);
}
