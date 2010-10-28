// 3dLib\3DObjBar.cpp : 实现文件
//

#include "stdafx.h"
#include "3DObjBar.h"
#include "../Resource.h"
#include "../Interlayer.h"
#include "../IntersectSearchManager.h"
#include "../MainFrm.h"
#include "../3DLib/3DModelView.h"
#include "../3DLib/GridObject.h"
#include "../DlgImportModel.h"
#include "../DlgPhyParaCalc.h"
#include "../DlgPhyAdj.h"

// C3DObjBar

IMPLEMENT_DYNAMIC(C3DObjBar, CDockablePane)

C3DObjBar::C3DObjBar()
{

}

C3DObjBar::~C3DObjBar()
{
	m_PhyParaName.clear();
	m_PhyParaNamefilename.clear();
}


BEGIN_MESSAGE_MAP(C3DObjBar, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()

	ON_COMMAND(ID_NEW_3D, OnNew)
	ON_UPDATE_COMMAND_UI(ID_NEW_3D, OnUpdateNew)

	ON_COMMAND(ID_EDIT_RENAME, OnEditRename)
	ON_UPDATE_COMMAND_UI(ID_EDIT_RENAME, OnUpdateEditRename)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
	ON_COMMAND(ID_MATERIAL_SET, OnSetMaterial)
	ON_UPDATE_COMMAND_UI(ID_MATERIAL_SET, OnUpdateSetMaterial)
	ON_COMMAND(ID_COLOR_SET, OnSetColor)
	ON_UPDATE_COMMAND_UI(ID_COLOR_SET, OnUpdateSetColor)
	ON_COMMAND(ID_EDIT_ADDPRT, OnAddPhyPara)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADDPRT, OnUpdateAddPhyPara)

	ON_COMMAND(ID_EDIT_CALCPRT, OnCalcPhyPara)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CALCPRT, OnUpdateCalcPhyPara)
	ON_COMMAND(ID_EDIT_ADJPRT, OnAdjPhyPara)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADJPRT, OnUpdateAdjPhyPara)
END_MESSAGE_MAP()



// C3DObjBar 消息处理程序
int C3DObjBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建视图:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE
		| TVS_HASLINES 
		| TVS_LINESATROOT | TVS_HASBUTTONS 
		| TVS_CHECKBOXES | TVS_TRACKSELECT
		| TVS_EDITLABELS;

	if (!m_wndTree.Create(dwViewStyle, rectDummy, this, 4))
	{
		TRACE0("未能创建2D视图\n");
		return -1;      // 未能创建
	}
	
	m_TreeStateImages.Create(IDB_3D_OBJ_STATE, 16, 0, RGB(255, 255, 255));
	m_wndTree.SetImageList(&m_TreeStateImages, TVSIL_STATE);
	// 加载视图图像:
	m_Images.Create(IDB_3D_OBJ, 16, 0, RGB(255, 0, 0));
	m_wndTree.SetImageList(&m_Images, TVSIL_NORMAL);
	
	OnChangeVisualStyle();
	
	AdjustLayout();

	return 0;
}

HTREEITEM C3DObjBar::AddTreeItem(LPCTSTR lpszItem, int nImage, int nSelectedImage, 
								 HTREEITEM hParent, 
								 HTREEITEM hInsertAfter)
{
	HTREEITEM hItem = m_wndTree.InsertItem(lpszItem, nImage, nSelectedImage, hParent, hInsertAfter);
	return hItem;
}


void C3DObjBar::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}


void C3DObjBar::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);
	m_wndTree.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + 1, rectClient.Width() - 2, rectClient.Height() - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}


void C3DObjBar::OnChangeVisualStyle()
{
	m_Images.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_3D_OBJ_24 : IDB_3D_OBJ;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("无法加载位图: %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_Images.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_Images.Add(&bmp, RGB(255, 0, 0));

	m_wndTree.SetImageList(&m_Images, TVSIL_NORMAL);
	{	// 装入树状态图标
		m_TreeStateImages.DeleteImageList();	

		UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_3D_OBJ_STATE_24 : IDB_3D_OBJ_STATE;

		CBitmap bmp;
		if (!bmp.LoadBitmap(uiBmpId))
		{
			TRACE(_T("无法加载位图: %x\n"), uiBmpId);
			ASSERT(FALSE);
			return;
		}

		BITMAP bmpObj;
		bmp.GetBitmap(&bmpObj);

		UINT nFlags = ILC_MASK;

		nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

		m_TreeStateImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
		m_TreeStateImages.Add(&bmp, RGB(255, 255, 255));

		m_wndTree.SetImageList(&m_TreeStateImages, TVSIL_STATE);
	}
}

void C3DObjBar::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndTree.SetFocus();
}

