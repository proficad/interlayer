//********************************************
// Utils3d.cpp
//********************************************
// alliez@usc.edu
// Created : 29/01/97
// Modified : 14/09/98
//********************************************

#include "stdafx.h"
#include "math.h"
#include "Utils3d.h"
#include "MMath.h"

int round(double x)
{
	int _ceil = (int)ceil(x);
	int _floor = (int)floor(x);
	if ((_ceil - x) < (x - _floor))
		return _ceil;
	else
		return _floor;
}

CVector3D CalculateNormal(const CVector3D& v1, const CVector3D& v2)
{
	CVector3D Normal;
	CVector3D u(v1);
	CVector3D v(v2);
	u.Inner(v);
	Normal = u;
	Normal.NormalizeL2();
	return Normal;
}

//**************************************
// SinAngle between two faces
// u  ^ v  = w
// |w| = |u||v| |sin(u,v)|
//**************************************
double SinAngle(CPlane* pFace1, CPlane* pFace2)
{
	ASSERT(pFace1 != NULL);
	ASSERT(pFace2 != NULL);

	pFace1->CalculateNormal();
	pFace2->CalculateNormal();

	if (!pFace1->HasNormal() || !pFace2->HasNormal())
		return 0.0f;

	// Avoid illegal call of non-static function
	CVector3D& w = CVector3D::Inner(pFace1->GetNormal(), pFace2->GetNormal());
	double Norm1 = pFace1->GetNormal().GetNormL2();
	double Norm2 = pFace2->GetNormal().GetNormL2();
	double Norm_w = w.GetNormL2();

	if (Norm1 * Norm2)
		return (Norm_w / (Norm1 * Norm2));
	else
	{
		TRACE("** Norms : %g %g\n", Norm1, Norm2);
		return 0.0;
	}
}


//**************************************
// CosAngle between two faces
// uv = |u||v| cos(u,v)
//**************************************
double CosAngle(CPlane* pFace1, CPlane* pFace2)
{
	ASSERT(pFace1 != NULL);
	ASSERT(pFace2 != NULL);

	pFace1->CalculateNormal();
	pFace2->CalculateNormal();

	if (!pFace1->HasNormal() || !pFace2->HasNormal())
		return 0.0f;

	CVector3D U = pFace1->GetNormal();
	CVector3D V = pFace2->GetNormal();
	double NormU = U.GetNormL2();
	double NormV = V.GetNormL2();
	double product = NormU* NormV;

	// Check
	if (product == 0)
	{
		TRACE("** Norms : %g %g\n", NormU, NormV);
		return 0.0;
	}

	double scalar = ::Scalar(U, V);

	return (scalar / product);
}

//**************************************
// Angle between two faces (in radians)
// we use this formula
// uv = |u||v| cos(u,v)
// u  ^ v  = w
// |w| = |u||v| |sin(u,v)|
//**************************************
double Angle(CPlane* pFace1, CPlane* pFace2)
{
	ASSERT(pFace1 != NULL);
	ASSERT(pFace2 != NULL);

	pFace1->CalculateNormal();
	pFace2->CalculateNormal();

	if (!pFace1->HasNormal() || !pFace2->HasNormal())
		return 0.0f;

	CVector3D U = pFace1->GetNormal();
	CVector3D V = pFace2->GetNormal();

	return Angle(U, V);
}

//**************************************
// Angle between two vectors (in radians)
// we use this formula
// uv = |u||v| cos(u,v)
// u  ^ v  = w
// |w| = |u||v| |sin(u,v)|
//**************************************
double Angle(CVector3D* pU, CVector3D* pV)
{
	double NormU = pU->GetNormL2();
	double NormV = pV->GetNormL2();
	double product = NormU* NormV;

	// Check
	if (product == 0)
	{
		TRACE("** Norms : %g %g\n", NormU, NormV);
		return 0.0;
	}

	double scalar = ::Scalar(pU, pV);
	ASSERT(product != 0);
	double cosinus = scalar / product;

	// Sinus
	CVector3D w = CVector3D::Inner(pU, pV);
	double Norm_w = w.GetNormL2();

	double AbsSinus = Norm_w / product;

	// Remove degeneracy
	AbsSinus = (AbsSinus > 1) ? 1 : AbsSinus;
	AbsSinus = (AbsSinus < -1) ? -1 : AbsSinus;
	if (AbsSinus > 1)
	{
		TRACE("Remove degeneracy (AbsSinus : %lg)\n", AbsSinus);
	}

	if (cosinus >= 0)
		return asin(AbsSinus);
	else
		return (PI - asin(AbsSinus));
}

