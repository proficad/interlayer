#if !defined(AFX_GEOMAPVIEW_H__A01F658F_AA48_4860_BE29_4053DD2B8019__INCLUDED_)
#define AFX_GEOMAPVIEW_H__A01F658F_AA48_4860_BE29_4053DD2B8019__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GeoMapView.h : header file
//
#include "GeoMapDoc.h"
#include "EditEx.h"

/////////////////////////////////////////////////////////////////////////////
// CGeoMapView view
class CGeoDrawObj;
class CGeoTextObj;
class CGeoMapView;
class CGeoLineObj;
class CGeoPolyObj;
class CGeoPictrueObj;

//-----------------------------------------------------------------------
class CZoomCmd : public CObject
{
public:
	long m_lZoom;		// ��������
	CPoint m_ptPos;		// λ��
};
//-----------------------------------------------------------------------
class CFontDesc
{
public:
	CFontDesc(LPCTSTR lpszName, LPCTSTR lpszScript, BYTE nCharSet,
		BYTE nPitchAndFamily, DWORD dwFlags);
	CString m_strName;
	CString m_strScript;
	BYTE m_nCharSet;
	BYTE m_nPitchAndFamily;
	DWORD m_dwFlags;
};
//-----------------------------------------------------------------------
struct CCharFormat : public CHARFORMAT
{
	CCharFormat() {cbSize = sizeof(CHARFORMAT);}
	BOOL operator==(CCharFormat& cf);
};
//-----------------------------------------------------------------------
struct CHARHDR : public tagNMHDR
{
	CHARFORMAT cf;
	CHARHDR() {cf.cbSize = sizeof(CHARFORMAT);}
};
//-----------------------------------------------------------------------
class CHandTracker : public CRectTracker
{
public:
	CGeoMapView *m_pView;
	CHandTracker(CGeoMapView *pView);
	virtual void DrawTrackerRect(LPCRECT lpRect, CWnd* pWndClipTo, CDC* pDC, CWnd* pWnd);
	virtual void OnChangedRect(const CRect& rectOld);
};
//------------------------------------------------------------------------
namespace Angle
{
	class Line 
	{
		double m_x1, m_y1, m_x2, m_y2;
	public:
		Line(double x1, double y1, double x2, double y2)
		{
			m_x1 = x1;
			m_y1 = y1;
			m_x2 = x2;
			m_y2 = y2;
		}
		double getXVector() 
		{
			return m_x2 - m_x1;
		}
		double getYVector() 
		{
			return m_y2 - m_y1;
		}
		double getSquareLength() 
		{
			double x = getXVector();
			double y = getYVector();
			return sqrt(x * x + y * y);
		}
	};

	class AngleCalculator 
	{
	public:
		double getDegree(Line l1, Line l2)
		{
			double vx1=l1.getXVector();
			double vy1=l1.getYVector();
			double vx2=l2.getXVector();
			double vy2=l2.getYVector();
			double X = (vx1*vx2+vy1*vy2)/(l1.getSquareLength()*l2.getSquareLength());
			return (acos(X));
		}
	};

	//------------------------------------------------------------------------
	/* �÷�
	Angle::AngleCalculator cal;
	// ֱ��1
	Angle::Line l1(x1, y1, x2,y2);
	// ֱ��2
	Angle::Line l2(x3, y3, x4,y4);

	// �Ƕ�
	double dAngle = cal.getDegree(l1,l2);

	CString msg;
	msg.Format("�Ƕȣ�%.2lf��",dAngle);
	AfxMessageBox(msg);
	*/
}
//_______________________________________________________________________
//
struct tagPrCorIndex
{
	double dDis;
	CGeoPolyObj *pPoly;
};

struct tagPrNormalIndex
{
	CGeoLineObj *pLine;
	double dDis;
};
//________________________________________________________________________
//
class CGeoMapView : public CScrollView
{
protected:
	CGeoMapView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CGeoMapView)

	enum Operation
	{
		opNone,
		opMoveView,		// �ƶ��Ӵ�
		opZoomOut,		// ��С
		opZoomIn,		// �Ŵ�
		opZoomRect,		// ����Ŵ�
		opChalk,		// �Ƿ��÷۱�
		opBrush,		// ��ʽˢ
		opAddCtrlPoint,	// ��ӿ��Ƶ�
		opDelCtrlPoint, // ɾ�����Ƶ�
		opEditCtrlPoint,// �༭���Ƶ�
	};
