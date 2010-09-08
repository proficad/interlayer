// Point3D.h: interface for the CPoint3D class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POINT3D_H__3DC0BD4D_3CDC_44DD_B05C_F598E2B85DE2__INCLUDED_)
#define AFX_POINT3D_H__3DC0BD4D_3CDC_44DD_B05C_F598E2B85DE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class AFX_EXT_CLASS CPoint3D  
{
public:
	double x;
	double y;
	double z;
	double m_dValue;
	TCHAR  szInfo[20];
public:
	CPoint3D();
	CPoint3D(double initX, double initY, double initZ, double dValue = 0.0);
	const CPoint3D& operator=(const CPoint3D& ptSrc);
private:
	friend CArchive& AFXAPI operator<<(CArchive& ar, const CPoint3D& pt)
	{
		ar << pt.x;
		ar << pt.y;	
		ar << pt.z;	
		ar << pt.m_dValue;
		ar.Write(pt.szInfo, 20);
		return ar;
	}

	friend CArchive& AFXAPI operator>>(CArchive& ar, CPoint3D& pt)
	{
		ar >> pt.x;
		ar >> pt.y;	
		ar >> pt.z;
		ar >> pt.m_dValue;
		ar.Read(pt.szInfo, 20);
		return ar;
	}

	friend BOOL AFXAPI operator==(const CPoint3D& s1, const CPoint3D& s2)
	{
		return (s1.x == s2.x && s1.y == s2.y && s1.z == s2.z); 
	}
};

#define MINERROR 0.000001

//����ռ������ṹ
typedef struct SpaceVector
{
	double m;
	double n;
	double p;
}SpaceVector;

// ��֪�߶�2��Ϳռ�3���㹹�ɵ������棬���߶������������Ч����
// ����ֵ-0�޽��㣬1�н��������������ڵ������߶��ϣ�2�н������������������߶���
extern "C" int AFX_EXT_API InterPoint(CPoint3D LinePoint1,CPoint3D LinePoint2, CPoint3D TrianglePoint1, 
									   CPoint3D TrianglePoint2, CPoint3D TrianglePoint3, CPoint3D &CrossPoint);

extern "C" bool AFX_EXT_API DecideCollineation(const CPoint3D &pt1,const CPoint3D &pt2, const CPoint3D &pt3);

#endif // !defined(AFX_POINT3D_H__3DC0BD4D_3CDC_44DD_B05C_F598E2B85DE2__INCLUDED_)
