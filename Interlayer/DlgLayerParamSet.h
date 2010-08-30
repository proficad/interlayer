#pragma once


// CDlgLayerParamSet �Ի���

class CDlgLayerParamSet : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLayerParamSet)

public:
	CDlgLayerParamSet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgLayerParamSet();

// �Ի�������
	enum { IDD = IDD_DLG_INTERLAYER_PARA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	double m_dLayers;
	double m_dNormals;
	double m_dVerts;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
