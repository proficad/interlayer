// WellPathView.cpp : 实现文件
//

#include "stdafx.h"
#include "Interlayer.h"
#include "WellPathView.h"
#include "WellPathDoc.h"

// CWellPathView

IMPLEMENT_DYNCREATE(CWellPathView, CResizableFormView)

CWellPathView::CWellPathView()
: CResizableFormView(CWellPathView::IDD)
{

}

CWellPathView::~CWellPathView()
{
}

void CWellPathView::DoDataExchange(CDataExchange* pDX)
{
	CResizableFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_INFO, m_strInfo);	
	DDX_GridControl(pDX, IDC_GRID, m_wndGrid);
}

BEGIN_MESSAGE_MAP(CWellPathView, CResizableFormView)
	ON_COMMAND(ID_EDIT_COPY, &CWellPathView::OnEditCopy)
	ON_BN_CLICKED(IDC_BTN_ADD_ROW, OnBtnAddRow)
	ON_BN_CLICKED(IDC_BTN_DEL_ROW, OnBtnDelRow)
	ON_BN_CLICKED(IDC_BTN_IMPORT, OnBtnImport)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)	
	ON_NOTIFY(NM_CLICK, IDC_GRID, OnGridClick)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID, OnGridEndEdit)
	ON_COMMAND(IDC_BTN_EXPORT, OnBtnExport)
END_MESSAGE_MAP()


// CWellPathView 诊断

#ifdef _DEBUG
void CWellPathView::AssertValid() const
{
	CResizableFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CWellPathView::Dump(CDumpContext& dc) const
{
	CResizableFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CWellPathView 消息处理程序

void CWellPathView::OnInitialUpdate()
{
	CResizableFormView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
	AddAnchor(IDC_GRID,TOP_LEFT,BOTTOM_RIGHT);

	CWellPathDoc *pDoc = GetDocument();
	//pDoc->m_bAutoDelete = TRUE;

	m_wndGrid.SetRowCount(pDoc->m_nRows+1);
	m_wndGrid.SetColumnCount(pDoc->m_nCols+1);
	m_wndGrid.SetPrintFixedRow(TRUE);

	m_wndGrid.SetFixedColumnCount(1);
	m_wndGrid.SetFixedRowCount(1);

	for (int j=1; j<pDoc->m_nCols+1; j++)
	{
		m_wndGrid.SetItemText(0,j,pDoc->m_arWellPath[j-1]);
	}

	for ( int i=1; i<pDoc->m_nRows+1; i++)
	{
		for (int j=1; j<pDoc->m_nCols+1; j++)
		{
			m_wndGrid.SetItemText(i,j,pDoc->m_arWellPath[(i)*pDoc->m_nCols+j-1]);
		}
	}
	m_wndGrid.SetColumnWidth(0,15);

	if( pDoc->m_nRows < 2 )
		GetDlgItem(IDC_BTN_DEL_ROW)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_BTN_DEL_ROW)->EnableWindow(TRUE);

	m_wndGrid.EnableTitleTips(FALSE);
	m_wndGrid.SetRowResize(FALSE);
	m_wndGrid.EnableDragAndDrop(FALSE);

	m_wndGrid.SetModified(FALSE);
	m_wndGrid.SetHeaderSort(TRUE);
	m_wndGrid.SetAutoAddRow(TRUE);
}

BOOL CWellPathView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	if( pMsg->message == WM_KEYDOWN )
	{
		if(pMsg->wParam == VK_RETURN
			|| pMsg->wParam == VK_ESCAPE )
		{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return TRUE;                    // DO NOT process further
		}
	}

	return CResizableFormView::PreTranslateMessage(pMsg);
}

void CWellPathView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (m_wndGrid.m_hWnd)
		m_wndGrid.OnBeginPrinting(pDC, pInfo);
}

void CWellPathView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (m_wndGrid.m_hWnd)
		m_wndGrid.OnEndPrinting(pDC, pInfo);
}

BOOL CWellPathView::OnPreparePrinting(CPrintInfo* pInfo)
{

	// TODO:  调用 DoPreparePrinting 以调用“打印”对话框

	return DoPreparePrinting(pInfo);
	//return CResizableFormView::OnPreparePrinting(pInfo);
}

void CWellPathView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (m_wndGrid.m_hWnd)
		m_wndGrid.OnPrint(pDC, pInfo);
}

CWellPathDoc* CWellPathView::GetDocument()
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWellPathDoc)));
	return (CWellPathDoc*)m_pDocument;
}

void CWellPathView::OnGridClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	if( pItem->iColumn > 0 && pItem->iRow>0 )
	{
		m_strInfo.Format(_T("行 %d, 列 %d"), pItem->iRow, pItem->iColumn);
		UpdateData(FALSE);
	}
}

