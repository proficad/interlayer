//////////////////////////////////////////////////////////////////////
// SplitPath.h : interface for the CSplitPath class.                //
// (c) 1999, Kevin Lussier                                          //
//////////////////////////////////////////////////////////////////////

#pragma once

class AFX_EXT_CLASS CSplitPath
{ 
// Construction
public: 
    CSplitPath( LPCTSTR lpszPath = NULL );

// Operations
    BOOL    Split(LPCTSTR lpszPath );
    CString GetPath( void ) { return path_buffer; }	// 路径
    CString GetDrive( void ) { return drive; }		// 驱动器
    CString GetDirectory( void ) { return dir; }	// 目录
    CString GetFileName( void ) { return fname; }	// 文件名
    CString GetExtension( void ) { return ext; }	// 扩展名
	BOOL	IsExist();								// 文件是否存在
	CString GetDescription();						// 文件描述

// Attributes
protected:
    TCHAR path_buffer[_MAX_PATH];
    TCHAR drive[_MAX_DRIVE];
    TCHAR dir[_MAX_DIR];
    TCHAR fname[_MAX_FNAME];
    TCHAR ext[_MAX_EXT];
}; 
