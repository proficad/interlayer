#pragma once
#include "afxwin.h"


// CDlgTrangleExport �Ի���

class CDlgTrangleExport : public CDialog
{
	DECLARE_DYNAMIC(CDlgTrangleExport)

public:
	CDlgTrangleExport(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgTrangleExport();

// �Ի�������
	enum { IDD = IDD_TRANGLEEXPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
