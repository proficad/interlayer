// CmdGeoDrawObj.cpp: implementation of the CCmdGeoDrawObj class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CmdGeoDrawObj.h"
#include "GeoMapDoc.h"
#include "GeoMapView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_COMMAND(CCmdAddDrawObj, CCmdDraw, CMD_ADDOBJ)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCmdAddDrawObj::CCmdAddDrawObj(CGeoMapDoc *pDoc, CGeoDrawObj *pObj)
: CCmdDraw(pDoc)
{
	m_pObj = pObj;
	m_pObj->AddRef();
}

CCmdAddDrawObj::~CCmdAddDrawObj()
{
	m_pObj->Release();
}

BOOL CCmdAddDrawObj::Execute()
{
	ASSERT_VALID(m_pObj);
	return m_pDoc->Add(m_pObj);
}

CCommand* CCmdAddDrawObj::GetUndoCommand() const
{
	return new CCmdRemoveDrawObj(m_pDoc, m_pObj);
}

CCommand* CCmdAddDrawObj::GetRedoCommand() const
{
	return new CCmdAddDrawObj(m_pDoc, m_pObj);
}

void CCmdAddDrawObj::Sprint(CString &strLabel) const
{
	strLabel = _T("增加图素");
}


//////////////////////////////////////////////////////////////////////
// CCmdRemoveDrawObj Class
//////////////////////////////////////////////////////////////////////
IMPLEMENT_COMMAND(CCmdRemoveDrawObj, CCmdDraw, CMD_REMOVEOBJ)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdRemoveDrawObj::CCmdRemoveDrawObj(CGeoMapDoc *pDoc, CGeoDrawObj *pObj)
: CCmdDraw(pDoc)
{
	m_pObj = pObj;
	m_pObj->AddRef();
}

CCmdRemoveDrawObj::~CCmdRemoveDrawObj()
{
	m_pObj->Release();
}

BOOL CCmdRemoveDrawObj::Execute()
{
	ASSERT_VALID(m_pObj);
	return m_pDoc->Remove(m_pObj);
}

CCommand* CCmdRemoveDrawObj::GetUndoCommand() const
{
	return new CCmdAddDrawObj(m_pDoc, m_pObj);
}

CCommand* CCmdRemoveDrawObj::GetRedoCommand() const
{
	return new CCmdRemoveDrawObj(m_pDoc, m_pObj);
}

void CCmdRemoveDrawObj::Sprint(CString &strLabel) const
{
	strLabel = _T("删除图素");
}



//////////////////////////////////////////////////////////////////////
// CCmdObjMove Class
//////////////////////////////////////////////////////////////////////

IMPLEMENT_COMMAND(CCmdObjMove, CCmdDraw, CMD_OBJMOVE)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdObjMove::CCmdObjMove(CGeoMapDoc *pDoc, CGeoDrawObj *pObj, 
						 CRect newrect, CRect oldrect, CGeoMapView *pView)
: CCmdDraw(pDoc)
{
	m_pObj = pObj;
	m_position = newrect;
	m_CurRect = oldrect;
	p_View = pView;
	m_pObj->AddRef();
}

CCmdObjMove::~CCmdObjMove()
{
	m_pObj->Release();
}

BOOL CCmdObjMove::Execute()
{
	ASSERT_VALID(m_pObj);
	return m_pDoc->MoveObj(m_pObj, m_position, p_View);
}

CCommand* CCmdObjMove::GetUndoCommand() const
{
	return new CCmdObjNoMove(m_pDoc, m_pObj, m_CurRect, m_position, p_View);
}

CCommand* CCmdObjMove::GetRedoCommand() const
{
	return new CCmdObjMove(m_pDoc, m_pObj, m_position, m_CurRect, p_View);
}

void CCmdObjMove::Sprint(CString &strLabel) const
{
	strLabel = _T("移动图素");
}

//////////////////////////////////////////////////////////////////////
// CCmdObjNoMove Class
//////////////////////////////////////////////////////////////////////

