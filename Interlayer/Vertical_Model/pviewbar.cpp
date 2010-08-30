#include "stdafx.h"
#include "pviewbar.h"
#include "graph_props.h"
#include "graphwnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define INIT_WINDOW_MACRO DWORD oldWindowStyle_Macro;

#define HIDE_WINDOW_MACRO(WND_TO_OPERATE) \
{ \
	oldWindowStyle_Macro = (DWORD)(WND_TO_OPERATE->GetStyle() & WS_VISIBLE); \
	if (oldWindowStyle_Macro != 0) WND_TO_OPERATE->ModifyStyle(WS_VISIBLE, 0, 0); \
};

#define SHOW_WINDOW_MACRO(WND_TO_OPERATE) \
	if (oldWindowStyle_Macro != 0) WND_TO_OPERATE->ModifyStyle(0, WS_VISIBLE, 0);

/////////////////////////////////////////////////////////////////////////////
// CPointViewBar

CPointViewBar::CPointViewBar()
{
	b_enabled = TRUE;
}

CPointViewBar::~CPointViewBar()
{
}


BEGIN_MESSAGE_MAP(CPointViewBar, CSizingControlBar)
	//{{AFX_MSG_MAP(CPointViewBar)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPointViewBar message handlers

void CPointViewBar::OnSize(UINT nType, int cx, int cy)
{
#define COMBO_SHIFT_TOP	5
#define COMBO_SHIFT_SIDE	5
#define PVIEW_LIST_SHIFT	5

	CSizingControlBar::OnSize(nType, cx, cy);

	CRect rect;
	GetClientRect(rect);	
	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rect.left, rect.top, rect.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);

	graph_combo_box.MoveWindow(COMBO_SHIFT_SIDE, COMBO_SHIFT_TOP + cyTlb + 1, rect.Width() - 2 * COMBO_SHIFT_SIDE, 100);

	CRect combo_rect;
	graph_combo_box.GetClientRect(combo_rect);
	long height_shift = combo_rect.Height() + COMBO_SHIFT_TOP + PVIEW_LIST_SHIFT + cyTlb + 1;
	point_list_ctrl.MoveWindow(COMBO_SHIFT_SIDE, height_shift, rect.Width() - 2 * COMBO_SHIFT_SIDE, rect.Height() - height_shift - COMBO_SHIFT_TOP);

	//resize columns
	point_list_ctrl.GetClientRect(rect);
	long col_size = (rect.right - rect.left) / 2;
	point_list_ctrl.SetColumnWidth(0, col_size);
	point_list_ctrl.SetColumnWidth(1, col_size);
}

int CPointViewBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CSizingControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC)
		|| !m_wndToolBar.LoadToolBar(IDR_TOOLBAR_VERT_MODEL))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;  	// fail to create
	}

	CRect rect(0, 0, 100, 100);
	BOOL b = graph_combo_box.Create(WS_CHILD | WS_VISIBLE, rect, this, GRAPH_COMBO_CHILD_ID);
	ASSERT(b);

	b = point_list_ctrl.Create(WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_NOSORTHEADER | LVS_SINGLESEL | LVS_OWNERDATA, rect, this, GRAPH_POINT_LIST_CHILD_ID);
	ASSERT(b);


	ListView_SetExtendedListViewStyleEx(point_list_ctrl.m_hWnd, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES); 

	point_list_ctrl.GetClientRect(rect);
	CString s;
	s = _T("X");
	point_list_ctrl.InsertColumn(0, s, LVCFMT_LEFT, (rect.right - rect.left) / 2);
	s = _T("Y");
	point_list_ctrl.InsertColumn(1, s, LVCFMT_LEFT, (rect.right - rect.left) / 2);

	return 0;
}

