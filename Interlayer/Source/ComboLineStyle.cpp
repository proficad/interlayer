// ComboLineStyle.cpp : implementation file
//

#include "stdafx.h"
#include "ComboLineStyle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CComboLineStyleStyle

SLineStyleAndName CComboLineStyle::nLineStyle[] = 
{
	SLineStyleAndName(0,	"单线"),
	SLineStyleAndName(1,	"双线"),
	SLineStyleAndName(2,	"文武线1"),
	SLineStyleAndName(3,	"文武线2"),
	SLineStyleAndName(4,	"三线1"),
	SLineStyleAndName(5,	"三线2"),
	SLineStyleAndName(6,	"三线3")
};

CComboLineStyle::CComboLineStyle()
{
	m_bInitialized = false;
}

CComboLineStyle::~CComboLineStyle()
{
}


BEGIN_MESSAGE_MAP(CComboLineStyle, CComboBox)
	//{{AFX_MSG_MAP(CComboLineStyle)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComboLineStyle message handlers

void CComboLineStyle::InitializeData()
{
	int		iAddedItem = -1;

	if( m_bInitialized )
		return;

	for( int i = 0; i < 7; i++ )// For All Colors
	{
		iAddedItem = AddString( nLineStyle[i].cLineName );
		if( iAddedItem == CB_ERRSPACE )						// If Not Added
			break;											// Stop
		else												// If Added Successfully
			SetItemData( iAddedItem, (DWORD) nLineStyle[i].nLineStyle );					// Set Color RGB Value
	}
	m_bInitialized = true;	
}

void CComboLineStyle::PreSubclassWindow() 
{
	InitializeData();											// Initialize Contents
	CComboBox::PreSubclassWindow();
	SetCurSel( 0 );	
}

int CComboLineStyle::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	InitializeData();											// Initialize Contents
	SetCurSel( 0 );	

	return 0;
}

void CComboLineStyle::DrawItem(LPDRAWITEMSTRUCT pDIStruct)
{
	static		CString	sPenName;								// No Need To Reallocate Each Time

	CDC			dcContext;
	CRect		rItemRect( pDIStruct -> rcItem );
	CRect		rBlockRect( rItemRect );
	CRect		rTextRect( rBlockRect );
	CPen		pen1, pen2;
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

		LOGBRUSH logBrush;
		if( iState & ODS_SELECTED )
		{		
			logBrush.lbStyle = BS_SOLID;
			logBrush.lbColor = 0x00FFFFFF & ~( crColor);
			logBrush.lbHatch = NULL;
		}
		else
		{
			logBrush.lbStyle = BS_SOLID;
			logBrush.lbColor = crColor;
			logBrush.lbHatch = NULL;
		}

		pen1.CreatePen( PS_GEOMETRIC| PS_SOLID|PS_ENDCAP_FLAT|PS_JOIN_MITER, 1, &logBrush);
		pen2.CreatePen( PS_GEOMETRIC| PS_SOLID|PS_ENDCAP_FLAT|PS_JOIN_MITER, 3, &logBrush);
		
		CPoint pt ;
		//CPoint pt1, pt2, pt3;

		switch(nStyle)
		{
		case 0://单线
			pOldPen = dcContext.SelectObject(&pen1);			
			pt = rBlockRect.CenterPoint();			
			dcContext.MoveTo(rBlockRect.left, pt.y);
			dcContext.LineTo(rBlockRect.right, pt.y);			
			break;
		case 1:
			pOldPen = dcContext.SelectObject(&pen1);			
			pt = rBlockRect.CenterPoint();
			pt.Offset(0,2);
			dcContext.MoveTo(rBlockRect.left, pt.y);
			dcContext.LineTo(rBlockRect.right, pt.y);			
			pt.Offset(0,-4);
			dcContext.MoveTo(rBlockRect.left, pt.y);
			dcContext.LineTo(rBlockRect.right, pt.y);			
			break;
		case 2:
			pOldPen = dcContext.SelectObject(&pen1);			
			pt = rBlockRect.CenterPoint();
			pt.Offset(0,2);
			dcContext.MoveTo(rBlockRect.left, pt.y);
			dcContext.LineTo(rBlockRect.right, pt.y);
			dcContext.SelectObject(&pen2);			
			pt.Offset(0,-4);
			dcContext.MoveTo(rBlockRect.left, pt.y);
			dcContext.LineTo(rBlockRect.right, pt.y);			
			break;
		case 3:
			pOldPen = dcContext.SelectObject(&pen2);			
			pt = rBlockRect.CenterPoint();
			pt.Offset(0,2);
			dcContext.MoveTo(rBlockRect.left, pt.y);
			dcContext.LineTo(rBlockRect.right, pt.y);
			dcContext.SelectObject(&pen1);			
			pt.Offset(0,-4);
			dcContext.MoveTo(rBlockRect.left, pt.y);
			dcContext.LineTo(rBlockRect.right, pt.y);			
			break;
		case 4:
			pOldPen = dcContext.SelectObject(&pen1);			
			pt = rBlockRect.CenterPoint();
			dcContext.MoveTo(rBlockRect.left, pt.y);
			dcContext.LineTo(rBlockRect.right, pt.y);
			pt.Offset(0,3);
			dcContext.MoveTo(rBlockRect.left, pt.y);
			dcContext.LineTo(rBlockRect.right, pt.y);			
			pt.Offset(0,-6);
			dcContext.MoveTo(rBlockRect.left, pt.y);
			dcContext.LineTo(rBlockRect.right, pt.y);
			break;
		case 5:
			pOldPen = dcContext.SelectObject(&pen1);			
			pt = rBlockRect.CenterPoint();
			dcContext.MoveTo(rBlockRect.left, pt.y);
			dcContext.LineTo(rBlockRect.right, pt.y);
			dcContext.SelectObject(&pen2);
			pt.Offset(0,3);
			dcContext.MoveTo(rBlockRect.left, pt.y);
			dcContext.LineTo(rBlockRect.right, pt.y);			
			pt.Offset(0,-6);
			dcContext.MoveTo(rBlockRect.left, pt.y);
			dcContext.LineTo(rBlockRect.right, pt.y);
			break;
		case 6:
			pOldPen = dcContext.SelectObject(&pen2);			
			pt = rBlockRect.CenterPoint();
			dcContext.MoveTo(rBlockRect.left, pt.y);
			dcContext.LineTo(rBlockRect.right, pt.y);
			dcContext.SelectObject(&pen1);
			pt.Offset(0,3);
			dcContext.MoveTo(rBlockRect.left, pt.y);
			dcContext.LineTo(rBlockRect.right, pt.y);			
			pt.Offset(0,-6);
			dcContext.MoveTo(rBlockRect.left, pt.y);
			dcContext.LineTo(rBlockRect.right, pt.y);			
			break;
		}
		dcContext.SelectObject(pOldPen);
		pen1.DeleteObject();		
		pen2.DeleteObject();		
	}
	dcContext.Detach();	
}
