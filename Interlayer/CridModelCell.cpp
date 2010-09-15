#include "stdafx.h"
#include "GridModelCell.h"

tagGridModelCellNew::tagGridModelCellNew()
:tagGridModelCell(),m_bIsGridRefinement(false),_x(0),_y(0),_z(0)
{
}

void tagGridModelCellNew::CalcNormals()
{
	CVector3D nrml;

	nrml = CVector3D(m_cornerPoint[0], m_cornerPoint[1]).CrossProduct(CVector3D(m_cornerPoint[1], m_cornerPoint[2])); 
	nrml.Normalize();
	m_faceNormals[0] = nrml;
	nrml = CVector3D(m_cornerPoint[4], m_cornerPoint[7]).CrossProduct(CVector3D(m_cornerPoint[7], m_cornerPoint[6])); 
	nrml.Normalize();
	m_faceNormals[1] = nrml;
	nrml = CVector3D(m_cornerPoint[0], m_cornerPoint[4]).CrossProduct(CVector3D(m_cornerPoint[4], m_cornerPoint[5])); 
	nrml.Normalize();
	m_faceNormals[2] = nrml;

	nrml = CVector3D(m_cornerPoint[1], m_cornerPoint[5]).CrossProduct(CVector3D(m_cornerPoint[5], m_cornerPoint[6])); 
	nrml.Normalize();
	m_faceNormals[3] = nrml;
	nrml = CVector3D(m_cornerPoint[2], m_cornerPoint[6]).CrossProduct(CVector3D(m_cornerPoint[6], m_cornerPoint[7])); 
	nrml.Normalize();
	m_faceNormals[4] = nrml;
	nrml = CVector3D(m_cornerPoint[0], m_cornerPoint[3]).CrossProduct(CVector3D(m_cornerPoint[3], m_cornerPoint[7])); 
	nrml.Normalize();
	m_faceNormals[5] = nrml;
}
