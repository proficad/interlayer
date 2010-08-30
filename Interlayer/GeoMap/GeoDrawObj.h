#pragma once

class CGeoMapDoc;
class CGeoMapView;

class CDrawLayer;
class CDrawPage;

#define HINT_UPDATE_WINDOW			0	// ˢ�´���
#define HINT_UPDATE_DRAWOBJ			1   // ˢ��ͼ��
#define HINT_UPDATE_SELECTION		2	// ˢ������ѡ���ͼ��
#define HINT_DELETE_SELECTION		3	// ��ȥ����ѡ��ͼ��
#define HINT_UPDATE_OLE_ITEMS		4	// ˢ��OLE����
#define HINT_UPDATE_LAYERBAR		5	// ˢ��ͼ�㹤����
#define HINT_UPDATE_PAGE			6	// �ı䵱ǰҳ��

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
	// ��������
	CRect		m_rtPosition;			// ͼ�ص�λ��
	CRectEx		m_geoPosition;			// ������귶Χ
	BOOL		m_bLocked;				// ����ͼ��
	BOOL		m_bShow;				// �Ƿ���ʾ
	CString		m_strID;				// ͼ��ID
	CArray<POINT,POINT> m_ptAnchors;	// ê��
	CPoint2D	m_geoCenterPoint;		// ͼ�����Ĵ������

	// ����������
	BOOL		m_bMoving;			// �Ƿ������ƶ�	
	BOOL		m_bEditing;			// �Ƿ����ڱ༭
	BOOL		m_bSizeChanging;	// �Ƿ����ڵ�����С
	UINT		m_uMenuID;			// �����˵�ID
	CString		m_strObjName;		// ͼ������
	TrackerState m_trackerState;	// 

	CGeoDrawObj	*m_pSelectedObj;	// �Ѿ�ѡ�е���ͼ��

	// ��������
	CGeoMapDoc *m_pDocument;		// �����ĵ�
	CDrawLayer *m_pParentLayer;		// ����ͼ��
	CDrawPage  *m_pParentPage;		// ������ҳ 	
	CGeoDrawObj *m_pParentObj;		// ��������

	CGeoDrawObjList m_DrawObjects;	// �������Ķ����б�

public:

	// ͼ������
	virtual BOOL MoveBack(CGeoDrawObj *pObj);					
	virtual BOOL MoveForward(CGeoDrawObj *pObj);
	virtual BOOL MoveToBack(CGeoDrawObj *pObj);
	virtual BOOL MoveToFront(CGeoDrawObj *pObj);

	virtual BOOL Add(CGeoDrawObj *pObj);						// ����ͼ��
	virtual BOOL Remove(CGeoDrawObj *pOb);						// ɾ��ͼ��
	virtual void RemoveAll();
	// ����ͼ������
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
	// ͼ�������Ľ�����
	virtual CPoint GetCorner(int nHandle);
	// �����Ľ�
	virtual CRect GetCornerRect(int nHandleID, CGeoMapView* pView = NULL);

	virtual void Invalidate(CGeoMapView* pView = NULL);
	virtual void Serialize(CArchive& ar);
	virtual CGeoDrawObj* Clone(CDrawLayer *pLayer);
	virtual BOOL OnEditProperties(CGeoMapView* pView = NULL);

	virtual int GetPathPoint(LPPOINT &lppt);
	virtual CPoint GetNearByIndex(const CPoint &point, int *iIndex);

	// �ƶ�ͼ�ش�С
	virtual void MoveHandleTo(int nHandle, CPoint point, CGeoMapView* pView = NULL);
	// �ƶ�ͼ��λ��
	virtual void MoveTo(const CRect& position, CGeoMapView* pView = NULL);
	// ��ͼ��״̬
	virtual void DrawTracker(CDC* pDC, CGeoMapView *pView);
	// ��ͼ��
	virtual void Draw(CDC* pDC, CGeoMapView* pView = NULL);
	// ���༭״̬
	virtual void DrawEditMode(CGeoMapView* pView);
	// ���ƶ�״̬
	virtual void DrawMoveState(CGeoMapView* pView);
	// ���ı��С״̬
	virtual void DrawSizeState(CGeoMapView* pView);
	// �������߷����ı�ʱ
	virtual void OnScaleChanged();
	// ��������תʱ
	virtual void OnAngleChanged();
	// ����������ԭ���иı�ʱ
	virtual void OnCoordOrigChanged();
	// ת�������ı�ʱ
	virtual void OnConvertParamChanged(){};

	// ����㼯
	virtual void ReversePoints(){}

	// ��ȡ���ڵ�ͼ��
	CDrawLayer* GetParentLayer() { return m_pParentLayer;}

	// ����Ǵ������ϵ����ת���ɴ������(geodetic coordinates)
	// �������������ϵ����Ϊͼ����������
	virtual CPoint2D CoordinateTransfer(const CPoint &pt);
	virtual CPoint CoordinateTransfer(const CPoint2D &pt);
	virtual CRectEx CoordinateTransfer(const CRect &rect);
	virtual CRect CoordinateTransfer(const CRectEx &rect);

	virtual UINT GetMenuID();
	virtual void SetObjName(CString strObjName);

	// ���캢��
	virtual void ClaimChildren();
};