
#pragma once

enum NODE_STYLE // 节点类型
{
	FOLDER=0,				// 文件夹
	WELL_DAT,				// 井基础数据
	WELL_INFO,				// 井基础信息数据
	WELL_HOLE,				// 井眼轨迹数据
	WELL_LOGING_INFO,		// 测井曲线数据集
	WELL_LOGINGS,			// 测井曲线数据集
	WELL_LOGING,			// 测井曲线名称
	WELL_PRO,				// 生产井
	WELL_INJ,				// 注入井

	FILE_POINT,				// 点
	FILE_LINE,				// 线
	FILE_POLYGON,			// 多边形
	FILE_PLANE,				// 面
	FILE_PICTRUE,			// 图片
	FILE_NORMAL,			// 法线
	FILE_LAYER,				// 夹层
	
	FILE_DAT,				// 文件数据(包括Word文件、Excel等)

	FILE_VERT_MODELS,		// 纵向模型集
	FILE_VERT_MODEL,		// 纵向模型
	FILE_LAYER_MODEL,		// 夹层模型
	MODEL_NORMAL,			// 夹层模型下的法线
	MODEL_LAYER,			// 夹层模型下的夹层
	
	GRID_DAT=1,				// 模型
	FARM_DAT,				// 场数据
	FARM_LAYER,				// 每层数据

	GRAPHICS_2DS=1,			// 二维视图集
	GRAPHICS_3DS,			// 三维视图集
	GRAPHICS_LOGS,			// 测井曲线视图集
	FILE_2D,				// 二维图文件
	FILE_3D,				// 二维图文件
	FILE_LOG,				// 测井曲线图文件
};

enum
{
	CUR_COPY_ROOT,
	CUR_COPY_SON,
	CUR_MOVE_ABOVE,
	CUR_COPY_ABOVE,
	CUR_MOVE_ROOT,
	CUR_MOVE_SON,
	CUR_ADD_DAT,
	CUR_NODROP,	
	CUR_COUNT,
};

static const int SCROLL_SPEED = 100;

class CTreeNodeDat : public CObject
{
	DECLARE_SERIAL(CTreeNodeDat);
public:
	CTreeNodeDat();
	~CTreeNodeDat();
	
	//------------------------------------------------------------------------
	// 属性
	UINT m_uState;			// 状态
	int m_nImage;			// 图标索引
	int m_nSelectedImage;	// 选择图标索引
	NODE_STYLE m_nType;		// 节点类别

	CString m_strFileName;	// 文件名

	CObject *m_pNodeDat;	// 结点数据

	//------------------------------------------------------------------------
	// 方法
	virtual void Serialize(CArchive& ar);
	CTreeNodeDat& operator=(const CTreeNodeDat& dat);
	friend inline CArchive& operator <<(CArchive& ar, const CTreeNodeDat& dat);
	friend inline CArchive& operator >>(CArchive& ar, CTreeNodeDat& dat);
};

/////////////////////////////////////////////////////////////////////////////
// CViewTree 窗口

class CViewTree : public CTreeCtrl
{
// 构造
public:
	CViewTree();
	typedef void (CViewTree::*PARSING_FUNCTION)(HTREEITEM hti, void *pvParam);
	typedef bool (CViewTree::*OPENTREE_FUNCTION)(CTreeNodeDat *pNodeDat, void *pvParam);

	enum TREETYPE{ fileTree, modelTree, graphTree }; // 文件树、类型树、图形窗口树
	TREETYPE	m_eTreeType;
	
	BOOL		m_bDragging;
	HCURSOR		m_hCursor;
	HTREEITEM m_hitemDrag;
	HTREEITEM m_hitemDrop;
	HCURSOR m_ahCursor[CUR_COUNT];
	BOOL m_bTimerABOVE_Active;
	BOOL m_bTimerBELOW_Active;
	UINT m_uOldFlags;

	BOOL m_bInsertionMode;

	static CLIPFORMAT m_cfTree; // 树剪切板

	int m_nCountImages;

	bool m_bNew3D;

	CImageList *m_pDragImage;
	HTREEITEM m_hSelectItem;
// 重写
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	enum RECORDTYPE{ recordRegular, recordPush, recordPop }; // 正常记录、	

	HTREEITEM m_hItems[100];
	int m_nIndex;

	bool m_bModified;					// 树是否修改过		

// 实现
public:
	virtual ~CViewTree();

