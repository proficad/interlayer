// GridCellMask.h: interface for the CGridCellMask class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRIDCELLMASK_H__4F1344B3_0C7B_49F9_BBC7_CD5B210B214C__INCLUDED_)
#define AFX_GRIDCELLMASK_H__4F1344B3_0C7B_49F9_BBC7_CD5B210B214C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\GRIDCTRL_SRC\GridCell.h"

class _GRID_API CGridCellMask : public CGridCell  
{
	friend class CGridCtrl;
    DECLARE_SERIAL(CGridCellMask)
	//DECLARE_DYNCREATE(CGridCellMask)
public:
	virtual void SetText(LPCTSTR szText);
	virtual void Serialize(CArchive &ar);
	
	CString m_strEditMaskedMask; 
	
	CGridCellMask();
	virtual ~CGridCellMask();

	virtual BOOL Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
    virtual void EndEdit();

};

#endif // !defined(AFX_GRIDCELLMASK_H__4F1344B3_0C7B_49F9_BBC7_CD5B210B214C__INCLUDED_)
