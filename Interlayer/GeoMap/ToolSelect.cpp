// ToolSelect.cpp: implementation of the CToolSelect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ToolSelect.h"
#include "GeoMapView.h"
#include "GeoDrawObj.h"
#include "GeoPolyObj.h"
#include "CmdGeoDrawObj.h"
#include "../Resource.h"
#include "GeoLineObj.h"
#include "GeoPictrueObj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CGeoToolSelect selectTool;

extern void StatusBarMessage(char* fmt, ...);
extern void SetStatusCenter(double x, double y, LPCSTR lpStr);

CToolBase::SelectMode selectMode = CToolBase::none;

int g_nDragHandle;		// ��ǰҪ�ı��Сʱ������ֱ�

CPoint lastPoint; // ���һ������
CPoint firstDownPoint;

CRect c_RectLast;
CRect c_RectDown;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeoToolSelect::CGeoToolSelect()
:CToolBase(CToolBase::selection)
{
	m_bEnableMove = FALSE;
}

CGeoToolSelect::~CGeoToolSelect()
{

}

void CGeoToolSelect::OnLButtonDown(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	CPoint local = point;
	pView->ClientToDoc(local);

	CGeoDrawObj* pObj;
	selectMode = none;

	// ��ֻ��һ��ͼ�ر�ѡ��ʱ,�ж��Ƿ���Ըı��ͼ�صĴ�С
	if (pView->m_selection.GetCount() == 1)
	{
		pObj = pView->m_selection.GetHead();
		
		g_nDragHandle = pObj->HitTestHandle(local, pView, TRUE); // ����ĳѡ�ж�����ֱ�
		if (g_nDragHandle != 0)		// �����ĳһ�ֱ��ϣ�
		{
			selectMode = sizeObj;		// ����ѡ��ģʽΪ�ı��Сģʽ
			SetCursor(pObj->GetHandleCursor(g_nDragHandle));
			c_RectDown = pObj->m_rtPosition;
			local = pObj->GetHandle(g_nDragHandle);
			pObj->m_bSizeChanging = TRUE;
		}
	}
	
	if (selectMode == none) // ���û�����������ѡ������ռ����ж���һ������
	{
		// �����Ƿ�����ĳ��������
		pObj = pView->GetDocument()->ObjectAt(local,pView);
		
		if (pObj != NULL) // ��ĳ��������
		{
			selectMode = moveObj; // �����ƶ�����ģʽ��
			
			if (!pView->IsSelected(pObj)) // �˶����������ѡ������ռ�����
				// �������Ctrl�����ԭѡ������ռ����е����ݣ�
				// ����Ӵ˶����ռ�����
				pView->Select(pObj, (nFlags & MK_CONTROL) != 0);
			else // ������ռ����������øö���Ϊ����״̬
				pView->SetSelectActive(pObj);
			
			// �������SHIFT�����ɿ�¡�ռ������ж���
			// ����CDrawWell��)
			if ((nFlags & MK_SHIFT) != 0)
				pView->CloneSelection();

			c_RectDown = pObj->m_rtPosition;

			if(m_bEnableMove) 
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));

			pObj->m_bMoving = TRUE;
		}
	}
	
	// ������ڱ����£�û�е��ڶ����ϣ�ʱ��ʹ������ѡģʽ
	if (selectMode == none)
	{
		// û�а���Ctrl��ʱ�����ѡ�ж����ռ���
		// �������ѡ�ж����ռ���
		if ((nFlags & MK_CONTROL ) == 0) 
			pView->Select(NULL);
		
		selectMode = netSelect; // ʹ������ѡģʽ��
		
		CClientDC dc(pView);
		CRect rect(point.x, point.y, point.x, point.y);
		rect.NormalizeRect();
		dc.DrawFocusRect(rect);
	}
	
	lastPoint = local;
	firstDownPoint = local;

	CToolBase::OnLButtonDown(pView, nFlags, point);
}

