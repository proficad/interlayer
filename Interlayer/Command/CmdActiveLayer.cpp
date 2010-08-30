// CmdActiveLayer.cpp: implementation of the CCmdActiveLayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CmdActiveLayer.h"
#include "../GeoMap/GeoMapDoc.h"
#include "CmdNoActiveLayer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_COMMAND(CCmdActiveLayer, CCmdDraw, CMD_ACTIVELAYER)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdActiveLayer::CCmdActiveLayer(CGeoMapDoc *pDoc, CDrawLayer *pLayer, CLayerListCtrl *pLayerList, const int &nNoActiveLayerID, const int &nActiveLayerID)
:CCmdDraw(pDoc)
{
	m_nNoActiveLayerID = nNoActiveLayerID;
	m_nActiveLayerID = nActiveLayerID;
	m_pLayerList = pLayerList;
	m_pLayer = pLayer;
	m_pLayer->AddRef();
}

CCmdActiveLayer::~CCmdActiveLayer()
{
	m_pLayer->Release();
}

BOOL CCmdActiveLayer::Execute()
{
	return m_pLayerList->ActiveLayer(m_nActiveLayerID);
}

CCommand* CCmdActiveLayer::GetUndoCommand() const
{
	return new CCmdNoActiveLayer(m_pDoc, m_pLayer,m_pLayerList,m_nNoActiveLayerID,m_nActiveLayerID );
}

CCommand* CCmdActiveLayer::GetRedoCommand() const
{
	return new CCmdActiveLayer(m_pDoc, m_pLayer,m_pLayerList,m_nNoActiveLayerID,m_nActiveLayerID);
}

