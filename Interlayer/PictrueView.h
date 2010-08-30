#pragma once

#ifdef _WIN32_WCE
#error "Windows CE ��֧�� CHtmlView��"
#endif 

// CPictrueView Html ��ͼ
class CPictrueDoc;

class CPictrueView : public CHtmlView
{
	DECLARE_DYNCREATE(CPictrueView)

protected:
	CPictrueView();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CPictrueView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CPictrueDoc* GetDocument();
	void ShowIt(LPITEMIDLIST pidl);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
};

#ifndef _DEBUG  // debug version in MyEditView.cpp
inline CPictrueDoc* CPictrueView::GetDocument()
{ return (CPictrueDoc*)m_pDocument; }
#endif

