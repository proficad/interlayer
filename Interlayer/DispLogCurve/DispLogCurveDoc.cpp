// LogCurveTabDoc.cpp : implementation file
//

#include "stdafx.h"
#include "DispLogCurveDoc.h"
#include "resource.h"
#include "DispLogCurveView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_GETITEMTITLE_GRAPH WM_USER+102

void CLogChartTool::OnLButtonDblClk(CDispLogCurveView* pView, UINT nFlags, const CPoint& point)
{
	CClientDC dc(pView);
	
	pView->OnPrepareDC(&dc);
	CRect cliRect;
	pView->GetClientRect(cliRect);
	dc.DPtoLP(cliRect);
	
	CRect rect = m_pLogChart->m_rectTitle;
	CSize size = m_pLogChart->GetChartSize();
	rect.OffsetRect(m_pLogChart->m_iMargin - (rect.Width() - size.cx)/2,0);
	
	int nXPos = pView->GetScrollPos(SB_HORZ);
	int nYPos = pView->GetScrollPos(SB_VERT);
	size.cx = nXPos;
	size.cy = nYPos;
	
	dc.DPtoLP(&size);
	
	int nCount = m_pLogChart->m_chartLanes.GetSize();
	
	rect.right = rect.left;
	CRect rt;  // 调整条
	CRect rt1; // 曲线标题

	for (int i=0;i<nCount;i++)
	{
		CLogCurveLane *pLane = (CLogCurveLane *)m_pLogChart->m_chartLanes.GetAt(i);
		int width = pLane->m_rectLane.Width();
		rect.left = rect.right; 
		rect.right = rect.left + width;
		
		// 调整条
		rt = rect; 
		rt.top = 0;
		rt.bottom = 30;
		rt.top += size.cy;
		rt.bottom += size.cy;
		
		// 曲线标题
		rt1 = rect; 
		rt1.top = 50 + 30;
		rt1.bottom = 50 + 30 + m_pLogChart->m_rectTitle.Height();
		if( m_pLogChart->m_bDispCaption )
		{
			rt1.top += m_pLogChart->m_chartCaption.m_rectCaption.Height();
			rt1.bottom += m_pLogChart->m_chartCaption.m_rectCaption.Height();
		}

		rt1.top += size.cy;
		rt1.bottom += size.cy;

		dc.LPtoDP(rt);
		dc.LPtoDP(rt1);
		
		if( rt.PtInRect(point) )
		{
			pLane->OnEditProperties(pView);
			return;
		}

		//////////////////////////////////////////////////////////////////////////
		// 在某一曲线上
		else if ( rt1.PtInRect(point) && pLane->m_nLaneType == CLogCurveLane::LOGCURVE)
		{
			int n = pLane->m_arLoging.GetSize();
			if( n > 0 )
			{
				int height = rt1.Height() / n;
				CRect rr = rt1;
				for (int j=0;j<n;j++ )
				{
					rr.top = rt1.top + j*height;
					rr.bottom = rr.top + height;
					if( rr.PtInRect(point))
					{
						pLane->m_arLoging[j].OnEditProperties(pView);
						return;
					}
				}
			}			
		}
		else if(point.x >= rt.left 
			&& point.x <= rt.right)
		{
			pLane->OnEditProperties(pView);
			return;
		}
	}
	
	return;
}

