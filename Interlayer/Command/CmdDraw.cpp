// CmdDraw.cpp: implementation of the CCmdDraw class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CmdDraw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_COMMAND_NOID(CCmdDraw, CCommand)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmdDraw::CCmdDraw(CGeoMapDoc* pDoc)
{
	m_pDoc = pDoc;
}

CCmdDraw::~CCmdDraw()
{
	
}

