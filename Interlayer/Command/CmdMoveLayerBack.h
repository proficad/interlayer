// CmdMoveLayerBack.h: interface for the CCmdMoveLayerBack class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CMDMOVELAYERBACK_H__480D2186_B8F8_49C3_8EF9_670F64F001F3__INCLUDED_)
#define AFX_CMDMOVELAYERBACK_H__480D2186_B8F8_49C3_8EF9_670F64F001F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../GeoMap/LayerListCtrl.h"
#include "CmdDraw.h"
#include "../GeoMap/DrawLayer.h"

class CCmdMoveLayerBack : public CCmdDraw  
{
	DECLARE_COMMAND(CCmdMoveLayerBack)
public:
	CCmdMoveLayerBack(CGeoMapDoc *pDoc, CDrawLayer *pLayer,  CLayerListCtrl *pLayerList);
	virtual ~CCmdMoveLayerBack();
public:
	virtual void Sprint(CString& strLabel) const;
	virtual CCommand* GetRedoCommand() const;
	virtual CCommand* GetUndoCommand() const;
	virtual BOOL Execute();
protected:
	CDrawLayer *m_pLayer;
	CLayerListCtrl *m_pLayerList;
	friend class CGeoMapView;
};


class CCmdMoveLayerToBack : public CCmdDraw  
{
	DECLARE_COMMAND(CCmdMoveLayerToBack)
public:
	CCmdMoveLayerToBack(CGeoMapDoc *pDoc, CDrawLayer *pLayer, CLayerListCtrl *pLayerList);
	virtual ~CCmdMoveLayerToBack();
public:
	virtual void Sprint(CString& strLabel) const;
	virtual CCommand* GetRedoCommand() const;
	virtual CCommand* GetUndoCommand() const;
	virtual BOOL Execute();
protected:
	CDrawLayer *m_pLayer;
	CLayerListCtrl *m_pLayerList;
	friend class CGeoMapView;
};


class CCmdAddLayer : public CCmdDraw  
{
	DECLARE_COMMAND(CCmdAddLayer)
public:
	CCmdAddLayer(CGeoMapDoc *pDoc, CDrawLayer *pLayer,  CLayerListCtrl *pLayerList);
	virtual ~CCmdAddLayer();
public:
	virtual void Sprint(CString& strLabel) const;
	virtual CCommand* GetRedoCommand() const;
	virtual CCommand* GetUndoCommand() const;
	virtual BOOL Execute();
protected:
	CDrawLayer *m_pLayer;
	CLayerListCtrl *m_pLayerList;
	friend class CGeoMapView;
};

class CCmdDelLayer : public CCmdDraw  
{
	DECLARE_COMMAND(CCmdDelLayer)
public:
	CCmdDelLayer(CGeoMapDoc *pDoc, CDrawLayer *pLayer,  CLayerListCtrl *pLayerList);
	virtual ~CCmdDelLayer();
public:
	virtual void Sprint(CString& strLabel) const;
	virtual CCommand* GetRedoCommand() const;
	virtual CCommand* GetUndoCommand() const;
	virtual BOOL Execute();
protected:
	CDrawLayer *m_pLayer;
	CLayerListCtrl *m_pLayerList;
	friend class CGeoMapView;
};

#endif // !defined(AFX_CMDMOVELAYERBACK_H__480D2186_B8F8_49C3_8EF9_670F64F001F3__INCLUDED_)