void CLogChartTool::OnLButtonDown(CDispLogCurveView* pView, UINT nFlags, const CPoint& point)
{
	if( m_pLogChart->m_logDat.GetLogCount() < 1 )
		return;
	
	if( m_bEditing )
		EndEdit();

	if( m_pLaneSelected )
		m_pLaneSelected->m_bSelected = FALSE;
	if( m_pCurveSelected )
		m_pCurveSelected->m_bSelected = FALSE;
				
	m_pLaneSelected = NULL;
	m_pCurveSelected = NULL;
				
	pView->Invalidate(FALSE);

	CClientDC dc(pView);
	
	pView->OnPrepareDC(&dc);
	
	CSize size = m_pLogChart->GetChartSize();
	int nXPos = pView->GetScrollPos(SB_HORZ);
	int nYPos = pView->GetScrollPos(SB_VERT);
	CSize sizePos(nXPos,nYPos);

	dc.DPtoLP(&sizePos);

	{ //测试是否点击了测井曲线图标题
		if( m_pLogChart->m_bDispCaption )
		{
			CRect rect(0,0,size.cx,150);
			
			rect.OffsetRect(m_pLogChart->m_iMargin - (rect.Width() - size.cx)/2,0);
			
			rect.top += sizePos.cy;
			rect.bottom += sizePos.cy;
			
			rect.DeflateRect(0,30,0,-30);
			dc.LPtoDP(rect);
			if( rect.PtInRect(point) )
			{
				Edit(-1,rect,VK_LBUTTON,pView);
				return;
			}
		}
	}

	{ // 测试是否点击了泳道或曲线
		CRect cliRect;
		pView->GetClientRect(cliRect);
		dc.DPtoLP(cliRect);
		
		CRect rect = m_pLogChart->m_rectTitle;
		
		rect.OffsetRect(m_pLogChart->m_iMargin - (rect.Width() - size.cx)/2,0);
		
		int nCount = m_pLogChart->m_chartLanes.GetSize();
		
		rect.right = rect.left;
		CRect rt;  // 调整条
		CRect rt1; // 曲线标题
		
		for (int i=0;i<nCount;i++)
		{
			CLogCurveLane *pLane = (CLogCurveLane *)m_pLogChart->m_chartLanes.GetAt(i);
			int width = pLane->m_rectLane.Width();
			rect.left = rect.right; 
			rect.right = rect.left + width;
			
			// 调整条
			rt = rect; 
			rt.top = 0;
			rt.bottom = 30;
			rt.top += sizePos.cy;
			rt.bottom += sizePos.cy;
			
			// 曲线标题
			rt1 = rect; 
			rt1.top = 50 + 30;
			rt1.bottom = 50 + 30 + m_pLogChart->m_rectTitle.Height();
			if( m_pLogChart->m_bDispCaption )
			{
				rt1.top += m_pLogChart->m_chartCaption.m_rectCaption.Height();
				rt1.bottom += m_pLogChart->m_chartCaption.m_rectCaption.Height();
			}
			
			rt1.top += sizePos.cy;
			rt1.bottom += sizePos.cy;
			
			dc.LPtoDP(rt);
			dc.LPtoDP(rt1);
			
			//////////////////////////////////////////////////////////////////////////
			// 在某一泳道边上
			if( point.x >= rt.left -2 
				&& point.x <= rt.left +2 
				&& point.y >= rt.top
				&& point.y <= rt.bottom)
			{			
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
				m_ptDown = point;
				m_ptLast = point;
				CPoint pt = point;
				dc.DPtoLP(&pt);
				dc.SetROP2(R2_NOTXORPEN);
				dc.MoveTo(pt.x, cliRect.top);
				dc.LineTo(pt.x, cliRect.bottom);
				pView->SetCapture();
				m_toolMode = SelectMode::sizehorz;
				m_iIndex = -1;
				return;
			}
			else if(point.x >= rt.right -2 
				&& point.x <= rt.right +2
				&& point.y >= rt.top
				&& point.y <= rt.bottom)
			{
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
				m_ptDown = point;
				m_ptLast = point;
				CPoint pt = point;
				dc.DPtoLP(&pt);
				dc.SetROP2(R2_NOTXORPEN);
				dc.MoveTo(pt.x, cliRect.top);
				dc.LineTo(pt.x, cliRect.bottom);
				pView->SetCapture();
				m_toolMode = SelectMode::sizehorz;
				m_iIndex = i;
				return;
			}
			
			//////////////////////////////////////////////////////////////////////////
			// 在某一泳道上
			else if( rt.PtInRect(point) )
			{
				if( m_pLaneSelected )
					m_pLaneSelected->m_bSelected = FALSE;
				if( m_pCurveSelected )
					m_pCurveSelected->m_bSelected = FALSE;
				
				m_pCurveSelected = NULL;
				
				m_pLaneSelected = pLane;
				m_pLaneSelected->m_bSelected = TRUE;
				pView->Invalidate(FALSE);
				pView->SetCapture();
				m_toolMode = SelectMode::drag;
				m_bDragLane = TRUE;
				pView->OnBeginDrag(TRUE,i,0,point);
				m_iLaneDrag = i;
				m_iLaneDragOver = i;
				return;
			}
			
			//////////////////////////////////////////////////////////////////////////
			// 在某一曲线上
			else if ( rt1.PtInRect(point) && pLane->m_nLaneType == CLogCurveLane::LOGCURVE)
			{
				int n = pLane->m_arLoging.GetSize();
				if( n > 0 )
				{
					int height = rt1.Height() / n;
					CRect rr = rt1;
					for (int j=0;j<n;j++ )
					{
						rr.top = rt1.top + j*height;
						rr.bottom = rr.top + height;
						if( rr.PtInRect(point))
						{
							if( m_pLaneSelected )
								m_pLaneSelected->m_bSelected = FALSE;
							if( m_pCurveSelected )
								m_pCurveSelected->m_bSelected = FALSE;
							
							m_pLaneSelected = NULL;
							m_pCurveSelected = &pLane->m_arLoging[j];
							m_pCurveSelected->m_bSelected = TRUE;
							pView->Invalidate(FALSE);
							pView->SetCapture();
							m_toolMode = SelectMode::drag;
							m_bDragLane = FALSE;
							pView->OnBeginDrag(FALSE,i,j,point);
							m_iLaneDrag = i;
							m_iCurveDrag = j;
							m_iLaneDragOver = i;
							return;
						}
					}
				}			
			}

			// 曲线标题的底
			else if(point.y >= rt1.bottom - 2 && point.y <= rt1.bottom + 2)
			{
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));
				m_ptDown = point;
				m_ptLast = point;
				CPoint pt = point;
				dc.DPtoLP(&pt);
				dc.SetROP2(R2_NOTXORPEN);
				dc.MoveTo(cliRect.left, pt.y);
				dc.LineTo(cliRect.right, pt.y);
				pView->SetCapture();
				m_toolMode = SelectMode::sizevert;
				return;
			}
			else if(point.x >= rt.left 
				&& point.x <= rt.right)
			{
				if( m_pLaneSelected )
					m_pLaneSelected->m_bSelected = FALSE;
				if( m_pCurveSelected )
					m_pCurveSelected->m_bSelected = FALSE;
				
				m_pCurveSelected = NULL;
				
				m_pLaneSelected = pLane;
				m_pLaneSelected->m_bSelected = TRUE;
				pView->Invalidate(FALSE);
				return;
			}
		}
	}	
}

