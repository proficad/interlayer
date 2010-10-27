#pragma once


// CDlgInterlayerParaRange 对话框

class CDlgInterlayerParaRange : public CDialog
{
	DECLARE_DYNAMIC(CDlgInterlayerParaRange)

public:
	CDlgInterlayerParaRange(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgInterlayerParaRange();

// 对话框数据
	enum { IDD = IDD_DLG_PHYPARARANGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	double m_rangeMin;
	double m_rangeMax;
};
