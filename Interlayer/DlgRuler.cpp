// DlgRuler.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Interlayer.h"
#include "DlgRuler.h"


// CDlgRuler �Ի���

IMPLEMENT_DYNAMIC(CDlgRuler, CDialog)

CDlgRuler::CDlgRuler(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRuler::IDD, pParent)
	, m_bRulerIsVisable(FALSE)
	, m_dAngle(0)
{

}

CDlgRuler::~CDlgRuler()
{
}

void CDlgRuler::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_RULER, m_bRulerIsVisable);
	DDX_Text(pDX, IDC_EDIT_ANGLE, m_dAngle);
	DDV_MinMaxDouble(pDX, m_dAngle, 0, 180);
}


BEGIN_MESSAGE_MAP(CDlgRuler, CDialog)
END_MESSAGE_MAP()


// CDlgRuler ��Ϣ�������
