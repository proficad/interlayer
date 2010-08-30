// CustomCommandHistory.cpp: implementation of the ICustomCommandHistory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CustomCommandHistory.h"
#include "Command.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ICustomCommandHistory::ICustomCommandHistory()
{

}

ICustomCommandHistory::~ICustomCommandHistory()
{

}

void ICustomCommandHistory::OnStatusChange()
{
	//static_cast<CGeoMapDoc*>(this)->UpdateTitle();
}

void ICustomCommandHistory::OnCommandExecute( const CCommand *pCmd)
{
	CString strLabel;
	pCmd->Sprint(strLabel);

	CString str;
	str.Format(_T("×îºó²Ù×÷: %s"), strLabel);
	STATIC_DOWNCAST(CFrameWnd, AfxGetMainWnd())->SetMessageText(str);

	//static_cast<CDrawDoc*>(this)->UpdateAllViews(NULL, 0, STATIC_DOWNCAST(CObject, pCmd));
}
