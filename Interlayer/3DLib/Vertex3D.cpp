// Point3D.cpp: implementation of the CVertex3D class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Vector3d.h"
#include "Vertex3D.h"
#include "OneAxis.h"
#include "Line3D.h"
#include "Plane.h"
#include "Matrix33.h"
#include "FaceTriangles.h"
#include "Utils3d.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVertex3D::CVertex3D()
{
	m_itsColor.Set(205, 205, 25);
	m_Flag = 0;
	SetParam(0, 0, 0);
	m_geomType = GEO_VERTEX;
}

CVertex3D::CVertex3D(const CVertex3D& pt)
{
	m_itsColor.Set(205, 205, 25);
	m_Flag = 0; 
	x = pt.GetX();
	y = pt.GetY();
	z = pt.GetZ();
	m_geomType = GEO_VERTEX;
}

CVertex3D::CVertex3D(CVertex3D* pt)
{
	m_itsColor.Set(205, 205, 25);
	m_Flag = 0; 
	x = pt->GetX();
	y = pt->GetY();
	z = pt->GetZ();
	m_geomType = GEO_VERTEX;
}

CVertex3D::CVertex3D(double x, double y, double z)
{
	m_itsColor.Set(205, 205, 25);
	m_Flag = 0;
	SetParam(x, y, z);
	m_geomType = GEO_VERTEX;
}

CVertex3D::~CVertex3D()
{
}

void CVertex3D::SetParam(double X, double Y, double Z)
{
	m_Flag = 0; 
	x = X;
	y = Y;
	z = Z;
}

double CVertex3D::GetX() const
{
	return x;
}

double CVertex3D::GetY() const
{
	return y;
}

double CVertex3D::GetZ() const
{
	return z;
}


CVertex3D CVertex3D::operator=(const CVertex3D& vertex)
{
	//CVertex3D v;
	this->SetParam(vertex);
	return (*this);
}

CVertex3D CVertex3D::operator+(const CVertex3D& newPoint)
{
	return CVertex3D(x + newPoint.GetX(), y + newPoint.GetY(), z + newPoint.GetZ());
}

CVertex3D CVertex3D::operator+(const double& inc)
{
	return CVertex3D(x + inc, y + inc, z + inc);
}

CVertex3D CVertex3D::operator-(const CVertex3D& newPoint)
{
	return CVertex3D(x - newPoint.GetX(), y - newPoint.GetY(), z - newPoint.GetZ());
}

CVertex3D CVertex3D::operator-(const double& dec)
{
	return CVertex3D(x - dec, y - dec, z - dec);
}

CVertex3D CVertex3D::operator *(const double scale)
{
	return CVertex3D(x * scale, y * scale, z * scale);
}

CVertex3D CVertex3D::operator/(const double scale)
{
	return CVertex3D(x / scale, y / scale, z / scale);
}

void CVertex3D::operator+=(const CVertex3D& P)
{
	(*this) = (*this) + P;
}

void CVertex3D::operator+=(const double& inc)
{
	(*this) = (*this) + inc;
}

void CVertex3D::operator-=(const CVertex3D& P)
{
	(*this) = (*this) - P;
}

void CVertex3D::operator-=(const double& dec)
{
	(*this) = (*this) - dec;
}

void CVertex3D::operator*=(const double scale)
{
	(*this) = (*this)*scale;
}

void CVertex3D::operator/=(const double scale)
{
	(*this) = (*this) / scale;
}

bool CVertex3D::operator ==(const CVertex3D& p) const
{
	return (x == p.GetX() && x == p.GetX() && x == p.GetX());
}

bool CVertex3D::operator !=(const CVertex3D& p) const
{
	return (x != p.GetX() || x != p.GetX() && x != p.GetX());
}


CVertex3D::operator CVector3D()
{
	CVector3D V((*this));
	return V;
}

