#include "stdafx.h"
#include "GraphFrame.h"
#include "GraphWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PVIEWBAR_ID	11003
#define DEMO_MAX_GRAPHS 16

COLORREF DefaultGraphColors[DEMO_MAX_GRAPHS] = {RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255), 
RGB(255, 255, 0), RGB(0, 255, 255), 32768, 128, 
8388736, 8421440, 12615808, 8421504, 33023, 
16711935, 12632256, 32896, RGB(0, 0, 0)};

/////////////////////////////////////////////////////////////////////////////
// CGraphFrame

IMPLEMENT_DYNCREATE(CGraphFrame, CFrameWnd)

CGraphFrame::CGraphFrame()
{
	m_bOwnTimer = FALSE;
}

CGraphFrame::~CGraphFrame()
{
}


BEGIN_MESSAGE_MAP(CGraphFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CGraphFrame)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(PVIEWBAR_ID, PVIEWBAR_ID, OnPointViewBarCommand)
	ON_UPDATE_COMMAND_UI_RANGE(PVIEWBAR_ID, PVIEWBAR_ID, OnPointViewUpdate)
	ON_COMMAND(ID_NEW_CURVE, OnNewCurve)
	ON_UPDATE_COMMAND_UI(ID_NEW_CURVE, OnUpdateNewCurve)
	ON_COMMAND(ID_DEL_CURVE, OnDelCurve)
	ON_UPDATE_COMMAND_UI(ID_DEL_CURVE, OnUpdateDelCurve)
	ON_COMMAND(ID_MOVE_POINT, OnMovePoint)
	ON_UPDATE_COMMAND_UI(ID_MOVE_POINT, OnUpdateMovePoint)
	ON_COMMAND(ID_ADD_POINT, OnAddPoint)
	ON_UPDATE_COMMAND_UI(ID_ADD_POINT, OnUpdateAddPoint)
	ON_COMMAND(ID_DEL_POINT, OnDelPoint)
	ON_UPDATE_COMMAND_UI(ID_DEL_POINT, OnUpdateDelPoint)	
	ON_COMMAND(ID_DEL_ALL_POINT, OnDelAllPoint)
	ON_UPDATE_COMMAND_UI(ID_DEL_ALL_POINT, OnUpdateDelAllPoint)
	ON_COMMAND(ID_SHOW_TRIANGLUER,OnShowTriangluer)
	ON_UPDATE_COMMAND_UI(ID_SHOW_TRIANGLUER,OnUpdateShowTriangluer)
	ON_COMMAND(ID_RULER_LINKAGE,OnRulerLinkage)
	ON_UPDATE_COMMAND_UI(ID_RULER_LINKAGE,OnUpdateRulerLinkage)
	ON_COMMAND(ID_ANGLE_LINKAGE,OnAngleLinkage)
	ON_UPDATE_COMMAND_UI(ID_ANGLE_LINKAGE,OnUpdateAngleLinkage)
	ON_COMMAND(ID_ANGLE_ZIP_LINKAGE,OnAngleZipLinkage)
	ON_UPDATE_COMMAND_UI(ID_ANGLE_ZIP_LINKAGE,OnUpdateAngleZipLinkage)
	ON_COMMAND(ID_LENGHT_LINKAGE,OnLenLinkage)
	ON_UPDATE_COMMAND_UI(ID_LENGHT_LINKAGE,OnUpdateLenLinkage)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphFrame message handlers

BOOL CGraphFrame::Create(LPCTSTR lpszWindowName, const RECT& rect, CWnd* pParentWnd, BOOL bOwnTimer/* = FALSE*/, DWORD dwStyle/* = WS_CHILD | WS_VISIBLE*/)
{
	m_bOwnTimer = bOwnTimer;
	return CFrameWnd::Create(NULL, lpszWindowName, dwStyle, rect, pParentWnd);
};


#define GRAPH_TIMER_ID	0x12345

int CGraphFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rect;
	GetClientRect(rect);

	BOOL b = graph_container.Create(NULL, _T(""), WS_VISIBLE | WS_CHILD, rect, this, 11001);
	ASSERT(b);

	CSize size(288, 200);

	if (!pview_bar.Create(_T("工具栏"), this, size, TRUE, PVIEWBAR_ID))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;  	// fail to create
	}

	pview_bar.SetBarStyle(pview_bar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT);

	pview_bar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&pview_bar, AFX_IDW_DOCKBAR_RIGHT);

	//set timer for explicit idle
	if (m_bOwnTimer)
	{
		SetTimer(GRAPH_TIMER_ID, 100, NULL);
	};

	return 0;
}

