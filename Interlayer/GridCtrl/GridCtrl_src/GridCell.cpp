// GridCell.cpp : implementation file
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GridCell.h"
#include "InPlaceEdit.h"
#include "GridCtrl.h"
#include "amsEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//IMPLEMENT_DYNCREATE(CGridCell, CGridCellBase)
IMPLEMENT_SERIAL(CGridCell, CGridCellBase, VERSIONABLE_SCHEMA | CURVERSION )

IMPLEMENT_SERIAL(CGridDefaultCell, CGridCell, VERSIONABLE_SCHEMA | CURVERSION )

/////////////////////////////////////////////////////////////////////////////
// GridCell

CGridCell::CGridCell()
{
    m_plfFont = NULL;
	m_pEditFont = NULL;
	CGridCell::Reset();
	m_nType = -1;
}

CGridCell::~CGridCell()
{
	if( m_pEditFont )
	{
		m_pEditFont->DeleteObject();
		delete m_pEditFont;
	}
	if( m_plfFont )
		delete m_plfFont;	
}

/////////////////////////////////////////////////////////////////////////////
// GridCell Attributes
//Used for merge cells,remove const
//by Huang Wei
void CGridCell::operator=( CGridCell& cell)
{
    if (this != &cell) CGridCellBase::operator=(cell);
}

void CGridCell::Reset()
{
    CGridCellBase::Reset();

    m_strText.Empty();
    m_nImage   = -1;
    m_pGrid    = NULL;
    m_bEditing = FALSE;
    m_pEditWnd = NULL;

    m_nFormat = (DWORD)-1;       // Use default from CGridDefaultCell
    m_crBkClr = CLR_DEFAULT;     // Background colour (or CLR_DEFAULT)
    m_crFgClr = CLR_DEFAULT;     // Forground colour (or CLR_DEFAULT)
    m_nMargin = (UINT)-1;        // Use default from CGridDefaultCell

    delete m_plfFont;
    m_plfFont = NULL;            // Cell font
}

void CGridCell::SetFont(const LOGFONT* plf)
{
    if (plf == NULL)
    {
        delete m_plfFont;
        m_plfFont = NULL;
    }
    else
    {
        if (!m_plfFont)
            m_plfFont = new LOGFONT;
        if (m_plfFont)
            memcpy(m_plfFont, plf, sizeof(LOGFONT)); 
    }
}

LOGFONT* CGridCell::GetFont() const
{
    if (m_plfFont == NULL)
    {
        CGridDefaultCell *pDefaultCell = (CGridDefaultCell*) GetDefaultCell();
        if (!pDefaultCell)
            return NULL;

        return pDefaultCell->GetFont();
    }

    return m_plfFont; 
}

CFont* CGridCell::GetFontObject() const
{
    // If the default font is specified, use the default cell implementation
    if (m_plfFont == NULL)
    {
        CGridDefaultCell *pDefaultCell = (CGridDefaultCell*) GetDefaultCell();
        if (!pDefaultCell)
            return NULL;

        return pDefaultCell->GetFontObject();
    }
    else
    {
        static CFont Font;
        Font.DeleteObject();
        Font.CreateFontIndirect(m_plfFont);
        return &Font;
    }
}

DWORD CGridCell::GetFormat() const
{
    if (m_nFormat == (DWORD)-1)
    {
        CGridDefaultCell *pDefaultCell = (CGridDefaultCell*) GetDefaultCell();
        if (!pDefaultCell)
            return 0;

        return pDefaultCell->GetFormat();
    }

    return m_nFormat; 
}

UINT CGridCell::GetMargin() const           
{
    if (m_nMargin == (UINT)-1)
    {
        CGridDefaultCell *pDefaultCell = (CGridDefaultCell*) GetDefaultCell();
        if (!pDefaultCell)
            return 0;

        return pDefaultCell->GetMargin();
    }

    return m_nMargin; 
}

/////////////////////////////////////////////////////////////////////////////
// GridCell Operations

BOOL CGridCell::Edit(int nRow, int nCol, CRect rect, CPoint /* point */, UINT nID, UINT nChar)
{
    if ( m_bEditing )
	{      
        if (m_pEditWnd)
		    m_pEditWnd->SendMessage ( WM_CHAR, nChar );    
    }  
	else  
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
		
		m_pEditWnd = new CAMSEdit(pGrid, rect, dwStyle, nID, nRow, nCol, GetText(), nChar);

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
			
		CAMSEdit* p = (CAMSEdit*)m_pEditWnd;
		p->SetTextColor(TextClr);
		p->SetBackgroundColor(TextBkClr);
		//m_pEditWnd = new CAMSNumericEdit(pGrid, rect, dwStyle, nID, nRow, nCol, GetText(), nChar);
    }
    return TRUE;
}

