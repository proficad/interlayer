
#include "stdafx.h"
#include "Interlayer.h"
#include "ViewTree.h"
#include "MainFrm.h"
#include <sys/types.h>
#include <sys/stat.h>
#include "MyRichEditView.h"
#include <shellapi.h>
#include "GeoMap/GeoMapView.h"
#include "GeoMap/GeoMapDoc.h"
#include "2DModelView.h"
#include "2DModelDoc.h"
#include "WndGraph.h"
#include ".\vertical_model\graphwnd.h"
#include "FormGenerateLayer.h"
#include "3dlib/3DModelView.h"
#include "3dlib/3DModelDoc.h"

extern CInterlayerApp theApp;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CLIPFORMAT CViewTree::m_cfTree = (CLIPFORMAT)::RegisterClipboardFormat(_T("TreeView"));
extern void StatusBarMessage(char* fmt, ...);
extern void StatusProgress(int nIndex, int nRange=-1);
extern void StatusSetProgress(int nIndex, int nPos);

IMPLEMENT_SERIAL(CTreeNodeDat, CObject, VERSIONABLE_SCHEMA | 0)

CTreeNodeDat::CTreeNodeDat()
{
	m_uState			= TVIS_EXPANDED;
	m_nImage			= -1;
	m_nSelectedImage	= -1;
	m_nType				= FOLDER;
	m_pNodeDat			= NULL;
	m_strFileName		= _T("");
	m_strGUIDName	= newGUID();
}

CTreeNodeDat::~CTreeNodeDat()
{
	if( m_pNodeDat != NULL )
		delete m_pNodeDat;
}

void CTreeNodeDat::Serialize(CArchive& ar)
{
	ar.SerializeClass(RUNTIME_CLASS(CTreeNodeDat));
	if(ar.IsStoring())
	{
		ar << m_uState;
		ar << m_nImage;
		ar << m_nSelectedImage;
		ar << (WORD)m_nType;
		ar << m_strFileName;
		ar << m_strGUIDName;

		BOOL bExist;
		if( m_pNodeDat )
		{
			bExist = TRUE;
			ar << bExist;
			CRuntimeClass *pRuntimeClass = m_pNodeDat->GetRuntimeClass();
			CString strClassName = pRuntimeClass->m_lpszClassName;

			ar << strClassName;

			m_pNodeDat->Serialize(ar);
		}
		else
		{
			bExist = FALSE;
			ar << bExist;
		}
	}
	else
	{
		int iVer = ar.GetObjectSchema();
		switch(iVer)
		{
		case 0:
			{
				WORD wdTmp;
				ar >> m_uState;
				ar >> m_nImage;
				ar >> m_nSelectedImage;

				ar >> wdTmp;
				m_nType = (NODE_STYLE)wdTmp;

				ar >> m_strFileName;
				ar >> m_strGUIDName;

				BOOL bExist;
				ar >> bExist;

				if( bExist )
				{
					CString strClassName;
					ar >> strClassName;					
					CRuntimeClass *pClass = CRuntimeClass::FromName(strClassName);
					m_pNodeDat = pClass->CreateObject();
				}
			}
			break;
		default:
			break;
		}			
	}
}


CArchive& operator <<(CArchive& ar, const CTreeNodeDat& dat)
{
	ar.WriteClass(RUNTIME_CLASS(CTreeNodeDat));
	ar << dat.m_uState;
	ar << dat.m_nImage;
	ar << dat.m_nSelectedImage;
	ar << (WORD)dat.m_nType;
	ar << dat.m_strFileName;
	ar << dat.m_strGUIDName;

	BOOL bExist;
	if( dat.m_pNodeDat )
	{
		bExist = TRUE;
		ar << bExist;

		CRuntimeClass *pRuntimeClass = dat.m_pNodeDat->GetRuntimeClass();
		CString strClassName = pRuntimeClass->m_lpszClassName;

		ar << strClassName;

		dat.m_pNodeDat->Serialize(ar);
	}
	else
	{
		bExist = FALSE;
		ar << bExist;
	}

	return ar;
}

CArchive& operator >>(CArchive& ar, CTreeNodeDat& dat)
{
	ar.ReadClass(RUNTIME_CLASS(CTreeNodeDat));
	int iVer = ar.GetObjectSchema();
	switch(iVer)
	{
	case 0:
		{
			WORD wdTmp;
			ar >> dat.m_uState;
			ar >> dat.m_nImage;
			ar >> dat.m_nSelectedImage;
			ar >> wdTmp;
			dat.m_nType = (NODE_STYLE)wdTmp;

			ar >> dat.m_strFileName;
			ar >> dat.m_strGUIDName;

			BOOL bExist;
			ar >> bExist;
			
			if( bExist )
			{
				CString strClassName;
				ar >> strClassName;
				CRuntimeClass *pClass = CRuntimeClass::FromName(strClassName);
				dat.m_pNodeDat = pClass->CreateObject();
				dat.m_pNodeDat->Serialize(ar);
			}
		}
		break;
	default:
		break;
	}

	return ar;
}

CTreeNodeDat &CTreeNodeDat::operator=(const CTreeNodeDat& dat)
{
	m_uState = dat.m_uState;
	m_nImage = dat.m_nImage;
	m_nSelectedImage = dat.m_nSelectedImage;
	m_nType = dat.m_nType;
	m_strFileName = dat.m_strFileName;

	if( dat.m_pNodeDat )
	{
		BYTE *pNewData = new BYTE[sizeof(*(dat.m_pNodeDat))];

		Checked::memcpy_s(pNewData, sizeof(*(dat.m_pNodeDat)), 
			dat.m_pNodeDat, sizeof(*(dat.m_pNodeDat)));

		m_pNodeDat = (CObject*)pNewData;
	}
 
	return (*this);
}


/////////////////////////////////////////////////////////////////////////////
// CViewTree

CViewTree::CViewTree()
{
	m_bModified			= false;
	m_hCursor			= NULL;
	m_bDragging			= false;
	pfOpenTree			= NULL;
	m_bInsertionMode	= false;

	m_bNew3D			= false;

	m_nCountImages		= 0;
	m_pDragImage		= NULL;

	m_hSelectItem		= NULL;

	int aid[] =
	{
		IDC_DROP_COPY_ROOT,
		IDC_DROP_COPY_SON,
		IDC_DROP_MOVE_ABOVE,
		IDC_DROP_COPY_ABOVE,
		IDC_DROP_MOVE_ROOT,
		IDC_DROP_MOVE_SON,
		IDC_DROP_ADD_DAT,
		IDC_NODROP,
	}, k;

	for (k = 0; k < CUR_COUNT; k++)
		m_ahCursor[k] = theApp.LoadCursor(aid[k]);
}

CViewTree::~CViewTree()
{
	if( m_pDragImage )
	{
		delete m_pDragImage;
		m_pDragImage = NULL;
	}

	COleDataObject dataObject;
	BOOL bEnable = dataObject.AttachClipboard() &&
		(dataObject.IsDataAvailable(m_cfTree) || COleClientItem::CanCreateFromData(&dataObject));

	if( !bEnable )
		return;

	CFile* pFile = dataObject.GetFileData(m_cfTree);
	if(pFile == NULL)
		return;

	delete pFile;
}

BEGIN_MESSAGE_MAP(CViewTree, CTreeCtrl)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, &CViewTree::OnTvnBegindrag)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, &CViewTree::OnTvnItemexpanded)
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, &CViewTree::OnTvnBeginlabeledit)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, &CViewTree::OnTvnEndlabeledit)
	ON_NOTIFY_REFLECT(NM_RCLICK, &CViewTree::OnNMRClick)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

void CViewTree::DeleteUnusedFile(LPCSTR lpszPathName)
{	
	CString strDirectory = lpszPathName;
	strDirectory.MakeUpper();
	strDirectory += _T("\\");

	CString strPathName = strDirectory;
	strPathName += "*.*";
	CFileFind finder;

	BOOL bWorking = finder.FindFile(strPathName);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		struct _stat buf;
		int result = _stat( finder.GetFilePath(), &buf );

		if( !finder.IsDots( ) && finder.IsDirectory() ) 
		{
			CFileFind finder2;
			CString tmp = finder.GetFilePath()+"\\*.*";
			BOOL bWorking = finder2.FindFile(tmp);
			while (bWorking)
			{
				bWorking = finder2.FindNextFile();

				struct _stat buf;
				int result = _stat( finder2.GetFilePath(), &buf );

				if( result == 0 ) 
				{						
					if ((buf.st_mode & _S_IFREG) == _S_IFREG)
					{
						CString strFileName = finder2.GetFilePath();

						strFileName.MakeUpper();
						strFileName.Replace(strDirectory,"");

						if( !IsExistFileInTree(TVI_ROOT, strFileName) )	// 不存在此文件，则删除
						{
							CString dst = finder2.GetFilePath();
							DeleteFile(dst);
						}
					}
				}
			}
		} 
		else if ((buf.st_mode & _S_IFREG) == _S_IFREG) // 是文件
		{
			CString strFileName = finder.GetFileName();
			strFileName.MakeUpper();
			if( !IsExistFileInTree(TVI_ROOT, strFileName) )	// 不存在此文件，则删除
			{
				CString dst = finder.GetFilePath();
				DeleteFile(dst);
			}
		}
	}
}
//------------------------------------------------------------------------
// 
void CViewTree::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// storing code	
		HTREEITEM hItem = GetRootItem();
		WriteTreeViewContent(ar, hItem);
	}
	else
	{
		// loading code	
		FreeAllChildNodeData(TVI_ROOT);
		DeleteAllItems();

		ReadTreeViewContent(ar, TVI_ROOT);

		UINT nCount = GetCount();
		if( nCount> 0 )
		{
			HTREEITEM hItem = GetRootItem();
			RestoreStates(hItem); // 恢复树状态
		}
	}
}

//------------------------------------------------------------------------
// 恢复树叶状态
void CViewTree::RestoreStates(HTREEITEM hItem)
{
	do
	{
		CTreeNodeDat *pNote = (CTreeNodeDat *)GetItemData(hItem);

		ExpandM(hItem, pNote->m_uState & TVIS_EXPANDED ? TVE_EXPAND :TVE_COLLAPSE);

		if (ItemHasChildren(hItem))
		{
			RestoreStates(GetChildItem(hItem));
		}
	}while ((hItem = GetNextSiblingItem(hItem)) != NULL);	
}

HTREEITEM CViewTree::ReadTreeViewContent(CArchive& ar, HTREEITEM hItem)
{	
	HTREEITEM hItemRet = NULL;

	UINT nCount;
	ar >> nCount;			// 节点总数

	if (nCount == 0)
		return NULL;

	m_nIndex = 0;
	m_hItems[m_nIndex] = hItem;
	HTREEITEM hAfter = TVI_FIRST;

	while (m_nIndex >= 0)
	{
		BYTE byteType;
		ar >> byteType;			// 类别

		switch (byteType)
		{
		case recordRegular:
			{
				CString strTxt;
				ar >> strTxt;			// 树叶名称
				CTreeNodeDat *pNote = new CTreeNodeDat;
				ar >> (*pNote);
				if( pNote->m_nType == FILE_DAT )
				{
					CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
					CString strTargePathName = pMF->GetProjectDatPath();
					strTargePathName += _T("\\files\\");
					strTargePathName += ((CFileViewObj*)pNote->m_pNodeDat)->m_strFileName;
					int iImage;
					int iSelectedImage;
					GetNormalAndSelectedIcons(strTargePathName, iImage, iSelectedImage);
					hAfter = InsertItem(strTxt, iImage, iSelectedImage, m_hItems[m_nIndex], hAfter);
				}
				else
				{	

					if( m_eTreeType == fileTree )
					{
						int nImage = m_nCountImages - 19;

						switch(pNote->m_nType)
						{
						case FOLDER:					// 文件夹
							break;
						case WELL_DAT:
							nImage = nImage + 1;
							break;
						case WELL_INFO:				// 井基础信息数据
							nImage = nImage + 2;
							break;
						case WELL_HOLE:				// 井眼轨迹数据
							nImage = nImage + 3;
							break;
						case WELL_LOGING_INFO:				// 测井
							nImage = nImage + 4;
							break;
						case WELL_LOGINGS:				// 测井曲线
							nImage = nImage + 5;
							break;
						case WELL_LOGING:				// 测井曲线
							nImage = nImage + 6;
							break;
						case WELL_PRO:					// 单井
							nImage = nImage + 7;
							break;
						case WELL_INJ:					// 单井
							nImage = nImage + 8;
							break;					
						case FILE_POINT:				// 点
							nImage = nImage + 9;
							break;
						case FILE_LINE:				// 线
							nImage = nImage + 10;
							break;
						case FILE_POLYGON:			// 多边形
							nImage = nImage + 11;
							break;
						case FILE_PLANE:				// 面
							nImage = nImage + 12;
							break;
						case FILE_PICTRUE:				// 图片
							nImage = nImage + 13;
							break;
						case FILE_NORMAL:				// 法线
						case MODEL_NORMAL:
							nImage = nImage + 14;
							break;
						case FILE_LAYER:				// 夹层线
						case MODEL_LAYER:
							nImage = nImage + 15;
							break;
						case FILE_VERT_MODELS:			// 纵向模型集
							nImage = nImage + 16;
							break;
						case FILE_VERT_MODEL:			// 纵向模型
							nImage = nImage + 17;
							break;
						case FILE_LAYER_MODEL:			// 夹层模型
							nImage = nImage + 18;
							break;
						default:
							nImage = pNote->m_nImage;
							break;
						}
						pNote->m_nImage = nImage;
						pNote->m_nSelectedImage = nImage;
					}
					if( m_eTreeType == modelTree )
					{
						switch(pNote->m_nType)
						{
						case GRID_LAYER:
							{
								pNote->m_nImage = 10;
								pNote->m_nSelectedImage = 10;
							}
							break;
						}
					}
					hAfter = InsertItem(strTxt, pNote->m_nImage, pNote->m_nSelectedImage, m_hItems[m_nIndex], hAfter);
				}
				SetItemData(hAfter, (DWORD_PTR)pNote);
	
				if( m_nIndex == 0 )
					hItemRet = hAfter;
			}
			break;
		
		case recordPush:
			ASSERT(m_nIndex < 99);
			m_hItems[++m_nIndex] = hAfter;
			hAfter = TVI_FIRST;
			break;

		case recordPop:
			m_nIndex--;
			break;
		
		default:
			{
				ASSERT(FALSE);
				AfxThrowMemoryException();
			}
		}
	}

	return hItemRet;
}

