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

bool CIntersectSearchManager::SearchInterSect()
{
	CWaitCursor wait;
	if((m_gridFilename.empty())||(m_interlayers.empty()))
		return false;
	int index = 1;
	for(std::vector<CGLObject*>::iterator it=m_interlayers.begin(); it!=m_interlayers.end(); ++it)
	{
		CGLObject* face = (*it);
		SearchALayer(face, index++);
	}

	////д����ļ�
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
	m_interlayerNames.clear();
	m_interlayers.clear();
}

void CIntersectSearchManager::SearchALayer( CGLObject* gird, int index )
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
	m_interlayerNames.push_back(strNewFileName.GetBuffer());

	TRY 
	{	
		bool rs = Tracking(strNewtempFileName.GetBuffer(),m_gridFilename,strNewFileName.GetBuffer());
		if(!rs)
			return ;
		//�ཻ�������ʾ
		CString title;
		title.Format("%s%d","�в�����", index);
		CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
		pMF->GetTreeModelView()->OnImportInterlayer(strNewFileName, title, m_gridModelTreeItemGuid.c_str());

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

void CIntersectSearchManager::RunCommond( const std::string& command )
{

	WinExec(_T("f:\\release\\WindowsFormsApplication5.exe test"), SW_SHOW);
}

bool CIntersectSearchManager::AveragePara( const std::string& filename, const std::string& savename )
{
	bool IsTrue = false;
	IsTrue = Average(filename, savename);
	return IsTrue;
}
