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
		// ����ϵͳ nx * ny * nz
		BOOL m_bCornerPointGrid;			// �Ƿ��ǵ�����
		int m_nGridX;						// x����������
		int m_nGridY;						// y����������
		int m_nGridZ;						// z����������

		double m_dMinX;						// ������С���ֵ
		double m_dMinY;						// 
		double m_dMinZ;						// 
		double m_dMaxX;
		double m_dMaxY;
		double m_dMaxZ;

		// ��������
		vector<double> m_gridDX;		// m_nGridX*m_nGridY*m_nGridZ
		vector<double> m_gridDY;		// m_nGridX*m_nGridY*m_nGridZ
		vector<double> m_gridDZ;		// m_nGridX*m_nGridY*m_nGridZ �����ĺ��
		vector<double> m_gridTops;		// m_nGridX*m_nGridY �����������

		// �ǵ�����
		vector<CVertex3D> m_coordGrid;	// ������ (m_nGridX+1)*(m_nGridY+1)
		vector<double> m_zcornGrid;		// �ǵ���� m_nGridX*m_nGridY*m_gridDZ*8
		vector<int>		  m_subMeshX;		// x��������ϸ���� m_nGridX*m_nGridY*m_gridD
		vector<int>		  m_subMeshY;		// y��������ϸ���� m_nGridX*m_nGridY*m_gridD
		vector<int>		  m_subMeshZ;		// y��������ϸ���� m_nGridX*m_nGridY*m_gridD

		VECTOR_ARRAY3D m_gridCells;		//��������Ԫ����Ϣ
	};
}
