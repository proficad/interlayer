// EditEx.cpp : 实现文件
//

#include "stdafx.h"
#include "EditEx.h"


// CEditEx

IMPLEMENT_DYNCREATE(CEditEx, CEdit)

BEGIN_MESSAGE_MAP(CEditEx, CEdit)
    //{{AFX_MSG_MAP(CEditEx)
    ON_WM_CONTEXTMENU()
    ON_WM_CHAR()
	ON_WM_SETFOCUS()
	ON_WM_CAPTURECHANGED()
	ON_WM_KILLFOCUS()
	ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
    ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Public Interfaces:
//

// Construction:
//
//** CEditEx::CEditEx()
//
CEditEx::CEditEx() : m_pCFont(0)
{
    // Use default popup menu
    //
    m_MenuResourceID = 0;

    // Use system colors for defaults
    //
    m_crTextColor = ::GetSysColor( COLOR_WINDOWTEXT );
    m_crBkColor   = ::GetSysColor( COLOR_WINDOW     );

    // The default brush type: SOLID
    //
    CreateBrushType();

    // Edit mask is null
    //
    m_pEditMask = 0;	
}

//** CEditEx::~CEditEx()
//
CEditEx::~CEditEx()
{
    if ( m_pCFont    ) delete m_pCFont; 
    if ( m_pEditMask ) delete m_pEditMask;
}

// Note: Copy construction is blocked for this class.
//       This is because there would be no defined
//       m_hWnd during the construction of the object.
//
// CEditEx::CEditEx( const CEditEx& o )
//

// Allow = operator to be used for copying basics.
//
CEditEx& CEditEx::operator = ( const CEditEx& o )
{

    _ASSERT( o != *this ); // You probably did not mean to do this...

    if ( o == *this ) return *this; // copying self...
    
    bkColor( o.m_crBkColor );
    textColor( o.m_crTextColor );

    if ( o.m_pCFont ) {
         CLogFont pLogFont;
         o.m_pCFont->GetLogFont( &pLogFont );
         setFont( &pLogFont );
    }

    return *this;
}
//** void SetFont( const LOGFONT* lpLogFont )
//

void CEditEx::setFont( const LOGFONT* lpLogFont )
{
    _ASSERT( lpLogFont ); // logfont is not defined!!!

    if ( !lpLogFont ) return;

    if ( m_pCFont ) delete m_pCFont;
    
    m_pCFont = new CFont;
    m_pCFont->CreateFontIndirect( lpLogFont );

    SetFont( m_pCFont );
}

void CEditEx::setFont( LONG fontHeight      /* = -8                         */, 
                       LONG fontWeight      /* = FW_NORMAL                  */,
                       UCHAR pitchAndFamily /* = DEFAULT_PITCH | FF_DONTCARE*/,
                       LPCSTR faceName      /* = _T("MS Sans Serif")        */ )
{
    if ( m_pCFont ) 
    {
        delete m_pCFont;
    }
    m_pCFont = new CFont;

    const CLogFont lf( fontHeight, 
                       FW_NORMAL,
                       pitchAndFamily,
                       faceName
                     );

    m_pCFont->CreateFontIndirect( &lf );

    SetFont( m_pCFont );
}

//** COLORREF CEditEx::bkColor( COLORREF crColor )
//
COLORREF CEditEx::bkColor( COLORREF crColor )
{
    _ASSERT(::IsWindow(m_hWnd)); 

    COLORREF crPrevBkColor = m_crBkColor;

    m_crBkColor = crColor;
    
    m_brBkGround.DeleteObject();

    CreateBrushType();

    Invalidate();

    return crPrevBkColor;
}

//** COLORREF CEditEx::textColor( COLORREF crColor )
//
COLORREF CEditEx::textColor( COLORREF crColor )
{
    _ASSERT(::IsWindow(m_hWnd)); 
    
    COLORREF crPrevTextColor = m_crTextColor;
    
    m_crTextColor = crColor;
    
    Invalidate();
    
    return crPrevTextColor;
}

/////////////////////////////////////////////////////////////////////////////
// Protected and private interfaces:

//
//** BOOL CEditEx::CreateBrushType()
//
BOOL CEditEx::CreateBrushType()
{
    return m_brBkGround.CreateSolidBrush( m_crBkColor );
}

//** void CEditEx::OnContextMenu(CWnd* pWnd, CPoint point) 
//
void CEditEx::OnContextMenu(CWnd* pWnd, CPoint point) 
{
    // Use default popup
    //
    if ( !m_MenuResourceID ) {
         CEdit::OnContextMenu( pWnd, point );
         return;
    }
    
    // Replace default popup menu
    //
    CMenu menu;
    
    if ( !menu.LoadMenu( m_MenuResourceID ) ) {
         CEdit::OnContextMenu( pWnd, point);
    }
    else {
         CMenu* pContext = menu.GetSubMenu(0);
         pContext->TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, point.x, point.y, this );
    }
}

//** void CEditEx::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
//
void CEditEx::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    if ( 0 == m_pEditMask ) {
         CEdit::OnChar(nChar, nRepCnt, nFlags);
         return;
    }

    // else Use custom mask...
    //
    if ( m_pEditMask->AddChar( nChar ) )
         CEdit::OnChar(nChar, nRepCnt, nFlags);
    else
         MessageBeep(0);
}

//** HBRUSH CEditEx::CtlColor(CDC* pDC, UINT nCtlColor) 
//
HBRUSH CEditEx::CtlColor(CDC* pDC, UINT nCtlColor) 
{
    pDC->SetTextColor( m_crTextColor );
    pDC->SetBkColor( m_crBkColor );
    return (HBRUSH)m_brBkGround;
}

void CEditEx::OnSetFocus(CWnd* pOldWnd) 
{
	ShowWindow(SW_SHOW);
	CEdit::OnSetFocus(pOldWnd);	
}

void CEditEx::OnCaptureChanged(CWnd *pWnd) 
{
	// TODO: Add your message handler code here
	
	CEdit::OnCaptureChanged(pWnd);
	
	CWnd *wnd;
	
	CPoint pt; 
	
	GetCursorPos(&pt);
	
	wnd = WindowFromPoint(pt);

	
	if(wnd != this)
	{
		SendMessage(WM_KILLFOCUS);
		return;
	}

	if(!pWnd) 
		SetCapture();
	
}

void CEditEx::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);
	ShowWindow(SW_HIDE);
	CWnd *pWnd = GetParent();
	pWnd->SendMessage(WM_ENDTEXTEDIT, (WPARAM)this, 0);
}

UINT CEditEx::OnGetDlgCode() 
{
	// TODO: Add your message handler code here and/or call default
	
	return DLGC_WANTALLKEYS;
}
