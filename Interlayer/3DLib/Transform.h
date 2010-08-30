#pragma once
#include "Vector3d.h"

class CTransform
{
private :

	// Datas
	CVector3D m_Scale;				
	CVector3D m_Rotation;
	CVector3D m_Translation;
	double m_ValueRotation;

public :

	// Constructor
	CTransform(); 
	~CTransform()
	{
	}

	// Data access
	void Clear(void);
	CVector3D GetScale()
	{
		return m_Scale;
	}
	CVector3D GetRotation()
	{
		return m_Rotation;
	}
	CVector3D GetTranslation()
	{
		return m_Translation;
	}
	float GetValueRotation()
	{
		return m_ValueRotation;
	}

	// Data setting
	void SetScale(CVector3D& v)
	{
		m_Scale = v;
	}
	void SetRotation(CVector3D& v)
	{
		m_Rotation = v;
	}
	void SetTranslation(CVector3D& v)
	{
		m_Translation = v;
	}
	void SetValueRotation(double val)
	{
		m_ValueRotation = val;
	}	

	void Copy(CTransform& transform);
	void Copy(CTransform* pTransform);
};
