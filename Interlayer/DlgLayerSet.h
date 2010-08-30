#pragma once

#include "Resource.h"
// CDlgLayerSet �Ի���

class CDlgLayerSet : public CDialog
{
	DECLARE_DYNAMIC(CDlgLayerSet)

public:
	CDlgLayerSet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgLayerSet();

// �Ի�������
	enum { IDD = IDD_DLG_LAYER_SET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_iType;
	CString m_strLayerName;
	BOOL m_bShow;
	BOOL m_bLocked;
	BOOL m_bClipping;
};
