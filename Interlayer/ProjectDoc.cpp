
// ProjectDoc.cpp : CProjectDoc ���ʵ��
//

#include "stdafx.h"
#include "Interlayer.h"

#include "ProjectDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CProjectDoc

IMPLEMENT_DYNCREATE(CProjectDoc, CDocument)

BEGIN_MESSAGE_MAP(CProjectDoc, CDocument)
END_MESSAGE_MAP()


// CProjectDoc ����/����

CProjectDoc::CProjectDoc()
{
	// TODO: �ڴ����һ���Թ������

}

CProjectDoc::~CProjectDoc()
{
}

BOOL CProjectDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}




// CProjectDoc ���л�

void CProjectDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}


// CProjectDoc ���

#ifdef _DEBUG
void CProjectDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CProjectDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

