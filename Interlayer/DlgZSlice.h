#pragma once


// CDlgZSlice �Ի���

class CDlgZSlice : public CDialog
{
	DECLARE_DYNAMIC(CDlgZSlice)

public:
	CDlgZSlice(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgZSlice();

// �Ի�������
	enum { IDD = IDD_DLG_SLICE_Z };

	double m_dMinZ, m_dMaxZ;
	CString m_strTitle;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	double m_dZ;
	BOOL m_bSpecified;
	double m_dHeight;
	virtual BOOL OnInitDialog();
	BOOL m_bShow;
	BOOL m_bShowSlicePlane;
};
