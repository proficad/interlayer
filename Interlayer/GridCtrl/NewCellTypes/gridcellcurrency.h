// gridcellcurrency.h: interface for the CGridCellCurrency class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRIDCELLCURRENCY_H__1AA4DF2E_6192_497E_A9C4_A31E10D5EA2F__INCLUDED_)
#define AFX_GRIDCELLCURRENCY_H__1AA4DF2E_6192_497E_A9C4_A31E10D5EA2F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\GRIDCTRL_SRC\GridCell.h"

class _GRID_API CGridCellCurrency : public CGridCell  
{

	friend class CGridCtrl;
	//DECLARE_DYNCREATE(CGridCellCurrency)
	DECLARE_SERIAL(CGridCellCurrency)

public:
	virtual CString GetValidText();
	void SetCellValue(double value);
	virtual double GetTextAsDouble();
	virtual void SetText(LPCTSTR szText);
	virtual void Serialize(CArchive &ar);

	double m_dMoney;
	int m_nDigitsInGroup;
	int m_nMaxDecimalPlaces;
	int m_nMaxWholeDigits;
	BOOL m_bSigned;

	CGridCellCurrency();
	virtual ~CGridCellCurrency();

	virtual BOOL Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar);
    virtual void EndEdit();

};

#endif // !defined(AFX_GRIDCELLCURRENCY_H__1AA4DF2E_6192_497E_A9C4_A31E10D5EA2F__INCLUDED_)
