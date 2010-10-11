// 3dlib\3DModelDoc.cpp : 实现文件
//

#include "stdafx.h"
#include "3DModelDoc.h"
#include "../MainFrm.h"
#include "../Resource.h"
#include "3DObject.h"
#include "StLReader.h"
#include "WrlReader.h"
#include "PlaneReader.h"
#include "GridObject.h"
#include "../GridModel.h"
#include "../FieldCell.h"
#include "PipeLine.h"
#include "../IntersectSearchManager.h"

using namespace  GridModel;

typedef CTypedPtrArray<CObArray, CFieldCell*> FIELD_ROW;
// C3DModelDoc

extern void StatusBarMessage(char* fmt, ...);
extern void StatusProgress(int nIndex, int nRange=-1);
extern void StatusSetProgress(int nIndex, int nPos);

IMPLEMENT_DYNCREATE(C3DModelDoc, CDocument)

C3DModelDoc::C3DModelDoc()
{
	m_pDisplayContext = new CGLDisplayContext(this);
}

BOOL C3DModelDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

C3DModelDoc::~C3DModelDoc()
{
	delete m_pDisplayContext;
}


BEGIN_MESSAGE_MAP(C3DModelDoc, CDocument)
	ON_COMMAND(ID_3DOBJ_ERASE, On3DObjErase)
	ON_COMMAND(ID_3DOBJ_DELETE, On3DObjDelete)
	ON_COMMAND(ID_SCREEN_DISPLAYALL, OnScreenDisplayall)
END_MESSAGE_MAP()


// C3DModelDoc 诊断

#ifdef _DEBUG
void C3DModelDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void C3DModelDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// C3DModelDoc 序列化

void C3DModelDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}

	m_pDisplayContext->Serialize(ar);
}
#endif


// C3DModelDoc 命令

void C3DModelDoc::SetTitle(LPCTSTR lpszTitle)
{
	// TODO: 在此添加专用代码和/或调用基类

	CString strFileName = GetPathName();
	strFileName.MakeUpper();

	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();

	CString strPath = pFrame->GetProjectDatPath();
	strPath += _T("\\graphics\\");
	strPath.MakeUpper();

	strFileName.Replace(strPath, _T(""));

	HTREEITEM hItem = pFrame->GetTreeGraphView()->GetTreeCtrl()->IsExistFileInTree(TVI_ROOT, strFileName);

	CString strTitle = pFrame->GetTreeGraphView()->GetTreeCtrl()->GetItemText(hItem);

	m_strTitle = strTitle;

	UpdateFrameCounts();
}

BOOL C3DModelDoc::SaveModified()
{
	// TODO: 在此添加专用代码和/或调用基类

	if (!IsModified())
		return TRUE;        // ok to continue

	// get name/title of document
	CString name = _T(" [");
	name += m_strTitle;
	name += _T("] ");

	CString prompt;
	AfxFormatString1(prompt, AFX_IDP_ASK_TO_SAVE, name);
	switch (AfxMessageBox(prompt, MB_YESNOCANCEL, AFX_IDP_ASK_TO_SAVE))
	{
	case IDCANCEL:
		return FALSE;       // don't continue

	case IDYES:
		// If so, either Save or Update, as appropriate
		if (!DoFileSave())
			return FALSE;       // don't continue
		break;

	case IDNO:
		// If not saving changes, revert the document
		break;

	default:
		ASSERT(FALSE);
		break;
	}
	return TRUE;    // keep going
}

void C3DModelDoc::On3DObjErase()
{
	// TODO: Add your command handler code here
	m_pDisplayContext->EraseSelected();
}

void C3DModelDoc::On3DObjDelete()
{
	// TODO: Add your command handler code here
	m_pDisplayContext->DeleteSelected();
}

void C3DModelDoc::OnScreenDisplayall()
{
	// TODO: Add your command handler code here
	m_pDisplayContext->DisplayAll();
}