double Angle(const CVector3D& U, const CVector3D& V)
{
	double NormU = U.GetNormL2();
	double NormV = V.GetNormL2();
	double product = NormU* NormV;

	// Check
	if (product == 0)
	{
		TRACE("** Norms : %g %g\n", NormU, NormV);
		return 0.0;
	}

	double scalar = ::Scalar(U, V);
	ASSERT(product != 0);
	double cosinus = scalar / product;

	// Sinus
	CVector3D w = CVector3D::Inner(U, V);
	double Norm_w = w.GetNormL2();

	double AbsSinus = Norm_w / product;

	// Remove degeneracy
	AbsSinus = (AbsSinus > 1) ? 1 : AbsSinus;
	AbsSinus = (AbsSinus < -1) ? -1 : AbsSinus;
	if (AbsSinus > 1)
	{
		TRACE("Remove degeneracy (AbsSinus : %lg)\n", AbsSinus);
	}

	if (cosinus >= 0)
		return asin(AbsSinus);
	else
		return (PI - asin(AbsSinus));
}


//**************************************
// AddFaceNoDuplicate
//**************************************
int AddFaceNoDuplicate(CArray3d<CFaceTriangles>& array, CFaceTriangles* pFace)
{
	int size = array.GetSize();
	for (int i = 0; i < size; i++)
	{
		if (array[i] == pFace)
			return 0;
	}
	array.Add(pFace);
	//TRACE("** AddFaceNoDuplicate : %x\n",pFace);

	return 1;
}

//**************************************
// AddFaceNoDuplicate
//**************************************
int AddFaceRecursive(CArray3d<CFaceTriangles>& array, CFaceTriangles* pFace, CVertex3D* pVertex)
{
	// Add vertex, if needed
	if (pFace->HasVertex(pVertex))
	{
		if (!AddFaceNoDuplicate(array, pFace))
			return 0;
	}
	else
		return 0;

	// Search in neighbors
	for (int i = 0; i < 3; i++)
	{
		CFaceTriangles* pFaceNeighbor = pFace->f(i);
		if (pFaceNeighbor != NULL)
			if (pFaceNeighbor->HasVertex(pVertex))
				AddFaceRecursive(array, pFaceNeighbor, pVertex);
	}

	return 1;
}


//**************************************
// NbSharpEdge
//**************************************
int NbSharpEdge(CArray3d<CFaceTriangles>& array, double threshold)
{
	int size = array.GetSize();

	// Check 
	if (size == 0)
		return 0;

	// Only one face, 2 sharp edges
	if (size == 1)
		return 2;

	int NbSharp = 0;

	for (int i = 0; i < size - 1; i++)
	{
		CFaceTriangles* pFace = array[i];
		for (int j = i + 1; j < size; j++)
		{
			CFaceTriangles* pFaceTest = array[j];
			if (pFace->HasNeighbor(pFaceTest))
				NbSharp += (SinAngle(pFace, pFaceTest) >= threshold);
		}
	}

	return NbSharp;
}


//**************************************
// NormalSum
//**************************************
int NormalSum(CArray3d<CFaceTriangles>& array, double* pSum)
{
	int size = array.GetSize();

	// Check 
	if (size == 0)
		return 0;

	// Only one face
	if (size == 1)
		return 0;

	double sum = 0;
	for (int i = 0; i < size - 1; i++)
	{
		CFaceTriangles* pFace = array[i];
		for (int j = i + 1; j < size; j++)
		{
			CFaceTriangles* pFaceTest = array[j];
			if (pFace->HasNeighbor(pFaceTest))
				sum += SinAngle(pFace, pFaceTest);
		}
	}

	*pSum = sum;
	//TRACE("Value : %g\n",sum);
	return 1;
}