void CGeoToolSelect::OnLButtonDblClk(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	if ((nFlags & MK_CONTROL) != 0)
	{
		// Ctrl+˫����ѡ����
		CPoint local = point;
		pView->ClientToDoc(local);

		CGeoDrawObj* pObj = pView->GetDocument()->ObjectAt(local,pView);
		if (pObj != NULL)
			pView->DeSelect(pObj);
	}
	else
	{
		// ˫��һ������༭��������
		if (pView->m_selection.GetCount() == 1)
		{
			CGeoDrawObj* pObj = pView->m_selection.GetHead();
			pObj->OnEditProperties(pView);			
		}
		else if (pView->m_selection.GetCount() > 1)
		{
			CGeoDrawObj* pFirstObj = pView->m_selection.GetHead();
			if( pFirstObj->OnEditProperties(pView) )
			{
				CRuntimeClass *pRuntimeClass = pFirstObj->GetRuntimeClass();
				CString strName = pRuntimeClass->m_lpszClassName;
				POSITION pos = pView->m_selection.GetHeadPosition();
				while (pos != NULL)
				{
					CGeoDrawObj* pObj = pView->m_selection.GetNext(pos);
					if( pObj == pFirstObj)
						continue;
					
					CRuntimeClass *pRuntimeClass = pObj->GetRuntimeClass();
					CString str = pRuntimeClass->m_lpszClassName;
					if( str == strName )
						pObj->CopyAttribute(pFirstObj);
				}
			}			
		}
	}

	CToolBase::OnLButtonDblClk(pView, nFlags, point);
}

void CGeoToolSelect::OnEditProperties(CGeoMapView* pView)
{
	if (pView->m_selection.GetCount() == 1)
		pView->m_selection.GetHead()->OnEditProperties(pView);
}

void CGeoToolSelect::OnLButtonUp(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	if (pView->GetCapture() == pView)
	{
		if (selectMode == netSelect) // ��ѡ����ģʽ
		{
			CClientDC dc(pView);
			CRect rect(c_down.x, c_down.y, c_last.x, c_last.y);
			rect.NormalizeRect();
			dc.DrawFocusRect(rect);
			
			pView->SelectWithinRect(rect, TRUE);
		}
		else if (selectMode != none) // ��������ƶ���ı��С״̬,��ˢ���Ӵ���
		{
			CPoint local = point;
			pView->ClientToDoc(local);	

			CGeoMapDoc *pDoc = pView->GetDocument();
			if( pView->m_selection.GetCount() == 1 )
			{
				CGeoDrawObj* pObj = pView->m_selection.GetTail();
				c_RectLast = pObj->m_rtPosition;
				pObj->m_bMoving = FALSE;
				
				if(firstDownPoint != local)
				{					
					if( g_nDragHandle != 0 
						&& selectMode == sizeObj )
					{
						if( pView->m_bCapture ) // �Զ���׽
						{
							CGeoDrawObj *p = pView->GetDocument()->HitOnBorder(local,pView,pObj);
							if( p != NULL  
								&& p != pObj )
							{
								int iIndex[2];
								local = p->GetNearByIndex(local, iIndex);
								pObj->MoveHandleTo(g_nDragHandle, local, pView);
							}
						}
						pDoc->Do(new CCmdObjMoveHandle(pDoc,pObj,g_nDragHandle,local,firstDownPoint,pView), TRUE);
					}
					else
					{
						pDoc->Do(new CCmdObjMove(pDoc, pObj, c_RectLast, c_RectDown, pView), TRUE);
					}
				}
				
				pObj->m_bMoving = FALSE;
				pObj->m_bSizeChanging = FALSE;
				pObj->m_bMoving = FALSE;
			}
			else if(pView->m_selection.GetCount()>1)
			{
				POSITION pos = pView->m_selection.GetHeadPosition();
				while (pos!=NULL)
				{
					CGeoDrawObj* pObj = pView->m_selection.GetNext(pos);
					pObj->m_bMoving = FALSE;
					pObj->m_bSizeChanging = FALSE;
					pObj->m_bMoving = FALSE;				
				}
		
				if(firstDownPoint != local 
					&& selectMode == moveObj
					&& m_bEnableMove)
				{					
					CPoint delta = local-firstDownPoint;

					pDoc->Do(new CCmdMoveDrawObjs(pDoc, pView->m_selection, CPoint(0,0), delta, pView), TRUE);
				}				
			}
		}
	}
	
	CToolBase::OnLButtonUp(pView, nFlags, point);
}

