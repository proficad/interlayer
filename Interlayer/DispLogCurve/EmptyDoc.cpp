// EmptyDoc.cpp : implementation file
//

#include "stdafx.h"
#include "EmptyDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEmptyDoc

IMPLEMENT_DYNCREATE(CEmptyDoc, CDocument)

CEmptyDoc::CEmptyDoc()
{
}

BOOL CEmptyDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CEmptyDoc::~CEmptyDoc()
{
}


BEGIN_MESSAGE_MAP(CEmptyDoc, CDocument)
	//{{AFX_MSG_MAP(CEmptyDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEmptyDoc diagnostics

#ifdef _DEBUG
void CEmptyDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CEmptyDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEmptyDoc serialization

void CEmptyDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CEmptyDoc commands
