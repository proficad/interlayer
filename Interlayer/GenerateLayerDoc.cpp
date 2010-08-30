// GenerateLayerDoc.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Interlayer.h"
#include "GenerateLayerDoc.h"
#include "MainFrm.h"
#include <float.h>

// CGenerateLayerDoc

IMPLEMENT_DYNCREATE(CGenerateLayerDoc, CDocument)

CGenerateLayerDoc::CGenerateLayerDoc()
{
}

BOOL CGenerateLayerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CGenerateLayerDoc::~CGenerateLayerDoc()
{
}


BEGIN_MESSAGE_MAP(CGenerateLayerDoc, CDocument)
END_MESSAGE_MAP()


// CGenerateLayerDoc ���

#ifdef _DEBUG
void CGenerateLayerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CGenerateLayerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CGenerateLayerDoc ���л�

void CGenerateLayerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
	m_layerModel.Serialize(ar);
}
#endif

void CGenerateLayerDoc::SetTitle(LPCTSTR lpszTitle)
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

BOOL CGenerateLayerDoc::SaveModified()
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

double CGenerateLayerDoc::GetMinX(int index)
{
	double d = DBL_MAX;
	int nSize = m_layerModel.m_vertModel[index].size();
	for (int i=0; i<nSize; i++)
	{
		if( m_layerModel.m_vertModel[index][i].x < d)
			d = m_layerModel.m_vertModel[index][i].x;
	}
	return d;
}
double CGenerateLayerDoc::GetMaxX(int index)
{
	double d = -DBL_MAX;
	int nSize = m_layerModel.m_vertModel[index].size();
	for (int i=0; i<nSize; i++)
	{
		if( m_layerModel.m_vertModel[index][i].x > d)
			d = m_layerModel.m_vertModel[index][i].x;
	}
	return d;
}
double CGenerateLayerDoc::GetMinY(int index)
{
	double d = DBL_MAX;
	int nSize = m_layerModel.m_vertModel[index].size();
	for (int i=0; i<nSize; i++)
	{
		if( m_layerModel.m_vertModel[index][i].y < d)
			d = m_layerModel.m_vertModel[index][i].y;
	}
	return d;
}
double CGenerateLayerDoc::GetMaxY(int index)
{
	double d = -DBL_MAX;
	int nSize = m_layerModel.m_vertModel[index].size();
	for (int i=0; i<nSize; i++)
	{
		if( m_layerModel.m_vertModel[index][i].y > d)
			d = m_layerModel.m_vertModel[index][i].y;
	}
	return d;
}