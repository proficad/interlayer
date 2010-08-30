#include "StdAfx.h"
#include "Point2D.h"
#include <math.h>
//#include <double.h>

CPoint2D::CPoint2D()
{
	x = 0.0;
	y = 0.0;
	m_dValue = 0.0;
	szInfo[0] = 0;
}

CPoint2D::CPoint2D(double initX, double initY, double dValue)
{
	x = initX;
	y = initY;
	m_dValue = dValue;
	szInfo[0] = 0;
}

const CPoint2D& CPoint2D::operator=(const CPoint2D& ptSrc)
{
	x = ptSrc.x;
	y = ptSrc.y;
	m_dValue = ptSrc.m_dValue;
	strcpy_s(szInfo, ptSrc.szInfo);
	return *this;
}

const CPoint2D& CPoint2D::operator=(const CPoint& ptSrc)
{
	x = ptSrc.x;
	y = ptSrc.y;
	m_dValue = 0;
	szInfo[0]=0;
	return *this;
}

extern "C" 
bool isPointOnSegment(const CPoint2D& p1, const CPoint2D& p2, const CPoint2D &p0)
{
	//叉积是否为0，判断是否在同一直线上
	if(fabs((p1.x-p0.x)*(p2.y-p0.y)-(p2.x-p0.x)*(p1.y-p0.y)) > 0.01)
		return false;
	//判断是否在线段上
// 	if((p0.x > p1.x && p0.x > p2.x) || (p0.x < p1.x && p0.x < p2.x))
// 		return false;
// 	if((p0.y > p1.y && p0.y > p2.y) || (p0.y < p1.y && p0.y < p2.y))
// 		return false;

	if(p0.x - min(p1.x,p2.x)<0.00001 || (max(p1.x,p2.x) - p0.x) < 0.00001)
		return false;
	if(p0.y - min(p1.y,p2.y)<0.00001 || (max(p1.y,p2.y) - p0.y) < 0.00001)
		return false;
	return true;
}

extern "C" 
double GetPointDistance(const CPoint2D &p1, const CPoint2D &p2) 
{
	return sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
}

extern "C" 
double GetNearestDistance(const CPoint2D &PA, const CPoint2D &PB, const CPoint2D &P3)
{
	double a,b,c;
	a=GetPointDistance(PB,P3);
	if(a<=0.0000001)
		return 0.0f;
	b=GetPointDistance(PA,P3);
	if(b<=0.0000001)
		return 0.0f;
	c=GetPointDistance(PA,PB);
	if(c<=0.0000001)
		return a;//如果PA和PB坐标相同，则退出函数，并返回距离
	
	if(a*a>=b*b+c*c)
		return b;
	if(b*b>=a*a+c*c)
		return a;

	double l=(a+b+c)/2;     //周长的一半
	double s=sqrt(l*(l-a)*(l-b)*(l-c));  //海伦公式求面积
	return 2*s/c;
}

/*

//判断两条线段是否相交(有交点)

bool IsLineSegmentCross(const CPoint2D pFirst1, const CPoint2D &pFirst2, const CPoint2D &pSecond1, const CPoint2D &pSecond2)
{
	//每个线段的两点都在另一个线段的左右不同侧，则能断定线段相交
	//公式对于向量(x1,y1)->(x2,y2),判断点(x3,y3)在向量的左边,右边,还是线上.
	//p=x1(y3-y2)+x2(y1-y3)+x3(y2-y1).p<0 左侧,	p=0 线上, p>0 右侧
	long Linep1,Linep2;
	//判断pSecond1和pSecond2是否在pFirst1->pFirst2两侧
	Linep1 = pFirst1.x * (pSecond1.y - pFirst2.y) +
		pFirst2.x * (pFirst1.y - pSecond1.y) +
		pSecond1.x * (pFirst2.y - pFirst1.y);
	Linep2 = pFirst1.x * (pSecond2.y - pFirst2.y) +
		pFirst2.x * (pFirst1.y - pSecond2.y) +
		pSecond2.x * (pFirst2.y - pFirst1.y);
	if ( ((Linep1 ^ Linep2) >= 0 ) && !(Linep1==0 && Linep2==0))//符号位异或为0:pSecond1和pSecond2在pFirst1->pFirst2同侧
	{
		return false;
	}
	//判断pFirst1和pFirst2是否在pSecond1->pSecond2两侧
	Linep1 = pSecond1.x * (pFirst1.y - pSecond2.y) +
		pSecond2.x * (pSecond1.y - pFirst1.y) +
		pFirst1.x * (pSecond2.y - pSecond1.y);
	Linep2 = pSecond1.x * (pFirst2.y - pSecond2.y) +
		pSecond2.x * (pSecond1.y - pFirst2.y) +
		pFirst2.x * (pSecond2.y - pSecond1.y);
	if ( ((Linep1 ^ Linep2) >= 0 ) && !(Linep1==0 && Linep2==0))//符号位异或为0:pFirst1和pFirst2在pSecond1->pSecond2同侧
	{
		return false;
	}
	//否则判为相交
	return true;
}

//求两直线交点，前提是两条直线必须有交点
//在相交的情况下，可以应付各种情况(垂直、系数等)

CPoint2D GetCrossPoint(const CPoint2D & p1, const CPoint2D & p2, POINT q1, const CPoint2D & q2)
{
	//必须相交求出的才是线段的交点，但是下面的程序段是通用的
	assert(IsLineSegmentCross(p1,p2,q1,q2));
	// 根据两点式化为标准式，进而求线性方程组
	POINT crossPoint;
	long tempLeft,tempRight;
	//求x坐标
	tempLeft = (q2.x - q1.x) * (p1.y - p2.y) - (p2.x - p1.x) * (q1.y - q2.y);
	tempRight = (p1.y - q1.y) * (p2.x - p1.x) * (q2.x - q1.x) + q1.x * (q2.y - q1.y) * (p2.x - p1.x) - p1.x * (p2.y - p1.y) * (q2.x - q1.x);
    crossPoint.x =(int)( (double)tempRight / (double)tempLeft );
	//求y坐标	
	tempLeft = (p1.x - p2.x) * (q2.y - q1.y) - (p2.y - p1.y) * (q1.x - q2.x);
	tempRight = p2.y * (p1.x - p2.x) * (q2.y - q1.y) + (q2.x- p2.x) * (q2.y - q1.y) * (p1.y - p2.y) - q2.y * (q1.x - q2.x) * (p2.y - p1.y);
	crossPoint.y =(int)( (double)tempRight / (double)tempLeft );
	return crossPoint;
}

*/