void CViewTree::WriteTreeViewContent(CArchive& ar, HTREEITEM hItem)
{
	UINT nCount = GetCount();
	ar << nCount;				// 节点总数

	if (nCount == 0)
		return;

	RecursiveWriteItems(ar, hItem);
}

void CViewTree::RecursiveWriteItems(CArchive& ar, HTREEITEM hItem)
{
	do
	{
		CString strTxt = GetItemText(hItem);
		CTreeNodeDat *pNote = (CTreeNodeDat *)GetItemData(hItem);

		ar << (BYTE) recordRegular;			// 树叶类别
		ar << strTxt;						// 保存树叶名称
		ar << (*pNote);						// 保存树叶数据

		if (ItemHasChildren(hItem))
		{
			ar << (BYTE) recordPush;
			RecursiveWriteItems(ar, GetChildItem(hItem));
		}
	}while ((hItem = GetNextSiblingItem(hItem)) != NULL);

	ar << (BYTE) recordPop;
}

//------------------------------------------------------------------------
// 
int CViewTree::GetCheck(HTREEITEM hItem)
{
	TVITEM tvItem;

	// Prepare to receive the desired information.
	tvItem.mask = TVIF_HANDLE | TVIF_STATE;
	tvItem.hItem = hItem;
	tvItem.stateMask = TVIS_STATEIMAGEMASK;

	// Request the information.
	TreeView_GetItem(this->m_hWnd, &tvItem);

	// Return zero if it's not checked, or nonzero otherwise.
	return ((tvItem.state >> 12)-1);
}

BOOL CViewTree::SetCheck(HTREEITEM hItem, UINT uCheck)
{
	TVITEM tvItem;

	tvItem.mask = TVIF_HANDLE | TVIF_STATE;
	tvItem.hItem = hItem;
	tvItem.stateMask = TVIS_STATEIMAGEMASK;

	tvItem.state = INDEXTOSTATEIMAGEMASK(uCheck);

	return TreeView_SetItem(m_hWnd, &tvItem);
}

//------------------------------------------------------------------------
// 
void CViewTree::VerifBoldParents(HTREEITEM hItem)
{
	HTREEITEM hParentItem = hItem;

	while(hParentItem = GetParentItem(hParentItem))
	{
		int nCheck = GetCheck(hParentItem);
		if( nCheck < 4 )
		{
			SetItemState(hParentItem, IsExistChildBold(hParentItem) ? TVIS_BOLD: 0, TVIS_BOLD);
		}
	}
}

int CViewTree::IsExistChildBold(HTREEITEM hItem)
{
	hItem = GetChildItem(hItem);

	while(hItem)
	{
		int nCheck = GetCheck(hItem);
		if(TVIS_BOLD & GetItemState(hItem, TVIS_BOLD) && nCheck < 4)
			return 1;

		hItem = GetNextItem(hItem, TVGN_NEXT);	
	}

	return 0;
}

//------------------------------------------------------------------------
// 
void CViewTree::VerifFullParents(HTREEITEM hItem)
{
	HTREEITEM hParentItem = hItem;
	while(hParentItem = GetParentItem(hParentItem))
	{
		if(IsAllChildFull(hParentItem))
			SetItemFull(hParentItem);
		else if(IsAllChildEmpty(hParentItem))
			SetItemEmpty(hParentItem);
		else
		{
			int nCheck = GetCheck(hItem);
			if( nCheck < 4 )
			{
				SetCheck(hParentItem, 2);
				SetItemState(hParentItem, true ? TVIS_BOLD: 0, TVIS_BOLD);
			}
		}
	}
}

int CViewTree::IsAllChildEmpty(HTREEITEM hItem)
{
	hItem = GetChildItem(hItem);

	while(hItem)
	{
		int nCheck = GetCheck(hItem);
		if( nCheck < 4 && nCheck > 0)
		{
			return 0;
		}

		hItem = GetNextItem(hItem, TVGN_NEXT);	
	}

	return 1;
}

int CViewTree::IsAllChildFull(HTREEITEM hItem)
{
	hItem = GetChildItem(hItem);

	while(hItem)
	{
		int nCheck = GetCheck(hItem);
		if( nCheck < 4 )
		{
			if( nCheck == 0 )
				return 0;
			if( nCheck == 1 )
				return 0;
		}

		hItem = GetNextItem(hItem, TVGN_NEXT);	
	}

	return 1;
}

void CViewTree::SetItemFull(HTREEITEM hItem)
{
	int nCheck = GetCheck(hItem);
	if( nCheck < 4 )
	{
		SetCheck(hItem, 3);
		SetItemState(hItem, true ? TVIS_BOLD: 0, TVIS_BOLD);
	}
}

void CViewTree::SetItemEmpty(HTREEITEM hItem)
{
	int nCheck = GetCheck(hItem);
	if( nCheck < 4 )
	{
		SetCheck(hItem, 1);
		SetItemState(hItem, false ? TVIS_BOLD: 0, TVIS_BOLD);
	}
}

void CViewTree::SetItemAndChildrenEmpty(HTREEITEM hItem)
{
	SetItemEmpty(hItem);
	HTREEITEM hChildItem = GetChildItem(hItem);

	while(hChildItem)
	{
		SetItemEmpty(hChildItem);
		SetItemAndChildrenEmpty(hChildItem);

		hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
	}
}

void CViewTree::SetItemAndChildrenFull(HTREEITEM hItem)
{
	SetItemFull(hItem);
	HTREEITEM hChildItem = GetChildItem(hItem);

	while(hChildItem)
	{
		SetItemFull(hChildItem);
		SetItemAndChildrenFull(hChildItem);

		hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
	}
}
//------------------------------------------------------------------------
// 
BOOL CViewTree::SelectVisibleItem(HTREEITEM hti)
{
	EnsureVisible(hti);
	return SelectItem(hti);
}

BOOL CViewTree::ExpandM(HTREEITEM hItem, UINT nCode)
{
	BOOL bFlag;

	bFlag = Expand(hItem, nCode);	// Since we save the expand/collapse state into the file,
	//if (bFlag)						// we consider this as a modification;
	//	SetModifiedFlag(); 
	return bFlag;
}

BOOL CViewTree::DeleteItemM(HTREEITEM hItem)
{
	FreeAllChildNodeData(hItem); // 释放所有子节点
	
	// 再释放自己

	if( hItem != TVI_ROOT)
	{
		CTreeNodeDat* pTreeNode = (CTreeNodeDat* )GetItemData(hItem);
		if( pTreeNode != NULL )
		{
			delete pTreeNode;
			pTreeNode = NULL;
		}
	}

	SetItemData(hItem, NULL);

	BOOL bFlag = DeleteItem(hItem);
	if (bFlag)
		SetModifiedFlag();

	return bFlag;
}

BOOL CViewTree::DeleteAllChildItem(HTREEITEM hItem)
{
	if(!ItemHasChildren(hItem))
		return FALSE;

	FreeAllChildNodeData(hItem);

	HTREEITEM hChild = GetChildItem(hItem);   
	do   
	{   
		HTREEITEM hSiblingItem = GetNextSiblingItem(hChild);

		if(ItemHasChildren(hChild))
			DeleteAllChildItem(hChild);
		else
		{
			DeleteItem(hChild);
		}

		hChild = hSiblingItem;

	}while(hChild != NULL);

	SetModifiedFlag();

	return TRUE;
}

void CViewTree::FreeNodeData(HTREEITEM hItem)
{
	do
	{
		CTreeNodeDat* pTreeNode = (CTreeNodeDat* )GetItemData(hItem);
		if( pTreeNode != NULL )
		{
			delete pTreeNode;
			pTreeNode = NULL;
		}

		SetItemData(hItem, NULL);
		
		if (ItemHasChildren(hItem))
			FreeNodeData(GetChildItem(hItem));

	}while ((hItem = GetNextSiblingItem(hItem)) != NULL);
}

void CViewTree::FreeAllChildNodeData(HTREEITEM hItem)
{
	hItem = GetChildItem(hItem); 

	while(hItem != NULL)
	{
		CTreeNodeDat* pTreeNode = (CTreeNodeDat* )GetItemData(hItem);
		if( pTreeNode != NULL )
		{
			delete pTreeNode;
			pTreeNode = NULL;
		}

		SetItemData(hItem, NULL);

		FreeAllChildNodeData(hItem);

		hItem = GetNextSiblingItem(hItem);
	}
}

//------------------------------------------------------------------------
// 
bool CViewTree::IsModified()
{
	return m_bModified;
}

void CViewTree::SetModifiedFlag(bool bFlag)
{
	m_bModified = bFlag;
}

//------------------------------------------------------------------------
// 

HTREEITEM CViewTree::GetChildNode(HTREEITEM hti, LPCSTR lpszNodeName, BOOL bCase)
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

BOOL CViewTree::IsExistChildNode(HTREEITEM hti, LPCSTR lpszNodeName, BOOL bCase)
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

BOOL CViewTree::IsExistChildNode(HTREEITEM hti, HTREEITEM htiChild)
{
	CString strNodeName = GetItemText(htiChild);

	HTREEITEM hSiblingItem = GetChildItem(hti);

	while(hSiblingItem != NULL)
	{
		CString strItem = GetItemText(hSiblingItem);
		if( strItem == strNodeName )
			return TRUE;
		hSiblingItem = GetNextSiblingItem(hSiblingItem);
	}

	return FALSE;
}

