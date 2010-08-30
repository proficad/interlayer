#pragma once

#include "Resource.h"
// CDlgLayerSet 对话框

class CDlgLayerSet : public CDialog
{
	DECLARE_DYNAMIC(CDlgLayerSet)

public:
	CDlgLayerSet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgLayerSet();

// 对话框数据
	enum { IDD = IDD_DLG_LAYER_SET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_iType;
	CString m_strLayerName;
	BOOL m_bShow;
	BOOL m_bLocked;
	BOOL m_bClipping;
};
