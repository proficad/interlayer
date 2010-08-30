// LogTemplPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"

#include "LogTemplPage.h"
#include "LogTemplSheet.h"
#include "DlgSetWidth.h"

#define READMAXROWS	51

extern AFX_EXTENSION_MODULE DispLogCurveDLL;
extern int TokenString(LPCTSTR lpszString, 
					   LPCTSTR lpszDelimiters,
					   CStringArray& saTokens,
					   int nMaxTokens = 0,
					   BOOL bTrimToken = TRUE,
					   BOOL bEnableEscapedChars = TRUE,
					   BOOL bEnableDoubleQuote = FALSE,
					   BOOL bReturnEmptyToken = FALSE);
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CLogTemplPage0, CPropertyPage)
IMPLEMENT_DYNCREATE(CLogTemplPage1, CPropertyPage)
IMPLEMENT_DYNCREATE(CLogTemplPage2, CPropertyPage)

/////////////////////////////////////////////////////////////////////////////
// CLogTemplPage0 property page

CLogTemplPage0::CLogTemplPage0() : CPropertyPage(CLogTemplPage0::IDD)
{
	//{{AFX_DATA_INIT(CLogTemplPage0)
	m_option = 0;	
	m_iFirstRow = 1;
	m_uTitleRow = 1;	
	m_nDepthCol = 1;
	//}}AFX_DATA_INIT
}

CLogTemplPage0::~CLogTemplPage0()
{
}

void CLogTemplPage0::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogTemplPage0)
	DDX_Control(pDX, IDC_LIST_DATA, m_list);
	DDX_Radio(pDX, IDC_RADIO1, m_option);
	DDX_Text(pDX, IDC_EDIT_PATH, m_strPath);
	DDX_Text(pDX, IDC_EDIT_FIRSTROW, m_iFirstRow);
	DDV_MinMaxUInt(pDX, m_iFirstRow, 1, 200);
	DDX_Text(pDX, IDC_EDIT_TITLEROW, m_uTitleRow);
	DDV_MinMaxUInt(pDX, m_uTitleRow, 1, 100);	
	DDX_Text(pDX, IDC_EDIT_DEPTH_COL, m_nDepthCol);
	DDV_MinMaxUInt(pDX, m_nDepthCol, 1, 100);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLogTemplPage0, CPropertyPage)
//{{AFX_MSG_MAP(CLogTemplPage0)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogTemplPage1 property page

CLogTemplPage1::CLogTemplPage1() : CPropertyPage(CLogTemplPage1::IDD)
{
	//{{AFX_DATA_INIT(CLogTemplPage1)
	m_iChar = 2;
	m_char = _T("|");
	m_path = _T("");
	m_strTitleWidth = _T("10");
	//}}AFX_DATA_INIT
}

CLogTemplPage1::~CLogTemplPage1()
{
}

void CLogTemplPage1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogTemplPage1)
	DDX_Radio(pDX, IDC_RADIO1, m_iChar);
	DDX_Text(pDX, IDC_EDIT_CHAR, m_char);
	DDV_MaxChars(pDX, m_char, 1);
	DDX_Text(pDX, IDC_EDIT_PATH, m_path);
	DDX_Text(pDX, IDC_EDIT_TITLEWIDTH, m_strTitleWidth);
	//}}AFX_DATA_MAP
//	DDX_GridControl(pDX, IDC_GRID,m_Grid);
}

BEGIN_MESSAGE_MAP(CLogTemplPage1, CPropertyPage)
	//{{AFX_MSG_MAP(CLogTemplPage1)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio)
	ON_EN_CHANGE(IDC_EDIT_CHAR, OnChangeEditChar)
	ON_BN_CLICKED(IDC_BTN_APPLY, OnBtnApply)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio)
	ON_BN_CLICKED(IDC_RADIO3, OnRadio)
	ON_BN_CLICKED(IDC_RADIO4, OnRadio)
	ON_BN_CLICKED(IDC_RADIO5, OnRadio)
	ON_BN_CLICKED(IDC_BTN_WIDTH, OnBtnWidth)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogTemplPage2 property page