void C3DObjBar::OnPaint()
{
	CPaintDC dc(this); // 用于绘制的设备上下文

	CRect rectTree;
	m_wndTree.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void C3DObjBar::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*) &m_wndTree;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// 选择已单击的项:
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			pWndTree->SelectItem(hTreeItem);
		}
	}

	pWndTree->SetFocus();

	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_POPUP_TREE));
	int nCount = menu.GetMenuItemCount();
	CMenu* pPopup = NULL;
	//
	//	GetMenuString
	CString strMenuName;
	for (int i=0;i<nCount;i++)
	{
		menu.GetMenuString(i,strMenuName,MF_BYPOSITION);
		if( strMenuName == "_三维对象_" )
		{
			pPopup = menu.GetSubMenu(i);
			break;
		}
	}

	if( pPopup == NULL )
		return;

	ASSERT(pPopup != NULL);

	theApp.GetContextMenuManager()->ShowPopupMenu(pPopup->m_hMenu, point.x, point.y, this, TRUE);
}

void C3DObjBar::OnNew()
{
	m_wndTree.OnNew();
}

void C3DObjBar::OnUpdateNew(CCmdUI *pCmdUI)
{
	m_wndTree.OnUpdateNew(pCmdUI);
}

void C3DObjBar::OnEditRename()
{
	// TODO: 在此添加命令处理程序代码
	m_wndTree.OnRename();
}

void C3DObjBar::OnUpdateEditRename(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	m_wndTree.OnUpdateRename(pCmdUI);
}

//________________________________________________________________________
// 删除节点
void C3DObjBar::OnEditClear()
{
	// TODO: 在此添加命令处理程序代码
	m_wndTree.OnEditClear();
}

void C3DObjBar::OnUpdateEditClear(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	m_wndTree.OnUpdateEditClear(pCmdUI);
}

void C3DObjBar::OnSetMaterial()
{
	m_wndTree.OnSetMaterial();
	
}

void C3DObjBar::OnUpdateSetMaterial(CCmdUI *pCmdUI)
{
	m_wndTree.OnUpdateSetMaterial(pCmdUI);
}

void C3DObjBar::OnSetColor()
{
	m_wndTree.OnSetColor();
}

void C3DObjBar::OnUpdateSetColor(CCmdUI *pCmdUI)
{
	m_wndTree.OnUpdateSetColor(pCmdUI);
}

void C3DObjBar::OnAddPhyPara()
{
	HTREEITEM hItem = m_wndTree.GetSelectedItem();

	CString strFileName;
	CString strName;
	CDlgImportModel dlg(TRUE,0,0,4|2|64,_T("所有文件 (*.*)|*.*||")); // 导入模型及场数据
	dlg.m_ofn.lpstrTitle = _T("导入场数据");
	if( dlg.DoModal() == IDOK )
	{
		strFileName = dlg.GetPathName();


		ReadEclipseGrid(strFileName);

		if( hItem != NULL )
		{
			CGLObject *pObj = (CGLObject*)(m_wndTree.GetItemData(hItem));
			if( pObj == NULL )
				return;
			else
			{
				switch(pObj->GetGLObjType())
				{
				case GLINTERLAYERCELL:
					{
						bool bIsExist = false;
						InterLayerGridObject * pGrid = dynamic_cast<InterLayerGridObject*>(pObj);
						//for(int i=0; i<m_PhyParaName.size(); i++)
						//{
						int nSize = pGrid->m_vecPhyPara.GetSize();


						for (int i=0; i<nSize;i++)
						{
							strName = m_PhyParaName[i];
							if( strName == pGrid->m_vecPhyPara[i].m_strName)
							{
								bIsExist = true;
								break;
							}
						}
						//}

						if( bIsExist )
						{
							strName.Format(_T("网格模型中已经存在[%s]属性数据了"), strName.GetBuffer());
							AfxMessageBox(strName, MB_OK|MB_ICONINFORMATION);
							return;
						}

						CWaitCursor wait;
						CFile file;
						for(int i=0; i<m_PhyParaNamefilename.size(); i++)
						{
							if( file.Open(m_PhyParaNamefilename[i].GetBuffer(), CFile::modeRead | CFile::typeBinary ) )
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
									pGrid->Add(m_PhyParaName[i],tmp);
								}
								pGrid->m_vecPhyPara[i].I = pGrid->I;
								pGrid->m_vecPhyPara[i].J = pGrid->J;
								pGrid->m_vecPhyPara[i].K = pGrid->K;

								ar.Close();
								file.Close();

								CMainFrame *pMF = (CMainFrame *)AfxGetMainWnd();

								CMDIChildWndEx *pWnd =(CMDIChildWndEx *) pMF->MDIGetActive();
								if( pWnd )
								{
									C3DModelView *pView = (C3DModelView *)pWnd->GetActiveView();
									if(pView)
									{
										if(pView->IsKindOf(RUNTIME_CLASS(C3DModelView)))
										{
											C3DModelDoc *pDoc = (C3DModelDoc *)pView ->GetDocument();
											pDoc->GetContext()->SetModifiedFlag();
										}
									}
								}	

							}
						}

						CMainFrame *pMF = (CMainFrame *)AfxGetMainWnd();
						C3DObjBar*pBar = pMF->Get3DBar();
						pBar->m_wndTree.FillTreeCtrl();
					}
					break;
				case GLSURFACE:
					{
						bool bIsExist = false;
						CGridObject * pGrid = dynamic_cast<CGridObject*>(pObj);
						//for(int i=0; i<m_PhyParaName.size(); i++)
						//{
						int nSize = pGrid->m_vecPhyPara.GetSize();


						for (int i=0; i<nSize;i++)
						{
							strName = m_PhyParaName[i];
							if( strName == pGrid->m_vecPhyPara[i].m_strName)
							{
								bIsExist = true;
								break;
							}
						}
						//}

						if( bIsExist )
						{
							strName.Format(_T("网格模型中已经存在[%s]属性数据了"), strName.GetBuffer());
							AfxMessageBox(strName, MB_OK|MB_ICONINFORMATION);
							return;
						}

						CWaitCursor wait;
						CFile file;
						for(int i=0; i<m_PhyParaNamefilename.size(); i++)
						{
							if( file.Open(m_PhyParaNamefilename[i].GetBuffer(), CFile::modeRead | CFile::typeBinary ) )
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
									pGrid->Add(m_PhyParaName[i],tmp);
								}
								pGrid->m_vecPhyPara[i].I = pGrid->I;
								pGrid->m_vecPhyPara[i].J = pGrid->J;
								pGrid->m_vecPhyPara[i].K = pGrid->K;
								ar.Close();
								file.Close();

								pGrid->m_vecPhyPara[0].I = pGrid->I;
								pGrid->m_vecPhyPara[0].J = pGrid->J;
								pGrid->m_vecPhyPara[0].K = pGrid->K;
		

								CMainFrame *pMF = (CMainFrame *)AfxGetMainWnd();
								pGrid->m_vecPhyPara[i].SavePara(m_PhyParaNamefilename[i].GetBuffer());
								HTREEITEM hItem = pMF->GetTreeModelView()->GetItemByGUID(pGrid->m_strGUID);
								if(hItem)
									pMF->GetTreeModelView()->AddGridEclipse(m_PhyParaNamefilename[i].GetBuffer(), m_PhyParaName[i], hItem);
								//CMDIChildWndEx *pWnd =(CMDIChildWndEx *) pMF->MDIGetActive();
								//if( pWnd )
								//{
								//	C3DModelView *pView = (C3DModelView *)pWnd->GetActiveView();
								//	if(pView)
								//	{
								//		if(pView->IsKindOf(RUNTIME_CLASS(C3DModelView)))
								//		{
								//			C3DModelDoc *pDoc = (C3DModelDoc *)pView ->GetDocument();
								//			pDoc->GetContext()->SetModifiedFlag();
								//		}
								//	}
								//}	

							}
						}

						CMainFrame *pMF = (CMainFrame *)AfxGetMainWnd();
						C3DObjBar*pBar = pMF->Get3DBar();
						pBar->m_wndTree.FillTreeCtrl();
					}
					break;
				default:
					break;
				}
			}
		}
	}
}

