
// MainFrm.h : CMainFrame 类的接口
//

#pragma once
#include "FileView.h"
#include "ModelView.h"
#include "GraphView.h"
#include "OutputWnd.h"
#include "PropertiesWnd.h"
#include "FormatBar.h"
#include "./GeoMap/LayerBar.h"
#include "3DLib/3DObjBar.h"

#define WM_GETITEMTITLE_FILE WM_USER+100
#define WM_GETITEMTITLE_MODEL WM_USER+101
#define WM_GETITEMTITLE_GRAPH WM_USER+102

#define WM_CREATE_FILE_LOG WM_USER+103				// 创建测井曲线图

class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// 属性
public:
	CString			m_strPrjFileName;	// 工程文件名
	CString			GetProjectDatPath();// 获取工程数据路径
// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

	CMFCStatusBar& GetStatusBar ()
	{
		return m_wndStatusBar;
	}
	CDocument* CreateOrActivateFrame(CDocTemplate * pTemplate, CRuntimeClass * pViewClass, LPCTSTR lpszFileName = NULL);
	
	bool OpenProjectFile(LPCSTR lpszFileName);
	bool SaveProjectFile(LPCSTR lpszFileName);

	CFileView*         GetTreeFileView(){ return &m_wndFileView; }
	CModelView*        GetTreeModelView(){ return &m_wndModelView; }
	CGraphView*        GetTreeGraphView(){ return &m_wndGraphView; }
	CLayerBar*         GetLayerToolbar(){ return &m_wndToolBarLayer; }
	C3DObjBar*		   Get3DBar(){return &m_wnd3DObjBar;}
	CLayerIntersectSearch* GetSearchBar(){return &m_wndSearchBar;}

	BOOL SaveEmptyOleDocument(LPCTSTR lpszFileName);
	void ActiveView(CView *pView);
	void DeactiveView(CView *pView);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
	CMFCMenuBar       m_wndMenuBar;	
	CMFCToolBar       m_wndToolBar;

	CMFCToolBar       m_wnd3DModelToolBar;
	CMFCToolBar       m_wnd2DModelToolBar;

	//------------------------------------------------------------------------
	// 
	CMFCToolBar  		m_wndToolBarDraw;
	CFormatBar			m_wndFormatBar;
	CLayerBar			m_wndToolBarLayer;

	CMFCDropDownToolBar	m_wndToolSort;
	CMFCDropDownToolBar	m_wndToolRect;
	CMFCDropDownToolBar	m_wndToolCurve;
	CMFCDropDownToolBar	m_wndToolZoom;
	CMFCDropDownToolBar	m_wndToolPoint;
	CMFCDropDownToolBar	m_wndToolAligin;
	CMFCDropDownToolBar	m_wndToolAttri;
	CMFCDropDownToolBar	m_wndToolCtrlPt;	
	
	//------------------------------------------------------------------------
	// 
	
	CMFCStatusBar     m_wndStatusBar;
	
	CMFCToolBarImages m_UserImages;
	
	CFileView         m_wndFileView;
	CModelView        m_wndModelView;
	CGraphView        m_wndGraphView;

	//------------------------------------------------------------------------
	C3DObjBar		  m_wnd3DObjBar;
	CLayerIntersectSearch	m_wndSearchBar;

//	COutputWnd        m_wndOutput;
//	CPropertiesWnd    m_wndProperties;	

// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
	BOOL CloseProjectFile();
public:	
	afx_msg void OnFileNewPrj();
	afx_msg void OnFileOpenPrj();
	afx_msg void OnFileClosePrj();
	afx_msg void OnUpdateFileSavePrj(CCmdUI *pCmdUI);
	afx_msg void OnFileSavePrj();
	afx_msg void OnUpdateFileClosePrj(CCmdUI *pCmdUI);
	afx_msg void OnFileSaveAsPrj();
	afx_msg void OnUpdateFileSaveAsPrj(CCmdUI *pCmdUI);	
	afx_msg void OnFileNew();
	afx_msg void OnUpdateFileNew(CCmdUI *pCmdUI);
	afx_msg void OnFileOpen();
	afx_msg void OnUpdateFileOpen(CCmdUI *pCmdUI);
	afx_msg void OnFileSaveAll();
	afx_msg void OnUpdateFileSaveAll(CCmdUI *pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnVerticalModel();
	afx_msg void OnGenerateLayer();
	afx_msg void OnInterlayerSpaceSet();
	afx_msg LRESULT OnGetItemTitleOfFile(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnGetItemTitleOfModel(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnGetItemTitleOfGraph(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnCreateFileLog(WPARAM wp, LPARAM lp);
};


