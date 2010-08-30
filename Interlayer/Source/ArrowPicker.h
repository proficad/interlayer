#if !defined(AFX_ARROWPICKER_H__F2078780_C146_4B2D_9146_375D39506CF9__INCLUDED_)
#define AFX_ARROWPICKER_H__F2078780_C146_4B2D_9146_375D39506CF9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ArrowPicker.h : header file
//
void AFXAPI DDX_ArrowPicker(CDataExchange *pDX, int nIDC, int arrowType);

/////////////////////////////////////////////////////////////////////////////
// CArrowPicker window

class CArrowPicker : public CButton
{
// Construction
public:
    CArrowPicker();
    DECLARE_DYNCREATE(CArrowPicker);

// Attributes
public:
    int GetArrow();
    void     SetArrow(int arrowType); 

    void     SetTrackSelection(BOOL bTracking = TRUE)  { m_bTrackSelection = bTracking; }
    BOOL     GetTrackSelection()                       { return m_bTrackSelection; }

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CArrowPicker)
    public:
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    protected:
    virtual void PreSubclassWindow();
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CArrowPicker();

protected:
    void SetWindowSize();

// protected attributes
protected:
    BOOL     m_bActive,                // Is the dropdown active?
             m_bTrackSelection;        // track colour changes?
    int      m_Arrow;
    CRect    m_ArrowRect;
    CImageList     m_image;

    // Generated message map functions
protected:
    //{{AFX_MSG(CArrowPicker)
    afx_msg BOOL OnClicked();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    //}}AFX_MSG
    afx_msg LONG OnSelEndOK(UINT lParam, LONG wParam);
    afx_msg LONG OnSelEndCancel(UINT lParam, LONG wParam);
    afx_msg LONG OnSelChange(UINT lParam, LONG wParam);

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ARROWPICKER_H__F2078780_C146_4B2D_9146_375D39506CF9__INCLUDED_)
