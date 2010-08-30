// FieldCell.cpp : 实现文件
//

#include "stdafx.h"
#include "Interlayer.h"
#include "FieldCell.h"
#include "2DModelView.h"

IMPLEMENT_SERIAL(CFieldCell, CObject, VERSIONABLE_SCHEMA|0)

CFieldCell::CFieldCell()
{
	m_clrCenter = RGB(255,255,255);
	m_clrCorner[0] = RGB(255,255,255);
	m_clrCorner[1] = RGB(255,255,255);
	m_clrCorner[2] = RGB(255,255,255);
	m_clrCorner[3] = RGB(255,255,255);

	m_clrCorner[4] = RGB(255,255,255);
	m_clrCorner[5] = RGB(255,255,255);
	m_clrCorner[6] = RGB(255,255,255);
	m_clrCorner[7] = RGB(255,255,255);

	m_bDispTop = TRUE;
}

CFieldCell::~CFieldCell()
{
}

CVertex3D CFieldCell::PointOfIntersection()
{
	double a1, b1, c1, a2, b2, c2;

	int iIndex = 0;
	if( m_bDispTop )
		iIndex = 0;
	else
		iIndex = 4;

	int sign = 1; 
	a1 = m_cornerPoint[iIndex+2].y - m_cornerPoint[iIndex+0].y; 
	if (a1 < 0.0) 
	{ 
		sign = -1; 
		a1 = sign*a1; 
	} 
	b1 = sign* (m_cornerPoint[iIndex+0].x - m_cornerPoint[iIndex+2].x); 
	c1 = sign* (m_cornerPoint[iIndex+0].y*m_cornerPoint[iIndex+2].x - m_cornerPoint[iIndex+0].x*m_cornerPoint[iIndex+2].y); 

	sign = 1; 
	a2 = m_cornerPoint[iIndex+3].y - m_cornerPoint[iIndex+1].y; 
	if (a1 < 0.0) 
	{ 
		sign = -1; 
		a2 = sign*a1; 
	} 

	b2 = sign* (m_cornerPoint[iIndex+1].x - m_cornerPoint[iIndex+3].x); 
	c2 = sign* (m_cornerPoint[iIndex+1].y*m_cornerPoint[iIndex+3].x - m_cornerPoint[iIndex+1].x*m_cornerPoint[iIndex+3].y);

	CVertex3D p;

	double d = a1*b2 - a2*b1;
	if (fabs(d) < 1E-10)
	{
		p.x = -1;
		p.y = -1;
	}
	else
	{
		p.x = (c2*b1 - c1*b2) / d; 
		p.y = (a2*c1 - a1*c2) / d; 
	}

	return p;
}