void C3DObjBar::OnUpdateAddPhyPara( CCmdUI *pCmdUI )
{
	HTREEITEM hti;
	hti = m_wndTree.GetSelectedItem();
	if( hti != NULL )
	{
		CGLObject *pObj = (CGLObject*)(m_wndTree.GetItemData(hti));

		if( pObj!=NULL)
		{
			if( pObj->GetGLObjType() == GLSURFACE)
			{
				if( !dynamic_cast<CGridObject*>(pObj)->m_vecPhyPara.IsEmpty()  )
					pCmdUI->Enable(TRUE);
			}
			else if(pObj->GetGLObjType() == GLINTERLAYERCELL )
			{
				if( !dynamic_cast<InterLayerGridObject*>(pObj)->m_vecPhyPara.IsEmpty()  )
					pCmdUI->Enable(TRUE);
			}
			else
				pCmdUI->Enable(FALSE);
		}
		else
			pCmdUI->Enable(FALSE);
	}
	else
		pCmdUI->Enable(FALSE);
}

void C3DObjBar::ReadEclipseGrid( LPCTSTR filename )
{
	m_PhyParaName.clear();
	m_PhyParaNamefilename.clear();
	CStdioFile fileOld;
	if( !fileOld.Open(filename, CFile::modeRead | CFile::typeText) )
		return ;
	if( fileOld.GetLength() < 1 ) // 如果文件长度为0 返回空的文件名
	{
		fileOld.Close();
		return ;
	}

	fileOld.Close();

	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();

	CWaitCursor wait;

	CString strMsg;
	strMsg = _T("正在分析静态场数据文件，请稍等...");
	pMF->GetStatusBar().SetPaneText(0, strMsg);

	CString strTmp;

	std::vector<double> values;

	CParseEclipseFile eclipseFile;
	eclipseFile.OpenFile(filename);
	CSimuToken token;
	CEclipseCommandItem commType; // 数据类型

	do
	{
		CEclipseCommandItem commItem;
		token = eclipseFile.GetToken(commItem);
		if( token.token_type == CSimuToken::COMMAND )
		{
			switch(commItem.m_nCommID)
			{
			case CEclipseCommandItem::PORO:		// 有效厚度
			case CEclipseCommandItem::GENERAL:			// x方向渗透率
			case CEclipseCommandItem::NTG:
			case CEclipseCommandItem::PERMX:
			case CEclipseCommandItem::PERMY:
			case CEclipseCommandItem::SWAT:				
				if( values.size()> 0 )
				{
					strMsg = _T("正在保存静态场数据文件，请稍等...");
					pMF->GetStatusBar().SetPaneText(0, strMsg);

					CString strName = newGUID();
					strName += _T(".grd");

					CString strNewFileName = pMF->GetProjectDatPath();
					strNewFileName += _T("\\models\\");
					strNewFileName += strName;

					CFile fileNew;
					if( fileNew.Open(strNewFileName, CFile::modeCreate | CFile::modeWrite ) )
					{
						CArchive archive(&fileNew, CArchive::store);

						CString strModelFileName = commType.m_strCommand;

						m_PhyParaName.push_back(strModelFileName);
						m_PhyParaNamefilename.push_back(strNewFileName);

						archive << strModelFileName;

						int nSize = values.size();

						archive << nSize;

						for ( int i=0; i<nSize; i++)
						{
							archive << values[i];
						}
						archive << RGB(255,0,0);
						archive << RGB(255,255,255);
						archive << (double)0;
						archive << (double)-1;
						archive.Close();
						fileNew.Close();
					}
					values.erase(values.begin(), values.end());
				}
				commType = commItem;			
				break;

			default:				
				if(values.size()>0)
				{					
					commType.m_nCommID = CEclipseCommandItem::UNKNOWN;
					strMsg = _T("正在保存静态场数据文件，请稍等...");
					pMF->GetStatusBar().SetPaneText(0, strMsg);

					CString strName = newGUID();
					strName += _T(".grd");

					CString strNewFileName = pMF->GetProjectDatPath();
					strNewFileName += _T("\\models\\");
					strNewFileName += strName;

					CFile fileNew;
					if( fileNew.Open(strNewFileName, CFile::modeCreate | CFile::modeWrite ) )
					{
						CArchive archive(&fileNew, CArchive::store);

						CString strModelFileName = commType.m_strCommand;

						m_PhyParaName.push_back(strModelFileName);
						m_PhyParaNamefilename.push_back(strNewFileName);

						archive << strModelFileName;

						int nSize = values.size();

						archive << nSize;

						for ( int i=0; i<nSize; i++)
						{
							archive << values[i];
						}

						archive << RGB(255,0,0);
						archive << RGB(255,255,255);
						archive << (double)0;
						archive << (double)-1;
						archive.Close();
						fileNew.Close();
					}

					values.erase(values.begin(), values.end());

					strMsg = "正在分析静态场数据文件，请稍等...";
				}
				break;
			}
		}		
		else if( token.token_type == CSimuToken::NUMBER )
		{
			CString strTmp = token.m_strToken;
			int index = strTmp.Find("*");

			switch(commType.m_nCommID)
			{
			case CEclipseCommandItem::PORO:		// 有效厚度
			case CEclipseCommandItem::GENERAL:	// x方向渗透率
			case CEclipseCommandItem::NTG:
			case CEclipseCommandItem::PERMX:
			case CEclipseCommandItem::PERMY:
			case CEclipseCommandItem::SWAT:	
				{					
					if(index > -1) // 是 2*0.5 格式
					{
						int n = atoi(strTmp.Left(index));
						double v = atof(strTmp.Right(strTmp.GetLength()-index+1));
						for (int i=0;i<n;i++)
							values.push_back(v);
					}
					else
					{
						double v = atof(strTmp);
						values.push_back(v);
					}
				}
				break;
			default:
				break;
			}			
		}
		else if( token.token_type == CSimuToken::STRING )
		{
			commType.m_nCommID = CEclipseCommandItem::UNKNOWN;
		}
		else
		{
			commType.m_nCommID = CEclipseCommandItem::UNKNOWN;
		}

	}while(token.token_type != CSimuToken::END);


	if( values.size() > 0 )
	{
		strMsg = _T("正在保存静态场数据文件，请稍等...");
		pMF->GetStatusBar().SetPaneText(0, strMsg);

		CString strName = newGUID();
		strName += _T(".grd");

		CString strNewFileName = pMF->GetProjectDatPath();
		strNewFileName += _T("\\models\\");
		strNewFileName += strName;

		CFile fileNew;
		if( fileNew.Open(strNewFileName, CFile::modeCreate | CFile::modeWrite ) )
		{
			CArchive archive(&fileNew, CArchive::store);

			CString strModelFileName = commType.m_strCommand;

			m_PhyParaName.push_back(strModelFileName);
			m_PhyParaNamefilename.push_back(strNewFileName);

			archive << strModelFileName;

			int nSize = values.size();

			archive << nSize;

			for ( int i=0; i<nSize; i++)
			{
				archive << values[i];
			}
			archive << RGB(255,0,0);
			archive << RGB(255,255,255);
			archive << (double)0;
			archive << (double)-1;
			archive.Close();
			fileNew.Close();
		}

	}
}

