// GeoMapDoc.cpp : 实现文件
//

#include "stdafx.h"
#include "GeoMapDoc.h"
#include "../Resource.h"
#include "GeoPolyObj.h"
#include "GeoMapView.h"
#include "../MainFrm.h"
#include "../Command/CmdActiveLayer.h"
#include "../Command/CmdMoveLayerBack.h"
#include "../Command/CmdMoveLayerForward.h"
#include "GeoPictrueObj.h"
#include "CmdGeoDrawObj.h"
#include "GeoWellObj.h"
#include "../CoordTrans.h"
#include "GeoPointObj.h"
#include "GeoPolyObj.h"
#include "GeoLineObj.h"

/////////////////////////////////////////////////////////////////////////////
// CGeoMapDoc

extern void StatusProgress(int nIndex, int nRange=-1);
extern void StatusSetProgress(int nIndex, int nPos);

IMPLEMENT_DYNCREATE(CGeoMapDoc, COleDocument)

CGeoMapDoc::CGeoMapDoc()
{
	EnableCompoundFile();
	m_pCurDrawPage = NULL;		// 当前页
	m_pCurDrawLayer = NULL;		// 当前图层
	GetPrinterPageSize(m_sizeDoc);
}

CGeoMapDoc::~CGeoMapDoc()
{
	ResetHistory();
	if( m_pCurDrawPage != NULL )
	{
		delete m_pCurDrawPage;
		m_pCurDrawPage = NULL;
	}
}


void CGeoMapDoc::GetPrinterPageSize(CSize &pageSize)
{
	CPrintInfo PrintInfo;
	pageSize = CSize(2100, 2960);		// default to A4
	if (AfxGetApp()->GetPrinterDeviceDefaults(&PrintInfo.m_pPD->m_pd))
	{
		if (PrintInfo.m_pPD->CreatePrinterDC() != NULL)
		{
			if (PrintInfo.m_pPD->m_pd.hDC == NULL)
				PrintInfo.m_pPD->CreatePrinterDC();

			// attach printer dc and retreive docSize
			CDC dcPrint;
			dcPrint.Attach(PrintInfo.m_pPD->m_pd.hDC);
			pageSize.cx = 10 * dcPrint.GetDeviceCaps(HORZSIZE);
			pageSize.cy = 10 * dcPrint.GetDeviceCaps(VERTSIZE);
		}
	}
}

CSize CGeoMapDoc::GetPageSize()
{
	return m_sizeDoc;
}

BOOL CGeoMapDoc::OnNewDocument()
{
	if (!COleDocument::OnNewDocument())
		return FALSE;

	m_pCurDrawPage = new CDrawPage(this);
	m_strCurPageID = m_pCurDrawPage->m_strID;
	m_pCurDrawLayer = new CDrawLayer(this);
	m_pCurDrawLayer->m_strLayerName = "图层 1";
	m_pCurDrawPage->Add(m_pCurDrawLayer);
	
	return TRUE;
}

BEGIN_MESSAGE_MAP(CGeoMapDoc, COleDocument)
	//{{AFX_MSG_MAP(CGeoMapDoc)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_DRAW_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_UPDATE_COMMAND_UI(ID_OLE_VERB_POPUP, OnUpdateOleVerbPopup)
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, COleDocument::OnUpdatePasteMenu)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_LINK, COleDocument::OnUpdatePasteLinkMenu)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_LINKS, COleDocument::OnUpdateEditLinksMenu)
	ON_COMMAND(ID_OLE_EDIT_LINKS, COleDocument::OnEditLinks)
	ON_UPDATE_COMMAND_UI(ID_OLE_VERB_FIRST, COleDocument::OnUpdateObjectVerbMenu)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGeoMapDoc diagnostics

#ifdef _DEBUG
void CGeoMapDoc::AssertValid() const
{
	COleDocument::AssertValid();
}

