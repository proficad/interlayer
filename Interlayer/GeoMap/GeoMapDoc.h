#pragma once

#include "DrawLayer.h"	// Added by ClassView
#include "DrawPage.h"	// Added by ClassView
#include "GeoDrawObj.h"
#include "../UndoBar.h"

enum enumCommand
{
	CMD_NULL,
	CMD_ADDOBJ,				// 加图素
	CMD_REMOVEOBJ,			// 删除图素
	CMD_ADDOBJS,			// 加图素组
	CMD_REMOVEOBJS,			// 删除图素组
	CMD_MOVEOBJS,			// 移动图素组
	CMD_NOMOVEOBJS,			// 反移动图素组
	CMD_OBJMOVE,			// 移动图素
	CMD_OBJNOMOVE,			// 反移动图素
	CMD_OBJDELPT,			// 删除节点
	CMD_OBJINSPT,			// 添加节点
	CMD_OBJMOVEPT,			// 移动节点
	CMD_OBJNOMOVEPT,		// 反移动节点
	CMD_DIVIDELINE,			// 剪断线段
	CMD_NODIVIDELINE,		// 反剪断线段
	CMD_JOINLINE,			// 拚接线段
	CMD_NOJOINLINE,			// 反拚接线段
	CMD_ADDPAGE,			// 加页
	CMD_INSERTPAGE,			// 插页
	CMD_REMOVEPAGE,			// 删除页
	CMD_MOVETOFRONT,		// 置顶
	CMD_MOVETOBACK,			// 置底
	CMD_MOVEFORWARD,		// 上移一层
	CMD_MOVEBACK,			// 下移一层
	CMD_SETCURPAGE,			// 设当前页
	CMD_SETPASTPAGE,		// 反设当前页
	CMD_SETCURLAYER,		// 设当前层
	CMD_SETPASTLAYER,		// 反设当前层
	CMD_MOVELAYERTOFRONT,	// 置顶层
	CMD_MOVELAYERTOBACK,	// 置底层
	CMD_MOVELAYERFORWARD,	// 上移
	CMD_MOVELAYERBACK,		// 下移
	CMD_ACTIVELAYER,		// 激活层
	CMD_NOACTIVELAYER,		// 反激活层
	CMD_RENAMELAYER,		// 重命名层
	CMD_NORENAMELAYER,		// 反重命名
	CMD_SETSHOWFLAGLAYER,	// 层显示
	CMD_NOSETSHOWFLAGLAYER,	//
	CMD_SETLOCKEDFLAGLAYER,	// 层上锁
	CMD_NOSETLOCKEDFLAGLAYER,
	CMD_ADDLAYER,			// 加层
	CMD_DELLAYER,			// 删层
};

class CGeoMapView;
class CGeoPolyObj;


// CGeoMapDoc 文档

