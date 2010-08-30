#if !defined(AFX_COLORTABLELIST_H__0141A48F_7596_4C11_B52A_67E039A28064__INCLUDED_)
#define AFX_COLORTABLELIST_H__0141A48F_7596_4C11_B52A_67E039A28064__INCLUDED_

#include "ColorLookUpTable.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorTableList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorTableList window

class CColorTableList : public CListBox
{
// Construction
public:
	CColorTableList();

// Attributes
public:

	CColorLookUpTable *m_pColorTable;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorTableList)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColorTableList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorTableList)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORTABLELIST_H__0141A48F_7596_4C11_B52A_67E039A28064__INCLUDED_)
