#pragma once
#include "afxwin.h"


// CDlgAddCtrlPoint �Ի���

class CDlgAddCtrlPoint : public CDialog
{
	DECLARE_DYNAMIC(CDlgAddCtrlPoint)

public:
	CDlgAddCtrlPoint(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgAddCtrlPoint();

// �Ի�������
	enum { IDD = IDD_DLG_EDIT_CTRL_POINT };
	CComboBox m_comboWellName;
	double m_x;
	double m_y;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();	
	afx_msg void OnCbnSelendokComboWellname();
	CString m_strWellName;
};
