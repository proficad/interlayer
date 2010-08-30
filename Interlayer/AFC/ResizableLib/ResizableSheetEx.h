#if !defined(AFX_RESIZABLESHEETEX_H__INCLUDED_)
#define AFX_RESIZABLESHEETEX_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "ResizableLayout.h"
#include "ResizableGrip.h"
#include "ResizableMinMax.h"
#include "ResizableState.h"

/////////////////////////////////////////////////////////////////////////////
// ResizableSheetEx.h : header file
//

class AFX_EXT_CLASS CResizableSheetEx : public CPropertySheetEx, public CResizableLayout,
						public CResizableGrip, public CResizableMinMax,
						public CResizableState
{
	DECLARE_DYNAMIC(CResizableSheetEx)

// Construction
public:
	CResizableSheetEx();
	CResizableSheetEx(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0,
		HBITMAP hbmWatermark = NULL, HPALETTE hpalWatermark = NULL, HBITMAP hbmHeader = NULL);
	CResizableSheetEx(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0,
		HBITMAP hbmWatermark = NULL, HPALETTE hpalWatermark = NULL, HBITMAP hbmHeader = NULL);

// Attributes
private:
	// flags
	BOOL m_bEnableSaveRestore;
	BOOL m_bRectOnly;
	BOOL m_bSavePage;

	// layout vars
	CSize m_sizePageTL, m_sizePageBR;

	// internal status
	CString m_sSection;			// section name (identifies a parent window)
	BOOL m_bInitDone;			// if all internal vars initialized

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResizableSheetEx)
	public:
	virtual BOOL OnInitDialog();
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CResizableSheetEx();

// used internally
private:
	void PresetLayout();
	void Construct();
	void SavePage();
	void LoadPage();

	BOOL IsWizard() { return (m_psh.dwFlags & PSH_WIZARD); }
	BOOL IsWizard97() { return (m_psh.dwFlags & PSH_WIZARD97); }

// callable from derived classes
protected:
	// section to use in app's profile
	void EnableSaveRestore(LPCTSTR pszSection, BOOL bRectOnly = FALSE,
		BOOL bWithPage = FALSE);
	int GetMinWidth();	// minimum width to display all buttons


	virtual CWnd* GetResizableWnd()
	{
		// make the layout know its parent window
		return this;
	};

// Generated message map functions
protected:
	virtual BOOL ArrangeLayoutCallback(LayoutInfo& layout);
	//{{AFX_MSG(CResizableSheetEx)
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	afx_msg BOOL OnPageChanging(NMHDR* pNotifyStruct, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif	// AFX_RESIZABLESHEETEX_H__INCLUDED_
