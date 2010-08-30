// GeoScaleObj.cpp: implementation of the CGeoScaleObj class.
//
//////////////////////////////////////////////////////////////////////

#pragma warning (disable:4786)

#include "stdafx.h"
#include "GeoScaleObj.h"
#include "DrawLayer.h"
#include "GeoMapDoc.h"
#include "GeoMapView.h"
#include "../Resource.h"
#include <float.h>
#include "CmdGeoDrawObj.h"
#include "DlgScaleRulerSet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(CGeoScaleObj, CGeoDrawObj, VERSIONABLE_SCHEMA|0)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeoScaleObj::CGeoScaleObj()
{
	
}

CGeoScaleObj::CGeoScaleObj(CDrawLayer *pLayer)
:CGeoDrawObj(pLayer)
{
	m_rtPosition = CRect(0,0, 500,50);
	// 比例字体
	memset(&m_lfFont, 0, sizeof(LOGFONT) );
	m_lfFont.lfHeight = -25;
	m_lfFont.lfCharSet = GB2312_CHARSET;
	strcpy_s(m_lfFont.lfFaceName, _T("宋体"));
	
	m_clrFont = RGB(0,0,0);				// 字体颜色
	m_clrPen = RGB(0,0,0);				// 字体颜色

	m_ScaleType = ST_FILLRECT2;

	m_nWidth	= 400;	// 4cm
	m_nHeight	= 25;	// 0.2cm	
}

CGeoScaleObj::CGeoScaleObj(const CPoint &pt, CDrawLayer *pLayer)
:CGeoDrawObj(pLayer)
{
	m_rtPosition = CRect(pt.x-250,pt.y-30, pt.x+250,pt.y+30);
	// 比例字体
	memset(&m_lfFont, 0, sizeof(LOGFONT) );
	m_lfFont.lfHeight = -25;
	m_lfFont.lfCharSet = ANSI_CHARSET;
	strcpy_s(m_lfFont.lfFaceName, _T("Arial"));

	m_clrFont = RGB(0,0,0);				// 字体颜色
	
	m_clrPen = RGB(0,0,0);				// 字体颜色

	m_ScaleType = ST_FILLRECT2;

	m_nWidth	= 400;	// 4cm
	m_nHeight	= 25;	// 0.2cm
}

CGeoScaleObj::~CGeoScaleObj()
{
}

