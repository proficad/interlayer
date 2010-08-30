#if !defined(AFX_COLOURPICKERPOPUP_H__F564D6FF_BDE0_4C65_9EE0_FF279B416E70__INCLUDED_)
#define AFX_COLOURPICKERPOPUP_H__F564D6FF_BDE0_4C65_9EE0_FF279B416E70__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../GeoMap/CommonStruct.h"
// ColourPickerPopup.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColourPickerPopup window

// CColourPickerPopup messages
#define CPN_SELCHANGE        WM_USER + 1001        // Colour Picker Selection change
#define CPN_DROPDOWN         WM_USER + 1002        // Colour Picker drop down
#define CPN_CLOSEUP          WM_USER + 1003        // Colour Picker close up
#define CPN_SELENDOK         WM_USER + 1004        // Colour Picker end OK
#define CPN_SELENDCANCEL     WM_USER + 1005        // Colour Picker end (cancelled)

// forward declaration
class CColourPicker;
class CPatternSheet;

// To hold the colours and their names
typedef struct tagColourTableEntr
{
    COLORREF crColour;
    TCHAR    *szName;
} ColourTableEntry;

/////////////////////////////////////////////////////////////////////////////
// CColourPickerPopup window

class CColourPickerPopup : public CWnd
{

// Construction
public:
    CColourPickerPopup();
    CColourPickerPopup(CPoint p, COLORREF crColour, CWnd* pParentWnd,
                 LPCTSTR szDefaultText = NULL, 
				 LPCTSTR szCustomText = NULL,
				 LPCTSTR szPatternText = NULL,
				 LPPATTERNSTYLE m_pPatternStyle = NULL);
    void Initialise();

// Attributes
public:

// Operations
public:
    BOOL Create(CPoint p, COLORREF crColour, CWnd* pParentWnd, 
                LPCTSTR szDefaultText = NULL,
				LPCTSTR szCustomText = NULL,
				LPCTSTR szPatternText = NULL,
				LPPATTERNSTYLE m_pPatternStyle = NULL);

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CColourPickerPopup)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    //}}AFX_VIRTUAL

// Implementation
public:
	void DrawDownCell(CDC* pDC, int nIndex);
    virtual ~CColourPickerPopup();

protected:
    BOOL GetCellRect(int nIndex, const LPRECT& rect);
    void FindCellFromColour(COLORREF crColour);
    void SetWindowSize();
    void CreateToolTips();
    void ChangeSelection(int nIndex, BOOL bDown = FALSE);
    void EndSelection(int nMessage);
    void DrawCell(CDC* pDC, int nIndex);

    COLORREF GetColour(int nIndex)              { return m_crColours[nIndex].crColour; }
    LPCTSTR GetColourName(int nIndex)           { return m_crColours[nIndex].szName; }
    int  GetIndex(int row, int col) const;
    int  GetRow(int nIndex) const;
    int  GetColumn(int nIndex) const;

// protected attributes
protected:
    static ColourTableEntry m_crColours[];
    int            m_nNumColours;
    int            m_nNumColumns, m_nNumRows;
    int            m_nBoxSize, m_nMargin;
    int            m_nCurrentSel;//当前选择的
    int            m_nChosenColourSel;//已经选择的
	CString        m_strDefaultText;
    CString        m_strCustomText;
	CString        m_strPatternText;
    CRect          m_PatternTextRect, m_CustomTextRect, m_DefaultTextRect, m_WindowRect;
    CFont          m_Font;
    CPalette       m_Palette;
    COLORREF       m_crInitialColour, m_crColour;

	LPPATTERNSTYLE m_pPatternStyle;

    CToolTipCtrl   m_ToolTip;
    CWnd*          m_pParent;
	CColorDialog*  m_pColorDlg;
	CPatternSheet* m_pPropSheet;


    // Generated message map functions
protected:
    //{{AFX_MSG(CColourPickerPopup)
    afx_msg void OnNcDestroy();
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnPaint();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg BOOL OnQueryNewPalette();
    afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnActivateApp(BOOL bActive, DWORD hTask);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLOURPICKERPOPUP_H__F564D6FF_BDE0_4C65_9EE0_FF279B416E70__INCLUDED_)
