// Vertical_Model\VertModelView.cpp : 实现文件
//

#include "stdafx.h"
#include "VertModelView.h"
#include "VertModelDoc.h"

// CVertModelView

IMPLEMENT_DYNCREATE(CVertModelView, CView)

CVertModelView::CVertModelView()
{
}

CVertModelView::~CVertModelView()
{
}

BEGIN_MESSAGE_MAP(CVertModelView, CView)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CVertModelView 绘图

void CVertModelView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CVertModelView 诊断

#ifdef _DEBUG
void CVertModelView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CVertModelView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG

CVertModelDoc* CVertModelView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVertModelDoc)));
	return (CVertModelDoc*)m_pDocument;
}
// CVertModelView 消息处理程序

int CVertModelView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	CVertModelDoc *pDoc = GetDocument();
	CRect rect;
	GetClientRect(rect);
	pDoc->graph_wnd.Create(_T("Graph Window"), rect, this, 11000);

	return 0;
}

BOOL CVertModelView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return FALSE;
}

void CVertModelView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	CVertModelDoc *pDoc = GetDocument();
	pDoc->graph_wnd.MoveWindow(0, 0, cx, cy);
}

BOOL CVertModelView::OnPreparePrinting(CPrintInfo* pInfo)
{

	// TODO:  调用 DoPreparePrinting 以调用“打印”对话框

	return DoPreparePrinting(pInfo);
}

void CVertModelView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: 在此添加专用代码和/或调用基类

	//CView::OnBeginPrinting(pDC, pInfo);
}

void CVertModelView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: 在此添加专用代码和/或调用基类

	//CView::OnEndPrinting(pDC, pInfo);
}

void CVertModelView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: 在此添加专用代码和/或调用基类

	CVertModelDoc *pDoc = GetDocument();
	CRect rect(100, 100, pDC->GetDeviceCaps(HORZRES) - 100, pDC->GetDeviceCaps(VERTRES) - 100);
	pDoc->graph_wnd.DrawGraphToDC(pDC, rect);
}

void CVertModelView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
}
