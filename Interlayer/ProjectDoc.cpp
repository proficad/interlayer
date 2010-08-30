
// ProjectDoc.cpp : CProjectDoc 类的实现
//

#include "stdafx.h"
#include "Interlayer.h"

#include "ProjectDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CProjectDoc

IMPLEMENT_DYNCREATE(CProjectDoc, CDocument)

BEGIN_MESSAGE_MAP(CProjectDoc, CDocument)
END_MESSAGE_MAP()


// CProjectDoc 构造/析构

CProjectDoc::CProjectDoc()
{
	// TODO: 在此添加一次性构造代码

}

CProjectDoc::~CProjectDoc()
{
}

BOOL CProjectDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CProjectDoc 序列化

void CProjectDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CProjectDoc 诊断

#ifdef _DEBUG
void CProjectDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CProjectDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

