// CmdActiveLayer.h: interface for the CCmdActiveLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CMDACTIVELAYER_H__0E028AA2_6F69_4C18_BF9A_D2AF797414BE__INCLUDED_)
#define AFX_CMDACTIVELAYER_H__0E028AA2_6F69_4C18_BF9A_D2AF797414BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CmdDraw.h"
#include "../GeoMap/LayerListCtrl.h"
#include "../GeoMap/DrawLayer.h"

class CCmdActiveLayer : public CCmdDraw  
{
	DECLARE_COMMAND(CCmdActiveLayer)
public:
	CCmdActiveLayer(CGeoMapDoc *pDoc, CDrawLayer *pLayer, CLayerListCtrl *pLayerList, const int &nNoActiveLayerID, const int &nActiveLayerID);
	virtual ~CCmdActiveLayer();
public:
	virtual void Sprint(CString& strLabel) const;
	virtual CCommand* GetRedoCommand() const;
	virtual CCommand* GetUndoCommand() const;
	virtual BOOL Execute();
protected:
	CDrawLayer *m_pLayer;
	CLayerListCtrl *m_pLayerList;
	int	m_nActiveLayerID;	// º§ªÓÕº≤„ID∫≈
	int	m_nNoActiveLayerID;	// ≤ªº§ªÓÕº≤„ID∫≈
	friend class CGeoMapView;
};


class CCmdRenameLayer : public CCmdDraw  
{
	DECLARE_COMMAND(CCmdRenameLayer)
public:
	CCmdRenameLayer(CGeoMapDoc *pDoc, CDrawLayer *pLayer, CLayerListCtrl *pLayerList, const int &nLayerID, const CString &strOldName, const CString &strNewName);
	virtual ~CCmdRenameLayer();
public:
	virtual void Sprint(CString& strLabel) const;
	virtual CCommand* GetRedoCommand() const;
	virtual CCommand* GetUndoCommand() const;
	virtual BOOL Execute();
protected:
	CDrawLayer *m_pLayer;
	CLayerListCtrl *m_pLayerList;
	CString m_strOldName;
	CString m_strNewName;
	int	m_nLayerID;	// Õº≤„ID∫≈
	friend class CGeoMapView;
};

class CCmdSetLayerShowFlag : public CCmdDraw  
{
	DECLARE_COMMAND(CCmdSetLayerShowFlag)
public:
	CCmdSetLayerShowFlag(CGeoMapDoc *pDoc, CDrawLayer *pLayer,
		CLayerListCtrl *pLayerList, const int &nLayerID, 
		const BOOL &bOldFlag, const BOOL &bNewFlag);
	virtual ~CCmdSetLayerShowFlag();
public:
	virtual void Sprint(CString& strLabel) const;
	virtual CCommand* GetRedoCommand() const;
	virtual CCommand* GetUndoCommand() const;
	virtual BOOL Execute();
protected:
	CDrawLayer *m_pLayer;
	CLayerListCtrl *m_pLayerList;
	BOOL m_bOldFlag;
	BOOL m_bNewFlag;
	int	m_nLayerID;	// Õº≤„ID∫≈
	friend class CGeoMapView;
};

class CCmdSetLayerLockedFlag : public CCmdDraw  
{
	DECLARE_COMMAND(CCmdSetLayerLockedFlag)
public:
	CCmdSetLayerLockedFlag(CGeoMapDoc *pDoc, CDrawLayer *pLayer,
		CLayerListCtrl *pLayerList, const int &nLayerID, 
		const BOOL &bOldFlag, const BOOL &bNewFlag);
	virtual ~CCmdSetLayerLockedFlag();
public:
	virtual void Sprint(CString& strLabel) const;
	virtual CCommand* GetRedoCommand() const;
	virtual CCommand* GetUndoCommand() const;
	virtual BOOL Execute();
protected:
	CDrawLayer *m_pLayer;
	CLayerListCtrl *m_pLayerList;
	BOOL m_bOldFlag;
	BOOL m_bNewFlag;
	int	m_nLayerID;	// Õº≤„ID∫≈
	friend class CGeoMapView;
};

class CCmdSetLayerClipFlag : public CCmdDraw  
{
	DECLARE_COMMAND(CCmdSetLayerClipFlag)
public:
	CCmdSetLayerClipFlag(CGeoMapDoc *pDoc, CDrawLayer *pLayer,
		CLayerListCtrl *pLayerList, const int &nLayerID, 
		const BOOL &bOldFlag, const BOOL &bNewFlag);
	virtual ~CCmdSetLayerClipFlag();
public:
	virtual void Sprint(CString& strLabel) const;
	virtual CCommand* GetRedoCommand() const;
	virtual CCommand* GetUndoCommand() const;
	virtual BOOL Execute();
protected:
	CDrawLayer *m_pLayer;
	CLayerListCtrl *m_pLayerList;
	BOOL m_bOldFlag;
	BOOL m_bNewFlag;
	int	m_nLayerID;	// Õº≤„ID∫≈
	friend class CGeoMapView;
};


#endif // !defined(AFX_CMDACTIVELAYER_H__0E028AA2_6F69_4C18_BF9A_D2AF797414BE__INCLUDED_)
