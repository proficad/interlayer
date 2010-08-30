#pragma once

class CGeoMapDoc;
class CGeoMapView;

class CDrawLayer;
class CDrawPage;

#define HINT_UPDATE_WINDOW			0	// 刷新窗口
#define HINT_UPDATE_DRAWOBJ			1   // 刷新图素
#define HINT_UPDATE_SELECTION		2	// 刷新所有选择的图素
#define HINT_DELETE_SELECTION		3	// 移去所有选中图素
#define HINT_UPDATE_OLE_ITEMS		4	// 刷新OLE对象
#define HINT_UPDATE_LAYERBAR		5	// 刷新图层工具栏
#define HINT_UPDATE_PAGE			6	// 改变当前页面

class CGeoDrawObj;

typedef CTypedRefObList<CGeoDrawObj*> CGeoDrawObjList;

extern const char* newGUID();

class CGeoDrawObj : public CRefObject  
{
	DECLARE_SERIAL(CGeoDrawObj);
public:
	CGeoDrawObj();
	CGeoDrawObj( CDrawLayer* pLayer);
	CGeoDrawObj(const CRect& position,  CDrawLayer* pLayer);
	CGeoDrawObj(const CRectEx& position,  CDrawLayer* pLayer);

	virtual ~CGeoDrawObj();
public:
	enum TrackerState { normal, selected, active };
	// 保存属性
	CRect		m_rtPosition;			// 图素的位置
	CRectEx		m_geoPosition;			// 大地坐标范围
	BOOL		m_bLocked;				// 锁定图素
	BOOL		m_bShow;				// 是否显示
	CString		m_strID;				// 图素ID
	CArray<POINT,POINT> m_ptAnchors;	// 锚点
	CPoint2D	m_geoCenterPoint;		// 图素中心大地坐标

	// 不保存属性
	BOOL		m_bMoving;			// 是否正在移动	
	BOOL		m_bEditing;			// 是否正在编辑
	BOOL		m_bSizeChanging;	// 是否正在调整大小
	UINT		m_uMenuID;			// 弹出菜单ID
	CString		m_strObjName;		// 图素名称
	TrackerState m_trackerState;	// 

	CGeoDrawObj	*m_pSelectedObj;	// 已经选中的子图素

	// 所属对象
	CGeoMapDoc *m_pDocument;		// 所属文档
	CDrawLayer *m_pParentLayer;		// 所属图层
	CDrawPage  *m_pParentPage;		// 所属画页 	
	CGeoDrawObj *m_pParentObj;		// 所属对象

	CGeoDrawObjList m_DrawObjects;	// 所包含的对象列表

public:

	// 图素排序
	virtual BOOL MoveBack(CGeoDrawObj *pObj);					
	virtual BOOL MoveForward(CGeoDrawObj *pObj);
	virtual BOOL MoveToBack(CGeoDrawObj *pObj);
	virtual BOOL MoveToFront(CGeoDrawObj *pObj);

	virtual BOOL Add(CGeoDrawObj *pObj);						// 增加图素
	virtual BOOL Remove(CGeoDrawObj *pOb);						// 删除图素
	virtual void RemoveAll();
	// 拷贝图素属性
	virtual void CopyAttribute(CGeoDrawObj *pSrcObj);
	virtual BOOL IsHitOnBorder(CPoint point, CGeoMapView* pView);
	virtual BOOL Intersects(const CRect& rect);
	virtual int HitTestHandle(CPoint point, CGeoMapView* pView, BOOL bSelected);
	virtual long HitTestEdit(CPoint point, CGeoMapView* pView);
	virtual CRect GetHandleRect(int nHandleID, CGeoMapView* pView = NULL);
	virtual HCURSOR GetHandleCursor(int nHandle);
	virtual CPoint GetHandle(int nHandle);
	virtual int GetHandleCount();
	virtual BOOL RecalcBounds(CGeoMapView* pView = NULL);
	virtual CGeoDrawObj* ObjectAt(const CPoint& point, CGeoMapView *pView); 
	// 图素区域四角坐标
	virtual CPoint GetCorner(int nHandle);
	// 区域四角
	virtual CRect GetCornerRect(int nHandleID, CGeoMapView* pView = NULL);

	virtual void Invalidate(CGeoMapView* pView = NULL);
	virtual void Serialize(CArchive& ar);
	virtual CGeoDrawObj* Clone(CDrawLayer *pLayer);
	virtual BOOL OnEditProperties(CGeoMapView* pView = NULL);

	virtual int GetPathPoint(LPPOINT &lppt);
	virtual CPoint GetNearByIndex(const CPoint &point, int *iIndex);

	// 移动图素大小
	virtual void MoveHandleTo(int nHandle, CPoint point, CGeoMapView* pView = NULL);
	// 移动图素位置
	virtual void MoveTo(const CRect& position, CGeoMapView* pView = NULL);
	// 画图素状态
	virtual void DrawTracker(CDC* pDC, CGeoMapView *pView);
	// 画图素
	virtual void Draw(CDC* pDC, CGeoMapView* pView = NULL);
	// 画编辑状态
	virtual void DrawEditMode(CGeoMapView* pView);
	// 画移动状态
	virtual void DrawMoveState(CGeoMapView* pView);
	// 画改变大小状态
	virtual void DrawSizeState(CGeoMapView* pView);
	// 当比例尺发生改变时
	virtual void OnScaleChanged();
	// 当发生旋转时
	virtual void OnAngleChanged();
	// 当发生坐标原点有改变时
	virtual void OnCoordOrigChanged();
	// 转换参数改变时
	virtual void OnConvertParamChanged(){};

	// 逆序点集
	virtual void ReversePoints(){}

	// 获取所在的图层
	CDrawLayer* GetParentLayer() { return m_pParentLayer;}

	// 如果是大地坐标系，则转换成大地坐标(geodetic coordinates)
	// 如果是数据坐标系，则为图形中心坐标
	virtual CPoint2D CoordinateTransfer(const CPoint &pt);
	virtual CPoint CoordinateTransfer(const CPoint2D &pt);
	virtual CRectEx CoordinateTransfer(const CRect &rect);
	virtual CRect CoordinateTransfer(const CRectEx &rect);

	virtual UINT GetMenuID();
	virtual void SetObjName(CString strObjName);

	// 认领孩子
	virtual void ClaimChildren();
};