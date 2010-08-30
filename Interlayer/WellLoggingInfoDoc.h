#pragma once

class CWellLoggingInfoDoc : public CDocument
{
	DECLARE_DYNCREATE(CWellLoggingInfoDoc)

public:
	CWellLoggingInfoDoc();
	virtual ~CWellLoggingInfoDoc();
#ifndef _WIN32_WCE
	virtual void Serialize(CArchive& ar);   // Ϊ�ĵ� I/O ��д
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	int m_nRows;			// ������-1
	int m_nCols;			// ������-1	
	CStringArray m_arWellLoggingInfo;
protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void SetTitle(LPCTSTR lpszTitle);
	virtual BOOL SaveModified();
};
