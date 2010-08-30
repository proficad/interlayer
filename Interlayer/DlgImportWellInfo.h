#pragma once
#include "afxwin.h"

class CWellInfoView;
// CDlgImportWellInfo 对话框

class CDlgImportWellInfo : public CDialog
{
	DECLARE_DYNAMIC(CDlgImportWellInfo)

public:
	CDlgImportWellInfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgImportWellInfo();

// 对话框数据
	enum { IDD = IDD_DLG_IMPORT_WELLINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strText;
	CString m_strSeq;
	int m_nSeq;
	BOOL m_bHeader;
	CComboBox m_wndSheet;
	CString m_strLoadFileName;
	UINT m_uLine;
	int m_iFileType;
	CGridCtrl m_wndGridFields;
	CWellInfoView *m_pView;
public:
	void Init();
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnFieldGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnBnClickedBtnFile();
	afx_msg void OnCheckFirstLine();
	afx_msg void OnSelendokComboSheet();
	afx_msg void OnRadioSeq();	
	afx_msg void OnBnClickedFileType();
	afx_msg void OnKillfocusEditCh();
	afx_msg void OnBnClickedOk();
};
