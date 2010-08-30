// CmdDraw.h: interface for the CCmdDraw class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CMDDRAW_H__0C9677CC_ABF5_4384_BA66_E5F4D5F78589__INCLUDED_)
#define AFX_CMDDRAW_H__0C9677CC_ABF5_4384_BA66_E5F4D5F78589__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGeoMapDoc;

class CCmdDraw : public CCommand  
{
	DECLARE_COMMAND_NOID(CCmdDraw)
public:
	CCmdDraw(CGeoMapDoc* pDoc);
	virtual ~CCmdDraw();
	
protected:
	CGeoMapDoc *m_pDoc;
};
#endif // !defined(AFX_CMDDRAW_H__0C9677CC_ABF5_4384_BA66_E5F4D5F78589__INCLUDED_)
