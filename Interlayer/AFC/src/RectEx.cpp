// RectEx.cpp: implementation of the CRectEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RectEx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRectEx::CRectEx(double l, double t, double r, double b)
{
	left = l;
	right = r;
	top = t;
	bottom = b;
}

CRectEx::CRectEx(const CRectEx& srcRect)
{
	left = srcRect.left;
	top = srcRect.top;
	right = srcRect.right;
	bottom = srcRect.bottom;
}

CRectEx::CRectEx(CPoint2D topLeft, CPoint2D bottomRight)
{
	left = topLeft.x;
	top = topLeft.y;
	right = bottomRight.x;
	bottom = bottomRight.y;
}

CRectEx::CRectEx(CPoint2D topLeft, SIZE size )
{
	left = topLeft.x;
	top = topLeft.y;
	right = left + size.cx;
	bottom = top + size.cy;
}

void CRectEx::SetRect(double l, double t, double r, double b)
{
	left = l;
	right = r;
	top = t;
	bottom = b;
}

void CRectEx::SetRect(CPoint2D topLeft, CPoint2D bottomRight)
{
	left = topLeft.x;
	top = topLeft.y;
	right = bottomRight.x;
	bottom = bottomRight.y;
}

const CRectEx& CRectEx::operator=(const CRectEx& rectSrc)
{
	left = rectSrc.left;
	top = rectSrc.top;
	right = rectSrc.right;
	bottom = rectSrc.bottom;
	return *this;
}

const CRectEx& CRectEx::operator=(const CRect& rectSrc)
{
	left = rectSrc.left;
	top = rectSrc.top;
	right = rectSrc.right;
	bottom = rectSrc.bottom;
	return *this;
}

CRectEx& CRectEx::operator+(CPoint2D point)
{
	left += point.x;
	right += point.x;
	top  += point.y;
	bottom += point.y;
	return *this;
}

CRectEx& CRectEx::operator-(CPoint2D point)
{
	left -= point.x;
	right -= point.x;
	top  -= point.y;
	bottom -= point.y;
	return *this;
}

void CRectEx::operator+=(CPoint2D point)
{
	left += point.x;
	right += point.x;
	top  += point.y;
	bottom += point.y;
}

void CRectEx::operator-=(CPoint2D point)
{
	left -= point.x;
	right -= point.x;
	top  -= point.y;
	bottom -= point.y;
}

BOOL CRectEx::IsRectEmpty()
{
	if( fabs(right-left) < DBL_EPSILON 
		&& fabs(bottom-top)<DBL_EPSILON)
		return TRUE;
	else
		return FALSE;
}

double CRectEx::Width()
{
	return fabs(right-left);
}

double CRectEx::Height()
{
	return fabs(bottom-top);
}

void CRectEx::NormalizeRect()
{
	if( bottom < top )
	{
		double tmp = bottom;
		bottom = top;
		top = tmp;
	}

	if( right < left )
	{
		double tmp = right;
		right = left;
		left = tmp;
	}
}

CPoint2D CRectEx::CenterPoint()
{
	CPoint2D point;
	point.x = (right+left)/2.0;
	point.y = (bottom+top)/2.0;
	return point;
}

CPoint2D CRectEx::TopLeft()
{
	CPoint2D point;
	point.x = left;
	point.y = top;
	return point;
}

CPoint2D CRectEx::BottomRight()
{
	CPoint2D point;
	point.x = right;
	point.y = bottom;
	return point;
}

BOOL CRectEx::PtInRect(CPoint2D point)
{
	CRectEx rt = *this;
	rt.NormalizeRect();
	if( point.x >=left && point.x <= right 
		&& point.y >= top && point.y<=bottom )
		return TRUE;
	else
		return FALSE;
}

