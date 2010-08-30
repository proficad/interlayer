// PatternPreviewWnd.h : header file
//
// This file contains the preview window used by the 
// CPatternSheet property sheet.

/////////////////////////////////////////////////////////////////////////////
// CPatternPreviewWnd window

#ifndef __PATTERNPREVIEWWND_H__
#define __PATTERNPREVIEWWND_H__

class CPatternPreviewWnd : public CWnd
{
// Construction
public:
	CPatternPreviewWnd();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPatternPreviewWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	COLORREF m_crColourBack;
	COLORREF m_crColourFore;

	int nCol;
	int nRow;
	virtual ~CPatternPreviewWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPatternPreviewWnd)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif		// __PATTERNPREVIEWWND_H__