void C3DObjBar::OnCalcPhyPara()
{
	CDlgPhyParaCalc dlg;

	dlg.m_phy1 = 1;
	dlg.m_phy2 = 1;
	dlg.m_new_phyname = "属性新";

	HTREEITEM hItem = m_wndTree.GetSelectedItem();
	CGLObject *pObj;
	if( hItem != NULL )
	{
		pObj = (CGLObject*)(m_wndTree.GetItemData(hItem));
		if( pObj == NULL )
			return;
		else
		{
			dlg.SetGLObj(pObj);
		}
	}
	
	if(dlg.DoModal()==IDOK)
	{
		CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
		CString strName1 = newGUID();
		strName1 += _T(".pro");
		CString strName2 = newGUID();
		strName2 += _T(".pro");
		CString strName3 = newGUID();
		strName3 += _T(".pro");

		CString strNewFileName1 = pMF->GetProjectDatPath();
		strNewFileName1 += _T("\\files\\");
		strNewFileName1 += strName1;
		CString strNewFileName2 = pMF->GetProjectDatPath();
		strNewFileName2 += _T("\\files\\");
		strNewFileName2 += strName2;
		CString strNewFileName3 = pMF->GetProjectDatPath();
		strNewFileName3 += _T("\\files\\");
		strNewFileName3 += strName3;
		
		
		CString strMsg;
		strMsg = _T("正在分析静态场数据文件，请稍等...");
		pMF->GetStatusBar().SetPaneText(0, strMsg);

		switch(pObj->GetGLObjType())
		{
		case GLINTERLAYERCELL:
			{
				bool bIsExist = false;
				InterLayerGridObject * pGrid = dynamic_cast<InterLayerGridObject*>(pObj);

				if(dlg.m_singleOperator)
				{
					pGrid->m_vecPhyPara[dlg.m_index_phyname1].SavePara(strNewFileName1.GetBuffer());

					CIntersectSearchManager::Instance()->ComputePara1(strNewFileName1.GetBuffer(),dlg.m_phy1, strNewFileName3.GetBuffer(), dlg.m_op1.GetBuffer());
				}
				else
				{
					pGrid->m_vecPhyPara[dlg.m_index_phyname1].SavePara(strNewFileName1.GetBuffer());
					pGrid->m_vecPhyPara[dlg.m_index_phyname2].SavePara(strNewFileName2.GetBuffer());

					CIntersectSearchManager::Instance()->ComputePara2(strNewFileName1.GetBuffer(), strNewFileName2.GetBuffer(), strNewFileName3.GetBuffer(), 
						dlg.m_op1.GetBuffer(), dlg.m_op2.GetBuffer(), dlg.m_op0.GetBuffer(),
						dlg.m_phy1, dlg.m_phy2);
				}
				
				int nSize = pGrid->m_vecPhyPara.GetSize();

				int i;
				for (i=0; i<nSize; i++)
				{
					if( dlg.m_new_phyname == pGrid->m_vecPhyPara[i].m_strName)
					{
						bIsExist = true;
						break;
					}
				}
				if( bIsExist )
				{
					pGrid->m_vecPhyPara[i].LoadPara(strNewFileName3.GetBuffer());
					pGrid->m_vecPhyPara[i].m_bShow = FALSE;
				}
				else
				{
					CPhyPara tmpara;
					tmpara.LoadPara(strNewFileName3.GetBuffer());
					tmpara.m_bShow = FALSE;
					tmpara.m_strName = dlg.m_new_phyname;
					pGrid->m_vecPhyPara.Add(tmpara);
				}
			}
			break;
		case GLSURFACE:
			{
				bool bIsExist = false;
				CGridObject * pGrid = dynamic_cast<CGridObject*>(pObj);

				if(dlg.m_singleOperator)
				{
					pGrid->m_vecPhyPara[dlg.m_index_phyname1].SavePara(strNewFileName1.GetBuffer());

					CIntersectSearchManager::Instance()->ComputePara1(strNewFileName1.GetBuffer(),dlg.m_phy1, strNewFileName3.GetBuffer(), dlg.m_op1.GetBuffer());
				}
				else
				{
					pGrid->m_vecPhyPara[dlg.m_index_phyname1].SavePara(strNewFileName1.GetBuffer());
					pGrid->m_vecPhyPara[dlg.m_index_phyname2].SavePara(strNewFileName2.GetBuffer());

					CIntersectSearchManager::Instance()->ComputePara2(strNewFileName1.GetBuffer(), strNewFileName2.GetBuffer(), strNewFileName3.GetBuffer(), 
						dlg.m_op1.GetBuffer(), dlg.m_op2.GetBuffer(), dlg.m_op0.GetBuffer(),
						dlg.m_phy1, dlg.m_phy2);
				}
				int nSize = pGrid->m_vecPhyPara.GetSize();

				int i;
				for (i=0; i<nSize; i++)
				{
					if( dlg.m_new_phyname == pGrid->m_vecPhyPara[i].m_strName)
					{
						bIsExist = true;
						break;
					}
				}
				if( bIsExist )
				{
					pGrid->m_vecPhyPara[i].LoadPara(strNewFileName3.GetBuffer());
					pGrid->m_vecPhyPara[i].m_bShow = FALSE;
				}
				else
				{
					CPhyPara tmpara;
					tmpara.LoadPara(strNewFileName3.GetBuffer());
					tmpara.m_bShow = FALSE;
					tmpara.m_strName = dlg.m_new_phyname;
					pGrid->m_vecPhyPara.Add(tmpara);
				}
			}
			break;
		default:
			break;
		}
	
		C3DObjBar*pBar = pMF->Get3DBar();
		pBar->m_wndTree.FillTreeCtrl();

	//	CMDIChildWndEx *pWnd =(CMDIChildWndEx *) pMF->MDIGetActive();
	//	if( pWnd )
	//	{
	//		C3DModelView *pView = (C3DModelView *)pWnd->GetActiveView();
	//		if(pView)
	//		{
	//			if(pView->IsKindOf(RUNTIME_CLASS(C3DModelView)))
	//			{
	//				C3DModelDoc *pDoc = (C3DModelDoc *)pView ->GetDocument();
	//				pDoc->GetContext()->SetModifiedFlag();
	//			}
	//		}
	//	}	
	}

}

