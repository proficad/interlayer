#pragma once
#include "Resource.h"
class CWellPathInfoDoc;

// CWellPathInfoView ������ͼ

class CWellPathInfoView : public CResizableFormView
{
	DECLARE_DYNCREATE(CWellPathInfoView)

protected:
	CWellPathInfoView();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CWellPathInfoView();

public:
	enum { IDD = IDD_FORM_WELLPATH_INFO };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	CString m_strInfo;

	CGridCtrl m_wndGrid;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CWellPathInfoDoc* GetDocument();
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
public:
	afx_msg void OnGridClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnBtnAddRow();
	afx_msg void OnBtnDelRow();
	afx_msg void OnBtnImport();
	afx_msg void OnBtnExport();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);	
	afx_msg void OnBnClickedBtnLoad();
};