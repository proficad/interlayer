// 3Dlib\3DObjTreeCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "3DObjTreeCtrl.h"
#include "../MainFrm.h"
#include "3DModelView.h"
#include "GLTrihedron.h"
#include "../DlgMaterial.h"
#include "../3DLib/C3DSlice.h"
#include "../3DLib/GridObject.h"
#include "../DlgZSlice.h"
#include "../DlgXYSlice.h"
#include "../IntersectSearchManager.h"
#include <afxtempl.h> 
// C3DObjTreeCtrl

IMPLEMENT_DYNAMIC(C3DObjTreeCtrl, CTreeCtrl)

C3DObjTreeCtrl::C3DObjTreeCtrl()
{
	m_pDoc = NULL;
}

C3DObjTreeCtrl::~C3DObjTreeCtrl()
{
}

// GLCURVE		= 123456,					// 曲线
// GLFONT		= 234561,					// 文字
// GLPLANE		= 345612,					// 平面
// GLPOINT		= 456123,					// 点
// GLSURFACE	= 561234,					// 表面
// GLTRIHEDRON	= 612345,					// 三面体
void C3DObjTreeCtrl::FillTreeCtrl()
{
	DeleteAllItems();
	if( m_pDoc == NULL )
		return;
	
	HTREEITEM hItem1 = InsertItem(_T("基本对象"),0,0);
	//HTREEITEM hItem2 = InsertItem(_T("图形切块"),0,0);
	HTREEITEM hItem3 = InsertItem(_T("图形切片"),0,0);
	HTREEITEM hItem31 = InsertItem(_T("横切片"),0,0,hItem3);
	HTREEITEM hItem32 = InsertItem(_T("纵切片"),0,0,hItem3);
	HTREEITEM hItem4 = InsertItem(_T("图形对象"),0,0);
	HTREEITEM hItem41 = InsertItem(_T("面"),0,0,hItem4);
	HTREEITEM hItem42 = InsertItem(_T("线"),0,0,hItem4);
	HTREEITEM hItem43 = InsertItem(_T("点"),0,0,hItem4);
	HTREEITEM hItem44 = InsertItem(_T("夹层网格"),0,0,hItem4);

	POSITION pos = m_pDoc->GetFirstViewPosition();
	C3DModelView *pView = (C3DModelView *)m_pDoc->GetNextView(pos);

	CGLTrihedron* pTri = pView->m_myView->m_myTrihedron;

	if( !pView->m_myView->IsShowAxis() && !pView->m_myView->IsShowGird() && !pView->m_myView->IsShowLegend() )
		SetCheck(hItem1,2);
	else
		SetCheck(hItem1,1);

	//////////////////////////////////////////////////////////////////////////
	HTREEITEM hItem = InsertItem(_T("坐标框"),1,1,hItem1);
	if( pView->m_myView->IsShowAxis() )
		SetCheck(hItem, 1);
	else
		SetCheck(hItem, 2);

	SetItemData(hItem, (DWORD)pTri);

	//------------------------------------------------------------------------
	hItem = InsertItem(_T("坐标网格"),2,2,hItem1);
	if( pView->m_myView->IsShowGird() )
		SetCheck(hItem, 1);
	else
		SetCheck(hItem, 2);

	SetItemData(hItem, (DWORD)pTri);

	//------------------------------------------------------------------------
	hItem = InsertItem(_T("图例色条"),3,3,hItem1);
	if( pView->m_myView->IsShowLegend() )
		SetCheck(hItem, 1);
	else
		SetCheck(hItem, 2);

	SetItemData(hItem, (DWORD)pTri);
	//------------------------------------------------------------------------

	std::vector<CGLObject*>::iterator iTerator;
	/*for (iTerator = m_pDoc->GetContext()->m_listDice->begin();iTerator != m_pDoc->GetContext()->m_listDice->end(); iTerator++)
	{
		CGLObject *pObj = (*iTerator);
		int nImage = 4, nSelectImage = 4;

		HTREEITEM hItem = InsertItem(pObj->GetObjName(),nImage,nSelectImage,hItem2);

		if( pObj->isShow())
			SetCheck(hItem, 1);
		else
			SetCheck(hItem, 2);

		SetItemData(hItem, (DWORD)pObj);		
	}*/

	for (iTerator = m_pDoc->GetContext()->m_listSlice->begin();iTerator != m_pDoc->GetContext()->m_listSlice->end(); iTerator++)
	{
		C3DSlice *pObj = (C3DSlice *)(*iTerator);
		int nImage = 0, nSelectImage = 0;
		
		HTREEITEM hItem;
		
		if( pObj->IsVertSlice() )
		{
			nImage = 6;
			nSelectImage = 6;
			hItem = InsertItem(pObj->GetObjName(),nImage,nSelectImage,hItem32);
		}
		else
		{
			nImage = 5;
			nSelectImage = 5;
			hItem = InsertItem(pObj->GetObjName(),nImage,nSelectImage,hItem31);
		}

		if( pObj->isShow())
			SetCheck(hItem, 1);
		else
			SetCheck(hItem, 2);

		SetItemData(hItem, (DWORD)pObj);		
	}
	
	for (iTerator = m_pDoc->GetContext()->m_listDisplay->begin();iTerator != m_pDoc->GetContext()->m_listDisplay->end(); iTerator++)
	{
		CGLObject *pObj = (*iTerator);
		int nImage = 0, nSelectImage = 0;

		switch(pObj->GetGLObjType())
		{		
		case GLSURFACE:
			{
				nImage = 7;
				nSelectImage = 7;
				hItem = InsertItem(pObj->GetObjName(),nImage,nSelectImage,hItem4, TVI_FIRST);
			}
			break;
		case GLINTERLAYERCELL:
			{
				nImage = 10;
				nSelectImage =10;
				hItem = InsertItem(pObj->GetObjName(),nImage,nSelectImage,hItem44, TVI_FIRST);

			}
			break;
		case GLPLANE:
			{
				nImage = 10;
				nSelectImage = 10;
				hItem = InsertItem(pObj->GetObjName(),nImage,nSelectImage,hItem41);
			}
			break;

		case GLPIPELINE:
		case GLCURVE:
			{
				nImage = 11;
				nSelectImage = 11;
				hItem = InsertItem(pObj->GetObjName(),nImage,nSelectImage,hItem42);
			}
			break;
		case GLPOINT:
			{
				nImage = 12;
				nSelectImage = 12;
				hItem = InsertItem(pObj->GetObjName(),nImage,nSelectImage,hItem43);
			}
			break;
		default:
			break;
		}		
		
		if( pObj->isShow())
			SetCheck(hItem, 1);
		else
			SetCheck(hItem, 2);

		SetItemData(hItem, (DWORD)pObj);

		if( nImage == 7 ) // 如果是网格模型
		{
			CGridObject *pGrid = (CGridObject *)pObj;

			HTREEITEM h = InsertItem(_T("X"),8,8,hItem);
			int n = 0;
			for (int i = 0; i < pGrid->I; i++)
			{
				CString str;
				str.Format(_T("#%d"), i+1);
				HTREEITEM hh = InsertItem(str,6,6,h);
				if( pGrid->m_bShowI[i] )
				{
					SetCheck(hh, 5);
					n++;
				}
				else
					SetCheck(hh, 3);
			}
			if( n == 0 )
				SetCheck(h, 3);
			else if( n < pGrid->I )
				SetCheck(h, 4);
			else
				SetCheck(h, 5);

			h = InsertItem(_T("Y"),8,8,hItem);
			n = 0;
			for (int i = 0; i < pGrid->J; i++)
			{
				CString str;
				str.Format(_T("#%d"), i+1);
				HTREEITEM hh = InsertItem(str,6,6,h);
				if( pGrid->m_bShowJ[i] )
				{
					SetCheck(hh, 5);
					n++;
				}
				else
					SetCheck(hh, 3);
			}
			if( n == 0 )
				SetCheck(h, 3);
			else if( n < pGrid->J )
				SetCheck(h, 4);
			else
				SetCheck(h, 5);

			h = InsertItem(_T("Z"),8,8,hItem);
			n = 0;
			for (int i = 0; i < pGrid->K; i++)
			{
				CString str;
				str.Format(_T("#%d"), i+1);
				HTREEITEM hh = InsertItem(str,5,5,h);
				if( pGrid->m_bShowK[i] )
				{
					SetCheck(hh, 5);
					n++;
				}
				else
					SetCheck(hh, 3);
			}
			if( n == 0 )
				SetCheck(h, 3);
			else if( n < pGrid->K )
				SetCheck(h, 4);
			else
				SetCheck(h, 5);


			h = InsertItem(_T("属性"),8,8,hItem);
			SetCheck(h, 6);
			for (int i=0; i<pGrid->m_vecPhyPara.GetSize(); i++)
			{				
				HTREEITEM hh = InsertItem(pGrid->m_vecPhyPara[i].m_strName,9,9,h);
				SetCheck(hh, 6);
				if( pGrid->m_vecPhyPara[i].m_bShow )
				{
					SetCheck(hh, 7);
					SetCheck(h, 7);
				}
			}
		}

		if(pObj->GetGLObjType()==GLINTERLAYERCELL)
		{
			InterLayerGridObject * pGrid = dynamic_cast<InterLayerGridObject*>(pObj);
			HTREEITEM h = InsertItem(_T("属性"),8,8,hItem);
			SetCheck(h, 6);
			for (int i=0; i<pGrid->m_vecPhyPara.GetSize(); i++)
			{				
				HTREEITEM hh = InsertItem(pGrid->m_vecPhyPara[i].m_strName,9,9,h);
				SetCheck(hh, 6);
				if( pGrid->m_vecPhyPara[i].m_bShow )
				{
					SetCheck(hh, 7);
					SetCheck(h, 7);
				}
			}
		}
	}

	if( IsAllChildHide(hItem1))
		SetCheck(hItem1, 2);
	else
		SetCheck(hItem1, 1);
	
	/*if( IsAllChildHide(hItem2))
		SetCheck(hItem2, 2);
	else
		SetCheck(hItem2, 1);
		*/

	if( IsAllChildHide(hItem31))
		SetCheck(hItem31, 2);
	else
		SetCheck(hItem31, 1);
	
	if( IsAllChildHide(hItem32))
		SetCheck(hItem32, 2);
	else
		SetCheck(hItem32, 1);

	if( IsAllChildHide(hItem3))
		SetCheck(hItem3, 2);
	else
		SetCheck(hItem3, 1);

	if( IsAllChildHide(hItem4))
		SetCheck(hItem4, 2);
	else
		SetCheck(hItem4, 1);

	Expand(hItem1, TVE_EXPAND);
//	Expand(hItem2, TVE_EXPAND);
	Expand(hItem3, TVE_EXPAND);
	Expand(hItem31, TVE_EXPAND);
	Expand(hItem32, TVE_EXPAND);
	Expand(hItem4, TVE_EXPAND);
}