//**************************************
// NormalMax
// (in radians)
//**************************************
int NormalMax(CArray3d<CFaceTriangles>& array, double* pMax)
{
	int size = array.GetSize();

	// Check 
	if (size == 0)
		return 0;

	// Only one face
	if (size == 1)
		return 0;

	double max = 0;
	for (int i = 0; i < size - 1; i++)
	{
		CFaceTriangles* pFace = array[i];
		for (int j = i + 1; j < size; j++)
		{
			CFaceTriangles* pFaceTest = array[j];
			if (pFace->HasNeighbor(pFaceTest))
			{
				double sin = SinAngle(pFace, pFaceTest);
				max = (sin > max) ? sin : max;
			}
		}
	}

	*pMax = max;
	max = (max > 1) ? 1 : max; // precision work-around
	//TRACE("Value : %g\n",max);

	return 1;
}


int MaxAngleBetweenFaces(CArray3d<CFaceTriangles>& array, double* pMax)
{
	int size = array.GetSize();
	// Check 
	if (size == 0)
		return 0;
	// Only one face
	if (size == 1)
		return 0;
	double max = 0;
	for (int i = 0; i < size - 1; i++)
	{
		CFaceTriangles* pFace = array[i];
		for (int j = i + 1; j < size; j++)
		{
			CFaceTriangles* pFaceTest = array[j];
			if (pFace->HasNeighbor(pFaceTest))
			{
				double angle = ::Angle(pFace, pFaceTest);
				max = (angle > max) ? angle : max;
			}
		}
	}

	*pMax = max;
	return 1;
}


//**************************************
// DistanceSquare
//**************************************
double DistanceSquare(CVertex3D* pVertex1, CVertex3D* pVertex2)
{
	return ((pVertex1->GetX() - pVertex2->GetX()) * (pVertex1->GetX() - pVertex2->GetX()) + (pVertex1->GetY() - pVertex2->GetY()) * (pVertex1->GetY() - pVertex2->GetY()) + (pVertex1->GetZ() - pVertex2->GetZ()) * (pVertex1->GetZ() - pVertex2->GetZ()));
}

//**************************************
// Distance
//**************************************
double Distance(CVertex3D* pVertex1, CVertex3D* pVertex2)
{
	return sqrt(DistanceSquare(pVertex1, pVertex2));
}

//**************************************
// DistanceVertexToLine
// Line go through Va and Vb
// Projected vertex is stored in 
// pVertexProjected
//**************************************
double DistanceVertexToLine(CVertex3D* pVertex, CVertex3D* pVa, CVertex3D* pVb, CVertex3D* pVertexProjected)
{
	CVector3D VectorAC(pVa, pVertex);
	CVector3D VectorAB(pVa, pVb);
	double DistanceSquareAB = DistanceSquare(pVa, pVb);
	double temp = Scalar(&VectorAC, &VectorAB) / DistanceSquareAB;
	VectorAB *= temp;
	pVertexProjected->SetParam(pVa);
	pVertexProjected->Move(&VectorAB);
	return Distance(pVertexProjected, pVertex);
}

//**************************************
// DistanceVertexToLine
// Line go through Va and Vb
// Projected vertex is stored in 
// pVertexProjected
//**************************************
int ProjectVertexOnLine(CVertex3D* pVertex, CVertex3D* pVa, CVertex3D* pVb, CVertex3D* pVertexProjected)
{
	CVector3D VectorAC(pVa, pVertex);
	CVector3D VectorAB(pVa, pVb);
	double DistanceSquareAB = DistanceSquare(pVa, pVb);
	if (DistanceSquareAB == 0.0f)
		return 0; // no line
	double temp = Scalar(&VectorAC, &VectorAB) / DistanceSquareAB;
	VectorAB *= temp;
	pVertexProjected->SetParam(pVa);
	pVertexProjected->Move(&VectorAB);
	return 1;
}


