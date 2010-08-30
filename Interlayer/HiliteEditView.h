#if !defined(AFX_HILITEEDITVIEW_H__B99F1E09_996B_4954_918C_BDC9A2978C88__INCLUDED_)
#define AFX_HILITEEDITVIEW_H__B99F1E09_996B_4954_918C_BDC9A2978C88__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HiliteEditView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHiliteEditView view

class CHiliteEditView : public CEditView
{
protected:
	CHiliteEditView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CHiliteEditView)

// Attributes
public:
	int GetCaretLine() const;			// return current caret line number
	void GetLineRect(int nLine, LPRECT lpRect) const;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHiliteEditView)
	public:
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

	virtual void DrawCaretLine(BOOL bInPaint = FALSE);

// Implementation
protected:
	virtual ~CHiliteEditView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CSize	m_sizeChar;
	CFont	m_fontDefault;
	BOOL	m_bCanPaint;
	int		m_nCaretLine;

	// Generated message map functions
protected:
	//{{AFX_MSG(CHiliteEditView)
	afx_msg void OnPaint();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HILITEEDITVIEW_H__B99F1E09_996B_4954_918C_BDC9A2978C88__INCLUDED_)