CLogTemplPage2::CLogTemplPage2() : CPropertyPage(CLogTemplPage2::IDD)
{
	//{{AFX_DATA_INIT(CLogTemplPage2)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_iTitleRow = 0;		// 标题行
	m_iDataRow = 0;			// 数据起始行
	m_strSeq = ",";		// 分隔符
	m_strTitleWidth = "";// 分隔标题行的宽度
}

CLogTemplPage2::~CLogTemplPage2()
{
}

void CLogTemplPage2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogTemplPage2)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLogTemplPage2, CPropertyPage)
	//{{AFX_MSG_MAP(CLogTemplPage2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CLogTemplPage1::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	CLogTemplSheet *pSheet = (CLogTemplSheet*)GetParent();
	
	CLogTemplPage0 *pPage0 = (CLogTemplPage0 *)pSheet->GetPage(0);
	
	m_path = pPage0->m_strPath;
	
	CWnd *pWnd = GetDlgItem(IDC_GRID);
	CRect rect;
    pWnd->GetClientRect(rect);
	m_Grid.Create(rect, pWnd, 1);
	m_Grid.SetColumnCount(2);
	m_Grid.SetRowCount(3);
	m_Grid.SetFixedColumnCount(1);
	m_Grid.SetFixedRowCount(1);
	m_Grid.SetItemText(1, 0, "->");
		
	m_Grid.SetItemFormat(1, 0, DT_RIGHT);
	m_Grid.SetItemFgColour(1, 0, RGB(0,128,0));
	
	this->UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CLogTemplPage1::OnRadio() 
{
	this->UpdateData();
	
	this->UpdateData(FALSE);
	CWaitCursor wait;
	((CPropertySheet*)GetParent())->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
	
	CLogTemplSheet *pSheet = (CLogTemplSheet*)GetParent();
	
	CLogTemplPage0 *pPage0 = (CLogTemplPage0 *)pSheet->GetPage(0);
	
	if( pPage0->m_option == 0 )	
	{
		int nRows = pPage0->m_list.GetItemCount();
				
		if( nRows > READMAXROWS )
			 nRows = READMAXROWS;

		char seps[5] = " ,	;";
		char sep[2] = "\0";
		if (m_iChar < 4)
			sep[0] = seps[m_iChar];
		else
			sep[0] = m_char[0];


		CString tmp;
		tmp = pPage0->m_list.GetItemText(pPage0->m_uTitleRow-1,1);

		CStringArray arr;
		
		TokenString(tmp,sep,arr);
		
		int nCols = arr.GetSize() + 1;
		
		m_Grid.SetColumnCount(nCols);
		m_Grid.SetRowCount(nRows-pPage0->m_iFirstRow + 3);
		nRows = nRows-pPage0->m_iFirstRow + 3;
				
		for (int i = 1; i < nRows; i++) 
		{		
			if( pPage0->m_nDepthCol < nCols-1)
				m_Grid.SetItemFgColour(i, pPage0->m_nDepthCol, RGB(0,0,255));
			if( i == 1)
			{
				m_Grid.SetItemText(i, 0, "->");
				tmp = pPage0->m_list.GetItemText(pPage0->m_uTitleRow-1,1);
			}
			else
			{
				tmp.Format("%d",i-1);
				m_Grid.SetItemText(i, 0, tmp);
				tmp = pPage0->m_list.GetItemText(i - 3 + pPage0->m_iFirstRow, 1);
			}

			CStringArray arr;
			
			TokenString(tmp,sep,arr);
			
			int nCount = arr.GetSize();
			if( nCount < 1 )
				continue;

			for(int index = 0; index < nCount; index ++ )
			{
				CString temp = arr[index];
				temp.TrimLeft();
				temp.TrimRight();
				if( i==1) 
					m_Grid.SetItemFgColour(i, index+1, RGB(0,128,0));

				m_Grid.SetItemText(i, index+1, temp);
				if( i>1)
					m_Grid.SetItemState(i, index+1, m_Grid.GetItemState(i, index+1) | GVNI_READONLY);
			}			
		}
		m_Grid.AutoSizeColumns();
	}
	else
	{
		int nRows = pPage0->m_list.GetItemCount();				

		if( nRows > READMAXROWS )
			 nRows = READMAXROWS;

		m_arTitleWidth.RemoveAll();
		
		CStringArray arr;
		
		TokenString(m_strTitleWidth,",",arr);// 每列宽度
		
		int nCount = arr.GetSize(); 
		int i;
		for ( i=0;i<nCount;i++)
		{
			m_arTitleWidth.Add(atoi(arr[i]));
		}
		
		CString tmp;
		CString strTemp;
		
		tmp = pPage0->m_list.GetItemText(pPage0->m_uTitleRow-1, 1);
		int nLen = tmp.GetLength();
		int nTitle = m_arTitleWidth.GetSize();
		
		int nCols = 1;
		
		i=0;
		int j=0;
		
		while(i<nLen) //计算标题行的列数
		{
			if( j<nTitle )
			{
				i += m_arTitleWidth[j];
				j++;
			}
			else
				i += m_arTitleWidth[nTitle-1];
			
			nCols++;
		}
				
		m_Grid.SetColumnCount(2);
		m_Grid.SetRowCount(3);
		m_Grid.SetFixedColumnCount(1);
		m_Grid.SetFixedRowCount(1);
		m_Grid.SetItemText(1, 0, "->");
		
		m_Grid.SetItemFormat(1, 0, DT_RIGHT);
		m_Grid.SetItemFgColour(1, 0, RGB(0,128,0));
		m_Grid.SetColumnCount(nCols);
		m_Grid.SetRowCount(nRows-pPage0->m_iFirstRow + 3);
		nRows = nRows-pPage0->m_iFirstRow + 3; //行数
		
		
		int first=0, count=0;
		for (i = 1; i < nRows; i++) 
		{		
			if( pPage0->m_nDepthCol < nCols-1)
			m_Grid.SetItemFgColour(i, pPage0->m_nDepthCol, RGB(0,0,255));
			if( i == 1)
			{
				m_Grid.SetItemText(i, 0, "->");
				tmp = pPage0->m_list.GetItemText(pPage0->m_uTitleRow-1,1);
				for( j= 1;j<nCols;j++)
				{			
					m_Grid.SetItemFgColour(i, j, RGB(0,128,0));
					if( j==1)
					{
						first = 0;
						count = m_arTitleWidth[j-1];
					}
					else if(j <= nTitle)
					{
						first += m_arTitleWidth[j-2];
						count = m_arTitleWidth[j-1];
					}
					else
					{
						first += m_arTitleWidth[nTitle-1];
						count = m_arTitleWidth[nTitle-1];
					}

					if( first >= tmp.GetLength())
						strTemp = "";
					else
						strTemp = tmp.Mid(first,count);
					
					m_Grid.SetItemText(i, j, strTemp);
					if( i>1) m_Grid.SetItemState(i, j, m_Grid.GetItemState(i, j) | GVNI_READONLY);
					
				}
			}
			else
			{
				tmp.Format("%d",i-1);
				m_Grid.SetItemText(i, 0, tmp);
				tmp = pPage0->m_list.GetItemText(i - 3 + pPage0->m_iFirstRow, 1);
				
				for( j= 1;j<nCols;j++)
				{	
					if( pPage0->m_nDepthCol < nCols-1)
						m_Grid.SetItemFgColour(i, pPage0->m_nDepthCol, RGB(0,0,255));
					if( j==1)
					{
						first = 0;
						count = m_arTitleWidth[j-1];
					}
					else if(j <= nTitle)
					{
						first += m_arTitleWidth[j-1];
						count = m_arTitleWidth[j-1];
					}
					else
					{
						first += m_arTitleWidth[nTitle-1];
						count = m_arTitleWidth[nTitle-1];
					}
					
					if( first >= tmp.GetLength())
						strTemp = "";
					else
						strTemp = tmp.Mid(first,count);
					
					m_Grid.SetItemText(i, j, strTemp);
					if( i>1) m_Grid.SetItemState(i, j, m_Grid.GetItemState(i, j) | GVNI_READONLY);	
				}
			}			
		}
		m_Grid.AutoSizeColumns();
	}
	
	this->UpdateData(FALSE);
}

