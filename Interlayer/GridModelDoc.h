#pragma once
#include "GridModel.h"

using namespace  GridModel;
// CGridModelDoc 文档
// 网格模型文件
class CGridModelDoc : public CDocument
{
	DECLARE_DYNCREATE(CGridModelDoc)

public:
	CGridModelDoc();
	virtual ~CGridModelDoc();
#ifndef _WIN32_WCE
	virtual void Serialize(CArchive& ar);   // 为文档 I/O 重写
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	CGridModel m_gridModel;
	BOOL ExportGridModel(LPCTSTR lpszGridFileName); // 导出模型

protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
};
