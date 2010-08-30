#pragma once
#include "WndGraph.h"
#include <vector>

// CFormGenerateLayer 窗体视图

class CGenerateLayerDoc;

struct tagNormal
{
	int iSection;		// 区间
	int iNormal;		// 法线索引
	const tagNormal& operator=(const tagNormal& Src)
	{
		iSection = Src.iSection;
		iNormal = Src.iNormal;
		return *this;
	}
};

class CFormGenerateLayer : public CResizableFormView
{
	DECLARE_DYNCREATE(CFormGenerateLayer)

protected:
	CFormGenerateLayer();           // 动态创建所使用的受保护的构造函数
	virtual ~CFormGenerateLayer();

public:
	enum { IDD = IDD_FROM_GENERATE_LAYER };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif	
public:
	CGenerateLayerDoc * GetDocument() const;

	// 插值后的
	std::vector<std::vector<CPoint2D>> m_layers;			// 复夹层线
	std::vector<std::vector<CPoint2D>> m_singleLayers;		// 单夹层线
	std::vector<int> m_iNormals;							// 每个法线区间的份数
	std::vector<int> m_iHorzSpaces;							// 横向间隔数

	std::vector<std::vector<tagNormal>> m_singleNormal;		// 第条单夹层对应的法线

	std::vector<std::vector<CPoint2D>> m_vertModel;			// 纵向模型

	std::vector<std::vector<CPoint2D>>	m_normals[2];		// 法线
	
	CWndGraph m_wndTop;
	CWndGraph m_wndBottom;
	CWndGraph m_wndHorz;
	CWndGraph m_wndVert;

	double m_dTop;
	double m_dBottom;
	double m_dDepthScale;

	// 获取某点所在的法线
	void GetNormal(const CPoint2D &pt, CPoint2D*pNor);
	// 获取顶面某点深度
	double GetTopPlaneZ(const CPoint2D &pt);
	// 获取底面某点深度
	double GetBottomPlaneZ(const CPoint2D &pt);

	// 添加纵向模型
	void AddVertModel(LPCTSTR lpszVertName);

	// 获取法线
	void GetNormal(int r, int c, CPoint2D*pNor);

	double GetInterpolatedX(const std::vector<CPoint2D> &points,double y);
	double GetInterpolatedY(const std::vector<CPoint2D> &points,double x);

	void GenerateLayers();								// 生成夹层线
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDataChanged(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnNormalChanged(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnBnClickedBtnImportTop();
	afx_msg void OnBnClickedBtnDelTop();
	afx_msg void OnBnClickedBtnImportBottom();
	afx_msg void OnBnClickedBtnDelBottom();
	afx_msg void OnBnClickedBtnImportHorz();
	afx_msg void OnBnClickedBtnDelHorz();
	afx_msg void OnBnClickedBtnImportVert();
	afx_msg void OnBnClickedBtnDelVert();
	afx_msg void OnEnKillfocus();	
	afx_msg void OnBnClickedBtnImportNormal();
	afx_msg void OnBnClickedBtnGenerateLayer();
	afx_msg void OnEnKillfocusEditMiddle();
	CString m_strAngle;
};


