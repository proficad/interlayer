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
	// �ܵ��� = 4*n
	// ��һ��Ϊ������
	// ����Ϊƽ�������
	std::vector<CVertex3D> m_pointList;
	std::vector<int> m_indexs;
	CString fileName;
};