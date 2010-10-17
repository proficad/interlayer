#include "stdafx.h"
#include "GridModelCell.h"

tagGridModelCellNew::tagGridModelCellNew()
:m_bIsGridRefinement(false),_x(0),_y(0),_z(0)
{
	//m_cornerPoint = new CVector3DF[8];
	//m_faceNormals = new CVector3DF[6];
	////网格加密信息
	//m_itsColor =new COLORREF[8];
}

void tagGridModelCellNew::CalcNormals()
{
	CVector3DF nrml;

	nrml = CVector3DF(m_cornerPoint[0], m_cornerPoint[1]).CrossProduct(CVector3DF(m_cornerPoint[1], m_cornerPoint[2])); 
	nrml.Normalize();
	m_faceNormals[0] = nrml;
	nrml = CVector3DF(m_cornerPoint[4], m_cornerPoint[7]).CrossProduct(CVector3DF(m_cornerPoint[7], m_cornerPoint[6])); 
	nrml.Normalize();
	m_faceNormals[1] = nrml;
	nrml = CVector3DF(m_cornerPoint[0], m_cornerPoint[4]).CrossProduct(CVector3DF(m_cornerPoint[4], m_cornerPoint[5])); 
	nrml.Normalize();
	m_faceNormals[2] = nrml;

	nrml = CVector3DF(m_cornerPoint[1], m_cornerPoint[5]).CrossProduct(CVector3DF(m_cornerPoint[5], m_cornerPoint[6])); 
	nrml.Normalize();
	m_faceNormals[3] = nrml;
	nrml = CVector3DF(m_cornerPoint[2], m_cornerPoint[6]).CrossProduct(CVector3DF(m_cornerPoint[6], m_cornerPoint[7])); 
	nrml.Normalize();
	m_faceNormals[4] = nrml;
	nrml = CVector3DF(m_cornerPoint[0], m_cornerPoint[3]).CrossProduct(CVector3DF(m_cornerPoint[3], m_cornerPoint[7])); 
	nrml.Normalize();
	m_faceNormals[5] = nrml;
}

GridCells::~GridCells()
{
	for(VECTOR_ARRAY3D_ITERATOR it3d=m_gridCells.begin(); it3d!=m_gridCells.end(); ++it3d)
	{
		for(VECTOR_ARRAY2D_ITERATOR it2d=(*it3d).begin(); it2d!=(*it3d).end(); ++it2d)
		{
			(*it2d).clear();
		}
		(*it3d).clear();
	}
	m_gridCells.clear();
}