#include "stdafx.h"
#include "Spline3.h"
#include <float.h>
#include "Point2D.h"
#include <math.h>
#include <stack>
using namespace std;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define SEGMENT 20	// 每两点之间插入的点数

extern "C" 
int atrde(double *b,long n,long m, double *d);

extern "C" 
BOOL PolySpline3Label(HDC hDC, LPCTSTR str, double space, 
					  LOGFONT *lgfn, CPoint2D* lpptControl, 
					  DWORD cPoints, BOOL bClose)
{
	LPPOINT ppp;
	int m_nPoints;
	
	ppp = GetSPPoints( lpptControl, cPoints, m_nPoints, bClose);
	
	int n = m_nPoints;

	LPPOINT pt = ppp;
	
	double len = space;

	double tmp = len-100.0;
	double L;
	double lamda;	
	double fnHeight;	
	HFONT hFont, hOldFont;
	TEXTMETRIC tm;

	int mode = SetBkMode(hDC, OPAQUE);

	UINT Align = SetTextAlign( hDC, TA_CENTER | TA_BOTTOM);
	
	for(int i = 1; i < n; i++)
	{
		L = sqrt( (double)( pt[i].x - pt[i-1].x ) * (pt[i].x - pt[i-1].x ) + ( pt[i].y - pt[i-1].y ) * (pt[i].y - pt[i-1].y ) );
		tmp += L;

		if(tmp >= len )
		{			
					
			lamda = (tmp - len) / ( L - tmp +len);
			
			double sita; 
			if( pt[i].y <= pt[i-1].y)
				sita = asin( ( pt[i].x - pt[i-1].x ) / L );
			else
				sita = asin( -( pt[i].x - pt[i-1].x ) / L );


			double x  = ( pt[i].x + pt[i-1].x * lamda ) / ( 1+lamda);
			double y  = ( pt[i].y + pt[i-1].y * lamda ) / ( 1+lamda);
			
			pt[i-1].x = (int)x;
			pt[i-1].y = (int)y;

			/*if( pt[i].y <= pt[i-1].y)
				sita = sita * 180.0 / 3.1415926 * 10.0 + 900;
			else
				sita = sita * 180.0 / 3.1415926 * 10.0 + 2700;
				*/

			sita = sita * 180.0 / 3.1415926 * 10.0 + 900;// 2002.11.7修改()
			sita = ( sita > 900 ? sita+1800:sita);       //  

			lgfn->lfEscapement = (long)-sita;

			hFont = CreateFontIndirect(lgfn);
			
			hOldFont = (HFONT)SelectObject(hDC, hFont);

			GetTextMetrics(hDC, &tm);

			fnHeight =  -fabs((double)tm.tmHeight/2);

			sita = - (sita )/1800.0 * 3.1415926;
			
			x += fnHeight*sin(sita) - fnHeight *cos(sita);
			y += fnHeight*cos(sita) + fnHeight *sin(sita);
			
			TextOut(hDC, (int)x, (int)y, str, strlen((LPCSTR)str));			

			
			SelectObject(hDC, hOldFont);
			DeleteObject(hFont);
			tmp = 0;

			i--;
		}
	}

	SetTextAlign( hDC, Align );

	SetBkMode(hDC, mode);

	delete ppp;

	
	return TRUE;
	
	//return DrawLabel(hDC, str, space, lgfn);
}

extern "C" 
BOOL PolySpline3Title(HDC hDC, LPCTSTR str, LOGFONT *lgfn, 
						CPoint2D *lpptControl, DWORD cPoints, BOOL bClose)
{
	LPPOINT ppp;
	int m_nPoints;
	
	ppp = GetSPPoints( lpptControl, cPoints, m_nPoints, bClose);
	
	int n = m_nPoints;

	LPPOINT pt = ppp;
	

	int i;
	
	double tmp = 0;
	double L;
	double lamda;	
	double fnHeight;
	double LenSum = 0;//总长
	HFONT hFont, hOldFont;
	TEXTMETRIC tm;

	LPCTSTR ls1 = str;                     //文本内容
	int cd = strlen(ls1);                       //文本字符长度
	int nStrLen = 0;
	char c1, c2;	
	while(cd > 0 )  //计算所有的字符个数
	{
		c1=*ls1;
		c2=*(ls1+1);
		if(c1<0&&c2<0127)  //如果是一个汉字
		{
			ls1=ls1 + 2 ;        //跳过汉字指向下一个字符位置
			cd = cd - 2 ;                  //字符数减2
			nStrLen ++;			
		}
		else //如果是一个西文字符
		{
			ls1++;                   //跳过这一个字符到下一个字符
			cd=cd-1;                 //字符数减1
			nStrLen ++;
		}
	}	
	
	for( i = 1; i < n; i++)
	{
		L = sqrt( (double)( pt[i].x - pt[i-1].x ) * (pt[i].x - pt[i-1].x ) + ( pt[i].y - pt[i-1].y ) * (pt[i].y - pt[i-1].y ) );
		LenSum += L;
	}

	ls1 = str;                     //文本内容
	TCHAR hz[4];
	cd = strlen(ls1);                       //文本字符长度

	double len = LenSum / ( nStrLen + 1 );

	int mode = SetBkMode(hDC, TRANSPARENT);

	UINT Align = SetTextAlign( hDC, TA_CENTER | TA_BOTTOM);
	
	for( i = 1; i < n; i++)
	{
		L = sqrt( (double)( pt[i].x - pt[i-1].x ) * (pt[i].x - pt[i-1].x ) + ( pt[i].y - pt[i-1].y ) * (pt[i].y - pt[i-1].y ) );
		tmp += L;

		if(tmp >= len )
		{			
					
			lamda = (tmp - len) / ( L - tmp +len);
			
			double sita; 
			if( pt[i].y <= pt[i-1].y)
				sita = asin( ( pt[i].x - pt[i-1].x ) / L );
			else
				sita = asin( -( pt[i].x - pt[i-1].x ) / L );


			double x  = ( pt[i].x + pt[i-1].x * lamda ) / ( 1+lamda);
			double y  = ( pt[i].y + pt[i-1].y * lamda ) / ( 1+lamda);
			
			pt[i-1].x = (int)x;
			pt[i-1].y = (int)y;

			if( pt[i].y <= pt[i-1].y)
				sita = sita * 180.0 / 3.1415926 * 10.0 ;
			else
				sita = sita * 180.0 / 3.1415926 * 10.0 + 1800;

			//sita = sita * 180.0 / 3.1415926 * 10.0 + 900;// 2002.11.7修改()
			sita = ( sita > 900 ? sita+1800:sita);       //  

			lgfn->lfEscapement = (long)-sita;

			hFont = CreateFontIndirect(lgfn);
			
			hOldFont = (HFONT)SelectObject(hDC, hFont);

			GetTextMetrics(hDC, &tm);

			fnHeight =  -fabs((double)tm.tmHeight/2);

			sita = - (sita )/1800.0 * 3.1415926;
			
			//x += fnHeight*sin(sita) - fnHeight *cos(sita);
			//y += fnHeight*cos(sita) + fnHeight *sin(sita);

			if( cd > 0)
			{
				c1=*ls1;
				c2=*(ls1+1);
				if(c1<0&&c2<0)  //如果是一个汉字
				{
					strncpy_s( hz, ls1, 2);
					hz[2] = 0;
					ls1=ls1 + 2 ;        //跳过汉字指向下一个字符位置
					cd = cd - 2 ;                  //字符数减2					
				}
				else //如果是一个西文字符
				{
					strncpy_s( hz, ls1, 1);
					hz[1] = 0;
					ls1++;                   //跳过这一个字符到下一个字符
					cd=cd-1;                 //字符数减1					
				}
				
				TextOut(hDC, (int)x, (int)y, hz, strlen(hz));			
			}

			
			SelectObject(hDC, hOldFont);
			DeleteObject(hFont);
			tmp = 0;

			i--;
		}
	}

	SetTextAlign( hDC, Align);

	SetBkMode(hDC, mode);

	delete ppp;
	
	return TRUE;	
}


extern "C" 
BOOL PolylineLabel( HDC hDC,LPCTSTR str, double space, 
				   LOGFONT *lgfn, CONST POINT *lppt, int cPoints )
{
	int n = cPoints;

	LPPOINT pt = new POINT [ n ];

	for( int j = 0; j < n; j++)
	{
		pt[j].x = lppt[j].x;
		pt[j].y = lppt[j].y;
	}
	
	double len = space;

	double tmp = len-100.0;
	double L;
	double lamda;	
	double fnHeight;	
	HFONT hFont, hOldFont;
	TEXTMETRIC tm;

	int mode = SetBkMode(hDC, OPAQUE);

	UINT Align = SetTextAlign( hDC, TA_CENTER | TA_BOTTOM);
	
	for(int i = 1; i < n; i++)
	{
		L = sqrt( (double)( pt[i].x - pt[i-1].x ) * (pt[i].x - pt[i-1].x ) + ( pt[i].y - pt[i-1].y ) * (pt[i].y - pt[i-1].y ) );
		tmp += L;

		if(tmp >= len )
		{			
					
			lamda = (tmp - len) / ( L - tmp +len);
			
			double sita; 
			if( pt[i].y <= pt[i-1].y)
				sita = asin( ( pt[i].x - pt[i-1].x ) / L );
			else
				sita = asin( -( pt[i].x - pt[i-1].x ) / L );


			double x  = ( pt[i].x + pt[i-1].x * lamda ) / ( 1+lamda);
			double y  = ( pt[i].y + pt[i-1].y * lamda ) / ( 1+lamda);
			
			pt[i-1].x = (int)x;
			pt[i-1].y = (int)y;

			/*if( pt[i].y <= pt[i-1].y)
				sita = sita * 180.0 / 3.1415926 * 10.0 + 900;
			else
				sita = sita * 180.0 / 3.1415926 * 10.0 + 2700;
				*/

			sita = sita * 180.0 / 3.1415926 * 10.0 + 900;// 2002.11.7修改()
			sita = ( sita > 900 ? sita+1800:sita);       //  

			lgfn->lfEscapement = (long)-sita;

			hFont = CreateFontIndirect(lgfn);
			
			hOldFont = (HFONT)SelectObject(hDC, hFont);

			GetTextMetrics(hDC, &tm);

			fnHeight =  -fabs((double)tm.tmHeight/2);

			sita = - (sita )/1800.0 * 3.1415926;
			
			x += fnHeight*sin(sita) - fnHeight *cos(sita);
			y += fnHeight*cos(sita) + fnHeight *sin(sita);
			
			TextOut(hDC, (int)x, (int)y, str, strlen(str));			

			
			SelectObject(hDC, hOldFont);
			DeleteObject(hFont);
			tmp = 0;

			i--;
		}
	}

	delete []pt;

	SetTextAlign( hDC, Align );

	SetBkMode(hDC, mode);
	
	return TRUE;	
}