double CVertex3D::Distance(const CVertex3D& P)
{
	double sqad, root;
	double dx, dy, dz, tx, ty, tz;
	dx = P.GetX(); dy = P.GetY(); dz = P.GetZ();
	tx = fabs(this->GetX() - dx);
	ty = fabs(this->GetY() - dy);
	tz = fabs(this->GetZ() - dz);
	sqad = ((pow(tx, 2)) + (pow(ty, 2)) + (pow(tz, 2)));
	root = sqrt(sqad);

	return root;
}

double CVertex3D::Distance(const CPlane& P)
{
	CVertex3D p = P.GetPosition();
	CVertex3D dp = (*this);
	CVector3D V1 = P.GetDirection();
	CVector3D V2(dp);
	double D = V1.Dot(V2) + P.GetConstant();
	return D;
}

void CVertex3D::Translate(const COneAxis& Ax, const double& amt)
{
	CGeometry::Translate(Ax, amt);
}

void CVertex3D::Translate(double dx, double dy, double dz)
{
	x += dx;
	y += dy;
	z += dz;
}

void CVertex3D::Translate(const CVector3D& VDir)
{
	CVector3D V = VDir;
	x += V.GetX();
	y += V.GetY();
	z += V.GetZ();
}

void CVertex3D::Translate(const CVertex3D& P1, const CVertex3D& P2)
{
	CVector3D V(P1, P2);
	Translate(V);
}

void CVertex3D::Rotate(const COneAxis& Ax, double ang)
{
	COneAxis ax = Ax;
	CVector3D P1 = ax.GetOrigin();
	CVector3D V = ax.GetDirection();
	V.Normalize();
	CMatrix33 M; CVector3D PV(*this);
	M.SetRotation(V, ang);
	CVector3D RV = M*(PV - P1);
	RV = RV + P1;
	*this = RV.Point();
}

void CVertex3D::Scale(const CVertex3D& P, double fact)
{
	CVertex3D p = P;
	p = p * (1.0 - fact);
	(*this) = (*this)*(fact);
	(*this) = (*this) + p;
}

void CVertex3D::Mirror(const CVertex3D& P)
{
	CVertex3D p = P;
	(*this) *= -1;
	p *= (2.0);
	(*this) += p;
}

void CVertex3D::Mirror(const COneAxis& Ax)
{
	CVertex3D P1 = Ax.GetPosition();
	CVertex3D P = (*this);
	CVector3D N = Ax.GetDirection();
	CLine3D L(P1, N);
	double D = L.Distance(P);
	CVector3D Q = L.NormalThrough(P) * D * (-2.0);
	CVector3D PV((*this));
	CVector3D R = PV + Q;
	this->SetParam(R.GetX(), R.GetY(), R.GetZ());
}

void CVertex3D::Mirror(const CPlane& Pln)
{
	CPlane pln = Pln;
	CVertex3D P = (*this);
	double D = P.Distance(pln);
	CVector3D N = pln.GetDirection().Unit();
	CVector3D Q = N* D * (-2.0);
	CVertex3D QP = Q.Point();
	(*this) = P + QP;
}

CVertex3D CVertex3D::Origin()
{
	return  CVertex3D(0, 0, 0);
}

istream& operator>>(istream& input, CVertex3D& p)
{
	input >> p.x >> p.y >> p.z;
	return input;
}

ostream& operator<<(ostream& output, CVertex3D& p)
{
	output << p.x << p.y << p.z;
	return output;
}


// Moving
void CVertex3D::Move(const double dx, const double dy, const double dz)
{
	x += dx;
	y += dy;
	z += dz;
}

// Moving
void CVertex3D::Move(CVector3D* pVector, double ratio)
{
	x += ratio * pVector->GetX();
	y += ratio * pVector->GetY();
	z += ratio * pVector->GetZ();
}

// Moving
void CVertex3D::Move(CVector3D& v, double ratio)
{
	x += ratio * v.GetX();
	y += ratio * v.GetY();
	z += ratio * v.GetZ();
}

//-----------------------------------------------------------------------
//////////////////////////////////////////////
// FLAGS
//////////////////////////////////////////////

bool CVertex3D::HasNeighborWithFlag(int flag)
{
	int size = NbVertexNeighbor();
	for (int i = 0; i < size; i++)
	{
		CVertex3D* pVertex = m_ArrayVertexNeighbor[i];
		if (pVertex->GetFlag() == flag)
			return true;
	}
	return false;
}

