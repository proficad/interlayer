// DlgPhyParaCalc.cpp : 实现文件
//

#include "stdafx.h"
#include "Interlayer.h"
#include "DlgPhyParaCalc.h"
#include "3DLib//GridObject.h"

// CDlgPhyParaCalc 对话框

IMPLEMENT_DYNAMIC(CDlgPhyParaCalc, CDialog)

CDlgPhyParaCalc::CDlgPhyParaCalc(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPhyParaCalc::IDD, pParent)
	, m_phy1(0)
	, m_phy2(0)
	,m_singleOperator(false)
	, m_new_phyname(_T(""))
	, m_op0(_T(""))
	, m_op1(_T(""))
	, m_op2(_T(""))
	, m_phyname1(_T(""))
	, m_phyname2(_T(""))
{
	m_index_op0 = m_index_op1 = m_index_op2 = m_index_phyname1 = m_index_phyname2 = 0;
}

CDlgPhyParaCalc::~CDlgPhyParaCalc()
{
}

void CDlgPhyParaCalc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_PHY1, m_combo_phy1);
	DDX_Control(pDX, IDC_COMBO_PHY2, m_combo_phy2);
	DDX_Control(pDX, IDC_COMBO_OPERATOR, m_combo_operator);
	DDX_Text(pDX, IDC_EDIT_PARA1, m_phy1);
	DDX_Text(pDX, IDC_EDIT_PHY2, m_phy2);
	DDX_Text(pDX, IDC_EDIT_PHY_NEWNAME, m_new_phyname);
	DDX_Control(pDX, IDC_COMBO_OPERATOR1, m_combo_operator1);
	DDX_Control(pDX, IDC_COMBO_OPERATOR2, m_combo_operator2);
	DDX_Control(pDX, IDC_EDIT_PARA1, m_ctr_para1);
	DDX_Control(pDX, IDC_EDIT_PHY2, m_ctr_para2);
	DDX_Control(pDX, IDC_EDIT_PHY_NEWNAME, m_ctr_phynewname);
	DDX_CBString(pDX, IDC_COMBO_OPERATOR, m_op0);
	DDX_CBString(pDX, IDC_COMBO_OPERATOR1, m_op1);
	DDX_CBString(pDX, IDC_COMBO_OPERATOR2, m_op2);
	DDX_CBString(pDX, IDC_COMBO_PHY1, m_phyname1);
	DDX_CBString(pDX, IDC_COMBO_PHY2, m_phyname2);
}


BEGIN_MESSAGE_MAP(CDlgPhyParaCalc, CDialog)
	//ON_EN_CHANGE(IDC_EDIT2, &CDlgPhyParaCalc::OnEnChangeEdit2)
	//ON_EN_CHANGE(IDC_EDIT_PARA1, &CDlgPhyParaCalc::OnEnChangeEditPara1)
	ON_BN_CLICKED(IDC_RADIO3, &CDlgPhyParaCalc::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO4, &CDlgPhyParaCalc::OnBnClickedRadio4)
	ON_CBN_SELCHANGE(IDC_COMBO_PHY1, &CDlgPhyParaCalc::OnCbnSelchangeComboPhy1)
	ON_CBN_SELCHANGE(IDC_COMBO_OPERATOR1, &CDlgPhyParaCalc::OnCbnSelchangeComboOperator1)
	ON_CBN_SELCHANGE(IDC_COMBO_OPERATOR, &CDlgPhyParaCalc::OnCbnSelchangeComboOperator)
	ON_CBN_SELCHANGE(IDC_COMBO_PHY2, &CDlgPhyParaCalc::OnCbnSelchangeComboPhy2)
	ON_CBN_SELCHANGE(IDC_COMBO_OPERATOR2, &CDlgPhyParaCalc::OnCbnSelchangeComboOperator2)
END_MESSAGE_MAP()


// CDlgPhyParaCalc 消息处理程序