extern "C" 
BOOL PolylineTitle( HDC hDC,LPCTSTR str, LOGFONT *lgfn, 
					 CONST POINT *lppt, int cPoints )
{
	int n = cPoints;

	LPPOINT pt = new POINT [ n ];

	for( int j = 0; j < n; j++)
	{
		pt[j].x = lppt[j].x;
		pt[j].y = lppt[j].y;
	}
	

	int i;
	
	double tmp = 0;
	double L;
	double lamda;	
	double fnHeight;
	double LenSum = 0;//总长
	HFONT hFont, hOldFont;
	TEXTMETRIC tm;

	LPCTSTR ls1 = str;                     //文本内容
	int cd = strlen(ls1);                       //文本字符长度
	int nStrLen = 0;
	char c1, c2;	
	while(cd > 0 )  //计算所有的字符个数
	{
		c1=*ls1;
		c2=*(ls1+1);
		if(c1<0&&c2<0127)  //如果是一个汉字
		{
			ls1=ls1 + 2 ;        //跳过汉字指向下一个字符位置
			cd = cd - 2 ;                  //字符数减2
			nStrLen ++;			
		}
		else //如果是一个西文字符
		{
			ls1++;                   //跳过这一个字符到下一个字符
			cd=cd-1;                 //字符数减1
			nStrLen ++;
		}
	}	
	
	for( i = 1; i < n; i++)
	{
		L = sqrt( (double)( pt[i].x - pt[i-1].x ) * (pt[i].x - pt[i-1].x ) + ( pt[i].y - pt[i-1].y ) * (pt[i].y - pt[i-1].y ) );
		LenSum += L;
	}

	ls1 = str;                     //文本内容
	TCHAR hz[4];
	cd = strlen(ls1);                       //文本字符长度

	double len = LenSum / ( nStrLen + 1 );

	int mode = SetBkMode(hDC, TRANSPARENT);

	UINT Align = SetTextAlign( hDC, TA_CENTER | TA_BOTTOM);
	
	for( i = 1; i < n; i++)
	{
		L = sqrt( (double)( pt[i].x - pt[i-1].x ) * (pt[i].x - pt[i-1].x ) + ( pt[i].y - pt[i-1].y ) * (pt[i].y - pt[i-1].y ) );
		tmp += L;

		if(tmp >= len )
		{			
					
			lamda = (tmp - len) / ( L - tmp +len);
			
			double sita; 
			if( pt[i].y <= pt[i-1].y)
				sita = asin( ( pt[i].x - pt[i-1].x ) / L );
			else
				sita = asin( -( pt[i].x - pt[i-1].x ) / L );


			double x  = ( pt[i].x + pt[i-1].x * lamda ) / ( 1+lamda);
			double y  = ( pt[i].y + pt[i-1].y * lamda ) / ( 1+lamda);
			
			pt[i-1].x = (int)x;
			pt[i-1].y = (int)y;

			if( pt[i].y <= pt[i-1].y)
				sita = sita * 180.0 / 3.1415926 * 10.0 ;
			else
				sita = sita * 180.0 / 3.1415926 * 10.0 + 1800;

			lgfn->lfEscapement = (long)-sita;

			hFont = CreateFontIndirect(lgfn);
			
			hOldFont = (HFONT)SelectObject(hDC, hFont);

			GetTextMetrics(hDC, &tm);

			fnHeight =  -fabs((double)tm.tmHeight/2);

			sita = - (sita )/1800.0 * 3.1415926;
			
			//x += fnHeight*sin(sita) - fnHeight *cos(sita);
			//y += fnHeight*cos(sita) + fnHeight *sin(sita);

			if( cd > 0)
			{
				c1=*ls1;
				c2=*(ls1+1);
				if(c1<0&&c2<0)  //如果是一个汉字
				{
					strncpy_s( hz, ls1, 2);
					hz[2] = 0;
					ls1=ls1 + 2 ;        //跳过汉字指向下一个字符位置
					cd = cd - 2 ;                  //字符数减2					
				}
				else //如果是一个西文字符
				{
					strncpy_s( hz, ls1, 1);
					hz[1] = 0;
					ls1++;                   //跳过这一个字符到下一个字符
					cd=cd-1;                 //字符数减1					
				}
				
				TextOut(hDC, (int)x, (int)y, hz, strlen(hz));			
			}
			
			SelectObject(hDC, hOldFont);
			DeleteObject(hFont);
			tmp = 0;

			i--;
		}
	}

	delete []pt;

	SetTextAlign( hDC, Align);

	SetBkMode(hDC, mode);
	
	return TRUE;	
}


extern "C" 
BOOL PolygonLabel( HDC hDC,	LPCTSTR str, double space, 
				  LOGFONT *lgfn, CONST POINT *lppt, int cPoints )
{
	return PolylineLabel( hDC,	str, space, lgfn, lppt, cPoints );	
}

extern "C" 
BOOL PolygonTitle( HDC hDC,	LPCTSTR str, 
					LOGFONT *lgfn, CONST POINT *lppt, int cPoints )
{
	return PolylineTitle( hDC,	str, lgfn, lppt, cPoints );	
}

//功能：画B样条曲线的等距线
//边界条件：自由端即两端点的二阶导数为0
//参数：lppt-控制点， 
//		nPoints-控制点数， 
//		bClose-是否封闭,值为：
//				TRUE 封闭，
//				FALSE 不封闭，
//				缺省是不封闭
//		bInner-是否为内等距线
//				FALSE 外等距线
//				TRUE  内等距线

