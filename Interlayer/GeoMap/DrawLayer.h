#pragma once

#include "GeoDrawObj.h"
#include "RgnClipping.h"
class CGeoMapDoc;
class CGeoMapView;

class CDrawLayer;

typedef CTypedRefObList<CDrawLayer*> CDrawLayerList;

class CDrawPage;

class CDrawLayer : public CRefObject  
{
public:
	CDrawLayer();
	CDrawLayer(CGeoMapDoc *pDoc);
	virtual ~CDrawLayer();

	enum GeoLayerType		// 图层类型
	{
		generic = 0,		// 普通层
		wellmap,			// 井位图层
		basepictrue,		// 底图
		interlayer,			// 侧积夹层
		faultage,			// 断层图层
		faultage_up,		// 断层上盘图层
		faultage_down,		// 断层上盘图层
		SediFacies,			// 沉积相图层 Sedimentary Facies
		contour,			// 等值线图
	};
public:
	// 属性	
	// 保存
	BOOL	m_bShow;				// 是否显示 TURE-显示 FALSE-隐藏
	BOOL	m_bLocked;				// 是否锁定 TRUE-是  FALSE-否
	CString	m_strLayerName;			// 图层名称
	CString m_strID;				// 图层ID
	CGeoDrawObjList m_DrawObjects;	// 管理图素的对象列表
	CRgnClipping	m_rgnClipping;	// 剪切区域
	BOOL	m_bClipping;			// 是否只显示剪切区域的图素	
	BOOL	m_bIsNew;				// 是否为新层
	BOOL	m_bIsModified;			// 是否已经修改过

	GeoLayerType m_eLayerType;		// 图层类型
	int		m_iTransparence;		// 透明度 (全透明~不透明 0~255)

	//////////////////////////////////////////////////////////////////////////
	CGeoMapDoc *m_pDocument;		// 当前文档
	CDrawPage *m_pParentPage;		// 所属页面
	CDrawPage *GetParentPage() { return m_pParentPage;}

	// 图层大小范围
	CRect GetLayerRect();

	//////////////////////////////////////////////////////////////////////////
	int GetCountObj() { return m_DrawObjects.GetCount();} // 图素总数
	CGeoDrawObj *GetHeadObj(){return m_DrawObjects.GetHead();}
	CGeoDrawObj *GetTailObj(){return m_DrawObjects.GetTail();}
	CGeoDrawObj *GetAtObj(POSITION pos){return m_DrawObjects.GetAt(pos);}
	POSITION GetHeadPosition(){return m_DrawObjects.GetHeadPosition();}
	CGeoDrawObj *GetNextObj(POSITION &pos){ return m_DrawObjects.GetNext(pos);}
public:
	//操作
	BOOL Add(CGeoDrawObj *pObj);						// 增加图素
	virtual void Draw(CDC* pDC, CGeoMapView *pView);	// 显示图层
	virtual void DrawTracker(CDC* pDC, CGeoMapView *pView);	// 显示图层
	virtual void Serialize(CArchive& ar);				// 图层串行化
	BOOL Remove(CGeoDrawObj *pObj);						// 删除图素
	void RemoveAll();									// 删除所有图素	

	// 图素排序
	BOOL MoveBack(CGeoDrawObj *pObj);					
	BOOL MoveForward(CGeoDrawObj *pObj);
	BOOL MoveToBack(CGeoDrawObj *pObj);
	BOOL MoveToFront(CGeoDrawObj *pObj);

	// 获取图层ID
	const CString GetLayerID() { return m_strID; }

	void Invalidate(CGeoMapView* pView = NULL);

	void OnScaleChanged();
	void ClaimChildren();

	BOOL OnEditProperties();

protected:
	DECLARE_SERIAL(CDrawLayer);
private:
};