void CGeoMapDoc::Dump(CDumpContext& dc) const
{
	COleDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGeoMapDoc serialization

void CGeoMapDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		m_pCurDrawPage->Serialize(ar);
		ar << m_strCurPageID;		
	}
	else
	{
		if( m_pCurDrawPage )
		{
			delete m_pCurDrawPage;
			m_pCurDrawPage = NULL;
		}

		m_pCurDrawPage = new CDrawPage(this);
		
		m_pCurDrawPage->Serialize(ar);
		ar >> m_strCurPageID;

		m_pCurDrawLayer = m_pCurDrawPage->m_pCurDrawLayer;

		this->UpdateAllViews(NULL);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGeoMapDoc commands

//功能:加图层
//参数:
//返回:
BOOL CGeoMapDoc::Add(CDrawLayer *pObj)
{
	POSITION pos = GetFirstViewPosition();
	while (pos!=NULL)
	{
		CView *pView = GetNextView(pos);
		COleClientItem* pActiveItem = GetInPlaceActiveItem(pView);
		if (pActiveItem != NULL)
		{
			// if we found one, deactivate it
			pActiveItem->Close();
		}

		ASSERT(GetInPlaceActiveItem(pView) == NULL);
	}

	ReleaseCapture();

	BOOL bRet = m_pCurDrawPage->Add(pObj);
	if( bRet )
	{
		m_pCurDrawLayer = m_pCurDrawPage->GetCurLayer();
		UpdateAllViews(NULL, HINT_UPDATE_LAYERBAR, m_pCurDrawLayer);
		SetModifiedFlag();
	}

	return bRet;
}

//功能:删除图层
//参数:
//返回:
BOOL CGeoMapDoc::Remove(CDrawLayer *pObj)
{
	TRY
	{		
		POSITION pos = GetFirstViewPosition();
		while (pos!=NULL)
		{
			CView *pView = GetNextView(pos);
			COleClientItem* pActiveItem = GetInPlaceActiveItem(pView);
			if (pActiveItem != NULL)
			{
				// if we found one, deactivate it
				pActiveItem->Close();
			}

			ASSERT(GetInPlaceActiveItem(pView) == NULL);
		}

		ReleaseCapture();
		if( m_pCurDrawPage->Remove(pObj) )
		{
			m_pCurDrawLayer = m_pCurDrawPage->GetCurLayer();
			UpdateAllViews(NULL, HINT_UPDATE_LAYERBAR, m_pCurDrawLayer);
			SetModifiedFlag();
		}
	}
	CATCH_ALL(e)
	{
		e->Delete();
		return FALSE;
	}
	END_CATCH_ALL

		return TRUE;	
}

//功能:加图素
//参数:
//返回:
BOOL CGeoMapDoc::Add(CGeoDrawObj *pObj)
{	
	if( pObj->m_pParentLayer )
		return pObj->m_pParentLayer->Add(pObj);
	else
	{
		pObj->m_pParentLayer = m_pCurDrawLayer;
		return m_pCurDrawLayer->Add(pObj);
	}
}

//功能:删除图素
//参数:
//返回:
BOOL CGeoMapDoc::Remove(CGeoDrawObj *pObj)
{
	if( pObj->m_pParentLayer )
		return pObj->m_pParentLayer->Remove(pObj);
	else
		return m_pCurDrawLayer->Remove(pObj);
}

//功能:撤消删除选中图素列表
//参数:选中图素列表
//返回:
BOOL CGeoMapDoc::Add(const CGeoDrawObjList &List)
{
	DWORD dwCount = 0;	

	TRY
	{
		CDrawLayer *pLayer;
		for (POSITION pos = List.GetHeadPosition(); pos != NULL; )
		{
			CGeoDrawObj* pObj = List.GetNext(pos);

			if(pObj->m_pParentLayer)
				pLayer = pObj->m_pParentLayer;
			else
			{
				pLayer = m_pCurDrawLayer;
				pObj->m_pParentLayer = pLayer;
				pObj->m_pParentPage = pLayer->m_pParentPage;
			}

			if (pLayer->m_DrawObjects.Find(pObj) == NULL)	// no dups
			{
				pObj->OnConvertParamChanged();				
				pLayer->Add(pObj);		
				dwCount++;
			}
		}
	}
	CATCH_ALL(e)
	{
		e->Delete();
	}
	END_CATCH_ALL

		UpdateAllViews(NULL);			

	return dwCount;		// Returns success even if only one was successfully added
}

//功能:删除所有选中的图素
//参数:所有选中图素列表
//返回:
BOOL CGeoMapDoc::Remove(const CGeoDrawObjList &List)
{
	DWORD dwCount = 0;
	TRY
	{
		for (POSITION posOut = List.GetHeadPosition(); posOut != NULL; )
		{
			CGeoDrawObj* pObj = List.GetNext(posOut);

			CDrawLayer *pLayer;

			if(pObj->m_pParentLayer)
				pLayer = pObj->m_pParentLayer;
			else
				pLayer = m_pCurDrawLayer;

			POSITION posIn = pLayer->m_DrawObjects.Find(pObj);
			if (posIn != NULL )
			{
				if(pLayer->Remove(pObj))
					dwCount++;
			}
		}
	}
	CATCH_ALL(e)
	{
		e->Delete();
	}
	END_CATCH_ALL

		UpdateAllViews(NULL);		

	return dwCount;		// Returns success even if only one was successfully removed
}



//功能:移动图素组
//参数:选中图素列表，移动距离
//返回:
BOOL CGeoMapDoc::MoveObjs(const CGeoDrawObjList &List, CPoint delta, CGeoMapView *pView)
{
	TRY
	{
		for (POSITION pos = List.GetHeadPosition(); pos != NULL; )
		{
			CGeoDrawObj* pObj = List.GetNext(pos);
			CRect rect = pObj->m_rtPosition;
			rect += delta;
			MoveObj(pObj, rect, pView);
		}
	}
	CATCH_ALL(e)
	{
		e->Delete();
	}
	END_CATCH_ALL

		UpdateAllViews(NULL);			

	return TRUE;		// Returns success even if only one was successfully added
}

//功能:移动图素组
//参数:选中图素列表，移动距离
//返回:
BOOL CGeoMapDoc::NoMoveObjs(const CGeoDrawObjList &List, CPoint delta, CGeoMapView *pView)
{
	TRY
	{
		for (POSITION posOut = List.GetHeadPosition(); posOut != NULL; )
		{
			CGeoDrawObj* pObj = List.GetNext(posOut);
			CRect rect = pObj->m_rtPosition;
			rect -= delta;
			MoveObj(pObj, rect, pView);
		}
	}
	CATCH_ALL(e)
	{
		e->Delete();
	}
	END_CATCH_ALL

		UpdateAllViews(NULL);		

	return TRUE;		// Returns success even if only one was successfully removed
}


//功能:判断点是否在图素上
//参数:该实际坐标
//返回:图素
CGeoDrawObj* CGeoMapDoc::ObjectAt(const CPoint &point, CGeoMapView *pView)
{
	POSITION pos = m_pCurDrawLayer->m_DrawObjects.GetTailPosition();
	while (pos != NULL)
	{
		CGeoDrawObj* pObj = m_pCurDrawLayer->m_DrawObjects.GetPrev(pos);
		if (pObj->HitTestHandle(point, pView,FALSE))
		{
			if( pObj->m_pSelectedObj != NULL)
				return pObj->m_pSelectedObj;
			else
				return pObj;
		}
	}

	return NULL;
}

CGeoDrawObj* CGeoMapDoc::HitOnBorder(const CPoint& point, 
									 CGeoMapView *pView,
									 CGeoDrawObj*pDrawObj)
{
	POSITION pos = m_pCurDrawLayer->m_DrawObjects.GetTailPosition();
	while (pos != NULL)
	{
		CGeoDrawObj* pObj = m_pCurDrawLayer->m_DrawObjects.GetPrev(pos);
		if (pObj->IsHitOnBorder(point, pView) && pDrawObj != pObj)
			return pObj;
	}

	return NULL;
}

//功能:显示当前页面
//参数:
//返回:
void CGeoMapDoc::Draw(CDC *pDC, CGeoMapView *pView)
{		
	if( m_pCurDrawPage )
		m_pCurDrawPage->Draw(pDC, pView);//只显示当前页	
}

//功能:在文档被删除之前调用
//参数:
//返回:
void CGeoMapDoc::DeleteContents() 
{
	ResetHistory();
	if( m_pCurDrawPage != NULL )
	{
		delete m_pCurDrawPage;
		m_pCurDrawPage = NULL;
	}

	COleDocument::DeleteContents();
}

//功能:撤消操作
//参数:
//返回:
void CGeoMapDoc::OnEditUndo() 
{
	Undo();
}

//功能:更新撤消标志
//参数:
//返回:
void CGeoMapDoc::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(CanUndo());
}

