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

//����p1��p2�ľ����ƽ��
extern "C" 
double Distance(const CPoint3D &p1, const CPoint3D &p2)
{
	double dist;
	dist = ((p2.x-p1.x)*(p2.x-p1.x) 
		+ (p2.y-p1.y)*(p2.y-p1.y) 
		+ (p2.z-p1.z)*(p2.z-p1.z));
	return (double)sqrt(dist);
}

//���ú��׹�ʽ����Ϊa,b,c�������ε����
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
	//�ռ�ֱ�ߵķ�������
	SpaceVector LineV;
	//����������ƽ��ķ�����
	SpaceVector TriangleV;

	//�����εı߷�������
	SpaceVector VP12, VP13;

	//��ռ�ֱ�ߵķ�������
	LineV.m = LinePoint2.x - LinePoint1.x;
	LineV.n = LinePoint2.y - LinePoint1.y;
	LineV.p = LinePoint2.z - LinePoint1.z;
	
	//ƽ�淽�̳�����
	double TriD;
	/*-------����ƽ��ķ�������������-------*/
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
	//���㳣����
	TriD = -(TriangleV.m*TrianglePoint1.x
		+ TriangleV.n*TrianglePoint1.y
		+ TriangleV.p*TrianglePoint1.z);
	/*-------���ֱ����ƽ��Ľ�������---------*/
	/* ˼·��
	*     ���Ƚ�ֱ�߷���ת��Ϊ����������ʽ��Ȼ�����ƽ�淽�̣���ò���t��
	* ��t����ֱ�ߵĲ������̼��������������
	*/
	double tempU, tempD;  //��ʱ����
	tempU = TriangleV.m*LinePoint1.x + TriangleV.n*LinePoint1.y 
		+ TriangleV.p*LinePoint1.z + TriD;
	tempD = TriangleV.m*LineV.m + TriangleV.n*LineV.n + TriangleV.p*LineV.p;
	
	if(tempD == 0.0) //ֱ����ƽ��ƽ�л���ƽ����
	{
		//printf("The line is parallel with the plane.\n");
		return 0;
	}

	//�������t
	double t = -tempU/tempD;
	
	//���㽻������
	CrossPoint.x = LineV.m*t + LinePoint1.x;
	CrossPoint.y = LineV.n*t + LinePoint1.y;
	CrossPoint.z = LineV.p*t + LinePoint1.z;
	
	/*----------�жϽ����Ƿ����������ڲ�---------*/
	//���������������ߵĳ���
	double d12 = Distance(TrianglePoint1, TrianglePoint2);
	double d13 = Distance(TrianglePoint1, TrianglePoint3);
	double d23 = Distance(TrianglePoint2, TrianglePoint3);
	
	//���㽻�㵽��������ĳ���
	double c1 = Distance(CrossPoint, TrianglePoint1);
	double c2 = Distance(CrossPoint, TrianglePoint2);
	double c3 = Distance(CrossPoint, TrianglePoint3);
	
	//�������μ��������ε����
	double areaD = Area(d12, d13, d23);  //���������
	double area1 = Area(c1, c2, d12);    //��������1
	double area2 = Area(c1, c3, d13);    //��������2
	double area3 = Area(c2, c3, d23);    //��������3
	
	//��������жϵ��Ƿ����������ڲ�
	if(fabs(area1+area2+area3-areaD) > MINERROR) // ���ཻ
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

// �ж������Ƿ���
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