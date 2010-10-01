//////////////////////////////////////////////////////////////////////////

#ifndef _GRID_MODEL_CELL_
#define _GRID_MODEL_CELL_

#include "3DLib/Vertex3D.h"
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

struct tagGridModelCell // 网格单元体
{
public:
	CVertex3D m_cornerPoint[8]; // 八个顶点数据
	tagGridModelCell()
	{
	};
	tagGridModelCell(const tagGridModelCell& varSrc)
	{
		for (int i = 0; i < 8; i++)
			m_cornerPoint[i] = varSrc.m_cornerPoint[i];
	}


	tagGridModelCell& operator=(const tagGridModelCell& varSrc)
	{
		for (int i = 0; i < 8; i++)
			m_cornerPoint[i] = varSrc.m_cornerPoint[i];
		return *this;
	}

	friend inline CArchive& operator <<(CArchive& ar, const tagGridModelCell& cell)
	{
		for (int i = 0; i < 8; i++)
			ar << cell.m_cornerPoint[i];

		return ar;
	}
	friend inline CArchive& operator >>(CArchive& ar, tagGridModelCell& cell)
	{
		for (int i = 0; i < 8; i++)
			ar >> cell.m_cornerPoint[i];

		return ar;
	}

	BOOL operator==(const tagGridModelCell& test)
	{
		CVertex3D pt;
		for (int i = 0; i < 8; i++)
		{
			pt = test.m_cornerPoint[i];
			if (pt != this->m_cornerPoint[i])
				return FALSE;
		}

		return TRUE;
	}

	BOOL operator!=(const tagGridModelCell& test)
	{
		CVertex3D pt;
		for (int i = 0; i < 8; i++)
		{
			pt = test.m_cornerPoint[i];
			if (pt != this->m_cornerPoint[i])
				return TRUE;
		}

		return FALSE;
	}

	void Serialize(CArchive& ar)
	{
		if (ar.IsStoring())
		{
			for (int i = 0; i < 8; i++)
				ar << m_cornerPoint[i];
		}
		else
		{
			for (int i = 0; i < 8; i++)
				ar >> m_cornerPoint[i];
		}
	}
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
struct tagGridModelCellNew
{
public:
	tagGridModelCellNew();
	tagGridModelCellNew(const tagGridModelCellNew& varSrc)
	{
		for (int i = 0; i < 8; i++)
			m_cornerPoint[i] = varSrc.m_cornerPoint[i];
	}
	void CalcNormals();
	CVector3DF m_cornerPoint[8];
	CVector3DF m_faceNormals[6];
	//网格加密信息
	bool	m_bIsGridRefinement;
	COLORREF m_itsColor[8];
	int	_x,_y,_z;
	vector<tagGridModelCellNew>	m_subCells;
};

typedef vector< tagGridModelCellNew > VECTOR_ARRAY;
typedef VECTOR_ARRAY::iterator VECTOR_ARRAY_ITERATOR;
typedef vector< VECTOR_ARRAY > VECTOR_ARRAY2D;
typedef VECTOR_ARRAY2D::iterator VECTOR_ARRAY2D_ITERATOR;
typedef vector<VECTOR_ARRAY2D > VECTOR_ARRAY3D;
typedef VECTOR_ARRAY3D::iterator VECTOR_ARRAY3D_ITERATOR;

#endif