//功能:重做
//参数:
//返回:
void CGeoMapDoc::OnEditRedo() 
{
	Redo();
}

//功能:更新重做标志
//参数:
//返回:
void CGeoMapDoc::OnUpdateEditRedo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(CanRedo());
}


//功能:判断文档是否被修改
//参数:
//返回:
BOOL CGeoMapDoc::IsModified()
{
	return (IsDirty() || COleDocument::IsModified());
}

//功能:保存文档时调用
//参数:文件名
//返回:
BOOL CGeoMapDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	// TODO: Add your specialized code here and/or call the base class

	if( COleDocument::OnSaveDocument(lpszPathName) )
	{
		SetClean();		// Tells the command history that the document is "clean" at this point	
		return TRUE;
	}
	return FALSE;
}

//功能:当图素改变位置时
//参数:
//返回:
BOOL CGeoMapDoc::MoveObj(CGeoDrawObj *pObj, CRect rect, CGeoMapView *pView)
{
	pObj->Invalidate(pView);

	pObj->MoveTo(rect, pView);

	CGeoDrawObj *pRootObj = NULL;
	CGeoDrawObj *pTmpObj = pObj->m_pParentObj;

	while(pTmpObj!=NULL)
	{
		pRootObj = pTmpObj;
		pTmpObj = pRootObj->m_pParentObj;
	}

	if( pRootObj )
		pRootObj->RecalcBounds(pView);
	else
		pObj->RecalcBounds(pView);

	pObj->Invalidate(pView);

	UpdateAllViews(NULL);
	return TRUE;
}

//功能:图素移到最上一层
//参数:
//返回:
BOOL CGeoMapDoc::MoveToFront(CGeoDrawObj *pObj)
{
	CDrawLayer *pLayer;

	if( pObj->m_pParentLayer )
		pLayer = pObj->m_pParentLayer;
	else
		pLayer = m_pCurDrawLayer;

	if(pLayer->Remove(pObj))
		return pLayer->Add(pObj);		
	else
		return FALSE;

}

//功能:图素移到最底一层
//参数:
//返回:
BOOL CGeoMapDoc::MoveToBack(CGeoDrawObj *pObj)
{
	CDrawLayer *pLayer;

	if( pObj->m_pParentLayer )
		pLayer = pObj->m_pParentLayer;
	else
		pLayer = m_pCurDrawLayer;

	if(pLayer->Remove(pObj))
	{
		pLayer->m_DrawObjects.AddHead(pObj);
		return TRUE;
	}
	else
		return FALSE;
}

//功能:图素上移一层
//参数:
//返回:
BOOL CGeoMapDoc::MoveForward(CGeoDrawObj *pObj)
{
	CDrawLayer *pLayer;

	if( pObj->m_pParentLayer )
		pLayer = pObj->m_pParentLayer;
	else
		pLayer = m_pCurDrawLayer;

	POSITION pos = pLayer->m_DrawObjects.Find(pObj);
	ASSERT(pos != NULL);
	if (pos != pLayer->m_DrawObjects.GetTailPosition())
	{
		POSITION posNext = pos;
		pLayer->m_DrawObjects.GetNext(posNext);
		pLayer->Remove(pObj);
		pLayer->m_DrawObjects.InsertAfter(posNext, pObj);
		return TRUE;
	}
	return FALSE;
}

//功能:图素下移一层
//参数:
//返回:
BOOL CGeoMapDoc::MoveBack(CGeoDrawObj *pObj)
{
	CDrawLayer *pLayer;

	if( pObj->m_pParentLayer )
		pLayer = pObj->m_pParentLayer;
	else
		pLayer = m_pCurDrawLayer;

	POSITION pos = pLayer->m_DrawObjects.Find(pObj);
	ASSERT(pos != NULL);
	if (pos != pLayer->m_DrawObjects.GetHeadPosition())
	{
		POSITION posPrev = pos;
		pLayer->m_DrawObjects.GetPrev(posPrev);
		pLayer->Remove(pObj);
		pLayer->m_DrawObjects.InsertBefore(posPrev, pObj);	
		return TRUE;
	}
	return FALSE;
}


