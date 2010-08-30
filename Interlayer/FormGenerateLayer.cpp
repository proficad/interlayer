// FormGenerateLayer.cpp : 实现文件
//

#include "stdafx.h"
#include "Interlayer.h"
#include "FormGenerateLayer.h"
#include "DlgWithGraph.h"
#include "./3DLib/FaceTriangles.h"
#include "MainFrm.h"
#include "GenerateLayerDoc.h"
#include "GeoMap/GeoMapView.h"

#define IDC_WND_TOP 100
#define IDC_WND_BOTTOM 101
#define IDC_WND_HORZ 102
#define IDC_WND_VERT 103

// CFormGenerateLayer
extern void StatusProgress(int nIndex, int nRange=-1);
extern void StatusSetProgress(int nIndex, int nPos);
extern bool Intersect(CPoint2D *p1, CPoint2D *p2, CPoint2D *p, bool bLine = true);
extern double GetMaxCurvarture(const std::vector<CPoint2D> &points, bool bFlag = true);
extern void Bresenham(const std::vector<CPoint2D> &points, std::vector<CPoint2D> &addPoints, int num);
extern CPoint2D LineExtension(const CPoint2D &p1,const CPoint2D &p2,double dLength,bool bFlags);

// 点积
double DotMultiply(CPoint2D *pLine1,CPoint2D *pLine2) 
{
	//x3*x4 + y3*y4。
	return ((pLine1[1].x-pLine1[0].x)*(pLine2[1].x-pLine2[0].x) + (pLine1[1].y-pLine1[0].y)*(pLine2[1].y-pLine2[0].y)); 
} 


// 圆和直线关系： 
// 0----相离 1----相切 2----相交
// p圆心，r半径,ax+by+c=0
//
int clpoint(CPoint2D p,double r,double a,double b,double c,CPoint2D &rp1,CPoint2D &rp2) 
{ 
	int res=0; 

	c=c+a*p.x+b*p.y; 
	double tmp;
	if(a==0&&b!=0) 
	{ 
		tmp=-c/b; 
		if(r*r<tmp*tmp) 
			res=0; 
		else if(r*r==tmp*tmp) 
		{ 
			res=1; 
			rp1.y=tmp; 
			rp1.x=0; 
		} 
		else 
		{ 
			res=2; 
			rp1.y=rp2.y=tmp; 
			rp1.x=sqrt(r*r-tmp*tmp); 
			rp2.x=-rp1.x; 
		} 
	} 
	else if(a!=0&&b==0) 
	{ 
		tmp=-c/a; 
		if(r*r<tmp*tmp) 
			res=0; 
		else if(r*r==tmp*tmp) 
		{ 
			res=1; 
			rp1.x=tmp; 
			rp1.y=0; 
		} 
		else 
		{ 
			res=2; 
			rp1.x=rp2.x=tmp; 
			rp1.y=sqrt(r*r-tmp*tmp); 
			rp2.y=-rp1.y; 
		} 
	} 
	else if(a!=0&&b!=0) 
	{ 
		double delta; 
		delta=b*b*c*c-(a*a+b*b)*(c*c-a*a*r*r); 
		if(delta<0) 
			res=0; 
		else if(delta==0) 
		{ 
			res=1; 
			rp1.y=-b*c/(a*a+b*b); 
			rp1.x=(-c-b*rp1.y)/a; 
		} 
		else 
		{ 
			res=2; 
			rp1.y=(-b*c+sqrt(delta))/(a*a+b*b); 
			rp2.y=(-b*c-sqrt(delta))/(a*a+b*b); 
			rp1.x=(-c-b*rp1.y)/a; 
			rp2.x=(-c-b*rp2.y)/a; 
		} 
	} 
	rp1.x+=p.x; 
	rp1.y+=p.y; 
	rp2.x+=p.x; 
	rp2.y+=p.y; 
	return res; 
}

/*
如果(x1,y1),(x2,y2),(x3,y3)不在一条直线上,那么当以下三条同时成立时,
点(x4,y4)在以(x1,y1),(x2,y2),(x3,y3)为顶点的三角形内部:
1).
  令 t1=(y2-y1)*(x4-x1)+(x1-x2)*(y4-y1),
     t2=(y2-y1)*(x3-x1)+(x1-x2)*(y3-y1),
则t1*t2>=0;
2).
  令 t3=(y3-y1)*(x4-x1)+(x1-x3)*(y4-y1),
     t4=(y3-y1)*(x2-x1)+(x1-x3)*(y2-y1),
  则t3*t4>=0;
3).
  令t5=(y3-y2)*(x4-x2)+(x2-x3)*(y4-y2),
    t6=(y3-y2)*(x1-x2)+(x2-x3)*(y1-y2),
  则t5*t6>=0.
*/

bool InTriangle(const CPoint2D &pt, const CPoint2D &pt1, const CPoint2D &pt2, const CPoint2D &pt3)
{
	double t1 = (pt2.y-pt1.y)*(pt.x-pt1.x) + (pt1.x-pt2.x)*(pt.y-pt1.y);
	double t2 = (pt2.y-pt1.y)*(pt3.x-pt1.x) + (pt1.x-pt2.x)*(pt3.y-pt1.y);
	
	double t3 = (pt3.y-pt1.y)*(pt.x-pt1.x) + (pt1.x-pt3.x)*(pt.y-pt1.y);
	double t4 = (pt3.y-pt1.y)*(pt2.x-pt1.x) + (pt1.x-pt3.x)*(pt2.y-pt1.y);

	double t5 = (pt3.y-pt2.y)*(pt.x-pt2.x) + (pt2.x-pt3.x)*(pt.y-pt2.y);
	double t6 = (pt3.y-pt2.y)*(pt1.x-pt2.x) + (pt2.x-pt3.x)*(pt1.y-pt2.y);

	if( t1*t2>=0 && t3*t4>=0 && t5*t6 >=0 )
		return true;
	else
		return false;
}

IMPLEMENT_DYNCREATE(CFormGenerateLayer, CResizableFormView)

CFormGenerateLayer::CFormGenerateLayer()
	: CResizableFormView(CFormGenerateLayer::IDD)
	, m_dTop(0)
	, m_dBottom(0)
	, m_dDepthScale(0.5)
	, m_strAngle(_T(""))
{

}

CFormGenerateLayer::~CFormGenerateLayer()
{
}

void CFormGenerateLayer::DoDataExchange(CDataExchange* pDX)
{
	CResizableFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TOP, m_dTop);
	DDV_MinMaxDouble(pDX, m_dTop, -2000, 20000);
	DDX_Text(pDX, IDC_EDIT_BOTTOM, m_dBottom);
	DDV_MinMaxDouble(pDX, m_dBottom, -2000, 20000);
	DDX_Text(pDX, IDC_EDIT_MIDDLE, m_dDepthScale);
	DDV_MinMaxDouble(pDX, m_dDepthScale, -2000, 20000);
	DDX_Text(pDX, IDC_EDIT_ANGLE, m_strAngle);
}

BEGIN_MESSAGE_MAP(CFormGenerateLayer, CResizableFormView)	
	ON_WM_SIZE()
	ON_NOTIFY(GVN_DATACHANGED, IDC_WND_TOP, OnDataChanged)
	ON_NOTIFY(GVN_DATACHANGED, IDC_WND_BOTTOM, OnDataChanged)
	ON_NOTIFY(GVN_DATACHANGED, IDC_WND_VERT, OnDataChanged)
	ON_NOTIFY(GVN_DATACHANGED, IDC_WND_HORZ, OnDataChanged)

	ON_NOTIFY(GVN_NORMALCHANGING, IDC_WND_HORZ, OnNormalChanged)

	ON_BN_CLICKED(IDC_BTN_IMPORT_TOP, OnBnClickedBtnImportTop)
	ON_BN_CLICKED(IDC_BTN_DEL_TOP, OnBnClickedBtnDelTop)
	ON_BN_CLICKED(IDC_BTN_IMPORT_BOTTOM, OnBnClickedBtnImportBottom)
	ON_BN_CLICKED(IDC_BTN_DEL_BOTTOM, OnBnClickedBtnDelBottom)
	ON_BN_CLICKED(IDC_BTN_IMPORT_HORZ, OnBnClickedBtnImportHorz)
	ON_BN_CLICKED(IDC_BTN_DEL_HORZ, OnBnClickedBtnDelHorz)
	ON_BN_CLICKED(IDC_BTN_IMPORT_VERT, OnBnClickedBtnImportVert)
	ON_BN_CLICKED(IDC_BTN_DEL_VERT, OnBnClickedBtnDelVert)
	ON_BN_CLICKED(IDC_BTN_IMPORT_NORMAL, OnBnClickedBtnImportNormal)
	ON_BN_CLICKED(IDC_BTN_GENERATE_LAYER, OnBnClickedBtnGenerateLayer)
	
	ON_EN_KILLFOCUS(IDC_EDIT_TOP, OnEnKillfocus)
	ON_EN_KILLFOCUS(IDC_EDIT_BOTTOM, OnEnKillfocus)
	ON_EN_KILLFOCUS(IDC_EDIT_MIDDLE, OnEnKillfocusEditMiddle)
END_MESSAGE_MAP()


// CFormGenerateLayer 诊断

#ifdef _DEBUG
void CFormGenerateLayer::AssertValid() const
{
	CResizableFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFormGenerateLayer::Dump(CDumpContext& dc) const
{
	CResizableFormView::Dump(dc);
}
#endif
#endif //_DEBUG

CGenerateLayerDoc * CFormGenerateLayer::GetDocument() const
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGenerateLayerDoc)));
	return (CGenerateLayerDoc*)m_pDocument;
}
// CFormGenerateLayer 消息处理程序

void CFormGenerateLayer::OnDataChanged(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	tagWGNMHDR* pItem = (tagWGNMHDR*) pNotifyStruct;
	CGenerateLayerDoc *pDoc = GetDocument();

	if( pItem->hdr.hwndFrom == m_wndTop.m_hWnd )
	{
		m_dTop = m_wndTop.GetMinZ();
		UpdateData(FALSE);
		OnEnKillfocus();
		pDoc->m_layerModel.m_dTop = m_dTop;
		pDoc->SetModifiedFlag();
	}
	if( pItem->hdr.hwndFrom == m_wndBottom.m_hWnd )
	{
		m_dBottom = m_wndBottom.GetMaxZ();
		UpdateData(FALSE);
		OnEnKillfocus();		
		pDoc->m_layerModel.m_dBottom = m_dBottom;
		pDoc->SetModifiedFlag();
	}

	if( pItem->hdr.hwndFrom == m_wndVert.m_hWnd )
	{	pDoc->m_layerModel.m_dDepthScale = pItem->m_dDepthScale;

		double dHeight = m_dBottom - m_dTop;
		if( dHeight <= 0 )
			m_dDepthScale =  pItem->m_dDepthScale;
		else
		{
			m_dDepthScale = m_dTop+(m_dBottom-m_dTop)*(1-pItem->m_dDepthScale);
		}
		UpdateData(FALSE);
		pDoc->SetModifiedFlag();
	}
	*pResult = 0;
}

void CFormGenerateLayer::OnNormalChanged(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	tagWGNMHDR* pItem = (tagWGNMHDR*) pNotifyStruct;
	m_wndVert.NormalChanged(pItem->m_iNormal);
	CGenerateLayerDoc *pDoc = GetDocument();
	if( pItem->m_iNormal > -1 && pDoc->m_layerModel.m_vertModel[pItem->m_iNormal].size()>0)
	{
		m_strAngle.Format(_T("%.2lf°"), pDoc->m_layerModel.m_dAngles[m_wndVert.m_iCurVertModel]);
		UpdateData(FALSE);

		CRect rect;
		m_wndVert.GetClientRect(rect);
		int x = rect.Width();
		int y = rect.Height();

		double xMin = pDoc->GetMinX(m_wndVert.m_iCurVertModel);
		double yMin = pDoc->GetMinY(m_wndVert.m_iCurVertModel);
		double xMax = pDoc->GetMaxX(m_wndVert.m_iCurVertModel);
		double yMax = pDoc->GetMaxY(m_wndVert.m_iCurVertModel);
		
		double xStep = (xMax-xMin)/(x-40);
		double yStep = (yMax-yMin)/(y-40);

		double step = (xStep>yStep?xStep:yStep);
		m_wndVert.SetMinX(xMin );
		m_wndVert.SetMaxX(xMax);
		m_wndVert.SetMinY(yMin );
		m_wndVert.SetMaxY(yMax);

		m_wndVert.SetWorldCoords(
			(xMin + xMax)/2 - step * x/2,
			(xMin + xMax)/2 + step * x/2,
			(yMin + yMax)/2 - step * y/2,
			(yMin + yMax)/2 + step * y/2,TRUE);
	}
	else
	{
		m_wndVert.SetMinX(0 );
		m_wndVert.SetMaxX(1);
		m_wndVert.SetMinY(0 );
		m_wndVert.SetMaxY(1);

		m_wndVert.SetWorldCoords(
			-0.1,
			1.1,
			-0.1,
			1.1,
			TRUE);
	}


	pDoc->SetModifiedFlag();
	*pResult = 0;
}

