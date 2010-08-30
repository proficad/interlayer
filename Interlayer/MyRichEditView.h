#pragma once


// CMyRichEditView ��ͼ
class CReCntrItem;
class CMyRichEditDoc;

class CMyRichEditView : public CRichEditView
{
protected: // �������л�����
	CMyRichEditView();
	DECLARE_DYNCREATE(CMyRichEditView)

	// ����
public:
	CMyRichEditDoc* GetDocument() const;

	// ����
public:

	// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // ������һ�ε���
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);

	// ʵ��
public:
	virtual ~CMyRichEditView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnDestroy();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // ReView.cpp �еĵ��԰汾
inline CMyRichEditDoc* CMyRichEditView::GetDocument() const
{ return reinterpret_cast<CMyRichEditDoc*>(m_pDocument); }
#endif

