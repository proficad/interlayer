
#pragma once

enum NODE_STYLE // �ڵ�����
{
	FOLDER=0,				// �ļ���
	WELL_DAT,				// ����������
	WELL_INFO,				// ��������Ϣ����
	WELL_HOLE,				// ���۹켣����
	WELL_LOGING_INFO,		// �⾮�������ݼ�
	WELL_LOGINGS,			// �⾮�������ݼ�
	WELL_LOGING,			// �⾮��������
	WELL_PRO,				// ������
	WELL_INJ,				// ע�뾮

	FILE_POINT,				// ��
	FILE_LINE,				// ��
	FILE_POLYGON,			// �����
	FILE_PLANE,				// ��
	FILE_PICTRUE,			// ͼƬ
	FILE_NORMAL,			// ����
	FILE_LAYER,				// �в�
	
	FILE_DAT,				// �ļ�����(����Word�ļ���Excel��)

	FILE_VERT_MODELS,		// ����ģ�ͼ�
	FILE_VERT_MODEL,		// ����ģ��
	FILE_LAYER_MODEL,		// �в�ģ��
	MODEL_NORMAL,			// �в�ģ���µķ���
	MODEL_LAYER,			// �в�ģ���µļв�
	
	GRID_DAT=1,				// ģ��
	FARM_DAT,				// ������
	FARM_LAYER,				// ÿ������

	GRAPHICS_2DS=1,			// ��ά��ͼ��
	GRAPHICS_3DS,			// ��ά��ͼ��
	GRAPHICS_LOGS,			// �⾮������ͼ��
	FILE_2D,				// ��άͼ�ļ�
	FILE_3D,				// ��άͼ�ļ�
	FILE_LOG,				// �⾮����ͼ�ļ�
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
	// ����
	UINT m_uState;			// ״̬
	int m_nImage;			// ͼ������
	int m_nSelectedImage;	// ѡ��ͼ������
	NODE_STYLE m_nType;		// �ڵ����

	CString m_strFileName;	// �ļ���

	CObject *m_pNodeDat;	// �������

	//------------------------------------------------------------------------
	// ����
	virtual void Serialize(CArchive& ar);
	CTreeNodeDat& operator=(const CTreeNodeDat& dat);
	friend inline CArchive& operator <<(CArchive& ar, const CTreeNodeDat& dat);
	friend inline CArchive& operator >>(CArchive& ar, CTreeNodeDat& dat);
};

/////////////////////////////////////////////////////////////////////////////
// CViewTree ����

class CViewTree : public CTreeCtrl
{
// ����
public:
	CViewTree();
	typedef void (CViewTree::*PARSING_FUNCTION)(HTREEITEM hti, void *pvParam);
	typedef bool (CViewTree::*OPENTREE_FUNCTION)(CTreeNodeDat *pNodeDat, void *pvParam);

	enum TREETYPE{ fileTree, modelTree, graphTree }; // �ļ�������������ͼ�δ�����
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

	static CLIPFORMAT m_cfTree; // �����а�

	int m_nCountImages;

	bool m_bNew3D;

	CImageList *m_pDragImage;
	HTREEITEM m_hSelectItem;
// ��д
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	enum RECORDTYPE{ recordRegular, recordPush, recordPop }; // ������¼��	

	HTREEITEM m_hItems[100];
	int m_nIndex;

	bool m_bModified;					// ���Ƿ��޸Ĺ�		

// ʵ��
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
	// ���л�
	virtual void Serialize(CArchive& ar);
	HTREEITEM ReadTreeViewContent(CArchive& ar, HTREEITEM hItem);
	void RecursiveWriteItems(CArchive& ar, HTREEITEM hItem); // �ݹ麯��
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
	// bCase ���ִ�Сд
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
