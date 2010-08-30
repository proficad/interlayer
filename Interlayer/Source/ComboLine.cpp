// ComboLine.cpp : implementation file
//

#include "stdafx.h"
#include "ComboLine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CComboLine
/*
#define PS_SOLID            0
#define PS_DASH             1  
#define PS_DOT              2  
#define PS_DASHDOT          3  
#define PS_DASHDOTDOT       4  
#define PS_NULL             5
#define PS_INSIDEFRAME      6
*/

SPenAndName CComboLine::nPenStyle[] = 
{
	SPenAndName(PS_SOLID,			"实线"),
	SPenAndName(PS_DASH,			"虚线"),
	SPenAndName(PS_DOT,				"点线"),
	SPenAndName(PS_DASHDOT,			"点虚线"),
	SPenAndName(PS_DASHDOTDOT,		"双点虚线"),
	SPenAndName(PS_INSIDEFRAME,		"内画线"),
	SPenAndName(PS_NULL,			"无")
};

CComboLine::CComboLine()
{
	m_bInitialized = false;
}

CComboLine::~CComboLine()
{
}


BEGIN_MESSAGE_MAP(CComboLine, CComboBox)
	//{{AFX_MSG_MAP(CComboLine)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComboLine message handlers

void CComboLine::InitializeData()
{
	int		iAddedItem = -1;

	if( m_bInitialized )
		return;

	for( int i = 0; i < 7; i++ )// For All Colors
	{
		iAddedItem = AddString( nPenStyle[i].cPenName );
		if( iAddedItem == CB_ERRSPACE )						// If Not Added
			break;											// Stop
		else												// If Added Successfully
			SetItemData( iAddedItem, (DWORD) nPenStyle[i].nPenStyle );					// Set Color RGB Value
	}
	m_bInitialized = true;	
}

void CComboLine::PreSubclassWindow() 
{
	InitializeData();											// Initialize Contents
	CComboBox::PreSubclassWindow();
	SetCurSel( 0 );	
}

int CComboLine::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	InitializeData();											// Initialize Contents
	SetCurSel( 0 );	

	return 0;
}

void CComboLine::DrawItem(LPDRAWITEMSTRUCT pDIStruct)
{
	static		CString	sPenName;								// No Need To Reallocate Each Time

	CDC			dcContext;
	CRect		rItemRect( pDIStruct -> rcItem );
	CRect		rBlockRect( rItemRect );
	CRect		rTextRect( rBlockRect );
	CPen		pen;
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
		
		CPen *pOldPen;

		if( iState & ODS_SELECTED )
		{
			pen.CreatePen( nStyle, 0, 0x00FFFFFF & ~( crColor));
			
			pOldPen = dcContext.SelectObject(&pen);
			
		}
		else
		{
			
			pen.CreatePen( nStyle, 0, crColor);
			
			pOldPen = dcContext.SelectObject(&pen);
		}

		CPoint pt = rBlockRect.CenterPoint();

		dcContext.MoveTo(rBlockRect.left, pt.y);
		dcContext.LineTo(rBlockRect.right, pt.y);
		dcContext.SelectObject(pOldPen);
		pen.DeleteObject();

		//dcContext.FillSolidRect( &rBlockRect, crColor );	// Draw Color
				
		//dcContext.FrameRect( &rBlockRect, &brFrameBrush );	// Draw Frame
	}
	dcContext.Detach();	
}
