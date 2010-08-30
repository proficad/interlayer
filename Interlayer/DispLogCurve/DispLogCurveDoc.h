#if !defined(AFX_LOGCURVETABDOC_H__88757CE5_6B38_4845_B6A1_8FA3DD7599B6__INCLUDED_)
#define AFX_LOGCURVETABDOC_H__88757CE5_6B38_4845_B6A1_8FA3DD7599B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LogCurveTabDoc.h : header file
//
#include "InPlaceEdit.h"	// Added by ClassView
#include "LogCurveDatStruct.h"

/////////////////////////////////////////////////////////////////////////////
// CDispLogCurveDoc document

#define IDC_INPLACE_CONTROL 8 

class CDispLogCurveView;

class CLogChartTool
{
public:
	CLogChartTool()
	{
		m_ptDown = CPoint(-1,-1);
		m_ptLast = CPoint(-1,-1);
		m_toolMode = none;
		m_iIndex = -1;
		m_iLaneDragOver = -1;
		m_bDragLane = FALSE;

		m_pLaneSelected = NULL;
		m_pCurveSelected = NULL;

		m_bEditing = FALSE;
		m_pEditFont = NULL;
		m_pEditWnd = NULL;
	}
	
	enum SelectMode
	{
		none,
			drag,
			move,
			sizehorz,
			sizevert
	};
public:
	CLogChart	*m_pLogChart;
	CPoint m_ptDown;			// 
	CPoint m_ptLast;
	int		m_iIndex;				// �ı��ȵ�Ӿ������

	SelectMode	m_toolMode;			// ��������
	BOOL		m_bDragLane;		// �Ƿ��϶�Ӿ��
	int			m_iLaneDrag;		// Ӿ������
	int			m_iCurveDrag;		// ��������
	int			m_iLaneDragOver;	// �϶�ʱ��ǰ��Ӿ�� 

	CLogCurveLane *m_pLaneSelected; // ��ǰѡ�еĵ�
	CLogCurveLane::strLOGINGPROP *m_pCurveSelected;	//// ��ǰѡ�е�����

	BOOL		m_bEditing;			// ���ڱ༭
	dispLogCurve::CInPlaceEdit *m_pEditWnd;		// �༭��
	CFont		*m_pEditFont;		//����
	
public:	
	void OnLButtonDblClk(CDispLogCurveView* pView, UINT nFlags, const CPoint& point);
	void OnLButtonDown(CDispLogCurveView* pView, UINT nFlags, const CPoint& point);
	void OnLButtonUp(CDispLogCurveView* pView, UINT nFlags, const CPoint& point);
	void OnRButtonDown(CDispLogCurveView* pView, UINT nFlags, const CPoint& point);
	void OnMouseMove(CDispLogCurveView* pView, UINT nFlags, const CPoint& point);
	void OnKeyDown(CDispLogCurveView* pView, UINT nChar, UINT nRepCnt, UINT nFlags);
	void Edit(int nIndex, CRect rect,UINT nChar,CDispLogCurveView* pView);

	void EndEdit();
};


//////////////////////////////////////////////////////////////////////////

class CDispLogCurveDoc : public CDocument
{
protected:
	CDispLogCurveDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDispLogCurveDoc)

// Attributes
public:

	CSize m_sizeDoc;
	CSize GetPageSize();
	void GetPrinterPageSize(CSize &pageSize);
// Operations
public:
	CString m_strWellName;
	CLogChart m_chartLog;
	CLogChartTool m_chartLogTool;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDispLogCurveDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDispLogCurveDoc();
	virtual BOOL SaveModified();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CDispLogCurveDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual void SetTitle(LPCTSTR lpszTitle);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGCURVETABDOC_H__88757CE5_6B38_4845_B6A1_8FA3DD7599B6__INCLUDED_)
