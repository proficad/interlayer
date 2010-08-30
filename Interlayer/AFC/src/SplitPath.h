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
    CString GetPath( void ) { return path_buffer; }	// ·��
    CString GetDrive( void ) { return drive; }		// ������
    CString GetDirectory( void ) { return dir; }	// Ŀ¼
    CString GetFileName( void ) { return fname; }	// �ļ���
    CString GetExtension( void ) { return ext; }	// ��չ��
	BOOL	IsExist();								// �ļ��Ƿ����
	CString GetDescription();						// �ļ�����

// Attributes
protected:
    TCHAR path_buffer[_MAX_PATH];
    TCHAR drive[_MAX_DRIVE];
    TCHAR dir[_MAX_DIR];
    TCHAR fname[_MAX_FNAME];
    TCHAR ext[_MAX_EXT];
}; 
