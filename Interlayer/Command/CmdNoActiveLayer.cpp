// CmdNoActiveLayer.cpp: implementation of the CCmdNoActiveLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CmdNoActiveLayer.h"
#include "../GeoMap/GeoMapDoc.h"
#include "CmdActiveLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_COMMAND(CCmdNoActiveLayer, CCmdDraw, CMD_NOACTIVELAYER)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdNoActiveLayer::CCmdNoActiveLayer(CGeoMapDoc *pDoc, CDrawLayer *pLayer, CLayerListCtrl *pLayerList, const int &nNoActiveLayerID, const int &nActiveLayerID)
:CCmdDraw(pDoc)
{
	m_nNoActiveLayerID = nNoActiveLayerID;
	m_nActiveLayerID = nActiveLayerID;
	m_pLayerList = pLayerList;
	m_pLayer = pLayer;
	m_pLayer->AddRef();
}

CCmdNoActiveLayer::~CCmdNoActiveLayer()
{
	m_pLayer->Release();
}

BOOL CCmdNoActiveLayer::Execute()
{
	return m_pLayerList->ActiveLayer(m_nNoActiveLayerID);
}

CCommand* CCmdNoActiveLayer::GetUndoCommand() const
{
	return new CCmdActiveLayer(m_pDoc, m_pLayer,m_pLayerList,m_nNoActiveLayerID,m_nActiveLayerID );
}

CCommand* CCmdNoActiveLayer::GetRedoCommand() const
{
	return new CCmdNoActiveLayer(m_pDoc, m_pLayer,m_pLayerList,m_nNoActiveLayerID,m_nActiveLayerID);
}

void CCmdNoActiveLayer::Sprint(CString &strLabel) const
{
	CLayerCell *pCell = (CLayerCell *)m_pLayerList->GetItemData(m_nNoActiveLayerID);
	CString strLayerName = pCell->m_strLayerName;
	strLabel = _T("激活 [") +strLayerName + "]";
}


//////////////////////////////////////////////////////////////////////////
IMPLEMENT_COMMAND(CCmdNoRenameLayer, CCmdDraw, CMD_NORENAMELAYER)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdNoRenameLayer::CCmdNoRenameLayer(CGeoMapDoc *pDoc, CDrawLayer *pLayer, CLayerListCtrl *pLayerList, const int &nLayerID, const CString &strOldName, const CString &strNewName)
:CCmdDraw(pDoc)
{
	m_nLayerID = nLayerID;
	m_pLayerList = pLayerList;
	m_strOldName = strOldName;
	m_strNewName = strNewName;
	m_pLayer = pLayer;
	m_pLayer->AddRef();
}

CCmdNoRenameLayer::~CCmdNoRenameLayer()
{
	m_pLayer->Release();	
}

BOOL CCmdNoRenameLayer::Execute()
{
	return m_pLayerList->SetLayerName(m_nLayerID,m_strOldName);
}

CCommand* CCmdNoRenameLayer::GetUndoCommand() const
{
	return new CCmdRenameLayer(m_pDoc, m_pLayer,m_pLayerList,m_nLayerID,m_strOldName,m_strNewName );
}

CCommand* CCmdNoRenameLayer::GetRedoCommand() const
{
	return new CCmdNoRenameLayer(m_pDoc, m_pLayer, m_pLayerList,m_nLayerID,m_strOldName,m_strNewName);
}

void CCmdNoRenameLayer::Sprint(CString &strLabel) const
{
	CLayerCell *pCell = (CLayerCell *)m_pLayerList->GetItemData(m_nLayerID);
	CString strLayerName = pCell->m_strLayerName;
	strLabel = _T("重命名 [") +strLayerName + "]";
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_COMMAND(CCmdNoSetLayerShowFlag, CCmdDraw, CMD_NOSETSHOWFLAGLAYER)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdNoSetLayerShowFlag::CCmdNoSetLayerShowFlag(CGeoMapDoc *pDoc, CDrawLayer *pLayer, 
										   CLayerListCtrl *pLayerList, const int &nLayerID, 
										   const BOOL &bOldFlag, const BOOL &bNewFlag)
										   :CCmdDraw(pDoc)
{
	m_nLayerID = nLayerID;
	m_pLayerList = pLayerList;
	m_bOldFlag = bOldFlag;
	m_bNewFlag = bNewFlag;
	m_pLayer = pLayer;
	m_pLayer->AddRef();
}

CCmdNoSetLayerShowFlag::~CCmdNoSetLayerShowFlag()
{
	m_pLayer->Release();	
}

BOOL CCmdNoSetLayerShowFlag::Execute()
{
	return m_pLayerList->SetLayerShowFlag(m_nLayerID,m_bOldFlag);
}

CCommand* CCmdNoSetLayerShowFlag::GetUndoCommand() const
{
	return new CCmdSetLayerShowFlag(m_pDoc, m_pLayer,m_pLayerList,m_nLayerID,m_bOldFlag,m_bNewFlag );
}

CCommand* CCmdNoSetLayerShowFlag::GetRedoCommand() const
{
	return new CCmdNoSetLayerShowFlag(m_pDoc, m_pLayer, m_pLayerList,m_nLayerID,m_bOldFlag,m_bNewFlag);
}

void CCmdNoSetLayerShowFlag::Sprint(CString &strLabel) const
{
	CLayerCell *pCell = (CLayerCell *)m_pLayerList->GetItemData(m_nLayerID);
	CString strLayerName = pCell->m_strLayerName;
	strLabel = _T("设置显示状态 [") +strLayerName + "]";
}


//////////////////////////////////////////////////////////////////////////
IMPLEMENT_COMMAND(CCmdNoSetLayerLockedFlag, CCmdDraw, CMD_NOSETSHOWFLAGLAYER)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdNoSetLayerLockedFlag::CCmdNoSetLayerLockedFlag(CGeoMapDoc *pDoc, CDrawLayer *pLayer, 
											   CLayerListCtrl *pLayerList, const int &nLayerID, 
											   const BOOL &bOldFlag, const BOOL &bNewFlag)
											   :CCmdDraw(pDoc)
{
	m_nLayerID = nLayerID;
	m_pLayerList = pLayerList;
	m_bOldFlag = bOldFlag;
	m_bNewFlag = bNewFlag;
	m_pLayer = pLayer;
	m_pLayer->AddRef();
}

CCmdNoSetLayerLockedFlag::~CCmdNoSetLayerLockedFlag()
{
	m_pLayer->Release();	
}

BOOL CCmdNoSetLayerLockedFlag::Execute()
{
	return m_pLayerList->SetLayerLockedFlag(m_nLayerID,m_bOldFlag);
}

CCommand* CCmdNoSetLayerLockedFlag::GetUndoCommand() const
{
	return new CCmdSetLayerLockedFlag(m_pDoc, m_pLayer,m_pLayerList,m_nLayerID,m_bOldFlag,m_bNewFlag );
}

CCommand* CCmdNoSetLayerLockedFlag::GetRedoCommand() const
{
	return new CCmdNoSetLayerLockedFlag(m_pDoc, m_pLayer, m_pLayerList,m_nLayerID,m_bOldFlag,m_bNewFlag);
}

void CCmdNoSetLayerLockedFlag::Sprint(CString &strLabel) const
{
	CLayerCell *pCell = (CLayerCell *)m_pLayerList->GetItemData(m_nLayerID);
	CString strLayerName = pCell->m_strLayerName;
	strLabel = _T("设置锁定状态 [") +strLayerName + "]";
}
