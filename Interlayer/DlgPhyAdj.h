#pragma once
#include "afxwin.h"


// CDlgPhyAdj �Ի���
class CGLObject;
class CDlgPhyAdj : public CDialog
{
	DECLARE_DYNAMIC(CDlgPhyAdj)

public:
	CDlgPhyAdj(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgPhyAdj();

// �Ի�������
	enum { IDD = IDD_DLG_PHY_ADJ };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
