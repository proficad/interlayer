// DlgPageSet.cpp : implementation file
//

#include "stdafx.h"
#include "DlgPageSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgPageSet dialog


CDlgPageSet::CDlgPageSet(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPageSet::IDD, pParent)
	, m_x(0)
	, m_y(0)
{
	//{{AFX_DATA_INIT(CDlgPageSet)
	m_strPageName = _T("");
	m_nHorzPages = 0;
	m_nVertPages = 0;
	m_lScale = 0;
	m_dAngle = 0.0;
	m_strMark = _T("");
	m_iMarginWidth = 0;
	//}}AFX_DATA_INIT
}

void CDlgPageSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPageSet)
	DDX_Text(pDX, IDC_EDIT_NAME, m_strPageName);
	DDX_Text(pDX, IDC_EDIT_HPAGE, m_nHorzPages);
	DDV_MinMaxUInt(pDX, m_nHorzPages, 1, 1000);
	DDX_Text(pDX, IDC_EDIT_VPAGE, m_nVertPages);
	DDV_MinMaxUInt(pDX, m_nVertPages, 1, 1000);
	DDX_Text(pDX, IDC_EDIT_SCALE, m_lScale);
	DDV_MinMaxDouble(pDX, m_lScale, 1, 500000);
	DDX_Text(pDX, IDC_EDIT_ANGLE, m_dAngle);
	DDV_MinMaxDouble(pDX, m_dAngle, -180., 180.);
	DDX_Text(pDX, IDC_EDIT_MARK, m_strMark);
	DDX_Text(pDX, IDC_EDIT_MARGIN_WIDTH, m_iMarginWidth);
	DDV_MinMaxInt(pDX, m_iMarginWidth, 0, 500);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT_X, m_x);
	DDX_Text(pDX, IDC_EDIT_Y, m_y);
}


BEGIN_MESSAGE_MAP(CDlgPageSet, CDialog)
	//{{AFX_MSG_MAP(CDlgPageSet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPageSet message handlers

