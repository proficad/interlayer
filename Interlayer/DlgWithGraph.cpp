// DlgWithGraph.cpp : 实现文件
//

#include "stdafx.h"
#include "Interlayer.h"
#include "DlgWithGraph.h"
#include "GeoMap/GeoMapView.h"

#define DEMO_MAX_GRAPHS 16

extern COLORREF DefaultGraphColors[DEMO_MAX_GRAPHS];

// CDlgWithGraph 对话框

IMPLEMENT_DYNAMIC(CDlgWithGraph, CResizableDialog)

CDlgWithGraph::CDlgWithGraph(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CDlgWithGraph::IDD, pParent)
{
	graph_count = 0;
}

CDlgWithGraph::~CDlgWithGraph()
{
}

void CDlgWithGraph::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgWithGraph, CResizableDialog)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_NEW_GRAPH_DLG, &CDlgWithGraph::OnBnClickedNewGraphDlg)
	ON_BN_CLICKED(IDOK, &CDlgWithGraph::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgWithGraph 消息处理程序

BOOL CDlgWithGraph::OnInitDialog()
{
	CResizableDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CRect rect;
	GetClientRect(rect);
	rect.right -= 100;
	graph_wnd.Create(_T("Graph Window"), rect, this, 11000, WS_CHILD | WS_VISIBLE, TRUE);

	TCHAR szCurPath[MAX_PATH];  
	memset(szCurPath,   0,   MAX_PATH);  
	GetModuleFileName(NULL,   szCurPath,   sizeof(szCurPath)/sizeof(TCHAR));   
	CString strMoudlePath = szCurPath;
	strMoudlePath = strMoudlePath.Left(strMoudlePath.ReverseFind('\\')+1);
	CString strFileName = strMoudlePath;
	strFileName += _T("Template\\models.dat");
	if(PathFileExists(strFileName))
	{
		CFile file(strFileName, CFile::modeRead|CFile::typeBinary);
		CArchive ar(&file, CArchive::load);
		graph_wnd.Serialize(ar);
		ar.Close();
		file.Close();
	}
	else
	{
		graph_wnd.SetAxisProps(_T("X-Axis"), _T(""), 4, GRAPH_X_AXIS, TRUE);
		graph_wnd.SetAxisProps(_T("Y-Axis"), _T(""), 4, GRAPH_Y_AXIS, TRUE);
		graph_wnd.SetGraphWorldCoords(0, 1, 0, 1, TRUE);
	}

	AddAnchor(IDC_NEW_GRAPH_DLG,TOP_RIGHT);
	AddAnchor(IDOK,TOP_RIGHT);
	AddAnchor(IDCANCEL,TOP_RIGHT);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgWithGraph::OnSize(UINT nType, int cx, int cy)
{
	CResizableDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码

	if( graph_wnd.m_hWnd != NULL )
	{
		CRect rect;
		GetClientRect(rect);
		rect.right -= 100;
		graph_wnd.MoveWindow(rect,TRUE);
	}
}

void CDlgWithGraph::OnBnClickedNewGraphDlg()
{
	// TODO: 在此添加控件通知处理程序代码

	graph_count = graph_wnd.GetGraphCount();
	CString title;
	title.Format(_T("New model - %d"), graph_count);
	graph_wnd.AddGraph(DefaultGraphColors[graph_count%16], title);
}

void CDlgWithGraph::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码

	TCHAR szCurPath[MAX_PATH];  
	memset(szCurPath,   0,   MAX_PATH);  
	GetModuleFileName(NULL,   szCurPath,   sizeof(szCurPath)/sizeof(TCHAR));   
	CString strMoudlePath = szCurPath;
	strMoudlePath = strMoudlePath.Left(strMoudlePath.ReverseFind('\\')+1);
	CString strFileName = strMoudlePath;
	strFileName += _T("Template\\models.dat");
	
	CFile file(strFileName, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
	CArchive ar(&file, CArchive::store);
	graph_wnd.Serialize(ar);
	ar.Close();
	file.Close();

	CGraphProps* pProps = graph_wnd.GetGraph(graph_wnd.GetGraphPanel()->m_lCurrentGraphId);
	double dMaxX = -DBL_MAX, dMinX = DBL_MAX;
	double dMaxY = -DBL_MAX, dMinY = DBL_MAX;
	if(pProps != NULL )
	{
		int n = pProps->GetSize();
		for (int i=0; i<n; i++)
		{
			SSinglePoint pt;
			pProps->GetPoint(i,&pt);
			m_points.push_back(pt);
			if( dMaxX < pt.x )
				dMaxX = pt.x;
			if( dMaxY < pt.y )
				dMaxY = pt.y;

			if( dMinX > pt.x )
				dMinX = pt.x;
			if( dMinY > pt.y )
				dMinY = pt.y;
		}

		Angle::AngleCalculator cal;
		// 直线1
		Angle::Line l1(pProps->p1.x, pProps->p1.y, pProps->p1.x+1.0,pProps->p1.y);
		// 直线2
		Angle::Line l2(pProps->p1.x, pProps->p1.y, pProps->p2.x,pProps->p2.y);

		// 角度
		m_dAngle = cal.getDegree(l1,l2)*180.0/PI;

		for (int i=0; i<n; i++)
		{
			m_points[i].x = (m_points[i].x-dMinX)/(dMaxY-dMinY);
			m_points[i].y = (m_points[i].y-dMinY)/(dMaxY-dMinY);
		}
	}

	OnOK();
}