extern "C" 
BOOL ParallelCurve(HDC hDC, LPPOINT lppt, DWORD nPoints, 
				   BOOL bClose, BOOL bInner, double dWidth)
{
	if(nPoints <= 1 || lppt == NULL )
		return FALSE;
	if(nPoints == 2)
	{
		MoveToEx( hDC, lppt[0].x, lppt[0].y, NULL);
		LineTo  ( hDC, lppt[1].x, lppt[1].y);
		return TRUE;
	}

	DWORD i, j;
	DWORD cPoints = nPoints;

	for( i = 1; i < nPoints ; i++ )
	{
		if( (lppt[i].x == lppt[i-1].x) && (lppt[i].y == lppt[i-1].y ))
		{
			cPoints--;
		}
	}

	if( cPoints < 2)
		return FALSE;
	
	LPPOINT lpptControl = new POINT [ cPoints ];

	j = 0;

	for( i = 1; i < nPoints ; i++ )
	{
		if( (lppt[i].x != lppt[i-1].x) || (lppt[i].y != lppt[i-1].y ) )
		{
			lpptControl[j].x = lppt[i-1].x;
			lpptControl[j].y = lppt[i-1].y;
			j++;
		}
	}

	lpptControl[cPoints -1 ].x = lppt[nPoints-1].x;	
	lpptControl[cPoints -1 ].y = lppt[nPoints-1].y;	

	double *t = new double [cPoints];//弦长	
	double F1, F2, G1, G2;	//权函数
							// P(t) = F1(t)*P1 + F2(t)*P2 + t1 * G1(t) * P'1 + t1 * G2(t) * P'2
							// F1(t) = 1 - 3 * (t/t1)^2 + 2 * (t/t1)^3
							// F2(t) = 3 * (/t1)^2 - 2 * (t/t1)^3
							// G1(t) = (t/t1) - 2 * (t/t1)^2 + (t/t1)^3
							// G2(t) = -(t/t1)^2 + (t/t1)^3
						    //
		                             //切矢分量及中间变量
	double *Qx = new double [cPoints];	// Q = 3 * t(i) / t(i-1) * ( P(i) - P(i-1) ) + 3 * t(i-1) / t(i) * ( P(i+1) - P(i) )
	double *Qy = new double [cPoints];
	double *T = new double [3*cPoints-2];	 
	

	for( i = 0; i < cPoints-1 ; i++) // 计算弦长
		t[i] = sqrt((double)
					(lpptControl[i+1].x - lpptControl[i].x) * (lpptControl[i+1].x - lpptControl[i].x) 
					+ (lpptControl[i+1].y - lpptControl[i].y) * (lpptControl[i+1].y - lpptControl[i].y) 
				);
	
	//求矩阵
	
	for( i = 1; i < cPoints-1 ; i++)
	{
		T[i*3 - 1] = t[i];
		T[i*3    ] = 2.0 * ( t[i-1] + t[i]);
		T[i*3 + 1] = t[i-1];
	}

	for(i = 1; i < cPoints-1; i++)
	{
		Qx[i] = 3 * t[i]/t[i-1] * ( lpptControl[i].x - lpptControl[i-1].x ) + 3 * t[i-1]/t[i] * ( lpptControl[i+1].x - lpptControl[i].x );
		Qy[i] = 3 * t[i]/t[i-1] * ( lpptControl[i].y - lpptControl[i-1].y ) + 3 * t[i-1]/t[i] * ( lpptControl[i+1].y - lpptControl[i].y );
	}
	
	T[0] = 1.0; T[1] = 0.5;
	T[ 3*cPoints - 4 ] = 1.0; T[ 3*cPoints - 3] = 2.0;

	Qx[0] = 1.5 / t[0] * (lpptControl[1].x - lpptControl[0].x);	
	Qx[cPoints-1] = 3.0 / t[cPoints-2] * ( lpptControl[cPoints-1].x - lpptControl[cPoints-2].x);

	Qy[0] = 1.5 / t[0] * (lpptControl[1].y - lpptControl[0].y);	
	Qy[cPoints-1] = 3.0 / t[cPoints-2] * ( lpptControl[cPoints-1].y - lpptControl[cPoints-2].y);
	
	if( atrde(T, (long)cPoints, (long)(3*cPoints-2), Qx) <= 0) 
	{
		delete T;
		delete t;	
		delete Qx;
		delete Qy;	
		return FALSE;
	}

	if( atrde(T, (long)cPoints, (long)(3*cPoints-2), Qy) <= 0)
	{
		delete T;
		delete t;	
		delete Qx;
		delete Qy;	
		return FALSE;
	}

	delete T;

	double x, y;
	double dx, dy;
	double a;//距离为1mm
	
	if( bInner ) //内等距线
		a = -dWidth;
	else//外等距线
		a = dWidth;
	
	POINT *ppp;

	long pps; 

	if( bClose )
		pps = cPoints * SEGMENT + cPoints + 1;
	else
		pps = ( cPoints - 1 ) * SEGMENT + cPoints ;

	ppp = new POINT [ pps];

	pps = 0;
	
	ppp[0].x = lpptControl[0].x + a * Qy[0] / sqrt(Qx[0]*Qx[0]+Qy[0]*Qy[0]);
	ppp[0].y = lpptControl[0].y - a * Qx[0] / sqrt(Qx[0]*Qx[0]+Qy[0]*Qy[0]);
	
	//MoveToEx(hDC, lpptControl[0].x, lpptControl[0].y, NULL);
	double U = 1.0 / (double)(SEGMENT+1);
	for(i = 0 ;  i<cPoints-1; i++)
	{		
		for(j = 0; j < SEGMENT; j++)
		{
			pps ++;
			U *= (j+1.0);
            F1 = 1.0 - 3.0 * U*U + 2 * U*U*U ;
			F2 = 3 * U*U - 2 * U*U*U ;
			G1 = U - 2 * U*U + U*U*U ;
			G2 = -U*U + U*U*U;

			x = F1 * lpptControl[i].x + F2 * lpptControl[i+1].x + t[i] * G1 * Qx[i] + t[i] * G2 * Qx[i+1];
			y = F1 * lpptControl[i].y + F2 * lpptControl[i+1].y + t[i] * G1 * Qy[i] + t[i] * G2 * Qy[i+1];

			dx = 6.0/t[i]*( U*U - U) * lpptControl[i].x + 6.0/t[i]*( U - U *U) * lpptControl[i+1].x 
				+ ( 1.0 - 4.0 * U + 3 * U *U) * Qx[i] + (3 * U*U - 2 * U ) * Qx[i+1];
			dy = 6.0/t[i]*( U*U - U) * lpptControl[i].y + 6.0/t[i]*( U - U *U) * lpptControl[i+1].y 
				+ ( 1.0 - 4.0 * U + 3 * U *U) * Qy[i] + (3 * U*U - 2 * U ) * Qy[i+1];

			
			ppp[pps].x = x + a * dy / sqrt(dx*dx+dy*dy);
			ppp[pps].y = y - a * dx / sqrt(dx*dx+dy*dy);

			//LineTo(hDC, (int)x, (int)y);
			U = 1.0 / (double)( SEGMENT + 1 );
		}
		pps ++;

		ppp[pps].x = lpptControl[i+1].x + a * Qy[i+1] / sqrt(Qx[i+1]*Qx[i+1]+Qy[i+1]*Qy[i+1]);
		ppp[pps].y = lpptControl[i+1].y - a * Qx[i+1] / sqrt(Qx[i+1]*Qx[i+1]+Qy[i+1]*Qy[i+1]);
	
		//LineTo(hDC, lpptControl[i+1].x, lpptControl[i+1].y);
	}
	if( bClose)
	{
		double te = sqrt((double)
			(lpptControl[0].x - lpptControl[cPoints-1].x) * (lpptControl[0].x - lpptControl[cPoints-1].x) 
			+ (lpptControl[0].y - lpptControl[cPoints-1].y) * (lpptControl[0].y - lpptControl[cPoints-1].y) 
			);

		U = 1.0 / (double)( SEGMENT + 1 );

		for(j = 0; j < SEGMENT; j++)
		{
			U *= (j+1.0);
			F1 = 1.0 - 3.0 * U*U + 2 * U*U*U ;
			F2 = 3 * U*U - 2 * U*U*U ;
			G1 = U - 2 * U*U + U*U*U ;
			G2 = -U*U + U*U*U;

			x = F1 * lpptControl[cPoints-1].x + F2 * lpptControl[0].x + te * G1 * Qx[cPoints-1] + te * G2 * Qx[0];
			y = F1 * lpptControl[cPoints-1].y + F2 * lpptControl[0].y + te * G1 * Qy[cPoints-1] + te * G2 * Qy[0];

			pps ++;
			dx = 6.0/te*( U*U - U) * lpptControl[cPoints-1].x + 6.0/te*( U - U *U) * lpptControl[0].x 
				+ ( 1.0 - 4.0 * U + 3 * U *U) * Qx[cPoints-1] + (3 * U*U - 2 * U ) * Qx[0];
			dy = 6.0/te*( U*U - U) * lpptControl[cPoints-1].y + 6.0/te*( U - U *U) * lpptControl[0].y 
				+ ( 1.0 - 4.0 * U + 3 * U *U) * Qy[cPoints-1] + (3 * U*U - 2 * U ) * Qy[0];

			
			ppp[pps].x = x + a * dy / sqrt(dx*dx+dy*dy);
			ppp[pps].y = y - a * dx / sqrt(dx*dx+dy*dy);

			//ppp[pps].x = (int)x;
			//ppp[pps].y = (int)y;

			//LineTo(hDC, (int)x, (int)y);
			U = 1.0 / (double)( SEGMENT + 1 );
		}

		pps++;

		ppp[pps].x = lpptControl[0].x + a * Qy[0] / sqrt(Qx[0]*Qx[0]+Qy[0]*Qy[0]);
		ppp[pps].y = lpptControl[0].y - a * Qx[0] / sqrt(Qx[0]*Qx[0]+Qy[0]*Qy[0]);
			
		//LineTo(hDC, lpptControl[0].x, lpptControl[0].y);
	}


	Polyline(hDC, ppp, pps+1);

	delete lpptControl;
	delete ppp;
	delete t;	
	delete Qx;
	delete Qy;	
	return TRUE;
}

extern "C" 
long GetSpline3Pointf(CPoint2D* lppt, DWORD nPoints, BOOL bClose, CPoint2D* lpptPoint)
{
	if(nPoints < 1 || lppt == NULL )
		return 0;
	
	if(nPoints == 1)
	{
		if( lpptPoint )
			lpptPoint[0] = lppt[0];
		return 1;
	}

	if(nPoints == 2)
	{
		if( lpptPoint )
		{
			lpptPoint[0] = lppt[0];
			lpptPoint[1] = lppt[1];
		}
		return 2;
	}

	DWORD i, j;
	DWORD cPoints = nPoints;

	for( i = 1; i < nPoints ; i++ )
	{
		if( (lppt[i].x == lppt[i-1].x) && (lppt[i].y == lppt[i-1].y ))
		{
			cPoints--;
		}
	}

	CPoint2D* lpptControl = new CPoint2D [ cPoints ];

	j = 0;

	for( i = 1; i < nPoints ; i++ )
	{
		if( (lppt[i].x != lppt[i-1].x) || (lppt[i].y != lppt[i-1].y ) )
		{
			lpptControl[j].x = lppt[i-1].x;
			lpptControl[j].y = lppt[i-1].y;
			j++;
		}
	}

	lpptControl[cPoints -1 ].x = lppt[nPoints-1].x;	
	lpptControl[cPoints -1 ].y = lppt[nPoints-1].y;	

	double *t = new double [cPoints];//弦长	
	double F1, F2, G1, G2;	//权函数
	// P(t) = F1(t)*P1 + F2(t)*P2 + t1 * G1(t) * P'1 + t1 * G2(t) * P'2
	// F1(t) = 1 - 3 * (t/t1)^2 + 2 * (t/t1)^3
	// F2(t) = 3 * (/t1)^2 - 2 * (t/t1)^3
	// G1(t) = (t/t1) - 2 * (t/t1)^2 + (t/t1)^3
	// G2(t) = -(t/t1)^2 + (t/t1)^3
	//
	//切矢分量及中间变量
	double *Qx = new double [cPoints];	// Q = 3 * t(i) / t(i-1) * ( P(i) - P(i-1) ) + 3 * t(i-1) / t(i) * ( P(i+1) - P(i) )
	double *Qy = new double [cPoints];
	double *T = new double [3*cPoints-2];	 


	for( i = 0; i < cPoints-1 ; i++) // 计算弦长
		t[i] = sqrt((double)
		(lpptControl[i+1].x - lpptControl[i].x) * (lpptControl[i+1].x - lpptControl[i].x) 
		+ (lpptControl[i+1].y - lpptControl[i].y) * (lpptControl[i+1].y - lpptControl[i].y) 
		);

	//求矩阵

	for( i = 1; i < cPoints-1 ; i++)
	{
		T[i*3 - 1] = t[i];
		T[i*3    ] = 2.0 * ( t[i-1] + t[i]);
		T[i*3 + 1] = t[i-1];
	}

	for(i = 1; i < cPoints-1; i++)
	{
		Qx[i] = 3 * t[i]/t[i-1] * ( lpptControl[i].x - lpptControl[i-1].x ) + 3 * t[i-1]/t[i] * ( lpptControl[i+1].x - lpptControl[i].x );
		Qy[i] = 3 * t[i]/t[i-1] * ( lpptControl[i].y - lpptControl[i-1].y ) + 3 * t[i-1]/t[i] * ( lpptControl[i+1].y - lpptControl[i].y );
	}

	T[0] = 1.0; T[1] = 0.5;
	T[ 3*cPoints - 4 ] = 1.0; T[ 3*cPoints - 3] = 2.0;

	Qx[0] = 1.5 / t[0] * (lpptControl[1].x - lpptControl[0].x);	
	Qx[cPoints-1] = 3.0 / t[cPoints-2] * ( lpptControl[cPoints-1].x - lpptControl[cPoints-2].x);

	Qy[0] = 1.5 / t[0] * (lpptControl[1].y - lpptControl[0].y);	
	Qy[cPoints-1] = 3.0 / t[cPoints-2] * ( lpptControl[cPoints-1].y - lpptControl[cPoints-2].y);

	if( atrde(T, (long)cPoints, (long)(3*cPoints-2), Qx) <= 0) 
	{
		delete T;
		delete t;	
		delete Qx;
		delete Qy;	
		return FALSE;
	}

	if( atrde(T, (long)cPoints, (long)(3*cPoints-2), Qy) <= 0)
	{
		delete T;
		delete t;	
		delete Qx;
		delete Qy;	
		return FALSE;
	}

	delete T;

	CPoint2D *ppp;

	long pps; 

	if( bClose )
		pps = cPoints * SEGMENT + cPoints + 1;
	else
		pps = ( cPoints - 1 ) * SEGMENT + cPoints ;

	ppp = new CPoint2D [ pps];

	ppp[0] = lpptControl[0];

	pps = 0;

	//MoveToEx(hDC, lpptControl[0].x, lpptControl[0].y, NULL);
	double x, y;
	double U = 1.0 / (double)(SEGMENT+1);
	for(i = 0 ;  i<cPoints-1; i++)
	{		
		for(j = 0; j < SEGMENT; j++)
		{
			pps ++;
			U *= (j+1.0);
			F1 = 1.0 - 3.0 * U*U + 2 * U*U*U ;
			F2 = 3 * U*U - 2 * U*U*U ;
			G1 = U - 2 * U*U + U*U*U ;
			G2 = -U*U + U*U*U;

			x = F1 * lpptControl[i].x + F2 * lpptControl[i+1].x + t[i] * G1 * Qx[i] + t[i] * G2 * Qx[i+1];
			y = F1 * lpptControl[i].y + F2 * lpptControl[i+1].y + t[i] * G1 * Qy[i] + t[i] * G2 * Qy[i+1];

			ppp[pps].x = x;
			ppp[pps].y = y;

			U = 1.0 / (double)( SEGMENT + 1 );
		}
		pps ++;
		ppp[pps] = lpptControl[i+1];
	}
	if( bClose)
	{
		double te = sqrt((double)
			(lpptControl[0].x - lpptControl[cPoints-1].x) * (lpptControl[0].x - lpptControl[cPoints-1].x) 
			+ (lpptControl[0].y - lpptControl[cPoints-1].y) * (lpptControl[0].y - lpptControl[cPoints-1].y) 
			);

		U = 1.0 / (double)( SEGMENT + 1 );

		for(j = 0; j < SEGMENT; j++)
		{
			U *= (j+1.0);
			F1 = 1.0 - 3.0 * U*U + 2 * U*U*U ;
			F2 = 3 * U*U - 2 * U*U*U ;
			G1 = U - 2 * U*U + U*U*U ;
			G2 = -U*U + U*U*U;

			x = F1 * lpptControl[cPoints-1].x + F2 * lpptControl[0].x + te * G1 * Qx[cPoints-1] + te * G2 * Qx[0];
			y = F1 * lpptControl[cPoints-1].y + F2 * lpptControl[0].y + te * G1 * Qy[cPoints-1] + te * G2 * Qy[0];

			pps ++;
			ppp[pps].x = x;
			ppp[pps].y = y;

			U = 1.0 / (double)( SEGMENT + 1 );
		}

		pps++;

		ppp[pps] = lpptControl[0];
	}
	
	if( lpptPoint != NULL )
	{
		for (long i=0; i<pps+1; i++)
			lpptPoint[i] = ppp[i];
	}

	delete ppp;
	delete t;	
	delete Qx;
	delete Qy;
	delete lpptControl;

	return (pps+1);
}

