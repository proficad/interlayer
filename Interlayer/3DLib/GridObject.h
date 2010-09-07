#pragma once
#include "globject.h"
#include "Vector3d.h"
#include "Array3d.h"
#include "Material.h"
#include "Texture.h"
#include "Vertex3D.h"
#include <vector>

class CPhyPara			// 物性参数 
{
public:
	CString m_strName;
	bool m_bShow;
	std::vector<double>	m_dValue;
	double m_dMinValue, m_dMaxValue;
public:
	CPhyPara()
	{
		m_bShow = false;
		m_dMinValue = DBL_MAX;
		m_dMaxValue = -DBL_MAX;
	};

	~CPhyPara(){};

	const CPhyPara& operator=(const CPhyPara& src);

	friend CArchive& AFXAPI operator << (CArchive& ar, const CPhyPara& param)
	{
		ar << param.m_strName;
		ar << param.m_bShow;	
		ar << param.m_dMinValue;
		ar << param.m_dMaxValue;
		int nSize = param.m_dValue.size();
		ar << nSize;
		for (int i=0; i<nSize; i++)
		{
			ar << param.m_dValue[i];
		}
		return ar;
	}
	friend CArchive& AFXAPI operator >> (CArchive& ar, CPhyPara& param)
	{
		ar >> param.m_strName;
		ar >> param.m_bShow;	
		ar >> param.m_dMinValue;
		ar >> param.m_dMaxValue;
		int nSize;
		ar >> nSize;
		if( param.m_dValue.capacity()<nSize)
			param.m_dValue.reserve(nSize);

		for (int i=0; i<nSize; i++)
		{
			double tmp;
			ar >> tmp;
			param.m_dValue.push_back(tmp);
		}
		return ar;
	}

	void Add(double dValue)
	{	
		m_dValue.push_back(dValue);

		if( dValue > m_dMaxValue )
			m_dMaxValue = dValue;
		if( dValue < m_dMinValue )
			m_dMinValue = dValue;
	}
};

CArchive& AFXAPI operator << (CArchive& ar, const CPhyPara& param);
CArchive& AFXAPI operator >> (CArchive& ar, CPhyPara& param);

class CGridObject :	public CGLObject
{
public:
	CGridObject(void);
	~CGridObject(void);

	virtual CGLObject* Copy();
	virtual void DefineDisplay();
	virtual void Display(const GLDisplayMode& = GLWIREFRAME, bool bForce = false);
	virtual void DisplaySelected(const GLSelectedMode& = GLSELECTED);
	virtual void Hilight(const GLSelectedMode&);
	virtual bool BuildList();
	void ComputeBoundLimits();

	//std::vector<CVertex3D>* GetPointList() { return m_pointList; }
	CArray<CPhyPara, CPhyPara&>	m_vecPhyPara;
	//CArray<CVertex3D,CVertex3D&> m_pointList;

	std::vector<CPoint3D> m_pointList;

	//CList<CVertex3D,CVertex3D&> m_pointList;

	// 保存
	virtual void Serialize(CArchive& ar);

	int I, J, K;

	CArray<BOOL, BOOL> m_bShowI;
	CArray<BOOL, BOOL> m_bShowJ;
	CArray<BOOL, BOOL> m_bShowK;
	
	void Add(LPCTSTR lpszName, double dValue);
private:
	void DrawWired();
	void DrawShaded();
	void ComputePoints();	
};
