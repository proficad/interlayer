#pragma once

#include "GLTypeEnums.h"

class CMaterial // 材质
{
private :
	float m_pAmbient[4];		// 环境光颜色(0.2, 0.2, 0.2, 1.0)
	float m_pDiffuse[4];		// 漫反射光颜色(0.8, 0.8, 0.8, 1.0)
	float m_pSpecular[4];		// 镜面反射光颜色(0.8, 0.8, 0.8, 1.0)
	float m_pShininess[1];		// 镜面指数（光亮度）
	float m_pEmission[4];		// 辐射光颜色
public :
	// Constructor
	CMaterial();
	CMaterial(CMaterial* pMaterial);
	~CMaterial()
	{
	}

	// Data access
	float* GetAmbient()
	{
		return m_pAmbient;
	}
	float* GetDiffuse()
	{
		return m_pDiffuse;
	}
	float* GetSpecular()
	{
		return m_pSpecular;
	}
	float* GetShininess()
	{
		return m_pShininess;
	}
	float* GetEmission()
	{
		return m_pEmission;
	}

	// Data access
	float GetAmbient(int idx) const
	{
		return m_pAmbient[idx];
	}
	float GetDiffuse(int idx) const
	{
		return m_pDiffuse[idx];
	}
	float GetSpecular(int idx) const
	{
		return m_pSpecular[idx];
	}
	float GetShininess(int idx) const
	{
		return m_pShininess[idx];
	}
	float GetEmission(int idx) const
	{
		return m_pEmission[idx];
	}

	// Data setting
	void SetAmbient(float r, float g, float b, float a);
	void SetDiffuse(float r, float g, float b, float a);
	void SetSpecular(float r, float g, float b, float a);
	void SetShininess(float value);
	void SetEmission(float r, float g, float b, float a);

	//////////////////////////////////////////////////////////////////////////
	void SetMaterial(float* matArray);
	void SetMaterial(const GLMaterial&mat);
	void SetMaterial(CMaterial* pMat);
	void SetMaterial(CMaterial& mat);
	void GetMaterial(float* matArray);
	//////////////////////////////////////////////////////////////////////////
	void ApplyMaterial();

	// Datas
	void Copy(CMaterial* pMaterial);
	CMaterial operator=(const CMaterial& Material);

	friend CArchive& AFXAPI operator<<(CArchive& ar, const CMaterial& mat)
	{
		ar << mat.m_pAmbient[0];
		ar << mat.m_pAmbient[1];
		ar << mat.m_pAmbient[2];
		ar << mat.m_pAmbient[3];

		ar << mat.m_pDiffuse[0];
		ar << mat.m_pDiffuse[1];
		ar << mat.m_pDiffuse[2];
		ar << mat.m_pDiffuse[3];

		ar << mat.m_pSpecular[0];
		ar << mat.m_pSpecular[1];
		ar << mat.m_pSpecular[2];
		ar << mat.m_pSpecular[3];

		ar << mat.m_pShininess[0];

		ar << mat.m_pEmission[0];
		ar << mat.m_pEmission[1];
		ar << mat.m_pEmission[2];
		ar << mat.m_pEmission[3];

		return ar;
	}

	friend CArchive& AFXAPI operator>>(CArchive& ar, CMaterial& mat)
	{
		ar >> mat.m_pAmbient[0];
		ar >> mat.m_pAmbient[1];
		ar >> mat.m_pAmbient[2];
		ar >> mat.m_pAmbient[3];

		ar >> mat.m_pDiffuse[0];
		ar >> mat.m_pDiffuse[1];
		ar >> mat.m_pDiffuse[2];
		ar >> mat.m_pDiffuse[3];

		ar >> mat.m_pSpecular[0];
		ar >> mat.m_pSpecular[1];
		ar >> mat.m_pSpecular[2];
		ar >> mat.m_pSpecular[3];

		ar >> mat.m_pShininess[0];

		ar >> mat.m_pEmission[0];
		ar >> mat.m_pEmission[1];
		ar >> mat.m_pEmission[2];
		ar >> mat.m_pEmission[3];
		return ar;
	}

