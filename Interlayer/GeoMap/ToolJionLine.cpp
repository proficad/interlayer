// ToolJionLine.cpp: implementation of the CToolJionLine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ToolJionLine.h"
#include "../Resource.h"
#include "GeoPolyObj.h"
#include "GeoMapDoc.h"
#include "GeoMapView.h"
#include "CommonStruct.h"
#include "CmdGeoDrawObj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CGeoToolJionLine joinLineTool;

extern CPoint lastPoint; // 最后一点坐标
extern CPoint firstDownPoint;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeoToolJionLine::CGeoToolJionLine()
:CToolBase(CToolBase::joinPoly)
{
	m_pJoinFirstCurve = NULL;
	m_pJoinEndCurve = NULL;
	m_bIsJoinFirstEnd = TRUE;
	m_bIsJoinLastEnd = TRUE;
}

CGeoToolJionLine::~CGeoToolJionLine()
{

}

void CGeoToolJionLine::OnLButtonDown(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	CPoint local = point;
	pView->ClientToDoc(local);	
	
	CGeoDrawObj *pObj = pView->GetDocument()->ObjectAt(local,pView);
	
	if( pObj!=NULL
		&& !pObj->m_bLocked
		&& pObj->IsKindOf(RUNTIME_CLASS(CGeoPolyObj))
		&& !((CGeoPolyObj*)pObj)->m_eLine.m_bClose)
	{
		m_pJoinFirstCurve = (CGeoPolyObj*)pObj;		// 拚接曲线 
		m_bIsJoinFirstEnd = TRUE;					// 是在后面接吗

		double dist1 = (m_pJoinFirstCurve->m_points[0].x - local.x)
			*(m_pJoinFirstCurve->m_points[0].x - local.x)
			+(m_pJoinFirstCurve->m_points[0].y - local.y)
			*(m_pJoinFirstCurve->m_points[0].y - local.y);

		double dist2 = (m_pJoinFirstCurve->m_points[m_pJoinFirstCurve->m_nPoints-1].x - local.x)
			*(m_pJoinFirstCurve->m_points[m_pJoinFirstCurve->m_nPoints-1].x - local.x)
			+(m_pJoinFirstCurve->m_points[m_pJoinFirstCurve->m_nPoints-1].y - local.y) 
			*(m_pJoinFirstCurve->m_points[m_pJoinFirstCurve->m_nPoints-1].y - local.y);

		if( dist1 <= dist2 )
		{
			m_bIsJoinFirstEnd = FALSE;
			lastPoint = m_pJoinFirstCurve->m_points[0];	
			firstDownPoint = m_pJoinFirstCurve->m_points[0];
		}
		else
		{
			lastPoint = m_pJoinFirstCurve->m_points[m_pJoinFirstCurve->m_nPoints-1];	
			firstDownPoint = m_pJoinFirstCurve->m_points[m_pJoinFirstCurve->m_nPoints-1];
		}

		pView->DocToClient(lastPoint);
		pView->DocToClient(firstDownPoint);

		CClientDC dc(pView);
		CPen pen(PS_SOLID, 0, RGB(255,0,0));
		CPen *pOldPen = dc.SelectObject(&pen);
		dc.SetROP2(R2_NOTXORPEN);
		dc.MoveTo(firstDownPoint);
		dc.LineTo(lastPoint);
		dc.MoveTo(firstDownPoint);
		dc.LineTo(lastPoint);

		dc.SelectObject(pOldPen);

		SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_JOIN_LINE));
	}
	else
		m_pJoinFirstCurve = NULL;

	CToolBase::OnLButtonDown(pView, nFlags, point);
}

