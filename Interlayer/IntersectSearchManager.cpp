#include "stdafx.h"
#include "IntersectSearchManager.h"
#include "3DLib/GridObject.h"
#include "3DLib/GLObject.h"
#include "3DLib/3DObject.h"
#include "MainFrm.h"
#include "3dlib/3DModelDoc.h"
#include "3dlib/3DModelView.h"
#include "../ShapeDll/GridCellShape/GridCellShape.h"

#ifdef _DEBUG
	#pragma comment(lib,"../debug/GridCellShape.lib");
#else
	#pragma comment(lib,"../release/GridCellShape.lib");
#endif

//using namespace System;

CIntersectSearchManager::CIntersectSearchManager()
{
	m_model = NULL;
	m_phyparaMin = -1.7e+308;
	m_phyparaMax = 1.7e+308;
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

UINT CIntersectSearchManager::SearchInterSect(LPVOID pParam)
{
	CModelView* modelTree = (CModelView*)(pParam);
	CWaitCursor wait;
	if((m_gridFilename.empty())||(m_interlayers.empty()))
		return false;
	int index = 1;
	SetGridFileName(m_gridFilename);
	for(std::vector<CGLObject*>::iterator it=m_interlayers.begin(); it!=m_interlayers.end(); ++it)
	{
		CGLObject* face = (*it);
		SearchALayer(face);
		delete face;
	}

	if(RunTrack())
	{
		for(int i=0; i<m_interlayerNames.size(); i++)
		{
			CString title;
			title.Format("%s%d","夹层网格", index++);
			//CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
			modelTree->OnImportInterlayer(m_interlayerNames[i].c_str(), title, m_gridModelTreeItemGuid.c_str());
		}
	}
	////写点坝文件
	//CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	//CString strFileName = pMF->GetProjectDatPath();
	//strFileName += _T("\\models\\");
	//CString strNewtempFileName;
	//strNewtempFileName = strFileName + newGUID() + ".pointbar";

	//CFile file(strNewtempFileName.GetBuffer(), CFile::modeWrite|CFile::typeBinary|CFile::modeCreate);
	//CArchive art(&file, CArchive::store);
	//art << m_interlayerNames.size();
	//for(int i=0; i<m_interlayerNames.size(); i++)
	//{
	//	art << (m_interlayerNames[i]).length();
	//	for(int j=0; j<(m_interlayerNames[i]).length(); j++)
	//		art <<m_interlayerNames[i].c_str()[j];
	//}
	//art.Close();
	for(int i=0; i<m_tempTriangleFiles.size(); i++)
	{
		TRY
		{
			CFile::Remove( m_tempTriangleFiles[i].c_str() );
		}
		CATCH( CFileException, e )
		{
			assert(false&&"删除文件失败");
		}
		END_CATCH 
	}
	m_tempTriangleFiles.clear();
	m_interlayerNames.clear();
	m_interlayers.clear();
	return 0;
}

void CIntersectSearchManager::SearchALayer( CGLObject* gird )
{
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	CString strFileName = m_pathname;
	CString strNewtempFileName;
	strNewtempFileName = strFileName + newGUID();
	CString strNewFileName = newGUID();

	C3DObject* obj3d = dynamic_cast<C3DObject*>(gird);
	if(!obj3d)
		return;
	obj3d->SaveSurface(strNewtempFileName.GetString());
	m_interlayerNames.push_back(strNewFileName.GetBuffer());

	strNewFileName = strFileName + strNewFileName;

	TRY 
	{	
		//bool rs = Tracking(strNewtempFileName.GetBuffer(),m_gridFilename,strNewFileName.GetBuffer());
		bool rs = AddItem(strNewtempFileName.GetBuffer(), strNewFileName.GetBuffer());
		if(!rs)
			return ;
		//相交网格的显示

		//CMDIChildWndEx *pWnd =(CMDIChildWndEx *) pMF->MDIGetActive();
		//if( pWnd )
		//{
		//	//C3DModelView *pView = (C3DModelView *)pWnd->GetActiveView();
		//	//if(pView)
		//	//{
		//	//	if(pView->IsKindOf(RUNTIME_CLASS(C3DModelView)))
		//	//	{
		//	//		C3DModelDoc *pDoc = (C3DModelDoc *)pView ->GetDocument();
		//	//		pDoc->AddInterlayer(strNewFileName.GetBuffer(),strNewFileName.GetBuffer());
		//	//	}
		//	}
		//}	

		//pMF->get3d
	}
	CATCH (CMemoryException, e)
	{
		
	}
	END_CATCH

	m_tempTriangleFiles.push_back(strNewtempFileName.GetBuffer());
	//TRY
	//{
	//	CFile::Remove( strNewtempFileName.GetBuffer() );
	//}
	//CATCH( CFileException, e )
	//{
	//	assert(false&&"删除文件失败");
	//}
	//END_CATCH 
		
}

bool CIntersectSearchManager::ExportTriangle( const std::string& file, const std::string& out, int size, int index )
{
	bool rl = false;
	rl = cExportTriangle(file, out, size, index);
	return rl;
}

bool CIntersectSearchManager::ComputePara2( const std::string& file1, const std::string& file2, const std::string& savefile, const std::string& op1, const std::string& op2, const std::string& op, float para1, float para2 )
{
	bool rl = false;
	rl = Operation2(file1, file2, savefile, op1, op2, op, para1, para2);
	return rl;
}

bool CIntersectSearchManager::ComputePara1( const std::string& file, float para, const std::string& savefile, const std::string& op )
{
	bool rl = false;
	rl = Operation1(file, para, savefile,op);
	return rl;
}

void CIntersectSearchManager::RunCommond( const std::string& filename )
{
	RunExeDotNet(filename);
}

bool CIntersectSearchManager::AveragePara( const std::string& filename, const std::string& savename, int type )
{
	bool IsTrue = false;
	switch(type)
	{
	case 0://ADJ_TYPE_VOLUME:
		IsTrue = Average(filename, savename);
		break;
	case 1://ADJ_TYPE_Z:
		IsTrue = AverageZ(filename, savename);
		break;
	default:
		break;
	}

	return IsTrue;
}

bool CExportManager::SetExportGridFileName(const std::string& filename)
{
	bool IsTrue = false;
	IsTrue = SetGridFileNameNet(filename);
	return IsTrue;
}

bool CExportManager::AddProName( const std::string& filename )
{
	bool IsTrue = false;
	IsTrue = AddPropertyFileNameNet(filename);
	return IsTrue;
}

bool CExportManager::ClearProNames()
{
	bool IsTrue = false;
	IsTrue = ClearPropertyFileNameNet();
	return IsTrue;
}

int CExportManager::GetProNameNums()
{
	return  GetPropertyFileNameCountNet();
}

bool CExportManager::RemoveProNameAt( int index )
{
	bool IsTrue = false;
	IsTrue = PropertyFileNameRemoveAtNet(index);
	return IsTrue;
}

bool CExportManager::AddTrackName( const std::string& filename )
{
	bool IsTrue = false;
	IsTrue = AddTrackFileNameNet(filename);
	return IsTrue;
}

bool CExportManager::ClearTrackNames()
{
	bool IsTrue = false;
	IsTrue = ClearTrackFileNameNet();
	return IsTrue;
}

int CExportManager::GetTrackNameNums()
{
	return  GetTrackFileNameCountNet();
}

bool CExportManager::RemoveTrackNameAt( int index )
{
	bool IsTrue = false;
	IsTrue = TrackFileNameRemoveAtNet(index);
	return IsTrue;
}

bool CExportManager::WriteExport( const std::string& filename )
{
	bool IsTrue = false;
	IsTrue = WriteNet(filename);
	return IsTrue;
}

bool CExportManager::WriteCenterPoints( const std::string& filename, const std::string& saveFileName, bool isWriteTick, int number )
{
	bool IsTrue = false;
	IsTrue = WriteCenterPointNet(filename, saveFileName, isWriteTick, number);
	return IsTrue;
}

bool CExportManager::SetRedefine( bool isRedefine )
{
	bool IsTrue = false;
	IsTrue = SetRedefineNet(isRedefine);
	return IsTrue;
}

bool CExportManager::SetTransRate(bool isWrite)
{
	bool IsTrue = false;
	IsTrue = SetTransRateNet(isWrite);
	return IsTrue;
}

bool CExportManager::SetWriteProperty( bool isWrite )
{
	bool IsTrue = false;
	IsTrue = SetWritePropertyNet(isWrite);
	return IsTrue;
}

bool CExportManager::SetRedefineProperty( bool isWrite )
{
	bool IsTrue = false;
	IsTrue = SetRedefinePropertyNet(isWrite);
	return IsTrue;
}

bool CExportManager::SetWriteChangeProperty( bool isWrite )
{
	bool IsTrue = false;
	IsTrue = SetWriteChangePropertyNet(isWrite);
	return IsTrue;
}

bool CExportManager::SetWriteRedefineMult( bool isWrite )
{
	bool IsTrue = false;
	IsTrue = SetWriteRedefineMultNet(isWrite);
	return IsTrue;
}

bool CExportManager::SetWriteRedefineChangeProperty( bool isWrite )
{
	bool IsTrue = false;
	IsTrue = SetWriteRedefineChangePropertyNet(isWrite);
	return IsTrue;
}

bool CExportManager::SetChangePropertyValue( double mValue )
{
	bool IsTrue = false;
	IsTrue = SetChangePropertyValueNet(mValue);
	return IsTrue;
}

bool CExportManager::SetChangePropertyName( const std::string& name )
{
	bool IsTrue = false;
	IsTrue = SetChangePropertyNameNet(name);
	return IsTrue;
}