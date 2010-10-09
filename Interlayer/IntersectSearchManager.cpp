#include "stdafx.h"
#include "IntersectSearchManager.h"
#include "3DLib/GridObject.h"
#include "3DLib/GLObject.h"
#include "3DLib/3DObject.h"
#include "MainFrm.h"
#include "../ShapeDll/GridCellShape/GridCellShape.h"

#ifdef _DEBUG
	#pragma comment(lib,"../debug/GridCellShape.lib");
#else
	#pragma comment(lib,"../release/GridCellShape.lib");
#endif

CIntersectSearchManager::CIntersectSearchManager()
{
	m_model = NULL;
	m_interlayers.clear();
}


CIntersectSearchManager::~CIntersectSearchManager()
{
	m_interlayers.clear();
}

void CIntersectSearchManager::AddLayerModel( CGLObject* layer )
{
	m_interlayers.push_back(layer);
}
CGridObject* CIntersectSearchManager::GetGrid()
{
	return m_model;
}

int CIntersectSearchManager::GetLayerNumber()
{
	return m_interlayers.size();
}

CGLObject* CIntersectSearchManager::GetLayer( int index )
{
	if((m_interlayers.size()<index)||(m_interlayers.size()==0))
		return NULL;
	else 
		return m_interlayers[index];
}

CGLObject* CIntersectSearchManager::GetLayer( const std::string& name )
{
	std::vector<CGLObject*>::iterator itbegin = m_interlayers.begin();
	std::vector<CGLObject*>::iterator itend = m_interlayers.end();
	std::vector<CGLObject*>::iterator it;
	CGLObject*	layer;
	for(it=itbegin; it!=itend; ++it)
	{
		layer = (*it);
		if(StrCmp(layer->GetObjName(),name.c_str())==0)
		{
			return layer;
		}
	}
	return NULL;
}

void CIntersectSearchManager::ReleaseAll()
{
	
}

bool CIntersectSearchManager::LoadGridModel( const std::string& filename,const std::string& outfilename )
{
	return LoadEclipseFile(filename, outfilename);
}

bool CIntersectSearchManager::SearchInterSect()
{
	if((m_model==NULL)||(m_interlayers.empty()))
		return false;
	for(std::vector<CGLObject*>::iterator it=m_interlayers.begin(); it!=m_interlayers.end(); ++it)
	{
		CGLObject* face = (*it);
		SearchALayer(face);
	}
}

void CIntersectSearchManager::SearchALayer( CGLObject* gird )
{
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	CString strFileName = pMF->GetProjectDatPath();
	strFileName += _T("\\models\\");
	CString strNewtempFileName;
	strNewtempFileName = strFileName + newGUID();
	CString strNewFileName;
	strNewFileName = strFileName + newGUID();

	C3DObject* obj3d = dynamic_cast<C3DObject*>(gird);
	if(!obj3d)
		return;
	obj3d->SaveSurface(strNewtempFileName.GetString());

	bool rs = Tracking(strNewtempFileName.GetString(),m_gridFilename,strNewFileName.GetString());

	TRY
	{
		CFile::Remove( strNewtempFileName.GetBuffer() );
	}
	CATCH( CFileException, e )
	{
		assert(false&&"ɾ���ļ�ʧ��");
	}
	END_CATCH 
}
