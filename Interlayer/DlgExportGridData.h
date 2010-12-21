#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CDlgExportGridData �Ի���

class CDlgExportGridData : public CDialog
{
	DECLARE_DYNAMIC(CDlgExportGridData)

public:
	CDlgExportGridData(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgExportGridData();

// �Ի�������
	enum { IDD = IDD_EXPORT_GRID };

protected:
	bool				m_bCoordinate;
	bool				m__bRedefine;

	bool				m_bTransRate;
	bool				m_bRedefineTransRate;
	
	bool				m_bCenterPoint;
	
	bool				m_bProperty;
	bool				m_bRedefineProperty;

	bool				m_bChangeProperty;
	bool				m_bRedefineChangeProperty;

	double			m_dChangePropertyValue;
	CString			m_csChangePropertyName;

	HTREEITEM	m_ModelItem;
	std::vector<CString>	m_strProperty;
	std::vector<CString> m_strGridLayer;
	std::vector<bool>		m_bPropertyExport;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_strFilePath;
	double m_changeValue;
	
	CTreeCtrl m_ctrTree;
	CEdit m_filenameEditCtr;
	CComboBox m_Ctr_pro_comb;
	CButton m_ctr_check5;

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
	afx_msg void OnBnClickedCheck7();

	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio4();
	afx_msg void OnCbnSelchangeCombo1();
};
