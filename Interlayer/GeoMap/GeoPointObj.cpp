#include "StdAfx.h"
#include "GeoPointObj.h"
#include "DrawLayer.h"
#include "GeoMapDoc.h"
#include "GeoMapView.h"
#include "../Resource.h"
#include <float.h>
#include "CmdGeoDrawObj.h"
//#include "DlgPointSet.h"
#include <typeinfo>


IMPLEMENT_SERIAL(CGeoPointObj, CGeoDrawObj, VERSIONABLE_SCHEMA|0)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeoPointObj::CGeoPointObj()
{
	SetObjName(_T("点"));
}

CGeoPointObj::CGeoPointObj(CDrawLayer *pLayer)
:CGeoDrawObj(pLayer)
{
	m_strName	= _T("Point");
	m_ptName	= CPoint(0,0);
	m_iSymbolSize	= 20;			// 1mm;
	m_ptName.x	= 0;
	m_ptName.y  = m_iSymbolSize/2 + 5 + 13;

	memset(&m_lfFont, 0, sizeof(LOGFONT) );
	m_lfFont.lfHeight = -25;
	m_lfFont.lfCharSet = GB2312_CHARSET;
	strcpy_s(m_lfFont.lfFaceName, _T("宋体"));

	m_clrFont = RGB(0,0,0);				// 字体颜色
	m_clrSymbolPen = RGB(0,0,0);
	m_clrSymbolFill = RGB(255,255,255);

	SetObjName(_T("点"));
}

CGeoPointObj::CGeoPointObj(const CPoint &pt, CDrawLayer *pLayer, LPCSTR strName)
:CGeoDrawObj(pLayer)
{
	m_ptSymbolPoint		= pt;
	// 转换为大地坐标
	m_ptSymbolCoord		= CoordinateTransfer(m_ptSymbolPoint);
	if( strName )
		m_strName = strName;
	else
		m_strName	= _T("Point");

	m_iSymbolSize	= 20;				// 1mm;
	m_ptName.x	= 0;
	m_ptName.y  = m_iSymbolSize/2 + 5 + 13;

	memset(&m_lfFont, 0, sizeof(LOGFONT) );
	m_lfFont.lfHeight = -25;
	m_lfFont.lfCharSet = ANSI_CHARSET;
	strcpy_s(m_lfFont.lfFaceName, _T("Arial"));

	m_clrFont = RGB(0,0,0);				// 字体颜色
	m_clrSymbolPen = RGB(0,0,0);
	m_clrSymbolFill = RGB(255,255,255);

	RecalcBounds(NULL);

	CPoint centerPoint = m_rtPosition.CenterPoint();
	m_geoCenterPoint = CoordinateTransfer(centerPoint);
	SetObjName(_T("点"));
}

CGeoPointObj::CGeoPointObj(const CPoint2D &pt, CDrawLayer *pLayer, LPCSTR strName)
:CGeoDrawObj(pLayer)
{
	m_ptSymbolCoord = pt;
	// 转换为图形坐标
	m_ptSymbolPoint = CoordinateTransfer(pt);

	if( strName )
		m_strName = strName;
	else
		m_strName = pt.szInfo;
	
	m_ptName	= CPoint(0,20);	
	m_iSymbolSize	= 20;				// 1mm;
	m_ptName.x	= 0;
	m_ptName.y  = m_iSymbolSize/2 + 5 + 13;

	memset(&m_lfFont, 0, sizeof(LOGFONT) );
	m_lfFont.lfHeight = -25;
	m_lfFont.lfCharSet = ANSI_CHARSET;
	strcpy_s(m_lfFont.lfFaceName, _T("Arial"));

	m_clrFont = RGB(0,0,0);				// 字体颜色
	m_clrSymbolPen = RGB(0,0,0);
	m_clrSymbolFill = RGB(255,255,255);

	RecalcBounds(NULL);

	CPoint centerPoint = m_rtPosition.CenterPoint();
	m_geoCenterPoint = CoordinateTransfer(centerPoint);
	SetObjName(_T("点"));
}

CGeoPointObj::~CGeoPointObj()
{
}

void CGeoPointObj::Draw(CDC *pDC, CGeoMapView *pView)
{
	if( !m_bShow )
		return;

	if( !pDC->IsPrinting() )
	{
		CRect cliRect;
		pView->GetClientRect(cliRect);
		pView->ClientToDoc(cliRect);
		if( !Intersects(cliRect) ) //如果该图素不在当前窗口中则不显示
			return ;
	}

	CGeoDrawObj::Draw(pDC, pView);

	int nSaved = pDC->SaveDC();
	pDC->SetBkMode(TRANSPARENT);          //设定显示方式

	pDC->SetTextColor(m_clrFont);
	CFont font;
	font.CreateFontIndirect(&m_lfFont);

	CBrush brush(RGB(255, 235,200));

	CBrush *pOldBrush;
	pOldBrush = pDC->SelectObject(&brush);

	CFont *pOldFont = pDC->SelectObject(&font);

	CSize size(m_iSymbolSize/2,m_iSymbolSize/2), size1;
	pDC->LPtoDP(&size);
	size1 = size;
	size.cx +=1;
	size.cy +=1;

	pDC->DPtoLP(&size);
	pDC->DPtoLP(&size1);

	pDC->Ellipse(CRect(m_ptSymbolPoint.x-size.cx, m_ptSymbolPoint.y - size.cy,
		m_ptSymbolPoint.x + size1.cx, m_ptSymbolPoint.y + size1.cy));

	CPoint pt = m_ptSymbolPoint+m_ptName;
	CRect rt(pt,CSize(1,1));
	pDC->DrawText(m_strName, rt, DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_NOCLIP);

	pDC->SelectObject(pOldFont);

	font.DeleteObject();

	pDC->SelectObject(pOldBrush);

	pDC->RestoreDC(nSaved);
}

