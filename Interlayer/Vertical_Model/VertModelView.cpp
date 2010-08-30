// Vertical_Model\VertModelView.cpp : ʵ���ļ�
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


// CVertModelView ��ͼ

void CVertModelView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: �ڴ���ӻ��ƴ���
}


// CVertModelView ���

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
// CVertModelView ��Ϣ�������

int CVertModelView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������

	CVertModelDoc *pDoc = GetDocument();
	CRect rect;
	GetClientRect(rect);
	pDoc->graph_wnd.Create(_T("Graph Window"), rect, this, 11000);

	return 0;
}

BOOL CVertModelView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	return FALSE;
}

void CVertModelView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
	CVertModelDoc *pDoc = GetDocument();
	pDoc->graph_wnd.MoveWindow(0, 0, cx, cy);
}

BOOL CVertModelView::OnPreparePrinting(CPrintInfo* pInfo)
{

	// TODO:  ���� DoPreparePrinting �Ե��á���ӡ���Ի���

	return DoPreparePrinting(pInfo);
}

void CVertModelView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: �ڴ����ר�ô����/����û���

	//CView::OnBeginPrinting(pDC, pInfo);
}

void CVertModelView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: �ڴ����ר�ô����/����û���

	//CView::OnEndPrinting(pDC, pInfo);
}

void CVertModelView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: �ڴ����ר�ô����/����û���

	CVertModelDoc *pDoc = GetDocument();
	CRect rect(100, 100, pDC->GetDeviceCaps(HORZRES) - 100, pDC->GetDeviceCaps(VERTRES) - 100);
	pDoc->graph_wnd.DrawGraphToDC(pDC, rect);
}

void CVertModelView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: �ڴ����ר�ô����/����û���
}
