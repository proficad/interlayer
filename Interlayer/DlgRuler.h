#pragma once


// CDlgRuler 对话框

class CDlgRuler : public CDialog
{
	DECLARE_DYNAMIC(CDlgRuler)

public:
	CDlgRuler(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgRuler();

// 对话框数据
	enum { IDD = IDD_DLG_TRIANGULAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bRulerIsVisable;
	double m_dAngle;
};
