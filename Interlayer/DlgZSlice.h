#pragma once


// CDlgZSlice 对话框

class CDlgZSlice : public CDialog
{
	DECLARE_DYNAMIC(CDlgZSlice)

public:
	CDlgZSlice(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgZSlice();

// 对话框数据
	enum { IDD = IDD_DLG_SLICE_Z };

	double m_dMinZ, m_dMaxZ;
	CString m_strTitle;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	double m_dZ;
	BOOL m_bSpecified;
	double m_dHeight;
	virtual BOOL OnInitDialog();
	BOOL m_bShow;
	BOOL m_bShowSlicePlane;
};
