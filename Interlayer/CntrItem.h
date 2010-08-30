#pragma once

class CMyRichEditDoc;
class CMyRichEditView;

class CMyRichEditCntrItem : public CRichEditCntrItem
{
	DECLARE_SERIAL(CMyRichEditCntrItem)

	// ���캯��
public:
	CMyRichEditCntrItem(REOBJECT* preo = NULL, CMyRichEditDoc* pContainer = NULL);
	// ע��: ���� pContainer Ϊ NULL ������ IMPLEMENT_SERIALIZE
	//  IMPLEMENT_SERIALIZE Ҫ������д���
	//  �����Ĺ��캯����OLE ��ͨ������
	//  �� NULL �ĵ�ָ�빹���

	// ����
public:
	CMyRichEditDoc* GetDocument()
	{ return reinterpret_cast<CMyRichEditDoc*>(CRichEditCntrItem::GetDocument()); }
	CMyRichEditView* GetActiveView()
	{ return reinterpret_cast<CMyRichEditView*>(CRichEditCntrItem::GetActiveView()); }

	// ʵ��
public:
	~CMyRichEditCntrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

