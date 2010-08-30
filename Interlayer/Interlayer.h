
// Interlayer.h : Interlayer 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号

#define NUM_FILTER 2

// CInterlayerApp:
// 有关此类的实现，请参阅 Interlayer.cpp
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
// 重写
public:
	virtual BOOL InitInstance();

	int LocalToWideChar(LPWSTR pWide, LPCTSTR pLocal, DWORD dwChars);
	HIMAGELIST GetSystemImageList(BOOL fSmall);
// 实现
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void AddToRecentFileList(LPCTSTR lpszPathName);
	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();
	CRecentFileList* m_pRecentFileList[NUM_FILTER];//最近文件列表对象指针数组;
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