// DlgPhyAdj.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Interlayer.h"
#include "DlgPhyAdj.h"
#include "3DLib/GridObject.h"


// CDlgPhyAdj �Ի���

IMPLEMENT_DYNAMIC(CDlgPhyAdj, CDialog)

CDlgPhyAdj::CDlgPhyAdj(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPhyAdj::IDD, pParent)
	, m_newphyname(_T(""))
{
	m_index = 0;
}

CDlgPhyAdj::~CDlgPhyAdj()
{
}

void CDlgPhyAdj::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_ctr_phyname);
	DDX_Text(pDX, IDC_EDIT1, m_newphyname);
}


BEGIN_MESSAGE_MAP(CDlgPhyAdj, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CDlgPhyAdj::OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


// CDlgPhyAdj ��Ϣ�������

BOOL CDlgPhyAdj::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	switch(m_pObj->GetGLObjType())
	{
	case GLINTERLAYERCELL:
		{
			InterLayerGridObject * pGrid = dynamic_cast<InterLayerGridObject*>(m_pObj);
			int nSize = pGrid->m_vecPhyPara.GetSize();
			for(int i=0; i<nSize; i++)
			{
				m_ctr_phyname.InsertString( i,  pGrid->m_vecPhyPara[i].m_strName );
			}
		}
		break;
	case GLSURFACE:
		{
			CGridObject * pGrid = dynamic_cast<CGridObject*>(m_pObj);

			int nSize = pGrid->m_vecPhyPara.GetSize();
			for(int i=0; i<nSize; i++)
			{
				m_ctr_phyname.InsertString( i,  pGrid->m_vecPhyPara[i].m_strName );
			}
		}
		break;
	default:
		break;
	}
	m_ctr_phyname.SetCurSel(0);
	m_newphyname = "�ữ����";
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CDlgPhyAdj::SetGLObj( CGLObject* pObj )
{
	m_pObj = pObj;
}

void CDlgPhyAdj::OnCbnSelchangeCombo1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_index = m_ctr_phyname.GetCurSel();
}
