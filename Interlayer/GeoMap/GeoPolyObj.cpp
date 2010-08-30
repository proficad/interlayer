// GeoPolyObj.cpp: implementation of the CGeoPolyObj class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GeoPolyObj.h"
#include "GeoMapView.h"
#include "GeoMapDoc.h"
//#include "Resource.h"
#include "DlgSetPoly.h"
#include <float.h>
#include "CmdGeoDrawObj.h"

#include "ToolBase.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

double ptopointset(int vcount, CPoint2D pointset[], CPoint2D p, CPoint2D &q, int *iIndex);

#define INF 1E200
#define EP 1E-10

/********************\
*                    *
*   ��Ļ�������     *
*                    *
\********************/


double dist(CPoint2D p1,CPoint2D p2)                // ��������֮��ŷ�Ͼ���
{
	return( sqrt( (p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y) ) );
}


/******************************************************************************
r=multiply(sp,ep,op),�õ�(sp-op)*(ep-op)��*��
r>0:ep��ʸ��opsp����ʱ�뷽��
r=0��opspep���㹲�ߣ�
r<0:ep��ʸ��opsp��˳ʱ�뷽��
*******************************************************************************/

double multiply(CPoint2D sp,CPoint2D ep,CPoint2D op)
{
	return((sp.x-op.x)*(ep.y-op.y)-(ep.x-op.x)*(sp.y-op.y));
}

/*******************************************************************************
r=dotmultiply(p1,p2,op),�õ�ʸ��(p1-op)��(p2-op)�ĵ�����������ʸ��������ʸ��
r<0:��ʸ���н�Ϊ��ǣ�r=0����ʸ���н�Ϊֱ�ǣ�r>0:��ʸ���н�Ϊ�۽�
*******************************************************************************/
double dotmultiply(CPoint2D p1,CPoint2D p2,CPoint2D p0)
{
	return ((p1.x-p0.x)*(p2.x-p0.x)+(p1.y-p0.y)*(p2.y-p0.y));
}

/* �жϵ�p�Ƿ����߶�l�ϣ�
	������(p���߶�l���ڵ�ֱ����)&& (��p�����߶�lΪ�Խ��ߵľ�����) */

bool OnLine(LINESEG l,CPoint2D p)
{
// �����ϼ�������
//	return((multiply(l.e,p,l.s)==0)				// ���㹲��
//		&& ( ( (p.x-l.s.x)*(p.x-l.e.x)<=0 )
//		&&( (p.y-l.s.y)*(p.y-l.e.y)<=0 ) ) );

	// ʵ���ϣ���ֵ��������˫���ȵĶ������ʹ��ڴ�С��1��Χ�ڵ����

	long xmin = min(l.s.x,l.e.x)-5;
	long xmax = max(l.s.x,l.e.x)+5;
	long ymin = min(l.s.y,l.e.y)-5;
	long ymax = max(l.s.y,l.e.y)+5;
	
	if( ((long)p.x >= xmin && (long)p.x<=xmax)
		&& ((long)p.y >=ymin && (long)p.y<=ymax) )
		return true;
	else
		return false;
}

// ���ص�p�Ե�oΪԲ����ʱ����תalpha(��λ������)�����ڵ�λ��
POINT rotate(CPoint2D o,double alpha,CPoint2D p)
{
	POINT tp;
	p.x-=o.x;
	p.y-=o.y;
	tp.x=p.x*cos(alpha)-p.y*sin(alpha)+o.x;
	tp.y=p.y*cos(alpha)+p.x*sin(alpha)+o.y;
	return tp;
}


/* ���ض�����o�㣬��ʼ��Ϊos����ֹ��Ϊoe�ļн�(��λ������)
�Ƕ�С��pi��������ֵ
�Ƕȴ���pi�����ظ�ֵ
�����������߶�֮��ļн�
*/
double angle(CPoint2D o,CPoint2D s,CPoint2D e)
{
	double cosfi,fi,norm;
	double dsx = s.x - o.x;
	double dsy = s.y - o.y;
	double dex = e.x - o.x;
	double dey = e.y - o.y;
	
	cosfi=dsx*dex+dsy*dey;
	norm=(dsx*dsx+dey*dey)*(dex*dex+dey*dey);
	cosfi /= sqrt( norm );
	
	if (cosfi >=  1.0 ) return 0;
	if (cosfi <= -1.0 ) return -3.1415926;
	
	fi=acos(cosfi);
	if (dsx*dey-dsy*dex>0)
		return fi;      // ˵��ʸ��os ��ʸ�� oe��˳ʱ�뷽��
	return -fi;
}

/*
* ��������֮�����ת��
* ������ȷ������ѧ���
* 1. ��������ʱ��ת���ķ�����������
* 2. ��������֮��ļн�theta�� ��ָ(A^B)/(|A|*|B|) = cos(theta)��0<=theta<=180 �ȣ� ����û�з���֮��
* 3. ������������ת�ǣ���ָ������p1��ʼ����ʱ����ת��ת������p2ʱ����ת���ĽǶȣ� ��Χ�� 0 ~ 360��
* ��������p1��p2����ת�ǣ��㷨���£�
* ����ͨ����˺�arccosine�ĵõ���������֮��ļн�
* Ȼ���ж�ͨ��������ж���������֮���λ�ù�ϵ
* ���p2��p1��˳ʱ�뷽��, ����arccose�ĽǶ�ֵ, ��Χ0 ~ 180.0(�������ֶ���,���Թ����������)
* ���򷵻� 360.0 - arecose��ֵ, ����180��360(�������ֶ���,���Ϊ��)
*/ 
double getRotateAngle(double x1, double y1, double x2, double y2)
{
	const double epsilon = 1.0e-6;
	const double nyPI = acos(-1.0);
	double dist, dot, degree, angle;

	// normalize
	dist = sqrt( x1 * x1 + y1 * y1 );
	x1 /= dist;
	y1 /= dist;
	dist = sqrt( x2 * x2 + y2 * y2 );
	x2 /= dist;
	y2 /= dist;
	// dot product
	dot = x1 * x2 + y1 * y2;
	if ( fabs(dot-1.0) <= epsilon ) 
		angle = 0.0;
	else if ( fabs(dot+1.0) <= epsilon ) 
		angle = nyPI;
	else
	{
		double cross;

		angle = acos(dot);
		//cross product
		cross = x1 * y2 - x2 * y1;
		// vector p2 is clockwise from vector p1 
		// with respect to the origin (0.0)
		if (cross < 0 )
		{ 
			angle = 2 * nyPI - angle;
		}    
	}
	degree = angle *  180.0 / nyPI;
	return degree;
}

/*****************************\
*                             *
*      �߶μ�ֱ�ߵĻ�������   *
*                             *
\*****************************/

/* �жϵ����߶εĹ�ϵ,��;�ܹ㷺
�������Ǹ�������Ĺ�ʽд�ģ�P�ǵ�C���߶�AB����ֱ�ߵĴ���

  AC dot AB
  r =     ---------
  ||AB||^2
  (Cx-Ax)(Bx-Ax) + (Cy-Ay)(By-Ay)
  = -------------------------------
  L^2
  
    r has the following meaning:
	
	  r=0      P = A
	  r=1      P = B
	  r<0 P is on the backward extension of AB
	  r>1      P is on the forward extension of AB
	  0<1
*/

double relation(CPoint2D p,LINESEG l)
{
	LINESEG tl;
	tl.s=l.s;
	tl.e=p;
	return dotmultiply(tl.e,l.e,l.s)/(dist(l.s,l.e)*dist(l.s,l.e));
}

// ���C���߶�AB����ֱ�ߵĴ��� P
CPoint2D perpendicular(CPoint2D p,LINESEG l)
{
	double r=relation(p,l);
	CPoint2D tp;
	tp.x=l.s.x+r*(l.e.x-l.s.x);
	tp.y=l.s.y+r*(l.e.y-l.s.y);
	return tp;
}


/* ���p���߶�l����̾���,�������߶��Ͼ�õ�����ĵ�np
	ע�⣺np���߶�l�ϵ���p����ĵ㣬��һ���Ǵ��� */
double ptolinesegdist(CPoint2D p, LINESEG l, CPoint2D &np, int &ret)
{
	double r=relation(p,l);

	if(r<0)
	{
		np=l.s;
		ret = -1;
		return dist(p,l.s);
	}

	if(r>1)
	{
		np=l.e;
		ret = 1;
		return dist(p,l.e);
	}

	np=perpendicular(p,l);
	
	ret = 0;
	
	return dist(p,np);
}

/* ����㵽���߼����������,�����������.
	ע�⣺���õ���ptolineseg()���� */

double ptopointset(int vcount, CPoint2D pointset[], CPoint2D p, CPoint2D &q, int *iIndex)
{
	int i;
	double cd = double(DBL_MAX), td;
	LINESEG l;
	CPoint2D tq, cq;
	int ret;
	for (i = 0; i < vcount - 1; i++)
	{
		l.s = pointset[i];
		l.e = pointset[i + 1];
		td = ptolinesegdist(p, l, tq, ret);
		if (td < cd)
		{
			cd = td;
			cq = tq;
			if(ret == 0) //����Ǵ���
			{
				iIndex[0] = i;
				iIndex[1] = i+1;
			}
			else if( ret == -1) // ���׵�
			{
				iIndex[0] = -1;
				iIndex[1] = i;
			}
			else				// ��β��
			{
				iIndex[0] = -1;
				iIndex[1] = i+2;
			}
		}
	}
	q = cq;
	return cd;
}

/* �ж�Բ�Ƿ��ڶ������.ptolineseg()������Ӧ��2 */
bool CircleInsidePolygon(int vcount,CPoint2D center,double radius,CPoint2D polygon[])
{
	CPoint2D q;
	double d;
	q.x = 0;
	q.y = 0;
	int index[2];
	d = ptopointset(vcount, polygon, center, q, index);
	if ( d < radius )
		return true;
	else
		return false;
}

/* ��������ʸ��l1��l2�ļнǵ�����(-1 --- 1)
	ע�⣺������������нǵĻ���ע�ⷴ���Һ����Ķ������Ǵ� 0��pi */

double cosine(LINESEG l1,LINESEG l2)
{
	return  (((l1.e.x-l1.s.x)*(l2.e.x-l2.s.x) +
		(l1.e.y-l1.s.y)*(l2.e.y-l2.s.y))/(dist(l1.e,l1.s)*dist(l2.e,l2.s))) ;
}

// �����߶�l1��l2֮��ļн� ��λ������ ��Χ(-pi��pi)
double lsangle(LINESEG l1,LINESEG l2)
{
	CPoint2D o,s,e;
	o.x=o.y=0;
	s.x=l1.e.x-l1.s.x;
	s.y=l1.e.y-l1.s.y;
	e.x=l2.e.x-l2.s.x;
	e.y=l2.e.y-l2.s.y;
	return angle(o,s,e);
}

// ����߶�u��v�ཻ(�����ཻ�ڶ˵㴦)ʱ������true
bool intersect(LINESEG u,LINESEG v)
{
	return( (max(u.s.x,u.e.x)>=min(v.s.x,v.e.x))&&                 // �ų�ʵ��
        (max(v.s.x,v.e.x)>=min(u.s.x,u.e.x))&&
        (max(u.s.y,u.e.y)>=min(v.s.y,v.e.y))&&
        (max(v.s.y,v.e.y)>=min(u.s.y,u.e.y))&&
        (multiply(v.s,u.e,u.s)*multiply(u.e,v.e,u.s)>=0)&&         // ����ʵ��
        (multiply(u.s,v.e,v.s)*multiply(v.e,u.e,v.s)>=0));
}


//  (�߶�u��v�ཻ)&&(���㲻��˫���Ķ˵�) ʱ����true   
bool intersect_A(LINESEG u,LINESEG v)
{
	return((intersect(u,v))&&
		(!OnLine(u,v.s))&&
		(!OnLine(u,v.e))&&
		(!OnLine(v,u.e))&&
		(!OnLine(v,u.s)));
}


// �߶�v����ֱ�����߶�u�ཻʱ����true���������ж��߶�u�Ƿ�����߶�v 
bool intersect_l(LINESEG u,LINESEG v)
{
	return multiply(u.s,v.e,v.s)*multiply(v.e,u.e,v.s)>=0;
}


// ������֪�������꣬����������ֱ�߽������̣� a*x+b*y+c = 0  (a >= 0) 
LINE makeline(CPoint2D p1,CPoint2D p2)
{
	LINE tl;
	int sign = 1;
	tl.a = p2.y - p1.y;
	if (tl.a < 0)
	{
		sign = -1;
		tl.a = sign*tl.a;
	}
	tl.b = sign* (p1.x - p2.x);
	tl.c = sign* (p1.y*p2.x - p1.x*p2.y);
	return tl;
}

