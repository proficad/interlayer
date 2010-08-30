#include "StdAfx.h"
#include "StLReader.h"

extern void StatusProgress(int nIndex, int nRange=-1);
extern void StatusSetProgress(int nIndex, int nPos);

CStLReader::CStLReader()
{
	fileName.Empty();
}

CStLReader::CStLReader(const CString& fName)
{
	fileName = fName;
	Read();
}

CStLReader::~CStLReader()
{
}

CStLReader* CStLReader::Copy()
{
	CStLReader* R = new CStLReader;
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

bool CStLReader::Read()
{
	if( fileName.IsEmpty() )
		return false;

	FILE* stlfp;
	int i = 0,j = 0,cnt = 0 ,pCnt = 4;
	char a[1024];
	char str[1024];
	double x = 0,y = 0,z = 0;
	CVertex3D tPoint;
	
	errno_t err;
	err  = fopen_s( &stlfp, fileName, "r" );
	if( err == 0 )	
		return false;
	else
	{
		StatusProgress(2,100);
		int num = 0;
		do
		{
			num ++;
			i = 0;
			cnt = 0;
			fgets(a, 1023, stlfp);
			while (a[i] != '\0')
			{
				if (!islower((int)a[i]) && !isupper((int)a[i]) && a[i] != ' ')
					break;
				cnt++;
				i++;
			}
			while (a[cnt] != '\0')
			{
				str[j] = a[cnt];
				cnt++;
				j++;
			}
			str[j] = '\0';
			j = 0;
			if (sscanf_s(str, "%lf%lf%lf", &x, &y, &z) == 3)
			{
				tPoint.SetParam(x, y, z);
				//pointList->push_back(tPoint);
			}
			pCnt++;

			StatusSetProgress(2, num);
			if( num >= 100 )
				num = 0;
		}
		while (!feof(stlfp));
		fclose(stlfp);

		StatusProgress(2);
		return true;
	}
}

void CStLReader::Serialize(CArchive& ar)
{
	CReader::Serialize(ar);
}