HTREEITEM CViewTree::IsExistFileInTree(HTREEITEM hItem, const CString &strFileName)
{
	HTREEITEM hChildItem = GetChildItem(hItem); 

	while(hChildItem != NULL)
	{
		CTreeNodeDat* pTreeNode = (CTreeNodeDat* )GetItemData(hChildItem);

		if( pTreeNode != NULL )
		{
			CString strNode = pTreeNode->m_strFileName;
			if( !strNode.IsEmpty() )
			{
				strNode.MakeUpper();
				if( strFileName == strNode)
					return hChildItem;
			}
		}

		HTREEITEM hFileItem = IsExistFileInTree(hChildItem, strFileName);
		if( hFileItem != NULL )
			return hFileItem;

		hChildItem = GetNextSiblingItem(hChildItem);
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// Selects the appropriate cursor in dragging mode
// F3										- MOVE as a new root
// Ctrl+F3									- COPY as a new root
// Cursor on the right of an item			- MOVE as a son to the right of dropped item.
// Ctrl + Cursor on the right of an item	- COPY as a son to the right of dropped item.
// Cursor on an item or to its left			- MOVE above dropped item.
// Ctrl + Cursor on an item or to its left	- COPY above dropped item.
HTREEITEM CViewTree::SelectDropCursor(CPoint *pPoint /*= NULL*/)
{
	HCURSOR hOldCursor = m_hCursor;
	UINT uFlags = 0;
	HTREEITEM hitem = NULL;
	CString str;
	int nKey = 0;

	if (m_bDragging)
	{
		if (pPoint != NULL)
		{
			if (GetKeyState(VK_F3) & 0x80000000) 
				nKey = VK_F3;
			
			hitem = HitTest(*pPoint, &uFlags);

			CPoint pt(*pPoint);	//get our current mouse coordinates
			ClientToScreen(&pt); //convert to screen coordinates
			m_pDragImage->DragMove(pt); //move the drag image to those coordinates
			// Unlock window updates (this allows the dragging image to be shown smoothly)
			m_pDragImage->DragShowNolock(false);


			// Set a Timer to scroll periodically, while dragging, if mouse cursor is 
			// either below or above the control.

			// The timer must be set only if the flags have changed since the last call.
			if (m_uOldFlags != uFlags)
			{
				m_uOldFlags = uFlags;
				if (uFlags & TVHT_ABOVE)
				{
					SetTimer(TVHT_ABOVE, SCROLL_SPEED, NULL);
					m_bTimerABOVE_Active = true;
				}
				else if (uFlags & TVHT_BELOW)
				{
					SetTimer(TVHT_BELOW, SCROLL_SPEED, NULL);
					m_bTimerBELOW_Active = true;
				}
				else
				{
					if (m_bTimerABOVE_Active)
					{
						KillTimer(TVHT_ABOVE);
						m_bTimerABOVE_Active = false;
					}
					if (m_bTimerBELOW_Active)
					{
						KillTimer(TVHT_BELOW);
						m_bTimerBELOW_Active = false;
					}
				}
			}

			m_pDragImage->DragShowNolock(true);

			CPoint scrPt = (*pPoint);
			ClientToScreen(&scrPt);
			CWnd* pDropWnd = WindowFromPoint (scrPt);
			if( pDropWnd != NULL
				&& pDropWnd->m_hWnd != this->m_hWnd )
			{
				//------------------------------------------------------------------------
				// 2D图形
				if( pDropWnd->IsKindOf(RUNTIME_CLASS(CGeoMapView)))
				{
					CTreeNodeDat *lpNodeDat = (CTreeNodeDat*)GetItemData(m_hitemDrag);

					if( m_eTreeType == fileTree && 
						( lpNodeDat->m_nType == WELL_INFO
							|| lpNodeDat->m_nType == FILE_PICTRUE			// 图片
							|| lpNodeDat->m_nType == FILE_POINT				// 点
							|| lpNodeDat->m_nType == FILE_LINE				// 线
							|| lpNodeDat->m_nType == FILE_POLYGON			// 多边形
							|| lpNodeDat->m_nType == FILE_PLANE				// 面
							|| lpNodeDat->m_nType == FILE_LAYER_MODEL		// 夹层模型
							|| lpNodeDat->m_nType == FILE_NORMAL			// 法线
							|| lpNodeDat->m_nType == FILE_LAYER				// 夹层
							|| lpNodeDat->m_nType == MODEL_NORMAL			// 法线
							|| lpNodeDat->m_nType == MODEL_LAYER			// 夹层
							|| lpNodeDat->m_nType == FOLDER					// 文件夹
							)
						)
					{
						m_hCursor = m_ahCursor[CUR_ADD_DAT];
						SetCursor(m_hCursor);
						return NULL;
					}
				}

				//------------------------------------------------------------------------
				// 2D模型
				if( pDropWnd->IsKindOf(RUNTIME_CLASS(C2DModelView)))
				{
					CTreeNodeDat *lpNodeDat = (CTreeNodeDat*)GetItemData(m_hitemDrag);

					if( m_eTreeType == fileTree && (lpNodeDat->m_nType == WELL_INFO
						|| lpNodeDat->m_nType == FILE_POINT				// 点
						|| lpNodeDat->m_nType == FILE_LINE				// 线
						|| lpNodeDat->m_nType == FILE_POLYGON			// 多边形
						|| lpNodeDat->m_nType == FILE_PLANE				// 面
						))
					{
						m_hCursor = m_ahCursor[CUR_ADD_DAT];
						SetCursor(m_hCursor);
						return NULL;
					}
				}

				//------------------------------------------------------------------------
				// 生成夹层面
				if( pDropWnd->IsKindOf(RUNTIME_CLASS(CWndGraph)))
				{
					CTreeNodeDat *lpNodeDat = (CTreeNodeDat*)GetItemData(m_hitemDrag);

					CWndGraph *pGraph = (CWndGraph*)pDropWnd;

					if( m_eTreeType == fileTree 
						&& lpNodeDat->m_nType == FILE_PLANE 
						&& (pGraph->m_eType == CWndGraph::TOP_PLANE
						|| pGraph->m_eType == CWndGraph::BOTTOM_PLANE) )// 面
					{
						m_hCursor = m_ahCursor[CUR_ADD_DAT];
						SetCursor(m_hCursor);
						return NULL;
					}
					
					if( m_eTreeType == fileTree 
						&& (lpNodeDat->m_nType == FILE_LAYER 
						|| lpNodeDat->m_nType == FILE_NORMAL
						|| lpNodeDat->m_nType == MODEL_NORMAL
						|| lpNodeDat->m_nType == MODEL_LAYER) 
						&& pGraph->m_eType == CWndGraph::HORZ )// 夹层线及法线
					{
						m_hCursor = m_ahCursor[CUR_ADD_DAT];
						SetCursor(m_hCursor);
						return NULL;
					}

					if( m_eTreeType == fileTree 
						&& lpNodeDat->m_nType == FILE_VERT_MODEL 
						&& (pGraph->m_eType == CWndGraph::VERT
						|| pGraph->m_eType == CWndGraph::HORZ))// 纵向模型
					{
						m_hCursor = m_ahCursor[CUR_ADD_DAT];
						SetCursor(m_hCursor);
						return NULL;
					}
				}				

				//------------------------------------------------------------------------
				// 3D图形
				if( pDropWnd->IsKindOf(RUNTIME_CLASS(C3DModelView)))
				{
					CTreeNodeDat *lpNodeDat = (CTreeNodeDat*)GetItemData(m_hitemDrag);

					if( (m_eTreeType == fileTree && 
						(
						lpNodeDat->m_nType == WELL_INFO
						|| lpNodeDat->m_nType == FILE_POINT				// 点
						|| lpNodeDat->m_nType == FILE_LINE				// 线
						|| lpNodeDat->m_nType == FILE_POLYGON			// 多边形
						|| lpNodeDat->m_nType == FILE_PLANE				// 面
						|| lpNodeDat->m_nType == FOLDER					// 文件夹
						))
						|| (m_eTreeType == modelTree && 
						(lpNodeDat->m_nType == GRID_DAT					// 模型
						|| lpNodeDat->m_nType == FARM_DAT				// 层
						))						
						)
					{
						m_hCursor = m_ahCursor[CUR_ADD_DAT];
						SetCursor(m_hCursor);
						return NULL;
					}
				}
			}

			// Dragging an item to itself or to a node of its subtree is
			// not allowed 

			if (( m_hitemDrag == hitem 
				|| hitem == NULL
				|| IsChildNodeOf(hitem, m_hitemDrag) 
				) && nKey != VK_F3)
			{
				m_hCursor = m_ahCursor[CUR_NODROP];
				SetCursor(m_hCursor);
				return NULL;
			}
			else
			{
				CTreeNodeDat *lpNodeDat = (CTreeNodeDat*)GetItemData(m_hitemDrag);
				
				if( (m_eTreeType == fileTree 
					&& (lpNodeDat->m_nType == WELL_DAT
					|| lpNodeDat->m_nType == WELL_INFO
					|| lpNodeDat->m_nType == WELL_HOLE
					|| lpNodeDat->m_nType == WELL_LOGING_INFO
					|| lpNodeDat->m_nType == WELL_LOGINGS
					|| lpNodeDat->m_nType == WELL_LOGING
					|| lpNodeDat->m_nType == WELL_PRO
					|| lpNodeDat->m_nType == WELL_INJ
					|| lpNodeDat->m_nType == FILE_VERT_MODELS
					|| lpNodeDat->m_nType == FILE_VERT_MODEL) )

					|| (m_eTreeType == graphTree
					&& (lpNodeDat->m_nType == GRAPHICS_2DS
					|| lpNodeDat->m_nType == GRAPHICS_3DS
					|| lpNodeDat->m_nType == GRAPHICS_LOGS))

					|| (m_eTreeType == modelTree
					&& (lpNodeDat->m_nType == FARM_LAYER
					|| lpNodeDat->m_nType == FARM_DAT))
					)
				{
					m_hCursor = m_ahCursor[CUR_NODROP];
					SetCursor(m_hCursor);
					return NULL;
				}
				else
				{
					if( hitem == NULL)
					{
						m_hCursor = m_ahCursor[CUR_NODROP];
						SetCursor(m_hCursor);
						return NULL;
					}

					lpNodeDat = (CTreeNodeDat*)GetItemData(hitem);

					if( lpNodeDat->m_nType != FOLDER)
					{
						m_hCursor = m_ahCursor[CUR_NODROP];
						SetCursor(m_hCursor);
						return NULL;
					}
				}
			}
		}

		if (nKey == VK_F3)
		{
			if(IsExistChildNode(TVI_ROOT,m_hitemDrag))
			{
				m_hCursor = m_ahCursor[CUR_NODROP];
				SetCursor(m_hCursor);
				return NULL;
			}

			if (GetKeyState(VK_CONTROL) & 0x80000000) 
				m_hCursor = m_ahCursor[CUR_COPY_ROOT];
			else
				m_hCursor = m_ahCursor[CUR_MOVE_ROOT];
		}
		else if (uFlags & (TVHT_ONITEM | TVHT_ONITEMINDENT | TVHT_ONITEMBUTTON))//平级
		{
			HTREEITEM htiParent = GetParentItem(hitem);
			if( htiParent == NULL )
				htiParent = TVI_ROOT;

			if(IsExistChildNode(htiParent,m_hitemDrag))
			{
				m_hCursor = m_ahCursor[CUR_NODROP];
				SetCursor(m_hCursor);
				return NULL;
			}

			if( hitem == NULL)
			{
				m_hCursor = m_ahCursor[CUR_NODROP];
				SetCursor(m_hCursor);
				return NULL;
			}

			CTreeNodeDat*lpNodeDat = (CTreeNodeDat*)GetItemData(hitem);

			if( lpNodeDat->m_nType != FOLDER)
			{
				m_hCursor = m_ahCursor[CUR_NODROP];
				SetCursor(m_hCursor);
				return NULL;
			}

			if (GetKeyState(VK_CONTROL) & 0x80000000) 
				m_hCursor = m_ahCursor[CUR_COPY_ABOVE];
			else
				m_hCursor = m_ahCursor[CUR_MOVE_ABOVE];
		}
		else if (GetKeyState(VK_CONTROL) & 0x80000000) // 子
		{
			if(IsExistChildNode(hitem,m_hitemDrag))
			{
				m_hCursor = m_ahCursor[CUR_NODROP];
				SetCursor(m_hCursor);
				return NULL;
			}

			if( hitem == NULL)
			{
				m_hCursor = m_ahCursor[CUR_NODROP];
				SetCursor(m_hCursor);
				return NULL;
			}

			CTreeNodeDat*lpNodeDat = (CTreeNodeDat*)GetItemData(hitem);

			if( lpNodeDat->m_nType != FOLDER)
			{
				m_hCursor = m_ahCursor[CUR_NODROP];
				SetCursor(m_hCursor);
				return NULL;
			}

			m_hCursor = m_ahCursor[CUR_COPY_SON];
		}
		else
		{
			if(IsExistChildNode(hitem,m_hitemDrag))
			{
				m_hCursor = m_ahCursor[CUR_NODROP];
				SetCursor(m_hCursor);
				return NULL;
			}

			if( hitem == NULL)
			{
				m_hCursor = m_ahCursor[CUR_NODROP];
				SetCursor(m_hCursor);
				return NULL;
			}

			CTreeNodeDat*lpNodeDat = (CTreeNodeDat*)GetItemData(hitem);

			if( lpNodeDat->m_nType != FOLDER)
			{
				m_hCursor = m_ahCursor[CUR_NODROP];
				SetCursor(m_hCursor);
				return NULL;
			}

			m_hCursor = m_ahCursor[CUR_MOVE_SON];
		}
		if (hOldCursor != m_hCursor)
			SetCursor(m_hCursor);
	}
	return hitem;
}

/////////////////////////////////////////////////////////////////////////////
// CViewTree 消息处理程序

BOOL CViewTree::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
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

BOOL CViewTree::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (GetEditControl())
	{
		if(pMsg->message == WM_KEYDOWN) 
		{
			if (pMsg->wParam == VK_INSERT)
			{
				// If VK_INSERT was preessed in course of editing, we want to cause
				// the program to add another sibling. Therefore, we simulate pressing of
				// VK_RETURN.
				m_bInsertionMode = true;
				pMsg->wParam = VK_RETURN;
			}
			else
				m_bInsertionMode = false;
		}
		::TranslateMessage(pMsg);
		::DispatchMessage(pMsg);
		return TRUE; // DO NOT process further
	}
	if (m_bDragging &&
		(pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP))
	{
		if (pMsg->wParam == VK_ESCAPE)
		{	// Cancel Drag Mode
			m_bDragging = false;
			ReleaseCapture();
			SelectDropTarget(NULL);
			m_hCursor = NULL;
		}
		POINT pt;
		GetCursorPos(&pt);
		// Cause MouseMove() (and as a result OnSetCursor()) to be called.
		SetCursorPos(pt.x, pt.y);
		return true;
	}
	if (pMsg->message == WM_KEYDOWN) 
	{
		switch(pMsg->wParam)
		{
		case 'C':
			if (GetKeyState(VK_CONTROL) & 0x80000000) 
				OnEditCopy();
			break;		
		case 'V':
			// Ctrl+V Paste m_ttTransfer as a son of current item
			// Ctrl+Shift+V Paste m_ttTransfer as a new root
			if (GetKeyState(VK_CONTROL) & 0x80000000) 
				OnEditPaste();
			break;
		case 'X':
			if (GetKeyState(VK_CONTROL) & 0x80000000) 
				OnEditCut();
			break;
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


void CViewTree::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CTreeCtrl::SetFocus();
	//CTreeCtrl::OnLButtonDblClk(nFlags, point);

	UINT uFlags = 0;
	HTREEITEM hItem = HitTest(point, &uFlags);

	if(!hItem)
	{
		CTreeCtrl::OnLButtonDblClk(nFlags, point);
		return;
	}

	CTreeNodeDat *lpData = (CTreeNodeDat *)GetItemData(hItem);

	CWaitCursor wait;
	CString strText = GetItemText(hItem);
	if( pfOpenTree )
	{
		m_bNew3D			= false;
		if( !(this->*pfOpenTree)(lpData, (LPVOID)(LPCSTR)strText) )
		{
			if(lpData->m_nType == FOLDER ) // 文件夹
			{
				ExpandM(hItem, TVE_EXPAND);
			}
			else if( m_eTreeType == fileTree && lpData->m_nType == FILE_DAT ) // 其它文件
			{
				CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
				CString strFileName = pMF->GetProjectDatPath();
				strFileName += _T("\\files\\");
				strFileName += ((CFileViewObj*)lpData->m_pNodeDat)->m_strFileName;
				if( !PathFileExists(strFileName) )
				{
					AfxMessageBox(_T("此文件不存在!"));
				}
				else
				{
//					返回值可能的错误有:
//					= 0   {内存不足}
// 					ERROR_FILE_NOT_FOUND   = 2;  {文件名错误}
// 					ERROR_PATH_NOT_FOUND   = 3;  {路径名错误}
// 					ERROR_BAD_FORMAT       = 11; {EXE 文件无效}
// 					SE_ERR_SHARE           = 26; {发生共享错误}
// 					SE_ERR_ASSOCINCOMPLETE = 27; {文件名不完全或无效}
// 					SE_ERR_DDETIMEOUT      = 28; {超时}
// 					SE_ERR_DDEFAIL         = 29; {DDE 事务失败}
// 					SE_ERR_DDEBUSY         = 30; {正在处理其他 DDE 事务而不能完成该 DDE 事务}
// 					SE_ERR_NOASSOC         = 31; {没有相关联的应用程序}

					HINSTANCE nResult = ShellExecute(NULL,"open",strFileName,_T(""), NULL,SW_SHOWNORMAL);
					
					if( (int)nResult == SE_ERR_NOASSOC) // {没有相关联的应用程序}
					{
						CDocument *pDoc = pMF->CreateOrActivateFrame(theApp.m_pTxtDocTempl, RUNTIME_CLASS(CMyRichEditView), strFileName);
						if( pDoc ) 
						{
							CString strTitle = GetItemText(hItem);
							pDoc->SetPathName(strFileName);
							pDoc->SetTitle(strTitle);
						}
					}
				}
			}
		}
	}
}

void CViewTree::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	UINT uFlags = 0;
	HTREEITEM hItem = HitTest(point, &uFlags);

	if( hItem != NULL )
	{
		CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
		if( m_eTreeType == fileTree )
		{
			pMF->GetTreeGraphView()->GetTreeCtrl()->m_hSelectItem = NULL;
			pMF->GetTreeModelView()->GetTreeCtrl()->m_hSelectItem = NULL;
		}
		else if( m_eTreeType == graphTree )
		{
			pMF->GetTreeFileView()->GetTreeCtrl()->m_hSelectItem = NULL;
			pMF->GetTreeModelView()->GetTreeCtrl()->m_hSelectItem = NULL;
		}
		else
		{
			pMF->GetTreeGraphView()->GetTreeCtrl()->m_hSelectItem = NULL;
			pMF->GetTreeFileView()->GetTreeCtrl()->m_hSelectItem = NULL;
		}

		m_hSelectItem = hItem;
	}

	if( hItem != NULL && uFlags & TVHT_ONITEMSTATEICON)
	{
		int nCheck = GetCheck(hItem);
		if( nCheck == 2 )
			SetItemAndChildrenEmpty(hItem);
		else if( nCheck == 1 )
			SetItemAndChildrenFull(hItem);
		else if( nCheck == 0 )
			SetItemAndChildrenFull(hItem);

		VerifFullParents(hItem);
		VerifBoldParents(hItem);

		if( m_eTreeType == fileTree )
		{
			CTreeNodeDat *pTreeNode = (CTreeNodeDat *)GetItemData(hItem);
			if( pTreeNode != NULL)
			{
				if( pTreeNode->m_nType == WELL_LOGINGS )
				{
					hItem = GetNextSiblingItem(hItem);
					while(hItem != NULL)
					{
						if( nCheck == 2 )
							SetItemAndChildrenEmpty(hItem);
						else if( nCheck == 1 )
							SetItemAndChildrenFull(hItem);
						else if( nCheck == 0 )
							SetItemAndChildrenFull(hItem);

						VerifFullParents(hItem);
						VerifBoldParents(hItem);

						hItem = GetNextSiblingItem(hItem);
					}
				}
				else if( pTreeNode->m_nType == WELL_LOGING )
				{
					CString strText = GetItemText(hItem);
					hItem = GetParentItem(hItem);
					CTreeNodeDat *lpNode = (CTreeNodeDat *)GetItemData(hItem);

					if( lpNode->m_nType == WELL_LOGINGS )
					{
						hItem = GetNextSiblingItem(hItem);
						while(hItem != NULL)
						{
							HTREEITEM h = GetChildItem(hItem);
							while(h!= NULL)
							{
								CString str = GetItemText(h);
								if( str == strText)
								{
									if( nCheck == 2 )
										SetItemAndChildrenEmpty(h);
									else if( nCheck == 1 )
										SetItemAndChildrenFull(h);
									else if( nCheck == 0 )
										SetItemAndChildrenFull(h);

									VerifFullParents(h);
									VerifBoldParents(h);
								}
								h = GetNextSiblingItem(h);
							}

							hItem = GetNextSiblingItem(hItem);
						}
						
					}					
				}
			}
		}

		return;
	}

	CTreeCtrl::OnLButtonDown(nFlags, point);
}

void CViewTree::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();

	if (m_bDragging)
	{
		m_bDragging = FALSE;

		if (m_bTimerABOVE_Active)
		{
			KillTimer(TVHT_ABOVE);
			m_bTimerABOVE_Active = false;
		}
		if (m_bTimerBELOW_Active)
		{
			KillTimer(TVHT_BELOW);
			m_bTimerBELOW_Active = false;
		}

		if (m_hCursor == m_ahCursor[CUR_COPY_ROOT] || 
			m_hCursor == m_ahCursor[CUR_MOVE_ROOT])
		{
			CSharedFile file;
			SaveTreeData(&file, m_hitemDrag);
			if (m_hCursor == m_ahCursor[CUR_MOVE_ROOT])
				DeleteItemM(m_hitemDrag);
			HGLOBAL hMem = file.Detach();
			HTREEITEM hItem = LoadTreeData(hMem, TVI_ROOT);
			SelectVisibleItem(hItem);
			ExpandM(m_hitemDrop, TVE_EXPAND);
		}
		// Dragging an item to itself or to a node of its subtree is
		// not allowed 
		else if (m_hitemDrag != m_hitemDrop && !IsChildNodeOf(m_hitemDrop, m_hitemDrag))
		{
			if (m_hCursor == m_ahCursor[CUR_MOVE_SON] ||
				m_hCursor == m_ahCursor[CUR_COPY_SON])
			{
				CSharedFile file;
				SaveTreeData(&file, m_hitemDrag);
				if (m_hCursor == m_ahCursor[CUR_MOVE_SON])
					DeleteItemM(m_hitemDrag);
				HGLOBAL hMem = file.Detach();
				HTREEITEM hItem = LoadTreeData(hMem, m_hitemDrop);
				SelectVisibleItem(hItem);
				ExpandM(m_hitemDrop, TVE_EXPAND);
			}
			else if (m_hCursor == m_ahCursor[CUR_MOVE_ABOVE] ||
				m_hCursor == m_ahCursor[CUR_COPY_ABOVE])
			{
				CSharedFile file;
				SaveTreeData(&file, m_hitemDrag);
				if (m_hCursor == m_ahCursor[CUR_MOVE_ABOVE])
					DeleteItemM(m_hitemDrag);
				HGLOBAL hMem = file.Detach();
				HTREEITEM hItem = LoadTreeData(hMem, m_hitemDrop);
				SelectVisibleItem(hItem);
			}
			else
			{
				CTreeNodeDat *lpNodeDat = (CTreeNodeDat*)GetItemData(m_hitemDrag);

				CPoint scrPt = point;
				ClientToScreen(&scrPt);
				CWnd* pDropWnd = WindowFromPoint (scrPt);
				if( pDropWnd != NULL
					&& pDropWnd->m_hWnd != this->m_hWnd )
				{
					//------------------------------------------------------------------------
					// 2D图形
					if( pDropWnd->IsKindOf(RUNTIME_CLASS(CGeoMapView)))
					{
						if( m_eTreeType == fileTree )
						{
							switch( lpNodeDat->m_nType )
							{
							case WELL_INFO:
								{
									CGeoMapDoc *pDoc = ((CGeoMapView*)pDropWnd)->GetDocument();
									pDoc->CreateWellLocationMap(m_hitemDrag);
								}
								break;
							case FILE_PICTRUE:				// 图片
								{
									CString strTargePathName = pMF->GetProjectDatPath();
									CGeoMapDoc *pDoc = ((CGeoMapView*)pDropWnd)->GetDocument();
									CString strFileName = ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;

									CString strSourcePathName = strTargePathName;
									strSourcePathName += _T("\\files\\");
									strSourcePathName += strFileName;

									strTargePathName += _T("\\other files\\");
									strTargePathName += strFileName;

									CopyFile(strSourcePathName, strTargePathName,FALSE);

									pDoc->CreateLayerPictrue(strFileName);
								}
								break;
							case FILE_POINT:				// 点
								{
									CString strSourcePathName = pMF->GetProjectDatPath();
									CGeoMapDoc *pDoc = ((CGeoMapView*)pDropWnd)->GetDocument();
									CString strFileName = ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;
									strSourcePathName += _T("\\files\\");
									strSourcePathName += strFileName;

									CString strText = GetItemText(m_hitemDrag);

									pDoc->CreatePointMap(strSourcePathName, strText);
								}
								break;
							case FILE_LAYER:	// 夹层线
							case MODEL_LAYER:
								{
									CString strSourcePathName = pMF->GetProjectDatPath();
									CGeoMapDoc *pDoc = ((CGeoMapView*)pDropWnd)->GetDocument();
									CString strFileName = ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;
									strSourcePathName += _T("\\files\\");
									strSourcePathName += strFileName;

									CString strText = GetItemText(m_hitemDrag);

									pDoc->CreateInterLayer(strSourcePathName, strText);
								}
								break;
							case FILE_NORMAL: // 法线
							case MODEL_NORMAL:
								{
									CString strSourcePathName = pMF->GetProjectDatPath();
									CGeoMapDoc *pDoc = ((CGeoMapView*)pDropWnd)->GetDocument();
									CString strFileName = ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;
									strSourcePathName += _T("\\files\\");
									strSourcePathName += strFileName;

									CString strText = GetItemText(m_hitemDrag);

									pDoc->CreateInterNormal(strSourcePathName, strText);
								}
								break;
							case FILE_LAYER_MODEL:
								{
									HTREEITEM hItem = GetChildItem(m_hitemDrag);
									while(hItem != NULL)
									{
										if( GetCheck(hItem) )
										{
											CTreeNodeDat *lpNode = (CTreeNodeDat *)GetItemData(hItem);
											switch( lpNode->m_nType )
											{
											case MODEL_LAYER:
												{
													CString strSourcePathName = pMF->GetProjectDatPath();
													CGeoMapDoc *pDoc = ((CGeoMapView*)pDropWnd)->GetDocument();
													CString strFileName = ((CFileViewObj*)lpNode->m_pNodeDat)->m_strFileName;
													strSourcePathName += _T("\\files\\");
													strSourcePathName += strFileName;

													CString strText = GetItemText(hItem);

													pDoc->CreateInterLayer(strSourcePathName, strText);
												}
												break;
											case MODEL_NORMAL:
												{
													CString strSourcePathName = pMF->GetProjectDatPath();
													CGeoMapDoc *pDoc = ((CGeoMapView*)pDropWnd)->GetDocument();
													CString strFileName = ((CFileViewObj*)lpNode->m_pNodeDat)->m_strFileName;
													strSourcePathName += _T("\\files\\");
													strSourcePathName += strFileName;

													CString strText = GetItemText(hItem);

													pDoc->CreateInterNormal(strSourcePathName, strText);
												}
												break;
											default:
												break;

											}
										}
										hItem = GetNextSiblingItem(hItem);
									}
								}
								break;
							case FOLDER:
								{
									CWaitCursor wait;
									HTREEITEM hItem = GetChildItem(m_hitemDrag);

									int num = 0;

									while(hItem != NULL)
									{
										num++;
										hItem = GetNextSiblingItem(hItem);
									}

									if( num > 0 )
										StatusProgress(1, num);

									num = 0;

									hItem = GetChildItem(m_hitemDrag);
									while(hItem != NULL)
									{
										if( GetCheck(hItem) )
										{
											CTreeNodeDat *lpNode = (CTreeNodeDat *)GetItemData(hItem);
											switch( lpNode->m_nType )
											{
											case FILE_LAYER:
											case MODEL_LAYER:
												{
													CString strSourcePathName = pMF->GetProjectDatPath();
													CGeoMapDoc *pDoc = ((CGeoMapView*)pDropWnd)->GetDocument();
													CString strFileName = ((CFileViewObj*)lpNode->m_pNodeDat)->m_strFileName;
													strSourcePathName += _T("\\files\\");
													strSourcePathName += strFileName;

													CString strText = GetItemText(hItem);

													pDoc->CreateInterLayer(strSourcePathName, strText);
												}
												break;
											case FILE_NORMAL:
											case MODEL_NORMAL:
												{
													CString strSourcePathName = pMF->GetProjectDatPath();
													CGeoMapDoc *pDoc = ((CGeoMapView*)pDropWnd)->GetDocument();
													CString strFileName = ((CFileViewObj*)lpNode->m_pNodeDat)->m_strFileName;
													strSourcePathName += _T("\\files\\");
													strSourcePathName += strFileName;

													CString strText = GetItemText(hItem);

													pDoc->CreateInterNormal(strSourcePathName, strText);
												}
												break;
											case FILE_POINT:
												{
													CString strSourcePathName = pMF->GetProjectDatPath();
													CGeoMapDoc *pDoc = ((CGeoMapView*)pDropWnd)->GetDocument();
													CString strFileName = ((CFileViewObj*)lpNode->m_pNodeDat)->m_strFileName;
													strSourcePathName += _T("\\files\\");
													strSourcePathName += strFileName;

													CString strText = GetItemText(hItem);

													pDoc->CreatePointMap(strSourcePathName, strText);
												}
												break;
											case FILE_PICTRUE:				// 图片
												{
													CString strTargePathName = pMF->GetProjectDatPath();
													CGeoMapDoc *pDoc = ((CGeoMapView*)pDropWnd)->GetDocument();
													CString strFileName = ((CFileViewObj*)lpNode->m_pNodeDat)->m_strFileName;

													CString strSourcePathName = strTargePathName;
													strSourcePathName += _T("\\files\\");
													strSourcePathName += strFileName;

													strTargePathName += _T("\\other files\\");
													strTargePathName += strFileName;

													CopyFile(strSourcePathName, strTargePathName,FALSE);

													pDoc->CreateLayerPictrue(strFileName);
												}
												break;
											case FILE_LINE:				// 线
												{
													CString strSourcePathName = pMF->GetProjectDatPath();
													CGeoMapDoc *pDoc = ((CGeoMapView*)pDropWnd)->GetDocument();
													CString strFileName = ((CFileViewObj*)lpNode->m_pNodeDat)->m_strFileName;
													strSourcePathName += _T("\\files\\");
													strSourcePathName += strFileName;

													CString strText = GetItemText(hItem);

													pDoc->CreatePolyline(strSourcePathName, strText);
												}
												break;
											case FILE_POLYGON:			// 多边形
												{
													CString strSourcePathName = pMF->GetProjectDatPath();
													CGeoMapDoc *pDoc = ((CGeoMapView*)pDropWnd)->GetDocument();
													CString strFileName = ((CFileViewObj*)lpNode->m_pNodeDat)->m_strFileName;
													strSourcePathName += _T("\\files\\");
													strSourcePathName += strFileName;
													CString strText = GetItemText(hItem);
													pDoc->CreatePolygon(strSourcePathName, strText);
												}
												break;
											case FILE_PLANE:				// 面
												{
													CString strSourcePathName = pMF->GetProjectDatPath();
													CGeoMapDoc *pDoc = ((CGeoMapView*)pDropWnd)->GetDocument();
													CString strFileName = ((CFileViewObj*)lpNode->m_pNodeDat)->m_strFileName;
													strSourcePathName += _T("\\files\\");
													strSourcePathName += strFileName;
													CString strText = GetItemText(hItem);
													pDoc->CreatePlane(strSourcePathName, strText);
												}
												break;
											default:
												break;

											}
										}
										hItem = GetNextSiblingItem(hItem);

										num ++;

										StatusSetProgress(1, num);
									}
									if( num > 0 )
										StatusProgress(1);
								}
								break;
							case FILE_LINE:				// 线
								{
									CString strSourcePathName = pMF->GetProjectDatPath();
									CGeoMapDoc *pDoc = ((CGeoMapView*)pDropWnd)->GetDocument();
									CString strFileName = ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;
									strSourcePathName += _T("\\files\\");
									strSourcePathName += strFileName;

									CString strText = GetItemText(m_hitemDrag);

									pDoc->CreatePolyline(strSourcePathName, strText);
								}
								break;
							case FILE_POLYGON:			// 多边形
								{
									CString strSourcePathName = pMF->GetProjectDatPath();
									CGeoMapDoc *pDoc = ((CGeoMapView*)pDropWnd)->GetDocument();
									CString strFileName = ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;
									strSourcePathName += _T("\\files\\");
									strSourcePathName += strFileName;
									CString strText = GetItemText(m_hitemDrag);
									pDoc->CreatePolygon(strSourcePathName, strText);
								}
								break;
							case FILE_PLANE:				// 面
								{
									CWaitCursor wait;
									CString strSourcePathName = pMF->GetProjectDatPath();
									CGeoMapDoc *pDoc = ((CGeoMapView*)pDropWnd)->GetDocument();
									CString strFileName = ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;
									strSourcePathName += _T("\\files\\");
									strSourcePathName += strFileName;
									CString strText = GetItemText(m_hitemDrag);
									pDoc->CreatePlane(strSourcePathName, strText);
								}
								break;
							default:
								break;
							}
						}
					}
					//------------------------------------------------------------------------
					//生成夹层
					else if( pDropWnd->IsKindOf(RUNTIME_CLASS(CWndGraph)))
					{
						CWndGraph *pGraph = (CWndGraph*)pDropWnd;
						if( m_eTreeType == fileTree )
						{
							switch( lpNodeDat->m_nType )
							{
							case FILE_LAYER:
								{
									if( pGraph->m_eType != CWndGraph::HORZ )
										break;

									CString strSourcePathName = pMF->GetProjectDatPath();								
									CString strFileName = ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;
									strSourcePathName += _T("\\files\\");
									strSourcePathName += strFileName;

									pGraph->AddLayers(strSourcePathName);
								}
								break;
							case FILE_NORMAL:
								{
									if( pGraph->m_eType != CWndGraph::HORZ )
										break;
									CString strSourcePathName = pMF->GetProjectDatPath();

									CString strFileName = ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;
									strSourcePathName += _T("\\files\\");
									strSourcePathName += strFileName;
									pGraph->AddNormals(strSourcePathName);
								}
								break;
							case FILE_PLANE:				// 面
								{
									if( pGraph->m_eType != CWndGraph::TOP_PLANE 
										&& pGraph->m_eType != CWndGraph::BOTTOM_PLANE )
										break;
									CString strSourcePathName = pMF->GetProjectDatPath();
									CString strFileName = ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;
									strSourcePathName += _T("\\files\\");
									strSourcePathName += strFileName;
									if( pGraph->m_eType == CWndGraph::TOP_PLANE )
										pGraph->AddPlanes(strSourcePathName, true);
									else
										pGraph->AddPlanes(strSourcePathName, false);
								}
								break;
							case FILE_VERT_MODEL:
								{
									if( pGraph->m_eType != CWndGraph::VERT 
										&& pGraph->m_eType != CWndGraph::HORZ )
										break;

									CString strVertModelName = GetItemText(m_hitemDrag);

									CFormGenerateLayer *pView = (CFormGenerateLayer *)pGraph->GetParent();
									pView->AddVertModel(strVertModelName);									
								}
								break;
							default:
								break;
							}
						}
					}
					//------------------------------------------------------------------------
					// 2D模型
					else if( pDropWnd->IsKindOf(RUNTIME_CLASS(C2DModelView)))
					{
						if( m_eTreeType == fileTree )
						{
							switch( lpNodeDat->m_nType )
							{
							case WELL_INFO:
								{

								}
								break;					
							case FILE_POINT:			// 点
								{

								}
								break;
							case FILE_LINE:				// 线
								break;
							case FILE_POLYGON:			// 多边形
								break;
							case FILE_PLANE:			// 面
								break;
							default:
								break;
							}
						}
					}
					//------------------------------------------------------------------------
					// 3D图形
					else if( pDropWnd->IsKindOf(RUNTIME_CLASS(C3DModelView)))
					{
						if( m_eTreeType == fileTree )
						{
							switch( lpNodeDat->m_nType )
							{
							case WELL_INFO:
								{
									C3DModelDoc *pDoc = ((C3DModelView*)pDropWnd)->GetDocument();
									pDoc->AddWellPath(m_hitemDrag);									
								}
								break;
							case FILE_POINT:			// 点								
								break;
							case FILE_LINE:				// 线
								break;
							case FILE_POLYGON:			// 多边形
								break;
							case FILE_PLANE:			// 面
								{	
									CWaitCursor wait;
									CString strSourcePathName = pMF->GetProjectDatPath();
									CString strFileName = ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;
									strSourcePathName += _T("\\files\\");
									strSourcePathName += strFileName;

									CString strText = GetItemText(m_hitemDrag);
									C3DModelDoc *pDoc = ((C3DModelView*)pDropWnd)->GetDocument();
									pDoc->AddPlane(strSourcePathName, strText);
								}
								break;
							case FOLDER:
								{
									CWaitCursor wait;
									HTREEITEM hItem = GetChildItem(m_hitemDrag);
									int num = 0;

									while(hItem != NULL)
									{
										if( GetCheck(hItem) )
											num++;
										hItem = GetNextSiblingItem(hItem);
									}

									if( num > 0 )
										StatusProgress(1, num);

									num = 0;

									hItem = GetChildItem(m_hitemDrag);
									while(hItem != NULL)
									{
										if( GetCheck(hItem) )
										{
											CTreeNodeDat *lpNode1 = (CTreeNodeDat *)GetItemData(hItem);
											switch( lpNode1->m_nType )
											{
											case FILE_PLANE:				// 面
												{
													CString strSourcePathName = pMF->GetProjectDatPath();
													CString strFileName = ((CFileViewObj*)lpNode1->m_pNodeDat)->m_strFileName;
													strSourcePathName += _T("\\files\\");
													strSourcePathName += strFileName;

													CString strText = GetItemText(hItem);
													StatusBarMessage(_T("正在添加面：%s"), strText);
													C3DModelDoc *pDoc = ((C3DModelView*)pDropWnd)->GetDocument();
													pDoc->AddPlane(strSourcePathName, strText);													
												}
												break;
											//case GRID_LAYER:			// 网格模型
											//	{
											//		C3DModelDoc *pDoc = ((C3DModelView*)pDropWnd)->GetDocument();

											//		CGLObject*pObj = NULL;

											//		{
											//			CWaitCursor wait;
											//			//CString strSourcePathName = pMF->GetProjectDatPath();
											//			CString strFileName = ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;
											//			//strSourcePathName += _T("\\models\\");
											//			//strSourcePathName += strFileName;

											//			CString strText = GetItemText(m_hitemDrag);

											//			//CStringArray ar1, ar2;
											//			pDoc->AddInterlayer(strFileName, strText);
											//		}
											//	}
											//	break;
											default:
												break;

											}
											num ++;

											StatusSetProgress(1, num);
										}
										hItem = GetNextSiblingItem(hItem);										
									}
									if( num > 0 )
										StatusProgress(1);
								}
								break;

							default:
								break;
							}
						}
						else if(m_eTreeType == modelTree)
						{
							switch( lpNodeDat->m_nType )
							{
							case GRID_DAT:			// 网格模型
								{
									C3DModelDoc *pDoc = ((C3DModelView*)pDropWnd)->GetDocument();
									std::vector<CGLObject*>::iterator iTer;
									CGLObject*pObj = NULL;
									bool bIsExist = false;
									for (iTer = pDoc->GetContext()->m_listDisplay->begin();
										iTer!=pDoc->GetContext()->m_listDisplay->end();
										iTer++)
									{
										pObj = (*iTer);
										if( pObj->GetGLObjType() == GLSURFACE)
										{
											AfxMessageBox(_T("图中已经存在网格模型了!"), MB_OK|MB_ICONINFORMATION);
											bIsExist = true;
											break;
										}
									}

									if(!bIsExist)
									{
										CWaitCursor wait;
										CString strSourcePathName = pMF->GetProjectDatPath();
										CString strFileName = ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;
										strSourcePathName += _T("\\models\\");
										strSourcePathName += strFileName;

										CString strText = GetItemText(m_hitemDrag);
										CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)GetItemData(m_hitemDrag);
										CStringArray ar1, ar2;
										pDoc->AddGridModel(strSourcePathName, strText, ar1, ar2, lpNodeDat->m_strGUIDName);
									}
								}
								break;
							case GRID_LAYER:			// 网格模型
								{
									C3DModelDoc *pDoc = ((C3DModelView*)pDropWnd)->GetDocument();

									CGLObject*pObj = NULL;

									{
										CWaitCursor wait;
										//CString strSourcePathName = pMF->GetProjectDatPath();
										CString strFileName = ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;
										//strSourcePathName += _T("\\models\\");
										//strSourcePathName += strFileName;

										CString strText = GetItemText(m_hitemDrag);

										CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)GetItemData(m_hitemDrag);
										CTreeNodeDat *lpNodeDatModel = (CTreeNodeDat *)GetItemData(GetParentItem(m_hitemDrag));
										//CStringArray ar1, ar2;
										pDoc->AddInterlayer(strFileName, strText,lpNodeDat->m_strGUIDName,lpNodeDatModel->m_strGUIDName);
									}
								}
								break;
							case FARM_DAT:
								{
									CWaitCursor wait;
									CString strSourcePathName = pMF->GetProjectDatPath();
									CString strFileName = ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;
									strSourcePathName += _T("\\models\\");
									strSourcePathName += strFileName;

									CString strText = GetItemText(m_hitemDrag);
									C3DModelDoc *pDoc = ((C3DModelView*)pDropWnd)->GetDocument();
									pDoc->AddPhyParam(strFileName, strText);
								}
								break;

							case FOLDER:
								{
									CWaitCursor wait;
									HTREEITEM hItem = GetChildItem(m_hitemDrag);
									int num = 0;

									while(hItem != NULL)
									{
										if( GetCheck(hItem) )
											num++;
										hItem = GetNextSiblingItem(hItem);
									}

									if( num > 0 )
										StatusProgress(1, num);

									num = 0;

									hItem = GetChildItem(m_hitemDrag);
									while(hItem != NULL)
									{
										if( GetCheck(hItem) )
										{
											CTreeNodeDat *lpNode = (CTreeNodeDat *)GetItemData(hItem);
											switch( lpNode->m_nType )
											{
											case  GRID_DAT:
												{
													//AfxMessageBox("不能同时导入多个网格模型");
													//return ;
												}
												break;
											case GRID_LAYER:			// 网格模型
												{
													C3DModelDoc *pDoc = ((C3DModelView*)pDropWnd)->GetDocument();

													CGLObject*pObj = NULL;

													{
														CWaitCursor wait;
														//CString strSourcePathName = pMF->GetProjectDatPath();
														CString strFileName = ((CFileViewObj*)lpNode->m_pNodeDat)->m_strFileName;
														//strSourcePathName += _T("\\models\\");
														//strSourcePathName += strFileName;

														CString strText = GetItemText(m_hitemDrag);

														CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)GetItemData(m_hitemDrag);
														CTreeNodeDat *lpNodeDatModel = (CTreeNodeDat *)GetItemData(GetParentItem(m_hitemDrag));
														//CStringArray ar1, ar2;
														pDoc->AddInterlayer(strFileName, strText,lpNodeDat->m_strGUIDName,lpNodeDatModel->m_strGUIDName);
													}
												}
												break;
											default:
												break;

											}
											num ++;

											StatusSetProgress(1, num);
										}
										hItem = GetNextSiblingItem(hItem);										
									}
									if( num > 0 )
										StatusProgress(1);
								}
								break;
							default:
								break;
							}
						}

					}
				}
			}
		}
		else
		{
			MessageBeep(0);
			SelectVisibleItem(m_hitemDrag);
		} 

		ReleaseCapture();
		SelectDropTarget(NULL);
		m_hCursor = NULL;

		m_pDragImage->DragLeave (GetDesktopWindow ());
		m_pDragImage->EndDrag ();
		delete m_pDragImage; //must delete it because it was created at the beginning of the drag
		m_pDragImage = NULL;
	}

	CTreeCtrl::OnLButtonUp(nFlags, point);
}

