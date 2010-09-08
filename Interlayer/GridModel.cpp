// GridModel.cpp : 实现文件
//

#include "stdafx.h"
#include "Interlayer.h"
#include "GridModel.h"

#include "./3DLib/FaceTriangles.h"

using namespace GridModel;

extern CString GetFileErrMsg(int nErr);

IMPLEMENT_SERIAL(CGridModel, CObject, VERSIONABLE_SCHEMA | 0)


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGridModel::CGridModel()
{
	m_nGridX = 0;
	m_nGridY = 0;
	m_nGridZ = 0;
	m_bCornerPointGrid = TRUE;
	m_dMinX = 0.0;						// 
	m_dMinY = 0.0;						// 
	m_dMinZ = 0.0;						// 
	m_dMaxX = 0.0;
	m_dMaxY = 0.0;
	m_dMaxZ = 0.0;
}

CGridModel::~CGridModel()
{
}

void CGridModel::InitGrid(int nX/* = 0*/, int nY/* = 0*/, int nZ/* = 0*/, BOOL bCorner/* = TRUE*/)
{
	m_nGridX = nX;
	m_nGridY = nY;
	m_nGridZ = nZ;
	m_bCornerPointGrid = bCorner;
	m_dMinX = 0.0;						// 
	m_dMinY = 0.0;						// 
	m_dMinZ = 0.0;						// 
	m_dMaxX = 0.0;
	m_dMaxY = 0.0;
	m_dMaxZ = 0.0;

	// 正交网格
	m_gridDX.clear();		// X 间隔
	m_gridDY.clear();		// Y 间隔
	m_gridDZ.clear();		// Z 间隔
	m_gridTops.clear();		// m_nGridX*m_nGridY 顶面中心深度（只顶层)

	// 角点网格
	m_coordGrid.clear();	// 坐标线 (m_nGridX+1)*(m_nGridY+1)
	m_zcornGrid.clear();	// 角点深度 m_nGridX*m_nGridY*m_gridDZ*8

	if (nX > 0 && nY > 0 && nZ > 0)
	{
		if (bCorner) // 角点网格
		{
			for (int i = 0; i < (m_nGridX + 1) * (m_nGridY + 1) * 2; i++)
			{
				CVertex3D pt;
				m_coordGrid.push_back(pt);
			}

			for (int i = 0; i < (m_nGridX* m_nGridY* m_nGridZ*8); i++)
				m_zcornGrid.push_back(0.0);
		}
		else	// 正交网格
		{
			for (int i = 0; i < m_nGridX; i++)
			{
				m_gridDX.push_back(0.0);
			}

			for (int i = 0; i < m_nGridY; i++)
			{
				m_gridDY.push_back(0.0);
			}

			for (int i = 0; i < m_nGridZ; i++)
			{
				m_gridDZ.push_back(0.0);
			}

			for (int i = 0; i < m_nGridX* m_nGridY; i++)
			{
				m_gridTops.push_back(0.0);
			}
		}
	}
}

