// GeoLineObj.cpp: implementation of the CGeoLineObj class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GeoLineObj.h"
#include "GeoMapView.h"
#include "DlgLineSet.h"
#include "CmdGeoDrawObj.h"

#include "RgnClipping.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


IMPLEMENT_SERIAL(CGeoLineObj, CGeoDrawObj, VERSIONABLE_SCHEMA|0)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeoLineObj::CGeoLineObj()
{
	SetObjName(_T("直线"));
}

CGeoLineObj::CGeoLineObj(CDrawLayer* pLayer)
:CGeoDrawObj(pLayer)
{
	SetObjName(_T("直线"));
}

CGeoLineObj::CGeoLineObj(const CRect& position, CDrawLayer* pLayer)
:CGeoDrawObj(position,pLayer)
{
	SetObjName(_T("直线"));
}

CGeoLineObj::~CGeoLineObj()
{

}

void CGeoLineObj::Draw(CDC *pDC, CGeoMapView *pView)
{
	ASSERT_VALID(this);
	
	if( !m_bShow )
		return;
	
	if( !pDC->IsPrinting() )
	{
		CRect cliRect;
		pView->GetClientRect(cliRect);
		pView->ClientToDoc(cliRect);
		if( !Intersects(cliRect) ) //如果该图素不在当前窗口中则不显示
			return ;
	}
	
	for (long i=0;i<2; i++)
	{
		m_points[i] = CoordinateTransfer(m_geoPoints[i]);
	}

	int nSaved = pDC->SaveDC();
	pDC->SetBkMode(TRANSPARENT);          //设定显示方式
	
	DrawLine(pDC, pView, CGeoLineObj::MIDST);

	DrawArrow(pDC, pView);
	
	pDC->RestoreDC(nSaved);
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoLineObj::OnEditProperties
// 返回  : void
// 参数  : CGeoMapView *pView
// 说明  : 编辑图素属性
//
BOOL CGeoLineObj::OnEditProperties(CGeoMapView *pView)
{
	ASSERT_VALID(this);
	if( m_bLocked )
		return FALSE;

	CDlgLineSet dlg;
	dlg.eLine = m_eLine;
	
	if(dlg.DoModal() == IDOK)
	{
		m_eLine = dlg.eLine;
		pView->InvalObj(this);
		m_pDocument->SetModifiedFlag();
		return TRUE;
	}

	return FALSE;
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::Clone
// 返回  : CGeoDrawObj*
// 参数  : CGeoMapDoc *pDoc
// 说明  : 克隆图素并将图素加入到文档中
//
CGeoDrawObj* CGeoLineObj::Clone(CDrawLayer *pLayer)
{
	ASSERT_VALID(this);
	
	CGeoLineObj* pClone = new CGeoLineObj(m_rtPosition,pLayer);
	pClone->m_rtPosition	=	m_rtPosition;
	pClone->m_bLocked		=	m_bLocked;			// 锁定图素
	pClone->m_bShow			=	m_bShow;			// 是否显示
	pClone->m_strID			=	newGUID();			// 新的ID
	pClone->m_ptAnchors.Copy(m_ptAnchors);			// 锚点

	pClone->m_pDocument		=	m_pDocument;		// 文档
	pClone->m_geoCenterPoint		=	m_geoCenterPoint;


	pClone->m_eLine				= m_eLine;							// 线属性
	pClone->m_points[0]			= m_points[0];
	pClone->m_points[1]			= m_points[1];
	
	pClone->m_geoPoints[0]		= CoordinateTransfer(pClone->m_points[0]);
	pClone->m_geoPoints[1]		= CoordinateTransfer(pClone->m_points[1]);
	
	ASSERT_VALID(pClone);	
	
	if (pClone != NULL)
	{
		m_pDocument->Do(new CCmdAddDrawObj(m_pDocument, pClone), TRUE);
		//pLayer->Add(pClone);
	}
	
	return pClone;
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::Serialize
// 返回  : void
// 参数  : CArchive &ar
// 说明  : 串行化图素
//
void CGeoLineObj::Serialize(CArchive &ar)
{
	CGeoDrawObj::Serialize(ar);
	
	ar.SerializeClass(RUNTIME_CLASS(CGeoLineObj));

	if (ar.IsStoring())
	{
		ar << m_eLine;
		ar << m_points[0];
		ar << m_points[1];
		ar << m_geoPoints[0];
		ar << m_geoPoints[1];
	}
	else
	{
		// get the document back pointer from the archive

		int nVersion = ar.GetObjectSchema(); // 获取版本信息
		switch( nVersion )
		{
		case 0:
			{				
				ar >> m_eLine;
				ar >> m_points[0];
				ar >> m_points[1];
				ar >> m_geoPoints[0];
				ar >> m_geoPoints[1];
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
// 函数  : CGeoLineObj::HitTestHanle
// 返回  : int
// 参数  : CPoint point是逻辑坐标
// 参数  : CGeoMapView *pView
// 参数  : BOOL bSelected 如果已经选中，返回是否点中某个手柄
// 说明  : 否则，如果返回1选中该图素，否则没有点中该图素
//
int CGeoLineObj::HitTestHandle(CPoint point, CGeoMapView *pView, BOOL bSelected)
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
			if (point.x >= rc.left && point.x <= rc.right &&
				point.y >= rc.top && point.y <= rc.bottom)
				return nHandle;
		}
	}
	else // 否则, 测试是否落在图素范围内了
	{
		if(IsPoint(point,pView))
			return 1;
	}
	return 0;
}

// 当比例尺发生改变时
void CGeoLineObj::OnScaleChanged()
{
	CGeoDrawObj::OnScaleChanged();
	
	for (int i = 0; i < 2; i += 1)
		m_points[i] = CoordinateTransfer(m_geoPoints[i]);
	
	RecalcBounds();	
}

// 当发生旋转时
void CGeoLineObj::OnAngleChanged()
{
	Invalidate();
}

void CGeoLineObj::ReversePoints()
{
	CPoint pt;
	CPoint2D geoPt;

	pt = m_points[0];
	m_points[0] = m_points[1];
	m_points[1] = pt;

	geoPt = m_geoPoints[0];
	m_geoPoints[0] = m_geoPoints[1];
	m_geoPoints[1] = geoPt;
}

// 当发生坐标原点有改变时
void CGeoLineObj::OnCoordOrigChanged()
{
	Invalidate();
}

// 转换参数改变时
void CGeoLineObj::OnConvertParamChanged()
{
	for (int i=0; i<2; i++)
		m_geoPoints[i] = CoordinateTransfer(m_points[i]);
}

void CGeoLineObj::DrawLine(CDC *pDC, CGeoMapView *pView, PLACE place, BOOL bThin)//画线
{	
	CRect rect;
	rect = m_rtPosition;	

	if(place == INNER)//内
		rect.DeflateRect(10, 10);
	if(place == OUTER)
		rect.DeflateRect(-10, -10);
	
	CPen pen;
	CBrush* pOldBrush;
	CPen* pOldPen;	
	
	pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
	
	if( m_eLine.m_bShow )
	{
		if(! CreatePen(&pen, pDC, bThin) )
			return;
		pOldPen = pDC->SelectObject(&pen);
	}
	else
	{		
		pDC->SelectObject(pOldBrush);
		return;		
	}
	
	pDC->MoveTo(m_points[0]);
	pDC->LineTo(m_points[1]);

	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);	
}

BOOL CGeoLineObj::IsPoint(CPoint point, CGeoMapView *pView)
{
	if(!m_bShow)
		return FALSE;
	
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
	
	dc.Polyline(m_points, 2);	
	
	dc.EndPath();
	
	dc.SelectObject(pOldPen);
	pen1.DeleteObject();
	
	dc.WidenPath();
	
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

BOOL CGeoLineObj::CreatePen(CPen *pPen, CDC *pDC, BOOL bThin)
{
	
	int penStyle[7]=
	{
		//笔的固定类型值，为以下几种，其它自定
		PS_SOLID,//            0
			PS_DASH,//             1       /* -------  */
			PS_DOT,//              2       /* .......  */
			PS_DASHDOT,//          3       /* _._._._  */
			PS_DASHDOTDOT,//       4       /* _.._.._  */
			PS_INSIDEFRAME,//      5
			PS_NULL//             6
	};
	
	if( m_eLine.m_nPenStyle == 6 )//如果无则直接返回
		return FALSE;
	
	int nWide = m_eLine.m_nPenWide;
	
	if(!bThin) nWide *= 3;
	
	int Endcap;
	
	if ( m_eLine.m_bSquarePen )
		Endcap = PS_ENDCAP_FLAT;
	else
		Endcap = PS_ENDCAP_ROUND;
	
	CBrush brush;
	
	/*unsigned Type[8];
	int c = CDashLine::GetPattern(Type, m_eLine.m_bSquarePen, 5, m_eLine.m_nPenStyle);
	
	  DashLine.SetDC(pDC);
	  
		if( c ) 
		DashLine.SetPattern(Type, c);
	*/
	
	if(m_eLine.s_LineStyle.m_PatternFill.m_nRow != -1 && m_eLine.s_LineStyle.m_PatternFill.m_nCol != -1)
	{
		
		if(!m_eLine.s_LineStyle.CreateBrush(&brush, pDC))
			return FALSE;
		
		CDC MemDC;
		MemDC.CreateCompatibleDC(pDC);
		CBitmap bit;
		bit.CreateCompatibleBitmap(&MemDC, 8, 8);
		CBitmap *pOldBit = MemDC.SelectObject(&bit);
		CRect rect(0, 0, 8, 8);
		MemDC.FillRect(rect, &brush);
		MemDC.SelectObject(pOldBit);
		MemDC.DeleteDC();
		
		CBrush b;
		b.CreatePatternBrush(&bit);
		
		LOGBRUSH logbrush;
		b.GetLogBrush(&logbrush);
		
		
		if (!pPen->CreatePen(penStyle[m_eLine.m_nPenStyle] | PS_GEOMETRIC | PS_JOIN_MITER | Endcap, 
			nWide, 
			&logbrush) 
			)
			return FALSE;
	}	
	else
	{
		LOGBRUSH logbrush;
		logbrush.lbColor = m_eLine.s_LineStyle.m_PatternFill.m_crColourFore;
		logbrush.lbStyle = BS_SOLID;
		logbrush.lbHatch = NULL;
		
		if (!pPen->CreatePen(penStyle[m_eLine.m_nPenStyle] | PS_GEOMETRIC | PS_JOIN_MITER | Endcap, 
			nWide, 
			&logbrush) 
			)
			return FALSE;
	}
	
	return TRUE;	
}

void CGeoLineObj::CopyAttribute(CGeoDrawObj *pSrcObj)
{	
	CGeoLineObj *pObj = (CGeoLineObj*)pSrcObj;
	m_eLine = pObj->m_eLine;
	
	CGeoDrawObj::CopyAttribute(pSrcObj);
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::GetHandleCount
// 返回  : int
// 说明  : 获取手柄总数
//
int CGeoLineObj::GetHandleCount()
{
	ASSERT_VALID(this);
	return 2;
}


void CGeoLineObj::DrawEditMode(CGeoMapView* pView)
{
	CClientDC dc(pView);
	pView->OnPrepareDC(&dc);
	
	int mode = dc.SetROP2(R2_NOTXORPEN);
	
	CBrush *pOldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	
	CSize size(3,3);
	dc.DPtoLP(&size);
	CRect rect = m_rtPosition;
	
	dc.MoveTo(m_points[0]);
	dc.LineTo(m_points[1]);

	dc.Rectangle(m_points[0].x - size.cx, m_points[0].y - size.cy,	
		m_points[0].x + size.cx, m_points[0].y + size.cy);

	dc.Rectangle(m_points[1].x - size.cx, m_points[1].y - size.cy,	
		m_points[1].x + size.cx, m_points[1].y + size.cy);
	
	dc.SelectObject(pOldBrush);	
	
	dc.SetROP2(mode);
}

void CGeoLineObj::DrawMoveState(CGeoMapView* pView)
{
	CClientDC dc(pView);
	pView->OnPrepareDC(&dc);

	int mode = dc.SetROP2(R2_NOTXORPEN);

	CPen pen;
	pen.CreatePen(PS_SOLID, 0, RGB(255, 0, 0));
	CPen *pOldPen = dc.SelectObject(&pen);
	CBrush *pOldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
		
	CSize size(3,3);
	dc.DPtoLP(&size);

	dc.MoveTo(m_points[0]);
	dc.LineTo(m_points[1]);
	
	dc.Rectangle(m_points[0].x - size.cx, m_points[0].y - size.cy,	
		m_points[0].x + size.cx, m_points[0].y + size.cy);
	
	dc.Rectangle(m_points[1].x - size.cx, m_points[1].y - size.cy,	
		m_points[1].x + size.cx, m_points[1].y + size.cy);

	dc.SelectObject(pOldBrush);
	dc.SelectObject(pOldPen);
	dc.SetROP2(mode);
}

void CGeoLineObj::DrawSizeState(CGeoMapView* pView)
{
	CClientDC dc(pView);
	pView->OnPrepareDC(&dc);

	int mode = dc.SetROP2(R2_NOTXORPEN);

	CPen pen;
	pen.CreatePen(PS_SOLID, 0, RGB(255, 0, 0));
	CPen *pOldPen = dc.SelectObject(&pen);
	CBrush *pOldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);	
	
	CSize size(3,3);
	dc.DPtoLP(&size);
	
	dc.MoveTo(m_points[0]);
	dc.LineTo(m_points[1]);
	
	dc.Rectangle(m_points[0].x - size.cx, m_points[0].y - size.cy,	
		m_points[0].x + size.cx, m_points[0].y + size.cy);
	
	dc.Rectangle(m_points[1].x - size.cx, m_points[1].y - size.cy,	
		m_points[1].x + size.cx, m_points[1].y + size.cy);

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
void CGeoLineObj::MoveHandleTo(int nHandle, CPoint point, CGeoMapView *pView)
{
	ASSERT(nHandle >= 1);
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


void CGeoLineObj::MoveTo(const CRect& position, CGeoMapView* pView )
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
	
	for (int i = 0; i < 2; i += 1)
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
// 函数  : CGeoDrawObj::GetHandleCursor
// 返回  : HCURSOR
// 参数  : int nHandle
// 说明  : 获取某手柄的鼠标指针
//
HCURSOR CGeoLineObj::GetHandleCursor(int nHandle)
{
	return  AfxGetApp()->LoadCursor(IDC_CURSOR_SIZEALL);
	
// 	if (nHandle == 2)
// 		nHandle = 5;
// 	
// 	return CGeoDrawObj::GetHandleCursor(nHandle);
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::Intersects
// 返回  : BOOL TRUE相交 FALSE 不相交
// 参数  : const CRect &rect 逻辑坐标
// 说明  : 测试矩形范围是否与图素范围相交
//
BOOL CGeoLineObj::Intersects(const CRect &rect)
{
	ASSERT_VALID(this);
	
	CRect fixed = m_rtPosition;
	fixed.NormalizeRect();
	
	if( m_pParentLayer->m_rgnClipping.m_nPoints > 2
		&& m_pParentLayer->m_bClipping)
		fixed = fixed&m_pParentLayer->m_rgnClipping.m_rtPosition;
	
	if( fixed.IsRectEmpty() )
		fixed.DeflateRect(-1,-1,-1,-1);
	CRect rectT = rect;
	rectT.NormalizeRect();
	
	return !(rectT & fixed).IsRectEmpty();
}

void CGeoLineObj::DrawArrow(CDC *pDC, CGeoMapView *pView)
{
	if( m_eLine.m_nLineType ) return;//不是单线不画箭头

	CPoint point;//
	CPoint pt[4];//箭头区域

	CPen pen;
	pen.CreatePen(PS_SOLID, 0, m_eLine.s_LineStyle.m_PatternFill.m_crColourFore);

	CPen *pOldPen = pDC->SelectObject(&pen);

	CBrush b1(RGB(255, 255, 255));
	CBrush b2(m_eLine.s_LineStyle.m_PatternFill.m_crColourFore);

	CRgn rgn;	
		
	if( m_eLine.m_uBeginPointMode )
	{
		point.x = m_points[0].x;
		point.y = m_points[0].y;
		rgn.DeleteObject();

		switch(m_eLine.m_uBeginPointMode)
		{
		case 1:								//箭头
			{
				CalcArrowRgn(pt, FALSE);
				CPoint p[3];
				p[0] = point;
				p[1] = pt[2];
				p[2] = pt[3];				
				rgn.CreatePolygonRgn(p, 3, ALTERNATE);
				pDC->FillRgn(&rgn, &b2);
				pDC->MoveTo(point);
				pDC->LineTo(pt[2]);
				pDC->LineTo(pt[3]);
				pDC->LineTo(point);			
			}
			break;
		case 2:								//开放型箭头
			CalcArrowRgn(pt, FALSE);
			pDC->MoveTo(point);
			pDC->LineTo(pt[2]);
			pDC->MoveTo(point);	
			pDC->LineTo(pt[3]);			
			break;
		case 3:								//燕尾箭头
			{
				CalcArrowRgn(pt, FALSE);
				CPoint p[4];
				
				p[0] = point;

				p[1] = pt[2];
				
				p[2].x = (pt[2].x + pt[3].x)/4 + point.x/2 ;
				p[2].y = (pt[2].y + pt[3].y)/4 + point.y/2 ;

				p[3] = pt[3];
				
				rgn.CreatePolygonRgn(p, 4, ALTERNATE);
				pDC->FillRgn(&rgn, &b2);
				pDC->MoveTo(p[0]);
				pDC->LineTo(p[1]);
				pDC->LineTo(p[2]);
				pDC->LineTo(p[3]);
				pDC->LineTo(p[0]);
			}
			break;
		case 4:								//钻石箭头
			{
				CalcArrowRgn(pt, FALSE);
				CPoint p[4];
				
				p[0].x = ( pt[0].x + pt[1].x * 0.4 ) / 1.4;
				p[0].y = ( pt[0].y + pt[1].y * 0.4 ) / 1.4;

				p[1].x = ( pt[0].x + pt[1].x * 2.5 ) / 3.5;
				p[1].y = ( pt[0].y + pt[1].y * 2.5 ) / 3.5;

				p[2].x = ( pt[2].x + pt[3].x * 0.4 ) / 1.4;
				p[2].y = ( pt[2].y + pt[3].y * 0.4 ) / 1.4;

				p[3].x = ( pt[2].x + pt[3].x * 2.5 ) / 3.5;
				p[3].y = ( pt[2].y + pt[3].y * 2.5 ) / 3.5;

				CPoint pp[4];

				pp[0] = point;

				pp[1].x = (p[1].x + p[2].x)/2;
				pp[1].y = (p[1].y + p[2].y)/2;

				pp[2].x = (p[2].x + p[3].x)/2;
				pp[2].y = (p[2].y + p[3].y)/2;

				pp[3].x = (p[3].x + p[0].x)/2;
				pp[3].y = (p[3].y + p[0].y)/2;
				
				rgn.CreatePolygonRgn(pp, 4, ALTERNATE);
				pDC->FillRgn(&rgn, &b2);
				pDC->MoveTo(pp[0]);
				pDC->LineTo(pp[1]);
				pDC->LineTo(pp[2]);
				pDC->LineTo(pp[3]);
				pDC->LineTo(pp[0]);
			}
			break;
		case 5:								//圆型箭头
			{
				CalcArrowRgn(pt, FALSE);
				CPoint p;
				p.x = ( pt[0].x + pt[2].x )/2;
				p.y = ( pt[0].y + pt[2].y )/2;
				CBrush *pOldBrush = pDC->SelectObject(&b2);
				pDC->Ellipse(p.x - m_eLine.m_uBeginPointSize/2, p.y - + m_eLine.m_uBeginPointSize/2, p.x + m_eLine.m_uBeginPointSize/2, p.y + m_eLine.m_uBeginPointSize/2);
				pDC->SelectObject(pOldBrush);
			}			
			break;
		case 6:								//空心箭头
			{
				CalcArrowRgn(pt, FALSE);
				CPoint p[3];
				p[0] = point;
				p[1] = pt[2];
				p[2] = pt[3];
				rgn.CreatePolygonRgn(p, 3, ALTERNATE);
				pDC->FillRgn(&rgn, &b1);
				pDC->MoveTo(point);
				pDC->LineTo(pt[2]);
				pDC->LineTo(pt[3]);
				pDC->LineTo(point);			
			}
			break;
		case 7:								//空心燕尾箭头
			{
				CalcArrowRgn(pt, FALSE);
				CPoint p[4];
				
				p[0] = point;

				p[1] = pt[2];
				
				p[2].x = (pt[2].x + pt[3].x)/4 + point.x/2 ;
				p[2].y = (pt[2].y + pt[3].y)/4 + point.y/2 ;

				p[3] = pt[3];
				
				rgn.CreatePolygonRgn(p, 4, ALTERNATE);
				pDC->FillRgn(&rgn, &b1);
				pDC->MoveTo(p[0]);
				pDC->LineTo(p[1]);
				pDC->LineTo(p[2]);
				pDC->LineTo(p[3]);
				pDC->LineTo(p[0]);
			}
			break;
		case 8:								//空心钻石箭头
			{
				CalcArrowRgn(pt, FALSE);
				CPoint p[4];
				
				p[0].x = ( pt[0].x + pt[1].x * 0.4 ) / 1.4;
				p[0].y = ( pt[0].y + pt[1].y * 0.4 ) / 1.4;

				p[1].x = ( pt[0].x + pt[1].x * 2.5 ) / 3.5;
				p[1].y = ( pt[0].y + pt[1].y * 2.5 ) / 3.5;

				p[2].x = ( pt[2].x + pt[3].x * 0.4 ) / 1.4;
				p[2].y = ( pt[2].y + pt[3].y * 0.4 ) / 1.4;

				p[3].x = ( pt[2].x + pt[3].x * 2.5 ) / 3.5;
				p[3].y = ( pt[2].y + pt[3].y * 2.5 ) / 3.5;

				CPoint pp[4];

				pp[0] = point;

				pp[1].x = (p[1].x + p[2].x)/2;
				pp[1].y = (p[1].y + p[2].y)/2;

				pp[2].x = (p[2].x + p[3].x)/2;
				pp[2].y = (p[2].y + p[3].y)/2;

				pp[3].x = (p[3].x + p[0].x)/2;
				pp[3].y = (p[3].y + p[0].y)/2;
				
				rgn.CreatePolygonRgn(pp, 4, ALTERNATE);
				pDC->FillRgn(&rgn, &b1);
				pDC->MoveTo(pp[0]);
				pDC->LineTo(pp[1]);
				pDC->LineTo(pp[2]);
				pDC->LineTo(pp[3]);
				pDC->LineTo(pp[0]);
			}
			break;
		case 9:								//空心圆型箭头
			{
				CalcArrowRgn(pt, FALSE);
				CPoint p;
				p.x = ( pt[0].x + pt[2].x )/2;
				p.y = ( pt[0].y + pt[2].y )/2;
				CBrush *pOldBrush = pDC->SelectObject(&b1);
				pDC->Ellipse(p.x - m_eLine.m_uBeginPointSize/2, p.y - + m_eLine.m_uBeginPointSize/2, p.x + m_eLine.m_uBeginPointSize/2, p.y + m_eLine.m_uBeginPointSize/2);
				pDC->SelectObject(pOldBrush);
			}
			
			break;
		default:
			break;
		}
	}
	
	if( m_eLine.m_uEndPointMode )
	{
		point.x = m_points[ 1 ].x;
		point.y = m_points[ 1 ].y;
		rgn.DeleteObject();

		switch(m_eLine.m_uEndPointMode)
		{
		case 1:								//箭头
			{
				CalcArrowRgn(pt);
				CPoint p[3];
				p[0] = point;
				p[1] = pt[2];
				p[2] = pt[3];
				rgn.CreatePolygonRgn(p, 3, ALTERNATE);
				pDC->FillRgn(&rgn, &b2);
				pDC->MoveTo(point);
				pDC->LineTo(pt[2]);
				pDC->LineTo(pt[3]);
				pDC->LineTo(point);			
			}
			break;
		case 2:								//开放型箭头
			CalcArrowRgn(pt);
			pDC->MoveTo(point);
			pDC->LineTo(pt[2]);
			pDC->MoveTo(point);	
			pDC->LineTo(pt[3]);			
			break;
		case 3:								//燕尾箭头
			{
				CalcArrowRgn(pt);
				CPoint p[4];
				
				p[0] = point;

				p[1] = pt[2];
				
				p[2].x = (pt[2].x + pt[3].x)/4 + point.x/2 ;
				p[2].y = (pt[2].y + pt[3].y)/4 + point.y/2 ;

				p[3] = pt[3];
				
				rgn.CreatePolygonRgn(p, 4, ALTERNATE);
				pDC->FillRgn(&rgn, &b2);
				pDC->MoveTo(p[0]);
				pDC->LineTo(p[1]);
				pDC->LineTo(p[2]);
				pDC->LineTo(p[3]);
				pDC->LineTo(p[0]);
			}
			break;
		case 4:								//钻石箭头
			{
				CalcArrowRgn(pt);
				CPoint p[4];
				
				p[0].x = ( pt[0].x + pt[1].x * 0.4 ) / 1.4;
				p[0].y = ( pt[0].y + pt[1].y * 0.4 ) / 1.4;

				p[1].x = ( pt[0].x + pt[1].x * 2.5 ) / 3.5;
				p[1].y = ( pt[0].y + pt[1].y * 2.5 ) / 3.5;

				p[2].x = ( pt[2].x + pt[3].x * 0.4 ) / 1.4;
				p[2].y = ( pt[2].y + pt[3].y * 0.4 ) / 1.4;

				p[3].x = ( pt[2].x + pt[3].x * 2.5 ) / 3.5;
				p[3].y = ( pt[2].y + pt[3].y * 2.5 ) / 3.5;

				CPoint pp[4];

				pp[0] = point;

				pp[1].x = (p[1].x + p[2].x)/2;
				pp[1].y = (p[1].y + p[2].y)/2;

				pp[2].x = (p[2].x + p[3].x)/2;
				pp[2].y = (p[2].y + p[3].y)/2;

				pp[3].x = (p[3].x + p[0].x)/2;
				pp[3].y = (p[3].y + p[0].y)/2;
				
				rgn.CreatePolygonRgn(pp, 4, ALTERNATE);
				pDC->FillRgn(&rgn, &b2);
				pDC->MoveTo(pp[0]);
				pDC->LineTo(pp[1]);
				pDC->LineTo(pp[2]);
				pDC->LineTo(pp[3]);
				pDC->LineTo(pp[0]);
			}
			break;
		case 5:								//圆型箭头
			{
				CalcArrowRgn(pt);
				CPoint p;
				p.x = ( pt[0].x + pt[2].x )/2;
				p.y = ( pt[0].y + pt[2].y )/2;
				CBrush *pOldBrush = pDC->SelectObject(&b2);
				pDC->Ellipse(p.x - m_eLine.m_uBeginPointSize/2, p.y - + m_eLine.m_uBeginPointSize/2, p.x + m_eLine.m_uBeginPointSize/2, p.y + m_eLine.m_uBeginPointSize/2);
				pDC->SelectObject(pOldBrush);
			}
			
			break;
		case 6:								//空心箭头
			{
				CalcArrowRgn(pt);
				CPoint p[3];
				p[0] = point;
				p[1] = pt[2];
				p[2] = pt[3];
				rgn.CreatePolygonRgn(p, 3, ALTERNATE);
				pDC->FillRgn(&rgn, &b1);
				pDC->MoveTo(point);
				pDC->LineTo(pt[2]);
				pDC->LineTo(pt[3]);
				pDC->LineTo(point);			
			}
			break;
		case 7:								//空心燕尾箭头
			{
				CalcArrowRgn(pt);
				CPoint p[4];
				
				p[0] = point;

				p[1] = pt[2];
				
				p[2].x = (pt[2].x + pt[3].x)/4 + point.x/2 ;
				p[2].y = (pt[2].y + pt[3].y)/4 + point.y/2 ;

				p[3] = pt[3];
				
				rgn.CreatePolygonRgn(p, 4, ALTERNATE);
				pDC->FillRgn(&rgn, &b1);
				pDC->MoveTo(p[0]);
				pDC->LineTo(p[1]);
				pDC->LineTo(p[2]);
				pDC->LineTo(p[3]);
				pDC->LineTo(p[0]);
			}
			break;
		case 8:								//空心钻石箭头
			{
				CalcArrowRgn(pt);
				CPoint p[4];
				
				p[0].x = ( pt[0].x + pt[1].x * 0.4 ) / 1.4;
				p[0].y = ( pt[0].y + pt[1].y * 0.4 ) / 1.4;

				p[1].x = ( pt[0].x + pt[1].x * 2.5 ) / 3.5;
				p[1].y = ( pt[0].y + pt[1].y * 2.5 ) / 3.5;

				p[2].x = ( pt[2].x + pt[3].x * 0.4 ) / 1.4;
				p[2].y = ( pt[2].y + pt[3].y * 0.4 ) / 1.4;

				p[3].x = ( pt[2].x + pt[3].x * 2.5 ) / 3.5;
				p[3].y = ( pt[2].y + pt[3].y * 2.5 ) / 3.5;

				CPoint pp[4];

				pp[0] = point;

				pp[1].x = (p[1].x + p[2].x)/2;
				pp[1].y = (p[1].y + p[2].y)/2;

				pp[2].x = (p[2].x + p[3].x)/2;
				pp[2].y = (p[2].y + p[3].y)/2;

				pp[3].x = (p[3].x + p[0].x)/2;
				pp[3].y = (p[3].y + p[0].y)/2;
				
				rgn.CreatePolygonRgn(pp, 4, ALTERNATE);
				pDC->FillRgn(&rgn, &b1);
				pDC->MoveTo(pp[0]);
				pDC->LineTo(pp[1]);
				pDC->LineTo(pp[2]);
				pDC->LineTo(pp[3]);
				pDC->LineTo(pp[0]);
			}
			break;
		case 9:								//空心圆型箭头
			{
				CalcArrowRgn(pt);
				CPoint p;
				p.x = ( pt[0].x + pt[2].x )/2;
				p.y = ( pt[0].y + pt[2].y )/2;
				CBrush *pOldBrush = pDC->SelectObject(&b1);
				pDC->Ellipse(p.x - m_eLine.m_uBeginPointSize/2, p.y - + m_eLine.m_uBeginPointSize/2, p.x + m_eLine.m_uBeginPointSize/2, p.y + m_eLine.m_uBeginPointSize/2);
				pDC->SelectObject(pOldBrush);
			}
			
			break;
		default:
			break;
		}
	}

	pDC->SelectObject(pOldPen);

	pen.DeleteObject();
	b1.DeleteObject();
	b2.DeleteObject();	
	rgn.DeleteObject();

	return ;
}

void CGeoLineObj::CalcArrowRgn(CPoint *pt, BOOL bEnd)
{
	double Len;
	CPoint point[3];
	
	if( bEnd )//末端
	{		
		Len = m_eLine.m_uEndPointSize;
		point[0].x = m_points[1].x;
		point[0].y = m_points[1].y;
		
		point[1].x = m_points[0].x;
		point[1].y = m_points[0].y;
	}
	else//始端
	{
		Len = m_eLine.m_uBeginPointSize;
		point[0].x = m_points[0].x;
		point[0].y = m_points[0].y;
		point[1].x = m_points[1].x;
		point[1].y = m_points[1].y;		
	}

	
	double A1, B1, C1, A2, B2, C2;			
	double A, B, C;	
	
	
	A = point[1].y - point[0].y ;
	B = point[0].x - point[1].x;
	C = - (  A * point[0].x + B * point[0].y );				
	
	A2 = A;
	B2 = B;
	
	A1 = -B2;
	B1 = A2;
	C1 = - A1 * point[0].x - B1 * point[0].y;//?
	
	C2 = Len/2 * sqrt(A*A + B*B ) + C;
	
	pt[0].x = ( B1 *C2 - B2 * C1 ) / ( A1 * B2 - A2 * B1 );
	pt[0].y = ( C1 *A2 - C2 * A1 ) / ( A1 * B2 - A2 * B1 );			
	
	C2 = C - Len/2 * sqrt(A*A + B*B );
	
	pt[1].x = ( B1 *C2 - B2 * C1 ) / ( A1 * B2 - A2 * B1 );
	pt[1].y = ( C1 *A2 - C2 * A1 ) / ( A1 * B2 - A2 * B1 );
	
	double lamta;
	
	double L = sqrt( (double)(point[0].x - point[1].x) * (point[0].x - point[1].x) + (point[0].y - point[1].y) * (point[0].y - point[1].y) );
	
	lamta = Len / ( L - Len );
	
	point[2].x = (int)( ( point[0].x + point[1].x * lamta )/ ( 1+lamta) );
	point[2].y = (int)( ( point[0].y + point[1].y * lamta )/ ( 1+lamta) );
	
	C1 = - A1 * point[2].x - B1 * point[2].y;//?
	
	C2 = Len / 2 * sqrt(A*A + B*B ) + C;
	
	pt[3].x = ( B1 *C2 - B2 * C1 ) / ( A1 * B2 - A2 * B1 );
	pt[3].y = ( C1 *A2 - C2 * A1 ) / ( A1 * B2 - A2 * B1 );			
	
	C2 = C - Len / 2 * sqrt(A*A + B*B );
	
	pt[2].x = ( B1 *C2 - B2 * C1 ) / ( A1 * B2 - A2 * B1 );
	pt[2].y = ( C1 *A2 - C2 * A1 ) / ( A1 * B2 - A2 * B1 );

	return;
}

BOOL CGeoLineObj::RecalcBounds(CGeoMapView *pView)
{
	ASSERT_VALID(this);
	
	if( pView == NULL )
	{
		POSITION pos = m_pDocument->GetFirstViewPosition();
		pView = (CGeoMapView*)m_pDocument->GetNextView(pos);
	}

	CRect bounds(m_points[0], CSize(1, 1));
	
	bounds.left = m_points[0].x;
	bounds.right = m_points[1].x;
	bounds.top = m_points[0].y;
	bounds.bottom = m_points[1].y;
	
	if (bounds == m_rtPosition)
		return FALSE;
	
	m_rtPosition = bounds;

	if( m_rtPosition.left == m_rtPosition.right ) // 如果是垂直直线段
	{
		m_rtPosition.left -= 2;
		m_rtPosition.right += 2; 
	}

	if( m_rtPosition.top == m_rtPosition.bottom ) // 如果是水平直线段
	{
		m_rtPosition.bottom -= 2;
		m_rtPosition.top += 2; 
	}
	
	return CGeoDrawObj::RecalcBounds(pView);
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::GetHandle
// 返回  : CPoint
// 参数  : int nHandle
// 说明  : 获取某手柄的中心逻辑坐标
//
CPoint CGeoLineObj::GetHandle(int nHandle)
{
	ASSERT_VALID(this);
	
	ASSERT(nHandle >= 1 );
	CPoint point;
	point = m_points[nHandle - 1];
	return point;
}

