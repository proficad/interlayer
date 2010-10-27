
#include "stdafx.h"
#include "mainfrm.h"
#include "FileView.h"
#include "Resource.h"
#include "Interlayer.h"
#include "DlgImportWellInfo.h"
#include "WellInfoView.h"
#include "WellPathInfoView.h"
#include "WellPathView.h"
#include "FileDatView.h"
#include "MyEditView.h"
#include "MyRichEditView.h"
#include "PictrueView.h"
#include "WellLoggingInfoView.h"
#include "Vertical_Model/graph_props.h"
#include "Vertical_Model/VertModelView.h"
#include "Vertical_Model/VertModelDoc.h"
#include "FormGenerateLayer.h"
#include "DlgTrangleExport.h"
#include "3DLib/3DObject.h"
#include "3DLib/PlaneReader.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CFileViewObj, CObject, VERSIONABLE_SCHEMA | 0)
IMPLEMENT_SERIAL(CWellInfo, CObject, VERSIONABLE_SCHEMA | 0)

CFileViewObj::CFileViewObj()
{
	m_strFileName = _T("");
}

CFileViewObj::~CFileViewObj()
{	
}

void CFileViewObj::Serialize(CArchive& ar)
{
	ar.SerializeClass(RUNTIME_CLASS(CFileViewObj));
	if(ar.IsStoring())
	{
		ar << m_strFileName;
	}
	else
	{
		int iVer = ar.GetObjectSchema();
		switch(iVer)
		{
		case 0:
			{
				ar >> m_strFileName;
			}
			break;
		default:
			break;
		}			
	}
}


CArchive& operator <<(CArchive& ar, const CFileViewObj& dat)
{
	ar.WriteClass(RUNTIME_CLASS(CFileViewObj));
	ar << dat.m_strFileName;
	return ar;
}

CArchive& operator >>(CArchive& ar, CFileViewObj& dat)
{
	ar.ReadClass(RUNTIME_CLASS(CFileViewObj));
	int iVer = ar.GetObjectSchema();
	switch(iVer)
	{
	case 0:
		{
			ar >> dat.m_strFileName;
		}
		break;
	default:
		break;
	}

	return ar;
}

CFileViewObj &CFileViewObj::operator=(const CFileViewObj& dat)
{
	m_strFileName = dat.m_strFileName;
	return (*this);
}

//////////////////////////////////////////////////////////////////////////
void CWellInfo::Serialize(CArchive& ar)
{
	ar.SerializeClass(RUNTIME_CLASS(CWellInfo));
	if(ar.IsStoring())
	{
		ar << m_strWellName;
		ar << m_strWellType;
		ar << m_x;
		ar << m_y;
		ar << m_dElevation;				// 补芯海拔
		ar << m_dTop;					// 油层顶
		ar << m_dBottom;				// 油层底
	}
	else
	{
		int iVer = ar.GetObjectSchema();
		switch(iVer)
		{
		case 0:
			{
				ar >> m_strWellName;
				ar >> m_strWellType;
				ar >> m_x;
				ar >> m_y;

				ar >> m_dElevation;				// 补芯海拔
				ar >> m_dTop;					// 油层顶
				ar >> m_dBottom;				// 油层底
			}
			break;
		default:
			break;
		}			
	}
}


CArchive& operator <<(CArchive& ar, const CWellInfo& dat)
{
	ar.WriteClass(RUNTIME_CLASS(CWellInfo));
	ar << dat.m_strWellName;
	ar << dat.m_strWellType;
	ar << dat.m_x;
	ar << dat.m_y;
	ar << dat.m_dElevation;				// 补芯海拔
	ar << dat.m_dTop;					// 油层顶
	ar << dat.m_dBottom;				// 油层底

	return ar;
}

CArchive& operator >>(CArchive& ar, CWellInfo& dat)
{
	ar.ReadClass(RUNTIME_CLASS(CWellInfo));
	int iVer = ar.GetObjectSchema();
	switch(iVer)
	{
	case 0:
		{
			ar >> dat.m_strWellName;
			ar >> dat.m_strWellType;
			ar >> dat.m_x;
			ar >> dat.m_y;
			ar >> dat.m_dElevation;				// 补芯海拔
			ar >> dat.m_dTop;					// 油层顶
			ar >> dat.m_dBottom;				// 油层底
		}
		break;
	default:
		break;
	}

	return ar;
}

CWellInfo &CWellInfo::operator=(const CWellInfo& dat)
{
	m_strWellName = dat.m_strWellName;
	m_strWellType = dat.m_strWellType;
	m_x = dat.m_x;
	m_y = dat.m_y;
	m_dElevation = dat.m_dElevation;		// 补芯海拔
	m_dTop = dat.m_dTop;					// 油层顶
	m_dBottom = dat.m_dBottom;				// 油层底

	return (*this);
}


/////////////////////////////////////////////////////////////////////////////
// CFileView

CFileView::CFileView()
{
	m_wndFileView.m_eTreeType = CViewTree::fileTree;
	m_pImagelist = NULL;
}

CFileView::~CFileView()
{
	if (m_pImagelist != NULL)
	{
		m_pImagelist->Detach();
		delete m_pImagelist;
		m_pImagelist = NULL;
	}
}

BEGIN_MESSAGE_MAP(CFileView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, OnUpdateFileNew)
	ON_COMMAND(ID_OPEN, OnFileOpen)	
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_EDIT_RENAME, &CFileView::OnEditRename)
	ON_UPDATE_COMMAND_UI(ID_EDIT_RENAME, &CFileView::OnUpdateEditRename)
	ON_COMMAND(ID_ADD_CHILD, &CFileView::OnAddChild)
	ON_UPDATE_COMMAND_UI(ID_ADD_CHILD, &CFileView::OnUpdateAddChild)
	ON_COMMAND(ID_NEW_FOLDER, &CFileView::OnNewFolder)
	ON_UPDATE_COMMAND_UI(ID_ADD_CHILD, &CFileView::OnUpdateNewFolder)
	ON_COMMAND(ID_ADD_NEW_FILE, &CFileView::OnAddNewFile)
	ON_UPDATE_COMMAND_UI(ID_ADD_NEW_FILE, &CFileView::OnUpdateAddNewFile)
	ON_COMMAND(IDC_EXPAND_ALL_TREE, &CFileView::OnExpandAllTree)
	ON_COMMAND(ID_COLLAPSE_ALL_ENTIRE_TREE, &CFileView::OnCollapseAllEntireTree)
	ON_COMMAND(ID_NEW_WELL_DAT, &CFileView::OnNewWellDat)
	ON_UPDATE_COMMAND_UI(ID_NEW_WELL_DAT, &CFileView::OnUpdateNewWellDat)
	ON_COMMAND(ID_IMPORT_POINT, &CFileView::OnImportPoint)
	ON_UPDATE_COMMAND_UI(ID_IMPORT_POINT, &CFileView::OnUpdateImportPoint)
	ON_COMMAND(ID_IMPORT_LINE, &CFileView::OnImportLine)
	ON_UPDATE_COMMAND_UI(ID_IMPORT_LINE, &CFileView::OnUpdateImportLine)
	ON_COMMAND(ID_IMPORT_POLYGON, &CFileView::OnImportPolygon)
	ON_UPDATE_COMMAND_UI(ID_IMPORT_POLYGON, &CFileView::OnUpdateImportPolygon)
	ON_COMMAND(ID_IMPORT_PLANE, &CFileView::OnImportPlane)
	ON_UPDATE_COMMAND_UI(ID_IMPORT_PLANE, &CFileView::OnUpdateImportPlane)
	ON_COMMAND(ID_IMPORT_PICTRUE, &CFileView::OnImportPictrue)
	ON_UPDATE_COMMAND_UI(ID_IMPORT_PICTRUE, &CFileView::OnUpdateImportPictrue)
	ON_COMMAND(ID_IMPORT_TRACK, &CFileView::OnImportTrack)
	ON_UPDATE_COMMAND_UI(ID_IMPORT_TRACK, &CFileView::OnUpdateImportTrack)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_TRIANGLE_EXPORT, OnExportTriangle)
	ON_UPDATE_COMMAND_UI(ID_TRIANGLE_EXPORT, OnUpdateExportTriangle)	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar 消息处理程序

