// DlgColorTableSet.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgColorTableSet.h"
#include "3dlib/ClrFileDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgColorTableSet dialog


CDlgColorTableSet::CDlgColorTableSet(CWnd* pParent /*=NULL*/)
: CDialog(CDlgColorTableSet::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgColorTableSet)
	m_NewPegPosition = 0.0;
	m_SelPegPosition = 0.0;
	//}}AFX_DATA_INIT
}


void CDlgColorTableSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgColorTableSet)
	DDX_Control(pDX, IDC_LIST_COLOR, m_wndListColor);
	DDX_Control(pDX, IDC_SELPEG_COLOUR, m_wndSelColor);
	DDX_Control(pDX, IDC_NEWPEG_COLOUR, m_wndNewColor);
	DDX_Text(pDX, IDC_NEWPEG_POSITION, m_NewPegPosition);
	DDV_MinMaxDouble(pDX, m_NewPegPosition, m_ColorTable.GetMinValue(), m_ColorTable.GetMaxValue());
	DDX_Text(pDX, IDC_SELPEG_POSITION, m_SelPegPosition);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgColorTableSet, CDialog)
	//{{AFX_MSG_MAP(CDlgColorTableSet)
	ON_BN_CLICKED(IDC_BTN_LOAD, OnBtnLoad)
	ON_BN_CLICKED(IDC_BTN_SAVE, OnBtnSave)
	ON_BN_CLICKED(IDC_DEL_PEG, OnDelPeg)
	ON_BN_CLICKED(IDC_ADD_PEG, OnAddPeg)
	ON_BN_CLICKED(IDC_SELPEG_COLOUR, OnSelpegColour)
	ON_WM_PAINT()
	ON_LBN_SELCHANGE(IDC_LIST_COLOR, OnSelchangeListColor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgColorTableSet message handlers

BOOL CDlgColorTableSet::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	m_wndNewColor.EnableAutomaticButton (_T("自动"), RGB (0, 0, 0));
	m_wndNewColor.EnableOtherButton (_T("其它"));
	m_wndNewColor.SetColumnsNumber (10);

	m_wndSelColor.EnableAutomaticButton (_T("自动"), RGB (0, 0, 0));
	m_wndSelColor.EnableOtherButton (_T("其它"));
	m_wndSelColor.SetColor (-1);
	m_wndSelColor.SetColumnsNumber (10);

	// 	m_NewPegPosition = 0.5f;
	// 	m_wndNewColor.SetColor(0x000000FF);

	m_wndListColor.m_pColorTable = &m_ColorTable;

	int nCount = m_ColorTable.GetColorTableCount();
	for (int i=0;i<nCount;i++)
	{
		double v = m_ColorTable.GetValue(i);

		CString str;
		str.Format("%lf",v);
		m_wndListColor.AddString(str);
	}

	int n = m_wndListColor.GetCurSel();
	if(n < 0)
	{
		GetDlgItem(IDC_DEL_PEG)->EnableWindow(FALSE);
		GetDlgItem(IDC_SELPEG_POSITION)->EnableWindow(FALSE);
		GetDlgItem(IDC_SELPEG_COLOUR)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_DEL_PEG)->EnableWindow(TRUE);
		GetDlgItem(IDC_SELPEG_POSITION)->EnableWindow(TRUE);
		GetDlgItem(IDC_SELPEG_COLOUR)->EnableWindow(TRUE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgColorTableSet::OnBtnLoad() 
{
	// TODO: Add your control notification handler code here
	CClrFileDialog Dlg(
		TRUE, // TRUE for FileOpen, FALSE for FileSaveAs
		"clr",
		NULL,	//LPCTSTR lpszFileName = NULL,
		OFN_HIDEREADONLY  | OFN_OVERWRITEPROMPT | OFN_ENABLETEMPLATE,
		"颜色定义 (*.clr) |*.clr||",
		NULL);

	
	if(Dlg.DoModal()!=IDOK)
		return;

	m_ColorTable.LoadColorTable(Dlg.GetPathName());

	m_wndListColor.ResetContent();

	m_wndListColor.m_pColorTable = &m_ColorTable;

	int nCount = m_ColorTable.GetColorTableCount();
	for (int i=0;i<nCount;i++)
	{
		double v = m_ColorTable.GetValue(i);

		CString str;
		str.Format("%lf",v);
		m_wndListColor.AddString(str);
	}

	CRect rect;
	GetDlgItem(IDC_GRADIENT)->GetClientRect(rect);
	GetDlgItem(IDC_GRADIENT)->ClientToScreen(rect);
	ScreenToClient(rect);
	InvalidateRect(rect);
}

void CDlgColorTableSet::OnBtnSave() 
{
	// TODO: Add your control notification handler code here

	CFileDialog Dlg(
		FALSE, // TRUE for FileOpen, FALSE for FileSaveAs
		"clr",
		NULL,	//LPCTSTR lpszFileName = NULL,
		OFN_HIDEREADONLY  | OFN_OVERWRITEPROMPT,
		"颜色定义 (*.clr) |*.clr||",
		NULL);

	if(Dlg.DoModal()!=IDOK)
	{
		return;
	}

	m_ColorTable.SaveColorTable(Dlg.GetPathName());
}

void CDlgColorTableSet::OnDelPeg() 
{
	UpdateData(TRUE);
	int n = m_wndListColor.GetCurSel();

	if(n < 0)
	{
		GetDlgItem(IDC_DEL_PEG)->EnableWindow(FALSE);
		GetDlgItem(IDC_SELPEG_POSITION)->EnableWindow(FALSE);
		GetDlgItem(IDC_SELPEG_COLOUR)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_DEL_PEG)->EnableWindow(TRUE);
		GetDlgItem(IDC_SELPEG_POSITION)->EnableWindow(TRUE);
		GetDlgItem(IDC_SELPEG_COLOUR)->EnableWindow(TRUE);
	}

	if(n < 0)
		return;

	double c = m_wndListColor.m_pColorTable->GetValue(n);

	double dMinZ, dMaxZ;
	m_ColorTable.GetValueRange(dMinZ, dMaxZ);

	c = (c-dMinZ)/(dMaxZ-dMinZ);

	if( m_ColorTable.RemoveColorFromTable(c) )
		m_wndListColor.DeleteString(n);

	int nCount = m_wndListColor.GetCount();
	if( nCount<0 )
	{
		GetDlgItem(IDC_DEL_PEG)->EnableWindow(FALSE);
		GetDlgItem(IDC_SELPEG_POSITION)->EnableWindow(FALSE);
		GetDlgItem(IDC_SELPEG_COLOUR)->EnableWindow(FALSE);
	}
	else
	{
		if( n>= nCount)
			n = nCount-1;
		m_wndListColor.SetCurSel(n);
	}

	CRect rect;
	GetDlgItem(IDC_GRADIENT)->GetClientRect(rect);
	GetDlgItem(IDC_GRADIENT)->ClientToScreen(rect);
	ScreenToClient(rect);
	InvalidateRect(rect);
}

void CDlgColorTableSet::OnAddPeg() 
{
	// TODO: Add your control notification handler code here
	if( !UpdateData(TRUE) )
		return;

	m_wndListColor.ResetContent();
	COLORREF newClr = m_wndNewColor.GetColor();
	m_ColorTable.AddColor(m_NewPegPosition, newClr);

	m_wndListColor.m_pColorTable = &m_ColorTable;

	int nCount = m_ColorTable.GetColorTableCount();
	for (int i=0;i<nCount;i++)
	{
		double v = m_ColorTable.GetValue(i);

		CString str;
		str.Format("%lf",v);
		m_wndListColor.AddString(str);
	}

	CRect rect;
	GetDlgItem(IDC_GRADIENT)->GetClientRect(rect);
	GetDlgItem(IDC_GRADIENT)->ClientToScreen(rect);
	ScreenToClient(rect);
	InvalidateRect(rect);
}


void CDlgColorTableSet::OnSelpegColour() 
{
	// TODO: Add your control notification handler code here

	UpdateData(TRUE);
	int n = m_wndListColor.GetCurSel();
	COLORREF newClr = m_wndSelColor.GetColor();
	m_ColorTable.SetColor(n,newClr);

	CRect rect;
	GetDlgItem(IDC_GRADIENT)->GetClientRect(rect);
	GetDlgItem(IDC_GRADIENT)->ClientToScreen(rect);
	ScreenToClient(rect);
	InvalidateRect(rect);

	m_wndListColor.RedrawWindow();
}

void CDlgColorTableSet::OnPaint() 
{
	CPaintDC dc(this);

	CRect rect;
	CWnd *pWnd = GetDlgItem(IDC_GRADIENT);
	if( pWnd->m_hWnd )
	{
		GetDlgItem(IDC_GRADIENT)->GetClientRect(rect);
		GetDlgItem(IDC_GRADIENT)->ClientToScreen(rect);
		ScreenToClient(rect);

		int top = rect.top;
		rect.top = rect.bottom;
		rect.bottom = top;

		m_ColorTable.DrawColorSpectrum(&dc, rect,FALSE);
	}	
}

void CDlgColorTableSet::OnSelchangeListColor() 
{
	// TODO: Add your control notification handler code here

	int n = m_wndListColor.GetCurSel();

	if(n < 0)
	{
		GetDlgItem(IDC_DEL_PEG)->EnableWindow(FALSE);
		GetDlgItem(IDC_SELPEG_POSITION)->EnableWindow(FALSE);
		GetDlgItem(IDC_SELPEG_COLOUR)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_DEL_PEG)->EnableWindow(TRUE);
		GetDlgItem(IDC_SELPEG_POSITION)->EnableWindow(TRUE);
		GetDlgItem(IDC_SELPEG_COLOUR)->EnableWindow(TRUE);
	}

	if(n < 0)
		return;

	double c = m_wndListColor.m_pColorTable->GetValue(n);

	m_SelPegPosition = c;

	m_wndSelColor.SetColor ( m_ColorTable.LookUpColor(m_SelPegPosition) );

	UpdateData(FALSE);
}
