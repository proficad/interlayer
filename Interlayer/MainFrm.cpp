
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "Interlayer.h"
#include "UndoBar.h"
#include "MainFrm.h"
#include "./GeoMap/GeoMapView.h"
#include "Vertical_Model/VertModelView.h"
#include "2DModelView.h"
#include "DlgWithGraph.h"
#include "DlgLayerParamSet.h"
#include "3DLib/3DModelView.h"
#include "GenerateLayerDoc.h"
#include "FormGenerateLayer.h"
#include "DispLogCurve/DispLogCurve.h"
#include "DlgInterlayerParaRange.h"
#include "IntersectSearchManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern void CopyDirectory( LPCSTR lpsrcPath, LPCSTR lpdstPath );
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnUpdateApplicationLook)	
	ON_COMMAND(ID_FILE_NEW_PRJ, &CMainFrame::OnFileNewPrj)
	ON_COMMAND(ID_FILE_OPEN_PRJ, &CMainFrame::OnFileOpenPrj)
	ON_COMMAND(ID_FILE_CLOSE_PRJ, &CMainFrame::OnFileClosePrj)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_PRJ, &CMainFrame::OnUpdateFileSavePrj)
	ON_COMMAND(ID_FILE_SAVE_PRJ, &CMainFrame::OnFileSavePrj)
	ON_COMMAND(ID_FILE_SAVE_AS_PRJ, &CMainFrame::OnFileSaveAsPrj)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS_PRJ, &CMainFrame::OnUpdateFileSaveAsPrj) 
	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE_PRJ, &CMainFrame::OnUpdateFileClosePrj)
	ON_COMMAND(ID_FILE_NEW, &CMainFrame::OnFileNew)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, &CMainFrame::OnUpdateFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CMainFrame::OnFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, &CMainFrame::OnUpdateFileOpen)
	ON_COMMAND(ID_FILE_SAVE_ALL, &CMainFrame::OnFileSaveAll)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_ALL, &CMainFrame::OnUpdateFileSaveAll)
	ON_WM_CLOSE()
	ON_COMMAND(ID_Vertical_Model, &CMainFrame::OnVerticalModel)
	ON_COMMAND(ID_Generate_Layer, &CMainFrame::OnGenerateLayer)
	ON_COMMAND(ID_INTERLAYER_SPACE_SET, &CMainFrame::OnInterlayerSpaceSet)
	ON_COMMAND(ID_PHYPARARANGESET, &CMainFrame::OnPhyPararangeSet)
	ON_MESSAGE(WM_GETITEMTITLE_FILE,OnGetItemTitleOfFile)
	ON_MESSAGE(WM_GETITEMTITLE_MODEL,OnGetItemTitleOfModel)
	ON_MESSAGE(WM_GETITEMTITLE_GRAPH,OnGetItemTitleOfGraph)

	ON_MESSAGE(WM_CREATE_FILE_LOG, OnCreateFileLog)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_PRESS,		// 进度条
	ID_INDICATOR_CENTER,	// 坐标
	ID_INDICATOR_COORD,		// 坐标
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_BLUE);

	m_strPrjFileName = _T("");
}

CMainFrame::~CMainFrame()
{
}

