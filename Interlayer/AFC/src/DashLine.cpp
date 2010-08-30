// DashLine.cpp : Defines the class behaviors for CDashLine

#include "stdafx.h"
#include "DashLine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// define _TIMING to null out the GDI calls.
//	i.e. you can find out what is the cost of the calculations
//#define _TIMING 1

/////////////////////////////////////////////////////////////////////////////
// helper

/////////////////////////////////////////////////////////////////////////////
// CDashLine

CDashLine::CDashLine()	
:m_Pattern(0)
{
	m_Count = 0;
}

CDashLine::CDashLine(CDC* dc, unsigned* pattern, unsigned count)
	:m_pDC(dc), m_Pattern(0)
{
	SetPattern(pattern, count);
}

CDashLine::~CDashLine()
{
	// destroy GDI objects
	delete[] m_Pattern;
}

void CDashLine::SetPattern(unsigned* pattern ,unsigned count)
{
	// Must be an even number of dash and gaps
	ASSERT(m_Count >=0);
	ASSERT(!(m_Count % 2));

	m_Count = count;
	delete[] m_Pattern;
	if (m_Count)
	{
		m_Pattern = new UINT[count];
		memcpy(m_Pattern, pattern, count * sizeof(UINT));
	}	
	else
		m_Pattern = 0;
	Reset();
}

void CDashLine::Reset()
{
	m_CurPat = 0;
	if (m_Count)
		m_CurStretch = m_Pattern[0];
}

// use linear interpolation to compute next position
void CDashLine::Bresenham(LONG x, LONG y)
{

	// Setup Bresenham
	LONG dx = x - m_CurPos.x;
	LONG dy = y - m_CurPos.y;

	LONG *p1, *p2, *pd1, *pd2;
	if (abs(dx) >= abs(dy))
	{
		p1 = &m_CurPos.x; 
		p2 = &m_CurPos.y; 
		pd1 = &dx; 
		pd2 = &dy; 
	}
	else
	{
		p1 = &m_CurPos.y; 
		p2 = &m_CurPos.x; 
		pd1 = &dy; 
		pd2 = &dx;
	}

	int max = abs(*pd1);
	int dec = abs(*pd2);
	int s1 = (*pd1 >= 0) ? 1: -1;
	int s2 = (*pd2 >= 0) ? 1: -1;
	int val = max;

	// count past correct number of pixels in current segment
	// 		or until end of this line
	for (int i = 0; i < max; i++)
	{
		val -= dec;
		if (val <= 0) 
		{	
			*p2 += s2; 
			val += max; 
		}
		*p1 += s1;

		--m_CurStretch;
		if (!m_CurStretch)
		{
			// use next pattern for next segment
			m_CurPat = (m_CurPat + 1) % m_Count;
			m_CurStretch = m_Pattern[m_CurPat];

			if (m_CurPat % 2)
				m_pDC->LineTo(m_CurPos);
			else
				m_pDC->MoveTo(m_CurPos);

		
			// if last point, return
			if (i == max - 1) 
				return;
		}
	}

	// draw to last point if necessary
	if (!(m_CurPat % 2))
		m_pDC->LineTo(m_CurPos);
}

void CDashLine::MoveTo(int x, int y)
{
	// with MoveTo, reset parameters
	Reset();

	// save current position
	m_CurPos.x = x; 
	m_CurPos.y = y;

	// move to position
	m_pDC->MoveTo(m_CurPos);
}


void CDashLine::LineTo(int x, int y)
{
	// line type and color
	if (!m_Count)
	{
		m_pDC->LineTo(x, y);
		m_CurPos.x = x; m_CurPos.y = y;
		return;
	}

	// calculate and draw next points
	Bresenham(x, y);
}



// Returns count of elements (dash/dot and gaps)
// You must be careful to pass in enough memory for pattern
// It is probably safest to always have an array of [8]
unsigned CDashLine::GetPattern(unsigned* pattern, BOOL round, unsigned penSize, unsigned style)
{
	ASSERT(style <= DL_DASHDOTDOTDOT_GAP);
	int gapLen	= round	?	penSize * 2 : penSize;
	int dotLen	= round	?	1			: penSize;
	int dashLen	= round	?	penSize * 2 : penSize * 3;
	if (style >= DL_DASH_GAP)
		gapLen *= 2;
	if (style == DL_SOLID)
		return(0);
	
	switch (style)
	{
		case DL_DASH:
		case DL_DASH_GAP:
				pattern[0] = dashLen;
				pattern[1] = gapLen;
				return 2;
		case DL_DOT:
		case DL_DOT_GAP:
				pattern[0] = dotLen;
				pattern[1] = gapLen;
				return 2;
		case DL_DASHDOT:
		case DL_DASHDOT_GAP:
				pattern[0] = dashLen;
				pattern[2] = dotLen;
				pattern[1] = 
				pattern[3] = gapLen;
				return 4;
		case DL_DASHDOTDOT:
		case DL_DASHDOTDOT_GAP:
				pattern[0] = dashLen;
				pattern[2] =
				pattern[4] = dotLen;
				pattern[1] =
				pattern[3] =
				pattern[5] = gapLen;
				return 6;
		case DL_DASHDOTDOTDOT:
		case DL_DASHDOTDOTDOT_GAP:
		default:
				pattern[0] = dashLen;
				pattern[2] =
				pattern[4] = 
				pattern[6] = dotLen;
				pattern[1] =
				pattern[3] =
				pattern[5] = 
				pattern[7] = gapLen;
				return 8;
		
	}	
		
}	


void CDashLine::Polyline(LPPOINT lpPoints, int nCount)
{
	MoveTo(lpPoints[0]);

	for(int i = 1; i < nCount; i++)
		LineTo(lpPoints[i]);
}

void CDashLine::Polygon(LPPOINT lpPoints, int nCount)
{
	CRgn rgn;
	rgn.CreatePolygonRgn(lpPoints, nCount, m_pDC->GetPolyFillMode());

	m_pDC->FillRgn(&rgn, m_pDC->GetCurrentBrush());

	rgn.DeleteObject();

	MoveTo(lpPoints[0]);

	for(int i = 1; i < nCount; i++)
		LineTo(lpPoints[i]);

	LineTo(lpPoints[0]);
}

void CDashLine::Rectangle(int x1, int y1, int x2, int y2)
{
	m_pDC->FillRect(CRect(x1, y1, x2, y2), m_pDC->GetCurrentBrush());

	MoveTo( x1, y1 );
	LineTo( x2, y1 );
	LineTo( x2, y2 );
	LineTo( x1, y2);
	LineTo( x1, y1);
}

void CDashLine::Rectangle(LPCRECT lpRect)
{
	m_pDC->FillRect(lpRect, m_pDC->GetCurrentBrush());

	MoveTo( lpRect->left, lpRect->top );
	LineTo( lpRect->right, lpRect->top );
	LineTo( lpRect->right , lpRect->bottom );
	LineTo( lpRect->left, lpRect->bottom);
	LineTo( lpRect->left, lpRect->top);
}

void CDashLine::SetDC(CDC *dc)
{
	m_pDC = dc;
}