IMPLEMENT_COMMAND(CCmdObjNoMove, CCmdDraw, CMD_OBJNOMOVE)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdObjNoMove::CCmdObjNoMove(CGeoMapDoc *pDoc, CGeoDrawObj *pObj,
							 CRect newrect, CRect oldrect, CGeoMapView *pView)
: CCmdDraw(pDoc)
{
	m_pObj = pObj;
	m_position = newrect;
	m_CurRect = oldrect;
	p_View = pView;
	m_pObj->AddRef();
}

CCmdObjNoMove::~CCmdObjNoMove()
{
	m_pObj->Release();
}

BOOL CCmdObjNoMove::Execute()
{
	ASSERT_VALID(m_pObj);
	return m_pDoc->MoveObj(m_pObj, m_position, p_View);
}

CCommand* CCmdObjNoMove::GetUndoCommand() const
{
	return new CCmdObjMove(m_pDoc, m_pObj, m_CurRect, m_position, p_View);
}

CCommand* CCmdObjNoMove::GetRedoCommand() const
{
	return new CCmdObjNoMove(m_pDoc, m_pObj, m_position, m_CurRect, p_View);
}

void CCmdObjNoMove::Sprint(CString &strLabel) const
{
	strLabel = _T("移动图素");
}

//////////////////////////////////////////////////////////////////////
// CCmdMoveToFront Class
//////////////////////////////////////////////////////////////////////
IMPLEMENT_COMMAND(CCmdMoveToFront, CCmdDraw, CMD_MOVETOFRONT)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdMoveToFront::CCmdMoveToFront(CGeoMapDoc *pDoc, CGeoDrawObj *pObj)
:CCmdDraw(pDoc)
{
	m_pObj = pObj;
	m_pObj->AddRef();
}

CCmdMoveToFront::~CCmdMoveToFront()
{

	m_pObj->Release();
}

BOOL CCmdMoveToFront::Execute()
{
	ASSERT_VALID(m_pObj);
	return m_pDoc->MoveToFront(m_pObj);
}

CCommand* CCmdMoveToFront::GetUndoCommand() const
{
	return new CCmdMoveToBack(m_pDoc, m_pObj);
}

CCommand* CCmdMoveToFront::GetRedoCommand() const
{
	return new CCmdMoveToFront(m_pDoc, m_pObj);
}

void CCmdMoveToFront::Sprint(CString &strLabel) const
{
	strLabel = _T("置图素于顶层");
}


//////////////////////////////////////////////////////////////////////
// CCmdMoveToBack Class
//////////////////////////////////////////////////////////////////////
IMPLEMENT_COMMAND(CCmdMoveToBack, CCmdDraw, CMD_MOVETOBACK)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdMoveToBack::CCmdMoveToBack(CGeoMapDoc *pDoc, CGeoDrawObj *pObj)
:CCmdDraw(pDoc)
{
	m_pObj = pObj;
	m_pObj->AddRef();
}

CCmdMoveToBack::~CCmdMoveToBack()
{

	m_pObj->Release();
}

BOOL CCmdMoveToBack::Execute()
{
	ASSERT_VALID(m_pObj);
	return m_pDoc->MoveToBack(m_pObj);
}

CCommand* CCmdMoveToBack::GetUndoCommand() const
{
	return new CCmdMoveToFront(m_pDoc, m_pObj);
}

CCommand* CCmdMoveToBack::GetRedoCommand() const
{
	return new CCmdMoveToBack(m_pDoc, m_pObj);
}

void CCmdMoveToBack::Sprint(CString &strLabel) const
{
	strLabel = _T("置图素于底层");
}

//////////////////////////////////////////////////////////////////////
// CCmdMoveForward Class
//////////////////////////////////////////////////////////////////////
IMPLEMENT_COMMAND(CCmdMoveForward, CCmdDraw, CMD_MOVEFORWARD)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdMoveForward::CCmdMoveForward(CGeoMapDoc *pDoc, CGeoDrawObj *pObj)
:CCmdDraw(pDoc)
{
	m_pObj = pObj;
	m_pObj->AddRef();
}