void CCmdActiveLayer::Sprint(CString &strLabel) const
{
	CLayerCell *pCell = (CLayerCell *)m_pLayerList->GetItemData(m_nActiveLayerID);
	CString strLayerName = pCell->m_strLayerName;
	strLabel = _T("激活 [") +strLayerName + "]";
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_COMMAND(CCmdRenameLayer, CCmdDraw, CMD_RENAMELAYER)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdRenameLayer::CCmdRenameLayer(CGeoMapDoc *pDoc, CDrawLayer *pLayer, CLayerListCtrl *pLayerList, const int &nLayerID, const CString &strOldName, const CString &strNewName)
:CCmdDraw(pDoc)
{
	m_nLayerID = nLayerID;
	m_pLayerList = pLayerList;
	m_strOldName = strOldName;
	m_strNewName = strNewName;
	m_pLayer = pLayer;
	m_pLayer->AddRef();
}

CCmdRenameLayer::~CCmdRenameLayer()
{
	m_pLayer->Release();	
}

BOOL CCmdRenameLayer::Execute()
{
	return m_pLayerList->SetLayerName(m_nLayerID,m_strNewName);
}

CCommand* CCmdRenameLayer::GetUndoCommand() const
{
	return new CCmdNoRenameLayer(m_pDoc, m_pLayer,m_pLayerList,m_nLayerID,m_strOldName,m_strNewName );
}

CCommand* CCmdRenameLayer::GetRedoCommand() const
{
	return new CCmdRenameLayer(m_pDoc, m_pLayer, m_pLayerList,m_nLayerID,m_strOldName,m_strNewName);
}

void CCmdRenameLayer::Sprint(CString &strLabel) const
{
	CLayerCell *pCell = (CLayerCell *)m_pLayerList->GetItemData(m_nLayerID);
	CString strLayerName = pCell->m_strLayerName;
	strLabel = _T("重命名 [") +strLayerName + "]";
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_COMMAND(CCmdSetLayerShowFlag, CCmdDraw, CMD_SETSHOWFLAGLAYER)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdSetLayerShowFlag::CCmdSetLayerShowFlag(CGeoMapDoc *pDoc, CDrawLayer *pLayer, 
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

CCmdSetLayerShowFlag::~CCmdSetLayerShowFlag()
{
	m_pLayer->Release();	
}

BOOL CCmdSetLayerShowFlag::Execute()
{
	return m_pLayerList->SetLayerShowFlag(m_nLayerID,m_bNewFlag);
}

CCommand* CCmdSetLayerShowFlag::GetUndoCommand() const
{
	return new CCmdNoSetLayerShowFlag(m_pDoc, m_pLayer,m_pLayerList,m_nLayerID,m_bOldFlag,m_bNewFlag );
}

CCommand* CCmdSetLayerShowFlag::GetRedoCommand() const
{
	return new CCmdSetLayerShowFlag(m_pDoc, m_pLayer, m_pLayerList,m_nLayerID,m_bOldFlag,m_bNewFlag);
}

void CCmdSetLayerShowFlag::Sprint(CString &strLabel) const
{
	CLayerCell *pCell = (CLayerCell *)m_pLayerList->GetItemData(m_nLayerID);
	CString strLayerName = pCell->m_strLayerName;
	strLabel = _T("设置显示状态 [") +strLayerName + "]";
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_COMMAND(CCmdSetLayerLockedFlag, CCmdDraw, CMD_SETLOCKEDFLAGLAYER)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdSetLayerLockedFlag::CCmdSetLayerLockedFlag(CGeoMapDoc *pDoc, CDrawLayer *pLayer, 
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

CCmdSetLayerLockedFlag::~CCmdSetLayerLockedFlag()
{
	m_pLayer->Release();	
}

BOOL CCmdSetLayerLockedFlag::Execute()
{
	return m_pLayerList->SetLayerLockedFlag(m_nLayerID,m_bNewFlag);
}

CCommand* CCmdSetLayerLockedFlag::GetUndoCommand() const
{
	return new CCmdNoSetLayerLockedFlag(m_pDoc, m_pLayer,m_pLayerList,m_nLayerID,m_bOldFlag,m_bNewFlag );
}

CCommand* CCmdSetLayerLockedFlag::GetRedoCommand() const
{
	return new CCmdSetLayerLockedFlag(m_pDoc, m_pLayer, m_pLayerList,m_nLayerID,m_bOldFlag,m_bNewFlag);
}

void CCmdSetLayerLockedFlag::Sprint(CString &strLabel) const
{
	CLayerCell *pCell = (CLayerCell *)m_pLayerList->GetItemData(m_nLayerID);
	CString strLayerName = pCell->m_strLayerName;
	strLabel = _T("设置锁定状态 [") +strLayerName + "]";
}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_COMMAND(CCmdSetLayerClipFlag, CCmdDraw, CMD_SETLOCKEDFLAGLAYER)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdSetLayerClipFlag::CCmdSetLayerClipFlag(CGeoMapDoc *pDoc, CDrawLayer *pLayer, 
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

CCmdSetLayerClipFlag::~CCmdSetLayerClipFlag()
{
	m_pLayer->Release();	
}

BOOL CCmdSetLayerClipFlag::Execute()
{
	return m_pLayerList->SetLayerClipFlag(m_nLayerID,m_bNewFlag);
}

CCommand* CCmdSetLayerClipFlag::GetUndoCommand() const
{
	return new CCmdSetLayerClipFlag(m_pDoc, m_pLayer,m_pLayerList,m_nLayerID,m_bOldFlag,m_bNewFlag );
}

CCommand* CCmdSetLayerClipFlag::GetRedoCommand() const
{
	return new CCmdSetLayerClipFlag(m_pDoc, m_pLayer, m_pLayerList,m_nLayerID,m_bOldFlag,m_bNewFlag);
}

void CCmdSetLayerClipFlag::Sprint(CString &strLabel) const
{
	CLayerCell *pCell = (CLayerCell *)m_pLayerList->GetItemData(m_nLayerID);
	CString strLayerName = pCell->m_strLayerName;
	strLabel = _T("设置剪切状态 [") +strLayerName + "]";
}
