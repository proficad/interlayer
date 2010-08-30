// MyEditDoc.cpp : implementation of the CMyEditDoc class
//

#include "stdafx.h"

#include "MyEditDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyEditDoc

IMPLEMENT_DYNCREATE(CMyEditDoc, CDocument)

BEGIN_MESSAGE_MAP(CMyEditDoc, CDocument)
	//{{AFX_MSG_MAP(CMyEditDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyEditDoc construction/destruction

CMyEditDoc::CMyEditDoc()
{
	// TODO: add one-time construction code here

}

CMyEditDoc::~CMyEditDoc()
{
}

BOOL CMyEditDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CMyEditDoc serialization

void CMyEditDoc::Serialize(CArchive& ar)
{
	// CEditView contains an edit control which handles all serialization
	((CEditView*)m_viewList.GetHead())->SerializeRaw(ar);
}

/////////////////////////////////////////////////////////////////////////////
// CMyEditDoc diagnostics

#ifdef _DEBUG
void CMyEditDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMyEditDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMyEditDoc commands