HTREEITEM C3DObjTreeCtrl::GetChildNode(HTREEITEM hti, LPCSTR lpszNodeName, BOOL bCase)
{
	CString strNodeName = lpszNodeName;

	if( !bCase ) // 不区分大小
		strNodeName.MakeLower();

	HTREEITEM hSiblingItem = GetChildItem(hti);

	while(hSiblingItem != NULL)
	{
		CString strItem = GetItemText(hSiblingItem);

		if( !bCase ) // 不区分大小
			strItem.MakeLower();

		if( strItem == strNodeName )
			return hSiblingItem;
		hSiblingItem = GetNextSiblingItem(hSiblingItem);
	}

	return NULL;
}

void C3DObjTreeCtrl::AddObj(CGLObject *pObj)
{
	if( m_pDoc == NULL )
		return;

	//HTREEITEM hSrc2 = GetChildNode(TVI_ROOT, _T("图形切块"));
	HTREEITEM hSrc3 = GetChildNode(TVI_ROOT, _T("图形切片"));
	HTREEITEM hSrc31 = GetChildNode(hSrc3, _T("横切片"));
	HTREEITEM hSrc32 = GetChildNode(hSrc3, _T("纵切片"));
	HTREEITEM hSrc4 = GetChildNode(TVI_ROOT, _T("图形对象"));
	HTREEITEM hSrc41 = GetChildNode(hSrc4, _T("面"));
	HTREEITEM hSrc42 = GetChildNode(hSrc4, _T("线"));
	HTREEITEM hSrc43 = GetChildNode(hSrc4, _T("点"));
	HTREEITEM hSrc44 = GetChildNode(hSrc4, _T("夹层网格"));

	HTREEITEM hItem;
	int nImage = 0, nSelectImage = 0;
	switch(pObj->GetGLObjType())
	{
	/*case GLDICE:
		{
			nImage = 4;
			nSelectImage = 4;
			hItem = InsertItem(pObj->GetObjName(),nImage,nSelectImage, hSrc2);
		}
		break;
		*/
	case GLSLICE:
		{
			C3DSlice *pSlice = (C3DSlice*)pObj;
			if( pSlice->IsVertSlice() )
			{
				nImage = 6;
				nSelectImage = 6;
				hItem = InsertItem(pObj->GetObjName(),nImage,nSelectImage, hSrc32);
			}
			else
			{
				nImage = 5;
				nSelectImage = 5;
				hItem = InsertItem(pObj->GetObjName(),nImage,nSelectImage, hSrc31);
			}
		}
		break;
	case GLSURFACE:
		{
			nImage = 7;
			nSelectImage = 7;
			hItem = InsertItem(pObj->GetObjName(),nImage,nSelectImage, hSrc4, TVI_FIRST);

			CGridObject *pGrid = (CGridObject *)pObj;

			HTREEITEM h = InsertItem(_T("X"),8,8,hItem);
			int n = 0;
			for (int i = 0; i < pGrid->I; i++)
			{
				CString str;
				str.Format(_T("#%d"), i+1);
				HTREEITEM hh = InsertItem(str,6,6,h);
				if( pGrid->m_bShowI[i] )
				{
					SetCheck(hh, 5);
					n++;
				}
				else
					SetCheck(hh, 3);
			}
			if( n == 0 )
				SetCheck(h, 3);
			else if( n < pGrid->I )
				SetCheck(h, 4);
			else
				SetCheck(h, 5);

			h = InsertItem(_T("Y"),8,8,hItem);
			n = 0;
			for (int i = 0; i < pGrid->J; i++)
			{
				CString str;
				str.Format(_T("#%d"), i+1);
				HTREEITEM hh = InsertItem(str,6,6,h);
				if( pGrid->m_bShowJ[i] )
				{
					SetCheck(hh, 5);
					n++;
				}
				else
					SetCheck(hh, 3);
			}
			if( n == 0 )
				SetCheck(h, 3);
			else if( n < pGrid->J )
				SetCheck(h, 4);
			else
				SetCheck(h, 5);

			h = InsertItem(_T("Z"),8,8,hItem);
			n = 0;
			for (int i = 0; i < pGrid->K; i++)
			{
				CString str;
				str.Format(_T("#%d"), i+1);
				HTREEITEM hh = InsertItem(str,5,5,h);
				if( pGrid->m_bShowK[i] )
				{
					SetCheck(hh, 5);
					n++;
				}
				else
					SetCheck(hh, 3);
			}
			if( n == 0 )
				SetCheck(h, 3);
			else if( n < pGrid->K )
				SetCheck(h, 4);
			else
				SetCheck(h, 5);


			h = InsertItem(_T("属性"),8,8,hItem);
			SetCheck(h, 6);
			for (int i=0; i<pGrid->m_vecPhyPara.GetSize(); i++)
			{				
				HTREEITEM hh = InsertItem(pGrid->m_vecPhyPara[i].m_strName,9,9,h);
				SetCheck(hh, 6);
				if( pGrid->m_vecPhyPara[i].m_bShow )
				{
					SetCheck(hh, 7);
					SetCheck(h, 7);
				}
			}
		}
		break;
	case GLPLANE:
		{
			nImage = 10;
			nSelectImage = 10;
			hItem = InsertItem(pObj->GetObjName(),nImage,nSelectImage, hSrc41);
		}
		break;
	case GLINTERLAYERCELL:
		{
			nImage = 10;
			nSelectImage = 10;
			hItem = InsertItem(pObj->GetObjName(),nImage,nSelectImage, hSrc44);
		}
		break;

	case GLPIPELINE:
	case GLCURVE:
		{
			nImage = 11;
			nSelectImage = 11;
			hItem = InsertItem(pObj->GetObjName(),nImage,nSelectImage, hSrc42);
		}
		break;
	case GLPOINT:
		{
			nImage = 12;
			nSelectImage = 12;
			hItem = InsertItem(pObj->GetObjName(),nImage,nSelectImage, hSrc43);
		}
		break;
	default:
		break;
	}

	if( pObj->isShow())
		SetCheck(hItem, 1);
	else
		SetCheck(hItem, 2);

	SetItemData(hItem, (DWORD)pObj);

	Select(hItem, TVGN_CARET);

	/*if( pObj->GetGLObjType() == GLDICE )
		Expand(hSrc2, TVE_EXPAND);
	else*/
	if( pObj->GetGLObjType() == GLSLICE )
	{
		Expand(hSrc3, TVE_EXPAND);

		C3DSlice *pSlice = (C3DSlice*)pObj;
		if( pSlice->IsVertSlice() )
			Expand(hSrc32, TVE_EXPAND);
		else
			Expand(hSrc31, TVE_EXPAND);
	}
	else
		Expand(hSrc4, TVE_EXPAND);
}

