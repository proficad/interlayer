// GeoDrawOleObj.cpp: implementation of the CGeoDrawOleObj class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GeoDrawOleObj.h"
#include "GeoMapDoc.h"
#include "GeoMapView.h"
#include "CmdGeoDrawObj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


extern CArchive& AFXAPI operator>>(CArchive& ar, CGeoDrawItem*& pOb);


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(CGeoDrawOleObj, CGeoDrawObj, VERSIONABLE_SCHEMA|0)

CGeoDrawOleObj::CGeoDrawOleObj() : m_extent(0,0)
{
	m_pClientItem = NULL;
}

CGeoDrawOleObj::CGeoDrawOleObj(const CRect& position, CDrawLayer *pLayer)
	: CGeoDrawObj(position, pLayer), m_extent(0, 0)
{
	m_pClientItem = NULL;
}

CGeoDrawOleObj::~CGeoDrawOleObj()
{
	if (m_pClientItem != NULL)
	{
		m_pClientItem->Release();
		delete m_pClientItem;
		m_pClientItem = NULL;
	}
}

void CGeoDrawOleObj::Remove()
{
	if (m_pClientItem != NULL)
	{
		m_pClientItem->Delete();
		m_pClientItem = NULL;
	}
}

void CGeoDrawOleObj::Serialize( CArchive& ar )
{
	ASSERT_VALID(this);

	CGeoDrawObj::Serialize(ar);

	ar.SerializeClass(RUNTIME_CLASS(CGeoDrawOleObj));

	if (ar.IsStoring())
	{
		ar << m_extent;
		ar << m_pClientItem;
	}
	else
	{
		int nVersion = ar.GetObjectSchema(); // ��ȡ�汾��Ϣ
		switch( nVersion )
		{
		case 0:
			ar >> m_extent;
			ar >> m_pClientItem;
			break;
		case 1:
			break;
		case 2:
			break;
		default:
			break;
		}
		
		m_pClientItem->m_pDrawObj = this;
	}
}

CGeoDrawObj* CGeoDrawOleObj::Clone(CDrawLayer *pLayer)
{
	ASSERT_VALID(this);

	AfxGetApp()->BeginWaitCursor();

	CGeoDrawOleObj* pClone = NULL;
	CGeoDrawItem* pItem = NULL;
	TRY
	{
		// perform a "deep copy" -- need to copy CGeoDrawOleObj and the CGeoDrawItem
		//  that it points to.
		CGeoDrawOleObj* pClone = new CGeoDrawOleObj(m_rtPosition, m_pDocument->m_pCurDrawLayer);
		CGeoDrawItem* pItem = new CGeoDrawItem(m_pDocument, pClone);
		if (!pItem->CreateCloneFrom(m_pClientItem))
			AfxThrowMemoryException();

		pClone->m_pClientItem	=	pItem;
		pClone->m_rtPosition	=	m_rtPosition;
		pClone->m_bLocked		=	m_bLocked;			// ����ͼ��
		pClone->m_bShow			=	m_bShow;			// �Ƿ���ʾ
		pClone->m_pDocument		=	m_pDocument;		// �ĵ�
		ASSERT_VALID(pClone);

		if (pClone != NULL)
		{
			m_pDocument->Do(new CCmdAddDrawObj(m_pDocument, pClone), TRUE);
		}
	}
	CATCH_ALL(e)
	{
		pItem->Delete();
		pClone->m_pClientItem = NULL;
		pClone->Remove();
		AfxGetApp()->EndWaitCursor();

		THROW_LAST();
	}
	END_CATCH_ALL

	AfxGetApp()->EndWaitCursor();
	return pClone;
}

void CGeoDrawOleObj::Draw(CDC* pDC, CGeoMapView* pView)
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
	
	CGeoDrawItem* pItem = m_pClientItem;
	if (pItem != NULL)
	{
		// draw the OLE item itself
		pItem->Draw(pDC, m_rtPosition);

		// don't draw tracker in print preview or on printer
		if (!pDC->IsPrinting())
		{
			// use a CRectTracker to draw the standard effects
			CRectTracker tracker;
			tracker.m_rect = m_rtPosition;
			pDC->LPtoDP(tracker.m_rect);

			// put hatching over the item if it is currently open
			if (pItem->GetItemState() == COleClientItem::openState ||
				pItem->GetItemState() == COleClientItem::activeUIState)
			{
				tracker.m_nStyle |= CRectTracker::hatchInside;
			}
			tracker.Draw(pDC);
		}
	}
}

void CGeoDrawOleObj::OnOpen(CGeoMapView* pView)
{
	AfxGetApp()->BeginWaitCursor();
	m_pClientItem->DoVerb(
		GetKeyState(VK_CONTROL) < 0 ? OLEIVERB_OPEN : OLEIVERB_PRIMARY,
		pView);
	AfxGetApp()->EndWaitCursor();
}

BOOL CGeoDrawOleObj::OnEditProperties(CGeoMapView* pView)
{
	OnOpen(pView);
	return FALSE;
}

// position is in logical
void CGeoDrawOleObj::MoveTo(const CRect& position, CGeoMapView* pView)
{
	ASSERT_VALID(this);

	if( m_bLocked )
		return;

	if (position == m_rtPosition)
		return;

	// call base class to update position
	CGeoDrawObj::MoveTo(position, pView);

	// update position of in-place editing session on position change
	if (m_pClientItem->IsInPlaceActive())
		m_pClientItem->SetItemRects();
}

// �������߷����ı�ʱ
void CGeoDrawOleObj::OnScaleChanged()
{
// 	CPoint2D point = CoordinateTransfer(m_geoCenterPoint,FALSE);
// 	CPoint2D pt = m_rtPosition.CenterPoint();
// 	CPoint delta = (CPoint)(point - pt);
// 	CRect position = m_rtPosition;
// 	MoveTo(position, NULL);	
	CGeoDrawObj::OnScaleChanged();
}

// ��������תʱ
void CGeoDrawOleObj::OnAngleChanged()
{
	CGeoDrawObj::OnAngleChanged();
}

// ����������ԭ���иı�ʱ
void CGeoDrawOleObj::OnCoordOrigChanged()
{
	CGeoDrawObj::OnCoordOrigChanged();
}