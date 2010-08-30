// LogCurveTabFrame.cpp : implementation file
//

#include "stdafx.h"
#include "DispLogCurveFrame.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern AFX_EXTENSION_MODULE DispLogCurveDLL;

/////////////////////////////////////////////////////////////////////////////
// CDispLogCurveFrame

IMPLEMENT_DYNCREATE(CDispLogCurveFrame, CMDIChildWndEx)

CDispLogCurveFrame::CDispLogCurveFrame()
{
}

CDispLogCurveFrame::~CDispLogCurveFrame()
{
}


BEGIN_MESSAGE_MAP(CDispLogCurveFrame, CMDIChildWndEx)
	//{{AFX_MSG_MAP(CDispLogCurveFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_WATCH, OnViewWatchBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WATCH, OnUpdateViewWatchBar)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(AFX_WM_RESETTOOLBAR, OnToolbarReset)
	ON_REGISTERED_MESSAGE(AFX_WM_TOOLBARMENU, OnToolbarContextMenu)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDispLogCurveFrame message handlers

int CDispLogCurveFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	EnableAutoHidePanes(CBRS_ALIGN_BOTTOM);

	HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(DispLogCurveDLL.hModule);
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC,CRect(1,1,1,1),ID_VIEW_LOGCURVEBAR) ||
		!m_wndToolBar.LoadToolBar(IDR_DISPLOGCURVE_TOOLBAR))
	{
		AfxSetResourceHandle(hInstOld);
		return -1;      // fail to create
	}
	
	if (!m_wndWatchBar.Create (_T("数据采集"), this, CRect (0, 0, 100, 100),
		TRUE, 
		ID_VIEW_WATCH,
		WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create watch bar\n");
		AfxSetResourceHandle(hInstOld);
		return -1;      // fail to create
	}

	AfxSetResourceHandle(hInstOld);

	m_wndWatchBar.SetWindowText(_T("数据采集"));
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndWatchBar.EnableDocking(CBRS_ALIGN_ANY);
	
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndToolBar,AFX_IDW_DOCKBAR_TOP);
	DockPane(&m_wndWatchBar,AFX_IDW_DOCKBAR_BOTTOM);
	
	return 0;
}

LRESULT CDispLogCurveFrame::OnToolbarReset(WPARAM wp,LPARAM)
{
	UINT uiToolBarId = (UINT) wp;
	
	switch (uiToolBarId)
	{		
	case IDR_DISPLOGCURVE_TOOLBAR:
		{
			//-----------------------------------------------
			// Replace "Confoguration..." button by combobox:
			//-----------------------------------------------
			HINSTANCE hInstOld = AfxGetResourceHandle();
			AfxSetResourceHandle(DispLogCurveDLL.hModule);

			CMFCToolBarComboBoxButton comboButton (ID_VIEW_SCALE, afxCommandManager->GetCmdImage(ID_VIEW_SCALE, TRUE), CBS_DROPDOWNLIST, 200);
			comboButton.AddItem (_T(" 1 : 2000 "));
			comboButton.AddItem (_T(" 1 : 1000 "));
			comboButton.AddItem (_T(" 1 : 800  "));
			comboButton.AddItem (_T(" 1 : 500  "));
			comboButton.AddItem (_T(" 1 : 200  "));
			comboButton.AddItem (_T(" 1 : 100  "));
			comboButton.AddItem (_T(" 1 : 50   "));
			comboButton.AddItem (_T(" 1 : 25   "));
			comboButton.AddItem (_T(" 1 : 20   "));
			comboButton.AddItem (_T(" 1 : 10   "));
			comboButton.AddItem (_T(" 1 : 5    "));
			comboButton.SelectItem (4);
			
			m_wndToolBar.ReplaceButton (ID_VIEW_SCALE,
				comboButton);
			AfxSetResourceHandle(hInstOld);
		}
		break;
	}
	
	return 0;
}

LRESULT CDispLogCurveFrame::OnToolbarContextMenu(WPARAM,LPARAM lp)
{
	CPoint point (GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
	
	HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(DispLogCurveDLL.hModule);
	
	CMenu menu;
	VERIFY(menu.LoadMenu (IDR_POPUP_DISPLOGCURVE));

	AfxSetResourceHandle(hInstOld);
	
	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	
	CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;
	pPopupMenu->Create (this, point.x, point.y, pPopup->Detach ());
	
	return 0;
}

void CDispLogCurveFrame::OnViewWatchBar() 
{
	ShowPane (&m_wndWatchBar,
		!(m_wndWatchBar.GetStyle () & WS_VISIBLE),
		FALSE, TRUE);
}

void CDispLogCurveFrame::OnUpdateViewWatchBar(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck (m_wndWatchBar.GetStyle () & WS_VISIBLE);
}

BOOL CDispLogCurveFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	cs.style &= ~WS_SYSMENU;
	return CMDIChildWndEx::PreCreateWindow(cs);
}

void CDispLogCurveFrame::OnSize(UINT nType, int cx, int cy) 
{
	CMDIChildWndEx::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	CMDIFrameWndEx * pMainFrame = (CMDIFrameWndEx *)AfxGetMainWnd();
	pMainFrame->RecalcLayout();	
}