int CFileView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建视图:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | WS_DISABLED
		| TVS_HASLINES 
		| TVS_LINESATROOT | TVS_HASBUTTONS 
		| TVS_CHECKBOXES | TVS_TRACKSELECT
		| TVS_EDITLABELS;

	if (!m_wndFileView.Create(dwViewStyle, rectDummy, this, 4))
	{
		TRACE0("未能创建文件视图\n");
		return -1;      // 未能创建
	}

	m_wndFileView.pfOpenTree = (CViewTree::OPENTREE_FUNCTION)&CFileView::OpenTree;

	// 加载视图图像:

	GetSysImgList();

	// 加载树状态
	m_TreeStateImages.Create(IDB_TREE_STATE, 16, 0, RGB(255, 255, 255));
	m_wndFileView.SetImageList(&m_TreeStateImages, TVSIL_STATE);

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_EXPLORER);
	m_wndToolBar.LoadToolBar(IDR_EXPLORER, 0, 0, TRUE /* 已锁定*/);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// 所有命令将通过此控件路由，而不是通过主框架路由:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	// 填入一些静态树视图数据(此处只需填入虚拟代码，而不是复杂的数据)
	//FillFileView();
	//OnChangeVisualStyle();
	AdjustLayout();
	return 0;
}

BOOL CFileView::GetSysImgList()
{
	if (m_pImagelist != NULL)
	{
		m_pImagelist->Detach();
		delete m_pImagelist;
		m_pImagelist = NULL;
	}

	HIMAGELIST hImgList = theApp.GetSystemImageList(TRUE);

	m_pImagelist = new CImageList();
	m_pImagelist->Attach(hImgList); 
	m_pImagelist->Add(AfxGetApp()->LoadIcon(IDI_ICON_FOLDER));
	m_pImagelist->Add(AfxGetApp()->LoadIcon(IDI_ICON_WELL_DAT));
	m_pImagelist->Add(AfxGetApp()->LoadIcon(IDI_ICON_WELL_INFO));
	m_pImagelist->Add(AfxGetApp()->LoadIcon(IDI_ICON_WELL_PATH));
	m_pImagelist->Add(AfxGetApp()->LoadIcon(IDI_ICON_LOGGING_INFO));
	m_pImagelist->Add(AfxGetApp()->LoadIcon(IDI_ICON_LOGGINGS));	
	m_pImagelist->Add(AfxGetApp()->LoadIcon(IDI_ICON_LOGGING));
	m_pImagelist->Add(AfxGetApp()->LoadIcon(IDI_ICON_WELL_PRO));
	m_pImagelist->Add(AfxGetApp()->LoadIcon(IDI_ICON_WELL_INJ));
	m_pImagelist->Add(AfxGetApp()->LoadIcon(IDI_ICON_POINT_DAT));
	m_pImagelist->Add(AfxGetApp()->LoadIcon(IDI_ICON_LINE_DAT));
	m_pImagelist->Add(AfxGetApp()->LoadIcon(IDI_ICON_POLYGON_DAT));
	m_pImagelist->Add(AfxGetApp()->LoadIcon(IDI_ICON_PLANE_DAT));
	m_pImagelist->Add(AfxGetApp()->LoadIcon(IDI_ICON_PICTRUE_DAT));
	m_pImagelist->Add(AfxGetApp()->LoadIcon(IDI_ICON_NORMAL));
	m_pImagelist->Add(AfxGetApp()->LoadIcon(IDI_ICON_LAYER));
	m_pImagelist->Add(AfxGetApp()->LoadIcon(IDI_ICON_VERT_MODELS));
	m_pImagelist->Add(AfxGetApp()->LoadIcon(IDI_ICON_VERT_MODEL));
	m_pImagelist->Add(AfxGetApp()->LoadIcon(IDI_ICON_LAYER_MODEL));	

	m_wndFileView.m_nCountImages = m_pImagelist->GetImageCount();

	m_wndFileView.SetImageList( m_pImagelist, TVSIL_NORMAL );

	return TRUE;   // OK
}

