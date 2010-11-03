#include "StdAfx.h"
#include "GeoDrawObj.h"
#include "drawlayer.h"
#include "GeoMapView.h"
#include "CmdGeoDrawObj.h"
#include "../Resource.h"

extern double ptopointset(int vcount, CPoint2D pointset[], CPoint2D p, CPoint2D &q, int *iIndex);
extern const char* newGUID();

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CGeoDrawObj, CRefObject, VERSIONABLE_SCHEMA|0)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeoDrawObj::CGeoDrawObj()
{
	m_pParentLayer = NULL;
	m_pParentPage = NULL;
	m_pParentObj = NULL;
	m_pDocument = NULL;

	m_bLocked = FALSE;
	m_bEditing = FALSE;
	m_bMoving = FALSE;
	m_bShow = TRUE;
	m_pParentLayer = NULL;
	m_trackerState = normal;
	m_bSizeChanging = FALSE;
	m_uMenuID = -1;
	m_strObjName = _T("");
	m_strID = newGUID();

	m_pSelectedObj = NULL;
}

CGeoDrawObj::CGeoDrawObj(CDrawLayer* pLayer)
{
	m_pParentObj = NULL;
	m_pParentLayer = pLayer;
	m_pDocument = pLayer->m_pDocument;
	m_pParentPage = pLayer->GetParentPage();

	m_bLocked = FALSE;
	m_bEditing = FALSE;
	m_bMoving = FALSE;
	m_bShow = TRUE;
	m_pParentLayer = pLayer;
	m_trackerState = normal;
	m_bSizeChanging = FALSE;
	m_pDocument = pLayer->m_pDocument;
	m_uMenuID = -1;
	m_strObjName = _T("");
	m_strID = newGUID();
	m_pSelectedObj = NULL;
}

CGeoDrawObj::CGeoDrawObj(const CRect& position, CDrawLayer* pLayer)
{
	m_pParentObj = NULL;
	m_pParentLayer = pLayer;
	m_pDocument = pLayer->m_pDocument;
	m_pParentPage = pLayer->GetParentPage();

	m_rtPosition = position;
	m_geoPosition = CoordinateTransfer(m_rtPosition);
	m_bLocked = FALSE;
	m_bEditing = FALSE;
	m_bMoving = FALSE;
	m_bShow = TRUE;	
	m_trackerState = normal;
	m_pDocument = pLayer->m_pDocument;
	m_bSizeChanging = FALSE;
	m_uMenuID = -1;
	m_strObjName = _T("");
	m_strID = newGUID();
	m_pSelectedObj = NULL;
}

CGeoDrawObj::CGeoDrawObj(const CRectEx& position, CDrawLayer* pLayer)
{
	m_pParentObj = NULL;
	m_pParentLayer = pLayer;
	m_pDocument = pLayer->m_pDocument;
	m_pParentPage = pLayer->GetParentPage();

	m_geoPosition = position;
	m_rtPosition = CoordinateTransfer(m_geoPosition);
	m_bLocked = FALSE;
	m_bEditing = FALSE;
	m_bMoving = FALSE;
	m_bShow = TRUE;	
	m_trackerState = normal;
	m_pDocument = pLayer->m_pDocument;
	m_bSizeChanging = FALSE;
	m_uMenuID = -1;
	m_strObjName = _T("");
	m_strID = newGUID();
	m_pSelectedObj = NULL;
}


CGeoDrawObj::~CGeoDrawObj()
{
	m_DrawObjects.RemoveAll();
}

void CGeoDrawObj::Draw(CDC *pDC, CGeoMapView *pView)
{
	POSITION pos = m_DrawObjects.GetHeadPosition();
	while (pos != NULL)
	{
		CGeoDrawObj* pObj = m_DrawObjects.GetNext(pos);
		pObj->Draw(pDC, pView);
		if (pView->m_bActive 
			&& !pDC->IsPrinting() 
			&& pView->IsSelected(pObj) )
			pObj->DrawTracker(pDC, pView);
	}
}

void CGeoDrawObj::DrawEditMode(CGeoMapView* pView)
{
	CClientDC dc(pView);
	pView->OnPrepareDC(&dc);
	int mode = dc.SetROP2(R2_NOTXORPEN);

	DrawTracker(&dc, pView);

	dc.SetROP2(mode);
}

void CGeoDrawObj::DrawMoveState(CGeoMapView* pView)
{
	CClientDC dc(pView);
	pView->OnPrepareDC(&dc);
	int mode = dc.SetROP2(R2_NOTXORPEN);

	DrawTracker(&dc, pView);

	dc.SetROP2(mode);
}

