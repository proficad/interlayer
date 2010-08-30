#pragma once
#include <vector>

// CLayerModel 命令目标
struct tagPlane
{
	std::vector<CPoint2D>	m_points;		// 点集
	std::vector<int>		m_indexs;		// 点索引
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
	std::vector<std::vector<CPoint2D>> m_layers;		// 复夹层线
	std::vector<std::vector<CPoint2D>> m_singleLayers;	// 单夹层线
	
	std::vector<CPoint2D>	m_normals[2];				// 法线
	
	std::vector<std::vector<CPoint2D>> m_vertModel;		// 纵向模型
	std::vector<double> m_dAngles;						// 纵向模型倾斜角度
	CStringArray m_vertTitle;							// 纵向模型标题
	std::vector<COLORREF> m_vertColor;					// 纵向模型颜色

	std::vector<CPoint2D> m_layerMark;					// 夹层标志

	std::vector<tagPlane> m_topPlane;					// 顶面
	std::vector<tagPlane> m_bottomPlane;				// 底面

	double m_dDepthScale;								// 夹层投影模型深度比例
	double m_dTop;										// 顶面深度
	double m_dBottom;									// 底面深度

	double m_dHorzSpace;								// 夹层间隔
	double m_dNormalSpace;								// 法线间隔
	double m_dVertSpace;								// 纵向间隔
	
public:
	void GeneratePlane(LPCTSTR lpszFileName, int index); // 生成面
};


