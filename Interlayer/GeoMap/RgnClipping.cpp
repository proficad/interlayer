#include "StdAfx.h"
#include "RgnClipping.h"
#include "GeoMapView.h"
#include "../Resource.h"

extern double ptopointset(int vcount, CPoint2D pointset[], CPoint2D p, CPoint2D &q, int *iIndex);
extern double dist(CPoint2D p1,CPoint2D p2);

IMPLEMENT_SERIAL(CRgnClipping, CGeoDrawObj, VERSIONABLE_SCHEMA|0)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRgnClipping::CRgnClipping()
{
	m_points = NULL;
	m_geoPoints = NULL;
	m_nPoints = 0;
	m_nAllocPoints = 0;	
	m_bSmooth = FALSE;
	m_trackerState = active;
	m_rtPosition = CRect(0,0,1,1);
}

CRgnClipping::CRgnClipping(CDrawLayer* pLayer)
:CGeoDrawObj(pLayer)
{
	m_rtPosition = CRect(0,0,1,1);
	m_points = NULL;
	m_geoPoints = NULL;
	m_nPoints = 0;
	m_nAllocPoints = 0;	
	m_bSmooth = FALSE;
	m_trackerState = active;
}

CRgnClipping::CRgnClipping(const CRect& position, CDrawLayer* pLayer)
:CGeoDrawObj(position,pLayer)
{
	m_points = NULL;
	m_geoPoints = NULL;
	m_nPoints = 0;
	m_nAllocPoints = 0;	
	m_bSmooth = FALSE;
	m_trackerState = active;
}

CRgnClipping::~CRgnClipping()
{
	if (m_points != NULL)
		delete[] m_points;
	if (m_geoPoints != NULL)
		delete[] m_geoPoints;
}

void CRgnClipping::Draw(CDC *pDC, CGeoMapView *pView)
{
	if(pDC->IsPrinting())
		return;

 	if( !pDC->IsPrinting() )
 	{
 		CRect cliRect;
 		pView->GetClientRect(cliRect);
 		pView->ClientToDoc(cliRect);
 		if( !Intersects(cliRect) ) //如果该图素不在当前窗口中则不显示
 			return ;
 	}

	int nSaved = pDC->SaveDC();
	pDC->SetBkMode(TRANSPARENT);          //设定显示方式	
	
	DrawLine(pDC, pView);
	
	pDC->RestoreDC(nSaved);	
}

void CRgnClipping::DrawEditMode(CGeoMapView* pView)
{
	CClientDC dc(pView);
	pView->OnPrepareDC(&dc);
	
	int mode = dc.SetROP2(R2_NOTXORPEN);

	// DrawTracker(&dc, pView);
	
	CBrush *pOldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	
	CSize size(3,3);
	dc.DPtoLP(&size);
	
	if (!m_bSmooth)//不光滑
	{
		if( m_nPoints < 3 )
			dc.Polyline(m_points, m_nPoints);
		else
			dc.Polygon(m_points, m_nPoints);
	}

	//*******************
	else//光滑
	{
		if( m_nPoints < 3 )
			PolySpline3(dc.m_hDC, m_points, m_nPoints);	
		else
			PolySpline3(dc.m_hDC, m_points, m_nPoints, TRUE);
	}
	
  	if( m_nPoints == 2 )
  	{
  		for(int i = 0 ; i < m_nPoints; i++)
  			dc.Rectangle(m_points[i].x - size.cx, m_points[i].y - size.cy,	m_points[i].x + size.cx, m_points[i].y + size.cy);
  	}
  	else
 	{
 		for(int i = 0 ; i < m_nPoints-1; i++)
 			dc.Rectangle(m_points[i].x - size.cx, m_points[i].y - size.cy,	m_points[i].x + size.cx, m_points[i].y + size.cy);
 	}

	
	dc.SelectObject(pOldBrush);	
	
	dc.SetROP2(mode);
}

void CRgnClipping::DrawMoveState(CGeoMapView* pView)
{
	CClientDC dc(pView);
	pView->OnPrepareDC(&dc);

	int mode = dc.SetROP2(R2_NOTXORPEN);
	DrawTracker(&dc, pView);

	CPen pen;
	pen.CreatePen(PS_SOLID, 0, RGB(255, 0, 0));
	CPen *pOldPen = dc.SelectObject(&pen);
	CBrush *pOldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);

	if (!m_bSmooth)//不光滑
		dc.Polygon(m_points, m_nPoints);
	//*******************
	else//光滑
		PolySpline3(dc.m_hDC, m_points, m_nPoints, TRUE);
		
	dc.SelectObject(pOldBrush);
	dc.SelectObject(pOldPen);
	dc.SetROP2(mode);
}