void CGraphFrame::OnPointViewBarCommand(UINT nID)
{
	BOOL bShow = pview_bar.IsVisible();
	ShowControlBar(&pview_bar, !bShow, FALSE);
}

void CGraphFrame::OnPointViewUpdate(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
	pCmdUI->SetCheck(pview_bar.IsVisible());
}

void CGraphFrame::RecalcLayout(BOOL bNotify)
{
	CFrameWnd::RecalcLayout(bNotify);

	CRect rect;
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0, reposQuery, rect);

	if (!rect.IsRectEmpty() && ::IsWindow(graph_container.m_hWnd))
	{
		graph_container.MoveWindow(rect);

		CRect rect;
		graph_container.graph_panel->GetClientRect(rect);
		int x = rect.Width();
		int y = rect.Height();

		CGraphWnd* pWnd = (CGraphWnd*)this->GetParent();
		if( x>y)
			pWnd->SetGraphWorldCoords(-0.1-(double)(x-y)/(double)x/2.,1.1+(double)(x-y)/(double)x/2.0,-0.1,1.1,TRUE);
		else
			pWnd->SetGraphWorldCoords(-0.1,1.1,-0.1-(double)(y-x)/(double)y/2.0,1.1+(double)(y-x)/(double)y/2.0,TRUE);
		
	};
}

BOOL CGraphFrame::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

void CGraphFrame::UpdateViews(DWORD message_id, void* param)
{
	graph_container.UpdateViews(message_id, param);
	pview_bar.SendMessage(CM_GRAPH_NOTIFICATION, message_id, (DWORD)param);
}

void CGraphFrame::UpdateWindows(unsigned long what_to_update)
{
	graph_container.UpdateWindows(what_to_update);
	pview_bar.SendMessage(CM_GRAPH_NOTIFICATION, GRAPH_GRAPHIX_UPDATE, what_to_update);
}

void CGraphFrame::AppendMenuItems(CMenu* menu)
{
	if (menu->GetMenuItemCount() != 0)
	{
		//separator
		menu->AppendMenu(MF_SEPARATOR, 0, _T(""));
	};
	//View
	CMenu sub_menu;
	sub_menu.CreateMenu();
	menu->AppendMenu(MF_POPUP | MF_ENABLED | MF_STRING, (UINT)sub_menu.m_hMenu, _T("视窗"));
	//Zoom tool
	UINT menu_flags = MF_STRING | MF_ENABLED;
	if (pview_bar.IsWindowVisible())
	{
		menu_flags |= MF_CHECKED;
	};
	sub_menu.AppendMenu(menu_flags, GRAPH_RBMC_VIEW_POINT_WINDOW, _T("数据窗口"));
}

void CGraphFrame::OnRBMenuCommand(UINT command_id)
{
	switch (command_id)
	{
	case GRAPH_RBMC_VIEW_POINT_WINDOW:
		{
			if (pview_bar.IsWindowVisible())
			{
				OperateWithPointView(GRAPH_PO_HIDE);
			}
			else
			{
				OperateWithPointView(GRAPH_PO_SHOW);
			}
		}
		break;
	}
}

void CGraphFrame::OperateWithPointView(unsigned long pview_operations)
{
	if ((pview_operations & GRAPH_PO_SHOW) != 0)
	{
		ShowControlBar(&pview_bar, TRUE, FALSE);
	}
	if ((pview_operations & GRAPH_PO_HIDE) != 0)
	{
		ShowControlBar(&pview_bar, FALSE, FALSE);
	}

	if ((pview_operations & GRAPH_PO_DISABLE) != 0)
	{
		pview_bar.SendMessage(CM_GRAPH_NOTIFICATION, GRAPH_PVIEW_DISABLED, 0);
	}
	if ((pview_operations & GRAPH_PO_ENABLE) != 0)
	{
		pview_bar.SendMessage(CM_GRAPH_NOTIFICATION, GRAPH_PVIEW_ENABLED, 0);
	}
}


void CGraphFrame::OnTimer(UINT nIDEvent)
{
	CFrameWnd::OnTimer(nIDEvent);

	if (nIDEvent == GRAPH_TIMER_ID)
	{
		SendMessage(867, 1, 0);
	}
}

void CGraphFrame::DrawGraphToDC(CDC* dest_dc, CRect& rect_to_draw)
{
	graph_container.DrawGraphToDC(dest_dc, rect_to_draw);
}