void CLogTemplPage1::OnChangeEditChar() 
{
	if( m_iChar == 4)
		OnRadio();
}

BOOL CLogTemplPage0::OnSetActive() 
{
	((CPropertySheet*)GetParent())->SetWizardButtons(PSWIZB_NEXT);
	
	return CPropertyPage::OnSetActive();
}


BOOL CLogTemplPage1::OnSetActive() 
{	
	this->UpdateData(FALSE);
	CWaitCursor wait;
	((CPropertySheet*)GetParent())->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
	
	CLogTemplSheet *pSheet = (CLogTemplSheet*)GetParent();
	
	CLogTemplPage0 *pPage0 = (CLogTemplPage0 *)pSheet->GetPage(0);
	
	if( pPage0->m_option == 0 )	
	{
		GetDlgItem(IDC_STATIC_1)->SetWindowText("分隔符");
		
		GetDlgItem(IDC_EDIT_TITLEWIDTH)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_APPLY)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_WIDTH)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO1)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO2)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO3)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO4)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO5)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_CHAR)->EnableWindow(TRUE);

		int nRows = pPage0->m_list.GetItemCount();
		
		 if( nRows > READMAXROWS )
			 nRows = READMAXROWS;

		char seps[5] = " ,	;";
		char sep[2] = "\0";
		if (m_iChar < 4)
			sep[0] = seps[m_iChar];
		else
			sep[0] = m_char[0];	
		
		CString tmp;
		tmp = pPage0->m_list.GetItemText(pPage0->m_uTitleRow-1,1);
		
		CStringArray arr;
		
		TokenString(tmp,sep,arr);
		
		int nCols = arr.GetSize() + 1;
		
		m_Grid.SetColumnCount(nCols);
		m_Grid.SetRowCount(nRows-pPage0->m_iFirstRow + 3);
		nRows = nRows-pPage0->m_iFirstRow + 3;
		
		
		for (int i = 1; i < nRows; i++) 
		{	
			if( pPage0->m_nDepthCol < nCols-1)
			m_Grid.SetItemFgColour(i, pPage0->m_nDepthCol, RGB(0,0,255));
			
			if( i == 1)
			{
				m_Grid.SetItemText(i, 0, "->");
				tmp = pPage0->m_list.GetItemText(pPage0->m_uTitleRow-1,1);
			}
			else
			{
				tmp.Format("%d",i-1);
				m_Grid.SetItemText(i, 0, tmp);
				tmp = pPage0->m_list.GetItemText(i - 3 + pPage0->m_iFirstRow, 1);
			}
			CStringArray arr;
			
			TokenString(tmp,sep,arr);
			
			int nCount = arr.GetSize();
			if( nCount < 1 )
				continue;
			
			for(int index = 0; index < nCount; index ++ )
			{
				CString temp = arr[index];
				temp.TrimLeft();
				temp.TrimRight();
				if( i==1) 
					m_Grid.SetItemFgColour(i, index+1, RGB(0,128,0));
				
				m_Grid.SetItemText(i, index+1, temp);
				if( i>1)
					m_Grid.SetItemState(i, index+1, m_Grid.GetItemState(i, index+1) | GVNI_READONLY);
			}	
			
		}
		m_Grid.AutoSizeColumns();
	}
	else
	{		
		GetDlgItem(IDC_STATIC_1)->SetWindowText("指定宽度和分隔符");
		GetDlgItem(IDC_RADIO1)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO2)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO3)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO4)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO5)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_CHAR)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_TITLEWIDTH)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_APPLY)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_WIDTH)->EnableWindow(TRUE);

		m_iChar = 0;
		this->UpdateData(FALSE);

		int nRows = pPage0->m_list.GetItemCount();

		if( nRows > READMAXROWS )
			 nRows = READMAXROWS;
		
		m_arTitleWidth.RemoveAll();
		

		CStringArray arr;
		
		TokenString(m_strTitleWidth,",",arr);// 每列宽度

		int nCount = arr.GetSize(); 
		int i;
		for ( i=0;i<nCount;i++)
		{
			m_arTitleWidth.Add(atoi(arr[i]));
		}
		
		CString tmp, strTemp;
		
		tmp = pPage0->m_list.GetItemText(pPage0->m_uTitleRow-1, 1);
		int nLen = tmp.GetLength();
		int nTitle = m_arTitleWidth.GetSize();
		
		int nCols = 1;
		
		i=0;
		int j=0;
		
		while(i<nLen) //计算标题行的列数
		{
			if( j<nTitle )
			{
				i += m_arTitleWidth[j];
				j++;
			}
			else
				i += m_arTitleWidth[nTitle-1];
			
			nCols++;
		}
		
		m_Grid.SetColumnCount(nCols);
		m_Grid.SetRowCount(nRows-pPage0->m_iFirstRow + 3);
		nRows = nRows-pPage0->m_iFirstRow + 3; //行数
		m_Grid.SetItemFormat(1, 0, DT_RIGHT);
		
		int first=0, count=0;
		for (i = 1; i < nRows; i++) 
		{	
			if( pPage0->m_nDepthCol < nCols-1)
			m_Grid.SetItemFgColour(i, pPage0->m_nDepthCol, RGB(0,0,255));
			if( i == 1)
			{
				m_Grid.SetItemText(i, 0, "->");				

				tmp = pPage0->m_list.GetItemText(pPage0->m_uTitleRow-1,1);
				for( j= 1;j<nCols;j++)
				{	
					m_Grid.SetItemFgColour(i, j, RGB(0,128,0));
					if( j==1)
					{
						first = 0;
						count = m_arTitleWidth[j-1];
					}
					else if(j <= nTitle)
					{
						first += m_arTitleWidth[j-1];
						count = m_arTitleWidth[j-1];
					}
					else
					{
						first += m_arTitleWidth[nTitle-1];
						count = m_arTitleWidth[nTitle-1];
					}

					if( first >= tmp.GetLength())
						strTemp = "";
					else
						strTemp = tmp.Mid(first,count);
					
					m_Grid.SetItemText(i, j, strTemp);
					if( i>1) m_Grid.SetItemState(i, j, m_Grid.GetItemState(i, j) | GVNI_READONLY);					
				}
			}
			else
			{
				tmp.Format("%d",i-1);
				m_Grid.SetItemText(i, 0, tmp);
				tmp = pPage0->m_list.GetItemText(i - 3 + pPage0->m_iFirstRow, 1);

				for( j= 1;j<nCols;j++)
				{	
					if( pPage0->m_nDepthCol < nCols-1)
						m_Grid.SetItemFgColour(i, pPage0->m_nDepthCol, RGB(0,0,255));
					if( j==1)
					{
						first = 0;
						count = m_arTitleWidth[j-1];
					}
					else if(j <= nTitle)
					{
						first += m_arTitleWidth[j-1];
						count = m_arTitleWidth[j-1];
					}
					else
					{
						first += m_arTitleWidth[nTitle-1];
						count = m_arTitleWidth[nTitle-1];
					}
					
					if( first >= tmp.GetLength())
						strTemp = "";
					else
						strTemp = tmp.Mid(first,count);
					
					m_Grid.SetItemText(i, j, strTemp);
					if( i>1) m_Grid.SetItemState(i, j, m_Grid.GetItemState(i, j) | GVNI_READONLY);					
				}
			}			
		}
		m_Grid.AutoSizeColumns();
	}
	
	this->UpdateData(FALSE);
	return CPropertyPage::OnSetActive();
}

