// ColourPicker.cpp : implementation file
//

#include "stdafx.h"
#include "ColourPicker.h"
#include "ColourPickerPopup.h"
#include "../Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void AFXAPI DDX_ColourPicker(CDataExchange *pDX, int nIDC, COLORREF& crColour)
{
    HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
    ASSERT (hWndCtrl != NULL);                
    
    CColourPicker* pColourPicker = (CColourPicker*) CWnd::FromHandle(hWndCtrl);
    if (pDX->m_bSaveAndValidate)
    {
        crColour = pColourPicker->GetColour();
    }
    else // initializing
    {
        pColourPicker->SetColour(crColour);
    }
}

/////////////////////////////////////////////////////////////////////////////
// CColourPicker

CColourPicker::CColourPicker()
{
    //SetBkColour(GetSysColor(COLOR_3DFACE));
    //SetTextColour(GetSysColor(COLOR_BTNTEXT));

	SetBkColour(RGB(255, 255, 255));
    SetTextColour(RGB(0, 0, 0));

    m_bTrackSelection = FALSE;
    m_nSelectionMode = CP_MODE_BK;
    m_bActive = FALSE;

    m_strDefaultText = _T("自动");
    m_strCustomText  = _T("其它颜色(&M)...");
	m_strPatternText = _T("填充效果(&F)...");	
	m_PatternStyle.m_PatternFill.m_nCol = -1;
	m_PatternStyle.m_PatternFill.m_nRow = -1;
}	

CColourPicker::CColourPicker(LPCTSTR szDefaultText/* = NULL*/, 
				 LPCTSTR szCustomText/* = NULL*/,
				 LPCTSTR szPatternText/* = NULL*/)				 
{
    //SetBkColour(GetSysColor(COLOR_3DFACE));
    //SetTextColour(GetSysColor(COLOR_BTNTEXT));

	SetBkColour(RGB(255, 255, 255));
    SetTextColour(RGB(0, 0, 0));

    m_bTrackSelection = FALSE;
    m_nSelectionMode = CP_MODE_BK;
    m_bActive = FALSE;

    m_strDefaultText = szDefaultText;
    m_strCustomText  = szCustomText;
	m_strPatternText = szCustomText;	
	m_PatternStyle.m_PatternFill.m_nCol = -1;
	m_PatternStyle.m_PatternFill.m_nRow = -1;
}


CColourPicker::~CColourPicker()
{	
}

IMPLEMENT_DYNCREATE(CColourPicker, CButton)