void CLogChartTool::OnLButtonUp(CDispLogCurveView* pView, UINT nFlags, const CPoint& point)
{
	if( m_pLogChart->m_logDat.GetLogCount() < 1 )
		return;
	if(  m_toolMode == SelectMode::sizehorz )
	{
		CClientDC dc(pView);	
		pView->OnPrepareDC(&dc);
		
		CRect cliRect;
		pView->GetClientRect(cliRect);
		dc.DPtoLP(cliRect);

		ReleaseCapture();
		dc.SetROP2(R2_NOTXORPEN);
		CPoint pt = m_ptDown;
		
		dc.DPtoLP(&pt);				
		dc.MoveTo(pt.x, cliRect.top);
		dc.LineTo(pt.x, cliRect.bottom);
		if( point != m_ptLast)
		{
			CSize size = point - m_ptLast;
			dc.DPtoLP(&size);
			if( m_iIndex > -1 )
			{
				m_pLogChart->ChangeLaneWidth(m_iIndex, size.cx, pView);
			}
			else
			{
				m_pLogChart->m_iMargin += size.cx;
				if( m_pLogChart->m_iMargin < 4 )
					m_pLogChart->m_iMargin = 4;
			}

			pView->GetDocument()->SetModifiedFlag();
			pView->GetDocument()->UpdateAllViews(NULL, HINT_UPDATE_CHANGEDSIZE);
			pView->Invalidate(FALSE);
		}		
		m_toolMode = SelectMode::none;
	}
	else if(  m_toolMode == SelectMode::sizevert )
	{
		CClientDC dc(pView);	
		pView->OnPrepareDC(&dc);
		
		CRect cliRect;
		pView->GetClientRect(cliRect);
		dc.DPtoLP(cliRect);

		ReleaseCapture();
		dc.SetROP2(R2_NOTXORPEN);
		CPoint pt = m_ptDown;
		
		dc.DPtoLP(&pt);				
		dc.MoveTo(cliRect.left, pt.y);
		dc.LineTo(cliRect.left, pt.y);
		if( point != m_ptLast)
		{
			CSize size = point - m_ptLast;
			dc.DPtoLP(&size);
			m_pLogChart->m_iTitleHeight += size.cy;
			if(m_pLogChart->m_iTitleHeight <= 0 )
				m_pLogChart->m_iTitleHeight = 100;
			m_pLogChart->ReCaleAllRect();

			pView->GetDocument()->SetModifiedFlag();
			pView->GetDocument()->UpdateAllViews(NULL, HINT_UPDATE_CHANGEDSIZE);
			pView->Invalidate(FALSE);
 		}		
		m_toolMode = SelectMode::none;
	}
	else if( m_toolMode == SelectMode::drag )
	{
		ReleaseCapture();		
		pView->m_pDragImageList->EndDrag();
		delete pView->m_pDragImageList;
		pView->m_pDragImageList = NULL;
		m_toolMode = SelectMode::none;

		if( m_iLaneDragOver != -1 && m_iLaneDragOver != m_iLaneDrag )
		{
			if( m_bDragLane ) // 拖动泳道
			{
				CLogCurveLane *pLane = (CLogCurveLane *)m_pLogChart->m_chartLanes.GetAt(m_iLaneDrag);
				m_pLogChart->m_chartLanes.RemoveAt(m_iLaneDrag);
				m_pLogChart->m_chartLanes.InsertAt(m_iLaneDragOver,(DWORD)pLane);				
				pView->Invalidate(FALSE);
				pView->GetDocument()->SetModifiedFlag();
			}
			else				// 拖动曲线
			{
				CLogCurveLane *pLaneOver = (CLogCurveLane *)m_pLogChart->m_chartLanes.GetAt(m_iLaneDragOver);
				if( pLaneOver->m_nLaneType == CLogCurveLane::LOGCURVE )
				{
					CLogCurveLane *pLane = (CLogCurveLane *)m_pLogChart->m_chartLanes.GetAt(m_iLaneDrag);
					CLogCurveLane::strLOGINGPROP prop = pLane->m_arLoging[m_iCurveDrag];
					pLane->m_arLoging.RemoveAt(m_iCurveDrag);
					pLaneOver->AddCurve(prop, pView);					
					m_pCurveSelected = NULL;
				}
			}
		}
	}
}