extern "C" 
long GetSpline3Point(LPPOINT lppt, DWORD nPoints, BOOL bClose, LPPOINT lpptPoint)
{
	if(nPoints < 1 || lppt == NULL )
		return 0;

	if(nPoints == 1)
	{
		if( lpptPoint )
			lpptPoint[0] = lppt[0];
		return 1;
	}

	if(nPoints == 2)
	{
		if( lpptPoint )
		{
			lpptPoint[0] = lppt[0];
			lpptPoint[1] = lppt[1];
		}
		return 2;
	}

	DWORD i, j;
	DWORD cPoints = nPoints;

	for( i = 1; i < nPoints ; i++ )
	{
		if( (lppt[i].x == lppt[i-1].x) && (lppt[i].y == lppt[i-1].y ))
		{
			cPoints--;
		}
	}

	LPPOINT lpptControl = new POINT [ cPoints ];

	j = 0;

	for( i = 1; i < nPoints ; i++ )
	{
		if( (lppt[i].x != lppt[i-1].x) || (lppt[i].y != lppt[i-1].y ) )
		{
			lpptControl[j].x = lppt[i-1].x;
			lpptControl[j].y = lppt[i-1].y;
			j++;
		}
	}

	lpptControl[cPoints -1 ].x = lppt[nPoints-1].x;	
	lpptControl[cPoints -1 ].y = lppt[nPoints-1].y;	

	double *t = new double [cPoints];//弦长	
	double F1, F2, G1, G2;	//权函数
	// P(t) = F1(t)*P1 + F2(t)*P2 + t1 * G1(t) * P'1 + t1 * G2(t) * P'2
	// F1(t) = 1 - 3 * (t/t1)^2 + 2 * (t/t1)^3
	// F2(t) = 3 * (/t1)^2 - 2 * (t/t1)^3
	// G1(t) = (t/t1) - 2 * (t/t1)^2 + (t/t1)^3
	// G2(t) = -(t/t1)^2 + (t/t1)^3
	//
	//切矢分量及中间变量
	double *Qx = new double [cPoints];	// Q = 3 * t(i) / t(i-1) * ( P(i) - P(i-1) ) + 3 * t(i-1) / t(i) * ( P(i+1) - P(i) )
	double *Qy = new double [cPoints];
	double *T = new double [3*cPoints-2];	 


	for( i = 0; i < cPoints-1 ; i++) // 计算弦长
		t[i] = sqrt((double)
		(lpptControl[i+1].x - lpptControl[i].x) * (lpptControl[i+1].x - lpptControl[i].x) 
		+ (lpptControl[i+1].y - lpptControl[i].y) * (lpptControl[i+1].y - lpptControl[i].y) 
		);

	//求矩阵

	for( i = 1; i < cPoints-1 ; i++)
	{
		T[i*3 - 1] = t[i];
		T[i*3    ] = 2.0 * ( t[i-1] + t[i]);
		T[i*3 + 1] = t[i-1];
	}

	for(i = 1; i < cPoints-1; i++)
	{
		Qx[i] = 3 * t[i]/t[i-1] * ( lpptControl[i].x - lpptControl[i-1].x ) + 3 * t[i-1]/t[i] * ( lpptControl[i+1].x - lpptControl[i].x );
		Qy[i] = 3 * t[i]/t[i-1] * ( lpptControl[i].y - lpptControl[i-1].y ) + 3 * t[i-1]/t[i] * ( lpptControl[i+1].y - lpptControl[i].y );
	}

	T[0] = 1.0; T[1] = 0.5;
	T[ 3*cPoints - 4 ] = 1.0; T[ 3*cPoints - 3] = 2.0;

	Qx[0] = 1.5 / t[0] * (lpptControl[1].x - lpptControl[0].x);	
	Qx[cPoints-1] = 3.0 / t[cPoints-2] * ( lpptControl[cPoints-1].x - lpptControl[cPoints-2].x);

	Qy[0] = 1.5 / t[0] * (lpptControl[1].y - lpptControl[0].y);	
	Qy[cPoints-1] = 3.0 / t[cPoints-2] * ( lpptControl[cPoints-1].y - lpptControl[cPoints-2].y);

	if( atrde(T, (long)cPoints, (long)(3*cPoints-2), Qx) <= 0) 
	{
		delete T;
		delete t;	
		delete Qx;
		delete Qy;	
		return FALSE;
	}

	if( atrde(T, (long)cPoints, (long)(3*cPoints-2), Qy) <= 0)
	{
		delete T;
		delete t;	
		delete Qx;
		delete Qy;	
		return FALSE;
	}

	delete T;

	POINT *ppp;

	long pps; 

	if( bClose )
		pps = cPoints * SEGMENT + cPoints + 1;
	else
		pps = ( cPoints - 1 ) * SEGMENT + cPoints ;

	ppp = new POINT [ pps];

	ppp[0] = lpptControl[0];

	pps = 0;

	//MoveToEx(hDC, lpptControl[0].x, lpptControl[0].y, NULL);
	double x, y;
	double U = 1.0 / (double)(SEGMENT+1);
	for(i = 0 ;  i<cPoints-1; i++)
	{		
		for(j = 0; j < SEGMENT; j++)
		{
			pps ++;
			U *= (j+1.0);
			F1 = 1.0 - 3.0 * U*U + 2 * U*U*U ;
			F2 = 3 * U*U - 2 * U*U*U ;
			G1 = U - 2 * U*U + U*U*U ;
			G2 = -U*U + U*U*U;

			x = F1 * lpptControl[i].x + F2 * lpptControl[i+1].x + t[i] * G1 * Qx[i] + t[i] * G2 * Qx[i+1];
			y = F1 * lpptControl[i].y + F2 * lpptControl[i+1].y + t[i] * G1 * Qy[i] + t[i] * G2 * Qy[i+1];

			ppp[pps].x = x;
			ppp[pps].y = y;

			U = 1.0 / (double)( SEGMENT + 1 );
		}
		pps ++;
		ppp[pps] = lpptControl[i+1];
	}
	if( bClose)
	{
		double te = sqrt((double)
			(lpptControl[0].x - lpptControl[cPoints-1].x) * (lpptControl[0].x - lpptControl[cPoints-1].x) 
			+ (lpptControl[0].y - lpptControl[cPoints-1].y) * (lpptControl[0].y - lpptControl[cPoints-1].y) 
			);

		U = 1.0 / (double)( SEGMENT + 1 );

		for(j = 0; j < SEGMENT; j++)
		{
			U *= (j+1.0);
			F1 = 1.0 - 3.0 * U*U + 2 * U*U*U ;
			F2 = 3 * U*U - 2 * U*U*U ;
			G1 = U - 2 * U*U + U*U*U ;
			G2 = -U*U + U*U*U;

			x = F1 * lpptControl[cPoints-1].x + F2 * lpptControl[0].x + te * G1 * Qx[cPoints-1] + te * G2 * Qx[0];
			y = F1 * lpptControl[cPoints-1].y + F2 * lpptControl[0].y + te * G1 * Qy[cPoints-1] + te * G2 * Qy[0];

			pps ++;
			ppp[pps].x = x;
			ppp[pps].y = y;

			U = 1.0 / (double)( SEGMENT + 1 );
		}

		pps++;

		ppp[pps] = lpptControl[0];
	}

	if( lpptPoint != NULL )
	{
		for (long i=0; i<pps+1; i++)
			lpptPoint[i] = ppp[i];
	}

	delete ppp;
	delete t;	
	delete Qx;
	delete Qy;
	delete lpptControl;

	return (pps+1);
}
//功能：画B样条曲线
//边界条件：自由端即两端点的二阶导数为0
//参数：lppt-控制点， 
//		nPoints-控制点数， 
//		bClose-是否封闭,值为：
//				TRUE 封闭，
//				FALSE 不封闭，
//				缺省是不封闭
extern "C" 
BOOL PolySpline3(HDC hDC, LPPOINT lppt, DWORD nPoints, BOOL bClose)
{
	if(nPoints <= 1 || lppt == NULL )
		return FALSE;
	if(nPoints == 2)
	{
		MoveToEx( hDC, lppt[0].x, lppt[0].y, NULL);
		LineTo  ( hDC, lppt[1].x, lppt[1].y);
		return TRUE;
	}

	DWORD i, j;
	DWORD cPoints = nPoints;

	for( i = 1; i < nPoints ; i++ )
	{
		if( (lppt[i].x == lppt[i-1].x) && (lppt[i].y == lppt[i-1].y ))
		{
			cPoints--;
		}
	}
	
	LPPOINT lpptControl = new POINT [ cPoints ];

	j = 0;

	for( i = 1; i < nPoints ; i++ )
	{
		if( (lppt[i].x != lppt[i-1].x) || (lppt[i].y != lppt[i-1].y ) )
		{
			lpptControl[j].x = lppt[i-1].x;
			lpptControl[j].y = lppt[i-1].y;
			j++;
		}
	}

	lpptControl[cPoints -1 ].x = lppt[nPoints-1].x;	
	lpptControl[cPoints -1 ].y = lppt[nPoints-1].y;	

	double *t = new double [cPoints];//弦长	
	double F1, F2, G1, G2;	//权函数
							// P(t) = F1(t)*P1 + F2(t)*P2 + t1 * G1(t) * P'1 + t1 * G2(t) * P'2
							// F1(t) = 1 - 3 * (t/t1)^2 + 2 * (t/t1)^3
							// F2(t) = 3 * (/t1)^2 - 2 * (t/t1)^3
							// G1(t) = (t/t1) - 2 * (t/t1)^2 + (t/t1)^3
							// G2(t) = -(t/t1)^2 + (t/t1)^3
						    //
		                             //切矢分量及中间变量
	double *Qx = new double [cPoints];	// Q = 3 * t(i) / t(i-1) * ( P(i) - P(i-1) ) + 3 * t(i-1) / t(i) * ( P(i+1) - P(i) )
	double *Qy = new double [cPoints];
	double *T = new double [3*cPoints-2];	 
	

	for( i = 0; i < cPoints-1 ; i++) // 计算弦长
		t[i] = sqrt((double)
					(lpptControl[i+1].x - lpptControl[i].x) * (lpptControl[i+1].x - lpptControl[i].x) 
					+ (lpptControl[i+1].y - lpptControl[i].y) * (lpptControl[i+1].y - lpptControl[i].y) 
				);
	
	//求矩阵
	
	for( i = 1; i < cPoints-1 ; i++)
	{
		T[i*3 - 1] = t[i];
		T[i*3    ] = 2.0 * ( t[i-1] + t[i]);
		T[i*3 + 1] = t[i-1];
	}

	for(i = 1; i < cPoints-1; i++)
	{
		Qx[i] = 3 * t[i]/t[i-1] * ( lpptControl[i].x - lpptControl[i-1].x ) + 3 * t[i-1]/t[i] * ( lpptControl[i+1].x - lpptControl[i].x );
		Qy[i] = 3 * t[i]/t[i-1] * ( lpptControl[i].y - lpptControl[i-1].y ) + 3 * t[i-1]/t[i] * ( lpptControl[i+1].y - lpptControl[i].y );
	}
	
	T[0] = 1.0; T[1] = 0.5;
	T[ 3*cPoints - 4 ] = 1.0; T[ 3*cPoints - 3] = 2.0;

	Qx[0] = 1.5 / t[0] * (lpptControl[1].x - lpptControl[0].x);	
	Qx[cPoints-1] = 3.0 / t[cPoints-2] * ( lpptControl[cPoints-1].x - lpptControl[cPoints-2].x);

	Qy[0] = 1.5 / t[0] * (lpptControl[1].y - lpptControl[0].y);	
	Qy[cPoints-1] = 3.0 / t[cPoints-2] * ( lpptControl[cPoints-1].y - lpptControl[cPoints-2].y);
	
	if( atrde(T, (long)cPoints, (long)(3*cPoints-2), Qx) <= 0) 
	{
		delete T;
		delete t;	
		delete Qx;
		delete Qy;	
		return FALSE;
	}

	if( atrde(T, (long)cPoints, (long)(3*cPoints-2), Qy) <= 0)
	{
		delete T;
		delete t;	
		delete Qx;
		delete Qy;	
		return FALSE;
	}

	delete T;

	POINT *ppp;

	long pps; 

	if( bClose )
		pps = cPoints * SEGMENT + cPoints + 1;
	else
		pps = ( cPoints - 1 ) * SEGMENT + cPoints ;

	ppp = new POINT [ pps];

	ppp[0] = lpptControl[0];

	pps = 0;
	
	//MoveToEx(hDC, lpptControl[0].x, lpptControl[0].y, NULL);
	double x, y;
	double U = 1.0 / (double)(SEGMENT+1);
	for(i = 0 ;  i<cPoints-1; i++)
	{		
		for(j = 0; j < SEGMENT; j++)
		{
			pps ++;
			U *= (j+1.0);
            F1 = 1.0 - 3.0 * U*U + 2 * U*U*U ;
			F2 = 3 * U*U - 2 * U*U*U ;
			G1 = U - 2 * U*U + U*U*U ;
			G2 = -U*U + U*U*U;

			x = F1 * lpptControl[i].x + F2 * lpptControl[i+1].x + t[i] * G1 * Qx[i] + t[i] * G2 * Qx[i+1];
			y = F1 * lpptControl[i].y + F2 * lpptControl[i+1].y + t[i] * G1 * Qy[i] + t[i] * G2 * Qy[i+1];

			ppp[pps].x = (int)x;
			ppp[pps].y = (int)y;

			//LineTo(hDC, (int)x, (int)y);
			U = 1.0 / (double)( SEGMENT + 1 );
		}
		pps ++;
		ppp[pps] = lpptControl[i+1];

		//LineTo(hDC, lpptControl[i+1].x, lpptControl[i+1].y);
	}
	if( bClose)
	{
		double te = sqrt((double)
			(lpptControl[0].x - lpptControl[cPoints-1].x) * (lpptControl[0].x - lpptControl[cPoints-1].x) 
			+ (lpptControl[0].y - lpptControl[cPoints-1].y) * (lpptControl[0].y - lpptControl[cPoints-1].y) 
			);

		U = 1.0 / (double)( SEGMENT + 1 );

		for(j = 0; j < SEGMENT; j++)
		{
			U *= (j+1.0);
			F1 = 1.0 - 3.0 * U*U + 2 * U*U*U ;
			F2 = 3 * U*U - 2 * U*U*U ;
			G1 = U - 2 * U*U + U*U*U ;
			G2 = -U*U + U*U*U;

			x = F1 * lpptControl[cPoints-1].x + F2 * lpptControl[0].x + te * G1 * Qx[cPoints-1] + te * G2 * Qx[0];
			y = F1 * lpptControl[cPoints-1].y + F2 * lpptControl[0].y + te * G1 * Qy[cPoints-1] + te * G2 * Qy[0];

			pps ++;
			ppp[pps].x = (int)x;
			ppp[pps].y = (int)y;

			U = 1.0 / (double)( SEGMENT + 1 );
		}

		pps++;

		ppp[pps] = lpptControl[0];
			
		//LineTo(hDC, lpptControl[0].x, lpptControl[0].y);
	}

	Polyline(hDC, ppp, pps+1);

	delete ppp;
	delete t;	
	delete Qx;
	delete Qy;
	delete lpptControl;
	return TRUE;
}