// 设置
void CMainFrame::OnInterlayerSpaceSet()
{
	// TODO: 在此添加命令处理程序代码
	CDlgLayerParamSet dlg;
	if( dlg.DoModal() == IDOK )
 	{
		double dLayers,dNormals,dVerts;
		CString strTmp;
		strTmp = AfxGetApp()->GetProfileString(_T("Settings"),_T("Layers"),strTmp);
		dLayers = atof(strTmp);

		strTmp = AfxGetApp()->GetProfileString(_T("Settings"),_T("Normals"),strTmp);
		dNormals = atof(strTmp);

		strTmp = AfxGetApp()->GetProfileString(_T("Settings"),_T("Verts"),strTmp);
		dVerts = atof(strTmp);

		CMDIChildWndEx *pWnd =(CMDIChildWndEx *) MDIGetActive();
		if( !pWnd )
			return;

		CView *pView = pWnd->GetActiveView();
		if(!pView)
			return;

		if(!pView->IsKindOf(RUNTIME_CLASS(CFormGenerateLayer)))
			return;

		CGenerateLayerDoc *pDoc = (CGenerateLayerDoc *)pView ->GetDocument();

		pDoc->m_layerModel.m_dHorzSpace = dLayers;
		pDoc->m_layerModel.m_dNormalSpace = dNormals;
		pDoc->m_layerModel.m_dVertSpace = dVerts;

		pDoc->SetModifiedFlag();
 	}
}
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;
	// 基于持久值设置视觉管理器和样式
	OnApplicationLook(theApp.m_nAppLook);

	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // 其他可用样式...
	mdiTabParams.m_bActiveTabCloseButton = TRUE;      // 设置为 FALSE 会将关闭按钮放置在选项卡区域的右侧
	mdiTabParams.m_bTabIcons = FALSE;    // 设置为 TRUE 将在 MDI 选项卡上启用文档图标
	mdiTabParams.m_bAutoColor = TRUE;    // 设置为 FALSE 将禁用 MDI 选项卡的自动着色
	mdiTabParams.m_bDocumentMenu = TRUE; // 在选项卡区域的右边缘启用文档菜单
	EnableMDITabbedGroups(TRUE, mdiTabParams);

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("未能创建菜单栏\n");
		return -1;      // 未能创建
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// 防止菜单栏在激活时获得焦点
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC,
		CRect(1, 1, 1,1), 1) ||
		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}
	
	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	//////////////////////////////////////////////////////////////////////////
	if (!m_wnd3DModelToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD /*| WS_VISIBLE*/ | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC,
		CRect(1,1,1,1), 2) ||
		!m_wnd3DModelToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_TOOLBAR_MODEL_256 : IDR_TOOLBAR_MODEL))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}


	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_MODEL);
	ASSERT(bNameValid);
	m_wnd3DModelToolBar.SetWindowText(strToolBarName);

	m_wnd3DModelToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	//////////////////////////////////////////////////////////////////////////

	if (!m_wnd2DModelToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD /*| WS_VISIBLE*/ | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC,
		CRect(1,1,1,1), 3) ||
		!m_wnd2DModelToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_TOOLBAR_2DVIEW_256 : IDR_TOOLBAR_2DVIEW))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}

	CMFCToolBarEditBoxButton editButton (ID_EDIT_CELL,
		afxCommandManager->GetCmdImage(ID_EDIT_CELL, FALSE),
		ES_AUTOHSCROLL, 150);
	m_wnd2DModelToolBar.ReplaceButton (ID_EDIT_CELL, editButton);

	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_2DVIEW);
	ASSERT(bNameValid);
	m_wnd2DModelToolBar.SetWindowText(strToolBarName);

	m_wnd2DModelToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	//_______________________________________________________________________
	// 
	if (!m_wndToolRect.Create(this,
		WS_CHILD|CBRS_TOP|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_HIDE_INPLACE|CBRS_SIZE_DYNAMIC|
		CBRS_GRIPPER | CBRS_BORDER_3D) || 
		!m_wndToolRect.LoadToolBar (IDR_TOOLBAR_RECT_BASE_OBJ))
	{
		return -1;      // fail to create
	}

	if (!m_wndToolSort.Create(this,
		WS_CHILD|CBRS_TOP|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_HIDE_INPLACE|CBRS_SIZE_DYNAMIC|
		CBRS_GRIPPER | CBRS_BORDER_3D) || 
		!m_wndToolSort.LoadToolBar (IDR_TOOLBAR_ELEMENTSORT))
	{
		return -1;      // fail to create
	}

	if (!m_wndToolCurve.Create(this,
		WS_CHILD|CBRS_TOP|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_HIDE_INPLACE|CBRS_SIZE_DYNAMIC|
		CBRS_GRIPPER | CBRS_BORDER_3D) || 
		!m_wndToolCurve.LoadToolBar (IDR_TOOLBAR_CURVE_BASE_OBJ))
	{
		return -1;      // fail to create
	}	

	if (!m_wndToolZoom.Create(this,
		WS_CHILD|CBRS_TOP|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_HIDE_INPLACE|CBRS_SIZE_DYNAMIC|
		CBRS_GRIPPER | CBRS_BORDER_3D) || 
		!m_wndToolZoom.LoadToolBar (IDR_TOOLBAR_ZOOM))
	{
		return -1;      // fail to create
	}	

	if (!m_wndToolPoint.Create(this,
		WS_CHILD|CBRS_TOP|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_HIDE_INPLACE|CBRS_SIZE_DYNAMIC|
		CBRS_GRIPPER | CBRS_BORDER_3D) || 
		!m_wndToolPoint.LoadToolBar (IDR_TOOLBAR_POINT))
	{
		return -1;      // fail to create
	}	

	if (!m_wndToolAligin.Create(this,
		WS_CHILD|CBRS_TOP|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_HIDE_INPLACE|CBRS_SIZE_DYNAMIC|
		CBRS_GRIPPER | CBRS_BORDER_3D) || 
		!m_wndToolAligin.LoadToolBar (IDR_TOOLBAR_ALIGIN))
	{
		return -1;      // fail to create
	}

	if (!m_wndToolAttri.Create(this,
		WS_CHILD|CBRS_TOP|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_HIDE_INPLACE|CBRS_SIZE_DYNAMIC|
		CBRS_GRIPPER | CBRS_BORDER_3D) || 
		!m_wndToolAttri.LoadToolBar (IDR_TOOLBAR_ATTRI))
	{
		return -1;      // fail to create
	}

	if (!m_wndToolCtrlPt.Create(this,
		WS_CHILD|CBRS_TOP|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_HIDE_INPLACE|CBRS_SIZE_DYNAMIC|
		CBRS_GRIPPER | CBRS_BORDER_3D) || 
		!m_wndToolCtrlPt.LoadToolBar (IDR_TOOLBAR_CTRL_POINT))
	{
		return -1;      // fail to create
	}

	//-----------------------------------------------------------------------	
	if (!m_wndToolBarDraw.CreateEx(this, TBSTYLE_FLAT, WS_CHILD /*| WS_VISIBLE*/ | CBRS_LEFT
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, 
		CRect(1,1,1,1), 4) ||
		!m_wndToolBarDraw.LoadToolBar(IDR_TOOLBAR_DRAW))
	{
		return -1;      // fail to create
	}

	m_wndToolBarDraw.SetWindowText(_T("二维作图"));

	m_wndToolBarDraw.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	//-----------------------------------------------------------------------
	
	if (!m_wndFormatBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD /*| WS_VISIBLE*/ | CBRS_BOTTOM
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC,CRect(1,1,1,1), 5) ||
		!m_wndFormatBar.LoadToolBar(IDR_TOOLBAR_FORMAT))
	{
		return -1;      // fail to create
	}

	m_wndFormatBar.SetWindowText(_T("文字属性"));

	m_wndFormatBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	//-----------------------------------------------------------------------
	
	m_wndToolBarDraw.ReplaceButton (ID_DRAW_EDIT_UNDO, CUndoButton (ID_DRAW_EDIT_UNDO, _T("&Undo")));
	m_wndToolBarDraw.ReplaceButton (ID_DRAW_EDIT_REDO, CUndoButton (ID_DRAW_EDIT_REDO, _T("&Redo")));

	//-----------------------------------
	// Replace dropdown resources button:
	//-----------------------------------
	m_wndToolBarDraw.ReplaceButton (ID_DUMMY_CURVE,
		CMFCDropDownToolbarButton (_T("曲线"), &m_wndToolCurve));

	m_wndToolBarDraw.ReplaceButton (ID_DUMMY_ZOOM,
		CMFCDropDownToolbarButton (_T("放缩工具"), &m_wndToolZoom));

	m_wndToolBarDraw.ReplaceButton (ID_DUMMY_RECT,
		CMFCDropDownToolbarButton (_T("矩形工具"), &m_wndToolRect));

	m_wndToolBarDraw.ReplaceButton (ID_DUMMY_ELEMENTSORT,
		CMFCDropDownToolbarButton (_T("图素排序"), &m_wndToolSort));

	m_wndToolBarDraw.ReplaceButton (ID_DUMMY_POINT,
		CMFCDropDownToolbarButton (_T("曲线操作"), &m_wndToolPoint));

	m_wndToolBarDraw.ReplaceButton (ID_DUMMY_ALIGIN,
		CMFCDropDownToolbarButton (_T("排列"), &m_wndToolAligin));

	m_wndToolBarDraw.ReplaceButton (ID_DUMMY_ATTRI,
		CMFCDropDownToolbarButton (_T("属性"), &m_wndToolAttri));

	m_wndToolBarDraw.ReplaceButton (ID_DUMMY_CTRL_POINT,
		CMFCDropDownToolbarButton (_T("坐标校正"), &m_wndToolCtrlPt));	

	//_______________________________________________________________________
	// 
	// 允许用户定义的工具栏操作:

	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	m_wndStatusBar.SetPaneInfo(1, ID_INDICATOR_PRESS, SBPS_NORMAL, 250);
	m_wndStatusBar.SetPaneInfo(2, ID_INDICATOR_CENTER, SBPS_NORMAL, 250);
	m_wndStatusBar.SetPaneInfo(3, ID_INDICATOR_COORD, SBPS_NORMAL, 250);

	// TODO: 如果您不希望工具栏和菜单栏可停靠，请删除这五行
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wnd3DModelToolBar.EnableDocking(CBRS_ALIGN_ANY);	
	m_wnd2DModelToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBarDraw.EnableDocking(CBRS_ALIGN_ANY);
	m_wndFormatBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);	
	DockPane(&m_wnd2DModelToolBar);
	//DockPane(&m_wndToolBarDraw);
	//DockPane(&m_wndToolBarDraw);
	DockPane(&m_wnd3DModelToolBar);
	DockPane(&m_wndFormatBar);	
	//DockPane(&m_wndFormatBar,AFX_IDW_DOCKBAR_BOTTOM);
	
	//DockPaneLeftOf(&m_wnd2DModelToolBar, &m_wnd3DModelToolBar);
	//DockPaneLeftOf(&m_wndToolBar, &m_wnd2DModelToolBar);
	DockPaneLeftOf(&m_wndToolBarDraw, &m_wndFormatBar);	

	// 启用 Visual Studio 2005 样式停靠窗口行为
	CDockingManager::SetDockingMode(DT_SMART);
	// 启用 Visual Studio 2005 样式停靠窗口自动隐藏行为
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 加载菜单项图像(不在任何标准工具栏上):
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	//_______________________________________________________________________
	// 创建停靠窗口
	//
	if (!CreateDockingWindows())
	{
		TRACE0("未能创建停靠窗口\n");
		return -1;
	}
	//_______________________________________________________________________
	//
	
	// 启用增强的窗口管理对话框
	EnableWindowsDialog(ID_WINDOW_MANAGER, IDS_WINDOWS_MANAGER, TRUE);

	// 启用工具栏和停靠窗口菜单替换
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// 启用快速(按住 Alt 拖动)工具栏自定义
	CMFCToolBar::EnableQuickCustomization();

	if (CMFCToolBar::GetUserImages() == NULL)
	{
		// 加载用户定义的工具栏图像
		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
		{
			m_UserImages.SetImageSize(CSize(16, 16), FALSE);
			CMFCToolBar::SetUserImages(&m_UserImages);
		}
	}

	// 启用菜单个性化(最近使用的命令)
	// TODO: 定义您自己的基本命令，确保每个下拉菜单至少有一个基本命令。
	CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_FILE_NEW);
	lstBasicCommands.AddTail(ID_FILE_OPEN);
	lstBasicCommands.AddTail(ID_FILE_SAVE);
	lstBasicCommands.AddTail(ID_FILE_PRINT);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_EDIT_CUT);
	lstBasicCommands.AddTail(ID_EDIT_PASTE);
	lstBasicCommands.AddTail(ID_EDIT_UNDO);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);
	lstBasicCommands.AddTail(ID_SORTING_SORTALPHABETIC);
	lstBasicCommands.AddTail(ID_SORTING_SORTBYTYPE);
	lstBasicCommands.AddTail(ID_SORTING_SORTBYACCESS);
	lstBasicCommands.AddTail(ID_SORTING_GROUPBYTYPE);

	CMFCToolBar::SetBasicCommands(lstBasicCommands);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	srand((int)time(0));
	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;

	// 创建模型视图
	CString strModelView;
	bNameValid = strModelView.LoadString(IDS_CLASS_VIEW);
	ASSERT(bNameValid);
	
	if (!m_wndModelView.Create(strModelView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_CLASSVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“模型视图”窗口\n");
		return FALSE; // 未能创建
	}

	// 创建文件视图
	CString strFileView;
	bNameValid = strFileView.LoadString(IDS_FILE_VIEW);
	ASSERT(bNameValid);
	if (!m_wndFileView.Create(strFileView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_FILEVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“文件视图”窗口\n");
		return FALSE; // 未能创建
	}

	// 创建图形窗口视图
	CString strGraphView;
	bNameValid = strGraphView.LoadString(IDS_GRAPH_VIEW);
	ASSERT(bNameValid);
	if (!m_wndGraphView.Create(strGraphView, 
		this, 
		CRect(0, 0, 200, 200), TRUE, 
		ID_VIEW_2DVIEW, 
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“图形窗口视图”窗口\n");
		return FALSE; // 未能创建
	}

	
	if (!m_wndToolBarLayer.Create (_T("图层管理"), this, CRect (0, 0, 200, 200),
		TRUE, 
		ID_VIEW_TOOL_LAYER,
		WS_CHILD | WS_VISIBLE| WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		return FALSE;      // fail to create
	}

	if (!m_wnd3DObjBar.Create (_T("对象管理"), this, CRect (0, 0, 200, 200),
		TRUE, 
		ID_VIEW_3D_OBJ,
		WS_CHILD | WS_VISIBLE| WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		return FALSE;      // fail to create
	}

	if (!m_wndSearchBar.Create (_T("套接管理"), this, CRect (0, 0, 200, 200),
		TRUE, 
		ID_VIEW_INTERSECT_SEARCH,
		WS_CHILD | WS_VISIBLE| WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		return FALSE;      // fail to create
	}

	SetDockingWindowIcons(theApp.m_bHiColorIcons);


	m_wndFileView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndModelView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndGraphView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBarLayer.EnableDocking(CBRS_ALIGN_ANY);
	m_wnd3DObjBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndSearchBar.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndFileView);
	DockPane(&m_wndToolBarLayer,AFX_IDW_DOCKBAR_RIGHT);
	DockPane(&m_wnd3DObjBar,AFX_IDW_DOCKBAR_RIGHT);
	DockPane(&m_wndSearchBar,AFX_IDW_DOCKBAR_RIGHT);
	CDockablePane* pTabbedBar = NULL;
	//m_wndGraphView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);
	m_wndGraphView.DockToWindow (&m_wndFileView, CBRS_ALIGN_BOTTOM);
	m_wndModelView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);
	m_wndToolBarLayer.EnableAutohideAll(TRUE);
	m_wndToolBarLayer.ToggleAutoHide();
	m_wnd3DObjBar.EnableAutohideAll(TRUE);
	m_wnd3DObjBar.ToggleAutoHide();
	m_wndSearchBar.EnableAutohideAll(TRUE);
	m_wndSearchBar.ToggleAutoHide();
	//m_wndSearchBar.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);

	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hFileViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_FILE_VIEW_HC : IDI_FILE_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndFileView.SetIcon(hFileViewIcon, FALSE);

	HICON hModelViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_MODEL_VIEW_HC : IDI_MODEL_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndModelView.SetIcon(hModelViewIcon, FALSE);

	HICON hGraphViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_GRAPH_VIEW_HC : IDI_GRAPH_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndGraphView.SetIcon(hGraphViewIcon, FALSE);

	HICON hLayerIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_LAYERBAR_HC : IDI_LAYERBAR), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndToolBarLayer.SetIcon(hLayerIcon, FALSE);

	HICON h3DObjIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_3D_OBJ_HC : IDI_3D_OBJ), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wnd3DObjBar.SetIcon(h3DObjIcon, FALSE);
	

	UpdateMDITabbedBarsIcons();
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* 扫描菜单*/);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CMDIFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// 基类将执行真正的工作

	if (!CMDIFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// 为所有用户工具栏启用自定义按钮
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}

// 关闭工程文件
BOOL CMainFrame::CloseProjectFile()
{
	BOOL bRet = FALSE;

	if( !m_strPrjFileName.IsEmpty() ) // 已经存在
	{		
		
		if( m_wndFileView.GetTreeCtrl()->IsModified()
			|| m_wndModelView.GetTreeCtrl()->IsModified()
			|| m_wndGraphView.GetTreeCtrl()->IsModified() )
		{
			int ret = MessageBox(_T("工程文件已经被修改是否保存？"), _T("提示信息"),
				MB_ICONQUESTION | MB_YESNOCANCEL );

			if( ret == IDYES )
			{
				AfxGetApp ()->WriteProfileString(_T("Settings"),_T("Project File"), m_strPrjFileName);
				theApp.AddToRecentFileList(m_strPrjFileName);

				CString strFileName = m_strPrjFileName;

				SaveProjectFile(strFileName);

				m_wndFileView.EmptyTree();
				m_wndModelView.EmptyTree();
				m_wndGraphView.EmptyTree();
				m_wndGraphView.FillGraphView();

				m_wndFileView.GetTreeCtrl()->EnableWindow(FALSE);
				m_wndModelView.GetTreeCtrl()->EnableWindow(FALSE);
				m_wndGraphView.GetTreeCtrl()->EnableWindow(FALSE);

				m_wndFileView.GetTreeCtrl()->SetModifiedFlag(false);
				m_wndModelView.GetTreeCtrl()->SetModifiedFlag(false);
				m_wndGraphView.GetTreeCtrl()->SetModifiedFlag(false);

				m_strPrjFileName = _T("");

				bRet = TRUE;
			}
			else if( ret == IDNO )
			{
				AfxGetApp ()->WriteProfileString(_T("Settings"),_T("Project File"), m_strPrjFileName);
				theApp.AddToRecentFileList(m_strPrjFileName);

				m_wndFileView.EmptyTree();
				m_wndModelView.EmptyTree();
				m_wndGraphView.EmptyTree();
				m_wndGraphView.FillGraphView();

				m_wndFileView.GetTreeCtrl()->EnableWindow(FALSE);
				m_wndModelView.GetTreeCtrl()->EnableWindow(FALSE);
				m_wndGraphView.GetTreeCtrl()->EnableWindow(FALSE);

				m_wndFileView.GetTreeCtrl()->SetModifiedFlag(false);
				m_wndModelView.GetTreeCtrl()->SetModifiedFlag(false);
				m_wndGraphView.GetTreeCtrl()->SetModifiedFlag(false);

				m_strPrjFileName = _T("");

				bRet = TRUE;
			}
			else
				bRet = FALSE;
		}
		else
		{
			AfxGetApp ()->WriteProfileString(_T("Settings"),_T("Project File"), m_strPrjFileName);
			theApp.AddToRecentFileList(m_strPrjFileName);

			m_wndFileView.EmptyTree();
			m_wndModelView.EmptyTree();
			m_wndGraphView.EmptyTree();
			m_wndGraphView.FillGraphView();

			m_wndFileView.GetTreeCtrl()->EnableWindow(FALSE);
			m_wndModelView.GetTreeCtrl()->EnableWindow(FALSE);
			m_wndGraphView.GetTreeCtrl()->EnableWindow(FALSE);

			m_wndFileView.GetTreeCtrl()->SetModifiedFlag(false);
			m_wndModelView.GetTreeCtrl()->SetModifiedFlag(false);
			m_wndGraphView.GetTreeCtrl()->SetModifiedFlag(false);

			m_strPrjFileName = _T("");

			bRet = TRUE;
		}
	}
	else
	{
		m_wndFileView.GetTreeCtrl()->SetModifiedFlag(false);
		m_wndModelView.GetTreeCtrl()->SetModifiedFlag(false);
		m_wndGraphView.GetTreeCtrl()->SetModifiedFlag(false);

		AfxGetApp ()->WriteProfileString(_T("Settings"),_T("Project File"), m_strPrjFileName);
		theApp.AddToRecentFileList(m_strPrjFileName);

		bRet = TRUE;
	}
	
	return bRet;
}

CString CMainFrame::GetProjectDatPath()
{
	CString strPathName = _T("");
	if( !m_strPrjFileName.IsEmpty() )
	{
		CSplitPath path(m_strPrjFileName);
		strPathName = path.GetDrive() + path.GetDirectory() + path.GetFileName();
		strPathName += _T("_dat");
	}

	return strPathName;
}

void CMainFrame::OnFileNewPrj()
{
	// TODO: 在此添加命令处理程序代码
	if( !theApp.SaveAllModified() )
		return;

	if( !CloseProjectFile() )
		return;

	theApp.CloseAllDocuments(TRUE);

	CFileDialog dlg(FALSE,_T("prj"),_T(""),4|2,_T("工程文件(*.Prj)|*.Prj||"));
	dlg.m_ofn.lpstrTitle = _T("新建工程文件");
	if(dlg.DoModal() == IDOK )
	{
		CString strFileName = dlg.GetPathName();
		CString strExt = dlg.GetFileExt();

		CString strPathName;
		if( !strExt.IsEmpty() )
			strPathName = strFileName.Left(strFileName.ReverseFind('.'));
		else
			strPathName = strFileName;

		strPathName += _T("_dat");

		SaveProjectFile(strFileName);		

		CreateDirectory(strPathName, NULL);

		m_strPrjFileName = strFileName;

		CString strTmp = strPathName;
		strTmp += _T("\\files");
		CreateDirectory(strTmp, NULL);

		strTmp = strPathName;
		strTmp += _T("\\other files");
		CreateDirectory(strTmp, NULL);

		strTmp = strPathName;
		strTmp += _T("\\models");
		CreateDirectory(strTmp, NULL);

		strTmp = strPathName;
		strTmp += _T("\\graphics");
		CreateDirectory(strTmp, NULL);

		m_wndFileView.GetTreeCtrl()->EnableWindow(TRUE);
		m_wndModelView.GetTreeCtrl()->EnableWindow(TRUE);
		m_wndGraphView.GetTreeCtrl()->EnableWindow(TRUE);
	}
}

bool CMainFrame::OpenProjectFile(LPCSTR lpszFileName)
{
	if( m_strPrjFileName.IsEmpty())
		theApp.AddToRecentFileList(m_strPrjFileName);

	m_strPrjFileName = lpszFileName;
	CFile file(lpszFileName, CFile::modeRead|CFile::typeBinary);
	if( file.GetLength() < 11 )
	{
		file.Close();
		AfxMessageBox(_T("不是工程文件格式文件!"));
		m_strPrjFileName = _T("");
		return false;
	}
	CArchive ar(&file, CArchive::load);
	CString str;
	ar >> str;
	if( str != _T("Interlayer"))
	{
		ar.Close();
		file.Close();
		AfxMessageBox(_T("不是工程文件格式文件!"));
		m_strPrjFileName = _T("");
		return false;
	}
	m_wndFileView.LoadTree(ar);
	m_wndModelView.LoadTree(ar);
	m_wndGraphView.LoadTree(ar);
	ar.Close();
	file.Close();

	m_wndFileView.GetTreeCtrl()->EnableWindow(TRUE);
	m_wndModelView.GetTreeCtrl()->EnableWindow(TRUE);
	m_wndGraphView.GetTreeCtrl()->EnableWindow(TRUE);

	return true;
}

bool CMainFrame::SaveProjectFile(LPCSTR lpszFileName)
{
	CFile file( lpszFileName, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
	CArchive ar(&file, CArchive::store);
	CString str = _T("Interlayer");	
	ar << str;
	m_wndFileView.SaveTree(ar);
	m_wndModelView.SaveTree(ar);
	m_wndGraphView.SaveTree(ar);
	ar.Close();
	file.Close();
	return true;
}

void CMainFrame::OnFileOpenPrj()
{
	// TODO: 在此添加命令处理程序代码

	CFileDialog dlg(TRUE,_T("prj"),_T(""),4|2,_T("工程文件(*.Prj)|*.Prj||"));
	if(dlg.DoModal() == IDOK )
	{
		if( !theApp.SaveAllModified() )
			return;	

		if( !CloseProjectFile() )
			return;

		theApp.CloseAllDocuments(TRUE);

		CString strFileName = dlg.GetPathName();
		
		if( !OpenProjectFile(strFileName) )
			return;

		m_strPrjFileName = strFileName;

		m_wndFileView.GetTreeCtrl()->EnableWindow(TRUE);
		m_wndModelView.GetTreeCtrl()->EnableWindow(TRUE);
		m_wndGraphView.GetTreeCtrl()->EnableWindow(TRUE);
	}
}

void CMainFrame::OnFileClosePrj()
{
	// TODO: 在此添加命令处理程序代码
	if( !theApp.SaveAllModified() )
		return;
	
	if( !CloseProjectFile() )
		return;

	theApp.CloseAllDocuments(TRUE);
}

void CMainFrame::OnUpdateFileClosePrj(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码

	if( m_strPrjFileName.IsEmpty() )
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);

}

void CMainFrame::OnFileSavePrj()
{
	// TODO: 在此添加命令处理程序代码
	SaveProjectFile(m_strPrjFileName);
}


void CMainFrame::OnUpdateFileSavePrj(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码

	if( m_strPrjFileName.IsEmpty() )
		pCmdUI->Enable(FALSE);
	else
	{
		pCmdUI->Enable(TRUE);
	}
}


void CMainFrame::OnFileSaveAsPrj()
{
	// TODO: 在此添加命令处理程序代码

	CFileDialog dlg(FALSE,_T("prj"),_T(""),4|2,_T("工程文件(*.Prj)|*.Prj||"));
	if(dlg.DoModal() == IDOK )
	{		
		CSplitPath path(m_strPrjFileName);
		CString strOldPathName = path.GetDrive() + path.GetDirectory() + path.GetFileName();
		strOldPathName += _T("_dat\0");

		CString strFileName = dlg.GetPathName();
		CString strExt = dlg.GetFileExt();

		CString strPathName;
		if( !strExt.IsEmpty() )
			strPathName = strFileName.Left(strFileName.ReverseFind('.'));
		else
			strPathName = strFileName;

		strPathName += _T("_dat\0");

		SaveProjectFile(strFileName);
		
		m_strPrjFileName = strFileName;

		CopyDirectory(strOldPathName, strPathName);
	}
}

void CMainFrame::OnUpdateFileSaveAsPrj(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码

	if( m_strPrjFileName.IsEmpty() )
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

void CMainFrame::OnFileSaveAll()
{
	CDocTemplate* pTemplate;
	POSITION pos1 = theApp.GetFirstDocTemplatePosition();
	ASSERT(pos1 != NULL);
	while(pos1!=NULL)
	{
		pTemplate = theApp.GetNextDocTemplate(pos1);
		CDocument* pDoc;
		POSITION pos2 = pTemplate->GetFirstDocPosition();
		while(pos2 != NULL)
		{
			pDoc = pTemplate->GetNextDoc(pos2);
			if( pDoc->IsModified() )
				pDoc->DoFileSave();
		}
	}
	SaveProjectFile(m_strPrjFileName);
}

void CMainFrame::OnUpdateFileSaveAll(CCmdUI *pCmdUI)
{
	if( m_strPrjFileName.IsEmpty() )
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

BOOL CMainFrame::SaveEmptyOleDocument(LPCTSTR lpszFileName)
{
	LPSTORAGE lpStorage = NULL;

	const CStringW strPathName(lpszFileName);
	SCODE sc = ::StgCreateDocfile(strPathName.GetString(),
		STGM_READWRITE|STGM_TRANSACTED|STGM_SHARE_DENY_WRITE|STGM_CREATE,
		0, &lpStorage);
	if (sc != S_OK)
	{
		AfxThrowOleException(sc);
		return FALSE;
	}


	// create Contents stream
	COleStreamFile file;
	CFileException fe;
	if (!file.CreateStream(lpStorage, _T("Contents"),
		CFile::modeReadWrite|CFile::shareExclusive|CFile::modeCreate, &fe))
	{
		if (fe.m_cause == CFileException::fileNotFound)
		{
			AfxThrowArchiveException(CArchiveException::badSchema);
			return FALSE;
		}
		else
		{
			AfxThrowFileException(fe.m_cause, fe.m_lOsError);
			return FALSE;
		}
	}

	// save to Contents stream
	CArchive saveArchive(&file, CArchive::store);
	// save the contents

	CDrawPage page;
	CDrawLayer *p = new CDrawLayer;
	p->m_strLayerName = _T("图层 1");
	page.Add(p);
	CString strID = page.m_strID;

	page.Serialize(saveArchive);
	saveArchive << strID;

	saveArchive.Close();
	file.Close();

	// commit the root storage
	sc = lpStorage->Commit(STGC_ONLYIFCURRENT);
	if (sc != S_OK)
	{
		AfxThrowOleException(sc);
		return FALSE;
	}

	lpStorage->Release();

	return TRUE;	
}

void CMainFrame::OnFileNew()
{
	// TODO: 在此添加命令处理程序代码	
	if( m_wndGraphView.GetTreeCtrl()->m_hSelectItem != NULL)
	{
		CTreeNodeDat *pNote = (CTreeNodeDat *)m_wndGraphView.GetTreeCtrl()->GetItemData(m_wndGraphView.GetTreeCtrl()->m_hSelectItem);
		if( pNote->m_nType == GRAPHICS_2DS )
		{	
			CString strFileName = newGUID();
			strFileName += _T(".2df");

			CString strPathName = GetProjectDatPath();
			strPathName += _T("\\graphics\\");
			strPathName += strFileName;

			if( !SaveEmptyOleDocument(strPathName) )
				return;

			HTREEITEM hItem = m_wndGraphView.GetTreeCtrl()->GetSelectedItem();
			if( hItem == NULL )
				hItem = TVI_ROOT;

			int num = 0;

			CString strTitle;
			do 
			{
				if(num>0)
					strTitle.Format(_T("2D图形 %d"),num);
				else
					strTitle = _T("2D图形");
				num++;
			} while(m_wndGraphView.GetTreeCtrl()->IsExistChildNode(hItem,(LPCSTR)strTitle));

			HTREEITEM hSrc = m_wndGraphView.AddTreeItem(strTitle, 1, 1, hItem);
			CTreeNodeDat *lpNodeDat = new CTreeNodeDat;
			CFileViewObj *pObj = new CFileViewObj;			
			pObj->m_strFileName = strFileName;

			lpNodeDat->m_nImage			= 1;
			lpNodeDat->m_nSelectedImage	= 1;
			lpNodeDat->m_nType			= FILE_2D;
			lpNodeDat->m_pNodeDat		= pObj;
			lpNodeDat->m_uState			= TVIS_EXPANDED;
			strFileName.MakeUpper();
			lpNodeDat->m_strFileName	= strFileName;

			m_wndGraphView.GetTreeCtrl()->SetItemData(hSrc, (DWORD)lpNodeDat);

			m_wndGraphView.GetTreeCtrl()->Expand(hItem, TVE_EXPAND);

			CreateOrActivateFrame(theApp.m_pGeoMapDocTemplate, RUNTIME_CLASS(CGeoMapView), strPathName);
		}
		else if( pNote->m_nType == GRAPHICS_3DS )
		{
			CString strFileName = newGUID();
			strFileName += _T(".3df");

			CString strPathName = GetProjectDatPath();
			strPathName += _T("\\graphics\\");
			strPathName += strFileName;

			CFile file(strPathName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
			file.Close();

			HTREEITEM hItem = m_wndGraphView.GetTreeCtrl()->GetSelectedItem();
			
			if( hItem == NULL )
				hItem = TVI_ROOT;
			
			int num = 0;
			
			CString strTitle;
			do 
			{
				if(num>0)
					strTitle.Format(_T("3D图形 %d"),num);
				else
					strTitle = _T("3D图形");
				num++;
			} while(m_wndGraphView.GetTreeCtrl()->IsExistChildNode(hItem,(LPCSTR)strTitle));

			HTREEITEM hSrc = m_wndGraphView.AddTreeItem(strTitle, 2, 2, hItem);
			CTreeNodeDat *lpNodeDat = new CTreeNodeDat;
			CFileViewObj *pObj = new CFileViewObj;
			pObj->m_strFileName = strFileName;

			lpNodeDat->m_nImage			= 2;
			lpNodeDat->m_nSelectedImage	= 2;
			lpNodeDat->m_nType			= FILE_3D;
			lpNodeDat->m_pNodeDat		= pObj;
			lpNodeDat->m_uState			= TVIS_EXPANDED;
			strFileName.MakeUpper();
			lpNodeDat->m_strFileName	= strFileName;

			m_wndGraphView.GetTreeCtrl()->SetItemData(hSrc, (DWORD)lpNodeDat);

			m_wndGraphView.GetTreeCtrl()->Expand(hItem, TVE_EXPAND);

			CreateOrActivateFrame(theApp.m_p3DModelDocTemplate, RUNTIME_CLASS(C3DModelView), strPathName);
		}
		else if( pNote->m_nType == GRAPHICS_LOGS )
		{
			CString strFileName = newGUID();
			strFileName += _T(".LOG");

			CString strPathName = GetProjectDatPath();
			strPathName += _T("\\graphics\\");
			strPathName += strFileName;

			CFile file(strPathName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
			file.Close();

			HTREEITEM hItem = m_wndGraphView.GetTreeCtrl()->GetSelectedItem();

			if( hItem == NULL )
				hItem = TVI_ROOT;

			int num = 0;

			CString strTitle;
			do 
			{
				if(num>0)
					strTitle.Format(_T("LOGING %d"),num);
				else
					strTitle = _T("LOGING");
				num++;
			} while(m_wndGraphView.GetTreeCtrl()->IsExistChildNode(hItem,(LPCSTR)strTitle));

			HTREEITEM hSrc = m_wndGraphView.AddTreeItem(strTitle, 3, 3, hItem);
			CTreeNodeDat *lpNodeDat = new CTreeNodeDat;
			CFileViewObj *pObj = new CFileViewObj;
			pObj->m_strFileName = strFileName;

			lpNodeDat->m_nImage			= 3;
			lpNodeDat->m_nSelectedImage	= 3;
			lpNodeDat->m_nType			= FILE_LOG;
			lpNodeDat->m_pNodeDat		= pObj;
			lpNodeDat->m_uState			= TVIS_EXPANDED;
			strFileName.MakeUpper();
			lpNodeDat->m_strFileName	= strFileName;

			m_wndGraphView.GetTreeCtrl()->SetItemData(hSrc, (DWORD)lpNodeDat);

			m_wndGraphView.GetTreeCtrl()->Expand(hItem, TVE_EXPAND);

			CreateOrActivateFrame(theApp.m_pLoggingDocTemplate, RUNTIME_CLASS(C3DModelView), strPathName);
		}
		else
		{
			
		}
	}
	else if( m_wndFileView.GetTreeCtrl()->m_hSelectItem != NULL)
	{
		CTreeNodeDat *pNote = (CTreeNodeDat *)m_wndFileView.GetTreeCtrl()->GetItemData(m_wndFileView.GetTreeCtrl()->m_hSelectItem);
		if( pNote->m_nType == WELL_PRO
			|| pNote->m_nType == WELL_INJ)
		{
			HTREEITEM h = m_wndFileView.GetTreeCtrl()->GetParentItem(m_wndFileView.GetTreeCtrl()->m_hSelectItem);
			CTreeNodeDat *lpNoteDat = (CTreeNodeDat *)m_wndFileView.GetTreeCtrl()->GetItemData(h);
			if( lpNoteDat->m_nType == WELL_LOGING_INFO)
			{
				CString strTitle = m_wndFileView.GetTreeCtrl()->GetItemText(m_wndFileView.GetTreeCtrl()->m_hSelectItem);

				TCHAR strFileName[1024];
				SendMessage(WM_CREATE_FILE_LOG,(WPARAM)(LPCTSTR)strTitle, (LPARAM)(LPCTSTR)strFileName);

				CDocument* pDoc = theApp.m_pLoggingDocTemplate->OpenDocumentFile(strFileName);

				if( pDoc != NULL )
				{
					CString strPathName = GetProjectDatPath();
					strPathName += _T("\\files\\");
					strPathName += pNote->m_strFileName;
					LoadLogFile(pDoc, strPathName);
				}
			}
			else
			{

			}
		}
	}
	else
	{

	}
}

void CMainFrame::OnUpdateFileNew(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	
	if( m_strPrjFileName.IsEmpty() )
		pCmdUI->Enable(FALSE);
	else
	{		
		if( m_wndGraphView.GetTreeCtrl()->m_hSelectItem != NULL)
		{
			CTreeNodeDat *pNote = (CTreeNodeDat *)m_wndGraphView.GetTreeCtrl()->GetItemData(m_wndGraphView.GetTreeCtrl()->m_hSelectItem);
			if( pNote->m_nType == GRAPHICS_2DS
				|| pNote->m_nType == GRAPHICS_3DS
				|| pNote->m_nType == GRAPHICS_LOGS)
			{
				pCmdUI->Enable(TRUE);
			}
			else
				pCmdUI->Enable(FALSE);
		}
		else if( m_wndFileView.GetTreeCtrl()->m_hSelectItem != NULL)
		{
			CTreeNodeDat *pNote = (CTreeNodeDat *)m_wndFileView.GetTreeCtrl()->GetItemData(m_wndFileView.GetTreeCtrl()->m_hSelectItem);
			if( pNote->m_nType == WELL_PRO
				|| pNote->m_nType == WELL_INJ)
			{
				HTREEITEM h = m_wndFileView.GetTreeCtrl()->GetParentItem(m_wndFileView.GetTreeCtrl()->m_hSelectItem);
				CTreeNodeDat *lpNoteDat = (CTreeNodeDat *)m_wndFileView.GetTreeCtrl()->GetItemData(h);
				if( lpNoteDat->m_nType == WELL_LOGING_INFO)
				{
					pCmdUI->SetText(_T("新建测井曲线图"));
					pCmdUI->Enable(TRUE);
				}
				else
					pCmdUI->Enable(FALSE);
			}
			else
				pCmdUI->Enable(FALSE);
		}
		else
			pCmdUI->Enable(FALSE);
	}
}

void CMainFrame::OnFileOpen()
{
	// TODO: 在此添加命令处理程序代码

	HTREEITEM hItem = m_wndGraphView.GetTreeCtrl()->GetSelectedItem();

	if( hItem != NULL)
	{
		CTreeNodeDat *pNote = (CTreeNodeDat *)m_wndGraphView.GetTreeCtrl()->GetItemData(hItem);
		if( pNote->m_nType == FILE_2D )
		{
			CString strFileName = GetProjectDatPath();
			strFileName += _T("\\graphics\\");
			strFileName += ((CFileViewObj*)pNote->m_pNodeDat)->m_strFileName;
			if( !PathFileExists(strFileName) )
			{
				AfxMessageBox(_T("文件没有找到!"));
			}
			else
			{
				CString strTitle = m_wndGraphView.GetTreeCtrl()->GetItemText(hItem);
				CDocument *pDoc = CreateOrActivateFrame(theApp.m_pGeoMapDocTemplate, RUNTIME_CLASS(CGeoMapView), strFileName);
				pDoc->SetTitle(strTitle);
			}
		}
		else if( pNote->m_nType == FILE_3D )
		{
			CString strFileName = GetProjectDatPath();
			strFileName += _T("\\graphics\\");
			strFileName += ((CFileViewObj*)pNote->m_pNodeDat)->m_strFileName;
			if( !PathFileExists(strFileName) )
			{
				AfxMessageBox(_T("文件没有找到!"));
			}
			else
			{
				CString strTitle = m_wndGraphView.GetTreeCtrl()->GetItemText(hItem);
				CDocument *pDoc = CreateOrActivateFrame(theApp.m_p3DModelDocTemplate, NULL, strFileName);
				pDoc->SetTitle(strTitle);
			}
		}
		else if( pNote->m_nType == FILE_LOG )
		{
			CString strFileName = GetProjectDatPath();
			strFileName += _T("\\graphics\\");
			strFileName += ((CFileViewObj*)pNote->m_pNodeDat)->m_strFileName;
			if( !PathFileExists(strFileName) )
			{
				AfxMessageBox(_T("文件没有找到!"));
			}
			else
			{
				CString strTitle = m_wndGraphView.GetTreeCtrl()->GetItemText(hItem);
				CDocument *pDoc = CreateOrActivateFrame(theApp.m_pLoggingDocTemplate, NULL, strFileName);
				pDoc->SetTitle(strTitle);
			}
		}
	}
}

void CMainFrame::OnUpdateFileOpen(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码

	if( m_strPrjFileName.IsEmpty() )
		pCmdUI->Enable(FALSE);
	else
	{
		HTREEITEM hItem = m_wndGraphView.GetTreeCtrl()->GetSelectedItem();

		if( hItem != NULL)
		{
			CTreeNodeDat *pNote = (CTreeNodeDat *)m_wndGraphView.GetTreeCtrl()->GetItemData(hItem);
			if( pNote->m_nType == FILE_2D
				|| pNote->m_nType == FILE_3D
				|| pNote->m_nType == FILE_LOG)
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

CDocument* CMainFrame::CreateOrActivateFrame(CDocTemplate * pTemplate, CRuntimeClass * pViewClass, LPCTSTR lpszFileName)
{	
	CDocument* pDoc = NULL;

	if( lpszFileName != NULL )
	{
		// 先判断是否已经有文档打开
		CDocTemplate::Confidence match;

		match = pTemplate->MatchDocType(lpszFileName, pDoc);

		if (match == CDocTemplate::yesAlreadyOpen) // 已经有文档打开
		{
			CView* pView;
			POSITION pos = pDoc->GetFirstViewPosition ();
			while (pos != NULL)
			{
				pView = pDoc->GetNextView (pos);
				//if (pView->IsKindOf (pViewClass))
				{
					pView->GetParentFrame ()->ActivateFrame ();
					return pDoc;
				}
			}
		}
	}
	
	return pTemplate->OpenDocumentFile(lpszFileName);
}

void CMainFrame::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if( !theApp.SaveAllModified())
		return;

	if( !CloseProjectFile() )
		return;

	theApp.CloseAllDocuments(TRUE);

	CMDIFrameWndEx::OnClose();
}


void CMainFrame::ActiveView(CView *pView)
{
	if( pView )
	{
		if( pView->IsKindOf(RUNTIME_CLASS(CGeoMapView)))
		{
			m_wndToolBarDraw.ShowPane(TRUE, FALSE, TRUE);
			m_wndFormatBar.ShowPane(TRUE, FALSE, TRUE);
			m_wndToolBarLayer.ShowPane(TRUE, FALSE, TRUE);
		}
		else if( pView->IsKindOf(RUNTIME_CLASS(C2DModelView)))
			m_wnd2DModelToolBar.ShowPane(TRUE, FALSE, TRUE);
		else if( pView->IsKindOf(RUNTIME_CLASS(C3DModelView)))
		{
			m_wnd3DModelToolBar.ShowPane(TRUE, FALSE, TRUE);
			m_wnd3DObjBar.ShowPane(TRUE, FALSE, TRUE);
			m_wndSearchBar.ShowPane(TRUE, FALSE, TRUE);
		}
		RecalcLayout ();
	}
}

void CMainFrame::DeactiveView(CView *pView)
{
	if( pView )
	{
		if( pView->IsKindOf(RUNTIME_CLASS(CGeoMapView)))
		{
			m_wndToolBarDraw.ShowPane(FALSE, FALSE, TRUE);
			m_wndFormatBar.ShowPane(FALSE, FALSE, TRUE);
			m_wndToolBarLayer.ShowPane(FALSE, FALSE, TRUE);
		}
		else if( pView->IsKindOf(RUNTIME_CLASS(C2DModelView)))
			m_wnd2DModelToolBar.ShowPane(FALSE, FALSE, TRUE);
		else if( pView->IsKindOf(RUNTIME_CLASS(C3DModelView)))
		{
			m_wnd3DModelToolBar.ShowPane(FALSE, FALSE, TRUE);
			m_wnd3DObjBar.ShowPane(FALSE, FALSE, TRUE);
			m_wndSearchBar.ShowPane(FALSE, FALSE, TRUE);
		}

		RecalcLayout ();
	}
}

void CMainFrame::OnVerticalModel()
{
	// TODO: 在此添加命令处理程序代码

	TCHAR szCurPath[MAX_PATH];
	memset(szCurPath,   0,   MAX_PATH);  
	GetModuleFileName(NULL,   szCurPath,   sizeof(szCurPath)/sizeof(TCHAR));   
	CString strMoudlePath = szCurPath;
	strMoudlePath = strMoudlePath.Left(strMoudlePath.ReverseFind('\\')+1);
	CString strFileName = strMoudlePath;
	strFileName += _T("Template\\models.dat");
	CDocument *pDoc = NULL;
	if(PathFileExists(strFileName))
		pDoc = CreateOrActivateFrame(theApp.m_pVertModelDocTemplate, RUNTIME_CLASS(CVertModelView), strFileName);
	else
		pDoc = CreateOrActivateFrame(theApp.m_pVertModelDocTemplate, RUNTIME_CLASS(CVertModelView), NULL);
	if( pDoc )
		pDoc->SetTitle(_T("纵向模型"));
}

void CMainFrame::OnGenerateLayer()
{
	// TODO: 在此添加命令处理程序代码
	CDocument *pDoc = CreateOrActivateFrame(theApp.m_pLayerDocTemplate, NULL, NULL);
	if( pDoc )
		pDoc->SetTitle(_T("夹层生成器"));
}

LRESULT CMainFrame::OnGetItemTitleOfFile(WPARAM wp, LPARAM lp)
{
	CString strFileName = (LPCTSTR)wp;
	strFileName.MakeUpper();

	CString strPath = GetProjectDatPath();
	strPath += _T("\\files\\");
	strPath.MakeUpper();

	strFileName.Replace(strPath, _T(""));

	HTREEITEM hItem = GetTreeFileView()->GetTreeCtrl()->IsExistFileInTree(TVI_ROOT, strFileName);

	if( hItem != NULL )
	{
		CString strTitle = GetTreeFileView()->GetTreeCtrl()->GetItemText(hItem);
		
		strcpy_s((LPSTR)lp,255,(LPCTSTR)strTitle);

		return -1;
	}

	return 0;
}

LRESULT CMainFrame::OnGetItemTitleOfModel(WPARAM wp, LPARAM lp)
{
	CString strFileName = (LPCTSTR)wp;
	strFileName.MakeUpper();

	CString strPath = GetProjectDatPath();
	strPath += _T("\\models\\");
	strPath.MakeUpper();

	strFileName.Replace(strPath, _T(""));

	HTREEITEM hItem = GetTreeModelView()->GetTreeCtrl()->IsExistFileInTree(TVI_ROOT, strFileName);

	if( hItem != NULL )
	{
		CString strTitle = GetTreeModelView()->GetTreeCtrl()->GetItemText(hItem);

		strcpy_s((LPSTR)lp,255,(LPCTSTR)strTitle);

		return -1;
	}

	return 0;
}

LRESULT CMainFrame::OnGetItemTitleOfGraph(WPARAM wp, LPARAM lp)
{
	CString strFileName = (LPCTSTR)wp;
	strFileName.MakeUpper();

	CString strPath = GetProjectDatPath();
	strPath += _T("\\graphics\\");
	strPath.MakeUpper();

	strFileName.Replace(strPath, _T(""));

	HTREEITEM hItem = GetTreeGraphView()->GetTreeCtrl()->IsExistFileInTree(TVI_ROOT, strFileName);

	if( hItem != NULL )
	{
		CString strTitle = GetTreeGraphView()->GetTreeCtrl()->GetItemText(hItem);

		strcpy_s((LPSTR)lp,255,(LPCTSTR)strTitle);
		
		return -1;
	}

	return 0;
}

LRESULT CMainFrame::OnCreateFileLog(WPARAM wp, LPARAM lp)
{
	CString strTitle = (LPCTSTR)wp;

	CString strFileName = newGUID();
	strFileName += _T(".log");
	CString strTargePathName = GetProjectDatPath();
	strTargePathName += _T("\\graphics\\");
	strTargePathName += strFileName;

	CFile file(strTargePathName,CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
	file.Close();

	HTREEITEM hItem = m_wndGraphView.GetTreeCtrl()->GetChildNode(TVI_ROOT,_T("测井曲线视图"));

	if( hItem == NULL )
		hItem = TVI_ROOT;

	HTREEITEM hSrc = m_wndGraphView.AddTreeItem(strTitle, 3, 3, hItem);
	CTreeNodeDat *lpNodeDat = new CTreeNodeDat;
	CFileViewObj *pObj = new CFileViewObj;
	pObj->m_strFileName = strFileName;

	lpNodeDat->m_nImage			= 3;
	lpNodeDat->m_nSelectedImage	= 3;
	lpNodeDat->m_nType			= FILE_LOG;
	lpNodeDat->m_pNodeDat		= pObj;
	lpNodeDat->m_uState			= TVIS_EXPANDED;
	strFileName.MakeUpper();
	lpNodeDat->m_strFileName	= strFileName;

	m_wndGraphView.GetTreeCtrl()->SetItemData(hSrc, (DWORD)lpNodeDat);

	m_wndGraphView.GetTreeCtrl()->Expand(hItem, TVE_EXPAND);

	strcpy_s((LPSTR)lp,255,(LPCTSTR)strTargePathName);

	return -1;
}

void CMainFrame::OnPhyPararangeSet()
{
	CDlgInterlayerParaRange dlg;
	dlg.m_rangeMin = CIntersectSearchManager::Instance()->GetPhyParaMin();
	dlg.m_rangeMax = CIntersectSearchManager::Instance()->GetPhyParaMax();
	if( dlg.DoModal() == IDOK )
	{
		if(dlg.m_rangeMin>=dlg.m_rangeMax)
		{
			AfxMessageBox("最小值大于等于最大值");
			return ;
		}
	}

	CIntersectSearchManager::Instance()->SetPhyParaMin(dlg.m_rangeMin);
	CIntersectSearchManager::Instance()->SetPhyParaMax(dlg.m_rangeMax);

	CMainFrame *pMF = (CMainFrame *)AfxGetMainWnd();

	CMDIChildWndEx *pWnd =(CMDIChildWndEx *) pMF->MDIGetActive();
	if( pWnd )
	{
		C3DModelView *pView = (C3DModelView *)pWnd->GetActiveView();
		if(pView)
		{
			if(pView->IsKindOf(RUNTIME_CLASS(C3DModelView)))
			{
				C3DModelDoc *pDoc = (C3DModelDoc *)pView ->GetDocument();
				pDoc->GetContext()->SetModifiedFlag();
			}
		}
	}	
}
