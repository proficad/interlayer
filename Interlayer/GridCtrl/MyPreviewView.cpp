#include "stdafx.h"
#include "GridCtrl_src/gridctrl.h"
#include "MyPreviewView.h"
#include "DummyView.h"

IMPLEMENT_DYNCREATE(CMyPreviewView, CPreviewView)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyPreviewView

CMyPreviewView::CMyPreviewView()
{
	m_pGridCtrl = NULL;
}

CMyPreviewView::~CMyPreviewView()
{
}

BEGIN_MESSAGE_MAP(CMyPreviewView, CPreviewView)
	//{{AFX_MSG_MAP(CMyPreviewView)
	//}}AFX_MSG_MAP
	ON_COMMAND(AFX_ID_PREVIEW_PRINT, OnPreviewPrint)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyPreviewView drawing

/////////////////////////////////////////////////////////////////////////////
// CMyPreviewView diagnostics
#ifdef _DEBUG
void CMyPreviewView::AssertValid() const
{
	// Bug nearby Article ID: Q192853 
	//	CPreviewView::AssertValid();
}

void CMyPreviewView::Dump(CDumpContext& dc) const
{
	CPreviewView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMyPreviewView message handlers

void CMyPreviewView::OnPreviewPrint()
{
	if (m_pGridCtrl != NULL)
	{
		m_pGridCtrl->PostMessage(WM_COMMAND, ID_FILE_PRINT);
	}
	else
	{
		// the view created for the dialog needs to set the m_pDialog pointer
		ASSERT(FALSE);
	}
	OnPreviewClose();               // force close of Preview
}

void CMyPreviewView::SetDialogPointer(CGridCtrl* pGrid)
{
	m_pGridCtrl = pGrid;
}