// Attributes
public:

	CObList	m_listUndoHisZoom;		// ͼ�η�����ʷ��¼
	CObList	m_listRedoHisZoom;	
	void ResetHisZoom();
	POSITION m_posUndoZoom;			// ��ǰ��ʾ��ʷ��¼��λ��
	POSITION m_posRedoZoom;	
	CPoint m_ptDown;
	CPoint m_ptLast;
	HCURSOR m_hCursor;
	Operation m_eOperation;
	BOOL m_bActive;

	static CLIPFORMAT m_cfDraw_E; // ���Ʋ������ʽ-ͼ��
	static CLIPFORMAT m_cfDraw_L; // ���Ʋ������ʽ-ͼ��
	static CLIPFORMAT m_cfDraw_P; // ���Ʋ������ʽ-ͼҳ

	static CLIPFORMAT m_cfObjectDescriptor; // ����������
	
	CGeoDrawObjList m_selection; // Ŀǰѡ���ͼ��

	CGeoTextObj *m_pDrawTextObj;
	CEditEx m_LeftEdit;
	CEditEx m_CenterEdit;
	CEditEx m_RightEdit;

	BOOL m_bWeight;
	BOOL m_bItalic;
	BOOL m_bUnderline;
	BOOL m_bCharColor;

	CPoint m_dragPoint;                 // current position
	CSize m_dragSize;                   // size of dragged object
	CSize m_dragOffset;                 // offset between pt and drag object corner
	DROPEFFECT m_prevDropEffect;
	BOOL m_bDragDataAcceptable;

	long m_lStepZoom;						// �Ŵ󲽳�
	BOOL	m_bCapture;						// �Զ���׽
	BOOL	m_bPageSelectAll;				// �Ƿ�ѡ��ȫҳ

	int m_nCtrlPointIndex;

	CGeoMapDoc* GetDocument();
	
