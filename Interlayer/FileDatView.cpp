// FileDatView.cpp : 实现文件
//

#include "stdafx.h"
#include "Interlayer.h"
#include "FileDatView.h"
#include "FileDatDoc.h"

// CFileDatView

IMPLEMENT_DYNCREATE(CFileDatView, CResizableFormView)

CFileDatView::CFileDatView()
: CResizableFormView(CFileDatView::IDD)
{

}

CFileDatView::~CFileDatView()
{
}

void CFileDatView::DoDataExchange(CDataExchange* pDX)
{
	CResizableFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_INFO, m_strInfo);	
	DDX_GridControl(pDX, IDC_GRID, m_wndGrid);
}

BEGIN_MESSAGE_MAP(CFileDatView, CResizableFormView)
	ON_COMMAND(ID_EDIT_COPY, &CFileDatView::OnEditCopy)
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


// CFileDatView 诊断

#ifdef _DEBUG
void CFileDatView::AssertValid() const
{
	CResizableFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFileDatView::Dump(CDumpContext& dc) const
{
	CResizableFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFileDatView 消息处理程序

void CFileDatView::OnInitialUpdate()
{
	CResizableFormView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
	AddAnchor(IDC_GRID,TOP_LEFT,BOTTOM_RIGHT);

	CFileDatDoc *pDoc = GetDocument();
	//pDoc->m_bAutoDelete = TRUE;

	m_wndGrid.SetRowCount(pDoc->m_nRows+1);
	m_wndGrid.SetColumnCount(pDoc->m_nCols+1);

	m_wndGrid.SetFixedColumnCount(1);
	m_wndGrid.SetFixedRowCount(1);

	for (int j=1; j<pDoc->m_nCols+1; j++)
	{
		m_wndGrid.SetItemText(0,j,pDoc->m_arDat[j-1]);		
	}

	CString strTmp;
	for ( int i=1; i<pDoc->m_nRows+1; i++)
	{
		for (int j=1; j<pDoc->m_nCols+1; j++)
		{
			strTmp = pDoc->m_arDat[(i)*pDoc->m_nCols+j-1];
			if( j== 1 && strTmp == _T("EOM"))
			{
				m_wndGrid.SetItemBkColour(i,j,RGB(200,255,200));
				m_wndGrid.SetItemBkColour(i,j+1,RGB(200,255,200));
				m_wndGrid.SetItemBkColour(i,j+2,RGB(200,255,200));
				m_wndGrid.SetItemBkColour(i,j+3,RGB(200,255,200));
			}

			if( j== 1 && strTmp == _T("Coordinate"))
			{
				m_wndGrid.SetItemBkColour(i,j,RGB(255,235,200));
				m_wndGrid.SetItemBkColour(i,j+1,RGB(255,235,200));
				m_wndGrid.SetItemBkColour(i,j+2,RGB(255,235,200));
				m_wndGrid.SetItemBkColour(i,j+3,RGB(255,235,200));
			}

			if( j== 1 && strTmp == _T("CoordIndex"))
			{
				m_wndGrid.SetItemBkColour(i,j,RGB(235,235,200));
				m_wndGrid.SetItemBkColour(i,j+1,RGB(235,235,200));
				m_wndGrid.SetItemBkColour(i,j+2,RGB(235,235,200));
				m_wndGrid.SetItemBkColour(i,j+3,RGB(235,235,200));
			}

			m_wndGrid.SetItemText(i,j,strTmp);
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

BOOL CFileDatView::PreTranslateMessage(MSG* pMsg)
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

void CFileDatView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (m_wndGrid.m_hWnd)
		m_wndGrid.OnBeginPrinting(pDC, pInfo);
}

void CFileDatView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (m_wndGrid.m_hWnd)
		m_wndGrid.OnEndPrinting(pDC, pInfo);
}

BOOL CFileDatView::OnPreparePrinting(CPrintInfo* pInfo)
{

	// TODO:  调用 DoPreparePrinting 以调用“打印”对话框

	return DoPreparePrinting(pInfo);
	//return CResizableFormView::OnPreparePrinting(pInfo);
}

void CFileDatView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (m_wndGrid.m_hWnd)
		m_wndGrid.OnPrint(pDC, pInfo);
}

CFileDatDoc* CFileDatView::GetDocument()
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFileDatDoc)));
	return (CFileDatDoc*)m_pDocument;
}