	friend BOOL AFXAPI operator==(const CMaterial& s1, const CMaterial& s2)
	{
		return (fabs(s1.m_pAmbient[0] - s2.m_pAmbient[0])<=DBL_EPSILON
			&& fabs(s1.m_pAmbient[1] - s2.m_pAmbient[1])<=DBL_EPSILON
			&& fabs(s1.m_pAmbient[2] - s2.m_pAmbient[2])<=DBL_EPSILON
			&& fabs(s1.m_pAmbient[3] - s2.m_pAmbient[3])<=DBL_EPSILON
			&& fabs(s1.m_pDiffuse[0] - s2.m_pDiffuse[0])<=DBL_EPSILON
			&& fabs(s1.m_pDiffuse[1] - s2.m_pDiffuse[1])<=DBL_EPSILON
			&& fabs(s1.m_pDiffuse[2] - s2.m_pDiffuse[2])<=DBL_EPSILON
			&& fabs(s1.m_pDiffuse[3] - s2.m_pDiffuse[3])<=DBL_EPSILON
			&& fabs(s1.m_pSpecular[0] - s2.m_pSpecular[0])<=DBL_EPSILON
			&& fabs(s1.m_pSpecular[1] - s2.m_pSpecular[1])<=DBL_EPSILON
			&& fabs(s1.m_pSpecular[2] - s2.m_pSpecular[2])<=DBL_EPSILON
			&& fabs(s1.m_pSpecular[3] - s2.m_pSpecular[3])<=DBL_EPSILON
			&& fabs(s1.m_pEmission[0] - s2.m_pEmission[0])<=DBL_EPSILON
			&& fabs(s1.m_pEmission[1] - s2.m_pEmission[1])<=DBL_EPSILON
			&& fabs(s1.m_pEmission[2] - s2.m_pEmission[2])<=DBL_EPSILON
			&& fabs(s1.m_pEmission[3] - s2.m_pEmission[3])<=DBL_EPSILON
			&& fabs(s1.m_pShininess[0] - s2.m_pShininess[0])<=DBL_EPSILON); 
	}
	friend BOOL AFXAPI operator!=(const CMaterial& s1, const CMaterial& s2)
	{
		return (fabs(s1.m_pAmbient[0] - s2.m_pAmbient[0])>DBL_EPSILON
			|| fabs(s1.m_pAmbient[1] - s2.m_pAmbient[1])>DBL_EPSILON
			|| fabs(s1.m_pAmbient[2] - s2.m_pAmbient[2])>DBL_EPSILON
			|| fabs(s1.m_pAmbient[3] - s2.m_pAmbient[3])>DBL_EPSILON
			|| fabs(s1.m_pDiffuse[0] - s2.m_pDiffuse[0])>DBL_EPSILON
			|| fabs(s1.m_pDiffuse[1] - s2.m_pDiffuse[1])>DBL_EPSILON
			|| fabs(s1.m_pDiffuse[2] - s2.m_pDiffuse[2])>DBL_EPSILON
			|| fabs(s1.m_pDiffuse[3] - s2.m_pDiffuse[3])>DBL_EPSILON
			|| fabs(s1.m_pSpecular[0] - s2.m_pSpecular[0])>DBL_EPSILON
			|| fabs(s1.m_pSpecular[1] - s2.m_pSpecular[1])>DBL_EPSILON
			|| fabs(s1.m_pSpecular[2] - s2.m_pSpecular[2])>DBL_EPSILON
			|| fabs(s1.m_pSpecular[3] - s2.m_pSpecular[3])>DBL_EPSILON
			|| fabs(s1.m_pEmission[0] - s2.m_pEmission[0])>DBL_EPSILON
			|| fabs(s1.m_pEmission[1] - s2.m_pEmission[1])>DBL_EPSILON
			|| fabs(s1.m_pEmission[2] - s2.m_pEmission[2])>DBL_EPSILON
			|| fabs(s1.m_pEmission[3] - s2.m_pEmission[3])>DBL_EPSILON
			|| fabs(s1.m_pShininess[0] - s2.m_pShininess[0])>DBL_EPSILON); 
	}
};

CArchive& AFXAPI operator << (CArchive& ar, const CMaterial& mat);
CArchive& AFXAPI operator >> (CArchive& ar, CMaterial& mat);

BOOL AFXAPI operator==(const CMaterial& s1, const CMaterial& s2);
BOOL AFXAPI operator!=(const CMaterial& s1, const CMaterial& s2);