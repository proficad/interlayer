#pragma once
#include "afxwin.h"

enum AdjType
{
	ADJ_TYPE_VOLUME=0,
	ADJ_TYPE_Z=1
};
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
	CComboBox m_ctrType;
	CString m_newphyname;
	CGLObject	*m_pObj;
	int				m_adjType;
	int				m_index;
	void				SetGLObj(CGLObject* pObj);
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnCbnSelchangeComboType();
};