void CFieldCell::Serialize(CArchive& ar)
{

	CObject::Serialize(ar);

	ar.SerializeClass(RUNTIME_CLASS(CFieldCell));

	if (ar.IsStoring())
	{
		ar << m_cornerPoint[0];
		ar << m_cornerPoint[1];
		ar << m_cornerPoint[2];
		ar << m_cornerPoint[3];
		ar << m_cornerPoint[4];
		ar << m_cornerPoint[5];
		ar << m_cornerPoint[6];
		ar << m_cornerPoint[7];
		ar << m_dValue;
		ar << m_bDispTop;
	}
	else
	{
		// get the document back pointer from the archive

		int nVersion = ar.GetObjectSchema(); // 获取版本信息
		switch( nVersion )
		{
		case 0:
			{
				ar >> m_cornerPoint[0];
				ar >> m_cornerPoint[1];
				ar >> m_cornerPoint[2];
				ar >> m_cornerPoint[3];
				ar >> m_cornerPoint[4];
				ar >> m_cornerPoint[5];
				ar >> m_cornerPoint[6];
				ar >> m_cornerPoint[7];
				ar >> m_dValue;
				ar >> m_bDispTop;
			}
			break;
		default:
			break;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
// CFieldCell message handlers

void CFieldCell::Draw(CDC *pDC,  C2DModelView *pView)
{
	if( !pDC->IsPrinting() )
	{
		CRect cliRect;
		pView->GetClientRect(cliRect);
		pView->ClientToDoc(cliRect);
		if( !Intersects(cliRect) ) //如果该图素不在当前窗口中则不显示
			return ;
	}

	if( pView->m_bDrawFill )
	{
		if( pView->m_bGradientFill )
			GradientFill(pDC, pView);
		else
			DrawFill(pDC, pView);
	}
	if( pView->m_bDrawText)
		DrawText(pDC, pView);
	if( pView->m_bDrawBorder)
		DrawBorder(pDC, pView);
}

void CFieldCell::DrawText(CDC *pDC, C2DModelView *pView)
{	
	int nSave = pDC->SaveDC();
	pDC->SetBkMode(1);

	int iIndex = 0;
	if( m_bDispTop )
		iIndex = 0;
	else
		iIndex = 4;

	CFont font;
	font.CreateFontIndirect(&pView->m_lgFont);
	CFont *pOldFont = pDC->SelectObject(&font);

	CString strTmp;
	strTmp.Format("%g",m_dValue);

	CRect rect;
	CRgn rgn;
	rgn.CreatePolygonRgn(&m_point[iIndex], 4, ALTERNATE);
	rgn.GetRgnBox(rect);
	pDC->DrawText(strTmp, rect, DT_VCENTER|DT_CENTER|DT_SINGLELINE);
	pDC->SelectObject(pOldFont);

	pDC->RestoreDC(nSave);
}

void CFieldCell::DrawBorder(CDC *pDC, C2DModelView *pView)
{
	int nSave = pDC->SaveDC();

	CPen pen1(PS_SOLID, 0, RGB(0,0,0));
	CPen pen2(PS_SOLID, 0, RGB(191,191,191));

	CPen *pOldPen;

	int iIndex = 0;
	if( m_bDispTop )
		iIndex = 0;
	else
		iIndex = 4;

	pOldPen = pDC->SelectObject(&pen2);

	if( m_i == 0 )
	{
		pOldPen = pDC->SelectObject(&pen1);
		pDC->MoveTo(m_point[iIndex+0]);
		pDC->LineTo(m_point[iIndex+3]);
		pDC->SelectObject(pOldPen);
	}
	else
	{
		pDC->MoveTo(m_point[iIndex+0]);
		pDC->LineTo(m_point[iIndex+3]);
	}

	if( m_i == m_nX )
	{
		pOldPen = pDC->SelectObject(&pen1);
		pDC->MoveTo(m_point[iIndex+1]);
		pDC->LineTo(m_point[iIndex+2]);
		pDC->SelectObject(pOldPen);
	}
	else
	{
		pDC->MoveTo(m_point[iIndex+1]);
		pDC->LineTo(m_point[iIndex+2]);
	}

	if( m_j == 0 )
	{
		pOldPen = pDC->SelectObject(&pen1);
		pDC->MoveTo(m_point[iIndex+0]);
		pDC->LineTo(m_point[iIndex+1]);
		pDC->SelectObject(pOldPen);
	}
	else
	{
		pDC->MoveTo(m_point[iIndex+0]);
		pDC->LineTo(m_point[iIndex+1]);
	}
	if( m_j == m_nY )
	{
		pOldPen = pDC->SelectObject(&pen1);
		pDC->MoveTo(m_point[iIndex+2]);
		pDC->LineTo(m_point[iIndex+3]);
		pDC->SelectObject(pOldPen);
	}
	else
	{
		pDC->MoveTo(m_point[iIndex+2]);
		pDC->LineTo(m_point[iIndex+3]);
	}

	pDC->SelectObject(pOldPen);


	pDC->RestoreDC(nSave);
}

void CFieldCell::DrawStateSelected(CDC *pDC, C2DModelView *pView)
{	
	CRect cliRect;
	pView->GetClientRect(cliRect);
	pView->ClientToDoc(cliRect);
	if( !Intersects(cliRect) ) //如果该图素不在当前窗口中则不显示
		return ;
	int nSave = pDC->SaveDC();

	CPen pen(PS_SOLID, 0, RGB(0,255,255));

	int iIndex = 0;
	if( m_bDispTop )
		iIndex = 0;
	else
		iIndex = 4;

	CPen *pOldPen;	
	pOldPen = pDC->SelectObject(&pen);
	pDC->MoveTo(m_point[iIndex+0]);
	pDC->LineTo(m_point[iIndex+1]);
	pDC->LineTo(m_point[iIndex+2]);
	pDC->LineTo(m_point[iIndex+3]);
	pDC->LineTo(m_point[iIndex+0]);
	pDC->SelectObject(pOldPen);	
	pDC->RestoreDC(nSave);
}


void CFieldCell::DrawCurCell(CDC *pDC, C2DModelView *pView)
{	
	CRect cliRect;
	pView->GetClientRect(cliRect);
	pView->ClientToDoc(cliRect);
	if( !Intersects(cliRect) ) //如果该图素不在当前窗口中则不显示
		return ;

	int nSave = pDC->SaveDC();

	CPen pen(PS_SOLID, 0, RGB(0,0,0));

	int iIndex = 0;
	if( m_bDispTop )
		iIndex = 0;
	else
		iIndex = 4;

	CPen *pOldPen;	
	pOldPen = pDC->SelectObject(&pen);
	pDC->MoveTo(m_point[iIndex+0]);
	pDC->LineTo(m_point[iIndex+1]);
	pDC->LineTo(m_point[iIndex+2]);
	pDC->LineTo(m_point[iIndex+3]);
	pDC->LineTo(m_point[iIndex+0]);
	pDC->SelectObject(pOldPen);	
	pDC->RestoreDC(nSave);
}

CRect CFieldCell::GetRect ()
{
	CRect rect;
	CRgn rgn;
	int iIndex = 0;
	if( m_bDispTop )
		iIndex = 0;
	else
		iIndex = 4;
	rgn.CreatePolygonRgn(&m_point[iIndex], 4, ALTERNATE);
	rgn.GetRgnBox(rect);
	return rect;
}

BOOL CFieldCell::PtInRegion(const CPoint &point)
{
	CRgn rgn;
	int iIndex = 0;
	if( m_bDispTop )
		iIndex = 0;
	else
		iIndex = 4;
	rgn.CreatePolygonRgn(&m_point[iIndex], 4, ALTERNATE);
	return rgn.PtInRegion(point);
}

void CFieldCell::DrawFill(CDC *pDC, C2DModelView *pView)
{
	int nSave = pDC->SaveDC();

	int iIndex = 0;
	if( m_bDispTop )
		iIndex = 0;
	else
		iIndex = 4;

	CPen pen(PS_NULL, 0, RGB(0,0,0));
	CPen *pOldPen = pDC->SelectObject(&pen);

	CBrush brush(m_clrCenter);
	CBrush *pOldBrush = pDC->SelectObject(&brush);

	pDC->Polygon(&m_point[iIndex],4);

	pDC->SelectObject(pOldBrush);

	pDC->SelectObject(pOldPen);

	pDC->RestoreDC(nSave);
}

CPoint2D CFieldCell::intersect(CPoint2D p1[2], CPoint2D p2[2])
{
	double A1, B1, C1, A2, B2, C2, D;

	A1 = p1[0].y - p1[1].y;
	B1 = p1[1].x - p1[0].x;
	C1 = p1[1].y*p1[0].x - p1[1].x*p1[0].y;

	A2 = p2[0].y - p2[1].y;
	B2 = p2[1].x - p2[0].x;
	C2 = p2[1].y*p2[0].x - p2[1].x*p2[0].y;

	D = B2*A1 - B1*A2;

	CPoint2D pt;
	pt.x = (B1*C2 - B2*C1) / D;
	pt.y = (A2*C1 - A1*C2) / D;

	return pt;	
}

void CFieldCell::GradientFill(CDC *pDC, C2DModelView *pView)
{	
	int iIndex = 0;
	if( m_bDispTop )
		iIndex = 0;
	else
		iIndex = 4;

	int nSave = pDC->SaveDC();	

	TRIVERTEX tri[5];
	GRADIENT_TRIANGLE grd[4];

	CPoint ptCenter; // 四边形重心坐标
	// 013 - 123
	CPoint2D pt1[2],pt2[2];

	pt1[0].x = (m_point[iIndex+0].x+m_point[iIndex+1].x+m_point[iIndex+3].x)/3;
	pt1[0].y = (m_point[iIndex+0].y+m_point[iIndex+1].y+m_point[iIndex+3].y)/3;
	pt1[1].x = (m_point[iIndex+1].x+m_point[iIndex+2].x+m_point[iIndex+3].x)/3;
	pt1[1].y = (m_point[iIndex+1].y+m_point[iIndex+2].y+m_point[iIndex+3].y)/3;


	pt2[0].x = (m_point[iIndex+0].x+m_point[iIndex+1].x+m_point[iIndex+2].x)/3;
	pt2[0].y = (m_point[iIndex+0].y+m_point[iIndex+1].y+m_point[iIndex+2].y)/3;
	pt2[1].x = (m_point[iIndex+0].x+m_point[iIndex+2].x+m_point[iIndex+3].x)/3;
	pt2[1].y = (m_point[iIndex+0].y+m_point[iIndex+2].y+m_point[iIndex+3].y)/3;

	CPoint2D ptC = intersect(pt1, pt2);

	ptCenter.x = ptC.x;
	ptCenter.y = ptC.y;

	COLORREF clrCenter;
	int R, G, B;
	R = (GetRValue(m_clrCorner[iIndex+0]) + GetRValue(m_clrCorner[iIndex+1]) + GetRValue(m_clrCorner[iIndex+2]) + GetRValue(m_clrCorner[iIndex+3]))/4;
	G = (GetGValue(m_clrCorner[iIndex+0]) + GetGValue(m_clrCorner[iIndex+1]) + GetGValue(m_clrCorner[iIndex+2]) + GetGValue(m_clrCorner[iIndex+3]))/4;
	B = (GetBValue(m_clrCorner[iIndex+0]) + GetBValue(m_clrCorner[iIndex+1]) + GetBValue(m_clrCorner[iIndex+2]) + GetBValue(m_clrCorner[iIndex+3]))/4;
	clrCenter = RGB(R,G,B);

	tri [0] .x       =  m_point[iIndex+0].x;
	tri [0] .y       =  m_point[iIndex+0].y;
	tri [0] .Red     =  GetRValue(m_clrCorner[iIndex+0])<<8;
	tri [0] .Green   =  GetGValue(m_clrCorner[iIndex+0])<<8;
	tri [0] .Blue    =  GetBValue(m_clrCorner[iIndex+0])<<8;
	tri [0] .Alpha   =  0x0000;

	tri [1] .x       =  m_point[iIndex+1].x;
	tri [1] .y       =  m_point[iIndex+1].y;
	tri [1] .Red     =  GetRValue(m_clrCorner[iIndex+1])<<8;
	tri [1] .Green   =  GetGValue(m_clrCorner[iIndex+1])<<8;
	tri [1] .Blue    =  GetBValue(m_clrCorner[iIndex+1])<<8;
	tri [1] .Alpha   =  0x0000;

	tri [2] .x       =  m_point[iIndex+2].x;
	tri [2] .y       =  m_point[iIndex+2].y; 
	tri [2] .Red     =  GetRValue(m_clrCorner[iIndex+2])<<8;
	tri [2] .Green   =  GetGValue(m_clrCorner[iIndex+2])<<8;
	tri [2] .Blue    =  GetBValue(m_clrCorner[iIndex+2])<<8;
	tri [2] .Alpha   =  0x0000;

	tri [3] .x       =  m_point[iIndex+3].x;
	tri [3] .y       =  m_point[iIndex+3].y; 
	tri [3] .Red     =  GetRValue(m_clrCorner[iIndex+3])<<8;
	tri [3] .Green   =  GetGValue(m_clrCorner[iIndex+3])<<8;
	tri [3] .Blue    =  GetBValue(m_clrCorner[iIndex+3])<<8;
	tri [3] .Alpha   =  0x0000;

	tri [4] .x       =  ptCenter.x;
	tri [4] .y       =  ptCenter.y; 
	tri [4] .Red     =  GetRValue(clrCenter)<<8;
	tri [4] .Green   =  GetGValue(clrCenter)<<8;
	tri [4] .Blue    =  GetBValue(clrCenter)<<8;
	tri [4] .Alpha   =  0x0000;


	grd [0].Vertex1  = 0;
	grd [0].Vertex2  = 1;
	grd [0].Vertex3  = 4;

	grd [1].Vertex1  = 1;
	grd [1].Vertex2  = 2;
	grd [1].Vertex3  = 4;

	grd [2].Vertex1  = 2;
	grd [2].Vertex2  = 3;
	grd [2].Vertex3  = 4;

	grd [3].Vertex1  = 0;
	grd [3].Vertex2  = 3;
	grd [3].Vertex3  = 4;

	::GradientFill(pDC->m_hDC, tri, 5, &grd, 4, GRADIENT_FILL_TRIANGLE);

	pDC->RestoreDC(nSave);
}

BOOL CFieldCell::Intersects(CRect &rect)
{
	CRect rt = rect;
	rt.NormalizeRect();

	int iIndex = 0;
	if( m_bDispTop )
		iIndex = 0;
	else
		iIndex = 4;

	if( rt.PtInRect(m_point[iIndex+0]) )
		return TRUE;
	if( rt.PtInRect(m_point[iIndex+1]) )
		return TRUE;
	if( rt.PtInRect(m_point[iIndex+2]) )
		return TRUE;
	if( rt.PtInRect(m_point[iIndex+3]) )
		return TRUE;

	return FALSE;
}