// ����ֱ�߽������̷���ֱ�ߵ�б��k,ˮƽ�߷��� 0,��ֱ�߷��� 1e200
double slope(LINE l)
{
	if(abs(l.a) < 1e-20) return 0;
	if(abs(l.b) < 1e-20) return INF;
	return -(l.a/l.b);
}

// ����ֱ�ߵ���б��alpha ( 0 - pi)
double alpha(LINE l)
{
	if(abs(l.a)< EP)return 0;
	if(abs(l.b)< EP)return PI/2;
	double k=slope(l);
	if(k>0)
		return atan(k);
	else
		return PI+atan(k);
}

// ���p����ֱ��l�ĶԳƵ� 
CPoint2D symmetry(LINE l,CPoint2D p)
{
	CPoint2D tp;
	tp.x=((l.b*l.b-l.a*l.a)*p.x-2*l.a*l.b*p.y-2*l.a*l.c)/(l.a*l.a+l.b*l.b);
	tp.y=((l.a*l.a-l.b*l.b)*p.y-2*l.a*l.b*p.x-2*l.b*l.c)/(l.a*l.a+l.b*l.b);
	return tp;
}

/* �������ֱ�� l1(a1*x+b1*y+c1 = 0), l2(a2*x+b2*y+c2 = 0)�ཻ��
	����true���ҷ��ؽ���p */
bool lineintersect(LINE l1,LINE l2,CPoint2D &p) // �� L1��L2
{
	double d = l1.a*l2.b - l2.a*l1.b;
	if (fabs(d) < DBL_EPSILON )
		return false;
	p.x = (l2.c*l1.b - l1.c*l2.b)/d;
	p.y = (l2.a*l1.c - l1.a*l2.c)/d;
	return true;
}

// ����߶�l1��l2�ཻ������true�ҽ�����(inter)���أ����򷵻�false
bool intersection(LINESEG l1,LINESEG l2,CPoint2D &inter)
{
	LINE ll1,ll2;
	ll1=makeline(l1.s,l1.e);
	ll2=makeline(l2.s,l2.e);
	if(lineintersect(ll1,ll2,inter))
	{
		return OnLine(l1,inter)&&OnLine(l2,inter);
	}
	else
		return false;
}

// 
// bool intersection2(LINESEG l1,LINESEG l2,CPoint2D &inter)
// {
// 	if( !intersect(l1,l2) )
// 		return false;
// 
// 	LINE ll1,ll2;
// 	ll1=makeline2(l1.s,l1.e);
// 	ll2=makeline2(l2.s,l2.e);
// 	
// 	double denom = ll1.a * ll2.b - ll2.a * ll1.b;
// 	
// 	if( fabs(denom) < EP )
// 		return false;
// 
// 	inter.x = (ll1.b * ll2.c - ll2.b * ll1.c) / denom;
// 	inter.y = (ll2.a * ll1.c - ll1.a * ll2.c) / denom;
// 
// 	return true;
// }


IMPLEMENT_SERIAL(CGeoPolyObj, CGeoDrawObj, VERSIONABLE_SCHEMA|0)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeoPolyObj::CGeoPolyObj()
{
	m_points = NULL;
	m_geoPoints = NULL;
	m_nPoints = 0;
	m_nAllocPoints = 0;	
	m_nHandleMoving = -1;
	m_iSpaceWidth = 10;
	m_eLine2.m_nPenWide = 10;

	m_ePolyType = generic;

	m_strText = _T("");						// ��ע�ı�
	memset(&m_lfFont,0,sizeof(LOGFONT));	// ��ע���弰��С
	m_lfFont.lfHeight = 25;
	m_lfFont.lfCharSet = GB2312_CHARSET;
	strcpy_s(m_lfFont.lfFaceName,_T("����"));

	m_clrText = RGB(0,0,0);					// ��ɫ
	m_bShowText = TRUE;						// �Ƿ���ʾ

	SetObjName(_T("����"));
}

CGeoPolyObj::CGeoPolyObj(CDrawLayer* pLayer)
:CGeoDrawObj(pLayer)
{
	m_points = NULL;
	m_geoPoints = NULL;
	m_nPoints = 0;
	m_nAllocPoints = 0;	
	m_nHandleMoving = -1;
	m_iSpaceWidth = 10;
	m_eLine2.m_nPenWide = 10;
	m_ePolyType = generic;
	m_strText = _T("");			// ��ע�ı�
	memset(&m_lfFont,0,sizeof(LOGFONT));			// ��ע���弰��С
	m_lfFont.lfHeight = 25;
	m_lfFont.lfCharSet = GB2312_CHARSET;
	strcpy_s(m_lfFont.lfFaceName,_T("����"));
	
	m_clrText = RGB(0,0,0);			// ��ɫ
	m_bShowText = TRUE;		// �Ƿ���ʾ
	SetObjName(_T("����"));
}

CGeoPolyObj::CGeoPolyObj(const CRect& position, CDrawLayer* pLayer)
:CGeoDrawObj(position,pLayer)
{
	m_points = NULL;
	m_geoPoints = NULL;
	m_nPoints = 0;
	m_nAllocPoints = 0;	
	m_nHandleMoving = -1;
	m_iSpaceWidth = 10;
	m_eLine2.m_nPenWide = 10;
	m_ePolyType = generic;
	m_strText = _T("");			// ��ע�ı�
	memset(&m_lfFont,0,sizeof(LOGFONT));			// ��ע���弰��С
	m_lfFont.lfHeight = 25;
	m_lfFont.lfCharSet = GB2312_CHARSET;
	strcpy_s(m_lfFont.lfFaceName,_T("����"));
	
	m_clrText = RGB(0,0,0);			// ��ɫ
	m_bShowText = TRUE;		// �Ƿ���ʾ
	SetObjName(_T("����"));
}

CGeoPolyObj::~CGeoPolyObj()
{
	if (m_points != NULL)
		delete[] m_points;
	if (m_geoPoints != NULL)
		delete[] m_geoPoints;
}

void CGeoPolyObj::Draw(CDC *pDC, CGeoMapView *pView)
{
	if( !m_bShow )
		return;

 	if( !pDC->IsPrinting() )
 	{
 		CRect cliRect;
 		pView->GetClientRect(cliRect);
 		pView->ClientToDoc(cliRect);
 		if( !Intersects(cliRect) ) //�����ͼ�ز��ڵ�ǰ����������ʾ
 			return ;
 	}

	for (long i=0;i<m_nPoints; i++)
	{
		m_points[i] = CoordinateTransfer(m_geoPoints[i]);
	}

	int nSaved = pDC->SaveDC();
	pDC->SetBkMode(TRANSPARENT);          //�趨��ʾ��ʽ

	switch(m_eLine.m_nLineType)
	{
	case 0://����
		DrawFill(pDC, pView, CGeoPolyObj::MIDST);
		DrawLine(pDC, pView, CGeoPolyObj::MIDST, TRUE);
		break;
	case 1://˫��
		DrawFill(pDC, pView, CGeoPolyObj::OUTER);
		DrawLineSpace(pDC, pView);
		DrawLine(pDC, pView, CGeoPolyObj::OUTER, TRUE);
		DrawLine(pDC, pView, CGeoPolyObj::INNER, TRUE);
		break;
	case 2://������1
		DrawFill(pDC, pView, CGeoPolyObj::OUTER);
		DrawLineSpace(pDC, pView);
		DrawLine(pDC, pView, CGeoPolyObj::OUTER, TRUE);
		DrawLine(pDC, pView, CGeoPolyObj::INNER, FALSE);//��
		break;
	case 3://������2;
		DrawFill(pDC, pView, CGeoPolyObj::OUTER);
		DrawLineSpace(pDC, pView);
		DrawLine(pDC, pView, CGeoPolyObj::OUTER, FALSE);
		DrawLine(pDC, pView, CGeoPolyObj::INNER, TRUE);
		break;
	case 4://����1
		DrawFill(pDC, pView, CGeoPolyObj::OUTER);
		DrawLineSpace(pDC, pView);
		DrawLine(pDC, pView, CGeoPolyObj::OUTER, TRUE);
		DrawLine(pDC, pView, CGeoPolyObj::INNER, TRUE);		
		DrawLine(pDC, pView, CGeoPolyObj::MIDST, TRUE);
		break;
	case 5://����2
		DrawFill(pDC, pView, CGeoPolyObj::OUTER);
		DrawLineSpace(pDC, pView);
		DrawLine(pDC, pView, CGeoPolyObj::OUTER, FALSE);
		DrawLine(pDC, pView, CGeoPolyObj::INNER, FALSE);		
		DrawLine(pDC, pView, CGeoPolyObj::MIDST, TRUE);
		break;
	case 6://����3
		DrawFill(pDC, pView, CGeoPolyObj::OUTER);
		DrawLineSpace(pDC, pView);
		DrawLine(pDC, pView, CGeoPolyObj::OUTER, TRUE);
		DrawLine(pDC, pView, CGeoPolyObj::INNER, TRUE);		
		DrawLine(pDC, pView, CGeoPolyObj::MIDST, FALSE);
		break;
	}
	
	DrawArrow(pDC, pView);

	pDC->RestoreDC(nSaved);	
}

void CGeoPolyObj::DrawEditMode(CGeoMapView* pView)
{
	CClientDC dc(pView);
	pView->OnPrepareDC(&dc);
	
	int mode = dc.SetROP2(R2_NOTXORPEN);
	
	CBrush *pOldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	
	CSize size(3,3);
	dc.DPtoLP(&size);
	
	if (!m_eLine.m_bSmooth)//���⻬
	{
		if( m_eLine.m_bClose )//���
		{
			if( m_nPoints < 3 )
			{
				dc.MoveTo(m_points[m_nHandleMoving-2]);
				dc.LineTo(m_points[m_nHandleMoving-1]);
			}
			else
			{				
				dc.MoveTo(m_points[m_nHandleMoving-2]);
				dc.LineTo(m_points[m_nHandleMoving-1]);
				dc.LineTo(m_points[0]);
			}
		}
		
		else//�����
		{
			dc.MoveTo(m_points[m_nHandleMoving-2]);
			dc.LineTo(m_points[m_nHandleMoving-1]);
		}
	}

	//*******************
	else//�⻬
	{
		if( m_eLine.m_bClose )//���
		{
			if( m_nPoints < 3 )
				PolySpline3(dc.m_hDC, m_points, m_nPoints);	
			else
				PolySpline3(dc.m_hDC, m_points, m_nPoints, TRUE);
		}
		else//�����
			PolySpline3(dc.m_hDC, m_points, m_nPoints);
	}

	CPen pen(PS_SOLID,0,RGB(255,0,0));
	CPen *pOldPen = dc.SelectObject(&pen);
		

	for(int i = 1 ; i < m_nPoints-1; i++)
	{
		dc.MoveTo(m_points[i].x - size.cx, m_points[i].y);
		dc.LineTo(m_points[i].x + size.cx, m_points[i].y);
		
		dc.MoveTo(m_points[i].x, m_points[i].y - size.cy);
		dc.LineTo(m_points[i].x, m_points[i].y + size.cy);
	}

	dc.MoveTo(m_points[m_nPoints-1].x - size.cx, m_points[m_nPoints-1].y);
	dc.LineTo(m_points[m_nPoints-1].x + size.cx, m_points[m_nPoints-1].y);

	dc.MoveTo(m_points[m_nPoints-1].x, m_points[m_nPoints-1].y - size.cy);
	dc.LineTo(m_points[m_nPoints-1].x, m_points[m_nPoints-1].y + size.cy);

//	dc.Rectangle(m_points[m_nPoints-1].x - size.cx, m_points[m_nPoints-1].y - size.cy,	
//		m_points[m_nPoints-1].x + size.cx, m_points[m_nPoints-1].y + size.cy);
	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);	
	
	dc.SetROP2(mode);
}

