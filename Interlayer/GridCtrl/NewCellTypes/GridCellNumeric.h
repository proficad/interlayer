// GridCellNumeric.h: interface for the CGridCellNumeric class.
//
// Written by Andrew Truckle [ajtruckle@wsatkins.co.uk]
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRIDINTEGERCELL_H__3479ED0D_B57D_4940_B83D_9E2296ED75B5__INCLUDED_)
#define AFX_GRIDINTEGERCELL_H__3479ED0D_B57D_4940_B83D_9E2296ED75B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../GridCtrl_src/GridCell.h"

class _GRID_API CGridCellNumeric : public CGridCell  
{
	friend class CGridCtrl;
    DECLARE_SERIAL(CGridCellNumeric)
    //DECLARE_DYNCREATE(CGridCellNumeric)
public:
	virtual void Serialize(CArchive &ar);
	void SetCellLong(long value);
	virtual void SetText(LPCTSTR szText);
	virtual long GetTextAsLong();
	CGridCellNumeric();
	virtual ~CGridCellNumeric();
    virtual BOOL Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
    virtual void EndEdit();

	long m_nValue;
	bool m_bSigned;
};

#endif // !defined(AFX_GRIDINTEGERCELL_H__3479ED0D_B57D_4940_B83D_9E2296ED75B5__INCLUDED_)
