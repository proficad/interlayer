
#include "stdafx.h"
#include "SPLINE.h"
#include "math.h"
#include "Point2D.h"

#define DIV_FACTOR 8.0 //adjust this factor to adjust the curve smoothness

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace SPLINE;

CCurve::CCurve(double ax,double ay,double bx,double by,double cx,double cy,int ndiv) 
{
	Ax = ax;
	Ay = ay;
	Bx = bx;
	By = by;
	Cx = cx;
	Cy = cy;
	Ndiv = ndiv;
}

CCurve::CCurve(double ax,double ay,double bx,double by,double cx,double cy)
{
	Ax = ax; 
	Ay = ay;
	Bx = bx; 
	By = by;
	Cx = cx; 
	Cy = cy;
	Ndiv = (int)(max(abs((int)Ax),abs((int)Ay))/DIV_FACTOR);
}

void CCurve::PutCurve(double ax,double ay,double bx,double by,double cx,double cy) 
{
	Ax = ax;
	Ay = ay;
	Bx = bx; 
	By = by;
	Cx = cx; 
	Cy = cy;
	Ndiv = (int)(max(abs((int)Ax),abs((int)Ay))/DIV_FACTOR);
}

void CCurve::PutCurve(double ax,double ay,double bx,double by,double cx,double cy, int ndiv) 
{
	Ax = ax;
	Ay = ay;
	Bx = bx; 
	By = by;
	Cx = cx; 
	Cy = cy;
	Ndiv = ndiv;
}

void CCurve::draw(HDC hdc,double x,double y) 
{
	int OrigX,OrigY,NewX,NewY;
	double  t,f,g,h;
	if (Ndiv==0)
		Ndiv=1;
	
	OrigX = (int)x; 
	OrigY= (int)y;
	for(int i=1; i<=Ndiv ; i++)
	{
		t = 1.0f / (double)Ndiv * (double)i;
		f = t*t*(3.0f-2.0f*t);
		g = t*(t-1.0f)*(t-1.0f);
		h = t*t*(t-1.0f);
		NewX = (int)(x + Ax*f + Bx*g + Cx*h);
		NewY = (int)(y + Ay*f + By*g + Cy*h);
		MoveToEx(hdc, OrigX, OrigY, NULL);
		LineTo(hdc, NewX, NewY);
		OrigX = NewX;  
		OrigY=NewY;
	}
}

int CCurve::GetCount()
{
	if (Ndiv==0)
		Ndiv=1;
	int PointCount = 1;
	
	for(int i=1; i<=Ndiv ; i++)
	{
		PointCount++;
	}
	return PointCount;
}

void CCurve::GetCurve(double x,double y, POINT points[], int& PointCount)
{
	int X,Y;
	double  t,f,g,h;
	if (Ndiv==0)
		Ndiv=1;
	
	X = (int)x; 
	Y= (int)y;
	points[PointCount].x = X;
	points[PointCount].y = Y;
	PointCount++;
	
	for(int i=1; i<=Ndiv ; i++)
	{
		t = 1.0f / (double)Ndiv * (double)i;
		f = t*t*(3.0f-2.0f*t);
		g = t*(t-1.0f)*(t-1.0f);
		h = t*t*(t-1.0f);
		X = (int)(x + Ax*f + Bx*g + Cx*h);
		Y = (int)(y + Ay*f + By*g + Cy*h);
		points[PointCount].x = X;
		points[PointCount].y = Y;
		PointCount++;
	}
}

void CCurve::GetCurve(double x,double y, CPoint2D *points, int& PointCount)
{
	double X,Y;
	double  t,f,g,h;
	if (Ndiv==0)
		Ndiv=1;

	X = x; 
	Y= y;
	points[PointCount].x = X;
	points[PointCount].y = Y;
	PointCount++;

	for(int i=1; i<=Ndiv ; i++)
	{
		t = 1.0f / (double)Ndiv * (double)i;
		f = t*t*(3.0f-2.0f*t);
		g = t*(t-1.0f)*(t-1.0f);
		h = t*t*(t-1.0f);
		X = (x + Ax*f + Bx*g + Cx*h);
		Y = (y + Ay*f + By*g + Cy*h);
		points[PointCount].x = X;
		points[PointCount].y = Y;
		PointCount++;
	}
}