void C3DModelDoc::ImportFile(LPCTSTR lpszFileName)
{	
	CString strFileName = lpszFileName;
	CString ext = strFileName.Right(strFileName.GetLength()-strFileName.ReverseFind(_T('.')));
	ext.MakeUpper();

	CString strTitle = strFileName.Right(strFileName.GetLength()-strFileName.ReverseFind(_T('\\')));

	if (ext == _T(".STL"))
	{
		CStLReader* pReader = new CStLReader(strFileName);
		C3DObject* pStlObj = new C3DObject(pReader);
		pStlObj->SetContext(m_pDisplayContext);
		delete pReader;
		pStlObj->SetGLObjType(GLPLANE);
		pStlObj->SetObjName(strTitle);
		m_pDisplayContext->AddGLObj(pStlObj);
		//delete pStlObj;
	}
	else if (ext == _T(".WRL"))
	{
		CWrlReader* pReader = new CWrlReader(strFileName);
		C3DObject* pWrlObj = new C3DObject(pReader);
		pWrlObj->SetContext(m_pDisplayContext);
		delete pReader;
		pWrlObj->SetGLObjType(GLPLANE);
		pWrlObj->SetObjName(strTitle);
		m_pDisplayContext->AddGLObj(pWrlObj);
		//delete pWrlObj;
	}
	else
	{
		MessageBox(0, _T("File Format Not Recognized!!"), _T("提示信息"), MB_OK | MB_ICONERROR);
		return;
	}

	UpdateAllViews(NULL);
}

void C3DModelDoc::AddPlane(LPCTSTR lpszFileName, LPCTSTR lpszName)
{
	CPlaneReader* pReader = new CPlaneReader(lpszFileName);
	C3DObject* pPlaneObj = new C3DObject(pReader);
	//pPlaneObj->ReversePoints();
	pPlaneObj->SetContext(m_pDisplayContext);
	delete pReader;
	pPlaneObj->SetGLObjType(GLPLANE);
	pPlaneObj->SetObjName(lpszName);
	m_pDisplayContext->AddGLObj(pPlaneObj);
	//delete pPlaneObj;
	
	UpdateAllViews(NULL);
}

void C3DModelDoc::AddGridModel(LPCTSTR lpszFileName, LPCTSTR lpszName, const CStringArray &arParamName, const CStringArray &arFileName)
{
	CWaitCursor wait;

	StatusBarMessage(_T("正在读取数据，请稍等..."));

	StatusProgress(1,arParamName.GetSize()+1);

	int n = 0;
	
	CFile gridfile;
	CGridModel gridModel;
	if( gridfile.Open(lpszFileName, CFile::modeRead | CFile::typeBinary ) )
	{
		CIntersectSearchManager::Instance()->SetGridModelName(lpszFileName);
		CArchive ar(&gridfile,CArchive::load);
		gridModel.Serialize(ar);
		ar.Close();
		gridfile.Close();
		n ++;
	}
	else
	{
		StatusProgress(1);
		return;
	}

	CGridObject* pGridObj = new CGridObject;
	pGridObj->SetContext(m_pDisplayContext);
	pGridObj->SetGLObjType(GLSURFACE);
	pGridObj->SetObjName(lpszName);

	pGridObj->I = gridModel.m_nGridX;
	pGridObj->J = gridModel.m_nGridY;
	pGridObj->K = gridModel.m_nGridZ;

	pGridObj->m_gridCells = gridModel.m_gridCells;
	//pGridObj->m_gridRedine = gridModel.m_gridRedine;
	////pGridObj->K = 2;
	////pGridObj->m_pointList.SetSize(pGridObj->I*pGridObj->J*pGridObj->K*8);
	//if( pGridObj->m_pointList.capacity()<pGridObj->I*pGridObj->J*pGridObj->K*8 )
	//	pGridObj->m_pointList.reserve(pGridObj->I*pGridObj->J*pGridObj->K*8);
	//int nn = 0;
	//for (int k = 0; k<gridModel.m_nGridZ; k ++)
	////for (int k = 0; k<2; k ++)
	//{
	//	for(int j=0;j<gridModel.m_nGridY;j++)
	//	{
	//		for (int i=0;i<gridModel.m_nGridX;i++)
	//		{
	//			for (int m= 0; m < 8; m++)
	//			{
	//				CVertex3D vert = gridModel.GetCornerPoint(i,j,k,m);
	//				vert.z *= -1;

	//				//pGridObj->m_pointList[nn] = vert;
	//				pGridObj->m_pointList.push_back(vert);
	//				nn++;
	//			}
	//		}
	//	}
	//}

	for (int i=0;i<gridModel.m_nGridX;i++)
		pGridObj->m_bShowI.Add(FALSE);
	for (int i=0;i<gridModel.m_nGridY;i++)
		pGridObj->m_bShowJ.Add(FALSE);
	for (int i=0;i<gridModel.m_nGridZ;i++)
		pGridObj->m_bShowK.Add(FALSE);
	for (int i=0;i<gridModel.m_nGridZ;i++)
		pGridObj->m_bChangeK.Add(TRUE);

	int nSize = arParamName.GetSize();
	for (int i=0;i<nSize; i++)
	{
		StatusSetProgress(1,n);

		CString strParamName = arParamName[i];
		CFile file;
		CString strff = arFileName[i];
		if( file.Open(strff, CFile::modeRead | CFile::typeBinary ) )
		{
			CArchive ar(&file,CArchive::load);

			CString str;
			ar >> str;

			int nSize;
			ar >> nSize;
			for( int j = 0; j< gridModel.m_nGridX*gridModel.m_nGridY*gridModel.m_nGridZ; j++)
			{
				double tmp;
				ar >> tmp;
				pGridObj->Add(strParamName,tmp);
			}

			ar.Close();
			file.Close();
		}
		n++;		
	}	

	StatusProgress(1);
	//pGridObj->SetMaterial(White);
	
	pGridObj->ComputeBoundLimits();
	//pGridObj->ComputePoints();
	m_pDisplayContext->AddGLObj(pGridObj);
	
	//delete pGridObj;
	UpdateAllViews(NULL);	
}

