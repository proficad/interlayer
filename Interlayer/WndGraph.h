#pragma once

#include <vector>

#define GVN_DATACHANGED			100			// 数据改变了
#define GVN_NORMALCHANGING		101			// 法线改变了

class FOffScreen;
class CCoordinates;
// CWndGraph
struct tagWGNMHDR
{
	NMHDR	hdr;
	double	m_dDepthScale;
	int		m_iNormal;
};

class CFormGenerateLayer;

class CWndGraph : public CWnd
{
	DECLARE_DYNAMIC(CWndGraph)

public:
	CWndGraph();
	virtual ~CWndGraph();

	FOffScreen *offscreen;
	CCoordinates* CurrentCoordsX;
	CCoordinates* CurrentCoordsY;
	enum WNDTYPE
	{
		TOP_PLANE,		// 顶面
		BOTTOM_PLANE,	// 底面
		HORZ,			// 横向模型
		VERT			// 纵向模型
	};
protected:
	DECLARE_MESSAGE_MAP()
public:

	WNDTYPE m_eType;

	double m_dMinX;
	double m_dMaxX;
	double m_dMinY;
	double m_dMaxY;
	double m_dMinZ;
	double m_dMaxZ;

	BOOL Create(const RECT& rect, CWnd* parent, UINT nID,
		DWORD dwStyle = WS_CHILD | WS_BORDER | WS_TABSTOP | WS_VISIBLE);
	BOOL Initialise();
	void UpdateGraphWindow(LPCRECT rect);
	void InitCoords(double x1, double x2, double y1, double y2);

	double GetMinX();
	double GetMaxX();
	double GetMinY();
	double GetMaxY();
	double GetMinZ();
	double GetMaxZ();
	void GetWorldCoords(double* x1, double* x2, double* y1, double* y2);	

	void SetMinX(double x);
	void SetMaxX(double x);
	void SetMinY(double y);
	void SetMaxY(double y);
	void SetMinZ(double z);
	void SetMaxZ(double z);	
	void SetWorldCoords(double _x1, double _x2, double _y1, double _y2, BOOL bRedraw);

	void SetMinMax(const CPoint2D &pt);
	void AddPoint(int index, const CPoint2D &pt);
	void AddIndex(int index, const int &vertexIndex);
	void AddNormals(int index, const CPoint2D &pt); // 添加法线

	void Init(int idx=-1);

	void AddVertModel(int idx);				// 加纵向模型
	void AddNormals(LPCTSTR lpszFileName);	// 加法线
	void AddLayers(LPCTSTR lpszFileName);	// 加夹层
	void AddPlanes(LPCTSTR lpszFileName, bool bTop);	// 加面
	CFormGenerateLayer *GetParentView();
	LRESULT SendMessageToParent(int nMessage) const;

	int GetDatCount();
	void RefreshCoord();
	
	double m_dDepthScale;								// 横向模型深度位置
	double m_dXYScale;									// 横向模型法线方向比例

	std::vector<int> m_selectVertModel;					// 当前选择的纵向模型

	bool CalcArrowRgn(CPoint *pt, int index);			// 箭头

	void NormalChanged(int index);						// 法线改变了

	int m_iCurVertModel;								// 当前纵向模型

protected:
	BOOL RegisterWindowClass();
	virtual void PreSubclassWindow();
	void DoRedraw(CDC* dc, LPCRECT r);
	void DrawToDC(CDC* dc_to_draw, CRect& rect_to_draw);
	void OnDraw(CDC* dc, CRect& rect_to_draw);
	void DrawAxis(CDC* pDC, CRect& rect_to_draw);
	int GetNMax(CDC* dc_to_draw, CRect rect_to_draw);
	int GetMMax(CDC* dc_to_draw, CRect rect_to_draw);
	int ConvertLogPixToRealPix(CDC* dest_dc, double log_pix_num, BOOL b_x_axis);
	int GetWidth(CDC* dest_dc);

	void DrawPlane(CDC* dc, CRect& rect_to_draw, bool bTop);
	void DrawVertModel(CDC* dc, CRect& rect_to_draw);
	void DrawHorzModel(CDC* dc, CRect& rect_to_draw);

public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
