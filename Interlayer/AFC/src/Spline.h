#ifndef _SPLINE_H
#define _SPLINE_H
class CPoint2D;
namespace SPLINE
{	
	class AFX_EXT_CLASS CCurve
	{
	public:
		double  Ax,Ay;
		double  Bx,By;
		double  Cx,Cy;
		int    Ndiv;
	public:
		CCurve(double ax,double ay,double bx,double by,double cx,double cy,int ndiv);	
		CCurve(double ax,double ay,double bx,double by,double cx,double cy);
		CCurve(){};

		void PutCurve(double ax,double ay,double bx,double by,double cx,double cy);
		void PutCurve(double ax,double ay,double bx,double by,double cx,double cy, int ndiv);
		void draw(HDC hdc,double x,double y);
		int GetCount();
		void GetCurve(double x,double y, POINT points[], int& PointCount);
		void GetCurve(double x,double y, CPoint2D *points, int& PointCount);
		void GetCurve(double x,double y, double px[] , double py[], int& PointCount);
	};

	class AFX_EXT_CLASS CSpline 
	{

	public:
		double* Px;
		double* Py;
		double* Ax;
		double* Ay;
		double* Bx;
		double* By;
		double* Cx;
		double* Cy;
		double*  k;
		double*  Mat[3];

		int  NP;

	public:
		// constructor
		CSpline(POINT pt[], int np);
		CSpline(CPoint2D pt[], int np);
		CSpline(double px[] , double py[] , int np);	
		~CSpline();
	public:
		void Generate();
		void MatrixSolve(double B[]) ;
		void draw(HDC hdc) ;
		int GetCurveCount(int nDiv = 0);
		void GetCurve(POINT points[], int& PointCount);
		void GetCurve(CPoint2D *points, int& PointCount,int nDiv = 0);
		void GetCurve(double px[] , double py[], int& PointCount);
		//////////// closed cubic spline ////////////////////
		void GenClosed();
		///// tridiagonal matrix + elements of [0][0], [N-1][N-1] //// 
		void MatrixSolveEX(double B[]);
		void drawClosed(HDC hdc);
	};
};
#endif