BOOL CLogTemplPage2::OnSetActive() 
{	
	
	((CPropertySheet*)GetParent())->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
	
	CLogTemplSheet *pSheet = (CLogTemplSheet*)GetParent();
	CLogTemplPage0 *pPage0 = (CLogTemplPage0 *)pSheet->GetPage(0);	
	
	CLogTemplPage1 *pPage1 = (CLogTemplPage1 *)pSheet->GetPage(1);

	int nCols = pPage1->m_Grid.GetColumnCount();
	
	CString tmp;
	BOOL bExist = FALSE;

	int i;

	for ( i=1; i<nCols; i++)
	{
		tmp = pPage1->m_Grid.GetItemText(1,i);
		if(tmp.IsEmpty())
		{
			((CPropertySheet*)GetParent())->SetWizardButtons(PSWIZB_BACK);
			break;
		}

		if(tmp == "DEPTH")
			bExist = TRUE;			
	}

	if( !bExist ) //没有井深
		((CPropertySheet*)GetParent())->SetWizardButtons(PSWIZB_BACK);
	
	int nRows = pPage1->m_Grid.GetRowCount();
	
	int j;
	
	int n=1;
	for (j=1;j<nCols;j++)
	{
		tmp = pPage1->m_Grid.GetItemText(1,j);
		if( !tmp.IsEmpty() )
			n++;
	}
	
	m_Grid.SetRowCount(nRows - pPage0->m_iFirstRow);
	m_Grid.SetColumnCount(n);
	
	n = 1;
	for (j=1;j<nCols;j++)
	{
		tmp = pPage1->m_Grid.GetItemText(1,j);
		if( !tmp.IsEmpty() )
		{
			m_Grid.SetItemText(0,n,tmp);
			n++;
		}
	}
	
	int row, col;
	row = 1; 
	for (i = pPage0->m_iFirstRow+1; i<nRows; i++)
	{
		col=1;
		for (j=1; j<nCols; j++)
		{	
			tmp = pPage1->m_Grid.GetItemText(1,j);
			if( !tmp.IsEmpty())
			{
				tmp = pPage1->m_Grid.GetItemText(i,j);
				m_Grid.SetItemText(row, col, tmp);
				col++;
			}
		}
		row++;
	}
	
	
	m_Grid.EnableTitleTips(FALSE);
	m_Grid.EnableSelection(FALSE);
	m_Grid.AutoSizeColumns();
	
	return CPropertyPage::OnSetActive();
}

