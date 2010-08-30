// CmdGeoDrawObj.h: interface for the CCmdGeoDrawObj class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CMDGEODRAWOBJ_H__87A3E497_63A3_4015_850B_FF1013105F4E__INCLUDED_)
#define AFX_CMDGEODRAWOBJ_H__87A3E497_63A3_4015_850B_FF1013105F4E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../COMMAND/CmdDraw.h"
#include "GeoDrawObj.h"
#include "GeoPolyObj.h"

class CCmdAddDrawObj : public CCmdDraw  
{
	DECLARE_COMMAND(CCmdAddDrawObj)
public:
	CCmdAddDrawObj(CGeoMapDoc *pDoc, CGeoDrawObj *pObj);
	virtual ~CCmdAddDrawObj();
public:
	virtual void Sprint(CString& strLabel) const;
	virtual CCommand* GetRedoCommand() const;
	virtual CCommand* GetUndoCommand() const;
	virtual BOOL Execute();
protected:
	CGeoDrawObj *m_pObj;
	friend class CGeoMapView;

};


class CCmdRemoveDrawObj : public CCmdDraw  
{
	DECLARE_COMMAND(CCmdRemoveDrawObj)
public:
	CCmdRemoveDrawObj(CGeoMapDoc *pDoc, CGeoDrawObj *pObj);
	virtual ~CCmdRemoveDrawObj();
public:
	virtual void Sprint(CString& strLabel) const;
	virtual CCommand* GetRedoCommand() const;
	virtual CCommand* GetUndoCommand() const;
	virtual BOOL Execute();
protected:
	CGeoDrawObj *m_pObj;
	friend class CGeoMapView;
};


class CCmdObjMove : public CCmdDraw  
{
	DECLARE_COMMAND(CCmdObjMove)
public:
	CCmdObjMove(CGeoMapDoc *pDoc, CGeoDrawObj *pObj, CRect newrect, 
		CRect oldrect, CGeoMapView *pView);
	virtual ~CCmdObjMove();
public:
	virtual void Sprint(CString& strLabel) const;
	virtual CCommand* GetRedoCommand() const;
	virtual CCommand* GetUndoCommand() const;
	virtual BOOL Execute();
protected:
	CGeoDrawObj *m_pObj;
	CRect m_CurRect;
	CRect m_position;
	CGeoMapView *p_View;
	friend class CGeoMapView;
};

class CCmdObjNoMove : public CCmdDraw  
{
	DECLARE_COMMAND(CCmdObjNoMove)
public:
	CCmdObjNoMove(CGeoMapDoc *pDoc, CGeoDrawObj *pObj, CRect newrect, 
		CRect oldrect, CGeoMapView *pView);
	virtual ~CCmdObjNoMove();
public:
	virtual void Sprint(CString& strLabel) const;
	virtual CCommand* GetRedoCommand() const;
	virtual CCommand* GetUndoCommand() const;
	virtual BOOL Execute();
protected:
	CGeoDrawObj *m_pObj;
	CRect m_CurRect;
	CRect m_position;
	CGeoMapView *p_View;
	friend class CGeoMapView;
};

class CCmdMoveToFront : public CCmdDraw  
{
	DECLARE_COMMAND(CCmdMoveToFront)
public:
	CCmdMoveToFront(CGeoMapDoc *pDoc, CGeoDrawObj *pObj);
	virtual ~CCmdMoveToFront();
public:
	virtual void Sprint(CString& strLabel) const;
	virtual CCommand* GetRedoCommand() const;
	virtual CCommand* GetUndoCommand() const;
	virtual BOOL Execute();
protected:
	CGeoDrawObj *m_pObj;
	friend class CGeoMapView;
};


class CCmdMoveToBack : public CCmdDraw  
{
	DECLARE_COMMAND(CCmdMoveToBack)
public:
	CCmdMoveToBack(CGeoMapDoc *pDoc, CGeoDrawObj *pObj);
	virtual ~CCmdMoveToBack();
public:
	virtual void Sprint(CString& strLabel) const;
	virtual CCommand* GetRedoCommand() const;
	virtual CCommand* GetUndoCommand() const;
	virtual BOOL Execute();
protected:
	CGeoDrawObj *m_pObj;
	friend class CGeoMapView;
};

class CCmdMoveForward : public CCmdDraw  
{
	DECLARE_COMMAND(CCmdMoveForward)
public:
	CCmdMoveForward(CGeoMapDoc *pDoc, CGeoDrawObj *pObj);
	virtual ~CCmdMoveForward();
public:
	virtual void Sprint(CString& strLabel) const;
	virtual CCommand* GetRedoCommand() const;
	virtual CCommand* GetUndoCommand() const;
	virtual BOOL Execute();
protected:
	CGeoDrawObj *m_pObj;
	friend class CGeoMapView;
};


