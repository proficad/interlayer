#pragma once
#include <vector>

// CLayerModel ����Ŀ��
struct tagPlane
{
	std::vector<CPoint2D>	m_points;		// �㼯
	std::vector<int>		m_indexs;		// ������
	friend CArchive& AFXAPI operator<<(CArchive& ar, const tagPlane& plane)
	{
		int nSize = plane.m_points.size();
		ar << nSize;
		for (int i=0; i<nSize; i++)
		{
			ar << plane.m_points[i];
		}

		nSize = plane.m_indexs.size();
		ar << nSize;
		for (int i=0; i<nSize; i++)
		{
			ar << plane.m_indexs[i];
		}

		return ar;
	}

	friend CArchive& AFXAPI operator>>(CArchive& ar, tagPlane& plane)
	{
		int nSize;
		ar >> nSize;
		for (int i=0; i<nSize; i++)
		{
			CPoint2D pt;
			ar >> pt;
			plane.m_points.push_back(pt);
		}

		ar >> nSize;
		for (int i=0; i<nSize; i++)
		{
			int iTmp;
			ar >> iTmp;
			plane.m_indexs.push_back(iTmp);
		}
		return ar;
	}

	const tagPlane& operator=(const tagPlane& plane)
	{
		int nSize = plane.m_points.size();
		
		m_points.resize(nSize);
		std::copy(plane.m_points.begin(), plane.m_points.end(), m_points.begin());
		
		nSize = plane.m_indexs.size();
		
		m_indexs.resize(nSize);
		std::copy(plane.m_indexs.begin(), plane.m_indexs.end(), m_indexs.begin());

		return *this;
	}
};
//________________________________________________________________________
//
class CLayerModel : public CObject
{
	DECLARE_SERIAL(CLayerModel);
public:
	CLayerModel();
	virtual ~CLayerModel();
	virtual void Serialize(CArchive& ar);
public:
	std::vector<std::vector<CPoint2D>> m_layers;		// ���в���
	std::vector<std::vector<CPoint2D>> m_singleLayers;	// ���в���
	
	std::vector<CPoint2D>	m_normals[2];				// ����
	
	std::vector<std::vector<CPoint2D>> m_vertModel;		// ����ģ��
	std::vector<double> m_dAngles;						// ����ģ����б�Ƕ�
	CStringArray m_vertTitle;							// ����ģ�ͱ���
	std::vector<COLORREF> m_vertColor;					// ����ģ����ɫ

	std::vector<CPoint2D> m_layerMark;					// �в��־

	std::vector<tagPlane> m_topPlane;					// ����
	std::vector<tagPlane> m_bottomPlane;				// ����

	double m_dDepthScale;								// �в�ͶӰģ����ȱ���
	double m_dTop;										// �������
	double m_dBottom;									// �������

	double m_dHorzSpace;								// �в���
	double m_dNormalSpace;								// ���߼��
	double m_dVertSpace;								// ������
	
public:
	void GeneratePlane(LPCTSTR lpszFileName, int index); // ������
};


