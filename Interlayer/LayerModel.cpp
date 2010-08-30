// LayerModel.cpp : 实现文件
//

#include "stdafx.h"
#include "Interlayer.h"
#include "LayerModel.h"


// CLayerModel
IMPLEMENT_SERIAL(CLayerModel, CObject, VERSIONABLE_SCHEMA|0)

CLayerModel::CLayerModel()
{
	m_dDepthScale		= 0.5;			// 夹层投影模型深度
	m_dTop				= 0;			// 顶面深度
	m_dBottom			= 0;			// 底面深度
	
	CString strTmp;

	// 夹层间隔
	strTmp = AfxGetApp()->GetProfileString(_T("Settings"),_T("Layers"));
	m_dHorzSpace = atof(strTmp);

	// 法线间隔
	strTmp = AfxGetApp()->GetProfileString(_T("Settings"),_T("Normals"));
	m_dNormalSpace = atof(strTmp);

	// 纵向间隔
	strTmp = AfxGetApp()->GetProfileString(_T("Settings"),_T("Verts"));
	m_dVertSpace = atof(strTmp);
}

CLayerModel::~CLayerModel()
{
}


// CLayerModel 成员函数

void CLayerModel::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);

	ar.SerializeClass(RUNTIME_CLASS(CLayerModel));

	if (ar.IsStoring())
	{
		int nSize = m_layers.size();	// 复夹层线
		ar <<nSize;
		for (int i=0; i<nSize; i++)
		{
			int num = m_layers[i].size();
			ar << num;
			for (int j=0; j<num; j++)
			{
				ar << m_layers[i][j];
			}
		}

		nSize = m_singleLayers.size();	// 单夹层线
		ar <<nSize;
		for (int i=0; i<nSize; i++)
		{
			int num = m_singleLayers[i].size();
			ar << num;
			for (int j=0; j<num; j++)
			{
				ar << m_singleLayers[i][j];
			}
		}
		
		nSize = m_normals[0].size();
		ar << nSize;
		for (int i=0; i<nSize; i++)
		{
			ar << m_normals[0][i];
			ar << m_normals[1][i];
		}
				
		nSize = m_vertModel.size();
		ar << nSize;

		for (int i=0; i<nSize; i++)
		{
			int num = m_vertModel[i].size();
			ar << num;
			for (int j=0; j<num; j++)
			{
				ar << m_vertModel[i][j];
			}
			ar << m_dAngles[i];
			ar << m_vertTitle[i];					// 纵向模型标题
			ar << m_vertColor[i];					// 纵向模型颜色
		}

		nSize = m_layerMark.size();
		ar << nSize;
		for (int i=0; i<nSize; i++)
		{
			ar << m_layerMark[i];
		}

		nSize = m_topPlane.size();					// 顶面
		ar << nSize;
		for (int i=0; i<nSize; i++)
		{
			ar << m_topPlane[i];
		}

		nSize = m_bottomPlane.size();					// 底面
		ar << nSize;
		for (int i=0; i<nSize; i++)
		{
			ar << m_bottomPlane[i];
		}

		ar << m_dDepthScale;						// 夹层投影模型深度
		ar << m_dTop;								// 顶面深度
		ar << m_dBottom;							// 底面深度

		ar << m_dHorzSpace;							// 夹层间隔
		ar << m_dNormalSpace;						// 法线间隔
		ar << m_dVertSpace;							// 纵向间隔
	}
	else
	{
		int nVersion = ar.GetObjectSchema(); // 获取版本信息
		switch( nVersion )
		{
		case 0:
			{
				int nSize = 0;
				ar >> nSize;
				for (int i=0; i<nSize; i++)
				{	
					std::vector<CPoint2D> points;
					int num;
					ar >> num;
					for (int j=0; j<num; j++)
					{
						CPoint2D pt;
						ar >> pt;
						points.push_back(pt);
					}
					m_layers.push_back(points);
				}

				ar >> nSize;
				for (int i=0; i<nSize; i++)
				{	
					std::vector<CPoint2D> points;
					int num;
					ar >> num;
					for (int j=0; j<num; j++)
					{
						CPoint2D pt;
						ar >> pt;
						points.push_back(pt);
					}
					m_singleLayers.push_back(points);
				}
				
				ar >> nSize;
				for (int i=0; i<nSize; i++)
				{
					CPoint2D pt;
					ar >> pt;
					m_normals[0].push_back(pt);
					ar >> pt;
					m_normals[1].push_back(pt);
				}
				
				ar >> nSize;
				for (int i=0; i<nSize; i++)
				{
					std::vector<CPoint2D> points;
					int num;					
					ar >> num;
					for (int j=0; j<num; j++)
					{
						CPoint2D pt;
						ar >> pt;
						points.push_back(pt);
					}
					m_vertModel.push_back(points);
					
					double angle;
					ar >> angle;
					m_dAngles.push_back(angle);

					CString str;
					ar >> str;
					m_vertTitle.Add(str);					// 纵向模型标题
					
					COLORREF clr;
					ar >> clr;
					m_vertColor.push_back(clr);				// 纵向模型颜色
				}
				
				ar >> nSize;
				for (int i=0; i<nSize; i++)
				{
					CPoint2D pt;
					ar >> pt;
					m_layerMark.push_back(pt);
				}

				ar >> nSize;								// 顶面
				for (int i=0; i<nSize; i++)
				{
					tagPlane plane;
					ar >> plane;
					m_topPlane.push_back(plane);
				}
				
				ar >> nSize;								// 底面
				for (int i=0; i<nSize; i++)
				{
					tagPlane plane;
					ar >> plane;
					m_bottomPlane.push_back(plane);
				}

				ar >> m_dDepthScale;						// 夹层投影模型深度
				ar >> m_dTop;								// 顶面深度
				ar >> m_dBottom;							// 底面深度

				ar >> m_dHorzSpace;							// 夹层间隔
				ar >> m_dNormalSpace;						// 法线间隔
				ar >> m_dVertSpace;							// 纵向间隔
			}
			break;
		default:
			break;
		}
	}	
}

void CLayerModel::GeneratePlane(LPCTSTR lpszFileName, int index) // 生成面
{
	
}
