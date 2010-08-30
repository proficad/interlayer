#pragma once
#include "GridModel.h"

using namespace  GridModel;
// CGridModelDoc �ĵ�
// ����ģ���ļ�
class CGridModelDoc : public CDocument
{
	DECLARE_DYNCREATE(CGridModelDoc)

public:
	CGridModelDoc();
	virtual ~CGridModelDoc();
#ifndef _WIN32_WCE
	virtual void Serialize(CArchive& ar);   // Ϊ�ĵ� I/O ��д
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	CGridModel m_gridModel;
	BOOL ExportGridModel(LPCTSTR lpszGridFileName); // ����ģ��

protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
};
