#ifndef __COLORPALETTE_H__
#define __COLORPALETTE_H__


#ifdef _MAC
#undef AFX_EXT_API
#undef AFX_EXT_CLASS
#define AFX_EXT_API
#define AFX_EXT_CLASS
#endif

#define CBCGPDialogBar CDialogBar

class CColorPalette : public CDialogBar
{
public:
	CColorPalette();
	virtual ~CColorPalette();
	
	//{{AFX_VIRTUAL(CColorPalette)
	//}}AFX_VIRTUAL
	
	COLORREF m_ForeColor;
	COLORREF m_BackColor;
	
protected:
	//{{AFX_MSG(CColorPalette)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg LRESULT OnIdleUpdate(WPARAM wParam, LPARAM lParam);
	
	DECLARE_MESSAGE_MAP()
};

#endif //__COLORPALETTE_H__
