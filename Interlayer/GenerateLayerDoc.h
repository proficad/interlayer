#pragma once
#include "layermodel.h"

// CGenerateLayerDoc 文档

class CGenerateLayerDoc : public CDocument
{
	DECLARE_DYNCREATE(CGenerateLayerDoc)

public:
	CGenerateLayerDoc();
	virtual ~CGenerateLayerDoc();
#ifndef _WIN32_WCE
	virtual void Serialize(CArchive& ar);   // 为文档 I/O 重写
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
public:
	CLayerModel m_layerModel;
	virtual void SetTitle(LPCTSTR lpszTitle);
	double GetMinX(int index);
	double GetMaxX(int index);
	double GetMinY(int index);
	double GetMaxY(int index);
protected:
	virtual BOOL SaveModified();
};
