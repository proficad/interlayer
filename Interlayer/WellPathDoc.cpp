// WellPathDoc.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Interlayer.h"
#include "WellPathDoc.h"
#include "WellPathView.h"
#include "MainFrm.h"

// CWellPathDoc

IMPLEMENT_DYNCREATE(CWellPathDoc, CDocument)

CWellPathDoc::CWellPathDoc()
{
	m_nRows = 0;
	m_nCols = 6;
}

BOOL CWellPathDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CWellPathDoc::~CWellPathDoc()
{
}


BEGIN_MESSAGE_MAP(CWellPathDoc, CDocument)
END_MESSAGE_MAP()


// CWellPathDoc ���

#ifdef _DEBUG
void CWellPathDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CWellPathDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CWellPathDoc ���л�

void CWellPathDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
		m_arWellPath.RemoveAll();

		m_arWellPath.Add(_T("����"));
		m_arWellPath.Add(_T("��б��"));
		m_arWellPath.Add(_T("��λ��"));
		m_arWellPath.Add(_T("������"));
		m_arWellPath.Add(_T("������"));
		m_arWellPath.Add(_T("����"));

		POSITION pos = GetFirstViewPosition();
		CWellPathView *pView = (CWellPathView*)GetNextView(pos);
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
				m_arWellPath.Add(strTmp);
			}
		}
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
		m_arWellPath.RemoveAll();

		m_arWellPath.Add(_T("����"));
		m_arWellPath.Add(_T("��б��"));
		m_arWellPath.Add(_T("��λ��"));
		m_arWellPath.Add(_T("������"));
		m_arWellPath.Add(_T("������"));
		m_arWellPath.Add(_T("����"));

		ar >> m_nRows;
		ar >> m_nCols;		
		long n = (m_nCols*m_nRows); 
		for (long i=0;i<n;i++)
		{
			CString strTmp;
			ar >> strTmp;
			m_arWellPath.Add(strTmp);
		}
	}
}
#endif


// CWellPathDoc ����

BOOL CWellPathDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// TODO:  �ڴ������ר�õĴ�������
	
	return TRUE;
}

BOOL CWellPathDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: �ڴ����ר�ô����/����û���

	return CDocument::OnSaveDocument(lpszPathName);
}

void CWellPathDoc::SetTitle(LPCTSTR lpszTitle)
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

BOOL CWellPathDoc::SaveModified()
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