void CViewTree::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	HTREEITEM	hitem;

	if (m_bDragging)
	{
		hitem = SelectDropCursor(&point);
		if (hitem != NULL)
		{
			SelectDropTarget(hitem);
			m_hitemDrop = hitem;
		}
	}

	CTreeCtrl::OnMouseMove(nFlags, point);
}

void CViewTree::OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	CPoint ptAction;

	GetCursorPos(&ptAction);
	ScreenToClient(&ptAction);
	ASSERT(!m_bDragging);
	m_bDragging = TRUE;
	m_bTimerABOVE_Active = false;
	m_bTimerBELOW_Active = false;
	m_uOldFlags = 0;

	// determine the item being dragged:
	m_hitemDrag = pNMTreeView->itemNew.hItem;
	m_hitemDrop = NULL;

	m_pDragImage = CreateDragImage(m_hitemDrag);
	
	int nOffset = - 4; 
	m_pDragImage->BeginDrag(0, CPoint(nOffset, nOffset - 4));
	m_pDragImage->DragEnter(GetDesktopWindow(), pNMTreeView->ptDrag);

	SetCapture();

	SelectDropCursor();

	*pResult = 0;
}

void CViewTree::OnTvnItemexpanded(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码	

	CTreeNodeDat *lpNodeDat = (CTreeNodeDat*)GetItemData(pNMTreeView->itemNew.hItem);
	if( lpNodeDat )
		lpNodeDat->m_uState = pNMTreeView->itemNew.state;
	*pResult = 0;
}