void C3DObjBar::OnUpdateCalcPhyPara( CCmdUI *pCmdUI )
{
	HTREEITEM hti;
	hti = m_wndTree.GetSelectedItem();
	if( hti != NULL )
	{
		CGLObject *pObj = (CGLObject*)(m_wndTree.GetItemData(hti));

		if( pObj!=NULL)
		{
			if(pObj->GetGLObjType() == GLSURFACE)
			{
				if( !dynamic_cast<CGridObject*>(pObj)->m_vecPhyPara.IsEmpty()  )
					pCmdUI->Enable(TRUE);
				else
					pCmdUI->Enable(FALSE);
			}
			else if(pObj->GetGLObjType() == GLINTERLAYERCELL )
			{
				if( !dynamic_cast<InterLayerGridObject*>(pObj)->m_vecPhyPara.IsEmpty()  )
					pCmdUI->Enable(TRUE);
				else
					pCmdUI->Enable(FALSE);
			}
			else
				pCmdUI->Enable(FALSE);
		}
		else
			pCmdUI->Enable(FALSE);
	}
	else
		pCmdUI->Enable(FALSE);
}

void C3DObjBar::OnAdjPhyPara()
{
	//CIntersectSearchManager::Instance()->RunCommond(_T("f:\\release\\WindowsFormsApplication5.exe 测试") );
	HTREEITEM hItem = m_wndTree.GetSelectedItem();

	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	CString strName1 = newGUID();
	strName1 += _T(".pro");
	CString strName2 = newGUID();
	strName2 += _T(".pro");
	CString strName3 = newGUID();
	strName3 += _T(".pro");

	CString strNewFileName1 = pMF->GetProjectDatPath();
	strNewFileName1 += _T("\\files\\");
	strNewFileName1 += strName1;

	CString strNewFileName3 = pMF->GetProjectDatPath();
	strNewFileName3 += _T("\\files\\");
	strNewFileName3 += strName3;
	CDlgPhyAdj dlg;
	CGLObject *pObj = (CGLObject*)(m_wndTree.GetItemData(hItem));
	dlg.SetGLObj(pObj);
	if( dlg.DoModal() == IDOK )
	{
		if( hItem != NULL )
		{
			if( pObj == NULL )
				return;
			else
			{
				switch(pObj->GetGLObjType())
				{
				case GLINTERLAYERCELL:
					{
						bool bIsExist = false;
						InterLayerGridObject * pGrid = dynamic_cast<InterLayerGridObject*>(pObj);

						pGrid->m_vecPhyPara[dlg.m_index].SavePara(strNewFileName1.GetBuffer());

						CIntersectSearchManager::Instance()->AveragePara(strNewFileName1.GetBuffer(), strNewFileName3.GetBuffer());

						int nSize = pGrid->m_vecPhyPara.GetSize();

						int i;
						for (i=0; i<nSize; i++)
						{
							if( dlg.m_newphyname == pGrid->m_vecPhyPara[i].m_strName)
							{
								bIsExist = true;
								break;
							}
						}
						if( bIsExist )
						{
							pGrid->m_vecPhyPara[i].LoadPara(strNewFileName3.GetBuffer());
							pGrid->m_vecPhyPara[i].m_bShow = FALSE;
						}
						else
						{
							CPhyPara tmpara;
							tmpara.LoadPara(strNewFileName3.GetBuffer());
							tmpara.m_bShow = FALSE;
							tmpara.m_strName = dlg.m_newphyname;
							pGrid->m_vecPhyPara.Add(tmpara);
						}
					}
					break;
				case GLSURFACE:
					{
						bool bIsExist = false;
						CGridObject * pGrid = dynamic_cast<CGridObject*>(pObj);

						pGrid->m_vecPhyPara[dlg.m_index].SavePara(strNewFileName1.GetBuffer());

						CIntersectSearchManager::Instance()->AveragePara(strNewFileName1.GetBuffer(), strNewFileName3.GetBuffer());

						int nSize = pGrid->m_vecPhyPara.GetSize();

						int i;
						for (i=0; i<nSize; i++)
						{
							if( dlg.m_newphyname == pGrid->m_vecPhyPara[i].m_strName)
							{
								bIsExist = true;
								break;
							}
						}
						if( bIsExist )
						{
							pGrid->m_vecPhyPara[i].LoadPara(strNewFileName3.GetBuffer());
							pGrid->m_vecPhyPara[i].m_bShow = FALSE;
						}
						else
						{
							CPhyPara tmpara;
							tmpara.LoadPara(strNewFileName3.GetBuffer());
							tmpara.m_bShow = FALSE;
							tmpara.m_strName = dlg.m_newphyname;
							pGrid->m_vecPhyPara.Add(tmpara);
						}
					}
					break;
				default:
					break;
				}
			}
		}

		CMainFrame *pMF = (CMainFrame *)AfxGetMainWnd();

		//CMDIChildWndEx *pWnd =(CMDIChildWndEx *) pMF->MDIGetActive();
		//if( pWnd )
		//{
		//	C3DModelView *pView = (C3DModelView *)pWnd->GetActiveView();
		//	if(pView)
		//	{
		//		if(pView->IsKindOf(RUNTIME_CLASS(C3DModelView)))
		//		{
		//			C3DModelDoc *pDoc = (C3DModelDoc *)pView ->GetDocument();
		//			pDoc->GetContext()->SetModifiedFlag();
		//		}
		//	}
		//}	

		C3DObjBar*pBar = pMF->Get3DBar();
		pBar->m_wndTree.FillTreeCtrl();
	}
}