void CCurve::GetCurve(double x,double y, double px[] , double py[], int& PointCount)
{
	double X,Y;
	double  t,f,g,h;
	if (Ndiv==0)
		Ndiv=1;

	X = x; 
	Y= y;
	px[PointCount] = X;
	py[PointCount] = Y;
	PointCount++;

	for(int i=1; i<=Ndiv ; i++)
	{
		t = 1.0f / (double)Ndiv * (double)i;
		f = t*t*(3.0f-2.0f*t);
		g = t*(t-1.0f)*(t-1.0f);
		h = t*t*(t-1.0f);
		X = x + Ax*f + Bx*g + Cx*h;
		Y = y + Ay*f + By*g + Cy*h;
		px[PointCount] = X;
		py[PointCount] = Y;
		PointCount++;
	}
}

CSpline::CSpline(POINT pt[], int np)
{
	NP = np;
	Px = new double[NP];
	Py = new double[NP];
	Ax = new double[NP];
	Ay = new double[NP];
	Bx = new double[NP];
	By = new double[NP];
	Cx = new double[NP];
	Cy = new double[NP];
	k = new double[NP];
	Mat[0] = new double[NP];
	Mat[1] = new double[NP];
	Mat[2] = new double[NP];
	
	for(int i=0;i<NP ;i++) 
	{
		Px[i] = (double)pt[i].x;  
		Py[i] = (double)pt[i].y;
	}	
}

CSpline::CSpline(CPoint2D pt[], int np)
{
	NP = np;
	Px = new double[NP];
	Py = new double[NP];
	Ax = new double[NP];
	Ay = new double[NP];
	Bx = new double[NP];
	By = new double[NP];
	Cx = new double[NP];
	Cy = new double[NP];
	k = new double[NP];
	Mat[0] = new double[NP];
	Mat[1] = new double[NP];
	Mat[2] = new double[NP];

	for(int i=0;i<NP ;i++) 
	{
		Px[i] = pt[i].x;  
		Py[i] = pt[i].y;
	}
}

CSpline::CSpline(double px[] , double py[] , int np) 
{
	NP = np;
	Px = new double[NP];
	Py = new double[NP];
	Ax = new double[NP];
	Ay = new double[NP];
	Bx = new double[NP];
	By = new double[NP];
	Cx = new double[NP];
	Cy = new double[NP];
	k = new double[NP];
	Mat[0] = new double[NP];
	Mat[1] = new double[NP];
	Mat[2] = new double[NP];
	
	for(int i=0;i<NP ;i++) 
	{
		Px[i] = px[i];  
		Py[i] = py[i];
	}
	
}

CSpline::~CSpline()
{
	delete[] Px;
	delete[] Py;
	delete[] Ax;
	delete[] Ay;
	delete[] Bx;
	delete[] By;
	delete[] Cx;
	delete[] Cy;
	delete[] k;
	delete[] Mat[0];
	delete[] Mat[1];
	delete[] Mat[2];
}

