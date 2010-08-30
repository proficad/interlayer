//############################################################################

#ifndef __MMATH_H__
#define __MMATH_H__

#include <math.h>

//Some Useful Math Routines and definitions
//Some Are Directly From Graphics Gems

#ifndef PI
#define PI (double)3.1415926535897932384626433832795
#endif

#ifndef INFINITY
#define INFINITY (double)100000
#endif

#define MAX(X, Y)\
		(X) > (Y) ? (X) : (Y)

#define vsin(x) ((1.0) - (double)(cos(x))) //versine theta (1 - cos theta)

double round(const double&);

typedef struct Matrix4Struct /* 4-by-4 matrix */
{
	double element[4][4];
}Matrix4;

void inverse(Matrix4* in, Matrix4* out);
void adjoint(Matrix4* in, Matrix4* out);
double det2x2(double a, double b, double c, double d);
double det3x3(double a1, double a2, double a3, double b1, double b2, double b3, double c1, double c2, double c3);
double det4x4(Matrix4* m);

#endif