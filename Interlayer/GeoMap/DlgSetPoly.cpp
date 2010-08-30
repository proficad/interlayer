// DlgSetPoly.cpp : implementation file
//

#include "stdafx.h"
#include "DlgSetPoly.h"

#define CPN_SELENDOK         WM_USER + 1004        // Colour Picker end OK
#define APN_SELENDOK         WM_USER + 1014        // Arrow Picker end OK


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgPolySet dialog


CDlgPolySet::CDlgPolySet(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPolySet::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgPolySet)
	m_nTrans = 0;	
	m_iSpaceWidth = 10;
	//}}AFX_DATA_INIT
}


void CDlgPolySet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPolySet)
	DDX_Control(pDX, IDC_SLIDER_TRANS, m_CtrlTrans);
	DDX_Control(pDX, IDC_BUTTON_PENTYPE, m_PenPicker);
	DDX_Control(pDX, IDC_BUTTON_PENTYPE_2, m_PenPicker2);
	DDX_Control(pDX, IDC_BUTTON_FILLTYPE, m_FillPicker);
	DDX_Control(pDX, IDC_BUTTON_LINEFILLCOLOR, m_ColourPickerLineFill);
	DDX_Control(pDX, IDC_BUTTON_BEGINARROW, m_BeginArrowPicker);
	DDX_Control(pDX, IDC_BUTTON_ENDARROW, m_EndArrowPicker);
	DDX_Slider(pDX, IDC_SLIDER_TRANS, m_nTrans);
	//}}AFX_DATA_MAP

	DDX_Text(pDX, IDC_EDIT_LINE_WIDTH, m_iSpaceWidth);
	DDV_MinMaxUInt(pDX, m_iSpaceWidth, 5, 500);

	DDX_Check(pDX, IDC_CHECK_FILL, eFill.m_bShow);
	DDX_Check(pDX, IDC_CHECK_POLYGON, eLine.m_bClose);	
	DDX_Check(pDX, IDC_CHECK_SQUAREPEN, eLine.m_bSquarePen);
	
	DDX_CBIndex(pDX, IDC_COMBO_PENTYPE, eLine.m_nPenStyle);
	DDX_CBIndex(pDX, IDC_COMBO_PENSTYLE, eLine.m_nLineType);	
	DDX_Text(pDX, IDC_EDIT_PENWIDE, eLine.m_nPenWide);
	DDV_MinMaxUInt(pDX, eLine.m_nPenWide, 0, 300);
	DDX_Check(pDX, IDC_CHECK_PLINE, eLine.m_bSmooth);
	
	DDX_Text(pDX, IDC_EDIT_BEGINSIZE, eLine.m_uBeginPointSize);
	DDX_Text(pDX, IDC_EDIT_ENDSIZE, eLine.m_uEndPointSize);
	DDV_MinMaxUInt(pDX, eLine.m_uBeginPointSize, 0, 300);
	DDV_MinMaxUInt(pDX, eLine.m_uEndPointSize, 0, 300);
	
	DDX_Check(pDX, IDC_CHECK_LINEFILL, eLine.eLineSpaceFill.m_bShow);
	DDX_ColourPicker(pDX, IDC_BUTTON_PENTYPE, eLine.s_LineStyle.m_PatternFill.m_crColourFore);
	DDX_ColourPicker(pDX, IDC_BUTTON_FILLTYPE, eFill.sFillStyle.m_PatternFill.m_crColourFore);
	DDX_ColourPicker(pDX, IDC_BUTTON_LINEFILLCOLOR, eLine.eLineSpaceFill.sFillStyle.m_PatternFill.m_crColourFore);	
	DDX_ArrowPicker(pDX, IDC_BUTTON_BEGINARROW, eLine.m_uBeginPointMode);	
	DDX_ArrowPicker(pDX, IDC_BUTTON_ENDARROW, eLine.m_uEndPointMode);
		
	DDX_CBIndex(pDX, IDC_COMBO_PENTYPE_2, eLine2.m_nPenStyle);
	DDX_Text(pDX, IDC_EDIT_PENWIDE_2, eLine2.m_nPenWide);
	DDV_MinMaxUInt(pDX, eLine2.m_nPenWide, 0, 300);
		
	DDX_ColourPicker(pDX, IDC_BUTTON_PENTYPE_2, eLine2.s_LineStyle.m_PatternFill.m_crColourFore);
}