void CGeoPolyObj::DrawMoveState(CGeoMapView* pView)
{
	CClientDC dc(pView);
	pView->OnPrepareDC(&dc);

	int mode = dc.SetROP2(R2_NOTXORPEN);
	DrawTracker(&dc, pView);

	CPen pen;
	pen.CreatePen(PS_SOLID, 0, RGB(255, 0, 0));
	CPen *pOldPen = dc.SelectObject(&pen);
	CBrush *pOldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);

	if (!m_eLine.m_bSmooth)//���⻬
	{
		if( m_eLine.m_bClose )//���
		{			
			dc.Polygon(m_points, m_nPoints);
		}
		
		else//�����
			dc.Polyline(m_points, m_nPoints);
		
	}
	//*******************
	else//�⻬
	{
		if( m_eLine.m_bClose )//���
			PolySpline3(dc.m_hDC, m_points, m_nPoints, TRUE);			
		else//�����
			PolySpline3(dc.m_hDC, m_points, m_nPoints);			
	}
		
	dc.SelectObject(pOldBrush);
	dc.SelectObject(pOldPen);
	dc.SetROP2(mode);
}

void CGeoPolyObj::DrawSizeState(CGeoMapView* pView)
{
	CClientDC dc(pView);
	pView->OnPrepareDC(&dc);

	int mode = dc.SetROP2(R2_NOTXORPEN);

	DrawTracker(&dc, pView);

	CPen pen;
	pen.CreatePen(PS_SOLID, 0, RGB(255, 0, 0));
	CPen *pOldPen = dc.SelectObject(&pen);
	CBrush *pOldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);	
	
	if (!m_eLine.m_bSmooth)//���⻬
	{
		if( m_eLine.m_bClose )//���
		{
			if( m_nHandleMoving == 1)
			{
				dc.MoveTo(m_points[m_nHandleMoving-1]);
				dc.LineTo(m_points[m_nPoints-1]);
				dc.MoveTo(m_points[m_nHandleMoving-1]);
				dc.LineTo(m_points[m_nHandleMoving]);
			}
			else if( m_nHandleMoving == m_nPoints )
			{
				dc.MoveTo(m_points[m_nHandleMoving-2]);
				dc.LineTo(m_points[m_nHandleMoving-1]);
				dc.LineTo(m_points[0]);
			}
			else
			{
				dc.MoveTo(m_points[m_nHandleMoving-2]);
				dc.LineTo(m_points[m_nHandleMoving-1]);
				dc.MoveTo(m_points[m_nHandleMoving-1]);
				dc.LineTo(m_points[m_nHandleMoving]);
			}
		}
		
		else//�����
		{
			if( m_nHandleMoving == 1)
			{
				dc.MoveTo(m_points[m_nHandleMoving-1]);
				dc.LineTo(m_points[m_nHandleMoving]);
			}
			else if( m_nHandleMoving == m_nPoints )
			{
				dc.MoveTo(m_points[m_nHandleMoving-2]);
				dc.LineTo(m_points[m_nHandleMoving-1]);
			}
			else
			{
				dc.MoveTo(m_points[m_nHandleMoving-2]);
				dc.LineTo(m_points[m_nHandleMoving-1]);
				dc.MoveTo(m_points[m_nHandleMoving-1]);
				dc.LineTo(m_points[m_nHandleMoving]);
			}
		}
	}
	//*******************
	else//�⻬
	{
		if( m_eLine.m_bClose )//���
		{
			PolySpline3(dc.m_hDC, m_points, m_nPoints, TRUE);
		}
		else//�����
		{
			CPoint points[11];
			int nCounts=0;

			if( m_nHandleMoving-5 > 0 )
			{
				points[nCounts] = m_points[m_nHandleMoving-5-1];
				nCounts ++;
			}

			if( m_nHandleMoving-4 > 0 )
			{
				points[nCounts] = m_points[m_nHandleMoving-4-1];
				nCounts ++;
			}

			if( m_nHandleMoving-3 > 0 )
			{
				points[nCounts] = m_points[m_nHandleMoving-3-1];
				nCounts ++;
			}

			if( m_nHandleMoving-2 > 0 )
			{
				points[nCounts] = m_points[m_nHandleMoving-2-1];
				nCounts ++;
			}

			if( m_nHandleMoving-1 > 0 )
			{
				points[nCounts] = m_points[m_nHandleMoving-1-1];
				nCounts ++;
			}

			points[nCounts] = m_points[m_nHandleMoving-1];
			nCounts ++;

			if( m_nHandleMoving+1 <= m_nPoints )
			{
				points[nCounts] = m_points[m_nHandleMoving+1-1];
				nCounts ++;
			}
			if( m_nHandleMoving+2 <= m_nPoints )
			{
				points[nCounts] = m_points[m_nHandleMoving+2-1];
				nCounts ++;
			}
			if( m_nHandleMoving+3 <= m_nPoints )
			{
				points[nCounts] = m_points[m_nHandleMoving+3-1];
				nCounts ++;
			}
			if( m_nHandleMoving+4 <= m_nPoints )
			{
				points[nCounts] = m_points[m_nHandleMoving+4-1];
				nCounts ++;
			}
			if( m_nHandleMoving+5 <= m_nPoints )
			{
				points[nCounts] = m_points[m_nHandleMoving+5-1];
				nCounts ++;
			}

			PolySpline3(dc.m_hDC, points, nCounts);
		}
	}	
	
	dc.SelectObject(pOldBrush);
	dc.SelectObject(pOldPen);
	dc.SetROP2(mode);
}


////////////////////////////////////////////////////////////////////////////////
//
// ����  : CGeoDrawObj::MoveHandleTo
// ����  : void
// ����  : int nHandle �ֱ�����
// ����  : CPoint point Ŀǰ�߼�λ��
// ����  : CGeoMapView *pView
// ˵��  : �ƶ�ĳ�ֱ�
//
void CGeoPolyObj::MoveHandleTo(int nHandle, CPoint point, CGeoMapView *pView)
{
	ASSERT_VALID(this);
	
	if( m_bLocked )
		return;
	
	ASSERT(nHandle >= 1 && nHandle <= m_nPoints);
	CPoint pt;
	pt = m_points[nHandle - 1];
	if (pt == point)
		return;
	
	m_nHandleMoving = nHandle;

	if (pView == NULL)
		Invalidate();
	else
	{		
		if( m_bEditing )
			DrawEditMode( pView );
		else if( m_bSizeChanging )
			DrawSizeState( pView );
		else if( m_bMoving )
			DrawMoveState( pView );
		else
			pView->InvalObj(this);	
	}
	
	m_points[nHandle - 1] = point;
	
	m_geoPoints[nHandle - 1] = CoordinateTransfer(point);
	
	if (pView == NULL)
		Invalidate();
	else
	{
		if( m_bEditing )
			DrawEditMode( pView );
		else if( m_bSizeChanging )
			DrawSizeState( pView );
		else if( m_bMoving )
			DrawMoveState( pView );
		else
			pView->InvalObj(this);
	}

	m_pDocument->SetModifiedFlag();
}

void CGeoPolyObj::MoveTo(const CRect& position, CGeoMapView* pView )
{
	ASSERT_VALID(this);
	
	if( m_bLocked )
		return;
	
	CRect rect = m_rtPosition;
	if (rect == position)
		return;
	
	if (pView == NULL)
		Invalidate();
	else
	{
		if( m_bEditing )
			DrawEditMode( pView );
		else if( m_bSizeChanging )
			DrawSizeState( pView );
		else if( m_bMoving )
			DrawMoveState( pView );
		else
			pView->InvalObj(this);	
	}
	
	for (int i = 0; i < m_nPoints; i += 1)
	{
		m_points[i].x += position.left - rect.left;
		m_points[i].y += position.top - rect.top;
		m_geoPoints[i] = CoordinateTransfer(m_points[i]);
	}
	
	m_rtPosition = position;
	
	if (pView == NULL)
		Invalidate();
	else
	{
		if( m_bEditing )
			DrawEditMode( pView );
		else if( m_bSizeChanging )
			DrawSizeState( pView );
		else if( m_bMoving )
			DrawMoveState( pView );
		else
			pView->InvalObj(this);
	}

	CPoint pt = m_rtPosition.CenterPoint();
	
	// ת��Ϊ�������
	m_geoCenterPoint = CoordinateTransfer(pt);
	m_pDocument->SetModifiedFlag();
}

////////////////////////////////////////////////////////////////////////////////
//
// ����  : CGeoDrawObj::OnEditProperties
// ����  : void
// ����  : CGeoMapView *pView
// ˵��  : �༭ͼ������
//
BOOL CGeoPolyObj::OnEditProperties(CGeoMapView *pView)
{
	ASSERT_VALID(this);

	if( m_bLocked )
		return FALSE;
	
	CDlgPolySet dlg;
	
	dlg.eFill = m_eFill;
	dlg.eLine = m_eLine;
	dlg.eLine2 = m_eLine2;
	dlg.m_iSpaceWidth = m_iSpaceWidth;
	
	if(dlg.DoModal() == IDOK)
	{
		m_eFill = dlg.eFill;
		m_eLine = dlg.eLine;
		m_eLine2 = dlg.eLine2;
		m_iSpaceWidth = dlg.m_iSpaceWidth;
				
		RecalcBounds(pView);
		pView->InvalObj(this);
		m_pDocument->SetModifiedFlag();

		return TRUE;
	}
	
	return FALSE;
}


////////////////////////////////////////////////////////////////////////////////
//
// ����  : CGeoDrawObj::Clone
// ����  : CGeoDrawObj*
// ����  : CGeoMapDoc *pDoc
// ˵��  : ��¡ͼ�ز���ͼ�ؼ��뵽�ĵ���
//
CGeoDrawObj* CGeoPolyObj::Clone(CDrawLayer *pLayer)
{
	ASSERT_VALID(this);
	
	CGeoPolyObj* pClone = new CGeoPolyObj(pLayer);
	pClone->m_rtPosition	=	m_rtPosition;
	pClone->m_bLocked		=	m_bLocked;			// ����ͼ��
	pClone->m_bShow			=	m_bShow;			// �Ƿ���ʾ
	pClone->m_strID			=	newGUID();			// �µ�ID
	pClone->m_ptAnchors.Copy(m_ptAnchors);			// ê��

	pClone->m_pDocument		=	m_pDocument;		// �ĵ�

	//////////////////////////////////////////////////////////////////////////
	pClone->m_nPoints			= m_nPoints	;						// ���Ƶ���
	pClone->m_nAllocPoints		= m_nAllocPoints;					// ����������
	pClone->m_points			= new CPoint[m_nAllocPoints];		// ���Ƶ�ͼ������
	pClone->m_geoPoints			= new CPoint2D[m_nAllocPoints];		// ���Ƶ�������
	
	pClone->m_eLine				= m_eLine;							// ������
	pClone->m_eFill				= m_eFill;							// �������
	pClone->m_eLine2			= m_eLine2;							// ������
	pClone->m_iSpaceWidth		= m_iSpaceWidth;					// �߼���
	pClone->m_ePolyType			= m_ePolyType;						// ���
	pClone->m_strText			= m_strText;			// ��ע�ı�
	memcpy(&pClone->m_lfFont, &m_lfFont,sizeof(LOGFONT));			// ��ע���弰��С
	pClone->m_clrText			= m_clrText;			// ��ɫ
	pClone->m_bShowText			= m_bShowText;		// �Ƿ���ʾ
	
	ASSERT_VALID(pClone);
	
	memcpy(pClone->m_points,	m_points,	 sizeof(CPoint) * m_nPoints);
	//memcpy(pClone->m_geoPoints, m_geoPoints, sizeof(CPoint2D) * m_nPoints);

	for (int i=0; i<m_nPoints; i++)
		pClone->m_geoPoints[i] = CoordinateTransfer(pClone->m_points[i]);

 	if (pLayer != NULL)
	{
		m_pDocument->Do(new CCmdAddDrawObj(m_pDocument, pClone), TRUE);
	}
	
	pClone->Invalidate();
	
	return pClone;
}