void CFormGenerateLayer::OnInitialUpdate()
{
	CResizableFormView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类

	CRect rect(0, 0, 1, 1);
	m_wndTop.Create(rect,this,IDC_WND_TOP);
	m_wndBottom.Create(rect,this,IDC_WND_BOTTOM);
	m_wndHorz.Create(rect,this,IDC_WND_HORZ);
	m_wndVert.Create(rect,this,IDC_WND_VERT);

	m_wndTop.m_eType = CWndGraph::TOP_PLANE;
	m_wndBottom.m_eType = CWndGraph::BOTTOM_PLANE;
	m_wndHorz.m_eType = CWndGraph::HORZ;
	m_wndVert.m_eType = CWndGraph::VERT;

	CGenerateLayerDoc *pDoc = GetDocument();

	m_wndHorz.RefreshCoord();
	m_wndVert.RefreshCoord();
	m_wndTop.RefreshCoord();
	m_wndBottom.RefreshCoord();

	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
	AddAnchor(IDC_TOP,TOP_LEFT,MIDDLE_CENTER);
	AddAnchor(IDC_BOTTOM,TOP_CENTER,MIDDLE_RIGHT);
	AddAnchor(IDC_HORZ,MIDDLE_LEFT,BOTTOM_CENTER);
	AddAnchor(IDC_VERT,MIDDLE_CENTER,BOTTOM_RIGHT);

	AddAnchor(IDC_STATIC_TOP,TOP_LEFT);
	AddAnchor(IDC_BTN_IMPORT_TOP,TOP_LEFT);
	AddAnchor(IDC_BTN_DEL_TOP,TOP_LEFT);
	AddAnchor(IDC_STATIC_TOP1,MIDDLE_CENTER);
	AddAnchor(IDC_EDIT_TOP,MIDDLE_CENTER);	

	AddAnchor(IDC_STATIC_BOTTOM, TOP_CENTER);
	AddAnchor(IDC_BTN_IMPORT_BOTTOM,TOP_CENTER);
	AddAnchor(IDC_BTN_DEL_BOTTOM,TOP_CENTER);
	AddAnchor(IDC_STATIC_BOTTOM1, MIDDLE_CENTER);
	AddAnchor(IDC_EDIT_BOTTOM, MIDDLE_CENTER);

	AddAnchor(IDC_STATIC_MIDDLE, MIDDLE_LEFT);
	AddAnchor(IDC_BTN_IMPORT_HORZ,MIDDLE_LEFT);
	AddAnchor(IDC_BTN_DEL_HORZ,MIDDLE_LEFT);
	AddAnchor(IDC_BTN_IMPORT_NORMAL,MIDDLE_LEFT);
	AddAnchor(IDC_STATIC_MODEL, BOTTOM_CENTER);
	AddAnchor(IDC_EDIT_MIDDLE, BOTTOM_CENTER);

	AddAnchor(IDC_STATIC_V_MODLE, MIDDLE_CENTER);
	AddAnchor(IDC_BTN_IMPORT_VERT,MIDDLE_CENTER);
	AddAnchor(IDC_BTN_DEL_VERT,MIDDLE_CENTER);
	AddAnchor(IDC_STATIC_VERT, BOTTOM_CENTER);

	AddAnchor(IDC_STATIC_ANGLE,BOTTOM_CENTER);
	AddAnchor(IDC_EDIT_ANGLE, BOTTOM_CENTER);
	
	AddAnchor(IDC_BTN_GENERATE_LAYER, BOTTOM_RIGHT);

	m_dBottom = pDoc->m_layerModel.m_dBottom;
	m_dTop = pDoc->m_layerModel.m_dTop;

	double dHeight = m_dBottom - m_dTop;
	if( dHeight <= 0 )
		m_dDepthScale =  pDoc->m_layerModel.m_dDepthScale;
	else
	{
		m_dDepthScale = m_dTop+(m_dBottom-m_dTop)*(1-pDoc->m_layerModel.m_dDepthScale);
	}
	
	UpdateData(FALSE);
}


void CFormGenerateLayer::OnSize(UINT nType, int cx, int cy)
{
	CResizableFormView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码

	if( m_wndTop.m_hWnd )
	{
		CRect rect;
		GetDlgItem(IDC_TOP)->GetClientRect(rect);
		GetDlgItem(IDC_TOP)->ClientToScreen(rect);
		ScreenToClient(rect);
		m_wndTop.MoveWindow(rect);
	}

	if( m_wndBottom.m_hWnd )
	{
		CRect rect;
		GetDlgItem(IDC_BOTTOM)->GetClientRect(rect);
		GetDlgItem(IDC_BOTTOM)->ClientToScreen(rect);
		ScreenToClient(rect);
		m_wndBottom.MoveWindow(rect);
	}

	if( m_wndHorz.m_hWnd )
	{
		CRect rect;
		GetDlgItem(IDC_HORZ)->GetClientRect(rect);
		GetDlgItem(IDC_HORZ)->ClientToScreen(rect);
		ScreenToClient(rect);
		m_wndHorz.MoveWindow(rect);
	}

	if( m_wndVert.m_hWnd )
	{
		CRect rect;
		GetDlgItem(IDC_VERT)->GetClientRect(rect);
		GetDlgItem(IDC_VERT)->ClientToScreen(rect);
		ScreenToClient(rect);
		m_wndVert.MoveWindow(rect);
				
		int x = rect.Width();
		int y = rect.Height();

		if( m_wndVert.m_iCurVertModel != -1 )
		{
			CGenerateLayerDoc *pDoc = GetDocument();
			
			int x = rect.Width();
			int y = rect.Height();

			double xMin = pDoc->GetMinX(m_wndVert.m_iCurVertModel);
			double yMin = pDoc->GetMinY(m_wndVert.m_iCurVertModel);
			double xMax = pDoc->GetMaxX(m_wndVert.m_iCurVertModel);
			double yMax = pDoc->GetMaxY(m_wndVert.m_iCurVertModel);

			double xStep = (xMax-xMin)/(x-40);
			double yStep = (yMax-yMin)/(y-40);

			double step = (xStep>yStep?xStep:yStep);
			m_wndVert.SetMinX(xMin );
			m_wndVert.SetMaxX(xMax);
			m_wndVert.SetMinY(yMin );
			m_wndVert.SetMaxY(yMax);

			m_wndVert.SetWorldCoords(
				(xMin + xMax)/2 - step * x/2, 
				(xMin + xMax)/2 + step * x/2,
				(yMin + yMax)/2 - step * y/2,
				(yMin + yMax)/2 + step * y/2,TRUE);			
		}
		else
		{
			if( x>y)
				m_wndVert.SetWorldCoords(-0.1-(double)(x-y)/(double)x/1.0,1.1+(double)(x-y)/(double)x/1.0,-0.1,1.1,TRUE);
			else
				m_wndVert.SetWorldCoords(-0.1,1.1,-0.1-(double)(y-x)/(double)y/1.0,1.1+(double)(y-x)/(double)y/1.0,TRUE);
		}

	}
}

// 导入顶面
void CFormGenerateLayer::OnBnClickedBtnImportTop()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlg(TRUE,NULL, NULL,4|2,_T("所有文件 (*.*)|*.*||"));
	dlg.m_ofn.lpstrTitle = _T("导入顶面");
	if( dlg.DoModal() == IDOK)
		m_wndTop.AddPlanes(dlg.GetPathName(), true);
}

void CFormGenerateLayer::OnBnClickedBtnDelTop()
{
	// TODO: 在此添加控件通知处理程序代码
	m_wndTop.Init();
	GetDocument()->SetModifiedFlag();
}

//  导入底面
void CFormGenerateLayer::OnBnClickedBtnImportBottom()
{
	// TODO: 在此添加控件通知处理程序代码

	CFileDialog dlg(TRUE,NULL, NULL,4|2,_T("所有文件 (*.*)|*.*||"));
	dlg.m_ofn.lpstrTitle = _T("导入底面");
	if( dlg.DoModal() == IDOK)
		m_wndBottom.AddPlanes(dlg.GetPathName(), false);
}

void CFormGenerateLayer::OnBnClickedBtnDelBottom()
{
	// TODO: 在此添加控件通知处理程序代码
	m_wndBottom.Init();
	GetDocument()->SetModifiedFlag();
}

// 导入夹层线
void CFormGenerateLayer::OnBnClickedBtnImportHorz()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlg(TRUE,NULL, NULL,4|2,_T("所有文件 (*.*)|*.*||"));
	dlg.m_ofn.lpstrTitle = _T("导入夹层线");
	if( dlg.DoModal() == IDOK)
		m_wndHorz.AddLayers(dlg.GetPathName());
}

void CFormGenerateLayer::OnBnClickedBtnDelHorz()
{
	// TODO: 在此添加控件通知处理程序代码
	m_wndHorz.Init();
	GetDocument()->SetModifiedFlag();
}

// 导入纵向模型
void CFormGenerateLayer::AddVertModel(LPCTSTR lpszVertName)
{
	CGenerateLayerDoc *pDoc = GetDocument();

	if ( m_wndHorz.m_selectVertModel.size() < 1 )
	{
		int nSize = pDoc->m_layerModel.m_vertModel.size();
		if( nSize < 1)
			return;
		for (int i=0; i<nSize; i++)
			m_wndHorz.m_selectVertModel.push_back(i);
	}

	CGraphWnd graph_wnd;
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

		std::vector<SSinglePoint> points;
		CString strVertName = lpszVertName;
		
		double dAngle = 0;
		COLORREF clr = 0;
		double dMaxX = -DBL_MAX, dMinX = DBL_MAX;
		double dMaxY = -DBL_MAX, dMinY = DBL_MAX;

		int nSize = graph_wnd.GetGraphCount();
		for (int i=0; i<nSize; i++)
		{
			CGraphProps* pProps = graph_wnd.GetGraph(i);
			CString strTitle = pProps->GetTitle();
			if( strTitle != strVertName)
				continue;
			
			int n = pProps->GetSize();
			for (int j=0; j<n; j++)
			{
				SSinglePoint pt;
				pProps->GetPoint(j,&pt);
				points.push_back(pt);

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
			dAngle = cal.getDegree(l1,l2)*180.0/PI;	

			clr = pProps->GetGraphColor();

			break;
		}

		int num = m_wndHorz.m_selectVertModel.size();
		for (int j=0; j<num; j++)
			m_wndVert.Init(m_wndHorz.m_selectVertModel[j]);

		m_wndVert.m_iCurVertModel = m_wndHorz.m_selectVertModel[0];

		nSize = points.size();

		double yMax = 0, xMax = 0;
		double yMin = 10000, xMin = 1000;
		for (int i=0; i<nSize; i++)
		{
			CPoint2D pt;
			pt.x = (points[i].x-dMinX)/(dMaxY-dMinY);
			pt.y = (points[i].y-dMinY)/(dMaxY-dMinY);
			pt.m_dValue = -99999.0;

			if( pt.x > xMax )
				xMax = pt.x;
			if( pt.y > yMax )
				yMax = pt.y;

			if( pt.x < xMin )
				xMin = pt.x;
			if( pt.y < yMin )
				yMin = pt.y;

			for (int j=0; j<num; j++)
				m_wndVert.AddPoint(m_wndHorz.m_selectVertModel[j], pt);
		}
		
		for (int j=0; j<num; j++)
		{
			pDoc->m_layerModel.m_dAngles[m_wndHorz.m_selectVertModel[j]] = dAngle;
			pDoc->m_layerModel.m_vertTitle[m_wndHorz.m_selectVertModel[j]] = lpszVertName;
			pDoc->m_layerModel.m_vertColor[m_wndHorz.m_selectVertModel[j]] = clr;
		}

		CRect rect;
		m_wndVert.GetClientRect(rect);

		int x = rect.Width();
		int y = rect.Height();

		double xStep = (xMax-xMin)/(x-40);
		double yStep = (yMax-yMin)/(y-40);

		double step = (xStep>yStep?xStep:yStep);
		m_wndVert.SetMinX(xMin );
		m_wndVert.SetMaxX(xMax);
		m_wndVert.SetMinY(yMin );
		m_wndVert.SetMaxY(yMax);

		m_wndVert.SetWorldCoords(
			(xMin + xMax)/2 - step * x/2, 
			(xMin + xMax)/2 + step * x/2,
			(yMin + yMax)/2 - step * y/2,
			(yMin + yMax)/2 + step * y/2,TRUE);

		GetDocument()->SetModifiedFlag();

		m_strAngle.Format(_T("%.2lf°"), pDoc->m_layerModel.m_dAngles[m_wndVert.m_iCurVertModel]);

		UpdateData(FALSE);
	}
}

