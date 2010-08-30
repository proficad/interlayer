// PictrueView.cpp : 实现文件
//

#include "stdafx.h"
#include "Interlayer.h"
#include "PictrueView.h"
#include "PictrueDoc.h"

// CPictrueView

IMPLEMENT_DYNCREATE(CPictrueView, CHtmlView)

CPictrueView::CPictrueView()
{

}

CPictrueView::~CPictrueView()
{
}

void CPictrueView::DoDataExchange(CDataExchange* pDX)
{
	CHtmlView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPictrueView, CHtmlView)
END_MESSAGE_MAP()


// CPictrueView 诊断

#ifdef _DEBUG
void CPictrueView::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CPictrueView::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}

CPictrueDoc* CPictrueView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPictrueDoc)));
	return (CPictrueDoc*)m_pDocument;
}
#endif //_DEBUG


// CPictrueView 消息处理程序

void CPictrueView::OnInitialUpdate()
{
	CHtmlView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类

	CPictrueDoc * pDoc = GetDocument();
	//pDoc->m_bAutoDelete = TRUE;

	HRESULT hr;

	Navigate2( _T("about:blank"),NULL,NULL,NULL,NULL);

	CComPtr< IDispatch > spDoc(GetHtmlDocument());
	ASSERT( spDoc );

	CComQIPtr< IPersistStreamInit, &IID_IPersistStreamInit > spPSI( spDoc );
	ASSERT(spPSI);

	LPTSTR lpMem = (LPTSTR)::GlobalAlloc( GPTR, pDoc->m_strHtml.GetLength()+1 );
	ASSERT( lpMem );
	::lstrcpy( lpMem, (LPCSTR)pDoc->m_strHtml );

	CComPtr< IStream > spStream;
	::CreateStreamOnHGlobal( lpMem, TRUE, &spStream );
	ASSERT( spStream );

	hr = spPSI->InitNew();
	ASSERT( SUCCEEDED(hr) );

	hr = spPSI->Load( spStream );
	ASSERT( SUCCEEDED(hr) );
}

void CPictrueView::ShowIt(LPITEMIDLIST pidl)
{
	Navigate2(pidl);
}