void CSpline::Generate() 
{
	double AMag , AMagOld;
	int i;
	// vector A
	for( i= 0 ; i<=NP-2 ; i++ ) 
	{
		Ax[i] = Px[i+1] - Px[i];
		Ay[i] = Py[i+1] - Py[i];
	}
	// k
	AMagOld = (double)sqrt(Ax[0]*Ax[0] + Ay[0]*Ay[0]);
	for( i=0 ; i<=NP-3 ; i++) 
	{
		AMag = (double)sqrt(Ax[i+1]*Ax[i+1] + Ay[i+1]*Ay[i+1]);
		k[i] = AMagOld / AMag;
		AMagOld = AMag;
	}
	k[NP-2] = 1.0f;
	
	// Matrix
	for(i=1; i<=NP-2;i++) 
	{
		Mat[0][i] = 1.0f;
		Mat[1][i] = 2.0f*k[i-1]*(1.0f + k[i-1]);
		Mat[2][i] = k[i-1]*k[i-1]*k[i];
	}
	Mat[1][0] = 2.0f;
	Mat[2][0] = k[0];
	Mat[0][NP-1] = 1.0f;
	Mat[1][NP-1] = 2.0f*k[NP-2];
	
	// 
	for(i=1; i<=NP-2;i++) 
	{
		Bx[i] = 3.0f*(Ax[i-1] + k[i-1]*k[i-1]*Ax[i]);
		By[i] = 3.0f*(Ay[i-1] + k[i-1]*k[i-1]*Ay[i]);
	}
	Bx[0] = 3.0f*Ax[0];
	By[0] = 3.0f*Ay[0];
	Bx[NP-1] = 3.0f*Ax[NP-2];
	By[NP-1] = 3.0f*Ay[NP-2];
	
	//
	MatrixSolve(Bx);
	MatrixSolve(By);
	
	for(i=0 ; i<=NP-2 ; i++ ) 
	{
		Cx[i] = k[i]*Bx[i+1];
		Cy[i] = k[i]*By[i+1];
	}
}

void CSpline::MatrixSolve(double B[]) 
{
	double* Work = new double[NP];
	double* WorkB = new double[NP];
	int i,j;

	for( i=0;i<=NP-1;i++) 
	{
		Work[i] = B[i] / Mat[1][i];
		WorkB[i] = Work[i];
	}
	
	for( j=0 ; j<10 ; j++) 
	{ ///  need convergence judge
		Work[0] = (B[0] - Mat[2][0]*WorkB[1])/Mat[1][0];
		for( i=1; i<NP-1 ; i++ ) 
		{
			Work[i] = (B[i]-Mat[0][i]*WorkB[i-1]-Mat[2][i]*WorkB[i+1])
				/Mat[1][i];
		}
		Work[NP-1] = (B[NP-1] - Mat[0][NP-1]*WorkB[NP-2])/Mat[1][NP-1];
		
		for(i=0 ; i<=NP-1 ; i++ ) 
		{
			WorkB[i] = Work[i];
		}
	}
	for(i=0 ; i<=NP-1 ; i++ ) 
	{
		B[i] = Work[i];
	}
	delete[] Work;
	delete[] WorkB;
}

void CSpline::draw(HDC hdc) 
{
	CCurve c;
	for(int i=0; i<NP-1 ; i++) 
	{
		c.PutCurve(Ax[i],Ay[i],Bx[i],By[i],Cx[i],Cy[i]);
		c.draw(hdc,Px[i],Py[i]);
	}
	
}
int CSpline::GetCurveCount(int nDiv)
{
	CCurve c;
	int count = 0;
	for(int i=0; i<NP-1 ; i++) 
	{
		c.PutCurve(Ax[i],Ay[i],Bx[i],By[i],Cx[i],Cy[i]);
		if( nDiv > 0 )
			c.Ndiv = nDiv;
		count += c.GetCount();
	}
	return count;
}

void CSpline::GetCurve(POINT points[], int& PointCount)
{
	CCurve c;
	for(int i=0; i<NP-1 ; i++) 
	{
		c.PutCurve(Ax[i],Ay[i],Bx[i],By[i],Cx[i],Cy[i]);
		c.GetCurve(Px[i],Py[i], points, PointCount);
	}
}

void CSpline::GetCurve(CPoint2D *points, int& PointCount, int nDiv)
{
	CCurve c;
	for(int i=0; i<NP-1 ; i++) 
	{
		c.PutCurve(Ax[i],Ay[i],Bx[i],By[i],Cx[i],Cy[i]);
		if( nDiv > 0 )
			c.Ndiv = nDiv;
		c.GetCurve(Px[i],Py[i], points, PointCount);
	}
}

