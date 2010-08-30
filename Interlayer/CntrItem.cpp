#include "StdAfx.h"
#include "CntrItem.h"
#include "MyRichEditDoc.h"
#include "MyRichEditView.h"

// CMyRichEditCntrItem 的实现

IMPLEMENT_SERIAL(CMyRichEditCntrItem, CRichEditCntrItem, 0)

CMyRichEditCntrItem::CMyRichEditCntrItem(REOBJECT* preo, CMyRichEditDoc* pContainer)
: CRichEditCntrItem(preo, pContainer)
{
	// TODO: 在此添加一次性构造代码
}

CMyRichEditCntrItem::~CMyRichEditCntrItem()
{
	// TODO: 在此处添加清理代码
}


// CMyRichEditCntrItem 诊断

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