////////////////////////////////////////////////////////////////////////////////
//
// ����  : CGeoDrawObj::Serialize
// ����  : void
// ����  : CArchive &ar
// ˵��  : ���л�ͼ��
//
void CGeoPolyObj::Serialize(CArchive &ar)
{
	CGeoDrawObj::Serialize(ar);
	
	ar.SerializeClass(RUNTIME_CLASS(CGeoPolyObj));

	if (ar.IsStoring())
	{
		ar << (WORD) m_nPoints;
		ar << (WORD) m_nAllocPoints;
		int i;
		for ( i = 0;i< m_nPoints; i++)
			ar << m_points[i];
		for ( i = 0;i< m_nPoints; i++)
			ar << m_geoPoints[i];	
		ar << m_eLine;
		ar << m_eFill;
		ar << m_eLine2;
		ar << m_iSpaceWidth;
		ar << (WORD)m_ePolyType;
		ar << m_strText;			// ��ע�ı�
		ar.Write(&m_lfFont, sizeof(LOGFONT));// ��ע���弰��С
		ar << m_clrText;		// ��ɫ
		ar << m_bShowText;		// �Ƿ���ʾ
	}
	else
	{
		// get the document back pointer from the archive

		int nVersion = ar.GetObjectSchema(); // ��ȡ�汾��Ϣ
		switch( nVersion )
		{
		case 0:
			{
				WORD wTemp;	
				ar >> wTemp; m_nPoints = wTemp;
				ar >> wTemp; m_nAllocPoints = wTemp;
				m_points = new CPoint[m_nAllocPoints];
				m_geoPoints	= new CPoint2D[m_nAllocPoints];
				int i;
				for ( i = 0;i < m_nPoints; i++)
					ar >> m_points[i];
				for ( i = 0;i < m_nPoints; i++)
					ar >> m_geoPoints[i];
				ar >> m_eLine;
				ar >> m_eFill;
				ar >> m_eLine2;
				ar >> m_iSpaceWidth;

				ar >> wTemp; m_ePolyType = (POLYTYPE)wTemp;
				ar >> m_strText;			// ��ע�ı�
				ar.Read(&m_lfFont, sizeof(LOGFONT));// ��ע���弰��С
				ar >> m_clrText;		// ��ɫ
				ar >> m_bShowText;		// �Ƿ���ʾ
			}
			break;
		case 1:
			break;
		case 2:
			break;
		default:
			break;
		}
		
	}
}

////////////////////////////////////////////////////////////////////////////////
//
// ����  : CGeoDrawObj::GetHandleCount
// ����  : int
// ˵��  : ��ȡ�ֱ�����
//
int CGeoPolyObj::GetHandleCount()
{
	ASSERT_VALID(this);
	return m_nPoints;
}

////////////////////////////////////////////////////////////////////////////////
//
// ����  : CGeoDrawObj::GetHandle
// ����  : CPoint
// ����  : int nHandle
// ˵��  : ��ȡĳ�ֱ��������߼�����
//
CPoint CGeoPolyObj::GetHandle(int nHandle)
{
	ASSERT_VALID(this);
	
	ASSERT(nHandle >= 1 && nHandle <= m_nPoints);
	CPoint point;
	point = m_points[nHandle - 1];
	return point;
}


////////////////////////////////////////////////////////////////////////////////
//
// ����  : CGeoDrawObj::GetHandleCursor
// ����  : HCURSOR
// ����  : int nHandle
// ˵��  : ��ȡĳ�ֱ������ָ��
//
HCURSOR CGeoPolyObj::GetHandleCursor(int nHandle)
{
	return AfxGetApp()->LoadCursor(IDC_CURSOR_SIZEALL);
}

////////////////////////////////////////////////////////////////////////////////
//
// ����  : CGeoDrawObj::HitTestHanle
// ����  : int
// ����  : CPoint point���߼�����
// ����  : CGeoMapView *pView
// ����  : BOOL bSelected ����Ѿ�ѡ�У������Ƿ����ĳ���ֱ�
// ˵��  : �����������1ѡ�и�ͼ�أ�����û�е��и�ͼ��
//
int CGeoPolyObj::HitTestHandle(CPoint point, CGeoMapView *pView, BOOL bSelected)
{
	ASSERT_VALID(this);
	ASSERT(pView != NULL);
	
	if (bSelected) // �����ͼ���Ѿ���ѡ����
	{
		int nHandleCount = GetHandleCount(); // �ֱ�����
		for (int nHandle = 1; nHandle <= nHandleCount; nHandle += 1)
		{
			// ����ĳ�ֱ����߼���Χ
			CRect rc = GetHandleRect(nHandle,pView);
			rc.NormalizeRect();
			if( rc.PtInRect(point))
				return nHandle;
		}
	}
	else // ����, �����Ƿ�����ͼ�ط�Χ����
	{		
		if( IsPoint(point,pView) )
			return 1;
	}
	return 0;
}

// �������߷����ı�ʱ
void CGeoPolyObj::OnScaleChanged()
{
	CGeoDrawObj::OnScaleChanged();

	for (int i = 0; i < m_nPoints; i += 1)
		m_points[i] = CoordinateTransfer(m_geoPoints[i]);
	
	RecalcBounds();
}

void CGeoPolyObj::ReversePoints()
{
	CPoint pt;
	CPoint2D geoPt;

	for (int i=0;i<m_nPoints/2; i++)
	{
		pt = m_points[i];
		m_points[i] = m_points[m_nPoints-i-1];
		m_points[m_nPoints-i-1] = pt;

		geoPt = m_geoPoints[i];
		m_geoPoints[i] = m_geoPoints[m_nPoints-i-1];
		m_geoPoints[m_nPoints-i-1] = geoPt;
	}
}

// ��������תʱ
void CGeoPolyObj::OnAngleChanged()
{

}
// ����������ԭ���иı�ʱ
void CGeoPolyObj::OnCoordOrigChanged()
{

}

// ת�������ı�ʱ
void CGeoPolyObj::OnConvertParamChanged()
{
	for (int i=0; i<m_nPoints; i++)
		m_geoPoints[i] = CoordinateTransfer(m_points[i]);
}

void CGeoPolyObj::AddPoint(const CPoint2D &point, CGeoMapView *pView)
{
	CPoint pt = CoordinateTransfer(point);

	AddPoint(pt, pView);
	return;
	
	ASSERT_VALID(this);
	
	if( m_bLocked ) 
		return;
	
	if (m_nPoints == m_nAllocPoints)
	{
		CPoint* newPoints = new CPoint[m_nAllocPoints + 10];
		
		if (m_points != NULL )
		{
			memcpy(newPoints, m_points, sizeof(CPoint) * m_nAllocPoints);
			delete[] m_points;
		}
		
		CPoint2D* newGeoPoints = new CPoint2D[m_nAllocPoints + 10];
		if (m_geoPoints != NULL )
		{
			memcpy(newGeoPoints, m_geoPoints, sizeof(CPoint2D) * m_nAllocPoints);
			delete[] m_geoPoints;
		}
		
		m_points = newPoints;
		m_geoPoints = newGeoPoints;
		
		m_nAllocPoints += 10;	
	}

	

	if (m_nPoints == 0 || m_geoPoints[m_nPoints - 1] != point)
	{
		if( m_bEditing )
			DrawEditMode(pView);

		m_geoPoints[m_nPoints] = point;		
		m_points[m_nPoints] = pt;		
		m_nPoints++;
		
		if( m_bEditing )
			DrawEditMode(pView);
		
		m_pDocument->SetModifiedFlag();

		RecalcBounds(pView);
	}
}

void CGeoPolyObj::AddPoint(const CPoint &point, CGeoMapView *pView)
{
	ASSERT_VALID(this);
	
	if( m_bLocked ) 
		return;

	if (m_nPoints == m_nAllocPoints)
	{
		CPoint* newPoints = new CPoint[m_nAllocPoints + 10];
		
		if (m_points != NULL )
		{
			memcpy(newPoints, m_points, sizeof(CPoint) * m_nAllocPoints);
			delete[] m_points;
		}

		CPoint2D* newGeoPoints = new CPoint2D[m_nAllocPoints + 10];
		if (m_geoPoints != NULL )
		{
			memcpy(newGeoPoints, m_geoPoints, sizeof(CPoint2D) * m_nAllocPoints);
			delete[] m_geoPoints;
		}

		m_points = newPoints;
		m_geoPoints = newGeoPoints;

		m_nAllocPoints += 10;	
	}

	if (m_nPoints == 0 || m_points[m_nPoints - 1] != point)
	{
		if( m_bEditing )
			DrawEditMode(pView);
		
		m_points[m_nPoints] = point;

		CPoint2D geoPoint = CoordinateTransfer(point);
		m_geoPoints[m_nPoints] = geoPoint;

		m_nPoints++;
		
		if( m_bEditing )
			DrawEditMode(pView);

		m_pDocument->SetModifiedFlag();

		RecalcBounds(pView);
	}
}

BOOL CGeoPolyObj::RecalcBounds(CGeoMapView *pView)
{
	ASSERT_VALID(this);

	if (m_nPoints == 0)
		return FALSE;
	
	if( pView == NULL )
	{
		POSITION pos = m_pDocument->GetFirstViewPosition();
		pView = (CGeoMapView*)m_pDocument->GetNextView(pos);
	}

	CRect bounds(m_points[0], CSize(1, 1));
	if( m_nPoints == 2)
	{
		bounds.left = m_points[0].x;
		bounds.right =m_points[1].x;
		bounds.top = m_points[0].y;
		bounds.bottom = m_points[1].y;
		if( m_eLine.m_nLineType != 0 )
		{
			bounds.NormalizeRect();
			bounds.DeflateRect(-m_iSpaceWidth,-m_iSpaceWidth,
			-m_iSpaceWidth,-m_iSpaceWidth);
		}		
	}
	else
	{
		if( m_eLine.m_bSmooth )
		{		
			CClientDC dc(pView);
			pView->OnPrepareDC(&dc);
			dc.BeginPath();

			if( m_eLine.m_nLineType == 0 )
			{
				if(m_eLine.m_bClose)
					PolySpline3(dc.m_hDC, m_points, m_nPoints, TRUE);
				else
					PolySpline3(dc.m_hDC, m_points, m_nPoints);				
			}
			else
			{
				if( m_eLine.m_bClose )//���
				{
					ParallelCurve(dc.m_hDC, m_points, m_nPoints, TRUE, TRUE, (double)m_iSpaceWidth);
					ParallelCurve(dc.m_hDC, m_points, m_nPoints, TRUE, FALSE, (double)m_iSpaceWidth);
				}
				else//�����
				{
					ParallelCurve(dc.m_hDC, m_points, m_nPoints, FALSE, TRUE, (double)m_iSpaceWidth);
					ParallelCurve(dc.m_hDC, m_points, m_nPoints, FALSE, FALSE, (double)m_iSpaceWidth);
				}
			}
			
			dc.EndPath();			

			CRgn rgn;
			if(rgn.CreateFromPath(&dc))
			{
				CRect rt;
				rgn.GetRgnBox(&bounds);
				rgn.DeleteObject();
				pView->ClientToDoc(bounds);
			}			
		}
		else
		{	
			if( m_eLine.m_nLineType == 0 )
			{
				for (int i = 1; i < m_nPoints; ++i)
				{
					if (m_points[i].x < bounds.left)
						bounds.left = m_points[i].x;
					if (m_points[i].x > bounds.right)
						bounds.right = m_points[i].x;
					if (m_points[i].y < bounds.top)
						bounds.top = m_points[i].y;
					if (m_points[i].y > bounds.bottom)
						bounds.bottom = m_points[i].y;
				}
			}
			else
			{
				CClientDC dc(pView);
				pView->OnPrepareDC(&dc);
				dc.BeginPath();

				if( m_eLine.m_bClose )//���
				{	
					Parallelgon(dc.m_hDC, m_points, m_nPoints, TRUE, (double)m_iSpaceWidth);
					Parallelgon(dc.m_hDC, m_points, m_nPoints, FALSE, (double)m_iSpaceWidth);
				}
				else//�����
				{			
					Parallelline(dc.m_hDC,m_points, m_nPoints, TRUE, (double)m_iSpaceWidth);
					Parallelline(dc.m_hDC,m_points, m_nPoints, FALSE, (double)m_iSpaceWidth);
				}

				dc.EndPath();
				
				CRgn rgn;
				if(rgn.CreateFromPath(&dc))
				{
					CRect rt;
					rgn.GetRgnBox(&bounds);
					rgn.DeleteObject();
					pView->ClientToDoc(bounds);
				}			
			}
		}
	}

	if (bounds == m_rtPosition)
		return FALSE;
	
	m_rtPosition = bounds;

	if( m_rtPosition.left == m_rtPosition.right ) // ����Ǵ�ֱֱ�߶�
	{
		m_rtPosition.left -= 1;
		m_rtPosition.right += 1; 
	}

	if( m_rtPosition.top == m_rtPosition.bottom ) // �����ˮƽֱ�߶�
	{
		m_rtPosition.bottom -= 1;
		m_rtPosition.top += 1; 
	}
	
	return CGeoDrawObj::RecalcBounds(pView);
}

