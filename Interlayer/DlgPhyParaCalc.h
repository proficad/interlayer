#pragma once
#include "afxwin.h"


// CDlgPhyParaCalc 对话框
class CGLObject;
class CDlgPhyParaCalc : public CDialog
{
	DECLARE_DYNAMIC(CDlgPhyParaCalc)

public:
	CDlgPhyParaCalc(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgPhyParaCalc();

// 对话框数据
	enum { IDD = IDD_DLG_PHY_CALC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_combo_phy1;
	CComboBox m_combo_phy2;
	CComboBox m_combo_operator;
	CComboBox m_combo_operator1;
	CComboBox m_combo_operator2;
	bool	m_singleOperator;
	float m_phy1;
	float m_phy2;
	CString m_new_phyname;
	CGLObject *m_pGLObj;
	CEdit m_ctr_para1;
	CEdit m_ctr_para2;
	CEdit m_ctr_phynewname;
	CString m_op0;
	CString m_op1;
	CString m_op2;
	CString m_phyname1;
	CString m_phyname2;
	int		m_index_op0;
	int		m_index_op1;
	int		m_index_op2;
	int		m_index_phyname1;
	int		m_index_phyname2;
	void				SetGLObj(CGLObject* pObj);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio4();
	afx_msg void OnCbnSelchangeComboPhy1();
	afx_msg void OnCbnSelchangeComboOperator1();
	afx_msg void OnCbnSelchangeComboOperator();
	afx_msg void OnCbnSelchangeComboPhy2();
	afx_msg void OnCbnSelchangeComboOperator2();
};
