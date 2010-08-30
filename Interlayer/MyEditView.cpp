// MyEditView.cpp : implementation of the CMyEditView class
//

#include "stdafx.h"
#include "MyEditDoc.h"
#include "MyEditView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyEditView

IMPLEMENT_DYNCREATE(CMyEditView, CHiliteEditView)

BEGIN_MESSAGE_MAP(CMyEditView, CHiliteEditView)
	//{{AFX_MSG_MAP(CMyEditView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CHiliteEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CHiliteEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CHiliteEditView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyEditView construction/destruction

CMyEditView::CMyEditView()
{
	// TODO: add construction code here

}

CMyEditView::~CMyEditView()
{
}

BOOL CMyEditView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	BOOL bPreCreated = CHiliteEditView::PreCreateWindow(cs);
	cs.style &= ~(ES_AUTOHSCROLL|WS_HSCROLL);	// Enable word-wrapping

	return bPreCreated;
}

/////////////////////////////////////////////////////////////////////////////
// CMyEditView drawing

void CMyEditView::OnDraw(CDC* /*pDC*/)
{
	CMyEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CMyEditView printing

BOOL CMyEditView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default CHiliteEditView preparation
	return CHiliteEditView::OnPreparePrinting(pInfo);
}

void CMyEditView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// Default CHiliteEditView begin printing.
	CHiliteEditView::OnBeginPrinting(pDC, pInfo);
}

void CMyEditView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// Default CHiliteEditView end printing
	CHiliteEditView::OnEndPrinting(pDC, pInfo);
}

/////////////////////////////////////////////////////////////////////////////
// CMyEditView diagnostics

#ifdef _DEBUG
void CMyEditView::AssertValid() const
{
	CHiliteEditView::AssertValid();
}

void CMyEditView::Dump(CDumpContext& dc) const
{
	CHiliteEditView::Dump(dc);
}

CMyEditDoc* CMyEditView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMyEditDoc)));
	return (CMyEditDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMyEditView message handlers
