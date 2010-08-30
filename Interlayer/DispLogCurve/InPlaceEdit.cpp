// InPlaceEdit.cpp : implementation file
//

#include "stdafx.h"
#include "InPlaceEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace dispLogCurve
{

/////////////////////////////////////////////////////////////////////////////
// CInPlaceEdit

CInPlaceEdit::CInPlaceEdit(CWnd* pParent, CRect& rect, DWORD dwStyle, UINT nID,
                           int nIndex, CString sInitText, 
                           UINT nFirstChar)
{
	m_sInitText     = sInitText;
    m_nIndex          = nIndex;
    m_nLastChar     = 0; 
    m_bExitOnArrows = (nFirstChar != VK_LBUTTON);    // If mouse click brought us here,
	// then no exit on arrows
	
    m_Rect = rect;  // For bizarre CE bug.
    
    DWORD dwEditStyle = WS_BORDER|WS_CHILD|WS_VISIBLE| ES_AUTOHSCROLL|WS_CLIPSIBLINGS //|ES_MULTILINE
        | dwStyle;
    if (!Create(dwEditStyle, rect, pParent, nID)) return;
    
    SetFont(pParent->GetFont());
    
    SetWindowText(sInitText);
    SetFocus();
    
    switch (nFirstChar){
	case VK_LBUTTON: 
	case VK_RETURN:   SetSel((int)_tcslen(m_sInitText), -1); return;
	case VK_BACK:     SetSel((int)_tcslen(m_sInitText), -1); break;
	case VK_TAB:
	case VK_DOWN: 
	case VK_UP:   
	case VK_RIGHT:
	case VK_LEFT:  
	case VK_NEXT:  
	case VK_PRIOR: 
	case VK_HOME:
	case VK_SPACE:
	case VK_END:      SetSel(0,-1); return;
	default:          SetSel(0,-1);
    }
	
    // Added by KiteFly. When entering DBCS chars into cells the first char was being lost
    // SenMessage changed to PostMessage (John Lagerquist)
    if( nFirstChar < 0x80)
        PostMessage(WM_CHAR, nFirstChar);   
    else
        PostMessage(WM_IME_CHAR, nFirstChar);
}

CInPlaceEdit::~CInPlaceEdit()
{
}


BEGIN_MESSAGE_MAP(CInPlaceEdit, CEdit)
	//{{AFX_MSG_MAP(CInPlaceEdit)
	ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceEdit message handlers




UINT CInPlaceEdit::OnGetDlgCode() 
{
	// TODO: Add your message handler code here and/or call default
	
	return DLGC_WANTALLKEYS;
}

void CInPlaceEdit::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CEdit::PostNcDestroy();
	delete this;
}

BOOL CInPlaceEdit::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_SYSCHAR)
        return TRUE;
    
    return CWnd::PreTranslateMessage(pMsg);
}

void CInPlaceEdit::EndEdit()
{
    // Close this window (PostNcDestroy will delete this)
    if (IsWindow(GetSafeHwnd()))
        SendMessage(WM_CLOSE, 0, 0);
}
}