class CCmdMoveBack : public CCmdDraw  
{
	DECLARE_COMMAND(CCmdMoveBack)
public:
	CCmdMoveBack(CGeoMapDoc *pDoc, CGeoDrawObj *pObj);
	virtual ~CCmdMoveBack();
public:
	virtual void Sprint(CString& strLabel) const;
	virtual CCommand* GetRedoCommand() const;
	virtual CCommand* GetUndoCommand() const;
	virtual BOOL Execute();
protected:
	CGeoDrawObj *m_pObj;
	friend class CGeoMapView;
};

//////////////////////////////////////////////////////////////////////////

class CCmdAddDrawObjs : public CCmdDraw  
{
	DECLARE_COMMAND(CCmdAddDrawObjs)
public:
	CCmdAddDrawObjs(CGeoMapDoc* pDoc, const CGeoDrawObjList& list);
	virtual ~CCmdAddDrawObjs();
public:
	virtual void Sprint(CString& strLabel) const;
	virtual CCommand* GetRedoCommand() const;
	virtual CCommand* GetUndoCommand() const;
	virtual BOOL Execute();
protected:
	CGeoDrawObjList m_list;
	friend class CGeoMapView;
};


class CCmdRemoveDrawObjs : public CCmdDraw  
{
	DECLARE_COMMAND(CCmdRemoveDrawObjs)
public:
	CCmdRemoveDrawObjs(CGeoMapDoc* pDoc, const CGeoDrawObjList& list);
	virtual ~CCmdRemoveDrawObjs();
public:
	virtual void Sprint(CString& strLabel) const;
	virtual CCommand* GetRedoCommand() const;
	virtual CCommand* GetUndoCommand() const;
	virtual BOOL Execute();
protected:
	CGeoDrawObjList m_list;
	friend class CGeoMapView;
};



class CCmdMoveDrawObjs : public CCmdDraw  
{
	DECLARE_COMMAND(CCmdMoveDrawObjs)
public:
	CCmdMoveDrawObjs(CGeoMapDoc* pDoc, const CGeoDrawObjList& list, CPoint alpha, CPoint delta, CGeoMapView *pView);
	virtual ~CCmdMoveDrawObjs();
public:
	virtual void Sprint(CString& strLabel) const;
	virtual CCommand* GetRedoCommand() const;
	virtual CCommand* GetUndoCommand() const;
	virtual BOOL Execute();
protected:
	CGeoDrawObjList m_list;
	CPoint m_ptDelta;
	CPoint m_ptAlpha;
	CGeoMapView *p_View;
	friend class CGeoMapView;
};


class CCmdNoMoveDrawObjs : public CCmdDraw  
{
	DECLARE_COMMAND(CCmdNoMoveDrawObjs)
public:
	CCmdNoMoveDrawObjs(CGeoMapDoc* pDoc, const CGeoDrawObjList& list, CPoint alpha, CPoint delta, CGeoMapView *pView);
	virtual ~CCmdNoMoveDrawObjs();
public:
	virtual void Sprint(CString& strLabel) const;
	virtual CCommand* GetRedoCommand() const;
	virtual CCommand* GetUndoCommand() const;
	virtual BOOL Execute();
protected:
	CGeoDrawObjList m_list;
	CPoint m_ptDelta;
	CPoint m_ptAlpha;
	CGeoMapView *p_View;
	friend class CGeoMapView;
};




//////////////////////////////////////////////////////////////////////////

class CCmdObjDelPt : public CCmdDraw  
{
	DECLARE_COMMAND(CCmdObjDelPt)
public:
	CCmdObjDelPt(CGeoMapDoc *pDoc, CGeoDrawObj *pObj, 
		const int&index , const CPoint &point, CGeoMapView *pView);
	virtual ~CCmdObjDelPt();
public:
	virtual void Sprint(CString& strLabel) const;
	virtual CCommand* GetRedoCommand() const;
	virtual CCommand* GetUndoCommand() const;
	virtual BOOL Execute();
protected:
	CGeoDrawObj *m_pObj;
	int m_nIndex;
	CPoint m_point;
	CGeoMapView *m_pView;
	friend class CGeoMapView;
};

class CCmdObjInsPt : public CCmdDraw  
{
	DECLARE_COMMAND(CCmdObjInsPt)
public:
	CCmdObjInsPt(CGeoMapDoc *pDoc, CGeoDrawObj *pObj,
		const int&index, const CPoint &point, CGeoMapView *pView);
	virtual ~CCmdObjInsPt();
public:
	virtual void Sprint(CString& strLabel) const;
	virtual CCommand* GetRedoCommand() const;
	virtual CCommand* GetUndoCommand() const;
	virtual BOOL Execute();
protected:
	CGeoDrawObj *m_pObj;
	int m_nIndex;
	CPoint m_point;
	CGeoMapView *m_pView;
	friend class CGeoMapView;
};

