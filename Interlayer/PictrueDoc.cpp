// PictrueDoc.cpp : 实现文件
//

#include "stdafx.h"
#include "Interlayer.h"
#include "PictrueDoc.h"
#include "MainFrm.h"

// CPictrueDoc

IMPLEMENT_DYNCREATE(CPictrueDoc, CDocument)

CPictrueDoc::CPictrueDoc()
{
	m_strHtml = _T("");
}

BOOL CPictrueDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CPictrueDoc::~CPictrueDoc()
{
}


BEGIN_MESSAGE_MAP(CPictrueDoc, CDocument)
END_MESSAGE_MAP()


// CPictrueDoc 诊断

#ifdef _DEBUG
void CPictrueDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CPictrueDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CPictrueDoc 序列化

void CPictrueDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码

		CString strFileName = ar.GetFile()->GetFilePath();
		m_strHtml = GenerateHtml(strFileName);
	}
}
#endif


// CPictrueDoc 命令

CString CPictrueDoc::GenerateHtml(LPCSTR szPicFileName)
{
	CString strOutput;
	LoadLongResource(strOutput, IDR_HTML_DISPPIC);

	TCHAR szCurPath[MAX_PATH];  
	memset(szCurPath,   0,   MAX_PATH);  
	GetModuleFileName(NULL,   szCurPath,   sizeof(szCurPath)/sizeof(TCHAR));   
	CString strMoudlePath = szCurPath;
	strMoudlePath = strMoudlePath.Left(strMoudlePath.ReverseFind('\\')+1);

	CString strSource = _T("src=\"");

	strSource += strMoudlePath;
	strSource += _T("Images\\down.gif\"");
	strSource.Replace(_T("\\"),_T("/"));
	strOutput.Replace(_T("src=\"down.gif\""), strSource);
	
	strSource = _T("src=\"");
	strSource += strMoudlePath;
	strSource += _T("Images\\up.gif\"");
	strSource.Replace(_T("\\"),_T("/"));
	strOutput.Replace(_T("src=\"up.gif\""), strSource);

	strSource = _T("src=\"");
	strSource += strMoudlePath;
	strSource += _T("Images\\left.gif\"");
	strSource.Replace(_T("\\"),_T("/"));
	strOutput.Replace(_T("src=\"left.gif\""), strSource);

	strSource = _T("src=\"");
	strSource += strMoudlePath;
	strSource += _T("Images\\right.gif\"");
	strSource.Replace(_T("\\"),_T("/"));
	strOutput.Replace(_T("src=\"right.gif\""), strSource);

	strSource = _T("src=\"");
	strSource += strMoudlePath;
	strSource += _T("Images\\zoom.gif\"");
	strSource.Replace(_T("\\"),_T("/"));
	strOutput.Replace(_T("src=\"zoom.gif\""), strSource);

	strSource = _T("src=\"");
	strSource += strMoudlePath;
	strSource += _T("Images\\zoom_in.gif\"");
	strSource.Replace(_T("\\"),_T("/"));
	strOutput.Replace(_T("src=\"zoom_in.gif\""), strSource);

	strSource = _T("src=\"");
	strSource += strMoudlePath;
	strSource += _T("Images\\zoom_out.gif\"");
	strSource.Replace(_T("\\"),_T("/"));
	strOutput.Replace(_T("src=\"zoom_out.gif\""), strSource);

	strSource = _T("src=\"");
	strSource += strMoudlePath;
	strSource += _T("Images\\zoom_featsize.gif\"");
	strSource.Replace(_T("\\"),_T("/"));
	strOutput.Replace(_T("src=\"zoom_featsize.gif\""), strSource);

	strSource = _T("src='");
	strSource += szPicFileName;
	strSource += _T("'");
	strSource.Replace(_T("\\"),_T("/"));
	strOutput.Replace(_T("src='pictrue.jpg'"), strSource);	

	return strOutput;
}

BOOL CPictrueDoc::LoadLongResource(CString &str, UINT nID)
{
	HRSRC hRes;
	HINSTANCE hInst = AfxGetResourceHandle();
	BOOL bResult = FALSE;

	hRes = FindResource(hInst, MAKEINTRESOURCE(nID), RT_HTML);
	if (hRes == NULL)
	{
		str.Format(_T("Error: Resource %d could not be found"),nID);
	}
	else
	{
		DWORD dwSize = SizeofResource(hInst, hRes);
		if (dwSize == 0)
		{
			str.Empty();
			bResult = TRUE;
		}
		else
		{
			LPTSTR pszStorage = str.GetBufferSetLength(dwSize);

			HGLOBAL hGlob = LoadResource(hInst, hRes);
			if (hGlob != NULL)
			{
				LPVOID lpData = LockResource(hGlob);

				if (lpData != NULL)
				{
					memcpy(pszStorage, lpData, dwSize);
					bResult = TRUE;
				}

				FreeResource(hGlob);
			}
		}
	}

#ifdef _DEBUG
	if (!bResult)
		str.Format(_T("<b>没有发现 %d</b>"), nID);
#endif

	return bResult;
}

void CPictrueDoc::SetTitle(LPCTSTR lpszTitle)
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