// 导入纵向模型
void CFormGenerateLayer::OnBnClickedBtnImportVert()
{
	// TODO: 在此添加控件通知处理程序代码

	CGenerateLayerDoc *pDoc = GetDocument();

	if ( m_wndHorz.m_selectVertModel.size() < 1 )
	{
		int nSize = pDoc->m_layerModel.m_vertModel.size();
		if( nSize < 1)
			return;
		for (int i=0; i<nSize; i++)
			m_wndHorz.m_selectVertModel.push_back(i);
	}	

	CDlgWithGraph dlg;
	if( dlg.DoModal() == IDOK )
	{
		int num = m_wndHorz.m_selectVertModel.size();
		for (int j=0; j<num; j++)
			m_wndVert.Init(j);

		m_wndVert.m_iCurVertModel = m_wndHorz.m_selectVertModel[0];

		int nSize = dlg.m_points.size();

		double yMax = 0, xMax = 0;
		double yMin = 10000, xMin = 1000;
		for (int i=0; i<nSize; i++)
		{
			CPoint2D pt;
			pt.x = dlg.m_points[i].x;
			pt.y = dlg.m_points[i].y;
			pt.m_dValue = -99999.0;
			
			if( pt.x > xMax )
				xMax = pt.x;
			if( pt.y > yMax )
				yMax = pt.y;

			if( pt.x < xMin )
				xMin = pt.x;
			if( pt.y < yMin )
				yMin = pt.y;

			int num = m_wndHorz.m_selectVertModel.size();
			for (int j=0; j<num; j++)
				m_wndVert.AddPoint(m_wndHorz.m_selectVertModel[j], pt);
		}

		for (int j=0; j<num; j++)
			pDoc->m_layerModel.m_dAngles[m_wndHorz.m_selectVertModel[j]] = dlg.m_dAngle;
		
		CRect rect;
		m_wndVert.GetClientRect(rect);

		int x = rect.Width();
		int y = rect.Height();

		double xStep = (xMax-xMin)/(x-40);
		double yStep = (yMax-yMin)/(y-40);

		double step = (xStep>yStep?xStep:yStep);
		m_wndVert.SetMinX(xMin );
		m_wndVert.SetMaxX(xMax);
		m_wndVert.SetMinY(yMin );
		m_wndVert.SetMaxY(yMax);

		m_wndVert.SetWorldCoords(
			(xMin + xMax)/2 - step * x/2, 
			(xMin + xMax)/2 + step * x/2,
			(yMin + yMax)/2 - step * y/2,
			(yMin + yMax)/2 + step * y/2,TRUE);

		GetDocument()->SetModifiedFlag();

		m_strAngle.Format(_T("%.2lf°"), pDoc->m_layerModel.m_dAngles[m_wndVert.m_iCurVertModel]);

		GetDocument()->SetModifiedFlag();
	}
}

void CFormGenerateLayer::OnBnClickedBtnDelVert()
{
	// TODO: 在此添加控件通知处理程序代码

	if ( m_wndHorz.m_selectVertModel.size() < 0 )
		return;
	int num = m_wndHorz.m_selectVertModel.size();
	for (int j=0; j<num; j++)
		m_wndVert.Init(m_wndHorz.m_selectVertModel[j]);
	GetDocument()->SetModifiedFlag();
}

// 导入法线
void CFormGenerateLayer::OnBnClickedBtnImportNormal()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlg(TRUE,NULL, NULL,4|2,_T("所有文件 (*.*)|*.*||"));
	dlg.m_ofn.lpstrTitle = _T("导入法线");
	if( dlg.DoModal() == IDOK)
	{
		m_wndHorz.AddNormals(dlg.GetPathName());
		GetDocument()->SetModifiedFlag();
	}
}

void CFormGenerateLayer::OnEnKillfocus()
{
	// TODO: 在此添加控件通知处理程序代码
 	UpdateData(TRUE);
	CGenerateLayerDoc *pDoc = GetDocument();
	pDoc->m_layerModel.m_dBottom = m_dBottom;
	pDoc->m_layerModel.m_dTop = m_dTop;

 	double dHeight = m_dBottom - m_dTop;
 	if( dHeight <= 0 )
 		return;

	if( m_dDepthScale >= m_dTop &&  m_dDepthScale<=m_dBottom)
	{
		m_wndVert.m_dDepthScale = 1.0-(m_dDepthScale-m_dTop)/(m_dBottom-m_dTop);
	}
	else
		m_wndVert.m_dDepthScale = m_dDepthScale;

	pDoc->m_layerModel.m_dDepthScale = m_wndVert.m_dDepthScale;

	m_wndVert.UpdateGraphWindow(NULL);

	pDoc->SetModifiedFlag();
}

void CFormGenerateLayer::OnEnKillfocusEditMiddle()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	
	double dHeight = m_dBottom - m_dTop;

	if( m_dDepthScale >= m_dTop &&  m_dDepthScale<=m_dBottom)
	{
		m_wndVert.m_dDepthScale = 1.0-(m_dDepthScale-m_dTop)/(m_dBottom-m_dTop);
	}
	else
		m_wndVert.m_dDepthScale = m_dDepthScale;

	m_wndVert.UpdateGraphWindow(NULL);

	CGenerateLayerDoc *pDoc = GetDocument();
	pDoc->m_layerModel.m_dDepthScale = m_wndVert.m_dDepthScale;
	pDoc->SetModifiedFlag();
}

