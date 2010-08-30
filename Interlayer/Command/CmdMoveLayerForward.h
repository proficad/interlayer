// CmdMoveLayerForward.h: interface for the CCmdMoveLayerForward class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CMDMOVELAYERFORWARD_H__DD601D9F_68A8_4C37_9615_6EA3336A16E0__INCLUDED_)
#define AFX_CMDMOVELAYERFORWARD_H__DD601D9F_68A8_4C37_9615_6EA3336A16E0__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "CmdDraw.h"
#include "../GeoMap/DrawLayer.h"
#include "../GeoMap/LayerListCtrl.h"

class CCmdMoveLayerForward : public CCmdDraw  
{
	DECLARE_COMMAND(CCmdMoveLayerForward)
public:
	CCmdMoveLayerForward(CGeoMapDoc *pDoc, CDrawLayer *pLayer,  CLayerListCtrl *pLayerList);
	virtual ~CCmdMoveLayerForward();
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

class CCmdMoveLayerToFront : public CCmdDraw  
{
	DECLARE_COMMAND(CCmdMoveLayerToFront)
public:
	CCmdMoveLayerToFront(CGeoMapDoc *pDoc, CDrawLayer *pLayer,  CLayerListCtrl *pLayerList);
	virtual ~CCmdMoveLayerToFront();
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
#endif // !defined(AFX_CMDMOVELAYERFORWARD_H__DD601D9F_68A8_4C37_9615_6EA3336A16E0__INCLUDED_)
