#pragma once

#define WM_ENDTEXTEDIT WM_USER+301

struct CLogFont : public LOGFONT
{
	CLogFont( LONG fontHeight      = -8, 
		LONG fontWeight      = FW_NORMAL,
		UCHAR pitchAndFamily = DEFAULT_PITCH | FF_DONTCARE,
		LPCSTR faceName      = _T("MS Sans Serif" )
		)
	{
		const int size = sizeof(*this);

		memset( this, 0, size );

		lfHeight         = fontHeight;
		lfWeight         = fontWeight;
		lfPitchAndFamily = pitchAndFamily;

		_ASSERT( LF_FACESIZE > lstrlen( faceName ) );

		lstrcpy( lfFaceName, faceName );
	}

	// Note: No need for CLogFont& operator =(const CLogFont& lf) {...}
	//       We let the compiler handle it...

	void lfFaceNameSet( LPCSTR faceName )
	{
		_ASSERT( faceName );

		if ( !faceName ) return;

		_ASSERT( LF_FACESIZE > lstrlen( faceName ) );

		lstrcpy( lfFaceName, faceName );        
	}

};

class CEditMask
{
public:
	virtual BOOL AddChar( UINT nChar ) = 0;
};

// CEditEx

class CEditEx : public CEdit
{
	// Construction
public:
	CEditEx();

	DECLARE_DYNCREATE( CEditEx )

	// Copy Semantics

	// Block copy construction...
	//
	// No m_hWnd defined for object. 
	//
private:
	CEditEx( const CEditEx& ); 

public:
	// Allow basics to be copied...
	CEditEx& operator = ( const CEditEx& );

	// Attributes
public:

	void setCustomMask( CEditMask* editMask )
	{
		if ( m_pEditMask ) delete m_pEditMask;
		m_pEditMask = editMask;
	} 

	COLORREF bkColor  ( COLORREF );    
	COLORREF textColor( COLORREF );

	COLORREF bkColor()   const { return m_crBkColor;   }   
	COLORREF textColor() const { return m_crTextColor; }

	void setFont( const LOGFONT* lpLogFont );

	void setFont( LONG fontHeight      = -8, 
		LONG fontWeight      = FW_NORMAL,
		UCHAR pitchAndFamily = DEFAULT_PITCH | FF_DONTCARE,
		LPCSTR faceName      = _T("MS Sans Serif" ) );

	void definePopupMenu( UINT uResourceID = 0 /* 0 uses default */ )
	{
		m_MenuResourceID = uResourceID;
	}

	// Operations:
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditEx)
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CEditEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditEx)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()

	// Customize your brush
	//
	virtual BOOL CreateBrushType();

private:
	CBrush   m_brBkGround;
	COLORREF m_crBkColor;
	COLORREF m_crTextColor;

	CEditMask* m_pEditMask;

	CFont* m_pCFont;

	UINT m_MenuResourceID;
};