// Operations
public:
	//////////////////////////////////////////////////////////////////////////
	CPoint m_Offset;		// document offset [vlu]
	void UpdateScrollPosition(CPoint point);
	CPoint GetScrollPosition() const;				// [vlu]
	void   ScrollToPosition(POINT Point);			// Point [vlu]
	void   CenterOnPoint(CPoint Center);			// Center [vlu]
	void   GetLogClientRect(CRect *pRect) const;	// [vlu]
	CPoint GetLogPosition(CPoint Point) const;		// [vlu]
	CPoint GetLogClientCenter() const;				// [vlu]
	//////////////////////////////////////////////////////////////////////////
	
	void DocToClient(CRect& rect);
	void DocToClient(CPoint& point);
	void ClientToDoc(CPoint& point);
	void DocToClient(CSize& size);
	void ClientToDoc(CSize& size);
	void ClientToDoc(CRect& rect);

	void Remove(CGeoDrawObj* pObj);							// ��ѡ���б���ɾ��ͼ��
	virtual BOOL IsSelected(const CObject* pDocItem) const;	// �ж�ͼ���Ƿ���ѡ���б���
	void DeSelect(CGeoDrawObj *pObj);						// ȡ��ѡ��ͼ��
	void SelectWithinRect(CRect rect, BOOL bAdd = FALSE);	// ��rect�����ڵ�ͼ����뵽ѡ���б���
	void CloneSelection();									// ��¡ѡ���б��е�����ͼ��
	void InvalObj(CGeoDrawObj* pObj);						// ˢ��ͼ��
	void Select(CGeoDrawObj *pObj, BOOL bAdd = FALSE);		// ��ͼ�ؼ��뵽ѡ���б���
	void UpdateActiveItem();

	void InvalLayer(CDrawLayer* pLayer);					// ˢ��ͼ��
	
	BOOL GetObjectInfo(COleDataObject* pDataObject,	CSize* pSize, CSize* pOffset);
	COleDropTarget m_dropTarget; // OLE�϶�Ŀ����
	void PasteEmbedded(COleDataObject& dataObject, CPoint point );

	// OLE����ĳ�ʼ��С
	CRect m_rectObject;
	CRect GetInitialPosition();

	// ����ĳͼ��Ϊ����״̬
	void SetSelectActive(CGeoDrawObj* pDrawObj);

	// ճ��ͼ��
	void PasteNative(COleDataObject& dataObject);

	// �ı���ʽ
	void SetCharFormat(CCharFormat& cf);
	void UpdateFormatBar();

	//------------------------------------------------------------------------
	// 
	// ����в���
	void SortInterLayerLine(CGeoDrawObjList &polys, CGeoLineObj *pLine);
	// ������
	void SortNormalLine(CGeoDrawObjList &Lines, CGeoPolyObj *pPoly);
	// ɾ��û���ཻ�ķ���
	void DeleteNoCorssNormal(CGeoDrawObjList &polys, CGeoDrawObjList &Lines);
	// ��ȡ�����в��ߵ���������
	CGeoLineObj * GetCrossFirstNormal(CGeoDrawObjList &Lines, CGeoPolyObj *pPoly); 
	// ��ȡ�������ߵ������в���
	CGeoPolyObj * GetCrossFirstLayer(CGeoDrawObjList &polys, CGeoLineObj *pLine);
	// ��ȡ�в㷽��ֵ
	int GetPolyDirection(CGeoDrawObjList &Lines, CGeoPolyObj *pPoly);

	//________________________________________________________________________
	// ����ת���Ĳ���
	bool CaleParameter(CGeoPictrueObj*pPictrue);		// ����ת���Ĳ���
	void ParamterChanged();								// ת���Ĳ����ı���

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGeoMapView)
	public:
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual void OnDragLeave();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual BOOL OnScrollBy(CSize sizeScroll, BOOL bDoScroll = TRUE);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CGeoMapView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CGeoMapView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFilePrintPreview();
	afx_msg void OnZoomPageCenter();
	afx_msg void OnZoomAll();
	afx_msg void OnZoomMove();
	afx_msg void OnUpdateZoomMove(CCmdUI* pCmdUI);
	afx_msg void OnZoomRect();
	afx_msg void OnUpdateZoomRect(CCmdUI* pCmdUI);
	afx_msg void OnZoomInPoint();
	afx_msg void OnUpdateZoomInPoint(CCmdUI* pCmdUI);
	afx_msg void OnZoomOutPoint();
	afx_msg void OnUpdateZoomOutPoint(CCmdUI* pCmdUI);
	afx_msg void OnZoomIn();
	afx_msg void OnUpdateZoomIn(CCmdUI* pCmdUI);
	afx_msg void OnZoomOut();
	afx_msg void OnUpdateZoomOut(CCmdUI* pCmdUI);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnCancelEdit();
	afx_msg void OnUpdateCancelEdit(CCmdUI* pCmdUI);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnZoomPrev();
	afx_msg void OnUpdateZoomPrev(CCmdUI* pCmdUI);
	afx_msg void OnZoomNext();
	afx_msg void OnUpdateZoomNext(CCmdUI* pCmdUI);
	afx_msg void OnUpdateZoomAll(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnFileExport();
	afx_msg void OnDispPagepanel();
	afx_msg void OnUpdateDispPagepanel(CCmdUI* pCmdUI);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDrawSelect();
	afx_msg void OnUpdateDrawSelect(CCmdUI* pCmdUI);
	afx_msg void OnDrawBasePolyline();
	afx_msg void OnUpdateDrawBasePolyline(CCmdUI* pCmdUI);
	afx_msg void OnDrawBasePline();
	afx_msg void OnUpdateDrawBasePline(CCmdUI* pCmdUI);
	afx_msg void OnDrawBasePolygon();
	afx_msg void OnUpdateDrawBasePolygon(CCmdUI* pCmdUI);
	afx_msg void OnDrawBasePlinegon();
	afx_msg void OnUpdateDrawBasePlinegon(CCmdUI* pCmdUI);
	afx_msg void OnDrawBasePolygonFill();
	afx_msg void OnUpdateDrawBasePolygonFill(CCmdUI* pCmdUI);
	afx_msg void OnDrawBasePlinegonFill();
	afx_msg void OnUpdateDrawBasePlinegonFill(CCmdUI* pCmdUI);
	afx_msg void OnEditClear();
	afx_msg void OnUpdateEditClear(CCmdUI* pCmdUI);
	afx_msg void OnPloyAddPt();
	afx_msg void OnUpdatePloyAddPt(CCmdUI* pCmdUI);
	afx_msg void OnPolyDelPt();
	afx_msg void OnUpdatePolyDelPt(CCmdUI* pCmdUI);
	afx_msg void OnPolyMovePt();
	afx_msg void OnUpdatePolyMovePt(CCmdUI* pCmdUI);
	afx_msg void OnPolyDivideLine();
	afx_msg void OnUpdatePolyDivideLine(CCmdUI* pCmdUI);
	afx_msg void OnPolyJoinLine();
	afx_msg void OnUpdatePolyJoinLine(CCmdUI* pCmdUI);
	afx_msg void OnObjectMovetofront();
	afx_msg void OnUpdateObjectMovetofront(CCmdUI* pCmdUI);
	afx_msg void OnObjectMovetoback();
	afx_msg void OnUpdateObjectMovetoback(CCmdUI* pCmdUI);
	afx_msg void OnObjectMoveforward();
	afx_msg void OnUpdateObjectMoveforward(CCmdUI* pCmdUI);
	afx_msg void OnObjectMoveback();
	afx_msg void OnUpdateObjectMoveback(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDrawPolygon();
	afx_msg void OnUpdateDrawPolygon(CCmdUI* pCmdUI);
	afx_msg void OnDrawSmoothFill();
	afx_msg void OnUpdateDrawSmoothFill(CCmdUI* pCmdUI);
	afx_msg void OnDrawBaseRectEmpty();
	afx_msg void OnUpdateDrawBaseRectEmpty(CCmdUI* pCmdUI);
	afx_msg void OnDrawBaseRect();
	afx_msg void OnUpdateDrawBaseRect(CCmdUI* pCmdUI);
	afx_msg void OnDrawRect();
	afx_msg void OnUpdateDrawRect(CCmdUI* pCmdUI);
	afx_msg void OnDrawShow();
	afx_msg void OnUpdateDrawShow(CCmdUI* pCmdUI);
	afx_msg void OnDrawLock();
	afx_msg void OnUpdateDrawLock(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnDrawHide();
	afx_msg void OnUpdateDrawHide(CCmdUI* pCmdUI);
	afx_msg void OnDrawNolock();
	afx_msg void OnUpdateDrawNolock(CCmdUI* pCmdUI);
	afx_msg void OnDrawCapture();
	afx_msg void OnUpdateDrawCapture(CCmdUI* pCmdUI);
	afx_msg void OnEditSelectAll();
	afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnElementSelectall();
	afx_msg void OnUpdateElementSelectall(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnOleInsertNew();
	afx_msg void OnArrangeLeft();
	afx_msg void OnUpdateArrangeLeft(CCmdUI* pCmdUI);
	afx_msg void OnArrangeRight();
	afx_msg void OnUpdateArrangeRight(CCmdUI* pCmdUI);
	afx_msg void OnArrangeTop();
	afx_msg void OnUpdateArrangeTop(CCmdUI* pCmdUI);
	afx_msg void OnArrangeBottom();
	afx_msg void OnUpdateArrangeBottom(CCmdUI* pCmdUI);
	afx_msg void OnArrangeCenter();
	afx_msg void OnUpdateArrangeCenter(CCmdUI* pCmdUI);
	afx_msg void OnArrangeVcenter();
	afx_msg void OnUpdateArrangeVcenter(CCmdUI* pCmdUI);
	afx_msg void OnEvenlyHorz();
	afx_msg void OnUpdateEvenlyHorz(CCmdUI* pCmdUI);
	afx_msg void OnEvenlyVert();
	afx_msg void OnUpdateEvenlyVert(CCmdUI* pCmdUI);
	afx_msg void OnAliginWidth();
	afx_msg void OnUpdateAliginWidth(CCmdUI* pCmdUI);
	afx_msg void OnAliginHeight();
	afx_msg void OnUpdateAliginHeight(CCmdUI* pCmdUI);
	afx_msg void OnAliginSize();
	afx_msg void OnUpdateAliginSize(CCmdUI* pCmdUI);
	afx_msg void OnImportLine();
	afx_msg void OnDrawClip();
	afx_msg void OnUpdateDrawClip(CCmdUI* pCmdUI);
	afx_msg void OnAttributeBrush();
	afx_msg void OnUpdateAttributeBrush(CCmdUI* pCmdUI);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnZoomSetPageCenter();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnFontname();
	afx_msg void OnUpdateFontname(CCmdUI* pCmdUI);
	afx_msg void OnFontsize();
	afx_msg void OnUpdateFontsize(CCmdUI* pCmdUI);
	afx_msg void OnCharBold();
	afx_msg void OnUpdateCharBold(CCmdUI* pCmdUI);
	afx_msg void OnCharItalic();
	afx_msg void OnUpdateCharItalic(CCmdUI* pCmdUI);
	afx_msg void OnCharUnderline();
	afx_msg void OnUpdateCharUnderline(CCmdUI* pCmdUI);
	afx_msg void OnCharColor();
	afx_msg void OnUpdateCharColor(CCmdUI* pCmdUI);
	afx_msg void OnCharWidth();
	afx_msg void OnUpdateCharWidth(CCmdUI* pCmdUI);
	afx_msg void OnCharNarrow();
	afx_msg void OnUpdateCharNarrow(CCmdUI* pCmdUI);
	afx_msg void OnParaLeft();
	afx_msg void OnUpdateParaLeft(CCmdUI* pCmdUI);
	afx_msg void OnParaCenter();
	afx_msg void OnUpdateParaCenter(CCmdUI* pCmdUI);
	afx_msg void OnParaRight();
	afx_msg void OnUpdateParaRight(CCmdUI* pCmdUI);
	afx_msg void OnDrawBaseText();
	afx_msg void OnUpdateDrawBaseText(CCmdUI* pCmdUI);
	afx_msg void OnCancelMode();
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnUpdateZoomSetPageCenter(CCmdUI* pCmdUI);
	afx_msg void OnZoomPageMove();
	afx_msg void OnUpdateZoomPageMove(CCmdUI* pCmdUI);
	afx_msg void OnDrawBaseLine();
	afx_msg void OnUpdateDrawBaseLine(CCmdUI* pCmdUI);
	afx_msg void OnDrawEnableMove();
	afx_msg void OnUpdateDrawEnableMove(CCmdUI* pCmdUI);
	afx_msg void OnDrawEditProperties();
	afx_msg void OnUpdateDrawEditProperties(CCmdUI* pCmdUI);
	afx_msg void OnDrawBasePicture();
	afx_msg void OnUpdateDrawBasePicture(CCmdUI* pCmdUI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDrawScaleRuler();
	afx_msg void OnUpdateDrawScaleRuler(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg LRESULT OnEndTextEdit(WPARAM wp, LPARAM lp);
	DECLARE_MESSAGE_MAP()
private:
	void DrawPage(CDC *pDC);
public:
	afx_msg void OnClose();
	afx_msg void OnAddCtrlPoint();
	afx_msg void OnUpdateAddCtrlPoint(CCmdUI *pCmdUI);
	afx_msg void OnDeleteCtrlPoint();
	afx_msg void OnUpdateDeleteCtrlPoint(CCmdUI *pCmdUI);
	afx_msg void OnEditCtrlPoint();
	afx_msg void OnUpdateEditCtrlPoint(CCmdUI *pCmdUI);
	afx_msg void OnExportCurve();
	afx_msg void OnUpdateExportCurve(CCmdUI *pCmdUI);
	afx_msg void OnExportLine();
	afx_msg void OnUpdateExportLine(CCmdUI *pCmdUI);
	afx_msg void OnCaleParameter();
	afx_msg void OnUpdateCaleParameter(CCmdUI *pCmdUI);	
	afx_msg void OnDrawNormal();
	afx_msg void OnUpdateDrawNormal(CCmdUI *pCmdUI);
	afx_msg void OnDrawInterlayer();
	afx_msg void OnUpdateDrawInterlayer(CCmdUI *pCmdUI);
	afx_msg void OnSingleInterlayer();
	afx_msg void OnUpdateSingleInterlayer(CCmdUI *pCmdUI);
	afx_msg void OnSaveLayerModel();
	afx_msg void OnUpdateSaveLayerModel(CCmdUI *pCmdUI);
	afx_msg void OnSaveLayermodelLine();
	afx_msg void OnUpdateSaveLayermodelLine(CCmdUI *pCmdUI);
	afx_msg void OnChangeDirection();
	afx_msg void OnChangeGeneral();
	afx_msg void OnUpdateChangeGeneral(CCmdUI *pCmdUI);
	afx_msg void OnChangeInterlayer();
	afx_msg void OnUpdateChangeInterlayer(CCmdUI *pCmdUI);
	afx_msg void OnChangeNormal();
	afx_msg void OnUpdateChangeNormal(CCmdUI *pCmdUI);
	afx_msg void OnChangeLine();
	afx_msg void OnUpdateChangeLine(CCmdUI *pCmdUI);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GEOMAPVIEW_H__A01F658F_AA48_4860_BE29_4053DD2B8019__INCLUDED_)
