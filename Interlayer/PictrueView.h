#pragma once

#ifdef _WIN32_WCE
#error "Windows CE 不支持 CHtmlView。"
#endif 

// CPictrueView Html 视图
class CPictrueDoc;

class CPictrueView : public CHtmlView
{
	DECLARE_DYNCREATE(CPictrueView)

protected:
	CPictrueView();           // 动态创建所使用的受保护的构造函数
	virtual ~CPictrueView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CPictrueDoc* GetDocument();
	void ShowIt(LPITEMIDLIST pidl);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
};

#ifndef _DEBUG  // debug version in MyEditView.cpp
inline CPictrueDoc* CPictrueView::GetDocument()
{ return (CPictrueDoc*)m_pDocument; }
#endif