CCmdMoveForward::~CCmdMoveForward()
{

	m_pObj->Release();
}

BOOL CCmdMoveForward::Execute()
{
	ASSERT_VALID(m_pObj);
	return m_pDoc->MoveForward(m_pObj);
}

CCommand* CCmdMoveForward::GetUndoCommand() const
{
	return new CCmdMoveBack(m_pDoc, m_pObj);
}

CCommand* CCmdMoveForward::GetRedoCommand() const
{
	return new CCmdMoveForward(m_pDoc, m_pObj);
}

void CCmdMoveForward::Sprint(CString &strLabel) const
{
	strLabel = _T("上移一层");
}


//////////////////////////////////////////////////////////////////////
// CCmdMoveBack Class
//////////////////////////////////////////////////////////////////////
IMPLEMENT_COMMAND(CCmdMoveBack, CCmdDraw, CMD_MOVEBACK)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdMoveBack::CCmdMoveBack(CGeoMapDoc *pDoc, CGeoDrawObj *pObj)
:CCmdDraw(pDoc)
{
	m_pObj = pObj;
	m_pObj->AddRef();
}

CCmdMoveBack::~CCmdMoveBack()
{

	m_pObj->Release();
}

BOOL CCmdMoveBack::Execute()
{
	ASSERT_VALID(m_pObj);
	return m_pDoc->MoveBack(m_pObj);
}

CCommand* CCmdMoveBack::GetUndoCommand() const
{
	return new CCmdMoveForward(m_pDoc, m_pObj);
}

CCommand* CCmdMoveBack::GetRedoCommand() const
{
	return new CCmdMoveBack(m_pDoc, m_pObj);
}

void CCmdMoveBack::Sprint(CString &strLabel) const
{
	strLabel = _T("下移一层");
}



//////////////////////////////////////////////////////////////////////
// CCmdAddDrawObjs Class
//////////////////////////////////////////////////////////////////////

IMPLEMENT_COMMAND(CCmdAddDrawObjs, CCmdDraw, CMD_ADDOBJS)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CCmdAddDrawObjs::CCmdAddDrawObjs(CGeoMapDoc* pDoc, const CGeoDrawObjList& List)
: CCmdDraw(pDoc), m_list(List.GetCount())
{
	m_list.AddTail((CGeoDrawObjList*)&List);
}

CCmdAddDrawObjs::~CCmdAddDrawObjs()
{
	
}

BOOL CCmdAddDrawObjs::Execute()
{
	return m_pDoc->Add(m_list);
}

CCommand* CCmdAddDrawObjs::GetUndoCommand() const
{
	return new CCmdRemoveDrawObjs(m_pDoc, m_list);
}

CCommand* CCmdAddDrawObjs::GetRedoCommand() const
{
	return new CCmdAddDrawObjs(m_pDoc, m_list);
}

void CCmdAddDrawObjs::Sprint(CString &strLabel) const
{
	strLabel = _T("撤消删除选中的所有图素");
}

//////////////////////////////////////////////////////////////////////
// CCmdRemoveDrawObjs Class
//////////////////////////////////////////////////////////////////////

IMPLEMENT_COMMAND(CCmdRemoveDrawObjs, CCmdDraw, CMD_REMOVEOBJS)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CCmdRemoveDrawObjs::CCmdRemoveDrawObjs(CGeoMapDoc* pDoc, const CGeoDrawObjList& List)
: CCmdDraw(pDoc), m_list(List.GetCount())
{
	m_list.AddTail((CGeoDrawObjList*)&List);
}

CCmdRemoveDrawObjs::~CCmdRemoveDrawObjs()
{
	
}

BOOL CCmdRemoveDrawObjs::Execute()
{
	return m_pDoc->Remove(m_list);
}

CCommand* CCmdRemoveDrawObjs::GetUndoCommand() const
{
	return new CCmdAddDrawObjs(m_pDoc, m_list);
}

CCommand* CCmdRemoveDrawObjs::GetRedoCommand() const
{
	return new CCmdRemoveDrawObjs(m_pDoc, m_list);
}