BOOL CDlgPhyParaCalc::OnInitDialog()
{
	CDialog::OnInitDialog();

	CButton *pButton = (CButton *)GetDlgItem(IDC_RADIO4);
	pButton->SetCheck(2);
	// TODO:  在此添加额外的初始化
	switch(m_pGLObj->GetGLObjType())
	{
	case GLINTERLAYERCELL:
		{
			InterLayerGridObject * pGrid = dynamic_cast<InterLayerGridObject*>(m_pGLObj);
			int nSize = pGrid->m_vecPhyPara.GetSize();
			for(int i=0; i<nSize; i++)
			{
				m_combo_phy1.InsertString( i,  pGrid->m_vecPhyPara[i].m_strName );
				m_combo_phy2.InsertString( i,  pGrid->m_vecPhyPara[i].m_strName );
			}
		}
		break;
	case GLSURFACE:
		{
			CGridObject * pGrid = dynamic_cast<CGridObject*>(m_pGLObj);

			int nSize = pGrid->m_vecPhyPara.GetSize();
			for(int i=0; i<nSize; i++)
			{
				m_combo_phy1.InsertString( i,  pGrid->m_vecPhyPara[i].m_strName );
				m_combo_phy2.InsertString( i,  pGrid->m_vecPhyPara[i].m_strName );
			}
		}
		break;
	default:
		break;
	}

	m_combo_operator.InsertString(0, _T("+") );
	m_combo_operator.InsertString(1, _T("-") );
	m_combo_operator.InsertString(2, _T("*") );
	m_combo_operator1.InsertString(0, _T("+") );
	m_combo_operator1.InsertString(1, _T("-") );
	m_combo_operator1.InsertString(2, _T("*") );
	m_combo_operator2.InsertString(0, _T("+") );
	m_combo_operator2.InsertString(1, _T("-") );
	m_combo_operator2.InsertString(2, _T("*") );

	m_combo_phy1.SetCurSel(0);
	m_combo_phy2.SetCurSel(0);
	m_combo_operator.SetCurSel(0);
	m_combo_operator1.SetCurSel(0);
	m_combo_operator2.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgPhyParaCalc::SetGLObj( CGLObject* pObj )
{
	m_pGLObj = pObj;
}

void CDlgPhyParaCalc::OnBnClickedRadio3()
{
	// TODO: 在此添加控件通知处理程序代码
	m_singleOperator = true;
	m_combo_operator2.EnableWindow(FALSE);
	m_ctr_para2.EnableWindow(FALSE);
	m_combo_phy2.EnableWindow(FALSE);
	m_combo_operator.EnableWindow(FALSE);
}

void CDlgPhyParaCalc::OnBnClickedRadio4()
{
	// TODO: 在此添加控件通知处理程序代码
	m_singleOperator = false;
	m_combo_operator2.EnableWindow(TRUE);
	m_ctr_para2.EnableWindow(TRUE);
	m_combo_phy2.EnableWindow(TRUE);
	m_combo_operator.EnableWindow(TRUE);
}

void CDlgPhyParaCalc::OnCbnSelchangeComboPhy1()
{
	// TODO: 在此添加控件通知处理程序代码
	m_index_phyname1 = m_combo_phy1.GetCurSel();
}

void CDlgPhyParaCalc::OnCbnSelchangeComboOperator1()
{
	// TODO: 在此添加控件通知处理程序代码
	m_index_op1 = m_combo_operator1.GetCurSel();
}

void CDlgPhyParaCalc::OnCbnSelchangeComboOperator()
{
	// TODO: 在此添加控件通知处理程序代码
	m_index_op0 = m_combo_operator.GetCurSel();
}

void CDlgPhyParaCalc::OnCbnSelchangeComboPhy2()
{
	// TODO: 在此添加控件通知处理程序代码
	m_index_phyname2 = m_combo_phy2.GetCurSel();
}

void CDlgPhyParaCalc::OnCbnSelchangeComboOperator2()
{
	// TODO: 在此添加控件通知处理程序代码
	m_index_op2 = m_combo_operator2.GetCurSel();
}