BOOL CLogTemplPage0::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	CLogTemplSheet *pSheet = (CLogTemplSheet*)GetParent();
	
	m_strPath = pSheet->m_strPath;	
	
	TRY
	{
		CWaitCursor wait;

		CStdioFile file(m_strPath,	CFile::modeRead | CFile::typeText);
		CString tmp1, tmp2;
//		m_list.SetExtendedStyle(LVS_EX_GRIDLINES);
		int i = 0;
		LV_COLUMN lvc;
		lvc.mask = LVCF_TEXT| LVCF_SUBITEM ;
		lvc.pszText = "行";
		lvc.iSubItem = 0;
		m_list.InsertColumn(0,&lvc);

		lvc.mask = LVCF_TEXT | LVCF_SUBITEM|LVCF_FMT;
		lvc.fmt = LVCFMT_LEFT;
		lvc.pszText = "内容";
		lvc.iSubItem = 1;
		m_list.InsertColumn(1,&lvc);
		
		while (file.ReadString(tmp1) && i < READMAXROWS)
		{
			tmp2.Format("%5d >", i+1);
			LV_ITEM lvItem;
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = i;
			lvItem.iSubItem = 0;
			lvItem.pszText = (LPTSTR)(LPCSTR)tmp2;			
			m_list.InsertItem(&lvItem);
			
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = i;
			lvItem.iSubItem = 1;
			lvItem.pszText = (LPTSTR)(LPCSTR)tmp1;
			m_list.SetItem(&lvItem);
			i++;
		}
		file.Close();
		
		m_list.SetColumnWidth(0,LVSCW_AUTOSIZE);
		m_list.SetColumnWidth(1,LVSCW_AUTOSIZE);
	}
	CATCH(CFileException, e)
	{
		AfxMessageBox("读数据错误！");
		return FALSE;
	}
	END_CATCH
		
	this->UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CLogTemplPage2::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	CLogTemplSheet *pSheet = (CLogTemplSheet*)GetParent();
	CLogTemplPage0 *pPage0 = (CLogTemplPage0 *)pSheet->GetPage(0);

	CWnd *pWnd = GetDlgItem(IDC_STATIC_1);
	
	CRect rect;
    pWnd->GetClientRect(rect);
	m_Grid.Create(rect, pWnd, 1);
	m_Grid.SetColumnCount(2);
	m_Grid.SetRowCount(2);
	m_Grid.SetFixedColumnCount(1);
	m_Grid.SetFixedRowCount(1);
	m_Grid.SetItemText(0, 1, "列1");
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}



