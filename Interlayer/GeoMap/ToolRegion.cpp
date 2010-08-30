// ToolRegion.cpp: implementation of the CToolRegion class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ToolRegion.h"
#include "ToolSelect.h"
#include "GeoMapView.h"
#include "GeoMapDoc.h"
#include "../Resource.h"
#include "CmdGeoDrawObj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


extern CGeoToolSelect selectTool;

extern int g_nMoveLine;
extern int g_nMovePage;

CGeoToolRegion regionTool;

extern CToolBase::SelectMode selectMode;
extern int g_nDragHandle;

extern CPoint lastPoint; // 最后一点坐标

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CGeoToolRegion::CGeoToolRegion()
:CToolBase(CToolBase::region)
{
	m_pPolyObj = NULL;
	m_pFirstObj = NULL;
	m_pLastObj = NULL;

	m_nFirstIndex[0] = -1;
	m_nFirstIndex[1] = -1;
	m_nLastIndex[0] = -1;
	m_nLastIndex[1] = -1;
}

CGeoToolRegion::~CGeoToolRegion()
{
	
}

void CGeoToolRegion::OnLButtonDown(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	CPoint local = point;
	pView->ClientToDoc(local);	

	CGeoMapDoc *pDoc = pView->GetDocument();

	if (m_pPolyObj == NULL)
	{
		pView->SetCapture();
		
		CGeoDrawObj *pObj = pView->GetDocument()->HitOnBorder(local,pView,m_pPolyObj);
		if( pObj != NULL )
		{
			int iIndex[2];
			local = pObj->GetNearByIndex(local, iIndex);

			m_pFirstObj = pObj;
			m_pLastObj = pObj;
			m_nFirstIndex[0] = iIndex[0];
			m_nFirstIndex[1] = iIndex[1];
			m_nLastIndex[0] = iIndex[0];
			m_nLastIndex[1] = iIndex[1];
		}

		m_pPolyObj = new CGeoPolyObj(CRect(local, CSize(0, 0)),pDoc->m_pCurDrawLayer);
		m_pPolyObj->m_bEditing = TRUE;			// 处于编辑状态		
		
		m_pPolyObj->m_eLine.m_nPenStyle = 6;
		m_pPolyObj->m_eLine.m_bClose = TRUE;
		m_pPolyObj->m_eFill.m_bShow = TRUE;
		
		pView->Select(m_pPolyObj);

		m_pPolyObj->AddPoint(local, pView);

		local.x += 1;
		m_pPolyObj->AddPoint(local, pView);
	}
	else
	{
		CGeoDrawObj *pObj = pView->GetDocument()->HitOnBorder(local,pView,m_pPolyObj);
		if( pObj != NULL  )
		{
			int iIndex[2];
			local = pObj->GetNearByIndex(local, iIndex);

			LPPOINT lppt;
			int nCounts;
			if(pObj == m_pLastObj && nFlags&MK_CONTROL )
			{
				nCounts = m_pLastObj->GetPathPoint(lppt);
				if( nCounts > 0 )
				{
					if(m_nLastIndex[0]==-1)
					{
						if(m_nLastIndex[1]==0)
						{
							int i;
							for ( i=0;i<=iIndex[0];i++)
							{
								if( i== 0)
								{
									g_nDragHandle = m_pPolyObj->GetHandleCount();
									m_pPolyObj->MoveHandleTo(g_nDragHandle, lppt[i], pView);
								}
								else
									m_pPolyObj->AddPoint(lppt[i], pView);
							}
							g_nDragHandle = m_pPolyObj->GetHandleCount();
							m_pPolyObj->MoveHandleTo(g_nDragHandle, lppt[i-1], pView);
							local.x += 1;
							m_pPolyObj->AddPoint(local, pView);
						}
						else
						{
							int i;
							for ( i=m_nLastIndex[1];i>=iIndex[1];i--)
							{
								if( i== m_nLastIndex[1] )
								{
									g_nDragHandle = m_pPolyObj->GetHandleCount();
									m_pPolyObj->MoveHandleTo(g_nDragHandle, lppt[i], pView);
								}
								else
									m_pPolyObj->AddPoint(lppt[i], pView);
							}
							g_nDragHandle = m_pPolyObj->GetHandleCount();
							m_pPolyObj->MoveHandleTo(g_nDragHandle, lppt[i+1], pView);
							local.x += 1;
							m_pPolyObj->AddPoint(local, pView);
						}
					}
					else
					{
						if( m_nLastIndex[0] > iIndex[1] )
						{
							int i;
							for ( i=m_nLastIndex[0]; i>= iIndex[1] ;i-- )
							{
								if( i== m_nLastIndex[0] )
								{
									g_nDragHandle = m_pPolyObj->GetHandleCount();
									m_pPolyObj->MoveHandleTo(g_nDragHandle, lppt[i], pView);
								}
								else
									m_pPolyObj->AddPoint(lppt[i], pView);
							}
							g_nDragHandle = m_pPolyObj->GetHandleCount();
							m_pPolyObj->MoveHandleTo(g_nDragHandle, lppt[i+1], pView);
							local.x += 1;

							m_pPolyObj->AddPoint(local, pView);
						
						}
						else if( m_nLastIndex[1] < iIndex[0] )
						{
							int i;
							for ( i=m_nLastIndex[1]; i <= iIndex[0] ;i++ )
							{
								if( i== m_nLastIndex[1] )
								{
									g_nDragHandle = m_pPolyObj->GetHandleCount();
									m_pPolyObj->MoveHandleTo(g_nDragHandle, lppt[i], pView);
								}
								else
									m_pPolyObj->AddPoint(lppt[i], pView);
							
							}
							g_nDragHandle = m_pPolyObj->GetHandleCount();
							m_pPolyObj->MoveHandleTo(g_nDragHandle, lppt[i-1], pView);
							local.x += 1;
							m_pPolyObj->AddPoint(local, pView);						
						}
						else
						{
							g_nDragHandle = m_pPolyObj->GetHandleCount();
							m_pPolyObj->MoveHandleTo(g_nDragHandle, local, pView);
							local.x += 1;
							m_pPolyObj->AddPoint(local, pView);
						}
					}
					delete lppt;
				}
				else
				{
					g_nDragHandle = m_pPolyObj->GetHandleCount();
					m_pPolyObj->MoveHandleTo(g_nDragHandle, local, pView);
					local.x += 1;
					m_pPolyObj->AddPoint(local, pView);
				}
			}
			else if( pObj == m_pFirstObj
				&& nFlags&MK_SHIFT ) // 如果是第一个曲线
			{
				nCounts = m_pFirstObj->GetPathPoint(lppt);
				if( nCounts > 0 )
				{
					if(m_nFirstIndex[0]==-1)
					{
						if(m_nFirstIndex[1]==0)
						{
							int i;

							for ( i=iIndex[0];i>=0;i--)
							{
								if( i== iIndex[0])
								{
									g_nDragHandle = m_pPolyObj->GetHandleCount();
									m_pPolyObj->MoveHandleTo(g_nDragHandle, lppt[i], pView);
								}
								else
									m_pPolyObj->AddPoint(lppt[i], pView);
							}
							g_nDragHandle = m_pPolyObj->GetHandleCount();
							m_pPolyObj->MoveHandleTo(g_nDragHandle, lppt[i+1], pView);
							local.x += 1;
							m_pPolyObj->AddPoint(local, pView);
						}
						else
						{
							int i;
							for ( i=iIndex[1];i<=m_nFirstIndex[1];i++)
							{
								if( i== iIndex[1] )
								{
									g_nDragHandle = m_pPolyObj->GetHandleCount();
									m_pPolyObj->MoveHandleTo(g_nDragHandle, lppt[i], pView);
								}
								else
									m_pPolyObj->AddPoint(lppt[i], pView);
							}
							g_nDragHandle = m_pPolyObj->GetHandleCount();
							m_pPolyObj->MoveHandleTo(g_nDragHandle, lppt[i-1], pView);
							local.x += 1;
							m_pPolyObj->AddPoint(local, pView);
						}
					}
					else
					{
						if( m_nFirstIndex[0] > iIndex[1] )
						{
							int i;
							for ( i=iIndex[1]; i<=m_nFirstIndex[0]  ;i++ )
							{
								if( i== iIndex[1])
								{
									g_nDragHandle = m_pPolyObj->GetHandleCount();
									m_pPolyObj->MoveHandleTo(g_nDragHandle, lppt[i], pView);
								}
								else
									m_pPolyObj->AddPoint(lppt[i], pView);
							}
							g_nDragHandle = m_pPolyObj->GetHandleCount();
							m_pPolyObj->MoveHandleTo(g_nDragHandle, lppt[i-1], pView);
							local.x += 1;
							m_pPolyObj->AddPoint(local, pView);							
						}
						else if( m_nFirstIndex[1] < iIndex[0] )
						{
							int i;
							for ( i=iIndex[0]; i >= m_nFirstIndex[1] ;i-- )
							{
								if( i== iIndex[0] )
								{
									g_nDragHandle = m_pPolyObj->GetHandleCount();
									m_pPolyObj->MoveHandleTo(g_nDragHandle, lppt[i], pView);
								}
								else
									m_pPolyObj->AddPoint(lppt[i], pView);
								
							}
							g_nDragHandle = m_pPolyObj->GetHandleCount();
							m_pPolyObj->MoveHandleTo(g_nDragHandle, lppt[i+1], pView);
							local.x += 1;
							m_pPolyObj->AddPoint(local, pView);						
						}
						else
						{
							g_nDragHandle = m_pPolyObj->GetHandleCount();
							m_pPolyObj->MoveHandleTo(g_nDragHandle, local, pView);
							local.x += 1;

							m_pPolyObj->AddPoint(local, pView);
						}
					}
					delete lppt;
				}
				else
				{
					g_nDragHandle = m_pPolyObj->GetHandleCount();
					m_pPolyObj->MoveHandleTo(g_nDragHandle, local, pView);
					local.x += 1;

					m_pPolyObj->AddPoint(local, pView);
				}
			}
			else
			{
				g_nDragHandle = m_pPolyObj->GetHandleCount();
				m_pPolyObj->MoveHandleTo(g_nDragHandle, local, pView);
				local.x += 1;

				m_pPolyObj->AddPoint(local, pView);
			}
			m_pLastObj = (CGeoPolyObj*)pObj;

			m_nLastIndex[0] = iIndex[0];
			m_nLastIndex[1] = iIndex[1];
		}
		else 
		{
			g_nDragHandle = m_pPolyObj->GetHandleCount();
			m_pPolyObj->MoveHandleTo(g_nDragHandle, local, pView);
			local.x += 1;

			m_pPolyObj->AddPoint(local, pView);
			m_pLastObj = NULL;
			m_nLastIndex[0] = -1;
			m_nLastIndex[1] = -1;
		}

		//m_pPolyObj->AddPoint(local, pView);	
	}
	
	selectMode = sizeObj;
	g_nDragHandle = m_pPolyObj->GetHandleCount();
	lastPoint = local;
}

