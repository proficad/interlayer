#include "StdAfx.h"
#include "C3DSlice.h"

C3DSlice::C3DSlice(void)
{
	m_strObjName = _T("切片");
	m_ListDone = 0;
	m_Modified = 1;
	m_ColorBinding = COLOR_PER_VERTEX;

	m_glObjType = GLSLICE;

	A = 0, B = 0, C = 0, D = 0;		// 平面裁剪,这四个系数分别是裁剪平面Ax+By+Cz+D=0的A、B、C、D值

	m_dHeight = 1.0;
	m_bSpecified = false;
	m_bShowSlicePlane = true;

	GUID guid;
	if (S_OK == ::CoCreateGuid(&guid))
	{
		m_glObjID = guid.Data1;
	}
	else
		m_glObjID = (int)m_glObjType + CGLObject::GetCount();
}

C3DSlice::~C3DSlice(void)
{
}

void C3DSlice::Serialize(CArchive& ar)
{
	CGLObject::Serialize(ar);
	if(ar.IsLoading())
	{
		ar >> A;
		ar >> B;
		ar >> C;
		ar >> D;
 		ar >> m_dHeight;
 		ar >> m_bSpecified;
		ar >> m_bShowSlicePlane;
	}
	else
	{
		ar << A;
		ar << B;
		ar << C;
		ar << D;
		ar << m_dHeight;
		ar << m_bSpecified;
		ar << m_bShowSlicePlane;
	}
}

CGLObject* C3DSlice::Copy()
{
	C3DSlice* O = new C3DSlice();
	O->SetContext(m_pDisplayContext);
	O->m_glObjID = m_glObjID;
	O->m_strObjName = m_strObjName;
	O->m_glObjType = m_glObjType;
	O->A = A;
	O->B = B;
	O->C = C;
	O->D = D;
	O->m_bSpecified = m_bSpecified;
	O->m_dHeight = m_dHeight;

	O->Show(isShow());

	return O;
}

void C3DSlice::GetCoordinate(double *eq, CVertex3D *pCoord)
{
	double A,B,C,D;
	double x1, x2, y1, y2;

	//AX+BY+CZ+D=0;
	A = eq[0];
	B = eq[1];
	C = eq[2];
	D = eq[3];	

	if( C == 0.0 ) // 纵切片
	{
		if( fabs(A) < 0.000000001)
		{
			x1 = -1.0;
			y1 = -(A*x1+D)/B;
			x2 = 1.0;
			y2 = -(A*x2+D)/B;						
		}
		else if( fabs(B)<0.0000000001)
		{
			y1 = -1.0;
			x1 = -(B*y1+D)/A;
			y2 = 1.0;
			x2 = -(B*y2+D)/A;
		}
		else
		{
			double xx[4],yy[4];
			xx[0] = -1.0;
			yy[0] = -(A*xx[0]+D)/B;

			xx[1] = 1.0;
			yy[1] = -(A*xx[1]+D)/B;

			yy[2] = 1.0;
			xx[2] = -(B*yy[2]+D)/A;

			yy[3] = -1.0;
			xx[3] = -(B*yy[3]+D)/A;

			bool b = false;
			for (int i=0;i<4;i++)
			{
				if( fabs(xx[i])<=1.0 && fabs(yy[i])<=1.0)
				{
					if( !b )
					{
						x1 = xx[i];
						y1 = yy[i];

						b = true;

					}
					else
					{
						x2 = xx[i];
						y2 = yy[i];
					}
				}
			}
		}


		pCoord[0].x = x1;
		pCoord[0].y = y1;
		pCoord[0].z = 1.0;

		pCoord[1].x = x1;
		pCoord[1].y = y1;
		pCoord[1].z = -1.0;

		pCoord[2].x = x2;
		pCoord[2].y = y2;
		pCoord[2].z = -1.0;

		pCoord[3].x = x2;
		pCoord[3].y = y2;
		pCoord[3].z = 1.0;
	}

	if( A == 0.0 && B == 0.0)
	{
		pCoord[0].x = -1;
		pCoord[0].y = 1;
		pCoord[0].z = -D/C;

		pCoord[1].x = 1;
		pCoord[1].y = 1;
		pCoord[1].z = -D/C;

		pCoord[2].x = 1;
		pCoord[2].y = -1;
		pCoord[2].z = -D/C;

		pCoord[3].x = -1;
		pCoord[3].y = -1;
		pCoord[3].z = -D/C;
	}
}