BOOL CLogTemplPage2::OnWizardFinish() 
{
	// TODO: Add your specialized code here and/or call the base class
	CLogTemplSheet *pSheet = (CLogTemplSheet*)GetParent();
	CLogTemplPage0 *pPage0 = (CLogTemplPage0 *)pSheet->GetPage(0);
	CLogTemplPage1 *pPage1 = (CLogTemplPage1 *)pSheet->GetPage(1);

	CString tmp;
		
	if( pPage0->m_option == 0 )
	{
		m_iTitleRow = pPage0->m_uTitleRow;
		m_iDataRow = pPage0->m_iFirstRow;
		char seps[5] = " ,	;";
		if (pPage1->m_iChar < 4)
			m_strSeq = seps[pPage1->m_iChar];
		else
			m_strSeq = pPage1->m_char;		
	}
	else
	{
		m_iTitleRow = pPage0->m_uTitleRow;
		m_iDataRow = pPage0->m_iFirstRow;
		
		m_strTitleWidth = pPage1->m_strTitleWidth;
		char seps[5] = " ,	;";
		if (pPage1->m_iChar < 4)
			m_strSeq = seps[pPage1->m_iChar];
		else
			m_strSeq = pPage1->m_char;
	}

	return CPropertyPage::OnWizardFinish();
}

