// CmdNoActiveLayer.h: interface for the CCmdNoActiveLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CMDNOACTIVELAYER_H__BC143FDE_2925_4B8F_80DB_D10FBFB7066A__INCLUDED_)
#define AFX_CMDNOACTIVELAYER_H__BC143FDE_2925_4B8F_80DB_D10FBFB7066A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CmdDraw.h"
#include "../GeoMap/LayerListCtrl.h"
#include "../GeoMap/DrawLayer.h"

class CCmdNoActiveLayer : public CCmdDraw  
{
	DECLARE_COMMAND(CCmdNoActiveLayer)
public:
	CCmdNoActiveLayer(CGeoMapDoc *pDoc, CDrawLayer *pLayer, CLayerListCtrl *pLayerList, const int &nNoActiveLayerID, const int &nActiveLayerID);
	virtual ~CCmdNoActiveLayer();
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

class CCmdNoRenameLayer : public CCmdDraw  
{
	DECLARE_COMMAND(CCmdNoRenameLayer)
public:
	CCmdNoRenameLayer(CGeoMapDoc *pDoc, CDrawLayer *pLayer, CLayerListCtrl *pLayerList, const int &nLayerID, const CString &strOldName, const CString &strNewName);
	virtual ~CCmdNoRenameLayer();
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

class CCmdNoSetLayerShowFlag : public CCmdDraw  
{
	DECLARE_COMMAND(CCmdNoSetLayerShowFlag)
public:
	CCmdNoSetLayerShowFlag(CGeoMapDoc *pDoc, CDrawLayer *pLayer,
		CLayerListCtrl *pLayerList, const int &nLayerID, 
		const BOOL &bOldFlag, const BOOL &bNewFlag);
	virtual ~CCmdNoSetLayerShowFlag();
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


class CCmdNoSetLayerLockedFlag : public CCmdDraw  
{
	DECLARE_COMMAND(CCmdNoSetLayerLockedFlag)
public:
	CCmdNoSetLayerLockedFlag(CGeoMapDoc *pDoc, CDrawLayer *pLayer,
		CLayerListCtrl *pLayerList, const int &nLayerID, 
		const BOOL &bOldFlag, const BOOL &bNewFlag);
	virtual ~CCmdNoSetLayerLockedFlag();
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

#endif // !defined(AFX_CMDNOACTIVELAYER_H__BC143FDE_2925_4B8F_80DB_D10FBFB7066A__INCLUDED_)
