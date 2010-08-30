#include "StdAfx.h"
#include "Interlayer.h"
#include "WellLoggingInfoView.h"
#include "WellLoggingInfoDoc.h"
#include "MainFrm.h"
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include "ReadLogFile/ReadLogFile.h"

extern CInterlayerApp theApp;
extern void StatusProgress(int nIndex, int nRange=-1);
extern void StatusSetProgress(int nIndex, int nPos);

IMPLEMENT_DYNCREATE(CWellLoggingInfoView, CResizableFormView)

CWellLoggingInfoView::CWellLoggingInfoView()
: CResizableFormView(CWellLoggingInfoView::IDD)
{

}

CWellLoggingInfoView::~CWellLoggingInfoView()
{
}

void CWellLoggingInfoView::DoDataExchange(CDataExchange* pDX)
{
	CResizableFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_INFO, m_strInfo);	
	DDX_GridControl(pDX, IDC_GRID, m_wndGrid);
}

BEGIN_MESSAGE_MAP(CWellLoggingInfoView, CResizableFormView)
	ON_COMMAND(ID_EDIT_COPY, &CWellLoggingInfoView::OnEditCopy)
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
	ON_BN_CLICKED(IDC_BTN_LOAD, OnBnClickedBtnLoad)
END_MESSAGE_MAP()


// CWellLoggingInfoView 诊断

#ifdef _DEBUG
void CWellLoggingInfoView::AssertValid() const
{
	CResizableFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CWellLoggingInfoView::Dump(CDumpContext& dc) const
{
	CResizableFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CWellLoggingInfoView 消息处理程序

void CWellLoggingInfoView::OnInitialUpdate()
{
	CResizableFormView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
	AddAnchor(IDC_GRID,TOP_LEFT,BOTTOM_RIGHT);

	CWellLoggingInfoDoc *pDoc = GetDocument();
	//pDoc->m_bAutoDelete = TRUE;

	m_wndGrid.SetRowCount(pDoc->m_nRows+1);
	m_wndGrid.SetColumnCount(pDoc->m_nCols+1);

	m_wndGrid.SetFixedColumnCount(1);
	m_wndGrid.SetFixedRowCount(1);

	for (int j=1; j<pDoc->m_nCols+1; j++)
	{
		m_wndGrid.SetItemText(0, j, pDoc->m_arWellLoggingInfo[j-1]);		
	}

	for ( int i=1; i<pDoc->m_nRows+1; i++)
	{
		for (int j=1; j<pDoc->m_nCols+1; j++)
		{
			CString strtmp = pDoc->m_arWellLoggingInfo[(i)*pDoc->m_nCols+j-1];
			m_wndGrid.SetCellType(i, 2, RUNTIME_CLASS(CGridCellEditFile));
			m_wndGrid.SetItemText(i, j, pDoc->m_arWellLoggingInfo[(i)*pDoc->m_nCols+j-1]);			
		}
	}
	m_wndGrid.SetColumnWidth(0,15);

	m_wndGrid.SetColumnWidth(2, 350);

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

BOOL CWellLoggingInfoView::PreTranslateMessage(MSG* pMsg)
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

void CWellLoggingInfoView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (m_wndGrid.m_hWnd)
		m_wndGrid.OnBeginPrinting(pDC, pInfo);
}

void CWellLoggingInfoView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (m_wndGrid.m_hWnd)
		m_wndGrid.OnEndPrinting(pDC, pInfo);
}

BOOL CWellLoggingInfoView::OnPreparePrinting(CPrintInfo* pInfo)
{

	// TODO:  调用 DoPreparePrinting 以调用“打印”对话框

	return DoPreparePrinting(pInfo);
	//return CResizableFormView::OnPreparePrinting(pInfo);
}

void CWellLoggingInfoView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (m_wndGrid.m_hWnd)
		m_wndGrid.OnPrint(pDC, pInfo);
}

CWellLoggingInfoDoc* CWellLoggingInfoView::GetDocument()
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWellLoggingInfoDoc)));
	return (CWellLoggingInfoDoc*)m_pDocument;
}

void CWellLoggingInfoView::OnGridClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	if( pItem->iColumn > 0 && pItem->iRow>0 )
	{
		m_strInfo.Format(_T("行 %d, 列 %d"), pItem->iRow, pItem->iColumn);
		UpdateData(FALSE);
	}
}

void CWellLoggingInfoView::OnBtnAddRow() 
{
	// TODO: Add your control notification handler code here
	m_wndGrid.InsertRow(_T(""));
	m_wndGrid.Invalidate();
	GetDlgItem(IDC_BTN_DEL_ROW)->EnableWindow(TRUE);
	CWellLoggingInfoDoc *pDoc = GetDocument();
	pDoc->SetModifiedFlag(TRUE);
}