void C3DObjBar::OnUpdateAdjPhyPara( CCmdUI *pCmdUI )
{
	HTREEITEM hti;
	hti = m_wndTree.GetSelectedItem();
	if( hti != NULL )
	{
		CGLObject *pObj = (CGLObject*)(m_wndTree.GetItemData(hti));

		if( pObj!=NULL)
		{
			if( pObj->GetGLObjType() == GLSURFACE)
			{
				if( !dynamic_cast<CGridObject*>(pObj)->m_vecPhyPara.IsEmpty()  )
					pCmdUI->Enable(TRUE);
				else
					pCmdUI->Enable(FALSE);
			}
			else if(pObj->GetGLObjType() == GLINTERLAYERCELL )
			{
				if( !dynamic_cast<InterLayerGridObject*>(pObj)->m_vecPhyPara.IsEmpty()  )
					pCmdUI->Enable(TRUE);
				else
					pCmdUI->Enable(FALSE);
			}
			else
				pCmdUI->Enable(FALSE);
		}
		else
			pCmdUI->Enable(FALSE);
	}
	else
		pCmdUI->Enable(FALSE);
}

void C3DObjBar::DeleteItemByGUID( CString guid )
{
	m_wndTree.DeleteItemByGUID(guid);
}
IMPLEMENT_DYNAMIC(CLayerIntersectSearch, CDockablePane)

