// MyRichEditView.cpp : ʵ���ļ�
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
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CRichEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CRichEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMyRichEditView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CMyRichEditView ����/����

CMyRichEditView::CMyRichEditView()
{
	// TODO: �ڴ˴���ӹ������

}

CMyRichEditView::~CMyRichEditView()
{
}

BOOL CMyRichEditView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CRichEditView::PreCreateWindow(cs);
}

void CMyRichEditView::OnInitialUpdate()
{
	CRichEditView::OnInitialUpdate();


	CMyRichEditDoc *pDoc = GetDocument();
	//pDoc->m_bAutoDelete = TRUE;

	// ���ô�ӡ�߾�(720 � = 1/2 Ӣ��)
	SetMargins(CRect(720, 720, 720, 720));

	// 	
	// 	// I WANT TO USE ONLY THIS FONT
	CHARFORMAT2 cf;
	cf.dwMask = CFM_COLOR | CFM_FACE | CFM_BOLD | CFM_SIZE;
	cf.dwEffects = CFM_FACE;
	cf.yHeight = 200;
	cf.crTextColor = RGB(0,0,0);
	cf.bCharSet = GB2312_CHARSET;
	strcpy(cf.szFaceName,"����");	
	GetRichEditCtrl().SetDefaultCharFormat(cf);
	//GetRichEditCtrl().SetSelectionCharFormat (cf);
}


// CMyRichEditView ��ӡ


void CMyRichEditView::OnFilePrintPreview()
{
	AFXPrintPreview(this);
}

BOOL CMyRichEditView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}


void CMyRichEditView::OnDestroy()
{
	// ����ʱͣ�ô������
	// ʹ�ò����ͼʱ�ǳ���Ҫ 
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


// CMyRichEditView ���

#ifdef _DEBUG
void CMyRichEditView::AssertValid() const
{
	CRichEditView::AssertValid();
}

void CMyRichEditView::Dump(CDumpContext& dc) const
{
	CRichEditView::Dump(dc);
}

CMyRichEditDoc* CMyRichEditView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMyRichEditDoc)));
	return (CMyRichEditDoc*)m_pDocument;
}
#endif //_DEBUG


// CMyRichEditView ��Ϣ�������
