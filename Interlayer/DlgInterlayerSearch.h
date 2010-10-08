#pragma once

#include <afxdockablepane.h>
// DlgInterlayerSearch 对话框

class DlgInterlayerSearch : public CDialog
{
	DECLARE_DYNAMIC(DlgInterlayerSearch)

public:
	DlgInterlayerSearch(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DlgInterlayerSearch();

// 对话框数据
	enum { IDD = 509/*IDD_DLG_INTERSECT_SEARCH*/ };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedDlgIntersectOk();
};

template<class InterlayerSearch>
class CDockableDialogPane : public CDockablePane
{
 DECLARE_DYNAMIC(CDockableDialogPane##InterlayerSearch)
public:
	void AdjustLayout()
	{
		if (GetSafeHwnd() == NULL)
		{
			return;
		}
		CRect rectClient;
		GetClientRect(rectClient);
		m_wndDialog.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + 1, rectClient.Width() - 2, rectClient.Height() - 2, SWP_NOACTIVATE | SWP_NOZORDER);
	}

	virtual BOOL IsResizable() const { return m_bIsResizable; }
	virtual void SetResizeable(bool resizeable) { m_bIsResizable = resizeable; }
	
	CDockableDialogPane(){ m_bIsResizable = TRUE;};
	virtual ~CDockableDialogPane(){};
protected:
	bool m_bIsResizable;
	// CViewTree m_wndFileView;
	DlgInterlayerSearch m_wndDialog;
protected: 
	virtual const AFX_MSGMAP* GetMessageMap() const
	{
		return GetThisMessageMap();
	}
	static const AFX_MSGMAP* PASCAL GetThisMessageMap()
	{
		typedef CDockableDialogPane<InterlayerSearch> ThisClass;  
		typedef CDockablePane TheBaseClass;
		static const AFX_MSGMAP_ENTRY _messageEntries[] =  
		{ 
			ON_WM_CREATE()
			ON_WM_SETFOCUS()
			ON_WM_PAINT()
			ON_WM_SIZE()
			{
				0, 0, 0, 0, AfxSig_end, (AFX_PMSG)0 
			} 
		}; 
		RUNTIME_CLASS
		static const AFX_MSGMAP messageMap = 
		{
			&TheBaseClass::GetThisMessageMap, &_messageEntries[0]
		}; 
		return &messageMap;
	}
public:
	virtual afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (CDockablePane::OnCreate(lpCreateStruct) == -1)
			return -1;
		// TODO: Add your specialized creation code here
		CRect rectDummy;
		rectDummy.SetRectEmpty();
		// 创建视图:
		const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;
		// if (!m_wndFileView.Create(dwViewStyle, rectDummy, this, 4))
		// {
		// TRACE0("未能创建文件视图\n");
		// return -1; // 未能创建
		// }
		if (!m_wndDialog.Create(InterlayerSearch::IDD, this))
		{
			TRACE("未能创建文件视图\n");
			return -1; // 未能创建
		}
		return 0;
	}
	virtual afx_msg void OnSetFocus(CWnd* pOldWnd)
	{
		CDockablePane::OnSetFocus(pOldWnd);
		// m_wndFileView.SetFocus();
		m_wndDialog.SetFocus();
		// TODO: Add your message handler code here
	}
	virtual afx_msg void OnPaint()
	{
		CPaintDC dc(this); // device context for painting
		// TODO: Add your message handler code here
		// Do not call CDockablePane::OnPaint() for painting messages
		CRect rectTree;
		// m_wndFileView.GetWindowRect(rectTree);
		m_wndDialog.GetWindowRect(rectTree);
		ScreenToClient(rectTree);
		rectTree.InflateRect(1, 1);
		dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
	}
	virtual afx_msg void OnSize(UINT nType, int cx, int cy)
	{
		if (!m_bIsResizable)
		{
			return ;
		}
		CDockablePane::OnSize(nType, 300, 180);
		// TODO: Add your message handler code here
		AdjustLayout();
		m_wndDialog.SendMessage(WM_SIZE,0,0);
		//m_bIsResizable = FALSE;
	}
};