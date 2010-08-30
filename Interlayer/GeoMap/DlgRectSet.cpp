// DlgRectSet.cpp : implementation file
//

#include "stdafx.h"
#include "DlgRectSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CPN_SELENDOK WM_USER + 1004 

/////////////////////////////////////////////////////////////////////////////
// CDlgRectSet dialog


CDlgRectSet::CDlgRectSet(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRectSet::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgRectSet)
	m_nTrans = 0;
	m_iSpaceWidth = 10;
	//}}AFX_DATA_INIT
}


void CDlgRectSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRectSet)
	DDX_Control(pDX, IDC_SLIDER_RECT, m_Trans);
	DDX_Control(pDX, IDC_SPIN_PENWIDE, m_spin);
	DDX_Slider(pDX, IDC_SLIDER_RECT, m_nTrans);
	DDX_Control(pDX, IDC_BUTTON_LINECOLOR, m_LineColourPicker);
	DDX_Control(pDX, IDC_BUTTON_LINECOLOR_2, m_LineColourPicker2);
	DDX_Control(pDX, IDC_BUTTON_FILLTYPE, m_FillColourPicker);
	DDX_Control(pDX, IDC_BUTTON_FILLLINE, m_ColourPickerLineFill);
	//}}AFX_DATA_MAP

	DDX_Text(pDX, IDC_EDIT_SPACE_WIDTH, m_iSpaceWidth);
	DDV_MinMaxUInt(pDX, m_iSpaceWidth, 0, 500);

	DDX_Text(pDX, IDC_EDIT_PENWIDE, eLine.m_nPenWide);
	DDV_MinMaxUInt(pDX, eLine.m_nPenWide, 0, 300);
	DDX_CBIndex(pDX, IDC_COMBO_PENTYPE, eLine.m_nPenStyle);
	DDX_CBIndex(pDX, IDC_COMBO_LINETYPE, eLine.m_nLineType);
	DDX_Check(pDX, IDC_CHECK_FILL, eFill.m_bShow);
	DDX_Check(pDX, IDC_CHECK_LINEFILL, eLine.eLineSpaceFill.m_bShow);
	DDX_Check(pDX, IDC_CHECK_SQUAREPEN, eLine.m_bSquarePen);
	
	DDX_ColourPicker(pDX, IDC_BUTTON_FILLTYPE,eFill.sFillStyle.m_PatternFill.m_crColourFore);
	DDX_ColourPicker(pDX, IDC_BUTTON_LINECOLOR,eLine.s_LineStyle.m_PatternFill.m_crColourFore);	
	DDX_ColourPicker(pDX, IDC_BUTTON_FILLLINE, eLine.eLineSpaceFill.sFillStyle.m_PatternFill.m_crColourFore);

	DDX_Text(pDX, IDC_EDIT_PENWIDE2, eLine2.m_nPenWide);
	DDV_MinMaxUInt(pDX, eLine2.m_nPenWide, 0, 300);
	DDX_CBIndex(pDX, IDC_COMBO_PENTYPE_2, eLine2.m_nPenStyle);
	DDX_ColourPicker(pDX, IDC_BUTTON_LINECOLOR_2,eLine2.s_LineStyle.m_PatternFill.m_crColourFore);	
}

BEGIN_MESSAGE_MAP(CDlgRectSet, CDialog)
	//{{AFX_MSG_MAP(CDlgRectSet)
	ON_BN_CLICKED(IDC_CHECK_FILL, OnCheckFill)
	ON_BN_CLICKED(IDOK, OnOk)
	ON_BN_CLICKED(IDC_CHECK_LINEFILL, OnCheckLinefill)
	ON_CBN_SELENDOK(IDC_COMBO_LINETYPE, OnSelendokComboLinetype)
	//}}AFX_MSG_MAP
	ON_MESSAGE(CPN_SELENDOK,     OnSelEndOK)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgRectSet message handlers


BOOL CDlgRectSet::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	UpdateData(FALSE);

	m_LineColourPicker.SetPatternStyle(eLine.s_LineStyle);
	m_LineColourPicker2.SetPatternStyle(eLine2.s_LineStyle);
	m_FillColourPicker.SetPatternStyle(eFill.sFillStyle);

	m_nTrans = 255 - eFill.blendFunction.SourceConstantAlpha;
	
	penStyle.SubclassDlgItem(IDC_COMBO_PENTYPE, this);
	penStyle2.SubclassDlgItem(IDC_COMBO_PENTYPE_2, this);
	lineStyle.SubclassDlgItem(IDC_COMBO_LINETYPE, this);
	
