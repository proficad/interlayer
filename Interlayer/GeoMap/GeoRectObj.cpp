// GeoRectObj.cpp: implementation of the CGeoRectObj class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GeoRectObj.h"
#include "GeoMapView.h"
#include "DlgRectSet.h"
#include "CmdGeoDrawObj.h"

#include "RgnClipping.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


IMPLEMENT_SERIAL(CGeoRectObj, CGeoDrawObj, VERSIONABLE_SCHEMA|0)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeoRectObj::CGeoRectObj()
{
	m_iSpaceWidth = 10;
	m_eLine2.m_nPenWide = 10;
}

CGeoRectObj::CGeoRectObj(CDrawLayer* pLayer)
:CGeoDrawObj(pLayer)
{
	m_iSpaceWidth = 10;
	m_eLine2.m_nPenWide = 10;
}

CGeoRectObj::CGeoRectObj(const CRect& position, CDrawLayer* pLayer)
:CGeoDrawObj(position,pLayer)
{
	m_iSpaceWidth = 10;
	m_eLine2.m_nPenWide = 10;
}

CGeoRectObj::~CGeoRectObj()
{

}

void CGeoRectObj::Draw(CDC *pDC, CGeoMapView *pView)
{
	ASSERT_VALID(this);
	
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
	
	int nSaved = pDC->SaveDC();
	pDC->SetBkMode(TRANSPARENT);          //设定显示方式
	
	switch(m_eLine.m_nLineType)
	{
	case 0://单线
		DrawFill(pDC, pView, CGeoRectObj::MIDST);
		DrawLine(pDC, pView, CGeoRectObj::MIDST, TRUE);
		break;
	case 1://双线
		DrawFill(pDC, pView, CGeoRectObj::OUTER);
		DrawLineSpace(pDC, pView);
		DrawLine(pDC, pView, CGeoRectObj::OUTER, TRUE);
		DrawLine(pDC, pView, CGeoRectObj::INNER, TRUE);
		break;
	case 2://文武线1
		DrawFill(pDC, pView, CGeoRectObj::OUTER);
		DrawLineSpace(pDC, pView);
		DrawLine(pDC, pView, CGeoRectObj::OUTER, TRUE);
		DrawLine(pDC, pView, CGeoRectObj::INNER, FALSE);//粗
		break;
	case 3://文武线2;
		DrawFill(pDC, pView, CGeoRectObj::OUTER);
		DrawLineSpace(pDC, pView);
		DrawLine(pDC, pView, CGeoRectObj::OUTER, FALSE);
		DrawLine(pDC, pView, CGeoRectObj::INNER, TRUE);
		break;
	case 4://三线1
		DrawFill(pDC, pView, CGeoRectObj::OUTER);
		DrawLineSpace(pDC, pView);
		DrawLine(pDC, pView, CGeoRectObj::OUTER, TRUE);
		DrawLine(pDC, pView, CGeoRectObj::INNER, TRUE);		
		DrawLine(pDC, pView, CGeoRectObj::MIDST, TRUE);
		break;
	case 5://三线2
		DrawFill(pDC, pView, CGeoRectObj::OUTER);
		DrawLineSpace(pDC, pView);
		DrawLine(pDC, pView, CGeoRectObj::OUTER, FALSE);
		DrawLine(pDC, pView, CGeoRectObj::INNER, FALSE);		
		DrawLine(pDC, pView, CGeoRectObj::MIDST, TRUE);
		break;
	case 6://三线3
		DrawFill(pDC, pView, CGeoRectObj::OUTER);
		DrawLineSpace(pDC, pView);
		DrawLine(pDC, pView, CGeoRectObj::OUTER, TRUE);
		DrawLine(pDC, pView, CGeoRectObj::INNER, TRUE);		
		DrawLine(pDC, pView, CGeoRectObj::MIDST, FALSE);
		break;
	}

	pDC->RestoreDC(nSaved);
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoRectObj::OnEditProperties
// 返回  : void
// 参数  : CGeoMapView *pView
// 说明  : 编辑图素属性
//
BOOL CGeoRectObj::OnEditProperties(CGeoMapView *pView)
{
	ASSERT_VALID(this);
	if( m_bLocked )
		return FALSE;

	CDlgRectSet dlg;
	dlg.eFill = m_eFill;
	dlg.eLine = m_eLine;
	dlg.eLine2 = m_eLine2;
	dlg.m_iSpaceWidth = m_iSpaceWidth;
	
	if(dlg.DoModal() == IDOK)
	{
		m_eFill = dlg.eFill;
		m_eLine = dlg.eLine;
		m_eLine2 = dlg.eLine2;
		m_iSpaceWidth = dlg.m_iSpaceWidth;

		pView->InvalObj(this);
		m_pDocument->SetModifiedFlag();
		return TRUE;
	}

	return FALSE;
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::Clone
// 返回  : CGeoDrawObj*
// 参数  : CGeoMapDoc *pDoc
// 说明  : 克隆图素并将图素加入到文档中
//
CGeoDrawObj* CGeoRectObj::Clone(CDrawLayer *pLayer)
{
	ASSERT_VALID(this);
	
	CGeoRectObj* pClone = new CGeoRectObj(m_rtPosition,pLayer);
	pClone->m_rtPosition	=	m_rtPosition;
	pClone->m_bLocked		=	m_bLocked;			// 锁定图素
	pClone->m_bShow			=	m_bShow;			// 是否显示
	pClone->m_strID			=	newGUID();			// 新的ID
	pClone->m_ptAnchors.Copy(m_ptAnchors);			// 锚点

	pClone->m_pDocument		=	m_pDocument;		// 文档
	pClone->m_geoCenterPoint		=	m_geoCenterPoint;


	pClone->m_eLine				= m_eLine;							// 线属性
	pClone->m_eFill				= m_eFill;							// 填充属性
	pClone->m_eLine2			= m_eLine2;
	pClone->m_iSpaceWidth		= m_iSpaceWidth;

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
void CGeoRectObj::Serialize(CArchive &ar)
{
	CGeoDrawObj::Serialize(ar);
	
	ar.SerializeClass(RUNTIME_CLASS(CGeoRectObj));

	if (ar.IsStoring())
	{
		ar << m_eLine;
		ar << m_eFill;
		ar << m_eLine2;
		ar << m_iSpaceWidth;
	}
	else
	{
		// get the document back pointer from the archive

		int nVersion = ar.GetObjectSchema(); // 获取版本信息
		switch( nVersion )
		{
		case 0:
			{				
				ar >> m_eLine;
				ar >> m_eFill;
				ar >> m_eLine2;
				ar >> m_iSpaceWidth;
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
// 函数  : CGeoRectObj::HitTestHanle
// 返回  : int
// 参数  : CPoint point是逻辑坐标
// 参数  : CGeoMapView *pView
// 参数  : BOOL bSelected 如果已经选中，返回是否点中某个手柄
// 说明  : 否则，如果返回1选中该图素，否则没有点中该图素
//
int CGeoRectObj::HitTestHandle(CPoint point, CGeoMapView *pView, BOOL bSelected)
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
		if(IsPoint(point,pView))
			return 1;
	}
	return 0;
}

// 当比例尺发生改变时
void CGeoRectObj::OnScaleChanged()
{
	CGeoDrawObj::OnScaleChanged();
}

// 当发生旋转时
void CGeoRectObj::OnAngleChanged()
{
	Invalidate();
}

// 当发生坐标原点有改变时
void CGeoRectObj::OnCoordOrigChanged()
{
	Invalidate();
}

void CGeoRectObj::DrawLineSpace(CDC *pDC, CGeoMapView *pView)
{
	if( !m_eLine.eLineSpaceFill.m_bShow )
		return;
	
	CRect rect;
	rect = m_rtPosition;
	rect.NormalizeRect();
	if( m_eLine.m_nLineType )
		rect.DeflateRect(m_iSpaceWidth,m_iSpaceWidth);
	
	CPen pen;
	CBrush* pOldBrush;
	CPen* pOldPen;	
	
	pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
	
	if( m_eLine.m_bShow )
	{
		if(! CreatePenLine(&pen, pDC) )
			return;
		pOldPen = pDC->SelectObject(&pen);
	}
	else
	{		
		pDC->SelectObject(pOldBrush);
		return;		
	}
	
	pDC->Rectangle(rect);
	
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);	

}

void CGeoRectObj::DrawLine(CDC *pDC, CGeoMapView *pView, PLACE place, BOOL bThin)//画线
{	
	CRect rect;
	rect = m_rtPosition;	
	rect.NormalizeRect();

	if( m_eLine.m_nLineType )
		rect.DeflateRect(m_iSpaceWidth,m_iSpaceWidth);
	if(place == INNER)//内
		rect.DeflateRect(m_iSpaceWidth, m_iSpaceWidth);
	if(place == OUTER)
		rect.DeflateRect(-m_iSpaceWidth, -m_iSpaceWidth);
	
	CPen pen;
	CBrush* pOldBrush;
	CPen* pOldPen;	
	
	pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
	
	if( m_eLine.m_bShow )
	{
		if(! CreatePen(&pen, pDC, bThin) )
			return;
		pOldPen = pDC->SelectObject(&pen);
	}
	else
	{		
		pDC->SelectObject(pOldBrush);
		return;		
	}

	pDC->Rectangle(rect);

	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);	
}

void CGeoRectObj::DrawFill(CDC *pDC, CGeoMapView *pView, PLACE place)
{
	if( m_bEditing )
		return;
	
	CRect rect = m_rtPosition;
	rect.NormalizeRect();

	if( m_eLine.m_nLineType )
		rect.DeflateRect(m_iSpaceWidth,m_iSpaceWidth);

	if(place == INNER)//内
		rect.DeflateRect(m_iSpaceWidth, m_iSpaceWidth);
	
	if(place == OUTER)
		rect.DeflateRect(-m_iSpaceWidth, -m_iSpaceWidth);
	
	CPen* pOldPen;	
	
	if( m_eFill.m_bShow && m_eFill.blendFunction.SourceConstantAlpha != 0)//如果填充
	{
		CBrush brush;
		CBrush* pOldBrush;		
		
		if(! CreateBrush(&brush, pDC) )
			return;		
		if(m_rgn.m_hObject && m_eFill.blendFunction.SourceConstantAlpha != 255)
		{			
			SelectClipRgn(pDC);

			pDC->SetBkColor(m_eFill.sFillStyle.m_PatternFill.m_crColourBack);
			pDC->SetTextColor(m_eFill.sFillStyle.m_PatternFill.m_crColourFore);			

			CRect rt;
			rt = rect;
			pView->DocToClient(rt);
			rt.NormalizeRect();

			CDC MemDC;
			MemDC.CreateCompatibleDC(pDC);
			CBitmap bit;
			bit.CreateBitmap(rt.Width(), rt.Height(), 1, 32, NULL);
			CBitmap *pOldBit = MemDC.SelectObject(&bit);
			CRect FillRt(0, 0, 0, 0);
			FillRt.right = rt.Width();
			FillRt.bottom = rt.Height();

			MemDC.SetBkColor(m_eFill.sFillStyle.m_PatternFill.m_crColourBack);
			MemDC.SetTextColor(m_eFill.sFillStyle.m_PatternFill.m_crColourFore);
			
			MemDC.FillRect(FillRt, &brush);
			AlphaBlend(pDC->m_hDC, rect.left, rect.top, rect.Width(), rect.Height(), MemDC.m_hDC, 0, 0, FillRt.Width(), FillRt.Height(), m_eFill.blendFunction);
			MemDC.SelectObject(pOldBit);
			bit.DeleteObject();
			MemDC.DeleteDC();
			pDC->SelectClipRgn(NULL);
		}
		else
		{
			pOldBrush = pDC->SelectObject(&brush);		
			pOldPen = (CPen*)pDC->SelectStockObject(NULL_PEN);	
			
			pDC->Rectangle(rect);
			
			pDC->SelectObject(pOldBrush);
			pDC->SelectObject(pOldPen);
		}
	}	
	
}

BOOL CGeoRectObj::IsPoint(CPoint point, CGeoMapView *pView)
{
	if(!m_bShow)
		return FALSE;
	
	if( m_pParentLayer->m_rgnClipping.m_nPoints > 2
		&& m_pParentLayer->m_bClipping)		
	{
		CRect rt = m_pParentLayer->m_rgnClipping.m_rtPosition;
		rt.NormalizeRect();
		if(!rt.PtInRect(point))
			return FALSE;
	}

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

	if( !m_eFill.m_bShow )
		dc.WidenPath();
		  
	if( m_rgn.m_hObject )
		m_rgn.DeleteObject();
	
	m_rgn.CreateFromPath(&dc);

	if( !m_rgn.m_hObject )
		return FALSE;
	
	if( m_rgn.PtInRegion(point) )
		return TRUE;
	else
		return FALSE;
}


void CGeoRectObj::SelectClipRgn(CDC *pDC)
{
	CRect rect = m_rtPosition;
	rect.NormalizeRect();
	
	pDC->BeginPath();	
	
	pDC->Rectangle(rect);
	
	pDC->EndPath();
	
	pDC->SelectClipPath(RGN_COPY);
	
	return;
}


BOOL CGeoRectObj::CreatePen(CPen *pPen, CDC *pDC, BOOL bThin)
{
	
	int penStyle[7]=
	{
		//笔的固定类型值，为以下几种，其它自定
		PS_SOLID,//            0
			PS_DASH,//             1       /* -------  */
			PS_DOT,//              2       /* .......  */
			PS_DASHDOT,//          3       /* _._._._  */
			PS_DASHDOTDOT,//       4       /* _.._.._  */
			PS_INSIDEFRAME,//      5
			PS_NULL//             6
	};
	
	if( m_eLine.m_nPenStyle == 6 )//如果无则直接返回
		return FALSE;
	
	int nWide;	
	
	if(bThin)
		nWide = m_eLine.m_nPenWide;
	else
		nWide = m_eLine2.m_nPenWide;
	
	int Endcap;
	
	if ( m_eLine.m_bSquarePen )
		Endcap = PS_ENDCAP_FLAT;
	else
		Endcap = PS_ENDCAP_ROUND;
	
	CBrush brush;
	
	/*unsigned Type[8];
	int c = CDashLine::GetPattern(Type, m_eLine.m_bSquarePen, 5, m_eLine.m_nPenStyle);
	
	  DashLine.SetDC(pDC);
	  
		if( c ) 
		DashLine.SetPattern(Type, c);
	*/
	
	if(bThin)
	{
		if(m_eLine.s_LineStyle.m_PatternFill.m_nRow != -1 
			&& m_eLine.s_LineStyle.m_PatternFill.m_nCol != -1)
		{
			
			if(!m_eLine.s_LineStyle.CreateBrush(&brush, pDC))
				return FALSE;
			
			CDC MemDC;
			MemDC.CreateCompatibleDC(pDC);
			CBitmap bit;
			bit.CreateCompatibleBitmap(&MemDC, 8, 8);
			CBitmap *pOldBit = MemDC.SelectObject(&bit);
			CRect rect(0, 0, 8, 8);
			MemDC.FillRect(rect, &brush);
			MemDC.SelectObject(pOldBit);
			MemDC.DeleteDC();
			
			CBrush b;
			b.CreatePatternBrush(&bit);
			
			LOGBRUSH logbrush;
			b.GetLogBrush(&logbrush);
			
			
			if (!pPen->CreatePen(penStyle[m_eLine.m_nPenStyle] | PS_GEOMETRIC | PS_JOIN_MITER | Endcap, 
				nWide, 
				&logbrush) 
				)
				return FALSE;
		}	
		else
		{
			LOGBRUSH logbrush;
			logbrush.lbColor = m_eLine.s_LineStyle.m_PatternFill.m_crColourFore;
			logbrush.lbStyle = BS_SOLID;
			logbrush.lbHatch = NULL;
			
			if (!pPen->CreatePen(penStyle[m_eLine.m_nPenStyle] | PS_GEOMETRIC | PS_JOIN_MITER | Endcap, 
				nWide, 
				&logbrush) 
				)
				return FALSE;
		}
	}
	else
	{
		if(m_eLine2.s_LineStyle.m_PatternFill.m_nRow != -1 
			&& m_eLine2.s_LineStyle.m_PatternFill.m_nCol != -1)
		{
			
			if(!m_eLine2.s_LineStyle.CreateBrush(&brush, pDC))
				return FALSE;
			
			CDC MemDC;
			MemDC.CreateCompatibleDC(pDC);
			CBitmap bit;
			bit.CreateCompatibleBitmap(&MemDC, 8, 8);
			CBitmap *pOldBit = MemDC.SelectObject(&bit);
			CRect rect(0, 0, 8, 8);
			MemDC.FillRect(rect, &brush);
			MemDC.SelectObject(pOldBit);
			MemDC.DeleteDC();
			
			CBrush b;
			b.CreatePatternBrush(&bit);
			
			LOGBRUSH logbrush;
			b.GetLogBrush(&logbrush);
			
			
			if (!pPen->CreatePen(penStyle[m_eLine2.m_nPenStyle] | PS_GEOMETRIC | PS_JOIN_MITER | Endcap, 
				nWide, 
				&logbrush) 
				)
				return FALSE;
		}	
		else
		{
			LOGBRUSH logbrush;
			logbrush.lbColor = m_eLine2.s_LineStyle.m_PatternFill.m_crColourFore;
			logbrush.lbStyle = BS_SOLID;
			logbrush.lbHatch = NULL;
			
			if (!pPen->CreatePen(penStyle[m_eLine2.m_nPenStyle] | PS_GEOMETRIC | PS_JOIN_MITER | Endcap, 
				nWide, 
				&logbrush) 
				)
				return FALSE;
		}
	}	
	
	return TRUE;	
}

BOOL CGeoRectObj::CreatePenLine(CPen *pPen, CDC *pDC)
{
	int nWide = 2*m_iSpaceWidth;	
	
	CBrush brush;
	
	if(m_eLine.eLineSpaceFill.sFillStyle.m_PatternFill.m_nRow != -1 && m_eLine.eLineSpaceFill.sFillStyle.m_PatternFill.m_nCol != -1)
	{
		
		if(!m_eLine.eLineSpaceFill.sFillStyle.CreateBrush(&brush, pDC))
			return FALSE;
		
		CDC MemDC;
		MemDC.CreateCompatibleDC(pDC);
		CBitmap bit;
		bit.CreateCompatibleBitmap(&MemDC, 8, 8);
		CBitmap *pOldBit = MemDC.SelectObject(&bit);
		CRect rect(0, 0, 8, 8);
		MemDC.FillRect(rect, &brush);
		MemDC.SelectObject(pOldBit);
		MemDC.DeleteDC();
		
		CBrush b;
		b.CreatePatternBrush(&bit);
		
		LOGBRUSH logbrush;
		b.GetLogBrush(&logbrush);
		
		
		if (!pPen->CreatePen(PS_SOLID | PS_GEOMETRIC | PS_ENDCAP_FLAT | PS_JOIN_MITER, 
			nWide, 
			&logbrush) 
			)
			return FALSE;
	}	
	else
	{
		LOGBRUSH logbrush;
		logbrush.lbColor = m_eLine.eLineSpaceFill.sFillStyle.m_PatternFill.m_crColourFore;
		logbrush.lbStyle = BS_SOLID;
		logbrush.lbHatch = NULL;
		
		if (!pPen->CreatePen(PS_SOLID | PS_GEOMETRIC | PS_ENDCAP_FLAT | PS_JOIN_MITER, 
			nWide, 
			&logbrush) 
			)
			return FALSE;
	}
	
	return TRUE;	
}


BOOL CGeoRectObj::CreateBrush(CBrush *pBrush, CDC *pDC)
{	
	if(m_eFill.sFillStyle.m_PatternFill.m_nRow != -1 && m_eFill.sFillStyle.m_PatternFill.m_nCol != -1)
	{
		
		if(!m_eFill.sFillStyle.CreateBrush(pBrush, pDC))
			return FALSE;		
	}
	
	else
	{
		if (!pBrush->CreateSolidBrush( m_eFill.sFillStyle.m_PatternFill.m_crColourFore ) )
			return FALSE;
	}
	return TRUE;
}

void CGeoRectObj::CopyAttribute(CGeoDrawObj *pSrcObj)
{	
	CGeoRectObj *pObj = (CGeoRectObj*)pSrcObj;
	m_eLine = pObj->m_eLine;
	m_eFill = pObj->m_eFill;
	m_eLine2 = pObj->m_eLine2;
	m_iSpaceWidth = pObj->m_iSpaceWidth;
	
	CGeoDrawObj::CopyAttribute(pSrcObj);
}