#if !defined(AFX_COLOURPICKER_H__9C7427F2_1BEA_4D78_9D8A_D45B121B717E__INCLUDED_)
#define AFX_COLOURPICKER_H__9C7427F2_1BEA_4D78_9D8A_D45B121B717E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColourPicker.h : header file
//
#include "ColourPickerPopup.h"

void AFXAPI DDX_ColourPicker(CDataExchange *pDX, int nIDC, COLORREF& crColour);


/////////////////////////////////////////////////////////////////////////////
// CColourPicker window

#define CP_MODE_TEXT 1  // edit text colour
#define CP_MODE_BK   2  // edit background colour (default)

class CColourPicker : public CButton
{
// Construction
public:
    CColourPicker();
	CColourPicker(LPCTSTR szDefaultText, LPCTSTR szCustomText, LPCTSTR szPatternText);
    DECLARE_DYNCREATE(CColourPicker);

// Attributes
public:
    COLORREF GetColour();
    void     SetColour(COLORREF crColour); 

    void     SetDefaultText(LPCTSTR szDefaultText);
    void     SetCustomText(LPCTSTR szCustomText);
	void     SetPatternText(LPCTSTR szPatternText);

    void     SetTrackSelection(BOOL bTracking = TRUE)  { m_bTrackSelection = bTracking; }
    BOOL     GetTrackSelection()                       { return m_bTrackSelection; }

    void     SetSelectionMode(UINT nMode)              { m_nSelectionMode = nMode; }
    UINT     GetSelectionMode()                        { return m_nSelectionMode; };

    void     SetBkColour(COLORREF crColourBk);
    COLORREF GetBkColour()                             { return m_crColourBk; }
    
    void     SetTextColour(COLORREF crColourText);
    COLORREF GetTextColour()                           { return m_crColourText;}

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CColourPicker)
    public:
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    protected:
    virtual void PreSubclassWindow();
    //}}AFX_VIRTUAL

// Implementation
public:
	void SetPatternStyle(PATTERNSTYLE& PatternStyle);
    virtual ~CColourPicker();

protected:
    void SetWindowSize();

// protected attributes
protected:
    BOOL     m_bActive,                // Is the dropdown active?
             m_bTrackSelection;        // track colour changes?
    COLORREF m_crColourBk;
    COLORREF m_crColourText;
    UINT     m_nSelectionMode;
    CRect    m_ArrowRect;
    CString  m_strDefaultText;
    CString  m_strCustomText;
	CString  m_strPatternText;
	PATTERNSTYLE m_PatternStyle;

    // Generated message map functions
protected:
    //{{AFX_MSG(CColourPicker)
    afx_msg BOOL OnClicked();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    //}}AFX_MSG
    afx_msg LONG OnSelEndOK(WPARAM wParam, LPARAM lParam);
    afx_msg LONG OnSelEndCancel(UINT lParam, LONG wParam);
    afx_msg LONG OnSelChange(UINT lParam, LONG wParam);

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLOURPICKER_H__9C7427F2_1BEA_4D78_9D8A_D45B121B717E__INCLUDED_)
