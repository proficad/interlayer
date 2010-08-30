// DlgDepthSetup.cpp : implementation file
//

#include "stdafx.h"
#include "DlgDepthSetup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgDepthSetup dialog


CDlgDepthSetup::CDlgDepthSetup(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDepthSetup::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgDepthSetup)
	m_dMaxDepth = 0.0;
	m_dMaxVal = 0.0;
	m_dMinDepth = 0.0;
	m_dMinVal = 0.0;
	//}}AFX_DATA_INIT
}


void CDlgDepthSetup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDepthSetup)
	DDX_Text(pDX, IDC_EDIT_MAXDEPTH, m_dMaxDepth);
	DDX_Text(pDX, IDC_EDIT_MAXVAL, m_dMaxVal);
	DDX_Text(pDX, IDC_EDIT_MINDEPTH, m_dMinDepth);
	DDX_Text(pDX, IDC_EDIT_MINVAL, m_dMinVal);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgDepthSetup, CDialog)
	//{{AFX_MSG_MAP(CDlgDepthSetup)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgDepthSetup message handlers