BOOL CGeoPolyObj::IsPoint(CPoint point, CGeoMapView *pView)
{
	if(!m_bShow)
		return FALSE;
	
	CRect cliRect;
	pView->GetClientRect(cliRect);
	pView->ClientToDoc(cliRect);

	if( !Intersects(cliRect) ) //�����ͼ�ز��ڵ�ǰ����������ʾ
		return FALSE;

	CClientDC dc(pView);
	pView->OnPrepareDC(&dc);
	
	CSize size(20,20);
	dc.DPtoLP(&size);
	dc.LPtoDP(&point);
	
	CPen pen1(PS_GEOMETRIC | PS_SOLID | PS_JOIN_MITER | PS_ENDCAP_FLAT , size.cx , RGB(0, 0, 0) );
	
	CPen *pOldPen = dc.SelectObject(&pen1);	
	
	dc.BeginPath();	
	
	if (!m_eLine.m_bSmooth)
	{
		if(m_eLine.m_bClose)
			dc.Polygon(m_points, m_nPoints);
		else
			dc.Polyline(m_points, m_nPoints);
	}
	else
	{
		if(m_eLine.m_bClose)
			PolySpline3(dc.m_hDC, m_points, m_nPoints, TRUE);
		else
			PolySpline3(dc.m_hDC, m_points, m_nPoints);
	}
	
	dc.EndPath();
	
	dc.SelectObject(pOldPen);
	pen1.DeleteObject();
	
	if( !m_eFill.m_bShow )
		dc.WidenPath();
		 
	if(m_rgn.m_hObject != NULL )
		m_rgn.DeleteObject();

	m_rgn.CreateFromPath(&dc);
	
	if( !m_rgn.m_hObject )
		return FALSE;
	
	if( m_rgn.PtInRegion(point) )
		return TRUE;
	else
		return FALSE;
}

BOOL CGeoPolyObj::IsHitOnBorder(CPoint point, CGeoMapView *pView)
{
	if(!m_bShow)
		return FALSE;

	CRect cliRect;
	pView->GetClientRect(cliRect);
	pView->ClientToDoc(cliRect);
	if( !Intersects(cliRect) ) //�����ͼ�ز��ڵ�ǰ����������ʾ
		return FALSE;
	
	CClientDC dc(pView);
	pView->OnPrepareDC(&dc);
	
	CSize size(20,20);
	dc.DPtoLP(&size);
	dc.LPtoDP(&point);
	
	CPen pen1(PS_GEOMETRIC | PS_SOLID | PS_JOIN_MITER | PS_ENDCAP_FLAT , size.cx , RGB(0, 0, 0) );
	
	CPen *pOldPen = dc.SelectObject(&pen1);	
	
	dc.BeginPath();	
	
	if (!m_eLine.m_bSmooth)
	{
		if(m_eLine.m_bClose)
			dc.Polygon(m_points, m_nPoints);
		else
			dc.Polyline(m_points, m_nPoints);
	}
	else
	{
		if(m_eLine.m_bClose)
			PolySpline3(dc.m_hDC, m_points, m_nPoints, TRUE);
		else
			PolySpline3(dc.m_hDC, m_points, m_nPoints);
	}
	
	dc.EndPath();
	
	dc.SelectObject(pOldPen);
	pen1.DeleteObject();
	
	dc.WidenPath();
	
	CRgn rgn;
	
	rgn.CreateFromPath(&dc);
	
	if( !rgn.m_hObject )
		return FALSE;
	
	if( rgn.PtInRegion(point) )
		return TRUE;
	else
		return FALSE;
}
//////////////////////////////////////////////////////////////////////////
// ɾ����
// nIndex ����� ��һ��������1
// 

void CGeoPolyObj::DelPoint( int nIndex, CGeoMapView *pView)
{
	if( m_bLocked )
		return;
	
	if( m_nPoints == 2)
	{
		m_pDocument->Do(new CCmdRemoveDrawObj(m_pDocument,this), TRUE);
		CToolBase::c_drawShape = CToolBase::selection;
	}
	else
	{
		CPoint point = m_points[nIndex-1];
		m_pDocument->Do(new CCmdObjDelPt(m_pDocument, this, nIndex, point, pView), TRUE);
	}
}

//////////////////////////////////////////////////////////////////////////
// ɾ����
// nIndex ����� ��һ��������1
// 
void CGeoPolyObj::DeletePoint( int nIndex, CGeoMapView *pView)
{	
	for(int i = nIndex ; i < m_nPoints; i++)
	{
		m_points[i-1] = m_points[i]; 
		m_geoPoints[i-1] = m_geoPoints[i]; 
	}
	
	m_nPoints --;	
	
	pView->InvalObj(this);

	RecalcBounds(pView);

	pView->InvalObj(this);
	
	m_pDocument->SetModifiedFlag();
}

void CGeoPolyObj::DelPoint( const CPoint &point, CGeoMapView *pView)
{
	int index[2];
	
	CPoint2D pt(point.x, point.y);
	CPoint2D retPt;

	CPoint2D *points = new CPoint2D [m_nPoints];
	if( points == NULL )
		return;
	
	for (int i=0;i<m_nPoints;i++)
		points[i] = m_points[i];

	ptopointset(m_nPoints,points,pt,retPt,index);

	delete[]points;
	if( index[0]==-1
		&& index[1]==-1)
		return;
	
	if( index[0] == -1 )
	{
		if(index[1] == 0 ) 
			index[1] = 1;
	}
	else
	{
		CPoint2D p1,p2;
		p1 = m_points[index[0]];		
		p2 = m_points[index[1]];
		
		double d1 = dist(p1,retPt);
		double d2 = dist(p2,retPt);
		
		if( d1<d2)
			index[1] = index[0];
		
		index[1] +=1;
	}

	DelPoint(index[1],pView);
}

void CGeoPolyObj::MovePointTo(const CPoint &point, CGeoMapView *pView)
{
	int index[2];
	
	CPoint2D pt(point.x, point.y);
	CPoint2D retPt;
	CPoint2D *points = new CPoint2D [m_nPoints];
	if( points == NULL )
		return;
	
	for (int i=0;i<m_nPoints;i++)
		points[i] = m_points[i];

	ptopointset(m_nPoints,points,pt,retPt,index);

	delete[]points;

	if( index[0]==-1&&index[1]==-1)
		return;

	if( index[0] == -1 )
	{
		if(index[1] == 0 ) 
			index[1] = 1;	
	}
	else
	{
		CPoint2D p1,p2;
		p1 = m_points[index[0]];		
		p2 = m_points[index[1]];

		double d1 = dist(p1,retPt);
		double d2 = dist(p2,retPt);
		
		if( d1<d2)
			index[1] = index[0];

		index[1] +=1;
	}
	
	m_pDocument->Do(new CCmdObjMoveHandle(m_pDocument, this, index[1], point, m_points[index[1]-1], pView), TRUE);

	pView->InvalObj(this);
	
	RecalcBounds(pView);
	
	pView->InvalObj(this);
}

// 
void CGeoPolyObj::InsertPoint(const CPoint &point, CGeoMapView *pView)
{
	int index[2];
	
	CPoint2D pt(point.x, point.y);
	CPoint2D retPt;
	CPoint2D *points = new CPoint2D [m_nPoints];
	if( points == NULL )
		return;
	
	for (int i=0;i<m_nPoints;i++)
		points[i] = m_points[i];

	ptopointset(m_nPoints,points,pt,retPt,index);

	delete[]points;

	if( index[0]==-1&&index[1]==-1)
		return;
	
	m_pDocument->Do(new CCmdObjInsPt(m_pDocument, this, index[1]+1, point, pView), TRUE);	
}

void CGeoPolyObj::InsertPoint(const CPoint &point, int nIndex, CGeoMapView *pView)
{
	if (m_nPoints == m_nAllocPoints)
	{
		CPoint* newPoints = new CPoint[m_nAllocPoints + 10];
		
		if (m_points != NULL)
		{
			memcpy(newPoints, m_points, sizeof(CPoint) * m_nAllocPoints);
			delete[] m_points;
		}

		CPoint2D* newGeoPoints = new CPoint2D[m_nAllocPoints + 10];
		if (m_geoPoints != NULL )
		{
			memcpy(newGeoPoints, m_geoPoints, sizeof(CPoint2D) * m_nAllocPoints);
			delete[] m_geoPoints;
		}

		m_points = newPoints;
		m_geoPoints = newGeoPoints;

		m_nAllocPoints += 10;
	}
	
	for(int i = m_nPoints ; i >= nIndex ; i--)
	{
		m_points[i] = m_points[i-1]; 
		m_geoPoints[i] = m_geoPoints[i-1]; 
	}
	
	m_points[nIndex-1] = point;

	CPoint2D geoPoint = CoordinateTransfer(point);
	m_geoPoints[nIndex-1] = geoPoint;
	
	m_nPoints ++;
	
	pView->InvalObj(this);

	RecalcBounds(pView);

	pView->InvalObj(this);
	
	m_pDocument->SetModifiedFlag();
}

void CGeoPolyObj::DrawFill(CDC *pDC, CGeoMapView *pView, PLACE place)
{	
	if( !m_eFill.m_bShow  )
		return;

	CPen* pOldPen;	

	if( m_eLine.m_bClose)//������
	{
		CBrush brush;
		CBrush* pOldBrush;		
		
		if(! CreateBrush(&brush, pDC) )
			return;	
		if(m_rgn.m_hObject && m_eFill.blendFunction.SourceConstantAlpha != 255)
		{	
			CRect rect = m_rtPosition;
			rect.NormalizeRect();
			SelectClipRgn(pDC);

			pDC->SetBkColor(m_eFill.sFillStyle.m_PatternFill.m_crColourBack);
			pDC->SetTextColor(m_eFill.sFillStyle.m_PatternFill.m_crColourFore);			

			CRect rt;
			rt = rect;
			pView->DocToClient(rt);
			rt.NormalizeRect();

			CDC MemDC;
			MemDC.CreateCompatibleDC(pDC);
			CBitmap bit;
			bit.CreateBitmap(rt.Width(), rt.Height(), 1, 32, NULL);
			CBitmap *pOldBit = MemDC.SelectObject(&bit);
			CRect FillRt(0, 0, 0, 0);
			FillRt.right = rt.Width();
			FillRt.bottom = rt.Height();

			MemDC.SetBkColor(m_eFill.sFillStyle.m_PatternFill.m_crColourBack);
			MemDC.SetTextColor(m_eFill.sFillStyle.m_PatternFill.m_crColourFore);
			
			MemDC.FillRect(FillRt, &brush);
			AlphaBlend(pDC->m_hDC, rect.left, rect.top, rect.Width(), rect.Height(), MemDC.m_hDC, 0, 0, FillRt.Width(), FillRt.Height(), m_eFill.blendFunction);
			MemDC.SelectObject(pOldBit);
			bit.DeleteObject();
			MemDC.DeleteDC();
			pDC->SelectClipRgn(NULL);
		}
		else
		{
			pOldBrush = pDC->SelectObject(&brush);		
			pOldPen = (CPen*)pDC->SelectStockObject(NULL_PEN);	
			
			if (!m_eLine.m_bSmooth)
			{
				pDC->Polygon(m_points, m_nPoints);
			}
			else
			{
				pDC->BeginPath();
				PolySpline3(pDC->m_hDC, m_points, m_nPoints, TRUE);
				pDC->EndPath();
				pDC->FillPath();
			}
			
			pDC->SelectObject(pOldBrush);
			pDC->SelectObject(pOldPen);	
		}
	}
	else
	{
		CBrush brush;
		CBrush* pOldBrush;		
		
		if(! CreateBrush(&brush, pDC) )
			return;	
		
		if( m_eFill.blendFunction.SourceConstantAlpha == 255 )
		{
			pOldBrush = pDC->SelectObject(&brush);		
			pOldPen = (CPen*)pDC->SelectStockObject(NULL_PEN);	
			
			if (!m_eLine.m_bSmooth )
			{
				pDC->Polygon(m_points, m_nPoints);
			}
			else
			{
				pDC->BeginPath();
				PolySpline3(pDC->m_hDC, m_points, m_nPoints, FALSE);
				pDC->EndPath();
				pDC->FillPath();
			}
			pDC->SelectObject(pOldBrush);
			pDC->SelectObject(pOldPen);			
		}
		else
		{
			pDC->BeginPath();
			
			if (!m_eLine.m_bSmooth)
			{				
				pDC->Polygon(m_points, m_nPoints);
			}
			else
				PolySpline3(pDC->m_hDC, m_points, m_nPoints, FALSE);			
			
			pDC->EndPath();

			pDC->SelectClipPath(RGN_COPY);

			CRect rect = m_rtPosition;
			rect.NormalizeRect();
			
			pDC->SetBkColor(m_eFill.sFillStyle.m_PatternFill.m_crColourBack);
			pDC->SetTextColor(m_eFill.sFillStyle.m_PatternFill.m_crColourFore);			
			
			CRect rt;
			rt = rect;
			pView->DocToClient(rt);
			rt.NormalizeRect();
			
			CDC MemDC;
			MemDC.CreateCompatibleDC(pDC);
			CBitmap bit;
			bit.CreateBitmap(rt.Width(), rt.Height(), 1, 32, NULL);
			CBitmap *pOldBit = MemDC.SelectObject(&bit);
			CRect FillRt(0, 0, 0, 0);
			FillRt.right = rt.Width();
			FillRt.bottom = rt.Height();
			
			MemDC.SetBkColor(m_eFill.sFillStyle.m_PatternFill.m_crColourBack);
			MemDC.SetTextColor(m_eFill.sFillStyle.m_PatternFill.m_crColourFore);
			
			MemDC.FillRect(FillRt, &brush);
			AlphaBlend(pDC->m_hDC, rect.left, rect.top, rect.Width(), rect.Height(), MemDC.m_hDC, 0, 0, FillRt.Width(), FillRt.Height(), m_eFill.blendFunction);
			MemDC.SelectObject(pOldBit);
			bit.DeleteObject();
			MemDC.DeleteDC();
			pDC->SelectClipRgn(NULL);
		}
	}
}

