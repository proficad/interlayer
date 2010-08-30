
// Interlayer.h : Interlayer Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������

#define NUM_FILTER 2

// CInterlayerApp:
// �йش����ʵ�֣������ Interlayer.cpp
//

class CInterlayerApp : public CWinAppEx
{
public:
	CInterlayerApp();

	CMultiDocTemplate* m_pWellInfoDocTempl;
	CMultiDocTemplate* m_pWellPathInfoDocTempl;
	CMultiDocTemplate* m_pWellLoggingInfoDocTempl;
	CMultiDocTemplate* m_pWellPathDocTempl;
	CMultiDocTemplate* m_pFileDatDocTemplate;
	CMultiDocTemplate* m_pTxtDocTempl;
	CMultiDocTemplate* m_pPictrueDocTempl;
	CMultiDocTemplate* m_p2DModelDocTemplate;
	CMultiDocTemplate* m_pGeoMapDocTemplate;
	CMultiDocTemplate* m_p3DModelDocTemplate;
	CMultiDocTemplate* m_pLayerDocTemplate;
	CMultiDocTemplate* m_pVertModelDocTemplate;

	CMultiDocTemplate* m_pLoggingDocTemplate;
// ��д
public:
	virtual BOOL InitInstance();

	int LocalToWideChar(LPWSTR pWide, LPCTSTR pLocal, DWORD dwChars);
	HIMAGELIST GetSystemImageList(BOOL fSmall);
// ʵ��
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void AddToRecentFileList(LPCTSTR lpszPathName);
	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();
	CRecentFileList* m_pRecentFileList[NUM_FILTER];//����ļ��б����ָ������;
	void SaveStdProfileSettings();
	void LoadStdProfileSettings(UINT nMaxMRU = _AFX_MRU_COUNT);	

	afx_msg void OnAppAbout();
	afx_msg BOOL OnOpenRecentPrjFile(UINT nID);	
	afx_msg void OnUpdateRecentPrjFileMenu(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CInterlayerApp theApp;
extern const char* newGUID();
extern CString GetFileErrMsg(int nErr);
extern void CopyDirectory( LPCSTR lpsrcPath, LPCSTR lpdstPath );
extern int TokenString(LPCTSTR lpszString, 
							LPCTSTR lpszDelimiters,
							CStringArray& saTokens,
							int nMaxTokens = 0,
							BOOL bTrimToken = TRUE,
							BOOL bEnableEscapedChars = TRUE,
							BOOL bEnableDoubleQuote = FALSE,
							BOOL bReturnEmptyToken = FALSE);

extern CString GetExcelDriver();