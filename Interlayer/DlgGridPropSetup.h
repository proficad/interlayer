#pragma once


// CDlgGridPropSetup 对话框

class CDlgGridPropSetup : public CDialog
{
	DECLARE_DYNAMIC(CDlgGridPropSetup)

public:
	CDlgGridPropSetup(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgGridPropSetup();

// 对话框数据
	enum { IDD = IDD_DLG_GRID_PROP_SETUP };
	CMFCColorButton	m_wndMinColor;
	CMFCColorButton	m_wndMaxColor;
	double	m_dMinValue;
	double	m_dMaxValue;
	double	m_dDispMinValue;
	double	m_dDispMaxValue;
	CString	m_strGridInfo;

	COLORREF m_clrMinColor;
	COLORREF m_clrMaxColor;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtnMinc();
	afx_msg void OnBnClickedBtnMaxc();
	virtual BOOL OnInitDialog();
};
