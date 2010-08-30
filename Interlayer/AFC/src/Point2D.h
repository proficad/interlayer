#pragma once
#include <float.h>
#include <math.h>

class AFX_EXT_CLASS CPoint2D  
{
public:
	double x;
	double y;
	double m_dValue;
	TCHAR  szInfo[20];
public:
	CPoint2D();
	CPoint2D(double initX, double initY, double dValue = 0.0);
	const CPoint2D& operator=(const CPoint2D& ptSrc);
	const CPoint2D& operator=(const CPoint& ptSrc);

	friend CArchive& AFXAPI operator << (CArchive& ar, const CPoint2D& pt)
	{
		ar << pt.x;
		ar << pt.y;	
		ar << pt.m_dValue;
		ar.Write(pt.szInfo, 20);
		return ar;
	}
	friend CArchive& AFXAPI operator >> (CArchive& ar, CPoint2D& pt)
	{
		ar >> pt.x;
		ar >> pt.y;	
		ar >> pt.m_dValue;
		ar.Read(pt.szInfo, 20);
		return ar;
	}

	friend BOOL AFXAPI operator==(const CPoint2D& s1, const CPoint2D& s2)
	{
		return (fabs(s1.x - s2.x)<=DBL_EPSILON
			&& fabs(s1.y - s2.y)<=DBL_EPSILON); 
	}
	friend BOOL AFXAPI operator!=(const CPoint2D& s1, const CPoint2D& s2)
	{
		return (fabs(s1.x - s2.x)>DBL_EPSILON
			|| fabs(s1.y - s2.y)>DBL_EPSILON); 
	}
};

CArchive& AFXAPI operator << (CArchive& ar, const CPoint2D& pt);
CArchive& AFXAPI operator >> (CArchive& ar, CPoint2D& pt);

BOOL AFXAPI operator==(const CPoint2D& s1, const CPoint2D& s2);
BOOL AFXAPI operator!=(const CPoint2D& s1, const CPoint2D& s2);

typedef class CPoint2D* LPPOINT2D;

// 判断是否在线段上
extern "C" bool AFX_EXT_API isPointOnSegment(const CPoint2D& p1, const CPoint2D& p2, const CPoint2D &p0);
//点到线段距离
extern "C" double AFX_EXT_API GetNearestDistance(const CPoint2D &PA, const CPoint2D &PB, const CPoint2D &P3);
// 两点距离
extern "C" double AFX_EXT_API GetPointDistance(const CPoint2D &p1, const CPoint2D &p2);