void CGeoToolSelect::OnMouseMove(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	if (pView->GetCapture() != pView)
	{
		// ���Ҫѡ�����ģʽ�£�����ֻ��һ��ѡ�еĶ���
		// ������ֱ�������ʾ�ֱ����Ĺ�꣬��ֱ�ӷ���
		if (c_drawShape == selection && pView->m_selection.GetCount() == 1)
		{
			CGeoDrawObj* pObj = pView->m_selection.GetHead();
			CPoint local = point;
			pView->ClientToDoc(local);

			int nHandle = pObj->HitTestHandle(local, pView, TRUE);
			if (nHandle != 0)
			{
				SetCursor(pObj->GetHandleCursor(nHandle));
				return; // bypass CDrawTool
			}
		}
		if (c_drawShape == selection)
			CToolBase::OnMouseMove(pView, nFlags, point);
		return;
	}

	if (selectMode == netSelect) // �������ѡ����ģʽ����ʾ������ѡ����
	{
		CClientDC dc(pView);
		CRect rect(c_down.x, c_down.y, c_last.x, c_last.y);
		rect.NormalizeRect();
		dc.DrawFocusRect(rect);
		rect.SetRect(c_down.x, c_down.y, point.x, point.y);
		rect.NormalizeRect();
		dc.DrawFocusRect(rect);

		CToolBase::OnMouseMove(pView, nFlags, point);
		return;
	}

	CPoint local = point;
	pView->ClientToDoc(local);

	CPoint delta = (CPoint)(local - lastPoint);

	POSITION pos = pView->m_selection.GetHeadPosition();
	while (pos != NULL)
	{
		CGeoDrawObj* pObj = pView->m_selection.GetNext(pos);
		CRect position = pObj->m_rtPosition;

		if (selectMode == moveObj) // �ƶ�����
		{
			pObj->m_bMoving = TRUE;
			position += delta;

			if(m_bEnableMove) 
				pObj->MoveTo(position, pView);

			if( pObj->m_trackerState == CGeoDrawObj::active ) // ����ƶ�ͼ����ֻ��һ��ͼ��ʱ����״̬������ʾͼ����������
				SetStatusCenter(pObj->m_geoCenterPoint.x, pObj->m_geoCenterPoint.y, "ͼ������: ");
		}
		else if ( g_nDragHandle != 0 && selectMode == sizeObj ) // �ı�����С
		{
			// ������߶Σ�ʹ�û���������ֱ��
			if( pObj->IsKindOf(RUNTIME_CLASS(CGeoLineObj)))
			{
				CPoint pt;
				if( g_nDragHandle == 2 )
					pt = pObj->GetHandle(1);
				else
					pt = pObj->GetHandle(2);

				CSize size(5,5);
				pView->ClientToDoc(size);
				if(local.x>=pt.x-size.cx 
					&& local.x<=pt.x+size.cx)
					local.x = pt.x;

				if(local.y>=pt.y-size.cy 
					&& local.y<=pt.y+size.cy)
					local.y = pt.y;

				pObj->RecalcBounds();
			}

			// �Զ���׽
			if( pView->m_bCapture ) // �Զ���׽
			{
				CGeoDrawObj *p = pView->GetDocument()->HitOnBorder(local,pView,pObj);
				if( p != NULL
					&& p != pObj )
				{
					int iIndex[2];
					local = p->GetNearByIndex(local, iIndex);
				}
			}
			pObj->MoveHandleTo(g_nDragHandle, local, pView);
			
			if( pObj->m_trackerState == CGeoDrawObj::active ) // ����ƶ�ͼ����ֻ��һ��ͼ��ʱ����״̬������ʾͼ����������
				SetStatusCenter(pObj->m_geoCenterPoint.x, pObj->m_geoCenterPoint.y, "ͼ������: ");
		}
	}

	lastPoint = local;

	if (selectMode == sizeObj 
		&& c_drawShape == selection)
	{
		c_last = point; // �ı�����Сʱ����ʾ�ֱ����Ĺ��
		SetCursor(pView->m_selection.GetHead()->GetHandleCursor(g_nDragHandle));
		return; // bypass CDrawTool
	}
	

	if (selectMode == moveObj 
		&& c_drawShape == selection 
		&& m_bEnableMove)
	{
		c_last = point; 
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));
		return; // bypass CDrawTool
	}

	c_last = point;
	if (c_drawShape == selection)
		CToolBase::OnMouseMove(pView, nFlags, point);
}