void CWellLoggingInfoView::OnBtnDelRow() 
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

	CWellLoggingInfoDoc *pDoc = GetDocument();
	pDoc->SetModifiedFlag(TRUE);
}

void CWellLoggingInfoView::OnBtnImport() 
{
	// TODO: Add your control notification handler code here
	CWellLoggingInfoDoc*pDoc = GetDocument();

	POSITION pos;
	CString strPathName;
	CString strFileName;

	LPTSTR pFileBuffer = NULL;
	enum {enumMaxFileCount = 1024};

	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT, "*.*|*.*||");

	pFileBuffer = new TCHAR[_MAX_PATH * enumMaxFileCount];
	ASSERT(pFileBuffer);
	memset(pFileBuffer, 0, _MAX_PATH * enumMaxFileCount * sizeof(TCHAR));
	dlg.m_ofn.lpstrFilter = _T("所有文件 (*.*)\0*.*\0\0");
	dlg.m_ofn.lpstrFile = pFileBuffer;

	dlg.m_ofn.nMaxFile = _MAX_PATH * enumMaxFileCount;

	if(dlg.DoModal() == IDOK)
	{		
		pos = dlg.GetStartPosition();
		while(pos)
		{
			strPathName = dlg.GetNextPathName(pos);

			CSplitPath path(strPathName);

			CString strTmp = path.GetFileName();

			strTmp.MakeUpper();

			int nRows = m_wndGrid.GetRowCount();

			CString strJH;
			for (int i=1;i<nRows; i++)
			{
				CString strJH = m_wndGrid.GetItemText(i, 1 );
				strJH.MakeUpper();
				if( strTmp == strJH )
				{
					m_wndGrid.SetItemText(i, 2, strPathName);
					pDoc->SetModifiedFlag();
					break;
				}
			}
		}
	}

	delete pFileBuffer;
}

void CWellLoggingInfoView::OnContextMenu(CWnd* pWnd, CPoint point) 
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
		if( strMenuName == "_井测井信息_" )
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

void CWellLoggingInfoView::OnEditCut() 
{
	// TODO: Add your command handler code here
	m_wndGrid.OnEditCut();
	CWellLoggingInfoDoc *pDoc = GetDocument();
	pDoc->SetModifiedFlag(TRUE);
}

void CWellLoggingInfoView::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_wndGrid.GetSelectedCount() > 0);
}

void CWellLoggingInfoView::OnEditCopy() 
{
	// TODO: Add your command handler code here
	m_wndGrid.OnEditCopy();
}

void CWellLoggingInfoView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_wndGrid.GetSelectedCount() > 0);
}

void CWellLoggingInfoView::OnEditPaste() 
{
	// TODO: Add your command handler code here
	m_wndGrid.OnEditPaste();
	CWellLoggingInfoDoc *pDoc = GetDocument();
	pDoc->SetModifiedFlag(TRUE);
}

void CWellLoggingInfoView::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	COleDataObject obj;
	pCmdUI->Enable(obj.AttachClipboard() && obj.IsDataAvailable(CF_TEXT)); 
}

void CWellLoggingInfoView::OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	if( pItem->iColumn != -1 && 
		pItem->iRow != -1)
	{

		CWellLoggingInfoDoc *pDoc = GetDocument();
		pDoc->SetModifiedFlag(TRUE);
	}
	*pResult = 0;

}
void CWellLoggingInfoView::OnBtnExport() 
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


