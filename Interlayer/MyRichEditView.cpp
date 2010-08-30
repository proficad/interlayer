// MyRichEditView.cpp : 实现文件
//

#include "stdafx.h"
#include "Interlayer.h"
#include "MyRichEditView.h"
#include "MyRichEditDoc.h"
#include "CntrItem.h"

// CMyRichEditView

IMPLEMENT_DYNCREATE(CMyRichEditView, CRichEditView)

BEGIN_MESSAGE_MAP(CMyRichEditView, CRichEditView)
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CRichEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CRichEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMyRichEditView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CMyRichEditView 构造/析构

CMyRichEditView::CMyRichEditView()
{
	// TODO: 在此处添加构造代码

}

CMyRichEditView::~CMyRichEditView()
{
}

BOOL CMyRichEditView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CRichEditView::PreCreateWindow(cs);
}

void CMyRichEditView::OnInitialUpdate()
{
	CRichEditView::OnInitialUpdate();


	CMyRichEditDoc *pDoc = GetDocument();
	//pDoc->m_bAutoDelete = TRUE;

	// 设置打印边距(720 缇 = 1/2 英寸)
	SetMargins(CRect(720, 720, 720, 720));

	// 	
	// 	// I WANT TO USE ONLY THIS FONT
	CHARFORMAT2 cf;
	cf.dwMask = CFM_COLOR | CFM_FACE | CFM_BOLD | CFM_SIZE;
	cf.dwEffects = CFM_FACE;
	cf.yHeight = 200;
	cf.crTextColor = RGB(0,0,0);
	cf.bCharSet = GB2312_CHARSET;
	strcpy(cf.szFaceName,"宋体");	
	GetRichEditCtrl().SetDefaultCharFormat(cf);
	//GetRichEditCtrl().SetSelectionCharFormat (cf);
}


// CMyRichEditView 打印


void CMyRichEditView::OnFilePrintPreview()
{
	AFXPrintPreview(this);
}

BOOL CMyRichEditView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}


void CMyRichEditView::OnDestroy()
{
	// 析构时停用此项；这在
	// 使用拆分视图时非常重要 
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL && pActiveItem->GetActiveView() == this)
	{
		pActiveItem->Deactivate();
		ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
	}
	CRichEditView::OnDestroy();
}


void CMyRichEditView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMyRichEditView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// CMyRichEditView 诊断

#ifdef _DEBUG
void CMyRichEditView::AssertValid() const
{
	CRichEditView::AssertValid();
}

void CMyRichEditView::Dump(CDumpContext& dc) const
{
	CRichEditView::Dump(dc);
}

CMyRichEditDoc* CMyRichEditView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMyRichEditDoc)));
	return (CMyRichEditDoc*)m_pDocument;
}
#endif //_DEBUG


// CMyRichEditView 消息处理程序
