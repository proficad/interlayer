// RectEx.h: interface for the CRectEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECTEX_H__7E6A0DFE_46B2_4B54_9432_9972E584CF44__INCLUDED_)
#define AFX_RECTEX_H__7E6A0DFE_46B2_4B54_9432_9972E584CF44__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Point2D.h"
#include <math.h>
#include <float.h>

class AFX_EXT_CLASS CRectEx  
{
public:
	CRectEx(){};
	CRectEx(double l, double t, double r, double b);
	CRectEx(const CRectEx& srcRect);
	CRectEx(CPoint2D topLeft, CPoint2D bottomRight);
	CRectEx(CPoint2D topLeft, SIZE size);
	
	void SetRect(double l, double t, double r, double b);
	void SetRect(CPoint2D topLeft, CPoint2D bottomRight);

	const CRectEx& operator=(const CRectEx& rectSrc);
	const CRectEx& operator=(const CRect& rectSrc);
	CRectEx& operator+(CPoint2D point);
	CRectEx& operator-(CPoint2D point);
	void operator+=(CPoint2D point);
	void operator-=(CPoint2D point);

	BOOL IsRectEmpty();
	double Width();
	double Height();	
	void NormalizeRect();
	CPoint2D CenterPoint();
	CPoint2D TopLeft();
	CPoint2D BottomRight();
	BOOL PtInRect(CPoint2D point);
public:
	double left;
	double right;
	double top;
	double bottom;
private:
	friend CArchive& operator<<(CArchive& ar, const CRectEx& rect)
	{
		ar << rect.left;
		ar << rect.top;
		ar << rect.right;
		ar << rect.bottom;	
		return ar;
	}

	friend CArchive& operator>>(CArchive& ar, CRectEx& rect)
	{
		ar >> rect.left;
		ar >> rect.top;
		ar >> rect.right;
		ar >> rect.bottom;	
		return ar;
	}

	friend BOOL operator==(const CRectEx& s1, const CRectEx& s2)
	{
		return (fabs(s1.left - s2.left)<=DBL_EPSILON 
			&& fabs(s1.top - s2.top)<=DBL_EPSILON 
			&& fabs(s1.right - s2.right)<=DBL_EPSILON 
			&& fabs(s1.bottom - s2.bottom)<=DBL_EPSILON);
	}

	friend BOOL operator!=(const CRectEx& s1, const CRectEx& s2)
	{
		return (fabs(s1.left - s2.left)>DBL_EPSILON 
			|| fabs(s1.top - s2.top)>DBL_EPSILON 
			|| fabs(s1.right - s2.right)>DBL_EPSILON 
			|| fabs(s1.bottom - s2.bottom)>DBL_EPSILON);
	}
};

#endif // !defined(AFX_RECTEX_H__7E6A0DFE_46B2_4B54_9432_9972E584CF44__INCLUDED_)