void C3DModelDoc::AddPhyParam(LPCTSTR lpszFileName, LPCTSTR lpszName)
{
	std::vector<CGLObject*>::iterator iTer;
	CGLObject*pObj = NULL;
	bool bIsExist = false;
	for (iTer = GetContext()->m_listDisplay->begin();
		iTer!=GetContext()->m_listDisplay->end();
		iTer++)
	{
		pObj = (*iTer);
		if( pObj->GetGLObjType() == GLSURFACE)
		{			
			bIsExist = true;
			break;
		}
	}

	if( !bIsExist )
	{
		AfxMessageBox(_T("图中不存在网格模型!"), MB_OK|MB_ICONWARNING);
		return;
	}

	CGridObject *pGrid = (CGridObject *)pObj;

	int nSize = pGrid->m_vecPhyPara.GetSize();
	
	bIsExist = false;

	CString strName = lpszName;
	for (int i=0; i<nSize;i++)
	{
		if( strName == pGrid->m_vecPhyPara[i].m_strName)
		{
			bIsExist = true;
			break;
		}
	}

	if( bIsExist )
	{
		strName.Format(_T("网格模型中已经存在[%s]属性数据了"), lpszName);
		AfxMessageBox(strName, MB_OK|MB_ICONINFORMATION);
		return;
	}

	StatusBarMessage(_T("正在读取数据，请稍等..."));
	CWaitCursor wait;
	CFile file;
	if( file.Open(lpszFileName, CFile::modeRead | CFile::typeBinary ) )
	{
		CArchive ar(&file,CArchive::load);

		CString str;
		ar >> str;

		int nSize;
		ar >> nSize;
		if( nSize != (pGrid->I*pGrid->J*pGrid->K))
		{
			ar.Close();
			file.Close();
			AfxMessageBox(_T("此属性数据个数与网格模型单元个数不符!"), MB_OK|MB_ICONWARNING);
			return;
		}

		for( int j = 0; j< nSize; j++)
		{
			double tmp;
			ar >> tmp;
			pGrid->Add(strName,tmp);
		}

		ar.Close();
		file.Close();
		SetModifiedFlag();

		CMainFrame *pMF = (CMainFrame *)AfxGetMainWnd();

		C3DObjBar*pBar = pMF->Get3DBar();
		pBar->m_wndTree.FillTreeCtrl();
	}
}

