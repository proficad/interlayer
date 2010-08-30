// UndoListBox.cpp : implementation file
//

#include "stdafx.h"
#include "UndoListBox.h"
#include "UndoBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUndoListBox

CUndoListBox::CUndoListBox(CUndoBar& bar) :
	m_Bar (bar)
{
}

CUndoListBox::~CUndoListBox()
{
}


BEGIN_MESSAGE_MAP(CUndoListBox, CListBox)
	//{{AFX_MSG_MAP(CUndoListBox)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUndoListBox message handlers

void CUndoListBox::OnLButtonUp(UINT nFlags, CPoint point) 
{
	BOOL bOutside;
	ItemFromPoint (point, bOutside);

	if (!bOutside)
	{
		m_Bar.DoUndo ();
	}

	if (IsWindow (GetSafeHwnd ()))
	{
		CListBox::OnLButtonUp(nFlags, point);
	}
}

void CUndoListBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	CListBox::OnMouseMove(nFlags, point);

	BOOL bOutside;
	int iItem = ItemFromPoint (point, bOutside);

	if (!bOutside)
	{
		SelItemRange (FALSE, iItem + 1, GetCount () - 1);

		if (iItem == 0)
		{
			SetSel (0);
		}
		else
		{
			SelItemRange (TRUE, 0, iItem);
		}


		CString strLabel;

		if( m_Bar.m_bUnDo )
			strLabel.Format (_T("撤消 %d 步操作"), iItem + 1);
		else
			strLabel.Format (_T("恢复 %d 步操作"), iItem + 1);

		m_Bar.SetLabel (strLabel);
	}
}