//功能:设置当前图层
//参数:
//返回:
BOOL CGeoMapDoc::SetCurLayer(CDrawLayer *pObj)
{
	if(pObj != m_pCurDrawLayer )
	{
		POSITION pos = GetFirstViewPosition();
		while (pos!=NULL)
		{
			CView *pView = GetNextView(pos);
			COleClientItem* pActiveItem = GetInPlaceActiveItem(pView);
			if (pActiveItem != NULL)
			{
				// if we found one, deactivate it
				pActiveItem->Close();
			}

			ASSERT(GetInPlaceActiveItem(pView) == NULL);
		}

		ReleaseCapture();

		m_pCurDrawLayer = pObj;
		m_pCurDrawPage->SetCurLayer(pObj);
	}	

	return TRUE;
}

//功能:获取当前页面下给定索引的图层对象
//参数:图层索引
//返回:
CDrawLayer* CGeoMapDoc::GetLayer(int nIndex)
{
	return m_pCurDrawPage->GetLayer(nIndex);
}



//功能:刷新OLE菜单项
//参数:
//返回:

void CGeoMapDoc::OnUpdateOleVerbPopup(CCmdUI* pCmdUI) 
{
	pCmdUI->m_pParentMenu = pCmdUI->m_pMenu;
	COleDocument::OnUpdateObjectVerbMenu(pCmdUI);
}

BOOL CGeoMapDoc::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (nCode == CN_COMMAND && nID == ID_OLE_VERB_POPUP)
		nID = ID_OLE_VERB_FIRST;
	return COleDocument::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CGeoMapDoc::GetUndoActions(CStringList &lstActions) const
{
	lstActions.RemoveAll();
	int n = 0;
	for (int i = m_iCommandHistoryUndoPos-1; i >= 0; i--)
	{
		if (m_commandHistory[i] && n<m_iCommandHistoryUndoCount)
		{
			CCommand *pCmd = m_commandHistory[i];
			CString str;
			pCmd->Sprint(str);
			lstActions.AddTail(str);
			n++;
		}
	}
}

void CGeoMapDoc::GetRedoActions(CStringList &lstActions) const
{
	lstActions.RemoveAll();
	int n = 0;
	for (int i = m_iCommandHistoryUndoPos; i<m_commandHistory.GetSize(); i++)	
	{
		if (m_commandHistory[i] && n<m_iCommandHistoryRedoCount)
		{
			CCommand *pCmd = m_commandHistory[i];
			CString str;
			pCmd->Sprint(str);

			lstActions.AddTail(str);
			n++;
		}
	}
}

void CGeoMapDoc::DoUndo(CUndoButton *pUndoBtn)
{

	if (pUndoBtn == NULL)
	{
		ASSERT (FALSE);
		return;
	}

	switch (pUndoBtn->m_nID) 
	{
	case ID_DRAW_EDIT_UNDO:
		{
			for (int ind = 0; ind < pUndoBtn->GetSelNum(); ind++)
			{
				Undo();
			}
		}
		break;

	case ID_DRAW_EDIT_REDO:
		{
			for (int ind = pUndoBtn->GetSelNum(); --ind >= 0;)
			{
				Redo();
			}
		}
		break;

	default:
		ASSERT(FALSE);
	}
}

BOOL CGeoMapDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{	
	if (!COleDocument::OnOpenDocument(lpszPathName))
	{
		return FALSE;
	}

	// TODO: Add your specialized creation code here

	return TRUE;
}

BOOL CGeoMapDoc::DeletePointFromPoly(CGeoDrawObj* pObj, int index, CPoint point, CGeoMapView *pView)
{
	CGeoPolyObj *pPoly  = (CGeoPolyObj *)pObj;
	pPoly->DeletePoint( index,  pView);	
	return TRUE;
}

BOOL CGeoMapDoc::InsertPointOfPoly(CGeoDrawObj* pObj, int index, CPoint point, CGeoMapView *pView)
{
	CGeoPolyObj *pPoly  = (CGeoPolyObj *)pObj;
	pPoly->InsertPoint(point, index,  pView);
	return TRUE;
}

BOOL CGeoMapDoc::MoveHandle(CGeoDrawObj *pObj, int nHandle, CPoint pt, CGeoMapView *pView)
{ 
	if(pObj->m_bLocked )
		return FALSE;

	pObj->Invalidate(pView);

	pObj->MoveHandleTo(nHandle, pt, pView);

	CGeoDrawObj *pRootObj = NULL;
	CGeoDrawObj *pTmpObj = pObj->m_pParentObj;

	while(pTmpObj!=NULL)
	{
		pRootObj = pTmpObj;
		pTmpObj = pRootObj->m_pParentObj;
	}

	if( pRootObj )
		pRootObj->RecalcBounds(pView);
	else
		pObj->RecalcBounds(pView);

	pObj->Invalidate(pView);

	UpdateAllViews(NULL);

	return TRUE;
}

BOOL CGeoMapDoc::JoinLine(CGeoPolyObj *pObj1, CGeoPolyObj *pObj2, CGeoPolyObj *pJoinObj, CGeoMapView *pView)
{
	CDrawLayer *pLayer = pObj1->GetParentLayer();
	pLayer->Remove(pObj1);

	pLayer = pObj2->GetParentLayer();
	pLayer->Remove(pObj2);

	pLayer = pJoinObj->GetParentLayer();
	pLayer->Add(pJoinObj);

	SetModifiedFlag();
	UpdateAllViews(NULL);

	return TRUE;
}