BEGIN_MESSAGE_MAP(CColourPicker, CButton)
    //{{AFX_MSG_MAP(CColourPicker)
    ON_CONTROL_REFLECT_EX(BN_CLICKED, OnClicked)
    ON_WM_CREATE()
    //}}AFX_MSG_MAP
    ON_MESSAGE(CPN_SELENDOK,     OnSelEndOK)
    ON_MESSAGE(CPN_SELENDCANCEL, OnSelEndCancel)
    ON_MESSAGE(CPN_SELCHANGE,    OnSelChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColourPicker message handlers

LONG CColourPicker::OnSelEndOK(WPARAM wParam, LPARAM lParam)
{
    COLORREF crNewColour = (COLORREF) wParam;
	m_bActive = FALSE;
	if(!lParam)
	{		
		SetColour(crNewColour);
		m_PatternStyle.m_PatternFill.m_crColourFore = (COLORREF) wParam;
		m_PatternStyle.m_PatternFill.m_nCol = -1;
		m_PatternStyle.m_PatternFill.m_nRow = -1;
	}
	else
	{
		PATTERNSTYLE* lp = (PATTERNSTYLE*)(PATTERNSTYLE*)lParam;
		m_PatternStyle.m_nFill = lp->m_nFill;
		m_PatternStyle.m_GradientFill = lp->m_GradientFill;
		m_PatternStyle.m_PatternFill = lp->m_PatternFill;
		m_PatternStyle.m_PictureFill = lp->m_PictureFill;
		if( m_PatternStyle.m_PatternFill.m_nCol != -1 && m_PatternStyle.m_PatternFill.m_nRow != -1)
			RedrawWindow();
		else
		{
			crNewColour = lp->m_PatternFill.m_crColourFore;
			SetColour(crNewColour);
		}	
	}

    CWnd *pParent = GetParent();
    if (pParent) {
        pParent->SendMessage(CPN_CLOSEUP, (WPARAM)&m_PatternStyle, (LPARAM) GetDlgCtrlID());
        pParent->SendMessage(CPN_SELENDOK, (WPARAM)&m_PatternStyle, (LPARAM) GetDlgCtrlID());
    }

    if (pParent) 
		pParent->SendMessage(CPN_SELCHANGE, (WPARAM)&m_PatternStyle, (LPARAM) GetDlgCtrlID());

    return TRUE;
}

LONG CColourPicker::OnSelEndCancel(UINT lParam, LONG /*wParam*/)
{
    m_bActive = FALSE;
    SetColour((COLORREF) lParam);

    CWnd *pParent = GetParent();
    if (pParent) {
        pParent->SendMessage(CPN_CLOSEUP, lParam, (WPARAM) GetDlgCtrlID());
        pParent->SendMessage(CPN_SELENDCANCEL, lParam, (WPARAM) GetDlgCtrlID());
    }

    return TRUE;
}

LONG CColourPicker::OnSelChange(UINT lParam, LONG /*wParam*/)
{
    if (m_bTrackSelection) SetColour((COLORREF) lParam);

    CWnd *pParent = GetParent();
    if (pParent) pParent->SendMessage(CPN_SELCHANGE, lParam, (WPARAM) GetDlgCtrlID());

    return TRUE;
}

int CColourPicker::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CButton::OnCreate(lpCreateStruct) == -1)
        return -1;
    
    SetWindowSize();    // resize appropriately
    return 0;
}

// On mouse click, create and show a CColourPopup window for colour selection
BOOL CColourPicker::OnClicked()
{
    m_bActive = TRUE;
    CRect rect;
    GetWindowRect(rect);
	
	new CColourPickerPopup(CPoint(rect.left, rect.bottom),    // Point to display popup
                     GetColour(),                       // Selected colour
                     this, m_strDefaultText,
					 m_strCustomText,
					 m_strPatternText					 
					 );

    CWnd *pParent = GetParent();
    if (pParent)
        pParent->SendMessage(CPN_DROPDOWN, (LPARAM)GetColour(), (WPARAM) GetDlgCtrlID());

    // Docs say I should return FALSE to stop the parent also getting the message.
    // HA! What a joke.

    return TRUE;
}

