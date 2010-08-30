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
	//����Ƿ�Ϊ0���ж��Ƿ���ͬһֱ����
	if(fabs((p1.x-p0.x)*(p2.y-p0.y)-(p2.x-p0.x)*(p1.y-p0.y)) > 0.01)
		return false;
	//�ж��Ƿ����߶���
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
		return a;//���PA��PB������ͬ�����˳������������ؾ���
	
	if(a*a>=b*b+c*c)
		return b;
	if(b*b>=a*a+c*c)
		return a;

	double l=(a+b+c)/2;     //�ܳ���һ��
	double s=sqrt(l*(l-a)*(l-b)*(l-c));  //���׹�ʽ�����
	return 2*s/c;
}

/*

//�ж������߶��Ƿ��ཻ(�н���)

bool IsLineSegmentCross(const CPoint2D pFirst1, const CPoint2D &pFirst2, const CPoint2D &pSecond1, const CPoint2D &pSecond2)
{
	//ÿ���߶ε����㶼����һ���߶ε����Ҳ�ͬ�࣬���ܶ϶��߶��ཻ
	//��ʽ��������(x1,y1)->(x2,y2),�жϵ�(x3,y3)�����������,�ұ�,��������.
	//p=x1(y3-y2)+x2(y1-y3)+x3(y2-y1).p<0 ���,	p=0 ����, p>0 �Ҳ�
	long Linep1,Linep2;
	//�ж�pSecond1��pSecond2�Ƿ���pFirst1->pFirst2����
	Linep1 = pFirst1.x * (pSecond1.y - pFirst2.y) +
		pFirst2.x * (pFirst1.y - pSecond1.y) +
		pSecond1.x * (pFirst2.y - pFirst1.y);
	Linep2 = pFirst1.x * (pSecond2.y - pFirst2.y) +
		pFirst2.x * (pFirst1.y - pSecond2.y) +
		pSecond2.x * (pFirst2.y - pFirst1.y);
	if ( ((Linep1 ^ Linep2) >= 0 ) && !(Linep1==0 && Linep2==0))//����λ���Ϊ0:pSecond1��pSecond2��pFirst1->pFirst2ͬ��
	{
		return false;
	}
	//�ж�pFirst1��pFirst2�Ƿ���pSecond1->pSecond2����
	Linep1 = pSecond1.x * (pFirst1.y - pSecond2.y) +
		pSecond2.x * (pSecond1.y - pFirst1.y) +
		pFirst1.x * (pSecond2.y - pSecond1.y);
	Linep2 = pSecond1.x * (pFirst2.y - pSecond2.y) +
		pSecond2.x * (pSecond1.y - pFirst2.y) +
		pFirst2.x * (pSecond2.y - pSecond1.y);
	if ( ((Linep1 ^ Linep2) >= 0 ) && !(Linep1==0 && Linep2==0))//����λ���Ϊ0:pFirst1��pFirst2��pSecond1->pSecond2ͬ��
	{
		return false;
	}
	//������Ϊ�ཻ
	return true;
}

//����ֱ�߽��㣬ǰ��������ֱ�߱����н���
//���ཻ������£�����Ӧ���������(��ֱ��ϵ����)

CPoint2D GetCrossPoint(const CPoint2D & p1, const CPoint2D & p2, POINT q1, const CPoint2D & q2)
{
	//�����ཻ����Ĳ����߶εĽ��㣬��������ĳ������ͨ�õ�
	assert(IsLineSegmentCross(p1,p2,q1,q2));
	// ��������ʽ��Ϊ��׼ʽ�����������Է�����
	POINT crossPoint;
	long tempLeft,tempRight;
	//��x����
	tempLeft = (q2.x - q1.x) * (p1.y - p2.y) - (p2.x - p1.x) * (q1.y - q2.y);
	tempRight = (p1.y - q1.y) * (p2.x - p1.x) * (q2.x - q1.x) + q1.x * (q2.y - q1.y) * (p2.x - p1.x) - p1.x * (p2.y - p1.y) * (q2.x - q1.x);
    crossPoint.x =(int)( (double)tempRight / (double)tempLeft );
	//��y����	
	tempLeft = (p1.x - p2.x) * (q2.y - q1.y) - (p2.y - p1.y) * (q1.x - q2.x);
	tempRight = p2.y * (p1.x - p2.x) * (q2.y - q1.y) + (q2.x- p2.x) * (q2.y - q1.y) * (p1.y - p2.y) - q2.y * (q1.x - q2.x) * (p2.y - p1.y);
	crossPoint.y =(int)( (double)tempRight / (double)tempLeft );
	return crossPoint;
}

*/