void CCmdRemoveDrawObjs::Sprint(CString &strLabel) const
{
	strLabel = _T("删除选中的所有图素");
}



//////////////////////////////////////////////////////////////////////
// CCmdAddDrawObjs Class
//////////////////////////////////////////////////////////////////////

IMPLEMENT_COMMAND(CCmdMoveDrawObjs, CCmdDraw, CMD_MOVEOBJS)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CCmdMoveDrawObjs::CCmdMoveDrawObjs(CGeoMapDoc* pDoc, 
	const CGeoDrawObjList& List, 
	CPoint alpha, CPoint delta, CGeoMapView *pView)
: CCmdDraw(pDoc), m_list(List.GetCount())
{
	m_ptAlpha = alpha;
	m_ptDelta = delta;
	m_list.AddTail((CGeoDrawObjList*)&List);
	p_View = pView;
}

CCmdMoveDrawObjs::~CCmdMoveDrawObjs()
{
	
}

BOOL CCmdMoveDrawObjs::Execute()
{
	return m_pDoc->MoveObjs(m_list, m_ptAlpha, p_View);
}

CCommand* CCmdMoveDrawObjs::GetUndoCommand() const
{
	return new CCmdNoMoveDrawObjs(m_pDoc, m_list, m_ptDelta, m_ptDelta, p_View);
}

CCommand* CCmdMoveDrawObjs::GetRedoCommand() const
{
	return new CCmdMoveDrawObjs(m_pDoc, m_list, m_ptDelta, m_ptDelta, p_View);
}

void CCmdMoveDrawObjs::Sprint(CString &strLabel) const
{
	strLabel = _T("移动选中的所有图素");
}

//////////////////////////////////////////////////////////////////////
// CCmdRemoveDrawObjs Class
//////////////////////////////////////////////////////////////////////

IMPLEMENT_COMMAND(CCmdNoMoveDrawObjs, CCmdDraw, CMD_NOMOVEOBJS)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CCmdNoMoveDrawObjs::CCmdNoMoveDrawObjs(CGeoMapDoc* pDoc, 
	const CGeoDrawObjList& List, 
	CPoint alpha, CPoint delta, CGeoMapView *pView)
: CCmdDraw(pDoc), m_list(List.GetCount())
{
	m_ptAlpha = alpha;
	m_ptDelta = delta;
	m_list.AddTail((CGeoDrawObjList*)&List);
	p_View = pView;
}

CCmdNoMoveDrawObjs::~CCmdNoMoveDrawObjs()
{
	
}

BOOL CCmdNoMoveDrawObjs::Execute()
{
	return m_pDoc->NoMoveObjs(m_list, m_ptAlpha, p_View);
}

CCommand* CCmdNoMoveDrawObjs::GetUndoCommand() const
{
	return new CCmdMoveDrawObjs(m_pDoc, m_list, m_ptDelta, m_ptDelta, p_View);
}

CCommand* CCmdNoMoveDrawObjs::GetRedoCommand() const
{
	return new CCmdNoMoveDrawObjs(m_pDoc, m_list, m_ptDelta, m_ptDelta, p_View);
}

void CCmdNoMoveDrawObjs::Sprint(CString &strLabel) const
{
	strLabel = _T("移动选中的所有图素");
}


//////////////////////////////////////////////////////////////////////////
IMPLEMENT_COMMAND(CCmdObjDelPt, CCmdDraw, CMD_OBJDELPT)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdObjDelPt::CCmdObjDelPt(CGeoMapDoc *pDoc, CGeoDrawObj *pObj,
						   const int&index, const CPoint &point , CGeoMapView *pView)
: CCmdDraw(pDoc)
{
	m_pObj = pObj;
	m_nIndex = index;
	m_point = point;
	m_pView = pView;
	m_pObj->AddRef();
}

CCmdObjDelPt::~CCmdObjDelPt()
{
	m_pObj->Release();
}

