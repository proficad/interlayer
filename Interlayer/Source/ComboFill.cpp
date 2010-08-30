// ComboFill.cpp : implementation file
//

#include "stdafx.h"
#include "ComboFill.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CComboFill

//#define HS_HORIZONTAL       0       /* ----- */
//#define HS_VERTICAL         1       /* ||||| */
//#define HS_FDIAGONAL        2       /* \\\\\ */
//#define HS_BDIAGONAL        3       /* ///// */
//#define HS_CROSS            4       /* +++++ */
//#define HS_DIAGCROSS        5       /* xxxxx */

SFillAndName CComboFill::nFillStyle[] = 
{
	SFillAndName(999,					"实心"),	
	SFillAndName(HS_CROSS   ,			"十字网格"),
	SFillAndName(HS_DIAGCROSS,			"交叉网格"),
	SFillAndName(HS_BDIAGONAL,			"左斜线"),	
	SFillAndName(HS_FDIAGONAL,			"右斜线"),
	SFillAndName(HS_HORIZONTAL,			"水平线"),
	SFillAndName(HS_VERTICAL   ,		"垂直线")
};

CComboFill::CComboFill()
{
	m_bInitialized = false;
}

CComboFill::~CComboFill()
{
}


BEGIN_MESSAGE_MAP(CComboFill, CComboBox)
	//{{AFX_MSG_MAP(CComboFill)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComboFill message handlers

void CComboFill::InitializeData()
{
	int		iAddedItem = -1;

	if( m_bInitialized )
		return;

	for( int i = 0; i < 7; i++ )// For All Colors
	{
		iAddedItem = AddString( nFillStyle[i].cFillName );
		if( iAddedItem == CB_ERRSPACE )						// If Not Added
			break;											// Stop
		else												// If Added Successfully
			SetItemData( iAddedItem, (DWORD) nFillStyle[i].nFillStyle );					// Set Color RGB Value
	}
	m_bInitialized = true;	
}

void CComboFill::PreSubclassWindow() 
{
	InitializeData();											// Initialize Contents
	CComboBox::PreSubclassWindow();
	SetCurSel( 0 );	
}

int CComboFill::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	InitializeData();											// Initialize Contents
	SetCurSel( 0 );	

	return 0;
}

void CComboFill::DrawItem(LPDRAWITEMSTRUCT pDIStruct)
{
	static		CString	sPenName;								// No Need To Reallocate Each Time

	CDC			dcContext;
	CRect		rItemRect( pDIStruct -> rcItem );
	CRect		rBlockRect( rItemRect );
	CRect		rTextRect( rBlockRect );
	CBrush		brFrameBrush;
	int			iFourthWidth = 0;
	int			iItem = pDIStruct -> itemID;
	int			iAction = pDIStruct -> itemAction;
	int			iState = pDIStruct -> itemState;
	COLORREF	crColor = 0;
	COLORREF	crNormal = GetSysColor( COLOR_WINDOW );
	COLORREF	crSelected = GetSysColor( COLOR_HIGHLIGHT );
	COLORREF	crText = GetSysColor( COLOR_WINDOWTEXT );

	if( !dcContext.Attach( pDIStruct -> hDC ) )				// Attach CDC Object
		return;												// Stop If Attach Failed

	iFourthWidth = ( rBlockRect.Width() / 2 );				// Get 1/4 Of Item Area
	
	if( iState & ODS_SELECTED )								// If Selected
	{														// Set Selected Attributes
		dcContext.SetTextColor(
				( 0x00FFFFFF & ~( crText ) ) );				// Set Inverted Text Color (With Mask)
		dcContext.SetBkColor( crSelected );					// Set BG To Highlight Color
		dcContext.FillSolidRect( &rBlockRect, crSelected );	// Erase Item
	}
	else													// If Not Selected
	{														// Set Standard Attributes
		dcContext.SetTextColor( crText );					// Set Text Color
		dcContext.SetBkColor( crNormal );					// Set BG Color
		dcContext.FillSolidRect( &rBlockRect, crNormal );	// Erase Item
	}
	if( iState & ODS_FOCUS )								// If Item Has The Focus
		dcContext.DrawFocusRect( &rItemRect );				// Draw Focus Rect

	//
	//	Calculate Text Area
	//
	rTextRect.left += ( iFourthWidth + 2 );					// Set Start Of Text
	rTextRect.top += 2;										// Offset A Bit

	//
	//	Calculate Color Block Area
	//
	rBlockRect.DeflateRect( CSize( 2, 2 ) );				// Reduce Color Block Size
	rBlockRect.right = iFourthWidth;						// Set Width Of Color Block

	//
	//	Draw Color Text And Block
	//
	if( iItem != -1 )										// If Not An Empty Item
	{
		GetLBText( iItem, sPenName );							// Get Color Text
		if( iState & ODS_DISABLED )							// If Disabled
		{
			crColor = GetSysColor( COLOR_INACTIVECAPTIONTEXT );
			dcContext.SetTextColor( crColor );				// Set Text Color
		}
		else												// If Normal
			crColor = RGB(0, 0, 0);
	
		int nStyle = (int)GetItemData( iItem );					// Get Color Value

		dcContext.SetBkMode( TRANSPARENT );					// Transparent Background
		dcContext.TextOut( rTextRect.left, rTextRect.top,
				sPenName );									// Draw Color Name

		CPen pen;
		CPen *pOldPen;
		
		if( iState & ODS_SELECTED )
		{
			pen.CreatePen( PS_SOLID, 0, 0x00FFFFFF & ~( crColor));
			
			pOldPen = dcContext.SelectObject(&pen);
			

			if(nStyle == 999)
				brFrameBrush.CreateSolidBrush(0x00FFFFFF & ~( crColor) );
			else
				brFrameBrush.CreateHatchBrush(nStyle, 0x00FFFFFF & ~( crColor) );
		}
		else
		{
			pen.CreatePen( PS_SOLID, 0, crColor);
			
			pOldPen = dcContext.SelectObject(&pen);
			
			
			if(nStyle == 999)
				brFrameBrush.CreateSolidBrush( crColor );
			else
				brFrameBrush.CreateHatchBrush(nStyle,  crColor );
		}
	
		CBrush *pOldBrush = dcContext.SelectObject(&brFrameBrush);

		dcContext.Rectangle( rBlockRect );	// Draw Color

		dcContext.SelectObject(pOldBrush);
		brFrameBrush.DeleteObject();

		dcContext.SelectObject(pOldPen);
		pen.DeleteObject();

				
		//dcContext.FrameRect( &rBlockRect, &brFrameBrush );	// Draw Frame
	}
	dcContext.Detach();	
}