void CGeoToolRegion::OnLButtonDblClk(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	CToolBase::OnLButtonDblClk(pView, nFlags, point );
}

void CGeoToolRegion::OnLButtonUp(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	CToolBase::OnLButtonDown(pView, nFlags, point );
}

void CGeoToolRegion::OnMouseMove(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	CPoint local = point;
	pView->ClientToDoc(local);

	CGeoMapDoc *pDoc = pView->GetDocument();

	CGeoDrawObj *pObj = pView->GetDocument()->HitOnBorder(local,pView,m_pPolyObj);
	
	if( pView->m_bCapture )
	{
		if( pObj != NULL 
			&& pObj != m_pPolyObj )
		{
			int iIndex[2];
			local = pObj->GetNearByIndex(local, iIndex);
		}
	}

	if(m_pPolyObj != NULL)
	{
		
		if( pObj != NULL 
			&& pObj != m_pPolyObj)
		{
			if( pObj == m_pLastObj )
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
			else
				SetCursor(AfxGetApp()->LoadCursor(IDC_PENCIL));
		}
		else
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

		selectTool.OnMouseMove(pView, nFlags, point);
	}
	else
	{
		if( pObj != NULL )
		{
			SetCursor(AfxGetApp()->LoadCursor(IDC_PENCIL));
		}
		else
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));	
	
		CToolBase::OnMouseMove(pView, nFlags, point);
	}
}