void CViewTree::OnTvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	CEdit *pEdit;
	pEdit = GetEditControl();
	pEdit->SetLimitText(1024);

	*pResult = 0;
}

void CViewTree::OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	if (pTVDispInfo->item.pszText != NULL)
	{
		HTREEITEM htiParent = GetParentItem(pTVDispInfo->item.hItem);
		if( htiParent == NULL )
			htiParent = TVI_ROOT;
		if (IsExistChildNode(htiParent,pTVDispInfo->item.pszText))
		{
			EditLabel(pTVDispInfo->item.hItem);
			return;
		}
		else if( strlen(pTVDispInfo->item.pszText) < 1 )
		{
			EditLabel(pTVDispInfo->item.hItem);
			return;
		}

		TVITEM tvi;
		tvi.hItem = pTVDispInfo->item.hItem;		
		pTVDispInfo->item.mask = TVIF_TEXT;
		SetItem(&pTVDispInfo->item);
		SetModifiedFlag();

		CTreeNodeDat *pNodeDat = (CTreeNodeDat *)GetItemData(pTVDispInfo->item.hItem);
		CMultiDocTemplate *pTemplate = NULL;
		CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
		CString strTargePathName = pMF->GetProjectDatPath();

		switch(m_eTreeType)
		{
		case fileTree:
			{	
				switch(pNodeDat->m_nType)
				{
				case FILE_PICTRUE:
					{
						pTemplate = theApp.m_pPictrueDocTempl;
						strTargePathName += _T("\\files\\");
					}
					break;
				case FILE_POINT:
					{
						pTemplate = theApp.m_pFileDatDocTemplate;
						strTargePathName += _T("\\files\\");
					}
					break;
				case FILE_LINE:
					{
						pTemplate = theApp.m_pFileDatDocTemplate;
						strTargePathName += _T("\\files\\");
					}
					break;
				case FILE_POLYGON:
					{
						pTemplate = theApp.m_pFileDatDocTemplate;
						strTargePathName += _T("\\files\\");
					}
					break;
				case FILE_PLANE: 
					{
						pTemplate = theApp.m_pFileDatDocTemplate;
						strTargePathName += _T("\\files\\");
					}
					break;
				case FILE_LAYER_MODEL: 
					{
						pTemplate = theApp.m_pLayerDocTemplate;
						strTargePathName += _T("\\files\\");
					}
					break;
				default:
					break;
				}			
				
				if( pTemplate != NULL )
				{
					strTargePathName += ((CFileViewObj*)pNodeDat->m_pNodeDat)->m_strFileName;

					CDocTemplate::Confidence match;

					CDocument* pDoc = NULL;
					match = pTemplate->MatchDocType(strTargePathName, pDoc);

					if (match == CDocTemplate::yesAlreadyOpen) // 已经有文档打开
					{
						CView* pView;
						POSITION pos = pDoc->GetFirstViewPosition ();
						while (pos != NULL)
						{
							pView = pDoc->GetNextView (pos);
							pView->GetParentFrame ()->ActivateFrame ();
						}

						pDoc->SetTitle(pTVDispInfo->item.pszText);
					}
				}
			}
			break;
		case modelTree:
			{	
				switch(pNodeDat->m_nType)
				{
				case FARM_DAT:
					{
						pTemplate = theApp.m_p2DModelDocTemplate;
						strTargePathName += _T("\\models\\");
					}
					break;
				default:
					break;
				}			
				
				if( pTemplate != NULL )
				{
					strTargePathName += ((CFileViewObj*)pNodeDat->m_pNodeDat)->m_strFileName;

					CDocTemplate::Confidence match;

					CDocument* pDoc = NULL;
					match = pTemplate->MatchDocType(strTargePathName, pDoc);

					if (match == CDocTemplate::yesAlreadyOpen) // 已经有文档打开
					{
						CView* pView;
						POSITION pos = pDoc->GetFirstViewPosition ();
						while (pos != NULL)
						{
							pView = pDoc->GetNextView (pos);
							pView->GetParentFrame ()->ActivateFrame ();
						}

						pDoc->SetTitle(pTVDispInfo->item.pszText);
					}
				}
			}
			break;
		case graphTree:
			{
				switch(pNodeDat->m_nType)
				{
				case FILE_2D:
					{
						pTemplate = theApp.m_pGeoMapDocTemplate;
						strTargePathName += _T("\\graphics\\");
					}
					break;
				case FILE_3D:
					{
						pTemplate = theApp.m_p3DModelDocTemplate;
						strTargePathName += _T("\\graphics\\");
					}
					break;
				case FILE_LOG:
					{
						pTemplate = theApp.m_pLoggingDocTemplate;
						strTargePathName += _T("\\graphics\\");
					}
					break;
				default:
					break;
				}
				
				if( pTemplate != NULL )
				{
					strTargePathName += ((CFileViewObj*)pNodeDat->m_pNodeDat)->m_strFileName;

					CDocTemplate::Confidence match;

					CDocument* pDoc = NULL;
					match = pTemplate->MatchDocType(strTargePathName, pDoc);

					if (match == CDocTemplate::yesAlreadyOpen) // 已经有文档打开
					{
						CView* pView;
						POSITION pos = pDoc->GetFirstViewPosition ();
						while (pos != NULL)
						{
							pView = pDoc->GetNextView (pos);
							pView->GetParentFrame ()->ActivateFrame ();
						}

						pDoc->SetTitle(pTVDispInfo->item.pszText);
					}
				}
			}
			break;
		default:
			break;
		}
	}
 	
	if (m_bInsertionMode) // VK_INSERT was pressed in course of editing
	{
		int nImage = 0;
		
		if( m_eTreeType == fileTree )
			nImage = m_nCountImages - 19;

		AddSiblingFolder(NULL, nImage);
	}
	else
		m_bInsertionMode = false;

	*pResult = TRUE; // TRUE => Accept the label

	//*pResult = 0;
}

