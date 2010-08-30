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

// C2DModelDoc �ĵ�

class C2DModelDoc : public CDocument
{
	DECLARE_DYNCREATE(C2DModelDoc)

public:
	C2DModelDoc();
	virtual ~C2DModelDoc();
#ifndef _WIN32_WCE
	virtual void Serialize(CArchive& ar);   // Ϊ�ĵ� I/O ��д
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	CSize m_sizeDoc;
	CSize m_sizeGridMap;					// ͼ�δ�С
	CSize GetGridMapSize() const;			// ͼ�δ�С
	CSize m_sizePicture;					// ͼ����С
	CSize GetPictureSize() const;			// ͼ����С
	long	m_lGridMapScale;
	int		m_nCurLayer;					// ��ǰ���

	double LogicToRealY(long y);			// �߼�y��ʵ��Y
	double LogicToRealX(long x);			// �߼�x��ʵ��x
	long RealToLogicY(double y);			// ʵ��y���߼�y
	long RealToLogicX(double x);			// ʵ��X���߼�X

	CGridModel	m_gridModel;				// ����ģ��	

	CTypedPtrArray<CObArray, FIELD_ROW*> m_fieldDat;

	CRuntimeClass *m_pCellDefault;

	double m_dMinValue,m_dMaxValue;			// ������С���ֵ
	double m_dDispMinValue, m_dDispMaxValue;
	COLORREF	m_clrMax;					// ���ֵ��ɫ
	COLORREF	m_clrMin;					// ��Сֵ��ɫ

	CString		m_strModelFileName;			// 

	void CreateGrid(int index);				//
	void DeleteGrid();
	CFieldCell* CreateCell(int nRow, int nCol);
	CFieldCell* GetCell(int nRow, int nCol) const;
	void SetCellValue(int nX, int nY, int nZ, double v);

	std::vector<double> m_values;					// ������
	CArray<tagCellDepth, tagCellDepth&> m_arDepth;	// �������
	void GetPrinterPageSize(CSize &pageSize);
protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
public:
	virtual void DeleteContents();
	virtual void SetTitle(LPCTSTR lpszTitle);
	virtual BOOL SaveModified();
};
