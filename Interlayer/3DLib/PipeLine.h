#pragma once
#include "globject.h"
#include "Vector3d.h"
#include "Array3d.h"
#include "Material.h"
#include "Texture.h"
#include "Vertex3D.h"
#include <vector>

class PIPELINE							//
{
public:
	CString m_strTitle;							// 管子名
	CArray<CVertex3D,CVertex3D> m_arrPoint;		//管点数组

	PIPELINE()
	{
		m_strTitle = _T("井");
	}

	PIPELINE& operator=(const PIPELINE &s)
	{
		m_strTitle = s.m_strTitle;
		for(int i = 0; i < s.m_arrPoint.GetSize(); i++)
			m_arrPoint.Add(s.m_arrPoint[i]);
		return *this;
	}

	PIPELINE(const PIPELINE &s)
	{
		m_strTitle = s.m_strTitle;
		for(int i = 0; i < s.m_arrPoint.GetSize(); i++)
			m_arrPoint.Add(s.m_arrPoint[i]);
	}

	friend CArchive& AFXAPI operator << (CArchive& ar, const PIPELINE& line)
	{
		ar << line.m_strTitle;
		int nSize = line.m_arrPoint.GetSize();
		ar << nSize;
		for(int i = 0; i < nSize; i++)
			ar << line.m_arrPoint[i];
		
		return ar;
	}
	friend CArchive& AFXAPI operator >> (CArchive& ar, PIPELINE& line)
	{
		ar >> line.m_strTitle;
		int nSize;
		ar >> nSize;
		for(int i = 0; i < nSize; i++)
		{
			CVertex3D vert;
			ar >> vert;
			line.m_arrPoint.Add(vert);
		}
		return ar;
	}

	~PIPELINE()
	{
		m_arrPoint.RemoveAll();
	}
};

class CPipeLine : public CGLObject
{
public:
	CPipeLine(void);
	~CPipeLine(void);
	virtual CGLObject* Copy();
	virtual void DefineDisplay();
	virtual void Display(const GLDisplayMode& = GLWIREFRAME, bool bForce = false);
	virtual void DisplaySelected(const GLSelectedMode& = GLSELECTED);
	virtual void Hilight(const GLSelectedMode&);
	virtual bool BuildList();
	void ComputeBoundLimits();

	// 数据
	double m_dRadius;											// 半径
	PIPELINE m_Pipeline;										// 管子
	// 保存
	virtual void Serialize(CArchive& ar);
	
private:
	void DrawWired();
	void DrawShaded();

	void DrawPipe();
	void DrawTitle();
};
