#pragma once
#include ".\vertical_model\graphwnd.h"

// CDlgWithGraph �Ի���

class CDlgWithGraph : public CResizableDialog
{
	DECLARE_DYNAMIC(CDlgWithGraph)

public:
	CDlgWithGraph(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgWithGraph();
public:
	CGraphWnd graph_wnd;
	long graph_count;
	vector<SSinglePoint> m_points;
	double m_dAngle;
// �Ի�������
	enum { IDD = IDD_GRAPH_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedNewGraphDlg();
	afx_msg void OnBnClickedOk();
};