bool CFileView::OpenTree(CTreeNodeDat *lpNodeDat, void *lpVoid)
{	
	CString strTitle = (LPCSTR)lpVoid;

	switch ( lpNodeDat->m_nType )
	{
	case WELL_INFO:
		{	
			CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
			CString strFileName = pMF->GetProjectDatPath();
			strFileName += _T("\\files\\");
			strFileName += ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;
			if( !PathFileExists(strFileName) )
			{
				CFile file(strFileName, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
				CArchive ar(&file, CArchive::store);

				int nRows = 0;
				int nCols = 7;

				ar << nRows;
				ar << nCols;

				CString str = _T("井号");
				ar << str;	
				str = _T("X");
				ar << str;
				str = _T("Y");
				ar << str;
				str = _T("补芯海拔");
				ar << str;
				str = _T("目前井别");
				ar << str;
				str = _T("油层顶深");
				ar << str;
				str = _T("油层底深");
				ar << str;

				ar.Close();
				file.Close();
			}
			CDocument *pDoc = pMF->CreateOrActivateFrame(theApp.m_pWellInfoDocTempl, RUNTIME_CLASS(CWellInfoView), strFileName);

			pDoc->SetPathName(strFileName);
			pDoc->SetTitle(strTitle);
		}
		break;
	case WELL_HOLE:
		{
			CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
			CString strFileName = pMF->GetProjectDatPath();
			strFileName += _T("\\files\\");
			strFileName += ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;
			if( !PathFileExists(strFileName) )
			{
				CFile file(strFileName, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
				CArchive ar(&file, CArchive::store);

				int nRows = 0;
				int nCols = 2;

				ar << nRows;
				ar << nCols;

				CString str = _T("井号");
				ar << str;	
				str = _T("文件");
				ar << str;

				ar.Close();
				file.Close();
			}
			CDocument *pDoc = pMF->CreateOrActivateFrame(theApp.m_pWellPathInfoDocTempl, RUNTIME_CLASS(CWellPathInfoView), strFileName);
			pDoc->SetPathName(strFileName);
			pDoc->SetTitle(strTitle);
		}
		break;
	case WELL_LOGING_INFO:
		{
			CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
			CString strFileName = pMF->GetProjectDatPath();
			strFileName += _T("\\files\\");
			strFileName += ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;
			if( !PathFileExists(strFileName) )
			{
				CFile file(strFileName, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
				CArchive ar(&file, CArchive::store);

				int nRows = 0;
				int nCols = 2;

				ar << nRows;
				ar << nCols;

				CString str = _T("井号");
				ar << str;	
				str = _T("文件");
				ar << str;

				ar.Close();
				file.Close();
			}
			CDocument *pDoc = pMF->CreateOrActivateFrame(theApp.m_pWellLoggingInfoDocTempl, RUNTIME_CLASS(CWellLoggingInfoView), strFileName);
			pDoc->SetPathName(strFileName);
			pDoc->SetTitle(strTitle);
		}
		break;
	case WELL_PRO:
	case WELL_INJ:
		{
			if( lpNodeDat->m_pNodeDat && lpNodeDat->m_pNodeDat->IsKindOf(RUNTIME_CLASS(CFileViewObj)) )
			{
				CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
				CString strFileName = pMF->GetProjectDatPath();
				strFileName += _T("\\files\\");
				strFileName += ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;
				if( !PathFileExists(strFileName) )
				{
					AfxMessageBox(_T("此文件不存在!"));
				}
				else
				{
					CString strExt = strFileName.Right(5);
					strExt.MakeUpper();
					if( strExt == _T(".PATH") )
					{
						CDocument *pDoc = pMF->CreateOrActivateFrame(theApp.m_pWellPathDocTempl, RUNTIME_CLASS(CWellPathView), strFileName);
						if( pDoc ) 
						{
							pDoc->SetPathName(strFileName);
							pDoc->SetTitle(strTitle);
						}
					}
					else
					{
						//CDocument *pDoc = pMF->CreateOrActivateFrame(theApp.m_pLoggingDocTemplate, NULL, NULL);
						CDocument *pDoc = pMF->CreateOrActivateFrame(theApp.m_pTxtDocTempl, NULL, strFileName);
						//if( pDoc ) 
						{
							//pDoc->SetPathName(strFileName);
							//pDoc->SetTitle(strTitle);
						}
					}
				}
			}
		}
		break;
	case FILE_POINT:
	case FILE_LINE:
	case FILE_POLYGON:
	case FILE_PLANE:
	case FILE_LAYER:
	case FILE_NORMAL:
	case MODEL_NORMAL:
	case MODEL_LAYER:
		{
			CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
			CString strFileName = pMF->GetProjectDatPath();
			strFileName += _T("\\files\\");
			strFileName += ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;
			if( !PathFileExists(strFileName) )
			{
				AfxMessageBox(_T("此文件不存在!"));
			}
			else
			{
				CDocument *pDoc = pMF->CreateOrActivateFrame(theApp.m_pFileDatDocTemplate, RUNTIME_CLASS(CFileDatView), strFileName);
				if( pDoc ) 
				{
					pDoc->SetPathName(strFileName);
					pDoc->SetTitle(strTitle);
				}
			}
		}
		break;
	case FILE_PICTRUE:
		{
			//ShellExecute(NULL,"open",((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName,_T(""), NULL,SW_SHOWNORMAL);

			CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
			CString strFileName = pMF->GetProjectDatPath();
			strFileName += _T("\\files\\");
			strFileName += ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;
			if( !PathFileExists(strFileName) )
			{
				AfxMessageBox(_T("此文件不存在!"));
			}
			else
			{
				CDocument *pDoc = pMF->CreateOrActivateFrame(theApp.m_pPictrueDocTempl, RUNTIME_CLASS(CPictrueView), strFileName);
				if( pDoc ) 
				{
					pDoc->SetPathName(strFileName);
					pDoc->SetTitle(strTitle);
				}
			}			
		}
		break;
	case FILE_VERT_MODELS:
		{
			CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
			TCHAR szCurPath[MAX_PATH];
			memset(szCurPath,   0,   MAX_PATH);  
			GetModuleFileName(NULL,   szCurPath,   sizeof(szCurPath)/sizeof(TCHAR));   
			CString strMoudlePath = szCurPath;
			strMoudlePath = strMoudlePath.Left(strMoudlePath.ReverseFind('\\')+1);
			CString strFileName = strMoudlePath;
			strFileName += _T("Template\\models.dat");
			CDocument *pDoc = NULL;
			if(PathFileExists(strFileName))
				pDoc = pMF->CreateOrActivateFrame(theApp.m_pVertModelDocTemplate, RUNTIME_CLASS(CVertModelView), strFileName);
			else
				pDoc = pMF->CreateOrActivateFrame(theApp.m_pVertModelDocTemplate, RUNTIME_CLASS(CVertModelView), NULL);
			if( pDoc )
				pDoc->SetTitle(_T("纵向模型"));
		}
		break;
	case FILE_VERT_MODEL:
		{
			CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
			TCHAR szCurPath[MAX_PATH];
			memset(szCurPath,   0,   MAX_PATH);  
			GetModuleFileName(NULL,   szCurPath,   sizeof(szCurPath)/sizeof(TCHAR));   
			CString strMoudlePath = szCurPath;
			strMoudlePath = strMoudlePath.Left(strMoudlePath.ReverseFind('\\')+1);
			CString strFileName = strMoudlePath;
			strFileName += _T("Template\\models.dat");
			CVertModelDoc *pDoc = NULL;
			if(PathFileExists(strFileName))
				pDoc = (CVertModelDoc *)pMF->CreateOrActivateFrame(theApp.m_pVertModelDocTemplate, RUNTIME_CLASS(CVertModelView), strFileName);
			else
				pDoc = (CVertModelDoc *)pMF->CreateOrActivateFrame(theApp.m_pVertModelDocTemplate, RUNTIME_CLASS(CVertModelView), NULL);
			if( pDoc )
				pDoc->SetTitle(_T("纵向模型"));

			pDoc->SetCurrentGraph(strTitle);
		}
		break;
	case FILE_LAYER_MODEL:
		{
			CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
			CString strFileName = pMF->GetProjectDatPath();
			strFileName += _T("\\files\\");
			strFileName += ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;
			if( !PathFileExists(strFileName) )
			{
				AfxMessageBox(_T("此文件不存在!"));
			}
			else
			{
				CDocument *pDoc = pMF->CreateOrActivateFrame(theApp.m_pLayerDocTemplate, RUNTIME_CLASS(CFormGenerateLayer), strFileName);
				if( pDoc ) 
				{
					pDoc->SetPathName(strFileName);
					pDoc->SetTitle(strTitle);
				}
			}
		}
		break;
	default:
		return false;
	}
	return true;
}

void CFileView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CFileView::FillFileView(LPCTSTR lpszItem, int nImage, int nSelectedImage, 
							 HTREEITEM hParent/* = TVI_ROOT*/, 
							 HTREEITEM hInsertAfter/* = TVI_LAST*/)
{
	HTREEITEM hRoot = AddTreeItem(_T("DEMO 文件"), 0, 0);
	m_wndFileView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);

	HTREEITEM hSrc = AddTreeItem(_T("井基础数据"), 0, 0, hRoot);

	HTREEITEM hItem;
	hItem = AddTreeItem(_T("well1"), 1, 1, hSrc);
	hItem = AddTreeItem(_T("well2"), 1, 1, hSrc);
	hItem = AddTreeItem(_T("well3"), 1, 1, hSrc);
	hItem = AddTreeItem(_T("well4"), 1, 1, hSrc);
	hItem = AddTreeItem(_T("well5"), 1, 1, hSrc);
	hItem = AddTreeItem(_T("well6"), 1, 1, hSrc);

	HTREEITEM hInc = m_wndFileView.InsertItem(_T("分层数据"), 0, 0, hRoot);

	hItem = AddTreeItem(_T("well1"), 2, 2, hInc);
	hItem = AddTreeItem(_T("well2"), 2, 2, hInc);
	hItem = AddTreeItem(_T("well3"), 2, 2, hInc);
	hItem = AddTreeItem(_T("well4"), 2, 2, hInc);
	hItem = AddTreeItem(_T("well5"), 2, 2, hInc);
	hItem = AddTreeItem(_T("well6"), 2, 2, hInc);

	HTREEITEM hRes = m_wndFileView.InsertItem(_T("射孔井段"), 0, 0, hRoot);

	hItem = AddTreeItem(_T("well1"), 2, 2, hRes);
	hItem = AddTreeItem(_T("well2"), 2, 2, hRes);
	hItem = AddTreeItem(_T("well3"), 2, 2, hRes);
	hItem = AddTreeItem(_T("well4"), 2, 2, hRes);

	HTREEITEM hGrid = AddTreeItem(_T("网格模型"), 0, 0, hRoot);

	hItem = AddTreeItem(_T("Param1"), 1, 1, hGrid);
	hItem = AddTreeItem(_T("Param2"), 1, 1, hGrid);
	hItem = AddTreeItem(_T("Param3"), 1, 1, hGrid);
	hItem = AddTreeItem(_T("Param4"), 1, 1, hGrid);

	m_wndFileView.Expand(hRoot, TVE_EXPAND);
	m_wndFileView.Expand(hSrc, TVE_EXPAND);
	m_wndFileView.Expand(hInc, TVE_EXPAND);
}

HTREEITEM CFileView::AddTreeItem(LPCTSTR lpszItem, int nImage, int nSelectedImage, 
					  HTREEITEM hParent, 
					  HTREEITEM hInsertAfter)
{
	HTREEITEM hItem = m_wndFileView.InsertItem(lpszItem, nImage, nSelectedImage, hParent, hInsertAfter);
	m_wndFileView.SetModifiedFlag(true);
	return hItem;
}

void CFileView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*) &m_wndFileView;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// 选择已单击的项:
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			pWndTree->SelectItem(hTreeItem);
		}
	}

	pWndTree->SetFocus();
	
	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_POPUP_TREE));
	int nCount = menu.GetMenuItemCount();
	CMenu* pPopup = NULL;
	//
	//	GetMenuString
	CString strMenuName;
	for (int i=0;i<nCount;i++)
	{
		menu.GetMenuString(i,strMenuName,MF_BYPOSITION);
		if( strMenuName == "_文件管理器_" )
		{
			pPopup = menu.GetSubMenu(i);
			break;
		}
	}

	if( pPopup == NULL )
		return;

	ASSERT(pPopup != NULL);

	theApp.GetContextMenuManager()->ShowPopupMenu(pPopup->m_hMenu, point.x, point.y, this, TRUE);
}

void CFileView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndFileView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CFileView::OnFileOpen()
{
	// TODO: 在此处添加命令处理程序代码

	HTREEITEM hItem = m_wndFileView.GetSelectedItem();
	
	if( hItem == NULL )
		return;

	CString strTitle = m_wndFileView.GetItemText(hItem);
	CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)m_wndFileView.GetItemData(hItem);
	OpenTree(lpNodeDat, (LPVOID)(LPCSTR)strTitle);
}

void CFileView::OnFileNew()
{
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	pMF->OnFileNew();
}

void CFileView::OnUpdateFileNew(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码

	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	pMF->OnUpdateFileNew(pCmdUI);
}


void CFileView::OnPaint()
{
	CPaintDC dc(this); // 用于绘制的设备上下文

	CRect rectTree;
	m_wndFileView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CFileView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndFileView.SetFocus();
}

void CFileView::OnChangeVisualStyle()
{
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_EXPLORER_24 : IDR_EXPLORER, 0, 0, TRUE /* 锁定 */);

	GetSysImgList();

	// 装入树状态图标
	m_TreeStateImages.DeleteImageList();	

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_TREE_STATE_24 : IDB_TREE_STATE;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("无法加载位图: %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_TreeStateImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_TreeStateImages.Add(&bmp, RGB(255, 255, 255));

	m_wndFileView.SetImageList(&m_TreeStateImages, TVSIL_STATE);
}

