#pragma once
#include "GLDisplayContext.h"
// C3DModelDoc 文档

class C3DModelDoc : public CDocument
{
	DECLARE_DYNCREATE(C3DModelDoc)

public:
	C3DModelDoc();
	virtual ~C3DModelDoc();
#ifndef _WIN32_WCE
	virtual void Serialize(CArchive& ar);   // 为文档 I/O 重写
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	CGLDisplayContext* GetContext() const
	{
		return m_pDisplayContext;
	}
	void ImportFile(LPCTSTR lpszFileName);
	void AddPlane(LPCTSTR lpszFileName, LPCTSTR lpszName);
	void AddGridModel(LPCTSTR lpszFileName, LPCTSTR lpszName, const CStringArray &arParamName, const CStringArray &arFileName, CString guid);
	void AddPhyParam(LPCTSTR lpszFileName, LPCTSTR lpszName);
	void AddWellPath(HTREEITEM hItem);
	void AddInterlayer(LPCTSTR lpszFileName, LPCTSTR lpszName,CString guid, CString modelguid);
private:
	CGLDisplayContext* m_pDisplayContext;		// 显示场景

protected:
	virtual BOOL OnNewDocument();

	afx_msg void On3DObjErase();
	afx_msg void On3DObjDelete();
	afx_msg void OnScreenDisplayall();
	DECLARE_MESSAGE_MAP()
public:
	virtual void SetTitle(LPCTSTR lpszTitle);
	virtual BOOL SaveModified();
};