BOOL CGeoMapDoc::NoJoinLine(CGeoPolyObj *pObj1, CGeoPolyObj *pObj2, CGeoPolyObj *pJoinObj, CGeoMapView *pView)
{
	CDrawLayer *pLayer = pJoinObj->GetParentLayer();
	pLayer->Remove(pJoinObj);

	pLayer = pObj1->GetParentLayer();
	pLayer->Add(pObj1);

	pLayer = pObj2->GetParentLayer();
	pLayer->Add(pObj2);

	SetModifiedFlag();
	UpdateAllViews(NULL);

	return TRUE;
}

BOOL CGeoMapDoc::DivideLine(CGeoPolyObj *pObj1, CGeoPolyObj *pObj2, CGeoPolyObj *pDivideObj, CGeoMapView *pView)
{
	pView->m_selection.RemoveAll();

	CDrawLayer *pLayer = pDivideObj->GetParentLayer();

	if( pObj1 != NULL )
		pLayer->Add(pObj1);
	if( pObj2 != NULL ) // 中间截断
		pLayer->Add(pObj2);

	pLayer->Remove(pDivideObj);

	pView->Select(pObj1,TRUE);

	if(pObj2!=NULL)
		pView->Select(pObj2,TRUE);

	SetModifiedFlag();
	UpdateAllViews(NULL);

	return TRUE;
}

BOOL CGeoMapDoc::NoDivideLine(CGeoPolyObj *pObj1, CGeoPolyObj *pObj2, CGeoPolyObj *pDivideObj, CGeoMapView *pView)
{	
	pView->m_selection.RemoveAll();

	CDrawLayer *pLayer = pDivideObj->GetParentLayer();	
	pLayer->Remove(pObj1);

	if( pObj2 != NULL )
		pLayer->Remove(pObj2);

	pLayer->Add(pDivideObj);
	pView->Select(pDivideObj);

	SetModifiedFlag();
	UpdateAllViews(NULL);

	return TRUE;
}


void CGeoMapDoc::OnCloseDocument()
{
	// TODO: 在此添加专用代码和/或调用基类

	__super::OnCloseDocument();
}

//  图像图层
void CGeoMapDoc::CreateLayerPictrue(LPCTSTR lpFileName)
{
	POSITION pos = GetFirstViewPosition();
	CGeoMapView *pView = (CGeoMapView *)GetNextView(pos);
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CDrawLayer *pLayer = new CDrawLayer(this);
	pLayer->m_strLayerName = _T("底图");
	pLayer->m_eLayerType = CDrawLayer::basepictrue;
	Do(new CCmdAddLayer(this, pLayer, &pFrame->GetLayerToolbar()->m_wndListCtrl));

	CRect rect(-500,-500,500,500);
	CGeoPictrueObj *pObj = new CGeoPictrueObj(rect,m_pCurDrawLayer);
	pObj->SetObjName(_T("底图"));
	pObj->m_strFileName = lpFileName;
	Do(new CCmdAddDrawObj(this, pObj));
	pObj->LoadPictrue();	
	SetModifiedFlag();
	pView->Invalidate(FALSE);
}

void CGeoMapDoc::CreatePointMap(LPCTSTR lpFileName, LPCTSTR lpLayerName)
{
	POSITION pos = GetFirstViewPosition();
	CGeoMapView *pView = (CGeoMapView *)GetNextView(pos);
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CDrawLayer *pLayer = new CDrawLayer(this);
	pLayer->m_strLayerName = lpLayerName;
	pLayer->m_eLayerType = CDrawLayer::generic;
	Do(new CCmdAddLayer(this, pLayer, &pFrame->GetLayerToolbar()->m_wndListCtrl));

	CFile file(lpFileName, CFile::modeRead|CFile::typeBinary);
	CArchive ar(&file, CArchive::load);

	int nRows;
	int nCols;

	ar >> nRows;
	ar >> nCols;

	CString strTmp[4];
	CGeoPolyObj *pObj = new CGeoPolyObj(pLayer);
	std::vector<CPoint2D> points;
	for (int i=0;i<nRows;i++)
	{
		ar >> strTmp[0];
		ar >> strTmp[1];
		ar >> strTmp[2];
		ar >> strTmp[3];

		CPoint2D pt;
		pt.x = atof(strTmp[0]);
		pt.y = atof(strTmp[1]);
		if( strTmp[3].IsEmpty())
			strcpy_s(pt.szInfo,(LPCTSTR)strTmp[3]);		
	}
	ar.Close();
	file.Close();


	if( !m_pCurDrawPage->m_bParameter ) // 如果没有设置参数
	{		
		double minX = DBL_MAX,minY=DBL_MAX, maxX=-DBL_MAX, maxY=-DBL_MAX;

		int nSize = points.size();
		for (int i=0; i<nSize; i++)
		{
			double x,y;

			x = points[i].x;
			y = points[i].y;

			if( x>maxX )
				maxX = x;
			if( x<minX)
				minX =x;
			if( y>maxY)
				maxY = y;
			if(y<minY)
				minY = y;
		}

		m_pCurDrawPage->m_ptCoordOrig.x = -(maxX+minX)/2.0;
		m_pCurDrawPage->m_ptCoordOrig.y = -(maxY+minY)/2.0;

		m_pCurDrawPage->m_lScale = 1;
	}

	CGeoDrawObjList objList;
	
	int nSize = points.size();
	for (int i=0; i<nSize; i++)
	{
		CPoint2D point = points[i];
		CGeoPointObj *pObj = new CGeoPointObj(point,m_pCurDrawLayer);
		objList.AddTail(pObj);
	}

	Do(new CCmdAddDrawObjs(this, objList));

	SetModifiedFlag();
	pView->Invalidate(FALSE);
}

