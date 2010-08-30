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

	enum GeoLayerType		// ͼ������
	{
		generic = 0,		// ��ͨ��
		wellmap,			// ��λͼ��
		basepictrue,		// ��ͼ
		interlayer,			// ����в�
		faultage,			// �ϲ�ͼ��
		faultage_up,		// �ϲ�����ͼ��
		faultage_down,		// �ϲ�����ͼ��
		SediFacies,			// ������ͼ�� Sedimentary Facies
		contour,			// ��ֵ��ͼ
	};
public:
	// ����	
	// ����
	BOOL	m_bShow;				// �Ƿ���ʾ TURE-��ʾ FALSE-����
	BOOL	m_bLocked;				// �Ƿ����� TRUE-��  FALSE-��
	CString	m_strLayerName;			// ͼ������
	CString m_strID;				// ͼ��ID
	CGeoDrawObjList m_DrawObjects;	// ����ͼ�صĶ����б�
	CRgnClipping	m_rgnClipping;	// ��������
	BOOL	m_bClipping;			// �Ƿ�ֻ��ʾ���������ͼ��	
	BOOL	m_bIsNew;				// �Ƿ�Ϊ�²�
	BOOL	m_bIsModified;			// �Ƿ��Ѿ��޸Ĺ�

	GeoLayerType m_eLayerType;		// ͼ������
	int		m_iTransparence;		// ͸���� (ȫ͸��~��͸�� 0~255)

	//////////////////////////////////////////////////////////////////////////
	CGeoMapDoc *m_pDocument;		// ��ǰ�ĵ�
	CDrawPage *m_pParentPage;		// ����ҳ��
	CDrawPage *GetParentPage() { return m_pParentPage;}

	// ͼ���С��Χ
	CRect GetLayerRect();

	//////////////////////////////////////////////////////////////////////////
	int GetCountObj() { return m_DrawObjects.GetCount();} // ͼ������
	CGeoDrawObj *GetHeadObj(){return m_DrawObjects.GetHead();}
	CGeoDrawObj *GetTailObj(){return m_DrawObjects.GetTail();}
	CGeoDrawObj *GetAtObj(POSITION pos){return m_DrawObjects.GetAt(pos);}
	POSITION GetHeadPosition(){return m_DrawObjects.GetHeadPosition();}
	CGeoDrawObj *GetNextObj(POSITION &pos){ return m_DrawObjects.GetNext(pos);}
public:
	//����
	BOOL Add(CGeoDrawObj *pObj);						// ����ͼ��
	virtual void Draw(CDC* pDC, CGeoMapView *pView);	// ��ʾͼ��
	virtual void DrawTracker(CDC* pDC, CGeoMapView *pView);	// ��ʾͼ��
	virtual void Serialize(CArchive& ar);				// ͼ�㴮�л�
	BOOL Remove(CGeoDrawObj *pObj);						// ɾ��ͼ��
	void RemoveAll();									// ɾ������ͼ��	

	// ͼ������
	BOOL MoveBack(CGeoDrawObj *pObj);					
	BOOL MoveForward(CGeoDrawObj *pObj);
	BOOL MoveToBack(CGeoDrawObj *pObj);
	BOOL MoveToFront(CGeoDrawObj *pObj);

	// ��ȡͼ��ID
	const CString GetLayerID() { return m_strID; }

	void Invalidate(CGeoMapView* pView = NULL);

	void OnScaleChanged();
	void ClaimChildren();

	BOOL OnEditProperties();

protected:
	DECLARE_SERIAL(CDrawLayer);
private:
};
