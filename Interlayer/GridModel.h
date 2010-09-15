#pragma once

#include "afx.h"
#include <vector>
#include <math.h>
#include <float.h>
#include "./3Dlib/Vertex3D.h"
#include "GridModelCell.h"

using namespace std;

namespace GridModel
{
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

		void ReleaseGrid();
		void FillGridCells();
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
		vector<int>		  m_subMeshX;		// x方向网格细分数 m_nGridX*m_nGridY*m_gridD
		vector<int>		  m_subMeshY;		// y方向网格细分数 m_nGridX*m_nGridY*m_gridD
		vector<int>		  m_subMeshZ;		// y方向网格细分数 m_nGridX*m_nGridY*m_gridD

		VECTOR_ARRAY3D m_gridCells;		//保存网格单元格信息
	};
}