//**************************************
// DistanceVertexToFace
// Projected vertex is stored in 
// pVertexProjected
//**************************************
double SquareDistanceVertexToFaceIfInf(CVertex3D* pVertex, CFaceTriangles* pFace, CVertex3D* pVertexProjected, double OldDistance)
{
	// Project point on triangle's plane
	ProjectPointOnFace(pVertex, pFace, pVertexProjected);

	if (DistanceSquare(pVertexProjected, pVertex) > OldDistance && !VertexInFace(pVertexProjected, pFace))
		return -1.0; // flag
	else
		return SquareDistanceVertexToFace(pVertex, pFace, pVertexProjected);
}

//**************************************
// DistanceVertexToFace
// Projected vertex is stored in 
// pVertexProjected
//**************************************
double SquareDistanceVertexToFace(CVertex3D* pVertex, CFaceTriangles* pFace, CVertex3D* pVertexProjected)
{
	// ** DEBUG **

	// Project point on triangle's plane
	ProjectPointOnFace(pVertex, pFace, pVertexProjected);

	// Is point in triangle  ?
	if (VertexInFace(pVertexProjected, pFace))
	{
		// TRACE(" vertex in face... (projected : %g %g %g)\n",pVertexProjected->x(),pVertexProjected->y(),pVertexProjected->z());
		return DistanceSquare(pVertexProjected, pVertex);
	}

	// Out, also project "projected vertex" on 3 lines
	CVertex3D VertexProjectedOnLine[3];
	for (int i = 0; i < 3; i++)
	{
		ProjectVertexOnLine(pVertexProjected, pFace->v(i), pFace->v((i + 1) % 3), &VertexProjectedOnLine[i]);
		//// TRACE("  ProjectVertexOnLine : (%g %g %g)...\n",VertexProjectedOnLine[i].x(),VertexProjectedOnLine[i].y(),VertexProjectedOnLine[i].z());
	}

	// Is point on segment of face ?
	int success = 0;
	for (int i = 0; i < 3; i++)
	{
		if (VertexOnSegment(&VertexProjectedOnLine[i], pFace->v(i), pFace->v((i + 1) % 3)))
		{
			success = 1;
			VertexProjectedOnLine[i].SetFlag(1);
			//// TRACE("  vertex on segment of face (%g %g %g)...\n",VertexProjectedOnLine[i].x(),VertexProjectedOnLine[i].y(),VertexProjectedOnLine[i].z());
		}
	}

	// At least one vertex on segment
	if (success)
	{
		// Compare with projected points in segment and vertices of triangle

		// Get nearest vertex
		CVertex3D* pVertexTmp = pFace->FindNearestVertex(pVertexProjected);
		CVertex3D VertexProjectedFinal(pVertexTmp);
		double MinDistance = DistanceSquare(pVertexProjected, pVertexTmp);

		for (int i = 0; i < 3; i++)
		{
			if (VertexProjectedOnLine[i].GetFlag())
			{
				VertexProjectedOnLine[i].SetFlag(0); // Restore
				double tmp = DistanceSquare(pVertexProjected, &VertexProjectedOnLine[i]);
				if (tmp <= MinDistance)
				{
					VertexProjectedFinal.SetParam(VertexProjectedOnLine[i]);
					MinDistance = tmp;
				}
			}
		}
		pVertexProjected->SetParam(VertexProjectedFinal);
		// TRACE("  nearest vertex on face (%g %g %g)...\n",VertexProjectedFinal.x(),VertexProjectedFinal.y(),VertexProjectedFinal.z());
		return DistanceSquare(&VertexProjectedFinal, pVertex);
	}

	// Out, we must search nearest vertex
	CVertex3D* pVertexProjectedFinal = pFace->FindNearestVertex(pVertexProjected);

	pVertexProjected->SetParam(pVertexProjectedFinal);
	// TRACE("  nearest vertex on face (%g %g %g)...\n",pVertexProjectedFinal->x(),pVertexProjectedFinal->y(),pVertexProjectedFinal->z());
	return DistanceSquare(pVertexProjectedFinal, pVertex);
}