CLayerIntersectSearch::CLayerIntersectSearch()
{

}

CLayerIntersectSearch::~CLayerIntersectSearch()
{
}


BEGIN_MESSAGE_MAP(CLayerIntersectSearch, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()

	ON_COMMAND(ID_NEW_3D, OnNew)
	ON_UPDATE_COMMAND_UI(ID_NEW_3D, OnUpdateNew)

	ON_COMMAND(ID_EDIT_RENAME, OnEditRename)
	ON_UPDATE_COMMAND_UI(ID_EDIT_RENAME, OnUpdateEditRename)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
	ON_COMMAND(ID_MATERIAL_SET, OnSetMaterial)
	ON_UPDATE_COMMAND_UI(ID_MATERIAL_SET, OnUpdateSetMaterial)
	ON_COMMAND(ID_COLOR_SET, OnSetColor)
	ON_UPDATE_COMMAND_UI(ID_COLOR_SET, OnUpdateSetColor)
	ON_BN_CLICKED(IDC_BUTTON_INTERSECTSEARCH, OnClickButton)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_INTERSECTSEARCH, OnUpdateClickMyButton) 
END_MESSAGE_MAP()



// CLayerIntersectSearch 消息处理程序
int CLayerIntersectSearch::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建视图:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE
		| TVS_HASLINES 
		| TVS_LINESATROOT | TVS_HASBUTTONS 
		| TVS_CHECKBOXES | TVS_TRACKSELECT
		| TVS_EDITLABELS;

	if (!m_wndTree.Create(dwViewStyle, rectDummy, this, 4))
	{
		TRACE0("未能创建2D视图\n");
		return -1;      // 未能创建
	}

	if(!m_wndButton.Create(_T("查找相交模型网格"),WS_CHILD | WS_VISIBLE,rectDummy,this,IDC_BUTTON_INTERSECTSEARCH))
	{
		TRACE0("未能创建2D视图\n");
		return -1;      // 未能创建
	}
	m_TreeStateImages.Create(IDB_3D_OBJ_STATE, 16, 0, RGB(255, 255, 255));
	m_wndTree.SetImageList(&m_TreeStateImages, TVSIL_STATE);
	// 加载视图图像:
	m_Images.Create(IDB_3D_OBJ, 16, 0, RGB(255, 0, 0));
	m_wndTree.SetImageList(&m_Images, TVSIL_NORMAL);

	OnChangeVisualStyle();

	AdjustLayout();

	return 0;
}