void CColourPicker::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
    ASSERT(lpDrawItemStruct);
    
    CDC*    pDC     = CDC::FromHandle(lpDrawItemStruct->hDC);
    CRect   rect    = lpDrawItemStruct->rcItem;
    UINT    state   = lpDrawItemStruct->itemState;
    DWORD   dwStyle = GetStyle();
    CString m_strText;

    CSize Margins(::GetSystemMetrics(SM_CXEDGE), ::GetSystemMetrics(SM_CYEDGE));

    // Draw arrow
    if (m_bActive) state |= ODS_SELECTED;
    pDC->DrawFrameControl(&m_ArrowRect, DFC_SCROLL, DFCS_SCROLLDOWN  | 
                          ((state & ODS_SELECTED) ? DFCS_PUSHED : 0) |
                          ((state & ODS_DISABLED) ? DFCS_INACTIVE : 0));

    pDC->DrawEdge(rect, EDGE_SUNKEN, BF_RECT);

    // Must reduce the size of the "client" area of the button due to edge thickness.
    rect.DeflateRect(Margins.cx, Margins.cy);

    // Fill remaining area with colour
    rect.right -= m_ArrowRect.Width();

    CBrush brush;
	if(m_PatternStyle.m_PatternFill.m_nCol != -1 && m_PatternStyle.m_PatternFill.m_nRow != -1)
	{
		if(!m_PatternStyle.CreateBrush(&brush, pDC))
			return;
	}
		
	else
		brush.CreateSolidBrush( ((state & ODS_DISABLED) || m_crColourBk == CLR_DEFAULT)? 
                  ::GetSysColor(COLOR_3DFACE) : m_crColourBk);
    CBrush* pOldBrush = (CBrush*) pDC->SelectObject(&brush);
	pDC->SelectStockObject(NULL_PEN);
    pDC->Rectangle(rect);
    pDC->SelectObject(pOldBrush);

    // Draw the window text (if any)
    GetWindowText(m_strText);
    if (m_strText.GetLength())
    {
        pDC->SetBkMode(TRANSPARENT);
        if (state & ODS_DISABLED)
        {
            rect.OffsetRect(1,1);
            pDC->SetTextColor(::GetSysColor(COLOR_3DHILIGHT));
            pDC->DrawText(m_strText, rect, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
            rect.OffsetRect(-1,-1);
            pDC->SetTextColor(::GetSysColor(COLOR_3DSHADOW));
            pDC->DrawText(m_strText, rect, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
        }
        else
        {
            pDC->SetTextColor((m_crColourText == CLR_DEFAULT)? 0 : m_crColourText);
            pDC->DrawText(m_strText, rect, DT_CENTER|DT_SINGLELINE|DT_VCENTER);
        }
    }

    // Draw focus rect
    if (state & ODS_FOCUS) 
    {
        rect.DeflateRect(1,1);
    //    pDC->DrawFocusRect(rect);
    }
}

/////////////////////////////////////////////////////////////////////////////
// CColourPicker overrides

void CColourPicker::PreSubclassWindow() 
{
    ModifyStyle(0, BS_OWNERDRAW);        // Make it owner drawn
    CButton::PreSubclassWindow();
    SetWindowSize();                     // resize appropriately
}

/////////////////////////////////////////////////////////////////////////////
// CColourPicker attributes

COLORREF CColourPicker::GetColour()
{ 
    return (m_nSelectionMode == CP_MODE_TEXT)? 
        GetTextColour(): GetBkColour(); 
}

void CColourPicker::SetColour(COLORREF crColour)
{ 
    (m_nSelectionMode == CP_MODE_TEXT)? 
        SetTextColour(crColour): SetBkColour(crColour); 
}

void CColourPicker::SetBkColour(COLORREF crColourBk)
{
    m_crColourBk = crColourBk;
    if (IsWindow(m_hWnd)) RedrawWindow();
}

void CColourPicker::SetTextColour(COLORREF crColourText)
{
    m_crColourText = crColourText;
    if (IsWindow(m_hWnd)) RedrawWindow();
}

void CColourPicker::SetDefaultText(LPCTSTR szDefaultText)
{
    m_strDefaultText = (szDefaultText)? szDefaultText : _T("");
}

void CColourPicker::SetCustomText(LPCTSTR szCustomText)
{
    m_strCustomText = (szCustomText)? szCustomText : _T("");
}

void CColourPicker::SetPatternText(LPCTSTR szPatternText)
{
    m_strPatternText = (szPatternText)? szPatternText : _T("");
}

/////////////////////////////////////////////////////////////////////////////
// CColourPicker implementation

void CColourPicker::SetWindowSize()
{
    // Get size dimensions of edges
    CSize MarginSize(::GetSystemMetrics(SM_CXEDGE), ::GetSystemMetrics(SM_CYEDGE));    
    // Get window size
    CRect rect;
    
    // Get the new coords of this window
    GetWindowRect(rect);
    ScreenToClient(rect);

    // Get the rect where the arrow goes, and convert to client coords.
    m_ArrowRect.SetRect(rect.right - rect.Height() + MarginSize.cx, 
                        rect.top + MarginSize.cy, rect.right - MarginSize.cx,
                        rect.bottom - MarginSize.cy);
}

void CColourPicker::SetPatternStyle(PATTERNSTYLE &PatternStyle)
{
	m_PatternStyle = PatternStyle;
}