BEGIN_MESSAGE_MAP(C3DObjTreeCtrl, CTreeCtrl)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnNMRClick)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnTvnBeginlabeledit)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnTvnEndlabeledit)
	ON_MESSAGE_VOID(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &C3DObjTreeCtrl::OnNMDblclk)
END_MESSAGE_MAP()

// C3DObjTreeCtrl 消息处理程序
BOOL C3DObjTreeCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT(pNMHDR != NULL);

	if (pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != NULL)
	{
		GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}

//------------------------------------------------------------------------
// 
int C3DObjTreeCtrl::GetCheck(HTREEITEM hItem)
{
	TVITEM tvItem;

	// Prepare to receive the desired information.
	tvItem.mask = TVIF_HANDLE | TVIF_STATE;
	tvItem.hItem = hItem;
	tvItem.stateMask = TVIS_STATEIMAGEMASK;

	TreeView_GetItem(this->m_hWnd, &tvItem);

	return ((tvItem.state >> 12));
}

BOOL C3DObjTreeCtrl::SetCheck(HTREEITEM hItem, UINT uCheck)
{
	TVITEM tvItem;

	tvItem.mask = TVIF_HANDLE | TVIF_STATE;
	tvItem.hItem = hItem;
	tvItem.stateMask = TVIS_STATEIMAGEMASK;

	tvItem.state = INDEXTOSTATEIMAGEMASK(uCheck);

	return TreeView_SetItem(m_hWnd, &tvItem);
}

void C3DObjTreeCtrl::SetItemAndChildren(HTREEITEM hItem,UINT uCheck )
{
	SetCheck(hItem, uCheck);
	HTREEITEM hChildItem = GetChildItem(hItem);


	POSITION pos = m_pDoc->GetFirstViewPosition();
	C3DModelView *pView = (C3DModelView *)m_pDoc->GetNextView(pos);


	while(hChildItem)
	{
		int nCheck = GetCheck(hChildItem);
		if( (nCheck<3&&uCheck<3)
			|| (nCheck>=3&&uCheck>=3&&nCheck<=5&&uCheck<=5)
			|| (nCheck>5&&uCheck>5));
		else
			return;

		SetCheck(hChildItem, uCheck);
		CGLObject *pObj = (CGLObject*)GetItemData(hChildItem);

		if( pObj!=NULL )
		{
			if(pObj->GetGLObjType() != GLTRIHEDRON)
				pObj->Show(uCheck!=2);
			else
			{
				CGLTrihedron *pTri = (CGLTrihedron*)pObj;
				CString strTemp = GetItemText(hChildItem);
				if( strTemp == _T("坐标框"))
					pView->m_myView->ShowAxis(uCheck!=2);
				else if( strTemp == _T("坐标网格"))
					pView->m_myView->ShowGird(uCheck!=2);
				else if( strTemp == _T("图例色条"))
					pView->m_myView->ShowLegend(uCheck!=2);
			}
			pObj->SetModified(true);

			POSITION pos = m_pDoc->GetFirstViewPosition();
			CView *pView = m_pDoc->GetNextView(pos);
			pView->Invalidate(FALSE);

			m_pDoc->SetModifiedFlag();
		}
		SetItemAndChildren(hChildItem,uCheck);
		hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
	}
}


int C3DObjTreeCtrl::IsAllChildHide(HTREEITEM hItem)
{
	hItem = GetChildItem(hItem);

	while(hItem)
	{
		int nCheck = GetCheck(hItem);
		if( nCheck == 1	 // 有一个显示
			||  nCheck == 5
			||  nCheck == 7
			)
			return 0;

		hItem = GetNextItem(hItem, TVGN_NEXT);	
	}

	return 1;
}

int C3DObjTreeCtrl::IsAllChildShow(HTREEITEM hItem)
{
	hItem = GetChildItem(hItem);

	while(hItem)
	{
		int nCheck = GetCheck(hItem);
		if( nCheck == 2	 // 有一个不显示
			||  nCheck == 3
			||  nCheck == 4
			||  nCheck == 6
			)
			return 0;

		hItem = GetNextItem(hItem, TVGN_NEXT);	
	}

	return 1;
}

void C3DObjTreeCtrl::OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	SetFocus();
	*pResult = 0;
}


