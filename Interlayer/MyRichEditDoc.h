#pragma once

// CMyRichEditDoc 文档
class CMyRichEditDoc : public CRichEditDoc
{
protected: // 仅从序列化创建
	CMyRichEditDoc();
	DECLARE_DYNCREATE(CMyRichEditDoc)

	// 属性
public:

	// 操作
public:

	// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual CRichEditCntrItem* CreateClientItem(REOBJECT* preo) const;

	// 实现
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

	// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
};