void CGeoMapDoc::CreateInterLayer(LPCTSTR lpFileName, LPCTSTR lpLayerName)
{
	POSITION pos = GetFirstViewPosition();
	CGeoMapView *pView = (CGeoMapView *)GetNextView(pos);
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CDrawLayer *pLayer = new CDrawLayer(this);
	pLayer->m_strLayerName = lpLayerName;
	pLayer->m_eLayerType = CDrawLayer::generic;
	Do(new CCmdAddLayer(this, pLayer, &pFrame->GetLayerToolbar()->m_wndListCtrl));

	CFile file(lpFileName, CFile::modeRead|CFile::typeBinary);
	CArchive ar(&file, CArchive::load);

	int nRows;
	int nCols;

	ar >> nRows;
	ar >> nCols;

	int num = 0;
	CString strTmp[4];
	CGeoPolyObj *pObj = new CGeoPolyObj(pLayer);
	for (int i=0;i<nRows;i++)
	{
		ar >> strTmp[0];
		ar >> strTmp[1];
		ar >> strTmp[2];
		ar >> strTmp[3];
		if( strTmp[0] == _T("EOM"))
		{
			if( pObj->m_points > 0 )
			{
				pObj->RecalcBounds(pView);
				Do(new CCmdAddDrawObj(this, pObj));
				pObj = NULL;
				num ++;
			}
			else
			{
				delete pObj;
				pObj = NULL;
			}

			pObj = new CGeoPolyObj(pLayer);
		}
		else
		{
			CPoint2D point;
			point.x = atof(strTmp[0]);
			point.y = atof(strTmp[1]);
			pObj->AddPoint(point,pView);
		}
	}

	if( pObj != NULL )
	{
		if( pObj->m_points > 0 )
		{
			pObj->RecalcBounds(pView);
			Do(new CCmdAddDrawObj(this, pObj));
			pObj = NULL;
			num ++;
		}
		else
		{
			delete pObj;
			pObj = NULL;
		}
	}
	ar.Close();
	file.Close();

	COLORREF clrMax = RGB(255,0,0);
	COLORREF clrMin = RGB(0,0,255);
	double dRStep = (double)( GetRValue(clrMax) - GetRValue(clrMin) ) / num ;
	double dGStep = (double)( GetGValue(clrMax) - GetGValue(clrMin) ) / num ;
	double dBStep = (double)( GetBValue(clrMax) - GetBValue(clrMin) ) / num ;

	int R, G, B;
	POSITION pos1 = pLayer->m_DrawObjects.GetTailPosition();
	for (int i=0; i<num; i++)
	{
		pObj = (CGeoPolyObj*)pLayer->m_DrawObjects.GetPrev(pos1);
		R = i * dRStep + GetRValue(clrMin);
		G = i * dGStep + GetGValue(clrMin);
		B = i * dBStep + GetBValue(clrMin);

		pObj->m_eLine.s_LineStyle.m_PatternFill.m_crColourFore = RGB(R, G, B);
	}	

	SetModifiedFlag();
	pView->Invalidate(FALSE);
}

void CGeoMapDoc::CreateInterNormal(LPCTSTR lpFileName, LPCTSTR lpLayerName)
{
	POSITION pos = GetFirstViewPosition();
	CGeoMapView *pView = (CGeoMapView *)GetNextView(pos);
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CDrawLayer *pLayer = new CDrawLayer(this);
	pLayer->m_strLayerName = lpLayerName;
	pLayer->m_eLayerType = CDrawLayer::generic;
	Do(new CCmdAddLayer(this, pLayer, &pFrame->GetLayerToolbar()->m_wndListCtrl));

	CFile file(lpFileName, CFile::modeRead|CFile::typeBinary);
	CArchive ar(&file, CArchive::load);

	int nRows;
	int nCols;

	ar >> nRows;
	ar >> nCols;

	//int num = 0;

	CString strTmp[4];
	CGeoPolyObj *pObj = new CGeoPolyObj(pLayer);
	pObj->m_eLine.s_LineStyle.m_PatternFill.m_crColourFore = RGB(0, 0, 255);
	for (int i=0;i<nRows;i++)
	{
		ar >> strTmp[0];
		ar >> strTmp[1];
		ar >> strTmp[2];
		ar >> strTmp[3];
		if( strTmp[0] == _T("EOM"))
		{
			if( pObj->m_points > 0 )
			{
				pObj->RecalcBounds(pView);
				Do(new CCmdAddDrawObj(this, pObj));
				pObj = NULL;
				//num ++;
			}
			else
			{
				delete pObj;
				pObj = NULL;
			}

			pObj = new CGeoPolyObj(pLayer);
			pObj->m_eLine.s_LineStyle.m_PatternFill.m_crColourFore = RGB(0, 0, 255);
		}
		else
		{
			CPoint2D point;
			point.x = atof(strTmp[0]);
			point.y = atof(strTmp[1]);
			pObj->AddPoint(point,pView);
		}
	}

	if( pObj != NULL )
	{
		if( pObj->m_points > 0 )
		{
			pObj->RecalcBounds(pView);
			Do(new CCmdAddDrawObj(this, pObj));
			pObj = NULL;
			//num ++;
		}
		else
		{
			delete pObj;
			pObj = NULL;
		}
	}
	ar.Close();
	file.Close();

	/*COLORREF clrMax = RGB(255,0,0);
	COLORREF clrMin = RGB(0,0,255);
	double dRStep = (double)( GetRValue(clrMax) - GetRValue(clrMin) ) / num ;
	double dGStep = (double)( GetGValue(clrMax) - GetGValue(clrMin) ) / num ;
	double dBStep = (double)( GetBValue(clrMax) - GetBValue(clrMin) ) / num ;

	int R, G, B;
	POSITION pos1 = pLayer->m_DrawObjects.GetTailPosition();
	for (int i=0; i<num; i++)
	{
		pObj = (CGeoPolyObj*)pLayer->m_DrawObjects.GetPrev(pos1);
		R = i * dRStep + GetRValue(clrMin);
		G = i * dGStep + GetGValue(clrMin);
		B = i * dBStep + GetBValue(clrMin);

		pObj->m_eLine.s_LineStyle.m_PatternFill.m_crColourFore = RGB(R, G, B);
	}	
	*/

	SetModifiedFlag();
	pView->Invalidate(FALSE);
}

