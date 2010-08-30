// ToolDivideLine.cpp: implementation of the CToolDivideLine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ToolDivideLine.h"
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

extern double ptopointset(int vcount, CPoint2D pointset[], CPoint2D p, CPoint2D &q, int *iIndex);

CGeoToolDivideLine divideLineTool;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeoToolDivideLine::CGeoToolDivideLine()
:CToolBase(CToolBase::dividePoly)
{

}

CGeoToolDivideLine::~CGeoToolDivideLine()
{

}

void CGeoToolDivideLine::OnLButtonDown(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	CPoint local = point;
	pView->ClientToDoc(local);	
	
	CGeoDrawObj *pObj = pView->GetDocument()->ObjectAt(local,pView);
	
	if( pObj!=NULL
		&& !pObj->m_bLocked
		&& pObj->IsKindOf(RUNTIME_CLASS(CGeoPolyObj))
		&& !((CGeoPolyObj*)pObj)->m_eLine.m_bClose)
	{
		SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_DIVIDE_LINE_DOWN));

		CGeoPolyObj *pPolyObj = (CGeoPolyObj *)pObj;
		int index[2];
		
		CPoint2D pt(local.x, local.y);
		CPoint2D retPt;
		CPoint2D *points = new CPoint2D [pPolyObj->m_nPoints];
		if( points == NULL )
			return;
		
		for (int i=0;i<pPolyObj->m_nPoints;i++)
			points[i] = pPolyObj->m_points[i];
		
		ptopointset(pPolyObj->m_nPoints,points,pt,retPt,index);
		
		delete[]points;
		
		if( index[0]==-1&&index[1]==-1)
			return;
		
		CGeoMapDoc *pDoc = pView->GetDocument();

		CGeoPolyObj *pLogCurve1 = new CGeoPolyObj(CRect(local, CSize(0, 0)),pDoc->m_pCurDrawLayer);
		pLogCurve1->CopyAttribute(pPolyObj);
		pLogCurve1->m_pDocument = pDoc;
		
		CGeoPolyObj *pLogCurve2 = NULL;
		
		if( nFlags & MK_SHIFT  ) // 上截断
		{				
			if( ( pPolyObj->m_points[0].y < pPolyObj->m_points[index[0]].y 
				&& pPolyObj->m_points[index[0]].y < pPolyObj->m_points[index[1]].y 
				&& pPolyObj->m_points[index[1]].y < pPolyObj->m_points[pPolyObj->m_nPoints-1].y ) || 
				( pPolyObj->m_points[0].x < pPolyObj->m_points[index[0]].x 
				&& pPolyObj->m_points[index[0]].x < pPolyObj->m_points[index[1]].x 
				&& pPolyObj->m_points[index[1]].x < pPolyObj->m_points[pPolyObj->m_nPoints-1].x )
				)
			{
				
				pLogCurve1->AddPoint(local, pView);
				
				for(int i = index[1] ; i < pPolyObj->m_nPoints ; i++)
					pLogCurve1->AddPoint(pPolyObj->m_points[i], pView);
			}
			else
			{
				for(int i = 0 ; i < index[1] ; i++)
					pLogCurve1->AddPoint(pPolyObj->m_points[i], pView);
				pLogCurve1->AddPoint(local, pView);
			}	
			
		}
		else if( nFlags & MK_CONTROL) // 下截断  nOption == 2
		{
			
			if( ( pPolyObj->m_points[0].y < pPolyObj->m_points[index[0]].y 
				&& pPolyObj->m_points[index[0]].y < pPolyObj->m_points[index[1]].y 
				&& pPolyObj->m_points[index[1]].y < pPolyObj->m_points[pPolyObj->m_nPoints-1].y ) || 
				( pPolyObj->m_points[0].x < pPolyObj->m_points[index[0]].x 
				&& pPolyObj->m_points[index[0]].x < pPolyObj->m_points[index[1]].x 
				&& pPolyObj->m_points[index[1]].x < pPolyObj->m_points[pPolyObj->m_nPoints-1].x )
				)
			{
				for(int i = 0 ; i < index[1] ; i++)
					pLogCurve1->AddPoint(pPolyObj->m_points[i], pView);
				pLogCurve1->AddPoint(local, pView);			
			}
			else
			{
				pLogCurve1->AddPoint(local, pView);
				
				for(int i = index[1] ; i < pPolyObj->m_nPoints ; i++)
					pLogCurve1->AddPoint(pPolyObj->m_points[i], pView);
				
			}
		}			
		else // 中间截断
		{
			pLogCurve2 = new CGeoPolyObj(CRect(local, CSize(0, 0)),pDoc->m_pCurDrawLayer);
			pLogCurve2->CopyAttribute(pPolyObj);
			pLogCurve2->m_pDocument = pDoc;
			
			pLogCurve1->AddPoint(local, pView);
			
			int i;
			
			for( i = index[1] ; i < pPolyObj->m_nPoints ; i++)
				pLogCurve1->AddPoint(pPolyObj->m_points[i], pView);
			
			for(i = 0 ; i < index[1] ; i++)
				pLogCurve2->AddPoint(pPolyObj->m_points[i], pView);
			pLogCurve2->AddPoint(local, pView);
		}
		
		pDoc->Do(new CCmdDivideLine(pDoc,pLogCurve1, pLogCurve2, pPolyObj, pView), TRUE);

	}

	CToolBase::OnLButtonDown(pView, nFlags, point);
}

void CGeoToolDivideLine::OnLButtonUp(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	ReleaseCapture();
}

void CGeoToolDivideLine::OnMouseMove(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	if (pView->GetCapture() != pView)
	{
		CPoint local = point;
		pView->ClientToDoc(local);
		
		CGeoDrawObj *pObj = pView->GetDocument()->ObjectAt(local,pView);

		if( pObj!=NULL
			&& !pObj->m_bLocked
			&& pObj->IsKindOf(RUNTIME_CLASS(CGeoPolyObj))
			&& !((CGeoPolyObj*)pObj)->m_eLine.m_bClose)
				SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_DIVIDE_LINE));
		return;
	}
}
