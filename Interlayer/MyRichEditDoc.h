#pragma once

// CMyRichEditDoc �ĵ�
class CMyRichEditDoc : public CRichEditDoc
{
protected: // �������л�����
	CMyRichEditDoc();
	DECLARE_DYNCREATE(CMyRichEditDoc)

	// ����
public:

	// ����
public:

	// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual CRichEditCntrItem* CreateClientItem(REOBJECT* preo) const;

	// ʵ��
public:
	virtual ~CMyRichEditDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	virtual void SetTitle(LPCTSTR lpszTitle);
	virtual BOOL SaveModified();

protected:

	// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
};