void CGridCell::EndEdit()
{
    if (m_pEditWnd)
	{
        ((CAMSEdit*)m_pEditWnd)->EndEdit();
	}
}

void CGridCell::OnEndEdit()
{
	if( m_pEditFont)
	{
		m_pEditFont->DeleteObject();
		delete m_pEditFont;
		m_pEditFont = NULL;
	}

    m_bEditing = FALSE;
    m_pEditWnd = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CGridDefaultCell

CGridDefaultCell::CGridDefaultCell() 
{
#ifdef _WIN32_WCE
    m_nFormat = DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX;
#else
    m_nFormat = DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX | DT_END_ELLIPSIS;
#endif
    m_crFgClr = CLR_DEFAULT;
    m_crBkClr = CLR_DEFAULT;
    m_Size    = CSize(30,10);
    m_dwStyle = 0;

#ifdef _WIN32_WCE
    LOGFONT lf;
    GetObject(GetStockObject(SYSTEM_FONT), sizeof(LOGFONT), &lf);
    SetFont(&lf);
#else // not CE

	OSVERSIONINFO osvi; 
	memset(&osvi, 0, sizeof(OSVERSIONINFO)); 
	osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO); 
	GetVersionEx (&osvi);

	NONCLIENTMETRICS ncm;
	int size;

	if( osvi.dwMajorVersion > 5)
	{
		ncm.cbSize = sizeof(NONCLIENTMETRICS);
		size = sizeof(NONCLIENTMETRICS);
	}
	else
	{
		ncm.cbSize = sizeof(NONCLIENTMETRICS) - sizeof(ncm.iPaddedBorderWidth);
		size = sizeof(NONCLIENTMETRICS) - sizeof(ncm.iPaddedBorderWidth);
	}

	VERIFY(SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0));
	
	SetFont(&(ncm.lfMessageFont));
#endif
}

CGridDefaultCell::~CGridDefaultCell()
{
    m_Font.DeleteObject(); 
}

void CGridDefaultCell::SetFont(const LOGFONT* plf)
{
    ASSERT(plf);

    if (!plf) return;

    m_Font.DeleteObject();
    m_Font.CreateFontIndirect(plf);

    CGridCell::SetFont(plf);

    // Get the font size and hence the default cell size
    CDC* pDC = CDC::FromHandle(::GetDC(NULL));
    if (pDC)
    {
        CFont* pOldFont = pDC->SelectObject(&m_Font);

        SetMargin(pDC->GetTextExtent(_T(" "), 1).cx);
        m_Size = pDC->GetTextExtent(_T(" XXXXXXXXXXXX "), 14);
        m_Size.cy = (m_Size.cy * 3) / 2;

        pDC->SelectObject(pOldFont);
        ReleaseDC(NULL, pDC->GetSafeHdc());
    }
    else
    {
        SetMargin(3);
        m_Size = CSize(40,16);
    }
}

LOGFONT* CGridDefaultCell::GetFont() const
{
    ASSERT(m_plfFont);  // This is the default - it CAN'T be NULL!
    return m_plfFont;
}

CFont* CGridDefaultCell::GetFontObject() const
{
    ASSERT(m_Font.GetSafeHandle());
    return (CFont*) &m_Font; 
}

void CGridCell::Serialize(CArchive &ar)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState());

	CGridCellBase::Serialize(ar);
	ar.SerializeClass(RUNTIME_CLASS(CGridCell));

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
			ar >> m_strText;
			ar >> m_lParam; 
			ar >> m_nImage; 
			ar >> m_nFormat;
			ar >> m_crFgClr;
			ar >> m_crBkClr;
			if (!m_plfFont)
				m_plfFont = new LOGFONT;
			ar.Read(m_plfFont, sizeof(LOGFONT)) ;
			ar >> m_nMargin;

			break;
		default:
			// 未知版本			
			break;
		}	
		
	}
	else
	{
		ar << m_strText;
		ar << m_lParam; 
		ar << m_nImage; 
		ar << m_nFormat;
		ar << m_crFgClr;
		ar << m_crBkClr;
		LOGFONT *lp = GetFont();
		ar.Write(lp, sizeof(LOGFONT)) ;
		ar << m_nMargin;
	}
}

void CGridDefaultCell::Serialize(CArchive &ar)
{

	AFX_MANAGE_STATE(AfxGetAppModuleState());
	CGridCell::Serialize(ar);
	ar.SerializeClass(RUNTIME_CLASS(CGridDefaultCell));
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
			
			break;
		default:
			// 未知版本			
			break;
		}	
		
	}
	else
	{		
	}
}