void CGraphFrame::OnMovePoint()
{
	// TODO: 在此添加命令处理程序代码
	if (graph_container.graph_panel->m_opPoint == MOVE_POINT)
		graph_container.graph_panel->m_opPoint = NONE_POINT;
	else
	{
		graph_container.graph_panel->bZoomActive = FALSE;
		graph_container.graph_panel->bCreateZoomRect = FALSE;
		CRect r;
		graph_container.graph_panel->zoomrect->GetTrueRect(&r);
		graph_container.graph_panel->InvalidateRect(&r, TRUE);

		graph_container.graph_panel->m_opPoint = MOVE_POINT;
	}
}

void CGraphFrame::OnUpdateMovePoint(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	CGraphWnd* pWnd = (CGraphWnd*)this->GetParent();
	pCmdUI->Enable(pWnd->GetGraphCount() > 0 && pWnd->GetGraphPanel()->m_lCurrentGraphId > -1);
	pCmdUI->SetRadio(graph_container.graph_panel->m_opPoint == MOVE_POINT);
}

void CGraphFrame::OnAddPoint()
{
	// TODO: 在此添加命令处理程序代码
	if (graph_container.graph_panel->m_opPoint == ADD_POINT)
		graph_container.graph_panel->m_opPoint = NONE_POINT;
	else
	{
		graph_container.graph_panel->bZoomActive = FALSE;
		graph_container.graph_panel->bCreateZoomRect = FALSE;
		CRect r;
		graph_container.graph_panel->zoomrect->GetTrueRect(&r);
		graph_container.graph_panel->InvalidateRect(&r, TRUE);

		graph_container.graph_panel->m_opPoint = ADD_POINT;
	}
}

void CGraphFrame::OnUpdateAddPoint(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	CGraphWnd* pWnd = (CGraphWnd*)this->GetParent();
	pCmdUI->Enable(pWnd->GetGraphCount() > 0 && pWnd->GetGraphPanel()->m_lCurrentGraphId > -1);
	pCmdUI->SetRadio(graph_container.graph_panel->m_opPoint == ADD_POINT);
}

void CGraphFrame::OnDelPoint()
{
	// TODO: 在此添加命令处理程序代码

	if (graph_container.graph_panel->m_opPoint == DEL_POINT)
		graph_container.graph_panel->m_opPoint = NONE_POINT;
	else
	{
		graph_container.graph_panel->bZoomActive = FALSE;
		graph_container.graph_panel->bCreateZoomRect = FALSE;
		CRect r;
		graph_container.graph_panel->zoomrect->GetTrueRect(&r);
		graph_container.graph_panel->InvalidateRect(&r, TRUE);

		graph_container.graph_panel->m_opPoint = DEL_POINT;
	}
}

void CGraphFrame::OnUpdateDelPoint(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码

	CGraphWnd* pWnd = (CGraphWnd*)this->GetParent();
	pCmdUI->Enable(pWnd->GetGraphCount() > 0 && pWnd->GetGraphPanel()->m_lCurrentGraphId > -1);

	pCmdUI->SetRadio(graph_container.graph_panel->m_opPoint == DEL_POINT);
}

void CGraphFrame::OnDelCurve()
{
	// TODO: 在此添加命令处理程序代码

	CGraphWnd* pWnd = (CGraphWnd*)this->GetParent();
	long index = pWnd->GetGraphPanel()->m_lCurrentGraphId;
	pWnd->RemoveGraph(index, TRUE);

	if (pWnd->GetGraphCount() == 0)
		index = -1;
	else
	{
		long n = pWnd->GetGraphCount();
		if (index == n)
			index--;
	}

	pWnd->GetGraphPanel()->m_lCurrentGraphId = index;

	pview_bar.graph_combo_box.SetCurSel(index);

	pWnd->GetGraphPanel()->UpdateGraphWindow(NULL);
}

void CGraphFrame::OnNewCurve()
{
	// TODO: 在此添加命令处理程序代码

	CGraphWnd* pWnd = (CGraphWnd*)this->GetParent();
	int graph_count = pWnd->GetGraphCount();
	CString title;
	title.Format(_T("New model - %d"), graph_count);
	pWnd->AddGraph(DefaultGraphColors[graph_count%16], title);
}

void CGraphFrame::OnUpdateDelCurve(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码

	CGraphWnd* pWnd = (CGraphWnd*)this->GetParent();
	pCmdUI->Enable(pWnd->GetGraphPanel()->m_lCurrentGraphId > -1);
}

void CGraphFrame::OnUpdateNewCurve(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
}

void CGraphFrame::OnDelAllPoint()
{
	// TODO: 在此添加命令处理程序代码

	CGraphWnd* pWnd = (CGraphWnd*)this->GetParent();
	long index = pWnd->GetGraphPanel()->m_lCurrentGraphId;
	pWnd->ClearGraph(index, TRUE);
	pWnd->GetGraphPanel()->UpdateGraphWindow(NULL);
}

