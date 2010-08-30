//////////////////////////////////////////////////////////////////////
// SplitPath.cpp : implementation of the CSplitPath class.          //
// (c) 1999, Kevin Lussier                                          //
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SplitPath.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CSplitPath::CSplitPath( LPCTSTR lpszPath )
{
    // Initialize our objects
    memset( path_buffer, 0, sizeof( path_buffer ) );
    memset( drive, 0, sizeof( drive ) );
    memset( dir, 0, sizeof( dir ) );
    memset( fname, 0, sizeof( fname ) );
    memset( ext, 0, sizeof( ext ) );

    // If we were given a path, split it
    if ( lpszPath ) {
        Split( lpszPath );
    }
}

BOOL CSplitPath::Split( LPCTSTR lpszPath )
{
    // If we weren't given a path, fail
    if ( lpszPath == NULL ) {
        // Return failure
        return FALSE;
    }

    // Copy the path
    _tcsncpy_s( path_buffer, lpszPath, sizeof( path_buffer ) - 1 );
    // Split the given path
    _tsplitpath_s( path_buffer, drive, dir, fname, ext );
	
    return TRUE;
}

BOOL CSplitPath::IsExist()
{
	WIN32_FIND_DATA fd;
	
	CString	szFindPath=path_buffer;
	int nSlash = szFindPath.ReverseFind('\\');
	
	if( nSlash == szFindPath.GetLength()-1)
	{
		szFindPath = szFindPath.Left(nSlash);
	}
	
	HANDLE hFind = FindFirstFile( szFindPath, &fd );
	
	if ( hFind != INVALID_HANDLE_VALUE )
	{
		FindClose( hFind );
	}
	
	return hFind != INVALID_HANDLE_VALUE;
}

CString CSplitPath::GetDescription()
{
	CString		szTypeName;
	SHFILEINFO	sfi;
	
	SHGetFileInfo(path_buffer,
		0,
		&sfi, 
		sizeof(SHFILEINFO), 
		SHGFI_TYPENAME);
	
	szTypeName = sfi.szTypeName;
	
	return szTypeName;
}