void CFileView::LoadTree(CArchive& ar)
{
	m_wndFileView.Serialize(ar);

	HTREEITEM hSrc = m_wndFileView.GetChildNode(TVI_ROOT, _T("纵向模型"));
	
	if(hSrc!=NULL)
	{
		m_wndFileView.DeleteAllChildItem(hSrc);
		TCHAR szCurPath[MAX_PATH];  
		memset(szCurPath,   0,   MAX_PATH);  
		GetModuleFileName(NULL,   szCurPath,   sizeof(szCurPath)/sizeof(TCHAR));   
		CString strMoudlePath = szCurPath;
		strMoudlePath = strMoudlePath.Left(strMoudlePath.ReverseFind('\\')+1);
		CString strFileName = strMoudlePath;
		strFileName += _T("Template\\models.dat");
		if(PathFileExists(strFileName))
		{
			CFile file(strFileName, CFile::modeRead|CFile::typeBinary);
			CArchive ar(&file, CArchive::load);

			std::vector<CGraphProps*> graphs;
			long graphCount;

			ar >> graphCount;

			for (int i = 0; i < graphCount; i++)
			{
				CGraphProps* pGraph = new CGraphProps;
				pGraph->Serialize(ar);
				graphs.push_back(pGraph);
			}

			ar.Close();
			file.Close();

			int nImage = m_wndFileView.m_nCountImages - 19;

			for (int i = 0; i < graphCount; i++)
			{
				if (graphs[i] != NULL)
				{
					HTREEITEM hItem = AddTreeItem(graphs[i]->GetTitle(), nImage + 17, nImage + 17, hSrc);

					CTreeNodeDat*lpNodeDat = new CTreeNodeDat;

					lpNodeDat->m_nImage			= nImage + 17;
					lpNodeDat->m_nSelectedImage	= nImage + 17;
					lpNodeDat->m_nType			= FILE_VERT_MODEL;
					lpNodeDat->m_pNodeDat		= NULL;
					lpNodeDat->m_uState			= TVIS_EXPANDED;

					m_wndFileView.SetItemData(hItem, (DWORD)lpNodeDat);

					delete graphs[i];
					graphs[i] = NULL;
				}
			}			
		}
	}

	m_wndFileView.SetModifiedFlag(false);
}

void CFileView::SaveTree(CArchive& ar)
{
	m_wndFileView.Serialize(ar);

	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	CString strDirectory = pMF->GetProjectDatPath();
	CString strTmp = strDirectory;
	strTmp += _T("\\files");
	m_wndFileView.DeleteUnusedFile(strTmp);
}

void CFileView::EmptyTree()
{
	m_wndFileView.DeleteItemM(TVI_ROOT);
}

void CFileView::OnEditRename()
{
	// TODO: 在此添加命令处理程序代码
	m_wndFileView.OnRename();
}

void CFileView::OnUpdateEditRename(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	m_wndFileView.OnUpdateRename(pCmdUI);
}

void CFileView::OnAddChild()
{
	// TODO: 在此添加命令处理程序代码

	int nImage = m_wndFileView.m_nCountImages - 19;
	m_wndFileView.AddChildFolder(NULL, nImage);
}

void CFileView::OnUpdateAddChild(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码

	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	if( pMF->m_strPrjFileName.IsEmpty() )
		pCmdUI->Enable(FALSE);
	else
	{
		HTREEITEM hItem = m_wndFileView.GetSelectedItem();

		if( hItem == NULL)
			pCmdUI->Enable(TRUE);
		else
		{
			CTreeNodeDat *lpNode = (CTreeNodeDat *)m_wndFileView.GetItemData(hItem);
			if(lpNode->m_nType == FOLDER )
				pCmdUI->Enable(TRUE);
			else
				pCmdUI->Enable(FALSE);
		}
	}
}

void CFileView::OnNewFolder()
{
	// TODO: 在此添加命令处理程序代码

	int nImage = m_wndFileView.m_nCountImages - 19;
	m_wndFileView.AddSiblingFolder(NULL, nImage);
}

void CFileView::OnUpdateNewFolder(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();

	if( pMF->m_strPrjFileName.IsEmpty() )
		pCmdUI->Enable(FALSE);
	else
	{
		pCmdUI->Enable(TRUE);
	}
}

// 添加文件
void CFileView::OnAddNewFile()
{
	// TODO: 在此添加命令处理程序代码

	CFileDialog dlg(TRUE, 0, 0, 4|2, _T("数据文件|*.*|所有支持的图形文件(*.bmp;*.gif;*.jpg;*.jpeg;*.png;*.ico;*.tif;*.tiff;*.tga;*.pcx;*.wbmp;*.wmf;*.emf;*.j2k;*.jp2;*.jbg;*.j2c;*.jpc;*.pgx;*.pnm;*.pgm;*.ppm;*.ras)|*.bmp;*.gif;*.jpg;*.jpeg;*.png;*.ico;*.tif;*.tiff;*.tga;*.pcx;*.wbmp;*.wmf;*.emf;*.j2k;*.jp2;*.jbg;*.j2c;*.jpc;*.pgx;*.pnm;*.pgm;*.ppm;*.ras||"));
	if( dlg.DoModal() == IDOK )
	{
		CString strSourcePathName = dlg.GetPathName();
		CString strFileName = dlg.GetFileName();
		CString strFileTitle = dlg.GetFileTitle();

		CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
		CString strTargePathName = pMF->GetProjectDatPath();
		strTargePathName += _T("\\files\\");
		strTargePathName += strFileName;

		CopyFile(strSourcePathName, strTargePathName, FALSE);

		CString strExt = dlg.GetFileExt();
		strExt.MakeLower();

		if( strExt == _T("bmp")
			|| strExt == _T("gif")
			|| strExt == _T("jpg")
			|| strExt == _T("jpeg")
			|| strExt == _T("png")
			|| strExt == _T("ico")
			|| strExt == _T("tif")
			|| strExt == _T("tiff")
			|| strExt == _T("tga")
			|| strExt == _T("pcx")
			|| strExt == _T("wbmp")
			|| strExt == _T("wmf")
			|| strExt == _T("emf")
			|| strExt == _T("j2k")
			|| strExt == _T("jp2")
			|| strExt == _T("jbg")
			|| strExt == _T("j2c")
			|| strExt == _T("jpc")
			|| strExt == _T("pgx")
			|| strExt == _T("pnm")
			|| strExt == _T("pgm")
			|| strExt == _T("ppm")
			|| strExt == _T("ras"))
		{
			HTREEITEM hItem = m_wndFileView.GetSelectedItem();
			if( hItem == NULL )
				hItem = TVI_ROOT;
			
			int nImage = m_wndFileView.m_nCountImages - 19;

			HTREEITEM h = AddTreeItem(strFileTitle, nImage + 13 , nImage + 13, hItem);
			
			CTreeNodeDat *lpNodeDat = new CTreeNodeDat;

			CFileViewObj *pObj = new CFileViewObj;
			pObj->m_strFileName = strFileName;

			lpNodeDat->m_nImage			= nImage + 13;
			lpNodeDat->m_nSelectedImage	= nImage + 13;
			lpNodeDat->m_nType			= FILE_PICTRUE;
			lpNodeDat->m_pNodeDat		= pObj;
			lpNodeDat->m_uState			= TVIS_EXPANDED;

			strFileName.MakeUpper();
			lpNodeDat->m_strFileName = strFileName;

			m_wndFileView.SetItemData(h, (DWORD)lpNodeDat);

			m_wndFileView.Expand(hItem, TVE_EXPAND);

			m_wndFileView.EditLabel(h);
		}
		else
		{
			HTREEITEM hItem = m_wndFileView.GetSelectedItem();
			if( hItem == NULL )
				hItem = TVI_ROOT;
			
			int iImage;
			int iSelectedImage;
			m_wndFileView.GetNormalAndSelectedIcons(strTargePathName, iImage, iSelectedImage);

			HTREEITEM h = AddTreeItem(strFileTitle, iImage, iSelectedImage, hItem);

			CTreeNodeDat *lpNodeDat = new CTreeNodeDat;

			CFileViewObj *pObj = new CFileViewObj;
			pObj->m_strFileName = strFileName;

			lpNodeDat->m_nImage			= iImage;
			lpNodeDat->m_nSelectedImage	= iSelectedImage;
			lpNodeDat->m_nType			= FILE_DAT;
			lpNodeDat->m_pNodeDat		= pObj;
			lpNodeDat->m_uState			= TVIS_EXPANDED;
			strFileName.MakeUpper();
			lpNodeDat->m_strFileName = strFileName;

			m_wndFileView.SetItemData(h, (DWORD)lpNodeDat);

			m_wndFileView.Expand(hItem, TVE_EXPAND);

			m_wndFileView.EditLabel(h);
		}
	}
}