void CFormGenerateLayer::GenerateLayers()
{
	CGenerateLayerDoc *pDoc = GetDocument();

	CString strTmp;
	strTmp.Format(_T("%g"), pDoc->m_layerModel.m_dHorzSpace);
	strTmp = AfxGetApp()->GetProfileString(_T("Settings"),_T("Layers"),strTmp);
	pDoc->m_layerModel.m_dHorzSpace = atof(strTmp);

	strTmp.Format(_T("%g"), pDoc->m_layerModel.m_dNormalSpace);
	strTmp = AfxGetApp()->GetProfileString(_T("Settings"),_T("Normals"),strTmp);
	pDoc->m_layerModel.m_dNormalSpace = atof(strTmp);

	strTmp.Format(_T("%g"), pDoc->m_layerModel.m_dVertSpace);
	strTmp = AfxGetApp()->GetProfileString(_T("Settings"),_T("Verts"),strTmp);
	pDoc->m_layerModel.m_dVertSpace = atof(strTmp);

	//////////////////////////////////////////////////////////////////////////

	if( pDoc->m_layerModel.m_layers.size() < 2 )
	{
		AfxMessageBox(_T("夹层线少于2条!"));
		return;
	}

	if( pDoc->m_layerModel.m_normals[0].size() < 1 )
	{
		AfxMessageBox(_T("主法线不能少于1条!"));
		return;
	}
	//////////////////////////////////////////////////////////////////////////	
	m_layers.clear();
	m_singleLayers.clear();
	m_iNormals.clear();
	m_iHorzSpaces.clear();
	m_vertModel.clear();
	m_normals[0].clear();
	m_normals[1].clear();
	//------------------------------------------------------------------------
	std::vector<std::vector<CPoint2D>> layers;			// 复夹层线--插值后
	std::vector<std::vector<CPoint2D>> singleLayers;	// 单夹层线--插值后
	std::vector<CPoint2D> normals[2];					// 法线	
	//------------------------------------------------------------------------
	// 计算每条夹层线插值后的数据
	int nLayers = pDoc->m_layerModel.m_layers.size();
	for (int i=0; i<nLayers; i++)
	{
		int num = pDoc->m_layerModel.m_layers[i].size();
		CPoint2D *point = new CPoint2D[num];
		for (int j=0; j<num; j++)
			point[j] = pDoc->m_layerModel.m_layers[i][j];

		long nSize = GetSpline3Pointf(point,num);

		if( nSize > 0 )
		{
			CPoint2D *pt = new CPoint2D[nSize];
			GetSpline3Pointf(point,num,FALSE,pt);
			std::vector<CPoint2D> pts;
			for (int j=0; j<nSize; j++)
				pts.push_back(pt[j]);
			delete []pt;

			layers.push_back(pts);
		}
		delete []point;
	}

	// 计算每条单夹层线插值后的数据
	nLayers = pDoc->m_layerModel.m_singleLayers.size();
	for (int i=0; i<nLayers; i++)
	{
		int num = pDoc->m_layerModel.m_singleLayers[i].size();
		CPoint2D *point = new CPoint2D[num];
		for (int j=0; j<num; j++)
			point[j] = pDoc->m_layerModel.m_singleLayers[i][j];

		long nSize = GetSpline3Pointf(point,num);

		if( nSize > 0 )
		{
			CPoint2D *pt = new CPoint2D[nSize];
			GetSpline3Pointf(point,num,FALSE,pt);
			std::vector<CPoint2D> pts;
			for (int j=0; j<nSize; j++)
				pts.push_back(pt[j]);
			delete []pt;

			singleLayers.push_back(pts);
		}
		delete []point;
	}

	//////////////////////////////////////////////////////////////////////////
	// 复制法线
	normals[0].resize(pDoc->m_layerModel.m_normals[0].size());
	normals[1].resize(pDoc->m_layerModel.m_normals[1].size());
	std::copy(pDoc->m_layerModel.m_normals[0].begin(), pDoc->m_layerModel.m_normals[0].end(), normals[0].begin());
	std::copy(pDoc->m_layerModel.m_normals[1].begin(), pDoc->m_layerModel.m_normals[1].end(), normals[1].begin());

	//////////////////////////////////////////////////////////////////////////
	//
	// 计算每个法线区间的份数
	// 份数存入m_iNormals
	//
	{	
		for (int i=0; i<layers.size();i++)
		{			
			int nSize = layers[i].size();

			long iSeek = 0;
			CPoint2D fPt;
			for (int j=0; j<normals[0].size(); j++)
			{
				CPoint2D p1[2];
				p1[0] = normals[0][j];
				p1[1] = normals[1][j];
				double dDist = 0;					
				for(long k=iSeek; k<nSize-1; k++)
				{
					CPoint2D p2[2];
					p2[0] = layers[i][k];
					p2[1] = layers[i][k+1];
					if( k==0 )
						fPt = p2[0];

					CPoint2D p;
					if( Intersect(p1,p2,&p))
					{	
						dDist += GetPointDistance(p2[0],p);
						fPt = p;
						int num = dDist/pDoc->m_layerModel.m_dNormalSpace + 0.5;
						if( i == 0 )
							m_iNormals.push_back(num);
						else
						{
							if( num>m_iNormals[j] )
								m_iNormals[j] = num;
						}

						iSeek = k;

						if( j == normals[0].size()-1)
						{
							dDist = GetPointDistance(p, p2[1]);

							k++;

							for ( ;k<nSize-1; k++)
							{
								p2[0] = layers[i][k];
								p2[1] = layers[i][k+1];
								dDist += GetPointDistance(p2[0], p2[1]);
							}

							int num = dDist/pDoc->m_layerModel.m_dNormalSpace + 0.5;
							if( i == 0 )
								m_iNormals.push_back(num);
							else
							{
								if( num>m_iNormals[j+1] )
									m_iNormals[j+1] = num;
							}
						}

						break;
					}
					else
						dDist += GetPointDistance(fPt, p2[1]);

					fPt = p2[1];
				}
			}

		}
	}

	//------------------------------------------------------------------------
	// 计算每两条夹层线间的间隔数
	// 份数存入m_iHorzSpaces
	//
	{
		// 求每条法线
		for(int i=0; i<layers.size()-1; i++)
		{
			CPoint2D p1 = layers[i][0];
			CPoint2D p2 = layers[i+1][0];

			double dDis = GetPointDistance(p1, p2);

			int num = dDis/pDoc->m_layerModel.m_dHorzSpace + 0.5;

			m_iHorzSpaces.push_back(num);
		}

	}
	//////////////////////////////////////////////////////////////////////////
	// 先求首尾夹层线与法线的交点
	//
	// 首尾夹层线与法线交点集
	std::vector<CPoint2D> *insPt = new std::vector<CPoint2D> [layers.size()];
	std::vector<double> dFirstCurvarture;	// 每条夹层线开始段的曲率
	std::vector<double> dEndCurvarture;		// 每条夹层线最后段的曲率

	// 首条法线为夹层线首点相连
	// 尾条法线为夹层线尾点相连

	// 求每条法线
	for(int i=0; i<layers.size(); i++)
	{
		CPoint2D point= layers[i][0];

		insPt[i].push_back(point);	// 加首条法线

		int nSize = layers[i].size();

		int iSeek = 1;

		for (int j=0; j<normals[0].size();j++)
		{
			std::vector<CPoint2D> pts;

			CPoint2D p1[2];
			p1[0] = normals[0][j];
			p1[1] = normals[1][j];

			int n = insPt[i].size();
			pts.push_back(insPt[i][n-1]);

			for (long k=iSeek;k<nSize; k++)
			{
				CPoint2D p2[2];

				p2[0] = layers[i][k-1];
				p2[1] = layers[i][k];

				pts.push_back(p2[0]);

				CPoint2D p;

				if( Intersect(p1,p2, &p) )
				{	
					pts.push_back(p);
					iSeek = k+1;

					if( j==0 )
						dFirstCurvarture.push_back(GetMaxCurvarture(pts,true));

					Bresenham(pts, insPt[i], m_iNormals[j]);

					insPt[i].push_back(p);

					// 求末段曲率
					if( j==normals[0].size()-1)
					{
						pts.erase(pts.begin(), pts.end());

						pts.push_back(p);

						for (;k<nSize; k++)
							pts.push_back(layers[i][k]);

						dEndCurvarture.push_back(GetMaxCurvarture(pts,false));

						Bresenham(pts, insPt[i], m_iNormals[j+1]);

						CPoint2D point = layers[i][nSize-1];
						insPt[i].push_back(point); // 加末条法线
					}
					break;
				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 计算每条法线
	std::vector<CPoint2D> *insPt1 = new std::vector<CPoint2D> [(layers.size()-1)*2];
	for (int i=0;i<layers.size();i++)
	{
		if( i==0 )
		{
			insPt1[i].resize(insPt[0].size());
			std::copy(insPt[i].begin(),insPt[i].end(), insPt1[i].begin());
		}
		else if(i == layers.size()-1 )
		{
			insPt1[(layers.size()-1)*2-1].resize(insPt[0].size());
			std::copy(insPt[i].begin(),insPt[i].end(), insPt1[(layers.size()-1)*2-1].begin());
		}
		else
		{
			insPt1[2*i-1].resize(insPt[0].size());
			std::copy(insPt[i].begin(),insPt[i].end(), insPt1[2*i-1].begin());

			insPt1[2*i].resize(insPt[0].size());
			std::copy(insPt[i].begin(),insPt[i].end(), insPt1[2*i].begin());			
		}
	}

	delete []insPt;

	// 判断首末法相交情况
	for(int i=0; i<layers.size()-1; i++)
	{		
		//首
		CPoint2D p1[2];
		p1[0] = layers[i][0];
		p1[1] = layers[i+1][0];
		
		int j = 1; // 从第二点开始判断

		for ( j=1;j<m_iNormals[0]; j++)
		{
			CPoint2D p2[2];
			p2[0] = insPt1[2*i][j];
			p2[1] = insPt1[2*i+1][j];

			CPoint2D p;
			if( Intersect(p1, p2, &p, false))//是否相交
			{
				if( dFirstCurvarture[i] < dFirstCurvarture[i+1])
					insPt1[2*i][j] = layers[i][0];
				else
					insPt1[2*i+1][j] = layers[i+1][0];
			}
			else
				break;
		}

		int k=j;
		std::vector<CPoint2D> pts1;
		std::vector<CPoint2D> pts2;
		
 		if( dFirstCurvarture[i] < dFirstCurvarture[i+1])
 			pts1.push_back(layers[i][0]);
 		else
 			pts1.push_back(layers[i+1][0]);

		for (;j<m_iNormals[0]+1; j++)
		{
			if( dFirstCurvarture[i] < dFirstCurvarture[i+1])
			{				
				pts1.push_back(insPt1[2*i][j]);
			}
			else
			{	
				pts1.push_back(insPt1[2*i+1][j]);
			}
		}

		if( dFirstCurvarture[i] < dFirstCurvarture[i+1])
		{
			Bresenham(pts1, pts2, m_iNormals[0]-k+1);
			int m=0;
			for (j=k;j<m_iNormals[0]; j++)
			{
				insPt1[2*i][j] = pts2[m];
				m++;
			}
		}
		else
		{
			Bresenham(pts1, pts2, m_iNormals[0]-k+1);
			int m=0;
			for (j=k;j<m_iNormals[0]; j++)
			{
				insPt1[2*i+1][j] = pts2[m];
				m++;
			}
		}
		
		pts1.clear();
		pts2.clear();
		
		// 末

		int n = insPt1[2*i].size();

		p1[0] = layers[i][layers[i].size()-1];
		p1[1] = layers[i+1][layers[i+1].size()-1];
		
		for (j=1;j<m_iNormals[m_iNormals.size()-1]; j++)
		{
			CPoint2D p2[2];
			p2[0] = insPt1[2*i][n-1-j];
			p2[1] = insPt1[2*i+1][n-1-j];

			CPoint2D p;
			if( Intersect(p1, p2, &p, false))//是否相交
			{
				if( dEndCurvarture[i] < dEndCurvarture[i+1])
					insPt1[2*i][n-1-j] = layers[i][layers[i].size()-1];
				else
					insPt1[2*i+1][n-1-j] = layers[i+1][layers[i+1].size()-1];
			}
			else
				break;
		}

		k=j;

		if( dEndCurvarture[i] < dEndCurvarture[i+1])
			pts1.push_back(layers[i][layers[i].size()-1]);
		else
			pts1.push_back(layers[i+1][layers[i+1].size()-1]);

		for (;j<m_iNormals[m_iNormals.size()-1]+1; j++)
		{
			if( dEndCurvarture[i] < dEndCurvarture[i+1])
			{				
				pts1.push_back(insPt1[2*i][n-1-j]);
			}
			else
			{
				pts1.push_back(insPt1[2*i+1][n-1-j]);
			}
		}

		if( dEndCurvarture[i] < dEndCurvarture[i+1])
		{
			Bresenham(pts1, pts2, m_iNormals[m_iNormals.size()-1]-k+1);
			int m=0;
			for (j=k;j<m_iNormals[m_iNormals.size()-1]; j++)
			{
				insPt1[2*i][n-1-j] = pts2[m];
				m++;
			}
		}
		else
		{
			Bresenham(pts1, pts2,m_iNormals[m_iNormals.size()-1]-k+1);
			int m=0;
			for (j=k;j<m_iNormals[m_iNormals.size()-1]; j++)
			{
				insPt1[2*i+1][n-1-j] = pts2[m];
				m++;
			}
		}

		pts1.clear();
		pts2.clear();
	}

	dFirstCurvarture.clear();
	dEndCurvarture.clear();

	//////////////////////////////////////////////////////////////////////////
	// 计算每条法线对应的纵向模型
	//
	{
		double dHeight = m_dBottom - m_dTop;
		int iVertNum = dHeight/pDoc->m_layerModel.m_dVertSpace+0.5;
		int nSize = pDoc->m_layerModel.m_vertModel.size(); // 纵向模型数 = 主法线数

		std::vector<std::vector<CPoint2D>> vertModel;
		// 先纵向插值
		for (int i=0; i<nSize; i++)
		{
			std::vector<CPoint2D> points;
			
			CPoint2D *pp = new CPoint2D[pDoc->m_layerModel.m_vertModel[i].size()];
			for (int j=0; j<pDoc->m_layerModel.m_vertModel[i].size(); j++)
				pp[j] = pDoc->m_layerModel.m_vertModel[i][j];

			SPLINE::CSpline sp(pp, pDoc->m_layerModel.m_vertModel[i].size());
			sp.Generate();

			int nPts = sp.GetCurveCount(20);

			//long nPts = GetSpline3Pointf(pp,pDoc->m_layerModel.m_vertModel[i].size());

			CPoint2D *pts = new CPoint2D[nPts];
			//GetSpline3Pointf(pp, pDoc->m_layerModel.m_vertModel[i].size(), FALSE, pts);
			nPts = 0;
			sp.GetCurve(pts,nPts,20);

			delete []pp;

			long iSeek = 0;
			for (int j=0;j<iVertNum+1; j++)
			{
				double y = 1.0-(double)j/iVertNum;
				double x;
				for (long k=iSeek; k<nPts-1; k++)
				{
					CPoint2D pt[2];
					pt[0] = pts[k];
					pt[1] = pts[k+1];
					if( y<=pt[0].y && y>=pt[1].y)
					{
						iSeek = k;
						x = pt[0].x + ((y-pt[0].y)/(pt[1].y-pt[0].y)*(pt[1].x-pt[0].x));
						break;
					}
				}

				CPoint2D p;
				p.x = x;
				p.y = y;
				points.push_back(p);
			}		

			delete []pts;

			vertModel.push_back(points);
		}

		//////////////////////////////////////////////////////////////////////////
		// 再给每条法线设置纵向模型

		for (int i=0;i<nSize+1;i++)
		{
			int num = m_iNormals[i];
			for (int j=0;j<num;j++)
			{
				std::vector<CPoint2D> pts;
				pts.resize(vertModel[0].size());
				m_vertModel.push_back(pts);
			}
			std::vector<CPoint2D> pts;
			pts.resize(vertModel[0].size());
			m_vertModel.push_back(pts);
		}
		
		int idx = 1;

		for (int i=0;i<nSize+1;i++)
		{
			if( i== 0 )
				std::copy(vertModel[0].begin(), vertModel[0].end(), m_vertModel[idx-1].begin());
			else if(i<nSize)
			{
				std::copy(vertModel[i-1].begin(), vertModel[i-1].end(), m_vertModel[idx-1].begin());
			}
			else
			{
				std::copy(vertModel[nSize-1].begin(), vertModel[nSize-1].end(), m_vertModel[idx-1].begin());
			}

			int num = m_iNormals[i];

			for (int k=0;k<iVertNum+1; k++)
			{
				CPoint2D *pPts = new CPoint2D[nSize+2];

				pPts[0].x = vertModel[0][k].x;
				pPts[0].y = 0;

				int nn = vertModel[0].size();
				for (int m = 0; m<nSize; m++)
				{						
					pPts[m+1].x = vertModel[m][k].x;
					pPts[m+1].y = m+1;
				}

				pPts[nSize+1].x = vertModel[nSize-1][k].x;
				pPts[nSize+1].y = nSize+1;

				SPLINE::CSpline sp(pPts, nSize+2);
				sp.Generate();

				SPLINE::CCurve c;
				c.PutCurve(sp.Ax[i],sp.Ay[i],sp.Bx[i],sp.By[i],sp.Cx[i],sp.Cy[i],num);
				CPoint2D *ppp = new CPoint2D[num+2];
				int nnn= 0;
				c.GetCurve(sp.Px[i],sp.Py[i], ppp, nnn);			

				int index = idx;

				for (int j=0;j<num;j++)
				{	
					m_vertModel[index][k].x = ppp[j+1].x;
					m_vertModel[index][k].y = vertModel[0][k].y;
					index ++;
				}				

				delete [] ppp;
				delete [] pPts;
			}

			idx += num+1;
		}

		/*
		for (int i=0;i<nSize;i++)
		{
			int num1 = m_iNormals[i];
			int num2 = m_iNormals[i+1];
			if( i== 0 )
			{
				for (int j=0;j<num1; j++)
				{
					std::vector<CPoint2D> pts;
					pts.resize(vertModel[0].size());
					std::copy(vertModel[0].begin(), vertModel[0].end(), pts.begin());
					m_vertModel.push_back(pts);
				}
			}

			{
				std::vector<CPoint2D> pts;
				pts.resize(vertModel[i].size());
				std::copy(vertModel[i].begin(), vertModel[i].end(), pts.begin());
				m_vertModel.push_back(pts);
			}

			if( i<nSize-1)
			{
				for (int j=0;j<num2;j++)
				{
					std::vector<CPoint2D> pts;
					for (int k=0;k<vertModel[0].size(); k++)
					{
						CPoint2D pt;

						CPoint2D p[2];

						p[0] = vertModel[i][k];
						p[1] = vertModel[i+1][k];

						pt.x = p[0].x+(p[1].x - p[0].x)*(j+1)/(num2+1);
						pt.y = p[0].y+(p[1].y - p[0].y)*(j+1)/(num2+1);
						
						pts.push_back(pt);
					}

					m_vertModel.push_back(pts);
				}
			}

			if( i==nSize-1)
			{
				for (int j=0;j<num2; j++)
				{
					std::vector<CPoint2D> pts;
					pts.resize(vertModel[i].size());
					std::copy(vertModel[i].begin(), vertModel[i].end(), pts.begin());
					m_vertModel.push_back(pts);
				}
			}
		}

		*/
	}


	//////////////////////////////////////////////////////////////////////////
	{ // 写夹层线
		for (int i=0; i<layers.size()-1;i++)
		{
			std::vector<CPoint2D> pts;
			int n = insPt1[0].size();
			for (int j=0; j<n; j++)
				pts.push_back(insPt1[2*i][j]);
			m_layers.push_back(pts);
			
			int iLayers = m_iHorzSpaces[i];
			
			for (int j=0; j<iLayers;j++)
			{
				pts.clear();
				int n = insPt1[0].size();
				for (int k=0; k<n; k++)
				{			
					CPoint2D pt;
					pt.x = insPt1[2*i][k].x + (insPt1[2*i+1][k].x-insPt1[2*i][k].x)/(iLayers+1)*(j+1);
					pt.y = insPt1[2*i][k].y + (insPt1[2*i+1][k].y-insPt1[2*i][k].y)/(iLayers+1)*(j+1);
					pts.push_back(pt);
				}
				m_layers.push_back(pts);
			}

			pts.clear();

			for (int j=0; j<n; j++)
				pts.push_back(insPt1[2*i+1][j]);

			m_layers.push_back(pts);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	{ // 获取单夹层线的每点法线
		int nSize = singleLayers.size();
		for (int i=0; i<nSize; i++)
		{
			std::vector<tagNormal> nors;
			std::vector<CPoint2D> pts;
			
			pts.push_back(singleLayers[i][0]);

			int nPoints = singleLayers[i].size();
			int n = insPt1[0].size();				// 法线总数
			for (int m=0; m<nPoints-1; m++)
			{
				CPoint2D p2[2];			// 夹层线
				p2[0] = singleLayers[i][m];
				p2[1] = singleLayers[i][m+1];
				for (int j=0; j<n; j++)
				{
					for (int k=0;k<layers.size()-1; k++)
					{
						CPoint2D p1[2];				// 法线
						p1[0]=insPt1[2*k][j];
						p1[1]=insPt1[2*k+1][j];
						CPoint2D p;
						if( Intersect(p1, p2, &p, false))
						{
							tagNormal nor;
							nor.iSection = k;
							nor.iNormal = j;

							nors.push_back(nor);

							pts.push_back(p);

							j = n;

							break;
						}
					}
				}
			}

			m_singleNormal.push_back(nors);

			pts.push_back(singleLayers[i][nPoints-1]);
			m_singleLayers.push_back(pts);			
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 写法线
	{
		int n = insPt1[0].size();
		for (int i=0;i<layers.size()-1; i++)
		{
			std::vector<CPoint2D> nor[2];
			for (int j=0; j<n; j++)
			{
				CPoint2D pt1 = insPt1[2*i][j];
				CPoint2D pt2 = insPt1[2*i+1][j];

				nor[1].push_back(pt1);
				nor[0].push_back(pt2);
			}

			m_normals[0].push_back(nor[0]);
			m_normals[1].push_back(nor[1]);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	// 写法线
	{
		CString strFileName = newGUID();
		strFileName += _T(".dat");

		CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
		CString strTargetName = pMF->GetProjectDatPath();
		strTargetName += _T("\\files\\");
		strTargetName += strFileName;

		CFile file(strTargetName, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
		CArchive ar(&file, CArchive::store);

		int nRows = (3*(layers.size()-1))*insPt1[0].size();
		int nCols = 4;

		ar << nRows;
		ar << nCols;
		CString strTmp;
		int n = insPt1[0].size();
		for (int i=0;i<layers.size()-1; i++)
		{
			for (int j=0; j<n; j++)
			{
				strTmp.Format(_T("%g"), m_normals[0][i][j].x);
				ar << strTmp;
				strTmp.Format(_T("%g"), m_normals[0][i][j].y);
				ar << strTmp;
				strTmp.Format(_T("%g"), -99999.0);
				ar << strTmp;
				strTmp = _T("");
				ar << strTmp;

				strTmp.Format(_T("%g"), m_normals[1][i][j].x);
				ar << strTmp;
				strTmp.Format(_T("%g"), m_normals[1][i][j].y);
				ar << strTmp;
				strTmp.Format(_T("%g"), -99999.0);
				ar << strTmp;
				strTmp = _T("");
				ar << strTmp;

				strTmp = _T("EOM");
				ar << strTmp;
				strTmp = _T("");
				ar << strTmp;
				strTmp = _T("");
				ar << strTmp;
				strTmp = _T("");
				ar << strTmp;
			}			
		}

		ar.Close();
		file.Close();

		//////////////////////////////////////////////////////////////////////////
		CFileView *pTreeView = pMF->GetTreeFileView();
		CViewTree *pTree = pTreeView->GetTreeCtrl();

		CString strFile = pDoc->GetPathName();
		CString strDirectory = pMF->GetProjectDatPath();
		strTmp = strDirectory;
		strTmp += _T("\\files\\");
		strFile.Replace(strTmp, "");
		strFile.MakeUpper();
		HTREEITEM hItem = pTree->IsExistFileInTree(TVI_ROOT, strFile);

		if( hItem == NULL )
			hItem = TVI_ROOT;
		else
		{
			HTREEITEM h = pTree->GetChildNode(hItem, _T("夹层法线"));
			if( h != NULL )
			{
				CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)pTree->GetItemData(h);
				delete lpNodeDat;
				pTree->DeleteItem(h);
			}
		}

		int nImage = pTree->m_nCountImages - 19;

		HTREEITEM h = pTreeView->AddTreeItem(_T("夹层法线"), nImage + 14, nImage + 14, hItem);

		CTreeNodeDat *lpNodeDat = new CTreeNodeDat;

		CFileViewObj *pObj = new CFileViewObj;
		pObj->m_strFileName = strFileName;

		lpNodeDat->m_nImage			= nImage + 14;
		lpNodeDat->m_nSelectedImage	= nImage + 14;
		lpNodeDat->m_nType			= MODEL_NORMAL;
		lpNodeDat->m_pNodeDat		= pObj;
		lpNodeDat->m_uState			= TVIS_EXPANDED;
		strFileName.MakeUpper();
		lpNodeDat->m_strFileName = strFileName;

		pTree->SetItemData(h, (DWORD)lpNodeDat);

		pTree->Expand(hItem, TVE_EXPAND);
	}

	//////////////////////////////////////////////////////////////////////////
	// 写夹层线
	{
		CString strFileName = newGUID();
		strFileName += _T(".dat");

		CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
		CString strTargetName = pMF->GetProjectDatPath();
		strTargetName += _T("\\files\\");
		strTargetName += strFileName;

		CFile file(strTargetName, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
		CArchive ar(&file, CArchive::store);
		
		int iLayers = 0;
		for (int i=0; i<layers.size()-1;i++)
			iLayers += m_iHorzSpaces[i];

		int nRows = (layers.size()+iLayers)*(insPt1[0].size()+1);
		int nCols = 4;

		ar << nRows;
		ar << nCols;
		CString strTmp;
		for (int i=0; i<layers.size()-1;i++)
		{
			int n = insPt1[0].size();
			for (int j=0; j<n; j++)
			{
				strTmp.Format(_T("%g"), insPt1[2*i][j].x);
				ar << strTmp;
				strTmp.Format(_T("%g"), insPt1[2*i][j].y);
				ar << strTmp;
				strTmp.Format(_T("%g"), -99999.0);
				ar << strTmp;
				strTmp = _T("");
				ar << strTmp;
			}
			strTmp = _T("EOM");
			ar << strTmp;
			strTmp = _T("");
			ar << strTmp;
			strTmp = _T("");
			ar << strTmp;
			strTmp = _T("");
			ar << strTmp;
			int num = m_iHorzSpaces[i];
			for (int j=0; j<num;j++)
			{
				int n = insPt1[0].size();
				for (int k=0; k<n; k++)
				{			
					CPoint2D pt;
					pt.x = insPt1[2*i][k].x + (insPt1[2*i+1][k].x-insPt1[2*i][k].x)/(num+1)*(j+1);
					pt.y = insPt1[2*i][k].y + (insPt1[2*i+1][k].y-insPt1[2*i][k].y)/(num+1)*(j+1);
					strTmp.Format(_T("%g"), pt.x);
					ar << strTmp;
					strTmp.Format(_T("%g"), pt.y);
					ar << strTmp;
					strTmp.Format(_T("%g"), -99999.0);
					ar << strTmp;
					strTmp = _T("");
					ar << strTmp;
				}
				strTmp = _T("EOM");
				ar << strTmp;
				strTmp = _T("");
				ar << strTmp;
				strTmp = _T("");
				ar << strTmp;
				strTmp = _T("");
				ar << strTmp;
			}

			for (int j=0; j<n; j++)
			{
				strTmp.Format(_T("%g"), insPt1[2*i+1][j].x);
				ar << strTmp;
				strTmp.Format(_T("%g"), insPt1[2*i+1][j].y);
				ar << strTmp;
				strTmp.Format(_T("%g"), -99999.0);
				ar << strTmp;
				strTmp = _T("");
				ar << strTmp;
			}
			strTmp = _T("EOM");
			ar << strTmp;
			strTmp = _T("");
			ar << strTmp;
			strTmp = _T("");
			ar << strTmp;
			strTmp = _T("");
			ar << strTmp;
		}

		ar.Close();
		file.Close();

		//////////////////////////////////////////////////////////////////////////
		CFileView *pTreeView = pMF->GetTreeFileView();
		CViewTree *pTree = pTreeView->GetTreeCtrl();

		CString strFile = pDoc->GetPathName();
		CString strDirectory = pMF->GetProjectDatPath();
		strTmp = strDirectory;
		strTmp += _T("\\files\\");
		strFile.Replace(strTmp, "");
		strFile.MakeUpper();
		HTREEITEM hItem = pTree->IsExistFileInTree(TVI_ROOT, strFile);
		
		if( hItem == NULL )
			hItem = TVI_ROOT;
		else
		{
			HTREEITEM h = pTree->GetChildNode(hItem, _T("侧积夹层"));
			if( h != NULL )
			{
				CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)pTree->GetItemData(h);
				delete lpNodeDat;
				pTree->DeleteItem(h);
			}
		}

		int nImage = pTree->m_nCountImages - 19;

		HTREEITEM h = pTreeView->AddTreeItem(_T("侧积夹层"), nImage + 15, nImage + 15, hItem);

		CTreeNodeDat *lpNodeDat = new CTreeNodeDat;

		CFileViewObj *pObj = new CFileViewObj;
		pObj->m_strFileName = strFileName;

		lpNodeDat->m_nImage			= nImage + 15;
		lpNodeDat->m_nSelectedImage	= nImage + 15;
		lpNodeDat->m_nType			= MODEL_LAYER;
		lpNodeDat->m_pNodeDat		= pObj;
		lpNodeDat->m_uState			= TVIS_EXPANDED;
		strFileName.MakeUpper();
		lpNodeDat->m_strFileName = strFileName;

		pTree->SetItemData(h, (DWORD)lpNodeDat);

		pTree->Expand(hItem, TVE_EXPAND);
	}

	delete []insPt1;
}

// 生成夹层面
void CFormGenerateLayer::OnBnClickedBtnGenerateLayer()
{
	// TODO: 在此添加控件通知处理程序代码

	CWaitCursor wait;

	UpdateData(TRUE);
	
	CGenerateLayerDoc *pDoc = GetDocument();

	if((m_dBottom-m_dTop) < 0.1)
	{
		AfxMessageBox(_T("包含夹层的地层厚度太小!"));
		GetDlgItem(IDC_EDIT_TOP)->SetFocus();
		return;
	}

	if( pDoc->m_layerModel.m_layers.size() < 1 )
	{
		AfxMessageBox(_T("没有选择夹层投影模型!"));
		return;
	}
	else
	{
		if( pDoc->m_layerModel.m_normals[0].size() < 1 )
		{
			AfxMessageBox(_T("夹层投影模型中法线!"));
			return;
		}
	}

	if( pDoc->m_layerModel.m_normals[0].size() > pDoc->m_layerModel.m_vertModel.size() )
	{
		AfxMessageBox(_T("不是每个法线都选择了纵向模型!"));
		return;
	}

	GenerateLayers();
	
	//////////////////////////////////////////////////////////////////////////
	// 替换深度值
	if( pDoc->m_layerModel.m_topPlane.size() > 0 ) //如果有顶部面
	{
		int nSize = pDoc->m_layerModel.m_topPlane.size(); // 替换深度值
		for (int i=0; i<nSize; i++)
		{
			int num = pDoc->m_layerModel.m_topPlane[i].m_points.size();
			for (int j=0; j<num; j++)
			{
				if( pDoc->m_layerModel.m_topPlane[i].m_points[j].m_dValue <= -9000.0)
					pDoc->m_layerModel.m_topPlane[i].m_points[j].m_dValue = m_dTop;
			}
		}
	}

	if( pDoc->m_layerModel.m_bottomPlane.size() > 0 ) //如果有底部面
	{
		int nSize = pDoc->m_layerModel.m_bottomPlane.size(); // 替换深度值
		for (int i=0; i<nSize; i++)
		{
			int num = pDoc->m_layerModel.m_bottomPlane[i].m_points.size();
			for (int j=0; j<num; j++)
			{
				if( pDoc->m_layerModel.m_bottomPlane[i].m_points[j].m_dValue <= -9000.0)
					pDoc->m_layerModel.m_bottomPlane[i].m_points[j].m_dValue = m_dBottom;
			}
		}
	}

	int nSize =  m_layers.size(); // 替换深度值
	for (int i=0; i<nSize; i++)
	{
		int num = m_layers[i].size();
		for (int j=0; j<num; j++)
		{
			if( m_layers[i][j].m_dValue <= -9000.0)
				m_layers[i][j].m_dValue = m_dDepthScale;
		}
	}

	nSize =  m_singleLayers.size(); // 替换深度值
	for (int i=0; i<nSize; i++)
	{
		int num = m_singleLayers[i].size();
		for (int j=0; j<num; j++)
		{
			if( m_singleLayers[i][j].m_dValue <= -9000.0)
				m_singleLayers[i][j].m_dValue = m_dDepthScale;
		}
	}

	int nLayers = 0;

	double dDepth = (m_dBottom - m_dDepthScale) + m_dTop;
	
	StatusProgress(1,m_layers.size()+m_singleLayers.size());
	//////////////////////////////////////////////////////////////////////////
	// 复夹层
	//
	{
		//////////////////////////////////////////////////////////////////////////
		//
		// 先求顶底面夹层线的x,y,z坐标

		std::vector<std::vector<CPoint2D>> topLines;
		std::vector<std::vector<CPoint2D>> bottomLines;

		nSize = m_layers.size(); // 线总数

		for (int i=0;i<nSize; i++)
		{
			std::vector<CPoint2D> topLine;
			topLines.push_back(topLine);

			std::vector<CPoint2D> bottomLine;
			bottomLines.push_back(bottomLine);

			int num = m_layers[i].size();	// 点数
			for (int j=0; j<num; j++)
			{
				CPoint2D pt = m_layers[i][j];

				CPoint2D pLine2[2];
				GetNormal(i,j,pLine2);
				// ax+by+c=0
				// a=y2-y1
				// b=x1-x2
				// c=x2y1-x1y2
				double la = pLine2[1].y-pLine2[0].y;
				double lb = pLine2[0].x-pLine2[1].x;
				double lc = pLine2[1].x*pLine2[0].y-pLine2[0].x*pLine2[1].y;

				// 
				// 求沿法线上的分量
				//
				int n = m_vertModel[j].size();
				double dXYScale = GetInterpolatedX(m_vertModel[j],(dDepth-m_dTop)/(m_dBottom-m_dTop));

				double dXY1 = m_vertModel[j][0].x;   // 顶面
				double dXY2 = m_vertModel[j][n-1].x; // 底面

				// 求顶底面坐标

				double ca, cb, cr;

				ca = pt.x;
				cb = pt.y;
				cr = (m_dBottom-m_dTop) * dXYScale;

				CPoint2D C0, rP1, rP2;
				C0.x = ca;
				C0.y = cb;

				clpoint(C0, cr, la, lb, lc, rP1, rP2);

				CPoint2D pLine1[2];

				pLine1[0] = pt;
				pLine1[1] = rP1;

				double dm1 = DotMultiply(pLine1, pLine2);
				pLine1[1] = rP2;
				double dm2 = DotMultiply(pLine1, pLine2);
				double Xo, Yo;

				if( cr < 0.0000000001)
				{
					Xo = pt.x;
					Yo = pt.y;
				}
				else if( dm1<0)
				{
					Xo = rP1.x;
					Yo = rP1.y;
				}
				else
				{
					Xo = rP2.x;
					Yo = rP2.y;
				}

				CPoint2D point;

				cr = (m_dBottom-m_dTop) * dXY1;

				C0.x = Xo;
				C0.y = Yo;
				clpoint(C0, cr, la, lb, lc, rP1, rP2);

				pLine1[0] = C0;
				pLine1[1] = rP1;

				dm1 = DotMultiply(pLine1, pLine2);
				pLine1[1] = rP2;
				dm2 = DotMultiply(pLine1, pLine2);

				if( cr < 0.0000000001)
				{
					point.x = Xo;
					point.y = Yo;
				}
				else if( dm1>0)
				{
					point.x = rP1.x;
					point.y = rP1.y;
				}
				else
				{
					point.x = rP2.x;
					point.y = rP2.y;
				}			

				point.m_dValue = GetTopPlaneZ(point);
				topLines[i].push_back(point);

				cr = (m_dBottom-m_dTop) * dXY2;
				C0.x = Xo;
				C0.y = Yo;
				clpoint(C0, cr, la, lb, lc, rP1, rP2);

				pLine1[0] = C0;
				pLine1[1] = rP1;

				dm1 = DotMultiply(pLine1, pLine2);
				pLine1[1] = rP2;
				dm2 = DotMultiply(pLine1, pLine2);

				if( cr < 0.0000000001)
				{
					point.x = Xo;
					point.y = Yo;
				}
				else if( dm1>0)
				{
					point.x = rP1.x;
					point.y = rP1.y;
				}
				else
				{
					point.x = rP2.x;
					point.y = rP2.y;
				}

				point.m_dValue = GetBottomPlaneZ(point);
				bottomLines[i].push_back(point);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// 按纵向比例求每一条夹层线的x,y,z

		CString strTmp;

		nSize = m_layers.size(); // 线总数
		for (int i=0;i<nSize; i++)
		{
			/*CStringArray fileDat;
			fileDat.Add(_T("Coordinate"));
			fileDat.Add(_T(""));
			fileDat.Add(_T(""));
			fileDat.Add(_T(""));
			*/
			CArray<double,double> arCoord; // 坐标

			int num = m_layers[i].size();	// 点数
			for (int j=0; j<num; j++)
			{
				CPoint2D pt = m_layers[i][j];
				CPoint2D pLine2[2];
				GetNormal(i,j,pLine2);	// 获取法线

				// ax+by+c=0
				// a=y2-y1
				// b=x1-x2
				// c=x2y1-x1y2
				double la = pLine2[1].y-pLine2[0].y;
				double lb = pLine2[0].x-pLine2[1].x;
				double lc = pLine2[1].x*pLine2[0].y-pLine2[0].x*pLine2[1].y;

				//////////////////////////////////////////////////////////////////////////			

				double dTopZ = topLines[i][j].m_dValue;
				double dBottomZ = bottomLines[i][j].m_dValue;
				double dHeight = dBottomZ - dTopZ;

				double dXYScale = GetInterpolatedX(m_vertModel[j],(dDepth-dTopZ)/dHeight);

				double ca, cb, cr;

				ca = pt.x;
				cb = pt.y;
				cr = dHeight * dXYScale;

				CPoint2D C0, rP1, rP2;
				C0.x = ca;
				C0.y = cb;

				clpoint(C0, cr, la, lb, lc, rP1, rP2);

				CPoint2D pLine1[2];

				pLine1[0] = pt;
				pLine1[1] = rP1;

				double dm1 = DotMultiply(pLine1, pLine2);
				pLine1[1] = rP2;
				double dm2 = DotMultiply(pLine1, pLine2);
				double Xo, Yo;

				if( cr < 0.0000000001)
				{
					Xo = pt.x;
					Yo = pt.y;
				}
				else if( dm1<0)
				{
					Xo = rP1.x;
					Yo = rP1.y;
				}
				else
				{
					Xo = rP2.x;
					Yo = rP2.y;
				}

				int n = m_vertModel[j].size();

				for (int k=0; k<n; k++)
				{
					// 
					// 求沿法线上的分量
					//
					double dXY = m_vertModel[j][k].x;
					double dZ = m_vertModel[j][k].y;

					TRACE("%g %g\n",dXY,dZ);

					CPoint2D point;

					cr = dHeight * dXY;
					C0.x = Xo;
					C0.y = Yo;
					clpoint(C0, cr, la, lb, lc, rP1, rP2);

					pLine1[0] = C0;
					pLine1[1] = rP1;

					dm1 = DotMultiply(pLine1, pLine2);
					pLine1[1] = rP2;
					dm2 = DotMultiply(pLine1, pLine2);

					if( cr < 0.000000001)
					{
						point.x = Xo;
						point.y = Yo;
					}
					else if( dm1>0)
					{
						point.x = rP1.x;
						point.y = rP1.y;
					}
					else
					{
						point.x = rP2.x;
						point.y = rP2.y;
					}

					point.m_dValue = dTopZ + (1-dZ)*dHeight;

					//strTmp.Format(_T("%g"),point.x);
					//fileDat.Add(strTmp);

					//strTmp.Format(_T("%g"),point.y);
					//fileDat.Add(strTmp);

					//strTmp.Format(_T("%g"),-point.m_dValue); // 保存为井深(海拔)
					//fileDat.Add(strTmp);

					//strTmp = _T("");
					//fileDat.Add(strTmp);

					arCoord.Add(point.x);
					arCoord.Add(point.y);
					arCoord.Add(-point.m_dValue);
					arCoord.Add(0);
				}
			}

// 			fileDat.Add(_T("CoordIndex"));
// 			fileDat.Add(_T(""));
// 			fileDat.Add(_T(""));
// 			fileDat.Add(_T(""));

			CArray<int, int> arCoordInx;

			for (int j=1; j<num; j++)
			{
				int n = m_vertModel[j].size();

				for (int k= 1; k<n; k++)
				{
// 					strTmp.Format(_T("%d"),(j-1)*n + k-1);
// 					fileDat.Add(strTmp);
// 					strTmp.Format(_T("%d"),(j-1)*n + k);
// 					fileDat.Add(strTmp);
// 					strTmp.Format(_T("%d"),j*n     + k-1);
// 					fileDat.Add(strTmp);
// 					strTmp = _T("-1");
// 					fileDat.Add(strTmp);
// 
// 					strTmp.Format(_T("%d"),(j-1)*n + k);
// 					fileDat.Add(strTmp);
// 					strTmp.Format(_T("%d"),j*n     + k);
// 					fileDat.Add(strTmp);
// 					strTmp.Format(_T("%d"),j*n     + k-1);
// 					fileDat.Add(strTmp);
// 					strTmp = _T("-1");
// 					fileDat.Add(strTmp);

					arCoordInx.Add((j-1)*n + k-1);
					arCoordInx.Add((j-1)*n + k);
					arCoordInx.Add(j*n     + k-1);
					arCoordInx.Add(-1);

					arCoordInx.Add((j-1)*n + k);
					arCoordInx.Add(j*n     + k);
					arCoordInx.Add(j*n     + k-1);
					arCoordInx.Add(-1);
				}
			}

			CString strFileName = newGUID();
			strFileName += _T(".dat");

			CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
			CString strTargePathName = pMF->GetProjectDatPath();
			strTargePathName += _T("\\files\\");
			strTargePathName += strFileName;

			int nRows1 = arCoord.GetSize();
			int nRows2 = arCoordInx.GetSize();
			int nCols = 4;
			CFile file(strTargePathName,CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
			CArchive ar(&file, CArchive::store);

			ar << nRows1;
			ar << nRows2;
			ar << nCols;
// 			long n = (nRows*nCols); 
// 			for (long j=0;j<n;j++)
// 			{
// 				CString strTmp = fileDat[j];
// 
// 				if( strTmp == _T("Coordinate"))
// 				ar << strTmp;
// 			}

			//strTmp = _T("Coordinate");
			//ar << strTmp;

			//strTmp = _T("CoordIndex");
			//ar << strTmp;

			for (long j=0;j<nRows1;j++)
			{	
				ar << arCoord[j];
			}

			for (long j=0;j<nRows2;j++)
			{	
				ar << arCoordInx[j];
			}

			ar.Close();
			file.Close();

			HTREEITEM hItem = pMF->GetTreeFileView()->GetTreeCtrl()->GetSelectedItem();
			if( hItem == NULL )
				hItem = TVI_ROOT;
			else
			{
				CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)pMF->GetTreeFileView()->GetTreeCtrl()->GetItemData(hItem);
				if( lpNodeDat->m_nType != FOLDER)
					hItem = TVI_ROOT;
			}

			int nImage = pMF->GetTreeFileView()->GetTreeCtrl()->m_nCountImages - 19;

			nLayers++;
			strTmp.Format(_T("夹层面 %d"),nLayers);
			HTREEITEM h = pMF->GetTreeFileView()->AddTreeItem(strTmp, nImage + 12, nImage + 12, hItem);

			CTreeNodeDat *lpNodeDat = new CTreeNodeDat;

			CFileViewObj *pObj = new CFileViewObj;
			pObj->m_strFileName = strFileName;

			lpNodeDat->m_nImage			= nImage + 12;
			lpNodeDat->m_nSelectedImage	= nImage + 12;
			lpNodeDat->m_nType			= FILE_PLANE;
			lpNodeDat->m_pNodeDat		= pObj;
			lpNodeDat->m_uState			= TVIS_EXPANDED;
			strFileName.MakeUpper();
			lpNodeDat->m_strFileName = strFileName;

			pMF->GetTreeFileView()->GetTreeCtrl()->SetItemData(h, (DWORD)lpNodeDat);

			pMF->GetTreeFileView()->GetTreeCtrl()->Expand(hItem, TVE_EXPAND);

			StatusSetProgress(1,nLayers);

			pMF->GetTreeFileView()->GetTreeCtrl()->Select(hItem, TVGN_CARET);
		}
	}
	
	//________________________________________________________________________
	// 
	// 单夹层
	{
		//////////////////////////////////////////////////////////////////////////
		//
		// 先求顶底面夹层线的x,y,z坐标

		std::vector<std::vector<CPoint2D>> topLines;
		std::vector<std::vector<CPoint2D>> bottomLines;

		nSize = m_singleLayers.size(); // 线总数

		for (int i=0;i<nSize; i++)
		{
			std::vector<CPoint2D> topLine;
			topLines.push_back(topLine);

			std::vector<CPoint2D> bottomLine;
			bottomLines.push_back(bottomLine);

			int num = m_singleLayers[i].size();	// 点数
			for (int j=0; j<num; j++)
			{
				if( j == 209 )
					TRACE("");
				CPoint2D pt = m_singleLayers[i][j];

				CPoint2D pLine2[2];
				if( j== 0 )
				{
					if(m_singleNormal[i][j].iNormal == 0 )
					{
						pLine2[1] = m_normals[1][m_singleNormal[i][j].iSection][m_singleNormal[i][j].iNormal];
						pLine2[0] = m_normals[0][m_singleNormal[i][j].iSection][m_singleNormal[i][j].iNormal];
					}
					else
					{
						pLine2[1].x = (m_normals[1][m_singleNormal[i][j].iSection][m_singleNormal[i][j].iNormal-1].x+m_normals[1][m_singleNormal[i][j].iSection][m_singleNormal[i][j].iNormal].x)/2;
						pLine2[1].y = (m_normals[1][m_singleNormal[i][j].iSection][m_singleNormal[i][j].iNormal-1].y+m_normals[1][m_singleNormal[i][j].iSection][m_singleNormal[i][j].iNormal].y)/2;

						pLine2[0].x = (m_normals[0][m_singleNormal[i][j].iSection][m_singleNormal[i][j].iNormal-1].x+m_normals[0][m_singleNormal[i][j].iSection][m_singleNormal[i][j].iNormal].x)/2;
						pLine2[0].y = (m_normals[0][m_singleNormal[i][j].iSection][m_singleNormal[i][j].iNormal-1].y+m_normals[0][m_singleNormal[i][j].iSection][m_singleNormal[i][j].iNormal].y)/2;
					}
				}
				else if( j== num -1)
				{
					int m = m_normals[1][m_singleNormal[i][j-2].iSection].size()-1;
					if(m_singleNormal[i][j-2].iNormal == m )
					{
						pLine2[1] = m_normals[1][m_singleNormal[i][j-2].iSection][m_singleNormal[i][j-2].iNormal];
						pLine2[0] = m_normals[0][m_singleNormal[i][j-2].iSection][m_singleNormal[i][j-2].iNormal];
					}
					else
					{
						pLine2[1].x = (m_normals[1][m_singleNormal[i][j-2].iSection][m_singleNormal[i][j-2].iNormal-1].x+m_normals[1][m_singleNormal[i][j-2].iSection][m_singleNormal[i][j-2].iNormal].x)/2;
						pLine2[1].y = (m_normals[1][m_singleNormal[i][j-2].iSection][m_singleNormal[i][j-2].iNormal-1].y+m_normals[1][m_singleNormal[i][j-2].iSection][m_singleNormal[i][j-2].iNormal].y)/2;

						pLine2[0].x = (m_normals[0][m_singleNormal[i][j-2].iSection][m_singleNormal[i][j-2].iNormal-1].x+m_normals[0][m_singleNormal[i][j-2].iSection][m_singleNormal[i][j-2].iNormal].x)/2;
						pLine2[0].y = (m_normals[0][m_singleNormal[i][j-2].iSection][m_singleNormal[i][j-2].iNormal-1].y+m_normals[0][m_singleNormal[i][j-2].iSection][m_singleNormal[i][j-2].iNormal].y)/2;
					}
				}
				else
				{					
					pLine2[1] = m_normals[1][m_singleNormal[i][j-1].iSection][m_singleNormal[i][j-1].iNormal];
					pLine2[0] = m_normals[0][m_singleNormal[i][j-1].iSection][m_singleNormal[i][j-1].iNormal];
				}
				// ax+by+c=0
				// a=y2-y1
				// b=x1-x2
				// c=x2y1-x1y2
				double la = pLine2[1].y-pLine2[0].y;
				double lb = pLine2[0].x-pLine2[1].x;
				double lc = pLine2[1].x*pLine2[0].y-pLine2[0].x*pLine2[1].y;

				// 
				// 求沿法线上的分量
				//
				int n;
				double dXYScale,dXY1,dXY2;

				if( j== 0 )
				{
					n = m_vertModel[m_singleNormal[i][j].iNormal].size();
					dXYScale = GetInterpolatedX(m_vertModel[m_singleNormal[i][j].iNormal],(dDepth-m_dTop)/(m_dBottom-m_dTop));

					dXY1 = m_vertModel[m_singleNormal[i][j].iNormal][0].x;   // 顶面
					dXY2 = m_vertModel[m_singleNormal[i][j].iNormal][n-1].x; // 底面
				}
				else if( j== num-1)
				{
					n = m_vertModel[m_singleNormal[i][j-2].iNormal].size();
					dXYScale = GetInterpolatedX(m_vertModel[m_singleNormal[i][j-2].iNormal],(dDepth-m_dTop)/(m_dBottom-m_dTop));

					dXY1 = m_vertModel[m_singleNormal[i][j-2].iNormal][0].x;   // 顶面
					dXY2 = m_vertModel[m_singleNormal[i][j-2].iNormal][n-1].x; // 底面
				}
				else
				{
					n = m_vertModel[m_singleNormal[i][j-1].iNormal].size();
					dXYScale = GetInterpolatedX(m_vertModel[m_singleNormal[i][j-1].iNormal],(dDepth-m_dTop)/(m_dBottom-m_dTop));

					dXY1 = m_vertModel[m_singleNormal[i][j-1].iNormal][0].x;   // 顶面
					dXY2 = m_vertModel[m_singleNormal[i][j-1].iNormal][n-1].x; // 底面
				}

				// 求顶底面坐标

				double ca, cb, cr;

				ca = pt.x;
				cb = pt.y;
				cr = (m_dBottom-m_dTop) * dXYScale;

				CPoint2D C0, rP1, rP2;
				C0.x = ca;
				C0.y = cb;

				clpoint(C0, cr, la, lb, lc, rP1, rP2);

				CPoint2D pLine1[2];

				pLine1[0] = pt;
				pLine1[1] = rP1;

				double dm1 = DotMultiply(pLine1, pLine2);
				pLine1[1] = rP2;
				double dm2 = DotMultiply(pLine1, pLine2);
				double Xo, Yo;

				if( cr < 0.0000000001)
				{
					Xo = pt.x;
					Yo = pt.y;
				}
				else if( dm1<0)
				{
					Xo = rP1.x;
					Yo = rP1.y;
				}
				else
				{
					Xo = rP2.x;
					Yo = rP2.y;
				}

				CPoint2D point;

				cr = (m_dBottom-m_dTop) * dXY1;

				C0.x = Xo;
				C0.y = Yo;
				clpoint(C0, cr, la, lb, lc, rP1, rP2);

				pLine1[0] = C0;
				pLine1[1] = rP1;

				dm1 = DotMultiply(pLine1, pLine2);
				pLine1[1] = rP2;
				dm2 = DotMultiply(pLine1, pLine2);

				if( cr < 0.0000000001)
				{
					point.x = Xo;
					point.y = Yo;
				}
				else if( dm1>0)
				{
					point.x = rP1.x;
					point.y = rP1.y;
				}
				else
				{
					point.x = rP2.x;
					point.y = rP2.y;
				}			

				point.m_dValue = GetTopPlaneZ(point);
				topLines[i].push_back(point);

				cr = (m_dBottom-m_dTop) * dXY2;
				C0.x = Xo;
				C0.y = Yo;
				clpoint(C0, cr, la, lb, lc, rP1, rP2);

				pLine1[0] = C0;
				pLine1[1] = rP1;

				dm1 = DotMultiply(pLine1, pLine2);
				pLine1[1] = rP2;
				dm2 = DotMultiply(pLine1, pLine2);

				if( cr < 0.0000000001)
				{
					point.x = Xo;
					point.y = Yo;
				}
				else if( dm1>0)
				{
					point.x = rP1.x;
					point.y = rP1.y;
				}
				else
				{
					point.x = rP2.x;
					point.y = rP2.y;
				}

				point.m_dValue = GetBottomPlaneZ(point);
				bottomLines[i].push_back(point);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// 按纵向比例求每一条夹层线的x,y,z

		CString strTmp;

		nSize = m_singleLayers.size(); // 线总数
		for (int i=0;i<nSize; i++)
		{
// 			CStringArray fileDat;
// 			fileDat.Add(_T("Coordinate"));
// 			fileDat.Add(_T(""));
// 			fileDat.Add(_T(""));
// 			fileDat.Add(_T(""));

			CArray <double, double> arCoord;

			int num = m_singleLayers[i].size();	// 点数
			for (int j=0; j<num; j++)
			{
				CPoint2D pt = m_singleLayers[i][j];
				CPoint2D pLine2[2];
				if( j== 0 )
				{
					if(m_singleNormal[i][j].iNormal == 0 )
					{
						pLine2[1] = m_normals[1][m_singleNormal[i][j].iSection][m_singleNormal[i][j].iNormal];
						pLine2[0] = m_normals[0][m_singleNormal[i][j].iSection][m_singleNormal[i][j].iNormal];
					}
					else
					{
						pLine2[1].x = (m_normals[1][m_singleNormal[i][j].iSection][m_singleNormal[i][j].iNormal-1].x+m_normals[1][m_singleNormal[i][j].iSection][m_singleNormal[i][j].iNormal].x)/2;
						pLine2[1].y = (m_normals[1][m_singleNormal[i][j].iSection][m_singleNormal[i][j].iNormal-1].y+m_normals[1][m_singleNormal[i][j].iSection][m_singleNormal[i][j].iNormal].y)/2;

						pLine2[0].x = (m_normals[0][m_singleNormal[i][j].iSection][m_singleNormal[i][j].iNormal-1].x+m_normals[0][m_singleNormal[i][j].iSection][m_singleNormal[i][j].iNormal].x)/2;
						pLine2[0].y = (m_normals[0][m_singleNormal[i][j].iSection][m_singleNormal[i][j].iNormal-1].y+m_normals[0][m_singleNormal[i][j].iSection][m_singleNormal[i][j].iNormal].y)/2;
					}
				}
				else if( j== num -1)
				{
					int m = m_normals[1][m_singleNormal[i][j-2].iSection].size()-1;
					if(m_singleNormal[i][j-2].iNormal == m )
					{
						pLine2[1] = m_normals[1][m_singleNormal[i][j-2].iSection][m_singleNormal[i][j-2].iNormal];
						pLine2[0] = m_normals[0][m_singleNormal[i][j-2].iSection][m_singleNormal[i][j-2].iNormal];
					}
					else
					{
						pLine2[1].x = (m_normals[1][m_singleNormal[i][j-2].iSection][m_singleNormal[i][j-2].iNormal-1].x+m_normals[1][m_singleNormal[i][j-2].iSection][m_singleNormal[i][j-2].iNormal].x)/2;
						pLine2[1].y = (m_normals[1][m_singleNormal[i][j-2].iSection][m_singleNormal[i][j-2].iNormal-1].y+m_normals[1][m_singleNormal[i][j-2].iSection][m_singleNormal[i][j-2].iNormal].y)/2;

						pLine2[0].x = (m_normals[0][m_singleNormal[i][j-2].iSection][m_singleNormal[i][j-2].iNormal-1].x+m_normals[0][m_singleNormal[i][j-2].iSection][m_singleNormal[i][j-2].iNormal].x)/2;
						pLine2[0].y = (m_normals[0][m_singleNormal[i][j-2].iSection][m_singleNormal[i][j-2].iNormal-1].y+m_normals[0][m_singleNormal[i][j-2].iSection][m_singleNormal[i][j-2].iNormal].y)/2;
					}
				}
				else
				{					
					pLine2[1] = m_normals[1][m_singleNormal[i][j-1].iSection][m_singleNormal[i][j-1].iNormal];
					pLine2[0] = m_normals[0][m_singleNormal[i][j-1].iSection][m_singleNormal[i][j-1].iNormal];
				}

				// ax+by+c=0
				// a=y2-y1
				// b=x1-x2
				// c=x2y1-x1y2
				double la = pLine2[1].y-pLine2[0].y;
				double lb = pLine2[0].x-pLine2[1].x;
				double lc = pLine2[1].x*pLine2[0].y-pLine2[0].x*pLine2[1].y;

				//////////////////////////////////////////////////////////////////////////			

				double dTopZ = topLines[i][j].m_dValue;
				double dBottomZ = bottomLines[i][j].m_dValue;
				double dHeight = dBottomZ - dTopZ;

				double dXYScale;
				if( j == 0 )
					dXYScale = GetInterpolatedX(m_vertModel[m_singleNormal[i][0].iNormal],(dDepth-dTopZ)/dHeight);
				else if( j== num-1)
					dXYScale = GetInterpolatedX(m_vertModel[m_singleNormal[i][j-2].iNormal],(dDepth-dTopZ)/dHeight);
				else
					dXYScale = GetInterpolatedX(m_vertModel[m_singleNormal[i][j-1].iNormal],(dDepth-dTopZ)/dHeight);

				double ca, cb, cr;

				ca = pt.x;
				cb = pt.y;
				cr = dHeight * dXYScale;

				CPoint2D C0, rP1, rP2;
				C0.x = ca;
				C0.y = cb;

				clpoint(C0, cr, la, lb, lc, rP1, rP2);

				CPoint2D pLine1[2];

				pLine1[0] = pt;
				pLine1[1] = rP1;

				double dm1 = DotMultiply(pLine1, pLine2);
				pLine1[1] = rP2;
				double dm2 = DotMultiply(pLine1, pLine2);
				double Xo, Yo;

				if( cr < 0.0000000001)
				{
					Xo = pt.x;
					Yo = pt.y;
				}
				else if( dm1<0)
				{
					Xo = rP1.x;
					Yo = rP1.y;
				}
				else
				{
					Xo = rP2.x;
					Yo = rP2.y;
				}

				int n;
				if( j== 0)
					n = m_vertModel[m_singleNormal[i][0].iNormal].size();
				else if( j==num-1)
					n = m_vertModel[m_singleNormal[i][j-2].iNormal].size();
				else
					n = m_vertModel[m_singleNormal[i][j-1].iNormal].size();

				for (int k=0; k<n; k++)
				{
					// 
					// 求沿法线上的分量
					//
					double dXY;
					double dZ;
					if( j== 0 )
					{
						dXY = m_vertModel[m_singleNormal[i][0].iNormal][k].x;
						dZ = m_vertModel[m_singleNormal[i][0].iNormal][k].y;
					}
					else if (j==num-1)
					{						
						dXY = m_vertModel[m_singleNormal[i][j-2].iNormal][k].x;
						dZ = m_vertModel[m_singleNormal[i][j-2].iNormal][k].y;						
					}
					else
					{
						dXY = m_vertModel[m_singleNormal[i][j-1].iNormal][k].x;
						dZ = m_vertModel[m_singleNormal[i][j-1].iNormal][k].y;
					}


					TRACE("%g %g\n",dXY,dZ);

					CPoint2D point;

					cr = dHeight * dXY;
					C0.x = Xo;
					C0.y = Yo;
					clpoint(C0, cr, la, lb, lc, rP1, rP2);

					pLine1[0] = C0;
					pLine1[1] = rP1;

					dm1 = DotMultiply(pLine1, pLine2);
					pLine1[1] = rP2;
					dm2 = DotMultiply(pLine1, pLine2);

					if( cr < 0.000000001)
					{
						point.x = Xo;
						point.y = Yo;
					}
					else if( dm1>0)
					{
						point.x = rP1.x;
						point.y = rP1.y;
					}
					else
					{
						point.x = rP2.x;
						point.y = rP2.y;
					}

					if( point.x > 10000 )
						TRACE("");
					point.m_dValue = dTopZ + (1-dZ)*dHeight;

// 					strTmp.Format(_T("%g"),point.x);
// 					fileDat.Add(strTmp);
// 
// 					strTmp.Format(_T("%g"),point.y);
// 					fileDat.Add(strTmp);
// 
// 					strTmp.Format(_T("%g"),-point.m_dValue);
// 					fileDat.Add(strTmp);
// 
// 					strTmp = _T("");
// 					fileDat.Add(strTmp);

					arCoord.Add(point.x);
					arCoord.Add(point.y);
					arCoord.Add(-point.m_dValue);
					arCoord.Add(0);
				}
			}

// 			fileDat.Add(_T("CoordIndex"));
// 			fileDat.Add(_T(""));
// 			fileDat.Add(_T(""));
// 			fileDat.Add(_T(""));
			CArray<int, int> arCoordInx;

			for (int j=1; j<num; j++)
			{
				int n;
				if( j==num-1)
					n = m_vertModel[m_singleNormal[i][j-2].iNormal].size();
				else
					n = m_vertModel[m_singleNormal[i][j-1].iNormal].size();

				for (int k= 1; k<n; k++) // 四边形分为两个三角形
				{
// 					strTmp.Format(_T("%d"),(j-1)*n + k-1);					//  (j-1, k-1) -------------------------- (j, k-1)
// 					fileDat.Add(strTmp);									//             |						|
// 					strTmp.Format(_T("%d"),(j-1)*n + k);					//             |						|
// 					fileDat.Add(strTmp);									//             |						|
// 					strTmp.Format(_T("%d"),j*n     + k-1);					//             |						|
// 					fileDat.Add(strTmp);									//  (j-1, k)   -------------------------- (j , k)
// 					strTmp = _T("-1");
// 					fileDat.Add(strTmp);
// 
// 					strTmp.Format(_T("%d"),(j-1)*n + k);
// 					fileDat.Add(strTmp);
// 					strTmp.Format(_T("%d"),j*n     + k);
// 					fileDat.Add(strTmp);
// 					strTmp.Format(_T("%d"),j*n     + k-1);
// 					fileDat.Add(strTmp);
// 					strTmp = _T("-1");
// 					fileDat.Add(strTmp);

					arCoordInx.Add((j-1)*n + k-1);
					arCoordInx.Add((j-1)*n + k);
					arCoordInx.Add(j*n     + k-1);
					arCoordInx.Add(-1);

					arCoordInx.Add((j-1)*n + k);
					arCoordInx.Add(j*n     + k);
					arCoordInx.Add(j*n     + k-1);
					arCoordInx.Add(-1);
				}
			}

			CString strFileName = newGUID();
			strFileName += _T(".dat");

			CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
			CString strTargePathName = pMF->GetProjectDatPath();
			strTargePathName += _T("\\files\\");
			strTargePathName += strFileName;

			//int nRows = fileDat.GetSize()/4;
			int nRows1 = arCoord.GetSize();
			int nRows2 = arCoordInx.GetSize();
			int nCols = 4;
			CFile file(strTargePathName,CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
			CArchive ar(&file, CArchive::store);

// 			ar << nRows;
// 			ar << nCols;

			ar << nRows1;
			ar << nRows2;
			ar << nCols;

// 			long n = (nRows*nCols); 
// 			for (long j=0;j<n;j++)
// 			{
// 				CString strTmp = fileDat[j];
// 				ar << strTmp;
// 			}

			for (long j=0;j<nRows1;j++)
			{	
				ar << arCoord[j];
			}

			for (long j=0;j<nRows2;j++)
			{	
				ar << arCoordInx[j];
			}

			ar.Close();
			file.Close();

			HTREEITEM hItem = pMF->GetTreeFileView()->GetTreeCtrl()->GetSelectedItem();
			if( hItem == NULL )
				hItem = TVI_ROOT;
			else
			{
				CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)pMF->GetTreeFileView()->GetTreeCtrl()->GetItemData(hItem);
				if( lpNodeDat->m_nType != FOLDER)
					hItem = TVI_ROOT;
			}

			int nImage = pMF->GetTreeFileView()->GetTreeCtrl()->m_nCountImages - 19;

			nLayers++;
			strTmp.Format(_T("夹层面 %d"),nLayers);
			HTREEITEM h = pMF->GetTreeFileView()->AddTreeItem(strTmp, nImage + 12, nImage + 12, hItem);

			CTreeNodeDat *lpNodeDat = new CTreeNodeDat;

			CFileViewObj *pObj = new CFileViewObj;
			pObj->m_strFileName = strFileName;

			lpNodeDat->m_nImage			= nImage + 12;
			lpNodeDat->m_nSelectedImage	= nImage + 12;
			lpNodeDat->m_nType			= FILE_PLANE;
			lpNodeDat->m_pNodeDat		= pObj;
			lpNodeDat->m_uState			= TVIS_EXPANDED;
			strFileName.MakeUpper();
			lpNodeDat->m_strFileName = strFileName;

			pMF->GetTreeFileView()->GetTreeCtrl()->SetItemData(h, (DWORD)lpNodeDat);

			pMF->GetTreeFileView()->GetTreeCtrl()->Expand(hItem, TVE_EXPAND);

			StatusSetProgress(1,nLayers);

			pMF->GetTreeFileView()->GetTreeCtrl()->Select(hItem, TVGN_CARET);
		}
	}

	StatusProgress(1);
}

// 获取顶面某点深度
double CFormGenerateLayer::GetTopPlaneZ(const CPoint2D &pt)
{
	CGenerateLayerDoc *pDoc = GetDocument();
	int nSize = pDoc->m_layerModel.m_topPlane.size();
	if( nSize < 1 ) // 无顶面时，直接返回定义的顶面深度
		return m_dTop;
	
	for (int i=0; i<nSize; i++)
	{
		int num = pDoc->m_layerModel.m_topPlane[i].m_indexs.size()/3;
		for (int j=0; j<num; j++)
		{
			CPoint2D pt1, pt2, pt3;
			pt1 = pDoc->m_layerModel.m_topPlane[i].m_points[pDoc->m_layerModel.m_topPlane[i].m_indexs[j*3]];
			pt2 = pDoc->m_layerModel.m_topPlane[i].m_points[pDoc->m_layerModel.m_topPlane[i].m_indexs[j*3+1]];
			pt3 = pDoc->m_layerModel.m_topPlane[i].m_points[pDoc->m_layerModel.m_topPlane[i].m_indexs[j*3+2]];

			if( InTriangle(pt, pt1, pt2, pt3))
			{
				CVertex3D v1, v2, v3;
				v1.x = pt1.x;
				v1.y = pt1.y;
				v1.z = pt1.m_dValue;
				
				v2.x = pt2.x;
				v2.y = pt2.y;
				v2.z = pt2.m_dValue;

				v3.x = pt3.x;
				v3.y = pt3.y;
				v3.z = pt3.m_dValue;

				CFaceTriangles tri(&v1, &v2, &v3);
				tri.CalculateNormal();
				double a,b,c,d;
				tri.GetPlaneParam(&a, &b, &c, &d);

				// ax+by+cz+d=0
				// z = (-ax-by-d)/c;
				//

				if( fabs(c) <= 0.00000001)
					return m_dTop;

				double z = (-a*pt.x-b*pt.y-d)/c;

				return z;
			}
		}
	}

	return m_dTop;
}

// 获取底面某点深度
double CFormGenerateLayer::GetBottomPlaneZ(const CPoint2D &pt)
{
	CGenerateLayerDoc *pDoc = GetDocument();
	int nSize = pDoc->m_layerModel.m_bottomPlane.size();
	if( nSize < 1 ) // 无底面时，直接返回定义的底面深度
		return m_dBottom;

	for (int i=0; i<nSize; i++)
	{
		int num = pDoc->m_layerModel.m_bottomPlane[i].m_indexs.size()/3;
		for (int j=0; j<num; j++)
		{
			CPoint2D pt1, pt2, pt3;
			pt1 = pDoc->m_layerModel.m_bottomPlane[i].m_points[pDoc->m_layerModel.m_bottomPlane[i].m_indexs[j*3]];
			pt2 = pDoc->m_layerModel.m_bottomPlane[i].m_points[pDoc->m_layerModel.m_bottomPlane[i].m_indexs[j*3+1]];
			pt3 = pDoc->m_layerModel.m_bottomPlane[i].m_points[pDoc->m_layerModel.m_bottomPlane[i].m_indexs[j*3+2]];

			if( InTriangle(pt, pt1, pt2, pt3))
			{
				CVertex3D v1, v2, v3;
				v1.x = pt1.x;
				v1.y = pt1.y;
				v1.z = pt1.m_dValue;

				v2.x = pt2.x;
				v2.y = pt2.y;
				v2.z = pt2.m_dValue;

				v3.x = pt3.x;
				v3.y = pt3.y;
				v3.z = pt3.m_dValue;

				CFaceTriangles tri(&v1, &v2, &v3);
				tri.CalculateNormal();
				double a,b,c,d;
				tri.GetPlaneParam(&a, &b, &c, &d);

				// ax+by+cz+d=0
				// z = (-ax-by-d)/c;
				//

				if( fabs(c) <= 0.00000001)
					return m_dBottom;

				double z = (-a*pt.x-b*pt.y-d)/c;

				return z;
			}
		}
	}

	return m_dBottom;
}

// 获取某点所在的法线
void CFormGenerateLayer::GetNormal(const CPoint2D &pt, CPoint2D*pNor)
{
	double dDist = DBL_MAX;
	int iIndex = 0;
	int jIndex = 0;
	int nSize = m_normals[0].size();
	for (int i=0; i<nSize; i++)
	{
		int n = m_normals[0][i].size();
		for (int j = 0; j< n; j++)
		{	
			CPoint2D pt1 = m_normals[0][i][j];
			CPoint2D pt2 = m_normals[1][i][j];

			double ds = GetNearestDistance(pt1, pt2, pt);
			if( ds < dDist )
			{
				dDist = ds;
				iIndex = i;
				jIndex = j;
			}
		}
	}

	pNor[0] = m_normals[0][iIndex][jIndex];
	pNor[1] = m_normals[1][iIndex][jIndex];
}

double CFormGenerateLayer::GetInterpolatedX(const std::vector<CPoint2D> &points,double y)
{
	double x;
	if( y>points[0].y)
	{
		x = points[0].x + (y-points[0].y)/(points[1].y-points[0].y)*(points[1].x-points[0].x);
		return x;
	}
	int nSize = points.size();
	CPoint2D p1, p2;
	for (int i=0; i<nSize-1; i++)
	{
		p1 = points[i];
		p2 = points[i+1];

		if( i == 0 && y>p1.y)
		{
			x = p1.x + (y-p1.y)/(p2.y-p1.y)*(p2.x-p1.x);
			return x;
		}
		else if( y<=p1.y && y>p2.y)
		{
			x = p1.x + (y-p1.y)/(p2.y-p1.y)*(p2.x-p1.x);
			return x;
		}
	}


	x = points[nSize-1].x + (y-points[nSize-1].y)/(points[nSize-2].y-points[nSize-1].y)*(points[nSize-2].x-points[nSize-1].x);
	
	return x;
}

double CFormGenerateLayer::GetInterpolatedY(const std::vector<CPoint2D> &points,double x)
{
	double y;
	
	if( x<points[0].x)
	{
		y = points[0].y + (x-points[0].x)/(points[1].x-points[0].x)*(points[1].y-points[0].y);
		return y;
	}

	int nSize = points.size();
	CPoint2D p1, p2;
	for (int i=0; i<nSize-1; i++)
	{
		p1 = points[i];
		p2 = points[i+1];

		if( x>=p1.x && x<p2.x)
		{
			y = p1.y + (x-p1.x)/(p2.x-p1.x)*(p2.y-p1.y);
			return y;
		}
	}

	y = points[nSize-1].y + (x-points[nSize-1].x)/(points[nSize-2].x-points[nSize-1].x)*(points[nSize-2].y-points[nSize-1].y);

	return y;
}

void CFormGenerateLayer::GetNormal(int r, int c, CPoint2D*pNor)
{
	CGenerateLayerDoc *pDoc = GetDocument();
	int index = -1;
	int idx = -1;

	int nSize = pDoc->m_layerModel.m_layers.size();

	for (int i=0; i<nSize-1; i++)
	{
		idx ++;
		
		if( idx == r)
		{
			index = i;
			break;
		}

		int iHorz = m_iHorzSpaces[i];

		for (int j = 0; j<iHorz; j++)
		{
			idx ++;

			if( idx == r)
			{
				index = i;
				break;
			}
		}

		if( idx == r)
		{
			index = i;
			break;
		}
	}

	if( index == -1 )
		index = nSize - 2;

	pNor[0] = m_normals[0][index][c];
	pNor[1] = m_normals[1][index][c];
}