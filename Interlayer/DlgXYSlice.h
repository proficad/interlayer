#pragma once


// CDlgXYSlice 对话框

class CDlgXYSlice : public CDialog
{
	DECLARE_DYNAMIC(CDlgXYSlice)

public:
	CDlgXYSlice(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgXYSlice();

// 对话框数据
	enum { IDD = IDD_DLG_SLICE_XY };

	CString m_strTitle;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	double m_dX1;
	double m_dY1;
	double m_dX2;
	double m_dY2;
	BOOL m_bSpecified;
	double m_dHeight;
	BOOL m_bShow;
	virtual BOOL OnInitDialog();
	double m_dMinX;
	double m_dMaxX;
	double m_dMinY;
	double m_dMaxY;
	BOOL m_bShowSlicePlane;
};