class CCmdObjMoveHandle : public CCmdDraw  
{
	DECLARE_COMMAND(CCmdObjMoveHandle)
public:
	CCmdObjMoveHandle(CGeoMapDoc *pDoc, CGeoDrawObj *pObj, 
		int nHandle, CPoint newPt, CPoint oldPt, CGeoMapView *pView);
	virtual ~CCmdObjMoveHandle();
public:
	virtual void Sprint(CString& strLabel) const;
	virtual CCommand* GetRedoCommand() const;
	virtual CCommand* GetUndoCommand() const;
	virtual BOOL Execute();
protected:
	CGeoDrawObj *m_pObj;
	CPoint m_oriPt;
	CPoint m_curPt;
	CGeoMapView *p_View;
	int m_nHandle;
	friend class CGeoMapView;
};

class CCmdObjNoMoveHandle : public CCmdDraw  
{
	DECLARE_COMMAND(CCmdObjNoMoveHandle)
public:
	CCmdObjNoMoveHandle(CGeoMapDoc *pDoc, CGeoDrawObj *pObj, int nHandle, 
		CPoint newPt, CPoint oldPt, CGeoMapView *pView);
	virtual ~CCmdObjNoMoveHandle();
public:
	virtual void Sprint(CString& strLabel) const;
	virtual CCommand* GetRedoCommand() const;
	virtual CCommand* GetUndoCommand() const;
	virtual BOOL Execute();
protected:
	CGeoDrawObj *m_pObj;
	CPoint m_oriPt;
	CPoint m_curPt;
	CGeoMapView *p_View;
	int m_nHandle;
	friend class CGeoMapView;
};

class CCmdDivideLine : public CCmdDraw
{
	DECLARE_COMMAND(CCmdDivideLine)
public:
	CCmdDivideLine(
		CGeoMapDoc *pDoc, 
		CGeoPolyObj *pObj1, 
		CGeoPolyObj *pObj2,
		CGeoPolyObj *pDivideObj,
		CGeoMapView *pView);
	virtual ~CCmdDivideLine();
public:
	virtual void Sprint(CString& strLabel) const;
	virtual CCommand* GetRedoCommand() const;
	virtual CCommand* GetUndoCommand() const;
	virtual BOOL Execute();
protected:
	CGeoPolyObj *m_pObj1;
	CGeoPolyObj *m_pObj2;
	CGeoPolyObj *m_pDivideObj;
	CGeoMapView *m_pView;
	friend class CGeoMapView;
};

class CCmdNoDivideLine : public CCmdDraw
{
	DECLARE_COMMAND(CCmdNoDivideLine)
public:
	CCmdNoDivideLine(
		CGeoMapDoc *pDoc, 
		CGeoPolyObj *pObj1, 
		CGeoPolyObj *pObj2,
		CGeoPolyObj *pDivideObj,
		CGeoMapView *pView);
	virtual ~CCmdNoDivideLine();
public:
	virtual void Sprint(CString& strLabel) const;
	virtual CCommand* GetRedoCommand() const;
	virtual CCommand* GetUndoCommand() const;
	virtual BOOL Execute();
protected:
	CGeoPolyObj *m_pObj1;
	CGeoPolyObj *m_pObj2;
	CGeoPolyObj *m_pDivideObj;
	CGeoMapView *m_pView;
	friend class CGeoMapView;
};

class CCmdJoinLine : public CCmdDraw
{
	DECLARE_COMMAND(CCmdJoinLine)
public:
	CCmdJoinLine(
		CGeoMapDoc *pDoc, 
		CGeoPolyObj *pObj1, 
		CGeoPolyObj *pObj2,
		CGeoPolyObj *pJoinObj,
		CGeoMapView *pView);
	virtual ~CCmdJoinLine();
public:
	virtual void Sprint(CString& strLabel) const;
	virtual CCommand* GetRedoCommand() const;
	virtual CCommand* GetUndoCommand() const;
	virtual BOOL Execute();
protected:
	CGeoPolyObj *m_pObj1;
	CGeoPolyObj *m_pObj2;
	CGeoPolyObj *m_pJoinObj;
	CGeoMapView *m_pView;
	friend class CGeoMapView;
};

class CCmdNoJoinLine : public CCmdDraw
{
	DECLARE_COMMAND(CCmdNoJoinLine)
public:
	CCmdNoJoinLine(
		CGeoMapDoc *pDoc, 
		CGeoPolyObj *pObj1, 
		CGeoPolyObj *pObj2,
		CGeoPolyObj *pJoinObj,
		CGeoMapView *pView);
	virtual ~CCmdNoJoinLine();
public:
	virtual void Sprint(CString& strLabel) const;
	virtual CCommand* GetRedoCommand() const;
	virtual CCommand* GetUndoCommand() const;
	virtual BOOL Execute();
protected:
	CGeoPolyObj *m_pObj1;
	CGeoPolyObj *m_pObj2;
	CGeoPolyObj *m_pJoinObj;
	CGeoMapView *m_pView;
	friend class CGeoMapView;
};

#endif // !defined(AFX_CMDGEODRAWOBJ_H__87A3E497_63A3_4015_850B_FF1013105F4E__INCLUDED_)