void CGeoMapDoc::CreatePolyline(LPCTSTR lpFileName, LPCTSTR lpLayerName)
{
	POSITION pos = GetFirstViewPosition();
	CGeoMapView *pView = (CGeoMapView *)GetNextView(pos);
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CDrawLayer *pLayer = new CDrawLayer(this);
	pLayer->m_strLayerName = lpLayerName;
	pLayer->m_eLayerType = CDrawLayer::generic;
	Do(new CCmdAddLayer(this, pLayer, &pFrame->GetLayerToolbar()->m_wndListCtrl));

	CFile file(lpFileName, CFile::modeRead|CFile::typeBinary);
	CArchive ar(&file, CArchive::load);

	int nRows;
	int nCols;

	ar >> nRows;
	ar >> nCols;

	CString strTmp[4];
	CGeoPolyObj *pObj = new CGeoPolyObj(pLayer);
	for (int i=0;i<nRows;i++)
	{
		ar >> strTmp[0];
		ar >> strTmp[1];
		ar >> strTmp[2];
		ar >> strTmp[3];
		if( strTmp[0] == _T("EOM"))
		{
			if( pObj->m_points > 0 )
			{
				pObj->RecalcBounds(pView);
				Do(new CCmdAddDrawObj(this, pObj));
				pObj = NULL;
			}
			else
			{
				delete pObj;
				pObj = NULL;
			}

			pObj = new CGeoPolyObj(pLayer);
		}
		else
		{
			CPoint2D point;
			point.x = atof(strTmp[0]);
			point.y = atof(strTmp[1]);
			pObj->AddPoint(point,pView);
		}
	}

	if( pObj != NULL )
	{
		if( pObj->m_points > 0 )
		{
			pObj->RecalcBounds(pView);
			Do(new CCmdAddDrawObj(this, pObj));
			pObj = NULL;
		}
		else
		{
			delete pObj;
			pObj = NULL;
		}
	}
	ar.Close();
	file.Close();
	
	SetModifiedFlag();
	pView->Invalidate(FALSE);
}

void CGeoMapDoc::CreatePolygon(LPCTSTR lpFileName, LPCTSTR lpLayerName)
{
	POSITION pos = GetFirstViewPosition();
	CGeoMapView *pView = (CGeoMapView *)GetNextView(pos);
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CDrawLayer *pLayer = new CDrawLayer(this);
	pLayer->m_strLayerName = lpLayerName;
	pLayer->m_eLayerType = CDrawLayer::generic;
	Do(new CCmdAddLayer(this, pLayer, &pFrame->GetLayerToolbar()->m_wndListCtrl));

	CFile file(lpFileName, CFile::modeRead|CFile::typeBinary);
	CArchive ar(&file, CArchive::load);

	int nRows;
	int nCols;

	ar >> nRows;
	ar >> nCols;

	CString strTmp[4];
	CGeoPolyObj *pObj = new CGeoPolyObj(pLayer);
	for (int i=0;i<nRows;i++)
	{
		ar >> strTmp[0];
		ar >> strTmp[1];
		ar >> strTmp[2];
		ar >> strTmp[4];
		if( strTmp[0] == _T("EOM"))
		{
			if( pObj->m_points > 0 )
			{
				pObj->RecalcBounds(pView);
				pObj->m_eLine.m_bClose = TRUE;
				Do(new CCmdAddDrawObj(this, pObj));
				pObj = NULL;
			}
			else
			{
				delete pObj;
				pObj = NULL;
			}

			pObj = new CGeoPolyObj(pLayer);
		}
		else
		{
			CPoint2D point;
			point.x = atof(strTmp[0]);
			point.y = atof(strTmp[1]);
			pObj->AddPoint(point,pView);
		}
	}

	if( pObj != NULL )
	{
		if( pObj->m_points > 0 )
		{
			pObj->RecalcBounds(pView);
			Do(new CCmdAddDrawObj(this, pObj));
			pObj = NULL;
		}
		else
		{
			delete pObj;
			pObj = NULL;
		}
	}
	ar.Close();
	file.Close();

	SetModifiedFlag();
	pView->Invalidate(FALSE);
}

