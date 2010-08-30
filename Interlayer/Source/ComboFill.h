#if !defined(AFX_COMBOFILL_H__E5DDF961_51C3_11D5_B4E7_001060FC881A__INCLUDED_)
#define AFX_COMBOFILL_H__E5DDF961_51C3_11D5_B4E7_001060FC881A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ComboFill.h : header file
//

struct	SFillAndName
{
	/**/	SFillAndName()
	{
		ZeroMemory( this, sizeof( SFillAndName ) );		// Init Structure
	};
	
	/**/	SFillAndName( int nFillType, PCSTR cpName )
	{
		ZeroMemory( this, sizeof( SFillAndName ) );		// Init Structure
		nFillStyle = nFillType;
		strcpy_s( cFillName, 255, cpName);
	};

	int	nFillStyle;
	char		cFillName[ 256 ];
};

/////////////////////////////////////////////////////////////////////////////
// CComboFill window

class CComboFill : public CComboBox
{
// Construction
public:
	CComboFill();

// Attributes
private:
	bool m_bInitialized;
	static SFillAndName nFillStyle[];


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComboFill)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual ~CComboFill();

	// Generated message map functions
protected:
	//{{AFX_MSG(CComboFill)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	void InitializeData();

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMBOFILL_H__E5DDF961_51C3_11D5_B4E7_001060FC881A__INCLUDED_)
