#if !defined(AFX_DLGEXPORTPICTURE_H__8D682227_BD04_490D_977E_EE06A363BC35__INCLUDED_)
#define AFX_DLGEXPORTPICTURE_H__8D682227_BD04_490D_977E_EE06A363BC35__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgExportPicture.h : header file
//
#include "../Resource.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgExportPicture dialog

class CDlgExportPicture : public CFileDialog
{
	DECLARE_DYNAMIC(CDlgExportPicture)

public:
	CDlgExportPicture(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);

	// Dialog Data
	//{{AFX_DATA(CDlgExportPicture)
	enum { IDD = IDD_DLG_EXPORT_PICTURE };
	CSliderCtrl	m_wndSilder;
	int		m_iExportType;
	int		m_iQuality;
	//}}AFX_DATA
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgExportPicture)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
protected:
	//{{AFX_MSG(CDlgExportPicture)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnRadioType();
	afx_msg void OnReleasedcaptureSliderQuality(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEXPORTPICTURE_H__8D682227_BD04_490D_977E_EE06A363BC35__INCLUDED_)