void CRgnClipping::DrawSizeState(CGeoMapView* pView)
{
	CClientDC dc(pView);
	pView->OnPrepareDC(&dc);

	int mode = dc.SetROP2(R2_NOTXORPEN);
	DrawTracker(&dc, pView);

	CPen pen;
	pen.CreatePen(PS_SOLID, 0, RGB(255, 0, 0));
	CPen *pOldPen = dc.SelectObject(&pen);
	CBrush *pOldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);

	if (!m_bSmooth)//不光滑
		dc.Polygon(m_points, m_nPoints);
	//*******************
	else//光滑
		PolySpline3(dc.m_hDC, m_points, m_nPoints, TRUE);
		
	dc.SelectObject(pOldBrush);
	dc.SelectObject(pOldPen);
	dc.SetROP2(mode);
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::MoveHandleTo
// 返回  : void
// 参数  : int nHandle 手柄索引
// 参数  : CPoint point 目前逻辑位置
// 参数  : CGeoMapView *pView
// 说明  : 移动某手柄
//
void CRgnClipping::MoveHandleTo(int nHandle, CPoint point, CGeoMapView *pView)
{
	ASSERT_VALID(this);
	
	if( m_bLocked )
		return;
	
	ASSERT(nHandle >= 1 && nHandle <= m_nPoints);
	CPoint pt;
	pt = m_points[nHandle - 1];
	if (pt == point)
		return;
	
	if (pView == NULL)
		Invalidate();
	else
	{
		
		if( m_bEditing )
			DrawEditMode( pView );
		else if( m_bSizeChanging )
			DrawSizeState( pView );
		else if( m_bMoving )
			DrawMoveState( pView );
		else
			pView->InvalObj(this);	
	}
	
	m_points[nHandle - 1] = point;
	
	m_geoPoints[nHandle - 1] = CoordinateTransfer(point);

	RecalcBounds(pView);
	
	if (pView == NULL)
		Invalidate();
	else
	{
		if( m_bEditing )
			DrawEditMode( pView );
		else if( m_bSizeChanging )
			DrawSizeState( pView );
		else if( m_bMoving )
			DrawMoveState( pView );
		else
			pView->InvalObj(this);
	}

	m_pDocument->SetModifiedFlag();
}