void CFileView::OnUpdateAddNewFile(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码

	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	if( pMF->m_strPrjFileName.IsEmpty() )
		pCmdUI->Enable(FALSE);
	else
	{
		HTREEITEM hItem = m_wndFileView.GetSelectedItem();

		if( hItem == NULL)
			pCmdUI->Enable(TRUE);
		else
		{
			CTreeNodeDat *lpNode = (CTreeNodeDat *)m_wndFileView.GetItemData(hItem);
			if(lpNode->m_nType == FOLDER )
				pCmdUI->Enable(TRUE);
			else
				pCmdUI->Enable(FALSE);
		}
	}
}

void CFileView::OnExpandAllTree()
{
	// TODO: 在此添加命令处理程序代码
	m_wndFileView.OnExpandAllTree();
}

void CFileView::OnCollapseAllEntireTree()
{
	// TODO: 在此添加命令处理程序代码
	m_wndFileView.OnCollapseAllEntireTree();
}

void CFileView::OnNewWellDat()
{
	// TODO: 在此添加命令处理程序代码
	
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();

	int nImage = m_wndFileView.m_nCountImages - 19;

	HTREEITEM hRoot = AddTreeItem(_T("单井数据"), nImage + 1, nImage + 1, TVI_ROOT, TVI_FIRST);

	CTreeNodeDat *lpNodeDat = new CTreeNodeDat;
	
	lpNodeDat->m_nImage			= nImage + 1;
	lpNodeDat->m_nSelectedImage	= nImage + 1;
	lpNodeDat->m_nType			= WELL_DAT;
	lpNodeDat->m_pNodeDat		= NULL;
	lpNodeDat->m_uState			= TVIS_EXPANDED;

	m_wndFileView.SetItemData(hRoot, (DWORD)lpNodeDat);

	m_wndFileView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);

	//------------------------------------------------------------------------

	HTREEITEM hSrc = AddTreeItem(_T("井基础数据"), nImage + 2, nImage + 2, hRoot);

	lpNodeDat = new CTreeNodeDat;

	CFileViewObj *pObj = new CFileViewObj;
	
	pObj->m_strFileName = _T("WellInfo.dat");

	CString strFileName = pMF->GetProjectDatPath();
	strFileName += _T("\\files\\");
	strFileName += pObj->m_strFileName;

	CFile file(strFileName, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
	CArchive ar(&file, CArchive::store);

	int nRows = 0;
	int nCols = 7;
	long n=0;

	ar << nRows;
	ar << nCols;
	ar << n;

	CString str = _T("井号");
	ar << str;	
	str = _T("X");
	ar << str;
	str = _T("Y");
	ar << str;
	str = _T("补芯海拔");
	ar << str;
	str = _T("目前井别");
	ar << str;
	str = _T("油层顶深");
	ar << str;
	str = _T("油层底深");
	ar << str;

	ar.Close();
	file.Close();

	lpNodeDat->m_nImage			= nImage+2;
	lpNodeDat->m_nSelectedImage	= nImage+2;
	lpNodeDat->m_nType			= WELL_INFO;
	lpNodeDat->m_pNodeDat		= pObj;
	lpNodeDat->m_uState			= TVIS_EXPANDED;
	lpNodeDat->m_strFileName	= _T("WELLINFO.DAT");

	m_wndFileView.SetItemData(hSrc, (DWORD)lpNodeDat);

	//------------------------------------------------------------------------

	hSrc = AddTreeItem(_T("井眼轨迹数据"), nImage + 3, nImage + 3, hRoot);

	lpNodeDat = new CTreeNodeDat;
	pObj = new CFileViewObj;
	pObj->m_strFileName = _T("WellPathInfo.dat");
	
	lpNodeDat->m_nImage			= nImage + 3;
	lpNodeDat->m_nSelectedImage	= nImage + 3;
	lpNodeDat->m_nType			= WELL_HOLE;
	lpNodeDat->m_pNodeDat		= pObj;
	lpNodeDat->m_uState			= TVIS_EXPANDED;
	lpNodeDat->m_strFileName	= _T("WELLPATHINFO.DAT");

	m_wndFileView.SetItemData(hSrc, (DWORD)lpNodeDat);

	//------------------------------------------------------------------------

	hSrc = AddTreeItem(_T("测井数据"), nImage + 4, nImage + 4, hRoot);

	lpNodeDat = new CTreeNodeDat;
	pObj = new CFileViewObj;
	pObj->m_strFileName = _T("WellLoggingInfo.dat");

	lpNodeDat->m_nImage			= nImage + 4;
	lpNodeDat->m_nSelectedImage	= nImage + 4;
	lpNodeDat->m_nType			= WELL_LOGING_INFO;
	lpNodeDat->m_pNodeDat		= pObj;
	lpNodeDat->m_uState			= TVIS_EXPANDED;
	lpNodeDat->m_strFileName	= _T("WELLLOGGINGINFO.DAT");

	m_wndFileView.SetItemData(hSrc, (DWORD)lpNodeDat);

	//------------------------------------------------------------------------

	hSrc = AddTreeItem(_T("纵向模型"), nImage + 16, nImage + 16, TVI_ROOT);

	lpNodeDat = new CTreeNodeDat;

	lpNodeDat->m_nImage			= nImage + 16;
	lpNodeDat->m_nSelectedImage	= nImage + 16;
	lpNodeDat->m_nType			= FILE_VERT_MODELS;
	lpNodeDat->m_pNodeDat		= NULL;
	lpNodeDat->m_uState			= TVIS_EXPANDED;

	m_wndFileView.SetItemData(hSrc, (DWORD)lpNodeDat);
	{
		TCHAR szCurPath[MAX_PATH];  
		memset(szCurPath,   0,   MAX_PATH);  
		GetModuleFileName(NULL,   szCurPath,   sizeof(szCurPath)/sizeof(TCHAR));   
		CString strMoudlePath = szCurPath;
		strMoudlePath = strMoudlePath.Left(strMoudlePath.ReverseFind('\\')+1);
		CString strFileName = strMoudlePath;
		strFileName += _T("Template\\models.dat");
		if(PathFileExists(strFileName))
		{
			CFile file(strFileName, CFile::modeRead|CFile::typeBinary);
			CArchive ar(&file, CArchive::load);
			
			std::vector<CGraphProps*> graphs;
			long graphCount;

			ar >> graphCount;
			
			for (int i = 0; i < graphCount; i++)
			{
				CGraphProps* pGraph = new CGraphProps;
				pGraph->Serialize(ar);
				graphs.push_back(pGraph);
			}
			
			ar.Close();
			file.Close();

			for (int i = 0; i < graphCount; i++)
			{
				if (graphs[i] != NULL)
				{
					HTREEITEM hItem = AddTreeItem(graphs[i]->GetTitle(), nImage + 17, nImage + 17, hSrc);

					lpNodeDat = new CTreeNodeDat;

					lpNodeDat->m_nImage			= nImage + 17;
					lpNodeDat->m_nSelectedImage	= nImage + 17;
					lpNodeDat->m_nType			= FILE_VERT_MODEL;
					lpNodeDat->m_pNodeDat		= NULL;
					lpNodeDat->m_uState			= TVIS_EXPANDED;

					m_wndFileView.SetItemData(hItem, (DWORD)lpNodeDat);

					delete graphs[i];
					graphs[i] = NULL;
				}
			}			
		}
	}
	//------------------------------------------------------------------------

	m_wndFileView.Expand(hRoot, TVE_EXPAND);
}

