// DlgCurveSetup.cpp : implementation file
//

#include "stdafx.h"
#include "DlgCurveSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgCurveSetup dialog


CDlgCurveSetup::CDlgCurveSetup(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCurveSetup::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgCurveSetup)
	m_iFillStyle = -1;
	m_uPenWidth = 0;
	m_dMinVal = 0.0;
	m_dMaxVal = 0.0;
	m_dCoordMin = 0.0;
	m_dCoordMax = 0.0;
	m_bFill = FALSE;
	m_iPart = 0;
	m_iMarkPart = 0;
	m_bDirect = FALSE;
	m_bSameCoord = FALSE;
	m_iLineStyle = 0;
	m_bLogCoord = FALSE;
	m_strCurveTitle = _T("");
	m_strDepthFormula = _T("");
	m_strVarFormula = _T("");
	//}}AFX_DATA_INIT

	m_clrFill = RGB(0,0,0);
	m_clrLine = RGB(0,0,0);
}


void CDlgCurveSetup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCurveSetup)
	//DDX_Control(pDX, IDC_CBLINESTYLE, m_lineType);
	DDX_Radio(pDX, IDC_BSSTYLE, m_iFillStyle);
	DDX_Text(pDX, IDC_LINESIZE, m_uPenWidth);
	DDV_MinMaxUInt(pDX, m_uPenWidth, 0, 50);
	DDX_Text(pDX, IDC_EDIT_MIN, m_dMinVal);
	DDX_Text(pDX, IDC_EDIT_MAX, m_dMaxVal);
	DDX_Text(pDX, IDC_EDIT_CMIN, m_dCoordMin);
	DDX_Text(pDX, IDC_EDIT_CMAX, m_dCoordMax);
	DDX_Check(pDX, IDC_CBFILL, m_bFill);
	DDX_Text(pDX, IDC_EDIT_STEPS, m_iPart);
	DDV_MinMaxUInt(pDX, m_iPart, 1, 100);
	DDX_Text(pDX, IDC_EDIT_MSTEPS, m_iMarkPart);
	DDV_MinMaxUInt(pDX, m_iMarkPart, 1, 10);
	DDX_Check(pDX, IDC_CHECK_DIR, m_bDirect);
	DDX_Check(pDX, IDC_CHECK_SAMECOORD, m_bSameCoord);
	DDX_CBIndex(pDX, IDC_CBLINESTYLE,m_iLineStyle );
	DDX_Check(pDX, IDC_CHECK_LOGCOORD, m_bLogCoord);
	DDX_Text(pDX, IDC_EDIT_CURVE_NAME, m_strCurveTitle);
	DDX_Text(pDX, IDC_EDIT_DEPTH, m_strDepthFormula);
	DDX_Text(pDX, IDC_EDIT_VAR, m_strVarFormula);
	DDX_Control(pDX, IDC_BTN_LINECOLOR, m_LineColor);
	DDX_Control(pDX, IDC_FILLCOLOR, m_FillColor);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgCurveSetup, CDialog)
	//{{AFX_MSG_MAP(CDlgCurveSetup)
	ON_BN_CLICKED(IDC_BTN_LINECOLOR, OnBtnLinecolor)
	ON_BN_CLICKED(IDC_FILLCOLOR, OnFillcolor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgCurveSetup message handlers

void CDlgCurveSetup::OnBtnLinecolor() 
{
	m_clrLine = m_LineColor.GetColor();
}

void CDlgCurveSetup::OnFillcolor() 
{
	m_clrFill = m_FillColor.GetColor();
}

BOOL CDlgCurveSetup::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_lineType.SubclassDlgItem(IDC_CBLINESTYLE, this);
	
//	m_FillColor.EnableAutomaticButton (_T("自动"), RGB(0,0,0));
	m_FillColor.EnableOtherButton (_T("其它"));
	m_FillColor.SetColor(m_clrFill);
	m_FillColor.SetColumnsNumber (8);

//	m_LineColor.EnableAutomaticButton (_T("自动"), RGB(0,0,0));
	m_LineColor.EnableOtherButton (_T("其它"));
	m_LineColor.SetColor(m_clrLine);
	m_LineColor.SetColumnsNumber (8);	
	
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