void CGeoToolJionLine::OnLButtonUp(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	ReleaseCapture();

	if( m_pJoinFirstCurve )
	{
		CClientDC dc(pView);
		CPen pen(PS_SOLID, 0, RGB(255,0,0));
		CPen *pOldPen = dc.SelectObject(&pen);
		dc.SetROP2(R2_NOTXORPEN);
		dc.MoveTo(firstDownPoint);
		dc.LineTo(lastPoint);
		
		dc.SelectObject(pOldPen);
	}
	

	if( m_pJoinFirstCurve 
		&& m_pJoinEndCurve)
	{
		CPoint local = point;
		pView->ClientToDoc(local);
		
		CGeoMapDoc *pDoc = pView->GetDocument();
		
		CGeoPolyObj *pLogCurve = new CGeoPolyObj(CRect(local, CSize(0, 0)),pDoc->m_pCurDrawLayer);
		pLogCurve->CopyAttribute(m_pJoinFirstCurve);
		pLogCurve->m_pDocument = pDoc;

		if( m_bIsJoinFirstEnd )
		{
			int n = m_pJoinFirstCurve->m_nPoints;
			for (int i=0;i<n;i++)
				pLogCurve->AddPoint(m_pJoinFirstCurve->m_points[i],pView);
		}
		else
		{
			int n = m_pJoinFirstCurve->m_nPoints;
			for (int i=n-1;i>=0;i--)
				pLogCurve->AddPoint(m_pJoinFirstCurve->m_points[i],pView);
		}
		
		if( !m_bIsJoinLastEnd )
		{
			int n = m_pJoinEndCurve->m_nPoints;
			for (int i=0;i<n;i++)
				pLogCurve->AddPoint(m_pJoinEndCurve->m_points[i],pView);
		}
		else
		{
			int n = m_pJoinEndCurve->m_nPoints;
			for (int i=n-1;i>=0;i--)
				pLogCurve->AddPoint(m_pJoinEndCurve->m_points[i],pView);
		}
		
		pDoc->Do(new CCmdJoinLine(pDoc,m_pJoinFirstCurve,m_pJoinEndCurve,pLogCurve,pView), TRUE);
	}

	m_pJoinFirstCurve = NULL;
	m_pJoinEndCurve = NULL;	
}

void CGeoToolJionLine::OnMouseMove(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	if (!m_pJoinFirstCurve)
	{
		CPoint local = point;
		pView->ClientToDoc(local);
		
		CGeoDrawObj *pObj = pView->GetDocument()->ObjectAt(local,pView);

		if( pObj!=NULL
			&& !pObj->m_bLocked
			&& pObj->IsKindOf(RUNTIME_CLASS(CGeoPolyObj))
			&& !((CGeoPolyObj*)pObj)->m_eLine.m_bClose)
				SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_HAND));
	}
	else
	{
		CPoint local = point;
		pView->ClientToDoc(local);
		
		CGeoDrawObj *pObj = pView->GetDocument()->ObjectAt(local,pView);
		
		if( pObj!=NULL
			&& !pObj->m_bLocked
			&& pObj->IsKindOf(RUNTIME_CLASS(CGeoPolyObj))
			&& pObj != m_pJoinFirstCurve
			&& !((CGeoPolyObj*)pObj)->m_eLine.m_bClose)
		{
			m_pJoinEndCurve = (CGeoPolyObj*)pObj;

			double dist1 = (((CGeoPolyObj*)pObj)->m_points[0].x - local.x)
				*(((CGeoPolyObj*)pObj)->m_points[0].x - local.x)
				+(((CGeoPolyObj*)pObj)->m_points[0].y - local.y)
				*(((CGeoPolyObj*)pObj)->m_points[0].y - local.y);
			
			double dist2 = (((CGeoPolyObj*)pObj)->m_points[((CGeoPolyObj*)pObj)->m_nPoints-1].x - local.x)
				*(((CGeoPolyObj*)pObj)->m_points[((CGeoPolyObj*)pObj)->m_nPoints-1].x - local.x)
				+(((CGeoPolyObj*)pObj)->m_points[((CGeoPolyObj*)pObj)->m_nPoints-1].y - local.y) 
				*(((CGeoPolyObj*)pObj)->m_points[((CGeoPolyObj*)pObj)->m_nPoints-1].y - local.y);
			
			if( dist1 <= dist2 )
			{
				local = ((CGeoPolyObj*)pObj)->m_points[0];
				m_bIsJoinLastEnd = FALSE;
			}
			else
			{
				local = ((CGeoPolyObj*)pObj)->m_points[((CGeoPolyObj*)pObj)->m_nPoints-1];
				m_bIsJoinLastEnd = TRUE;
			}
			
		}		
		else
			m_pJoinEndCurve = NULL;

		pView->DocToClient(local);
		CClientDC dc(pView);
		CPen pen(PS_SOLID, 0, RGB(255,0,0));
		CPen *pOldPen = dc.SelectObject(&pen);

		dc.SetROP2(R2_NOTXORPEN);
		
		dc.MoveTo(firstDownPoint);
		dc.LineTo(lastPoint);

		dc.MoveTo(firstDownPoint);
		dc.LineTo(local);

		dc.SelectObject(pOldPen);

		lastPoint = local;
	}
	return;
}
