#include "StdAfx.h"
#include "Reader.h"

CReader::CReader(void)
{	
}

CReader::~CReader(void)
{	
}

CReader* CReader::Copy()
{
	CReader *R = new CReader;
	
	if( m_pointList.empty() )
	{
		R->m_pointList.resize(m_pointList.size());
		std::copy(m_pointList.begin(), m_pointList.end(), R->m_pointList.begin());

		R->m_indexs.resize(m_indexs.size());
		std::copy(m_indexs.begin(), m_indexs.end(), R->m_indexs.begin());		
	}

	return R;
}

void CReader::Serialize(CArchive& ar)
{
	if( ar.IsStoring() )
	{
		int size = m_pointList.size();
		ar << size;

		for (int i= 0; i < size; i++)
			 ar << m_pointList[i];

		size = m_indexs.size();
		ar << size;

		for (int i=0; i<size; i++)
			ar << m_indexs[i];
	}
	else
	{
		m_pointList.clear();
		m_indexs.clear();

		int size;
		ar >> size;

		for (int i=0; i<size; i++)
		{
			CVertex3D point;
			ar >> point;
			m_pointList.push_back(point);
		}

		ar >> size;

		for (int i=0; i<size; i++)
		{
			int index;
			ar >> index;
			m_indexs.push_back(index);
		}
	}
}