void CGeoPointObj::DrawEditMode(CGeoMapView* pView)
{
	CGeoDrawObj::DrawEditMode(pView);
}

void CGeoPointObj::DrawMoveState(CGeoMapView* pView)
{
	CGeoDrawObj::DrawMoveState(pView);
}

void CGeoPointObj::DrawSizeState(CGeoMapView* pView)
{
	CGeoDrawObj::DrawSizeState(pView);

	// 	CClientDC dc(pView);
	// 	pView->OnPrepareDC(&dc);
	// 	int mode = dc.SetROP2(R2_XORPEN);
	// 	CRect rect = m_rtPosition;
	// 	CPen pen;
	// 	pen.CreatePen(PS_SOLID, 0, RGB(255, 0, 0));
	// 	CPen *pOldPen = dc.SelectObject(&pen);
	// 	CBrush *pOldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	// 	dc.SetROP2(R2_XORPEN);
	// 	
	// 	dc.Rectangle(rect);
	// 	
	// 	dc.SelectObject(pOldBrush);
	// 	dc.SelectObject(pOldPen);	
	// 	dc.SetROP2(mode);	
}

void CGeoPointObj::MoveTo(const CRect &position, CGeoMapView *pView)
{
	ASSERT_VALID(this);

	if( m_bLocked )
		return;

	if (position == m_rtPosition)
		return;

	CPoint pt = m_rtPosition.CenterPoint();
	CPoint delta = position.CenterPoint();
	delta -= pt;

	if (pView == NULL)
	{
		Invalidate();
		m_rtPosition = position;
		m_ptSymbolPoint += delta;
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

		m_rtPosition = position;		
		m_ptSymbolPoint += delta;
		if( m_bEditing )
			DrawEditMode( pView );
		else if( m_bSizeChanging )
			DrawSizeState( pView );
		else if( m_bMoving )
			DrawMoveState( pView );
		else
			pView->InvalObj(this);
	}

	pt = m_rtPosition.CenterPoint();

	// 转换为大地坐标
	m_geoCenterPoint = CoordinateTransfer(pt);
	m_geoPosition = CoordinateTransfer(m_rtPosition);

	m_ptSymbolCoord = CoordinateTransfer(m_ptSymbolPoint);

	m_pDocument->SetModifiedFlag();

	RecalcBounds(pView);
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
void CGeoPointObj::MoveHandleTo(int nHandle, CPoint point, CGeoMapView *pView)
{
	ASSERT_VALID(this);

	if( m_bLocked )
		return;

	CPoint pt = m_ptSymbolPoint + m_ptName;
	if ( point == pt )
	{
		return;
	}
	m_ptName = point-m_ptSymbolPoint;

	Invalidate(pView);

	RecalcBounds(pView);

	Invalidate(pView);

	m_pDocument->SetModifiedFlag();
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::OnEditProperties
// 返回  : void
// 参数  : CGeoMapView *pView
// 说明  : 编辑图素属性
//
BOOL CGeoPointObj::OnEditProperties(CGeoMapView *pView)
{
	ASSERT_VALID(this);
	if( m_bLocked )
		return FALSE;

	return FALSE;
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::Clone
// 返回  : CGeoDrawObj*
// 参数  : CGeoMapDoc *pDoc
// 说明  : 克隆图素并将图素加入到文档中
//
CGeoDrawObj* CGeoPointObj::Clone(CDrawLayer *pLayer)
{
	ASSERT_VALID(this);

	CGeoPointObj* pClone = new CGeoPointObj(pLayer);
	pClone->m_rtPosition	=	m_rtPosition;
	pClone->m_bLocked		=	m_bLocked;			// 锁定图素
	pClone->m_bShow			=	m_bShow;			// 是否显示
	pClone->m_strID			=	newGUID();			// 新的ID
	pClone->m_ptAnchors.Copy(m_ptAnchors);			// 锚点

	pClone->m_pDocument		=	m_pDocument;		// 文档

	//////////////////////////////////////////////////////////////////////////
	pClone->m_ptSymbolPoint		= m_ptSymbolPoint;
	pClone->m_ptSymbolCoord		= m_ptSymbolCoord;

	pClone->m_strName	= m_strName;
	pClone->m_ptName	= m_ptName;	
	pClone->m_iSymbolSize		= m_iSymbolSize;				// 1mm;

	memcpy(&pClone->m_lfFont, &m_lfFont, sizeof(LOGFONT) );	
	pClone->m_clrFont = m_clrFont;				// 字体颜色
	pClone->m_clrSymbolPen = m_clrSymbolPen;
	pClone->m_clrSymbolFill = m_clrSymbolFill;

	ASSERT_VALID(pClone);

	if (pClone != NULL)
	{
		m_pDocument->Do(new CCmdAddDrawObj(m_pDocument, pClone), TRUE);
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
void CGeoPointObj::Serialize(CArchive &ar)
{
	CGeoDrawObj::Serialize(ar);

	ar.SerializeClass(RUNTIME_CLASS(CGeoPointObj));

	if (ar.IsStoring())
	{
		ar << m_ptSymbolPoint;
		ar << m_ptSymbolCoord;

		ar << m_strName;
		ar << m_ptName;	
		ar << m_iSymbolSize;				// 1mm;

		ar.Write(&m_lfFont, sizeof(LOGFONT) );	
		ar << m_clrFont;				// 字体颜色	
		ar << m_clrSymbolPen;
		ar << m_clrSymbolFill;
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

				ar >> m_ptSymbolPoint;
				ar >> m_ptSymbolCoord;

				ar >> m_strName;
				ar >> m_ptName;	
				ar >> m_iSymbolSize;				// 1mm;
				ar.Read(&m_lfFont, sizeof(LOGFONT) );	
				ar >> m_clrFont;				// 字体颜色
				ar >> m_clrSymbolPen;
				ar >> m_clrSymbolFill;
			}
			break;
		default:
			break;
		}		
	}
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::GetHandleCount
// 返回  : int
// 说明  : 获取手柄总数
//
int CGeoPointObj::GetHandleCount()
{
	ASSERT_VALID(this);
	return 1;
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::GetHandle
// 返回  : CPoint
// 参数  : int nHandle
// 说明  : 获取某手柄的中心逻辑坐标
//
CPoint CGeoPointObj::GetHandle(int nHandle)
{
	CPoint pt = m_ptSymbolPoint + m_ptName;

	return pt;
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::GetHandleCursor
// 返回  : HCURSOR
// 参数  : int nHandle
// 说明  : 获取某手柄的鼠标指针
//
HCURSOR CGeoPointObj::GetHandleCursor(int nHandle)
{	
	return AfxGetApp()->LoadCursor(IDC_CURSOR_MOVE_PT);
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
int CGeoPointObj::HitTestHandle(CPoint point, CGeoMapView *pView, BOOL bSelected)
{
	return CGeoDrawObj::HitTestHandle(point, pView, bSelected);
}

// 当比例尺发生改变时
void CGeoPointObj::OnScaleChanged()
{
	// 转换为屏幕坐标
	CPoint point = CoordinateTransfer(m_geoCenterPoint);

	int iWidth = m_rtPosition.Width();
	int iHeight = m_rtPosition.Height();

	m_rtPosition.SetRect(
		point.x - iWidth/2 - iWidth%2, 
		point.y - iHeight/2 - iHeight%2,
		point.x + iWidth/2,
		point.y + iHeight/2);

	m_ptSymbolPoint = CoordinateTransfer(m_ptSymbolCoord);

	RecalcBounds(NULL);
}

// 当发生旋转时
void CGeoPointObj::OnAngleChanged()
{

}

// 当发生坐标原点有改变时
void CGeoPointObj::OnCoordOrigChanged()
{

}


BOOL CGeoPointObj::RecalcBounds(CGeoMapView* pView)
{
	CRect rect;
	CPoint pt1 = m_ptSymbolPoint;
	CPoint pt2 = m_ptName+m_ptSymbolPoint;
	int nWidth = m_strName.GetLength() * labs(m_lfFont.lfHeight)/2 + 20;	
	rect.left = min(pt1.x-m_iSymbolSize/2-10, pt2.x-nWidth/2);
	rect.right = max(pt1.x+m_iSymbolSize/2+10, pt2.x+nWidth/2);
	rect.top = min(pt1.y-m_iSymbolSize/2-10,pt2.y - labs(m_lfFont.lfHeight)/2-10);
	rect.bottom = max(pt1.y+m_iSymbolSize/2+10,pt2.y + labs(m_lfFont.lfHeight)/2+10);

	m_rtPosition = rect;

	return CGeoDrawObj::RecalcBounds(pView);
}


void CGeoPointObj::CopyAttribute(CGeoDrawObj *pSrcObj)
{
	CGeoDrawObj::CopyAttribute(pSrcObj);

	CGeoPointObj *pSrcWell = (CGeoPointObj*)pSrcObj;

	m_clrSymbolFill = pSrcWell->m_clrSymbolFill;
	m_clrSymbolPen = pSrcWell->m_clrSymbolPen;

	m_clrFont = pSrcWell->m_clrFont;
	memcpy(&m_lfFont, &pSrcWell->m_lfFont, sizeof(LOGFONT) );
	m_iSymbolSize = pSrcWell->m_iSymbolSize;
	m_ptName = pSrcWell->m_ptName;
}