//********************************************
// SquareDistanceToFaceFast
//********************************************
// Search min distance between pVertex and 
// mesh's faces around pFace
//********************************************
double SquareDistanceToSetOfFace(CVertex3D* pVertex, CVertex3D* pVertexRef, CFaceTriangles** ppFace, CArray3d<CFaceTriangles>& ArrayFace)
{
	ASSERT(pVertex != NULL);
	ASSERT(pVertexRef != NULL);

	int size = ArrayFace.GetSize();
	ASSERT(size != 0);

	// For each face in set
	CVertex3D vertex;
	double distance = MAX_DOUBLE;

	for (int i = 0; i < size; i++)
	{
		double tmp = SquareDistanceVertexToFaceIfInf(pVertex, ArrayFace[i], &vertex, distance);
		if (tmp < distance && tmp != -1.0f)
		{
			distance = tmp;
			pVertexRef->SetParam(vertex);
			*ppFace = ArrayFace[i];
		}
	}

	//TRACE("Dist : %g vertex(%g,%g,%g)",distance,pVertex->x(),pVertex->y(),pVertex->z());

	return distance;
}

//********************************************
// MaxSquareDistanceVertexFace
// For each vertex, search smallest distance 
// to set of faces (pArrayFace)
// Return max founded
//********************************************
double MaxSquareDistanceVertexFace(CArray3d<CVertex3D>* pArrayVertex, CArray3d<CFaceTriangles>* pArrayFace, CVertex3D** ppVertex, CFaceTriangles** ppFace)
{
	int NbVertex = pArrayVertex->GetSize();
	int NbFace = pArrayFace->GetSize();

	// Check
	ASSERT(NbVertex != 0 && NbFace != 0);

	// Non-optimized
	CVertex3D v;
	double max = 0.0;
	double distance;
	for (int i = 0; i < NbVertex; i++)
	{
		// Get distance to set of faces
		distance = MAX_DOUBLE;
		int j;
		for (j = 0; j < NbFace; j++)
		{
			double d = ::SquareDistanceVertexToFace(pArrayVertex->GetAt(i), pArrayFace->GetAt(j), &v);
			if (d < distance)
				distance = d;
		}
		// Get max for each vertex
		if (distance > max)
		{
			max = distance;
			if (ppFace != NULL && ppVertex != NULL)
			{
				*ppFace = pArrayFace->GetAt(j);
				*ppVertex = pArrayVertex->GetAt(i);
			}
		}
	}
	return max;
}


//**************************************
// Scalar product
//**************************************
double Scalar(CVector3D* pV1, CVector3D* pV2)
{
	return (pV1->GetX() * pV2->GetX() + pV1->GetY() * pV2->GetY() + pV1->GetZ() * pV2->GetZ());
}

//**************************************
// Opposite per coord
//**************************************
int OppositePerCoord(CVector3D* pV1, CVector3D* pV2)
{
	return (pV1->GetX() * pV2->GetX() < 0.0f && pV1->GetY() * pV2->GetY() < 0.0f && pV1->GetZ() * pV2->GetZ() < 0.0f);
}

//**************************************
// Scalar product
//**************************************
double Scalar(const CVector3D& v1, const CVector3D& v2)
{
	return (v1.GetX() * v2.GetX() + v1.GetY() * v2.GetY() + v1.GetZ() * v2.GetZ());
}

//********************************************
// Inner
//********************************************
CVector3D Inner(CVector3D* u, CVector3D* v)
{
	// w = u ^ v
	CVector3D w;
	w.SetParam((u->GetY() * v->GetZ() - u->GetZ() * v->GetY()), (u->GetZ() * v->GetX() - u->GetX() * v->GetZ()), (u->GetX() * v->GetY() - u->GetY() * v->GetX()));
	return w;
}