BOOL CCmdObjDelPt::Execute()
{
	ASSERT_VALID(m_pObj);
	return m_pDoc->DeletePointFromPoly(m_pObj, m_nIndex, m_point,m_pView);
}

CCommand* CCmdObjDelPt::GetUndoCommand() const
{
	return new CCmdObjInsPt(m_pDoc, m_pObj, m_nIndex, m_point,m_pView);
}

CCommand* CCmdObjDelPt::GetRedoCommand() const
{
	return new CCmdObjDelPt(m_pDoc, m_pObj, m_nIndex, m_point,m_pView);
}

void CCmdObjDelPt::Sprint(CString &strLabel) const
{
	strLabel = _T("删除点");
}


//////////////////////////////////////////////////////////////////////
// CCmdObjInsPt Class
//////////////////////////////////////////////////////////////////////
IMPLEMENT_COMMAND(CCmdObjInsPt, CCmdDraw, CMD_OBJINSPT)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdObjInsPt::CCmdObjInsPt(CGeoMapDoc *pDoc, CGeoDrawObj *pObj, 
						   const int&index , const CPoint &point, CGeoMapView *pView)
: CCmdDraw(pDoc)
{
	m_pObj = pObj;
	m_nIndex = index;
	m_point = point;
	m_pView = pView;
	m_pObj->AddRef();
}

CCmdObjInsPt::~CCmdObjInsPt()
{
	m_pObj->Release();
}

BOOL CCmdObjInsPt::Execute()
{
	ASSERT_VALID(m_pObj);
	return m_pDoc->InsertPointOfPoly(m_pObj, m_nIndex, m_point, m_pView);
}

CCommand* CCmdObjInsPt::GetUndoCommand() const
{
	return new CCmdObjDelPt(m_pDoc, m_pObj, m_nIndex, m_point,m_pView);
}

CCommand* CCmdObjInsPt::GetRedoCommand() const
{
	return new CCmdObjInsPt(m_pDoc, m_pObj, m_nIndex, m_point,m_pView);
}

void CCmdObjInsPt::Sprint(CString &strLabel) const
{
	strLabel = _T("插入点");
}

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_COMMAND(CCmdObjMoveHandle, CCmdDraw, CMD_OBJMOVEPT)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdObjMoveHandle::CCmdObjMoveHandle(CGeoMapDoc *pDoc, CGeoDrawObj *pObj, 
		int nHandle, CPoint newPt, CPoint oldPt, CGeoMapView *pView)
: CCmdDraw(pDoc)
{
	m_pObj = pObj;
	m_curPt = newPt;
	m_oriPt = oldPt;
	p_View = pView;
	m_nHandle = nHandle;
	m_pObj->AddRef();
}

CCmdObjMoveHandle::~CCmdObjMoveHandle()
{
	m_pObj->Release();
}

BOOL CCmdObjMoveHandle::Execute()
{
	ASSERT_VALID(m_pObj);
	return m_pDoc->MoveHandle(m_pObj, m_nHandle, m_curPt, p_View);
}

CCommand* CCmdObjMoveHandle::GetUndoCommand() const
{
	return new CCmdObjNoMoveHandle(m_pDoc, m_pObj, m_nHandle, m_oriPt, m_curPt, p_View);
}

CCommand* CCmdObjMoveHandle::GetRedoCommand() const
{
	return new CCmdObjMoveHandle(m_pDoc, m_pObj, m_nHandle, m_curPt, m_oriPt, p_View);
}

void CCmdObjMoveHandle::Sprint(CString &strLabel) const
{
	strLabel = _T("移动曲线点");
}

//////////////////////////////////////////////////////////////////////
// CCmdObjNoMove Class
//////////////////////////////////////////////////////////////////////

IMPLEMENT_COMMAND(CCmdObjNoMoveHandle, CCmdDraw, CMD_OBJNOMOVEPT)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdObjNoMoveHandle::CCmdObjNoMoveHandle(CGeoMapDoc *pDoc, CGeoDrawObj *pObj,
		int nHandle, CPoint newPt, CPoint oldPt, CGeoMapView *pView)