// 三对角线方程组的求解
extern "C" 
int atrde(double *b,long n,long m,double *d)
{
	long k, j;
	double s;

	if ( m != (3*n-2) )
	{
		AfxMessageBox("数据的个数不对！");
		return(-2);
	}

	for (k = 0; k <= n-2; k++ )
	{
		j = 3 * k;
		s = b[j];
		if ( fabs(s)+1.0 == 1.0 )
		{ 
			AfxMessageBox("失败！"); //主元素的绝对值太小或为零
			return(0);
		}

		b[j+1] = b[j+1] / s;
		d[k] = d[k] / s;
		b[j+3] = b[j+3] - b[j+2] * b[j+1];
		d[k+1] = d[k+1] - b[j+2] * d[k];
	}

	s = b[3*n-3];

	if ( fabs(s)+1.0 == 1.0 )
	{ 
		AfxMessageBox("失败！");  //主元素的绝对值太小或为零
		return(0);
	}

	d[n-1] = d[n-1] / s;

	for ( k = n-2; k >= 0; k-- )
		d[k] = d[k] - b[3*k+1] * d[k+1];

	return(2);
}

//等距线
//
extern "C" 
void Parallelline(HDC hDC, LPPOINT lppt, DWORD nPoints, BOOL bInner, double dWidth)
{
	// 两点式直线方程为
	// (x2-x1) * (y-y1) = (y2-y1) * (x-x1)
	// (y2-y1)*x + (x1-x2) *y  -[ y1*(x1-x2) + x1*(y2-y1) ] = 0
	//   A            B                      C
	// 设原始方程为
	//   A*X + B * Y + C = 0
	// 则等距方程为 
	//	 A*X + B * Y + C2 = 0; 
	//   
	// 其中
	//   A = ( y2 - y1 )
	//   B = ( x1 - x2 )
	//   C = - ( y1*B + A * x1 );
	//
	//   D = | C - C2 | / SQRT(A^2+B^2)
	//  | C - C2 |= D * SQRT(A^2 + B^2)
	//
	//   C2 > C 上
	//   C2 = D * SQRT(A^2 + B^2 ) + C
	//   C2 < C 下
	//   C2 = C - D * SQRT(A^2 + B^2 )
	//   
	//   D = 两线间距离
	//
	// 当是首点和尾点时，交点是等距线与其垂线的交点
	// 当两点之间的等距线是一条时，交点是等距线与其垂线的交点
	// 

	DWORD i, j;
	DWORD cPoints = nPoints;

	for( i = 1; i < nPoints ; i++ ) 
	{
		if( (lppt[i].x == lppt[i-1].x) && (lppt[i].y == lppt[i-1].y ))// 去掉相同点
			cPoints--;
	}

	if( cPoints <= 1 )
		return;
	
	LPPOINT lpptControl = new POINT [ cPoints ];

	j = 1;

	lpptControl[0].x = lppt[0].x;
	lpptControl[0].y = lppt[0].y;

	for( i = 1; i < nPoints ; i++ ) // 去掉相同点
	{
		if( (lppt[i].x == lppt[i-1].x) && (lppt[i].y == lppt[i-1].y ) )
			continue;
		lpptControl[j].x = lppt[i].x;
		lpptControl[j].y = lppt[i].y;
		j++;		
	}
	
	lpptControl[cPoints -1 ].x = lppt[nPoints-1].x;	
	lpptControl[cPoints -1 ].y = lppt[nPoints-1].y;

	LPPOINT points = new POINT[ cPoints ]; //等距线控制点

	double A1, B1, C1, A2, B2, C2;
	double D = dWidth ;
	double A, B, C;		
	
	for( i = 1; i < cPoints ; i++ )
	{
		A = lpptControl[i].y - lpptControl[i-1].y ;
		B = lpptControl[i-1].x - lpptControl[i].x;
		C = - (  A * lpptControl[i-1].x + B * lpptControl[i-1].y );
		
		if( bInner )
			C2 = D * sqrt(A*A + B*B ) + C;
		else
			C2 = C - D * sqrt(A*A + B*B );			
		
		A2 = A;
		B2 = B;
		
		if( i == 1)
		{
			A1 = -B2;
			B1 = A2;
			C1 = - A1 * lpptControl[i-1].x - B1 * lpptControl[i-1].y;//?
		}
			
		if( fabs( A1 * B2 - A2 * B1 ) < DBL_EPSILON )
		{
			A1 = -B2;
			B1 = A2;
			C1 = - A1 * lpptControl[i-1].x - B1 * lpptControl[i-1].y;//?
		}

		//求交点
		
		points[i-1].x = ( B1 *C2 - B2 * C1 ) / ( A1 * B2 - A2 * B1 );
		points[i-1].y = ( C1 *A2 - C2 * A1 ) / ( A1 * B2 - A2 * B1 );
		//
		A1 = A2;
		B1 = B2;
		C1 = C2;
	}
	

	A2 = -B1;
	B2 = A1;
	C2 = -A2 * lpptControl[ cPoints - 1 ].x - B2 * lpptControl[cPoints - 1].y;//?

	points[cPoints - 1].x = ( B1 *C2 - B2 * C1 ) / ( A1 * B2 - A2 * B1 );
	points[cPoints - 1].y = ( C1 *A2 - C2 * A1 ) / ( A1 * B2 - A2 * B1 );
	
	Polyline(hDC, points, cPoints);
	
	delete lpptControl;
	delete points;	
}

