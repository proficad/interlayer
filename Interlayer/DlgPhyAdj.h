#pragma once
#include "afxwin.h"


// CDlgPhyAdj 对话框
class CGLObject;
class CDlgPhyAdj : public CDialog
{
	DECLARE_DYNAMIC(CDlgPhyAdj)

public:
	CDlgPhyAdj(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgPhyAdj();

// 对话框数据
	enum { IDD = IDD_DLG_PHY_ADJ };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_ctr_phyname;
	CString m_newphyname;
	CGLObject	*m_pObj;
	int				m_index;
	void				SetGLObj(CGLObject* pObj);
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeCombo1();
};
