// DlgWellSet.cpp : implementation file
//

#include "stdafx.h"
#include "DlgWellSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgWellSet dialog


CDlgWellSet::CDlgWellSet(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgWellSet::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgWellSet)
	m_nFontSize = 0;
	m_nSymbolSize = 0;
	m_iApplyWell = 3;
	m_strWellName = _T("");
	m_dZZBX = 0.0;
	m_dHZBY = 0.0;
	m_dBXHB = 0.0;
	m_iWellStyle = -1;
	m_iWellType = -1;
	m_iApply = 0;
	//}}AFX_DATA_INIT
}


void CDlgWellSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgWellSet)
	DDX_Control(pDX, IDC_BTN_PEN_COLOR, m_wndPenColor);
	DDX_Control(pDX, IDC_BTN_FILL_COLOR, m_wndFillColor);
	DDX_Control(pDX, IDC_COMBO_SYMBOL, m_wndSymbol);
	DDX_Control(pDX, IDC_BTN_FONT_COLOR, m_wndFontColor);
	DDX_Control(pDX, IDC_COMBO_FONT, m_wndFont);
	DDX_Text(pDX, IDC_EDIT_FONT_SIZE, m_nFontSize);
	DDV_MinMaxUInt(pDX, m_nFontSize, 1, 200);
	DDX_Text(pDX, IDC_EDIT_SYMBOL_SIZE, m_nSymbolSize);
	DDV_MinMaxUInt(pDX, m_nSymbolSize, 1, 200);
	DDX_Radio(pDX, IDC_RADIO_APPLY_TYPE, m_iApplyWell);
	DDX_Text(pDX, IDC_EDIT_NAME, m_strWellName);
	DDX_Text(pDX, IDC_EDIT_ZZBX, m_dZZBX);
	DDX_Text(pDX, IDC_EDIT_HZBY, m_dHZBY);
	DDX_Text(pDX, IDC_EDIT_BXHB, m_dBXHB);
	DDX_CBIndex(pDX, IDC_COMBO_STYLE, m_iWellStyle);
	DDX_CBIndex(pDX, IDC_COMBO_TYPE, m_iWellType);
	DDX_Radio(pDX, IDC_RADIO_APPLY_LAYER, m_iApply);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgWellSet, CDialog)
	//{{AFX_MSG_MAP(CDlgWellSet)
	ON_BN_CLICKED(IDC_BTN_FONT_COLOR, OnBtnFontColor)
	ON_BN_CLICKED(IDC_BTN_PEN_COLOR, OnBtnPenColor)
	ON_BN_CLICKED(IDC_BTN_FILL_COLOR, OnBtnFillColor)	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgWellSet message handlers

void CDlgWellSet::OnBtnFontColor() 
{
	m_clrFont = m_wndFontColor.GetColor ();
}

void CDlgWellSet::OnBtnPenColor() 
{
	m_clrPen = m_wndPenColor.GetColor ();
}

void CDlgWellSet::OnBtnFillColor() 
{
	m_clrFill = m_wndFillColor.GetColor ();
}

void CDlgWellSet::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	
	CMFCFontInfo* pDesc = m_wndFont.GetSelFont();
	
	strcpy_s(m_lfFont.lfFaceName,pDesc->m_strName);	
	m_lfFont.lfPitchAndFamily = pDesc->m_nPitchAndFamily;
	m_lfFont.lfCharSet = pDesc->m_nCharSet;
	m_lfFont.lfHeight = m_nFontSize;

	CDialog::OnOK();
}

BOOL CDlgWellSet::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_wndFontColor.EnableAutomaticButton (_T("自动"), RGB (0, 0, 0));
	m_wndFontColor.EnableOtherButton (_T("其它"));
	m_wndFontColor.SetColor (m_clrFont);
	m_wndFontColor.SetColumnsNumber (10);
	
	m_wndPenColor.EnableAutomaticButton (_T("自动"), RGB (0, 0, 0));
	m_wndPenColor.EnableOtherButton (_T("其它"));
	m_wndPenColor.SetColor (m_clrPen);
	m_wndPenColor.SetColumnsNumber (10);
	
	m_wndFillColor.EnableAutomaticButton (_T("自动"), RGB (0, 0, 0));
	m_wndFillColor.EnableOtherButton (_T("其它"));
	m_wndFillColor.SetColor (m_clrFill);
	m_wndFillColor.SetColumnsNumber (10);
	
	m_wndFont.SelectFont (m_lfFont.lfFaceName);
	//	m_wndFont.SelectFont (_T("Arial"));
	
	m_nFontSize = labs(m_lfFont.lfHeight);

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

