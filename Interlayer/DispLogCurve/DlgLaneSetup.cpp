// DlgLaneSetup.cpp : implementation file
//

#include "stdafx.h"
#include "DlgLaneSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgLaneSetup dialog


CDlgLaneSetup::CDlgLaneSetup(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLaneSetup::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgLaneSetup)
	m_bFill = FALSE;
	m_bBorder = FALSE;
	m_bHorzLine = FALSE;
	m_bVertLine = FALSE;
	m_dWidth = 1.0;
	m_bApplyAll = FALSE;
	m_strTitle = _T("");
	//}}AFX_DATA_INIT
	m_clrFill = RGB(255,255,239);
}


void CDlgLaneSetup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgLaneSetup)
	DDX_Check(pDX, IDC_CHECK_FILL, m_bFill);
	DDX_Check(pDX, IDC_CHECK_BORDER, m_bBorder);
	DDX_Check(pDX, IDC_CHECK_HORZ, m_bHorzLine);
	DDX_Check(pDX, IDC_CHECK_VERT, m_bVertLine);
	DDX_Text(pDX, IDC_EDIT_WIDTH, m_dWidth);
	DDV_MinMaxDouble(pDX, m_dWidth, 1., 20.);
	DDX_Check(pDX, IDC_CHECK_APPLY_ALL, m_bApplyAll);
	DDX_Control(pDX, IDC_BTN_FILLCOLOR, m_ColorPicker);
	DDX_Text(pDX, IDC_EDIT_TITLE, m_strTitle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgLaneSetup, CDialog)
	//{{AFX_MSG_MAP(CDlgLaneSetup)
	ON_BN_CLICKED(IDC_BTN_FILLCOLOR, OnBtnFillcolor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgLaneSetup message handlers

void CDlgLaneSetup::OnBtnFillcolor() 
{
	m_clrFill = m_ColorPicker.GetColor ();
}

BOOL CDlgLaneSetup::OnInitDialog() 
{
	CDialog::OnInitDialog();
		
//	m_ColorPicker.EnableAutomaticButton (_T("自动"), RGB(255,255,239));
	m_ColorPicker.EnableOtherButton (_T("其它"));
	m_ColorPicker.SetColor(m_clrFill);
	m_ColorPicker.SetColumnsNumber (8);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
