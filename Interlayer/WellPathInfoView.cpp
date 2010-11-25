#include "StdAfx.h"
#include "Interlayer.h"
#include "WellPathInfoView.h"
#include "WellPathInfoDoc.h"
#include "MainFrm.h"
#include <vector>
#include <string>

extern CInterlayerApp theApp;
extern void StatusProgress(int nIndex, int nRange=-1);
extern void StatusSetProgress(int nIndex, int nPos);

IMPLEMENT_DYNCREATE(CWellPathInfoView, CResizableFormView)

CWellPathInfoView::CWellPathInfoView()
: CResizableFormView(CWellPathInfoView::IDD)
{

}

CWellPathInfoView::~CWellPathInfoView()
{
}

void CWellPathInfoView::DoDataExchange(CDataExchange* pDX)
{
	CResizableFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_INFO, m_strInfo);	
	DDX_GridControl(pDX, IDC_GRID, m_wndGrid);
}

BEGIN_MESSAGE_MAP(CWellPathInfoView, CResizableFormView)
	ON_COMMAND(ID_EDIT_COPY, &CWellPathInfoView::OnEditCopy)
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


// CWellPathInfoView 诊断

#ifdef _DEBUG
void CWellPathInfoView::AssertValid() const
{
	CResizableFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CWellPathInfoView::Dump(CDumpContext& dc) const
{
	CResizableFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CWellPathInfoView 消息处理程序

void CWellPathInfoView::OnInitialUpdate()
{
	CResizableFormView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
	AddAnchor(IDC_GRID,TOP_LEFT,BOTTOM_RIGHT);

	CWellPathInfoDoc *pDoc = GetDocument();
	//pDoc->m_bAutoDelete = TRUE;

	m_wndGrid.SetRowCount(pDoc->m_nRows+1);
	m_wndGrid.SetColumnCount(pDoc->m_nCols+1);

	m_wndGrid.SetFixedColumnCount(1);
	m_wndGrid.SetFixedRowCount(1);

	for (int j=1; j<pDoc->m_nCols+1; j++)
	{
		m_wndGrid.SetItemText(0, j, pDoc->m_arWellPathInfo[j-1]);		
	}

	for ( int i=1; i<pDoc->m_nRows+1; i++)
	{
		for (int j=1; j<pDoc->m_nCols+1; j++)
		{
			CString strtmp = pDoc->m_arWellPathInfo[(i)*pDoc->m_nCols+j-1];
			m_wndGrid.SetCellType(i, 2, RUNTIME_CLASS(CGridCellEditFile));
			m_wndGrid.SetItemText(i, j, pDoc->m_arWellPathInfo[(i)*pDoc->m_nCols+j-1]);			
		}
	}
	m_wndGrid.SetColumnWidth(0, 15);

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

BOOL CWellPathInfoView::PreTranslateMessage(MSG* pMsg)
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

void CWellPathInfoView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (m_wndGrid.m_hWnd)
		m_wndGrid.OnBeginPrinting(pDC, pInfo);
}

void CWellPathInfoView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (m_wndGrid.m_hWnd)
		m_wndGrid.OnEndPrinting(pDC, pInfo);
}

BOOL CWellPathInfoView::OnPreparePrinting(CPrintInfo* pInfo)
{

	// TODO:  调用 DoPreparePrinting 以调用“打印”对话框

	return DoPreparePrinting(pInfo);
	//return CResizableFormView::OnPreparePrinting(pInfo);
}

void CWellPathInfoView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (m_wndGrid.m_hWnd)
		m_wndGrid.OnPrint(pDC, pInfo);
}

CWellPathInfoDoc* CWellPathInfoView::GetDocument()
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWellPathInfoDoc)));
	return (CWellPathInfoDoc*)m_pDocument;
}

void CWellPathInfoView::OnGridClick(NMHDR *pNotifyStruct, LRESULT* /*pResult*/)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	if( pItem->iColumn > 0 && pItem->iRow>0 )
	{
		m_strInfo.Format(_T("行 %d, 列 %d"), pItem->iRow, pItem->iColumn);
		UpdateData(FALSE);
	}
}

void CWellPathInfoView::OnBtnAddRow() 
{
	// TODO: Add your control notification handler code here
	m_wndGrid.InsertRow(_T(""));
	m_wndGrid.Invalidate();
	GetDlgItem(IDC_BTN_DEL_ROW)->EnableWindow(TRUE);
	CWellPathInfoDoc *pDoc = GetDocument();
	pDoc->SetModifiedFlag(TRUE);
}

