#pragma once


// CDlgRuler �Ի���

class CDlgRuler : public CDialog
{
	DECLARE_DYNAMIC(CDlgRuler)

public:
	CDlgRuler(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgRuler();

// �Ի�������
	enum { IDD = IDD_DLG_TRIANGULAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bRulerIsVisable;
	double m_dAngle;
};
