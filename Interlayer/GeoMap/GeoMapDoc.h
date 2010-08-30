#pragma once

#include "DrawLayer.h"	// Added by ClassView
#include "DrawPage.h"	// Added by ClassView
#include "GeoDrawObj.h"
#include "../UndoBar.h"

enum enumCommand
{
	CMD_NULL,
	CMD_ADDOBJ,				// ��ͼ��
	CMD_REMOVEOBJ,			// ɾ��ͼ��
	CMD_ADDOBJS,			// ��ͼ����
	CMD_REMOVEOBJS,			// ɾ��ͼ����
	CMD_MOVEOBJS,			// �ƶ�ͼ����
	CMD_NOMOVEOBJS,			// ���ƶ�ͼ����
	CMD_OBJMOVE,			// �ƶ�ͼ��
	CMD_OBJNOMOVE,			// ���ƶ�ͼ��
	CMD_OBJDELPT,			// ɾ���ڵ�
	CMD_OBJINSPT,			// ��ӽڵ�
	CMD_OBJMOVEPT,			// �ƶ��ڵ�
	CMD_OBJNOMOVEPT,		// ���ƶ��ڵ�
	CMD_DIVIDELINE,			// �����߶�
	CMD_NODIVIDELINE,		// �������߶�
	CMD_JOINLINE,			// �ս��߶�
	CMD_NOJOINLINE,			// ���ս��߶�
	CMD_ADDPAGE,			// ��ҳ
	CMD_INSERTPAGE,			// ��ҳ
	CMD_REMOVEPAGE,			// ɾ��ҳ
	CMD_MOVETOFRONT,		// �ö�
	CMD_MOVETOBACK,			// �õ�
	CMD_MOVEFORWARD,		// ����һ��
	CMD_MOVEBACK,			// ����һ��
	CMD_SETCURPAGE,			// �赱ǰҳ
	CMD_SETPASTPAGE,		// ���赱ǰҳ
	CMD_SETCURLAYER,		// �赱ǰ��
	CMD_SETPASTLAYER,		// ���赱ǰ��
	CMD_MOVELAYERTOFRONT,	// �ö���
	CMD_MOVELAYERTOBACK,	// �õײ�
	CMD_MOVELAYERFORWARD,	// ����
	CMD_MOVELAYERBACK,		// ����
	CMD_ACTIVELAYER,		// �����
	CMD_NOACTIVELAYER,		// �������
	CMD_RENAMELAYER,		// ��������
	CMD_NORENAMELAYER,		// ��������
	CMD_SETSHOWFLAGLAYER,	// ����ʾ
	CMD_NOSETSHOWFLAGLAYER,	//
	CMD_SETLOCKEDFLAGLAYER,	// ������
	CMD_NOSETLOCKEDFLAGLAYER,
	CMD_ADDLAYER,			// �Ӳ�
	CMD_DELLAYER,			// ɾ��
};

class CGeoMapView;
class CGeoPolyObj;


// CGeoMapDoc �ĵ�

class CGeoMapDoc : public COleDocument,
	public ICustomCommandHistory
{
	DECLARE_DYNCREATE(CGeoMapDoc)

public:
	CGeoMapDoc();
	virtual ~CGeoMapDoc();
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
	//_______________________________________________________________________
	// ����
	CSize				m_sizeDoc;			//�ĵ��ߴ�
	CString				m_strCurPageID;		// ��ǰҳ��ID
	CDrawPage*			m_pCurDrawPage;		//��ǰҳ
	CDrawLayer*			m_pCurDrawLayer;	//��ǰҳ�еĵ�ǰͼ��

	// ��ȡ��ǰҳ��ĳͼ�� 
	CDrawLayer* GetLayer(int nIndex);
	// ���õ�ǰͼ��
	BOOL SetCurLayer(CDrawLayer *pObj);

	// ͼ������
	BOOL MoveBack(CGeoDrawObj *pObj);
	BOOL MoveForward(CGeoDrawObj *pObj);
	BOOL MoveToBack(CGeoDrawObj *pObj);
	BOOL MoveToFront(CGeoDrawObj *pObj);

	// �����
	BOOL DeletePointFromPoly(CGeoDrawObj* pObj, int index, CPoint point, CGeoMapView *pView);	
	BOOL InsertPointOfPoly(CGeoDrawObj* pObj, int index, CPoint point, CGeoMapView *pView);

	// �߶β���
	BOOL JoinLine(CGeoPolyObj *pObj1, CGeoPolyObj *pObj2, CGeoPolyObj *pJoinObj, CGeoMapView *pView);
	BOOL NoJoinLine(CGeoPolyObj *pObj1, CGeoPolyObj *pObj2, CGeoPolyObj *pJoinObj, CGeoMapView *pView);
	BOOL DivideLine(CGeoPolyObj *pObj1, CGeoPolyObj *pObj2, CGeoPolyObj *pDivideObj, CGeoMapView *pView);
	BOOL NoDivideLine(CGeoPolyObj *pObj1, CGeoPolyObj *pObj2, CGeoPolyObj *pDivideObj, CGeoMapView *pView);

	// ͼ�������
	BOOL Remove(const CGeoDrawObjList &List);
	BOOL Add(const CGeoDrawObjList &List);
	BOOL MoveObjs(const CGeoDrawObjList &List, CPoint delta, CGeoMapView *pView);
	BOOL NoMoveObjs(const CGeoDrawObjList &List, CPoint delta, CGeoMapView *pView);
	//_______________________________________________________________________
	//
	BOOL Add(CDrawLayer *pObj);		// �ڵ�ǰҳ����ͼ��
	BOOL Add(CGeoDrawObj *pObj);		// �ڵ�ǰͼ��������ͼ��
	BOOL Remove(CGeoDrawObj *pObj);	// �ڵ�ǰͼ����ɾ��ͼ��
	BOOL Remove(CDrawLayer *pObj);	// �ڵ�ǰҳ��ɾ��ͼ��
	//_______________________________________________________________________
	//
	virtual BOOL IsModified();
	void Draw(CDC *pDC, CGeoMapView *pView);
	//_______________________________________________________________________
	// ��ȡ��ǰpoint����ͼ��
	CGeoDrawObj* ObjectAt(const CPoint& point, CGeoMapView *pView); 
	CGeoDrawObj* HitOnBorder(const CPoint& point, CGeoMapView *pView, CGeoDrawObj*pDrawObj = NULL);
	//_______________________________________________________________________
	//
	BOOL MoveObj(CGeoDrawObj *pObj, CRect rect, CGeoMapView *pView);
	BOOL MoveHandle(CGeoDrawObj *pObj, int nHandle, CPoint pt, CGeoMapView *pView);
	//_______________________________________________________________________
	// ҳ��С
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
