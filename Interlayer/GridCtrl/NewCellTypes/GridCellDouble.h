// GridCellDouble.h: interface for the CGridCellDouble class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRIDCELLDOUBLE_H__589521E6_CC26_48DB_9CB9_B478238DCF40__INCLUDED_)
#define AFX_GRIDCELLDOUBLE_H__589521E6_CC26_48DB_9CB9_B478238DCF40__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\GRIDCTRL_SRC\GridCell.h"

class _GRID_API CGridCellDouble : public CGridCell  
{
	friend class CGridCtrl;
    DECLARE_SERIAL(CGridCellDouble)
	//DECLARE_DYNCREATE(CGridCellDouble)
public:
	void SetCellValue(double Value);
	virtual void SetText(LPCTSTR szText);
	double GetTextAsDouble();
	virtual void Serialize(CArchive &ar);
	
	double m_dValue;
	BOOL m_bSigned;	
	int m_nEditNumericLeftDigits;
	int m_nEditNumericRightDigits;
	
	CGridCellDouble();
	virtual ~CGridCellDouble();

	virtual BOOL Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
    virtual void EndEdit();
};

#endif // !defined(AFX_GRIDCELLDOUBLE_H__589521E6_CC26_48DB_9CB9_B478238DCF40__INCLUDED_)
