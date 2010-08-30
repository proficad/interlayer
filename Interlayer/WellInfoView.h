#pragma once

class CWellInfoDoc;

// CWellInfoView ������ͼ

class CWellInfoView : public CResizableFormView
{
	DECLARE_DYNCREATE(CWellInfoView)

protected:
	CWellInfoView();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CWellInfoView();

public:
	enum { IDD = IDD_FORM_WELLINFO };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	CString m_strInfo;
	CString	m_strFieldName;

	CGridCtrl m_wndGrid;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CWellInfoDoc* GetDocument();
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
};