void CGeoMapDoc::CreatePlane(LPCTSTR lpFileName, LPCTSTR lpLayerName)
{
	POSITION pos = GetFirstViewPosition();
	CGeoMapView *pView = (CGeoMapView *)GetNextView(pos);
	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
	CDrawLayer *pLayer = new CDrawLayer(this);
	pLayer->m_strLayerName = lpLayerName;
	pLayer->m_eLayerType = CDrawLayer::generic;
	Do(new CCmdAddLayer(this, pLayer, &pFrame->GetLayerToolbar()->m_wndListCtrl));

	CFile file(lpFileName, CFile::modeRead|CFile::typeBinary);
	CArchive ar(&file, CArchive::load);

	int nRows;
	int nCols;

	ar >> nRows;
	ar >> nCols;

	CString strTmp[4];
	
	std::vector<CPoint2D> points;
	std::vector<int> indexs;

	BOOL bIsVertex = TRUE;
	StatusProgress(2,nRows);
	for (int i=0;i<nRows;i++)
	{
		ar >> strTmp[0];
		ar >> strTmp[1];
		ar >> strTmp[2];
		ar >> strTmp[3];
		if( strTmp[0] == _T("Coordinate"))
		{
			if( points.size() > 0 )
			{
				int n = indexs.size()/3;
				for (int j=0;j<n;j++)
				{
					CGeoPolyObj *pObj = new CGeoPolyObj(pLayer);
					
					CPoint2D point = points[indexs[j*3]];
					pObj->AddPoint(point,pView);
					point = points[indexs[j*3+1]];
					pObj->AddPoint(point,pView);
					point = points[indexs[j*3+2]];
					pObj->AddPoint(point,pView);

					pObj->m_eLine.m_bClose = TRUE;
					pObj->RecalcBounds(pView);
					Do(new CCmdAddDrawObj(this, pObj));
				}

				points.erase(points.begin(), points.end());
				indexs.erase(indexs.begin(), indexs.end());
			}

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
			CPoint2D point;
			point.x = atof(strTmp[0]);
			point.y = atof(strTmp[1]);
			point.m_dValue = atof(strTmp[2]);

			points.push_back(point);
		}
		else
		{
			indexs.push_back(atoi(strTmp[0]));
			indexs.push_back(atoi(strTmp[1]));
			indexs.push_back(atoi(strTmp[2]));
		}

		StatusSetProgress(2, i+1);
	}

	StatusProgress(2);

	if( points.size() > 0 )
	{
		int n = indexs.size()/3;
		for (int j=0;j<n;j++)
		{
			CGeoPolyObj *pObj = new CGeoPolyObj(pLayer);

			CPoint2D point = points[indexs[j*3]];
			pObj->AddPoint(point,pView);
			point = points[indexs[j*3+1]];
			pObj->AddPoint(point,pView);
			point = points[indexs[j*3+2]];
			pObj->AddPoint(point,pView);

			pObj->m_eLine.m_bClose = TRUE;
			pObj->RecalcBounds(pView);
			Do(new CCmdAddDrawObj(this, pObj));
		}

		points.erase(points.begin(), points.end());
		indexs.erase(indexs.begin(), indexs.end());
		
	}
	
	ar.Close();
	file.Close();

	SetModifiedFlag();
	pView->Invalidate(FALSE);
}

// 井位图
void CGeoMapDoc::CreateWellLocationMap(HTREEITEM hItem)
{
	POSITION pos = GetFirstViewPosition();
	CGeoMapView *pView = (CGeoMapView *)GetNextView(pos);
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

	if( !m_pCurDrawPage->m_bParameter ) // 如果没有设置参数
	{		
		double minX = DBL_MAX,minY=DBL_MAX, maxX=-DBL_MAX, maxY=-DBL_MAX;

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
			hItemChild = pTree->GetNextSiblingItem(hItemChild);
		}

		m_pCurDrawPage->m_ptCoordOrig.x = -(maxX+minX)/2.0;
		m_pCurDrawPage->m_ptCoordOrig.y = -(maxY+minY)/2.0;

		m_pCurDrawPage->m_lScale = 1;
	}

	CDrawLayer *pLayer = new CDrawLayer(this);
	pLayer->m_strLayerName = _T("井位图");
	pLayer->m_eLayerType = CDrawLayer::wellmap;
	Do(new CCmdAddLayer(this, pLayer, &pFrame->GetLayerToolbar()->m_wndListCtrl));	

	CGeoDrawObjList objList;
	hItemChild = pTree->GetChildItem(hItem);

	while(hItemChild != NULL )
	{
		if( pTree->GetCheck(hItemChild) )
		{
			CTreeNodeDat *lpNodeDat = (CTreeNodeDat *)pTree->GetItemData(hItemChild);
			if( lpNodeDat != NULL )
			{
				CWellInfo *pInfo = (CWellInfo *)lpNodeDat->m_pNodeDat;
				CString strWellName;
				CString strMQJB, strJX; // 井别、井型
				double x,y;

				x = pInfo->m_x;
				y = pInfo->m_y;

				strWellName = pInfo->m_strWellName;
				strMQJB = pInfo->m_strWellType;
				if( strMQJB == _T("采油井")
					|| strMQJB == _T("O")
					|| strMQJB == _T("11"))
					strMQJB = _T("11");
				else
					strMQJB = _T("31");

				CPoint2D point(x,y);

				CGeoWellObj *pObj = new CGeoWellObj(point,m_pCurDrawLayer,strWellName);
				pObj->m_strWellType = strMQJB;

				objList.AddTail(pObj);
			}
		}
		hItemChild = pTree->GetNextSiblingItem(hItemChild);
	}

	Do(new CCmdAddDrawObjs(this, objList));

	pView->Invalidate(FALSE);
}

void CGeoMapDoc::SetTitle(LPCTSTR lpszTitle)
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

BOOL CGeoMapDoc::SaveModified()
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