void CGeoPolyObj::DrawLine(CDC *pDC, CGeoMapView *pView, PLACE place, BOOL bThin)
{
	CPen pen;
	CBrush* pOldBrush;
	CPen* pOldPen;	
	
	pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
	
	if( m_eLine.m_bShow )
	{
		if(! CreatePen(&pen, pDC, bThin) )
			return;
		pOldPen = pDC->SelectObject(&pen);
	}
	else
		pOldPen = (CPen*)pDC->SelectStockObject(NULL_PEN);
	
	if (!m_eLine.m_bSmooth)//���⻬
	{
		if( m_eLine.m_bClose )//���
		{	
			if(place == MIDST )
				pDC->Polygon(m_points, m_nPoints);
			else
				Parallelgon(pDC->m_hDC, m_points, m_nPoints, place == INNER? TRUE: FALSE, (double)m_iSpaceWidth);
		}
		else//�����
		{			
			if(place == MIDST )
				pDC->Polyline(m_points, m_nPoints);
			else
				Parallelline(pDC->m_hDC,m_points, m_nPoints, place == INNER? TRUE: FALSE, (double)m_iSpaceWidth);
		}
	}
	//*******************
	else//�⻬
	{
		if( m_eLine.m_bClose )//���
		{
			if(place == MIDST )
			{
				if( m_ePolyType == contour )
					SPLine2(pDC->m_hDC, m_points, m_nPoints, TRUE);
				else
				{					
					long nSize = GetSpline3Pointf(m_geoPoints,m_nPoints);

					if( nSize > 0 )
					{
						CPoint2D *pt = new CPoint2D[nSize];
						GetSpline3Pointf(m_geoPoints,m_nPoints,FALSE,pt);
						CPoint *points = new CPoint[nSize];
						for (long i=0;i<nSize; i++)
						{
							points[i] = CoordinateTransfer(pt[i]);
						}

						pDC->Polyline(points,nSize);
						delete[] pt;
						delete[]points;
					}
					//PolySpline3(pDC->m_hDC, m_points, m_nPoints, TRUE);
				}
			}
			else
				ParallelCurve(pDC->m_hDC, m_points, m_nPoints, TRUE, place == INNER? TRUE: FALSE, (double)m_iSpaceWidth);
		}
		else//�����
		{
			if(place == MIDST )
			{
				if( m_ePolyType == contour )
					SPLine2(pDC->m_hDC, m_points, m_nPoints);
				else
				{

					long nSize = GetSpline3Pointf(m_geoPoints,m_nPoints);

					if( nSize > 0 )
					{
						CPoint2D *pt = new CPoint2D[nSize];
						GetSpline3Pointf(m_geoPoints,m_nPoints,FALSE,pt);
						CPoint *points = new CPoint[nSize];
						for (long i=0;i<nSize; i++)
						{
							points[i] = CoordinateTransfer(pt[i]);
						}

						pDC->Polyline(points,nSize);
						delete[] pt;
						delete[]points;
					}
					
					//PolySpline3(pDC->m_hDC, m_points, m_nPoints);

				}
			}
			else
				ParallelCurve(pDC->m_hDC, m_points, m_nPoints, FALSE, place == INNER? TRUE: FALSE, (double)m_iSpaceWidth);
		}
	}
	
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);	
}


void CGeoPolyObj::DrawLineSpace(CDC *pDC, CGeoMapView *pView)
{
	if( !m_eLine.eLineSpaceFill.m_bShow )
		return;

	CPen pen;
	CBrush* pOldBrush;
	CPen* pOldPen;	
	
	pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
	
	if( m_eLine.m_bShow )
	{
		if(! CreatePenLine(&pen, pDC) )
		{
			pDC->SelectObject(pOldBrush);
			return;
		}
		pOldPen = pDC->SelectObject(&pen);
	}
	else
		pOldPen = (CPen*)pDC->SelectStockObject(NULL_PEN);
	
	if (!m_eLine.m_bSmooth)//���⻬
	{
		if( m_eLine.m_bClose )//���
			pDC->Polygon(m_points, m_nPoints);
		else//�����
			pDC->Polyline(m_points, m_nPoints);	
	}
	//*******************
	else//�⻬
	{
		if( m_eLine.m_bClose )//���
			PolySpline3(pDC->m_hDC, m_points, m_nPoints, TRUE);
			
		else//�����
			PolySpline3(pDC->m_hDC, m_points, m_nPoints);				
	}
		
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);	

}

