//############################################################################
//
// OneAxis.h: interface for the COneAxis class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ONEAXIS_H__78EA474B_0A0E_4FAC_9C7F_F2B760C34538__INCLUDED_)
#define AFX_ONEAXIS_H__78EA474B_0A0E_4FAC_9C7F_F2B760C34538__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Geometry.h"
#include "Vector3d.h"
#include "Vertex3D.h"

class COneAxis : public CGeometry	// 坐标轴
{
public:
	COneAxis();
	COneAxis(const CVertex3D& Pos, const CVector3D& Dir);
	~COneAxis();
	void SetPosition(const CVertex3D& P);
	void SetPosition(double x, double y, double z);
	void SetDirection(const CVector3D& D);
	void SetDirection(double dx, double dy, double dz);
	CVertex3D GetPosition() const;
	CVector3D GetOrigin() const;
	CVector3D GetDirection() const;
	bool operator ==(const COneAxis&) const;
	virtual void Translate(const COneAxis&, const double&);
	virtual void Translate(double dx, double dy, double dz);
	virtual void Translate(const CVector3D&);
	virtual void Translate(const CVertex3D&, const CVertex3D&);
	virtual void Rotate(const COneAxis&, double);
	virtual void Scale(const CVertex3D&, double);
	virtual void Mirror(const CVertex3D&);
	virtual void Mirror(const COneAxis&);
	virtual void Mirror(const CPlane&);
	static COneAxis OX();
	static COneAxis OY();
	static COneAxis OZ();
private:
	CVertex3D m_Position;		// 位置
	CVector3D m_Origin;			// 原点
	CVector3D m_Direction;		// 方向
};

#endif // !defined(AFX_ONEAXIS_H__78EA474B_0A0E_4FAC_9C7F_F2B760C34538__INCLUDED_)
