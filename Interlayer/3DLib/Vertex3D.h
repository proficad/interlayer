//############################################################################
//
// Point3D.h: interface for the CVertex3D class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POINT3D_H__21228FA2_D57D_4321_95A7_E1EB539550C7__INCLUDED_)
#define AFX_POINT3D_H__21228FA2_D57D_4321_95A7_E1EB539550C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Geometry.h"
#include "Color.h"
#include "Array3d.h"
#include "Vector3d.h"
#include "../Vector3df.h"

class COneAxis;
class CAxisSystem;
class CPlane;
class CFaceTriangles;
class CVector3D;

class CVertex3D : public CGeometry
{
	friend istream& operator>>(istream&, CVertex3D&);
	friend ostream& operator<<(ostream&, CVertex3D&);
public:
	double x;
	double y;
	double z;

	CColor m_itsColor;		// 点颜色

private:
	//-----------------------------------------------------------------------
	CVector3D m_Normal;		// 法向量
	char m_Flag;			// Flag

	// Neighbors (faces and vertices)
	CArray3d<CVertex3D> m_ArrayVertexNeighbor;
	CArray3d<CFaceTriangles> m_ArrayFaceNeighbor;
public:
	CVertex3D();
	CVertex3D(const CVertex3D& pt);
	CVertex3D(CVertex3D* pt);
	CVertex3D(double, double, double);
	virtual ~CVertex3D();

	void SetParam(double, double, double);
	void SetParam(const CVertex3D pt)
	{
		SetParam(pt.GetX(), pt.GetY(), pt.GetZ());
	}
	void SetParam(CVertex3D* pt)
	{
		SetParam(pt->GetX(), pt->GetY(), pt->GetZ());
	}

	void SetX(double X)
	{
		x = X;
	}
	void SetY(double Y)
	{
		y = Y;
	}
	void SetZ(double Z)
	{
		z = Z;
	}
	
	void SetParam(const unsigned int index, const double val)
	{
		switch (index)
		{
		case 0:
			x = val;
		case 1:
			y = val;
		case 2:
			z = val;
		}
	}
	double GetX() const;
	double GetY() const;
	double GetZ() const;
	double GetValue() const;
	double GetParam(const unsigned int index)
	{
		switch (index)
		{
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		}
		return 0;
	}

	CVertex3D operator=(const CVertex3D& vertex);
	CVertex3D operator+(const CVertex3D&);
	CVertex3D operator+(const double&);
	CVertex3D operator-(const CVertex3D&);
	CVertex3D operator-(const double&);
	CVertex3D operator *(const double scale);
	CVertex3D operator/(const double scale);
	void operator+=(const CVertex3D&);
	void operator+=(const double&);
	void operator-=(const CVertex3D&);
	void operator-=(const double&);
	void operator*=(const double scale);
	void operator/=(const double scale);
	bool operator ==(const CVertex3D& p) const;
	bool operator !=(const CVertex3D& p) const;

	operator CVector3D();										// 转化为矢量

	//////////////////////////////////////////////////////////////////////////
	// Flag
	char GetFlag(void)
	{
		return m_Flag;
	}
	void SetFlag(const char flag)
	{
		m_Flag = flag;
	}
	//////////////////////////////////////////////////////////////////////////
	// Normal
	void SetNormal(const double x, const double y, const double z)
	{
		m_Normal.SetParam(x, y, z);
	}

	void SetNormal(CVector3D& vec)
	{
		m_Normal.SetParam(vec);
	}

	CVector3D* GetNormal(void)
	{
		return &m_Normal;
	}
	//////////////////////////////////////////////////////////////////////////
	// 颜色
	void SetColor(const GLubyte&, const GLubyte&, const GLubyte&);
	void SetColor(CColor& color);
	void GetColor(GLubyte*) const;
	CColor* GetColor(void);

	//////////////////////////////////////////////////////////////////////////
	double Distance(const CVertex3D&);							// 两点距离
	double Distance(const CPlane&);								// 到平面距离	

	virtual void Translate(const COneAxis&, const double&);		// 转换
	virtual void Translate(double dx, double dy, double dz);
	virtual void Translate(const CVector3D&);
	virtual void Translate(const CVector3DF&);
	virtual void Translate(const CVertex3D&, const CVertex3D&);
	virtual void Rotate(const COneAxis&, double);
	virtual void Scale(const CVertex3D&, double);
	virtual void Mirror(const CVertex3D&);
	virtual void Mirror(const COneAxis&);
	virtual void Mirror(const CPlane&);
	static CVertex3D Origin();									// 原点

	// Moving
	void Move(CVector3D& v, const double ratio);
	void Move(CVector3D* pV, const double ratio = 1.0);
	void Move(const double dx, const double dy, const double dz);