BEGIN_MESSAGE_MAP(CDlgPolySet, CDialog)
	//{{AFX_MSG_MAP(CDlgPolySet)
	ON_BN_CLICKED(IDC_CHECK_FILL, OnCheckFill)
	ON_BN_CLICKED(IDC_CHECK_POLYGON, OnCheckPolygon)
	ON_BN_CLICKED(IDOK, OnOk)
	ON_CBN_SELENDOK(IDC_COMBO_PENSTYLE, OnSelendokComboPenstyle)
	ON_BN_CLICKED(IDC_CHECK_LINEFILL, OnCheckLinefill)
	//}}AFX_MSG_MAP
	ON_MESSAGE(CPN_SELENDOK,     OnSelEndOK)
	ON_MESSAGE(APN_SELENDOK,     OnSelEndOK)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPolySet message handlers

BOOL CDlgPolySet::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	UpdateData(FALSE);

	line.SubclassDlgItem(IDC_COMBO_PENTYPE, this);
	line2.SubclassDlgItem(IDC_COMBO_PENTYPE_2, this);
	lineStyle.SubclassDlgItem(IDC_COMBO_PENSTYLE, this);

	m_FillPicker.SetPatternText("填充效果(&F)...");
	m_PenPicker.SetPatternText("带图案线条(&P)...");
	m_PenPicker2.SetPatternText("带图案线条(&P)...");

	m_nTrans = 255 - eFill.blendFunction.SourceConstantAlpha;

	m_CtrlTrans.SetRange(0, 255);
	m_CtrlTrans.SetPos(m_nTrans);
	
	if( eLine.m_nLineType)
	{
		CWnd *pWnd = GetDlgItem(IDC_CHECK_LINEFILL);
		pWnd->EnableWindow(TRUE);

		GetDlgItem(IDC_EDIT_LINE_WIDTH)->EnableWindow(TRUE);
		
		if( eLine.m_nLineType == 2
			|| eLine.m_nLineType == 3
			|| eLine.m_nLineType == 5
			|| eLine.m_nLineType == 6)
		{
			GetDlgItem(IDC_EDIT_PENWIDE_2)->EnableWindow(TRUE);
			
			GetDlgItem(IDC_COMBO_PENTYPE_2)->EnableWindow(TRUE);
			
			GetDlgItem(IDC_BUTTON_PENTYPE_2)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_EDIT_PENWIDE_2)->EnableWindow(FALSE);
			
			GetDlgItem(IDC_COMBO_PENTYPE_2)->EnableWindow(FALSE);
			
			GetDlgItem(IDC_BUTTON_PENTYPE_2)->EnableWindow(FALSE);
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
		
		pWnd = GetDlgItem(IDC_EDIT_BEGINSIZE);
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
		CWnd *pWnd = GetDlgItem(IDC_CHECK_LINEFILL);
		pWnd->EnableWindow(FALSE);
		
		if(eLine.m_nLineType == 0 )
			GetDlgItem(IDC_EDIT_LINE_WIDTH)->EnableWindow(FALSE);
		else
			GetDlgItem(IDC_EDIT_LINE_WIDTH)->EnableWindow(TRUE);
		
		GetDlgItem(IDC_EDIT_PENWIDE_2)->EnableWindow(FALSE);
		
		GetDlgItem(IDC_COMBO_PENTYPE_2)->EnableWindow(FALSE);
		
		GetDlgItem(IDC_BUTTON_PENTYPE_2)->EnableWindow(FALSE);

		m_ColourPickerLineFill.ModifyStyle(0, WS_DISABLED);
		m_ColourPickerLineFill.Invalidate();

		m_BeginArrowPicker.ModifyStyle(WS_DISABLED, 0);
		m_BeginArrowPicker.Invalidate();
		m_EndArrowPicker.ModifyStyle(WS_DISABLED, 0);
		m_EndArrowPicker.Invalidate();

		if( eLine.m_uBeginPointMode )
		{
			pWnd = GetDlgItem(IDC_EDIT_BEGINSIZE);
			pWnd->EnableWindow(TRUE);
		}
		else
		{
			pWnd = GetDlgItem(IDC_EDIT_BEGINSIZE);
			pWnd->EnableWindow(FALSE);
		}
		if( eLine.m_uEndPointMode )
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
// 	if(!eLine.m_bClose)
// 	{	
// 		CWnd *pWnd = GetDlgItem(IDC_CHECK_FILL);
// 		pWnd->EnableWindow(FALSE);
// 		m_FillPicker.ModifyStyle(0, WS_DISABLED);
// 		m_FillPicker.Invalidate();
// 		m_CtrlTrans.EnableWindow(FALSE);
// 	}
// 	else
// 	{
// 		if(!eFill.m_bShow)
// 		{	
// 			m_FillPicker.ModifyStyle(0, WS_DISABLED);
// 			m_FillPicker.Invalidate();
// 			m_CtrlTrans.EnableWindow(FALSE);
// 		}
// 		else
// 		{
// 			m_CtrlTrans.EnableWindow(TRUE);
// 			m_FillPicker.ModifyStyle(WS_DISABLED, 0);
// 			m_FillPicker.Invalidate();
// 		}
// 		CWnd *pWnd = GetDlgItem(IDC_CHECK_FILL);
// 		pWnd->EnableWindow(TRUE);
// 	}
	
	line.SetCurSel(eLine.m_nPenStyle);
	lineStyle.SetCurSel(eLine.m_nLineType);
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgPolySet::OnCheckFill() 
{
	this->UpdateData();
	if(!eFill.m_bShow)
	{	
		m_FillPicker.ModifyStyle(0, WS_DISABLED);
		m_FillPicker.Invalidate();
		m_CtrlTrans.EnableWindow(FALSE);
	}
	else
	{
		m_CtrlTrans.EnableWindow(TRUE);
		m_FillPicker.ModifyStyle(WS_DISABLED, 0);
		m_FillPicker.Invalidate();
	}	
}

void CDlgPolySet::OnCheckPolygon() 
{
	this->UpdateData();
// 	if(!eLine.m_bClose)
// 	{	
// 		CWnd *pWnd = GetDlgItem(IDC_CHECK_FILL);
// 		pWnd->EnableWindow(FALSE);
// 		m_FillPicker.ModifyStyle(0, WS_DISABLED);
// 		m_FillPicker.Invalidate();
// 		m_CtrlTrans.EnableWindow(FALSE);
// 	}
// 	else
// 	{
// 		if(!eFill.m_bShow)
// 		{	
// 			m_FillPicker.ModifyStyle(0, WS_DISABLED);
// 			m_FillPicker.Invalidate();
// 			m_CtrlTrans.EnableWindow(FALSE);
// 		}
// 		else
// 		{
// 			m_CtrlTrans.EnableWindow(TRUE);
// 			m_FillPicker.ModifyStyle(WS_DISABLED, 0);
// 			m_FillPicker.Invalidate();
// 		}
// 		CWnd *pWnd = GetDlgItem(IDC_CHECK_FILL);
// 		pWnd->EnableWindow(TRUE);
// 	}
}

void CDlgPolySet::OnOk() 
{
	UpdateData();
	eFill.blendFunction.SourceConstantAlpha = 255 - m_nTrans;
	CDialog::OnOK();
}


LONG CDlgPolySet::OnSelEndOK(WPARAM wParam, LPARAM lParam)
{
	LPPATTERNSTYLE lp = (LPPATTERNSTYLE) wParam;
	if(lParam == IDC_BUTTON_FILLTYPE)
	{
		eFill.sFillStyle.m_GradientFill = lp->m_GradientFill;
		eFill.sFillStyle.m_nFill = lp->m_nFill;
		eFill.sFillStyle.m_PatternFill = lp->m_PatternFill;
		eFill.sFillStyle.m_PictureFill = lp->m_PictureFill;
	}
	if(lParam == IDC_BUTTON_PENTYPE)
	{
		eLine.s_LineStyle.m_GradientFill = lp->m_GradientFill;
		eLine.s_LineStyle.m_nFill = lp->m_nFill;
		eLine.s_LineStyle.m_PatternFill = lp->m_PatternFill;
		eLine.s_LineStyle.m_PictureFill = lp->m_PictureFill;
	}
	if(lParam == IDC_BUTTON_PENTYPE_2)
	{
		eLine2.s_LineStyle.m_GradientFill = lp->m_GradientFill;
		eLine2.s_LineStyle.m_nFill = lp->m_nFill;
		eLine2.s_LineStyle.m_PatternFill = lp->m_PatternFill;
		eLine2.s_LineStyle.m_PictureFill = lp->m_PictureFill;
	}
	if(lParam == IDC_BUTTON_FILLTYPE)
	{
		eFill.sFillStyle.m_GradientFill = lp->m_GradientFill;
		eFill.sFillStyle.m_nFill = lp->m_nFill;
		eFill.sFillStyle.m_PatternFill = lp->m_PatternFill;
		eFill.sFillStyle.m_PictureFill = lp->m_PictureFill;
	}
	if(lParam == IDC_BUTTON_LINEFILLCOLOR)
	{
		eLine.eLineSpaceFill.sFillStyle.m_GradientFill = lp->m_GradientFill;
		eLine.eLineSpaceFill.sFillStyle.m_nFill = lp->m_nFill;
		eLine.eLineSpaceFill.sFillStyle.m_PatternFill = lp->m_PatternFill;
		eLine.eLineSpaceFill.sFillStyle.m_PictureFill = lp->m_PictureFill;
	}

	if(lParam == IDC_BUTTON_BEGINARROW)
	{
		eLine.m_uBeginPointMode = (int) wParam;
		CWnd *pWnd;
		if( eLine.m_uBeginPointMode )
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
	if( lParam == IDC_BUTTON_ENDARROW)
	{
		eLine.m_uEndPointMode = (int) wParam;
		CWnd *pWnd;
		
		if( eLine.m_uEndPointMode )
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
	return TRUE;
}

void CDlgPolySet::OnSelendokComboPenstyle() 
{
	UpdateData();

	if( eLine.m_nLineType)
	{		
		CWnd *pWnd = GetDlgItem(IDC_CHECK_LINEFILL);
		pWnd->EnableWindow(TRUE);

		GetDlgItem(IDC_EDIT_LINE_WIDTH)->EnableWindow(TRUE);

		if( eLine.m_nLineType == 2
			|| eLine.m_nLineType == 3
			|| eLine.m_nLineType == 5
			|| eLine.m_nLineType == 6)
		{
			GetDlgItem(IDC_EDIT_PENWIDE_2)->EnableWindow(TRUE);
			
			GetDlgItem(IDC_COMBO_PENTYPE_2)->EnableWindow(TRUE);
			
			GetDlgItem(IDC_BUTTON_PENTYPE_2)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_EDIT_PENWIDE_2)->EnableWindow(FALSE);
			
			GetDlgItem(IDC_COMBO_PENTYPE_2)->EnableWindow(FALSE);
			
			GetDlgItem(IDC_BUTTON_PENTYPE_2)->EnableWindow(FALSE);
		}	

		if( eLine.eLineSpaceFill.m_bShow )
		{
			m_ColourPickerLineFill.ModifyStyle(WS_DISABLED, 0);
			m_FillPicker.Invalidate();		
		}
		else
		{
			m_ColourPickerLineFill.ModifyStyle(0, WS_DISABLED);
			m_FillPicker.Invalidate();
		}

		pWnd = GetDlgItem(IDC_EDIT_BEGINSIZE);
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
		CWnd *pWnd = GetDlgItem(IDC_CHECK_LINEFILL);
		pWnd->EnableWindow(FALSE);
		
		if(eLine.m_nLineType == 0 )
			GetDlgItem(IDC_EDIT_LINE_WIDTH)->EnableWindow(FALSE);
		else
			GetDlgItem(IDC_EDIT_LINE_WIDTH)->EnableWindow(TRUE);

		GetDlgItem(IDC_EDIT_PENWIDE_2)->EnableWindow(FALSE);

		GetDlgItem(IDC_COMBO_PENTYPE_2)->EnableWindow(FALSE);

		GetDlgItem(IDC_BUTTON_PENTYPE_2)->EnableWindow(FALSE);		

		m_ColourPickerLineFill.ModifyStyle(0, WS_DISABLED);
		m_FillPicker.Invalidate();	
		
		m_BeginArrowPicker.ModifyStyle(WS_DISABLED, 0);
		m_BeginArrowPicker.Invalidate();
		m_EndArrowPicker.ModifyStyle(WS_DISABLED, 0);
		m_EndArrowPicker.Invalidate();

		if( eLine.m_uBeginPointMode )
		{
			pWnd = GetDlgItem(IDC_EDIT_BEGINSIZE);
			pWnd->EnableWindow(TRUE);
		}
		else
		{
			pWnd = GetDlgItem(IDC_EDIT_BEGINSIZE);
			pWnd->EnableWindow(FALSE);
		}
		if( eLine.m_uEndPointMode )
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

}

void CDlgPolySet::OnCheckLinefill() 
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