void CGeoToolRegion::OnKeyDown(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags)
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
	case VK_SPACE: // 空格
		if( m_pPolyObj != NULL )
		{
			ReleaseCapture();

			CGeoMapDoc *pDoc = pView->GetDocument();
						
			int nPoints = m_pPolyObj->m_nPoints;
			if (nPoints > 2 &&
				(m_pPolyObj->m_points[nPoints - 1] == m_pPolyObj->m_points[nPoints - 2] ||
				m_pPolyObj->m_points[nPoints - 1].x - 1 == m_pPolyObj->m_points[nPoints - 2].x &&
				m_pPolyObj->m_points[nPoints - 1].y == m_pPolyObj->m_points[nPoints - 2].y))
				
			{
				m_pPolyObj->m_nPoints -= 1;
				pView->InvalObj(m_pPolyObj);
			}

			m_pPolyObj->m_nPoints --;

			if (m_pPolyObj->m_nPoints < 3)
			{
				pDoc->Remove(m_pPolyObj);
				pView->Select(NULL);
			}
			else
			{
				pDoc->Do(new CCmdAddDrawObj(pDoc, m_pPolyObj), TRUE);	// Do add stroke command				
				
				m_pPolyObj->m_bEditing = FALSE;
				m_pPolyObj->m_trackerState = CGeoDrawObj::active;
			}
			
			m_pPolyObj = NULL;
			c_drawShape = selection;
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

void CGeoToolRegion::OnKeyUp(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags)
{

}

void CGeoToolRegion::OnChar(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	
}

void CGeoToolRegion::OnCancel(CGeoMapView* pView)
{
	CToolBase::OnCancel(pView);
	m_pPolyObj = NULL;
}