LRESULT CPointViewBar::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_COMMAND && (wParam > GRAPH_RBMC_FIRST && wParam < GRAPH_RBMC_LAST))
	{
		//this is right button menu commands
		EnumerateParentWindows(OnRBMenuCommandForAllParents, (void*)wParam);
	};
	if (message == CM_GRAPH_NOTIFICATION)
	{
		//this is graph's internal notification message
		switch (wParam)
		{
		case GRAPH_GRAPH_ADDED:
			{
				SGraphChange* sgc = (SGraphChange*)lParam;
				AddGraph(sgc);
			}; break;
		case GRAPH_POINT_ADDED:
			{
				SGraphChange* sgc = (SGraphChange*)lParam;
				AddPoint(sgc);
			}; break;
		case GRAPH_GRAPH_REMOVED:
			{
				SGraphChange* sgc = (SGraphChange*)lParam;
				RemoveGraph(sgc);
			}
			break;
		case GRAPH_POINT_REMOVED:
			{
				SGraphChange* sgc = (SGraphChange*)lParam;
				RemovePoint(sgc);
			}; break;
		case GRAPH_POINT_CHANGED:
			{
				SGraphChange* sgc = (SGraphChange*)lParam;
				EditPoint(sgc);
			}; break;
		case GRAPH_GRAPH_CLEARED:
			{
				SGraphChange* sgc = (SGraphChange*)lParam;
				ClearGraph(sgc);
			}; break;
		case GRAPH_GRAPHIX_AXISPROPS:
			{
				SAxisPropsChange* spc = (SAxisPropsChange*)lParam;
				if (spc->bRedraw)
				{
					InitPointList(spc->main_wnd_ptr);
				};
			}; break;
		case GRAPH_GRAPHIX_PROP:
			{
				graph_combo_box.InvalidateRect(NULL);
			}; break;
		case GRAPH_GRAPHIX_UPDATE:
			{
				graph_combo_box.RedrawWindow();
				InitPointList(NULL);
			}; break;
		case GRAPH_PVIEW_DISABLED:
			{
				b_enabled = FALSE;
			}; break;
		case GRAPH_PVIEW_ENABLED:
			{
				b_enabled = TRUE;
				graph_combo_box.RedrawWindow();
				InitPointList(NULL);
			}; break;
		};
	};
	return CSizingControlBar::WindowProc(message, wParam, lParam);
}

void CPointViewBar::AddGraph(SGraphChange* sgc)
{
	INIT_WINDOW_MACRO;
	if (!sgc->bRedraw)
	{
		HIDE_WINDOW_MACRO(this);
	};
	long cur_sel = graph_combo_box.GetCurSel();
	long index = graph_combo_box.AddString(_T(""));
	graph_combo_box.SetItemData(index, sgc->graphnum);
	if (cur_sel == CB_ERR)
	{
		//select newly added graph
		graph_combo_box.SetCurSel(index);
	};
	if (!sgc->bRedraw)
	{
		SHOW_WINDOW_MACRO(this);
	};
}

void CPointViewBar::RemoveGraph(SGraphChange* sgc)
{
	INIT_WINDOW_MACRO;
	if (!sgc->bRedraw)
	{
		HIDE_WINDOW_MACRO(this);
	};
	long index = -1;
	long graph_num = GetCurrentGraphId();
	//find combobox entry for this graph
	for (int i = 0; i < graph_combo_box.GetCount(); i++)
	{
		if (graph_combo_box.GetItemData(i) == sgc->graphnum)
		{
			index = i;
			break;
		};
	};
	if (index != -1)
	{
		graph_combo_box.DeleteString(index);
		//select next item (if any) in combobox
		if (graph_combo_box.GetCount() == 0)
		{
			point_list_ctrl.DeleteAllItems();
			graph_combo_box.ResetContent();
		}
		else
		{
			if ((graph_combo_box.GetCount() - 1) == sgc->graphnum)
			{
				graph_combo_box.SetCurSel(sgc->graphnum - 1);
				OnSelectionChanged();
			};
		};
	};

	for (int i = 0; i < graph_combo_box.GetCount(); i++)
	{
		graph_combo_box.SetItemData(i, i);
	};

	if (!sgc->bRedraw)
	{
		SHOW_WINDOW_MACRO(this);
	};
}

void CPointViewBar::ClearGraph(SGraphChange* sgc)
{
	INIT_WINDOW_MACRO;
	if (!sgc->bRedraw)
	{
		HIDE_WINDOW_MACRO(this);
	};
	long graph_num = GetCurrentGraphId();
	if (graph_num == sgc->graphnum)
	{
		point_list_ctrl.DeleteAllItems();
	};    
	if (!sgc->bRedraw)
	{
		SHOW_WINDOW_MACRO(this);
	};
}

