#include "StdAfx.h"
#include "DrawLayer.h"
#include "GeoMapDoc.h"
#include "GeoMapView.h"
#include "ToolBase.h"
#include "../MainFrm.h"
#include "../DlgLayerSet.h"
#include "../MainFrm.h"

extern const char* newGUID();

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(CDrawLayer, CRefObject, VERSIONABLE_SCHEMA|1)

//extern CGeoMapView *p_View;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDrawLayer::CDrawLayer()
{
	m_bShow = TRUE; //是否显示 TURE-显示 FALSE-隐藏
	m_bLocked = FALSE;//是否锁定 TRUE-是  FALSE-否   
	m_strID = newGUID();
	m_strLayerName = "";
	m_pParentPage = NULL;
	m_bClipping = FALSE;
	m_rgnClipping.m_pParentLayer = this;
	m_bIsNew = TRUE;
	m_bIsModified = TRUE;
	m_eLayerType = generic;
	m_iTransparence = 255;
	m_pDocument			= NULL;
}

CDrawLayer::CDrawLayer(CGeoMapDoc *pDoc)
{
	m_pDocument = pDoc;
	m_bShow = TRUE; //是否显示 TURE-显示 FALSE-隐藏
	m_bLocked = FALSE;//是否锁定 TRUE-是  FALSE-否
	m_strID = newGUID();
	m_strLayerName = "";
	m_pParentPage = NULL;
	m_bClipping = FALSE;
	m_rgnClipping.m_pParentLayer = this;
	m_rgnClipping.m_pDocument = pDoc;
	m_bIsNew = TRUE;
	m_bIsModified = TRUE;
	m_eLayerType = generic;
	m_iTransparence = 255;
}