: CCmdDraw(pDoc)
{
	m_pObj = pObj;
	m_curPt = newPt;
	m_oriPt = oldPt;
	p_View = pView;
	m_nHandle = nHandle;
	m_pObj->AddRef();
}

CCmdObjNoMoveHandle::~CCmdObjNoMoveHandle()
{
	m_pObj->Release();
}

BOOL CCmdObjNoMoveHandle::Execute()
{
	ASSERT_VALID(m_pObj);
	return m_pDoc->MoveHandle(m_pObj, m_nHandle, m_curPt, p_View);
}

CCommand* CCmdObjNoMoveHandle::GetUndoCommand() const
{
	return new CCmdObjMoveHandle(m_pDoc, m_pObj, m_nHandle, m_oriPt, m_curPt, p_View);
}

CCommand* CCmdObjNoMoveHandle::GetRedoCommand() const
{
	return new CCmdObjNoMoveHandle(m_pDoc, m_pObj, m_nHandle, m_curPt, m_oriPt, p_View);
}

void CCmdObjNoMoveHandle::Sprint(CString &strLabel) const
{
	strLabel = _T("移动曲线点");
}


//////////////////////////////////////////////////////////////////////
// CCmdDivideLine Class
//////////////////////////////////////////////////////////////////////

IMPLEMENT_COMMAND(CCmdDivideLine, CCmdDraw, CMD_DIVIDELINE)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdDivideLine::CCmdDivideLine(CGeoMapDoc *pDoc, 
							   CGeoPolyObj *pObj1, 
							   CGeoPolyObj *pObj2,
							   CGeoPolyObj *pDivideObj,
							   CGeoMapView *pView)
: CCmdDraw(pDoc)
{
	m_pObj1 = pObj1;
	m_pObj2 = pObj2;
	m_pDivideObj = pDivideObj;
	m_pView = pView;
	m_pObj1->AddRef();
	if(m_pObj2)
		m_pObj2->AddRef();
	m_pDivideObj->AddRef();
}

CCmdDivideLine::~CCmdDivideLine()
{
	m_pObj1->Release();
	if(m_pObj2)
		m_pObj2->Release();
	m_pDivideObj->Release();
}

BOOL CCmdDivideLine::Execute()
{
	return m_pDoc->DivideLine(m_pObj1, m_pObj2, m_pDivideObj, m_pView);
}

CCommand* CCmdDivideLine::GetUndoCommand() const
{
	return new CCmdNoDivideLine(m_pDoc, m_pObj1, m_pObj2, m_pDivideObj, m_pView);
}

CCommand* CCmdDivideLine::GetRedoCommand() const
{
	return new CCmdDivideLine(m_pDoc, m_pObj1, m_pObj2, m_pDivideObj, m_pView);
}

void CCmdDivideLine::Sprint(CString &strLabel) const
{
	strLabel = _T("截断线段");
}


//////////////////////////////////////////////////////////////////////
// CCmdNoDivideLine Class
//////////////////////////////////////////////////////////////////////

IMPLEMENT_COMMAND(CCmdNoDivideLine, CCmdDraw, CMD_NODIVIDELINE)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdNoDivideLine::CCmdNoDivideLine(CGeoMapDoc *pDoc, 
							   CGeoPolyObj *pObj1, 
							   CGeoPolyObj *pObj2,
							   CGeoPolyObj *pDivideObj,
							   CGeoMapView *pView)
: CCmdDraw(pDoc)
{
	m_pObj1 = pObj1;
	m_pObj2 = pObj2;
	m_pDivideObj = pDivideObj;
	m_pView = pView;
	m_pObj1->AddRef();
	if(m_pObj2)
		m_pObj2->AddRef();
	m_pDivideObj->AddRef();
}

CCmdNoDivideLine::~CCmdNoDivideLine()
{
	m_pObj1->Release();
	if(m_pObj2)
		m_pObj2->Release();
	m_pDivideObj->Release();
}