void CWellPathInfoView::OnBtnDelRow() 
{
	// TODO: Add your control notification handler code here
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();

	CFileView *pTreeView = pMF->GetTreeFileView();
	CViewTree *pTree = pTreeView->GetTreeCtrl();
	HTREEITEM hRoot = pTree->GetChildNode(TVI_ROOT, _T("单井数据"));
	HTREEITEM hSrc = pTree->GetChildNode(hRoot, _T("井眼轨迹数据"));
	int nImage = pTree->m_nCountImages - 19;

	int nRow = m_wndGrid.GetRowCount();
	CArray<int,int> arIndex;
	for (int i=nRow-1;i>0;i--)
	{
		if(m_wndGrid.IsRowSelected(i))
			arIndex.Add(i);
	}

	nRow = arIndex.GetSize();

	//pTree->DeleteAllChildItem(hSrc);
	//pTree->DeleteAllChildItem(hSrc);
	HTREEITEM hSon = pTree->GetChildItem(hSrc);

	while(hSon)
	{
		CString treeText = pTree->GetItemText(hSon);
		bool bDelete = false;
		for(int i=0; i<arIndex.GetSize(); i++)
		{
			CString gridText = m_wndGrid.GetItemText(arIndex[i], 1);
			if(gridText==treeText)
			{
				pTree->DeleteItem(hSon);
				hSon =  pTree->GetChildItem(hSrc);
				bDelete = true;
				break;
			}
		}
		if(!bDelete)
			hSon = pTree->GetNextSiblingItem(hSon);
	}

	for ( int i= 0; i<nRow; i++)
	{
		m_wndGrid.DeleteRow(arIndex[i]);
	}

	m_wndGrid.Invalidate();

	nRow = m_wndGrid.GetRowCount();
	if( nRow < 2 )
		GetDlgItem(IDC_BTN_DEL_ROW)->EnableWindow(FALSE);

	CWellPathInfoDoc *pDoc = GetDocument();
	pDoc->SetModifiedFlag(TRUE);
}

void CWellPathInfoView::OnBtnImport() 
{
	// TODO: Add your control notification handler code here
	CWellPathInfoDoc*pDoc = GetDocument();
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

void CWellPathInfoView::OnContextMenu(CWnd* pWnd, CPoint point) 
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
		if( strMenuName == "_井轨迹信息_" )
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

void CWellPathInfoView::OnEditCut() 
{
	// TODO: Add your command handler code here
	m_wndGrid.OnEditCut();
	CWellPathInfoDoc *pDoc = GetDocument();
	pDoc->SetModifiedFlag(TRUE);
}

void CWellPathInfoView::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_wndGrid.GetSelectedCount() > 0);
}

void CWellPathInfoView::OnEditCopy() 
{
	// TODO: Add your command handler code here
	m_wndGrid.OnEditCopy();
}

void CWellPathInfoView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_wndGrid.GetSelectedCount() > 0);
}

void CWellPathInfoView::OnEditPaste() 
{
	// TODO: Add your command handler code here
	m_wndGrid.OnEditPaste();
	CWellPathInfoDoc *pDoc = GetDocument();
	pDoc->SetModifiedFlag(TRUE);
}

void CWellPathInfoView::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	COleDataObject obj;
	pCmdUI->Enable(obj.AttachClipboard() && obj.IsDataAvailable(CF_TEXT)); 
}

void CWellPathInfoView::OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	if( pItem->iColumn != -1 && 
		pItem->iRow != -1)
	{

		CWellPathInfoDoc *pDoc = GetDocument();
		pDoc->SetModifiedFlag(TRUE);
	}
	*pResult = 0;

}
void CWellPathInfoView::OnBtnExport() 
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


