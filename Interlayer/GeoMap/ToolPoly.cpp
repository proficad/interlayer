// ToolPoly.cpp: implementation of the CToolPoly class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ToolPoly.h"
#include "GeoMapView.h"
#include "GeoMapDoc.h"
#include "ToolSelect.h"
#include "../Resource.h"
#include "CmdGeoDrawObj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CGeoToolSelect selectTool;

int g_nMoveLine = 10;
int g_nMovePage = 100;

CGeoToolPoly polylineTool(CToolBase::polyline);
CGeoToolPoly plineTool(CToolBase::pline);
CGeoToolPoly interlayerTool(CToolBase::interlayer);
CGeoToolPoly polygonemptyTool(CToolBase::polygonempty);
CGeoToolPoly plinepolygonemptyTool(CToolBase::plinepolygonempty);
CGeoToolPoly polygonTool(CToolBase::polygon);
CGeoToolPoly plinepolygonTool(CToolBase::plinepolygon);
CGeoToolPoly polyTool(CToolBase::poly);
CGeoToolPoly smoothfillTool(CToolBase::smoothfill);

extern CToolBase::SelectMode selectMode;
extern int g_nDragHandle;

extern CPoint lastPoint; // 最后一点坐标

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeoToolPoly::CGeoToolPoly(DrawShape drawShape)
: CToolBase(drawShape)
{
	m_pPolyObj = NULL;
}

CGeoToolPoly::~CGeoToolPoly()
{

}

void CGeoToolPoly::OnLButtonDown(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	CPoint local = point;
	pView->ClientToDoc(local);	

	CGeoMapDoc *pDoc = pView->GetDocument();

	if (m_pPolyObj == NULL)
	{
		pView->SetCapture();
		
		if( pView->m_bCapture )
		{
			CGeoDrawObj *pObj = pView->GetDocument()->HitOnBorder(local,pView);
			if( pObj != NULL && pObj->IsKindOf(RUNTIME_CLASS(CGeoPolyObj)) )
			{
				int iIndex[2];
				local = ((CGeoPolyObj*)pObj)->GetNearByIndex(local, iIndex);
			}
			
		}

		m_pPolyObj = new CGeoPolyObj(CRect(local, CSize(0, 0)),pDoc->m_pCurDrawLayer);
		m_pPolyObj->m_bEditing = TRUE;			// 处于编辑状态
		
		switch (c_drawShape)
		{
		default:
			ASSERT(FALSE); // unsuported shape!
			
		case polyline:			
			break;
		case polygonempty:
			m_pPolyObj->m_eLine.m_bClose = TRUE;
			break;
		case polygon:
			m_pPolyObj->m_eLine.m_bClose = TRUE;
			m_pPolyObj->m_eFill.m_bShow = TRUE;
			break;
			
		case pline:
			m_pPolyObj->m_eLine.m_bSmooth = TRUE;
			break;
		case interlayer:
			m_pPolyObj->m_eLine.m_bSmooth = TRUE;
			m_pPolyObj->SetObjName(_T("夹层线"));
			m_pPolyObj->m_ePolyType = CGeoPolyObj::interlayers;
			m_pPolyObj->m_eLine.s_LineStyle.m_PatternFill.m_crColourFore = RGB(0, 0, 192);			
			break;
		case plinepolygonempty:
			m_pPolyObj->m_eLine.m_bClose = TRUE;
			m_pPolyObj->m_eLine.m_bSmooth = TRUE;
			break;
		case plinepolygon:
			m_pPolyObj->m_eLine.m_bClose = TRUE;
			m_pPolyObj->m_eFill.m_bShow = TRUE;
			m_pPolyObj->m_eLine.m_bSmooth = TRUE;
			break;
		case poly:
			m_pPolyObj->m_eLine.m_nPenStyle = 6;
			m_pPolyObj->m_eLine.m_bClose = TRUE;
			m_pPolyObj->m_eFill.m_bShow = TRUE;
			break;
		case smoothfill:
			m_pPolyObj->m_eLine.m_nPenStyle = 6;
			m_pPolyObj->m_eLine.m_bClose = TRUE;
			m_pPolyObj->m_eFill.m_bShow = TRUE;
			m_pPolyObj->m_eLine.m_bSmooth = TRUE;
			break;
		}	
		
		CGeoMapDoc *pDoc = pView->GetDocument();
		pView->Select(m_pPolyObj);
		m_pPolyObj->AddPoint(local, pView);
	}
	else 
	{
		if( pView->m_bCapture )
		{
			CGeoDrawObj *pObj = pView->GetDocument()->HitOnBorder(local,pView);
			if( pObj != NULL && pObj->IsKindOf(RUNTIME_CLASS(CGeoPolyObj)) 
				&& pObj != m_pPolyObj )
			{
				int iIndex[2];
				local = ((CGeoPolyObj*)pObj)->GetNearByIndex(local, iIndex);
			}
		}

		m_pPolyObj->MoveHandleTo(g_nDragHandle, local, pView);
		
		if (local == m_pPolyObj->m_points[0])
		{
			ReleaseCapture();
			m_pPolyObj->m_nPoints -= 1;
			if (m_pPolyObj->m_nPoints < 2)
			{
				CGeoMapDoc *pDoc = pView->GetDocument();
				pDoc->Remove(m_pPolyObj);
				pView->Select(NULL);
			}
			else
			{
				pView->InvalObj(m_pPolyObj);
			}
			m_pPolyObj->m_bEditing = FALSE;			// 处于编辑状态
			m_pPolyObj->m_bSizeChanging = FALSE;
			m_pPolyObj->m_bMoving = FALSE;
			m_pPolyObj = NULL;
			c_drawShape = selection;
			return;
		}
	}
	
	local.x += 1;

	m_pPolyObj->AddPoint(local, pView);

	selectMode = sizeObj;
	g_nDragHandle = m_pPolyObj->GetHandleCount();
	lastPoint = local;	
}