BOOL CCmdNoDivideLine::Execute()
{
	return m_pDoc->NoDivideLine(m_pObj1, m_pObj2, m_pDivideObj, m_pView);
}

CCommand* CCmdNoDivideLine::GetUndoCommand() const
{
	return new CCmdDivideLine(m_pDoc, m_pObj1, m_pObj2, m_pDivideObj, m_pView);
}

CCommand* CCmdNoDivideLine::GetRedoCommand() const
{
	return new CCmdNoDivideLine(m_pDoc, m_pObj1, m_pObj2, m_pDivideObj, m_pView);
}

void CCmdNoDivideLine::Sprint(CString &strLabel) const
{
	strLabel = _T("截断线段");
}



//////////////////////////////////////////////////////////////////////
// CCmdJoinLine Class
//////////////////////////////////////////////////////////////////////

IMPLEMENT_COMMAND(CCmdJoinLine, CCmdDraw, CMD_JOINLINE)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdJoinLine::CCmdJoinLine(CGeoMapDoc *pDoc, 
						   CGeoPolyObj *pObj1, 
						   CGeoPolyObj *pObj2,
						   CGeoPolyObj *pJoinObj,
							 CGeoMapView *pView)
							 : CCmdDraw(pDoc)
{
	m_pObj1 = pObj1;
	m_pObj2 = pObj2;
	m_pJoinObj = pJoinObj;
	m_pView = pView;
	m_pObj1->AddRef();
	m_pObj2->AddRef();
	m_pJoinObj->AddRef();
}

CCmdJoinLine::~CCmdJoinLine()
{
	m_pObj1->Release();
	m_pObj2->Release();
	m_pJoinObj->Release();
}

BOOL CCmdJoinLine::Execute()
{
	return m_pDoc->JoinLine(m_pObj1, m_pObj2, m_pJoinObj, m_pView);
}

CCommand* CCmdJoinLine::GetUndoCommand() const
{
	return new CCmdNoJoinLine(m_pDoc, m_pObj1, m_pObj2, m_pJoinObj, m_pView);
}

CCommand* CCmdJoinLine::GetRedoCommand() const
{
	return new CCmdJoinLine(m_pDoc, m_pObj1, m_pObj2, m_pJoinObj, m_pView);
}

void CCmdJoinLine::Sprint(CString &strLabel) const
{
	strLabel = _T("拚接曲线");
}

//////////////////////////////////////////////////////////////////////
// CCmdNoJoinLine Class
//////////////////////////////////////////////////////////////////////

IMPLEMENT_COMMAND(CCmdNoJoinLine, CCmdDraw, CMD_NOJOINLINE)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdNoJoinLine::CCmdNoJoinLine(CGeoMapDoc *pDoc, 
							   CGeoPolyObj *pObj1, 
							   CGeoPolyObj *pObj2,
							   CGeoPolyObj *pJoinObj,
								 CGeoMapView *pView)
: CCmdDraw(pDoc)
{
	m_pObj1 = pObj1;
	m_pObj2 = pObj2;
	m_pJoinObj = pJoinObj;
	m_pView = pView;
	m_pObj1->AddRef();
	m_pObj2->AddRef();
	m_pJoinObj->AddRef();
}

CCmdNoJoinLine::~CCmdNoJoinLine()
{
	m_pObj1->Release();
	m_pObj2->Release();
	m_pJoinObj->Release();
}

BOOL CCmdNoJoinLine::Execute()
{
	return m_pDoc->NoJoinLine(m_pObj1, m_pObj2, m_pJoinObj, m_pView);
}

CCommand* CCmdNoJoinLine::GetUndoCommand() const
{
	return new CCmdJoinLine(m_pDoc, m_pObj1, m_pObj2, m_pJoinObj, m_pView);
}

CCommand* CCmdNoJoinLine::GetRedoCommand() const
{
	return new CCmdNoJoinLine(m_pDoc, m_pObj1, m_pObj2, m_pJoinObj, m_pView);
}

void CCmdNoJoinLine::Sprint(CString &strLabel) const
{
	strLabel = _T("拚接曲线");
}