void CLogChartTool::OnMouseMove(CDispLogCurveView* pView, UINT nFlags, const CPoint& point)
{
	if( m_pLogChart->m_logDat.GetLogCount() < 1 )
		return;

	if( m_toolMode == SelectMode::drag )
	{
		pView->m_pDragImageList->DragEnter(pView, point);
		pView->m_pDragImageList->DragMove(point);
		
		CClientDC dc(pView);
		
		pView->OnPrepareDC(&dc);
		CRect cliRect;
		pView->GetClientRect(cliRect);
		dc.DPtoLP(cliRect);
		
		CRect rect = m_pLogChart->m_rectTitle;
		CSize size = m_pLogChart->GetChartSize();
		rect.OffsetRect(m_pLogChart->m_iMargin - (rect.Width() - size.cx)/2,0);
		
		int nXPos = pView->GetScrollPos(SB_HORZ);
		int nYPos = pView->GetScrollPos(SB_VERT);
		size.cx = nXPos;
		size.cy = nYPos;
		
		dc.DPtoLP(&size);
		
		int nCount = m_pLogChart->m_chartLanes.GetSize();
		
		rect.right = rect.left;
		CRect rt;  // 调整条
		for (int i=0;i<nCount;i++)
		{
			CLogCurveLane *pLane = (CLogCurveLane *)m_pLogChart->m_chartLanes.GetAt(i);
			int width = pLane->m_rectLane.Width();
			rect.left = rect.right; 
			rect.right = rect.left + width;
			
			// 调整条
			rt = rect; 
			rt.top = 0;
			rt.bottom = 30;	
			rt.top += size.cy;
			rt.bottom += size.cy;

			dc.LPtoDP(rt);
			
			//////////////////////////////////////////////////////////////////////////
			// 在某一泳道边上
			if( point.x >= rt.left 
				&& point.x <= rt.right)
			{				
				m_iLaneDragOver = i;
				break;
			}
			else
				m_iLaneDragOver = -1;
		}
	}
	else if( nFlags & MK_LBUTTON && m_toolMode == SelectMode::sizehorz)
	{
		CClientDC dc(pView);
		
		pView->OnPrepareDC(&dc);
		
		CRect cliRect;
		pView->GetClientRect(cliRect);
		dc.DPtoLP(cliRect);
		
		dc.SetROP2(R2_NOTXORPEN);
		CPoint pt1 = m_ptDown;
		CPoint pt = point;
		dc.DPtoLP(&pt1);
		dc.MoveTo(pt1.x, cliRect.top);
		dc.LineTo(pt1.x, cliRect.bottom);
		dc.DPtoLP(&pt);				
		dc.MoveTo(pt.x, cliRect.top);
		dc.LineTo(pt.x, cliRect.bottom);
		m_ptDown = point;
	}
	else if( nFlags & MK_LBUTTON && m_toolMode == SelectMode::sizevert)
	{
		CClientDC dc(pView);
		
		pView->OnPrepareDC(&dc);
		
		CRect cliRect;
		pView->GetClientRect(cliRect);
		dc.DPtoLP(cliRect);
		
		dc.SetROP2(R2_NOTXORPEN);
		CPoint pt1 = m_ptDown;
		CPoint pt = point;
		dc.DPtoLP(&pt1);
		dc.MoveTo(cliRect.left, pt1.y);
		dc.LineTo(cliRect.right, pt1.y);
		dc.DPtoLP(&pt);				
		dc.MoveTo(cliRect.left, pt.y);
		dc.LineTo(cliRect.right, pt.y);
		m_ptDown = point;
	}
	else
	{
		CClientDC dc(pView);
		
		pView->OnPrepareDC(&dc);
		
		CRect cliRect;
		pView->GetClientRect(cliRect);
		dc.DPtoLP(cliRect);
		
		CRect rect = m_pLogChart->m_rectTitle;
		rect.top = 0;
		rect.bottom = 30;
		CSize size = m_pLogChart->GetChartSize();
		rect.OffsetRect(m_pLogChart->m_iMargin - (rect.Width() - size.cx)/2,0);
		int nXPos = pView->GetScrollPos(SB_HORZ);
		int nYPos = pView->GetScrollPos(SB_VERT);
		size.cx = nXPos;
		size.cy = nYPos;
		
		dc.DPtoLP(&size);

		rect.top += size.cy;
		rect.bottom += size.cy;
		
		int nCount = m_pLogChart->m_chartLanes.GetSize();
		
		rect.right = rect.left;
		CRect rt;
		for (int i=0;i<nCount;i++)
		{
			CLogCurveLane *pLane = (CLogCurveLane *)m_pLogChart->m_chartLanes.GetAt(i);
			int width = pLane->m_rectLane.Width();
			rect.left = rect.right; 
			rect.right = rect.left + width;
			
			rt = rect;
			dc.LPtoDP(rt);
			if( point.x >= rt.left -2 
				&& point.x <= rt.left +2
				&& point.y >= rt.top
				&& point.y <= rt.bottom)
			{
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));			
				return;
			}
			else if(point.x >= rt.right -2 
				&& point.x <= rt.right +2 
				&& point.y >= rt.top
				&& point.y <= rt.bottom)
			{
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
				return;
			}
		}

		rect = m_pLogChart->m_rectTitle;
		
		// 曲线标题
		rect.top = 50 + 30;
		rect.bottom = 50 + 30 + m_pLogChart->m_rectTitle.Height();
		if( m_pLogChart->m_bDispCaption )
		{
			rect.top += m_pLogChart->m_chartCaption.m_rectCaption.Height();
			rect.bottom += m_pLogChart->m_chartCaption.m_rectCaption.Height();
		}
		
		
		rect.top += size.cy;
		rect.bottom += size.cy;
		
		dc.LPtoDP(rect);

		if(point.y >= rect.bottom - 2 && point.y <= rect.bottom + 2)
		{
			SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));
			return;
		}
	}	
}