extern "C" 
void Parallelgon(HDC hDC, LPPOINT lppt, DWORD nPoints, BOOL bInner, double dWidth)
{
	// 两点式直线方程为
	// (x2-x1) * (y-y1) = (y2-y1) * (x-x1)
	// (y2-y1)*x + (x1-x2) *y  -[ y1*(x1-x2) + x1*(y2-y1) ] = 0
	//   A            B                      C
	// 设原始方程为
	//   A*X + B * Y + C = 0
	// 则等距方程为 
	//	 A*X + B * Y + C2 = 0; 
	//   
	// 其中
	//   A = ( y2 - y1 )
	//   B = ( x1 - x2 )
	//   C = - ( y1*B + A * x1 );
	//
	//   D = | C - C2 | / SQRT(A^2+B^2)
	//  | C - C2 |= D * SQRT(A^2 + B^2)
	//
	//   C2 > C 上
	//   C2 = D * SQRT(A^2 + B^2 ) + C
	//   C2 < C 下
	//   C2 = C - D * SQRT(A^2 + B^2 )
	//   
	//   D = 两线间距离
	//
	
	DWORD i, j;
	DWORD cPoints = nPoints;

	for( i = 1; i < nPoints ; i++ )
	{
		if( (lppt[i].x == lppt[i-1].x) && (lppt[i].y == lppt[i-1].y ))
			cPoints--;
		if(i < nPoints-1 )
		{
			if( (lppt[i].x == lppt[i-1].x) && (lppt[i].x == lppt[i+1].x ))
				cPoints--;
			if( (lppt[i].y == lppt[i-1].y) && (lppt[i].y == lppt[i+1].y ))
				cPoints--;
		}
	}
	
	if(cPoints < 3 )
	{
		Parallelline(hDC, lppt, nPoints, bInner);
		return ;
	}

	LPPOINT lpptControl = new POINT [ cPoints ];

	j = 1;

	lpptControl[0].x = lppt[0].x;
	lpptControl[0].y = lppt[0].y;

	for( i = 1; i < nPoints ; i++ )
	{
		if( (lppt[i].x == lppt[i-1].x) && (lppt[i].y == lppt[i-1].y ) )
			continue;
		if(i < nPoints-1 )
		{
			if( (lppt[i].x == lppt[i-1].x) && (lppt[i].x == lppt[i+1].x ))
				continue;
			if( (lppt[i].y == lppt[i-1].y) && (lppt[i].y == lppt[i+1].y ))
				continue;			
		}
		lpptControl[j].x = lppt[i].x;
		lpptControl[j].y = lppt[i].y;
		j++;		
	}
	
	lpptControl[cPoints -1 ].x = lppt[nPoints-1].x;	
	lpptControl[cPoints -1 ].y = lppt[nPoints-1].y;

	LPPOINT points = new POINT[ cPoints + 1 ]; //等距线控制点

	double A1, B1, C1, A2, B2, C2;
	double D = dWidth ;
	double A, B, C;
	double a, b, c;

	for( i = 1; i <= cPoints ; i++ )
	{
		if( i == cPoints )
		{
			A = lpptControl[0].y - lpptControl[i-1].y ;
			B = lpptControl[i-1].x - lpptControl[0].x;
			C = - (  A * lpptControl[i-1].x + B * lpptControl[i-1].y );
		}
		else
		{
			A = lpptControl[i].y - lpptControl[i-1].y ;
			B = lpptControl[i-1].x - lpptControl[i].x;
			C = - (  A * lpptControl[i-1].x + B * lpptControl[i-1].y );
		}
		
		if( bInner )
			C2 = D * sqrt(A*A + B*B ) + C;
		else
			C2 = C - D * sqrt(A*A + B*B );			
		
		A2 = A;
		B2 = B;
		
		if( i == 1)
		{
			A1 = -B2;
			B1 = A2;
			C1 = - A1 * lpptControl[i-1].x - B1 * lpptControl[i-1].y;//?

			a = A2;
			b = B2;
			c = C2;
		}
		
		if( fabs( A1 * B2 - A2 * B1 ) < DBL_EPSILON )
		{
			A1 = -B2;
			B1 = A2;
			C1 = - A1 * lpptControl[i-1].x - B1 * lpptControl[i-1].y;//?
			
			a = A2;
			b = B2;
			c = C2;
		}

		//求交点
		points[i-1].x = ( B1 *C2 - B2 * C1 ) / ( A1 * B2 - A2 * B1 );
		points[i-1].y = ( C1 *A2 - C2 * A1 ) / ( A1 * B2 - A2 * B1 );
		//
		A1 = A2;
		B1 = B2;
		C1 = C2;			
	}
	

	A2 = a;
	B2 = b;
	C2 = c;

	points[0].x = ( B1 *C2 - B2 * C1 ) / ( A1 * B2 - A2 * B1 );
	points[0].y = ( C1 *A2 - C2 * A1 ) / ( A1 * B2 - A2 * B1 );

	points[cPoints] = points[0];
	
	
	Polyline(hDC, points, cPoints+1);
	
	delete lpptControl;
	delete points;	
}

//画旋转文本
extern "C" 
void DrawRotatedText(HDC hDC, const LPCTSTR str, LPCRECT rect, 
                     double angle, UINT nOptions)
{
   // convert angle to radian
   double pi = 3.141592654;
   double radian = pi / 180.0 * angle;
 
   // get the center of a not-rotated text
   SIZE TextSize;
   GetTextExtentPoint32(hDC, str, strlen(str), &TextSize);
   
   POINT center;
   center.x = TextSize.cx / 2;
   center.y = TextSize.cy / 2;
 
   // now calculate the center of the rotated text
   POINT rcenter;
   rcenter.x = long(cos(radian) * center.x - sin(radian) * center.y);
   rcenter.y = long(sin(radian) * center.x + cos(radian) * center.y);
 
   // finally draw the text and move it to the center of the rectangle
   SetTextAlign(hDC, TA_BOTTOM);
   SetBkMode(hDC, TRANSPARENT);
   ExtTextOut(hDC, rect->left + labs( rect->right - rect->left ) / 2 - rcenter.x, 
                   rect->top + labs(rect->bottom - rect->top ) / 2 + rcenter.y,
                   nOptions, rect, str, strlen(str), NULL);
}  