//**************************************
// PlaneEquation
// Plane : ax + by + cz + d = 0
// Normal : a,b,c
// d : distance from origin along normal 
//     to the plane
//**************************************
int PlaneEquationFromFace(CPlane* pFace, double* a, double* b, double* c, double* d)
{
	ASSERT(a != NULL);
	ASSERT(b != NULL);
	ASSERT(c != NULL);
	ASSERT(d != NULL);

	pFace->CalculateNormal();
	if (!pFace->HasNormal())
		return 0;

	pFace->GetPlaneParam(a, b, c, d);

	return 1;
}

//**************************************
// VertexOnSegment
//**************************************
int VertexOnSegment(CVertex3D* pVertex, CVertex3D* pV1, CVertex3D* pV2)
{
	CVector3D v1(pV1, pVertex);	
	CVector3D v2(pV2, pVertex);	
	return (Scalar(v1, v2) <= 0);
}

//**************************************
// ProjectPointOnPlane
// Plane : ax + by + cz + d = 0
// We store projected vertex coordinates 
// in pVertexProjected
//**************************************
int ProjectPointOnPlane(CVertex3D* pVertex, CVertex3D* pVertexProjected, double a, double b, double c, double d)
{
	// We need valid plane
	double div = a* a + b* b + c* c;
	ASSERT(div != 0.0f);
	if (div == 0.0f)
		return 0;

	double x = pVertex->GetX();
	double y = pVertex->GetY();
	double z = pVertex->GetZ();
	double t = -(a* x + b* y + c* z + d) / div;
	pVertexProjected->SetParam(a * t + x, b * t + y, c * t + z);

	return 1;
}

//**************************************
// ProjectPointOnFace
// We store projected vertex coordinates 
// in pVertexProjected
//**************************************
int ProjectPointOnFace(CVertex3D* pVertex, CFaceTriangles* pFace, CVertex3D* pVertexProjected)
{
	// Get plane coeff
	double a, b, c, d;
	if (PlaneEquationFromFace(pFace, &a, &b, &c, &d))
		return ProjectPointOnPlane(pVertex, pVertexProjected, a, b, c, d);
	else
		return 0;
	//TRACE("** Plane equation : %f %f %f %f\n",a,b,c,d);
	// Project point on triangle's plane
}


//**************************************
// IsVertexInFace
// We know vertex is in plane of
// triangle, and just test whenever
// vertex V is in triangular face F
//**************************************
int VertexInFace(CVertex3D* pVertex, CFaceTriangles* pFace)
{
	CVector3D vv1(pVertex, pFace->v1());	
	CVector3D vv2(pVertex, pFace->v2());	
	CVector3D vv3(pVertex, pFace->v3());	

	CVector3D w1 = CVector3D::Inner(&vv1, &vv2);
	CVector3D w2 = CVector3D::Inner(&vv2, &vv3);
	CVector3D w3 = CVector3D::Inner(&vv3, &vv1);

	return (Scalar(&w1, &w2) >= 0 && Scalar(&w2, &w3) >= 0 && Scalar(&w1, &w3) >= 0);
}


//**************************************
// Spring term 
// SIGMA(j,k)(k|Vj - Vk|^2);
//**************************************
double Spring(CVertex3D* pVertex, double k)
{
	// Check
	int NbVertexNeighbor = pVertex->NbVertexNeighbor();
	if (NbVertexNeighbor == 0)
		return 0.0f;

	double sum = 0.0f;
	for (int i = 0; i < NbVertexNeighbor; i++)
		sum += DistanceSquare(pVertex, pVertex->GetVertexNeighbor(i));

	return (sum * k);
}