void CGeoToolPoly::OnLButtonDblClk(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	ReleaseCapture();
	
	CGeoMapDoc *pDoc = pView->GetDocument();

	CPoint local = point;
	pView->ClientToDoc(local);

	if( pView->m_bCapture )
	{
		CGeoDrawObj *pObj = pView->GetDocument()->HitOnBorder(local,pView);
		if( pObj != NULL 
			&& pObj != m_pPolyObj )
		{
			int iIndex[2];
			local = pObj->GetNearByIndex(local, iIndex);
			g_nDragHandle = m_pPolyObj->GetHandleCount();
			m_pPolyObj->MoveHandleTo(g_nDragHandle, local, pView);
		}
	}

	int nPoints = m_pPolyObj->m_nPoints;
	if (nPoints > 2 &&
		(m_pPolyObj->m_points[nPoints - 1] == m_pPolyObj->m_points[nPoints - 2] ||
		m_pPolyObj->m_points[nPoints - 1].x - 1 == m_pPolyObj->m_points[nPoints - 2].x &&
		m_pPolyObj->m_points[nPoints - 1].y == m_pPolyObj->m_points[nPoints - 2].y))
		
	{
		m_pPolyObj->m_nPoints -= 1;
		pView->InvalObj(m_pPolyObj);
	}
	
	if( nPoints == 2 && local == m_pPolyObj->m_points[0])	
		m_pPolyObj->m_nPoints -= 1;
	
	if (m_pPolyObj->m_nPoints < 2)
	{
		pDoc->Remove(m_pPolyObj);
		pView->Select(NULL);
	}
	else if( m_pPolyObj->m_nPoints < 3 && m_pPolyObj->m_eLine.m_bClose )
	{
		m_pPolyObj->m_eLine.m_bClose = FALSE;
	}
	else
	{
		m_pPolyObj->RecalcBounds(pView);
		pDoc->Do(new CCmdAddDrawObj(pDoc, m_pPolyObj), TRUE);	// Do add stroke command				
		
		m_pPolyObj->m_bEditing = FALSE;
		m_pPolyObj->m_bSizeChanging = FALSE;
		m_pPolyObj->m_bMoving = FALSE;
	}
	
	m_pPolyObj = NULL;
	c_drawShape = selection;
}

void CGeoToolPoly::OnLButtonUp(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	//CToolBase::OnLButtonDown(pView, nFlags, point );
	CToolBase::OnLButtonUp(pView, nFlags, point );
}

void CGeoToolPoly::OnMouseMove(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	if(m_pPolyObj != NULL)
	{
		CPoint local = point;
		pView->ClientToDoc(local);

		SetCursor(AfxGetApp()->LoadCursor(IDC_PENCIL));
		if ( g_nDragHandle != 0 && selectMode == sizeObj ) // 改变对象大小
		{
			m_pPolyObj->m_bSizeChanging = TRUE;
			if( pView->m_bCapture ) // 自动捕捉
			{
				CGeoDrawObj *p = pView->GetDocument()->HitOnBorder(local,pView,m_pPolyObj);
				if( p != NULL 
					&& p != m_pPolyObj )
				{
					int iIndex[2];
					local = p->GetNearByIndex(local, iIndex);
				}
			}
			m_pPolyObj->MoveHandleTo(g_nDragHandle, local, pView);
		}
	}
	else
	{		
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
		c_last = point;
	}
}