extern "C" 
void CalcArrowRgnSpline(LPPOINT lppt, DWORD nPoints,
						double Len, LPPOINT pt, BOOL bEnd)
{
	if(nPoints <= 1 || lppt == NULL )
		return;
	
	int i, j;
	DWORD cPoints = nPoints;

	for( i = 1; i < nPoints ; i++ )
	{
		if( (lppt[i].x == lppt[i-1].x) && (lppt[i].y == lppt[i-1].y ))
		{
			cPoints--;
		}
	}
	
	LPPOINT lpptControl = new POINT [ cPoints ];

	j = 0;

	for( i = 1; i < nPoints ; i++ )
	{
		if( (lppt[i].x != lppt[i-1].x) || (lppt[i].y != lppt[i-1].y ) )
		{
			lpptControl[j].x = lppt[i-1].x;
			lpptControl[j].y = lppt[i-1].y;
			j++;
		}
	}

	lpptControl[cPoints -1 ].x = lppt[nPoints-1].x;	
	lpptControl[cPoints -1 ].y = lppt[nPoints-1].y;	

	double *t = new double [cPoints];//弦长	
	double F1, F2, G1, G2;	//权函数
							// P(t) = F1(t)*P1 + F2(t)*P2 + t1 * G1(t) * P'1 + t1 * G2(t) * P'2
							// F1(t) = 1 - 3 * (t/t1)^2 + 2 * (t/t1)^3
							// F2(t) = 3 * (/t1)^2 - 2 * (t/t1)^3
							// G1(t) = (t/t1) - 2 * (t/t1)^2 + (t/t1)^3
							// G2(t) = -(t/t1)^2 + (t/t1)^3
						    //
		                             //切矢分量及中间变量
	double *Qx = new double [cPoints];	// Q = 3 * t(i) / t(i-1) * ( P(i) - P(i-1) ) + 3 * t(i-1) / t(i) * ( P(i+1) - P(i) )
	double *Qy = new double [cPoints];
	double *T = new double [3*cPoints-2];	 
	

	for( i = 0; i < cPoints-1 ; i++) // 计算弦长
		t[i] = sqrt((double)
					(lpptControl[i+1].x - lpptControl[i].x) * (lpptControl[i+1].x - lpptControl[i].x) 
					+ (lpptControl[i+1].y - lpptControl[i].y) * (lpptControl[i+1].y - lpptControl[i].y) 
				);
	
	//求矩阵
	
	for( i = 1; i < cPoints-1 ; i++)
	{
		T[i*3 - 1] = t[i];
		T[i*3    ] = 2.0 * ( t[i-1] + t[i]);
		T[i*3 + 1] = t[i-1];
	}

	for(i = 1; i < cPoints-1; i++)
	{
		Qx[i] = 3 * t[i]/t[i-1] * ( lpptControl[i].x - lpptControl[i-1].x ) + 3 * t[i-1]/t[i] * ( lpptControl[i+1].x - lpptControl[i].x );
		Qy[i] = 3 * t[i]/t[i-1] * ( lpptControl[i].y - lpptControl[i-1].y ) + 3 * t[i-1]/t[i] * ( lpptControl[i+1].y - lpptControl[i].y );
	}
	
	T[0] = 1.0; T[1] = 0.5;
	T[ 3*cPoints - 4 ] = 1.0; T[ 3*cPoints - 3] = 2.0;

	Qx[0] = 1.5 / t[0] * (lpptControl[1].x - lpptControl[0].x);	
	Qx[cPoints-1] = 3.0 / t[cPoints-2] * ( lpptControl[cPoints-1].x - lpptControl[cPoints-2].x);

	Qy[0] = 1.5 / t[0] * (lpptControl[1].y - lpptControl[0].y);	
	Qy[cPoints-1] = 3.0 / t[cPoints-2] * ( lpptControl[cPoints-1].y - lpptControl[cPoints-2].y);
	
	if( atrde(T, (long)cPoints, (long)(3*cPoints-2), Qx) <= 0) 
	{
		delete T;
		delete t;	
		delete Qx;
		delete Qy;	
		return ;
	}

	if( atrde(T, (long)cPoints, (long)(3*cPoints-2), Qy) <= 0)
	{
		delete T;
		delete t;	
		delete Qx;
		delete Qy;	
		return;
	}

	delete T;

	double x, y;
	
	if( bEnd )
	{
		double temp = 0;
		
		double U = 1.0 / (double)(SEGMENT+1);
		for(i = cPoints-1 ;  i > 0 ; i--)
		{		
			for(j = SEGMENT-1; j >= 0; j--)
			{
				U *= (j+1.0);
				F1 = 1.0 - 3.0 * U*U + 2 * U*U*U ;
				F2 = 3 * U*U - 2 * U*U*U ;
				G1 = U - 2 * U*U + U*U*U ;
				G2 = -U*U + U*U*U;
				
				x = F1 * lpptControl[i-1].x + F2 * lpptControl[i].x + t[i-1] * G1 * Qx[i-1] + t[i-1] * G2 * Qx[i];
				y = F1 * lpptControl[i-1].y + F2 * lpptControl[i].y + t[i-1] * G1 * Qy[i-1] + t[i-1] * G2 * Qy[i];
				
				temp = sqrt( (lpptControl[cPoints - 1].x - x) * (lpptControl[cPoints-1].x - x) + (lpptControl[cPoints-1].y - y) * (lpptControl[cPoints-1].y - y) );

				if( Len <= temp )
				{
					j = -1;
					i = 0;
					pt[1].x = (int)x;
					pt[1].y = (int)y;
				}
								
				U = 1.0 / (double)( SEGMENT + 1 );
			}

			if( Len <= temp )
				continue;
			
			x = lpptControl[i-1].x;
			y = lpptControl[i-1].y;

			temp = sqrt( (lpptControl[cPoints - 1].x - x) * (lpptControl[cPoints-1].x - x) + (lpptControl[cPoints-1].y - y) * (lpptControl[cPoints-1].y - y) );
			
			if( Len <= temp )
			{
				j = -1;
				i = 0;
				pt[1].x = (int)x;
				pt[1].y = (int)y;
			}
			
		}
	}
	else
	{		
		double temp = 0;
		
		double U = 1.0 / (double)(SEGMENT+1);
		for(i = 0 ;  i< cPoints-1; i++)
		{		
			for(j = 0; j < SEGMENT; j++)
			{
				U *= (j+1.0);
				F1 = 1.0 - 3.0 * U*U + 2 * U*U*U ;
				F2 = 3 * U*U - 2 * U*U*U ;
				G1 = U - 2 * U*U + U*U*U ;
				G2 = -U*U + U*U*U;
				
				x = F1 * lpptControl[i].x + F2 * lpptControl[i+1].x + t[i] * G1 * Qx[i] + t[i] * G2 * Qx[i+1];
				y = F1 * lpptControl[i].y + F2 * lpptControl[i+1].y + t[i] * G1 * Qy[i] + t[i] * G2 * Qy[i+1];				
				
				temp = sqrt( (lpptControl[0].x - x) * (lpptControl[0].x - x) + (lpptControl[0].y - y) * (lpptControl[0].y - y) );

				if( Len <= temp )
				{
					j = SEGMENT;
					i = cPoints;
					pt[1].x = (int)x;
					pt[1].y = (int)y;
				}
								
				U = 1.0 / (double)( SEGMENT + 1 );
			}

			if( Len <= temp )
				continue;
			
			x = lpptControl[i+1].x;
			y = lpptControl[i+1].y;

			temp = sqrt( (lpptControl[0].x - x) * (lpptControl[0].x - x) + (lpptControl[0].y - y) * (lpptControl[0].y - y) );
			
			if( Len <= temp )
			{
				j = SEGMENT;
				i = cPoints;
				pt[1].x = (int)x;
				pt[1].y = (int)y;
			}
			
		}
	}
	delete t;	
	delete Qx;
	delete Qy;	
	delete lpptControl;
	return ;
}

extern "C" 
LPPOINT GetSPPoints(CPoint2D* lppt, DWORD nPoints, int &num, BOOL bClose)
{
	if(nPoints <= 1 || lppt == NULL )
		return NULL;
	if(nPoints == 2)
	{
		LPPOINT ppp;
		ppp = new POINT [ 2 ];

		ppp[0].x = lppt[0].x;
		ppp[0].y = lppt[0].y;
		ppp[1].x = lppt[0].x;
		ppp[1].y = lppt[0].y;

		num = 2;

		return ppp;
	}

	DWORD i, j;
	DWORD cPoints = nPoints;

	for( i = 1; i < nPoints ; i++ )
	{
		if( (lppt[i].x == lppt[i-1].x) && (lppt[i].y == lppt[i-1].y ))
		{
			cPoints--;
		}
	}
	
	LPPOINT lpptControl = new POINT [ cPoints ];

	j = 0;

	for( i = 1; i < nPoints ; i++ )
	{
		if( (lppt[i].x != lppt[i-1].x) || (lppt[i].y != lppt[i-1].y ) )
		{
			lpptControl[j].x = lppt[i-1].x;
			lpptControl[j].y = lppt[i-1].y;
			j++;
		}
	}

	lpptControl[cPoints -1 ].x = lppt[nPoints-1].x;	
	lpptControl[cPoints -1 ].y = lppt[nPoints-1].y;	

	double *t = new double [cPoints];//弦长	
	double F1, F2, G1, G2;	//权函数
							// P(t) = F1(t)*P1 + F2(t)*P2 + t1 * G1(t) * P'1 + t1 * G2(t) * P'2
							// F1(t) = 1 - 3 * (t/t1)^2 + 2 * (t/t1)^3
							// F2(t) = 3 * (/t1)^2 - 2 * (t/t1)^3
							// G1(t) = (t/t1) - 2 * (t/t1)^2 + (t/t1)^3
							// G2(t) = -(t/t1)^2 + (t/t1)^3
						    //
		                             //切矢分量及中间变量
	double *Qx = new double [cPoints];	// Q = 3 * t(i) / t(i-1) * ( P(i) - P(i-1) ) + 3 * t(i-1) / t(i) * ( P(i+1) - P(i) )
	double *Qy = new double [cPoints];
	double *T = new double [3*cPoints-2];	 
	

	for( i = 0; i < cPoints-1 ; i++) // 计算弦长
		t[i] = sqrt((double)
					(lpptControl[i+1].x - lpptControl[i].x) * (lpptControl[i+1].x - lpptControl[i].x) 
					+ (lpptControl[i+1].y - lpptControl[i].y) * (lpptControl[i+1].y - lpptControl[i].y) 
				);
	
	//求矩阵
	
	for( i = 1; i < cPoints-1 ; i++)
	{
		T[i*3 - 1] = t[i];
		T[i*3    ] = 2.0 * ( t[i-1] + t[i]);
		T[i*3 + 1] = t[i-1];
	}

	for(i = 1; i < cPoints-1; i++)
	{
		Qx[i] = 3 * t[i]/t[i-1] * ( lpptControl[i].x - lpptControl[i-1].x ) + 3 * t[i-1]/t[i] * ( lpptControl[i+1].x - lpptControl[i].x );
		Qy[i] = 3 * t[i]/t[i-1] * ( lpptControl[i].y - lpptControl[i-1].y ) + 3 * t[i-1]/t[i] * ( lpptControl[i+1].y - lpptControl[i].y );
	}
	
	T[0] = 1.0; T[1] = 0.5;
	T[ 3*cPoints - 4 ] = 1.0; T[ 3*cPoints - 3] = 2.0;

	Qx[0] = 1.5 / t[0] * (lpptControl[1].x - lpptControl[0].x);	
	Qx[cPoints-1] = 3.0 / t[cPoints-2] * ( lpptControl[cPoints-1].x - lpptControl[cPoints-2].x);

	Qy[0] = 1.5 / t[0] * (lpptControl[1].y - lpptControl[0].y);	
	Qy[cPoints-1] = 3.0 / t[cPoints-2] * ( lpptControl[cPoints-1].y - lpptControl[cPoints-2].y);
	
	if( atrde(T, (long)cPoints, (long)(3*cPoints-2), Qx) <= 0) 
	{
		delete T;
		delete t;	
		delete Qx;
		delete Qy;	
		num = -1;
		return NULL;
	}

	if( atrde(T, (long)cPoints, (long)(3*cPoints-2), Qy) <= 0)
	{
		delete T;
		delete t;	
		delete Qx;
		delete Qy;	
		return NULL;
	}

	delete T;

	long pps; 

	if( bClose )
		pps = cPoints * SEGMENT + cPoints + 1;
	else
		pps = ( cPoints - 1 ) * SEGMENT + cPoints ;

	LPPOINT ppp;
	ppp = new POINT [ pps];

	ppp[0] = lpptControl[0];

	pps = 0;
	
	double x, y;
	double U = 1.0 / (double)(SEGMENT+1);
	for(i = 0 ;  i<cPoints-1; i++)
	{		
		for(j = 0; j < SEGMENT; j++)
		{
			pps ++;
			U *= (j+1.0);
            F1 = 1.0 - 3.0 * U*U + 2 * U*U*U ;
			F2 = 3 * U*U - 2 * U*U*U ;
			G1 = U - 2 * U*U + U*U*U ;
			G2 = -U*U + U*U*U;

			x = F1 * lpptControl[i].x + F2 * lpptControl[i+1].x + t[i] * G1 * Qx[i] + t[i] * G2 * Qx[i+1];
			y = F1 * lpptControl[i].y + F2 * lpptControl[i+1].y + t[i] * G1 * Qy[i] + t[i] * G2 * Qy[i+1];

			ppp[pps].x = (int)x;
			ppp[pps].y = (int)y;

			U = 1.0 / (double)( SEGMENT + 1 );
		}
		pps ++;
		ppp[pps] = lpptControl[i+1];

	}
	if( bClose)
	{
		double te = sqrt((double)
			(lpptControl[0].x - lpptControl[cPoints-1].x) * (lpptControl[0].x - lpptControl[cPoints-1].x) 
			+ (lpptControl[0].y - lpptControl[cPoints-1].y) * (lpptControl[0].y - lpptControl[cPoints-1].y) 
			);

		U = 1.0 / (double)( SEGMENT + 1 );

		for(j = 0; j < SEGMENT; j++)
		{
			U *= (j+1.0);
			F1 = 1.0 - 3.0 * U*U + 2 * U*U*U ;
			F2 = 3 * U*U - 2 * U*U*U ;
			G1 = U - 2 * U*U + U*U*U ;
			G2 = -U*U + U*U*U;

			x = F1 * lpptControl[cPoints-1].x + F2 * lpptControl[0].x + te * G1 * Qx[cPoints-1] + te * G2 * Qx[0];
			y = F1 * lpptControl[cPoints-1].y + F2 * lpptControl[0].y + te * G1 * Qy[cPoints-1] + te * G2 * Qy[0];

			pps ++;
			ppp[pps].x = (int)x;
			ppp[pps].y = (int)y;

			U = 1.0 / (double)( SEGMENT + 1 );
		}

		pps++;

		ppp[pps] = lpptControl[0];
			
	}

	num = pps+1;
	
	delete []t;	
	delete []Qx;
	delete []Qy;
	delete []lpptControl;
	return ppp;
}