//**************************************
// Area
//**************************************
double Area(CVertex3D* pV0, CVertex3D* pV1, CVertex3D* pV2)
{
	ASSERT(pV0 != NULL);
	ASSERT(pV1 != NULL);
	ASSERT(pV2 != NULL);
	CVector3D bc(pV1, pV2);
	CVector3D ba(pV1, pV0);
	//bc.Trace();
	//ba.Trace();
	CVector3D v = ::Inner(&bc, &ba);
	//v.Trace();
	return v.GetNormL2() / 2.0;
}

//**************************************
// GetMeanArea
//**************************************
double GetMeanArea(CArray3d<CFaceTriangles>* pArrayFace)
{
	int size = pArrayFace->GetSize();
	ASSERT(size > 0);
	double sum = 0.0f;
	for (int i = 0; i < size; i++)
		sum += pArrayFace->GetAt(i)->Area();
	return (sum / size);
}

//**************************************
// GetMinArea
//**************************************
double GetMinArea(CArray3d<CFaceTriangles>* pArrayFace)
{
	int size = pArrayFace->GetSize();
	ASSERT(size > 0);
	double min = MAX_DOUBLE;
	for (int i = 0; i < size; i++)
	{
		double area = pArrayFace->GetAt(i)->Area();
		//TRACE("Area : %g\n",area);
		min = (area < min) ? area : min;
	}
	return min;
}


//**************************************
// FormFunction
//**************************************
// Ratio (area(013)/area(012))
// Triangle : 012
// Vertex on which we want to evaluate 
// form function : 3
//**************************************
double FormFunction(CVertex3D* pV0, CVertex3D* pV1, CVertex3D* pV2, CVertex3D* pV3)
{
	double area = Area(pV0, pV1, pV2);
	ASSERT(area != 0.0);
	return Area(pV0, pV1, pV3) / area;
}


//**************************************
// IntersectionLineFace
//**************************************
int IntersectionLineFace(CVertex3D* pV0, CVertex3D* pV1, CFaceTriangles* pFace, CVertex3D* pVertexResult)
{
	double a, b, c, d;
	PlaneEquationFromFace(pFace, &a, &b, &c, &d);
	double i = pV1->GetX() - pV0->GetX();
	double j = pV1->GetY() - pV0->GetY();
	double k = pV1->GetZ() - pV0->GetZ();

	double den = a* i + b* j + c* k;
	double num = -(a* pV0->GetX() + b* pV0->GetY() + c* pV0->GetZ() + d);

	// parallel or face contain line
	if (den == 0.0f)
		return 0;

	double t = num / den;
	pVertexResult->SetParam(pV0->GetX() + t * i, pV0->GetY() + t * j, pV0->GetZ() + t * k);

	return VertexInFace(pVertexResult, pFace);
}

//********************************************
// IntersectionLineFaceRecursive
// (propagation from pFaceStart)
//********************************************
int IntersectionLineFaceRecursive(CVertex3D* pV0, CVertex3D* pV1, CFaceTriangles* pFaceStart, CVertex3D* pVertexResult, CFaceTriangles** ppFaceResult, CArray3d<CFaceTriangles>* pArrayFace, int MaxNbFace)
{
	if (pArrayFace->GetSize() >= MaxNbFace)
		return 0;

	pArrayFace->Add(pFaceStart);
	if (IntersectionLineFace(pV0, pV1, pFaceStart, pVertexResult))
	{
		*ppFaceResult = pFaceStart;
		return 1;
	}

	//TRACE("Size : %d (max : %d)\n",pArrayFace->GetSize(),MaxNbFace);

	pFaceStart->SetFlag(1);

	// neighboring
	for (int i = 0; i < 3; i++)
	{
		CFaceTriangles* pFace = pFaceStart->f(i);
		if (pFace != NULL)  // neighbor exists
			if (pFace->GetFlag() != 1) // not yet
				if (IntersectionLineFaceRecursive(pV0, pV1, pFace, pVertexResult, ppFaceResult, pArrayFace, MaxNbFace))
					return 1;
	}

	return 0;
}

