//********************************************
// Utils3d.h
//********************************************
// alliez@usc.edu
// Created : 29/01/97
// Modified : 19/02/97
//********************************************

#ifndef _UTILS_3D_
#define _UTILS_3D_

#include "Array3d.h"
#include "Vector3d.h"
#include "Vector3d.h"
#include "FaceTriangles.h"
#include "Plane.h"

#define MAX_FLOAT 3.4e+38f; 
#define MAX_DOUBLE 1.7e+308; 
//#define PI 3.14159265359;

// Rounding
int round(double x);
// Angle between faces
double SinAngle(CPlane* pFace1, CPlane* pFace2);
double CosAngle(CPlane* pFace1, CPlane* pFace2);
double Angle(CPlane* pFace1, CPlane* pFace2);
double Angle(CVector3D* pU, CVector3D* pV);
double Angle(const CVector3D& U, const CVector3D& V);

// Distance
double DistanceSquare(CVertex3D* pVertex1, CVertex3D* pVertex2);
double Distance(CVertex3D* pVertex1, CVertex3D* pVertex2);
double SquareDistanceVertexToFaceIfInf(CVertex3D* pVertex, CFaceTriangles* pFace, CVertex3D* pVertexProjected, double OldDistance);
double SquareDistanceVertexToFace(CVertex3D* pVertex, CFaceTriangles* pFace, CVertex3D* pVertexProjected);
double SquareDistanceToSetOfFace(CVertex3D* pVertex, CVertex3D* pVertexRef, CFaceTriangles** ppFace, CArray3d<CFaceTriangles>& ArrayFace);
double MaxSquareDistanceVertexFace(CArray3d<CVertex3D>* pArrayVertex, CArray3d<CFaceTriangles>* pArrayFace, CVertex3D** ppVertex = NULL, CFaceTriangles** pFace = NULL);

// Operators on vectors
CVector3D Inner(CVector3D* u, CVector3D* v);
double Scalar(CVector3D* pV1, CVector3D* pV2);
double Scalar(const CVector3D& v1, const CVector3D& v2);
int OppositePerCoord(CVector3D* pV1, CVector3D* pV2);

// Vertex / faces
int VertexInFace(CVertex3D* pVertex, CFaceTriangles* pFace);

// Faces
int AddFaceNoDuplicate(CArray3d<CFaceTriangles>& array, CFaceTriangles* pFace);
int AddFaceRecursive(CArray3d<CFaceTriangles>& array, CFaceTriangles* pFace, CVertex3D* pVertex);

// Sharp edges
int NbSharpEdge(CArray3d<CFaceTriangles>& array, double threshold);

// Normals 
int NormalSum(CArray3d<CFaceTriangles>& array, double* pSum);
int NormalMax(CArray3d<CFaceTriangles>& array, double* pMax);
int MaxAngleBetweenFaces(CArray3d<CFaceTriangles>& array, double* pMax);

// Plane
int PlaneEquationFromFace(CPlane* pFace, float* a, float* b, float* c, float* d);

// Vertex / segment
int VertexOnSegment(CVertex3D* pVertex, CVertex3D* pV1, CVertex3D* pV2);

// Projection
int ProjectPointOnPlane(CVertex3D* pVertex, CVertex3D* pVertexProjected, float a, float b, float c, float d);
int ProjectPointOnFace(CVertex3D* pVertex, CFaceTriangles* pFace, CVertex3D* pVertexProjected);
int ProjectVertexOnLine(CVertex3D* pVertex, CVertex3D* pVa, CVertex3D* pVb, CVertex3D* pVertexProjected);

// Spring
float Spring(CVertex3D* pVertex, float k);

// Area
double Area(CVertex3D* pV0, CVertex3D* pV1, CVertex3D* pV2);
double FormFunction(CVertex3D* pV0, CVertex3D* pV1, CVertex3D* pV2, CVertex3D* pV3);
double GetMeanArea(CArray3d<CFaceTriangles>* pArrayFace);
double GetMinArea(CArray3d<CFaceTriangles>* pArrayFace);

// Intersections
int IntersectionLineFace(CVertex3D* pV0, CVertex3D* pV1, CFaceTriangles* pFace, CVertex3D* pVertexResult);
int IntersectionLineFaceRecursive(CVertex3D* pV0, CVertex3D* pV1, CFaceTriangles* pFaceStart, CVertex3D* pVertexResult, CFaceTriangles** ppFaceResult, CArray3d<CFaceTriangles>* pArrayFace, int MaxNbFace);
int NearestIntersectionWithLine(CVertex3D* pV0, CVertex3D* pV1, CFaceTriangles* pFaceStart, CVertex3D* pVertexResult, CFaceTriangles** ppFaceResult, int MaxNbFace);
int NearestIntersectionWithLine(CArray3d<CFaceTriangles>* pArrayFace, CVertex3D* pV0, CVertex3D* pV1, CVertex3D* pVertexResult, CFaceTriangles** ppFaceResult, int* pNbVisitedFace);
int NearestIntersectionWithLineFromFaceAndNeighbors(CFaceTriangles* pFace, CVertex3D* pV0, CVertex3D* pV1, CVertex3D* pVertexResult, CFaceTriangles** ppFaceResult);


int GravityCenter(CArray3d<CVertex3D>* pArray, CVertex3D* pVertexCenter);

// 法向量计算
CVector3D CalculateNormal(const CVector3D& v1, const CVector3D& v2);

#endif // _UTILS_3D_

