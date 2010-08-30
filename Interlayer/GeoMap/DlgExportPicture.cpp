// DlgExportPicture.cpp : implementation file
//

#include "stdafx.h"
#include "../Resource.h"
#include "DlgExportPicture.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgExportPicture

IMPLEMENT_DYNAMIC(CDlgExportPicture, CFileDialog)

CDlgExportPicture::CDlgExportPicture(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
									 DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
	CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
	m_ofn.lpTemplateName = MAKEINTRESOURCE(IDD_DLG_EXPORT_PICTURE);

	m_iExportType = 1;
}


BEGIN_MESSAGE_MAP(CDlgExportPicture, CFileDialog)
	//{{AFX_MSG_MAP(CDlgExportPicture)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_RADIO_TYPE2, OnRadioType)
	ON_BN_CLICKED(IDC_RADIO_TYPE3, OnRadioType)
	ON_BN_CLICKED(IDC_RADIO_TYPE4, OnRadioType)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_QUALITY, OnReleasedcaptureSliderQuality)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDlgExportPicture::DoDataExchange(CDataExchange* pDX)
{
	CFileDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgExportPicture)
	DDX_Control(pDX, IDC_SLIDER_QUALITY, m_wndSilder);
	DDX_Radio(pDX, IDC_RADIO_TYPE2, m_iExportType);
	DDX_Slider(pDX, IDC_SLIDER_QUALITY, m_iQuality);
	//}}AFX_DATA_MAP
}

BOOL CDlgExportPicture::OnInitDialog() 
{
	CFileDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_ofn.hInstance = AfxGetInstanceHandle();

	m_iQuality = 50;
	m_wndSilder.SetRange(1,100);

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgExportPicture::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CFileDialog::OnPaint() for painting messages
}

void CDlgExportPicture::OnRadioType() 
{
	UpdateData(TRUE);	
}

void CDlgExportPicture::OnReleasedcaptureSliderQuality(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	m_iQuality = m_wndSilder.GetPos();
	*pResult = 0;
}
