#pragma once
#include "globject.h"
#include "Vector3d.h"
#include "Array3d.h"
#include "Material.h"
#include "Texture.h"
#include "Vertex3D.h"
#include "../GridModelCell.h"

class CPhyPara			// 物性参数 
{
public:
	CString m_strName;
	int m_bShow;
	std::vector<double>	m_dValue;
	double m_dMinValue, m_dMaxValue;
	int		I,J,K;
public:
	CPhyPara()
	{
		m_bShow = 0;
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
		ar << param.I;
		ar << param.J;
		ar << param.K;
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
		ar >> param.I;
		ar >> param.J;
		ar >> param.K;
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
	void SavePara(const std::string& filename);
	void LoadPara(const std::string& filename);
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

	void ComputePoints( const CBoundingBox& box );	
	//void ComputrColors( const CBoundingBox& box );

	//std::vector<CVertex3D>* GetPointList() { return m_pointList; }
	CArray<CPhyPara, CPhyPara&>	m_vecPhyPara;
	//CArray<CVertex3D,CVertex3D&> m_pointList;

	//std::vector<CVertex3D> m_pointList;

	VECTOR_ARRAY3DPTR		 m_gridCells;
	//std::vector<GridRefinement> m_gridRedine;

	int								m_layerIndex;
	//CList<CVertex3D,CVertex3D&> m_pointList;

	// 保存
	virtual void Serialize(CArchive& ar);

	int I, J, K;

	CArray<BOOL, BOOL> m_bShowI;
	CArray<BOOL, BOOL> m_bShowJ;
	CArray<BOOL, BOOL> m_bShowK;
	CArray<BOOL, BOOL> m_bChangeK;
	
	void Add(LPCTSTR lpszName, double dValue);
private:
	void DrawWired();
	void DrawShaded();
private:
	bool	m_bColorComputed;
};


class InterLayerGridObject : public CGLObject
{
public:
	InterLayerGridObject(void);
	~InterLayerGridObject(void);

	virtual CGLObject* Copy();
	virtual void DefineDisplay();
	virtual void Display(const GLDisplayMode& = GLWIREFRAME, bool bForce = false);
	virtual void DisplaySelected(const GLSelectedMode& = GLSELECTED);
	virtual void Hilight(const GLSelectedMode&);
	virtual bool BuildList(); 
	void ComputeBoundLimits();

	void ComputePoints( const CBoundingBox& box );

	CArray<CPhyPara, CPhyPara&>	m_vecPhyPara;
	//CArray<CVertex3D,CVertex3D&> m_pointList;

	CString							m_strModelGUID;

	VECTOR_ARRAY			m_gridCells;
	vector<std::string>		m_layerNames;
	int								m_layerIndex;
	HTREEITEM					m_modelItem;
	// 保存
	virtual void Serialize(CArchive& ar);
	void	LoadLayer(const std::string& filename);
	void AddGridGUID(const std::string& filename,CString guid);

	void Add(LPCTSTR lpszName, double dValue);

	int I, J, K;

	//void Add(LPCTSTR lpszName, double dValue);
private:
	void DrawWired();
	void DrawShaded();

	bool	m_bColorComputed;
	COLORREF		m_singleColor;
};