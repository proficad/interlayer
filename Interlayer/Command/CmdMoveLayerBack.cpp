// CmdMoveLayerBack.cpp: implementation of the CCmdMoveLayerBack class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CmdMoveLayerBack.h"
#include "CmdMoveLayerForward.h"
#include "../GeoMap/LayerListCtrl.h"
#include "../GeoMap/GeoMapDoc.h"
#include "../GeoMap/GeoMapView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_COMMAND(CCmdMoveLayerBack, CCmdDraw, CMD_MOVELAYERBACK)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdMoveLayerBack::CCmdMoveLayerBack(CGeoMapDoc *pDoc, CDrawLayer *pLayer,CLayerListCtrl *pLayerList)
:CCmdDraw(pDoc)
{
	m_pLayerList = pLayerList;
	m_pLayer = pLayer;
	m_pLayer->AddRef();
}

CCmdMoveLayerBack::~CCmdMoveLayerBack()
{	
	m_pLayer->Release();
}

BOOL CCmdMoveLayerBack::Execute()
{
	ASSERT_VALID(m_pLayer);
	return m_pLayerList->LayerNext(m_pLayer);
}

CCommand* CCmdMoveLayerBack::GetUndoCommand() const
{
	return new CCmdMoveLayerForward(m_pDoc, m_pLayer,m_pLayerList);
}

CCommand* CCmdMoveLayerBack::GetRedoCommand() const
{
	return new CCmdMoveLayerBack(m_pDoc, m_pLayer,m_pLayerList);
}

void CCmdMoveLayerBack::Sprint(CString &strLabel) const
{
	strLabel = _T("ÏÂÒÆÒ»²ã [") +m_pLayer->m_strLayerName + "]";
}

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_COMMAND(CCmdMoveLayerToBack, CCmdDraw, CMD_MOVELAYERTOBACK)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdMoveLayerToBack::CCmdMoveLayerToBack(CGeoMapDoc *pDoc, CDrawLayer *pLayer,CLayerListCtrl *pLayerList)
:CCmdDraw(pDoc)
{
	m_pLayerList = pLayerList;
	m_pLayer = pLayer;
	m_pLayer->AddRef();
}

CCmdMoveLayerToBack::~CCmdMoveLayerToBack()
{	
	m_pLayer->Release();
}

BOOL CCmdMoveLayerToBack::Execute()
{
	ASSERT_VALID(m_pLayer);
	return m_pLayerList->LayerToBack(m_pLayer);
}

CCommand* CCmdMoveLayerToBack::GetUndoCommand() const
{
	return new CCmdMoveLayerToFront(m_pDoc, m_pLayer,m_pLayerList);
}

CCommand* CCmdMoveLayerToBack::GetRedoCommand() const
{
	return new CCmdMoveLayerToBack(m_pDoc, m_pLayer,m_pLayerList);
}

void CCmdMoveLayerToBack::Sprint(CString &strLabel) const
{
	strLabel = _T("ÒÆµ½µ×²ã [") +m_pLayer->m_strLayerName + "]";
}

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_COMMAND(CCmdAddLayer, CCmdDraw, CMD_ADDLAYER)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdAddLayer::CCmdAddLayer(CGeoMapDoc *pDoc, CDrawLayer *pLayer,CLayerListCtrl *pLayerList)
:CCmdDraw(pDoc)
{
	m_pLayerList = pLayerList;
	m_pLayer = pLayer;
	m_pLayer->AddRef();
}

CCmdAddLayer::~CCmdAddLayer()
{	
	m_pLayer->Release();
}

BOOL CCmdAddLayer::Execute()
{
	ASSERT_VALID(m_pLayer);
	return m_pLayerList->AddLayer(m_pLayer);
}

CCommand* CCmdAddLayer::GetUndoCommand() const
{
	return new CCmdDelLayer(m_pDoc, m_pLayer,m_pLayerList);
}

CCommand* CCmdAddLayer::GetRedoCommand() const
{
	return new CCmdAddLayer(m_pDoc, m_pLayer,m_pLayerList);
}

void CCmdAddLayer::Sprint(CString &strLabel) const
{
	strLabel = _T("Ôö¼ÓÍ¼²ã [") +m_pLayer->m_strLayerName + "]";
}

//////////////////////////////////////////////////////////////////////////
IMPLEMENT_COMMAND(CCmdDelLayer, CCmdDraw, CMD_DELLAYER)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdDelLayer::CCmdDelLayer(CGeoMapDoc *pDoc, CDrawLayer *pLayer,CLayerListCtrl *pLayerList)
:CCmdDraw(pDoc)
{
	m_pLayerList = pLayerList;
	m_pLayer = pLayer;
	m_pLayer->AddRef();
}

CCmdDelLayer::~CCmdDelLayer()
{	
	m_pLayer->Release();
}

BOOL CCmdDelLayer::Execute()
{
	ASSERT_VALID(m_pLayer);
	return m_pLayerList->DelLayer(m_pLayer);
}

CCommand* CCmdDelLayer::GetUndoCommand() const
{
	return new CCmdAddLayer(m_pDoc, m_pLayer,m_pLayerList);
}

CCommand* CCmdDelLayer::GetRedoCommand() const
{
	return new CCmdDelLayer(m_pDoc, m_pLayer,m_pLayerList);
}

void CCmdDelLayer::Sprint(CString &strLabel) const
{
	strLabel = _T("É¾³ýÍ¼²ã [") +m_pLayer->m_strLayerName + "]";
}


