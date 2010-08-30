// DlgLineSet.cpp : implementation file
//
#pragma warning( disable : 4786 )

#include "stdafx.h"
#include "DlgLineSet.h"

#define CPN_SELENDOK         WM_USER + 1004        // Colour Picker end OK
#define APN_SELENDOK         WM_USER + 1014        // Arrow Picker end OK


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgLineSet dialog


CDlgLineSet::CDlgLineSet(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLineSet::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgLineSet)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgLineSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgLineSet)
	DDX_Control(pDX, IDC_BUTTON_PENTYPE, m_PenPicker);
	DDX_Control(pDX, IDC_BUTTON_BEGINARROW, m_BeginArrowPicker);
	DDX_Control(pDX, IDC_BUTTON_ENDARROW, m_EndArrowPicker);
	//}}AFX_DATA_MAP

	DDX_Check(pDX, IDC_CHECK_SQUAREPEN, eLine.m_bSquarePen);
	
	DDX_CBIndex(pDX, IDC_COMBO_PENTYPE, eLine.m_nPenStyle);
	DDX_Text(pDX, IDC_EDIT_PENWIDE, eLine.m_nPenWide);
	DDV_MinMaxUInt(pDX, eLine.m_nPenWide, 0, 300);
	
	DDX_Text(pDX, IDC_EDIT_BEGINSIZE, eLine.m_uEndPointSize);
	DDX_Text(pDX, IDC_EDIT_ENDSIZE, eLine.m_uBeginPointSize);
	DDV_MinMaxUInt(pDX, eLine.m_uBeginPointSize, 0, 300);
	DDV_MinMaxUInt(pDX, eLine.m_uEndPointSize, 0, 300);
	
	DDX_ColourPicker(pDX, IDC_BUTTON_PENTYPE, eLine.s_LineStyle.m_PatternFill.m_crColourFore);
	DDX_ArrowPicker(pDX, IDC_BUTTON_BEGINARROW, eLine.m_uEndPointMode);	
	DDX_ArrowPicker(pDX, IDC_BUTTON_ENDARROW, eLine.m_uBeginPointMode);
}


BEGIN_MESSAGE_MAP(CDlgLineSet, CDialog)
	//{{AFX_MSG_MAP(CDlgLineSet)
	ON_BN_CLICKED(IDOK, OnOk)
	//}}AFX_MSG_MAP
	ON_MESSAGE(CPN_SELENDOK,     OnSelEndOK)
	ON_MESSAGE(APN_SELENDOK,     OnSelEndOK)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgLineSet message handlers

BOOL CDlgLineSet::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	UpdateData(FALSE);

	line.SubclassDlgItem(IDC_COMBO_PENTYPE, this);

	m_PenPicker.SetPatternText("带图案线条(&P)...");
	
	if( eLine.m_nLineType )
	{		
		CWnd *pWnd = GetDlgItem(IDC_EDIT_BEGINSIZE);
		pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDC_EDIT_ENDSIZE);
		pWnd->EnableWindow(FALSE);
		
		m_BeginArrowPicker.ModifyStyle(0, WS_DISABLED);
		m_BeginArrowPicker.Invalidate();
		m_EndArrowPicker.ModifyStyle(0, WS_DISABLED);
		m_EndArrowPicker.Invalidate();
	}
	else
	{
		CWnd *pWnd;
		m_BeginArrowPicker.ModifyStyle(WS_DISABLED, 0);
		m_BeginArrowPicker.Invalidate();
		m_EndArrowPicker.ModifyStyle(WS_DISABLED, 0);
		m_EndArrowPicker.Invalidate();

		if( eLine.m_uEndPointMode )
		{
			pWnd = GetDlgItem(IDC_EDIT_BEGINSIZE);
			pWnd->EnableWindow(TRUE);
		}
		else
		{
			pWnd = GetDlgItem(IDC_EDIT_BEGINSIZE);
			pWnd->EnableWindow(FALSE);
		}
		if( eLine.m_uBeginPointMode )
		{
			pWnd = GetDlgItem(IDC_EDIT_ENDSIZE);
			pWnd->EnableWindow(TRUE);
		}
		else
		{
			pWnd = GetDlgItem(IDC_EDIT_ENDSIZE);
			pWnd->EnableWindow(FALSE);
		}
	}
		
	line.SetCurSel(eLine.m_nPenStyle);
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgLineSet::OnOk() 
{
	UpdateData();
	CDialog::OnOK();
}


LONG CDlgLineSet::OnSelEndOK(WPARAM wParam, LPARAM lParam)
{
	LPPATTERNSTYLE lp = (LPPATTERNSTYLE) wParam;
	
	if(lParam == IDC_BUTTON_PENTYPE)
	{
		eLine.s_LineStyle.m_GradientFill = lp->m_GradientFill;
		eLine.s_LineStyle.m_nFill = lp->m_nFill;
		eLine.s_LineStyle.m_PatternFill = lp->m_PatternFill;
		eLine.s_LineStyle.m_PictureFill = lp->m_PictureFill;
	}

	if(lParam == IDC_BUTTON_ENDARROW)
	{
		eLine.m_uBeginPointMode = (int) wParam;
		CWnd *pWnd;
		if( eLine.m_uBeginPointMode )
		{
			pWnd = GetDlgItem(IDC_EDIT_ENDSIZE);
			pWnd->EnableWindow(TRUE);
		}
		else
		{
			pWnd = GetDlgItem(IDC_EDIT_ENDSIZE);
			pWnd->EnableWindow(FALSE);
		}		
	}
	if( lParam == IDC_BUTTON_BEGINARROW)
	{
		eLine.m_uEndPointMode = (int) wParam;
		CWnd *pWnd;
		
		if( eLine.m_uEndPointMode )
		{
			pWnd = GetDlgItem(IDC_EDIT_BEGINSIZE);
			pWnd->EnableWindow(TRUE);
		}
		else
		{
			pWnd = GetDlgItem(IDC_EDIT_BEGINSIZE);
			pWnd->EnableWindow(FALSE);
		}
	}
	return TRUE;
}

