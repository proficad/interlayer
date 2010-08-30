// Point3D.cpp: implementation of the CPoint3D class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Point3D.h"
#include <math.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern "C" double Distance(const CPoint3D &p1, const CPoint3D &p2);
extern "C" double Area(double a, double b, double c);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPoint3D::CPoint3D()
{
	x = 0.0;
	y = 0.0;
	z = 0.0;
	m_dValue = 0.0;
}

CPoint3D::CPoint3D(double initX, double initY, double initZ, double dValue)
{
	x = initX;
	y = initY;
	z = initZ;
	m_dValue = dValue;
}

const CPoint3D& CPoint3D::operator=(const CPoint3D& ptSrc)
{
	x = ptSrc.x;
	y = ptSrc.y;
	z = ptSrc.z;
	strcpy_s(szInfo, ptSrc.szInfo);
	return *this;
}

//计算p1到p2的距离的平方
extern "C" 
double Distance(const CPoint3D &p1, const CPoint3D &p2)
{
	double dist;
	dist = ((p2.x-p1.x)*(p2.x-p1.x) 
		+ (p2.y-p1.y)*(p2.y-p1.y) 
		+ (p2.z-p1.z)*(p2.z-p1.z));
	return (double)sqrt(dist);
}

//利用海伦公式求变成为a,b,c的三角形的面积
extern "C" 
double Area(double a, double b, double c)
{
	double s = (a+b+c)/2;
	return (double)sqrt(s*(s-a)*(s-b)*(s-c));
}

extern "C" 
int InterPoint(CPoint3D LinePoint1, CPoint3D LinePoint2, 
			   CPoint3D TrianglePoint1, CPoint3D TrianglePoint2,
			   CPoint3D TrianglePoint3, CPoint3D &CrossPoint)
{
	//空间直线的方向向量
	SpaceVector LineV;
	//三角形所在平面的法向量
	SpaceVector TriangleV;

	//三角形的边方向向量
	SpaceVector VP12, VP13;

	//求空间直线的方向向量
	LineV.m = LinePoint2.x - LinePoint1.x;
	LineV.n = LinePoint2.y - LinePoint1.y;
	LineV.p = LinePoint2.z - LinePoint1.z;
	
	//平面方程常数项
	double TriD;
	/*-------计算平面的法向量及常数项-------*/
	//point1->point2
	VP12.m = TrianglePoint2.x - TrianglePoint1.x;
	VP12.n = TrianglePoint2.y - TrianglePoint1.y;
	VP12.p = TrianglePoint2.z - TrianglePoint1.z;
	//point1->point3
	VP13.m = TrianglePoint3.x - TrianglePoint1.x;
	VP13.n = TrianglePoint3.y - TrianglePoint1.y;
	VP13.p = TrianglePoint3.z - TrianglePoint1.z;
	//VP12xVP13
	TriangleV.m = VP12.n*VP13.p - VP12.p*VP13.n;
	TriangleV.n = -(VP12.m*VP13.p - VP12.p*VP13.m);
	TriangleV.p= VP12.m*VP13.n - VP12.n*VP13.m;
	//计算常数项
	TriD = -(TriangleV.m*TrianglePoint1.x
		+ TriangleV.n*TrianglePoint1.y
		+ TriangleV.p*TrianglePoint1.z);
	/*-------求解直线与平面的交点坐标---------*/
	/* 思路：
	*     首先将直线方程转换为参数方程形式，然后代入平面方程，求得参数t，
	* 将t代入直线的参数方程即可求出交点坐标
	*/
	double tempU, tempD;  //临时变量
	tempU = TriangleV.m*LinePoint1.x + TriangleV.n*LinePoint1.y 
		+ TriangleV.p*LinePoint1.z + TriD;
	tempD = TriangleV.m*LineV.m + TriangleV.n*LineV.n + TriangleV.p*LineV.p;
	
	if(tempD == 0.0) //直线与平面平行或在平面上
	{
		//printf("The line is parallel with the plane.\n");
		return 0;
	}

	//计算参数t
	double t = -tempU/tempD;
	
	//计算交点坐标
	CrossPoint.x = LineV.m*t + LinePoint1.x;
	CrossPoint.y = LineV.n*t + LinePoint1.y;
	CrossPoint.z = LineV.p*t + LinePoint1.z;
	
	/*----------判断交点是否在三角形内部---------*/
	//计算三角形三条边的长度
	double d12 = Distance(TrianglePoint1, TrianglePoint2);
	double d13 = Distance(TrianglePoint1, TrianglePoint3);
	double d23 = Distance(TrianglePoint2, TrianglePoint3);
	
	//计算交点到三个顶点的长度
	double c1 = Distance(CrossPoint, TrianglePoint1);
	double c2 = Distance(CrossPoint, TrianglePoint2);
	double c3 = Distance(CrossPoint, TrianglePoint3);
	
	//求三角形及子三角形的面积
	double areaD = Area(d12, d13, d23);  //三角形面积
	double area1 = Area(c1, c2, d12);    //子三角形1
	double area2 = Area(c1, c3, d13);    //子三角形2
	double area3 = Area(c2, c3, d23);    //子三角形3
	
	//根据面积判断点是否在三角形内部
	if(fabs(area1+area2+area3-areaD) > MINERROR) // 不相交
		return 0;

	if( CrossPoint.x >= (min(LinePoint1.x,LinePoint2.x)-MINERROR) 
		&& CrossPoint.x <= (max(LinePoint1.x,LinePoint2.x)+MINERROR ) 
		&& CrossPoint.y >= (min(LinePoint1.y,LinePoint2.y)-MINERROR) 
		&& CrossPoint.y <= (max(LinePoint1.y,LinePoint2.y)+MINERROR)
		&& CrossPoint.z >= (min(LinePoint1.z,LinePoint2.z)-MINERROR) 
		&& CrossPoint.z <= (max(LinePoint1.z,LinePoint2.z)+MINERROR)
		)
		return 2;

	return 1;
}

// 判断三点是否共线
extern "C" 
bool DecideCollineation(const CPoint3D &pt1,const CPoint3D &pt2, const CPoint3D &pt3)
{
	if( fabs((pt1.y-pt2.y)*(pt1.x-pt2.x) - (pt3.y-pt2.y)*(pt3.x-pt2.x))<0.000001
	 && fabs((pt1.z-pt2.z)*(pt1.x-pt2.x) - (pt3.z-pt2.z)*(pt3.x-pt2.x))<0.000001
	 && fabs((pt1.z-pt2.z)*(pt1.y-pt2.y) - (pt3.z-pt2.z)*(pt3.y-pt2.y))<0.000001)
		return true;
	else
		return false;	
}