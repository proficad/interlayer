// Geometry.h: interface for the CGeometry class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GEOMETRY_H__8716A236_EA76_458F_8315_FEA9C6A0DFD2__INCLUDED_)
#define AFX_GEOMETRY_H__8716A236_EA76_458F_8315_FEA9C6A0DFD2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <iostream>
#include "Material.h"
#include "GLTypeEnums.h"
#include "GLMaterials.h"

using namespace std;

enum GeometryType
{
	GEO_VERTEX,				// ����
	GEO_VECTOR,				// ʸ��
	GEO_CURVE,				// ����
	GEO_EDGE,				// ��
	GEO_PLANE,				// ��
	
	GEO_ONEAXIS,			// ��
	GEO_AXISSYSTEM,			// ����ϵ
	
	GEO_TRIANGLE,			// ������
	GEO_QUADS,				// �ı���
	GEO_POLYGON,			// �����

	GEO_CUBE,				// ������
	GEO_SURFACE,			// ����
};

class CVertex3D;
class CVector3D;
class COneAxis;
class CAxisSystem;
class CLine3D;
class CCircle3D;
class CEllipse3D;
class CHyperbola3D;
class CParabola3D;
class CBezierCurve;
class CBSplineCurve;
class CPlane;
class CCone;
class CCylinder;
class CSphere;
class CTorus;
class CExtrudedSurface;
class CRevolvedSurface;
class CRuledSurface;
class CPipeSurface;

class CGeometry
{
public:
	CGeometry();
	virtual ~CGeometry();
	//////////////////////////////////////////////////////////////////////////
	// ����
	CMaterial GetMaterial() const;

	void SetMaterial(const CMaterial&);
	void SetMaterial(const CMaterial*);

	void ApplyMaterial(const GLMaterial&);
	void ApplyMaterial(const CMaterial*);
	void ApplyMaterial(const CMaterial&);

	void AddMaterial(float*);
	void AddMaterial(const GLMaterial&);

	//////////////////////////////////////////////////////////////////////////
	virtual void Translate(const COneAxis&, const double& amt);
	virtual void Translate(double dx, double dy, double dz) = 0;
	virtual void Translate(const CVector3D&) = 0;
	virtual void Translate(const CVertex3D&, const CVertex3D&) = 0;
	virtual void Rotate(const COneAxis&, double) = 0;
	virtual void Scale(const CVertex3D&, double) = 0;
	virtual void Mirror(const CVertex3D&) = 0;
	virtual void Mirror(const COneAxis&) = 0;
	virtual void Mirror(const CPlane&) = 0;
	
	GeometryType GeomType() const
	{
		return m_geomType;
	}

private:
	static int m_refCount;				// ����
protected:
	GeometryType m_geomType;			// ���
	CMaterial m_itsMaterial;			// ����
};

#endif // !defined(AFX_GEOMETRY_H__8716A236_EA76_458F_8315_FEA9C6A0DFD2__INCLUDED_)
