// Command.h: interface for the CCommand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMAND_H__1D491DD8_4A6C_480F_BE8C_706BEF97728F__INCLUDED_)
#define AFX_COMMAND_H__1D491DD8_4A6C_480F_BE8C_706BEF97728F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <Afxtempl.h>

class ICommandHistory;

#define DECLARE_COMMAND(class_name)\
	DECLARE_DYNAMIC(class_name)\
	private: static const unsigned int m_nID;\
	public: virtual unsigned int GetID() const { return m_nID; } 
#define IMPLEMENT_COMMAND(class_name, base_class_name, cmdID)\
	IMPLEMENT_DYNAMIC(class_name, base_class_name)\
	const unsigned int class_name::m_nID = cmdID; 

#define DECLARE_COMMAND_NOID(class_name)\
	DECLARE_DYNAMIC(class_name)
#define IMPLEMENT_COMMAND_NOID(class_name, base_class_name)\
	IMPLEMENT_DYNAMIC(class_name, base_class_name)

#define PEX_DECLARE_COMMAND_MACRO(class_name)\
	DECLARE_DYNCREATE(class_name)
#define PEX_IMPLEMENT_COMMAND_MACRO(class_name, base_class_name)\
	IMPLEMENT_DYNCREATE(class_name, base_class_name)


////////////////////////////////////////////////////////////////////////
// CCommand

class AFX_EXT_CLASS CCommand : public CObject
{
	DECLARE_DYNAMIC(CCommand)
	public: 
		CCommand(){};
		virtual ~CCommand(){};

		virtual unsigned int GetID() const = 0;

// Operations
public:
	virtual BOOL Execute();
	// Return the inverse command of this one
	virtual CCommand* GetUndoCommand() const = 0;
	// Return a copy of this command
	virtual CCommand* GetRedoCommand() const = 0;
	// Get the name of the command
	virtual void Sprint(CString& strLabel) const = 0;
};


typedef CList<CCommand*, CCommand*> CCommandList;



////////////////////////////////////////////////////////////////////////
// CCommandMacro

class AFX_EXT_CLASS CCommandMacro : public CCommand
{
	DECLARE_DYNAMIC(CCommandMacro);
	public: virtual unsigned int GetID() const;

// Initialization/Destruction
public:
	CCommandMacro(){};
	virtual ~CCommandMacro();

// Operations
public:
	virtual BOOL Execute();
	virtual CCommand* GetUndoCommand() const;
	virtual CCommand* GetRedoCommand() const;

	// Override with the name of this command
	virtual void Sprint(CString& strLabel) const = 0;

// Attributes
public:
	CCommandList m_listCommands;
};


#endif // !defined(AFX_COMMAND_H__1D491DD8_4A6C_480F_BE8C_706BEF97728F__INCLUDED_)