void CWellPathInfoView::OnBnClickedBtnLoad()
{
	// TODO: 在此添加控件通知处理程序代码

	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();

	CFileView *pTreeView = pMF->GetTreeFileView();
	CViewTree *pTree = pTreeView->GetTreeCtrl();
	HTREEITEM hRoot = pTree->GetChildNode(TVI_ROOT, _T("单井数据"));
	HTREEITEM hSrc = pTree->GetChildNode(hRoot, _T("井眼轨迹数据"));

	pTree->DeleteAllChildItem(hSrc);

	CWaitCursor wait;

	int nRows = m_wndGrid.GetRowCount();

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

		CString strDatFile = pMF->GetProjectDatPath();
		strDatFile += _T("\\files\\");
		strDatFile += strWellName;
		strDatFile += _T(".path");

		CSplitPath path(strFileName);
		CString strExt = path.GetExtension();
		strExt.MakeUpper();
		if( strExt == _T(".XLS") )
		{
		}
		else
		{
			std::vector<std::string> vecDat;

			CCSVFile txtFile(strFileName);
			TCHAR ch = '	';
			if( strExt == _T(".CSV") )
				ch = ',';

			CStringArray arr;
			CString strTmp;

			std::vector<int> indexs;
			indexs.resize(3);
			indexs[0] = -1;
			indexs[1] = -1;
			indexs[2] = -1;

			if( txtFile.ReadData(arr,ch) )
			{
				int size = arr.GetSize();
				if( size<3)
					continue;
				size = size>3?3:size;
				for (int i=0; i<size; i++)
				{
					strTmp = arr[i];
					strTmp.MakeLower();

					if( strTmp.Left(4) == _T("dept")
						|| strTmp == _T("js")
						|| strTmp.Left(4) == _T("井深"))
					{
						indexs[0] = i;
					}
					else if( strTmp.Left(2) == _T("jx")
						|| strTmp.Left(3) == _T("dev")
						|| strTmp.Left(4) == _T("井斜"))
					{
						indexs[1] = i;
					}
					else if( strTmp.Left(2) == _T("fw")
						|| strTmp.Left(3) == _T("azi")
						|| strTmp.Left(4) == _T("方位"))
					{
						indexs[2] = i;
					}
				}
			}
			arr.RemoveAll();

			if( indexs[0] == -1||indexs[1] == -1||indexs[2] == -1)
				continue;

			CArray<double,double> JSs;
			CArray<double,double> JXs;
			CArray<double,double> FWs;

			double sd, jx, fw;

			while (txtFile.ReadData(arr,ch))
			{
				int size = arr.GetSize();
				if( size<3)
					continue;
				size = size>3?3:size;

				strTmp = arr[indexs[0]];
				sd = atof(strTmp);

				strTmp = arr[indexs[1]];
				jx = atof(strTmp);

				strTmp = arr[indexs[2]];
				fw = atof(strTmp);

				jx = jx * 3.1415926535/180.0;
				fw = fw * 3.1415926535/180.0;

				JSs.Add(sd);
				JXs.Add(jx);
				FWs.Add(fw);

				arr.RemoveAll();
			}

			txtFile.Close();

			CArray<double,double> Ss;
			CArray<double,double> Hs;
			CArray<double,double> Ns;
			CArray<double,double> Es;


			double JS, JX, FW, H, N, E, S;
			double dL, dH, dN, dE, dS;

			JS = JX = FW = H = N = E = S = 0.0f;

			double pJS = 0 , pJX = 0, pFW = 0;						

			int n = JSs.GetSize();

			for(int i=0; i<n ; i++)
			{
				JS = JSs.GetAt(i);
				JX = JXs.GetAt(i);
				FW = FWs.GetAt(i);

				dL = JS - pJS;

				//曲率
				if( JX == pJX)
				{
					dH = dL * cos(JX);
					dS = dL * sin(JX);
				}
				else
				{
					dH = dL/(JX-pJX) * (sin(JX) - sin(pJX));
					dS = dL/(JX-pJX) * (cos(pJX) - cos(JX));
				}
				if( FW == pFW)
				{
					dN = dS * cos(FW);
					dE = dS * sin(FW);
				}
				else
				{
					dN = dS/(FW - pFW) * (sin(FW) - sin(pFW));
					dE = dS/(FW - pFW) * (cos(pFW) - cos(FW));
				}

				S = S + dS;
				H = H + dH; 
				N = N + dN;
				E = E + dE;

				pJS = JS , pJX = JX, pFW = FW;

				Ss.Add(S);
				Hs.Add(H);
				Ns.Add(N);
				Es.Add(E);

				JX = JX / 3.1415926535 * 180.0;
				FW = FW / 3.1415926535 * 180.0;

				strTmp.Format(_T("%g"), JS);
				vecDat.push_back((LPCSTR)strTmp);
				strTmp.Format(_T("%g"), JX);
				vecDat.push_back((LPCSTR)strTmp);
				strTmp.Format(_T("%g"), FW);
				vecDat.push_back((LPCSTR)strTmp);

				strTmp.Format(_T("%.2lf"), E);
				vecDat.push_back((LPCSTR)strTmp);
				strTmp.Format(_T("%.2lf"), N);
				vecDat.push_back((LPCSTR)strTmp);
				strTmp.Format(_T("%.2lf"), H);
				vecDat.push_back((LPCSTR)strTmp);
			}

			int nRows = vecDat.size()/6;
			int nCols = 6;

			CFile file(strDatFile, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
			CArchive ar(&file, CArchive::store);
			ar << nRows;
			ar << nCols;
			for (int i=0;i<nRows;i++)
			{
				for (int j=0; j<nCols; j++)
				{
					CString strTmp;
					strTmp = vecDat[i*6+j].c_str();
					ar << strTmp;
				}
			}

			ar.Close();
			file.Close();

			CTreeNodeDat *lpNodeDat = new CTreeNodeDat;			

			HTREEITEM hParent = pTreeView->GetTreeCtrl()->GetParentItem(hSrc);
			HTREEITEM hWellInfo = pTreeView->GetTreeCtrl()->GetChildNode(hParent, _T("井基础数据"));
			HTREEITEM hWell = pTreeView->GetTreeCtrl()->GetChildNode(hWellInfo, strWellName);

			CTreeNodeDat *pWellNode = (CTreeNodeDat *)pTreeView->GetTreeCtrl()->GetItemData(hWell);
			HTREEITEM hItem = pTreeView->AddTreeItem(strWellName, pWellNode->m_nImage, pWellNode->m_nSelectedImage, hSrc);			

			strDatFile = strWellName;
			strDatFile += _T(".path");

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
			pTree->Select(hItem, TVGN_CARET);
		}
	}

	pTree->Expand(hSrc, TVE_EXPAND);
	
	StatusProgress(1);
}