void CGeoScaleObj::Draw(CDC *pDC, CGeoMapView *pView)
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

	int nSaved = pDC->SaveDC();
	pDC->SetBkMode(TRANSPARENT);          //设定显示方式
	
	CRect rect = m_rtPosition;
	rect.NormalizeRect();
	
	CPoint pt = rect.CenterPoint();
	CPoint p[2];
	p[0].x = rect.left+20;
	p[0].y = pt.y;
	p[1].x = rect.right - 80;
	p[1].y = rect.top+5;

	pDC->SetTextAlign(TA_BOTTOM | TA_LEFT );
	pDC->SetTextColor(m_clrFont);
	CFont font;
	font.CreateFontIndirect(&m_lfFont);

	CFont *pOldFont = pDC->SelectObject(&font);

	double dScale = m_pParentLayer->GetParentPage()->m_lScale;

	LOGBRUSH logbrush;
	logbrush.lbColor = m_clrPen;
	logbrush.lbStyle = BS_SOLID;
	logbrush.lbHatch = NULL;
	
	CPen thinPen1; // 细pen
	thinPen1.CreatePen(PS_SOLID | PS_GEOMETRIC | PS_JOIN_MITER | PS_ENDCAP_SQUARE, 
		1, 
		&logbrush);

	CPen thinPen; // 细pen
	thinPen.CreatePen(PS_SOLID | PS_GEOMETRIC | PS_JOIN_MITER | PS_ENDCAP_SQUARE, 
		3, 
		&logbrush);

	CPen thickPen; // 粗pen
	thickPen.CreatePen(PS_SOLID | PS_GEOMETRIC | PS_JOIN_MITER | PS_ENDCAP_SQUARE, 
		5, 
		&logbrush);

	CPen *pOldPen;

	switch(m_ScaleType)
	{
	case ST_SINGLELINE:
		{
			int midy = (int)((p[0].y + p[1].y) * 0.5f);
			int midx[7];
			int dx = p[1].x - p[0].x;
			int dy = p[0].y - p[1].y;
			int i;
			for( i = 1; i < 8; i++)
			{
				midx[i - 1] = p[0].x + i * (int)(dx / 8.0f);
			}
	
			pOldPen = pDC->SelectObject(&thickPen);
			pDC->MoveTo(p[0].x, midy);
			pDC->LineTo(p[0].x, p[0].y);
			pDC->TextOut(p[0].x, p[0].y, "0");
			
			pDC->MoveTo(p[0].x, midy);
			pDC->LineTo(p[1].x, midy);
			pDC->LineTo(p[1].x, p[0].y);
			CString mes;
			if( (dScale * dx * 0.0001f) > 3999.9)
				mes.Format("%.0fkm", dScale * dx * 0.0000001f);
			else
				mes.Format("%.0fm", dScale * dx * 0.0001f);

			pDC->TextOut(p[1].x, p[0].y, mes);			
			
			pDC->SelectObject(&thinPen);
			for(i = 0; i < 7; i++)
			{
				if(i != 3 && i != 1)
				{
					pDC->MoveTo(midx[i], midy);
					pDC->LineTo(midx[i], midy + (int)(dy * 0.25f));
				}
				else if(i == 1)
				{
					pDC->MoveTo(midx[i], midy);
					pDC->LineTo(midx[i], midy + (int)(dy * 0.25f));
					if( (dScale * dx * 0.0001f) > 3999.9)
						mes.Format("%.0f", dScale * dx * 0.25f * 0.0000001f);
					else
						mes.Format("%.0f", dScale * dx * 0.25f * 0.0001f);
					pDC->TextOut(midx[i], p[0].y, mes);
				}
				else if(i == 3)
				{
					pDC->MoveTo(midx[i], midy);
					pDC->LineTo(midx[i], p[0].y);
					if( (dScale * dx * 0.0001f) > 3999.9)
						mes.Format("%.0f", dScale * dx * 0.5f * 0.0000001f);
					else
						mes.Format("%.0f", dScale * dx * 0.5f * 0.0001f);
					pDC->TextOut(midx[i], p[0].y, mes);
				}
			}
			pDC->SelectObject(pOldPen);
		}
		break;
	case ST_SINGLELINE2:
		{
			int midy = (int)((p[0].y + p[1].y) * 0.5f);
			int midx[7];
			int dx = p[1].x - p[0].x;
			int dy = p[0].y - p[1].y;
			int i;
			for( i = 1; i < 8; i++)
			{
				midx[i - 1] = p[0].x + i * (int)(dx / 8.0f);
			}
			
			pOldPen = pDC->SelectObject(&thickPen);
			pDC->MoveTo(p[0].x, p[1].y);
			pDC->LineTo(p[0].x, p[0].y);
			pDC->TextOut(p[0].x, p[0].y, "0");
			
			pDC->MoveTo(p[0].x, midy);
			pDC->LineTo(p[1].x, midy);
			pDC->MoveTo(p[1].x, p[1].y);
			pDC->LineTo(p[1].x, p[0].y);
			CString mes;
			if( (dScale * dx * 0.0001f) > 3999.9)
				mes.Format("%.0fkm", dScale * dx * 0.0000001f);
			else
				mes.Format("%.0fm", dScale * dx * 0.0001f);
			pDC->TextOut(p[1].x, p[0].y , mes);
			
			pDC->SelectObject(&thinPen);
			for(i = 0; i < 7; i++)
			{
				if(i != 3 && i != 1)
				{
					pDC->MoveTo(midx[i], midy - (int)(dy * 0.25f));
					pDC->LineTo(midx[i], midy + (int)(dy * 0.25f));
				}
				else if(i == 1)
				{
					pDC->MoveTo(midx[i], midy - (int)(dy * 0.25f));
					pDC->LineTo(midx[i], midy + (int)(dy * 0.25f));
					if( (dScale * dx * 0.0001f) > 3999.9)
						mes.Format("%.0f", dScale * dx * 0.25f * 0.0000001f);
					else
						mes.Format("%.0f", dScale * dx * 0.25f * 0.0001f);
					pDC->TextOut(midx[i], p[0].y, mes);
				}
				else if(i == 3)
				{
					pDC->MoveTo(midx[i], midy - (int)(dy * 0.25f));
					pDC->LineTo(midx[i], p[0].y);
					if( (dScale * dx * 0.0001f) > 3999.9)
						mes.Format("%.0f", dScale * dx * 0.5f * 0.0000001f);
					else
						mes.Format("%.0f", dScale * dx * 0.5f * 0.0001f);
					pDC->TextOut(midx[i], p[0].y, mes);
				}
			}
			pDC->SelectObject(pOldPen);
		}
		break;
	case ST_SINGLELINE3:
		{
			int midy = (int)((p[0].y + p[1].y) * 0.5f);
			int midx[7];
			int dx = p[1].x - p[0].x;
			int dy = p[0].y - p[1].y;
			int i;
			for( i = 1; i < 8; i++)
			{
				midx[i - 1] = p[0].x + i * (int)(dx / 8.0f);
			}
			
			pOldPen = pDC->SelectObject(&thickPen);
			pDC->MoveTo(p[0].x, midy);
			pDC->LineTo(p[0].x, p[1].y);
			pDC->TextOut(p[0].x, p[0].y, "0");
			
			pDC->MoveTo(p[0].x, midy);
			pDC->LineTo(p[1].x, midy);
			pDC->LineTo(p[1].x, p[1].y);
			CString mes;
			if( (dScale * dx * 0.0001f) > 3999.9)
				mes.Format("%.0fkm", dScale * dx * 0.0000001f);
			else
				mes.Format("%.0fm", dScale * dx * 0.0001f);
			pDC->TextOut(p[1].x, p[0].y, mes);
			
			pDC->SelectObject(&thinPen);
			for(i = 0; i < 7; i++)
			{
				if(i != 3 && i != 1)
				{
					pDC->MoveTo(midx[i], midy);
					pDC->LineTo(midx[i], midy - (int)(dy * 0.25f));
				}
				else if(i == 1)
				{
					pDC->MoveTo(midx[i], midy);
					pDC->LineTo(midx[i], midy - (int)(dy * 0.25f));
					if( (dScale * dx * 0.0001f) > 3999.9)
						mes.Format("%.0f", dScale * dx * 0.25f * 0.0000001f);
					else
						mes.Format("%.0f", dScale * dx * 0.25f * 0.0001f);

					pDC->TextOut(midx[i], p[0].y, mes);
				}
				else if(i == 3)
				{
					pDC->MoveTo(midx[i], midy);
					pDC->LineTo(midx[i], p[1].y);

					if( (dScale * dx * 0.0001f) > 3999.9)
						mes.Format("%.0f", dScale * dx * 0.5f * 0.0000001f);
					else
						mes.Format("%.0f", dScale * dx * 0.5f * 0.0001f);
					pDC->TextOut(midx[i], p[0].y, mes);
				}
			}
			pDC->SelectObject(pOldPen);
		}
	    break;
	case ST_FILLRECT:
		{
			int midy = (int)((p[0].y + p[1].y) * 0.5f);
			int midx[15];
			int dx = p[1].x - p[0].x;
			int dy = p[0].y - p[1].y;
			for(int i = 1; i < 16; i++)
			{
				midx[i - 1] = p[0].x + i * (int)(dx / 16.0f);
			}
			
			pOldPen = pDC->SelectObject(&thinPen1);
			pDC->MoveTo(p[0].x, midy + (int)(dy * 0.25));
			pDC->LineTo(p[1].x, midy + (int)(dy * 0.25));
			pDC->LineTo(p[1].x, midy - (int)(dy * 0.25));
			pDC->LineTo(p[0].x, midy - (int)(dy * 0.25));
			pDC->LineTo(p[0].x, midy + (int)(dy * 0.25));
			pDC->SelectObject(pOldPen);
			pDC->TextOut(p[0].x, p[0].y, "0");
			CString mes;
			if( (dScale * dx * 0.0001f) > 3999.9)
			{
				mes.Format("%.0fkm", dScale * dx * 0.0000001f);
				pDC->TextOut(p[1].x, p[0].y, mes);
				mes.Format("%.0f", dScale * dx * 0.25f * 0.0000001f);
				pDC->TextOut(midx[3], p[0].y, mes);
				mes.Format("%.0f", dScale * dx * 0.5f * 0.0000001f);
				pDC->TextOut(midx[7], p[0].y, mes);
			}
			else
			{
				mes.Format("%.0fm", dScale * dx * 0.0001f);
				pDC->TextOut(p[1].x, p[0].y, mes);
				mes.Format("%.0f", dScale * dx * 0.25f * 0.0001f);
				pDC->TextOut(midx[3], p[0].y, mes);
				mes.Format("%.0f", dScale * dx * 0.5f * 0.0001f);
				pDC->TextOut(midx[7], p[0].y, mes);
			}
			
			CBrush tmpbrush(m_clrPen);
			CRect tmprect;
			tmprect.left = p[0].x;
			tmprect.right = midx[0];
			tmprect.bottom = midy - (int)(dy * 0.25);
			tmprect.top = midy + (int)(dy * 0.25);
			pDC->FillRect(&tmprect, &tmpbrush);
			tmprect.left = midx[1];
			tmprect.right = midx[2];
			pDC->FillRect(&tmprect, &tmpbrush);
			tmprect.left = midx[3];
			tmprect.right = midx[7];
			pDC->FillRect(&tmprect, &tmpbrush);
			tmprect.left = midx[11];
			tmprect.right = p[1].x;
			pDC->FillRect(&tmprect, &tmpbrush);

			pDC->SelectObject(pOldPen);
		}
	    break;
	case ST_FILLRECT2:
		{
			int midy = (int)((p[0].y + p[1].y) * 0.5f);
			int midx[15];
			int dx = p[1].x - p[0].x;
			int dy = p[0].y - p[1].y;
			for(int i = 1; i < 16; i++)
			{
				midx[i - 1] = p[0].x + i * (int)(dx / 16.0f);
			}

			pOldPen = pDC->SelectObject(&thinPen1);
			pDC->MoveTo(p[0].x, midy + (int)(dy * 0.25));
			pDC->LineTo(p[1].x, midy + (int)(dy * 0.25));
			pDC->LineTo(p[1].x, midy - (int)(dy * 0.25));
			pDC->LineTo(p[0].x, midy - (int)(dy * 0.25));
			pDC->LineTo(p[0].x, midy + (int)(dy * 0.25));
			pDC->SelectObject(pOldPen);
			pDC->TextOut(p[0].x, p[0].y, "0");
			CString mes;
			if( (dScale * dx * 0.0001f) > 3999.9)
			{
				mes.Format("%.0fkm", dScale * dx * 0.0000001f);
				pDC->TextOut(p[1].x, p[0].y, mes);
				
				mes.Format("%.0f", dScale * dx * 0.25f * 0.0000001f);
				pDC->TextOut(midx[3], p[0].y, mes);
				
				mes.Format("%.0f", dScale * dx * 0.5f * 0.0000001f);
				pDC->TextOut(midx[7], p[0].y, mes);
			}
			else				
			{
				mes.Format("%.0fm", dScale * dx * 0.0001f);
				pDC->TextOut(p[1].x, p[0].y, mes);
				
				mes.Format("%.0f", dScale * dx * 0.25f * 0.0001f);
				pDC->TextOut(midx[3], p[0].y, mes);
				
				mes.Format("%.0f", dScale * dx * 0.5f * 0.0001f);
				pDC->TextOut(midx[7], p[0].y, mes);
			}
			
			CBrush tmpbrush(m_clrPen);

			CRect tmprect;
			CRect tmprect2;
			tmprect.left = p[0].x;
			tmprect.right = midx[0];
			tmprect.bottom = midy;
			tmprect.top = midy + (int)(dy * 0.25);
			pDC->FillRect(&tmprect, &tmpbrush);
			tmprect2.left = midx[0];
			tmprect2.right = midx[1];
			tmprect2.bottom = midy - (int)(dy * 0.25);
			tmprect2.top = midy;
			pDC->FillRect(&tmprect2, &tmpbrush);
			tmprect.left = midx[1];
			tmprect.right = midx[2];
			pDC->FillRect(&tmprect, &tmpbrush);
			tmprect2.left = midx[2];
			tmprect2.right = midx[3];
			pDC->FillRect(&tmprect2, &tmpbrush);
			tmprect.left = midx[3];
			tmprect.right = midx[7];
			pDC->FillRect(&tmprect, &tmpbrush);
			tmprect2.left = midx[7];
			tmprect2.right = midx[11];
			pDC->FillRect(&tmprect2, &tmpbrush);
			tmprect.left = midx[11];
			tmprect.right = p[1].x;
			pDC->FillRect(&tmprect, &tmpbrush);

			pDC->SelectObject(pOldPen);
		}
		break;
	case ST_STEPPEDLINE:
		{
			int midy = (int)((p[0].y + p[1].y) * 0.5f);
			int midx[15];
			int dx = p[1].x - p[0].x;
			int dy = p[0].y - p[1].y;
			for(int i = 1; i < 16; i++)
			{
				midx[i - 1] = p[0].x + i * (int)(dx / 16.0f);
			}
			
			pDC->TextOut(p[0].x, p[0].y, "0");
			CString mes;
			if( (dScale * dx * 0.0001f) > 3999.9)
			{
				mes.Format("%.0fkm", dScale * dx * 0.0000001f);
				pDC->TextOut(p[1].x, p[0].y, mes);
				mes.Format("%.0f", dScale * dx * 0.25f * 0.0000001f);
				pDC->TextOut(midx[3], p[0].y, mes);
				mes.Format("%.0f", dScale * dx * 0.5f * 0.0000001f);
				pDC->TextOut(midx[7], p[0].y, mes);
			}
			else
				
			{
				mes.Format("%.0fm", dScale * dx * 0.0001f);
				
				pDC->TextOut(p[1].x, p[0].y, mes);
				mes.Format("%.0f", dScale * dx * 0.25f * 0.0001f);
				pDC->TextOut(midx[3], p[0].y, mes);
				mes.Format("%.0f", dScale * dx * 0.5f * 0.0001f);
				pDC->TextOut(midx[7], p[0].y, mes);
			}
			
			int y1, y2;
			y1 = midy - (int)(dy * 0.25);
			y2 = midy + (int)(dy * 0.25);

			pOldPen = pDC->SelectObject(&thinPen);
			pDC->MoveTo(p[0].x, y1);
			pDC->LineTo(p[0].x, y2);
			pDC->LineTo(midx[0], y2);
			pDC->LineTo(midx[0], y1);
			pDC->LineTo(midx[1], y1);
			pDC->LineTo(midx[1], y2);
			pDC->LineTo(midx[2], y2);
			pDC->LineTo(midx[2], y1);
			pDC->LineTo(midx[3], y1);
			pDC->LineTo(midx[3], y2);
			pDC->LineTo(midx[7], y2);
			pDC->LineTo(midx[7], y1);
			pDC->LineTo(midx[11], y1);
			pDC->LineTo(midx[11], y2);
			pDC->LineTo(p[1].x, y2);
			pDC->LineTo(p[1].x, y1);
			pDC->SelectObject(pOldPen);
		}
		break;
	case ST_LINERECT:
		{
			int midy = (int)((p[0].y + p[1].y) * 0.5f);
			int midx[15];
			int dx = p[1].x - p[0].x;
			int dy = p[0].y - p[1].y;
			for(int i = 1; i < 16; i++)
			{
				midx[i - 1] = p[0].x + i * (int)(dx / 16.0f);
			}
			
			pOldPen = pDC->SelectObject(&thinPen1);
			pDC->MoveTo(p[0].x, midy + (int)(dy * 0.25));
			pDC->LineTo(p[1].x, midy + (int)(dy * 0.25));
			pDC->LineTo(p[1].x, midy - (int)(dy * 0.25));
			pDC->LineTo(p[0].x, midy - (int)(dy * 0.25));
			pDC->LineTo(p[0].x, midy + (int)(dy * 0.25));

			pDC->TextOut(p[0].x, p[0].y, "0");
			CString mes;
			if( (dScale * dx * 0.0001f) > 3999.9)
			{
				mes.Format("%.0fkm", dScale * dx * 0.0000001f);
				pDC->TextOut(p[1].x, p[0].y, mes);
				mes.Format("%.0f", dScale * dx * 0.25f * 0.0000001f);
				pDC->TextOut(midx[3], p[0].y, mes);
				mes.Format("%.0f", dScale * dx * 0.5f * 0.0000001f);
				pDC->TextOut(midx[7], p[0].y, mes);
			}
			else				
			{
				mes.Format("%.0fm", dScale * dx * 0.0001f);
				pDC->TextOut(p[1].x, p[0].y, mes);
				mes.Format("%.0f", dScale * dx * 0.25f * 0.0001f);
				pDC->TextOut(midx[3], p[0].y, mes);
				mes.Format("%.0f", dScale * dx * 0.5f * 0.0001f);
				pDC->TextOut(midx[7], p[0].y, mes);
			}
			
			CRect tmprect;
			tmprect.left = p[0].x;
			tmprect.right = midx[0];
			tmprect.bottom = midy - (int)(dy * 0.25);
			tmprect.top = midy + (int)(dy * 0.25);
			pDC->MoveTo(tmprect.left, tmprect.bottom);
			pDC->LineTo(tmprect.left, tmprect.top);
			pDC->MoveTo(tmprect.right, tmprect.bottom);
			pDC->LineTo(tmprect.right, tmprect.top);
			pDC->MoveTo(tmprect.left, midy);
			pDC->LineTo(tmprect.right, midy);
			tmprect.left = midx[1];
			tmprect.right = midx[2];
			pDC->MoveTo(tmprect.left, tmprect.bottom);
			pDC->LineTo(tmprect.left, tmprect.top);
			pDC->MoveTo(tmprect.right, tmprect.bottom);
			pDC->LineTo(tmprect.right, tmprect.top);
			pDC->MoveTo(tmprect.left, midy);
			pDC->LineTo(tmprect.right, midy);
			tmprect.left = midx[3];
			tmprect.right = midx[7];
			pDC->MoveTo(tmprect.left, tmprect.bottom);
			pDC->LineTo(tmprect.left, tmprect.top);
			pDC->MoveTo(tmprect.right, tmprect.bottom);
			pDC->LineTo(tmprect.right, tmprect.top);
			pDC->MoveTo(tmprect.left, midy);
			pDC->LineTo(tmprect.right, midy);
			tmprect.left = midx[11];
			tmprect.right = p[1].x;
			pDC->MoveTo(tmprect.left, tmprect.bottom);
			pDC->LineTo(tmprect.left, tmprect.top);
			pDC->MoveTo(tmprect.right, tmprect.bottom);
			pDC->LineTo(tmprect.right, tmprect.top);
			pDC->MoveTo(tmprect.left, midy);
			pDC->LineTo(tmprect.right, midy);

			pDC->SelectObject(pOldPen);
		}
	    break;
	default:
	    break;
	}
	
	pDC->SelectObject(pOldFont);
	pDC->RestoreDC(nSaved);
}

