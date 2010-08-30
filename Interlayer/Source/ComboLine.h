#if !defined(AFX_COMBOLINE_H__8E5B9B16_51BB_11D5_B4E7_001060FC881A__INCLUDED_)
#define AFX_COMBOLINE_H__8E5B9B16_51BB_11D5_B4E7_001060FC881A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ComboLine.h : header file
//
struct	SPenAndName
{
	/**/	SPenAndName()
	{
		ZeroMemory( this, sizeof( SPenAndName ) );		// Init Structure
	};
	
	/**/	SPenAndName( int nPenType, PCSTR cpName )
	{
		ZeroMemory( this, sizeof( SPenAndName ) );		// Init Structure
		nPenStyle = nPenType;
		strcpy_s( cPenName, 255, cpName);
	};

	int	nPenStyle;
	char		cPenName[ 256 ];
};


/////////////////////////////////////////////////////////////////////////////
// CComboLine window

class CComboLine : public CComboBox
{
// Construction
public:
	CComboLine();
	static SPenAndName nPenStyle[];
// Attributes
private:
	bool m_bInitialized;

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
	virtual ~CComboLine();

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

#endif // !defined(AFX_COMBOLINE_H__8E5B9B16_51BB_11D5_B4E7_001060FC881A__INCLUDED_)