extern "C" 
BOOL DrawTitle(HDC hDC, LPCTSTR str , LOGFONT *lgfn)//画标题
{
	int n ;

	POINT *pt;
	BYTE *type;

	n = GetPath(hDC, NULL, NULL, 0);

	if( n <= 0 )
		return FALSE;

	pt = new POINT [n];

	type = new BYTE [n];

	if( !pt || !type)
		return FALSE;

	GetPath(hDC, pt, type, n);

	int i;
	
	double tmp = 0;
	double L;
	double lamda;	
	double fnHeight;
	double LenSum = 0;//总长
	HFONT hFont, hOldFont;
	TEXTMETRIC tm;

	LPCTSTR ls1 = str;                     //文本内容
	int cd = strlen(ls1);                   //文本字符长度
	int nStrLen = 0;
	char c1, c2;	
	while(cd > 0 )  //计算所有的字符个数
	{
		c1=*ls1;
		c2=*(ls1+1);
		if (c1 < 0 && c2 < 0127)  //如果是一个汉字
		{
			ls1=ls1 + 2 ;        //跳过汉字指向下一个字符位置
			cd = cd - 2 ;                  //字符数减2
			nStrLen ++;			
		}
		else //如果是一个西文字符
		{
			ls1++;                   //跳过这一个字符到下一个字符
			cd=cd-1;                 //字符数减1
			nStrLen ++;
		}
	}	
	
	for( i = 1; i < n; i++)
	{
		L = sqrt( (double)( pt[i].x - pt[i-1].x ) * (pt[i].x - pt[i-1].x ) + ( pt[i].y - pt[i-1].y ) * (pt[i].y - pt[i-1].y ) );
		LenSum += L;
	}

	if( LenSum == 0 )
	{
		delete pt;
		delete type;
		return TRUE;
	}

	ls1 = str;                     //文本内容
	TCHAR hz[4];
	cd = strlen(ls1);                       //文本字符长度

	double len = LenSum / ( nStrLen + 1 );

	int mode = SetBkMode(hDC, TRANSPARENT);

	SetTextAlign( hDC, TA_CENTER | TA_BOTTOM);
	
	for( i = 1; i < n; i++)
	{
		L = sqrt( (double)( pt[i].x - pt[i-1].x ) * (pt[i].x - pt[i-1].x ) + ( pt[i].y - pt[i-1].y ) * (pt[i].y - pt[i-1].y ) );
		tmp += L;

		if(tmp >= len )
		{				
			lamda = (tmp - len) / ( L - tmp +len);
			
			double sita; 
			if( pt[i].y <= pt[i-1].y)
				sita = asin( ( pt[i].x - pt[i-1].x ) / L );
			else
				sita = asin( -( pt[i].x - pt[i-1].x ) / L );


			double x  = ( pt[i].x + pt[i-1].x * lamda ) / ( 1+lamda);
			double y  = ( pt[i].y + pt[i-1].y * lamda ) / ( 1+lamda);
			
			pt[i-1].x = (int)x;
			pt[i-1].y = (int)y;

			if( pt[i].y >= pt[i-1].y)
				sita = sita * 180.0 / 3.1415926 * 10.0 ;
			else
				sita = sita * 180.0 / 3.1415926 * 10.0 + 1800;

			
			if( cd > 0)
			{
				c1=*ls1;
				c2=*(ls1+1);
				if(c1<0&&c2<0)  //如果是一个汉字
				{
					strncpy_s( hz, ls1, 2);
					hz[2] = 0;
					ls1=ls1 + 2 ;        //跳过汉字指向下一个字符位置
					cd = cd - 2 ;                  //字符数减2					
				}
				else //如果是一个西文字符
				{
					strncpy_s( hz, ls1, 1);
					hz[1] = 0;
					ls1++;                   //跳过这一个字符到下一个字符
					cd=cd-1;                 //字符数减1
					
					sita += 900;
				}
				
				lgfn->lfEscapement = (long)-sita;
				
				hFont = CreateFontIndirect(lgfn);
				
				hOldFont = (HFONT)SelectObject(hDC, hFont);
				
				GetTextMetrics(hDC, &tm);
				
				fnHeight =  -fabs((double)tm.tmHeight/2);
				
				if(!(c1<0&&c2<0) )  //如果是一个西文字符
					x += fnHeight;
				
				TextOut(hDC, (int)x, (int)y, hz, strlen(hz));
				SelectObject(hDC, hOldFont);
				DeleteObject(hFont);

				tmp = 0;
				i--;
			}			
		}
	}

	delete pt;
	delete type;

	SetBkMode(hDC, mode);
	
	return TRUE;
}

extern "C"
BOOL SPLine2(HDC hDC, LPPOINT lppt, DWORD nPoints, BOOL bClose)
{
	if(nPoints <= 1 || lppt == NULL )
		return FALSE;
	if(nPoints == 2)
	{
		MoveToEx( hDC, lppt[0].x, lppt[0].y, NULL);
		LineTo  ( hDC, lppt[1].x, lppt[1].y);
		return TRUE;
	}
	
	long i, j;
	long cPoints = nPoints;
	
	
	LPPOINT lpptControl = new POINT [ cPoints + 3 ];
		
	for( i = 0; i < nPoints ; i++ )
	{
		lpptControl[i+1].x = lppt[i].x;
		lpptControl[i+1].y = lppt[i].y;
	}
		
	if( bClose )
	{
		lpptControl[0] = lpptControl[cPoints];
		lpptControl[cPoints+1] = lpptControl[1];
	}
	else
	{
		lpptControl[0] = lpptControl[1];
		lpptControl[cPoints+1] = lpptControl[cPoints];
	}
	double t1,t2,t,a,b,c;

	t=1.0/SEGMENT;
	
	if( !bClose)
		::MoveToEx(hDC, (lpptControl[0].x+lpptControl[1].x)/2,(lpptControl[0].y+lpptControl[1].y)/2, NULL);

	POINT pt;
	double x,y;
	for(i=0;i<cPoints;i++)
	{
		for(j=1;j<SEGMENT;j++)
		{
            t1=j*t;
			t2=t1*t1;
			a=(t2-2*t1+1)/2.0;
			b=t1-t2+1/2.0;
			c=t2/2.0;
            x=a*lpptControl[i].x+b*lpptControl[i+1].x+c*lpptControl[i+2].x;
            y=a*lpptControl[i].y+b*lpptControl[i+1].y+c*lpptControl[i+2].y;
			if( i==0
				&& j==1
				&& bClose )
			{
				::MoveToEx(hDC, x,y, NULL);
				pt.x = x;
				pt.y = y;
			}
			else
				LineTo(hDC,x,y);
		}
	}
	
	if( bClose )
		LineTo(hDC,pt.x, pt.y);
	else
		LineTo(hDC,lpptControl[cPoints+1].x, lpptControl[cPoints+1].y);
	
	delete []lpptControl;

	return TRUE;
}