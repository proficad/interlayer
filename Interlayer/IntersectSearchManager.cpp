#include "stdafx.h"
#include "IntersectSearchManager.h"
#include "3DLib/GridObject.h"
#include "3DLib/GLObject.h"
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
