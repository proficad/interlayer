#pragma once

class CMyRichEditDoc;
class CMyRichEditView;

class CMyRichEditCntrItem : public CRichEditCntrItem
{
	DECLARE_SERIAL(CMyRichEditCntrItem)

	// 构造函数
public:
	CMyRichEditCntrItem(REOBJECT* preo = NULL, CMyRichEditDoc* pContainer = NULL);
	// 注意: 允许 pContainer 为 NULL 以启用 IMPLEMENT_SERIALIZE
	//  IMPLEMENT_SERIALIZE 要求类具有带零
	//  参数的构造函数。OLE 项通常是用
	//  非 NULL 文档指针构造的

	// 属性
public:
	CMyRichEditDoc* GetDocument()
	{ return reinterpret_cast<CMyRichEditDoc*>(CRichEditCntrItem::GetDocument()); }
	CMyRichEditView* GetActiveView()
	{ return reinterpret_cast<CMyRichEditView*>(CRichEditCntrItem::GetActiveView()); }

	// 实现
public:
	~CMyRichEditCntrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