void CGeoDrawObj::DrawSizeState(CGeoMapView* pView)
{
	CClientDC dc(pView);
	pView->OnPrepareDC(&dc);

	int mode = dc.SetROP2(R2_NOTXORPEN);	

	DrawTracker(&dc, pView);

	dc.SetROP2(mode);
}
////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::DrawTracker
// 返回  : void
// 参数  : CDC *pDC
// 参数  : TrackerState state
// 说明  : 画图素当前状态
//
void CGeoDrawObj::DrawTracker(CDC *pDC, CGeoMapView *pView)
{
	ASSERT_VALID(this);

	switch (m_trackerState)
	{
	case normal:
		break;

	case selected:
		{			
			CSize size(3,3);
			pDC->DPtoLP(&size);
			int nHandleCount = GetHandleCount();
			for (int nHandle = 1; nHandle <= nHandleCount; nHandle += 1)
			{
				CPoint handle = GetHandle(nHandle);
				if( !m_bLocked )
				{
					if( m_bSizeChanging || m_bMoving )
					{
						CPen pen(PS_SOLID,0,RGB(0,0,0));
						CPen *pOldPen = pDC->SelectObject(&pen);
						CBrush b(RGB(0,0,0));
						CBrush *pOldBrush = pDC->SelectObject(&b);
						pDC->Rectangle(handle.x - size.cx, handle.y - size.cy, handle.x + size.cx, handle.y + size.cy);
						pDC->SelectObject(pOldPen);
						pDC->SelectObject(pOldBrush);
					}
					else
					{
						CPen pen(PS_SOLID,0,RGB(0,0,0));
						CPen *pOldPen = pDC->SelectObject(&pen);
						CBrush b(RGB(255,255,255));
						CBrush *pOldBrush = pDC->SelectObject(&b);
						pDC->Rectangle(handle.x - size.cx, handle.y - size.cy, handle.x + size.cx, handle.y + size.cy);
						pDC->SelectObject(pOldPen);
						pDC->SelectObject(pOldBrush);
					}
				}
				else
				{
					CPen pen(PS_SOLID,0,RGB(0,0,0));
					CPen *pOldPen = pDC->SelectObject(&pen);
					CBrush b(RGB(255,0,0));
					CBrush *pOldBrush = pDC->SelectObject(&b);
					pDC->Rectangle(handle.x - size.cx, handle.y - size.cy, handle.x + size.cx, handle.y + size.cy);
					pDC->SelectObject(pOldPen);
					pDC->SelectObject(pOldBrush);
				}
			}

			// 没有可以移动的手柄
			if( nHandleCount == 0 )
			{
				for (int nCorner = 1; nCorner <= 4; nCorner += 1)
				{
					CRect rt = GetCornerRect(nCorner,pView);
					if( !m_bLocked )
					{
						if( m_bSizeChanging || m_bMoving )
						{
							CPen pen(PS_SOLID,0,RGB(0,0,0));
							CPen *pOldPen = pDC->SelectObject(&pen);
							CBrush b(RGB(0,0,0));
							CBrush *pOldBrush = pDC->SelectObject(&b);
							pDC->Ellipse(rt);
							pDC->SelectObject(pOldPen);
							pDC->SelectObject(pOldBrush);
						}
						else
						{
							CPen pen(PS_SOLID,0,RGB(0,0,0));
							CPen *pOldPen = pDC->SelectObject(&pen);
							CBrush b(RGB(255,255,255));
							CBrush *pOldBrush = pDC->SelectObject(&b);
							pDC->Ellipse(rt);
							pDC->SelectObject(pOldPen);
							pDC->SelectObject(pOldBrush);
						}
					}
					else
					{
						CPen pen(PS_SOLID,0,RGB(0,0,0));
						CPen *pOldPen = pDC->SelectObject(&pen);
						CBrush b(RGB(255,0,0));
						CBrush *pOldBrush = pDC->SelectObject(&b);
						pDC->Ellipse(rt);
						pDC->SelectObject(pOldPen);
						pDC->SelectObject(pOldBrush);
					}
				}
			}
		}
		break;
	case active:
		{			
			CSize size(3,3);
			pDC->DPtoLP(&size);
			int nHandleCount = GetHandleCount();
			for (int nHandle = 1; nHandle <= nHandleCount; nHandle += 1)
			{
				CPoint handle = GetHandle(nHandle);
				if( !m_bLocked )
				{
					if( m_bSizeChanging || m_bMoving )
					{
						CPen pen(PS_SOLID,0,RGB(0,0,0));
						CPen *pOldPen = pDC->SelectObject(&pen);
						CBrush b(RGB(0,0,0));
						CBrush *pOldBrush = pDC->SelectObject(&b);
						pDC->Rectangle(handle.x - size.cx, handle.y - size.cy, handle.x + size.cx, handle.y + size.cy);
						pDC->SelectObject(pOldPen);
						pDC->SelectObject(pOldBrush);
					}
					else
					{
						//pDC->PatBlt(handle.x - size.cx, handle.y - size.cy, size.cx*2+1, size.cy*2+1, BLACKNESS);
						CPen pen(PS_SOLID,0,RGB(0,0,0));
						CPen *pOldPen = pDC->SelectObject(&pen);
						CBrush b(RGB(0,0,0));
						CBrush *pOldBrush = pDC->SelectObject(&b);
						pDC->Rectangle(handle.x - size.cx, handle.y - size.cy, handle.x + size.cx, handle.y + size.cy);
						pDC->SelectObject(pOldPen);
						pDC->SelectObject(pOldBrush);
					}
				}
				else
				{
					CPen pen(PS_SOLID,0,RGB(0,0,0));
					CPen *pOldPen = pDC->SelectObject(&pen);
					CBrush b(RGB(255,0,0));
					CBrush *pOldBrush = pDC->SelectObject(&b);
					pDC->Rectangle(handle.x - size.cx, handle.y - size.cy, handle.x + size.cx, handle.y + size.cy);
					pDC->SelectObject(pOldPen);
					pDC->SelectObject(pOldBrush);
				}
			}

			// 没有可以移动的手柄
			if( nHandleCount == 0 )
			{
				for (int nCorner = 1; nCorner <= 4; nCorner += 1)
				{
					CRect rt = GetCornerRect(nCorner,pView);
					if( !m_bLocked )
					{
						if( m_bSizeChanging || m_bMoving )
						{
							CPen pen(PS_SOLID,0,RGB(0,0,0));
							CPen *pOldPen = pDC->SelectObject(&pen);
							CBrush b(RGB(0,0,0));
							CBrush *pOldBrush = pDC->SelectObject(&b);
							pDC->Ellipse(rt);
							pDC->SelectObject(pOldPen);
							pDC->SelectObject(pOldBrush);
						}
						else
						{
							//pDC->PatBlt(handle.x - size.cx, handle.y - size.cy, size.cx*2+1, size.cy*2+1, BLACKNESS);
							CPen pen(PS_SOLID,0,RGB(0,0,0));
							CPen *pOldPen = pDC->SelectObject(&pen);
							CBrush b(RGB(0,0,0));
							CBrush *pOldBrush = pDC->SelectObject(&b);
							pDC->Ellipse(rt);
							pDC->SelectObject(pOldPen);
							pDC->SelectObject(pOldBrush);
						}
					}
					else
					{
						CPen pen(PS_SOLID,0,RGB(0,0,0));
						CPen *pOldPen = pDC->SelectObject(&pen);
						CBrush b(RGB(255,0,0));
						CBrush *pOldBrush = pDC->SelectObject(&b);
						pDC->Ellipse(rt);
						pDC->SelectObject(pOldPen);
						pDC->SelectObject(pOldBrush);
					}
				}
			}

		}
		break;
	}
/*
	{ // 画区域中心点

		CSize size(3,3), size1(4,4);;
		pDC->DPtoLP(&size);
		pDC->DPtoLP(&size1);

		CPoint pt = m_rtPosition.CenterPoint();
		pDC->LPtoDP(&pt);
		pDC->DPtoLP(&pt);

		CPen pen(PS_SOLID, 0, RGB(255,0,0));
		CPen *pOldPen = pDC->SelectObject(&pen);

		pDC->MoveTo(pt.x,pt.y + size.cy);
		pDC->LineTo(pt.x,pt.y - size1.cy);
		pDC->MoveTo(pt.x+size.cx, pt.y);
		pDC->LineTo(pt.x-size1.cx, pt.y);

		pDC->SelectObject(pOldPen);		
	}
	*/
	/*
	{ // 画区域框

		CRect rect = m_rtPosition;
		CPen pen;
		LOGBRUSH LogBrush;

		LogBrush.lbColor = RGB(0, 255, 255);

		LogBrush.lbStyle = PS_SOLID;

		pen.CreatePen( PS_COSMETIC | PS_ALTERNATE, 1, &LogBrush, 0, NULL );
		CPen *pOldPen = pDC->SelectObject(&pen);
		CBrush *pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
		pDC->Rectangle(rect);
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);	
	}
	*/
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::MoveTo
// 返回  : void
// 参数  : const CRect &positon 当前逻辑位置
// 参数  : CGeoMapView *pView
// 说明  : 移动图素
//
void CGeoDrawObj::MoveTo(const CRect &position, CGeoMapView *pView)
{
	ASSERT_VALID(this);

	if( m_bLocked )
		return;

	if (position == m_rtPosition)
		return;

	if (pView == NULL)
	{
		Invalidate();
		CPoint pt1 = m_rtPosition.CenterPoint();
		CPoint pt2 = position.CenterPoint();

		CPoint delta = pt2-pt1;

		POSITION pos = m_DrawObjects.GetHeadPosition();
		CGeoDrawObj * pObj;
		while (pos != NULL)
		{		
			pObj = m_DrawObjects.GetNext(pos);
			CRect rc = pObj->m_rtPosition;
			rc += delta;
			pObj->m_rtPosition = rc;
		}

		m_rtPosition = position;
		Invalidate();
	}
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

		CPoint pt1 = m_rtPosition.CenterPoint();
		CPoint pt2 = position.CenterPoint();

		CPoint delta = pt2-pt1;

		POSITION pos = m_DrawObjects.GetHeadPosition();
		CGeoDrawObj * pObj;
		while (pos != NULL)
		{		
			pObj = m_DrawObjects.GetNext(pos);
			CRect rc = pObj->m_rtPosition;
			rc += delta;
			pObj->m_rtPosition = rc;
		}

		m_rtPosition = position;		

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

	// 转换为大地坐标
	m_geoCenterPoint = CoordinateTransfer(pt);
	m_geoPosition = CoordinateTransfer(m_rtPosition);

	m_pDocument->SetModifiedFlag();

	//RecalcBounds(pView);
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::MoveHandleTo
// 返回  : void
// 参数  : int nHandle 手柄索引
// 参数  : CPoint point 目前逻辑位置
// 参数  : CGeoMapView *pView
// 说明  : 移动某手柄
//
void CGeoDrawObj::MoveHandleTo(int nHandle, CPoint point, CGeoMapView *pView)
{
	ASSERT_VALID(this);

	if( m_bLocked )
		return;

	CRect position = m_rtPosition;

	switch (nHandle)
	{
	default:
		ASSERT(FALSE);

	case 1:
		position.left = point.x;
		position.top = point.y;
		break;

	case 2:
		position.top = point.y;
		break;

	case 3:
		position.right = point.x;
		position.top = point.y;
		break;

	case 4:
		position.right = point.x;
		break;

	case 5:
		position.right = point.x;
		position.bottom = point.y;
		break;

	case 6:
		position.bottom = point.y;
		break;

	case 7:
		position.left = point.x;
		position.bottom = point.y;
		break;

	case 8:
		position.left = point.x;
		break;
	}

	MoveTo(position, pView);
}

BOOL CGeoDrawObj::RecalcBounds(CGeoMapView* pView)
{
	CRect rect(0,0,0,0);
	POSITION pos = m_DrawObjects.GetHeadPosition();
	CGeoDrawObj * pObj;
	while (pos != NULL)
	{		
		pObj = m_DrawObjects.GetNext(pos);
		pObj->RecalcBounds(pView);
		CRect rc = pObj->m_rtPosition;
		rc.NormalizeRect();
		rect |= rc;
	}

	rect.NormalizeRect();

	CRect rc = m_rtPosition;
	rc.NormalizeRect();

	rc |= rect;

	m_rtPosition = rc;

	CPoint pt = m_rtPosition.CenterPoint();

	// 转换为大地坐标
	m_geoCenterPoint = CoordinateTransfer(pt);

	m_geoPosition = CoordinateTransfer(m_rtPosition);

	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::OnEditProperties
// 返回  : void
// 参数  : CGeoMapView *pView
// 说明  : 编辑图素属性
//
BOOL CGeoDrawObj::OnEditProperties(CGeoMapView *pView)
{
	ASSERT_VALID(this);
	return FALSE;
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::Clone
// 返回  : CGeoDrawObj*
// 参数  : CGeoMapDoc *pDoc
// 说明  : 克隆图素并将图素加入到文档中
//
CGeoDrawObj* CGeoDrawObj::Clone(CDrawLayer *pLayer)
{
	ASSERT_VALID(this);

	CGeoDrawObj* pClone = new CGeoDrawObj(m_rtPosition,pLayer);
	pClone->m_rtPosition	=	m_rtPosition;
	pClone->m_geoPosition	=	m_geoPosition;
	pClone->m_bLocked		=	m_bLocked;			// 锁定图素
	pClone->m_bShow			=	m_bShow;			// 是否显示
	pClone->m_strID			=	newGUID();			// 新的ID
	pClone->m_ptAnchors.Copy(m_ptAnchors);			// 锚点

	pClone->m_geoCenterPoint=	m_geoCenterPoint;
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
// 函数  : CGeoDrawObj::Serialize
// 返回  : void
// 参数  : CArchive &ar
// 说明  : 串行化图素
//
void CGeoDrawObj::Serialize(CArchive &ar)
{
	CRefObject::Serialize(ar);

	ar.SerializeClass(RUNTIME_CLASS(CGeoDrawObj));

	m_DrawObjects.Serialize(ar);

	if (ar.IsStoring())
	{
		ar << m_strID;
		ar << m_rtPosition;
		ar << m_geoPosition;
		ar << m_bLocked;
		ar << m_bShow;
		ar << m_strObjName;

		int n = m_ptAnchors.GetSize();
		ar << n;
		for (int i = 0; i< n; i++)
		{
			ar << m_ptAnchors[i].x;
			ar << m_ptAnchors[i].y;
		}
		ar << m_geoCenterPoint;
	}
	else
	{
		// get the document back pointer from the archive

		int nVersion = ar.GetObjectSchema(); // 获取版本信息
		switch( nVersion )
		{
		case 0:
			{
				m_pDocument = (CGeoMapDoc*)ar.m_pDocument;
				ASSERT_VALID(m_pDocument);
				ASSERT_KINDOF(CGeoMapDoc, m_pDocument);

				ar >> m_strID;
				ar >> m_rtPosition;
				ar >> m_geoPosition;
				ar >> m_bLocked;
				ar >> m_bShow;
				ar >> m_strObjName;

				int n;
				ar >> n;
				for (int i = 0; i< n; i++)
				{
					POINT pt;
					ar >> pt.x;
					ar >> pt.y;
					m_ptAnchors.Add(pt);
				}

				ar >> m_geoCenterPoint;
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
// 函数  : CGeoDrawObj::Invalidate
// 返回  : void
// 参数  : CGeoMapView *pView
// 说明  : 刷新图素（当图素位置改变或大小改变时调用)
//
void CGeoDrawObj::Invalidate(CGeoMapView *pView)
{
	ASSERT_VALID(this);
	if( pView != NULL )
		pView->InvalObj(this);
	else
		m_pDocument->UpdateAllViews(NULL, HINT_UPDATE_DRAWOBJ, this);
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::GetHandleCount
// 返回  : int
// 说明  : 获取手柄总数
//
int CGeoDrawObj::GetHandleCount()
{
	ASSERT_VALID(this);
	return 8;
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::GetHandle
// 返回  : CPoint
// 参数  : int nHandle
// 说明  : 获取某手柄的中心逻辑坐标
//
CPoint CGeoDrawObj::GetHandle(int nHandle)
{
	ASSERT_VALID(this);
	int x, y, xCenter, yCenter;

	// this gets the center regardless of left/right and top/bottom ordering

	CRect rect = m_rtPosition;

	xCenter = rect.left + rect.Width()/2;
	yCenter = rect.top + rect.Height()/2;

	switch (nHandle)
	{
	default:
		ASSERT(FALSE);

	case 1:
		x = rect.left;
		y = rect.top;
		break;

	case 2:
		x = xCenter;
		y = rect.top;
		break;

	case 3:
		x = rect.right;
		y = rect.top;
		break;

	case 4:
		x = rect.right;
		y = yCenter;
		break;

	case 5:
		x = rect.right;
		y = rect.bottom;
		break;

	case 6:
		x = xCenter;
		y = rect.bottom;
		break;

	case 7:
		x = rect.left;
		y = rect.bottom;
		break;

	case 8:
		x = rect.left;
		y = yCenter;
		break;
	}

	return CPoint(x, y);
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::GetHandleCursor
// 返回  : HCURSOR
// 参数  : int nHandle
// 说明  : 获取某手柄的鼠标指针
//
HCURSOR CGeoDrawObj::GetHandleCursor(int nHandle)
{
	ASSERT_VALID(this);

	LPCTSTR id;
	switch (nHandle)
	{
	default:
		ASSERT(FALSE);

	case 1:
	case 5:
		id = IDC_SIZENWSE;
		break;

	case 2:
	case 6:
		id = IDC_SIZENS;
		break;

	case 3:
	case 7:
		id = IDC_SIZENESW;
		break;

	case 4:
	case 8:
		id = IDC_SIZEWE;
		break;
	}

	return AfxGetApp()->LoadStandardCursor(id);
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::GetHandleRect
// 返回  : CRect 某手柄范围逻辑坐标
// 参数  : int nHandleID 手柄索引
// 参数  : CGeoMapView *pView
// 说明  : 
//
CRect CGeoDrawObj::GetHandleRect(int nHandleID, CGeoMapView *pView)
{
	ASSERT_VALID(this);
	ASSERT(pView != NULL);

	CRect rect;
	// 获取手柄中心逻辑坐标 
	CPoint point = GetHandle(nHandleID);
	// 转换成设备坐标

	pView->DocToClient(point);

	rect.SetRect(point.x-3, point.y-3, point.x+3, point.y+3);

	// 转换成逻辑坐标
	pView->ClientToDoc(rect);

	return rect;
}


// 图素区域四角坐标
CPoint CGeoDrawObj::GetCorner(int nHandle)
{
	ASSERT_VALID(this);
	int x, y, xCenter, yCenter;

	CRect rect = m_rtPosition;

	xCenter = rect.left + rect.Width() / 2;
	yCenter = rect.top + rect.Height() / 2;

	switch (nHandle)
	{
	default:
		ASSERT(FALSE);

	case 1:
		x = rect.left;
		y = rect.top;
		break;

	case 2:
		x = rect.right;
		y = rect.top;
		break;

	case 3:
		x = rect.right;
		y = rect.bottom;
		break;

	case 4:
		x = rect.left;
		y = rect.bottom;
		break;
	}

	return CPoint(x, y);
}

// 区域四角
CRect CGeoDrawObj::GetCornerRect(int nHandleID, CGeoMapView* pView)
{
	ASSERT_VALID(this);
	ASSERT(pView != NULL);

	CRect rect;
	// 获取区域四角逻辑坐标 
	CPoint point = GetCorner(nHandleID);
	// 转换成设备坐标

	pView->DocToClient(point);

	rect.SetRect(point.x-3, point.y-3, point.x+3, point.y+3);

	// 转换成逻辑坐标
	pView->ClientToDoc(rect);

	return rect;
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::HitTestHanle
// 返回  : int
// 参数  : CPoint point是逻辑坐标
// 参数  : CGeoMapView *pView
// 参数  : BOOL bSelected 如果已经选中，返回是否点中某个手柄
// 说明  : 否则，如果返回1选中该图素，否则没有点中该图素
//
int CGeoDrawObj::HitTestHandle(CPoint point, CGeoMapView *pView, BOOL bSelected)
{
	ASSERT_VALID(this);
	ASSERT(pView != NULL);

	if (bSelected) // 如果该图素已经被选中了
	{
		int nHandleCount = GetHandleCount(); // 手柄总数
		for (int nHandle = 1; nHandle <= nHandleCount; nHandle += 1)
		{
			// 返回某手柄的逻辑范围
			CRect rc = GetHandleRect(nHandle,pView);
			rc.NormalizeRect();
			if (point.x >= rc.left && point.x <= rc.right &&
				point.y >= rc.top && point.y <= rc.bottom)
				return nHandle;
		}
	}
	else // 否则, 测试是否落在图素范围内了
	{
		m_pSelectedObj = NULL;

		POSITION pos = m_DrawObjects.GetHeadPosition();

		while (pos != NULL)
		{
			CGeoDrawObj *pObj = m_DrawObjects.GetNext(pos);
			if( pObj->HitTestHandle(point,pView, bSelected) )
			{
				if( pObj->m_pSelectedObj )
					m_pSelectedObj = pObj->m_pSelectedObj;
				else
					m_pSelectedObj = pObj;
				return 1;
			}
		}

		CRect rt = m_rtPosition;

		rt.NormalizeRect();

		if( m_pParentLayer->m_rgnClipping.m_nPoints > 2
			&& m_pParentLayer->m_bClipping)
			rt = rt&m_pParentLayer->m_rgnClipping.m_rtPosition;

		if (point.x >= rt.left && point.x <= rt.right &&
			point.y >= rt.top && point.y <= rt.bottom)
			return 1;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::HitTestEdit
// 返回  : int
// 参数  : CPoint point是逻辑坐标
// 参数  : CGeoMapView *pView
// 说明  : 测试是否在编辑区域,返回非0在编辑区域，否则不在
//
long CGeoDrawObj::HitTestEdit(CPoint point, CGeoMapView *pView)
{
	// 测试是否落在图素编辑区域了
	if (point.x >= m_rtPosition.left && point.x < m_rtPosition.right &&
		point.y <= m_rtPosition.top && point.y > m_rtPosition.bottom)
		return 1;

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::Intersects
// 返回  : BOOL TRUE相交 FALSE 不相交
// 参数  : const CRect &rect 逻辑坐标
// 说明  : 测试矩形范围是否与图素范围相交
//
BOOL CGeoDrawObj::Intersects(const CRect &rect)
{
	ASSERT_VALID(this);

	CRect fixed = m_rtPosition;
	fixed.NormalizeRect();

	if( m_pParentLayer!=NULL 
		&& m_pParentLayer->m_rgnClipping.m_nPoints > 2
		&& m_pParentLayer->m_bClipping)
		fixed = fixed&m_pParentLayer->m_rgnClipping.m_rtPosition;

	CRect rectT = rect;
	rectT.NormalizeRect();

	return !(rectT & fixed).IsRectEmpty();
}

// 坐标转换的数学模型
// 设自定义坐标系为X'O'Y'，统一坐标系为XOY，X' 顺时针旋转θ角后与X平行，
// 则某一点的自定义坐标（X，Y）转换为统一坐标（X'，Y'）的计算公式为：
// X=Xo+X'COSθ-Y'SINθ
// Y=Yo+X'SINθ+Y'COSθ …………………(1)
// ⑴式中的Xo、Yo为自定义坐标系的原点O'在统一坐标系中的坐标。

// 坐标转换器
// 转换成大地坐标
CPoint2D CGeoDrawObj::CoordinateTransfer(const CPoint &pt)
{
	// 100 代表1cm
	// pt.x/10000.0 化为米

	CDrawPage *pPage = m_pParentLayer->GetParentPage();
	
	CPoint2D point = pPage->CoordinateTransferNew(pt);

	return point;
}

CPoint CGeoDrawObj::CoordinateTransfer(const CPoint2D &pt)
{
	// 100 代表1cm
	// pt.x/10000.0 化为米

	CDrawPage *pPage = m_pParentLayer->GetParentPage();

	CPoint point = pPage->CoordinateTransferNew(pt);

	return point;
}


CRectEx CGeoDrawObj::CoordinateTransfer(const CRect &rect)
{
	CDrawPage *pPage = m_pParentLayer->GetParentPage();

	CRectEx retRect = pPage->CoordinateTransferNew(rect);

	return retRect;
}

CRect CGeoDrawObj::CoordinateTransfer(const CRectEx &rect)
{
	CDrawPage *pPage = m_pParentLayer->GetParentPage();
	
	CRect retRect = pPage->CoordinateTransferNew(rect);

	return retRect;
}

BOOL CGeoDrawObj::IsHitOnBorder(CPoint point, CGeoMapView* pView)
{
	if(!m_bShow)
		return FALSE;

	CRect cliRect;
	pView->GetClientRect(cliRect);
	pView->ClientToDoc(cliRect);
	if( !Intersects(cliRect) ) //如果该图素不在当前窗口中则不显示
		return FALSE;

	CClientDC dc(pView);
	pView->OnPrepareDC(&dc);

	CSize size(20,20);
	dc.DPtoLP(&size);
	dc.LPtoDP(&point);

	CRect rect = m_rtPosition;
	rect.NormalizeRect();

	CPen pen1(PS_GEOMETRIC | PS_SOLID | PS_JOIN_MITER | PS_ENDCAP_FLAT , size.cx , RGB(0, 0, 0) );

	CPen *pOldPen = dc.SelectObject(&pen1);	

	dc.BeginPath();	

	dc.Rectangle(rect);

	dc.EndPath();

	dc.SelectObject(pOldPen);
	pen1.DeleteObject();

	dc.WidenPath();

	CRgn rgn;

	rgn.CreateFromPath(&dc);

	if( !rgn.m_hObject )
		return FALSE;

	if( rgn.PtInRegion(point) )
		return TRUE;
	else
		return FALSE;
}

//////////////////////////////////////////////////////////////////////////
// 获取距point处最近的在曲线上的点索引号
// 返回最近点的坐标
CPoint	CGeoDrawObj::GetNearByIndex(const CPoint &point, int *iIndex)
{
	CPoint2D points[5];
	int nPoints;
	CPoint tmp;

	tmp = m_rtPosition.TopLeft();
	points[0] = tmp;

	tmp = m_rtPosition.BottomRight();
	points[2] = tmp;

	points[1].x = points[2].x;
	points[1].y = points[0].y;

	points[3].x = points[0].x;
	points[3].y = points[2].y;

	points[4]	= points[0];

	nPoints = 5;

	CPoint2D pt(point.x,point.y), retPt;


	ptopointset(nPoints,points, pt, retPt, iIndex);

	if( iIndex[0] == -1 )
	{
		if(iIndex[1] != 0 ) 
			iIndex[1] -= 1;
	}

	CPoint retPoint(retPt.x,retPt.y);

	return retPoint;
}

//////////////////////////////////////////////////////////////////////////
// 获取图形路径的所有点
// 
int CGeoDrawObj::GetPathPoint(LPPOINT &lppt)
{
	lppt = new CPoint[5];

	lppt[0] = m_rtPosition.TopLeft();
	lppt[2] = m_rtPosition.BottomRight();	

	lppt[1].x = lppt[2].x;
	lppt[1].y = lppt[0].y;

	lppt[3].x = lppt[0].x;
	lppt[3].y = lppt[2].y;

	lppt[4]	= lppt[0];

	return 5;
}

// 当比例尺发生改变时
void CGeoDrawObj::OnScaleChanged()
{
	m_rtPosition = CoordinateTransfer(m_geoPosition);

	Invalidate();

	POSITION pos = m_DrawObjects.GetHeadPosition();
	while ( pos != NULL )
	{
		CGeoDrawObj *pObj = m_DrawObjects.GetNext(pos);
		pObj->OnScaleChanged();		
	}
}

// 当发生旋转时
void CGeoDrawObj::OnAngleChanged()
{
	Invalidate();
}

// 当发生坐标原点有改变时
void CGeoDrawObj::OnCoordOrigChanged()
{
	Invalidate();
}

void CGeoDrawObj::CopyAttribute(CGeoDrawObj *pSrcObj)
{
	m_bLocked		=	pSrcObj->m_bLocked;			// 锁定图素
	m_bShow			=	pSrcObj->m_bShow;			// 是否显示
}

UINT CGeoDrawObj::GetMenuID()
{
	CMenu menu;
	if (menu.LoadMenu(IDR_POPUP_GEOMAP))
	{
		CString strMenuName;
		int nCounts= menu.GetMenuItemCount();
		for (int i=0; i<nCounts; i++)
		{
			menu.GetMenuString(i,strMenuName,MF_BYPOSITION);
			if( strMenuName == m_strObjName)
			{
				m_uMenuID = i;
				break;
			}
		}
	}

	return m_uMenuID;
}

void CGeoDrawObj::SetObjName(CString strObjName)
{
	m_strObjName = strObjName;
}

void CGeoDrawObj::ClaimChildren()
{
	POSITION pos = m_DrawObjects.GetHeadPosition();
	while (pos != NULL)
	{
		CGeoDrawObj* pObj = m_DrawObjects.GetNext(pos);

		pObj->m_pParentLayer = m_pParentLayer;
		pObj->m_pParentPage = m_pParentPage;
		pObj->m_pParentObj = this;

		pObj->ClaimChildren();
	}
}

CGeoDrawObj* CGeoDrawObj::ObjectAt(const CPoint& point, CGeoMapView *pView)
{
	POSITION pos = m_DrawObjects.GetHeadPosition();
	while (pos != NULL)
	{
		CGeoDrawObj* pObj = m_DrawObjects.GetNext(pos);
		if( pObj->HitTestHandle(point,pView,FALSE) )
			return pObj;
	}
	return NULL;
}


BOOL CGeoDrawObj::Add(CGeoDrawObj *pObj)
{
	TRY
	{
		m_DrawObjects.AddTail(pObj);
		RecalcBounds(NULL);		
	}
	CATCH_ALL(e)
	{
		e->Delete();
		return FALSE;
	}
	END_CATCH_ALL

		return TRUE;
}


BOOL CGeoDrawObj::Remove(CGeoDrawObj *pObj)
{
	TRY
	{
		if( pObj->m_bLocked )
			return FALSE;

		// Find and remove from document
		POSITION pos = m_DrawObjects.Find(pObj);
		if (pos != NULL)
		{
			m_DrawObjects.GetAt(pos);
			m_DrawObjects.RemoveAt(pos);
			m_pDocument->UpdateAllViews(NULL);

			Invalidate();
			RecalcBounds();
			Invalidate();
		}
		else
			return FALSE;		
	}
	CATCH_ALL(e)
	{
		e->Delete();
		return FALSE;
	}
	END_CATCH_ALL

		return TRUE;
}


void CGeoDrawObj::RemoveAll()
{
	m_DrawObjects.RemoveAll();	
}

//功能:图素移到最上一层
//参数:
//返回:
BOOL CGeoDrawObj::MoveToFront(CGeoDrawObj *pObj)
{	
	if(Remove(pObj))
		return Add(pObj);
	else
		return FALSE;
}

//功能:图素移到最底一层
//参数:
//返回:
BOOL CGeoDrawObj::MoveToBack(CGeoDrawObj *pObj)
{	
	if(Remove(pObj))
	{
		m_DrawObjects.AddHead(pObj);
		return TRUE;
	}
	else
		return FALSE;
}

//功能:图素上移一层
//参数:
//返回:
BOOL CGeoDrawObj::MoveForward(CGeoDrawObj *pObj)
{	
	POSITION pos = m_DrawObjects.Find(pObj);
	ASSERT(pos != NULL);
	if (pos != m_DrawObjects.GetTailPosition())
	{
		POSITION posNext = pos;
		m_DrawObjects.GetNext(posNext);
		Remove(pObj);
		m_DrawObjects.InsertAfter(posNext, pObj);
		return TRUE;
	}
	return FALSE;
}

//功能:图素下移一层
//参数:
//返回:
BOOL CGeoDrawObj::MoveBack(CGeoDrawObj *pObj)
{	
	POSITION pos = m_DrawObjects.Find(pObj);
	ASSERT(pos != NULL);
	if (pos != m_DrawObjects.GetHeadPosition())
	{
		POSITION posPrev = pos;
		m_DrawObjects.GetPrev(posPrev);
		Remove(pObj);
		m_DrawObjects.InsertBefore(posPrev, pObj);	
		return TRUE;
	}
	return FALSE;
}
