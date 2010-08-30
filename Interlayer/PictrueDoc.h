#pragma once

// CPictrueDoc 文档

class CPictrueDoc : public CDocument
{
	DECLARE_DYNCREATE(CPictrueDoc)

public:
	CPictrueDoc();
	virtual ~CPictrueDoc();
#ifndef _WIN32_WCE
	virtual void Serialize(CArchive& ar);   // 为文档 I/O 重写
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	CString m_strHtml;
	CString GenerateHtml(LPCSTR szPicFileName);
	BOOL LoadLongResource(CString &str, UINT nID);
protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
public:
	virtual void SetTitle(LPCTSTR lpszTitle);
};
