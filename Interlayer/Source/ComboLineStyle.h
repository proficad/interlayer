#if !defined(AFX_COMBOLINESTYLE_H__C75A0540_6027_4EFF_A850_9AC0EDF677D1__INCLUDED_)
#define AFX_COMBOLINESTYLE_H__C75A0540_6027_4EFF_A850_9AC0EDF677D1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ComboLineStyle.h : header file
//
struct	SLineStyleAndName
{
	/**/	SLineStyleAndName()
	{
		ZeroMemory( this, sizeof( SLineStyleAndName ) );		// Init Structure
	};
	
	/**/	SLineStyleAndName( int nLineType, PCSTR cpName )
	{
		ZeroMemory( this, sizeof( SLineStyleAndName ) );		// Init Structure
		nLineStyle = nLineType;
		strncpy_s( cLineName, cpName, 255);
	};

	int	nLineStyle;
	char		cLineName[ 256 ];
};

/////////////////////////////////////////////////////////////////////////////
// CComboLineStyle window

class CComboLineStyle : public CComboBox
{
// Construction
public:
	CComboLineStyle();

// Attributes
public:
	bool m_bInitialized;
	static SLineStyleAndName nLineStyle[];


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComboLine)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual ~CComboLineStyle();

	// Generated message map functions
protected:
	//{{AFX_MSG(CComboLine)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	void InitializeData();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMBOLINESTYLE_H__C75A0540_6027_4EFF_A850_9AC0EDF677D1__INCLUDED_)