void CGeoToolPoly::OnKeyDown(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// 滚动屏幕
	
	CPoint delta(0,0);
	switch(nChar)
	{
	case VK_LEFT:
		delta.x = g_nMoveLine;
		break;
	case VK_RIGHT:
		delta.x = -g_nMoveLine;
		break;
	case VK_UP:
		delta.y = g_nMoveLine;
		break;
	case VK_DOWN:
		delta.y = -g_nMoveLine;
		break;
	case VK_HOME:
		delta.x = g_nMovePage;
		break;
	case VK_END:
		delta.x = -g_nMovePage;
		break;
	case VK_PRIOR:
		delta.y = g_nMovePage;
		break;
	case VK_NEXT:
		delta.y = -g_nMovePage;
		break;
	case VK_ADD:
	case '+':
		g_nMoveLine ++;
		break;
	case VK_SUBTRACT:
	case '-':
		g_nMoveLine --;
		if(g_nMoveLine < 1)
			g_nMoveLine = 1;
		break;
	case VK_SPACE:
		if( m_pPolyObj != NULL )
		{
			ReleaseCapture();

			CGeoMapDoc *pDoc = pView->GetDocument();
			
			int nPoints = m_pPolyObj->m_nPoints;			
			
			if( pView->m_bCapture )
			{
				CPoint local = m_pPolyObj->m_points[nPoints - 1];
				CGeoDrawObj *pObj = pView->GetDocument()->HitOnBorder(local,pView);
				if( pObj != NULL 
					&& pObj != m_pPolyObj )
				{
					int iIndex[2];
					local = pObj->GetNearByIndex(local, iIndex);
					g_nDragHandle = m_pPolyObj->GetHandleCount();
					m_pPolyObj->MoveHandleTo(g_nDragHandle, local, pView);
				}
			}

			nPoints = m_pPolyObj->m_nPoints;

			if (nPoints > 2 &&
				(m_pPolyObj->m_points[nPoints - 1] == m_pPolyObj->m_points[nPoints - 2] ||
				m_pPolyObj->m_points[nPoints - 1].x - 1 == m_pPolyObj->m_points[nPoints - 2].x &&
				m_pPolyObj->m_points[nPoints - 1].y == m_pPolyObj->m_points[nPoints - 2].y))
				
			{
				m_pPolyObj->m_nPoints -= 1;
				pView->InvalObj(m_pPolyObj);
			}	

			if (m_pPolyObj->m_nPoints < 2)
			{
				pDoc->Remove(m_pPolyObj);
				pView->Select(NULL);
			}
			else if( m_pPolyObj->m_nPoints < 3 && m_pPolyObj->m_eLine.m_bClose )
			{
				m_pPolyObj->m_eLine.m_bClose = FALSE;
			}
			else
			{
				m_pPolyObj->RecalcBounds(pView);
				
				pDoc->Do(new CCmdAddDrawObj(pDoc, m_pPolyObj), TRUE);	// Do add stroke command				
				
				m_pPolyObj->m_bEditing = FALSE;
				m_pPolyObj->m_bSizeChanging = FALSE;
				m_pPolyObj->m_bMoving = FALSE;
			}
			
			m_pPolyObj = NULL;
			//c_drawShape = selection;
		}
		break;
	case VK_BACK:
		if( m_pPolyObj != NULL )
		{
			CGeoMapDoc *pDoc = pView->GetDocument();
			
			CPoint local = m_pPolyObj->m_points[m_pPolyObj->m_nPoints-1];
			
			CPoint local1;
			if (m_pPolyObj->m_nPoints >= 2 )
			{
				CPoint local = m_pPolyObj->m_points[m_pPolyObj->m_nPoints-2];

				local1 = local;
				CClientDC dc(pView);
				pView->OnPrepareDC(&dc);
				int mode = dc.SetROP2(R2_NOTXORPEN);				
				CBrush *pOldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
				
				CSize size(3,3);
				dc.DPtoLP(&size);
				if( m_pPolyObj->m_nPoints != 3 )
					dc.Rectangle(local.x - size.cx, local.y - size.cy,	local.x + size.cx, local.y + size.cy);
				
				dc.SelectObject(pOldBrush);				
				dc.SetROP2(mode);
				
				m_pPolyObj->MoveHandleTo(g_nDragHandle, local, pView);
			}
			
			m_pPolyObj->m_nPoints --;
			
			g_nDragHandle = m_pPolyObj->GetHandleCount();
			m_pPolyObj->MoveHandleTo(g_nDragHandle, local, pView);

			if (m_pPolyObj->m_nPoints < 2 )
			{
				CPoint local = m_pPolyObj->m_points[1];
				
				CClientDC dc(pView);
				pView->OnPrepareDC(&dc);
				int mode = dc.SetROP2(R2_NOTXORPEN);				
				CBrush *pOldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
				
				CSize size(3,3);
				dc.DPtoLP(&size);
				dc.Rectangle(local.x - size.cx, local.y - size.cy,	local.x + size.cx, local.y + size.cy);
				
				dc.SelectObject(pOldBrush);				
				dc.SetROP2(mode);
				
				m_pPolyObj->MoveHandleTo(g_nDragHandle, local, pView);

				pDoc->Remove(m_pPolyObj);
				pView->Select(NULL);
				m_pPolyObj = NULL;
			}
		}
		break;
	default:
		break;
	}

	if( delta.x != 0 || delta.y != 0)
		pView->UpdateScrollPosition(delta);
}
void CGeoToolPoly::OnKeyUp(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags)
{

}
void CGeoToolPoly::OnChar(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	
}

void CGeoToolPoly::OnCancel(CGeoMapView* pView)
{
	CToolBase::OnCancel(pView);
	m_pPolyObj = NULL;
}