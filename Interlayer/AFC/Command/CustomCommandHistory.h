// CustomCommandHistory.h: interface for the ICustomCommandHistory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CUSTOMCOMMANDHISTORY_H__84071CA9_12F6_4F7E_A525_ECFB1B5CA2A0__INCLUDED_)
#define AFX_CUSTOMCOMMANDHISTORY_H__84071CA9_12F6_4F7E_A525_ECFB1B5CA2A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CommandHistory.h"

class AFX_EXT_CLASS ICustomCommandHistory : public ICommandHistory  
{
public:
	virtual void OnCommandExecute( const CCommand* pCmd);
	virtual void OnStatusChange();
	ICustomCommandHistory();
	virtual ~ICustomCommandHistory();

};

#endif // !defined(AFX_CUSTOMCOMMANDHISTORY_H__84071CA9_12F6_4F7E_A525_ECFB1B5CA2A0__INCLUDED_)
