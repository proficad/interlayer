#include "StdAfx.h"
#include "WellLoggingInfoDoc.h"
#include "WellLoggingInfoView.h"
#include "MainFrm.h"
// CWellLoggingInfoDoc

IMPLEMENT_DYNCREATE(CWellLoggingInfoDoc, CDocument)

CWellLoggingInfoDoc::CWellLoggingInfoDoc()
{
	m_nRows = 0;
	m_nCols = 2;
}

BOOL CWellLoggingInfoDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CWellLoggingInfoDoc::~CWellLoggingInfoDoc()
{
}


BEGIN_MESSAGE_MAP(CWellLoggingInfoDoc, CDocument)
END_MESSAGE_MAP()


// CWellLoggingInfoDoc 诊断

#ifdef _DEBUG
void CWellLoggingInfoDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CWellLoggingInfoDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CWellLoggingInfoDoc 序列化

void CWellLoggingInfoDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
		m_arWellLoggingInfo.RemoveAll();

		m_arWellLoggingInfo.Add(_T("井号"));
		m_arWellLoggingInfo.Add(_T("文件"));

		POSITION pos = GetFirstViewPosition();
		CWellLoggingInfoView *pView = (CWellLoggingInfoView*)GetNextView(pos);
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
				m_arWellLoggingInfo.Add(strTmp);
			}
		}
	}
	else
	{
		// TODO: 在此添加加载代码
		m_arWellLoggingInfo.RemoveAll();

		m_arWellLoggingInfo.Add(_T("井号"));
		m_arWellLoggingInfo.Add(_T("文件"));

		ar >> m_nRows;
		ar >> m_nCols;
		long n = (m_nCols*m_nRows); 
		for (long i=0;i<n;i++)
		{
			CString strTmp;
			ar >> strTmp;
			m_arWellLoggingInfo.Add(strTmp);
		}
	}
}
#endif


// CWellLoggingInfoDoc 命令

BOOL CWellLoggingInfoDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// TODO:  在此添加您专用的创建代码
	
	return TRUE;
}

BOOL CWellLoggingInfoDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CDocument::OnSaveDocument(lpszPathName);
}

void CWellLoggingInfoDoc::SetTitle(LPCTSTR lpszTitle)
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

BOOL CWellLoggingInfoDoc::SaveModified()
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