void CFileDatView::OnGridClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	if( pItem->iColumn > 0 && pItem->iRow>0 )
	{
		m_strInfo.Format(_T("行 %d, 列 %d"), pItem->iRow, pItem->iColumn);
		UpdateData(FALSE);
	}
}

void CFileDatView::OnBtnAddRow() 
{
	// TODO: Add your control notification handler code here
	m_wndGrid.InsertRow(_T(""));
	m_wndGrid.Invalidate();
	GetDlgItem(IDC_BTN_DEL_ROW)->EnableWindow(TRUE);
	CFileDatDoc *pDoc = GetDocument();
	pDoc->SetModifiedFlag(TRUE);
}

void CFileDatView::OnBtnDelRow() 
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

	CFileDatDoc *pDoc = GetDocument();
	pDoc->SetModifiedFlag(TRUE);
}

void CFileDatView::OnBtnImport() 
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("文本文件 (*.txt)|*.TXT|逗号分隔符文件 (*.csv)|*.csv|MS Excel (*.xls)|*.xls|所有文件 (*.*)|*.*||"), NULL);
	if( dlg.DoModal() == IDOK)
	{

	}
}

void CFileDatView::OnContextMenu(CWnd* pWnd, CPoint point) 
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
		if( strMenuName == "_数据文件_" )
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

void CFileDatView::OnEditCut() 
{
	// TODO: Add your command handler code here
	m_wndGrid.OnEditCut();
	CFileDatDoc *pDoc = GetDocument();
	pDoc->SetModifiedFlag(TRUE);
}

void CFileDatView::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_wndGrid.GetSelectedCount() > 0);
}

void CFileDatView::OnEditCopy() 
{
	// TODO: Add your command handler code here
	m_wndGrid.OnEditCopy();
}

void CFileDatView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_wndGrid.GetSelectedCount() > 0);
}

void CFileDatView::OnEditPaste() 
{
	// TODO: Add your command handler code here
	m_wndGrid.OnEditPaste();
	CFileDatDoc *pDoc = GetDocument();
	pDoc->SetModifiedFlag(TRUE);
}

void CFileDatView::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	COleDataObject obj;
	pCmdUI->Enable(obj.AttachClipboard() && obj.IsDataAvailable(CF_TEXT)); 
}

void CFileDatView::OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	if( pItem->iColumn != -1 && 
		pItem->iRow != -1)
	{

		CFileDatDoc *pDoc = GetDocument();
		pDoc->SetModifiedFlag(TRUE);
	}
	*pResult = 0;

}
void CFileDatView::OnBtnExport() 
{
	// TODO: Add your command handler code here
	CString strFileName;

	CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("文本文件 (*.txt)|*.TXT||"), NULL);

	if (dlg.DoModal() == IDOK)
		strFileName = dlg.GetPathName();
	else
		return;

	CStdioFile file;
	if( !file.Open(strFileName, CFile::modeCreate|CFile::modeWrite|CFile::typeText))
		return;
	int nRows = m_wndGrid.GetRowCount();
	int nCols = m_wndGrid.GetColumnCount();
	for (int i=1;i<nRows;i++)
	{
		for(int j=1;j<nCols;j++)
		{
			CString strTmp = m_wndGrid.GetItemText(i,j);
			file.WriteString(_T("  "));
			file.WriteString(strTmp);
		}
		file.WriteString(_T("\n"));
	}
	file.Close();
}
