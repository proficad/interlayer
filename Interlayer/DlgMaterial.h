#pragma once
#include "3DLib/GLMaterials.h"
#include "3DLib/Material.h"
#include "3DLib/GLTypeEnums.h"
#include "3DLib/GLObject.h"
#include "Resource.h"
// CDlgMaterial 对话框

class CDlgMaterial : public CDialog
{
	DECLARE_DYNAMIC(CDlgMaterial)

public:
	CDlgMaterial(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgMaterial();

	GLMaterial m_eMatl;

	CMaterial m_Matl;

	CGLObject *m_pObj;
	CDocument *m_pDoc;

// 对话框数据
	enum { IDD = IDD_DLG_MATERIAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	afx_msg BOOL OnMaterial(UINT nID);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnClr();
	afx_msg void OnBnClickedOk();
	BOOL m_bPreview;
};