void CWellLoggingInfoView::OnBnClickedBtnLoad()
{
	// TODO: 在此添加控件通知处理程序代码

	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();

	CFileView *pTreeView = pMF->GetTreeFileView();
	CViewTree *pTree = pTreeView->GetTreeCtrl();
	HTREEITEM hRoot = pTree->GetChildNode(TVI_ROOT, _T("单井数据"));
	HTREEITEM hSrc = pTree->GetChildNode(hRoot, _T("测井数据"));
	int nImage = pTree->m_nCountImages - 19;

	pTree->DeleteAllChildItem(hSrc);
	pTree->DeleteAllChildItem(hSrc);

	CWaitCursor wait;

	CTreeNodeDat *lpNode = new CTreeNodeDat;

	HTREEITEM hItemLogs = pTreeView->AddTreeItem(_T("全局曲线"), nImage + 5, nImage + 5, hSrc);			

	lpNode->m_nType				= WELL_LOGINGS;
	lpNode->m_nImage			= nImage + 5;
	lpNode->m_nSelectedImage	= nImage + 5;
	lpNode->m_pNodeDat			= NULL;
	lpNode->m_uState			= TVIS_EXPANDED;

	pTree->SetItemData(hItemLogs, (DWORD)lpNode);

	int nRows = m_wndGrid.GetRowCount();

	std::vector<std::string> mapName;

	StatusProgress(1, nRows-1);
	for (int i=1; i<nRows; i++)
	{
		StatusSetProgress(1,i-1);
		CString strWellName = m_wndGrid.GetItemText(i, 1);
		strWellName.Trim();
		if( strWellName.IsEmpty() )
			continue;

		CString strFileName = m_wndGrid.GetItemText(i, 2);
		if( !PathFileExists(strFileName) )
			continue;

		CSplitPath path(strFileName);		

		CString strDatFile = pMF->GetProjectDatPath();
		strDatFile += _T("\\files\\");
		strDatFile += strWellName;
		strDatFile += path.GetExtension();

		CopyFile(strFileName, strDatFile,FALSE);

		CTreeNodeDat *lpNodeDat = new CTreeNodeDat;			

		HTREEITEM hParent = pTreeView->GetTreeCtrl()->GetParentItem(hSrc);
		HTREEITEM hWellInfo = pTreeView->GetTreeCtrl()->GetChildNode(hParent, _T("井基础数据"));
		HTREEITEM hWell = pTreeView->GetTreeCtrl()->GetChildNode(hWellInfo, strWellName);

		CTreeNodeDat *pWellNode = (CTreeNodeDat *)pTreeView->GetTreeCtrl()->GetItemData(hWell);
		HTREEITEM hItem = pTreeView->AddTreeItem(strWellName, pWellNode->m_nImage, pWellNode->m_nSelectedImage, hSrc);			

		strDatFile = strWellName;
		strDatFile += path.GetExtension();

		CFileViewObj *pObj = new CFileViewObj;
		pObj->m_strFileName = strDatFile;

		lpNodeDat->m_nImage			= pWellNode->m_nImage;
		lpNodeDat->m_nSelectedImage	= pWellNode->m_nSelectedImage;
		lpNodeDat->m_nType			= pWellNode->m_nType;
		lpNodeDat->m_pNodeDat		= pObj;
		lpNodeDat->m_uState			= TVIS_EXPANDED;
		strDatFile.MakeUpper();
		lpNodeDat->m_strFileName	= strDatFile;

		pTree->SetItemData(hItem, (DWORD)lpNodeDat);

		CStringArray arFields;

		if( GetFieldsLoging(strFileName, arFields) )
		{
			int nSize = arFields.GetSize();
			for (int j = 0; j<nSize; j++)
			{
				CTreeNodeDat *lpNode = new CTreeNodeDat;

				CString tmp = arFields[j];
				tmp.MakeUpper();
				HTREEITEM h = pTreeView->AddTreeItem(tmp, nImage + 6, nImage + 6, hItem);			

				lpNode->m_nType				= WELL_LOGING;
				lpNode->m_nImage			= nImage + 6;
				lpNode->m_nSelectedImage	= nImage + 6;
				lpNode->m_pNodeDat			= NULL;
				lpNode->m_uState			= TVIS_EXPANDED;

				pTree->SetItemData(h, (DWORD)lpNode);
				pTree->Expand(hItem, TVE_COLLAPSE);

				mapName.push_back((LPCTSTR)tmp);
			}
		}
	}

	StatusProgress(1);

	std::sort(mapName.begin(), mapName.end());
	std::vector<std::string>::iterator iter=std::unique(mapName.begin(),mapName.end());
	mapName.erase(iter,mapName.end()); 

	int nSize = mapName.size();
	for (int i=0; i<nSize; i++)
	{
		CTreeNodeDat *lpNode = new CTreeNodeDat;

		CString tmp = mapName[i].c_str();
		tmp.MakeUpper();
		HTREEITEM h = pTreeView->AddTreeItem(tmp, nImage + 6, nImage + 6, hItemLogs);			

		lpNode->m_nType				= WELL_LOGING;
		lpNode->m_nImage			= nImage + 6;
		lpNode->m_nSelectedImage	= nImage + 6;
		lpNode->m_pNodeDat			= NULL;
		lpNode->m_uState			= TVIS_EXPANDED;

		pTree->SetItemData(h, (DWORD)lpNode);
		pTree->Expand(hItemLogs, TVE_COLLAPSE);
	}

	pTree->Expand(hSrc, TVE_EXPAND);
}