void CGeoToolSelect::OnKeyDown(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CSize line(1,1);
	CSize page(10,10);
	pView->ClientToDoc(line);
	pView->ClientToDoc(page);
	if(line.cx<1)
		line.cx = 1;
	if(line.cy<1)
		line.cy = 1;
	if(page.cx<1)
		page.cx = 1;
	if(page.cy<1)
		page.cy = 1;
	CPoint delta(0,0);
	switch(nChar)
	{
	case VK_LEFT:
		delta.x = -line.cx;
		break;
	case VK_RIGHT:
		delta.x = line.cx;
		break;
	case VK_UP:
		delta.y = line.cy;
		break;
	case VK_DOWN:
		delta.y = -line.cy;
		break;
	case VK_HOME:
		delta.x = -page.cx;
		break;
	case VK_END:
		delta.x = page.cx;
		break;
	case VK_PRIOR:
		delta.y = page.cy;
		break;
	case VK_NEXT:
		delta.y = -page.cy;
		break;
	case 'A':
		if( pView->m_bCapture )
			pView->m_bCapture = FALSE;
		else
			pView->m_bCapture = TRUE;
		break;
	}

	CGeoMapDoc *pDoc = pView->GetDocument();
	
	if( pView->m_selection.GetCount() == 1 )
	{		
		CGeoDrawObj* pObj = pView->m_selection.GetTail();

		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPictrueObj)))
		{
			CGeoPictrueObj *pPictrue = (CGeoPictrueObj *)pObj;
			if( pView->m_nCtrlPointIndex > -1 )
			{
				pPictrue->m_ptCtrl[pView->m_nCtrlPointIndex] += delta;
				pPictrue->Invalidate(false);

				if( pView )
				{
					if( pView->CaleParameter(pPictrue) ) // �����Ĳ���
					{
						pView->ParamterChanged();
					}
				}
				return;
			}
		}
		CRect oldRect = pObj->m_rtPosition;
		CRect newRect = pObj->m_rtPosition;
		
		newRect += delta;
		
		pDoc->Do(new CCmdObjMove(pDoc, pObj, newRect, oldRect, pView), TRUE);
	}

	else if(  pView->m_selection.GetCount() > 1 )
	{
		pDoc->Do(new CCmdMoveDrawObjs(pDoc, pView->m_selection, delta, delta, pView), TRUE);
	}
}

void CGeoToolSelect::OnKeyUp(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
}

void CGeoToolSelect::OnChar(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags) 
{
}


void CGeoToolSelect::OnContextMenu(CGeoMapView* pView, CWnd* pWnd, CPoint point)
{
	if( pView->m_selection.GetCount() != 1 )
	{
		CToolBase::OnContextMenu(pView, pWnd, point);
		return;
	}

	CGeoDrawObj* pMenuObj = pView->m_selection.GetTail();	
	
	if( pMenuObj->GetMenuID() != -1 )
	{	
		CMenu menu;
		if (menu.LoadMenu(IDR_POPUP_GEOMAP))
		{
			CMenu* pPopup = menu.GetSubMenu(pMenuObj->GetMenuID());
			ASSERT(pPopup != NULL);			
			
			CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;
			pPopupMenu->SetAutoDestroy (TRUE);
			
			pPopupMenu->Create (pView, point.x, point.y, pPopup->m_hMenu, FALSE, TRUE);		
			
			// 			pPopup->TrackPopupMenu(TPM_RIGHTBUTTON | TPM_LEFTALIGN,
			// 				point.x, point.y,
			// 				AfxGetMainWnd()); 
		}
	}
	else
		CToolBase::OnContextMenu(pView, pWnd, point);
}