void CWellPathView::OnBtnAddRow() 
{
	// TODO: Add your control notification handler code here
	m_wndGrid.InsertRow(_T(""));
	m_wndGrid.Invalidate();
	GetDlgItem(IDC_BTN_DEL_ROW)->EnableWindow(TRUE);
	CWellPathDoc *pDoc = GetDocument();
	pDoc->SetModifiedFlag(TRUE);
}

void CWellPathView::OnBtnDelRow() 
{
	// TODO: Add your control notification handler code here
	int nRow = m_wndGrid.GetRowCount();
	CArray<int,int> arIndex;
	for (int i=nRow-1;i>0;i--)
	{
		if(m_wndGrid.IsRowSelected(i))
			arIndex.Add(i);
	}

	nRow = arIndex.GetSize();
	for ( int i= 0; i<nRow; i++)
	{
		m_wndGrid.DeleteRow(arIndex[i]);
	}

	m_wndGrid.Invalidate();

	nRow = m_wndGrid.GetRowCount();
	if( nRow < 2 )
		GetDlgItem(IDC_BTN_DEL_ROW)->EnableWindow(FALSE);

	CWellPathDoc *pDoc = GetDocument();
	pDoc->SetModifiedFlag(TRUE);
	pDoc->DoFileSave();
}

void CWellPathView::OnBtnImport() 
{
	// TODO: Add your control notification handler code here
	
}

void CWellPathView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if (point.x == -1 && point.y == -1)
	{
		//keystroke invocation
		CRect rect;
		GetClientRect(rect);
		ClientToScreen(rect); 

		point = rect.TopLeft();
		point.Offset(5, 5);
	}

	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_POPUP_MENU));
	int nCount = menu.GetMenuItemCount();
	CMenu* pPopup = NULL;
	//
	//	GetMenuString
	CString strMenuName;
	for (int i=0;i<nCount;i++)
	{
		menu.GetMenuString(i,strMenuName,MF_BYPOSITION);
		if( strMenuName == "_井基础信息_" )
		{
			pPopup = menu.GetSubMenu(i);
			break;
		}
	}

	if( pPopup == NULL )
		return;

	ASSERT(pPopup != NULL);

	CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;
	pPopupMenu->SetAutoDestroy (TRUE);

	pPopupMenu->Create (this, point.x, point.y, pPopup->m_hMenu, FALSE, TRUE);
}

void CWellPathView::OnEditCut() 
{
	// TODO: Add your command handler code here
	m_wndGrid.OnEditCut();
	CWellPathDoc *pDoc = GetDocument();
	pDoc->SetModifiedFlag(TRUE);
}

void CWellPathView::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_wndGrid.GetSelectedCount() > 0);
}

void CWellPathView::OnEditCopy() 
{
	// TODO: Add your command handler code here
	m_wndGrid.OnEditCopy();
}

void CWellPathView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_wndGrid.GetSelectedCount() > 0);
}

void CWellPathView::OnEditPaste() 
{
	// TODO: Add your command handler code here
	m_wndGrid.OnEditPaste();
	CWellPathDoc *pDoc = GetDocument();
	pDoc->SetModifiedFlag(TRUE);
}

void CWellPathView::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	COleDataObject obj;
	pCmdUI->Enable(obj.AttachClipboard() && obj.IsDataAvailable(CF_TEXT)); 
}

void CWellPathView::OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	if( pItem->iColumn != -1 && 
		pItem->iRow != -1)
	{

		CWellPathDoc *pDoc = GetDocument();
		pDoc->SetModifiedFlag(TRUE);
	}
	*pResult = 0;

}
void CWellPathView::OnBtnExport() 
{
	// TODO: Add your command handler code here
	static char szFilter[] = "Microsoft Office Excel 工作薄 (*.xls)|*.xls|逗号分隔符文件 (*.csv)|*.csv|文本文件 (*.txt)|*.txt||"; 
	CString strTitle = GetDocument()->GetTitle();
	CFileDialog dlg(FALSE, "*.xls", strTitle, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,szFilter, NULL);
	CString strFileName;

	if (dlg.DoModal() == IDOK)
		strFileName = dlg.GetPathName();
	else
		return;

	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath_s( strFileName, drive, dir, fname, ext );

	CString strExt = ext;
	strExt.MakeUpper();
	if( strExt == ".XLS" )
	{
		m_wndGrid.SaveToExcelFile(strFileName);
	}
	else if( strExt == ".CSV" )
	{
		m_wndGrid.Save(strFileName,',');
	}
	else
	{
		m_wndGrid.Save(strFileName,'\t');
	}
}