void CSpline::GetCurve(double px[] , double py[], int& PointCount)
{
	CCurve c;
	for(int i=0; i<NP-1 ; i++) 
	{
		c.PutCurve(Ax[i],Ay[i],Bx[i],By[i],Cx[i],Cy[i]);
		c.GetCurve(Px[i],Py[i], px, py, PointCount);
	}
}
//////////// closed cubic spline ////////////////////
void CSpline::GenClosed() 
{
	double AMag , AMagOld , AMag0;
	int i;
	// vector A
	for( i= 0 ; i<=NP-2 ; i++ ) 
	{
		Ax[i] = Px[i+1] - Px[i];
		Ay[i] = Py[i+1] - Py[i];
	}
	Ax[NP-1] = Px[0] - Px[NP-1];
	Ay[NP-1] = Py[0] - Py[NP-1];
	
	// k
	AMag0 = AMagOld = (double)sqrt(Ax[0]*Ax[0] + Ay[0]*Ay[0]);
	for(i=0 ; i<=NP-2 ; i++) 
	{
		AMag = (double)sqrt(Ax[i+1]*Ax[i+1] + Ay[i+1]*Ay[i+1]);
		k[i] = AMagOld / AMag;
		AMagOld = AMag;
	}
	k[NP-1]=AMagOld/AMag0; 
	
	// Matrix
	for(i=1; i<=NP-1;i++) 
	{
		Mat[0][i] = 1.0f;
		Mat[1][1] = 2.0f*k[i-1]*(1.0f + k[i-1]);
		Mat[2][i] = k[i-1]*k[i-1]*k[i];
	}
	Mat[0][0] = 1.0f;
	Mat[1][0] = 2.0f*k[NP-1]*(1.0f + k[NP-1]);
	Mat[2][0] = k[NP-1]*k[NP-1]*k[0];
	
	// 
	for(i=1; i<=NP-1;i++) 
	{
		Bx[i] = 3.0f*(Ax[i-1] + k[i-1]*k[i-1]*Ax[i]);
		By[i] = 3.0f*(Ay[i-1] + k[i-1]*k[i-1]*Ay[i]);
	}
	Bx[0] = 3.0f*(Ax[NP-1] + k[NP-1]*k[NP-1]*Ax[0]);
	By[0] = 3.0f*(Ay[NP-1] + k[NP-1]*k[NP-1]*Ay[0]);
	
	//
	MatrixSolveEX(Bx);
	MatrixSolveEX(By);
	
	for(i=0 ; i<=NP-2 ; i++ ) 
	{
		Cx[i] = k[i]*Bx[i+1];
		Cy[i] = k[i]*By[i+1];
	}
	Cx[NP-1] = k[NP-1]*Bx[0];
	Cy[NP-1] = k[NP-1]*By[0];
}

///// tridiagonal matrix + elements of [0][0], [N-1][N-1] //// 
void CSpline::MatrixSolveEX(double B[]) 
{
	double* Work = new double[NP];
	double* WorkB = new double[NP];
	int i;
	for( i=0;i<=NP-1;i++) 
	{
		Work[i] = B[i] / Mat[1][i];
		WorkB[i] = Work[i];
	}
	
	for(int j=0 ; j<10 ; j++) 
	{  // need judge of convergence
		Work[0] = (B[0]-Mat[0][0]*WorkB[NP-1]-Mat[2][0]*WorkB[1])
			/Mat[1][0];
		for( i=1; i<NP-1 ; i++ ) 
		{
			Work[i] = (B[i]-Mat[0][i]*WorkB[i-1]-Mat[2][i]*WorkB[i+1])
				/Mat[1][i];
		}
		Work[NP-1] = (B[NP-1]-Mat[0][NP-1]*WorkB[NP-2]-Mat[2][NP-1]*WorkB[0])
			/Mat[1][NP-1];
		
		for(i=0 ; i<=NP-1 ; i++ ) 
		{
			WorkB[i] = Work[i];
		}
	}
	
	for(i=0 ; i<=NP-1 ; i++ ) 
	{
		B[i] = Work[i];
	}
}

void CSpline::drawClosed(HDC hdc) 
{
	CCurve c;
	for(int i=0; i<NP ; i++) 
	{
		c.PutCurve(Ax[i],Ay[i],Bx[i],By[i],Cx[i],Cy[i]);
		c.draw(hdc ,Px[i],Py[i]);
	}
}