void CGeoPolyObj::DrawArrow(CDC *pDC, CGeoMapView *pView)
{
	if( m_eLine.m_nLineType ) return;//���ǵ��߲�����ͷ

	CPoint point;//
	CPoint pt[4];//��ͷ����

	CPen pen;
	pen.CreatePen(PS_SOLID, 0, m_eLine.s_LineStyle.m_PatternFill.m_crColourFore);

	CPen *pOldPen = pDC->SelectObject(&pen);

	CBrush b1(RGB(255, 255, 255));
	CBrush b2(m_eLine.s_LineStyle.m_PatternFill.m_crColourFore);

	CRgn rgn;	
	
	if( m_eLine.m_uBeginPointMode )
	{
		point.x = m_points[0].x;
		point.y = m_points[0].y;
		rgn.DeleteObject();

		switch(m_eLine.m_uBeginPointMode)
		{
		case 1:								//��ͷ
			{
				CalcArrowRgn(pt, FALSE);
				CPoint p[3];
				p[0] = point;
				p[1] = pt[2];
				p[2] = pt[3];				
				rgn.CreatePolygonRgn(p, 3, ALTERNATE);
				pDC->FillRgn(&rgn, &b2);
				pDC->MoveTo(point);
				pDC->LineTo(pt[2]);
				pDC->LineTo(pt[3]);
				pDC->LineTo(point);			
			}
			break;
		case 2:								//�����ͼ�ͷ
			CalcArrowRgn(pt, FALSE);
			pDC->MoveTo(point);
			pDC->LineTo(pt[2]);
			pDC->MoveTo(point);	
			pDC->LineTo(pt[3]);			
			break;
		case 3:								//��β��ͷ
			{
				CalcArrowRgn(pt, FALSE);
				CPoint p[4];
				
				p[0] = point;

				p[1] = pt[2];
				
				p[2].x = (pt[2].x + pt[3].x)/4 + point.x/2 ;
				p[2].y = (pt[2].y + pt[3].y)/4 + point.y/2 ;

				p[3] = pt[3];
				
				rgn.CreatePolygonRgn(p, 4, ALTERNATE);
				pDC->FillRgn(&rgn, &b2);
				pDC->MoveTo(p[0]);
				pDC->LineTo(p[1]);
				pDC->LineTo(p[2]);
				pDC->LineTo(p[3]);
				pDC->LineTo(p[0]);
			}
			break;
		case 4:								//��ʯ��ͷ
			{
				CalcArrowRgn(pt, FALSE);
				CPoint p[4];
				
				p[0].x = ( pt[0].x + pt[1].x * 0.4 ) / 1.4;
				p[0].y = ( pt[0].y + pt[1].y * 0.4 ) / 1.4;

				p[1].x = ( pt[0].x + pt[1].x * 2.5 ) / 3.5;
				p[1].y = ( pt[0].y + pt[1].y * 2.5 ) / 3.5;

				p[2].x = ( pt[2].x + pt[3].x * 0.4 ) / 1.4;
				p[2].y = ( pt[2].y + pt[3].y * 0.4 ) / 1.4;

				p[3].x = ( pt[2].x + pt[3].x * 2.5 ) / 3.5;
				p[3].y = ( pt[2].y + pt[3].y * 2.5 ) / 3.5;

				CPoint pp[4];

				pp[0] = point;

				pp[1].x = (p[1].x + p[2].x)/2;
				pp[1].y = (p[1].y + p[2].y)/2;

				pp[2].x = (p[2].x + p[3].x)/2;
				pp[2].y = (p[2].y + p[3].y)/2;

				pp[3].x = (p[3].x + p[0].x)/2;
				pp[3].y = (p[3].y + p[0].y)/2;
				
				rgn.CreatePolygonRgn(pp, 4, ALTERNATE);
				pDC->FillRgn(&rgn, &b2);
				pDC->MoveTo(pp[0]);
				pDC->LineTo(pp[1]);
				pDC->LineTo(pp[2]);
				pDC->LineTo(pp[3]);
				pDC->LineTo(pp[0]);
			}
			break;
		case 5:								//Բ�ͼ�ͷ
			{
				CalcArrowRgn(pt, FALSE);
				CPoint p;
				p.x = ( pt[0].x + pt[2].x )/2;
				p.y = ( pt[0].y + pt[2].y )/2;
				CBrush *pOldBrush = pDC->SelectObject(&b2);
				pDC->Ellipse(p.x - m_eLine.m_uBeginPointSize/2, p.y - + m_eLine.m_uBeginPointSize/2, p.x + m_eLine.m_uBeginPointSize/2, p.y + m_eLine.m_uBeginPointSize/2);
				pDC->SelectObject(pOldBrush);
			}			
			break;
		case 6:								//���ļ�ͷ
			{
				CalcArrowRgn(pt, FALSE);
				CPoint p[3];
				p[0] = point;
				p[1] = pt[2];
				p[2] = pt[3];
				rgn.CreatePolygonRgn(p, 3, ALTERNATE);
				pDC->FillRgn(&rgn, &b1);
				pDC->MoveTo(point);
				pDC->LineTo(pt[2]);
				pDC->LineTo(pt[3]);
				pDC->LineTo(point);			
			}
			break;
		case 7:								//������β��ͷ
			{
				CalcArrowRgn(pt, FALSE);
				CPoint p[4];
				
				p[0] = point;

				p[1] = pt[2];
				
				p[2].x = (pt[2].x + pt[3].x)/4 + point.x/2 ;
				p[2].y = (pt[2].y + pt[3].y)/4 + point.y/2 ;

				p[3] = pt[3];
				
				rgn.CreatePolygonRgn(p, 4, ALTERNATE);
				pDC->FillRgn(&rgn, &b1);
				pDC->MoveTo(p[0]);
				pDC->LineTo(p[1]);
				pDC->LineTo(p[2]);
				pDC->LineTo(p[3]);
				pDC->LineTo(p[0]);
			}
			break;
		case 8:								//������ʯ��ͷ
			{
				CalcArrowRgn(pt, FALSE);
				CPoint p[4];
				
				p[0].x = ( pt[0].x + pt[1].x * 0.4 ) / 1.4;
				p[0].y = ( pt[0].y + pt[1].y * 0.4 ) / 1.4;

				p[1].x = ( pt[0].x + pt[1].x * 2.5 ) / 3.5;
				p[1].y = ( pt[0].y + pt[1].y * 2.5 ) / 3.5;

				p[2].x = ( pt[2].x + pt[3].x * 0.4 ) / 1.4;
				p[2].y = ( pt[2].y + pt[3].y * 0.4 ) / 1.4;

				p[3].x = ( pt[2].x + pt[3].x * 2.5 ) / 3.5;
				p[3].y = ( pt[2].y + pt[3].y * 2.5 ) / 3.5;

				CPoint pp[4];

				pp[0] = point;

				pp[1].x = (p[1].x + p[2].x)/2;
				pp[1].y = (p[1].y + p[2].y)/2;

				pp[2].x = (p[2].x + p[3].x)/2;
				pp[2].y = (p[2].y + p[3].y)/2;

				pp[3].x = (p[3].x + p[0].x)/2;
				pp[3].y = (p[3].y + p[0].y)/2;
				
				rgn.CreatePolygonRgn(pp, 4, ALTERNATE);
				pDC->FillRgn(&rgn, &b1);
				pDC->MoveTo(pp[0]);
				pDC->LineTo(pp[1]);
				pDC->LineTo(pp[2]);
				pDC->LineTo(pp[3]);
				pDC->LineTo(pp[0]);
			}
			break;
		case 9:								//����Բ�ͼ�ͷ
			{
				CalcArrowRgn(pt, FALSE);
				CPoint p;
				p.x = ( pt[0].x + pt[2].x )/2;
				p.y = ( pt[0].y + pt[2].y )/2;
				CBrush *pOldBrush = pDC->SelectObject(&b1);
				pDC->Ellipse(p.x - m_eLine.m_uBeginPointSize/2, p.y - + m_eLine.m_uBeginPointSize/2, p.x + m_eLine.m_uBeginPointSize/2, p.y + m_eLine.m_uBeginPointSize/2);
				pDC->SelectObject(pOldBrush);
			}
			
			break;
		default:
			break;
		}
	}
	
	if( m_eLine.m_uEndPointMode )
	{
		point.x = m_points[ m_nPoints - 1 ].x;
		point.y = m_points[ m_nPoints - 1 ].y;
		rgn.DeleteObject();

		switch(m_eLine.m_uEndPointMode)
		{
		case 1:								//��ͷ
			{
				CalcArrowRgn(pt);
				CPoint p[3];
				p[0] = point;
				p[1] = pt[2];
				p[2] = pt[3];
				rgn.CreatePolygonRgn(p, 3, ALTERNATE);
				pDC->FillRgn(&rgn, &b2);
				pDC->MoveTo(point);
				pDC->LineTo(pt[2]);
				pDC->LineTo(pt[3]);
				pDC->LineTo(point);			
			}
			break;
		case 2:								//�����ͼ�ͷ
			CalcArrowRgn(pt);
			pDC->MoveTo(point);
			pDC->LineTo(pt[2]);
			pDC->MoveTo(point);	
			pDC->LineTo(pt[3]);			
			break;
		case 3:								//��β��ͷ
			{
				CalcArrowRgn(pt);
				CPoint p[4];
				
				p[0] = point;

				p[1] = pt[2];
				
				p[2].x = (pt[2].x + pt[3].x)/4 + point.x/2 ;
				p[2].y = (pt[2].y + pt[3].y)/4 + point.y/2 ;

				p[3] = pt[3];
				
				rgn.CreatePolygonRgn(p, 4, ALTERNATE);
				pDC->FillRgn(&rgn, &b2);
				pDC->MoveTo(p[0]);
				pDC->LineTo(p[1]);
				pDC->LineTo(p[2]);
				pDC->LineTo(p[3]);
				pDC->LineTo(p[0]);
			}
			break;
		case 4:								//��ʯ��ͷ
			{
				CalcArrowRgn(pt);
				CPoint p[4];
				
				p[0].x = ( pt[0].x + pt[1].x * 0.4 ) / 1.4;
				p[0].y = ( pt[0].y + pt[1].y * 0.4 ) / 1.4;

				p[1].x = ( pt[0].x + pt[1].x * 2.5 ) / 3.5;
				p[1].y = ( pt[0].y + pt[1].y * 2.5 ) / 3.5;

				p[2].x = ( pt[2].x + pt[3].x * 0.4 ) / 1.4;
				p[2].y = ( pt[2].y + pt[3].y * 0.4 ) / 1.4;

				p[3].x = ( pt[2].x + pt[3].x * 2.5 ) / 3.5;
				p[3].y = ( pt[2].y + pt[3].y * 2.5 ) / 3.5;

				CPoint pp[4];

				pp[0] = point;

				pp[1].x = (p[1].x + p[2].x)/2;
				pp[1].y = (p[1].y + p[2].y)/2;

				pp[2].x = (p[2].x + p[3].x)/2;
				pp[2].y = (p[2].y + p[3].y)/2;

				pp[3].x = (p[3].x + p[0].x)/2;
				pp[3].y = (p[3].y + p[0].y)/2;
				
				rgn.CreatePolygonRgn(pp, 4, ALTERNATE);
				pDC->FillRgn(&rgn, &b2);
				pDC->MoveTo(pp[0]);
				pDC->LineTo(pp[1]);
				pDC->LineTo(pp[2]);
				pDC->LineTo(pp[3]);
				pDC->LineTo(pp[0]);
			}
			break;
		case 5:								//Բ�ͼ�ͷ
			{
				CalcArrowRgn(pt);
				CPoint p;
				p.x = ( pt[0].x + pt[2].x )/2;
				p.y = ( pt[0].y + pt[2].y )/2;
				CBrush *pOldBrush = pDC->SelectObject(&b2);
				pDC->Ellipse(p.x - m_eLine.m_uBeginPointSize/2, p.y - + m_eLine.m_uBeginPointSize/2, p.x + m_eLine.m_uBeginPointSize/2, p.y + m_eLine.m_uBeginPointSize/2);
				pDC->SelectObject(pOldBrush);
			}
			
			break;
		case 6:								//���ļ�ͷ
			{
				CalcArrowRgn(pt);
				CPoint p[3];
				p[0] = point;
				p[1] = pt[2];
				p[2] = pt[3];
				rgn.CreatePolygonRgn(p, 3, ALTERNATE);
				pDC->FillRgn(&rgn, &b1);
				pDC->MoveTo(point);
				pDC->LineTo(pt[2]);
				pDC->LineTo(pt[3]);
				pDC->LineTo(point);			
			}
			break;
		case 7:								//������β��ͷ
			{
				CalcArrowRgn(pt);
				CPoint p[4];
				
				p[0] = point;

				p[1] = pt[2];
				
				p[2].x = (pt[2].x + pt[3].x)/4 + point.x/2 ;
				p[2].y = (pt[2].y + pt[3].y)/4 + point.y/2 ;

				p[3] = pt[3];
				
				rgn.CreatePolygonRgn(p, 4, ALTERNATE);
				pDC->FillRgn(&rgn, &b1);
				pDC->MoveTo(p[0]);
				pDC->LineTo(p[1]);
				pDC->LineTo(p[2]);
				pDC->LineTo(p[3]);
				pDC->LineTo(p[0]);
			}
			break;
		case 8:								//������ʯ��ͷ
			{
				CalcArrowRgn(pt);
				CPoint p[4];
				
				p[0].x = ( pt[0].x + pt[1].x * 0.4 ) / 1.4;
				p[0].y = ( pt[0].y + pt[1].y * 0.4 ) / 1.4;

				p[1].x = ( pt[0].x + pt[1].x * 2.5 ) / 3.5;
				p[1].y = ( pt[0].y + pt[1].y * 2.5 ) / 3.5;

				p[2].x = ( pt[2].x + pt[3].x * 0.4 ) / 1.4;
				p[2].y = ( pt[2].y + pt[3].y * 0.4 ) / 1.4;

				p[3].x = ( pt[2].x + pt[3].x * 2.5 ) / 3.5;
				p[3].y = ( pt[2].y + pt[3].y * 2.5 ) / 3.5;

				CPoint pp[4];

				pp[0] = point;

				pp[1].x = (p[1].x + p[2].x)/2;
				pp[1].y = (p[1].y + p[2].y)/2;

				pp[2].x = (p[2].x + p[3].x)/2;
				pp[2].y = (p[2].y + p[3].y)/2;

				pp[3].x = (p[3].x + p[0].x)/2;
				pp[3].y = (p[3].y + p[0].y)/2;
				
				rgn.CreatePolygonRgn(pp, 4, ALTERNATE);
				pDC->FillRgn(&rgn, &b1);
				pDC->MoveTo(pp[0]);
				pDC->LineTo(pp[1]);
				pDC->LineTo(pp[2]);
				pDC->LineTo(pp[3]);
				pDC->LineTo(pp[0]);
			}
			break;
		case 9:								//����Բ�ͼ�ͷ
			{
				CalcArrowRgn(pt);
				CPoint p;
				p.x = ( pt[0].x + pt[2].x )/2;
				p.y = ( pt[0].y + pt[2].y )/2;
				CBrush *pOldBrush = pDC->SelectObject(&b1);
				pDC->Ellipse(p.x - m_eLine.m_uBeginPointSize/2, p.y - + m_eLine.m_uBeginPointSize/2, p.x + m_eLine.m_uBeginPointSize/2, p.y + m_eLine.m_uBeginPointSize/2);
				pDC->SelectObject(pOldBrush);
			}
			
			break;
		default:
			break;
		}
	}

	pDC->SelectObject(pOldPen);

	pen.DeleteObject();
	b1.DeleteObject();
	b2.DeleteObject();	
	rgn.DeleteObject();

	return ;
}

void CGeoPolyObj::CalcArrowRgn(CPoint *pt, BOOL bEnd)
{
	double Len;
	CPoint point[3];
	if( m_eLine.m_bSmooth && m_nPoints > 2 ) //�⻬�ҵ�������2����
	{		
		if( bEnd )//ĩ��
		{
			Len = m_eLine.m_uEndPointSize;
			point[0].x = m_points[m_nPoints-1].x;
			point[0].y = m_points[m_nPoints-1].y;
		}
		else
		{
			Len = m_eLine.m_uBeginPointSize;
			point[0].x = m_points[0].y;
			point[0].y = m_points[0].y;			
		}
		
		CalcArrowRgnSpline(m_points, m_nPoints, Len, point, bEnd);
		
	}
	else
	{		
		//���⻬
		if( bEnd )//ĩ��
		{		
			Len = m_eLine.m_uEndPointSize;
			point[0].x = m_points[m_nPoints-1].x;
			point[0].y = m_points[m_nPoints-1].y;
			
			int i;
			for( i = m_nPoints - 2; i >= 0 ; i-- )
			{
				if( (m_points[m_nPoints-1].x == m_points[i].x) && (m_points[m_nPoints-1].y == m_points[i].y ))
					continue;
				else
				{
					point[1].x = m_points[i].x;
					point[1].y = m_points[i].y;
					break;
				}
			}		
		}
		else//ʼ��
		{
			Len = m_eLine.m_uBeginPointSize;
			point[0].x = m_points[0].x;
			point[0].y = m_points[0].y;
			
			int i;
			for( i = 1; i < m_nPoints ; i++ )
			{
				if( (m_points[0].x == m_points[i].x) && (m_points[0].y == m_points[i].y ))
					continue;
				else
				{
					point[1].x = m_points[i].x;
					point[1].y = m_points[i].y;
					break;
				}
			}
		}
	}
	
	double A1, B1, C1, A2, B2, C2;			
	double A, B, C;	
	
	
	A = point[1].y - point[0].y ;
	B = point[0].x - point[1].x;
	C = - (  A * point[0].x + B * point[0].y );				
	
	A2 = A;
	B2 = B;
	
	A1 = -B2;
	B1 = A2;
	C1 = - A1 * point[0].x - B1 * point[0].y;//?
	
	C2 = Len/2 * sqrt(A*A + B*B ) + C;
	
	pt[0].x = ( B1 *C2 - B2 * C1 ) / ( A1 * B2 - A2 * B1 );
	pt[0].y = ( C1 *A2 - C2 * A1 ) / ( A1 * B2 - A2 * B1 );			
	
	C2 = C - Len/2 * sqrt(A*A + B*B );
	
	pt[1].x = ( B1 *C2 - B2 * C1 ) / ( A1 * B2 - A2 * B1 );
	pt[1].y = ( C1 *A2 - C2 * A1 ) / ( A1 * B2 - A2 * B1 );
	
	double lamta;
	
	double L = sqrt( (double)(point[0].x - point[1].x) * (point[0].x - point[1].x) + (point[0].y - point[1].y) * (point[0].y - point[1].y) );
	
	lamta = Len / ( L - Len );
	
	point[2].x = (int)( ( point[0].x + point[1].x * lamta )/ ( 1+lamta) );
	point[2].y = (int)( ( point[0].y + point[1].y * lamta )/ ( 1+lamta) );
	
	C1 = - A1 * point[2].x - B1 * point[2].y;//?
	
	C2 = Len / 2 * sqrt(A*A + B*B ) + C;
	
	pt[3].x = ( B1 *C2 - B2 * C1 ) / ( A1 * B2 - A2 * B1 );
	pt[3].y = ( C1 *A2 - C2 * A1 ) / ( A1 * B2 - A2 * B1 );			
	
	C2 = C - Len / 2 * sqrt(A*A + B*B );
	
	pt[2].x = ( B1 *C2 - B2 * C1 ) / ( A1 * B2 - A2 * B1 );
	pt[2].y = ( C1 *A2 - C2 * A1 ) / ( A1 * B2 - A2 * B1 );

	return;
}

