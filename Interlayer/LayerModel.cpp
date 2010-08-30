// LayerModel.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Interlayer.h"
#include "LayerModel.h"


// CLayerModel
IMPLEMENT_SERIAL(CLayerModel, CObject, VERSIONABLE_SCHEMA|0)

CLayerModel::CLayerModel()
{
	m_dDepthScale		= 0.5;			// �в�ͶӰģ�����
	m_dTop				= 0;			// �������
	m_dBottom			= 0;			// �������
	
	CString strTmp;

	// �в���
	strTmp = AfxGetApp()->GetProfileString(_T("Settings"),_T("Layers"));
	m_dHorzSpace = atof(strTmp);

	// ���߼��
	strTmp = AfxGetApp()->GetProfileString(_T("Settings"),_T("Normals"));
	m_dNormalSpace = atof(strTmp);

	// ������
	strTmp = AfxGetApp()->GetProfileString(_T("Settings"),_T("Verts"));
	m_dVertSpace = atof(strTmp);
}

CLayerModel::~CLayerModel()
{
}


// CLayerModel ��Ա����

void CLayerModel::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);

	ar.SerializeClass(RUNTIME_CLASS(CLayerModel));

	if (ar.IsStoring())
	{
		int nSize = m_layers.size();	// ���в���
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

		nSize = m_singleLayers.size();	// ���в���
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
			ar << m_vertTitle[i];					// ����ģ�ͱ���
			ar << m_vertColor[i];					// ����ģ����ɫ
		}

		nSize = m_layerMark.size();
		ar << nSize;
		for (int i=0; i<nSize; i++)
		{
			ar << m_layerMark[i];
		}

		nSize = m_topPlane.size();					// ����
		ar << nSize;
		for (int i=0; i<nSize; i++)
		{
			ar << m_topPlane[i];
		}

		nSize = m_bottomPlane.size();					// ����
		ar << nSize;
		for (int i=0; i<nSize; i++)
		{
			ar << m_bottomPlane[i];
		}

		ar << m_dDepthScale;						// �в�ͶӰģ�����
		ar << m_dTop;								// �������
		ar << m_dBottom;							// �������

		ar << m_dHorzSpace;							// �в���
		ar << m_dNormalSpace;						// ���߼��
		ar << m_dVertSpace;							// ������
	}
	else
	{
		int nVersion = ar.GetObjectSchema(); // ��ȡ�汾��Ϣ
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
					m_vertTitle.Add(str);					// ����ģ�ͱ���
					
					COLORREF clr;
					ar >> clr;
					m_vertColor.push_back(clr);				// ����ģ����ɫ
				}
				
				ar >> nSize;
				for (int i=0; i<nSize; i++)
				{
					CPoint2D pt;
					ar >> pt;
					m_layerMark.push_back(pt);
				}

				ar >> nSize;								// ����
				for (int i=0; i<nSize; i++)
				{
					tagPlane plane;
					ar >> plane;
					m_topPlane.push_back(plane);
				}
				
				ar >> nSize;								// ����
				for (int i=0; i<nSize; i++)
				{
					tagPlane plane;
					ar >> plane;
					m_bottomPlane.push_back(plane);
				}

				ar >> m_dDepthScale;						// �в�ͶӰģ�����
				ar >> m_dTop;								// �������
				ar >> m_dBottom;							// �������

				ar >> m_dHorzSpace;							// �в���
				ar >> m_dNormalSpace;						// ���߼��
				ar >> m_dVertSpace;							// ������
			}
			break;
		default:
			break;
		}
	}	
}

void CLayerModel::GeneratePlane(LPCTSTR lpszFileName, int index) // ������
{
	
}
