#pragma once


// CDlgLayerParamSet 对话框

class CDlgLayerParamSet : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLayerParamSet)

public:
	CDlgLayerParamSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgLayerParamSet();

// 对话框数据
	enum { IDD = IDD_DLG_INTERLAYER_PARA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	double m_dLayers;
	double m_dNormals;
	double m_dVerts;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
