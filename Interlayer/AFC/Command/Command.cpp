
// Command.cpp: implementation of the CCommand class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Command.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////////////
// CCommand
// 

IMPLEMENT_DYNAMIC(CCommand, CObject)


BOOL CCommand::Execute()
{ 
	return TRUE; 
}


const unsigned int COMMAND_ID(CCommand* pCmd)
{ 
	return pCmd->GetID(); 
}


////////////////////////////////////////////////////////////////////////
//
// CCommandMacro
//

IMPLEMENT_DYNAMIC(CCommandMacro, CCommand);

// dtor deletes it's command list
CCommandMacro::~CCommandMacro()
{
	while (!m_listCommands.IsEmpty())
		delete m_listCommands.RemoveHead();
}

// Execute each of the commands
BOOL CCommandMacro::Execute()
{
	// Don't log if there are no commands in the macro
	if (m_listCommands.IsEmpty())
		return FALSE;

	for (POSITION pos = m_listCommands.GetHeadPosition(); pos != NULL; )
		m_listCommands.GetNext(pos)->Execute();
	return TRUE;
}

// Make a copy of this command with it's list containing undo's for each command
CCommand* CCommandMacro::GetUndoCommand() const
{
	CCommandMacro* pCmd = STATIC_DOWNCAST(CCommandMacro, GetRuntimeClass()->CreateObject());
	for (POSITION pos = m_listCommands.GetTailPosition(); pos != NULL; )
		pCmd->m_listCommands.AddTail(m_listCommands.GetPrev(pos)->GetUndoCommand());
	return pCmd;
}

// Make a copy of this command
CCommand* CCommandMacro::GetRedoCommand() const
{
	CCommandMacro* pCmd = STATIC_DOWNCAST(CCommandMacro, GetRuntimeClass()->CreateObject());
	for (POSITION pos = m_listCommands.GetHeadPosition(); pos != NULL; )
		pCmd->m_listCommands.AddTail(m_listCommands.GetNext(pos)->GetRedoCommand());
	return pCmd;
}

// Command macro's don't need an ID as the command history will
// call OnCommandExecute for each command in the macro.
unsigned int CCommandMacro::GetID() const
{
	return (unsigned int)-1;
}