// ���ߣ��ž�
// ���ڣ�2001��10��13��
// �������ʣ�����������ϵ��
// E_mail��zj770315@hotmail.com
// 

#ifndef __SPLINE3_H__
#define __SPLINE3_H__
#include "Point2D.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPoint2D;
extern "C" 
LPPOINT AFX_EXT_API GetSPPoints(CPoint2D* lppt, DWORD nPoints, int &num, BOOL bClose);

extern "C" 
void AFX_EXT_API CalcArrowRgnSpline(LPPOINT lppt, DWORD nPoints, double Len, LPPOINT pt,
						BOOL bEnd = TRUE);

extern "C" 
void AFX_EXT_API DrawRotatedText(HDC hDC, const LPCTSTR str, LPCRECT rect, 
                     double angle, UINT nOptions=0);


// dWidth	-- �Ⱦ��߼������
extern "C" 
void AFX_EXT_API Parallelline(HDC hDC, LPPOINT lppt, DWORD nPoints, BOOL bInner, double dWidth = 10.0);

extern "C" 
void AFX_EXT_API Parallelgon(HDC hDC, LPPOINT lppt, DWORD nPoints, BOOL bInner, double dWidth = 10.0);

// hDC:����, lpptControl:��ֵ��, cPoints:����, bClose:�Ƿ���
extern "C" 
BOOL AFX_EXT_API ParallelCurve(HDC hDC, LPPOINT lpptControl, DWORD cPoints, 
				   BOOL bClose = FALSE, BOOL bInner = FALSE, double dWidth = 10.0); 

// hDC:����, lpptControl:��ֵ��, cPoints:����, bClose:�Ƿ���
extern "C" 
BOOL AFX_EXT_API PolySpline3(HDC hDC, LPPOINT lpptControl, DWORD cPoints, 
				 BOOL bClose = FALSE);

extern "C" 
long AFX_EXT_API GetSpline3Pointf(CPoint2D* lpptControl, DWORD cPoints, BOOL bClose = FALSE, CPoint2D* lpptPoint = NULL);

extern "C" 
long AFX_EXT_API GetSpline3Point(LPPOINT lpptControl, DWORD cPoints, BOOL bClose = FALSE, LPPOINT lpptPoint = NULL);

extern "C" 
BOOL AFX_EXT_API PolySpline3Label(HDC hDC, LPCTSTR str, double space, LOGFONT *lgfn,
					  CPoint2D* lpptControl, DWORD cPoints, BOOL bClose = FALSE);

extern "C" 
BOOL AFX_EXT_API PolylineLabel(
  HDC hdc,            // handle to device context
  LPCTSTR str, double space, LOGFONT *lgfn, 
  CONST POINT *lppt,  // pointer to array containing endpoints
  int cPoints         // number of points in the array
);

extern "C" 
BOOL AFX_EXT_API PolygonLabel(
  HDC hdc,            // handle to device context
  LPCTSTR str, double space, LOGFONT *lgfn, 
  CONST POINT *lppt,  // pointer to array containing endpoints
  int cPoints         // number of points in the array
); 

extern "C" 
BOOL AFX_EXT_API PolySpline3Title(
	HDC hDC, 
	LPCTSTR str, 
	LOGFONT *lgfn, 
	CPoint2D* lpptControl, 
	DWORD cPoints, 
	BOOL bClose = FALSE
	);

extern "C" 
BOOL AFX_EXT_API PolylineTitle(
  HDC hdc,            // handle to device context
  LPCTSTR str,  LOGFONT *lgfn, 
  CONST POINT *lppt,  // pointer to array containing endpoints
  int cPoints         // number of points in the array
);

extern "C" 
BOOL AFX_EXT_API PolygonTitle(
  HDC hdc,            // handle to device context
  LPCTSTR str, LOGFONT *lgfn, 
  CONST POINT *lppt,  // pointer to array containing endpoints
  int cPoints         // number of points in the array
); 

extern "C" 
BOOL AFX_EXT_API DrawLabel(HDC hDC, LPCTSTR str, double space, LOGFONT *lgfn);

extern "C" 
BOOL AFX_EXT_API DrawTitle(HDC hDC, LPCTSTR str, LOGFONT *lgfn);

// ����B�������������Ƶ�
extern "C"
BOOL AFX_EXT_API SPLine2(HDC hDC, LPPOINT lpptControl, DWORD cPoints, 
							BOOL bClose = FALSE);


#endif  // __SPLINE3_H__