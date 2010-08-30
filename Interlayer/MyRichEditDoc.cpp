// MyRichEditDoc.cpp : 实现文件
//

#include "stdafx.h"
#include "Interlayer.h"
#include "MyRichEditDoc.h"
#include "CntrItem.h"
#include "MainFrm.h"
// CMyRichEditDoc

IMPLEMENT_DYNCREATE(CMyRichEditDoc, CRichEditDoc)

BEGIN_MESSAGE_MAP(CMyRichEditDoc, CRichEditDoc)
	// 启用默认的 OLE 容器实现
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_LINKS, &CRichEditDoc::OnUpdateEditLinksMenu)
	ON_COMMAND(ID_OLE_EDIT_LINKS, &CRichEditDoc::OnEditLinks)
	ON_UPDATE_COMMAND_UI_RANGE(ID_OLE_VERB_FIRST, ID_OLE_VERB_LAST, &CRichEditDoc::OnUpdateObjectVerbMenu)
END_MESSAGE_MAP()


// CMyRichEditDoc 构造/析构

CMyRichEditDoc::CMyRichEditDoc()
{
	// TODO: 在此添加一次性构造代码
	m_bRTF = FALSE;
}

CMyRichEditDoc::~CMyRichEditDoc()
{
}

BOOL CMyRichEditDoc::OnNewDocument()
{
	if (!CRichEditDoc::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}

CRichEditCntrItem* CMyRichEditDoc::CreateClientItem(REOBJECT* preo) const
{
	return new CMyRichEditCntrItem(preo, const_cast<CMyRichEditDoc*>(this));
}




// CMyRichEditDoc 序列化

void CMyRichEditDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}

	// 调用基类 CRichEditDoc 将启用
	//  容器文档的 COleClientItem 对象的序列化。
	// TODO: 如果作为文本进行序列化，则设置 CRichEditDoc::m_bRTF = FALSE
	CRichEditDoc::Serialize(ar);
}


// CMyRichEditDoc 诊断

#ifdef _DEBUG
void CMyRichEditDoc::AssertValid() const
{
	CRichEditDoc::AssertValid();
}

void CMyRichEditDoc::Dump(CDumpContext& dc) const
{
	CRichEditDoc::Dump(dc);
}
#endif //_DEBUG


// CMyRichEditDoc 命令

void CMyRichEditDoc::SetTitle(LPCTSTR lpszTitle)
{
	// TODO: 在此添加专用代码和/或调用基类

	CString strFileName = GetPathName();
	strFileName.MakeUpper();

	CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();

	CString strPath = pFrame->GetProjectDatPath();
	strPath += _T("\\files\\");
	strPath.MakeUpper();

	strFileName.Replace(strPath, _T(""));

	HTREEITEM hItem = pFrame->GetTreeFileView()->GetTreeCtrl()->IsExistFileInTree(TVI_ROOT, strFileName);

	CString strTitle = pFrame->GetTreeFileView()->GetTreeCtrl()->GetItemText(hItem);

	m_strTitle = strTitle;

	UpdateFrameCounts();
}

BOOL CMyRichEditDoc::SaveModified()
{
	// TODO: 在此添加专用代码和/或调用基类

	if (!IsModified())
		return TRUE;        // ok to continue

	// get name/title of document
	CString name = _T(" [");
	name += m_strTitle;
	name += _T("] ");

	CString prompt;
	AfxFormatString1(prompt, AFX_IDP_ASK_TO_SAVE, name);
	switch (AfxMessageBox(prompt, MB_YESNOCANCEL, AFX_IDP_ASK_TO_SAVE))
	{
	case IDCANCEL:
		return FALSE;       // don't continue

	case IDYES:
		// If so, either Save or Update, as appropriate
		if (!DoFileSave())
			return FALSE;       // don't continue
		break;

	case IDNO:
		// If not saving changes, revert the document
		break;

	default:
		ASSERT(FALSE);
		break;
	}
	return TRUE;    // keep going
}