void CGraphFrame::OnUpdateDelAllPoint(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	CGraphWnd* pWnd = (CGraphWnd*)this->GetParent();
	pCmdUI->Enable(pWnd->GetGraphCount() > 0 && pWnd->GetGraphPanel()->m_lCurrentGraphId > -1);
}

void CGraphFrame::OnShowTriangluer()
{
	CGraphWnd* pWnd = (CGraphWnd*)this->GetParent();
	CGraphProps* grprop = pWnd->GetGraph(pWnd->GetGraphPanel()->m_lCurrentGraphId);
	if(grprop->m_bRulerIsVisible)
		grprop->m_bRulerIsVisible = FALSE;
	else
		grprop->m_bRulerIsVisible = TRUE;

	pWnd->GetGraphPanel()->UpdateGraphWindow(NULL);
}

void CGraphFrame::OnUpdateShowTriangluer(CCmdUI* pCmdUI)
{
	CGraphWnd* pWnd = (CGraphWnd*)this->GetParent();
	CGraphProps* grprop = pWnd->GetGraph(pWnd->GetGraphPanel()->m_lCurrentGraphId);
	pCmdUI->SetRadio(grprop->m_bRulerIsVisible);
}

void CGraphFrame::OnRulerLinkage()
{
	CGraphWnd* pWnd = (CGraphWnd*)this->GetParent();
	CGraphProps* grprop = pWnd->GetGraph(pWnd->GetGraphPanel()->m_lCurrentGraphId);
	if(grprop->m_bRulerLinkage)
		grprop->m_bRulerLinkage = FALSE;
	else
		grprop->m_bRulerLinkage = TRUE;
}

void CGraphFrame::OnUpdateRulerLinkage(CCmdUI* pCmdUI)
{
	CGraphWnd* pWnd = (CGraphWnd*)this->GetParent();
	CGraphProps* grprop = pWnd->GetGraph(pWnd->GetGraphPanel()->m_lCurrentGraphId);
	pCmdUI->SetRadio(grprop->m_bRulerLinkage);
}

void CGraphFrame::OnAngleLinkage()
{
	CGraphWnd* pWnd = (CGraphWnd*)this->GetParent();
	CGraphProps* grprop = pWnd->GetGraph(pWnd->GetGraphPanel()->m_lCurrentGraphId);
	if(grprop->m_bAngleLinkage)
		grprop->m_bAngleLinkage = FALSE;
	else
	{
		grprop->m_bAngleLinkage = TRUE;
		grprop->m_bAngleZipLinkage = FALSE;
	}
}

void CGraphFrame::OnUpdateAngleLinkage(CCmdUI* pCmdUI)
{
	CGraphWnd* pWnd = (CGraphWnd*)this->GetParent();
	CGraphProps* grprop = pWnd->GetGraph(pWnd->GetGraphPanel()->m_lCurrentGraphId);
	pCmdUI->SetRadio(grprop->m_bAngleLinkage);
}

void CGraphFrame::OnAngleZipLinkage()
{
	CGraphWnd* pWnd = (CGraphWnd*)this->GetParent();
	CGraphProps* grprop = pWnd->GetGraph(pWnd->GetGraphPanel()->m_lCurrentGraphId);
	if(grprop->m_bAngleZipLinkage)
		grprop->m_bAngleZipLinkage = FALSE;
	else
	{
		grprop->m_bAngleZipLinkage = TRUE;
		grprop->m_bAngleLinkage = FALSE;
	}
}

void CGraphFrame::OnUpdateAngleZipLinkage(CCmdUI* pCmdUI)
{
	CGraphWnd* pWnd = (CGraphWnd*)this->GetParent();
	CGraphProps* grprop = pWnd->GetGraph(pWnd->GetGraphPanel()->m_lCurrentGraphId);
	pCmdUI->SetRadio(grprop->m_bAngleZipLinkage);
}

void CGraphFrame::OnLenLinkage()
{
	CGraphWnd* pWnd = (CGraphWnd*)this->GetParent();
	CGraphProps* grprop = pWnd->GetGraph(pWnd->GetGraphPanel()->m_lCurrentGraphId);
	if(grprop->m_bLenLinkage)
		grprop->m_bLenLinkage = FALSE;
	else
		grprop->m_bLenLinkage = TRUE;
}

void CGraphFrame::OnUpdateLenLinkage(CCmdUI* pCmdUI)
{
	CGraphWnd* pWnd = (CGraphWnd*)this->GetParent();
	CGraphProps* grprop = pWnd->GetGraph(pWnd->GetGraphPanel()->m_lCurrentGraphId);
	pCmdUI->SetRadio(grprop->m_bLenLinkage);
}