void CPointViewBar::AddPoint(SGraphChange* sgc)
{
	long graph_num = GetCurrentGraphId();

	if (!b_enabled || graph_num != sgc->graphnum)
	{
		return;
	};

	INIT_WINDOW_MACRO;
	if (!sgc->bRedraw)
	{
		HIDE_WINDOW_MACRO(this);
	};

	LVITEM lv_item;
	memset(&lv_item, 0, sizeof(lv_item));
	lv_item.mask = LVIF_DI_SETITEM;
	lv_item.iItem = sgc->index;
	point_list_ctrl.InsertItem(&lv_item);

	if (!sgc->bRedraw)
	{
		SHOW_WINDOW_MACRO(this);
	};
}

void CPointViewBar::EditPoint(SGraphChange* sgc)
{
	long graph_num = GetCurrentGraphId();

	if (!b_enabled || graph_num != sgc->graphnum)
	{
		return;
	};

	INIT_WINDOW_MACRO;
	if (!sgc->bRedraw)
	{
		HIDE_WINDOW_MACRO(this);
	};

	point_list_ctrl.Update(sgc->index);

	if (!sgc->bRedraw)
	{
		SHOW_WINDOW_MACRO(this);
	};
}

void CPointViewBar::RemovePoint(SGraphChange* sgc)
{
	long graph_num = GetCurrentGraphId();
	if (!b_enabled || graph_num != sgc->graphnum)
	{
		return;
	};
	INIT_WINDOW_MACRO;
	if (!sgc->bRedraw)
	{
		HIDE_WINDOW_MACRO(this);
	};

	point_list_ctrl.DeleteItem(sgc->index);

	if (!sgc->bRedraw)
	{
		SHOW_WINDOW_MACRO(this);
	};
}

long CPointViewBar::GetCurrentGraphId()
{
	long res_id = -1;
	long cur_sel = graph_combo_box.GetCurSel();
	if (cur_sel != CB_ERR)
	{
		res_id = graph_combo_box.GetItemData(cur_sel);
	};
	return res_id;
}

BOOL CPointViewBar::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHDR* nm_header = reinterpret_cast<NMHDR*>(lParam);
	if (wParam == GRAPH_POINT_LIST_CHILD_ID)
	{
		//notification from list control
		switch (nm_header->code)
		{
		case LVN_GETDISPINFO :
			{
				NMLVDISPINFO* disp_info = reinterpret_cast<NMLVDISPINFO*>(lParam);
				OnGetDispInfo(disp_info);
			}; break;
		};
	};
	return CSizingControlBar::OnNotify(wParam, lParam, pResult);
}

void CPointViewBar::SetCurrentGraph(int index)
{
	graph_combo_box.SetCurSel(index);
	
	OnSelectionChanged();
}

void CPointViewBar::SetCurrentGraph(LPCTSTR lpszGraph)
{
	CString strGraph = lpszGraph;
	CGraphWnd* main_wnd = get_main_graph_window();
	int nCount = main_wnd->GetGraphCount();
	for (int index = 0; index< nCount; index++)
	{
		CGraphProps* graph_props = main_wnd->GetGraph(index);
		if( strGraph == graph_props->GetTitle())
			graph_combo_box.SetCurSel(index);
	}
	
	OnSelectionChanged();
}

BOOL CPointViewBar::OnCommand(WPARAM wParam, LPARAM lParam)
{
	WORD ctrl_id = LOWORD(wParam);
	if (ctrl_id == GRAPH_COMBO_CHILD_ID)
	{
		//notification from ComboBox
		WORD notify_id = HIWORD(wParam);
		switch (notify_id)
		{
		case CBN_SELCHANGE :
			{
				OnSelectionChanged();
			}
			break;
		}
	}

	return CSizingControlBar::OnCommand(wParam, lParam);
}

void CPointViewBar::OnSelectionChanged()
{
	InitPointList(NULL);
}

