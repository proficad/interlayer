// GridCellEditFile.h: interface for the CGridCellEditFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRIDCELLEDITFILE_H__114D9E6A_62B7_4A63_A13B_684CE943655D__INCLUDED_)
#define AFX_GRIDCELLEDITFILE_H__114D9E6A_62B7_4A63_A13B_684CE943655D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GRIDCTRL_SRC\GridCell.h"

class _GRID_API CGridCellEditFile : public CGridCell  
{
	friend class CGridCtrl;
	DECLARE_SERIAL(CGridCellEditFile)
		
public:
    CGridCellEditFile();
	
public:
	virtual void Serialize(CArchive &ar);
	
	// Operations
	virtual CSize GetCellExtent(CDC* pDC);
    virtual void OnClick( CPoint PointCellRelative);
	virtual void OnClickDown( CPoint PointCellRelative);
    virtual BOOL GetTextRect( LPRECT pRect);
	
protected:
	CRect GetButtonPlacement();
	
    virtual BOOL Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE);
	
protected:
    CRect m_Rect;
	BOOL m_bDown;
};

#endif // !defined(AFX_GRIDCELLEDITFILE_H__114D9E6A_62B7_4A63_A13B_684CE943655D__INCLUDED_)
