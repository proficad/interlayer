
// ProjectDoc.h : CProjectDoc ��Ľӿ�
//


#pragma once


class CProjectDoc : public CDocument
{
protected: // �������л�����
	CProjectDoc();
	DECLARE_DYNCREATE(CProjectDoc)

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ʵ��
public:
	virtual ~CProjectDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
};


