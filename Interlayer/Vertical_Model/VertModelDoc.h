#pragma once
#include "graphwnd.h"

// CVertModelDoc �ĵ�

class CVertModelDoc : public CDocument
{
	DECLARE_DYNCREATE(CVertModelDoc)

public:
	CVertModelDoc();
	virtual ~CVertModelDoc();
#ifndef _WIN32_WCE
	virtual void Serialize(CArchive& ar);   // Ϊ�ĵ� I/O ��д
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
public:
	CGraphWnd graph_wnd;
	
	void SetCurrentGraph(int index)
	{
		graph_wnd.SetCurrentGraph(index);
	}
	void SetCurrentGraph(LPCTSTR lpszGraph)
	{
		graph_wnd.SetCurrentGraph(lpszGraph);
	}
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);
	virtual void SetTitle(LPCTSTR lpszTitle);
	virtual BOOL SaveModified();
};
