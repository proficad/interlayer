// LogTemplPage.h : header file
//

#ifndef __LOGTEMPLPAGE_H__
#define __LOGTEMPLPAGE_H__

#include "SetWidthView.h"
/////////////////////////////////////////////////////////////////////////////
// CLogTemplPage1 dialog

class CGridCtrl;

/////////////////////////////////////////////////////////////////////////////
// CLogTemplPage0 dialog

class CLogTemplPage0 : public CPropertyPage
{
	DECLARE_DYNCREATE(CLogTemplPage0)
		
		// Construction
public:
	CLogTemplPage0();
	~CLogTemplPage0();
	
	// Dialog Data
	//{{AFX_DATA(CLogTemplPage0)
	enum { IDD = IDD_LOGTEMPLPAGE0 };
	CListCtrl	m_list;
	int		m_option;
	CString	m_strPath;
	UINT	m_iFirstRow;
	UINT	m_uTitleRow;
	UINT	m_nDepthCol;
	//}}AFX_DATA
	
	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CLogTemplPage0)
public:
	virtual BOOL OnSetActive();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CLogTemplPage0)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		
};

class CLogTemplPage1 : public CPropertyPage
{
	DECLARE_DYNCREATE(CLogTemplPage1)

// Construction
public:
	CLogTemplPage1();
	~CLogTemplPage1();

// Dialog Data
	//{{AFX_DATA(CLogTemplPage1)
	enum { IDD = IDD_LOGTEMPLPAGE1 };
	int		m_iChar;
	CString	m_char;
	CString	m_path;
//	CString	m_strTitleWidth;
	//}}AFX_DATA

	int m_iTitleRow;		// 标题行
	int m_iDataRow;			// 数据起始行
	CString m_strSeq;		// 分隔符
	CString m_strTitleWidth;// 分隔标题行的宽度
	CGridCtrl m_Grid;
//	CStringArray m_arLoging;
	CArray<int, int> m_arTitleWidth;
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CLogTemplPage1)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CLogTemplPage1)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadio();
	afx_msg void OnChangeEditChar();
	afx_msg void OnBtnApply();
	afx_msg void OnBtnWidth();
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////
// CLogTemplPage2 dialog

class CLogTemplPage2 : public CPropertyPage
{
	DECLARE_DYNCREATE(CLogTemplPage2)

// Construction
public:
	CLogTemplPage2();
	~CLogTemplPage2();

// Dialog Data
	//{{AFX_DATA(CLogTemplPage2)
	enum { IDD = IDD_LOGTEMPLPAGE2 };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


	CGridCtrl m_Grid;
	int m_iTitleRow;		// 标题行
	int m_iDataRow;			// 数据起始行
	CString m_strSeq;		// 分隔符
	CString m_strTitleWidth;// 分隔标题行的宽度
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CLogTemplPage2)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CLogTemplPage2)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif // __LOGTEMPLPAGE_H__
/////////////////////////////////////////////////////////////////////////////
// CLogTemplPage11 dialog

