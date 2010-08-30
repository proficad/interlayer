// ToolClip.cpp: implementation of the CGeoToolClip class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ToolClip.h"
#include "GeoMapDoc.h"
#include "GeoMapView.h"
#include "../Resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CGeoToolClip geoclipTool;
extern CToolBase::SelectMode selectMode;

extern int g_nDragHandle;		// ��ǰҪ�ı��Сʱ������ֱ�

extern CPoint lastPoint; // ���һ������
extern CPoint firstDownPoint;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeoToolClip::CGeoToolClip()
:CToolBase(CToolBase::clippingRgn)
{
}

CGeoToolClip::~CGeoToolClip()
{

}

void CGeoToolClip::OnLButtonDown(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	CPoint local = point;
	pView->ClientToDoc(local);

	selectMode = none;

	CGeoMapDoc *pDoc = pView->GetDocument();
	CRgnClipping* pObj = &pDoc->m_pCurDrawLayer->m_rgnClipping;
	
	if( nFlags&MK_CONTROL )
	{
		pObj->AddPoint(local, pView);
		pObj->RecalcBounds(pView);
		pView->InvalObj(pObj);
		return;
	}
	else if( nFlags&MK_SHIFT)
	{
		if(pObj->m_nPoints>2)
		{
			pObj->DelPoint(local, pView);
			pObj->RecalcBounds(pView);
		}
		pView->InvalObj(pObj);
		return;
	}
		
	g_nDragHandle = pObj->HitTestHandle(local, pView, TRUE); // ����ĳѡ�ж�����ֱ�
	if (g_nDragHandle != 0)		// �����ĳһ�ֱ��ϣ�
	{
		selectMode = sizeObj;		// ����ѡ��ģʽΪ�ı��Сģʽ
		SetCursor(pObj->GetHandleCursor(g_nDragHandle));
		pObj->m_bMoving = TRUE;
		local = pObj->GetHandle(g_nDragHandle);
	}
	
	if (selectMode == none) // ���û�����������ѡ������ռ����ж���һ������
	{
		if( pObj->IsHitOnBorder(local,pView) )
			selectMode = moveObj; // �����ƶ�����ģʽ��
	}

	lastPoint = local;
	firstDownPoint = local;

	CToolBase::OnLButtonDown(pView, nFlags, point);
}

void CGeoToolClip::OnRButtonDown(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	CPoint local = point;
	pView->ClientToDoc(local);
	
	selectMode = none;
	
	CGeoMapDoc *pDoc = pView->GetDocument();
	CRgnClipping* pObj = &pDoc->m_pCurDrawLayer->m_rgnClipping;
	pObj->InsertPoint(local, pView);
	
	pObj->RecalcBounds(pView);
	
	pView->InvalObj(pObj);
}

void CGeoToolClip::OnLButtonDblClk(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{	
	CGeoMapDoc *pDoc = pView->GetDocument();
	CRgnClipping* pObj = &pDoc->m_pCurDrawLayer->m_rgnClipping;
	
	if( pObj->m_bSmooth )
		pObj->m_bSmooth = FALSE;
	else
		pObj->m_bSmooth = TRUE;
	
	pView->InvalObj(pObj);
	
	CToolBase::OnLButtonDblClk(pView, nFlags, point);
}

void CGeoToolClip::OnEditProperties(CGeoMapView* pView)
{
	CGeoMapDoc *pDoc = pView->GetDocument();
	CGeoDrawObj* pObj = &pDoc->m_pCurDrawLayer->m_rgnClipping;
	pObj->OnEditProperties(pView);
}

void CGeoToolClip::OnLButtonUp(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	CGeoMapDoc *pDoc = pView->GetDocument();
	
	CGeoDrawObj* pObj = &pDoc->m_pCurDrawLayer->m_rgnClipping;
	pObj->m_bEditing = FALSE;
	pObj->m_bSizeChanging = FALSE;
	pObj->m_bMoving = FALSE;

	//pView->InvalObj(pObj);
	pView->Invalidate(FALSE);
				
	ReleaseCapture();
}

void CGeoToolClip::OnMouseMove(CGeoMapView* pView, UINT nFlags, const CPoint& point)
{
	CGeoMapDoc *pDoc = pView->GetDocument();
	CGeoDrawObj* pObj = &pDoc->m_pCurDrawLayer->m_rgnClipping;
	
	CPoint local = point;
	pView->ClientToDoc(local);

	if (pView->GetCapture() != pView)
	{		
		if( nFlags&MK_CONTROL )
		{
			SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_ADD_PT));				
		}
		else if( nFlags&MK_SHIFT )
			SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_DEL_PT));
		else
		{
			int nHandle = pObj->HitTestHandle(local, pView, TRUE);
			if (nHandle != 0)
				SetCursor(pObj->GetHandleCursor(nHandle));
			else
			{
				SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
			}
		}
		
		return;
	}

	CRect position = pObj->m_rtPosition;
	
	CPoint delta = (CPoint)(local - lastPoint);

	if (selectMode == moveObj) // �ƶ�����
	{
		position += delta;
		pObj->m_bMoving = TRUE;
		pObj->MoveTo(position, pView);			
	}
	else if ( g_nDragHandle != 0 && selectMode == sizeObj ) // �ı�����С
	{
		pObj->m_bSizeChanging = TRUE;
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
	}	

	lastPoint = local;

	if (selectMode == sizeObj)
	{
		c_last = point; // �ı�����Сʱ����ʾ�ֱ����Ĺ��
		SetCursor(pObj->GetHandleCursor(g_nDragHandle));
		return; // bypass CDrawTool
	}

	c_last = point;
	CToolBase::OnMouseMove(pView, nFlags, point);
}


void CGeoToolClip::OnKeyDown(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CSize line(1,1);
	CSize page(10,10);
	pView->ClientToDoc(line);
	pView->ClientToDoc(page);
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
	}

	CGeoMapDoc *pDoc = pView->GetDocument();
	
	if( pDoc->m_pCurDrawLayer->m_rgnClipping.m_nPoints < 3 )
		return;

	CRect newRect = pDoc->m_pCurDrawLayer->m_rgnClipping.m_rtPosition;	
	newRect += delta;
	pDoc->m_pCurDrawLayer->m_rgnClipping.MoveTo(newRect,pView);	
}

void CGeoToolClip::OnKeyUp(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
}

void CGeoToolClip::OnChar(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags) 
{
}