bool CVertex3D::HasNeighborWithFlagButDiff(int flag, CVertex3D* pVertexDiff)
{
	int size = NbVertexNeighbor();
	for (int i = 0; i < size; i++)
	{
		CVertex3D* pVertex = m_ArrayVertexNeighbor[i];
		if (pVertex != pVertexDiff)
			if (pVertex->GetFlag() == flag)
				return true;
	}
	return false;
}

CVertex3D* CVertex3D::GetFirstVertexNeighborWithFlag(int flag)
{
	int size = NbVertexNeighbor();
	for (int i = 0; i < size; i++)
	{
		CVertex3D* pVertex = m_ArrayVertexNeighbor[i];
		if (pVertex->GetFlag() == flag)
			return pVertex;
	}
	return NULL;
}

CVertex3D* CVertex3D::GetNearestVertexNeighborWithFlag(int flag)
{
	double distance = 1e38;
	CVertex3D* pVertexNearest = NULL;
	int size = NbVertexNeighbor();
	for (int i = 0; i < size; i++)
	{
		CVertex3D* pVertex = m_ArrayVertexNeighbor[i];
		if (pVertex->GetFlag() == flag)
		{
			double tmp = ::DistanceSquare(this, pVertex);
			if (tmp < distance)
			{
				pVertexNearest = pVertex;
				distance = tmp;
			}
		}
	}
	return pVertexNearest;
}


int CVertex3D::DiffFlagsOnNeighboringFaces()
{
	int size = NbFaceNeighbor();
	if (size <= 1)
		return 0;

	int flag = m_ArrayFaceNeighbor[0]->GetFlag(); // first flag
	for (int i = 1; i < size; i++)
	{
		CFaceTriangles* pFace = m_ArrayFaceNeighbor[i];
		int tmp = pFace->GetFlag();
		if (tmp != flag)
			return 1;
	}
	return 0;
}


int CVertex3D::IsFlagSmallestOnNeighboringFaces(int flag)
{
	int size = NbFaceNeighbor();
	for (int i = 0; i < size; i++)
	{
		CFaceTriangles* pFace = m_ArrayFaceNeighbor[i];
		int tmp = pFace->GetFlag();
		if (tmp < flag)
			return 0;
	}
	return 1;
}


CVertex3D* CVertex3D::GetFirstVertexNeighborWithFlagButDiff(int flag, CVertex3D* pVertexDiff)
{
	int size = NbVertexNeighbor();
	for (int i = 0; i < size; i++)
	{
		CVertex3D* pVertex = m_ArrayVertexNeighbor[i];
		if (pVertex != pVertexDiff)
			if (pVertex->GetFlag() == flag)
				return pVertex;
	}
	return NULL;
}


int CVertex3D::NbVertexNeighborWithFlag(int flag)
{
	int nb = 0;
	int size = NbVertexNeighbor();
	for (int i = 0; i < size; i++)
		if (m_ArrayVertexNeighbor[i]->GetFlag() == flag)
			nb++;
	return nb;
}

int CVertex3D::FindVertexNeighborsWhichFlagIsGreater(int flag, CArray3d<CVertex3D>& array)
{
	int added = 0;
	int size = NbVertexNeighbor();
	for (int i = 0; i < size; i++)
	{
		CVertex3D* pVertex = m_ArrayVertexNeighbor[i];
		if (pVertex->GetFlag() > flag)
		{
			array.Add(pVertex);
			added++;
		}
	}
	return added;
}

int CVertex3D::FindVertexNeighborsWhichFlagIsSmaller(int flag, CArray3d<CVertex3D>& array)
{
	int added = 0;
	int size = NbVertexNeighbor();
	for (int i = 0; i < size; i++)
	{
		CVertex3D* pVertex = m_ArrayVertexNeighbor[i];
		if (pVertex->GetFlag() < flag)
		{
			array.Add(pVertex);
			added++;
		}
	}
	return added;
}