HTREEITEM CLayerIntersectSearch::AddTreeItem(LPCTSTR lpszItem, int nImage, int nSelectedImage, 
								 HTREEITEM hParent, 
								 HTREEITEM hInsertAfter)
{
	HTREEITEM hItem = m_wndTree.InsertItem(lpszItem, nImage, nSelectedImage, hParent, hInsertAfter);
	return hItem;
}


void CLayerIntersectSearch::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}


void CLayerIntersectSearch::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);
	m_wndButton.SetWindowPos(NULL, rectClient.left + 1, 0, rectClient.Width() - 2, 50, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndTree.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + 51, rectClient.Width() - 2, rectClient.Height() - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}


void CLayerIntersectSearch::OnChangeVisualStyle()
{
	m_Images.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_3D_OBJ_24 : IDB_3D_OBJ;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("无法加载位图: %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_Images.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_Images.Add(&bmp, RGB(255, 0, 0));

	m_wndTree.SetImageList(&m_Images, TVSIL_NORMAL);
	{	// 装入树状态图标
		m_TreeStateImages.DeleteImageList();	

		UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_3D_OBJ_STATE_24 : IDB_3D_OBJ_STATE;

		CBitmap bmp;
		if (!bmp.LoadBitmap(uiBmpId))
		{
			TRACE(_T("无法加载位图: %x\n"), uiBmpId);
			ASSERT(FALSE);
			return;
		}

		BITMAP bmpObj;
		bmp.GetBitmap(&bmpObj);

		UINT nFlags = ILC_MASK;

		nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

		m_TreeStateImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
		m_TreeStateImages.Add(&bmp, RGB(255, 255, 255));

		m_wndTree.SetImageList(&m_TreeStateImages, TVSIL_STATE);
	}
}

void CLayerIntersectSearch::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndTree.SetFocus();
}

void CLayerIntersectSearch::OnPaint()
{
	CPaintDC dc(this); // 用于绘制的设备上下文

	CRect rectTree;
	m_wndTree.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CLayerIntersectSearch::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*) &m_wndTree;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// 选择已单击的项:
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			pWndTree->SelectItem(hTreeItem);
		}
	}

	pWndTree->SetFocus();

	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_POPUP_TREE));
	int nCount = menu.GetMenuItemCount();
	CMenu* pPopup = NULL;
	//
	//	GetMenuString
	CString strMenuName;
	for (int i=0;i<nCount;i++)
	{
		menu.GetMenuString(i,strMenuName,MF_BYPOSITION);
		if( strMenuName == "_三维对象_" )
		{
			pPopup = menu.GetSubMenu(i);
			break;
		}
	}

	if( pPopup == NULL )
		return;

	ASSERT(pPopup != NULL);

	theApp.GetContextMenuManager()->ShowPopupMenu(pPopup->m_hMenu, point.x, point.y, this, TRUE);
}

void CLayerIntersectSearch::OnNew()
{
	m_wndTree.OnNew();
}

void CLayerIntersectSearch::OnUpdateNew(CCmdUI *pCmdUI)
{
	m_wndTree.OnUpdateNew(pCmdUI);
}

void CLayerIntersectSearch::OnEditRename()
{
	// TODO: 在此添加命令处理程序代码
	m_wndTree.OnRename();
}

void CLayerIntersectSearch::OnUpdateEditRename(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	m_wndTree.OnUpdateRename(pCmdUI);
}

//________________________________________________________________________
// 删除节点
void CLayerIntersectSearch::OnEditClear()
{
	// TODO: 在此添加命令处理程序代码
	m_wndTree.OnEditClear();
}

void CLayerIntersectSearch::OnUpdateEditClear(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	m_wndTree.OnUpdateEditClear(pCmdUI);
}

void CLayerIntersectSearch::OnSetMaterial()
{
	m_wndTree.OnSetMaterial();

}

void CLayerIntersectSearch::OnUpdateSetMaterial(CCmdUI *pCmdUI)
{
	m_wndTree.OnUpdateSetMaterial(pCmdUI);
}

void CLayerIntersectSearch::OnSetColor()
{
	m_wndTree.OnSetColor();
}

void CLayerIntersectSearch::OnUpdateSetColor(CCmdUI *pCmdUI)
{
	m_wndTree.OnUpdateSetColor(pCmdUI);
}

void CLayerIntersectSearch::OnUpdateClickMyButton( CCmdUI* pCmdUI )
{
	pCmdUI->Enable();
}

void CLayerIntersectSearch::OnClickButton()
{
	CMainFrame *pMF = (CMainFrame *)AfxGetMainWnd();

	C3DObjBar*pBar = pMF->Get3DBar();
	CLayerIntersectSearch *pSearchbar = pMF->GetSearchBar();
	//CIntersectSearchManager::Instance()->ReleaseAll();
	//遍历管理器把模型和夹层保存并调用C#追夹层
	CIntersectSearchManager::Instance()->SearchInterSect();
	pBar->m_wndTree.DeleteAllItems();
	pBar->m_wndTree.FillTreeCtrl();
	pSearchbar->m_wndTree.DeleteAllItems();
	pSearchbar->m_wndTree.FillTreeCtrl();
}

void CLayerIntersectSearch::SetGrid( const std::string& gridname )
{
	m_wndTree.SetModel(gridname);
}
