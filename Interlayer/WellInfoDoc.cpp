// WellInfoDoc.cpp : 实现文件
//

#include "stdafx.h"
#include "Interlayer.h"
#include "WellInfoDoc.h"
#include "MainFrm.h"
#include "WellInfoView.h"
#include "FileView.h"
#include "ViewTree.h"

// CWellInfoDoc

IMPLEMENT_DYNCREATE(CWellInfoDoc, CDocument)

CWellInfoDoc::CWellInfoDoc()
{
	m_nRows = 0;
	m_nCols = 7;
}

BOOL CWellInfoDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CWellInfoDoc::~CWellInfoDoc()
{
}


BEGIN_MESSAGE_MAP(CWellInfoDoc, CDocument)
END_MESSAGE_MAP()


// CWellInfoDoc 诊断

#ifdef _DEBUG
void CWellInfoDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CWellInfoDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CWellInfoDoc 序列化

void CWellInfoDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
		m_arWellInfo.RemoveAll();

		m_arWellInfo.Add(_T("井号"));
		m_arWellInfo.Add(_T("X"));
		m_arWellInfo.Add(_T("Y"));
		m_arWellInfo.Add(_T("补芯海拔"));
		m_arWellInfo.Add(_T("目前井别"));
		m_arWellInfo.Add(_T("油层顶深"));
		m_arWellInfo.Add(_T("油层底深"));

		POSITION pos = GetFirstViewPosition();
		CWellInfoView *pView = (CWellInfoView*)GetNextView(pos);
		m_nRows = pView->m_wndGrid.GetRowCount()-1;
		m_nCols = pView->m_wndGrid.GetColumnCount()-1;

		ar << m_nRows;
		ar << m_nCols;
		for (int i=0;i<m_nRows;i++)
		{
			for (int j=0; j<m_nCols; j++)
			{
				CString strTmp;
				strTmp = pView->m_wndGrid.GetItemText(i+1,j+1);
				ar << strTmp;
				m_arWellInfo.Add(strTmp);
			}
		}
	}
	else
	{
		// TODO: 在此添加加载代码
		m_arWellInfo.RemoveAll();

		m_arWellInfo.Add(_T("井号"));
		m_arWellInfo.Add(_T("X"));
		m_arWellInfo.Add(_T("Y"));
		m_arWellInfo.Add(_T("补芯海拔"));
		m_arWellInfo.Add(_T("目前井别"));
		m_arWellInfo.Add(_T("油层顶深"));
		m_arWellInfo.Add(_T("油层底深"));
		
		ar >> m_nRows;
		ar >> m_nCols;		
		long n = (m_nCols*m_nRows); 
		for (long i=0;i<n;i++)
		{
			CString strTmp;
			ar >> strTmp;
			m_arWellInfo.Add(strTmp);
		}
	}
}
#endif


// CWellInfoDoc 命令

BOOL CWellInfoDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// TODO:  在此添加您专用的创建代码

	SetTitle(_T("井基础信息"));

	return TRUE;
}

BOOL CWellInfoDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: 在此添加专用代码和/或调用基类

	BOOL bRet = CDocument::OnSaveDocument(lpszPathName);
	if( bRet )
	{
		CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();

		CFileView *pTreeView = pMF->GetTreeFileView();
		CViewTree *pTree = pTreeView->GetTreeCtrl();
		HTREEITEM hRoot = pTree->GetChildNode(TVI_ROOT, _T("单井数据"));
		HTREEITEM hSrc = pTree->GetChildNode(hRoot, _T("井基础数据"));

		pTree->DeleteAllChildItem(hSrc);

		{ // 井轨迹数据信息文件
			CString strDatFile = pMF->GetProjectDatPath();
			strDatFile += _T("\\files\\");
			strDatFile += _T("WellPathInfo.dat");

			int nRows = m_nRows;
			int nCols = 2;

			CFile file(strDatFile, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
			CArchive ar(&file, CArchive::store);
			ar << nRows;
			ar << nCols;

			CString strTmp = _T("");		

			for ( int i=1; i<m_nRows+1; i++)
			{
				CString strWellName = m_arWellInfo[i*m_nCols];

				strWellName.Trim();
				if( strWellName.IsEmpty() )
					continue;

				int nImage = pTree->m_nCountImages - 19;
				int iImage;

				CString strWellType = m_arWellInfo[i*m_nCols + 4];

				CTreeNodeDat *lpNodeDat = new CTreeNodeDat;
				CWellInfo *pWellInfo = new CWellInfo;

				pWellInfo->m_strWellName = strWellName;
				pWellInfo->m_strWellType = strWellType;
				pWellInfo->m_x = atof(m_arWellInfo[i*m_nCols + 1]);
				pWellInfo->m_y = atof(m_arWellInfo[i*m_nCols + 2]);

				pWellInfo->m_dElevation = atof(m_arWellInfo[i*m_nCols + 3]);
				pWellInfo->m_dTop = atof(m_arWellInfo[i*m_nCols + 5]);
				pWellInfo->m_dBottom = atof(m_arWellInfo[i*m_nCols + 6]);				

				strWellType.Trim();
				strWellType.MakeUpper();
				if( strWellType == _T("采油井")
					|| strWellType == _T("O")
					|| strWellType == _T("11"))
				{
					iImage = nImage + 7;
					lpNodeDat->m_nType			= WELL_PRO;
				}
				else
				{
					iImage = nImage + 8;
					lpNodeDat->m_nType			= WELL_INJ;
				}

				HTREEITEM hItem = pTreeView->AddTreeItem(strWellName, iImage, iImage, hSrc);			

				lpNodeDat->m_nImage			= iImage;
				lpNodeDat->m_nSelectedImage	= iImage;
				lpNodeDat->m_pNodeDat		= pWellInfo;
				lpNodeDat->m_uState			= TVIS_EXPANDED;

				pTree->SetItemData(hItem, (DWORD)lpNodeDat);
				ar << strWellName;
				ar << strTmp;
			}

			ar.Close();
			file.Close();
		}
		{	// 测井信息数据
			CString strDatFile = pMF->GetProjectDatPath();
			strDatFile += _T("\\files\\");
			strDatFile += _T("WellLoggingInfo.dat");

			int nRows = m_nRows;
			int nCols = 2;

			CFile file(strDatFile, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
			CArchive ar(&file, CArchive::store);
			ar << nRows;
			ar << nCols;

			CString strTmp = _T("");		

			for ( int i=1; i<m_nRows+1; i++)
			{
				CString strWellName = m_arWellInfo[i*m_nCols];

				strWellName.Trim();
				if( strWellName.IsEmpty() )
					continue;

				ar << strWellName;
				ar << strTmp;
			}

			ar.Close();
			file.Close();
		}

		pTree->Expand(hSrc, TVE_EXPAND);
		
	}
	return bRet;
}

void CWellInfoDoc::SetTitle(LPCTSTR lpszTitle)
{
	// TODO: 在此添加专用代码和/或调用基类

	CString strFileName = GetPathName();
	strFileName.MakeUpper();

	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();

	CString strPath = pFrame->GetProjectDatPath();
	strPath += _T("\\files\\");
	strPath.MakeUpper();

	strFileName.Replace(strPath, _T(""));

	HTREEITEM hItem = pFrame->GetTreeFileView()->GetTreeCtrl()->IsExistFileInTree(TVI_ROOT, strFileName);

	CString strTitle = pFrame->GetTreeFileView()->GetTreeCtrl()->GetItemText(hItem);

	m_strTitle = strTitle;

	UpdateFrameCounts();
}

BOOL CWellInfoDoc::SaveModified()
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
