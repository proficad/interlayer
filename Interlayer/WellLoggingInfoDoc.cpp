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


// CWellLoggingInfoDoc ���

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
// CWellLoggingInfoDoc ���л�

void CWellLoggingInfoDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
		m_arWellLoggingInfo.RemoveAll();

		m_arWellLoggingInfo.Add(_T("����"));
		m_arWellLoggingInfo.Add(_T("�ļ�"));

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
		// TODO: �ڴ���Ӽ��ش���
		m_arWellLoggingInfo.RemoveAll();

		m_arWellLoggingInfo.Add(_T("����"));
		m_arWellLoggingInfo.Add(_T("�ļ�"));

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


// CWellLoggingInfoDoc ����

BOOL CWellLoggingInfoDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// TODO:  �ڴ������ר�õĴ�������
	
	return TRUE;
}

BOOL CWellLoggingInfoDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: �ڴ����ר�ô����/����û���

	return CDocument::OnSaveDocument(lpszPathName);
}

void CWellLoggingInfoDoc::SetTitle(LPCTSTR lpszTitle)
{
	// TODO: �ڴ����ר�ô����/����û���

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
	// TODO: �ڴ����ר�ô����/����û���

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