void CGeoPolyObj::SelectClipRgn(CDC *pDC)
{
	pDC->BeginPath();
	
	if (!m_eLine.m_bSmooth)
		pDC->Polygon(m_points, m_nPoints);
	else
		PolySpline3(pDC->m_hDC, m_points, m_nPoints, TRUE);	
	
	pDC->EndPath();
	
	pDC->SelectClipPath(RGN_AND);
	
	return;
}

BOOL CGeoPolyObj::CreatePen(CPen *pPen, CDC *pDC, BOOL bThin)
{
	
	int penStyle[7]=
	{
		//�ʵĹ̶�����ֵ��Ϊ���¼��֣������Զ�
		PS_SOLID,//            0
			PS_DASH,//             1       /* -------  */
			PS_DOT,//              2       /* .......  */
			PS_DASHDOT,//          3       /* _._._._  */
			PS_DASHDOTDOT,//       4       /* _.._.._  */
			PS_INSIDEFRAME,//      5
			PS_NULL//             6
	};
	
	if( m_eLine.m_nPenStyle == 6 )//�������ֱ�ӷ���
		return FALSE;
	
	int nWide;
	
	if( bThin )
		nWide = m_eLine.m_nPenWide;	
	else // ����ϸ��
		nWide = m_eLine2.m_nPenWide;
	
	int Endcap;
	
	if ( m_eLine.m_bSquarePen )
		Endcap = PS_ENDCAP_SQUARE;
	else
		Endcap = PS_ENDCAP_ROUND;
	
	CBrush brush;
	
	/*unsigned Type[8];
	int c = CDashLine::GetPattern(Type, m_eLine.m_bSquarePen, 5, m_eLine.m_nPenStyle);
	
	  DashLine.SetDC(pDC);
	  
		if( c ) 
		DashLine.SetPattern(Type, c);
	*/
	
	if( bThin )
	{
		if(m_eLine.s_LineStyle.m_PatternFill.m_nRow != -1 
			&& m_eLine.s_LineStyle.m_PatternFill.m_nCol != -1)
		{
			
			if(!m_eLine.s_LineStyle.CreateBrush(&brush, pDC))
				return FALSE;
			
			CDC MemDC;
			MemDC.CreateCompatibleDC(pDC);
			CBitmap bit;
			bit.CreateCompatibleBitmap(&MemDC, 8, 8);
			CBitmap *pOldBit = MemDC.SelectObject(&bit);
			CRect rect(0, 0, 8, 8);
			MemDC.FillRect(rect, &brush);
			MemDC.SelectObject(pOldBit);
			MemDC.DeleteDC();
			
			CBrush b;
			b.CreatePatternBrush(&bit);
			
			LOGBRUSH logbrush;
			b.GetLogBrush(&logbrush);
			
			
			if (!pPen->CreatePen(penStyle[m_eLine.m_nPenStyle] | PS_GEOMETRIC | PS_JOIN_MITER | Endcap, 
				nWide, 
				&logbrush) 
				)
				return FALSE;
		}	
		else
		{
			LOGBRUSH logbrush;
			logbrush.lbColor = m_eLine.s_LineStyle.m_PatternFill.m_crColourFore;
			logbrush.lbStyle = BS_SOLID;
			logbrush.lbHatch = NULL;
			
			if (!pPen->CreatePen(penStyle[m_eLine.m_nPenStyle] | PS_GEOMETRIC | PS_JOIN_MITER | Endcap, 
				nWide, 
				&logbrush) 
				)
				return FALSE;
		}
	}
	else
	{
		if(m_eLine2.s_LineStyle.m_PatternFill.m_nRow != -1 
			&& m_eLine2.s_LineStyle.m_PatternFill.m_nCol != -1)
		{			
			if(!m_eLine2.s_LineStyle.CreateBrush(&brush, pDC))
				return FALSE;
			
			CDC MemDC;
			MemDC.CreateCompatibleDC(pDC);
			CBitmap bit;
			bit.CreateCompatibleBitmap(&MemDC, 8, 8);
			CBitmap *pOldBit = MemDC.SelectObject(&bit);
			CRect rect(0, 0, 8, 8);
			MemDC.FillRect(rect, &brush);
			MemDC.SelectObject(pOldBit);
			MemDC.DeleteDC();
			
			CBrush b;
			b.CreatePatternBrush(&bit);
			
			LOGBRUSH logbrush;
			b.GetLogBrush(&logbrush);
			
			
			if (!pPen->CreatePen(penStyle[m_eLine2.m_nPenStyle] | PS_GEOMETRIC | PS_JOIN_MITER | Endcap, 
				nWide, 
				&logbrush) 
				)
				return FALSE;
		}	
		else
		{
			LOGBRUSH logbrush;
			logbrush.lbColor = m_eLine2.s_LineStyle.m_PatternFill.m_crColourFore;
			logbrush.lbStyle = BS_SOLID;
			logbrush.lbHatch = NULL;
			
			if (!pPen->CreatePen(penStyle[m_eLine2.m_nPenStyle] | PS_GEOMETRIC | PS_JOIN_MITER | Endcap, 
				nWide, 
				&logbrush) 
				)
				return FALSE;
		}
	}	
	
	return TRUE;	
}

BOOL CGeoPolyObj::CreatePenLine(CPen *pPen, CDC *pDC)
{
	int nWide = m_iSpaceWidth*2;
	
	CBrush brush;
	
	if(m_eLine.eLineSpaceFill.sFillStyle.m_PatternFill.m_nRow != -1 
		&& m_eLine.eLineSpaceFill.sFillStyle.m_PatternFill.m_nCol != -1)
	{
		
		if(!m_eLine.eLineSpaceFill.sFillStyle.CreateBrush(&brush, pDC))
			return FALSE;
		
		CDC MemDC;
		MemDC.CreateCompatibleDC(pDC);
		CBitmap bit;
		bit.CreateCompatibleBitmap(&MemDC, 8, 8);
		CBitmap *pOldBit = MemDC.SelectObject(&bit);
		CRect rect(0, 0, 8, 8);
		MemDC.FillRect(rect, &brush);
		MemDC.SelectObject(pOldBit);
		MemDC.DeleteDC();
		
		CBrush b;
		b.CreatePatternBrush(&bit);
		
		LOGBRUSH logbrush;
		b.GetLogBrush(&logbrush);
		
		
		if (!pPen->CreatePen(PS_SOLID | PS_GEOMETRIC | PS_ENDCAP_FLAT | PS_JOIN_MITER, 
			nWide, 
			&logbrush) 
			)
			return FALSE;
	}	
	else
	{
		LOGBRUSH logbrush;
		logbrush.lbColor = m_eLine.eLineSpaceFill.sFillStyle.m_PatternFill.m_crColourFore;
		logbrush.lbStyle = BS_SOLID;
		logbrush.lbHatch = NULL;
		
		if (!pPen->CreatePen(PS_SOLID | PS_GEOMETRIC | PS_ENDCAP_FLAT | PS_JOIN_MITER, 
			nWide, 
			&logbrush) 
			)
			return FALSE;
	}
	
	return TRUE;	
}


BOOL CGeoPolyObj::CreateBrush(CBrush *pBrush, CDC *pDC)
{	
	if(m_eFill.sFillStyle.m_PatternFill.m_nRow != -1 && m_eFill.sFillStyle.m_PatternFill.m_nCol != -1)
	{
		
		if(!m_eFill.sFillStyle.CreateBrush(pBrush, pDC))
			return FALSE;		
	}
	
	else
	{
		if (!pBrush->CreateSolidBrush( m_eFill.sFillStyle.m_PatternFill.m_crColourFore ) )
			return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// ��ȡ��point��������������ϵĵ�������
// ��������������
CPoint CGeoPolyObj::GetNearByIndex(const CPoint &point, int *iIndex)
{
	CPoint2D *points;
	int nPoints;
	if( m_eLine.m_bSmooth )
	{
		int nCounts;
		LPPOINT lppt;
		
		points = new CPoint2D[m_nPoints];
		if(points == NULL)
		{
			iIndex[0] = -1;
			iIndex[1] = -1;
			return CPoint(0,0);
		}

		int i;

		for ( i=0;i<m_nPoints;i++)
			points[i] = m_points[i];
		
		lppt = GetSPPoints(points,m_nPoints,nCounts,m_eLine.m_bClose);
		
		delete []points;

		points = new CPoint2D[nCounts];
		if(points == NULL)
		{
			iIndex[0] = -1;
			iIndex[1] = -1;
			return CPoint(0,0);
		}

		for (i=0;i<nCounts;i++)
			points[i] = lppt[i];

		delete lppt;
		
		nPoints = nCounts;
	}
	else
	{
		if( m_eLine.m_bClose && m_nPoints > 2 )
			points = new CPoint2D[m_nPoints+1];
		else
			points = new CPoint2D[m_nPoints];
		
		if(points == NULL )
		{
			iIndex[0] = -1;
			iIndex[1] = -1;
			return CPoint(0,0);
		}
		int i;

		for ( i=0; i<m_nPoints; i++)
		{
			points[i].x = m_points[i].x;
			points[i].y = m_points[i].y;
		}

		nPoints = m_nPoints;
		if( m_eLine.m_bClose && m_nPoints > 2 )
		{
			points[m_nPoints] = points[0];
			nPoints ++;
		}
	}

	CPoint2D pt(point.x,point.y), retPt;

	ptopointset(nPoints,points, pt, retPt, iIndex);

	delete [] points;

	if( iIndex[0] == -1 )
	{
		if(iIndex[1] != 0 ) 
			iIndex[1] -= 1;
	}

	return CPoint(retPt.x, retPt.y);
}

//////////////////////////////////////////////////////////////////////////
// ��ȡͼ��·�������е�
//  
//
int CGeoPolyObj::GetPathPoint(LPPOINT &lppt)
{
	int nPoints;
	if( m_eLine.m_bSmooth )
	{
		int nCounts;

		CPoint2D *points = new CPoint2D[m_nPoints];
		if(lppt == NULL)
		{			
			return -1;
		}
		
		for (int i=0;i<m_nPoints;i++)
			points[i] = m_points[i];

		lppt = GetSPPoints(points,m_nPoints,nCounts,m_eLine.m_bClose);
		
		delete [] points;

		nPoints = nCounts;
	}
	else
	{
		if( m_eLine.m_bClose && m_nPoints > 2 )
			lppt = new CPoint[m_nPoints+1];
		else
			lppt = new CPoint[m_nPoints];
		
		if(lppt == NULL )
			return -1;
		
		for (int i=0; i<m_nPoints; i++)
		{
			lppt[i].x = m_points[i].x;
			lppt[i].y = m_points[i].y;
		}
		
		nPoints = m_nPoints;

		if( m_eLine.m_bClose && m_nPoints > 2 )
		{
			lppt[m_nPoints] = lppt[0];
			nPoints ++;
		}
	}

	return nPoints;
}

// ������˳��
void CGeoPolyObj::SwitchOrderPoint()
{
	CPoint tmp;
	CPoint2D geoTmp;

	for (int i=0;i<m_nPoints/2; i++)
	{
		tmp = m_points[i];
		m_points[i] = m_points[m_nPoints-i-1];
		m_points[m_nPoints-i-1] = m_points[i];

		geoTmp = m_geoPoints[i];
		m_geoPoints[i] = m_geoPoints[m_nPoints-i-1];
		m_geoPoints[m_nPoints-i-1] = m_geoPoints[i];
	}
}

// ��������
void CGeoPolyObj::CopyAttribute(CGeoDrawObj *pSrcObj)
{	
	CGeoPolyObj *pObj = (CGeoPolyObj*)pSrcObj;
	m_eLine = pObj->m_eLine;
	m_eFill = pObj->m_eFill;
	m_eLine2 = pObj->m_eLine2;

	memcpy(&m_lfFont, &pObj->m_lfFont,sizeof(LOGFONT));			// ��ע���弰��С
	m_clrText			= pObj->m_clrText;			// ��ɫ
	m_bShowText			= pObj->m_bShowText;		// �Ƿ���ʾ

	CGeoDrawObj::CopyAttribute(pSrcObj);
}