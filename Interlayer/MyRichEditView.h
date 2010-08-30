#pragma once


// CMyRichEditView 视图
class CReCntrItem;
class CMyRichEditDoc;

class CMyRichEditView : public CRichEditView
{
protected: // 仅从序列化创建
	CMyRichEditView();
	DECLARE_DYNCREATE(CMyRichEditView)

	// 属性
public:
	CMyRichEditDoc* GetDocument() const;

	// 操作
public:

	// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);

	// 实现
public:
	virtual ~CMyRichEditView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// 生成的消息映射函数
protected:
	afx_msg void OnDestroy();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // ReView.cpp 中的调试版本
inline CMyRichEditDoc* CMyRichEditView::GetDocument() const
{ return reinterpret_cast<CMyRichEditDoc*>(m_pDocument); }
#endif

