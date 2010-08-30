// DlgAddLane.cpp : implementation file
//

#include "stdafx.h"
#include "DlgAddLane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgAddLane dialog

CDlgAddLane::CDlgAddLane(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAddLane::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAddLane)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pImageList = NULL;
	m_iSelected = 1;
}

CDlgAddLane::~CDlgAddLane()
{
	if( m_pImageList != NULL)
		delete m_pImageList;
}

void CDlgAddLane::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAddLane)
	DDX_Control(pDX, IDC_LIST_LANE, m_listLane);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAddLane, CDialog)
	//{{AFX_MSG_MAP(CDlgAddLane)
	ON_NOTIFY(NM_CLICK, IDC_LIST_LANE, OnClickListLane)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAddLane message handlers

BOOL CDlgAddLane::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_pImageList = new CImageList();
	m_pImageList->Create(32, 32, TRUE,  15, 15);
	m_pImageList->Add(AfxGetApp()->LoadIcon(IDI_DEPTH));
	m_pImageList->Add(AfxGetApp()->LoadIcon(IDI_LOG));
	m_pImageList->Add(AfxGetApp()->LoadIcon(IDI_INTERLAYER));


	CString laneName[3] = {
		_T("井深"),
		_T("测井曲线"),
		_T("夹层标记"),
	};

	LV_ITEM         lvitem;	
	m_listLane.SetImageList(m_pImageList, LVSIL_NORMAL);
	
	m_listLane.InsertColumn(0, "泳道项目");
	
	for (int iItem = 0; iItem < 3; iItem++)  // insert the items and subitems into the list view.
	{
		lvitem.mask = LVIF_TEXT | LVIF_IMAGE;
		
		lvitem.iItem = iItem;
		lvitem.iSubItem = 0;
		lvitem.pszText = (LPSTR)(LPCSTR)laneName[iItem];
		lvitem.iImage = iItem;
		m_listLane.InsertItem(&lvitem);
	}
		
	m_listLane.SetItem( 1, 0, LVIF_STATE,NULL, 0, LVNI_SELECTED | LVNI_FOCUSED,                         LVNI_SELECTED | LVNI_FOCUSED,NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAddLane::OnClickListLane(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if( pNMListView->iItem != -1 )
	{
		m_iSelected = pNMListView->iItem;
		GetDlgItem(IDOK)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDOK)->EnableWindow(FALSE);
	}
	*pResult = 0;
}