void CViewTree::OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	CPoint p(GetCurrentMessage()->pt);
	CPoint pt(p);
	ScreenToClient(&pt);
	HTREEITEM hItem = HitTest(pt);
	if( hItem != NULL )
	{
		Select(HitTest(pt), TVGN_CARET);
		CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
		if( m_eTreeType == fileTree )
		{
			pMF->GetTreeGraphView()->GetTreeCtrl()->m_hSelectItem = NULL;
			pMF->GetTreeModelView()->GetTreeCtrl()->m_hSelectItem = NULL;
		}
		else if( m_eTreeType == graphTree )
		{
			pMF->GetTreeFileView()->GetTreeCtrl()->m_hSelectItem = NULL;
			pMF->GetTreeModelView()->GetTreeCtrl()->m_hSelectItem = NULL;
		}
		else
		{
			pMF->GetTreeGraphView()->GetTreeCtrl()->m_hSelectItem = NULL;
			pMF->GetTreeFileView()->GetTreeCtrl()->m_hSelectItem = NULL;
		}

		m_hSelectItem = hItem;
	}

	SetFocus();
	*pResult = 0;
}

void CViewTree::OnExpandAllTree() 
{
	ParseTree((PARSING_FUNCTION)&CViewTree::pfExpand);
}

void CViewTree::OnCollapseAllEntireTree() 
{
	ParseTree((PARSING_FUNCTION)&CViewTree::pfCollapse);
}

void CViewTree::ParseTree(PARSING_FUNCTION pf, HTREEITEM hti /* = TVI_ROOT */,
						  void *pvParam /* = NULL */)
{
	if (hti == TVI_ROOT)
		hti = GetRootItem(); 
	else
	{
		hti = GetChildItem(hti); 
	}
	while(hti != NULL)
	{
		(this->*pf)(hti, pvParam);
		ParseTree(pf, hti, pvParam);
		hti = GetNextSiblingItem(hti);
	}
}

void CViewTree::pfExpand(HTREEITEM hti, void *)
{
	ExpandM(hti, TVE_EXPAND);
}

void CViewTree::pfCollapse(HTREEITEM hti, void *)
{
	ExpandM(hti, TVE_COLLAPSE);
}

void CViewTree::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == TVHT_ABOVE )
		SendMessage(WM_VSCROLL, SB_LINEUP, NULL);
	else
		SendMessage(WM_VSCROLL, SB_LINEDOWN, NULL);

	CTreeCtrl::OnTimer(nIDEvent);
}

void CViewTree::OnDestroy()
{	
	FreeAllChildNodeData(TVI_ROOT);

	CTreeCtrl::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}

//------------------------------------------------------------------------
// 添加邻节点
BOOL CViewTree::AddSiblingFolder(CObject *pNodeDat, int iImage)
{
	HTREEITEM hti, htiParent = NULL, htiChild = NULL;
	hti = GetSelectedItem();
	if( hti != NULL )
	{
		CTreeNodeDat *lpData = (CTreeNodeDat *)GetItemData(hti);
		if( lpData != NULL)
		{
			if( m_eTreeType == fileTree )
			{
				switch(lpData->m_nType)
				{
				case FOLDER: // 文件夹时
					htiParent = GetParentItem(hti);
					break;
				case WELL_DAT:				// 井基础数据
				case WELL_INFO:				// 井基础信息数据
				case WELL_HOLE:				// 井眼轨迹数据
				case WELL_LOGING_INFO: 
				case WELL_LOGINGS:			// 测井曲线数据集
				case WELL_LOGING:			// 测井曲线数据集
				case WELL_PRO:				// 生产井
				case WELL_INJ:				// 注入井
				case FILE_VERT_MODELS:		// 纵向模型集
				case FILE_VERT_MODEL:		// 纵抽模型
					htiParent = TVI_ROOT;
					hti = TVI_LAST;
					break;
				default:
					return FALSE;
				}
			}
		}
		else
		{
			return FALSE;
		}
	}

	if( htiParent == NULL )
		htiParent = TVI_ROOT;
	if( hti == NULL )
		hti = TVI_LAST;

	CTreeNodeDat *lpNodeDat = new CTreeNodeDat;

	lpNodeDat->m_nImage			= iImage;
	lpNodeDat->m_nSelectedImage	= iImage;
	lpNodeDat->m_nType			= FOLDER;
	lpNodeDat->m_pNodeDat		= pNodeDat;
	lpNodeDat->m_uState			= TVIS_EXPANDED;

	CString strFolder;
	int num = 0;
	do 
	{
		if(num>0)
			strFolder.Format(_T("新文件夹 %d"),num);
		else
			strFolder = _T("新文件夹");
		num++;
	} while(IsExistChildNode(htiParent,(LPCSTR)strFolder));

	htiChild = InsertItem(strFolder, iImage, iImage, htiParent, hti);
	SetItemData(htiChild, (DWORD)lpNodeDat);
	
	EnsureVisible(htiChild);
	EditLabel(htiChild);
	Select(htiChild, TVGN_CARET);

	SetModifiedFlag(true);

	return TRUE;
}

//------------------------------------------------------------------------
// 添加子节点
BOOL CViewTree::AddChildFolder(CObject *pNodeDat, int iImage)
{
	HTREEITEM hti, htiChild = NULL;
	hti = GetSelectedItem();
	if( hti != NULL )
	{
		CTreeNodeDat *lpData = (CTreeNodeDat *)GetItemData(hti);
		if( lpData != NULL )
		{
			switch(lpData->m_nType)
			{			
			case FOLDER:
				break;	
			default:
				return FALSE;
			}
		}
	}

	CTreeNodeDat *lpNodeDat = new CTreeNodeDat;

	lpNodeDat->m_nImage			= iImage;
	lpNodeDat->m_nSelectedImage	= iImage;
	lpNodeDat->m_nType			= FOLDER;
	lpNodeDat->m_pNodeDat		= pNodeDat;
	lpNodeDat->m_uState			= TVIS_EXPANDED;
	
	CString strFolder;
	int num = 0;
	do 
	{
		if(num>0)
			strFolder.Format(_T("新文件夹 %d"),num);
		else
			strFolder = _T("新文件夹");
		num++;
	} while(IsExistChildNode(hti,strFolder));

	htiChild = InsertItem(strFolder, iImage, iImage, hti, TVI_LAST);
	SetItemData(htiChild, (DWORD)lpNodeDat);
	EnsureVisible(htiChild);
	EditLabel(htiChild);
	Select(htiChild, TVGN_CARET);

	SetModifiedFlag(true);

	return TRUE;
}

//------------------------------------------------------------------------
// 重命名
void CViewTree::OnRename() 
{
	HTREEITEM hti;
	hti = GetSelectedItem();
	if( hti != NULL )
	{	
		CTreeNodeDat *lpData = (CTreeNodeDat *)GetItemData(hti);
		switch(m_eTreeType)
		{
		case fileTree:
			{
				switch(lpData->m_nType)
				{
				case FOLDER:
				case FILE_DAT:
				case FILE_PICTRUE:
				case FILE_POINT:
				case FILE_LINE:
				case FILE_POLYGON:
				case FILE_PLANE:
				case FILE_NORMAL:
				case FILE_LAYER:
				case FILE_LAYER_MODEL:
					EditLabel(hti);
					break;
				default:
					break;
				}
			}
			break;
		case modelTree:
			{
				switch(lpData->m_nType)
				{
				case FOLDER:
				case GRID_DAT:
				case FARM_DAT:
				case GRID_LAYER:
					EditLabel(hti);
					break;
				default:
					break;
				}
			}
			break;
		case graphTree:
			{
				switch(lpData->m_nType)
				{
				case FILE_2D:
				case FILE_3D:
				case FILE_LOG:
					EditLabel(hti);
					break;
				default:
					break;
				}
			}
			break;
		default:
			break;
		}
	}
}

void CViewTree::OnUpdateRename(CCmdUI *pCmdUI)
{
	HTREEITEM hItem = GetSelectedItem();
	if( hItem != NULL )
	{
		CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)GetItemData(hItem);

		switch(m_eTreeType)
		{
		case fileTree:
			{
				switch(lpNodeDat->m_nType)
				{
				case WELL_INFO:
				case WELL_HOLE:
				case WELL_LOGING_INFO:
				case WELL_LOGINGS:
				case WELL_LOGING:
				case WELL_PRO:
				case WELL_INJ:
				case FILE_VERT_MODELS:
				case FILE_VERT_MODEL:
					pCmdUI->Enable(FALSE);
					return;
				}
			}
			break;
		case graphTree:
			{
				switch(lpNodeDat->m_nType)
				{
				case GRAPHICS_2DS:
				case GRAPHICS_3DS:
				case GRAPHICS_LOGS:
					pCmdUI->Enable(FALSE);
					return;
				}
			}
			break;
		case modelTree:
			{
				switch(lpNodeDat->m_nType)
				{
				case FARM_LAYER:
					pCmdUI->Enable(FALSE);
					return;
				}
			}
			break;
		}

		pCmdUI->Enable(TRUE);
	}
	else // 如果是根节点
		pCmdUI->Enable(FALSE);
}