void CPointViewBar::OnGetDispInfo(NMLVDISPINFO* disp_info)
{
	CGraphWnd* main_wnd = get_main_graph_window();

	long current_graph_id = GetCurrentGraphId();
	CGraphProps* grprop = main_wnd->GetGraph(current_graph_id);

	if (grprop != NULL)
	{
		if (disp_info->item.mask & LVIF_TEXT)
		{
			SSinglePoint ssp;
			CString str;
			grprop->GetPoint(disp_info->item.iItem, &ssp);
			switch (disp_info->item.iSubItem)
			{
			case 0:
				{
					main_wnd->FormatAxisOutput(ssp.x, GRAPH_X_AXIS, 1, str);
					lstrcpy(disp_info->item.pszText, str);
				}
				break;
			case 1:
				{
					main_wnd->FormatAxisOutput(ssp.y, GRAPH_Y_AXIS, 1, str);
					lstrcpy(disp_info->item.pszText, str);
				}
				break;
			default:
				break;
			}
		}
	}
}

void CPointViewBar::InitPointList(CGraphWnd* main_wnd)
{
	if (!b_enabled)
	{
		return;
	}
	if (main_wnd == NULL)
	{
		main_wnd = get_main_graph_window();
	}
	long graph_num = GetCurrentGraphId();
	main_wnd->GetGraphPanel()->m_lCurrentGraphId = graph_num;
	main_wnd->GetGraphPanel()->UpdateGraphWindow(NULL);

	if (graph_num != -1)
	{
		CGraphProps* grprop = main_wnd->GetGraph(graph_num);

		if (point_list_ctrl.GetItemCount() != grprop->GetSize())
		{
			point_list_ctrl.SetItemCount(grprop->GetSize());
		}
		else
		{
			if (grprop->GetSize() > 0)
			{
				point_list_ctrl.RedrawItems(0, grprop->GetSize() - 1);
			};
		};
	}
	else
	{
		//no active graph
		point_list_ctrl.DeleteAllItems();
	}
}

void CPointViewBar::OnRButtonUp(UINT nFlags, CPoint point)
{
	CSizingControlBar::OnRButtonUp(nFlags, point);

	ClientToScreen(&point);

	CMenu* menu = new CMenu();
	menu->CreatePopupMenu();

	//append common menu items from parent windows
	EnumerateParentWindows(AppendMenuForAllParents, (void*)menu);

	menu->TrackPopupMenu(TPM_CENTERALIGN, point.x, point.y, this);

	delete menu;
}

void CPointViewBar::AppendMenuItems(CMenu* menu)
{
	//properties
	menu->AppendMenu(MF_STRING | MF_ENABLED, GRAPH_RBMC_VIEW_PROPERTIES, _T("����"));
}

void CPointViewBar::OnRBMenuCommand(UINT command_id)
{
	switch (command_id)
	{
	case GRAPH_RBMC_VIEW_PROPERTIES:
		{
			CPropertySheet prop_sheet(_T("����"), this);
			EnumerateParentWindows(AppendPropertyPageForAllParents, (void*)&prop_sheet);

			UINT result = prop_sheet.DoModal();

			EnumerateParentWindows(ReleasePropertyPageForAllParents, (void*)result);

			if (result == IDOK)
			{
				CGraphWnd* main_wnd = get_main_graph_window();
				main_wnd->UpdateWindows(GRAPH_WUV_ALL);
			};
		}; break;
	};
}

/////////////////////////////////////////////////////////////////////////////
// C3DListCtrl

C3DListCtrl::C3DListCtrl()
{
}

C3DListCtrl::~C3DListCtrl()
{
}


BEGIN_MESSAGE_MAP(C3DListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(C3DListCtrl)
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// C3DListCtrl message handlers
void C3DListCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
	CListCtrl::OnRButtonUp(nFlags, point);

	ClientToScreen(&point);

	CMenu* menu = new CMenu();
	menu->CreatePopupMenu();

	//append common menu items from parent windows
	EnumerateParentWindows(AppendMenuForAllParents, (void*)menu);

	menu->TrackPopupMenu(TPM_CENTERALIGN, point.x, point.y, this);

	delete menu;
}

BOOL C3DListCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	return CListCtrl::PreCreateWindow(cs);
}

LRESULT C3DListCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_COMMAND && (wParam > GRAPH_RBMC_FIRST && wParam < GRAPH_RBMC_LAST))
	{
		//this is right button menu commands
		EnumerateParentWindows(OnRBMenuCommandForAllParents, (void*)wParam);
	};
	return CListCtrl::WindowProc(message, wParam, lParam);
}

