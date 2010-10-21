
#pragma once

#include "ViewTree.h"

class CFileViewToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CFileViewObj : public CObject
{
	DECLARE_SERIAL(CFileViewObj);
public:
	CFileViewObj();
	~CFileViewObj();

	
	//------------------------------------------------------------------------
	// 属性
	CString m_strFileName;			// 文件名

	//------------------------------------------------------------------------
	// 方法
	virtual void Serialize(CArchive& ar);
	CFileViewObj& operator=(const CFileViewObj& dat);
	friend inline CArchive& operator <<(CArchive& ar, const CFileViewObj& dat);
	friend inline CArchive& operator >>(CArchive& ar, CFileViewObj& dat);
};

class CWellInfo : public CObject
{
	DECLARE_SERIAL(CWellInfo);
public:
	CWellInfo()
	{
		m_dElevation = 0;				// 补芯海拔
		m_dTop = 0;						// 油层顶
		m_dBottom = 0;
	};
	~CWellInfo(){};
	//------------------------------------------------------------------------
	// 属性
	CString m_strWellName;
	CString m_strWellType;
	double	m_x;
	double	m_y;
	double	m_dElevation;				// 补芯海拔
	double	m_dTop;						// 油层顶
	double	m_dBottom;					// 油层底
	//------------------------------------------------------------------------
	// 方法
	virtual void Serialize(CArchive& ar);
	CWellInfo& operator=(const CWellInfo& dat);
	friend inline CArchive& operator <<(CArchive& ar, const CWellInfo& dat);
	friend inline CArchive& operator >>(CArchive& ar, CWellInfo& dat);
};

class CFileView : public CDockablePane
{
// 构造
public:
	CFileView();

	void AdjustLayout();
	void OnChangeVisualStyle();
	BOOL GetSysImgList();

// 属性
protected:

	CViewTree m_wndFileView;
	CImageList m_FileViewImages;
	CImageList m_TreeStateImages;
	CFileViewToolBar m_wndToolBar;

	CImageList *m_pImagelist;

public:
	void FillFileView(LPCTSTR lpszItem = NULL, int nImage = 0, int nSelectedImage = 0, 
		HTREEITEM hParent = TVI_ROOT, 
		HTREEITEM hInsertAfter = TVI_LAST);

	HTREEITEM AddTreeItem(LPCTSTR lpszItem, int nImage = 0, int nSelectedImage = 0, 
		HTREEITEM hParent = TVI_ROOT, 
		HTREEITEM hInsertAfter = TVI_LAST);

	void LoadTree(CArchive& ar);
	void SaveTree(CArchive& ar);
	void EmptyTree();
	CViewTree * GetTreeCtrl() { return &m_wndFileView; }

	bool OpenTree(CTreeNodeDat *lpNodeDat, void *lpVoid);
// 实现
public:
	virtual ~CFileView();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileOpen();
	afx_msg void OnFileNew();
	afx_msg void OnUpdateFileNew(CCmdUI *pCmdUI);
	afx_msg void OnEditRename();
	afx_msg void OnUpdateEditRename(CCmdUI *pCmdUI);
	afx_msg void OnAddChild();
	afx_msg void OnUpdateAddChild(CCmdUI *pCmdUI);
	afx_msg void OnNewFolder();
	afx_msg void OnUpdateNewFolder(CCmdUI *pCmdUI);
	afx_msg void OnAddNewFile();
	afx_msg void OnUpdateAddNewFile(CCmdUI *pCmdUI);
	afx_msg void OnExpandAllTree();
	afx_msg void OnCollapseAllEntireTree();
	afx_msg void OnNewWellDat();
	afx_msg void OnUpdateNewWellDat(CCmdUI *pCmdUI);
	afx_msg void OnImportPoint();
	afx_msg void OnUpdateImportPoint(CCmdUI *pCmdUI);
	afx_msg void OnImportLine();
	afx_msg void OnUpdateImportLine(CCmdUI *pCmdUI);
	afx_msg void OnImportPolygon();
	afx_msg void OnUpdateImportPolygon(CCmdUI *pCmdUI);
	afx_msg void OnImportPlane();
	afx_msg void OnUpdateImportPlane(CCmdUI *pCmdUI);
	afx_msg void OnImportPictrue();
	afx_msg void OnUpdateImportPictrue(CCmdUI *pCmdUI);
	afx_msg void OnImportTrack();
	afx_msg void OnUpdateImportTrack(CCmdUI *pCmdUI);

	afx_msg void OnEditClear();
	afx_msg void OnUpdateEditClear(CCmdUI *pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI *pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI *pCmdUI);
	afx_msg void OnExportTriangle();
	afx_msg void OnUpdateExportTriangle(CCmdUI *pCmdUI);
};

