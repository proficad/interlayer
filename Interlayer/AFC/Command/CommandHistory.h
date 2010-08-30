// CommandHistory.h: interface for the ICommandHistory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMANDHISTORY_H__966D8BCB_9FA2_445D_B1F2_03F859F032C8__INCLUDED_)
#define AFX_COMMANDHISTORY_H__966D8BCB_9FA2_445D_B1F2_03F859F032C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Afxtempl.h>

#define COMMAND_HISTORY_DEPTH 256

class CCommand;


//////////////////////////////////////////////////////////////////////
// ICommandHistory

class AFX_EXT_CLASS ICommandHistory
{
// Construction
public:
	ICommandHistory();
	virtual ~ICommandHistory();

// Operations
protected:
	int NextPos(int iPos) const;
	int PrevPos(int iPos) const;
	void IncrementCount(int& rCount);
	void DecrementCount(int& rCount);
	void IncrementClean();
	void DecrementClean();

	void DeleteHistory();								// Protected, use ResetHistory instead

public:
	virtual BOOL Do(CCommand* pCmd, BOOL bLog = TRUE);	// Execute and log the command
	virtual void LogCommand(CCommand* pCmd);					// Put the command in the ring buffer

	void ResetHistory();									// Reset the command history
	void SetHistorySize(int nHistorySize);			// Set the size of the ring buffer

	virtual void Undo();											// Undo the previous command
	virtual void Redo();											// Redo the previously undone command
	virtual BOOL CanUndo() const;								// Is a command available to be undone?
	virtual BOOL CanRedo() const;								// Is a command available to be redone?
	const CCommand* GetUndoCommand() const;		// Returns the available undo command, if any
	const CCommand* GetRedoCommand() const;		// Returns the available redo command, if any

	void SetClean();
	BOOL IsClean() const;
	BOOL IsDirty() const;

	virtual void OnStatusChange();
	virtual void OnCommandExecute( const CCommand* pCmd);

// Attributes
protected:
	int m_iCommandHistoryUndoPos;

	int m_iCommandHistoryUndoCount;
	int m_iCommandHistoryRedoCount;
	int m_iCommandHistoryCleanCount;

	CTypedPtrArray<CPtrArray, CCommand*> m_commandHistory;
};


#endif // !defined(AFX_COMMANDHISTORY_H__966D8BCB_9FA2_445D_B1F2_03F859F032C8__INCLUDED_)
