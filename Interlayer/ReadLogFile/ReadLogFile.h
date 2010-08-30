
#ifndef  _READLOGFILE_H 
#define  _READLOGFILE_H 
#include <afxtempl.h>

BOOL AFX_EXT_API IsTextFile(LPCSTR lpstrFile);
BOOL AFX_EXT_API IsAscFile(LPCSTR lpstrFile);
BOOL AFX_EXT_API Is716Format(LPCSTR name, BOOL &bIsBigEndian, BOOL &bCurves, long &lReadPos);
BOOL AFX_EXT_API IsLasFile(LPCSTR lpstrFile);
BOOL AFX_EXT_API IsListFile(LPCSTR lpstrFile);


BOOL AFX_EXT_API GetFieldsLoging(LPCSTR lpszFileName, CStringArray &arLogingName);

BOOL AFX_EXT_API GetFieldsFromListFile(LPCSTR lpszFileName, CStringArray &arLogingName);
BOOL AFX_EXT_API GetFieldsFromAscFile(LPCSTR lpszFileName, CStringArray &arLogingName);
BOOL AFX_EXT_API GetFieldsFromLasFile(LPCSTR lpszFileName, CStringArray &arLogingName);
BOOL AFX_EXT_API GetFieldsFromTxt(LPCSTR lpszFileName, CStringArray &arLogingName);
BOOL AFX_EXT_API GetFieldsFrom716File(LPCSTR lpszFileName, CStringArray &arLogingName, BOOL bIsBigEndian, BOOL bCurves, long lReadPos);


BOOL AFX_EXT_API GetDatFromFile(LPCSTR lpszFileName, CStringArray &arDats);

BOOL AFX_EXT_API Load716File(const CString &fileName,CStringArray &arDats, BOOL bIsBigEndian, BOOL bCurves, long lReadPos);
BOOL AFX_EXT_API LoadDatTxt(const CString &strFileName, CStringArray &arDats);
BOOL AFX_EXT_API LoadLasFile(const CString &strFileName, CStringArray &arDats);
BOOL AFX_EXT_API LoadListFile( const CString &strFileName, CStringArray &arDats);
BOOL AFX_EXT_API LoadAscFile( const CString &strFileName, CStringArray &arDats);

BOOL AFX_EXT_API GetDatFromFile(LPCSTR lpszFileName, CArray<double, double> &arDats);

BOOL AFX_EXT_API Load716File(const CString &fileName,CArray<double, double> &arDats, BOOL bIsBigEndian, BOOL bCurves, long lReadPos);
BOOL AFX_EXT_API LoadDatTxt(const CString &strFileName, CArray<double, double> &arDats);
BOOL AFX_EXT_API LoadLasFile(const CString &strFileName, CArray<double, double> &arDats);
BOOL AFX_EXT_API LoadListFile( const CString &strFileName, CArray<double, double> &arDats);
BOOL AFX_EXT_API LoadAscFile( const CString &strFileName, CArray<double, double> &arDats);


BOOL AFX_EXT_API WriteLogCSV(LPCSTR lpszFileName, const CStringArray &arLogingName, const CStringArray &arDats);
BOOL AFX_EXT_API WriteLogWIS(LPCSTR lpszFileName, const CStringArray &arLogingName, const CStringArray &arDats);
BOOL AFX_EXT_API WriteLogLAS(LPCSTR lpszFileName, const CStringArray &arLogingName, const CStringArray &arDats);

#endif // READLOGFILE_H 