void CFileView::OnUpdateNewWellDat(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码

	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	if( pMF->m_strPrjFileName.IsEmpty() )
		pCmdUI->Enable(FALSE);
	else
	{
		HTREEITEM hRoot = m_wndFileView.GetChildNode(TVI_ROOT, _T("单井数据"));

		if( hRoot == NULL)
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
}

void CFileView::OnImportPoint()
{
	// TODO: 在此添加命令处理程序代码

	CFileDialog dlg(TRUE, 0, 0, 4|2, _T("所有文件(*.*)|*.*||"));
	if( dlg.DoModal() == IDOK )
	{
		CString strSourcePathName = dlg.GetPathName();		
		CString strFileTitle = dlg.GetFileTitle();
		CString strExt = dlg.GetFileExt();
		strExt.MakeLower();

		CString strFileName = newGUID();
		strFileName += _T(".dat");

		CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
		CString strTargePathName = pMF->GetProjectDatPath();
		strTargePathName += _T("\\files\\");
		strTargePathName += strFileName;

		TCHAR cSeq;
		if( strExt == _T("csv"))
			cSeq = ',';
		else
			cSeq = '	';

		CCSVFile txtFile(strSourcePathName);
		
		CStringArray fileDat;
		CStringArray arr;
		while( txtFile.ReadData(arr, cSeq) )
		{
			int size = arr.GetSize();
			if( size > 4 )
				size = 4;

			CString strTmp1 = arr[0];
			strTmp1.Trim();
			if( strTmp1.IsEmpty() )
				continue;
			
			if( !StrIsValidFloat(strTmp1)) // 第一个不是浮点数
				continue;

			if( size < 2 ) // 不够两个数
				continue;

			CString strTmp2 = arr[1];
			strTmp2.Trim();

			if( !StrIsValidFloat(strTmp2)) // 第二个不是浮点数
				continue;

			fileDat.Add(strTmp1);
			fileDat.Add(strTmp2);

			CString strTmp3;
			if( size == 3 )
			{
				strTmp3 = arr[2];
				strTmp3.Trim();

				if( !StrIsValidFloat(strTmp3)) // 第二个不是浮点数
					strTmp3 = _T("-99999");
			}
			else
				strTmp3 = _T("-99999");

			fileDat.Add(strTmp3);

			CString strTmp4;
			if( size == 4 )
			{
				strTmp4 = arr[3];
				strTmp4.Trim();
			}
			else
				strTmp4 = _T("");

			fileDat.Add(strTmp4);
			
			arr.RemoveAll();
		}
		txtFile.Close();

		int nRows = fileDat.GetSize()/4;
		int nCols = 4;
		CFile file(strTargePathName,CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
		CArchive ar(&file, CArchive::store);

		ar << nRows;
		ar << nCols;		
		long n = (nRows*nCols); 
		for (long i=0;i<n;i++)
		{
			CString strTmp = fileDat[i];
			ar << strTmp;
		}

		ar.Close();
		file.Close();

		//CopyFile(strSourcePathName, strTargePathName, FALSE);

		HTREEITEM hItem = m_wndFileView.GetSelectedItem();
		if( hItem == NULL )
			hItem = TVI_ROOT;

		int nImage = m_wndFileView.m_nCountImages - 19;

		HTREEITEM h = AddTreeItem(strFileTitle, nImage + 9, nImage + 9, hItem);

		CTreeNodeDat *lpNodeDat = new CTreeNodeDat;

		CFileViewObj *pObj = new CFileViewObj;
		pObj->m_strFileName = strFileName;

		lpNodeDat->m_nImage			= nImage + 9;
		lpNodeDat->m_nSelectedImage	= nImage + 9;
		lpNodeDat->m_nType			= FILE_POINT;
		lpNodeDat->m_pNodeDat		= pObj;
		lpNodeDat->m_uState			= TVIS_EXPANDED;
		strFileName.MakeUpper();
		lpNodeDat->m_strFileName = strFileName;

		m_wndFileView.SetItemData(h, (DWORD)lpNodeDat);

		m_wndFileView.Expand(hItem, TVE_EXPAND);

		m_wndFileView.EditLabel(h);
	}
}

void CFileView::OnUpdateImportPoint(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();

	if( pMF->m_strPrjFileName.IsEmpty() )
		pCmdUI->Enable(FALSE);
	else
	{
		HTREEITEM hItem = m_wndFileView.GetSelectedItem();

		if( hItem != NULL)
		{
			CTreeNodeDat *pNote = (CTreeNodeDat *)m_wndFileView.GetItemData(hItem);
			if(pNote->m_nType == FOLDER)
			{
				pCmdUI->Enable(TRUE);
			}
			else
				pCmdUI->Enable(FALSE);
		}
		else
			pCmdUI->Enable(TRUE);
	}
}

void CFileView::OnImportLine()
{
	// TODO: 在此添加命令处理程序代码

	CFileDialog dlg(TRUE, 0, 0, 4|2, _T("所有文件(*.*)|*.*||"));
	if( dlg.DoModal() == IDOK )
	{	
		CString strSourcePathName = dlg.GetPathName();		
		CString strFileTitle = dlg.GetFileTitle();
		CString strExt = dlg.GetFileExt();
		strExt.MakeLower();

		CString strFileName = newGUID();
		strFileName += _T(".dat");

		CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
		CString strTargePathName = pMF->GetProjectDatPath();
		strTargePathName += _T("\\files\\");
		strTargePathName += strFileName;

		TCHAR cSeq;
		if( strExt == _T("csv"))
			cSeq = ',';
		else
			cSeq = '	';

		CCSVFile txtFile(strSourcePathName);

		CStringArray fileDat;
		CStringArray arr;
		while( txtFile.ReadData(arr, cSeq) )
		{
			int size = arr.GetSize();
			if( size < 1 )
				continue;

			if( size > 4 )
				size = 4;

			CString strTmp1 = arr[0];
			strTmp1.Trim();
			if( strTmp1.IsEmpty() )
				continue;

			strTmp1.MakeUpper();

			if( strTmp1 == _T("END")
				|| strTmp1 == _T("EOM"))
				strTmp1 = _T("EOM");

			if( strTmp1 == _T("EOM"))
			{
				fileDat.Add(strTmp1);
				fileDat.Add(_T(""));
				fileDat.Add(_T(""));
				fileDat.Add(_T(""));
				continue;
			}

			if( !StrIsValidFloat(strTmp1)) // 第一个不是浮点数
				continue;

			if( size < 2 ) // 不够两个数
				continue;

			CString strTmp2 = arr[1];
			strTmp2.Trim();

			if( !StrIsValidFloat(strTmp2)) // 第二个不是浮点数
				continue;

			fileDat.Add(strTmp1);
			fileDat.Add(strTmp2);

			CString strTmp3;
			if( size == 3 )
			{
				strTmp3 = arr[2];
				strTmp3.Trim();
				if( !StrIsValidFloat(strTmp3)) // 第二个不是浮点数
					strTmp3 = _T("-99999");
			}
			else
				strTmp3 = _T("-99999");

			fileDat.Add(strTmp3);

			CString strTmp4;
			if( size == 4)
			{
				strTmp4 = arr[3];
				strTmp4.Trim();
			}
			else
				strTmp4 = _T("");

			fileDat.Add(strTmp4);

			arr.RemoveAll();
		}
		txtFile.Close();

		int nRows = fileDat.GetSize()/4;
		int nCols = 4;
		CFile file(strTargePathName,CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
		CArchive ar(&file, CArchive::store);

		ar << nRows;
		ar << nCols;		
		long n = (nRows*nCols); 
		for (long i=0;i<n;i++)
		{
			CString strTmp = fileDat[i];
			ar << strTmp;
		}

		ar.Close();
		file.Close();

		HTREEITEM hItem = m_wndFileView.GetSelectedItem();
		if( hItem == NULL )
			hItem = TVI_ROOT;

		int nImage = m_wndFileView.m_nCountImages - 19;

		HTREEITEM h = AddTreeItem(strFileTitle, nImage + 10, nImage + 10, hItem);

		CTreeNodeDat *lpNodeDat = new CTreeNodeDat;

		CFileViewObj *pObj = new CFileViewObj;
		pObj->m_strFileName = strFileName;

		lpNodeDat->m_nImage			= nImage + 10;
		lpNodeDat->m_nSelectedImage	= nImage + 10;
		lpNodeDat->m_nType			= FILE_LINE;
		lpNodeDat->m_pNodeDat		= pObj;
		lpNodeDat->m_uState			= TVIS_EXPANDED;
		strFileName.MakeUpper();
		lpNodeDat->m_strFileName = strFileName;


		m_wndFileView.SetItemData(h, (DWORD)lpNodeDat);

		m_wndFileView.Expand(hItem, TVE_EXPAND);

		m_wndFileView.EditLabel(h);
	}
}

void CFileView::OnUpdateImportLine(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();

	if( pMF->m_strPrjFileName.IsEmpty() )
		pCmdUI->Enable(FALSE);
	else
	{
		HTREEITEM hItem = m_wndFileView.GetSelectedItem();

		if( hItem != NULL)
		{
			CTreeNodeDat *pNote = (CTreeNodeDat *)m_wndFileView.GetItemData(hItem);
			if(pNote->m_nType == FOLDER)
			{
				pCmdUI->Enable(TRUE);
			}
			else
				pCmdUI->Enable(FALSE);
		}
		else
			pCmdUI->Enable(TRUE);
	}
}

void CFileView::OnImportPolygon()
{
	// TODO: 在此添加命令处理程序代码

	CFileDialog dlg(TRUE, 0, 0, 4|2, _T("所有文件(*.*)|*.*||"));
	if( dlg.DoModal() == IDOK )
	{
		CString strSourcePathName = dlg.GetPathName();		
		CString strFileTitle = dlg.GetFileTitle();
		CString strExt = dlg.GetFileExt();
		strExt.MakeLower();

		CString strFileName = newGUID();
		strFileName += _T(".dat");

		CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
		CString strTargePathName = pMF->GetProjectDatPath();
		strTargePathName += _T("\\files\\");
		strTargePathName += strFileName;

		TCHAR cSeq;
		if( strExt == _T("csv"))
			cSeq = ',';
		else
			cSeq = '	';

		CCSVFile txtFile(strSourcePathName);

		CStringArray fileDat;
		CStringArray arr;
		while( txtFile.ReadData(arr, cSeq) )
		{
			int size = arr.GetSize();
			if( size < 1 )
				continue;

			if( size > 4 )
				size = 4;

			CString strTmp1 = arr[0];
			strTmp1.Trim();
			if( strTmp1.IsEmpty() )
				continue;

			strTmp1.MakeUpper();

			if( strTmp1 == _T("END")
				|| strTmp1 == _T("EOM"))
				strTmp1 = _T("EOM");

			if( strTmp1 == _T("EOM"))
			{
				fileDat.Add(strTmp1);
				fileDat.Add(_T(""));
				fileDat.Add(_T(""));
				fileDat.Add(_T(""));
				continue;
			}

			if( !StrIsValidFloat(strTmp1)) // 第一个不是浮点数
				continue;

			if( size < 2 ) // 不够两个数
				continue;

			CString strTmp2 = arr[1];
			strTmp2.Trim();

			if( !StrIsValidFloat(strTmp2)) // 第二个不是浮点数
				continue;

			fileDat.Add(strTmp1);
			fileDat.Add(strTmp2);

			CString strTmp3;
			if( size == 3 )
			{
				strTmp3 = arr[2];
				strTmp3.Trim();

				if( !StrIsValidFloat(strTmp3)) // 第二个不是浮点数
					strTmp3 = _T("-99999");
			}
			else
				strTmp3 = _T("-99999");

			fileDat.Add(strTmp3);

			CString strTmp4;
			if( size == 4)
			{
				strTmp4 = arr[3];
				strTmp4.Trim();
			}
			else
				strTmp4 = _T("");

			fileDat.Add(strTmp4);

			arr.RemoveAll();
		}
		txtFile.Close();

		int nRows = fileDat.GetSize()/4;
		int nCols = 4;
		CFile file(strTargePathName,CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
		CArchive ar(&file, CArchive::store);

		ar << nRows;
		ar << nCols;		
		long n = (nRows*nCols); 
		for (long i=0;i<n;i++)
		{
			CString strTmp = fileDat[i];
			ar << strTmp;
		}

		ar.Close();
		file.Close();

		HTREEITEM hItem = m_wndFileView.GetSelectedItem();
		if( hItem == NULL )
			hItem = TVI_ROOT;

		int nImage = m_wndFileView.m_nCountImages - 19;

		HTREEITEM h = AddTreeItem(strFileTitle, nImage + 11, nImage + 11, hItem);

		CTreeNodeDat *lpNodeDat = new CTreeNodeDat;

		CFileViewObj *pObj = new CFileViewObj;
		pObj->m_strFileName = strFileName;

		lpNodeDat->m_nImage			= nImage + 11;
		lpNodeDat->m_nSelectedImage	= nImage + 11;
		lpNodeDat->m_nType			= FILE_POLYGON;
		lpNodeDat->m_pNodeDat		= pObj;
		lpNodeDat->m_uState			= TVIS_EXPANDED;
		strFileName.MakeUpper();
		lpNodeDat->m_strFileName = strFileName;


		m_wndFileView.SetItemData(h, (DWORD)lpNodeDat);

		m_wndFileView.Expand(hItem, TVE_EXPAND);

		m_wndFileView.EditLabel(h);
	}
}

void CFileView::OnUpdateImportPolygon(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();

	if( pMF->m_strPrjFileName.IsEmpty() )
		pCmdUI->Enable(FALSE);
	else
	{
		HTREEITEM hItem = m_wndFileView.GetSelectedItem();

		if( hItem != NULL)
		{
			CTreeNodeDat *pNote = (CTreeNodeDat *)m_wndFileView.GetItemData(hItem);
			if(pNote->m_nType == FOLDER)
			{
				pCmdUI->Enable(TRUE);
			}
			else
				pCmdUI->Enable(FALSE);
		}
		else
			pCmdUI->Enable(TRUE);
	}
}

void CFileView::OnImportPlane()
{
	// TODO: 在此添加命令处理程序代码
	CFileDialog dlg(TRUE, 0, 0, 4|2, _T("所有文件(*.*)|*.*||"));
	if( dlg.DoModal() == IDOK )
	{
		CString strSourcePathName = dlg.GetPathName();		
		CString strFileTitle = dlg.GetFileTitle();
		CString strExt = dlg.GetFileExt();
		strExt.MakeLower();

		CString strFileName = newGUID();
		strFileName += _T(".dat");

		CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
		CString strTargePathName = pMF->GetProjectDatPath();
		strTargePathName += _T("\\files\\");
		strTargePathName += strFileName;

		TCHAR cSeq;
		if( strExt == _T("csv"))
			cSeq = ',';
		else
			cSeq = '	';

		CCSVFile txtFile(strSourcePathName);

		CStringArray fileDat;
		CStringArray arr;
		while( txtFile.ReadData(arr, cSeq) )
		{
			int size = arr.GetSize();
			if( size < 1 )
				continue;

			if( size > 4 )
				size = 4;

			CString strTmp1 = arr[0];
			strTmp1.Trim();
			if( strTmp1.IsEmpty() )
				continue;

			strTmp1.MakeUpper();

			if( strTmp1 == _T("COORDINATE")
				|| strTmp1 == _T("POINT")
				|| strTmp1 == _T("COORD")
				|| strTmp1 == _T("VERT")
				|| strTmp1 == _T("VERTEX"))
				strTmp1 = _T("Coordinate");

			if( strTmp1 == _T("Coordinate"))
			{
				fileDat.Add(strTmp1);
				fileDat.Add(_T(""));
				fileDat.Add(_T(""));
				fileDat.Add(_T(""));
				continue;
			}

			if( strTmp1 == _T("CoordIndex")
				|| strTmp1 == _T("INDEX")
				|| strTmp1 == _T("VERTEXINDEX")
				|| strTmp1 == _T("VERTINDEX"))
				strTmp1 = _T("CoordIndex");

			if( strTmp1 == _T("CoordIndex"))
			{
				fileDat.Add(strTmp1);
				fileDat.Add(_T(""));
				fileDat.Add(_T(""));
				fileDat.Add(_T(""));
				continue;
			}

			if( !StrIsValidFloat(strTmp1)) // 第一个不是浮点数
				continue;

			if( size < 3 ) // 不够两个数
				continue;

			CString strTmp2 = arr[1];
			strTmp2.Trim();

			if( !StrIsValidFloat(strTmp2)) // 第二个不是浮点数
				continue;

			CString strTmp3 = arr[2];
			strTmp3.Trim();

			if( !StrIsValidFloat(strTmp3)) // 第二个不是浮点数
				continue;

			CString strTmp4;
			if( size == 4 )
			{
				strTmp4 = arr[3];
				strTmp4.Trim();
			}
			else
				strTmp4 = _T("");

			fileDat.Add(strTmp1);
			fileDat.Add(strTmp2);
			fileDat.Add(strTmp3);
			fileDat.Add(strTmp4);
			
			arr.RemoveAll();
		}
		txtFile.Close();

		int nRows = fileDat.GetSize()/4;
		int nCols = 4;
		CFile file(strTargePathName,CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
		CArchive ar(&file, CArchive::store);

		ar << nRows;
		ar << nCols;
		long n = (nRows*nCols); 
		for (long i=0;i<n;i++)
		{
			CString strTmp = fileDat[i];
			ar << strTmp;
		}

		ar.Close();
		file.Close();

		HTREEITEM hItem = m_wndFileView.GetSelectedItem();
		if( hItem == NULL )
			hItem = TVI_ROOT;

		int nImage = m_wndFileView.m_nCountImages - 19;

		HTREEITEM h = AddTreeItem(strFileTitle, nImage + 12, nImage + 12, hItem);

		CTreeNodeDat *lpNodeDat = new CTreeNodeDat;

		CFileViewObj *pObj = new CFileViewObj;
		pObj->m_strFileName = strFileName;

		lpNodeDat->m_nImage			= nImage + 12;
		lpNodeDat->m_nSelectedImage	= nImage + 12;
		lpNodeDat->m_nType			= FILE_PLANE;
		lpNodeDat->m_pNodeDat		= pObj;
		lpNodeDat->m_uState			= TVIS_EXPANDED;
		strFileName.MakeUpper();
		lpNodeDat->m_strFileName = strFileName;

		m_wndFileView.SetItemData(h, (DWORD)lpNodeDat);

		m_wndFileView.Expand(hItem, TVE_EXPAND);

		m_wndFileView.EditLabel(h);
	}
}

void CFileView::OnUpdateImportPlane(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码

	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();

	if( pMF->m_strPrjFileName.IsEmpty() )
		pCmdUI->Enable(FALSE);
	else
	{
		HTREEITEM hItem = m_wndFileView.GetSelectedItem();

		if( hItem != NULL)
		{
			CTreeNodeDat *pNote = (CTreeNodeDat *)m_wndFileView.GetItemData(hItem);
			if(pNote->m_nType == FOLDER)
			{
				pCmdUI->Enable(TRUE);
			}
			else
				pCmdUI->Enable(FALSE);
		}
		else
			pCmdUI->Enable(TRUE);
	}
}

void CFileView::OnImportPictrue()
{
	// TODO: 在此添加命令处理程序代码
	CFileDialog dlg(TRUE, 0, 0, 4|2, _T("所有支持的图形文件(*.bmp;*.gif;*.jpg;*.jpeg;*.png;*.ico;*.tif;*.tiff;*.tga;*.pcx;*.wbmp;*.wmf;*.emf;*.j2k;*.jp2;*.jbg;*.j2c;*.jpc;*.pgx;*.pnm;*.pgm;*.ppm;*.ras)|*.bmp;*.gif;*.jpg;*.jpeg;*.png;*.ico;*.tif;*.tiff;*.tga;*.pcx;*.wbmp;*.wmf;*.emf;*.j2k;*.jp2;*.jbg;*.j2c;*.jpc;*.pgx;*.pnm;*.pgm;*.ppm;*.ras||"));
	if( dlg.DoModal() == IDOK )
	{
		CString strSourcePathName = dlg.GetPathName();
		CString strFileName = dlg.GetFileName();
		CString strFileTitle = dlg.GetFileTitle();

		CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
		CString strTargePathName = pMF->GetProjectDatPath();
		strTargePathName += _T("\\files\\");
		strTargePathName += strFileName;

		CopyFile(strSourcePathName, strTargePathName, FALSE);

		HTREEITEM hItem = m_wndFileView.GetSelectedItem();
		if( hItem == NULL )
			hItem = TVI_ROOT;

		int nImage = m_wndFileView.m_nCountImages - 19;

		HTREEITEM h = AddTreeItem(strFileTitle, nImage + 13, nImage + 13, hItem);

		CTreeNodeDat *lpNodeDat = new CTreeNodeDat;

		CFileViewObj *pObj = new CFileViewObj;
		pObj->m_strFileName = strFileName;

		lpNodeDat->m_nImage			= nImage + 13;
		lpNodeDat->m_nSelectedImage	= nImage + 13;
		lpNodeDat->m_nType			= FILE_PICTRUE;
		lpNodeDat->m_pNodeDat		= pObj;
		lpNodeDat->m_uState			= TVIS_EXPANDED;
		strFileName.MakeUpper();
		lpNodeDat->m_strFileName = strFileName;

		m_wndFileView.SetItemData(h, (DWORD)lpNodeDat);

		m_wndFileView.Expand(hItem, TVE_EXPAND);

		m_wndFileView.EditLabel(h);
	}
}

void CFileView::OnUpdateImportPictrue(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码

	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();

	if( pMF->m_strPrjFileName.IsEmpty() )
		pCmdUI->Enable(FALSE);
	else
	{
		HTREEITEM hItem = m_wndFileView.GetSelectedItem();

		if( hItem != NULL)
		{
			CTreeNodeDat *pNote = (CTreeNodeDat *)m_wndFileView.GetItemData(hItem);
			if(pNote->m_nType == FOLDER)
			{
				pCmdUI->Enable(TRUE);
			}
			else
				pCmdUI->Enable(FALSE);
		}
		else
			pCmdUI->Enable(TRUE);
	}
}

void CFileView::OnImportTrack()
{
	// TODO: 在此添加命令处理程序代码
	//CFileDialog dlg;
	HTREEITEM hItem = m_wndFileView.GetSelectedItem();
	CString strTitle = m_wndFileView.GetItemText(hItem);
	CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)m_wndFileView.GetItemData(hItem);
	OpenTree(lpNodeDat, (LPVOID)(LPCSTR)strTitle);
}

void CFileView::OnUpdateImportTrack(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	if( pMF->m_strPrjFileName.IsEmpty() )
		pCmdUI->Enable(FALSE);
	else
	{
		HTREEITEM hRoot = m_wndFileView.GetChildNode(TVI_ROOT, _T("单井数据"));

		if( hRoot != NULL)
		{
			HTREEITEM hItem = m_wndFileView.GetChildNode(hRoot, _T("井眼轨迹数据"));
			if( hRoot != NULL )
			{
				pCmdUI->Enable(TRUE);
			}
			else
				pCmdUI->Enable(FALSE);
		}
		else
			pCmdUI->Enable(FALSE);
	}
}

//________________________________________________________________________
// 删除节点
void CFileView::OnEditClear()
{
	// TODO: 在此添加命令处理程序代码
	m_wndFileView.OnEditClear();
}

void CFileView::OnUpdateEditClear(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	m_wndFileView.OnUpdateEditClear(pCmdUI);
}

// 复制节点

void CFileView::OnEditCopy()
{
	// TODO: 在此添加命令处理程序代码
	m_wndFileView.OnEditCopy();
}

void CFileView::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	m_wndFileView.OnUpdateEditCopy(pCmdUI);
}

void CFileView::OnEditCut()
{
	// TODO: 在此添加命令处理程序代码
	m_wndFileView.OnEditCut();
}

void CFileView::OnUpdateEditCut(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	m_wndFileView.OnUpdateEditCut(pCmdUI);
}

void CFileView::OnEditPaste()
{
	// TODO: 在此添加命令处理程序代码
	m_wndFileView.OnEditPaste();
}

void CFileView::OnUpdateEditPaste(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	m_wndFileView.OnUpdateEditPaste(pCmdUI);
}

void CFileView::OnExportTriangle()
{
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	CString strSourcePathName = pMF->GetProjectDatPath();
	CDlgTrangleExport dlg;
	dlg.m_x = 1;
	dlg.m_y = 1;
	dlg.m_z = 1;
	dlg.m_strFileName = strSourcePathName + "\\default.tri";
	if(dlg.DoModal()==IDOK)
	{
		CString filename = dlg.m_strFileName;
		double _x = dlg.m_x;
		double _y = dlg.m_y;
		double _z = dlg.m_z;
		int index = dlg.m_index;
		CWaitCursor wait;

		HTREEITEM hItem = m_wndFileView.GetSelectedItem();

		if(!hItem)
		{
			return;
		}

		CTreeNodeDat *lpData = (CTreeNodeDat *)m_wndFileView.GetItemData(hItem);
		
		CString strFileName = ((CFileViewObj*)lpData->m_pNodeDat)->m_strFileName;
		strSourcePathName += _T("\\files\\");
		strSourcePathName += strFileName;

		CPlaneReader* pReader = new CPlaneReader(strSourcePathName);
		C3DObject* pPlaneObj = new C3DObject(pReader);

		CString strFileName1 = pMF->GetProjectDatPath();
		strFileName1 += _T("\\models\\");
		CString strNewtempFileName;
		strNewtempFileName = strFileName1 + newGUID();


		pPlaneObj->SaveSurface(strNewtempFileName.GetString());
		pPlaneObj->SaveDivideSurface(strNewtempFileName.GetBuffer(), filename.GetBuffer(), CVector3DF(_x, _y, _z), index );

		TRY
		{
			CFile::Remove( strNewtempFileName.GetBuffer() );
		}
		CATCH( CFileException, e )
		{
			assert(false&&"删除文件失败");
		}
		END_CATCH 
	}
}

void CFileView::OnUpdateExportTriangle( CCmdUI *pCmdUI )
{
	HTREEITEM hti;
	CTreeNodeDat *lpNodeDat ;
	hti = m_wndFileView.GetSelectedItem();
	if(hti)
		lpNodeDat = (CTreeNodeDat*)m_wndFileView.GetItemData(hti);
	if( hti != NULL )
	{
		if( lpNodeDat->m_nType == FILE_PLANE ) 
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
	else
		pCmdUI->Enable(FALSE);
}