	//-----------------------------------------------------------------------	
	bool HasNeighborWithFlag(int flag);
	bool HasNeighborWithFlagButDiff(int flag, CVertex3D* pVertexDiff);
	CVertex3D* GetFirstVertexNeighborWithFlag(int flag);
	CVertex3D* GetNearestVertexNeighborWithFlag(int flag);
	CVertex3D* GetFirstVertexNeighborWithFlagButDiff(int flag, CVertex3D* pVertexDiff);
	CVertex3D* GetFirstNeighborWithFlagAndSharingFaceWithFlag(int FlagVertex, int FlagFace);
	int DiffFlagsOnNeighboringFaces();
	int IsFlagSmallestOnNeighboringFaces(int flag);
	int FindVertexNeighborsWhichFlagIsGreater(int flag, CArray3d<CVertex3D>& array);
	int FindVertexNeighborsWhichFlagIsSmaller(int flag, CArray3d<CVertex3D>& array);
	int NbVertexNeighborWithFlag(int flag);

	// Operators
	bool Diff(CVertex3D* pVertex);			// !=
	bool Equal(CVertex3D* pVertex);			// ==

	// Vertex neighbors
	int NbVertexNeighbor(void);
	void AddNeighbor(CVertex3D* pVertex);
	void AddNeighborNoCheck(CVertex3D* pVertex);
	bool HasNeighbor(CVertex3D* pVertex)
	{
		return m_ArrayVertexNeighbor.Has(pVertex);
	}

	int RemoveNeighbor(CVertex3D* pVertex);
	void RemoveAllVertexNeighbor(void);
	void UpdateNeighborRecursive(CVertex3D* pVertexOld, CVertex3D* pVertexNew);
	int UpdateNeighbor(CVertex3D* pVertexOld, CVertex3D* pVertexNew);
	int FindFaceAroundContainVertex(CVertex3D* pVertex, CArray3d<CFaceTriangles>& ArrayFace);
	CVertex3D* GetVertexNeighbor(const unsigned int index)
	{
		return m_ArrayVertexNeighbor[index];
	}
	CArray3d<CVertex3D>* GetArrayVertexNeighbor()
	{
		return &m_ArrayVertexNeighbor;
	}

	// Face neighbors
	int NbFaceNeighbor(void);
	void AddNeighbor(CFaceTriangles* pFace);
	int RemoveNeighbor(CFaceTriangles* pFace);
	void RemoveAllFaceNeighbor(void);
	bool HasNeighbor(CFaceTriangles* pFace)
	{
		return m_ArrayFaceNeighbor.Has(pFace);
	}
	CFaceTriangles* GetFaceNeighbor(const unsigned int index)
	{
		return m_ArrayFaceNeighbor[index];
	}
	CArray3d<CFaceTriangles>* GetArrayFaceNeighbor()
	{
		return &m_ArrayFaceNeighbor;
	}

	// Face sharing
	int FindSharingFaces(CVertex3D* pVertex, CArray3d<CFaceTriangles>& array);

	// Area
	double GetMeanAreaAround();
	double GetMinAreaAround();
	// Distance
	double GetMeanLengthEdgeAround();
	// Curve
	double GetMeanCurveAround();
	double GetSumCurveAround();
	double GetMaxCurveAround();
	double GetMaxAngleAround();
	double GetMaxCurveAroundAndNeighbors();

	// Precision
	void ReplaceOnGrid(double threshold);

	// Boundaries
	bool IsOnBoundary();	// 是否在边上

	// Count sharp edges around vertex
	int NbSharpEdge(const double threshold = 0.5);

	// Normals's 
	int NormalSum(double* pSum);	// 法向量和
	int NormalMax(double* pMax);	// 最大法向量

	//////////////////////////////////////////////////////////////////////////
	friend inline CArchive& operator <<(CArchive& ar, const CVertex3D& pt)
	{
		ar << pt.GetX();
		ar << pt.GetY();
		ar << pt.GetZ();
		return ar;
	}

	friend inline CArchive& operator >>(CArchive& ar, CVertex3D& pt)
	{
		double x, y, z;
		ar >> x;
		ar >> y;
		ar >> z;
		pt.SetParam(x, y, z);
		return ar;
	}

	void Serialize(CArchive& ar)
	{
		if (ar.IsStoring())
		{
			ar << x;
			ar << y;
			ar << z;
		}
		else
		{
			ar >> x;
			ar >> y;
			ar >> z;
		}
	}
};

#endif // !defined(AFX_POINT3D_H__21228FA2_D57D_4321_95A7_E1EB539550C7__INCLUDED_)
