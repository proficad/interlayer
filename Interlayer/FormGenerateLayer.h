#pragma once
#include "WndGraph.h"
#include <vector>

// CFormGenerateLayer ������ͼ

class CGenerateLayerDoc;

struct tagNormal
{
	int iSection;		// ����
	int iNormal;		// ��������
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
	CFormGenerateLayer();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
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

	// ��ֵ���
	std::vector<std::vector<CPoint2D>> m_layers;			// ���в���
	std::vector<std::vector<CPoint2D>> m_singleLayers;		// ���в���
	std::vector<int> m_iNormals;							// ÿ����������ķ���
	std::vector<int> m_iHorzSpaces;							// ��������

	std::vector<std::vector<tagNormal>> m_singleNormal;		// �������в��Ӧ�ķ���

	std::vector<std::vector<CPoint2D>> m_vertModel;			// ����ģ��

	std::vector<std::vector<CPoint2D>>	m_normals[2];		// ����
	
	CWndGraph m_wndTop;
	CWndGraph m_wndBottom;
	CWndGraph m_wndHorz;
	CWndGraph m_wndVert;

	double m_dTop;
	double m_dBottom;
	double m_dDepthScale;

	// ��ȡĳ�����ڵķ���
	void GetNormal(const CPoint2D &pt, CPoint2D*pNor);
	// ��ȡ����ĳ�����
	double GetTopPlaneZ(const CPoint2D &pt);
	// ��ȡ����ĳ�����
	double GetBottomPlaneZ(const CPoint2D &pt);

	// �������ģ��
	void AddVertModel(LPCTSTR lpszVertName);

	// ��ȡ����
	void GetNormal(int r, int c, CPoint2D*pNor);

	double GetInterpolatedX(const std::vector<CPoint2D> &points,double y);
	double GetInterpolatedY(const std::vector<CPoint2D> &points,double x);

	void GenerateLayers();								// ���ɼв���
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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