void C3DObjTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值	

	UINT uFlags = 0;
	HTREEITEM hItem = HitTest(point, &uFlags);

	POSITION pos = m_pDoc->GetFirstViewPosition();
	C3DModelView *pView = (C3DModelView *)m_pDoc->GetNextView(pos);

	if( hItem != NULL && uFlags & TVHT_ONITEMSTATEICON)
	{
		int nCheck = GetCheck(hItem);

		CGLObject *pObj = (CGLObject*)GetItemData(hItem);

		if( pObj!=NULL )		// 是图形对像
		{
			if(pObj->GetGLObjType() != GLTRIHEDRON)
				pObj->Show(nCheck==2);
			else
			{
				CGLTrihedron *pTri = (CGLTrihedron*)pObj;
				CString strTemp = GetItemText(hItem);
				if( strTemp == _T("坐标框"))
					pView->m_myView->ShowAxis(nCheck==2);
				else if( strTemp == _T("坐标网格"))
					pView->m_myView->ShowGird(nCheck==2);
				else if( strTemp == _T("图例色条"))
					pView->m_myView->ShowLegend(nCheck==2);
			}

			pObj->SetModified(true);

			POSITION pos = m_pDoc->GetFirstViewPosition();
			CView *pView = m_pDoc->GetNextView(pos);
			pView->Invalidate(FALSE);

			m_pDoc->SetModifiedFlag();

			if( nCheck == 2 )
				SetCheck(hItem, 1);
			else
				SetCheck(hItem, 2);

			//CTreeCtrl::OnLButtonDown(nFlags, point);

			HTREEITEM hParent = GetParentItem(hItem);
			if( hParent != NULL )
			{
				if( IsAllChildHide(hParent))
					SetCheck(hParent, 2);
				else
					SetCheck(hParent, 1);
			}
			hParent = GetParentItem(hParent);
			if( hParent != NULL )
			{
				if( IsAllChildHide(hParent))
					SetCheck(hParent, 2);
				else
					SetCheck(hParent, 1);
			}
			return;
		}
		else // 不是图形对像
		{
			if( nCheck == 2 )
				SetItemAndChildren(hItem, 1);
			else if( nCheck == 1)
				SetItemAndChildren(hItem, 2);
			else if( nCheck == 3 || nCheck == 4 )
			{
				CGridObject *pGridObj = (CGridObject*)GetItemData(GetParentItem(hItem));

				if( pGridObj==NULL )
					pGridObj = (CGridObject*)GetItemData(GetParentItem(GetParentItem(hItem)));
				
				CString str = GetItemText(hItem);

				if(str == _T("X") )
				{
					for (int i=0;i<pGridObj->I; i++)
						pGridObj->m_bShowI[i] = TRUE;
				}
				else if(str == _T("Y") )
				{
					for (int i=0;i<pGridObj->J; i++)
						pGridObj->m_bShowJ[i] = TRUE;
				}
				else if( str == _T("Z"))
				{
					for (int i=0;i<pGridObj->K; i++)
						pGridObj->m_bShowK[i] = TRUE;
					//for (int i=0;i<pGridObj->K;i++)
					//	pGridObj->m_bChangeK[i] = TRUE;
				}
				else
				{
					CString strNum = str;
					strNum.Replace(_T("#"), _T(""));
					int num = atoi(strNum)-1;

					if( num > -1 )
					{
						str = GetItemText(GetParentItem(hItem));
						if(str == _T("X") )
						{
							pGridObj->m_bShowI[num] = TRUE;
						}
						else if(str == _T("Y") )
						{
							pGridObj->m_bShowJ[num] = TRUE;
						}
						else if( str == _T("Z"))
						{
							pGridObj->m_bShowK[num] = TRUE;
							//pGridObj->m_bChangeK[num] = TRUE;
						}
					}
				}

				SetItemAndChildren(hItem, 5);

				pGridObj->SetModified(true);

				POSITION pos = m_pDoc->GetFirstViewPosition();
				CView *pView = m_pDoc->GetNextView(pos);
				pView->Invalidate(FALSE);

				m_pDoc->SetModifiedFlag();
			}
			else if(nCheck == 5 )
			{
				CGridObject *pGridObj = (CGridObject*)GetItemData(GetParentItem(hItem));

				if( pGridObj==NULL )
					pGridObj = (CGridObject*)GetItemData(GetParentItem(GetParentItem(hItem)));

				CString str = GetItemText(hItem);

				if(str == _T("X") )
				{
					for (int i=0;i<pGridObj->I; i++)
						pGridObj->m_bShowI[i] = FALSE;
				}
				else if(str == _T("Y") )
				{
					for (int i=0;i<pGridObj->J; i++)
						pGridObj->m_bShowJ[i] = FALSE;
				}
				else if( str == _T("Z"))
				{
					for (int i=0;i<pGridObj->K; i++)
						pGridObj->m_bShowK[i] = FALSE;
					//for (int i=0;i<pGridObj->K; i++)
					//	pGridObj->m_bChangeK[i] = TRUE;
				}
				else
				{
					CString strNum = str;
					strNum.Replace(_T("#"), _T(""));
					int num = atoi(strNum)-1;

					if( num > -1 )
					{
						str = GetItemText(GetParentItem(hItem));
						if(str == _T("X") )
						{
							pGridObj->m_bShowI[num] = FALSE;
						}
						else if(str == _T("Y") )
						{
							pGridObj->m_bShowJ[num] = FALSE;
						}
						else if( str == _T("Z"))
						{
							pGridObj->m_bShowK[num] = FALSE;
							//pGridObj->m_bChangeK[num] = TRUE;
						}
					}

				}

				SetItemAndChildren(hItem, 3);

				pGridObj->SetModified(true);

				POSITION pos = m_pDoc->GetFirstViewPosition();
				CView *pView = m_pDoc->GetNextView(pos);
				pView->Invalidate(FALSE);

				m_pDoc->SetModifiedFlag();
			}
			else if(nCheck == 6 )
			{
				CGridObject *pGridObj = (CGridObject*)GetItemData(GetParentItem(hItem));

				if( pGridObj==NULL )
					pGridObj = (CGridObject*)GetItemData(GetParentItem(GetParentItem(hItem)));

				HTREEITEM h = hItem;

				SetCheck(hItem, 7);

				HTREEITEM hParent = GetParentItem(hItem);
				int nParentCheck = GetCheck(hParent);
				if( nParentCheck>5)
				{
					SetCheck(hParent, 7);
					HTREEITEM hChild = GetChildItem(hParent);
					while(hChild!= NULL)
					{
						if( hChild!=hItem )
							SetCheck(hChild, 6);
						hChild = GetNextSiblingItem(hChild);
					}
				}

				HTREEITEM hChild = GetChildItem(hItem);
				if(hChild!= NULL)
				{
					SetCheck(hChild, 7);
					h = hChild;
				}

				CString strParam = GetItemText(h);

				int nSize = pGridObj->m_vecPhyPara.GetSize();
				for (int i=0; i<nSize; i++)
				{
					if( pGridObj->m_vecPhyPara[i].m_strName == strParam )
					{
						pGridObj->m_vecPhyPara[i].m_bShow = TRUE;
						pGridObj->SetModified(true);
					}
					else
					{
						pGridObj->m_vecPhyPara[i].m_bShow = FALSE;
					}
				}

				POSITION pos = m_pDoc->GetFirstViewPosition();
				CView *pView = m_pDoc->GetNextView(pos);
				pView->Invalidate(FALSE);

				m_pDoc->SetModifiedFlag();
			}
			else if(nCheck == 7 )
			{
				CGridObject *pGridObj = (CGridObject*)GetItemData(GetParentItem(hItem));

				if( pGridObj==NULL )
					pGridObj = (CGridObject*)GetItemData(GetParentItem(GetParentItem(hItem)));

				SetCheck(hItem, 6);
				HTREEITEM hChild = GetChildItem(hItem);
				while(hChild!= NULL)
				{
					if( hChild!=hItem )
						SetCheck(hChild, 6);
					hChild = GetNextSiblingItem(hChild);
				}

				HTREEITEM hParent = GetParentItem(hItem);

				int nParentCheck = GetCheck(hParent);
				if( nParentCheck>5)
					SetCheck(hParent, 6);

				int nSize = pGridObj->m_vecPhyPara.GetSize();
				for (int i=0; i<nSize; i++)
				{
					pGridObj->m_vecPhyPara[i].m_bShow = FALSE;
					pGridObj->SetModified(true);
				}
				
				POSITION pos = m_pDoc->GetFirstViewPosition();
				CView *pView = m_pDoc->GetNextView(pos);
				pView->Invalidate(FALSE);

				m_pDoc->SetModifiedFlag();
			}

			HTREEITEM hParent = GetParentItem(hItem);

			int nParentCheck = GetCheck(hParent);

			if( hParent != NULL )
			{
				if( IsAllChildHide(hParent))
				{
					if( nParentCheck<3 && nCheck<3)
						SetCheck(hParent, 2);
					else if( nParentCheck>=3 && nCheck>=3&& nParentCheck<6 && nCheck<6)
						SetCheck(hParent, 3);
				}
				else
				{
					if( nParentCheck<3 && nCheck<3)
						SetCheck(hParent, 1);
					else if( nParentCheck>=3 && nCheck>=3 && nParentCheck<6 && nCheck<6)
					{
						if(IsAllChildShow(hParent))
							SetCheck(hParent, 5);
						else
							SetCheck(hParent, 4);
					}
				}
			}

			return;
		}
	}

	CTreeCtrl::OnLButtonDown(nFlags, point);
}

void C3DObjTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CTreeCtrl::OnLButtonUp(nFlags, point);
}
BOOL C3DObjTreeCtrl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (GetEditControl())
	{
		::TranslateMessage(pMsg);
		::DispatchMessage(pMsg);
		return TRUE; // DO NOT process further
	}
	if (pMsg->message == WM_KEYDOWN) 
	{
		switch(pMsg->wParam)
		{
		case VK_F2:
			OnRename();
			break;
		case VK_DELETE:
			OnDeleteItem();
			break;
		default:
			// Default behaviour for other keys such as arrows
			return CTreeCtrl::PreTranslateMessage(pMsg);
		}
		return TRUE; // DO NOT process further
	}

	return CTreeCtrl::PreTranslateMessage(pMsg);
}