void CLogChartTool::OnRButtonDown(CDispLogCurveView* pView, UINT nFlags, const CPoint& point)
{
	if( m_pLogChart->m_logDat.GetLogCount() < 1 )
		return;
	
	if( m_bEditing )
		EndEdit();

	if( m_pLaneSelected )
		m_pLaneSelected->m_bSelected = FALSE;
	if( m_pCurveSelected )
		m_pCurveSelected->m_bSelected = FALSE;
				
	m_pLaneSelected = NULL;
	m_pCurveSelected = NULL;
				
	pView->Invalidate(FALSE);

	CClientDC dc(pView);
	
	pView->OnPrepareDC(&dc);
	
	CSize size = m_pLogChart->GetChartSize();
	int nXPos = pView->GetScrollPos(SB_HORZ);
	int nYPos = pView->GetScrollPos(SB_VERT);
	CSize sizePos(nXPos,nYPos);

	dc.DPtoLP(&sizePos);

	{ // 测试是否点击了泳道或曲线
		CRect cliRect;
		pView->GetClientRect(cliRect);
		dc.DPtoLP(cliRect);
		
		CRect rect = m_pLogChart->m_rectTitle;
		
		rect.OffsetRect(m_pLogChart->m_iMargin - (rect.Width() - size.cx)/2,0);
		
		int nCount = m_pLogChart->m_chartLanes.GetSize();
		
		rect.right = rect.left;
		CRect rt;  // 调整条
		CRect rt1; // 曲线标题
		
		for (int i=0;i<nCount;i++)
		{
			CLogCurveLane *pLane = (CLogCurveLane *)m_pLogChart->m_chartLanes.GetAt(i);
			int width = pLane->m_rectLane.Width();
			rect.left = rect.right; 
			rect.right = rect.left + width;
			
			// 调整条
			rt = rect; 
			rt.top = 0;
			rt.bottom = 30;
			rt.top += sizePos.cy;
			rt.bottom += sizePos.cy;
			
			// 曲线标题
			rt1 = rect; 
			rt1.top = 50 + 30;
			rt1.bottom = 50 + 30 + m_pLogChart->m_rectTitle.Height();
			if( m_pLogChart->m_bDispCaption )
			{
				rt1.top += m_pLogChart->m_chartCaption.m_rectCaption.Height();
				rt1.bottom += m_pLogChart->m_chartCaption.m_rectCaption.Height();
			}
			
			
			rt1.top += sizePos.cy;
			rt1.bottom += sizePos.cy;
			
			dc.LPtoDP(rt);
			dc.LPtoDP(rt1);
			
			//////////////////////////////////////////////////////////////////////////
			// 在某一泳道上
			if( rt.PtInRect(point) )
			{
				if( m_pLaneSelected )
					m_pLaneSelected->m_bSelected = FALSE;
				if( m_pCurveSelected )
					m_pCurveSelected->m_bSelected = FALSE;
				
				m_pCurveSelected = NULL;
				
				m_pLaneSelected = pLane;
				m_pLaneSelected->m_bSelected = TRUE;
				pView->Invalidate(FALSE);
				
				return;
			}
			
			//////////////////////////////////////////////////////////////////////////
			// 在某一曲线上



			else if ( rt1.PtInRect(point) )
			{
				if( m_pLaneSelected )
					m_pLaneSelected->m_bSelected = FALSE;
				if( m_pCurveSelected )
					m_pCurveSelected->m_bSelected = FALSE;
				
				m_pLaneSelected = pLane;
				m_pLaneSelected->m_bSelected = TRUE;
				
				if( pLane->m_nLaneType == CLogCurveLane::LOGCURVE )
				{
					int n = pLane->m_arLoging.GetSize();
					if( n > 0 )
					{
						int height = rt1.Height() / n;
						CRect rr = rt1;
						for (int j=0;j<n;j++ )
						{
							rr.top = rt1.top + j*height;
							rr.bottom = rr.top + height;
							if( rr.PtInRect(point))
							{		
								m_pCurveSelected = &pLane->m_arLoging[j];
								m_pCurveSelected->m_bSelected = TRUE;
								m_iCurveDrag = j;
								return;
							}
						}
					}
				}

				pView->Invalidate(FALSE);

				return;			
			}
		}
	}	
}

