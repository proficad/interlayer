// DlgMaterial.cpp : 实现文件
//

#include "stdafx.h"
#include "Interlayer.h"
#include "DlgMaterial.h"
#include "3DLib/GLDisplayContext.h"

// CDlgMaterial 对话框

IMPLEMENT_DYNAMIC(CDlgMaterial, CDialog)

CDlgMaterial::CDlgMaterial(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMaterial::IDD, pParent)
	, m_bPreview(FALSE)
{

}

CDlgMaterial::~CDlgMaterial()
{
}

void CDlgMaterial::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_PREVIEW, m_bPreview);
}


BEGIN_MESSAGE_MAP(CDlgMaterial, CDialog)
	ON_COMMAND_EX_RANGE(IDC_ALUM,IDC_BPLASTIC, OnMaterial)
	ON_BN_CLICKED(IDC_BTN_CLR, &CDlgMaterial::OnBnClickedBtnClr)
	ON_BN_CLICKED(IDOK, &CDlgMaterial::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgMaterial 消息处理程序

BOOL CDlgMaterial::OnMaterial(UINT nID)
{
	UpdateData(TRUE);

	switch(nID)
	{
	case IDC_ALUM :
		m_eMatl = ALUMINIUM;
		break;
	case IDC_BRASS :
		m_eMatl = BRASS;
		break;
	case IDC_PBRONZE :
		m_eMatl = P_BRONZE;
		break;
	case IDC_CHROME :
		m_eMatl = CHROME;
		break;
	case IDC_PCOPPER :
		m_eMatl = P_COPPER;
		break;
	case IDC_GOLD :
		m_eMatl = GOLD;
		break;
	case IDC_PEWTER :
		m_eMatl = PEWTER;
		break;
	case IDC_SILVER :
		m_eMatl = SILVER;
		break;
	case IDC_STEEL :
		m_eMatl = STEEL;
		break;
	case IDC_EMERALD :
		m_eMatl = EMERALD;
		break;
	case IDC_OBSIDIAN :
		m_eMatl = OBSIDIAN;
		break;
	case IDC_PEARL :
		m_eMatl = PEARL;
		break;
	case IDC_TURQUOISE :
		m_eMatl = TURQUOISE;
		break;
	case IDC_BPLASTIC :
		m_eMatl = PLASTIC;
		break;
	case IDC_BRONZE :
		m_eMatl = BRONZE;
		break;
	case IDC_COPPER :
		m_eMatl = COPPER;
		break;
	case IDC_PGOLD :
		m_eMatl = P_GOLD;
		break;
	case IDC_PSILVER :
		m_eMatl = P_SILVER;
		break;
	case IDC_JADE :
		m_eMatl = JADE;
		break;
	case IDC_RUBY :
		m_eMatl = RUBY;
		break;
	case IDC_BRUBBER :
		m_eMatl = RUBBER;
		break;
	default:
		m_eMatl = JADE;
		break;
	}

	if( m_bPreview)
	{
		m_pObj->SetMaterial(m_eMatl);
		m_pObj->GetContext()->SetModifiedFlag();
		m_pDoc->UpdateAllViews(NULL);
	}

	switch (m_eMatl)
	{
		//Default Setting
	case DEFAULT:
		m_Matl.SetMaterial(DefaultM);
		break;
		//Non Metals
	case EMERALD:
		m_Matl.SetMaterial(Emerald);
		break;
	case JADE:
		m_Matl.SetMaterial(Jade);
		break;
	case OBSIDIAN:
		m_Matl.SetMaterial(Obsidian);
		break;
	case PEARL:
		m_Matl.SetMaterial(Pearl);
		break;
	case PEWTER:
		m_Matl.SetMaterial(Pewter);
		break;
	case RUBY:
		m_Matl.SetMaterial(Ruby);
		break;
	case TURQUOISE:
		m_Matl.SetMaterial(Turquoise);
		break;
	case PLASTIC:
		m_Matl.SetMaterial(Plastic);
		break;
	case RUBBER:
		m_Matl.SetMaterial(Rubber);
		break;
		//Metals
	case ALUMINIUM:
		m_Matl.SetMaterial(Aluminium);
		break;
	case BRASS:
		m_Matl.SetMaterial(Brass);
		break;
	case BRONZE:
		m_Matl.SetMaterial(Bronze);
		break;
	case P_BRONZE:
		m_Matl.SetMaterial(Polished_Bronze);
		break;
	case CHROME:
		m_Matl.SetMaterial(Chrome);
		break;
	case COPPER:
		m_Matl.SetMaterial(Copper);
		break;
	case P_COPPER:
		m_Matl.SetMaterial(Polished_Copper);
		break;
	case GOLD:
		m_Matl.SetMaterial(Gold);
		break;
	case P_GOLD:
		m_Matl.SetMaterial(Polished_Gold);
		break;
	case SILVER:
		m_Matl.SetMaterial(Silver);
		break;
	case P_SILVER:
		m_Matl.SetMaterial(Polished_Silver);
		break;
	case STEEL:
		m_Matl.SetMaterial(Steel);
		break;
	case NONE:
		m_Matl.SetMaterial(None);
		break;
	default:
		m_Matl.SetMaterial(DefaultM);
		break;
	}
	
	return TRUE;
}

void CDlgMaterial::OnBnClickedBtnClr()
{
	// TODO: 在此添加控件通知处理程序代码

	CMFCColorDialog dlg;
	if( dlg.DoModal() == IDOK)
	{
		COLORREF clr = dlg.GetColor();

		m_Matl.SetAmbient((float)GetRValue(clr)/255.0,(float)GetGValue(clr)/255.0,(float)GetBValue(clr)/255.0,1.0);

		if( m_bPreview )
		{
			m_pObj->SetMaterial(m_Matl);
			m_pObj->GetContext()->SetModifiedFlag();
			m_pDoc->UpdateAllViews(NULL);
		}
	}
}

void CDlgMaterial::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	
	OnOK();
}
