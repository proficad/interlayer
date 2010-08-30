#pragma once

#include "GridModel.h"
#include "FieldCell.h"
#include "./VFC/InverseDist.h"

using namespace  GridModel;

typedef CTypedPtrArray<CObArray, CFieldCell*> FIELD_ROW;

#define HINT_UPDATE_LAYER		1
#define HINT_UPDATE_SELECTION	2

struct tagCellDepth 
{
public:
	int m_i,m_j;
	double m_dDepth;
	tagCellDepth(){};
	tagCellDepth(const tagCellDepth &varSrc)
	{
		m_i = varSrc.m_i;
		m_j = varSrc.m_j;
		m_dDepth = varSrc.m_dDepth;
	};
	const tagCellDepth& operator=( const tagCellDepth& varSrc )
	{
		m_i = varSrc.m_i;
		m_j = varSrc.m_j;
		m_dDepth = varSrc.m_dDepth;
		return *this;
	};
};

// C2DModelDoc 文档

class C2DModelDoc : public CDocument
{
	DECLARE_DYNCREATE(C2DModelDoc)

public:
	C2DModelDoc();
	virtual ~C2DModelDoc();
#ifndef _WIN32_WCE
	virtual void Serialize(CArchive& ar);   // 为文档 I/O 重写
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	CSize m_sizeDoc;
	CSize m_sizeGridMap;					// 图形大小
	CSize GetGridMapSize() const;			// 图形大小
	CSize m_sizePicture;					// 图幅大小
	CSize GetPictureSize() const;			// 图幅大小
	long	m_lGridMapScale;
	int		m_nCurLayer;					// 当前层号

	double LogicToRealY(long y);			// 逻辑y到实际Y
	double LogicToRealX(long x);			// 逻辑x到实际x
	long RealToLogicY(double y);			// 实际y到逻辑y
	long RealToLogicX(double x);			// 实际X到逻辑X

	CGridModel	m_gridModel;				// 网格模型	

	CTypedPtrArray<CObArray, FIELD_ROW*> m_fieldDat;

	CRuntimeClass *m_pCellDefault;

	double m_dMinValue,m_dMaxValue;			// 数据最小最大值
	double m_dDispMinValue, m_dDispMaxValue;
	COLORREF	m_clrMax;					// 最大值颜色
	COLORREF	m_clrMin;					// 最小值颜色

	CString		m_strModelFileName;			// 

	void CreateGrid(int index);				//
	void DeleteGrid();
	CFieldCell* CreateCell(int nRow, int nCol);
	CFieldCell* GetCell(int nRow, int nCol) const;
	void SetCellValue(int nX, int nY, int nZ, double v);

	std::vector<double> m_values;					// 场数据
	CArray<tagCellDepth, tagCellDepth&> m_arDepth;	// 网格深度
	void GetPrinterPageSize(CSize &pageSize);
protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
public:
	virtual void DeleteContents();
	virtual void SetTitle(LPCTSTR lpszTitle);
	virtual BOOL SaveModified();
};
