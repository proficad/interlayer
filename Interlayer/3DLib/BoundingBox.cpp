// BoundingBox.cpp: implementation of the CBoundingBox class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BoundingBox.h"
#include "Vertex3D.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBoundingBox::CBoundingBox()
{
}

CBoundingBox::CBoundingBox(const double& xMin, const double& xMax, const double& yMin, const double& yMax, const double& zMin, const double& zMax) : itsXMax(xMax), itsXMin(xMin), itsYMax(yMax), itsYMin(yMin), itsZMax(zMax), itsZMin(zMin)
{
}

CBoundingBox::~CBoundingBox()
{
}

CVertex3D CBoundingBox::Center() const
{
	CVertex3D P;
	P.SetX((XMax() + XMin()) / 2);
	P.SetY((YMax() + YMin()) / 2);
	P.SetZ((ZMax() + ZMin()) / 2);
	return P;
}

double CBoundingBox::BoundingRadius() const
{
	double rad;
	CVertex3D Pmax(XMax(), YMax(), ZMax());
	CVertex3D Cen = Center();
	rad = Pmax.Distance(Cen);
	return rad;
}

void CBoundingBox::SetLimits(const double& xMin, const double& xMax, const double& yMin, const double& yMax, const double& zMin, const double& zMax)
{
	itsXMax = xMax;
	itsXMin = xMin;
	itsYMax = yMax;
	itsYMin = yMin;
	itsZMax = zMax;
	itsZMin = zMin;
}

void CBoundingBox::Serialize(CArchive& ar)
{
	if( ar.IsStoring())
	{
		ar << itsXMax;
		ar << itsXMin;
		ar << itsYMax;
		ar << itsYMin;
		ar << itsZMax;
		ar << itsZMin;
	}
	else
	{
		ar >> itsXMax;
		ar >> itsXMin;
		ar >> itsYMax;
		ar >> itsYMin;
		ar >> itsZMax;
		ar >> itsZMin;
	}
}

bool CBoundingBox::Contains(const CVertex3D& P) const
{
	bool bx = false, by = false, bz = false;
	bx = (P.GetX() <= XMax() && P.GetX() >= XMin());
	by = (P.GetY() <= YMax() && P.GetY() >= YMin());
	bz = (P.GetZ() <= ZMax() && P.GetZ() >= ZMin());

	return (bx && by && bz);
}

void CBoundingBox::AddBox(const CBoundingBox& B)
{
	if (B.XMax() > XMax())
		itsXMax = B.XMax();
	if (B.XMin() < XMin())
		itsXMin = B.XMin();

	if (B.YMax() > YMax())
		itsYMax = B.YMax();
	if (B.YMin() < YMin())
		itsYMin = B.YMin();

	if (B.ZMax() > ZMax())
		itsZMax = B.ZMax();
	if (B.ZMin() < ZMin())
		itsZMin = B.ZMin();
}
