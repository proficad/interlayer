// DrawItem.h: interface for the CGeoDrawItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWITEM_H__7E8E96AA_1403_420C_8127_54518F204C5A__INCLUDED_)
#define AFX_DRAWITEM_H__7E8E96AA_1403_420C_8127_54518F204C5A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGeoMapDoc;
class CGeoMapView;
class CGeoDrawOleObj;
/////////////////////////////////////////////////////////////////////////////
// CGeoDrawItem document

class CGeoDrawItem : public COleClientItem
{
	DECLARE_SERIAL(CGeoDrawItem)
		
		// Constructors
public:
	CGeoDrawItem(CGeoMapDoc* pContainer = NULL, CGeoDrawOleObj* pDrawObj = NULL);
	// Note: pContainer is allowed to be NULL to enable IMPLEMENT_SERIALIZE
	//  IMPLEMENT_SERIALIZE requires the class have a constructor with
	//  zero arguments.  Normally, OLE items are constructed with a
	//  non-NULL document pointer.
	
	// Attributes
public:
	CGeoMapDoc* GetDocument()
	{ return (CGeoMapDoc*)COleClientItem::GetDocument(); }
	CGeoMapView* GetActiveView()
	{ return (CGeoMapView*)COleClientItem::GetActiveView(); }
	
	CGeoDrawOleObj* m_pDrawObj;    // back pointer to OLE draw object
	
	// Operations
	BOOL UpdateExtent();
	
	// Implementation
public:
	~CGeoDrawItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual void Serialize(CArchive& ar);
	virtual void OnGetItemPosition(CRect& rPosition);
	
protected:
	virtual void OnChange(OLE_NOTIFICATION wNotification, DWORD dwParam);
	virtual BOOL OnChangeItemPosition(const CRect& rectPos);
	virtual void OnDeactivateUI(BOOL bUndoable);
	virtual void OnActivate();
	virtual BOOL OnShowControlBars(CFrameWnd* pFrameWnd, BOOL bShow);
};


#endif // !defined(AFX_DRAWITEM_H__7E8E96AA_1403_420C_8127_54518F204C5A__INCLUDED_)