void CLogChartTool::OnKeyDown(CDispLogCurveView* pView, UINT nChar, UINT nRepCnt, UINT nFlags) 
{	
	switch(nChar)
	{
	case VK_ESCAPE:
		if( m_toolMode == SelectMode::drag )
		{
			ReleaseCapture();
			pView->m_pDragImageList->DragLeave(NULL);
			pView->m_pDragImageList->EndDrag();
			delete pView->m_pDragImageList;
			pView->m_pDragImageList = NULL;
			m_toolMode = SelectMode::none;
		}		
		break;
	case VK_DELETE:
		if( m_pLaneSelected )
		{
			if( m_toolMode == SelectMode::drag )
			{
				ReleaseCapture();
				pView->m_pDragImageList->DragLeave(NULL);
				pView->m_pDragImageList->EndDrag();
				delete pView->m_pDragImageList;
				pView->m_pDragImageList = NULL;
				m_toolMode = SelectMode::none;
			}
			m_pLogChart->Del(m_iLaneDrag, pView);
			m_pLaneSelected = NULL;
		}
		else if( m_pCurveSelected )
		{
			if( m_toolMode == SelectMode::drag )
			{
				ReleaseCapture();
				pView->m_pDragImageList->DragLeave(NULL);
				pView->m_pDragImageList->EndDrag();
				delete pView->m_pDragImageList;
				pView->m_pDragImageList = NULL;
				m_toolMode = SelectMode::none;
			}
			CLogCurveLane *pLane = (CLogCurveLane *)m_pLogChart->m_chartLanes.GetAt(m_iLaneDrag);
			pLane->DelCurve(m_iCurveDrag, pView);
			m_pCurveSelected = NULL;
		}
	default:
		break;
	}
}