void C3DObjTreeCtrl::OnIdleUpdateCmdUI()
{
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	CMDIChildWndEx *pWnd =(CMDIChildWndEx *) pMF->MDIGetActive();
	if( !pWnd )
	{
		DeleteAllItems();
		m_pDoc = NULL;
		return;
	}

	C3DModelView *pView = (C3DModelView *)pWnd->GetActiveView();
	if(!pView)
	{
		DeleteAllItems();
		m_pDoc = NULL;
		return;
	}

	if(!pView->IsKindOf(RUNTIME_CLASS(C3DModelView)))
	{
		DeleteAllItems();
		m_pDoc = NULL;
		return;
	}

	C3DModelDoc *pDoc = (C3DModelDoc *)pView ->GetDocument();

	if(m_pDoc != pDoc )
	{
		m_pDoc = pDoc;
		FillTreeCtrl();
	}
}

void C3DObjTreeCtrl::OnTvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	CEdit *pEdit;
	pEdit = GetEditControl();
	pEdit->SetLimitText(1024);

	*pResult = 0;
}

void C3DObjTreeCtrl::OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	if (pTVDispInfo->item.pszText != NULL)
	{
		TVITEM tvi;
		tvi.hItem = pTVDispInfo->item.hItem;		
		pTVDispInfo->item.mask = TVIF_TEXT;
		SetItem(&pTVDispInfo->item);
		m_pDoc->SetModifiedFlag();

		CGLObject *pObj = (CGLObject*)GetItemData(pTVDispInfo->item.hItem);
		pObj->SetObjName(pTVDispInfo->item.pszText);
	}

	*pResult = TRUE; // TRUE => Accept the label

	//*pResult = 0;
}


//________________________________________________________________________
// 删除节点
void C3DObjTreeCtrl::OnEditClear()
{
	// TODO: 在此添加命令处理程序代码
	OnDeleteItem();
}

void C3DObjTreeCtrl::OnUpdateEditClear(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	//HTREEITEM hItem = GetSelectedItem();	
	pCmdUI->Enable(TRUE);
}

BOOL C3DObjTreeCtrl::IsExistChildNode(HTREEITEM hti, LPCSTR lpszNodeName, BOOL bCase)
{
	CString strNodeName = lpszNodeName;

	if( !bCase ) // 不区分大小
		strNodeName.MakeLower();

	HTREEITEM hSiblingItem = GetChildItem(hti);

	while(hSiblingItem != NULL)
	{
		CString strItem = GetItemText(hSiblingItem);

		if( !bCase ) // 不区分大小
			strItem.MakeLower();

		if( strItem == strNodeName )
			return TRUE;
		hSiblingItem = GetNextSiblingItem(hSiblingItem);
	}

	return FALSE;
}

void C3DObjTreeCtrl::OnNew()
{
	HTREEITEM hti;
	hti = GetSelectedItem();
	if( hti != NULL )
	{
		CGLObject *pObj = (CGLObject*)GetItemData(hti);
		if( pObj==NULL )
		{
			CString strText = GetItemText(hti);
			if( strText == _T("纵切片") )
			{
				CDlgXYSlice dlg;
				CBoundingBox box = m_pDoc->GetContext()->GetBoundingBox();

				dlg.m_dMinX = box.XMin();
				dlg.m_dMaxX = box.XMax();
				dlg.m_dMinY = box.YMin();
				dlg.m_dMaxY = box.YMax();

				double xRange = box.XMax()-box.XMin();
				double yRange = box.YMax()-box.YMin();

				double range = (xRange>yRange?xRange:yRange);

				
				dlg.m_dX1 = (box.XMin()+box.XMax())/2.0-range/2;
				dlg.m_dY1 = (box.YMin()+box.YMax())/2.0;

				dlg.m_dX2 = (box.XMin()+box.XMax())/2.0+range/2;
				dlg.m_dY2 = (box.YMin()+box.YMax())/2.0;

				double xMin, xMax, yMin, yMax;
				xMin = (box.XMin()+box.XMax())/2.0-range/2;
				xMax = (box.XMin()+box.XMax())/2.0+range/2;
				yMin = (box.YMin()+box.YMax())/2.0-range/2;
				yMax = (box.YMin()+box.YMax())/2.0+range/2;
				
				if( dlg.DoModal() == IDOK )
				{
					CString strName;
					int num = 0;
					do 
					{
						if(num>0)
							strName.Format(_T("新切片 %d"),num);
						else
							strName = _T("新切片");
						num++;
					} while(IsExistChildNode(hti,strName));

					C3DSlice *pSlice = new C3DSlice;

					// (x2-x1) * (y-y1) = (y2-y1) * (x-x1)
					// 
					// 设原始方程为
					//   A*X + B * Y + C = 0
					//   
					// 其中
					//   A = ( y2 - y1 )
					//   B = ( x1 - x2 )
					//   C = - ( y1*B + A * x1 );
					double x1 = (dlg.m_dX1-xMin)/range*2.0-1.0;
					double y1 = (dlg.m_dY1-yMin)/range*2.0-1.0;
					double x2 = (dlg.m_dX2-xMin)/range*2.0-1.0;
					double y2 = (dlg.m_dY2-yMin)/range*2.0-1.0;

					pSlice->A = y2-y1;
					pSlice->B = x1-x2;
					pSlice->C = 0.0;
					pSlice->D = - ( y1*pSlice->B + pSlice->A * x1 );
					pSlice->m_bSpecified = dlg.m_bSpecified;
					pSlice->m_dHeight = dlg.m_dHeight;
					pSlice->SetObjName(strName);
					pSlice->Show(dlg.m_bShow);
					pSlice->m_bShowSlicePlane = dlg.m_bShowSlicePlane;

					m_pDoc->GetContext()->AddGLObj(pSlice);

					//delete pSlice;
				}
			}
			else if(strText == _T("横切片"))
			{
				CDlgZSlice dlg;
				dlg.m_dMinZ = m_pDoc->GetContext()->GetBoundingBox().ZMin();
				dlg.m_dMaxZ = m_pDoc->GetContext()->GetBoundingBox().ZMax();
				if( dlg.DoModal() == IDOK )
				{
					CString strName;
					int num = 0;
					do 
					{
						if(num>0)
							strName.Format(_T("新切片 %d"),num);
						else
							strName = _T("新切片");
						num++;
					} while(IsExistChildNode(hti,strName));

					C3DSlice *pSlice = new C3DSlice;
					pSlice->A = 0.0;
					pSlice->B = 0.0;
					pSlice->C = 1.0;
					pSlice->D = -((dlg.m_dZ-dlg.m_dMinZ)/(dlg.m_dMaxZ-dlg.m_dMinZ)*2.0-1.0);
					pSlice->m_bSpecified = dlg.m_bSpecified;
					pSlice->m_dHeight = dlg.m_dHeight;
					pSlice->SetObjName(strName);
					pSlice->Show(dlg.m_bShow);
					pSlice->m_bShowSlicePlane = dlg.m_bShowSlicePlane;

					m_pDoc->GetContext()->AddGLObj(pSlice);

					//delete pSlice;
				}
			}
		}
	}
}

