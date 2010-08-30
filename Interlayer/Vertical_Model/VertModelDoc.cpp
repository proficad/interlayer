// Vertical_Model\VertModelDoc.cpp : 实现文件
//

#include "stdafx.h"
#include "VertModelDoc.h"
#include "../MainFrm.h"
#include "../FileView.h"
#include "../ViewTree.h"

// CVertModelDoc

IMPLEMENT_DYNCREATE(CVertModelDoc, CDocument)

CVertModelDoc::CVertModelDoc()
{	
}

BOOL CVertModelDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CVertModelDoc::~CVertModelDoc()
{
}


BEGIN_MESSAGE_MAP(CVertModelDoc, CDocument)
END_MESSAGE_MAP()


// CVertModelDoc 诊断

#ifdef _DEBUG
void CVertModelDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CVertModelDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CVertModelDoc 序列化

void CVertModelDoc::Serialize(CArchive& ar)
{	
	graph_wnd.Serialize(ar);
}
#endif


// CVertModelDoc 命令

BOOL CVertModelDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: 在此添加专用代码和/或调用基类

	BOOL bRet = CDocument::OnSaveDocument(lpszPathName);
	if( bRet )
	{
		CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();

		CFileView *pTreeView = pMF->GetTreeFileView();
		CViewTree *pTree = pTreeView->GetTreeCtrl();
		HTREEITEM hSrc = pTree->GetChildNode(TVI_ROOT, _T("纵向模型"));

		pTree->DeleteAllChildItem(hSrc);
		{
			int nImage = pTree->m_nCountImages - 19;
			for (int i = 0; i < graph_wnd.GetGraphCount(); i++)
			{
				CGraphProps* pProp = graph_wnd.GetGraph(i);

				HTREEITEM hItem = pTreeView->AddTreeItem(pProp->GetTitle(), nImage + 17, nImage + 17, hSrc);

				CTreeNodeDat *lpNodeDat = new CTreeNodeDat;

				lpNodeDat->m_nImage			= nImage + 17;
				lpNodeDat->m_nSelectedImage	= nImage + 17;
				lpNodeDat->m_nType			= FILE_VERT_MODEL;
				lpNodeDat->m_pNodeDat		= NULL;
				lpNodeDat->m_uState			= TVIS_EXPANDED;

				pTree->SetItemData(hItem, (DWORD)lpNodeDat);
			}
		}

		pTree->Expand(hSrc, TVE_EXPAND);

	}

	return bRet;
}

void CVertModelDoc::SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU)
{
	// TODO: 在此添加专用代码和/或调用基类

	CDocument::SetPathName(lpszPathName, bAddToMRU);
}

void CVertModelDoc::SetTitle(LPCTSTR lpszTitle)
{
	// TODO: 在此添加专用代码和/或调用基类

	m_strTitle = _T("纵向模型");
	UpdateFrameCounts();
}

BOOL CVertModelDoc::SaveModified()
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
