#pragma once
#include "afxwin.h"


// CDlgTrangleExport 对话框

class CDlgTrangleExport : public CDialog
{
	DECLARE_DYNAMIC(CDlgTrangleExport)

public:
	CDlgTrangleExport(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgTrangleExport();

// 对话框数据
	enum { IDD = IDD_TRANGLEEXPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	double m_x;
	double m_y;
	double m_z;
	CString m_strFileName;
	CEdit m_filenameEditCtr;
	UINT m_index;
};
