// DlgXYSlice.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Interlayer.h"
#include "DlgXYSlice.h"


// CDlgXYSlice �Ի���

IMPLEMENT_DYNAMIC(CDlgXYSlice, CDialog)

CDlgXYSlice::CDlgXYSlice(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgXYSlice::IDD, pParent)
	, m_dX1(1.0)
	, m_dY1(0)
	, m_dX2(1.0)
	, m_dY2(0)
	, m_bSpecified(FALSE)
	, m_dHeight(1.0)
	, m_bShow(TRUE)
	, m_strTitle(_T(""))
	, m_dMinX(0)
	, m_dMaxX(0)
	, m_dMinY(0)
	, m_dMaxY(0)
	, m_bShowSlicePlane(TRUE)
{
	
}

CDlgXYSlice::~CDlgXYSlice()
{
}

void CDlgXYSlice::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_X1, m_dX1);
	DDX_Text(pDX, IDC_EDIT_Y1, m_dY1);
	DDX_Text(pDX, IDC_EDIT_X2, m_dX2);
	DDX_Text(pDX, IDC_EDIT_Y2, m_dY2);
	DDX_Check(pDX, IDC_CHECK_HEIGHT, m_bSpecified);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_dHeight);
	DDX_Check(pDX, IDC_CHECK_SHOW, m_bShow);
	DDV_MinMaxDouble(pDX, m_dHeight, 0.001, 1000);
	DDX_Text(pDX, IDC_EDIT_MINX, m_dMinX);
	DDX_Text(pDX, IDC_EDIT_MAXX, m_dMaxX);
	DDX_Text(pDX, IDC_EDIT_MINY, m_dMinY);
	DDX_Text(pDX, IDC_EDIT_MAXY, m_dMaxY);
	DDX_Check(pDX, IDC_CHECK_PLANE, m_bShowSlicePlane);
}


BEGIN_MESSAGE_MAP(CDlgXYSlice, CDialog)
END_MESSAGE_MAP()


// CDlgXYSlice ��Ϣ�������

BOOL CDlgXYSlice::OnInitDialog()
{
	CDialog::OnInitDialog();

	if( !m_strTitle.IsEmpty() )
		SetWindowText(m_strTitle);

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
