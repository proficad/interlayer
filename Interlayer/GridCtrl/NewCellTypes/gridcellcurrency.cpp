// gridcellcurrency.cpp: implementation of the CGridCellCurrency class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "gridcellcurrency.h"
#include "../GridCtrl_src/amsEdit.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//IMPLEMENT_DYNCREATE(CGridCellCurrency, CGridCell)
IMPLEMENT_SERIAL(CGridCellCurrency, CGridCell, VERSIONABLE_SCHEMA | CURVERSION )

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGridCellCurrency::CGridCellCurrency()
{
	m_nType = 7;
	m_nDigitsInGroup = 3;
	m_nMaxDecimalPlaces = 3;
	m_nMaxWholeDigits = 12;
	m_bSigned = TRUE;
}

CGridCellCurrency::~CGridCellCurrency()
{	
}

// Create a control to do the editing
BOOL CGridCellCurrency::Edit(int nRow, int nCol, CRect rect, CPoint /* point */, UINT nID, UINT nChar)
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
	
	m_pEditWnd = new CAMSCurrencyEdit((CWnd*)pGrid, rect, dwStyle, nID, nRow, nCol, GetText(), nChar);
	
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
	
	CAMSCurrencyEdit* p = (CAMSCurrencyEdit*)m_pEditWnd;
	p->SetTextColor(TextClr);
	p->SetBackgroundColor(TextBkClr);

	p->SetMaxDecimalPlaces(m_nMaxDecimalPlaces);
	p->SetMaxWholeDigits(m_nMaxWholeDigits);
	p->SetDigitsInGroup(m_nDigitsInGroup);
	p->AllowNegative(m_bSigned);

    return TRUE;
}

// Cancel the editing.
void CGridCellCurrency::EndEdit()
{
    if (m_pEditWnd)
	{
		CAMSCurrencyEdit * p = (CAMSCurrencyEdit*) m_pEditWnd;
		m_dMoney = p->GetTextAsDouble();		
        ((CAMSCurrencyEdit*)m_pEditWnd)->EndEdit();

		m_pEditFont->DeleteObject();
		delete m_pEditFont;
		m_pEditFont = NULL;
	}
}


void CGridCellCurrency::Serialize(CArchive &ar)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());
	CGridCell::Serialize(ar);
	ar.SerializeClass(RUNTIME_CLASS(CGridCellCurrency));
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
			ar >> m_dMoney >> m_bSigned;
			ar >> m_nDigitsInGroup >> m_nMaxDecimalPlaces >> m_nMaxWholeDigits;
			break;
		default:
			// 未知版本			
			break;
		}	
		
	}
	else
	{
		ar << m_dMoney << m_bSigned;
		ar << m_nDigitsInGroup << m_nMaxDecimalPlaces << m_nMaxWholeDigits;
	}
}

void CGridCellCurrency::SetText(LPCTSTR szText)
{
	CString m_strPrefix;
	
	m_strPrefix = _T("$");
	
	// Get the system's current settings
	TCHAR szValue[10];
	if (::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SCURRENCY, szValue, 0))
	{
		::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SCURRENCY, szValue, sizeof(szValue));
		m_strPrefix = szValue;
	}

	int nPrefixLen = m_strPrefix.GetLength();

	m_strText = szText;
	CString strNewText = GetValidText();

	int len = strNewText.GetLength();

	int nDec = strNewText.Find('.');

	CString LeftStr, RightStr;
	LeftStr.Empty(); 
	RightStr.Empty();

	if( nDec != -1 )
	{
		LeftStr = strNewText.Left(nDec);
		RightStr = strNewText.Right(strNewText.GetLength() - nDec );
	}
	else
	{
		LeftStr = strNewText;
	}

	CString str;

	if( !LeftStr.IsEmpty() )
	{
		int n = LeftStr.GetLength();
		int m = n%m_nDigitsInGroup ;
		int k = n/m_nDigitsInGroup;
		int i;
		
		for( i = 0; i <	m; i++ )
		{
			str += LeftStr[i];
			if( i == m-1 )
				str += ',';
		}

		for( i = 0; i < k; i++)
		{
			for( int j = 0; j<m_nDigitsInGroup; j++)
			{
				str += LeftStr[i*m_nDigitsInGroup+j+m];
			}			
			str += ',';
		}
		
	}	

	m_strText = m_strPrefix + str.Left(str.GetLength()-1) + RightStr;	
}

double CGridCellCurrency::GetTextAsDouble()
{	
	CString strNewText = GetValidText();
	
	m_dMoney = _tcstod(strNewText, NULL);

	return m_dMoney;
}

void CGridCellCurrency::SetCellValue(double value)
{
	m_dMoney = value;

	CString str;	
	str.Format("%lf", value);
	SetText(str);
}

CString CGridCellCurrency::GetValidText()
{
	CString m_strPrefix;

	m_strPrefix = _T("$");
	
	// Get the system's current settings
	TCHAR szValue[10];
	if (::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SCURRENCY, szValue, 0))
	{
		::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SCURRENCY, szValue, sizeof(szValue));
		m_strPrefix = szValue;
	}

	CString strNewText;
	bool bIsNegative = false;
	int nPrefixLen = m_strPrefix.GetLength();

	// Remove any invalid characters from the number
	for (int iPos = 0, nDecimalPos = -1, nNewLen = 0, nLen = m_strText.GetLength(); iPos < nLen; iPos++)
	{
		TCHAR c = m_strText[iPos];

		// Check for a negative sign
		if (iPos == nPrefixLen && c == '-')
		{
			strNewText += c;
			nNewLen++;
			bIsNegative = true;
		}
		// Check for a digit
		else if (_istdigit(c))
		{
			// Make sure it doesn't go beyond the limits
			if (nDecimalPos < 0 && nNewLen == m_nMaxWholeDigits + bIsNegative)
				continue;

			if (nDecimalPos >= 0 && nNewLen > nDecimalPos + m_nMaxDecimalPlaces)
				break;

			strNewText += c;
			nNewLen++;
		}
		// Check for a decimal point
		else if (c == '.' && nDecimalPos < 0)
		{
			if (m_nMaxDecimalPlaces == 0)
				break;

			strNewText += c;
			nDecimalPos = nNewLen;
			nNewLen++;
		}
	}

	return strNewText;
}
