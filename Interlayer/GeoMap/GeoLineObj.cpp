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
	SetObjName(_T("ֱ��"));
}

CGeoLineObj::CGeoLineObj(CDrawLayer* pLayer)
:CGeoDrawObj(pLayer)
{
	SetObjName(_T("ֱ��"));
}

CGeoLineObj::CGeoLineObj(const CRect& position, CDrawLayer* pLayer)
:CGeoDrawObj(position,pLayer)
{
	SetObjName(_T("ֱ��"));
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
		if( !Intersects(cliRect) ) //�����ͼ�ز��ڵ�ǰ����������ʾ
			return ;
	}
	
	for (long i=0;i<2; i++)
	{
		m_points[i] = CoordinateTransfer(m_geoPoints[i]);
	}

	int nSaved = pDC->SaveDC();
	pDC->SetBkMode(TRANSPARENT);          //�趨��ʾ��ʽ
	
	DrawLine(pDC, pView, CGeoLineObj::MIDST);

	DrawArrow(pDC, pView);
	
	pDC->RestoreDC(nSaved);
}


////////////////////////////////////////////////////////////////////////////////
//
// ����  : CGeoLineObj::OnEditProperties
// ����  : void
// ����  : CGeoMapView *pView
// ˵��  : �༭ͼ������
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
// ����  : CGeoDrawObj::Clone
// ����  : CGeoDrawObj*
// ����  : CGeoMapDoc *pDoc
// ˵��  : ��¡ͼ�ز���ͼ�ؼ��뵽�ĵ���
//
CGeoDrawObj* CGeoLineObj::Clone(CDrawLayer *pLayer)
{
	ASSERT_VALID(this);
	
	CGeoLineObj* pClone = new CGeoLineObj(m_rtPosition,pLayer);
	pClone->m_rtPosition	=	m_rtPosition;
	pClone->m_bLocked		=	m_bLocked;			// ����ͼ��
	pClone->m_bShow			=	m_bShow;			// �Ƿ���ʾ
	pClone->m_strID			=	newGUID();			// �µ�ID
	pClone->m_ptAnchors.Copy(m_ptAnchors);			// ê��

	pClone->m_pDocument		=	m_pDocument;		// �ĵ�
	pClone->m_geoCenterPoint		=	m_geoCenterPoint;


	pClone->m_eLine				= m_eLine;							// ������
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
// ����  : CGeoDrawObj::Serialize
// ����  : void
// ����  : CArchive &ar
// ˵��  : ���л�ͼ��
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

		int nVersion = ar.GetObjectSchema(); // ��ȡ�汾��Ϣ
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
// ����  : CGeoLineObj::HitTestHanle
// ����  : int
// ����  : CPoint point���߼�����
// ����  : CGeoMapView *pView
// ����  : BOOL bSelected ����Ѿ�ѡ�У������Ƿ����ĳ���ֱ�
// ˵��  : �����������1ѡ�и�ͼ�أ�����û�е��и�ͼ��
//
int CGeoLineObj::HitTestHandle(CPoint point, CGeoMapView *pView, BOOL bSelected)
{
	ASSERT_VALID(this);
	ASSERT(pView != NULL);
	
	if (bSelected) // �����ͼ���Ѿ���ѡ����
	{
		int nHandleCount = GetHandleCount(); // �ֱ�����
		for (int nHandle = 1; nHandle <= nHandleCount; nHandle += 1)
		{
			// ����ĳ�ֱ����߼���Χ
			CRect rc = GetHandleRect(nHandle,pView);
			rc.NormalizeRect();
			if (point.x >= rc.left && point.x <= rc.right &&
				point.y >= rc.top && point.y <= rc.bottom)
				return nHandle;
		}
	}
	else // ����, �����Ƿ�����ͼ�ط�Χ����
	{
		if(IsPoint(point,pView))
			return 1;
	}
	return 0;
}

// �������߷����ı�ʱ
void CGeoLineObj::OnScaleChanged()
{
	CGeoDrawObj::OnScaleChanged();
	
	for (int i = 0; i < 2; i += 1)
		m_points[i] = CoordinateTransfer(m_geoPoints[i]);
	
	RecalcBounds();	
}

// ��������תʱ
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

// ����������ԭ���иı�ʱ
void CGeoLineObj::OnCoordOrigChanged()
{
	Invalidate();
}

// ת�������ı�ʱ
void CGeoLineObj::OnConvertParamChanged()
{
	for (int i=0; i<2; i++)
		m_geoPoints[i] = CoordinateTransfer(m_points[i]);
}

void CGeoLineObj::DrawLine(CDC *pDC, CGeoMapView *pView, PLACE place, BOOL bThin)//����
{	
	CRect rect;
	rect = m_rtPosition;	

	if(place == INNER)//��
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
	
	if( !Intersects(cliRect) ) //�����ͼ�ز��ڵ�ǰ����������ʾ
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
		//�ʵĹ̶�����ֵ��Ϊ���¼��֣������Զ�
		PS_SOLID,//            0
			PS_DASH,//             1       /* -------  */
			PS_DOT,//              2       /* .......  */
			PS_DASHDOT,//          3       /* _._._._  */
			PS_DASHDOTDOT,//       4       /* _.._.._  */
			PS_INSIDEFRAME,//      5
			PS_NULL//             6
	};
	
	if( m_eLine.m_nPenStyle == 6 )//�������ֱ�ӷ���
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
// ����  : CGeoDrawObj::GetHandleCount
// ����  : int
// ˵��  : ��ȡ�ֱ�����
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
// ����  : CGeoDrawObj::MoveHandleTo
// ����  : void
// ����  : int nHandle �ֱ�����
// ����  : CPoint point Ŀǰ�߼�λ��
// ����  : CGeoMapView *pView
// ˵��  : �ƶ�ĳ�ֱ�
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
	
	// ת��Ϊ�������
	m_geoCenterPoint = CoordinateTransfer(pt);
	m_pDocument->SetModifiedFlag();
}

