// CmdMoveLayerForward.cpp: implementation of the CCmdMoveLayerForward class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CmdMoveLayerForward.h"
#include "CmdMoveLayerBack.h"

#include "../GeoMap/GeoMapDoc.h"
#include "../GeoMap/GeoMapView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_COMMAND(CCmdMoveLayerForward, CCmdDraw, CMD_MOVELAYERFORWARD)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdMoveLayerForward::CCmdMoveLayerForward(CGeoMapDoc *pDoc, CDrawLayer *pLayer,CLayerListCtrl *pLayerList)
:CCmdDraw(pDoc)
{
	m_pLayerList = pLayerList;
	m_pLayer = pLayer;
	m_pLayer->AddRef();
}

CCmdMoveLayerForward::~CCmdMoveLayerForward()
{	
	m_pLayer->Release();
}

BOOL CCmdMoveLayerForward::Execute()
{
	ASSERT_VALID(m_pLayer);
	return m_pLayerList->LayerPrev(m_pLayer);
}

CCommand* CCmdMoveLayerForward::GetUndoCommand() const
{
	return new CCmdMoveLayerBack(m_pDoc, m_pLayer,m_pLayerList);
}

CCommand* CCmdMoveLayerForward::GetRedoCommand() const
{
	return new CCmdMoveLayerForward(m_pDoc, m_pLayer,m_pLayerList);
}

void CCmdMoveLayerForward::Sprint(CString &strLabel) const
{
	strLabel = _T("ÉÏÒÆÒ»²ã [") +m_pLayer->m_strLayerName + "]";
}


//////////////////////////////////////////////////////////////////////

IMPLEMENT_COMMAND(CCmdMoveLayerToFront, CCmdDraw, CMD_MOVELAYERTOFRONT)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdMoveLayerToFront::CCmdMoveLayerToFront(CGeoMapDoc *pDoc, CDrawLayer *pLayer,CLayerListCtrl *pLayerList)
:CCmdDraw(pDoc)
{
	m_pLayerList = pLayerList;
	m_pLayer = pLayer;
	m_pLayer->AddRef();
}

CCmdMoveLayerToFront::~CCmdMoveLayerToFront()
{	
	m_pLayer->Release();
}

BOOL CCmdMoveLayerToFront::Execute()
{
	ASSERT_VALID(m_pLayer);
	return m_pLayerList->LayerToFront(m_pLayer);
}

CCommand* CCmdMoveLayerToFront::GetUndoCommand() const
{
	return new CCmdMoveLayerToBack(m_pDoc, m_pLayer,m_pLayerList);
}

CCommand* CCmdMoveLayerToFront::GetRedoCommand() const
{
	return new CCmdMoveLayerToFront(m_pDoc, m_pLayer,m_pLayerList);
}

void CCmdMoveLayerToFront::Sprint(CString &strLabel) const
{
	strLabel = _T("ÒÆµ½¶¥²ã [") +m_pLayer->m_strLayerName + "]";
}