void CLogTemplPage1::OnBtnApply() 
{
	OnRadio();	
}

BOOL CLogTemplPage1::OnWizardFinish() 
{
	CWaitCursor wait;

	CLogTemplSheet *pSheet = (CLogTemplSheet*)GetParent();
	CLogTemplPage0 *pPage0 = (CLogTemplPage0 *)pSheet->GetPage(0);

	CString m_strPath = pSheet->m_strPath;

	CString tmp;
		
	if( pPage0->m_option == 0 )
	{
		m_iTitleRow = pPage0->m_uTitleRow;
		m_iDataRow = pPage0->m_iFirstRow;
	}
	else
	{
		m_iTitleRow = pPage0->m_uTitleRow;
		m_iDataRow = pPage0->m_iFirstRow;
	}

	return CPropertyPage::OnWizardFinish();
}

void CLogTemplPage1::OnBtnWidth() 
{
	UpdateData();
	CLogTemplSheet *pSheet = (CLogTemplSheet*)GetParent();
	CLogTemplPage0 *pPage0 = (CLogTemplPage0 *)pSheet->GetPage(0);
	
	HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(DispLogCurveDLL.hModule);	
						
	CDlgSetWidth dlg;
	dlg.m_strTitleWidth = m_strTitleWidth;
	dlg.pList = &pPage0->m_list;
	dlg.m_uTitleRow = pPage0->m_uTitleRow;
	dlg.m_iFirstRow = pPage0->m_iFirstRow;
	if( dlg.DoModal() == IDOK )
	{
		m_strTitleWidth = dlg.m_strTitleWidth;
		this->UpdateData(FALSE);
	}
	AfxSetResourceHandle(hInstOld);	
}
