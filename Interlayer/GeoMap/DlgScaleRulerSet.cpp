// DlgScaleRulerSet.cpp : implementation file
//

#include "stdafx.h"
#include "DlgScaleRulerSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgScaleRulerSet dialog


CDlgScaleRulerSet::CDlgScaleRulerSet(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgScaleRulerSet::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgScaleRulerSet)
	m_uFontSize = 0;
	m_uHeight = 0;
	m_uWidth = 0;
	m_ScaleType = -1;
	//}}AFX_DATA_INIT
}


void CDlgScaleRulerSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgScaleRulerSet)
	DDX_Control(pDX, IDC_COMBO_FONT, m_wndFont);
	DDX_Control(pDX, IDC_BTN_FONT_COLOR, m_ColorFont);
	DDX_Control(pDX, IDC_BTN_COLOR_PEN, m_ColorPen);
	DDX_Text(pDX, IDC_EDIT_FONT_SIZE, m_uFontSize);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_uHeight);
	DDX_Text(pDX, IDC_EDIT_WIDTH, m_uWidth);
	DDX_Radio(pDX, IDC_RADIO_SCALE_1, m_ScaleType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgScaleRulerSet, CDialog)
	//{{AFX_MSG_MAP(CDlgScaleRulerSet)
	ON_BN_CLICKED(IDC_BTN_FONT_COLOR, OnBtnFontColor)
	ON_BN_CLICKED(IDC_BTN_COLOR_PEN, OnBtnColorPen)
	ON_BN_CLICKED(IDOK, OnOk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgScaleRulerSet message handlers


void CDlgScaleRulerSet::OnBtnFontColor() 
{
	m_clrFont = m_ColorFont.GetColor ();
}

void CDlgScaleRulerSet::OnBtnColorPen() 
{
	m_clrPen = m_ColorPen.GetColor ();
}

void CDlgScaleRulerSet::OnOk() 
{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);

	CMFCFontInfo* pDesc = m_wndFont.GetSelFont();

	strcpy_s(m_lfFont.lfFaceName,pDesc->m_strName);	
	m_lfFont.lfPitchAndFamily = pDesc->m_nPitchAndFamily;
	m_lfFont.lfCharSet = pDesc->m_nCharSet;
	m_lfFont.lfHeight = m_uFontSize;

	CDialog::OnOK();
	
}

BOOL CDlgScaleRulerSet::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ColorFont.EnableAutomaticButton (_T("自动"), RGB (0, 0, 0));
	m_ColorFont.EnableOtherButton (_T("其它"));
	m_ColorFont.SetColor (m_clrFont);
	m_ColorFont.SetColumnsNumber (10);

	m_ColorPen.EnableAutomaticButton (_T("自动"), RGB (0, 0, 0));
	m_ColorPen.EnableOtherButton (_T("其它"));
	m_ColorPen.SetColor (m_clrPen);
	m_ColorPen.SetColumnsNumber (10);

	m_wndFont.SelectFont (m_lfFont.lfFaceName);
//	m_wndFont.SelectFont (_T("Arial"));

	m_uFontSize = labs(m_lfFont.lfHeight);

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
