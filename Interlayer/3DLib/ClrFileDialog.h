#if !defined(AFX_CLRFILEDIALOG_H__4C878472_1D0E_4FFB_BAFA_28DB85ABF60D__INCLUDED_)
#define AFX_CLRFILEDIALOG_H__4C878472_1D0E_4FFB_BAFA_28DB85ABF60D__INCLUDED_

#include "ColorLookUpTable.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ClrFileDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CClrFileDialog dialog

class CClrFileDialog : public CFileDialog
{
	DECLARE_DYNAMIC(CClrFileDialog)

public:
	CColorLookUpTable m_ColorTable;
	CClrFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);

	void DrawColorSpectrum();

protected:
	virtual void OnFileNameChange();
protected:
	//{{AFX_MSG(CClrFileDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLRFILEDIALOG_H__4C878472_1D0E_4FFB_BAFA_28DB85ABF60D__INCLUDED_)