//	m_spin.SetRange(1, 300);
//	m_spin.SetBuddy(GetDlgItem(IDC_EDIT_PENWIDE));
	
	penStyle.SetCurSel(eLine.m_nPenStyle);
	penStyle2.SetCurSel(eLine2.m_nPenStyle);
	lineStyle.SetCurSel(eLine.m_nLineType);

	m_Trans.SetRange(0, 255);
	m_Trans.SetPos(m_nTrans);
	m_Trans.SetLineSize(1);	

	m_LineColourPicker.SetPatternText("带图案线条(&P)...");
	m_LineColourPicker2.SetPatternText("带图案线条(&P)...");

	if( eLine.m_nLineType )
	{
		CWnd *pWnd = GetDlgItem(IDC_CHECK_LINEFILL);
		pWnd->EnableWindow(TRUE);

		GetDlgItem(IDC_EDIT_SPACE_WIDTH)->EnableWindow(TRUE);
		
		if( eLine.m_nLineType == 2
			|| eLine.m_nLineType == 3
			|| eLine.m_nLineType == 5
			|| eLine.m_nLineType == 6)
		{
			GetDlgItem(IDC_EDIT_PENWIDE2)->EnableWindow(TRUE);
			
			GetDlgItem(IDC_COMBO_PENTYPE_2)->EnableWindow(TRUE);
			
			GetDlgItem(IDC_BUTTON_LINECOLOR_2)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_EDIT_PENWIDE2)->EnableWindow(FALSE);
			
			GetDlgItem(IDC_COMBO_PENTYPE_2)->EnableWindow(FALSE);
			
			GetDlgItem(IDC_BUTTON_LINECOLOR_2)->EnableWindow(FALSE);
		}

		if( eLine.eLineSpaceFill.m_bShow )
		{
			m_ColourPickerLineFill.ModifyStyle(WS_DISABLED, 0);
			m_ColourPickerLineFill.Invalidate();		
		}
		else
		{
			m_ColourPickerLineFill.ModifyStyle(0, WS_DISABLED);
			m_ColourPickerLineFill.Invalidate();
		}
	}
	else
	{
		CWnd *pWnd = GetDlgItem(IDC_CHECK_LINEFILL);
		pWnd->EnableWindow(FALSE);

		if(eLine.m_nLineType == 0 )
			GetDlgItem(IDC_EDIT_SPACE_WIDTH)->EnableWindow(FALSE);
		else
			GetDlgItem(IDC_EDIT_SPACE_WIDTH)->EnableWindow(TRUE);

		GetDlgItem(IDC_EDIT_PENWIDE2)->EnableWindow(FALSE);
		
		GetDlgItem(IDC_COMBO_PENTYPE_2)->EnableWindow(FALSE);
		
		GetDlgItem(IDC_BUTTON_LINECOLOR_2)->EnableWindow(FALSE);
		
		m_ColourPickerLineFill.ModifyStyle(0, WS_DISABLED);
		m_ColourPickerLineFill.Invalidate();		
	}
	
	if(!eFill.m_bShow)
	{
		m_FillColourPicker.ModifyStyle(0, WS_DISABLED);
		m_FillColourPicker.Invalidate();
	}
	else
	{
		m_FillColourPicker.ModifyStyle(WS_DISABLED, 0);
		m_FillColourPicker.Invalidate();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgRectSet::OnCheckFill() 
{
	this->UpdateData();
	if(!eFill.m_bShow)
	{
		m_FillColourPicker.ModifyStyle(0, WS_DISABLED);
		m_FillColourPicker.Invalidate();
	}
	else
	{
		m_FillColourPicker.ModifyStyle(WS_DISABLED, 0);
		m_FillColourPicker.Invalidate();
	}
}

LONG CDlgRectSet::OnSelEndOK(WPARAM wParam, LPARAM lParam)
{
	UpdateData();	
	LPPATTERNSTYLE lp = (LPPATTERNSTYLE) wParam;
	if(lParam == IDC_BUTTON_FILLTYPE)
	{
		eFill.sFillStyle.m_GradientFill = lp->m_GradientFill;
		eFill.sFillStyle.m_nFill = lp->m_nFill;
		eFill.sFillStyle.m_PatternFill = lp->m_PatternFill;
		eFill.sFillStyle.m_PictureFill = lp->m_PictureFill;
	}
	if(lParam == IDC_BUTTON_LINECOLOR)
	{
		eLine.s_LineStyle.m_GradientFill = lp->m_GradientFill;
		eLine.s_LineStyle.m_nFill = lp->m_nFill;
		eLine.s_LineStyle.m_PatternFill = lp->m_PatternFill;
		eLine.s_LineStyle.m_PictureFill = lp->m_PictureFill;
	}

	if(lParam == IDC_BUTTON_LINECOLOR_2)
	{
		eLine2.s_LineStyle.m_GradientFill = lp->m_GradientFill;
		eLine2.s_LineStyle.m_nFill = lp->m_nFill;
		eLine2.s_LineStyle.m_PatternFill = lp->m_PatternFill;
		eLine2.s_LineStyle.m_PictureFill = lp->m_PictureFill;
	}

	if(lParam == IDC_BUTTON_FILLLINE)
	{
		eLine.eLineSpaceFill.sFillStyle.m_GradientFill = lp->m_GradientFill;
		eLine.eLineSpaceFill.sFillStyle.m_nFill = lp->m_nFill;
		eLine.eLineSpaceFill.sFillStyle.m_PatternFill = lp->m_PatternFill;
		eLine.eLineSpaceFill.sFillStyle.m_PictureFill = lp->m_PictureFill;
	}
	return TRUE;
}

void CDlgRectSet::OnOk() 
{
	UpdateData();
	eFill.blendFunction.SourceConstantAlpha = 255 - m_nTrans;
	CDialog::OnOK();
}

void CDlgRectSet::OnCheckLinefill() 
{
	UpdateData();

	if( eLine.eLineSpaceFill.m_bShow )
	{
		m_ColourPickerLineFill.ModifyStyle(WS_DISABLED, 0);
		m_ColourPickerLineFill.Invalidate();		
	}
	else
	{
		m_ColourPickerLineFill.ModifyStyle(0, WS_DISABLED);
		m_ColourPickerLineFill.Invalidate();
	}	
}

void CDlgRectSet::OnSelendokComboLinetype() 
{
	UpdateData();

	if( eLine.m_nLineType )
	{
		CWnd *pWnd = GetDlgItem(IDC_CHECK_LINEFILL);
		pWnd->EnableWindow(TRUE);
		
		GetDlgItem(IDC_EDIT_SPACE_WIDTH)->EnableWindow(TRUE);
		
		if( eLine.m_nLineType == 2
			|| eLine.m_nLineType == 3
			|| eLine.m_nLineType == 5
			|| eLine.m_nLineType == 6)
		{
			GetDlgItem(IDC_EDIT_PENWIDE2)->EnableWindow(TRUE);
			
			GetDlgItem(IDC_COMBO_PENTYPE_2)->EnableWindow(TRUE);
			
			GetDlgItem(IDC_BUTTON_LINECOLOR_2)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_EDIT_PENWIDE2)->EnableWindow(FALSE);
			
			GetDlgItem(IDC_COMBO_PENTYPE_2)->EnableWindow(FALSE);
			
			GetDlgItem(IDC_BUTTON_LINECOLOR_2)->EnableWindow(FALSE);
		}
		
		if( eLine.eLineSpaceFill.m_bShow )
		{
			m_ColourPickerLineFill.ModifyStyle(WS_DISABLED, 0);
			m_ColourPickerLineFill.Invalidate();		
		}
		else
		{
			m_ColourPickerLineFill.ModifyStyle(0, WS_DISABLED);
			m_ColourPickerLineFill.Invalidate();
		}
	}
	else
	{
		CWnd *pWnd = GetDlgItem(IDC_CHECK_LINEFILL);
		pWnd->EnableWindow(FALSE);
		
		if(eLine.m_nLineType == 0 )
			GetDlgItem(IDC_EDIT_SPACE_WIDTH)->EnableWindow(FALSE);
		else
			GetDlgItem(IDC_EDIT_SPACE_WIDTH)->EnableWindow(TRUE);
		
		GetDlgItem(IDC_EDIT_PENWIDE2)->EnableWindow(FALSE);
		
		GetDlgItem(IDC_COMBO_PENTYPE_2)->EnableWindow(FALSE);
		
		GetDlgItem(IDC_BUTTON_LINECOLOR_2)->EnableWindow(FALSE);
		
		m_ColourPickerLineFill.ModifyStyle(0, WS_DISABLED);
		m_ColourPickerLineFill.Invalidate();		
	}
}