class CGeoMapDoc : public COleDocument,
	public ICustomCommandHistory
{
	DECLARE_DYNCREATE(CGeoMapDoc)

public:
	CGeoMapDoc();
	virtual ~CGeoMapDoc();
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
	//_______________________________________________________________________
	// 属性
	CSize				m_sizeDoc;			//文档尺寸
	CString				m_strCurPageID;		// 当前页面ID
	CDrawPage*			m_pCurDrawPage;		//当前页
	CDrawLayer*			m_pCurDrawLayer;	//当前页中的当前图层

	// 获取当前页面某图层 
	CDrawLayer* GetLayer(int nIndex);
	// 设置当前图层
	BOOL SetCurLayer(CDrawLayer *pObj);

	// 图素排序
	BOOL MoveBack(CGeoDrawObj *pObj);
	BOOL MoveForward(CGeoDrawObj *pObj);
	BOOL MoveToBack(CGeoDrawObj *pObj);
	BOOL MoveToFront(CGeoDrawObj *pObj);

	// 点操作
	BOOL DeletePointFromPoly(CGeoDrawObj* pObj, int index, CPoint point, CGeoMapView *pView);	
	BOOL InsertPointOfPoly(CGeoDrawObj* pObj, int index, CPoint point, CGeoMapView *pView);

	// 线段操作
	BOOL JoinLine(CGeoPolyObj *pObj1, CGeoPolyObj *pObj2, CGeoPolyObj *pJoinObj, CGeoMapView *pView);
	BOOL NoJoinLine(CGeoPolyObj *pObj1, CGeoPolyObj *pObj2, CGeoPolyObj *pJoinObj, CGeoMapView *pView);
	BOOL DivideLine(CGeoPolyObj *pObj1, CGeoPolyObj *pObj2, CGeoPolyObj *pDivideObj, CGeoMapView *pView);
	BOOL NoDivideLine(CGeoPolyObj *pObj1, CGeoPolyObj *pObj2, CGeoPolyObj *pDivideObj, CGeoMapView *pView);

	// 图素组操作
	BOOL Remove(const CGeoDrawObjList &List);
	BOOL Add(const CGeoDrawObjList &List);
	BOOL MoveObjs(const CGeoDrawObjList &List, CPoint delta, CGeoMapView *pView);
	BOOL NoMoveObjs(const CGeoDrawObjList &List, CPoint delta, CGeoMapView *pView);
	//_______________________________________________________________________
	//
	BOOL Add(CDrawLayer *pObj);		// 在当前页增加图层
	BOOL Add(CGeoDrawObj *pObj);		// 在当前图层中增加图素
	BOOL Remove(CGeoDrawObj *pObj);	// 在当前图层中删除图素
	BOOL Remove(CDrawLayer *pObj);	// 在当前页中删除图层
	//_______________________________________________________________________
	//
	virtual BOOL IsModified();
	void Draw(CDC *pDC, CGeoMapView *pView);
	//_______________________________________________________________________
	// 获取当前point处的图素
	CGeoDrawObj* ObjectAt(const CPoint& point, CGeoMapView *pView); 
	CGeoDrawObj* HitOnBorder(const CPoint& point, CGeoMapView *pView, CGeoDrawObj*pDrawObj = NULL);
	//_______________________________________________________________________
	//
	BOOL MoveObj(CGeoDrawObj *pObj, CRect rect, CGeoMapView *pView);
	BOOL MoveHandle(CGeoDrawObj *pObj, int nHandle, CPoint pt, CGeoMapView *pView);
	//_______________________________________________________________________
	// 页大小
	CSize GetPageSize();
	void GetPrinterPageSize(CSize &pageSize);
	//_______________________________________________________________________
	// UnDo
	void DoUndo(CUndoButton* pUndoBtn);
	void GetRedoActions(CStringList& lstActions) const;
	void GetUndoActions(CStringList& lstActions) const;
	//________________________________________________________________________
	//
	void CreateInterLayer(LPCTSTR lpFileName, LPCTSTR lpLayerName);
	void CreateInterNormal(LPCTSTR lpFileName, LPCTSTR lpLayerName);
	//_______________________________________________________________________
	//
	void CreateWellLocationMap(HTREEITEM hItem);
	void CreateLayerPictrue(LPCTSTR lpFileName);
	void CreatePointMap(LPCTSTR lpFileName, LPCTSTR lpLayerName);
	void CreatePolyline(LPCTSTR lpFileName, LPCTSTR lpLayerName);
	void CreatePolygon(LPCTSTR lpFileName, LPCTSTR lpLayerName);
	void CreatePlane(LPCTSTR lpFileName, LPCTSTR lpLayerName);
	//________________________________________________________________________
	//
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual void DeleteContents();
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
protected:
	virtual BOOL OnNewDocument();
protected:
	afx_msg void OnUpdateOleVerbPopup(CCmdUI* pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnCloseDocument();
	virtual void SetTitle(LPCTSTR lpszTitle);
protected:
	virtual BOOL SaveModified();
};