////////////////////////////////////////////////////////////////////////////////
//
// ����  : CGeoDrawObj::GetHandleCursor
// ����  : HCURSOR
// ����  : int nHandle
// ˵��  : ��ȡĳ�ֱ������ָ��
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
// ����  : CGeoDrawObj::Intersects
// ����  : BOOL TRUE�ཻ FALSE ���ཻ
// ����  : const CRect &rect �߼�����
// ˵��  : ���Ծ��η�Χ�Ƿ���ͼ�ط�Χ�ཻ
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
	if( m_eLine.m_nLineType ) return;//���ǵ��߲�����ͷ

	CPoint point;//
	CPoint pt[4];//��ͷ����

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
		case 1:								//��ͷ
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
		case 2:								//�����ͼ�ͷ
			CalcArrowRgn(pt, FALSE);
			pDC->MoveTo(point);
			pDC->LineTo(pt[2]);
			pDC->MoveTo(point);	
			pDC->LineTo(pt[3]);			
			break;
		case 3:								//��β��ͷ
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
		case 4:								//��ʯ��ͷ
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
		case 5:								//Բ�ͼ�ͷ
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
		case 6:								//���ļ�ͷ
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
		case 7:								//������β��ͷ
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
		case 8:								//������ʯ��ͷ
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
		case 9:								//����Բ�ͼ�ͷ
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
		case 1:								//��ͷ
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
		case 2:								//�����ͼ�ͷ
			CalcArrowRgn(pt);
			pDC->MoveTo(point);
			pDC->LineTo(pt[2]);
			pDC->MoveTo(point);	
			pDC->LineTo(pt[3]);			
			break;
		case 3:								//��β��ͷ
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
		case 4:								//��ʯ��ͷ
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
		case 5:								//Բ�ͼ�ͷ
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
		case 6:								//���ļ�ͷ
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
		case 7:								//������β��ͷ
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
		case 8:								//������ʯ��ͷ
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
		case 9:								//����Բ�ͼ�ͷ
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
	
	if( bEnd )//ĩ��
	{		
		Len = m_eLine.m_uEndPointSize;
		point[0].x = m_points[1].x;
		point[0].y = m_points[1].y;
		
		point[1].x = m_points[0].x;
		point[1].y = m_points[0].y;
	}
	else//ʼ��
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

	if( m_rtPosition.left == m_rtPosition.right ) // ����Ǵ�ֱֱ�߶�
	{
		m_rtPosition.left -= 2;
		m_rtPosition.right += 2; 
	}

	if( m_rtPosition.top == m_rtPosition.bottom ) // �����ˮƽֱ�߶�
	{
		m_rtPosition.bottom -= 2;
		m_rtPosition.top += 2; 
	}
	
	return CGeoDrawObj::RecalcBounds(pView);
}


////////////////////////////////////////////////////////////////////////////////
//
// ����  : CGeoDrawObj::GetHandle
// ����  : CPoint
// ����  : int nHandle
// ˵��  : ��ȡĳ�ֱ��������߼�����
//
CPoint CGeoLineObj::GetHandle(int nHandle)
{
	ASSERT_VALID(this);
	
	ASSERT(nHandle >= 1 );
	CPoint point;
	point = m_points[nHandle - 1];
	return point;
}

