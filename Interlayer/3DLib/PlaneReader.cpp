#include "StdAfx.h"
#include "PlaneReader.h"
#include "Vertex3D.h"
#include "Transform.h"
#include "Utils3d.h"

extern void StatusProgress(int nIndex, int nRange=-1);
extern void StatusSetProgress(int nIndex, int nPos);

CPlaneReader::CPlaneReader(void)
{
	fileName.Empty();
}

CPlaneReader::CPlaneReader(const CString& fName)
{
	fileName = fName;
	Read();
}

CPlaneReader::~CPlaneReader(void)
{
}

CPlaneReader* CPlaneReader::Copy()
{
	CPlaneReader* R = new CPlaneReader;
	R->fileName = fileName;
	if( m_pointList.empty() )
	{
		R->m_pointList.resize(m_pointList.size());
		std::copy(m_pointList.begin(), m_pointList.end(), R->m_pointList.begin());

		R->m_indexs.resize(m_indexs.size());
		std::copy(m_indexs.begin(), m_indexs.end(), R->m_indexs.begin());
	}

	return R;
}

void CPlaneReader::Serialize(CArchive& ar)
{
	CReader::Serialize(ar);
}

bool CPlaneReader::Read()
{
	if( fileName.IsEmpty() )
		return false;

	CFile file(fileName, CFile::modeRead|CFile::typeBinary);
	CArchive ar(&file, CArchive::load);

	/*int nRows;
	int nCols;

	ar >> nRows;
	ar >> nCols;

	CString strTmp[4];
	
	double m_dMinX = DBL_MAX, m_dMaxX = -DBL_MAX;
	double m_dMinY = DBL_MAX, m_dMaxY = -DBL_MAX;
	double m_dMinZ = DBL_MAX, m_dMaxZ = -DBL_MAX;
	
	BOOL bIsVertex = TRUE;

	int nPoints = 0;

	//StatusProgress(2,nRows-1);

	for (int i=0;i<nRows;i++)
	{
		//StatusSetProgress(2,i);
		ar >> strTmp[0];
		ar >> strTmp[1];
		ar >> strTmp[2];
		ar >> strTmp[3];
		if( strTmp[0] == _T("Coordinate"))
		{
			bIsVertex = TRUE;
			continue;
		}

		if( strTmp[0] == _T("CoordIndex"))
		{
			bIsVertex = FALSE;
			continue;
		}

		if( bIsVertex )
		{
			CVertex3D point;
			point.x = atof(strTmp[0]);
			point.y = atof(strTmp[1]);
			point.z = atof(strTmp[2]);

			m_pointList.push_back(point);

			if( m_dMinX > point.x )
				m_dMinX = point.x;
			if( m_dMinY > point.y )
				m_dMinY = point.y;
			if( m_dMinZ > point.z )
				m_dMinZ = point.z;
			if( m_dMaxX < point.x )
				m_dMaxX = point.x;
			if( m_dMaxY < point.y )
				m_dMaxY = point.y;
			if( m_dMaxZ < point.z )
				m_dMaxZ = point.z;

			nPoints ++;
		}
		else
		{
			m_indexs.push_back(atoi(strTmp[0]));
			m_indexs.push_back(atoi(strTmp[1]));
			m_indexs.push_back(atoi(strTmp[2]));
		}
	}

	//StatusProgress(2);
*/




	int nRows1, nRows2, nCols;

	ar >> nRows1;
	ar >> nRows2;
	ar >> nCols;
	

	double m_dMinX = DBL_MAX, m_dMaxX = -DBL_MAX;
	double m_dMinY = DBL_MAX, m_dMaxY = -DBL_MAX;
	double m_dMinZ = DBL_MAX, m_dMaxZ = -DBL_MAX;
	
	//StatusProgress(2,nRows-1);

	double dTmp;
	
	if( m_pointList.capacity()<nRows1)
		m_pointList.reserve(nRows1);

	for (int i=0;i<nRows1/4;i++)
	{
		CVertex3D point;
		ar >> point.x;
		ar >> point.y;
		ar >> point.z;
		ar >> dTmp;

		m_pointList.push_back(point);

		if( m_dMinX > point.x )
			m_dMinX = point.x;
		if( m_dMinY > point.y )
			m_dMinY = point.y;
		if( m_dMinZ > point.z )
			m_dMinZ = point.z;
		if( m_dMaxX < point.x )
			m_dMaxX = point.x;
		if( m_dMaxY < point.y )
			m_dMaxY = point.y;
		if( m_dMaxZ < point.z )
			m_dMaxZ = point.z;

	}

	for (int i=0;i<nRows2/4;i++)
	{
		int iTmp;
		ar >> iTmp;
		m_indexs.push_back(iTmp);
		ar >> iTmp;
		m_indexs.push_back(iTmp);
		ar >> iTmp;
		m_indexs.push_back(iTmp);

		ar >> iTmp;
	}
	//StatusProgress(2);

	ar.Close();
	file.Close();

	return true;
}
