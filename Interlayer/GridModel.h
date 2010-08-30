#pragma once

#include "afx.h"
#include <vector>
#include <math.h>
#include <float.h>
#include "./3Dlib/Vertex3D.h"

using namespace std;

namespace GridModel
{
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

	class CGridModel : public CObject
	{
		DECLARE_SERIAL(CGridModel);
	public:
		CGridModel(void);
		~CGridModel(void);
		virtual void Serialize(CArchive& ar);
		tagGridModelCell GetGridCell(int nX, int nY, int nZ);
		CVertex3D GetCornerPoint(int nX, int nY, int nZ, int nIndex);
		BOOL OpenFile(LPCTSTR lpFileName);
		BOOL SaveFile(LPCTSTR lpFileName);
		void InitGrid(int nX = 0, int nY = 0, int nZ = 0, BOOL bCorner = TRUE);
		BOOL SaveFile2Wrl(LPCTSTR lpFileName);
		void SaveTop(CStdioFile& file);
		void SaveBottom(CStdioFile& file);
		void SaveFront(CStdioFile& file);
		void SaveBack(CStdioFile& file);
		void SaveLeft(CStdioFile& file);
		void SaveRight(CStdioFile& file);
	public:
		// 网格系统 nx * ny * nz
		BOOL m_bCornerPointGrid;			// 是否库角点网格
		int m_nGridX;						// x方向网格数
		int m_nGridY;						// y方向网格数
		int m_nGridZ;						// z方向网格数

		double m_dMinX;						// 网格最小最大值
		double m_dMinY;						// 
		double m_dMinZ;						// 
		double m_dMaxX;
		double m_dMaxY;
		double m_dMaxZ;

		// 正交网格
		vector<double> m_gridDX;		// m_nGridX*m_nGridY*m_nGridZ
		vector<double> m_gridDY;		// m_nGridX*m_nGridY*m_nGridZ
		vector<double> m_gridDZ;		// m_nGridX*m_nGridY*m_nGridZ 块中心厚度
		vector<double> m_gridTops;		// m_nGridX*m_nGridY 顶面中心深度

		// 角点网格
		vector<CVertex3D> m_coordGrid;	// 坐标线 (m_nGridX+1)*(m_nGridY+1)
		vector<double> m_zcornGrid;		// 角点深度 m_nGridX*m_nGridY*m_gridDZ*8
	};
}