void CGeoScaleObj::DrawEditMode(CGeoMapView* pView)
{
	CGeoDrawObj::DrawEditMode(pView);
}

void CGeoScaleObj::DrawMoveState(CGeoMapView* pView)
{
	CGeoDrawObj::DrawMoveState(pView);
}

void CGeoScaleObj::DrawSizeState(CGeoMapView* pView)
{
	CGeoDrawObj::DrawSizeState(pView);
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::OnEditProperties
// 返回  : void
// 参数  : CGeoMapView *pView
// 说明  : 编辑图素属性
//
BOOL CGeoScaleObj::OnEditProperties(CGeoMapView *pView)
{
	ASSERT_VALID(this);
	if( m_bLocked )
		return FALSE;
		
	CDlgScaleRulerSet dlg;
	dlg.m_clrFont = m_clrFont;
	dlg.m_clrPen = m_clrPen;
	dlg.m_uWidth = m_nWidth;
	dlg.m_uHeight = m_nHeight;

	dlg.m_ScaleType = m_ScaleType;

	memcpy(&dlg.m_lfFont, &m_lfFont, sizeof(LOGFONT) );	
	if( dlg.DoModal() == IDOK )
	{
		m_clrFont = dlg.m_clrFont;
		m_clrPen = dlg.m_clrPen;
		m_nWidth = dlg.m_uWidth;
		m_nHeight = dlg.m_uHeight;
		m_ScaleType = (ScaleType)dlg.m_ScaleType;

		memcpy(&m_lfFont, &dlg.m_lfFont, sizeof(LOGFONT) );	
		CPoint point = m_rtPosition.CenterPoint();
		int nWidth = m_nWidth + 100;
		int nHeight = labs( m_lfFont.lfHeight) + m_nHeight + 10;
		
		Invalidate(pView);
		CRect rect(point.x-nWidth/2,point.y-nHeight/2,point.x+nWidth/2,point.y+nHeight/2);
		m_rtPosition = rect;
		Invalidate(pView);
		
		m_pDocument->SetModifiedFlag();
	}
	
	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::Clone
// 返回  : CGeoDrawObj*
// 参数  : CGeoMapDoc *pDoc
// 说明  : 克隆图素并将图素加入到文档中
//
CGeoDrawObj* CGeoScaleObj::Clone(CDrawLayer *pLayer)
{
	ASSERT_VALID(this);
	
	CGeoScaleObj* pClone = new CGeoScaleObj(pLayer);
	pClone->m_rtPosition	=	m_rtPosition;
	pClone->m_bLocked		=	m_bLocked;			// 锁定图素
	pClone->m_bShow			=	m_bShow;			// 是否显示
	pClone->m_strID			=	newGUID();			// 新的ID
	pClone->m_ptAnchors.Copy(m_ptAnchors);			// 锚点

	pClone->m_pDocument		=	m_pDocument;		// 文档

	//////////////////////////////////////////////////////////////////////////
	
	// 字体
	memcpy(&pClone->m_lfFont, &m_lfFont, sizeof(LOGFONT) );	
	pClone->m_clrFont = m_clrFont;				// 字体颜色
	pClone->m_clrPen = m_clrPen;				// 字体颜色
	pClone->m_ScaleType = m_ScaleType;
	pClone->m_nWidth	= m_nWidth;
	pClone->m_nHeight	= m_nHeight;
	
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
void CGeoScaleObj::Serialize(CArchive &ar)
{
	CGeoDrawObj::Serialize(ar);
	
	ar.SerializeClass(RUNTIME_CLASS(CGeoScaleObj));

	if (ar.IsStoring())
	{
		// 比例字体
		ar.Write(&m_lfFont, sizeof(LOGFONT) );	
		ar << m_clrFont;			// 字体颜色	
		ar << m_clrPen;				// 字体颜色
		ar << (WORD)m_ScaleType;
		ar << m_nWidth;
		ar << m_nHeight;
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
				
				// 比例字体
				ar.Read(&m_lfFont, sizeof(LOGFONT) );	
				ar >> m_clrFont;				// 字体颜色
				ar >> m_clrPen;				// 字体颜色	
				WORD tmp;
				ar >> tmp;
				m_ScaleType = (ScaleType)tmp;

				ar >> m_nWidth;
				ar >> m_nHeight;
			}
			break;
		case 1:
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
int CGeoScaleObj::GetHandleCount()
{
	ASSERT_VALID(this);
	return 0;
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
int CGeoScaleObj::HitTestHandle(CPoint point, CGeoMapView *pView, BOOL bSelected)
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
			if (point.x >= rc.left && point.x < rc.right &&
				point.y >= rc.top && point.y < rc.bottom)
				return nHandle;
		}
	}
	else // 否则, 测试是否落在图素范围内了
	{
		CRect rt = m_rtPosition;
		
		rt.NormalizeRect();
		
		if (point.x >= rt.left && point.x <= rt.right &&
			point.y >= rt.top && point.y <= rt.bottom)
			return 1;
	}
	return 0;
}


// 当比例尺发生改变时
void CGeoScaleObj::OnScaleChanged()
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
}
// 当发生旋转时
void CGeoScaleObj::OnAngleChanged()
{

}
// 当发生坐标原点有改变时
void CGeoScaleObj::OnCoordOrigChanged()
{

}
