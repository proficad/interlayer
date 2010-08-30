// WatchBar.cpp: implementation of the CWatchBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "displogcurve.h"
#include "WatchBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#define ID_LIST_1	1

/////////////////////////////////////////////////////////////////////////////
// CWatchBar

CWatchBar::CWatchBar()
{
}

CWatchBar::~CWatchBar()
{
}


BEGIN_MESSAGE_MAP(CWatchBar, CDockablePane)
//{{AFX_MSG_MAP(CWatchBar)
ON_WM_CREATE()
ON_WM_SIZE()
ON_WM_PAINT()
ON_WM_SETFOCUS()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWatchBar message handlers

int CWatchBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_Font.CreateStockObject (DEFAULT_GUI_FONT);
	
	CRect rectDummy;
	rectDummy.SetRectEmpty ();
	
	// Create output pane:
	const DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_VSCROLL | LVS_REPORT;
	
	m_wndWatch.Create (dwStyle, rectDummy, this, ID_LIST_1);
	m_wndWatch.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0,
		LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_wndWatch.InsertColumn (0, _T("No."), LVCFMT_LEFT, 30);
	m_wndWatch.InsertColumn (1, _T("曲线名称"), LVCFMT_LEFT, 100);
	m_wndWatch.InsertColumn (2, _T("当前井深"), LVCFMT_LEFT, 100);
	m_wndWatch.InsertColumn (3, _T("当前值"), LVCFMT_LEFT, 100);
	m_wndWatch.InsertColumn (4, _T("井段"), LVCFMT_LEFT, 150);
	m_wndWatch.InsertColumn (5, _T("最小值"), LVCFMT_LEFT, 100);
	m_wndWatch.InsertColumn (6, _T("最大值"), LVCFMT_LEFT, 100);
	m_wndWatch.InsertColumn (7, _T("平均值"), LVCFMT_LEFT, 100);
	m_wndWatch.InsertColumn (8, _T("面积"), LVCFMT_LEFT, 100);
		
	m_wndWatch.SetFont (&m_Font);
	
	return 0;
}

void CWatchBar::OnSize(UINT nType, int cx, int cy) 
{
	CDockablePane::OnSize(nType, cx, cy);
	
	if (CanAdjustLayout ())
	{
		CRect rc;
		GetClientRect(rc);
		
		m_wndWatch.SetWindowPos(NULL,
			rc.left + 1, rc.top + 1,
			rc.Width() - 2, rc.Height () - 2,
			SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CWatchBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rectTree;
	m_wndWatch.GetWindowRect (rectTree);
	ScreenToClient (rectTree);
	
	rectTree.InflateRect (1, 1);
	dc.Draw3dRect (rectTree, ::GetSysColor (COLOR_3DSHADOW), ::GetSysColor (COLOR_3DSHADOW));
}

void CWatchBar::OnSetFocus(CWnd* pOldWnd) 
{
	CDockablePane::OnSetFocus(pOldWnd);
	
	m_wndWatch.SetFocus ();
	
}
