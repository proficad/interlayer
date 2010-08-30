// DrawItem.cpp: implementation of the CGeoDrawItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DrawItem.h"
#include "GeoDrawOleObj.h"
#include "GeoMapDoc.h"
#include "GeoMapView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(CGeoDrawItem, COleClientItem, 0)

CGeoDrawItem::CGeoDrawItem(CGeoMapDoc* pContainer, CGeoDrawOleObj* pDrawObj)
	: COleClientItem(pContainer)
{
	m_pDrawObj = pDrawObj;
}

CGeoDrawItem::~CGeoDrawItem()
{
	if (m_pDrawObj != NULL)
		m_pDrawObj->m_pClientItem = NULL;
}

void CGeoDrawItem::OnChange(OLE_NOTIFICATION nCode, DWORD dwParam)
{
	ASSERT_VALID(this);

	COleClientItem::OnChange(nCode, dwParam);

	switch(nCode)
	{
	case OLE_CHANGED_STATE:
	case OLE_CHANGED_ASPECT:
		m_pDrawObj->Invalidate();
		break;
	case OLE_CHANGED:
		UpdateExtent(); // extent may have changed
		m_pDrawObj->Invalidate();
		break;
	}
}

BOOL CGeoDrawItem::OnChangeItemPosition(const CRect& rectPos)
{
	ASSERT_VALID(this);

	CGeoMapView* pView = GetActiveView();
	ASSERT_VALID(pView);
	CRect rect = rectPos;
	pView->ClientToDoc(rect);

	if (rect != m_pDrawObj->m_rtPosition)
	{
		// invalidate old rectangle
		m_pDrawObj->Invalidate();

		// update to new rectangle
		m_pDrawObj->m_rtPosition = rect;
		GetExtent(&m_pDrawObj->m_extent);

		// and invalidate new rectangle
		m_pDrawObj->Invalidate();

		// mark document as dirty
		GetDocument()->SetModifiedFlag();
	}
	return COleClientItem::OnChangeItemPosition(rectPos);
}

void CGeoDrawItem::OnGetItemPosition(CRect& rPosition)
{
	ASSERT_VALID(this);

	// update to extent of item if m_position is not initialized
	if (m_pDrawObj->m_rtPosition.IsRectEmpty())
		UpdateExtent();

	// copy m_position, which is in document coordinates
	CGeoMapView* pView = GetActiveView();
	ASSERT_VALID(pView);
	rPosition = m_pDrawObj->m_rtPosition;
	pView->DocToClient(rPosition);
}

void CGeoDrawItem::Serialize(CArchive& ar)
{
	ASSERT_VALID(this);

	// Call base class first to read in COleClientItem data.
	// Note: this sets up the m_pDocument pointer returned from
	//  CGeoDrawItem::GetDocument, therefore it is a good idea
	//  to call the base class Serialize first.
	COleClientItem::Serialize(ar);

	// now store/retrieve data specific to CGeoDrawItem
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

BOOL CGeoDrawItem::UpdateExtent()
{
	CSize size;
	if (!GetExtent(&size) || size == m_pDrawObj->m_extent)
		return FALSE;       // blank

	// if new object (i.e. m_extent is empty) setup position
	if (m_pDrawObj->m_extent == CSize(0, 0))
	{
		m_pDrawObj->m_rtPosition.right =
			m_pDrawObj->m_rtPosition.left + MulDiv(size.cx, 10, 96);
		m_pDrawObj->m_rtPosition.bottom =
			m_pDrawObj->m_rtPosition.top - MulDiv(size.cy, 10, 96);
	}
	// else data changed so scale up rect as well
	else if (!IsInPlaceActive() && size != m_pDrawObj->m_extent)
	{
		m_pDrawObj->m_rtPosition.right = m_pDrawObj->m_rtPosition.left +
			MulDiv(m_pDrawObj->m_rtPosition.Width(), size.cx, m_pDrawObj->m_extent.cx);
		m_pDrawObj->m_rtPosition.bottom = m_pDrawObj->m_rtPosition.top +
			MulDiv(m_pDrawObj->m_rtPosition.Height(), size.cy, m_pDrawObj->m_extent.cy);
	}

	m_pDrawObj->m_extent = size;
	m_pDrawObj->Invalidate();   // redraw to the new size/position
	return TRUE;
}

void CGeoDrawItem::OnActivate()
{
	// allow only one inplace active item per frame
	CView* pView = GetActiveView();
	ASSERT_VALID(pView);
	COleClientItem* pItem = GetDocument()->GetInPlaceActiveItem(pView);
	if (pItem != NULL && pItem != this)
		pItem->Close();

	COleClientItem::OnActivate();
}

void CGeoDrawItem::OnDeactivateUI(BOOL bUndoable)
{
	COleClientItem::OnDeactivateUI(bUndoable);

	// hide the object if it is not an outside-in object
	DWORD dwMisc = 0;
	m_lpObject->GetMiscStatus(GetDrawAspect(), &dwMisc);
	if (dwMisc & OLEMISC_INSIDEOUT)
		DoVerb(OLEIVERB_HIDE, NULL);
}

BOOL CGeoDrawItem::OnShowControlBars(CFrameWnd* pFrameWnd, BOOL bShow)
{
	/*CMDIFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST (CMDIFrameWndEx, pFrameWnd);
	if (pMainFrame != NULL)
	{
		return pMainFrame->OnShowControlBars (bShow);
	}
	else	// Maybe, SDI frame...
	{
		CFrameWndEx* pFrame = DYNAMIC_DOWNCAST (CFrameWndEx, pFrameWnd);
		if (pFrame != NULL)
		{
			return pFrame->OnShowControlBars (bShow);
		}
	}
	*/
	
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CGeoDrawItem diagnostics

#ifdef _DEBUG
void CGeoDrawItem::AssertValid() const
{
	COleClientItem::AssertValid();
}

void CGeoDrawItem::Dump(CDumpContext& dc) const
{
	COleClientItem::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////
