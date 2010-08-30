#pragma once


// CFormatBar

class CFormatBar : public CMFCToolBar
{
	DECLARE_SERIAL(CFormatBar)

	// Construction
public:
	CFormatBar();

	// Operations
public:

	CMFCColorMenuButton* CreateFontColor ();
	CMFCToolBarFontComboBox* CreateFontComboButton ();

	void CFormatBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	// Attributes
public:
	CSize m_szBaseUnits;

protected:
	CPalette	m_palColorPicker;	// Palette for color picker
	int			m_nNumColours;

	// Implementation
protected:
	virtual void OnReset ();

	// Generated message map functions
	//{{AFX_MSG(CFormatBar)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

