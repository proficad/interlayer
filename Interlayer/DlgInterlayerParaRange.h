#pragma once


// CDlgInterlayerParaRange �Ի���

class CDlgInterlayerParaRange : public CDialog
{
	DECLARE_DYNAMIC(CDlgInterlayerParaRange)

public:
	CDlgInterlayerParaRange(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgInterlayerParaRange();

// �Ի�������
	enum { IDD = IDD_DLG_PHYPARARANGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	double m_rangeMin;
	double m_rangeMax;
};