void CGridModel::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);
	ar.SerializeClass(RUNTIME_CLASS(CGridModel));

	if (ar.IsStoring())
	{
		// TODO: add storing code here		
		ar << m_bCornerPointGrid;
		ar << m_nGridX;
		ar << m_nGridY;
		ar << m_nGridZ;

		ar << m_dMinX;						// 
		ar << m_dMinY;						// 
		ar << m_dMinZ;						// 
		ar << m_dMaxX;
		ar << m_dMaxY;
		ar << m_dMaxZ;

		if (m_bCornerPointGrid) // 角点网格
		{
			// 角点网格
			long size = (m_nGridY + 1) * (m_nGridX + 1) * 2;
			for (long i = 0; i < size; i++)
			{
				ar << m_coordGrid[i];
			}

			size = m_nGridY* m_nGridX* m_nGridZ*8;

			for (long i = 0; i < size; i++)
			{
				ar << m_zcornGrid[i];
			}
		}
		else
		{
			// 正交网格
			long size = m_nGridY* m_nGridX* m_nGridZ;
			for (long i = 0; i < size; i++)
			{
				ar << m_gridDX[i];
				ar << m_gridDY[i];
				ar << m_gridDZ[i];
			}

			size = m_nGridY* m_nGridX;
			for (long i = 0; i < size; i++)
				ar << m_gridTops[i];
		}
	}
	else
	{
		// 正交网格
		m_gridDX.clear();		// m_nGridX*m_nGridY*m_nGridZ
		m_gridDY.clear();		// m_nGridX*m_nGridY*m_nGridZ
		m_gridDZ.clear();		// m_nGridX*m_nGridY*m_nGridZ 块中心厚度
		m_gridTops.clear();		// m_nGridX*m_nGridY 顶面中心深度

		// 角点网格
		m_coordGrid.clear();	// 坐标线 (m_nGridX+1)*(m_nGridY+1)
		m_zcornGrid.clear();	// 角点深度 m_nGridX*m_nGridY*m_gridDZ*8

		int iVer = ar.GetObjectSchema(); // 获取版本信息

		switch (iVer)
		{
		case 0:
			{
				ar >> m_bCornerPointGrid;

				ar >> m_nGridX;
				ar >> m_nGridY;
				ar >> m_nGridZ;

				ar >> m_dMinX;						// 
				ar >> m_dMinY;						// 
				ar >> m_dMinZ;						// 
				ar >> m_dMaxX;
				ar >> m_dMaxY;
				ar >> m_dMaxZ;

				if (m_bCornerPointGrid) // 角点网格
				{
					// 角点网格
					CVertex3D point;
					double v;
					ULONG size = (m_nGridY + 1) * (m_nGridX + 1) * 2;
					for (ULONG i = 0; i < size; i++)
					{
						ar >> point;
						m_coordGrid.push_back(point);
					}

					size = m_nGridY * m_nGridX * m_nGridZ * 8;

					for (ULONG i = 0; i < size; i++)
					{
						ar >> v;
						m_zcornGrid.push_back(v);
					}
				}
				else
				{
					// 正交网格
					double v;
					long size = m_nGridY* m_nGridX* m_nGridZ;
					for (long i = 0; i < size; i++)
					{
						ar >> v;
						m_gridDX.push_back(v);
						ar >> v;
						m_gridDY.push_back(v);
						ar >> v;
						m_gridDZ.push_back(v);
					}
					size = m_nGridY* m_nGridX;
					for (long i = 0; i < size; i++)
					{
						ar >> v;
						m_gridTops.push_back(v);
					}
				}
			}
			break;
		default:
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGridModel commands
/* 
	 (0)*---------------------*(1)
	   /|   				 /|
	  / |   				/ |
	 /  |   			   /  |
	/   |   			  /   |
(3)*---------------------*(2) |
   |	|   			 |    |
   |	|   			 |    |
   |	|   			 |    |
   | [4]*---------------------*[5]
   |   /				 |   /
   |  / 				 |  /
   | /  				 | /
   |/   				 |/
[7]*---------------------*[6]
*/
CVertex3D CGridModel::GetCornerPoint(int nX, int nY, int nZ, int nIndex)
{
	CVertex3D point;
	if (m_bCornerPointGrid) // 是角点网格
	{
		CVertex3D p1, p2;

		// 直线过两点:(x1, y1, z1)及(x2, y2, z2)，则
		//
		//  			   x - x1   y - y1   z - z1
		// 空间直线方程为：------ = ------ = ------
		//  			   x2-x1	y2-y1    z2-z1
		// 
		// 则，已知z求得 x,y 如下：
		//     x = (z-z1)(x2-x1)/(z2-z1) + x1
		//     y = (z-z1)(y2-y1)/(z2-z1) + y1
		// 

		double x, y, z;
		
		/*if (nIndex == 0)
		{
			p1 = m_coordGrid[2 * nY * (m_nGridX + 1) + 2 * nX];
			p2 = m_coordGrid[2 * nY * (m_nGridX + 1) + 2 * nX + 1];
			z = m_zcornGrid[ 8 * nZ * m_nGridX*m_nGridY + 4 * nY * m_nGridX + nX * 2];
		}
		if (nIndex == 1)
		{
			p1 = m_coordGrid[2 * nY * (m_nGridX + 1) + 2 * (nX + 1)];
			p2 = m_coordGrid[2 * nY * (m_nGridX + 1) + 2 * (nX + 1) + 1];
			z = m_zcornGrid[ 8 * nZ * m_nGridX*m_nGridY + 4 * nY * m_nGridX + nX * 2 + 1];
		}
		if (nIndex == 2)
		{
			p1 = m_coordGrid[2 * (nY + 1) * (m_nGridX + 1) + 2 * (nX + 1)];
			p2 = m_coordGrid[2 * (nY + 1) * (m_nGridX + 1) + 2 * (nX + 1) + 1];
			z = m_zcornGrid[ 8 * nZ * m_nGridX*m_nGridY + ( 2 * nY + 1 ) * m_nGridX * 2 + nX * 2 + 1];
		}
		if (nIndex == 3)
		{
			p1 = m_coordGrid[2 * (nY + 1) * (m_nGridX + 1) + 2 * nX];
			p2 = m_coordGrid[2 * (nY + 1) * (m_nGridX + 1) + 2 * nX + 1];
			z = m_zcornGrid[ 8 * nZ * m_nGridX*m_nGridY + ( 2 * nY + 1 ) * m_nGridX * 2 + nX * 2];
		}
		if (nIndex == 4)
		{
			p1 = m_coordGrid[2 * nY * (m_nGridX + 1) + 2 * nX];
			p2 = m_coordGrid[2 * nY * (m_nGridX + 1) + 2 * nX + 1];
			z = m_zcornGrid[ ( 2 * nZ + 1 ) * (m_nGridX*m_nGridY * 4 ) + 2 * nY * m_nGridX * 2 + nX * 2];
		}
		if (nIndex == 5)
		{
			p1 = m_coordGrid[2 * nY * (m_nGridX + 1) + 2 * (nX + 1)];
			p2 = m_coordGrid[2 * nY * (m_nGridX + 1) + 2 * (nX + 1) + 1];
			z = m_zcornGrid[ ( 2 * nZ + 1 ) * (m_nGridX*m_nGridY * 4 ) + 2 * nY * m_nGridX * 2 + nX * 2 + 1];
		}
		if (nIndex == 6)
		{
			p1 = m_coordGrid[2 * (nY + 1) * (m_nGridX + 1) + 2 * (nX + 1)];
			p2 = m_coordGrid[2 * (nY + 1) * (m_nGridX + 1) + 2 * (nX + 1) + 1];
			z = m_zcornGrid[ ( 2 * nZ + 1 ) * (m_nGridX*m_nGridY * 4 ) + ( 2 * nY + 1 ) * m_nGridX * 2 + nX * 2 + 1];
		}
		if (nIndex == 7)
		{
			p1 = m_coordGrid[2 * (nY + 1) * (m_nGridX + 1) + 2 * nX];
			p2 = m_coordGrid[2 * (nY + 1) * (m_nGridX + 1) + 2 * nX + 1];
			z = m_zcornGrid[ ( 2 * nZ + 1 ) * (m_nGridX*m_nGridY * 4 ) + ( 2 * nY + 1 )* m_nGridX * 2 + nX * 2];
		}
		*/

		if (nIndex == 0)
		{
			p1 = m_coordGrid[2 * nY * (m_nGridX + 1) + 2 * nX];
			p2 = m_coordGrid[2 * nY * (m_nGridX + 1) + 2 * nX + 1];
			z = m_zcornGrid[ 8 * nZ * m_nGridX*m_nGridY + 4 * nY * m_nGridX + nX * 2];
		}
		if (nIndex == 1)
		{
			p1 = m_coordGrid[2 * nY * (m_nGridX + 1) + 2 * (nX + 1)];
			p2 = m_coordGrid[2 * nY * (m_nGridX + 1) + 2 * (nX + 1) + 1];
			z = m_zcornGrid[ 8 * nZ * m_nGridX*m_nGridY + 4 * nY * m_nGridX + nX * 2 + 1];
		}
		if (nIndex == 2)
		{
			p1 = m_coordGrid[2 * (nY + 1) * (m_nGridX + 1) + 2 * (nX + 1)];
			p2 = m_coordGrid[2 * (nY + 1) * (m_nGridX + 1) + 2 * (nX + 1) + 1];
			z = m_zcornGrid[ 8 * nZ * m_nGridX*m_nGridY + 4 * nY * m_nGridX + m_nGridX*2 + nX * 2 + 1];
		}
		if (nIndex == 3)
		{
			p1 = m_coordGrid[2 * (nY + 1) * (m_nGridX + 1) + 2 * nX];
			p2 = m_coordGrid[2 * (nY + 1) * (m_nGridX + 1) + 2 * nX + 1];
			z = m_zcornGrid[ 8 * nZ * m_nGridX*m_nGridY + 4 * nY * m_nGridX + m_nGridX*2 + nX * 2];
		}
		if (nIndex == 4)
		{
			p1 = m_coordGrid[2 * nY * (m_nGridX + 1) + 2 * nX];
			p2 = m_coordGrid[2 * nY * (m_nGridX + 1) + 2 * nX + 1];
			z = m_zcornGrid[ ( 2 * nZ + 1 ) * (m_nGridX*m_nGridY * 4 ) + 2 * nY * m_nGridX * 2 + nX * 2];
		}
		if (nIndex == 5)
		{
			p1 = m_coordGrid[2 * nY * (m_nGridX + 1) + 2 * (nX + 1)];
			p2 = m_coordGrid[2 * nY * (m_nGridX + 1) + 2 * (nX + 1) + 1];
			z = m_zcornGrid[ ( 2 * nZ + 1 ) * (m_nGridX*m_nGridY * 4 ) + 2 * nY * m_nGridX * 2 + nX * 2 + 1];
		}
		if (nIndex == 6)
		{
			p1 = m_coordGrid[2 * (nY + 1) * (m_nGridX + 1) + 2 * (nX + 1)];
			p2 = m_coordGrid[2 * (nY + 1) * (m_nGridX + 1) + 2 * (nX + 1) + 1];
			z = m_zcornGrid[ ( 2 * nZ + 1 ) * (m_nGridX*m_nGridY * 4 ) + ( 2 * nY + 1 ) * m_nGridX * 2 + nX * 2 + 1];
		}
		if (nIndex == 7)
		{
			p1 = m_coordGrid[2 * (nY + 1) * (m_nGridX + 1) + 2 * nX];
			p2 = m_coordGrid[2 * (nY + 1) * (m_nGridX + 1) + 2 * nX + 1];
			z = m_zcornGrid[ ( 2 * nZ + 1 ) * (m_nGridX*m_nGridY * 4 ) + ( 2 * nY + 1 )* m_nGridX * 2 + nX * 2];
		}

		if( nX == 0 && nY == 0 && nZ == 0 )
		{
			TRACE("%g ", z);
		}
		x = (z - p1.GetZ()) * (p2.GetX() - p1.GetX()) / (p2.GetZ() - p1.GetZ()) + p1.GetX(); 
		y = (z - p1.GetZ()) * (p2.GetY() - p1.GetY()) / (p2.GetZ() - p1.GetZ()) + p1.GetY(); 

		point.SetParam(x, y, z);
	}
	else	// 正交网格
	{
		double x1 = 0, x2 = 0.0;
		double y1 = 0, y2 = 0.0;

		double top, bottom;

		top = m_gridTops[nY * m_nGridX + nX];

		bottom = top;

		for (int i = 0; i <= nZ; i++)
		{
			if (i > 0)
			{
				top += m_gridDZ[(i - 1) * (m_nGridX * m_nGridY) + nY * m_nGridX + nX];
			}
			bottom += m_gridDZ[i * (m_nGridX * m_nGridY) + nY * m_nGridX + nX];
		}

		for (int j = 0; j <= nY; j++)
		{
			if (j > 0)
				y1 += m_gridDY[nZ * (m_nGridX * m_nGridY) + (j - 1) * m_nGridX + nX];
			y2 += m_gridDY[nZ * (m_nGridX * m_nGridY) + j * m_nGridX + nX];
		}	

		for (int k = 0; k <= nX; k++)
		{
			if (k > 0)
				x1 += m_gridDX[nZ * (m_nGridX * m_nGridY) + nY * m_nGridX + k - 1];					

			x2 += m_gridDX[nZ * (m_nGridX * m_nGridY) + nY * m_nGridX + k];
		}

		if (nIndex == 0 || nIndex == 3 || nIndex == 4 || nIndex == 7)
			point.SetX(x1);
		else
			point.SetX(x2);

		if (nIndex == 0 || nIndex == 1 || nIndex == 4 || nIndex == 5)
			point.SetY(y1);
		else
			point.SetY(y2);

		if (nIndex == 0 || nIndex == 1 || nIndex == 2 || nIndex == 3) // 上面四个点
			point.SetZ(top);
		else
			point.SetZ(bottom);
	}

	return point;
}

tagGridModelCell CGridModel::GetGridCell(int nX, int nY, int nZ)
{
	tagGridModelCell cell;
	for (int i = 0; i < 8; i++)
	{
		cell.m_cornerPoint[i] = GetCornerPoint(nX, nY, nZ, i);
	}
	return cell;
}

BOOL CGridModel::OpenFile(LPCTSTR lpFileName)
{
	try
	{
		CFile file(lpFileName, CFile::modeRead | CFile::typeBinary);
		CArchive ar(&file, CArchive::load);
		Serialize(ar);
		ar.Close();
		file.Close();
		return TRUE;
	}
	catch (CFileException* pe)
	{
		CString msg;
		msg.Format(_T("    文件打开错误: %s"), GetFileErrMsg(pe->m_cause));
		AfxMessageBox(msg);
		pe->Delete();
		return FALSE;
	}
	catch (...)
	{
		AfxMessageBox(_T("无法预料的错误!"));
		return FALSE;
	}
}

BOOL CGridModel::SaveFile(LPCTSTR lpFileName)
{
	try
	{
		CFile file(lpFileName, CFile::modeWrite | CFile::modeCreate | CFile::typeBinary);
		CArchive ar(&file, CArchive::store);
		Serialize(ar);
		ar.Close();
		file.Close();
		return TRUE;
	}
	catch (CFileException* pe)
	{
		CString msg;
		msg.Format(_T("    文件打开错误: %s"), GetFileErrMsg(pe->m_cause));
		AfxMessageBox(msg);
		pe->Delete();
		return FALSE;
	}
	catch (...)
	{
		AfxMessageBox(_T("无法预料的错误!"));
		return FALSE;
	}
}

void CGridModel::SaveTop(CStdioFile& file)
{
	CString strTmp;
	CVertex3D pt;
	double x, y, z;

	//-----------------------------------------------------------------------
	// 上面

	file.WriteString(_T("DEF Mesh-ROOT Transform {\n"));
	file.WriteString(_T("  translation 0 0 0\n"));
	file.WriteString(_T("  rotation 0 0 0 0\n"));
	file.WriteString(_T("  scale 2 1.2 2\n"));
	file.WriteString(_T("  children [\n"));
	file.WriteString(_T("    Shape {\n"));
	file.WriteString(_T("      appearance Appearance {\n"));
	file.WriteString(_T("        material Material {\n"));
	file.WriteString(_T("          diffuseColor 0 0 0\n"));
	file.WriteString(_T("        }\n"));
	file.WriteString(_T("      }\n"));
	file.WriteString(_T("      geometry DEF Mesh-FACES IndexedFaceSet {\n"));
	file.WriteString(_T("        ccw TRUE\n"));
	file.WriteString(_T("        solid TRUE\n"));
	file.WriteString(_T("        coord DEF Mesh-COORD Coordinate { point [\n"));

	for (int j = 0; j < m_nGridY; j++)
	{
		for (int i = 0; i < m_nGridX; i++)  //
		{
			if (i == 0)
			{
				pt = GetCornerPoint(i, j, 0, 0);
				x = (pt.GetX() - m_dMinX) / (m_dMaxX - m_dMinX) * 2 - 1;
				y = -((pt.GetZ() - m_dMinZ) / (m_dMaxZ - m_dMinZ) * 2 - 1);					
				z = -((pt.GetY() - m_dMinY) / (m_dMaxY - m_dMinY) * 2 - 1);
				strTmp.Format(_T("        %g %g %g,\n"), x, y, z);
				file.WriteString(strTmp);
			}

			pt = GetCornerPoint(i, j, 0, 1);
			x = (pt.GetX() - m_dMinX) / (m_dMaxX - m_dMinX) * 2 - 1;
			y = -((pt.GetZ() - m_dMinZ) / (m_dMaxZ - m_dMinZ) * 2 - 1);				
			z = -((pt.GetY() - m_dMinY) / (m_dMaxY - m_dMinY) * 2 - 1);
			strTmp.Format(_T("        %g %g %g,\n"), x, y, z);
			file.WriteString(strTmp);
		}

		if (j == m_nGridY - 1)
		{
			for (int i = 0; i < m_nGridX; i++)  //
			{
				if (i == 0)
				{
					pt = GetCornerPoint(i, j, 0, 3);
					x = (pt.GetX() - m_dMinX) / (m_dMaxX - m_dMinX) * 2 - 1;
					y = -((pt.GetZ() - m_dMinZ) / (m_dMaxZ - m_dMinZ) * 2 - 1);
					z = -((pt.GetY() - m_dMinY) / (m_dMaxY - m_dMinY) * 2 - 1);
					strTmp.Format(_T("        %g %g %g,\n"), x, y, z);
					file.WriteString(strTmp);
				}

				pt = GetCornerPoint(i, j, 0, 2);
				x = (pt.GetX() - m_dMinX) / (m_dMaxX - m_dMinX) * 2 - 1;
				y = -((pt.GetZ() - m_dMinZ) / (m_dMaxZ - m_dMinZ) * 2 - 1);					
				z = -((pt.GetY() - m_dMinY) / (m_dMaxY - m_dMinY) * 2 - 1);
				if (i < m_nGridX - 1)
					strTmp.Format(_T("        %g %g %g,\n"), x, y, z);
				else
					strTmp.Format(_T("        %g %g %g]\n"), x, y, z);
				file.WriteString(strTmp);
			}
		}
	}

	file.WriteString(_T("        }\n"));
	file.WriteString(_T("        coordIndex [\n"));

	for (int j = 0; j < m_nGridY; j++)
	{
		for (int i = 0; i < m_nGridX; i++)  //
		{
/*			CVertex3D pt0 = GetCornerPoint(i, j, 0, 0);
			CVertex3D pt1 = GetCornerPoint(i, j, 0, 1);
			CVertex3D pt2 = GetCornerPoint(i, j, 0, 2);

			CFaceTriangles face(&pt0, &pt1, &pt2);
			CVector3D v = face.GetNormal();
			if( v.GetX() == 0.0 && v.GetY()== 0.0)
				continue;
*/

			strTmp.Format(_T("        %d, %d, %d, -1,\n"), j * (m_nGridX + 1) + i + 1, (j + 1) * (m_nGridX + 1) + i, j * (m_nGridX + 1) + i);
			file.WriteString(strTmp);

			if (j == m_nGridY - 1 && i == m_nGridX - 1)
				strTmp.Format(_T("        %d, %d, %d, -1]\n"), (j + 1) * (m_nGridX + 1) + i + 1, (j + 1) * (m_nGridX + 1) + i, j * (m_nGridX + 1) + i + 1);
			else
				strTmp.Format(_T("        %d, %d, %d, -1,\n"), (j + 1) * (m_nGridX + 1) + i + 1, (j + 1) * (m_nGridX + 1) + i, j * (m_nGridX + 1) + i + 1);
			file.WriteString(strTmp);
		}
	}

	file.WriteString(_T("        ]}\n"));
	file.WriteString(_T("      }\n"));
	file.WriteString(_T("    ]\n"));
	file.WriteString(_T("  }\n"));
}

void CGridModel::SaveBottom(CStdioFile& file)
{
	CString strTmp;
	CVertex3D pt;
	double x, y, z;

	//-----------------------------------------------------------------------
	// 下面
	file.WriteString(_T("DEF Mesh-ROOT Transform {\n"));
	file.WriteString(_T("  translation 0 0 0\n"));
	file.WriteString(_T("  rotation 0 0 0 0\n"));
	file.WriteString(_T("  scale 2 1.2 2\n"));
	file.WriteString(_T("  children [\n"));
	file.WriteString(_T("    Shape {\n"));
	file.WriteString(_T("      appearance Appearance {\n"));
	file.WriteString(_T("        material Material {\n"));
	file.WriteString(_T("          diffuseColor 0 0 0\n"));
	file.WriteString(_T("        }\n"));
	file.WriteString(_T("      }\n"));
	file.WriteString(_T("      geometry DEF Mesh-FACES IndexedFaceSet {\n"));
	file.WriteString(_T("        ccw TRUE\n"));
	file.WriteString(_T("        solid TRUE\n"));
	file.WriteString(_T("        coord DEF Mesh-COORD Coordinate { point [\n"));

	for (int j = 0; j < m_nGridY; j++)
	{
		for (int i = 0; i < m_nGridX; i++)  //
		{
			if (i == 0)
			{
				pt = GetCornerPoint(i, j, m_nGridZ - 1, 4);
				x = (pt.GetX() - m_dMinX) / (m_dMaxX - m_dMinX) * 2 - 1;
				y = -((pt.GetZ() - m_dMinZ) / (m_dMaxZ - m_dMinZ) * 2 - 1);					
				z = -((pt.GetY() - m_dMinY) / (m_dMaxY - m_dMinY) * 2 - 1);
				strTmp.Format(_T("        %g %g %g,\n"), x, y, z);
				file.WriteString(strTmp);
			}

			pt = GetCornerPoint(i, j, m_nGridZ - 1, 5);
			x = (pt.GetX() - m_dMinX) / (m_dMaxX - m_dMinX) * 2 - 1;
			y = -((pt.GetZ() - m_dMinZ) / (m_dMaxZ - m_dMinZ) * 2 - 1);				
			z = -((pt.GetY() - m_dMinY) / (m_dMaxY - m_dMinY) * 2 - 1);
			strTmp.Format(_T("        %g %g %g,\n"), x, y, z);
			file.WriteString(strTmp);
		}

		if (j == m_nGridY - 1)
		{
			for (int i = 0; i < m_nGridX; i++)  //
			{
				if (i == 0)
				{
					pt = GetCornerPoint(i, j, m_nGridZ - 1, 7);
					x = (pt.GetX() - m_dMinX) / (m_dMaxX - m_dMinX) * 2 - 1;
					y = -((pt.GetZ() - m_dMinZ) / (m_dMaxZ - m_dMinZ) * 2 - 1);
					z = -((pt.GetY() - m_dMinY) / (m_dMaxY - m_dMinY) * 2 - 1);
					strTmp.Format(_T("        %g %g %g,\n"), x, y, z);
					file.WriteString(strTmp);
				}

				pt = GetCornerPoint(i, j, m_nGridZ - 1, 6);
				x = (pt.GetX() - m_dMinX) / (m_dMaxX - m_dMinX) * 2 - 1;
				y = -((pt.GetZ() - m_dMinZ) / (m_dMaxZ - m_dMinZ) * 2 - 1);
				z = -((pt.GetY() - m_dMinY) / (m_dMaxY - m_dMinY) * 2 - 1);
				if (i < m_nGridX - 1)
					strTmp.Format(_T("        %g %g %g,\n"), x, y, z);
				else
					strTmp.Format(_T("        %g %g %g]\n"), x, y, z);
				file.WriteString(strTmp);
			}
		}
	}

	file.WriteString(_T("        }\n"));
	file.WriteString(_T("        coordIndex [\n"));

	for (int j = 0; j < m_nGridY; j++)
	{
		for (int i = 0; i < m_nGridX; i++)
		{
			strTmp.Format(_T("        %d, %d, %d, -1,\n"), j * (m_nGridX + 1) + i, (j + 1) * (m_nGridX + 1) + i, j * (m_nGridX + 1) + i + 1);
			file.WriteString(strTmp);

			if (j == m_nGridY - 1 && i == m_nGridX - 1)
				strTmp.Format(_T("        %d, %d, %d, -1]\n"), j * (m_nGridX + 1) + i + 1, (j + 1) * (m_nGridX + 1) + i, (j + 1) * (m_nGridX + 1) + i + 1);
			else
				strTmp.Format(_T("        %d, %d, %d, -1,\n"), j * (m_nGridX + 1) + i + 1, (j + 1) * (m_nGridX + 1) + i, (j + 1) * (m_nGridX + 1) + i + 1);
			file.WriteString(strTmp);
		}
	}	

	file.WriteString(_T("        }\n"));
	file.WriteString(_T("      }\n"));
	file.WriteString(_T("    ]\n"));
	file.WriteString(_T("  }\n"));
}

void CGridModel::SaveFront(CStdioFile& file)
{
	CString strTmp;
	CVertex3D pt;
	double x, y, z;

	//-----------------------------------------------------------------------
	// 前面
	file.WriteString(_T("DEF Mesh-ROOT Transform {\n"));
	file.WriteString(_T("  translation 0 0 0\n"));
	file.WriteString(_T("  rotation 0 0 0 0\n"));
	file.WriteString(_T("  scale 2 1.2 2\n"));
	file.WriteString(_T("  children [\n"));
	file.WriteString(_T("    Shape {\n"));
	file.WriteString(_T("      appearance Appearance {\n"));
	file.WriteString(_T("        material Material {\n"));
	file.WriteString(_T("          diffuseColor 0 0 0\n"));
	file.WriteString(_T("        }\n"));
	file.WriteString(_T("      }\n"));
	file.WriteString(_T("      geometry DEF Mesh-FACES IndexedFaceSet {\n"));
	file.WriteString(_T("        ccw TRUE\n"));
	file.WriteString(_T("        solid TRUE\n"));
	file.WriteString(_T("        coord DEF Mesh-COORD Coordinate { point [\n"));

	for (int k = 0; k < m_nGridZ; k++)
	{
		for (int i = 0; i < m_nGridX; i++)  //
		{
			if (i == 0)
			{
				pt = GetCornerPoint(i, 0, k, 0);
				x = (pt.GetX() - m_dMinX) / (m_dMaxX - m_dMinX) * 2 - 1;
				y = -((pt.GetZ() - m_dMinZ) / (m_dMaxZ - m_dMinZ) * 2 - 1);					
				z = -((pt.GetY() - m_dMinY) / (m_dMaxY - m_dMinY) * 2 - 1);
				strTmp.Format(_T("        %g %g %g,\n"), x, y, z);
				file.WriteString(strTmp);
			}

			pt = GetCornerPoint(i, 0, k, 1);
			x = (pt.GetX() - m_dMinX) / (m_dMaxX - m_dMinX) * 2 - 1;
			y = -((pt.GetZ() - m_dMinZ) / (m_dMaxZ - m_dMinZ) * 2 - 1);
			z = -((pt.GetY() - m_dMinY) / (m_dMaxY - m_dMinY) * 2 - 1);
			strTmp.Format(_T("        %g %g %g,\n"), x, y, z);
			file.WriteString(strTmp);
		}

		if (k == m_nGridZ - 1)
		{
			for (int i = 0; i < m_nGridX; i++)  //
			{
				if (i == 0)
				{
					pt = GetCornerPoint(i, 0, k, 4);
					x = (pt.GetX() - m_dMinX) / (m_dMaxX - m_dMinX) * 2 - 1;
					y = -((pt.GetZ() - m_dMinZ) / (m_dMaxZ - m_dMinZ) * 2 - 1);					
					z = -((pt.GetY() - m_dMinY) / (m_dMaxY - m_dMinY) * 2 - 1);
					strTmp.Format(_T("        %g %g %g,\n"), x, y, z);
					file.WriteString(strTmp);
				}

				pt = GetCornerPoint(i, 0, k, 5);
				x = (pt.GetX() - m_dMinX) / (m_dMaxX - m_dMinX) * 2 - 1;
				y = -((pt.GetZ() - m_dMinZ) / (m_dMaxZ - m_dMinZ) * 2 - 1);					
				z = -((pt.GetY() - m_dMinY) / (m_dMaxY - m_dMinY) * 2 - 1);
				if (i < m_nGridX - 1)
					strTmp.Format(_T("        %g %g %g,\n"), x, y, z);
				else
					strTmp.Format(_T("        %g %g %g]\n"), x, y, z);
				file.WriteString(strTmp);
			}
		}
	}

	file.WriteString(_T("        }\n"));
	file.WriteString(_T("        coordIndex [\n"));

	for (int k = 0; k < m_nGridZ; k++)
	{
		for (int i = 0; i < m_nGridX; i++)
		{
			strTmp.Format(_T("        %d, %d, %d, -1,\n"), k * (m_nGridX + 1) + i, (k + 1) * (m_nGridX + 1) + i, k * (m_nGridX + 1) + i + 1);
			file.WriteString(strTmp);

			if (k == m_nGridZ - 1 && i == m_nGridX - 1)
				strTmp.Format(_T("        %d, %d, %d, -1]\n"), k * (m_nGridX + 1) + i + 1, (k + 1) * (m_nGridX + 1) + i, (k + 1) * (m_nGridX + 1) + i + 1);
			else
				strTmp.Format(_T("        %d, %d, %d, -1,\n"), k * (m_nGridX + 1) + i + 1, (k + 1) * (m_nGridX + 1) + i, (k + 1) * (m_nGridX + 1) + i + 1);
			file.WriteString(strTmp);
		}
	}

	file.WriteString(_T("        }\n"));
	file.WriteString(_T("      }\n"));
	file.WriteString(_T("    ]\n"));
	file.WriteString(_T("  }\n"));
}

void CGridModel::SaveBack(CStdioFile& file)
{
	CString strTmp;
	CVertex3D pt;
	double x, y, z;

	//-----------------------------------------------------------------------
	// 后面
	file.WriteString(_T("DEF Mesh-ROOT Transform {\n"));
	file.WriteString(_T("  translation 0 0 0\n"));
	file.WriteString(_T("  rotation 0 0 0 0\n"));
	file.WriteString(_T("  scale 2 1.2 2\n"));
	file.WriteString(_T("  children [\n"));
	file.WriteString(_T("    Shape {\n"));
	file.WriteString(_T("      appearance Appearance {\n"));
	file.WriteString(_T("        material Material {\n"));
	file.WriteString(_T("          diffuseColor 0 0 0\n"));
	file.WriteString(_T("        }\n"));
	file.WriteString(_T("      }\n"));
	file.WriteString(_T("      geometry DEF Mesh-FACES IndexedFaceSet {\n"));
	file.WriteString(_T("        ccw TRUE\n"));
	file.WriteString(_T("        solid TRUE\n"));
	file.WriteString(_T("        coord DEF Mesh-COORD Coordinate { point [\n"));

	for (int k = 0; k < m_nGridZ; k++)
	{
		for (int i = 0; i < m_nGridX; i++)  //
		{
			if (i == 0)
			{
				pt = GetCornerPoint(i, m_nGridY-1, k, 3);
				x = (pt.GetX() - m_dMinX) / (m_dMaxX - m_dMinX) * 2 - 1;
				y = -((pt.GetZ() - m_dMinZ) / (m_dMaxZ - m_dMinZ) * 2 - 1);					
				z = -((pt.GetY() - m_dMinY) / (m_dMaxY - m_dMinY) * 2 - 1);
				strTmp.Format(_T("        %g %g %g,\n"), x, y, z);
				file.WriteString(strTmp);
			}

			pt = GetCornerPoint(i, m_nGridY-1, k, 2);
			x = (pt.GetX() - m_dMinX) / (m_dMaxX - m_dMinX) * 2 - 1;
			y = -((pt.GetZ() - m_dMinZ) / (m_dMaxZ - m_dMinZ) * 2 - 1);
			z = -((pt.GetY() - m_dMinY) / (m_dMaxY - m_dMinY) * 2 - 1);
			strTmp.Format(_T("        %g %g %g,\n"), x, y, z);
			file.WriteString(strTmp);
		}

		if (k == m_nGridZ - 1)
		{
			for (int i = 0; i < m_nGridX; i++)  //
			{
				if (i == 0)
				{
					pt = GetCornerPoint(i, m_nGridY-1, k, 7);
					x = (pt.GetX() - m_dMinX) / (m_dMaxX - m_dMinX) * 2 - 1;
					y = -((pt.GetZ() - m_dMinZ) / (m_dMaxZ - m_dMinZ) * 2 - 1);					
					z = -((pt.GetY() - m_dMinY) / (m_dMaxY - m_dMinY) * 2 - 1);
					strTmp.Format(_T("        %g %g %g,\n"), x, y, z);
					file.WriteString(strTmp);
				}

				pt = GetCornerPoint(i, m_nGridY-1, k, 6);
				x = (pt.GetX() - m_dMinX) / (m_dMaxX - m_dMinX) * 2 - 1;
				y = -((pt.GetZ() - m_dMinZ) / (m_dMaxZ - m_dMinZ) * 2 - 1);					
				z = -((pt.GetY() - m_dMinY) / (m_dMaxY - m_dMinY) * 2 - 1);
				if (i < m_nGridX - 1)
					strTmp.Format(_T("        %g %g %g,\n"), x, y, z);
				else
					strTmp.Format(_T("        %g %g %g]\n"), x, y, z);
				file.WriteString(strTmp);
			}
		}
	}

	file.WriteString(_T("        }\n"));
	file.WriteString(_T("        coordIndex [\n"));

	for (int k = 0; k < m_nGridZ; k++)
	{
		for (int i = 0; i < m_nGridX; i++)
		{
			strTmp.Format(_T("        %d, %d, %d, -1,\n"), k * (m_nGridX + 1) + i, (k + 1) * (m_nGridX + 1) + i, k * (m_nGridX + 1) + i + 1);
			file.WriteString(strTmp);

			if (k == m_nGridZ - 1 && i == m_nGridX - 1)
				strTmp.Format(_T("        %d, %d, %d, -1]\n"), k * (m_nGridX + 1) + i + 1, (k + 1) * (m_nGridX + 1) + i, (k + 1) * (m_nGridX + 1) + i + 1);
			else
				strTmp.Format(_T("        %d, %d, %d, -1,\n"), k * (m_nGridX + 1) + i + 1, (k + 1) * (m_nGridX + 1) + i, (k + 1) * (m_nGridX + 1) + i + 1);
			file.WriteString(strTmp);
		}
	}

	file.WriteString(_T("        }\n"));
	file.WriteString(_T("      }\n"));
	file.WriteString(_T("    ]\n"));
	file.WriteString(_T("  }\n"));
}


void CGridModel::SaveLeft(CStdioFile& file)
{
	CString strTmp;
	CVertex3D pt;
	double x, y, z;

	//-----------------------------------------------------------------------
	// 左面
	file.WriteString(_T("DEF Mesh-ROOT Transform {\n"));
	file.WriteString(_T("  translation 0 0 0\n"));
	file.WriteString(_T("  rotation 0 0 0 0\n"));
	file.WriteString(_T("  scale 2 1.2 2\n"));
	file.WriteString(_T("  children [\n"));
	file.WriteString(_T("    Shape {\n"));
	file.WriteString(_T("      appearance Appearance {\n"));
	file.WriteString(_T("        material Material {\n"));
	file.WriteString(_T("          diffuseColor 0 0 0\n"));
	file.WriteString(_T("        }\n"));
	file.WriteString(_T("      }\n"));
	file.WriteString(_T("      geometry DEF Mesh-FACES IndexedFaceSet {\n"));
	file.WriteString(_T("        ccw TRUE\n"));
	file.WriteString(_T("        solid TRUE\n"));
	file.WriteString(_T("        coord DEF Mesh-COORD Coordinate { point [\n"));

	for (int k = 0; k < m_nGridZ; k++)
	{
		for (int j = 0; j < m_nGridY; j++)  //
		{
			if (j == 0)
			{
				pt = GetCornerPoint(0, j, k, 0);
				x = (pt.GetX() - m_dMinX) / (m_dMaxX - m_dMinX) * 2 - 1;
				y = -((pt.GetZ() - m_dMinZ) / (m_dMaxZ - m_dMinZ) * 2 - 1);					
				z = -((pt.GetY() - m_dMinY) / (m_dMaxY - m_dMinY) * 2 - 1);
				strTmp.Format(_T("        %g %g %g,\n"), x, y, z);
				file.WriteString(strTmp);
			}

			pt = GetCornerPoint(0, j, k, 3);
			x = (pt.GetX() - m_dMinX) / (m_dMaxX - m_dMinX) * 2 - 1;
			y = -((pt.GetZ() - m_dMinZ) / (m_dMaxZ - m_dMinZ) * 2 - 1);				
			z = -((pt.GetY() - m_dMinY) / (m_dMaxY - m_dMinY) * 2 - 1);
			strTmp.Format(_T("        %g %g %g,\n"), x, y, z);
			file.WriteString(strTmp);
		}
		if (k == m_nGridZ - 1)
		{
			for (int j = 0; j < m_nGridY; j++)  //
			{
				if (j == 0)
				{
					pt = GetCornerPoint(0, j, k, 7);
					x = (pt.GetX() - m_dMinX) / (m_dMaxX - m_dMinX) * 2 - 1;
					y = -((pt.GetZ() - m_dMinZ) / (m_dMaxZ - m_dMinZ) * 2 - 1);
					z = -((pt.GetY() - m_dMinY) / (m_dMaxY - m_dMinY) * 2 - 1);
					strTmp.Format(_T("        %g %g %g,\n"), x, y, z);
					file.WriteString(strTmp);
				}

				pt = GetCornerPoint(0, j, k, 4);
				x = (pt.GetX() - m_dMinX) / (m_dMaxX - m_dMinX) * 2 - 1;
				y = -((pt.GetZ() - m_dMinZ) / (m_dMaxZ - m_dMinZ) * 2 - 1);
				z = -((pt.GetY() - m_dMinY) / (m_dMaxY - m_dMinY) * 2 - 1);
				if (j < m_nGridY - 1)
					strTmp.Format(_T("        %g %g %g,\n"), x, y, z);
				else
					strTmp.Format(_T("        %g %g %g]\n"), x, y, z);
				file.WriteString(strTmp);
			}
		}
	}

	file.WriteString(_T("        }\n"));
	file.WriteString(_T("        coordIndex [\n"));

	for (int k = 0; k < m_nGridZ; k++)
	{
		for (int j = 0; j < m_nGridY; j++)
		{
			strTmp.Format(_T("        %d, %d, %d, -1,\n"), k * (m_nGridY + 1) + j, (k + 1) * (m_nGridY + 1) + j, k * (m_nGridY + 1) + j + 1);
			file.WriteString(strTmp);

			if (k == m_nGridZ - 1 && j == m_nGridY - 1)
				strTmp.Format(_T("        %d, %d, %d, -1]\n"), k * (m_nGridY + 1) + j + 1, (k + 1) * (m_nGridY + 1) + j, (k + 1) * (m_nGridY + 1) + j + 1);
			else
				strTmp.Format(_T("        %d, %d, %d, -1,\n"), k * (m_nGridY + 1) + j + 1, (k + 1) * (m_nGridY + 1) + j, (k + 1) * (m_nGridY + 1) + j + 1);
			file.WriteString(strTmp);
		}
	}

	file.WriteString(_T("        }\n"));
	file.WriteString(_T("      }\n"));
	file.WriteString(_T("    ]\n"));
	file.WriteString(_T("  }\n"));
}

void CGridModel::SaveRight(CStdioFile& file)
{
	CString strTmp;
	CVertex3D pt;
	double x, y, z;

	//-----------------------------------------------------------------------
	// 右面
	file.WriteString(_T("DEF Mesh-ROOT Transform {\n"));
	file.WriteString(_T("  translation 0 0 0\n"));
	file.WriteString(_T("  rotation 0 0 0 0\n"));
	file.WriteString(_T("  scale 2 1.2 2\n"));
	file.WriteString(_T("  children [\n"));
	file.WriteString(_T("    Shape {\n"));
	file.WriteString(_T("      appearance Appearance {\n"));
	file.WriteString(_T("        material Material {\n"));
	file.WriteString(_T("          diffuseColor 0 0 0\n"));
	file.WriteString(_T("        }\n"));
	file.WriteString(_T("      }\n"));
	file.WriteString(_T("      geometry DEF Mesh-FACES IndexedFaceSet {\n"));
	file.WriteString(_T("        ccw TRUE\n"));
	file.WriteString(_T("        solid TRUE\n"));
	file.WriteString(_T("        coord DEF Mesh-COORD Coordinate { point [\n"));

	for (int k = 0; k < m_nGridZ; k++)
	{
		for (int j = 0; j < m_nGridY; j++)  //
		{
			if (j == 0)
			{
				pt = GetCornerPoint(m_nGridX - 1, j, k, 1);
				x = (pt.GetX() - m_dMinX) / (m_dMaxX - m_dMinX) * 2 - 1;
				y = -((pt.GetZ() - m_dMinZ) / (m_dMaxZ - m_dMinZ) * 2 - 1);					
				z = -((pt.GetY() - m_dMinY) / (m_dMaxY - m_dMinY) * 2 - 1);
				strTmp.Format(_T("        %g %g %g,\n"), x, y, z);
				file.WriteString(strTmp);
			}

			pt = GetCornerPoint(m_nGridX - 1, j, k, 2);
			x = (pt.GetX() - m_dMinX) / (m_dMaxX - m_dMinX) * 2 - 1;
			y = -((pt.GetZ() - m_dMinZ) / (m_dMaxZ - m_dMinZ) * 2 - 1);				
			z = -((pt.GetY() - m_dMinY) / (m_dMaxY - m_dMinY) * 2 - 1);
			strTmp.Format(_T("        %g %g %g,\n"), x, y, z);
			file.WriteString(strTmp);
		}
		if (k == m_nGridZ - 1)
		{
			for (int j = 0; j < m_nGridY; j++)  //
			{
				if (j == 0)
				{
					pt = GetCornerPoint(m_nGridX - 1, j, k, 5);
					x = (pt.GetX() - m_dMinX) / (m_dMaxX - m_dMinX) * 2 - 1;
					y = -((pt.GetZ() - m_dMinZ) / (m_dMaxZ - m_dMinZ) * 2 - 1);
					z = -((pt.GetY() - m_dMinY) / (m_dMaxY - m_dMinY) * 2 - 1);
					strTmp.Format(_T("        %g %g %g,\n"), x, y, z);
					file.WriteString(strTmp);
				}

				pt = GetCornerPoint(m_nGridX - 1, j, k, 6);
				x = (pt.GetX() - m_dMinX) / (m_dMaxX - m_dMinX) * 2 - 1;
				y = -((pt.GetZ() - m_dMinZ) / (m_dMaxZ - m_dMinZ) * 2 - 1);
				z = -((pt.GetY() - m_dMinY) / (m_dMaxY - m_dMinY) * 2 - 1);
				if (j < m_nGridY - 1)
					strTmp.Format(_T("        %g %g %g,\n"), x, y, z);
				else
					strTmp.Format(_T("        %g %g %g]\n"), x, y, z);
				file.WriteString(strTmp);
			}
		}
	}

	file.WriteString(_T("        }\n"));
	file.WriteString(_T("        coordIndex [\n"));

	for (int k = 0; k < m_nGridZ; k++)
	{
		for (int j = 0; j < m_nGridY; j++)
		{
			strTmp.Format(_T("        %d, %d, %d, -1,\n"), k * (m_nGridY + 1) + j, (k + 1) * (m_nGridY + 1) + j, k * (m_nGridY + 1) + j + 1);
			file.WriteString(strTmp);

			if (k == m_nGridZ - 1 && j == m_nGridY - 1)
				strTmp.Format(_T("        %d, %d, %d, -1]\n"), k * (m_nGridY + 1) + j + 1, (k + 1) * (m_nGridY + 1) + j, (k + 1) * (m_nGridY + 1) + j + 1);
			else
				strTmp.Format(_T("        %d, %d, %d, -1,\n"), k * (m_nGridY + 1) + j + 1, (k + 1) * (m_nGridY + 1) + j, (k + 1) * (m_nGridY + 1) + j + 1);
			file.WriteString(strTmp);
		}
	}

	file.WriteString(_T("        }\n"));
	file.WriteString(_T("      }\n"));
	file.WriteString(_T("    ]\n"));
	file.WriteString(_T("  }\n"));
}

BOOL CGridModel::SaveFile2Wrl(LPCTSTR lpFileName)
{
	try
	{
		CStdioFile file(lpFileName, CFile::modeWrite | CFile::modeCreate | CFile::typeText);

		file.WriteString(_T("#VRML V2.0 utf8\n"));
		file.WriteString(_T("\n"));
		file.WriteString(_T("# Produced by 3d Mesh 1.0\n"));
		file.WriteString(_T("\n"));
		file.WriteString(_T("# 网格模型\n"));

		SaveTop(file);
		SaveBottom(file);
		SaveFront(file);
		SaveBack(file);
		SaveLeft(file);
		SaveRight(file);		

		file.Close();
		return TRUE;
	}
	catch (CFileException* pe)
	{
		CString msg;
		msg.Format(_T("    文件打开错误: %s"), GetFileErrMsg(pe->m_cause));
		AfxMessageBox(msg);
		pe->Delete();
		return FALSE;
	}
	catch (...)
	{
		AfxMessageBox(_T("无法预料的错误!"));
		return FALSE;
	}
}