void CViewTree::OnRefresh()
{
	HTREEITEM htiParent;
	// 要修改
	htiParent = GetSelectedItem();
	if (htiParent != NULL)
	{
		CTreeNodeDat *lpData = (CTreeNodeDat *)GetItemData(htiParent);

		if( lpData == NULL )
			return;

		switch(lpData->m_nType)
		{
		case FILE_DAT:
			{
				CString strFileName;

// 				strFileName = m_strDirectory;		
// 				strFileName += lpData->m_strFileName;
// 
// 				CSplitPath path(strFileName);
// 				if( !path.IsExist() )
// 				{
// 					DeleteItemM(htiParent);
// 				}
			}
			break;
		case FOLDER:
			break;
		default:
			break;
		}

	}
}

void CViewTree::OnDeleteItem() 
{	
	HTREEITEM hti;

	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();

	hti = GetSelectedItem();
	if (hti != NULL)
	{	
		CTreeNodeDat *lpNodeDat = (CTreeNodeDat*)GetItemData(hti);
		//pMF->Get3DBar()->DeleteItemByGUID(lpNodeDat->m_strGUIDName);
		switch(m_eTreeType)
		{
		case fileTree:
			{
				switch(lpNodeDat->m_nType)
				{
				case WELL_INFO:
				case WELL_HOLE:
				case WELL_LOGING_INFO:
				case WELL_LOGINGS:
				case WELL_LOGING:
				case WELL_PRO:
				case WELL_INJ:
				case FILE_VERT_MODELS:
				case FILE_VERT_MODEL:
					return;
				}
			}
			break;
		case graphTree:
			{
				switch(lpNodeDat->m_nType)
				{
				case GRAPHICS_2DS:
				case GRAPHICS_3DS:
				case GRAPHICS_LOGS:
					return;
				}
			}
			break;
		case modelTree:
			{
				switch(lpNodeDat->m_nType)
				{
				case FARM_LAYER:
					return;
				}
			}
			break;
		}
		

		if( ::MessageBox(m_hWnd,_T("确定要删除吗？"),_T("提示信息"),MB_YESNO|MB_ICONQUESTION) == IDYES )
		{
			
			FreeAllChildNodeData(hti);

			if( lpNodeDat->m_nType == WELL_DAT ) // 如果删除单井数据，由删除
			{
				
				CString strFileName = pMF->GetProjectDatPath();
				strFileName += _T("\\files\\");

				CString str = strFileName;
				str += _T("WellInfo.dat");
				DeleteFile(str);

				str = strFileName;
				str += _T("WellPathInfo.dat");
				DeleteFile(str);

				str = strFileName;
				str += _T("WellLoggingInfo.dat");
				DeleteFile(str);
			}

			CMultiDocTemplate *pTemplate = NULL;

			CString strTargePathName = pMF->GetProjectDatPath();

			switch(m_eTreeType)
			{
			case fileTree:
				{	
					switch(lpNodeDat->m_nType)
					{
					case FILE_PICTRUE:
						{
							pTemplate = theApp.m_pPictrueDocTempl;
							strTargePathName += _T("\\files\\");
						}
						break;
					case FILE_POINT:
						{
							pTemplate = theApp.m_pFileDatDocTemplate;
							strTargePathName += _T("\\files\\");
						}
						break;
					case FILE_LINE:
						{
							pTemplate = theApp.m_pFileDatDocTemplate;
							strTargePathName += _T("\\files\\");
						}
						break;
					case FILE_POLYGON:
						{
							pTemplate = theApp.m_pFileDatDocTemplate;
							strTargePathName += _T("\\files\\");
						}
						break;
					case FILE_PLANE: 
						{
							pTemplate = theApp.m_pFileDatDocTemplate;
							strTargePathName += _T("\\files\\");
						}
						break;
					case FILE_LAYER_MODEL: 
						{
							pTemplate = theApp.m_pLayerDocTemplate;
							strTargePathName += _T("\\files\\");
						}
						break;
					default:
						break;
					}			

					if( pTemplate != NULL )
					{
						strTargePathName += ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;

						CDocTemplate::Confidence match;

						CDocument* pDoc = NULL;
						match = pTemplate->MatchDocType(strTargePathName, pDoc);

						if (match == CDocTemplate::yesAlreadyOpen) // 已经有文档打开
						{
							CView* pView;
							POSITION pos = pDoc->GetFirstViewPosition ();
							while (pos != NULL)
							{
								pView = pDoc->GetNextView (pos);
								pView->GetParentFrame ()->ActivateFrame ();
							}

							pDoc->OnCloseDocument();
						}
					}
				}
				break;
			case modelTree:
				{	
					switch(lpNodeDat->m_nType)
					{
					case FARM_DAT:
						{
							pTemplate = theApp.m_p2DModelDocTemplate;
							strTargePathName += _T("\\models\\");
						}
						break;
					default:
						break;
					}			

					if( pTemplate != NULL )
					{
						strTargePathName += ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;

						CDocTemplate::Confidence match;

						CDocument* pDoc = NULL;
						match = pTemplate->MatchDocType(strTargePathName, pDoc);

						if (match == CDocTemplate::yesAlreadyOpen) // 已经有文档打开
						{
							CView* pView;
							POSITION pos = pDoc->GetFirstViewPosition ();
							while (pos != NULL)
							{
								pView = pDoc->GetNextView (pos);
								pView->GetParentFrame ()->ActivateFrame ();
							}

							pDoc->OnCloseDocument();
						}
					}
				}
				break;
			case graphTree:
				{
					switch(lpNodeDat->m_nType)
					{
					case FILE_2D:
						{
							pTemplate = theApp.m_pGeoMapDocTemplate;
							strTargePathName += _T("\\graphics\\");
						}
						break;
					case FILE_3D:
						{
							pTemplate = theApp.m_p3DModelDocTemplate;
							strTargePathName += _T("\\graphics\\");
						}
						break;
					case FILE_LOG:
						{
							pTemplate = theApp.m_pLoggingDocTemplate;
							strTargePathName += _T("\\graphics\\");
						}
						break;
					default:
						break;
					}

					if( pTemplate != NULL )
					{
						strTargePathName += ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;

						CDocTemplate::Confidence match;

						CDocument* pDoc = NULL;
						match = pTemplate->MatchDocType(strTargePathName, pDoc);

						if (match == CDocTemplate::yesAlreadyOpen) // 已经有文档打开
						{
							CView* pView;
							POSITION pos = pDoc->GetFirstViewPosition ();
							while (pos != NULL)
							{
								pView = pDoc->GetNextView (pos);
								pView->GetParentFrame ()->ActivateFrame ();
							}

							pDoc->OnCloseDocument();
						}
					}
				}
				break;
			default:
				break;
			}
			//CMainFrame *pMF = (CMainFrame *)AfxGetMainWnd();
			
			if( lpNodeDat != NULL )
			{
				delete lpNodeDat;
				lpNodeDat = NULL;
			}

			BOOL bFlag = DeleteItem(hti);
			if (bFlag)
				SetModifiedFlag();
		}

		SetFocus();

		m_hSelectItem = GetSelectedItem();
	}
}

//------------------------------------------------------------------------
// 
int CViewTree::GetIcon(LPCSTR lpszPath, UINT uFlags)
{
	SHFILEINFO    sfi;

	if( SHGetFileInfo(lpszPath, 0, &sfi, sizeof(SHFILEINFO), uFlags) )
		return sfi.iIcon;
	else
		return -1;
}

int CViewTree::GetIcons()
{

	TCHAR szWinDir [MAX_PATH + 1];
	if (GetWindowsDirectory (szWinDir, MAX_PATH) == 0)
	{
		return 0;
	}

	SHFILEINFO sfi;
	HIMAGELIST hImageList = (HIMAGELIST) SHGetFileInfo (szWinDir,
		0,
		&sfi, 
		sizeof (SHFILEINFO), SHGFI_SYSICONINDEX);
	return sfi.iIcon;
}

void CViewTree::GetNormalAndSelectedIcons(LPCSTR lpszPath, int &iImage, int &iSelectedImage)
{
	iImage = GetIcon (lpszPath, 
		SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

	iSelectedImage = GetIcon (lpszPath, 
		SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_OPENICON);

	return;
}

//________________________________________________________________________
// 删除节点
void CViewTree::OnEditClear()
{
	// TODO: 在此添加命令处理程序代码
	OnDeleteItem();
}

void CViewTree::OnUpdateEditClear(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	HTREEITEM hItem = GetSelectedItem();
	if( hItem != NULL )
	{	
		CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)GetItemData(hItem);

		switch(m_eTreeType)
		{
		case fileTree:
			{
				switch(lpNodeDat->m_nType)
				{
				case WELL_INFO:
				case WELL_HOLE:
				case WELL_LOGING_INFO:
				case WELL_LOGINGS:
				case WELL_LOGING:
				case WELL_PRO:
				case WELL_INJ:
				case FILE_VERT_MODELS:
				case FILE_VERT_MODEL:
					pCmdUI->Enable(FALSE);
					return;
				}
			}
			break;
		case graphTree:
			{
				switch(lpNodeDat->m_nType)
				{
				case GRAPHICS_2DS:
				case GRAPHICS_3DS:
				case GRAPHICS_LOGS:
					pCmdUI->Enable(FALSE);
					return;
				}
			}
			break;
		case modelTree:
			{
				switch(lpNodeDat->m_nType)
				{
				case FARM_LAYER:
					pCmdUI->Enable(FALSE);
					return;
				}
			}
			break;
		}
		pCmdUI->Enable(TRUE);
	}
	else // 如果是根节点
		pCmdUI->Enable(FALSE);
}

// 复制节点

void CViewTree::OnEditCopy()
{
	// TODO: 在此添加命令处理程序代码
	HTREEITEM hItem = GetSelectedItem();
	if( hItem == NULL )
		return;
	CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)GetItemData(hItem);

	switch(m_eTreeType)
	{
	case fileTree:
		{
			switch(lpNodeDat->m_nType)
			{
			case WELL_INFO:
			case WELL_HOLE:
			case WELL_LOGING_INFO:
			case WELL_LOGINGS:
			case WELL_LOGING:
			case WELL_PRO:
			case WELL_INJ:
			case FILE_VERT_MODELS:
			case FILE_VERT_MODEL:
				return;
			}
		}
		break;
	case graphTree:
		{
			switch(lpNodeDat->m_nType)
			{
			case GRAPHICS_2DS:
			case GRAPHICS_3DS:
			case GRAPHICS_LOGS:
				return;
			}
		}
		break;
	case modelTree:
		{
			switch(lpNodeDat->m_nType)
			{
			case FARM_LAYER:
				return;
			}
		}
		break;
	}

	ASSERT(m_cfTree != NULL);
	
	//////////////////////////////////////////////////////////////////////////
	// 保存到内存文件中
	CSharedFile file;
	CArchive ar(&file, CArchive::store);
	UINT nCount = GetCount();
	ar << nCount;				// 节点总数

	if (nCount > 0)
	{
		CString strTxt = GetItemText(hItem);
		CTreeNodeDat *pNote = (CTreeNodeDat *)GetItemData(hItem);

		ar << (BYTE) recordRegular;			// 树叶类别
		ar << strTxt;						// 保存树叶名称
		ar << (*pNote);						// 保存树叶数据

		if (ItemHasChildren(hItem))
		{
			ar << (BYTE) recordPush;
			RecursiveWriteItems(ar, GetChildItem(hItem));
		}

		ar << (BYTE) recordPop;
	}
	ar.Close();

	COleDataSource* pDataSource = NULL;
	TRY
	{
		pDataSource = new COleDataSource;
		pDataSource->CacheGlobalData(m_cfTree, file.Detach());
		pDataSource->SetClipboard();
	}
	CATCH_ALL(e)
	{
		delete pDataSource;
		THROW_LAST();
		return;
	}
	END_CATCH_ALL
}

void CViewTree::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	HTREEITEM hItem = GetSelectedItem();
	if( hItem != NULL )
	{
		CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)GetItemData(hItem);

		switch(m_eTreeType)
		{
		case fileTree:
			{
				switch(lpNodeDat->m_nType)
				{
				case WELL_INFO:
				case WELL_HOLE:
				case WELL_LOGING_INFO:
				case WELL_LOGINGS:
				case WELL_LOGING:
				case WELL_PRO:
				case WELL_INJ:
				case FILE_VERT_MODELS:
				case FILE_VERT_MODEL:
					pCmdUI->Enable(FALSE);
					return;
				}
			}
			break;
		case graphTree:
			{
				switch(lpNodeDat->m_nType)
				{
				case GRAPHICS_2DS:
				case GRAPHICS_3DS:
				case GRAPHICS_LOGS:
					pCmdUI->Enable(FALSE);
					return;
				}
			}
			break;
		case modelTree:
			{
				switch(lpNodeDat->m_nType)
				{
				case FARM_LAYER:
					pCmdUI->Enable(FALSE);
					return;
				}
			}
			break;
		}

		pCmdUI->Enable(TRUE);
	}
	else // 如果是根节点
		pCmdUI->Enable(FALSE);
}