	OPENTREE_FUNCTION pfOpenTree;
	int GetIcon(LPCSTR lpszPath, UINT uFlags);
	void GetNormalAndSelectedIcons (LPCSTR lpszPath, int &iImage, int &iSelectedImage);
	int GetIcons();

protected:
	DECLARE_MESSAGE_MAP()
public:
	//------------------------------------------------------------------------
	// 串行化
	virtual void Serialize(CArchive& ar);
	HTREEITEM ReadTreeViewContent(CArchive& ar, HTREEITEM hItem);
	void RecursiveWriteItems(CArchive& ar, HTREEITEM hItem); // 递归函数
	void WriteTreeViewContent(CArchive& ar, HTREEITEM hItem);
	void RestoreStates(HTREEITEM hItem);

	//////////////////////////////////////////////////////////////////////////
	bool IsModified();
	void SetModifiedFlag(bool bFlag = true);

	//------------------------------------------------------------------------
	// 	
	void OnRefresh();
	void OnDeleteItem();
	void DeleteUnusedFile(LPCSTR lpszPathName);

	//------------------------------------------------------------------------
	// 
	BOOL AddSiblingFolder(CObject *pNodeDat = NULL, int iImage = 0);
	BOOL AddChildFolder(CObject *pNodeDat = NULL, int iImage = 0);
	//------------------------------------------------------------------------
	// bCase 区分大小写
	HTREEITEM GetChildNode(HTREEITEM hti, LPCSTR lpszNodeName, BOOL bCase = TRUE);
	BOOL IsExistChildNode(HTREEITEM hti, LPCSTR lpszNodeName, BOOL bCase = TRUE);
	BOOL IsExistChildNode(HTREEITEM hti, HTREEITEM htiChild);
	HTREEITEM IsExistFileInTree(HTREEITEM hItem, const CString &strFileName);
	//------------------------------------------------------------------------
	// 
	BOOL SetCheck(HTREEITEM hItem, UINT uCheck);
	int GetCheck(HTREEITEM hItem);

	int IsExistChildBold(HTREEITEM hItem);
	void VerifBoldParents(HTREEITEM hItem);

	int IsAllChildEmpty(HTREEITEM hItem);
	int IsAllChildFull(HTREEITEM hItem);
	void VerifFullParents(HTREEITEM hItem);	
	
	void SetItemFull(HTREEITEM hItem);
	void SetItemEmpty(HTREEITEM hItem);
	void SetItemAndChildrenFull(HTREEITEM hItem);
	void SetItemAndChildrenEmpty(HTREEITEM hItem);
	//------------------------------------------------------------------------
	// 
	BOOL DeleteItemM(HTREEITEM hItem);
	BOOL ExpandM(HTREEITEM hItem, UINT nCode);
	BOOL SelectVisibleItem(HTREEITEM hti);
	void FreeNodeData(HTREEITEM hItem);

	void FreeAllChildNodeData(HTREEITEM hItem);
	BOOL DeleteAllChildItem(HTREEITEM hItem);

	void ParseTree(PARSING_FUNCTION pf, HTREEITEM hti = TVI_ROOT, void *pvParam = NULL);
	void pfCollapse(HTREEITEM hti, void *);
	void pfExpand(HTREEITEM hti, void *);
	void OnExpandAllTree();
	void OnCollapseAllEntireTree();
	//------------------------------------------------------------------------
	void OnEditClear();
	void OnUpdateEditClear(CCmdUI *pCmdUI);
	void OnEditCopy();
	void OnUpdateEditCopy(CCmdUI *pCmdUI);
	void OnEditCut();
	void OnUpdateEditCut(CCmdUI *pCmdUI);
	void OnEditPaste();
	void OnUpdateEditPaste(CCmdUI *pCmdUI);
	void OnRename();
	void OnUpdateRename(CCmdUI *pCmdUI);
	//------------------------------------------------------------------------
	HTREEITEM SelectDropCursor(CPoint *pPoint = NULL);
	BOOL IsChildNodeOf(HTREEITEM hitemChild, HTREEITEM hitemSuspectedParent);
	void SaveTreeData(CSharedFile *pFile,HTREEITEM hItem);
	HTREEITEM LoadTreeData(HGLOBAL hMem, HTREEITEM hItem);
	//------------------------------------------------------------------------
	// 
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnItemexpanded(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};