//********************************************
// NearestIntersectionWithLine
// First intersection founded (propagation
// from pFaceStart)
//********************************************
int NearestIntersectionWithLine(CVertex3D* pV0, CVertex3D* pV1, CFaceTriangles* pFaceStart, CVertex3D* pVertexResult, CFaceTriangles** ppFaceResult, int MaxNbFace)
{
	// Check
	ASSERT(pV0 != NULL);
	ASSERT(pV1 != NULL);
	ASSERT(pFaceStart != NULL);
	ASSERT(pVertexResult != NULL);

	// Use flag
	CArray3d<CFaceTriangles> ArrayFace;
	int success = IntersectionLineFaceRecursive(pV0, pV1, pFaceStart, pVertexResult, ppFaceResult, &ArrayFace, MaxNbFace);
	for (int i = 0; i < ArrayFace.GetSize(); i++)
		ArrayFace[i]->SetFlag(0);

	//TRACE("NbFace : %d\n",ArrayFace.GetSize());
	ArrayFace.SetSize(0);

	return success;
}


//********************************************
// NearestIntersectionWithLine
// Only search in pArrayFace
//********************************************
int NearestIntersectionWithLine(CArray3d<CFaceTriangles>* pArrayFace, CVertex3D* pV0, CVertex3D* pV1, CVertex3D* pVertexResult, CFaceTriangles** ppFaceResult, int* pNbVisitedFace)
{
	int success = 0;
	*ppFaceResult = NULL;
	int size = pArrayFace->GetSize();
	*pNbVisitedFace = size;
	double SqDistance = MAX_DOUBLE;
	// For each face
	for (int i = 0; i < size; i++)
	{
		CVertex3D v;
		if (IntersectionLineFace(pV0, pV1, pArrayFace->GetAt(i), &v))
		{
			success++;
			double d = ::DistanceSquare(pV0, &v);
			if (d < SqDistance)
			{
				SqDistance = d;
				pVertexResult->SetParam(v);
				// this face contains intersection
				*ppFaceResult = pArrayFace->GetAt(i);
			}
		}
	}
	//TRACE("NbIntersection : %d\n",success);
	if (success)
	{
		ASSERT(ppFaceResult != NULL);
	}
	return success; // NbIntersection
}


//********************************************
// NearestIntersectionWithLine
// Only search in pFace and its neighbors
//********************************************
int NearestIntersectionWithLineFromFaceAndNeighbors(CFaceTriangles* pFace, CVertex3D* pV0, CVertex3D* pV1, CVertex3D* pVertexResult, CFaceTriangles** ppFaceResult)
{
	int success = 0;
	*ppFaceResult = NULL;
	double SqDistance = MAX_DOUBLE;
	CVertex3D v;

	// pFace alone
	if (IntersectionLineFace(pV0, pV1, pFace, &v))
	{
		success++;
		double d = ::DistanceSquare(pV0, &v);
		if (d < SqDistance)
		{
			SqDistance = d;
			pVertexResult->SetParam(v);
			*ppFaceResult = pFace;
		}
	}

	// Its neighbors
	for (int i = 0; i < 3; i++)
	{
		CFaceTriangles* pFaceNeighbor = pFace->f(i);
		if (pFaceNeighbor != NULL) // neighbor
			if (IntersectionLineFace(pV0, pV1, pFaceNeighbor, &v))
			{
				success++;
				double d = ::DistanceSquare(pV0, &v);
				if (d < SqDistance)
				{
					SqDistance = d;
					pVertexResult->SetParam(v);
					*ppFaceResult = pFaceNeighbor;
				}
			}
	}

	if (success)
	{
		ASSERT((*ppFaceResult) != NULL);
	}
	return success;
}



int GravityCenter(CArray3d<CVertex3D>* pArray, CVertex3D* pVertexCenter)
{
	int size = pArray->GetSize();
	for (int j = 0; j < 3; j++)
	{
		double v = 0.0f;
		for (int i = 0; i < size; i++)
			v += pArray->GetAt(i)->GetParam(j);
		v /= size;
		pVertexCenter->SetParam(j, v);
	}
	return 1;
}

// ** EOF **
