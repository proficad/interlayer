//////////////////////////////////////////////////////////////////////////

#ifndef _GRID_MODEL_CELL_
#define _GRID_MODEL_CELL_

#include "3DLib/Vertex3D.h"
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

struct GridRefinement
{
	GridRefinement()
	{
		m_X1 = m_X2 = m_Y1 = m_Y2 = m_Z1 = m_Z2 = m_DX = m_DY = m_DZ = m_NWMAX = 0;
		memset(m_Name,0,16);
		memset(m_GLOBAL,0,16);
	}
	GridRefinement(const GridRefinement& varSrc)
	{
		for (int i = 0; i < 16; i++)
		{
			m_Name[i] = varSrc.m_Name[i];
			m_GLOBAL[i] = varSrc.m_GLOBAL[i];
		}
		m_X1 = varSrc.m_X1;
		m_X2 = varSrc.m_X2;
		m_Y1 = varSrc.m_Y1;
		m_Y2= varSrc.m_Y2;
		m_Z1= varSrc.m_Z1;
		m_Z2= varSrc.m_Z2;
		m_DX= varSrc.m_DX;
		m_DY = varSrc.m_DY;
		m_DZ= varSrc.m_DZ;
		m_NWMAX= varSrc.m_NWMAX;
	}
	char m_Name[16];
	int m_X1;
	int m_X2;
	int m_Y1;
	int m_Y2;
	int m_Z1;
	int m_Z2;
	int m_DX;
	int m_DY;
	int m_DZ;
	int m_NWMAX;
	char m_GLOBAL[16];

	void Serialize(CArchive& ar)
	{
		if (ar.IsStoring())
		{
			for (int i = 0; i < 16; i++)
				ar << m_Name[i];
			ar <<  m_X1;
			ar <<  m_X2;
			ar <<  m_Y1;
			ar <<  m_Y2;
			ar <<  m_Z1;
			ar <<  m_Z2;
			ar <<  m_DX;
			ar <<  m_DY;
			ar <<  m_DZ;
			ar <<  m_NWMAX;
			for (int i = 0; i < 16; i++)
				ar << m_GLOBAL[i];
		}
		else
		{
			for (int i = 0; i < 16; i++)
				ar >> m_Name[i];
			ar >>  m_X1;
			ar >>  m_X2;
			ar >>  m_Y1;
			ar >>  m_Y2;
			ar >>  m_Z1;
			ar >>  m_Z2;
			ar >>  m_DX;
			ar >>  m_DY;
			ar >>  m_DZ;
			ar >>  m_NWMAX;
			for (int i = 0; i < 16; i++)
				ar >> m_GLOBAL[i];
		}
	}
};



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
		for(int i=0; i<8; i++)
		{
			m_cornerPoint[i] = varSrc.m_cornerPoint[i];
			m_itsColor[i] = varSrc.m_itsColor[i];
		}

		for(int i=0; i<6; i++)
			m_faceNormals[i] = varSrc.m_faceNormals[i];

		m_bIsGridRefinement = varSrc.m_bIsGridRefinement;
		_x = varSrc._x;
		_y = varSrc._y;
		_z = varSrc._z;
		if(m_bIsGridRefinement)
		{
			for(int i=0; i<varSrc.m_subCells.size(); i++)
			{
				////tagGridModelCellNew cell(*it);
				m_subCells.push_back(varSrc.m_subCells[i]);
			}
		}
	}
	~tagGridModelCellNew()
	{
		//delete [] m_cornerPoint;
		//delete [] m_faceNormals;
		//delete [] m_itsColor;
		//m_subCells.clear();
	}
	void Serialize(CArchive& ar)
	{	
		for (int i = 0; i < 8; i++)
			m_cornerPoint[i].Serialize(ar);
		for (int i = 0; i <6; i++)
			m_faceNormals[i].Serialize(ar);
		for(vector<tagGridModelCellNew>::iterator it=m_subCells.begin(); it!=m_subCells.end(); ++it)
		{
			tagGridModelCellNew cell = (*it);
			cell.Serialize(ar);
		}
		if (ar.IsStoring())
		{
			ar << m_bIsGridRefinement;
			ar << _x;
			ar << _y;
			ar << _z;
			for(int i=0; i<8; i++)
			{
				ar << m_itsColor[i];
			}
		}
		else
		{
			ar >> m_bIsGridRefinement;
			ar >> _x;
			ar >> _y;
			ar >> _z;
			for(int i=0; i<8; i++)
			{
				ar >>m_itsColor[i];
			}
		}
	}
	void CalcNormals();
	CVector3DF m_cornerPoint[8];
	CVector3DF m_faceNormals[6];
	//网格加密信息
	COLORREF m_itsColor[8];
	int	_x,_y,_z;
	vector<tagGridModelCellNew>	m_subCells;
	bool	m_bIsGridRefinement;
};

typedef vector< tagGridModelCellNew > VECTOR_ARRAY;
typedef VECTOR_ARRAY::iterator VECTOR_ARRAY_ITERATOR;
typedef vector< VECTOR_ARRAY > VECTOR_ARRAY2D;
typedef VECTOR_ARRAY2D::iterator VECTOR_ARRAY2D_ITERATOR;
typedef vector<VECTOR_ARRAY2D > VECTOR_ARRAY3D;
typedef VECTOR_ARRAY3D::iterator VECTOR_ARRAY3D_ITERATOR;

#endif