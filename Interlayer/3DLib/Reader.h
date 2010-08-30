#pragma once

#include <vector>
#include "Vertex3D.h"

class CReader
{
public:
	CReader(void);
	~CReader(void);
	virtual CReader* Copy();
	virtual void Serialize(CArchive& ar);
public:
	// 总点数 = 4*n
	// 第一点为法向量
	// 三点为平面的三点
	std::vector<CVertex3D> m_pointList;
	std::vector<int> m_indexs;
	CString fileName;
};