void CRgnClipping::MoveTo(const CRect& position, CGeoMapView* pView )
{
	ASSERT_VALID(this);
	
	if( m_bLocked )
		return;
	
	CRect rect = m_rtPosition;
	if (rect == position)
		return;
	
	if (pView == NULL)
		Invalidate();
	else
	{
		if( m_bEditing )
			DrawEditMode( pView );
		else if( m_bSizeChanging )
			DrawSizeState( pView );
		else if( m_bMoving )
			DrawMoveState( pView );
		else
			pView->InvalObj(this);	
	}
	
	for (int i = 0; i < m_nPoints; i += 1)
	{
		m_points[i].x += position.left - rect.left;
		m_points[i].y += position.top - rect.top;
		m_geoPoints[i] = CoordinateTransfer(m_points[i]);
	}
	
	m_rtPosition = position;
	
	if (pView == NULL)
		Invalidate();
	else
	{
		if( m_bEditing )
			DrawEditMode( pView );
		else if( m_bSizeChanging )
			DrawSizeState( pView );
		else if( m_bMoving )
			DrawMoveState( pView );
		else
			pView->InvalObj(this);
	}

	CPoint pt = m_rtPosition.CenterPoint();
	
	// 转换为大地坐标
	m_geoCenterPoint = CoordinateTransfer(pt);
	m_pDocument->SetModifiedFlag();
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::OnEditProperties
// 返回  : void
// 参数  : CGeoMapView *pView
// 说明  : 编辑图素属性
//
BOOL CRgnClipping::OnEditProperties(CGeoMapView *pView)
{
	return FALSE;
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::Clone
// 返回  : CGeoDrawObj*
// 参数  : CGeoMapDoc *pDoc
// 说明  : 克隆图素并将图素加入到文档中
//
CGeoDrawObj* CRgnClipping::Clone(CDrawLayer *pLayer)
{
	ASSERT_VALID(this);
	
	return NULL;
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::Serialize
// 返回  : void
// 参数  : CArchive &ar
// 说明  : 串行化图素
//
void CRgnClipping::Serialize(CArchive &ar)
{
	CGeoDrawObj::Serialize(ar);
	
	ar.SerializeClass(RUNTIME_CLASS(CRgnClipping));

	if (ar.IsStoring())
	{
		ar << (WORD) m_nPoints;
		ar << (WORD) m_nAllocPoints;
		int i;
		for ( i = 0;i< m_nPoints; i++)
			ar << m_points[i];
		for ( i = 0;i< m_nPoints; i++)
			ar << m_geoPoints[i];	
		ar << m_bSmooth;
	}
	else
	{
		// get the document back pointer from the archive

		int nVersion = ar.GetObjectSchema(); // 获取版本信息
		switch( nVersion )
		{
		case 0:
			{
				m_pDocument = (CGeoMapDoc*)ar.m_pDocument;
				ASSERT_VALID(m_pDocument);
				ASSERT_KINDOF(CGeoMapDoc, m_pDocument);

				WORD wTemp;	
				ar >> wTemp; m_nPoints = wTemp;
				ar >> wTemp; m_nAllocPoints = wTemp;
				m_points = new CPoint[m_nAllocPoints];
				m_geoPoints	= new CPoint2D[m_nAllocPoints];
				int i;
				for ( i = 0;i < m_nPoints; i++)
					ar >> m_points[i];
				for ( i = 0;i < m_nPoints; i++)
					ar >> m_geoPoints[i];
				ar >> m_bSmooth;
			}
			break;
		case 1:
			break;
		case 2:
			break;
		default:
			break;
		}
		
	}
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::GetHandleCount
// 返回  : int
// 说明  : 获取手柄总数
//
int CRgnClipping::GetHandleCount()
{
	ASSERT_VALID(this);
	return m_nPoints;
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::GetHandle
// 返回  : CPoint
// 参数  : int nHandle
// 说明  : 获取某手柄的中心逻辑坐标
//
CPoint CRgnClipping::GetHandle(int nHandle)
{
	ASSERT_VALID(this);
	
	ASSERT(nHandle >= 1 && nHandle <= m_nPoints);
	CPoint point;
	point = m_points[nHandle - 1];
	return point;
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::GetHandleCursor
// 返回  : HCURSOR
// 参数  : int nHandle
// 说明  : 获取某手柄的鼠标指针
//
HCURSOR CRgnClipping::GetHandleCursor(int nHandle)
{
	return AfxGetApp()->LoadCursor(IDC_CURSOR_SIZEALL);
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::HitTestHanle
// 返回  : int
// 参数  : CPoint point是逻辑坐标
// 参数  : CGeoMapView *pView
// 参数  : BOOL bSelected 如果已经选中，返回是否点中某个手柄
// 说明  : 否则，如果返回1选中该图素，否则没有点中该图素
//
int CRgnClipping::HitTestHandle(CPoint point, CGeoMapView *pView, BOOL bSelected)
{
	ASSERT_VALID(this);
	ASSERT(pView != NULL);
	
	if (bSelected) // 如果该图素已经被选中了
	{
		int nHandleCount = GetHandleCount(); // 手柄总数
		for (int nHandle = 1; nHandle <= nHandleCount; nHandle += 1)
		{
			// 返回某手柄的逻辑范围
			CRect rc = GetHandleRect(nHandle,pView);
			rc.NormalizeRect();
			if( rc.PtInRect(point))
				return nHandle;
		}
	}
	else // 否则, 测试是否落在图素范围内了
	{		
		if( IsPoint(point,pView) )
			return 1;
	}
	return 0;
}

// 当比例尺发生改变时
void CRgnClipping::OnScaleChanged()
{
	CGeoDrawObj::OnScaleChanged();

	for (int i = 0; i < m_nPoints; i += 1)
		m_points[i] = CoordinateTransfer(m_geoPoints[i]);
	
	RecalcBounds();
}

// 当发生旋转时
void CRgnClipping::OnAngleChanged()
{

}
// 当发生坐标原点有改变时
void CRgnClipping::OnCoordOrigChanged()
{

}

void CRgnClipping::AddPoint(const CPoint2D &point, CGeoMapView *pView)
{
	ASSERT_VALID(this);
	
	if( m_bLocked ) 
		return;
	
	if (m_nPoints == m_nAllocPoints)
	{
		CPoint* newPoints = new CPoint[m_nAllocPoints + 10];
		
		if (m_points != NULL )
		{
			memcpy(newPoints, m_points, sizeof(CPoint) * m_nAllocPoints);
			delete[] m_points;
		}
		
		CPoint2D* newGeoPoints = new CPoint2D[m_nAllocPoints + 10];
		if (m_geoPoints != NULL )
		{
			memcpy(newGeoPoints, m_geoPoints, sizeof(CPoint2D) * m_nAllocPoints);
			delete[] m_geoPoints;
		}
		
		m_points = newPoints;
		m_geoPoints = newGeoPoints;
		
		m_nAllocPoints += 10;	
	}
	
	if (m_nPoints == 0 || m_geoPoints[m_nPoints - 1] != point)
	{
		if( m_bEditing )
			DrawEditMode(pView);
		
		m_geoPoints[m_nPoints] = point;
		
		CPoint pt = CoordinateTransfer(point);
		m_points[m_nPoints] = pt;
		
		m_nPoints++;
		
		if( m_bEditing )
			DrawEditMode(pView);
		
		m_pDocument->SetModifiedFlag();
	}
}

void CRgnClipping::AddPoint(const CPoint &point, CGeoMapView *pView)
{
	ASSERT_VALID(this);


	if (m_nPoints == m_nAllocPoints)
	{
		CPoint* newPoints = new CPoint[m_nAllocPoints + 10];
		
		if (m_points != NULL )
		{
			memcpy(newPoints, m_points, sizeof(CPoint) * m_nAllocPoints);
			delete[] m_points;
		}

		CPoint2D* newGeoPoints = new CPoint2D[m_nAllocPoints + 10];
		if (m_geoPoints != NULL )
		{
			memcpy(newGeoPoints, m_geoPoints, sizeof(CPoint2D) * m_nAllocPoints);
			delete[] m_geoPoints;
		}

		m_points = newPoints;
		m_geoPoints = newGeoPoints;

		m_nAllocPoints += 10;	
	}

	if (m_nPoints == 0 || m_points[m_nPoints - 1] != point)
	{
		if( m_bEditing )
			DrawEditMode(pView);
		
		m_points[m_nPoints] = point;

		CPoint2D geoPoint = CoordinateTransfer(point);
		m_geoPoints[m_nPoints] = geoPoint;

		m_nPoints++;
		
		if( m_bEditing )
			DrawEditMode(pView);

		m_pDocument->SetModifiedFlag();
	}
}

BOOL CRgnClipping::RecalcBounds(CGeoMapView *pView)
{
	ASSERT_VALID(this);

	if (m_nPoints == 0)
		return FALSE;
	if( pView == NULL )
	{
		POSITION pos = m_pDocument->GetFirstViewPosition();
		pView = (CGeoMapView*)m_pDocument->GetNextView(pos);
	}
	CRect bounds(m_points[0], CSize(1, 1));
	if( m_nPoints == 2)
	{
		bounds.left = m_points[0].x;
		bounds.right =m_points[1].x;
		bounds.top = m_points[0].y;
		bounds.bottom = m_points[1].y;
	}
	else
	{
		if( m_bSmooth )
		{		
			CClientDC dc(pView);
			pView->OnPrepareDC(&dc);
			dc.BeginPath();

			PolySpline3(dc.m_hDC, m_points, m_nPoints, TRUE);
			
			dc.EndPath();

			CRgn rgn;
			if(rgn.CreateFromPath(&dc))
			{
				CRect rt;
				rgn.GetRgnBox(&bounds);
				rgn.DeleteObject();
				pView->ClientToDoc(bounds);
			}
			
		}
		else
		{
			for (int i = 1; i < m_nPoints; ++i)
			{
				if (m_points[i].x < bounds.left)
					bounds.left = m_points[i].x;
				if (m_points[i].x > bounds.right)
					bounds.right = m_points[i].x;
				if (m_points[i].y < bounds.top)
					bounds.top = m_points[i].y;
				if (m_points[i].y > bounds.bottom)
					bounds.bottom = m_points[i].y;
			}
		}
	}

	if (bounds == m_rtPosition)
		return FALSE;
	
	m_rtPosition = bounds;
	
	CPoint pt = m_rtPosition.CenterPoint();
	
	// 转换为大地坐标
	m_geoCenterPoint = CoordinateTransfer(pt);
	
	return TRUE;
}

BOOL CRgnClipping::IsPoint(CPoint point, CGeoMapView *pView)
{
	CRect cliRect;
	pView->GetClientRect(cliRect);
	pView->ClientToDoc(cliRect);

	if( !Intersects(cliRect) ) //如果该图素不在当前窗口中则不显示
		return FALSE;

	CClientDC dc(pView);
	pView->OnPrepareDC(&dc);
	
	CSize size(20,20);
	dc.DPtoLP(&size);
	dc.LPtoDP(&point);
	
	CPen pen1(PS_GEOMETRIC | PS_SOLID | PS_JOIN_MITER | PS_ENDCAP_FLAT , size.cx , RGB(0, 0, 0) );
	
	CPen *pOldPen = dc.SelectObject(&pen1);	
	
	dc.BeginPath();	
	
	if (!m_bSmooth)
		dc.Polygon(m_points, m_nPoints);
	else
	{
		PolySpline3(dc.m_hDC, m_points, m_nPoints, TRUE);
	}
	
	dc.EndPath();
	
	dc.SelectObject(pOldPen);
	pen1.DeleteObject();
		 
	if(m_rgn.m_hObject != NULL )
		m_rgn.DeleteObject();

	m_rgn.CreateFromPath(&dc);
	
	if( !m_rgn.m_hObject )
		return FALSE;
	
	if( m_rgn.PtInRegion(point) )
		return TRUE;
	else
		return FALSE;
}

BOOL CRgnClipping::IsHitOnBorder(CPoint point, CGeoMapView *pView)
{
	CRect cliRect;
	pView->GetClientRect(cliRect);
	pView->ClientToDoc(cliRect);
	if( !Intersects(cliRect) ) //如果该图素不在当前窗口中则不显示
		return FALSE;
	
	CClientDC dc(pView);
	pView->OnPrepareDC(&dc);
	
	CSize size(20,20);
	dc.DPtoLP(&size);
	dc.LPtoDP(&point);
	
	CPen pen1(PS_GEOMETRIC | PS_SOLID | PS_JOIN_MITER | PS_ENDCAP_FLAT , size.cx , RGB(0, 0, 0) );
	
	CPen *pOldPen = dc.SelectObject(&pen1);	
	
	dc.BeginPath();	
	
	if (!m_bSmooth)
	{
		dc.Polygon(m_points, m_nPoints);
	}
	else
	{
		PolySpline3(dc.m_hDC, m_points, m_nPoints, TRUE);
	}
	
	dc.EndPath();
	
	dc.SelectObject(pOldPen);
	pen1.DeleteObject();
	
	dc.WidenPath();
	
	CRgn rgn;
	
	rgn.CreateFromPath(&dc);
	
	if( !rgn.m_hObject )
		return FALSE;
	
	if( rgn.PtInRegion(point) )
		return TRUE;
	else
		return FALSE;
}
//////////////////////////////////////////////////////////////////////////
// 删除点
// nIndex 点序号 第一点的序号是1
// 

void CRgnClipping::DelPoint( int nIndex, CGeoMapView *pView)
{
	DeletePoint( nIndex, pView);
}

//////////////////////////////////////////////////////////////////////////
// 删除点
// nIndex 点序号 第一点的序号是1
// 
void CRgnClipping::DeletePoint( int nIndex, CGeoMapView *pView)
{	
	for(int i = nIndex ; i < m_nPoints; i++)
	{
		m_points[i-1] = m_points[i]; 
		m_geoPoints[i-1] = m_geoPoints[i]; 
	}
	
	m_nPoints --;	
	
	pView->InvalObj(this);

	RecalcBounds(pView);

	pView->InvalObj(this);
	
	m_pDocument->SetModifiedFlag();
}

void CRgnClipping::DelPoint( const CPoint &point, CGeoMapView *pView)
{
	int index[2];
	
	CPoint2D pt(point.x, point.y);
	CPoint2D retPt;

	CPoint2D *points = new CPoint2D [m_nPoints];
	if( points == NULL )
		return;
	
	for (int i=0;i<m_nPoints;i++)
		points[i] = m_points[i];

	ptopointset(m_nPoints,points,pt,retPt,index);

	delete[]points;
	if( index[0]==-1
		&& index[1]==-1)
		return;
	
	if( index[0] == -1 )
	{
		if(index[1] == 0 ) 
			index[1] = 1;
	}
	else
	{
		CPoint2D p1,p2;
		p1 = m_points[index[0]];		
		p2 = m_points[index[1]];
		
		double d1 = dist(p1,retPt);
		double d2 = dist(p2,retPt);
		
		if( d1<d2)
			index[1] = index[0];
		
		index[1] +=1;
	}

	DelPoint(index[1],pView);
}

void CRgnClipping::MovePointTo(const CPoint &point, CGeoMapView *pView)
{
	int index[2];
	
	CPoint2D pt(point.x, point.y);
	CPoint2D retPt;
	CPoint2D *points = new CPoint2D [m_nPoints];
	if( points == NULL )
		return;
	
	for (int i=0;i<m_nPoints;i++)
		points[i] = m_points[i];

	ptopointset(m_nPoints,points,pt,retPt,index);

	delete[]points;

	if( index[0]==-1&&index[1]==-1)
		return;

	if( index[0] == -1 )
	{
		if(index[1] == 0 ) 
			index[1] = 1;	
	}
	else
	{
		CPoint2D p1,p2;
		p1 = m_points[index[0]];		
		p2 = m_points[index[1]];

		double d1 = dist(p1,retPt);
		double d2 = dist(p2,retPt);
		
		if( d1<d2)
			index[1] = index[0];

		index[1] +=1;
	}
	
	MoveHandleTo(index[1], point,pView);
}

// 
void CRgnClipping::InsertPoint(const CPoint &point, CGeoMapView *pView)
{
	int index[2];
	
	CPoint2D pt(point.x, point.y);
	CPoint2D retPt;
	CPoint2D *points = new CPoint2D [m_nPoints];
	if( points == NULL )
		return;
	
	for (int i=0;i<m_nPoints;i++)
		points[i] = m_points[i];

	ptopointset(m_nPoints,points,pt,retPt,index);

	delete[]points;

	if( index[0]==-1&&index[1]==-1)
		return;
	
	InsertPoint(point, index[1]+1, pView);
}

void CRgnClipping::InsertPoint(const CPoint &point, int nIndex, CGeoMapView *pView)
{
	if (m_nPoints == m_nAllocPoints)
	{
		CPoint* newPoints = new CPoint[m_nAllocPoints + 10];
		
		if (m_points != NULL)
		{
			memcpy(newPoints, m_points, sizeof(CPoint) * m_nAllocPoints);
			delete[] m_points;
		}

		CPoint2D* newGeoPoints = new CPoint2D[m_nAllocPoints + 10];
		if (m_geoPoints != NULL )
		{
			memcpy(newGeoPoints, m_geoPoints, sizeof(CPoint2D) * m_nAllocPoints);
			delete[] m_geoPoints;
		}

		m_points = newPoints;
		m_geoPoints = newGeoPoints;

		m_nAllocPoints += 10;
	}
	
	for(int i = m_nPoints ; i >= nIndex ; i--)
	{
		m_points[i] = m_points[i-1]; 
		m_geoPoints[i] = m_geoPoints[i-1]; 
	}
	
	m_points[nIndex-1] = point;

	CPoint2D geoPoint = CoordinateTransfer(point);
	m_geoPoints[nIndex-1] = geoPoint;
	
	m_nPoints ++;
	
	pView->InvalObj(this);

	RecalcBounds(pView);

	pView->InvalObj(this);
	
	m_pDocument->SetModifiedFlag();
}

void CRgnClipping::DrawLine(CDC *pDC, CGeoMapView *pView)
{
	CBrush* pOldBrush;
	
	pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
	
	CSize size(3,3);
	pDC->DPtoLP(&size);
	CPen pen(PS_SOLID, size.cx, RGB(255,0,0));

// 	CPen pen;
// 	LOGBRUSH LogBrush;
// 	
//     LogBrush.lbColor = RGB(255,0,0);
// 	
//     LogBrush.lbStyle = PS_SOLID;
// 	
//     pen.CreatePen( PS_COSMETIC | PS_ALTERNATE, 1, &LogBrush, 0, NULL );

	CPen *pOldPen = pDC->SelectObject(&pen);
	if (!m_bSmooth)//不光滑
		pDC->Polygon(m_points, m_nPoints);
	//*******************
	else//光滑
		PolySpline3(pDC->m_hDC, m_points, m_nPoints, TRUE);
	
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
}


//////////////////////////////////////////////////////////////////////////
// 获取距point处最近的在曲线上的点索引号
// 返回最近点的坐标
CPoint CRgnClipping::GetNearByIndex(const CPoint &point, int *iIndex)
{
	CPoint2D *points;
	int nPoints;
	if( m_bSmooth )
	{
		int nCounts;
		LPPOINT lppt;
		
		points = new CPoint2D[m_nPoints];
		if(points == NULL)
		{
			iIndex[0] = -1;
			iIndex[1] = -1;
			return CPoint(0,0);
		}

		int i;
		for ( i=0;i<m_nPoints;i++)
			points[i] = m_points[i];
		
		lppt = GetSPPoints(points,m_nPoints,nCounts,TRUE);
		
		delete []points;

		points = new CPoint2D[nCounts];
		if(points == NULL)
		{
			iIndex[0] = -1;
			iIndex[1] = -1;
			return CPoint(0,0);
		}

		for (i=0;i<nCounts;i++)
			points[i] = lppt[i];

		delete lppt;
		
		nPoints = nCounts;
	}
	else
	{
		if( m_nPoints > 2 )
			points = new CPoint2D[m_nPoints+1];
		else
			points = new CPoint2D[m_nPoints];
		
		if(points == NULL )
		{
			iIndex[0] = -1;
			iIndex[1] = -1;
			return CPoint(0,0);
		}
		
		for (int i=0; i<m_nPoints; i++)
		{
			points[i].x = m_points[i].x;
			points[i].y = m_points[i].y;
		}

		nPoints = m_nPoints;
		if( m_nPoints > 2 )
		{
			points[m_nPoints] = points[0];
			nPoints ++;
		}
	}

	CPoint2D pt(point.x,point.y), retPt;


	ptopointset(nPoints,points, pt, retPt, iIndex);

	delete [] points;

	if( iIndex[0] == -1 )
	{
		if(iIndex[1] != 0 ) 
			iIndex[1] -= 1;
	}

	return CPoint(retPt.x, retPt.y);
}

//////////////////////////////////////////////////////////////////////////
// 获取图形路径的所有点
// 
int CRgnClipping::GetPathPoint(LPPOINT &lppt)
{
	int nPoints;
	if( m_bSmooth )
	{
		int nCounts;

		CPoint2D *points = new CPoint2D[m_nPoints];
		if(lppt == NULL)
		{			
			return -1;
		}
		
		for (int i=0;i<m_nPoints;i++)
			points[i] = m_points[i];

		lppt = GetSPPoints(points,m_nPoints,nCounts,TRUE);
		
		delete [] points;

		nPoints = nCounts;
	}
	else
	{
		if( m_nPoints > 2 )
			lppt = new CPoint[m_nPoints+1];
		else
			lppt = new CPoint[m_nPoints];
		
		if(lppt == NULL )
			return -1;
		
		for (int i=0; i<m_nPoints; i++)
		{
			lppt[i].x = m_points[i].x;
			lppt[i].y = m_points[i].y;
		}
		
		nPoints = m_nPoints;

		if( m_nPoints > 2 )
		{
			lppt[m_nPoints] = lppt[0];
			nPoints ++;
		}
	}

	return nPoints;
}

BOOL CRgnClipping::GetClipRgn(CDC *pDC, CRgn &rgn)
{
	if( m_nPoints < 3 )
		return FALSE;

	pDC->BeginPath();	
	
	if (!m_bSmooth)
		pDC->Polygon(m_points, m_nPoints);
	else
	{
		PolySpline3(pDC->m_hDC, m_points, m_nPoints, TRUE);
	}
	
	pDC->EndPath();
	
	rgn.CreateFromPath(pDC);

	if(rgn.m_hObject == NULL )
		return FALSE;

	return TRUE;
}

void CRgnClipping::SelectClipRgn(CDC *pDC)
{
	if( m_nPoints < 3 )
		return;

	pDC->BeginPath();
	
	if (!m_bSmooth)
		pDC->Polygon(m_points, m_nPoints);
	else
		PolySpline3(pDC->m_hDC, m_points, m_nPoints, TRUE);	
	
	pDC->EndPath();
	
	pDC->SelectClipPath(RGN_AND);
	
	return;
}


void CRgnClipping::DrawTracker(CDC *pDC, CGeoMapView *pView)
{
	ASSERT_VALID(this);
	
	switch (m_trackerState)
	{
	case normal:
		break;
		
	case selected:
		{			
			CSize size(3,3);
			pDC->DPtoLP(&size);
			int nHandleCount = GetHandleCount();
			for (int nHandle = 1; nHandle <= nHandleCount; nHandle += 1)
			{
				CPoint handle = GetHandle(nHandle);
				if( !m_bLocked )
				{
					if( m_bSizeChanging || m_bMoving )
					{
						CPen pen(PS_SOLID,0,RGB(0,0,0));
						CPen *pOldPen = pDC->SelectObject(&pen);
						CBrush b(RGB(0,0,0));
						CBrush *pOldBrush = pDC->SelectObject(&b);
						pDC->Rectangle(handle.x - size.cx, handle.y - size.cy, handle.x + size.cx, handle.y + size.cy);
						pDC->SelectObject(pOldPen);
						pDC->SelectObject(pOldBrush);
					}
					else
					{
						CPen pen(PS_SOLID,0,RGB(0,0,0));
						CPen *pOldPen = pDC->SelectObject(&pen);
						CBrush b(RGB(255,255,255));
						CBrush *pOldBrush = pDC->SelectObject(&b);
						pDC->Rectangle(handle.x - size.cx, handle.y - size.cy, handle.x + size.cx, handle.y + size.cy);
						pDC->SelectObject(pOldPen);
						pDC->SelectObject(pOldBrush);
					}
				}
				else
				{
					CPen pen(PS_SOLID,0,RGB(0,0,0));
					CPen *pOldPen = pDC->SelectObject(&pen);
					CBrush b(RGB(255,0,0));
					CBrush *pOldBrush = pDC->SelectObject(&b);
					pDC->Rectangle(handle.x - size.cx, handle.y - size.cy, handle.x + size.cx, handle.y + size.cy);
					pDC->SelectObject(pOldPen);
					pDC->SelectObject(pOldBrush);
				}
			}

			// 没有可以移动的手柄
			if( nHandleCount == 0 )
			{
				for (int nCorner = 1; nCorner <= 4; nCorner += 1)
				{
					CRect rt = GetCornerRect(nCorner,pView);
					if( !m_bLocked )
					{
						if( m_bSizeChanging || m_bMoving )
						{
							CPen pen(PS_SOLID,0,RGB(0,0,0));
							CPen *pOldPen = pDC->SelectObject(&pen);
							CBrush b(RGB(0,0,0));
							CBrush *pOldBrush = pDC->SelectObject(&b);
							pDC->Ellipse(rt);
							pDC->SelectObject(pOldPen);
							pDC->SelectObject(pOldBrush);
						}
						else
						{
							CPen pen(PS_SOLID,0,RGB(0,0,0));
							CPen *pOldPen = pDC->SelectObject(&pen);
							CBrush b(RGB(255,255,255));
							CBrush *pOldBrush = pDC->SelectObject(&b);
							pDC->Ellipse(rt);
							pDC->SelectObject(pOldPen);
							pDC->SelectObject(pOldBrush);
						}
					}
					else
					{
						CPen pen(PS_SOLID,0,RGB(0,0,0));
						CPen *pOldPen = pDC->SelectObject(&pen);
						CBrush b(RGB(255,0,0));
						CBrush *pOldBrush = pDC->SelectObject(&b);
						pDC->Ellipse(rt);
						pDC->SelectObject(pOldPen);
						pDC->SelectObject(pOldBrush);
					}
				}
			}
		}
		break;
	case active:
		{			
			CSize size(3,3);
			pDC->DPtoLP(&size);
			int nHandleCount = GetHandleCount();
			for (int nHandle = 1; nHandle <= nHandleCount; nHandle += 1)
			{
				CPoint handle = GetHandle(nHandle);
				if( !m_bLocked )
				{
					if( m_bSizeChanging || m_bMoving )
					{
						CPen pen(PS_SOLID,0,RGB(0,0,0));
						CPen *pOldPen = pDC->SelectObject(&pen);
						CBrush b(RGB(0,0,0));
						CBrush *pOldBrush = pDC->SelectObject(&b);
						pDC->Rectangle(handle.x - size.cx, handle.y - size.cy, handle.x + size.cx, handle.y + size.cy);
						pDC->SelectObject(pOldPen);
						pDC->SelectObject(pOldBrush);
					}
					else
					{
						//pDC->PatBlt(handle.x - size.cx, handle.y - size.cy, size.cx*2+1, size.cy*2+1, BLACKNESS);
						CPen pen(PS_SOLID,0,RGB(0,0,0));
						CPen *pOldPen = pDC->SelectObject(&pen);
						CBrush b(RGB(0,0,0));
						CBrush *pOldBrush = pDC->SelectObject(&b);
						pDC->Rectangle(handle.x - size.cx, handle.y - size.cy, handle.x + size.cx, handle.y + size.cy);
						pDC->SelectObject(pOldPen);
						pDC->SelectObject(pOldBrush);
					}
				}
				else
				{
					CPen pen(PS_SOLID,0,RGB(0,0,0));
					CPen *pOldPen = pDC->SelectObject(&pen);
					CBrush b(RGB(255,0,0));
					CBrush *pOldBrush = pDC->SelectObject(&b);
					pDC->Rectangle(handle.x - size.cx, handle.y - size.cy, handle.x + size.cx, handle.y + size.cy);
					pDC->SelectObject(pOldPen);
					pDC->SelectObject(pOldBrush);
				}
			}
			
			// 没有可以移动的手柄
			if( nHandleCount == 0 )
			{
				for (int nCorner = 1; nCorner <= 4; nCorner += 1)
				{
					CRect rt = GetCornerRect(nCorner,pView);
					if( !m_bLocked )
					{
						if( m_bSizeChanging || m_bMoving )
						{
							CPen pen(PS_SOLID,0,RGB(0,0,0));
							CPen *pOldPen = pDC->SelectObject(&pen);
							CBrush b(RGB(0,0,0));
							CBrush *pOldBrush = pDC->SelectObject(&b);
							pDC->Ellipse(rt);
							pDC->SelectObject(pOldPen);
							pDC->SelectObject(pOldBrush);
						}
						else
						{
							//pDC->PatBlt(handle.x - size.cx, handle.y - size.cy, size.cx*2+1, size.cy*2+1, BLACKNESS);
							CPen pen(PS_SOLID,0,RGB(0,0,0));
							CPen *pOldPen = pDC->SelectObject(&pen);
							CBrush b(RGB(0,0,0));
							CBrush *pOldBrush = pDC->SelectObject(&b);
							pDC->Ellipse(rt);
							pDC->SelectObject(pOldPen);
							pDC->SelectObject(pOldBrush);
						}
					}
					else
					{
						CPen pen(PS_SOLID,0,RGB(0,0,0));
						CPen *pOldPen = pDC->SelectObject(&pen);
						CBrush b(RGB(255,0,0));
						CBrush *pOldBrush = pDC->SelectObject(&b);
						pDC->Ellipse(rt);
						pDC->SelectObject(pOldPen);
						pDC->SelectObject(pOldBrush);
					}
				}
			}
			
		}
		break;
	}
	
	{ // 画区域中心点
		
		CSize size(3,3), size1(4,4);;
		pDC->DPtoLP(&size);
		pDC->DPtoLP(&size1);
		
		CPoint pt = m_rtPosition.CenterPoint();
		pDC->LPtoDP(&pt);
		pDC->DPtoLP(&pt);
		
		CPen pen(PS_SOLID, 0, RGB(255,0,0));
		CPen *pOldPen = pDC->SelectObject(&pen);
		
		pDC->MoveTo(pt.x,pt.y + size.cy);
		pDC->LineTo(pt.x,pt.y - size1.cy);
		pDC->MoveTo(pt.x+size.cx, pt.y);
		pDC->LineTo(pt.x-size1.cx, pt.y);
		
		pDC->SelectObject(pOldPen);		
	}
	{ // 画区域框
		
		CRect rect = m_rtPosition;
		CPen pen;
		LOGBRUSH LogBrush;
		
		LogBrush.lbColor = RGB(0, 255, 255);
		
		LogBrush.lbStyle = PS_SOLID;
		
		pen.CreatePen( PS_COSMETIC | PS_ALTERNATE, 1, &LogBrush, 0, NULL );
		CPen *pOldPen = pDC->SelectObject(&pen);
		CBrush *pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
		pDC->Rectangle(rect);
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);
	}
}
