#include "StdAfx.h"
#include "CntrItem.h"
#include "MyRichEditDoc.h"
#include "MyRichEditView.h"

// CMyRichEditCntrItem ��ʵ��

IMPLEMENT_SERIAL(CMyRichEditCntrItem, CRichEditCntrItem, 0)

CMyRichEditCntrItem::CMyRichEditCntrItem(REOBJECT* preo, CMyRichEditDoc* pContainer)
: CRichEditCntrItem(preo, pContainer)
{
	// TODO: �ڴ����һ���Թ������
}

CMyRichEditCntrItem::~CMyRichEditCntrItem()
{
	// TODO: �ڴ˴�����������
}


// CMyRichEditCntrItem ���

#ifdef _DEBUG
void CMyRichEditCntrItem::AssertValid() const
{
	CRichEditCntrItem::AssertValid();
}

void CMyRichEditCntrItem::Dump(CDumpContext& dc) const
{
	CRichEditCntrItem::Dump(dc);
}
#endif