void C3DObjTreeCtrl::OnUpdateNew(CCmdUI *pCmdUI)
{
	HTREEITEM hti;
	hti = GetSelectedItem();
	if( hti != NULL )
	{
		CGLObject *pObj = (CGLObject*)GetItemData(hti);
		if( pObj==NULL )
		{
			CString strText = GetItemText(hti);
			if( strText == _T("纵切片")
				|| strText == _T("横切片"))
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

//------------------------------------------------------------------------
// 重命名
void C3DObjTreeCtrl::OnRename() 
{
	HTREEITEM hti;
	hti = GetSelectedItem();
	if( hti != NULL )
	{
		CGLObject *pObj = (CGLObject*)GetItemData(hti);
		if( pObj==NULL || pObj->GetGLObjType() == GLTRIHEDRON )
			return;
		EditLabel(hti);
	}
}

void C3DObjTreeCtrl::OnUpdateRename(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void C3DObjTreeCtrl::OnDeleteItem()
{
	HTREEITEM hItem = GetSelectedItem();

	if( hItem != NULL )
	{
		CGLObject *pObj = (CGLObject*)GetItemData(hItem);
		CMainFrame *pMF = (CMainFrame *)AfxGetMainWnd();
		//pMF->GetTreeModelView()->DeleteItemByGUID(pObj->m_strGUID);
		if( pObj == NULL || pObj->GetGLObjType() == GLTRIHEDRON )
			return;
		
		if( ::MessageBox(m_hWnd,_T("确定要删除吗？"),_T("提示信息"),MB_YESNO|MB_ICONQUESTION) == IDNO )
			return;

		DeleteItem(hItem);

		if( pObj->GetGLObjType() == GLDICE )
			m_pDoc->GetContext()->EraseDice(pObj);
		else if (pObj->GetGLObjType() == GLSLICE)
			m_pDoc->GetContext()->EraseSlice(pObj);
		else
			m_pDoc->GetContext()->Erase(pObj);

		POSITION pos = m_pDoc->GetFirstViewPosition();
		CView *pView = m_pDoc->GetNextView(pos);
		pView->Invalidate(FALSE);

		m_pDoc->SetModifiedFlag();

		SetFocus();
	}
}

void C3DObjTreeCtrl::OnSetMaterial()
{
	HTREEITEM hti;
	hti = GetSelectedItem();
	if( hti != NULL )
	{
		CGLObject *pObj = (CGLObject*)GetItemData(hti);
		if( pObj==NULL 
			|| pObj->GetGLObjType() == GLTRIHEDRON 
			|| pObj->GetGLObjType() == GLDICE
			|| pObj->GetGLObjType() == GLSLICE)
			return;

		CDlgMaterial dlg;
		CMaterial ma;
		CMaterial *p = pObj->GetMaterial();
		ma = (*p);

		dlg.m_pDoc = m_pDoc;
		dlg.m_pObj = pObj;

		BOOL bModified = m_pDoc->IsModified();

		if(dlg.DoModal() == IDOK)
		{
			if( ma != dlg.m_Matl && !dlg.m_bPreview )
			{
				pObj->SetMaterial(dlg.m_Matl);
				pObj->GetContext()->SetModifiedFlag();
				m_pDoc->UpdateAllViews(NULL);
			}
		}
		else
		{
			if( ma != dlg.m_Matl && dlg.m_bPreview )
			{
				pObj->SetMaterial(ma);
				pObj->GetContext()->SetModifiedFlag();
				m_pDoc->UpdateAllViews(NULL);

				m_pDoc->SetModifiedFlag(bModified);
			}
		}
	}
}

void C3DObjTreeCtrl::OnUpdateSetMaterial(CCmdUI *pCmdUI)
{
	HTREEITEM hti;
	hti = GetSelectedItem();
	if( hti != NULL )
	{
		CGLObject *pObj = (CGLObject*)GetItemData(hti);
		if( pObj==NULL
			|| pObj->GetGLObjType() == GLTRIHEDRON 
			|| pObj->GetGLObjType() == GLDICE
			|| pObj->GetGLObjType() == GLSLICE)
			pCmdUI->Enable(FALSE);
		else
			pCmdUI->Enable(TRUE);
	}
	else
		pCmdUI->Enable(FALSE);
}

void C3DObjTreeCtrl::OnSetColor()
{
	HTREEITEM hti;
	hti = GetSelectedItem();
	if( hti != NULL )
	{
		CGLObject *pObj = (CGLObject*)GetItemData(hti);
		if( pObj==NULL
			|| pObj->GetGLObjType() == GLTRIHEDRON 
			|| pObj->GetGLObjType() == GLDICE
			|| pObj->GetGLObjType() == GLSLICE)
			return;

		CMFCColorDialog dlg;
		dlg.SetCurrentColor(RGB(pObj->GetColor()->GetR(), pObj->GetColor()->GetG(), pObj->GetColor()->GetB()));
		if (dlg.DoModal() == IDOK)
		{
			COLORREF color = dlg.GetColor();
			GLfloat R = (GLfloat)GetRValue(color);
			GLfloat G = (GLfloat)GetGValue(color);
			GLfloat B = (GLfloat)GetBValue(color);
			pObj->SetColor(R, G, B);
			pObj->GetContext()->SetModifiedFlag();
			m_pDoc->UpdateAllViews(NULL);
		}
	}
}

void C3DObjTreeCtrl::OnUpdateSetColor(CCmdUI *pCmdUI)
{
	HTREEITEM hti;
	hti = GetSelectedItem();
	if( hti != NULL )
	{
		CGLObject *pObj = (CGLObject*)GetItemData(hti);
		if( pObj==NULL
			|| pObj->GetGLObjType() == GLTRIHEDRON 
			|| pObj->GetGLObjType() == GLDICE
			|| pObj->GetGLObjType() == GLSLICE)
			pCmdUI->Enable(FALSE);
		else
			pCmdUI->Enable(TRUE);
	}
	else
		pCmdUI->Enable(FALSE);
}

void C3DObjTreeCtrl::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	HTREEITEM hti;
	hti = GetSelectedItem();
	if( hti != NULL )
	{
		CGLObject *pObj = (CGLObject*)GetItemData(hti);
		if( pObj==NULL )
			*pResult = 0;
		else if(pObj->GetGLObjType() == GLTRIHEDRON )
			*pResult = 0;
		else if(pObj->GetGLObjType() == GLDICE)
			*pResult = -1;
		else if(pObj->GetGLObjType() == GLSLICE)
		{
			CString strText = GetItemText(GetParentItem(hti));
			if( strText == _T("横切片"))
			{
				C3DSlice *pSlice = (C3DSlice*)pObj;
				CDlgZSlice dlg;
				dlg.m_dMinZ = m_pDoc->GetContext()->GetBoundingBox().ZMin();
				dlg.m_dMaxZ = m_pDoc->GetContext()->GetBoundingBox().ZMax();

				dlg.m_dZ = dlg.m_dMinZ - (pSlice->D-1.0)/2.0*(dlg.m_dMaxZ-dlg.m_dMinZ);
				dlg.m_bSpecified = pSlice->m_bSpecified;
				dlg.m_dHeight = pSlice->m_dHeight;
				dlg.m_bShow = pSlice->isShow();

				dlg.m_strTitle = _T("横切片 - ");
				dlg.m_strTitle += pSlice->GetObjName();

				dlg.m_bShowSlicePlane = pSlice->m_bShowSlicePlane;

				if( dlg.DoModal() == IDOK )
				{
					pSlice->A = 0.0;
					pSlice->B = 0.0;
					pSlice->C = 1.0;
					pSlice->D = -((dlg.m_dZ-dlg.m_dMinZ)/(dlg.m_dMaxZ-dlg.m_dMinZ)*2.0-1.0);
					pSlice->m_bSpecified = dlg.m_bSpecified;
					pSlice->m_dHeight = dlg.m_dHeight;
					pSlice->Show(dlg.m_bShow);
					pSlice->m_bShowSlicePlane = dlg.m_bShowSlicePlane;

					m_pDoc->SetModifiedFlag();

					m_pDoc->UpdateAllViews(NULL);

					if( dlg.m_bShow )
						SetCheck(hti, 1);
					else
						SetCheck(hti, 2);

					if( IsAllChildHide(GetParentItem(hti)))
						SetCheck(GetParentItem(hti), 2);
					else
						SetCheck(GetParentItem(hti), 1);

					if( IsAllChildHide(GetParentItem(GetParentItem(hti))))
						SetCheck(GetParentItem(GetParentItem(hti)), 2);
					else
						SetCheck(GetParentItem(GetParentItem(hti)), 1);
				}
			}
			else
			{
				C3DSlice *pSlice = (C3DSlice*)pObj;
				CDlgXYSlice dlg;
				CBoundingBox box = m_pDoc->GetContext()->GetBoundingBox();

				dlg.m_dMinX = box.XMin();
				dlg.m_dMaxX = box.XMax();
				dlg.m_dMinY = box.YMin();
				dlg.m_dMaxY = box.YMax();

				double xRange = box.XMax()-box.XMin();
				double yRange = box.YMax()-box.YMin();

				double range = (xRange>yRange?xRange:yRange);


				dlg.m_dX1 = (box.XMin()+box.XMax())/2.0-range/2;
				dlg.m_dY1 = (box.YMin()+box.YMax())/2.0;

				dlg.m_dX2 = (box.XMin()+box.XMax())/2.0+range/2;
				dlg.m_dY2 = (box.YMin()+box.YMax())/2.0;

				double xMin, xMax, yMin, yMax;
				xMin = (box.XMin()+box.XMax())/2.0-range/2;
				xMax = (box.XMin()+box.XMax())/2.0+range/2;
				yMin = (box.YMin()+box.YMax())/2.0-range/2;
				yMax = (box.YMin()+box.YMax())/2.0+range/2;

				if( fabs(pSlice->A)<0.0000000001 )
				{
					dlg.m_dX1 = xMin;
					dlg.m_dX2 = xMax;

					dlg.m_dY1 = (-pSlice->D/pSlice->B + 1.0)/2.0*range + yMin;
					dlg.m_dY2 = (-pSlice->D/pSlice->B + 1.0)/2.0*range + yMin;
				}
				else if( fabs(pSlice->B) < 0.0000000001)
				{
					dlg.m_dX1 = (-pSlice->D/pSlice->A + 1.0)/2.0*range + xMin;
					dlg.m_dX2 = (-pSlice->D/pSlice->A + 1.0)/2.0*range + xMin;

					dlg.m_dY1 = yMin;
					dlg.m_dY2 = yMax;
				}
				else
				{
					dlg.m_dX1 = (-pSlice->B*1.0-pSlice->D)/pSlice->A;
					dlg.m_dX2 = (pSlice->B*1.0-pSlice->D)/pSlice->A;

					dlg.m_dY1 = (-pSlice->A*dlg.m_dX1-pSlice->D)/pSlice->B;
					dlg.m_dY2 = (-pSlice->A*dlg.m_dX2-pSlice->D)/pSlice->B;

					dlg.m_dX1 = (dlg.m_dX1 + 1.0)/2.0*range + xMin;
					dlg.m_dX2 = (dlg.m_dX2 + 1.0)/2.0*range + xMin;

					dlg.m_dY1 = (dlg.m_dY1 + 1.0)/2.0*range + yMin;
					dlg.m_dY2 = (dlg.m_dY2 + 1.0)/2.0*range + yMin;
				}

				dlg.m_bSpecified = pSlice->m_bSpecified;
				dlg.m_dHeight = pSlice->m_dHeight;
				dlg.m_bShow = pSlice->isShow();

				dlg.m_strTitle = _T("纵切片 - ");
				dlg.m_strTitle += pSlice->GetObjName();

				dlg.m_bShowSlicePlane = pSlice->m_bShowSlicePlane;

				if( dlg.DoModal() == IDOK )
				{
					// (x2-x1) * (y-y1) = (y2-y1) * (x-x1)
					// 
					// 设原始方程为
					//   A*X + B * Y + C = 0
					//   
					// 其中
					//   A = ( y2 - y1 )
					//   B = ( x1 - x2 )
					//   C = - ( y1*B + A * x1 );
					double x1 = (dlg.m_dX1-xMin)/range*2.0-1.0;
					double y1 = (dlg.m_dY1-yMin)/range*2.0-1.0;
					double x2 = (dlg.m_dX2-xMin)/range*2.0-1.0;
					double y2 = (dlg.m_dY2-yMin)/range*2.0-1.0;

					pSlice->A = y2-y1;
					pSlice->B = x1-x2;
					pSlice->C = 0.0;
					pSlice->D = - ( y1*pSlice->B + pSlice->A * x1 );
					
					pSlice->m_bSpecified = dlg.m_bSpecified;
					pSlice->m_dHeight = dlg.m_dHeight;
					pSlice->Show(dlg.m_bShow);
					pSlice->m_bShowSlicePlane = dlg.m_bShowSlicePlane;

					m_pDoc->SetModifiedFlag();

					m_pDoc->UpdateAllViews(NULL);

					if( dlg.m_bShow )
						SetCheck(hti, 1);
					else
						SetCheck(hti, 2);

					if( IsAllChildHide(GetParentItem(hti)))
						SetCheck(GetParentItem(hti), 2);
					else
						SetCheck(GetParentItem(hti), 1);

					if( IsAllChildHide(GetParentItem(GetParentItem(hti))))
						SetCheck(GetParentItem(GetParentItem(hti)), 2);
					else
						SetCheck(GetParentItem(GetParentItem(hti)), 1);
				}

			}			
		}
		else
		{
			CDlgMaterial dlg;
			CMaterial ma;
			CMaterial *p = pObj->GetMaterial();
			ma = (*p);

			dlg.m_pDoc = m_pDoc;
			dlg.m_pObj = pObj;

			BOOL bModified = m_pDoc->IsModified();

			if(dlg.DoModal() == IDOK)
			{
				if( ma != dlg.m_Matl && !dlg.m_bPreview )
				{
					pObj->SetMaterial(dlg.m_Matl);
					pObj->GetContext()->SetModifiedFlag();
					m_pDoc->UpdateAllViews(NULL);
				}
			}
			else
			{
				if( ma != dlg.m_Matl && dlg.m_bPreview )
				{
					pObj->SetMaterial(ma);
					pObj->GetContext()->SetModifiedFlag();
					m_pDoc->UpdateAllViews(NULL);

					m_pDoc->SetModifiedFlag(bModified);
				}
			}
		}
	}
	*pResult = 0;
}

void C3DObjTreeCtrl::TreeVisit(HTREEITEM hItem)
{
	if(ItemHasChildren(hItem))  
	{
		HTREEITEM hChildItem = GetChildItem(hItem);  

		while(hChildItem!=NULL)  
		{
			CGLObject *pObj = (CGLObject*)GetItemData(hItem);
			if( pObj == NULL || pObj->GetGLObjType() == GLTRIHEDRON )
				return;

			DeleteItem(hItem);

			if( pObj->GetGLObjType() == GLDICE )
				m_pDoc->GetContext()->EraseDice(pObj);
			else if (pObj->GetGLObjType() == GLSLICE)
				m_pDoc->GetContext()->EraseSlice(pObj);
			else
				m_pDoc->GetContext()->Erase(pObj);

			POSITION pos = m_pDoc->GetFirstViewPosition();
			CView *pView = m_pDoc->GetNextView(pos);
			pView->Invalidate(FALSE);

			m_pDoc->SetModifiedFlag();

			TreeVisit(hChildItem);
			hChildItem = GetNextItem(hChildItem, TVGN_NEXT);  
		}  
	}
}

HTREEITEM C3DObjTreeCtrl::GetItemByGUID( CString guid )
{
	////HTREEITEM root = GetRootItem();
	////if(!root)
	////	return NULL;
	////return SearchItemByGUID(guid, root);
	CList <HTREEITEM,HTREEITEM&>   TreeList; 
	HTREEITEM   hItemhi   =   GetRootItem();
	if   (hItemhi   !=   NULL)
	{
		TreeList.AddHead(hItemhi);
		while(!TreeList.IsEmpty())
		{
			hItemhi   =   TreeList.RemoveHead();
			CGLObject *pObj = (CGLObject*)GetItemData(hItemhi);
			if( pObj )
				if(pObj->GetGLObjType() != GLTRIHEDRON )
					if(guid   ==   pObj->m_strGUID)   //ItemName是你想找的字符
						return hItemhi;
			hItemhi   =    GetChildItem(hItemhi);
			while(hItemhi)
			{
				TreeList.AddHead(hItemhi);
				hItemhi   =   GetNextSiblingItem(hItemhi);
			}
		}
	}
	return NULL;
}

HTREEITEM C3DObjTreeCtrl::SearchItemByGUID( CString guid, HTREEITEM parent )
{
	if(!parent)
		return NULL;
	HTREEITEM point = parent;
	CGLObject *pObj = (CGLObject*)GetItemData(point);
	if(pObj)
	{
		if(pObj->m_strGUID==guid)
		{
			return point;
		}
	}
	HTREEITEM son = GetChildItem( point);
	do 
	{
		HTREEITEM result = SearchItemByGUID(guid, son);
		if(result)
			return result;
		son = GetNextSiblingItem(son);
	} while (son!=NULL);
	return NULL;
}

void C3DObjTreeCtrl::DeleteItemByGUID( CString guid )
{
	HTREEITEM hItem = GetItemByGUID(guid);

	if( hItem != NULL )
	{
		CGLObject *pObj = (CGLObject*)GetItemData(hItem);
		CMainFrame *pMF = (CMainFrame *)AfxGetMainWnd();
		//pMF->GetTreeModelView()->DeleteItemByGUID(pObj->m_strGUID);
		if( pObj == NULL || pObj->GetGLObjType() == GLTRIHEDRON )
			return;

		//if( ::MessageBox(m_hWnd,_T("确定要删除吗？"),_T("提示信息"),MB_YESNO|MB_ICONQUESTION) == IDNO )
		//	return;

		DeleteItem(hItem);

		if( pObj->GetGLObjType() == GLDICE )
			m_pDoc->GetContext()->EraseDice(pObj);
		else if (pObj->GetGLObjType() == GLSLICE)
			m_pDoc->GetContext()->EraseSlice(pObj);
		else
			m_pDoc->GetContext()->Erase(pObj);

		POSITION pos = m_pDoc->GetFirstViewPosition();
		CView *pView = m_pDoc->GetNextView(pos);
		pView->Invalidate(FALSE);

		m_pDoc->SetModifiedFlag();

		SetFocus();
	}
}
void CIntersectSearchTree::FillTreeCtrl()
{
	DeleteAllItems();
	if( m_pDoc == NULL )
		return;
	
	HTREEITEM hItem1 = InsertItem(_T("网格模型"),0,0);
	HTREEITEM hItem2 = InsertItem(_T("夹层曲面"),0,0);
	//HTREEITEM hItem31 = InsertItem(_T("横切片"),0,0,hItem3);
	//HTREEITEM hItem32 = InsertItem(_T("纵切片"),0,0,hItem3);

	POSITION pos = m_pDoc->GetFirstViewPosition();
	C3DModelView *pView = (C3DModelView *)m_pDoc->GetNextView(pos);

	CGLTrihedron* pTri = pView->m_myView->m_myTrihedron;

	//if( !pView->m_myView->IsShowAxis() && !pView->m_myView->IsShowGird() && !pView->m_myView->IsShowLegend() )
	//	SetCheck(hItem1,2);
	//else
	//	SetCheck(hItem1,1);

	if( IsAllChildHide(hItem1))
		SetCheck(hItem1, 2);
	else
		SetCheck(hItem1, 1);
	
	if( IsAllChildHide(hItem2))
		SetCheck(hItem2, 2);
	else
		SetCheck(hItem2, 1);
		

	Expand(hItem1, TVE_EXPAND);
	Expand(hItem2, TVE_EXPAND);
}

void CIntersectSearchTree::AddObj( CGLObject *pObj )
{
	if( m_pDoc == NULL )
		return;

	//HTREEITEM hSrc2 = GetChildNode(TVI_ROOT, _T("图形切块"));
	HTREEITEM hSrc1 = GetChildNode(TVI_ROOT, _T("网格模型"));
	HTREEITEM hSrc2 = GetChildNode(TVI_ROOT, _T("夹层曲面"));
	//HTREEITEM hSrc41 = GetChildNode(hSrc4, _T("面"));
	//HTREEITEM hSrc42 = GetChildNode(hSrc4, _T("线"));
	//HTREEITEM hSrc43 = GetChildNode(hSrc4, _T("点"));

	HTREEITEM hItem;
	int nImage = 0, nSelectImage = 0;
	switch(pObj->GetGLObjType())
	{
	/*case GLDICE:
		{
			nImage = 4;
			nSelectImage = 4;
			hItem = InsertItem(pObj->GetObjName(),nImage,nSelectImage, hSrc2);
		}
		break;
		*/
	case GLSURFACE:
		{
			nImage = 7;
			nSelectImage = 7;
			//hItem = InsertItem(pObj->GetObjName(),nImage,nSelectImage, hSrc4, TVI_FIRST);

			CGridObject *pGrid = (CGridObject *)pObj;

			HTREEITEM h = InsertItem(_T("X"),8,8,hItem);
			int n = 0;
			for (int i = 0; i < pGrid->I; i++)
			{
				CString str;
				str.Format(_T("#%d"), i+1);
				HTREEITEM hh = InsertItem(str,6,6,h);
				if( pGrid->m_bShowI[i] )
				{
					SetCheck(hh, 5);
					n++;
				}
				else
					SetCheck(hh, 3);
			}
			if( n == 0 )
				SetCheck(h, 3);
			else if( n < pGrid->I )
				SetCheck(h, 4);
			else
				SetCheck(h, 5);

			h = InsertItem(_T("Y"),8,8,hItem);
			n = 0;
			for (int i = 0; i < pGrid->J; i++)
			{
				CString str;
				str.Format(_T("#%d"), i+1);
				HTREEITEM hh = InsertItem(str,6,6,h);
				if( pGrid->m_bShowJ[i] )
				{
					SetCheck(hh, 5);
					n++;
				}
				else
					SetCheck(hh, 3);
			}
			if( n == 0 )
				SetCheck(h, 3);
			else if( n < pGrid->J )
				SetCheck(h, 4);
			else
				SetCheck(h, 5);

			h = InsertItem(_T("Z"),8,8,hItem);
			n = 0;
			for (int i = 0; i < pGrid->K; i++)
			{
				CString str;
				str.Format(_T("#%d"), i+1);
				HTREEITEM hh = InsertItem(str,5,5,h);
				if( pGrid->m_bShowK[i] )
				{
					SetCheck(hh, 5);
					n++;
				}
				else
					SetCheck(hh, 3);
			}
			if( n == 0 )
				SetCheck(h, 3);
			else if( n < pGrid->K )
				SetCheck(h, 4);
			else
				SetCheck(h, 5);


			h = InsertItem(_T("属性"),8,8,hItem);
			SetCheck(h, 6);
			for (int i=0; i<pGrid->m_vecPhyPara.GetSize(); i++)
			{				
				HTREEITEM hh = InsertItem(pGrid->m_vecPhyPara[i].m_strName,9,9,h);
				SetCheck(hh, 6);
				if( pGrid->m_vecPhyPara[i].m_bShow )
				{
					SetCheck(hh, 7);
					SetCheck(h, 7);
				}
			}
		}
		break;
	case GLPLANE:
		{
			nImage = 10;
			nSelectImage = 10;
			//hItem = InsertItem(pObj->GetObjName(),nImage,nSelectImage, hSrc41);
		}
		break;
	default:
		break;
	}

	if( pObj->isShow())
		SetCheck(hItem, 1);
	else
		SetCheck(hItem, 2);

	SetItemData(hItem, (DWORD)pObj);

	Select(hItem, TVGN_CARET);

	/*if( pObj->GetGLObjType() == GLDICE )
		Expand(hSrc2, TVE_EXPAND);
	else*/
}

void CIntersectSearchTree::SetModel( const std::string& pObj )
{
	HTREEITEM hSrc = GetChildNode(TVI_ROOT, _T("网格模型"));
	DeleteItem(hSrc);
	HTREEITEM hItem1 = InsertItem(_T("网格模型"),0,0);
	int nImage = 0, nSelectImage = 0;
	nImage = 10;
	nSelectImage = 10;
	HTREEITEM hItem = InsertItem(pObj.c_str(),nImage,nSelectImage,hSrc);
	//CIntersectSearchManager::Instance()->SetGridModel(dynamic_cast<CGridObject*>(pObj));
}

void CIntersectSearchTree::AddLayer( CGLObject* pObj )
{
	HTREEITEM hSrc = GetChildNode(TVI_ROOT, _T("夹层曲面"));
	int nImage = 0, nSelectImage = 0;
	nImage = 10;
	nSelectImage = 10;
	HTREEITEM hItem = InsertItem(pObj->GetObjName(),nImage,nSelectImage, hSrc);
	CIntersectSearchManager::Instance()->AddLayerModel(pObj);
}