void CLogChartTool::Edit(int nIndex, CRect rect,UINT nChar, CDispLogCurveView* pView)
{
	if ( m_bEditing )
	{      
        if (m_pEditWnd)
			m_pEditWnd->SendMessage ( WM_CHAR, nChar );    
    }  
	else  
	{   
		DWORD dwStyle = ES_CENTER|ES_MULTILINE|ES_WANTRETURN;
		
		m_bEditing = TRUE;
		
		CClientDC dc(pView);
		
		pView->OnPrepareDC(&dc);
		CSize size;
		CString str;
		if(nIndex == -1)
		{
			size.cx = 60;
			size.cy = 0;
			str = m_pLogChart->m_chartCaption.m_strTitle;
		}

		dc.LPtoDP(&size);

		m_pEditWnd = new dispLogCurve::CInPlaceEdit(pView, rect, dwStyle, IDC_INPLACE_CONTROL, nIndex, str , nChar);
		
		m_pEditFont = new CFont;
		m_pEditFont->CreateFont(size.cx, 0, 0, 0, 0, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, "仿宋_GB2312");
		
		ASSERT(m_pEditFont);
        if (m_pEditFont)
            m_pEditWnd->SetFont(m_pEditFont);
    }
    return;
}

void CLogChartTool::EndEdit()
{
	if (m_pEditWnd)
	{
		CString str;
		m_pEditWnd->GetWindowText(str);
		if(str != m_pEditWnd->m_sInitText )
		{
			if( m_pEditWnd->m_nIndex == -1 )
				m_pLogChart->m_chartCaption.m_strTitle = str;
			CDispLogCurveView* pView = (CDispLogCurveView*) m_pEditWnd->GetParent();
			pView->GetDocument()->SetModifiedFlag();
		}
        m_pEditWnd->EndEdit();
	}

	if( m_pEditFont)
	{
		m_pEditFont->DeleteObject();
		delete m_pEditFont;
		m_pEditFont = NULL;
	}

    m_bEditing = FALSE;
    m_pEditWnd = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CDispLogCurveDoc

IMPLEMENT_DYNCREATE(CDispLogCurveDoc, CDocument)

CDispLogCurveDoc::CDispLogCurveDoc()
{
	m_chartLogTool.m_pLogChart = &m_chartLog;
	m_strWellName = _T("");
	GetPrinterPageSize(m_sizeDoc);
}

BOOL CDispLogCurveDoc::OnNewDocument()
{

	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}

CSize CDispLogCurveDoc::GetPageSize()
{
	return m_sizeDoc;
}

BOOL CDispLogCurveDoc::SaveModified()
{
	// TODO: 在此添加专用代码和/或调用基类

	if (!IsModified())
		return TRUE;        // ok to continue

	// get name/title of document
	CString name = _T(" [");
	name += m_strTitle;
	name += _T("] ");

	CString prompt;
	AfxFormatString1(prompt, AFX_IDP_ASK_TO_SAVE, name);
	switch (AfxMessageBox(prompt, MB_YESNOCANCEL, AFX_IDP_ASK_TO_SAVE))
	{
	case IDCANCEL:
		return FALSE;       // don't continue

	case IDYES:
		// If so, either Save or Update, as appropriate
		if (!DoFileSave())
			return FALSE;       // don't continue
		break;

	case IDNO:
		// If not saving changes, revert the document
		break;

	default:
		ASSERT(FALSE);
		break;
	}
	return TRUE;    // keep going
}

void CDispLogCurveDoc::GetPrinterPageSize(CSize &pageSize)
{
	CPrintInfo PrintInfo;
	pageSize = CSize(2100, 2960);		// default to A4
	if (AfxGetApp()->GetPrinterDeviceDefaults(&PrintInfo.m_pPD->m_pd))
	{
		if (PrintInfo.m_pPD->CreatePrinterDC() != NULL)
		{
			if (PrintInfo.m_pPD->m_pd.hDC == NULL)
				PrintInfo.m_pPD->CreatePrinterDC();

			// attach printer dc and retreive docSize
			CDC dcPrint;
			dcPrint.Attach(PrintInfo.m_pPD->m_pd.hDC);
			pageSize.cx = 10 * dcPrint.GetDeviceCaps(HORZSIZE);
			pageSize.cy = 10 * dcPrint.GetDeviceCaps(VERTSIZE);
		}
	}
}

CDispLogCurveDoc::~CDispLogCurveDoc()
{
}


BEGIN_MESSAGE_MAP(CDispLogCurveDoc, CDocument)
	//{{AFX_MSG_MAP(CDispLogCurveDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDispLogCurveDoc diagnostics

#ifdef _DEBUG
void CDispLogCurveDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDispLogCurveDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDispLogCurveDoc serialization

void CDispLogCurveDoc::Serialize(CArchive& ar)
{
	m_chartLog.Serialize(ar);
	if (ar.IsStoring())
	{
		ar << m_strWellName;
	}
	else
	{
		ar >> m_strWellName;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDispLogCurveDoc commands


BOOL CDispLogCurveDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	
	// TODO: Add your specialized creation code here
	
	return TRUE;
}

BOOL CDispLogCurveDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDocument::OnSaveDocument(lpszPathName);
}

void CDispLogCurveDoc::SetTitle(LPCTSTR lpszTitle)
{
	// TODO: 在此添加专用代码和/或调用基类

	CString strFileName = GetPathName();

	CWnd *pFrame = AfxGetMainWnd();

	TCHAR strTitle[1024];
	
	if( pFrame->SendMessage(WM_GETITEMTITLE_GRAPH,(WPARAM)(LPCTSTR)strFileName,(LPARAM)strTitle) )
		m_strTitle = strTitle;
	else
		m_strTitle = lpszTitle;

	UpdateFrameCounts();
}