//********************************************
// 	GetFirstNeighborWithFlagAndSharingFaceWithFlag
//********************************************
CVertex3D* CVertex3D::GetFirstNeighborWithFlagAndSharingFaceWithFlag(int FlagVertex, int FlagFace)
{
	int size = NbVertexNeighbor();
	for (int i = 0; i < size; i++)
	{
		CVertex3D* pVertex = m_ArrayVertexNeighbor[i];
		if (pVertex->GetFlag() == FlagVertex)
		{
			CArray3d<CFaceTriangles> array;
			if (FindSharingFaces(pVertex, array))
			{
				int NbFace = array.GetSize();
				for (int j = 0; j < NbFace; j++)
				{
					CFaceTriangles* pFace = array[j];
					if (pFace->GetFlag() == FlagFace)
						return pVertex;
				}
			}
		}
	}
	return NULL;
}




//////////////////////////////////////////////
// SHARP EDGES
//////////////////////////////////////////////

//********************************************
// NbSharpEdge
//********************************************
int CVertex3D::NbSharpEdge(const double threshold)
{
	// 3 sharp edges on boundaries
	if (IsOnBoundary())
		return 3;

	int NbSharp = ::NbSharpEdge(m_ArrayFaceNeighbor, threshold);
	//TRACE("NbSharp : %d\n",NbSharp);

	return NbSharp;
}

//********************************************
// 	NormalSum
//********************************************
int CVertex3D::NormalSum(double* pSum)
{
	return ::NormalSum(m_ArrayFaceNeighbor, pSum);
}

//********************************************
// 	NormalMax
//********************************************
int CVertex3D::NormalMax(double* pMax)
{
	return ::NormalMax(m_ArrayFaceNeighbor, pMax);
}


//////////////////////////////////////////////
//////////////////////////////////////////////
// CURVENESS
//////////////////////////////////////////////
//////////////////////////////////////////////

//********************************************
// GetMeanCurveAround
//********************************************
double CVertex3D::GetMeanCurveAround()
{
	int size = m_ArrayFaceNeighbor.GetSize();
	if (size == 0)
		return 0.0f;
	double sum;
	::NormalSum(m_ArrayFaceNeighbor, &sum);
	return sum / size;
}

//********************************************
// GetSumCurveAround
//********************************************
double CVertex3D::GetSumCurveAround()
{
	double sum;
	if (::NormalSum(m_ArrayFaceNeighbor, &sum))
		return sum;
	else
		return 0.0;
}

//********************************************
// GetMaxCurveAround
//********************************************
double CVertex3D::GetMaxCurveAround()
{
	double max;
	if (::NormalMax(m_ArrayFaceNeighbor, &max))
		return max;
	else
	{
		TRACE("** CVertex3D::GetMaxCurveAround\n");
		return 0.0;
	}
}

double CVertex3D::GetMaxAngleAround()
{
	double max;
	if (::MaxAngleBetweenFaces(m_ArrayFaceNeighbor, &max))
		return max;
	else
		return 0.0;
}


//********************************************
// GetMaxCurveAroundAndNeighbors
//********************************************
double CVertex3D::GetMaxCurveAroundAndNeighbors()
{
	double max = 0.0;
	::NormalMax(m_ArrayFaceNeighbor, &max);
	int size = m_ArrayVertexNeighbor.GetSize();
	for (int i = 0; i < size; i++)
		max = max(max, m_ArrayVertexNeighbor[i]->GetMaxCurveAround());
	return max;
}




//////////////////////////////////////////////
// MISC
//////////////////////////////////////////////

//********************************************
// Diff
//********************************************
bool CVertex3D::Diff(CVertex3D* pVertex)
{
	return ((x != pVertex->GetX()) || (y != pVertex->GetY()) || (z != pVertex->GetZ()));
}

//********************************************
// Equal
//********************************************
bool CVertex3D::Equal(CVertex3D* pVertex)
{
	return ((x == pVertex->GetX()) && (y == pVertex->GetY()) && (z == pVertex->GetZ()));
}


//////////////////////////////////////////////
// PRECISION
//////////////////////////////////////////////