void C3DModelDoc::AddWellPath(HTREEITEM hItem)
{
	POSITION pos = GetFirstViewPosition();
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();

	CViewTree * pTree = pFrame->GetTreeFileView()->GetTreeCtrl();
	HTREEITEM hItemChild = pTree->GetChildItem(hItem);

	BOOL bFlag = FALSE; // 有无井位选择
	while(hItemChild != NULL )
	{
		if( pTree->GetCheck(hItemChild) )
		{
			CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)pTree->GetItemData(hItemChild);
			if( lpNodeDat != NULL )
			{				
				bFlag = TRUE;
				break;
			}
		}
		hItemChild = pTree->GetNextSiblingItem(hItemChild);
	}

	if( !bFlag )
		return;
	
	double minX = DBL_MAX,minY=DBL_MAX, maxX=-DBL_MAX, maxY=-DBL_MAX;

	HTREEITEM hItemPath = pTree->GetChildNode(pTree->GetParentItem(hItem),_T("井眼轨迹数据"));
	hItemChild = pTree->GetChildItem(hItem);

	while(hItemChild != NULL )
	{
		if( pTree->GetCheck(hItemChild) )
		{
			CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)pTree->GetItemData(hItemChild);
			if( lpNodeDat != NULL )
			{
				CWellInfo *pInfo = (CWellInfo *)lpNodeDat->m_pNodeDat;
				double x,y;

				x = pInfo->m_x;
				y = pInfo->m_y;

				HTREEITEM h = pTree->GetChildNode(hItemPath, pInfo->m_strWellName, FALSE);
				if( h != NULL )
				{
					double X, Y;
					CTreeNodeDat *lpNode = (CTreeNodeDat *)pTree->GetItemData(h);
					CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
					CString strFileName = pMF->GetProjectDatPath();
					strFileName += _T("\\files\\");
					strFileName += ((CFileViewObj*)lpNode->m_pNodeDat)->m_strFileName;

					CFile file(strFileName, CFile::modeRead|CFile::typeBinary);
					CArchive ar(&file, CArchive::load);
					
					int nRows, nCols;

					ar >> nRows;
					ar >> nCols;		
					
					double east,north;
					double JS;

					for (int i=0;i<nRows;i++)
					{
						CString strTmp;
						ar >> strTmp;
						double js = atof(strTmp);

						ar >> strTmp;
						ar >> strTmp;

						ar >> strTmp;
						double E = atof(strTmp);
						ar >> strTmp;
						double N = atof(strTmp);

						if( i>0 && JS<pInfo->m_dBottom && js>pInfo->m_dBottom)
						{
							double s = (pInfo->m_dBottom - JS) / (js - pInfo->m_dBottom);
							
							E = (east + s * E)/(1+s);
							N = (north + s * N)/(1+s);

							X = x + E;
							Y = y + N;

							if( X>maxX )
								maxX = X;
							if( X<minX)
								minX = X;
							if( Y>maxY)
								maxY = Y;
							if(Y<minY)
								minY = Y;
							break;
						}
						if( i == nRows-1 && js<pInfo->m_dBottom)
						{
							double s = (pInfo->m_dBottom - JS) / (js - pInfo->m_dBottom);

							E = (east + s * E)/(1+s);
							N = (north + s * N)/(1+s);
						}

						east = E;
						north = N;
						JS = js;

						X = x + E;
						Y = y + N;

						if( X>maxX )
							maxX = X;
						if( X<minX)
							minX = X;
						if( Y>maxY)
							maxY = Y;
						if(Y<minY)
							minY = Y;

						ar >> strTmp;
					}
					ar.Close();
					file.Close();					
				}
				else
				{
					if( x>maxX )
						maxX = x;
					if( x<minX)
						minX =x;
					if( y>maxY)
						maxY = y;
					if(y<minY)
						minY = y;
				}
			}
		}
		hItemChild = pTree->GetNextSiblingItem(hItemChild);
	}	

	hItemChild = pTree->GetChildItem(hItem);	

	while(hItemChild != NULL )
	{
		if( pTree->GetCheck(hItemChild) )
		{
			CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)pTree->GetItemData(hItemChild);
			if( lpNodeDat != NULL )
			{
				CPipeLine *pPipe = new CPipeLine;
				pPipe->SetContext(m_pDisplayContext);

				CWellInfo *pInfo = (CWellInfo *)lpNodeDat->m_pNodeDat;
				CString strMQJB; // 井别、井型

				pPipe->m_Pipeline.m_strTitle = pInfo->m_strWellName;
				

				strMQJB = pInfo->m_strWellType;
				if( strMQJB == _T("采油井")
					|| strMQJB == _T("O")
					|| strMQJB == _T("11"))
				{
					pPipe->SetColor(255,0,0);
				}
				else
				{
					pPipe->SetColor(0,255,0);
				}

				HTREEITEM h = pTree->GetChildNode(hItemPath, pInfo->m_strWellName, FALSE);
				if( h != NULL )
				{
					CTreeNodeDat *lpNode = (CTreeNodeDat *)pTree->GetItemData(h);
					CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
					CString strFileName = pMF->GetProjectDatPath();
					strFileName += _T("\\files\\");
					strFileName += ((CFileViewObj*)lpNode->m_pNodeDat)->m_strFileName;

					CFile file(strFileName, CFile::modeRead|CFile::typeBinary);
					CArchive ar(&file, CArchive::load);

					int nRows, nCols;

					ar >> nRows;
					ar >> nCols;		
					
					double east,north;
					double JS, z;

					for (long i=0;i<nRows;i++)
					{
						CString strTmp;
						ar >> strTmp;
						double js = atof(strTmp);

						ar >> strTmp;
						ar >> strTmp;

						ar >> strTmp;
						double E = atof(strTmp);
						ar >> strTmp;
						double N = atof(strTmp);
						ar >> strTmp;
						double Z = atof(strTmp);

						if( js<pInfo->m_dTop)
						{
							east = E;
							north = N;
							JS = js;
							z = Z;
							continue;
						}

						if( i>0 && JS<pInfo->m_dTop && js>pInfo->m_dTop )
						{
							double s = (pInfo->m_dTop - JS) / (js - pInfo->m_dTop);
							
							CVertex3D vert;
							vert.x = pInfo->m_x+(east + s * E)/(1+s);
							vert.y = pInfo->m_y+(north + s * N)/(1+s);
							vert.z = -((z + s * Z)/(1+s)-pInfo->m_dElevation);
							pPipe->m_Pipeline.m_arrPoint.Add(vert);

							east = E;
							north = N;
							JS = js;
							z = Z;

							continue;
						}
						if( i==1 && pInfo->m_dTop < js )
						{
							double s = (pInfo->m_dTop - JS) / (js - pInfo->m_dTop);

							E = (east + s * E)/(1+s);
							N = (north + s * N)/(1+s);
							Z = (z + s * Z)/(1+s);
						}

						if( i>0 && JS<pInfo->m_dBottom && js>pInfo->m_dBottom)
						{
							double s = (pInfo->m_dBottom - JS) / (js - pInfo->m_dBottom);

							E = (east + s * E)/(1+s);
							N = (north + s * N)/(1+s);
							Z = (z + s * Z)/(1+s);

							CVertex3D vert;
							vert.x = pInfo->m_x+E;
							vert.y = pInfo->m_y+N;
							vert.z = -(Z-pInfo->m_dElevation);
							pPipe->m_Pipeline.m_arrPoint.Add(vert);
							break;
						}
						if( i == nRows-1 && js<pInfo->m_dBottom)
						{
							double s = (pInfo->m_dBottom - JS) / (js - pInfo->m_dBottom);

							E = (east + s * E)/(1+s);
							N = (north + s * N)/(1+s);
							Z = (z + s * Z)/(1+s);
						}

						east = E;
						north = N;
						JS = js;
						z = Z;

						CVertex3D vert;
						vert.x = pInfo->m_x+E;
						vert.y = pInfo->m_y+N;
						vert.z = -(Z-pInfo->m_dElevation);
						pPipe->m_Pipeline.m_arrPoint.Add(vert);
					}
					ar.Close();
					file.Close();
				}
				else
				{
					CVertex3D vert;
					vert.x = pInfo->m_x;
					vert.y = pInfo->m_y;
					vert.z = -(pInfo->m_dTop-pInfo->m_dElevation);
					pPipe->m_Pipeline.m_arrPoint.Add(vert);

					vert.x = pInfo->m_x;
					vert.y = pInfo->m_y;
					vert.z = -(pInfo->m_dBottom-pInfo->m_dElevation);
					pPipe->m_Pipeline.m_arrPoint.Add(vert);
				}

				pPipe->ComputeBoundLimits();

				CBoundingBox box = pPipe->GetBoundingBox();				
				box.SetLimits(minX, maxX, minY, maxY, box.ZMin(), box.ZMax());
				pPipe->SetBoundingBox(box);

				pPipe->SetGLObjType(GLPIPELINE);
				pPipe->SetObjName(pInfo->m_strWellName);

				m_pDisplayContext->AddGLObj(pPipe);
			}
		}
		hItemChild = pTree->GetNextSiblingItem(hItemChild);
	}

	UpdateAllViews(NULL);
}