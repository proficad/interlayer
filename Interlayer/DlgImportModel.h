#pragma once


// CDlgImportModel

class CDlgImportModel : public CFileDialog
{
	DECLARE_DYNAMIC(CDlgImportModel)

public:
	CDlgImportModel(BOOL bOpenFileDialog, // 对于 FileOpen 为 TRUE，对于 FileSaveAs 为 FALSE
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLETEMPLATE,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);
	virtual ~CDlgImportModel();

	CComboBox	m_wndFileType;
	int		m_nFileType;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnSelendokComboType();
	DECLARE_MESSAGE_MAP()
};