//********************************************
// ReplaceOnGrid
//********************************************
void CVertex3D::ReplaceOnGrid(double threshold) // ¡ŸΩÁ÷µ
{
	x = (x < threshold) ? 0.0 : x;
	y = (y < threshold) ? 0.0 : y;
	z = (z < threshold) ? 0.0 : z;
}

//********************************************
// NbNeighbor
//********************************************
int CVertex3D::NbVertexNeighbor(void)
{
	return m_ArrayVertexNeighbor.GetSize();
}

//********************************************
// NbFaceNeighbor
//********************************************
int CVertex3D::NbFaceNeighbor(void)
{
	return m_ArrayFaceNeighbor.GetSize();
}

//********************************************
// Find each faces sharing between pVertex
// and "this"
// We assume adjacency is built
//********************************************
int CVertex3D::FindSharingFaces(CVertex3D* pVertex, CArray3d<CFaceTriangles>& array)
{
	int added = 0;
	int NbFaceNeighbor = pVertex->NbFaceNeighbor();
	for (int i = 0; i < NbFaceNeighbor; i++)
	{
		if (m_ArrayFaceNeighbor.Has(pVertex->GetFaceNeighbor(i)))
		{
			array.Add(pVertex->GetFaceNeighbor(i));
			added++;
		}
	}
	return added;
}


//////////////////////////////////////////////
// BOUNDARIES
//////////////////////////////////////////////


//********************************************
// IsOnBoundary
//********************************************
bool CVertex3D::IsOnBoundary()
{
	// ** DEBUG **
	int size = m_ArrayFaceNeighbor.GetSize();
	for (int i = 0; i < size; i++)
		for (int j = 0; j < 3; j++)
			if (m_ArrayFaceNeighbor[i]->f(j) == NULL && (m_ArrayFaceNeighbor[i]->v(j) == this || m_ArrayFaceNeighbor[i]->v((j + 1) % 3) == this))
				return true;
	return false;
}

//********************************************
// GetMeanLengthEdgeAround
//********************************************
double CVertex3D::GetMeanLengthEdgeAround()
{
	double sum = 0.0f;
	int size = m_ArrayVertexNeighbor.GetSize();
	if (size == 0)
		return 0.0f;

	for (int i = 0; i < size; i++)
		sum += ::Distance(m_ArrayVertexNeighbor[i], this);

	return (sum / size);
}


//********************************************
// AddFaceNeighbor
//********************************************
void CVertex3D::AddNeighbor(CFaceTriangles* pFace)
{
	// Add a neighboring face only when it
	// is not included in the array
	if (!HasNeighbor(pFace))
		m_ArrayFaceNeighbor.Add(pFace);
}

//********************************************
// AddNeighbor
//********************************************
void CVertex3D::AddNeighbor(CVertex3D* pVertex)
{
	// Duplicate vertices are removed
	if (pVertex == this)
		return;

	if (!HasNeighbor(pVertex))
		m_ArrayVertexNeighbor.Add(pVertex);
}

//********************************************
// RemoveAllNeighbor
//********************************************
void CVertex3D::RemoveAllVertexNeighbor(void)
{
	m_ArrayVertexNeighbor.RemoveAll();
}

//********************************************
// RemoveAllFaceNeighbor
//********************************************
void CVertex3D::RemoveAllFaceNeighbor(void)
{
	m_ArrayFaceNeighbor.RemoveAll();
}

//////////////////////////////////////////////////////////////////////////
// —’…´
void CVertex3D::SetColor(const GLubyte& red, const GLubyte& green, const GLubyte& blue)
{
	m_itsColor.Set(red, green, blue);
}

void CVertex3D::SetColor(CColor& color)
{
	m_itsColor.Set(color);
}

void CVertex3D::GetColor(GLubyte* colVect) const
{
	colVect = new GLubyte[3];
	colVect[0] = m_itsColor.GetR();
	colVect[1] = m_itsColor.GetG();
	colVect[2] = m_itsColor.GetB();
}

CColor* CVertex3D::GetColor(void)
{
	return &m_itsColor;
}