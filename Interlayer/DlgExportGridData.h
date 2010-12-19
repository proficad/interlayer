#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CDlgExportGridData 对话框

class CDlgExportGridData : public CDialog
{
	DECLARE_DYNAMIC(CDlgExportGridData)

public:
	CDlgExportGridData(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgExportGridData();

// 对话框数据
	enum { IDD = IDD_EXPORT_GRID };

protected:
	bool				m_bCoordinate;
	bool				m__bRedefine;
	bool				m_bRedefineProperty;
	bool				m_bTransRate;
	bool				m_bCenterPoint;
	bool				m_bProperty;
	HTREEITEM	m_ModelItem;
	std::vector<CString>	m_strProperty;
	std::vector<CString> m_strGridLayer;
	std::vector<bool>		m_bPropertyExport;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strFilePath;
	CTreeCtrl m_ctrTree;
	CEdit m_filenameEditCtr;
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedCheck2();
	afx_msg void OnBnClickedCheck3();
	afx_msg void OnBnClickedCheck4();
	afx_msg void OnBnClickedCheck6();
	virtual BOOL OnInitDialog();
	void				SetModelItem(HTREEITEM model);
	void				SaveExport();
protected:
	bool				FillProperty();
public:
	afx_msg void OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton1();

	afx_msg void OnBnClickedCheck5();
	afx_msg void OnBnClickedOk();
};