void CViewTree::OnEditCut()
{
	// TODO: 在此添加命令处理程序代码

	HTREEITEM hItem = GetSelectedItem();
	if( hItem == NULL )
		return;
	CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)GetItemData(hItem);

	switch(m_eTreeType)
	{
	case fileTree:
		{
			switch(lpNodeDat->m_nType)
			{
			case WELL_INFO:
			case WELL_HOLE:
			case WELL_LOGING_INFO:
			case WELL_LOGINGS:
			case WELL_LOGING:
			case WELL_PRO:
			case WELL_INJ:
			case FILE_VERT_MODELS:
			case FILE_VERT_MODEL:
				return;
			}
		}
		break;
	case graphTree:
		{
			switch(lpNodeDat->m_nType)
			{
			case GRAPHICS_2DS:
			case GRAPHICS_3DS:
			case GRAPHICS_LOGS:
				return;
			}
		}
		break;
	case modelTree:
		{
			switch(lpNodeDat->m_nType)
			{
			case FARM_LAYER:
				return;
			}
		}
		break;
	}

	ASSERT(m_cfTree != NULL);

	//////////////////////////////////////////////////////////////////////////
	// 保存到内存文件中
	CSharedFile file;
	CArchive ar(&file, CArchive::store);
	UINT nCount = GetCount();
	ar << nCount;				// 节点总数

	if (nCount > 0)
	{
		CString strTxt = GetItemText(hItem);
		CTreeNodeDat *pNote = (CTreeNodeDat *)GetItemData(hItem);

		ar << (BYTE) recordRegular;			// 树叶类别
		ar << strTxt;						// 保存树叶名称
		ar << (*pNote);						// 保存树叶数据

		if (ItemHasChildren(hItem))
		{
			ar << (BYTE) recordPush;
			RecursiveWriteItems(ar, GetChildItem(hItem));
		}

		ar << (BYTE) recordPop;
	}
	ar.Close();

	COleDataSource* pDataSource = NULL;
	TRY
	{
		pDataSource = new COleDataSource;
		pDataSource->CacheGlobalData(m_cfTree, file.Detach());
		pDataSource->SetClipboard();
	}
	CATCH_ALL(e)
	{
		delete pDataSource;
		THROW_LAST();
		return;
	}
	END_CATCH_ALL

	OnDeleteItem();
}

void CViewTree::OnUpdateEditCut(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	HTREEITEM hItem = GetSelectedItem();
	if( hItem != NULL )
	{
		CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)GetItemData(hItem);

		switch(m_eTreeType)
		{
		case fileTree:
			{
				switch(lpNodeDat->m_nType)
				{
				case WELL_INFO:
				case WELL_HOLE:
				case WELL_LOGING_INFO:
				case WELL_LOGINGS:
				case WELL_LOGING:
				case WELL_PRO:
				case WELL_INJ:
				case FILE_VERT_MODELS:
				case FILE_VERT_MODEL:
					pCmdUI->Enable(FALSE);
					return;
				}
			}
			break;
		case graphTree:
			{
				switch(lpNodeDat->m_nType)
				{
				case GRAPHICS_2DS:
				case GRAPHICS_3DS:
				case GRAPHICS_LOGS:
					pCmdUI->Enable(FALSE);
					return;
				}
			}
			break;
		case modelTree:
			{
				switch(lpNodeDat->m_nType)
				{
				case FARM_LAYER:
					pCmdUI->Enable(FALSE);
					return;
				}
			}
			break;
		}
		pCmdUI->Enable(TRUE);
	}
	else // 如果是根节点
		pCmdUI->Enable(FALSE);
}

void CViewTree::OnEditPaste()
{
	// TODO: 在此添加命令处理程序代码

	HTREEITEM hItem = GetSelectedItem();
	if( hItem != NULL )
	{
		CTreeNodeDat *pNodeDat = (CTreeNodeDat *)GetItemData(hItem);

		if( pNodeDat->m_nType != FOLDER )
			return;
	}
	else
		hItem = TVI_ROOT;

	COleDataObject dataObject;
	BOOL bEnable = dataObject.AttachClipboard() &&
		(dataObject.IsDataAvailable(m_cfTree) || COleClientItem::CanCreateFromData(&dataObject));

	if( !bEnable )
		return;

	CFile* pFile = dataObject.GetFileData(m_cfTree);
	if(pFile == NULL)
		return;

	CArchive ar(pFile, CArchive::load);
	TRY
	{
		ReadTreeViewContent(ar, hItem);
	}
	CATCH_ALL(e)
	{
		ar.Close();
		delete pFile;
		THROW_LAST();
	}
	END_CATCH_ALL
	
	ar.Close();

	delete pFile;

	Expand(hItem, TVE_EXPAND);
}

void CViewTree::OnUpdateEditPaste(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码

	HTREEITEM hItem = GetSelectedItem();
	if( hItem != NULL )
	{
		CTreeNodeDat *pNodeDat = (CTreeNodeDat *)GetItemData(hItem);

		if( pNodeDat->m_nType == FOLDER )
		{
			COleDataObject dataObject;
			BOOL bEnable = dataObject.AttachClipboard() &&
				(dataObject.IsDataAvailable(m_cfTree) || COleClientItem::CanCreateFromData(&dataObject));
			
			pCmdUI->Enable(bEnable);
		}
		else
			pCmdUI->Enable(FALSE);
	}
	else // 如果是根节点
		pCmdUI->Enable(TRUE);
}

BOOL CViewTree::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (m_hCursor == NULL)
		return CTreeCtrl::OnSetCursor(pWnd, nHitTest, message);
	SelectDropCursor();
	SetCursor(m_hCursor);
	return 0;
}

// Return true if the second parameter is an ancester of the first one.
// 如果第二个是第一个的祖先则返回TRUE
BOOL CViewTree::IsChildNodeOf(HTREEITEM hitemChild, HTREEITEM hitemSuspectedParent)
{
	do
	{
		if (hitemChild == hitemSuspectedParent)
			break;
	}
	while ((hitemChild = GetParentItem(hitemChild)) != NULL);

	return (hitemChild != NULL);
}

void CViewTree::SaveTreeData(CSharedFile *pFile,HTREEITEM hItem)
{
	CArchive ar(pFile, CArchive::store);
	TRY
	{
		UINT nCount = GetCount();
		ar << nCount;				// 节点总数

		if (nCount > 0)
		{
			CString strTxt = GetItemText(hItem);
			CTreeNodeDat *pNote = (CTreeNodeDat *)GetItemData(hItem);

			ar << (BYTE) recordRegular;			// 树叶类别
			ar << strTxt;						// 保存树叶名称
			ar << (*pNote);						// 保存树叶数据

			if (ItemHasChildren(hItem))
			{
				ar << (BYTE) recordPush;
				RecursiveWriteItems(ar, GetChildItem(hItem));
			}

			ar << (BYTE) recordPop;
		}
		ar.Close();
	}
	CATCH_ALL(e)
	{
		ar.Close();
		THROW_LAST();
	}
	END_CATCH_ALL
}

HTREEITEM CViewTree::LoadTreeData(HGLOBAL hMem, HTREEITEM hItem)
{
	CSharedFile file;
	file.SetHandle(hMem);
	HTREEITEM hItemRet = NULL;
	CArchive ar(&file, CArchive::load);
	TRY
	{
		hItemRet = ReadTreeViewContent(ar, hItem);
	}
	CATCH_ALL(e)
	{
		ar.Close();		
		THROW_LAST();
		return NULL;
	}
	END_CATCH_ALL

	ar.Close();

	return hItemRet;
}

HTREEITEM CViewTree::GetItemByGUID( CString guid )
{
	HTREEITEM root = GetRootItem();
	if(!root)
		return NULL;
	return SearchItemByGUID(guid, root);

	//CList <HTREEITEM,HTREEITEM&>   TreeList; 
	//HTREEITEM   hItemhi   =   GetRootItem();
	//if   (hItemhi   !=   NULL)
	//{
	//	TreeList.AddHead(hItemhi);
	//	while(!TreeList.IsEmpty())
	//	{
	//		hItemhi   =   TreeList.RemoveHead();
	//		CGLObject*   pObj   =   (CGLObject*)GetItemData(hItemhi);
	//		if(pObj)
	//		{
	//			CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)GetItemData(hItemhi);
	//			if(lpNodeDat)
	//			{
	//				if(lpNodeDat->m_strGUIDName==guid)
	//				{
	//					return hItemhi;
	//				}
	//			}
	//		}
	//			//if   (guid   ==   pObj->m_strGUID)   //ItemName是你想找的字符
	//			//	return hItemhi;
	//		hItemhi   =    GetChildItem(hItemhi);
	//		while(hItemhi)
	//		{
	//			TreeList.AddHead(hItemhi);
	//			hItemhi   =   GetNextSiblingItem(hItemhi);
	//		}
	//	}
	//}
	//return NULL;
}

HTREEITEM CViewTree::SearchItemByGUID( CString guid, HTREEITEM parent )
{
	HTREEITEM point = parent;
	CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)GetItemData(point);
	if(lpNodeDat)
	{
		if(lpNodeDat->m_strGUIDName==guid)
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

void CViewTree::DeleteGUIDItem( CString guid )
{
	HTREEITEM hti = GetItemByGUID(guid);

	if (hti != NULL)
	{	
		CTreeNodeDat *lpNodeDat = (CTreeNodeDat*)GetItemData(hti);
		switch(m_eTreeType)
		{
		case fileTree:
			{
				switch(lpNodeDat->m_nType)
				{
				case WELL_INFO:
				case WELL_HOLE:
				case WELL_LOGING_INFO:
				case WELL_LOGINGS:
				case WELL_LOGING:
				case WELL_PRO:
				case WELL_INJ:
				case FILE_VERT_MODELS:
				case FILE_VERT_MODEL:
					return;
				}
			}
			break;
		case graphTree:
			{
				switch(lpNodeDat->m_nType)
				{
				case GRAPHICS_2DS:
				case GRAPHICS_3DS:
				case GRAPHICS_LOGS:
					return;
				}
			}
			break;
		case modelTree:
			{
				switch(lpNodeDat->m_nType)
				{
				case FARM_LAYER:
					return;
				}
			}
			break;
		}


		//if( ::MessageBox(m_hWnd,_T("确定要删除吗？"),_T("提示信息"),MB_YESNO|MB_ICONQUESTION) == IDYES )
		{
			FreeAllChildNodeData(hti);

			if( lpNodeDat->m_nType == WELL_DAT ) // 如果删除单井数据，由删除
			{
				CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
				CString strFileName = pMF->GetProjectDatPath();
				strFileName += _T("\\files\\");

				CString str = strFileName;
				str += _T("WellInfo.dat");
				DeleteFile(str);

				str = strFileName;
				str += _T("WellPathInfo.dat");
				DeleteFile(str);

				str = strFileName;
				str += _T("WellLoggingInfo.dat");
				DeleteFile(str);
			}

			CMultiDocTemplate *pTemplate = NULL;
			CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
			CString strTargePathName = pMF->GetProjectDatPath();

			switch(m_eTreeType)
			{
			case fileTree:
				{	
					switch(lpNodeDat->m_nType)
					{
					case FILE_PICTRUE:
						{
							pTemplate = theApp.m_pPictrueDocTempl;
							strTargePathName += _T("\\files\\");
						}
						break;
					case FILE_POINT:
						{
							pTemplate = theApp.m_pFileDatDocTemplate;
							strTargePathName += _T("\\files\\");
						}
						break;
					case FILE_LINE:
						{
							pTemplate = theApp.m_pFileDatDocTemplate;
							strTargePathName += _T("\\files\\");
						}
						break;
					case FILE_POLYGON:
						{
							pTemplate = theApp.m_pFileDatDocTemplate;
							strTargePathName += _T("\\files\\");
						}
						break;
					case FILE_PLANE: 
						{
							pTemplate = theApp.m_pFileDatDocTemplate;
							strTargePathName += _T("\\files\\");
						}
						break;
					case FILE_LAYER_MODEL: 
						{
							pTemplate = theApp.m_pLayerDocTemplate;
							strTargePathName += _T("\\files\\");
						}
						break;
					default:
						break;
					}			

					if( pTemplate != NULL )
					{
						strTargePathName += ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;

						CDocTemplate::Confidence match;

						CDocument* pDoc = NULL;
						match = pTemplate->MatchDocType(strTargePathName, pDoc);

						if (match == CDocTemplate::yesAlreadyOpen) // 已经有文档打开
						{
							CView* pView;
							POSITION pos = pDoc->GetFirstViewPosition ();
							while (pos != NULL)
							{
								pView = pDoc->GetNextView (pos);
								pView->GetParentFrame ()->ActivateFrame ();
							}

							pDoc->OnCloseDocument();
						}
					}
				}
				break;
			case modelTree:
				{	
					switch(lpNodeDat->m_nType)
					{
					case FARM_DAT:
						{
							pTemplate = theApp.m_p2DModelDocTemplate;
							strTargePathName += _T("\\models\\");
						}
						break;
					default:
						break;
					}			

					if( pTemplate != NULL )
					{
						strTargePathName += ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;

						CDocTemplate::Confidence match;

						CDocument* pDoc = NULL;
						match = pTemplate->MatchDocType(strTargePathName, pDoc);

						if (match == CDocTemplate::yesAlreadyOpen) // 已经有文档打开
						{
							CView* pView;
							POSITION pos = pDoc->GetFirstViewPosition ();
							while (pos != NULL)
							{
								pView = pDoc->GetNextView (pos);
								pView->GetParentFrame ()->ActivateFrame ();
							}

							pDoc->OnCloseDocument();
						}
					}
				}
				break;
			case graphTree:
				{
					switch(lpNodeDat->m_nType)
					{
					case FILE_2D:
						{
							pTemplate = theApp.m_pGeoMapDocTemplate;
							strTargePathName += _T("\\graphics\\");
						}
						break;
					case FILE_3D:
						{
							pTemplate = theApp.m_p3DModelDocTemplate;
							strTargePathName += _T("\\graphics\\");
						}
						break;
					case FILE_LOG:
						{
							pTemplate = theApp.m_pLoggingDocTemplate;
							strTargePathName += _T("\\graphics\\");
						}
						break;
					default:
						break;
					}

					if( pTemplate != NULL )
					{
						strTargePathName += ((CFileViewObj*)lpNodeDat->m_pNodeDat)->m_strFileName;

						CDocTemplate::Confidence match;

						CDocument* pDoc = NULL;
						match = pTemplate->MatchDocType(strTargePathName, pDoc);

						if (match == CDocTemplate::yesAlreadyOpen) // 已经有文档打开
						{
							CView* pView;
							POSITION pos = pDoc->GetFirstViewPosition ();
							while (pos != NULL)
							{
								pView = pDoc->GetNextView (pos);
								pView->GetParentFrame ()->ActivateFrame ();
							}

							pDoc->OnCloseDocument();
						}
					}
				}
				break;
			default:
				break;
			}

			if( lpNodeDat != NULL )
			{
				delete lpNodeDat;
				lpNodeDat = NULL;
			}

			BOOL bFlag = DeleteItem(hti);
			if (bFlag)
				SetModifiedFlag();
		}

		SetFocus();

		m_hSelectItem = GetSelectedItem();
	}
}
