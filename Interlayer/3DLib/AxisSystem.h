//############################################################################
//
// AxisSystem.h: interface for the CAxisSystem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AXISSYSTEM_H__9C6F4BC8_FF32_413B_AEB1_D38B5962F89E__INCLUDED_)
#define AFX_AXISSYSTEM_H__9C6F4BC8_FF32_413B_AEB1_D38B5962F89E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Geometry.h"
#include "Vector3d.h"
#include "Vertex3D.h"

class CAxisSystem : public CGeometry
{
public:
	CAxisSystem();
	CAxisSystem(const CVertex3D& Pos, const CVector3D& Dir, const CVector3D& XDir);
	CAxisSystem(const CVertex3D& Pos, const CVector3D& Dir);
	CAxisSystem(const CAxisSystem&);
	~CAxisSystem();
	void SetPosition(const CVertex3D& P);
	void SetPosition(double x, double y, double z);
	void SetDirection(const CVector3D& D);
	void SetDirection(double dx, double dy, double dz);
	void SetXDirection(const CVector3D& XD);
	void SetXDirection(double dxx, double dxy, double dxz);
	void SetYDirection(const CVector3D& YD);
	void SetYDirection(double dyx, double dyy, double dyz);
	CVertex3D GetPosition() const;
	CVector3D GetOrigin() const;
	CVector3D GetDirection() const;
	CVector3D GetXDirection() const;
	CVector3D GetYDirection() const;
	bool operator ==(const CAxisSystem& C) const;
	virtual void Translate(const COneAxis&, const double&);
	virtual void Translate(double dx, double dy, double dz);
	virtual void Translate(const CVector3D&);
	virtual void Translate(const CVertex3D& P1, const CVertex3D& P2);
	virtual void Rotate(const COneAxis&, double);
	virtual void Scale(const CVertex3D&, double);
	virtual void Mirror(const CVertex3D&);
	virtual void Mirror(const COneAxis&);
	virtual void Mirror(const CPlane&);
private:
	CVertex3D m_Position;		// 位置
	CVector3D m_Origin;			// 原点
	CVector3D m_Direction;		// 方向
	CVector3D m_XDirection;		// X方向
	CVector3D m_YDirection;		// Y方向
};

#endif // !defined(AFX_AXISSYSTEM_H__9C6F4BC8_FF32_413B_AEB1_D38B5962F89E__INCLUDED_)