CDrawLayer::~CDrawLayer()
{
	m_DrawObjects.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////

void CDrawLayer::Draw(CDC* pDC, CGeoMapView *pView)
{
	if(!pDC->IsPrinting() 
		&& CToolBase::c_drawShape == CToolBase::clippingRgn
		&& m_pDocument->m_pCurDrawLayer == this)
	{
		m_rgnClipping.Draw(pDC,pView);
		m_rgnClipping.DrawTracker(pDC, pView);
	}

	if(m_bShow)
	{
		int nSavedDC = pDC->SaveDC();

		if( m_bClipping )
			m_rgnClipping.SelectClipRgn(pDC);
		else
			pDC->SelectClipRgn(NULL);

		if( m_iTransparence == 255 ) // 不透明
		{
			POSITION pos = m_DrawObjects.GetHeadPosition();
			while (pos != NULL)
			{
				CGeoDrawObj* pObj = m_DrawObjects.GetNext(pos);
				pObj->Draw(pDC, pView);
			}
		}
		else	// 透明
		{
		}

		if( m_bClipping )
			pDC->SelectClipRgn(NULL);

		pDC->RestoreDC(nSavedDC);
	}
}

void CDrawLayer::DrawTracker(CDC* pDC, CGeoMapView *pView)
{
	POSITION pos = m_DrawObjects.GetHeadPosition();
	while (pos != NULL)
	{
		CGeoDrawObj* pObj = m_DrawObjects.GetNext(pos);	
		if (pView->m_bActive 
			&& !pDC->IsPrinting() 
			&& pView->IsSelected(pObj) )
			pObj->DrawTracker(pDC, pView);
	}
}

void CDrawLayer::Serialize(CArchive& ar)
{
	CRefObject::Serialize(ar);

	ar.SerializeClass(RUNTIME_CLASS(CDrawLayer));

	m_DrawObjects.Serialize(ar);

	if (ar.IsStoring())
	{	
		CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
		pFrame->GetLayerToolbar()->Invalidate(FALSE);// 刷新图层工具条

		m_bIsNew = FALSE;
		m_rgnClipping.Serialize(ar);
		ar << m_bShow;
		ar << m_bLocked;
		ar << m_strLayerName;
		ar << m_strID;
		ar << m_bClipping;
		ar << m_bIsNew;
		ar << m_bIsModified;
		ar << (WORD)m_eLayerType;
		ar << m_iTransparence;
	}
	else
	{
		int nVersion = ar.GetObjectSchema(); // 获取版本信息
		switch( nVersion )
		{
		case 1:
			{
				m_rgnClipping.Serialize(ar);
				m_pDocument = (CGeoMapDoc*)ar.m_pDocument;
				ASSERT_VALID(m_pDocument);
				ASSERT_KINDOF(CGeoMapDoc, m_pDocument);
				ar >> m_bShow;
				ar >> m_bLocked;
				ar >> m_strLayerName;
				ar >> m_strID;
				ar >> m_bClipping;
				ar >> m_bIsNew;
				ar >> m_bIsModified;
				WORD tmp;
				ar >> tmp;
				m_eLayerType = (GeoLayerType)tmp;
				ar >> m_iTransparence;

				POSITION pos = m_DrawObjects.GetHeadPosition();
				while (pos != NULL)
				{
					CGeoDrawObj* pObj = m_DrawObjects.GetNext(pos);
					pObj->m_pParentLayer = this;	
				}
				m_rgnClipping.m_pParentLayer = this;
			}
			break;
		default:
			break;
		}
	}	
}

BOOL CDrawLayer::Add(CGeoDrawObj *pObj)
{
	TRY
	{
		if( pObj->m_pParentObj )
		{
			pObj->m_pParentObj->Add(pObj);
		}
		else
			m_DrawObjects.AddTail(pObj);

		CGeoDrawObj *pRootObj = NULL;
		CGeoDrawObj *pTmpObj = pObj->m_pParentObj;

		while(pTmpObj!=NULL)
		{
			pRootObj = pTmpObj;
			pTmpObj = pRootObj->m_pParentObj;
		}

		if( pRootObj )
			pRootObj->RecalcBounds(NULL);

		pObj->Invalidate();

		//m_pDocument->UpdateAllViews(NULL);
	}
	CATCH_ALL(e)
	{
		e->Delete();
		return FALSE;
	}
	END_CATCH_ALL

		return TRUE;
}

BOOL CDrawLayer::Remove(CGeoDrawObj *pObj)
{
	TRY
	{
		if( pObj->m_bLocked )
			return FALSE;

		// Find and remove from document
		if( pObj->m_pParentObj )			
		{
			CGeoDrawObj *pRootObj = NULL;
			CGeoDrawObj *pTmpObj = pObj->m_pParentObj;

			while(pTmpObj!=NULL)
			{
				pRootObj = pTmpObj;
				pTmpObj = pRootObj->m_pParentObj;
			}

			BOOL bRet = pObj->m_pParentObj->Remove(pObj);

			if( pRootObj && bRet )
				pRootObj->RecalcBounds(NULL);

			return bRet;
		}
		else
		{
			POSITION pos = m_DrawObjects.Find(pObj);
			if (pos != NULL)
			{
				m_DrawObjects.GetAt(pos);
				m_DrawObjects.RemoveAt(pos);
				m_pDocument->UpdateAllViews(NULL);
			}
			else
				return FALSE;
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

void CDrawLayer::RemoveAll()
{
	m_DrawObjects.RemoveAll();	
}

//功能:图素移到最上一层
//参数:
//返回:
BOOL CDrawLayer::MoveToFront(CGeoDrawObj *pObj)
{
	if( pObj->m_pParentObj )
	{
		return pObj->m_pParentObj->MoveToFront(pObj);
	}
	else
	{
		if(Remove(pObj))
			return Add(pObj);
		else
			return FALSE;
	}

}

//功能:图素移到最底一层
//参数:
//返回:
BOOL CDrawLayer::MoveToBack(CGeoDrawObj *pObj)
{
	if( pObj->m_pParentObj )
	{
		return pObj->m_pParentObj->MoveToBack(pObj);
	}
	{
		if(Remove(pObj))
		{
			m_DrawObjects.AddHead(pObj);
			return TRUE;
		}
		else
			return FALSE;	
	}
}

//功能:图素上移一层
//参数:
//返回:
BOOL CDrawLayer::MoveForward(CGeoDrawObj *pObj)
{
	if( pObj->m_pParentObj )
	{
		return pObj->m_pParentObj->MoveForward(pObj);
	}
	{
		POSITION pos = m_DrawObjects.Find(pObj);
		ASSERT(pos != NULL);
		if (pos != m_DrawObjects.GetTailPosition())
		{
			POSITION posNext = pos;
			m_DrawObjects.GetNext(posNext);
			Remove(pObj);
			m_DrawObjects.InsertAfter(posNext, pObj);
			return TRUE;
		}
		return FALSE;
	}
}

//功能:图素下移一层
//参数:
//返回:
BOOL CDrawLayer::MoveBack(CGeoDrawObj *pObj)
{
	if( pObj->m_pParentObj )
	{
		return pObj->m_pParentObj->MoveBack(pObj);
	}
	{
		POSITION pos = m_DrawObjects.Find(pObj);
		ASSERT(pos != NULL);
		if (pos != m_DrawObjects.GetHeadPosition())
		{
			POSITION posPrev = pos;
			m_DrawObjects.GetPrev(posPrev);
			Remove(pObj);
			m_DrawObjects.InsertBefore(posPrev, pObj);	
			return TRUE;
		}
		return FALSE;
	}
}

//////////////////////////////////////////////////////////////////////////
// 图层大小范围
CRect CDrawLayer::GetLayerRect()
{
	CRect rect(0, 0, 0, 0);
	POSITION pos = m_DrawObjects.GetHeadPosition();
	CGeoDrawObj * pObj;
	while (pos != NULL)
	{		
		pObj = m_DrawObjects.GetNext(pos);
		CRect rc = pObj->m_rtPosition;
		rc.NormalizeRect();
		rect |= rc;
	}

	if( m_bClipping && m_rgnClipping.m_nPoints>2 )
		rect = rect&m_rgnClipping.m_rtPosition;

	rect.NormalizeRect();	

	return rect;
}

// 比例尺改变了
void CDrawLayer::OnScaleChanged()
{
	m_rgnClipping.OnScaleChanged();
	POSITION pos = m_DrawObjects.GetHeadPosition();
	CGeoDrawObj * pObj;
	while (pos != NULL)
	{		
		pObj = m_DrawObjects.GetNext(pos);
		pObj->OnScaleChanged();
	}	
}

void CDrawLayer::Invalidate(CGeoMapView *pView)
{
	POSITION pos = m_DrawObjects.GetHeadPosition();
	CGeoDrawObj * pObj;
	while (pos != NULL)
	{		
		pObj = m_DrawObjects.GetNext(pos);
		if( pView == NULL )
			pObj->Invalidate();
		else
			pView->InvalObj(pObj);
	}
}

void CDrawLayer::ClaimChildren()
{
	POSITION pos = m_DrawObjects.GetHeadPosition();
	CGeoDrawObj * pObj;
	while (pos != NULL)
	{		
		pObj = m_DrawObjects.GetNext(pos);
		pObj->m_pParentLayer = this;
		pObj->m_pParentPage = GetParentPage();
		pObj->ClaimChildren();
	}
}

BOOL CDrawLayer::OnEditProperties()
{
	ASSERT_VALID(this);
	//if( m_bLocked )
	//	return FALSE;

	POSITION pos = m_pDocument->GetFirstViewPosition();
	CGeoMapView *pView = (CGeoMapView *) m_pDocument->GetNextView(pos);
	
	CDlgLayerSet dlg;
	dlg.m_bClipping = m_bClipping;
	dlg.m_bLocked = m_bLocked;
	dlg.m_bShow = m_bShow;
	dlg.m_iType = m_eLayerType;
	dlg.m_strLayerName = m_strLayerName;
	if(dlg.DoModal() == IDOK)
	{
		m_eLayerType = (CDrawLayer::GeoLayerType)dlg.m_iType;
		m_bClipping = dlg.m_bClipping;
		m_bLocked = dlg.m_bLocked;
		m_bShow = dlg.m_bShow;
		m_strLayerName = dlg.m_strLayerName;

		pView->InvalLayer(this);
		m_pDocument->SetModifiedFlag();

		CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
		pMF->GetLayerToolbar()->RefreshListCtrl();
		return TRUE;
	}

	return FALSE;
}