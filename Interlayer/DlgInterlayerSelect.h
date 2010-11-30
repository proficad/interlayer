#pragma once
#include "afxcmn.h"
#include "ViewTree.h"
#include "afxwin.h"

//class CInterSectThread : public CWinThread
//{
//DECLARE_DYNCREATE(CInterSectThread)
//protected:
//	 CInterSectThread();
//	 ~CInterSectThread();
//	 // Attributes
//public:
//	 // Operations
//public:
//
//public:
//	virtual BOOL InitInstance();
//	virtual int ExitInstance();
//
//DECLARE_MESSAGE_MAP()
//};

// CDlgInterlayerSelect 对话框
class CTreeCtrl;
class CDlgInterlayerSelect : public CDialog
{
	DECLARE_DYNAMIC(CDlgInterlayerSelect)

public:
	CDlgInterlayerSelect(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgInterlayerSelect();

	void	InitTree();
	void AddChildItems(CViewTree *srcTree, HTREEITEM src, HTREEITEM dest);
	void SearchInterlayerGrid();
	void AddSearchCheckedItem(HTREEITEM dest);
	void AddSearchCheckedChildren(HTREEITEM dest, HTREEITEM hItem);

	void DeleteAllChildren(HTREEITEM hItem);
	void SetAllChildrenCheck(CTreeCtrl* tree, HTREEITEM hItem, bool check);
// 对话框数据
	enum { IDD = IDD_DLG_SELECT_INTERSECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();
public:
	virtual BOOL DestroyWindow();
protected:
	UINT								m_uiNextPointBarIndex;
public:
	CTreeCtrl						m_ctrFileTree;
	CTreeCtrl						m_ctrSearchTree;
	CEdit							m_ctrGridModel;
	CString							m_strGridModelName;
	CString							m_strGuid;

	afx_msg void OnBnClickedButtonAddpointbar();
	afx_msg void OnBnClickedButtonAddinterlayers();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedOk();
	
	afx_msg void OnNMClickFileTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickSearchTree(NMHDR *pNMHDR, LRESULT *pResult);
};
