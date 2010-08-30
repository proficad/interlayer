
// Interlayer.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include <afxdb.h>
#include "odbcinst.h"
#include "afxwinappex.h"
#include "Interlayer.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "ProjectDoc.h"
#include "ProjectView.h"

#include "GridModelDoc.h"

#include "2DModelDoc.h"
#include "2DModelView.h"

#include "WellInfoDoc.h"
#include "WellInfoView.h"

#include "WellPathDoc.h"
#include "WellPathView.h"

#include "WellPathInfoDoc.h"
#include "WellPathInfoView.h"

#include "WellLoggingInfoDoc.h"
#include "WellLoggingInfoView.h"

#include "FileDatDoc.h"
#include "FileDatView.h"

#include "MyEditDoc.h"
#include "MyEditView.h"

#include "MyRichEditView.h"
#include "MyRichEditDoc.h"

#include "PictrueDoc.h"
#include "PictrueView.h"

#include "GeoMap/GeoMapDoc.h"
#include "GeoMap/GeoMapView.h"

#include "FormGenerateLayer.h"
#include "GenerateLayerDoc.h"

#include "Vertical_Model/VertModelDoc.h"
#include "Vertical_Model/VertModelView.h"

#include "3DLib/3DModelDoc.h"
#include "3DLib/3DModelView.h"

#include "DispLogCurve/DispLogCurve.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString GetFileErrMsg(int nErr)
{
	CString strErrMsg = _T("�޷�Ԥ�ϵĴ���!");

	switch (nErr)
	{
	case CFileException::none:
		strErrMsg = _T("No error occurred."); break;
	case CFileException::genericException:
		strErrMsg = _T("An unspecified error occurred."); break;
	case CFileException::fileNotFound:
		strErrMsg = _T("The file could not be located."); break;
	case CFileException::badPath:
		strErrMsg = _T("All or part of the path is invalid."); break;
	case CFileException::tooManyOpenFiles:
		strErrMsg = _T("The permitted number of open files was exceeded."); break;
	case CFileException::accessDenied:
		strErrMsg = _T("The file could not be accessed."); break;
	case CFileException::invalidFile:
		strErrMsg = _T("There was an attempt to use an invalid file handle."); break;
	case CFileException::removeCurrentDir:
		strErrMsg = _T("The current working directory cannot be removed."); break;
	case CFileException::directoryFull:
		strErrMsg = _T("There are no more directory entries."); break;
	case CFileException::badSeek:
		strErrMsg = _T("There was an error trying to set the file pointer."); break;
	case CFileException::hardIO:
		strErrMsg = _T("There was a hardware error."); break;
	case CFileException::sharingViolation:
		strErrMsg = _T("SHARE.EXE was not loaded, or a shared region was locked."); break;
	case CFileException::lockViolation:
		strErrMsg = _T("There was an attempt to lock a region that was already locked."); break;
	case CFileException::diskFull:
		strErrMsg = _T("The disk is full."); break;
	case CFileException::endOfFile:
		strErrMsg = _T("The end of file was reached. "); break;
	};

	return strErrMsg;
}

void CopyDirectory( LPCSTR lpsrcPath, LPCSTR lpdstPath )
{   
	SHFILEOPSTRUCT FileOp;

	FileOp.hwnd   =   NULL;
	FileOp.wFunc   =   FO_COPY;
	FileOp.pFrom   =   lpsrcPath;
	//FileOp.pFrom   =   _T("C:\\Documents and Settings\\Administrator\\My Documents\\aaa_dat\0");
	FileOp.pTo   =   lpdstPath;
	//FileOp.pTo   =   _T("C:\\Documents and Settings\\Administrator\\My Documents\\bbb_dat\0");
	FileOp.fFlags   =   FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI | FOF_SILENT;
	FileOp.hNameMappings   =   NULL;
	FileOp.lpszProgressTitle   =   NULL;

	SHFileOperation(&FileOp);
}

const char* newGUID()
{
	static char buf[64] = {0};
	GUID guid;
	if (S_OK == ::CoCreateGuid(&guid))
	{
		_snprintf_s(buf, sizeof(buf)
			, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}"
			, guid.Data1
			, guid.Data2
			, guid.Data3
			, guid.Data4[0], guid.Data4[1]
		, guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5]
		, guid.Data4[6], guid.Data4[7]
		);
	}
	return (const char*)buf;
}

int TokenString(LPCTSTR lpszString, 
				LPCTSTR lpszDelimiters,
				CStringArray& saTokens,
				int nMaxTokens /*= 0*/,
				BOOL bTrimToken /*= TRUE*/,
				BOOL bEnableEscapedChars /*= TRUE*/,
				BOOL bEnableDoubleQuote /*= TRUE*/,
				BOOL bReturnEmptyToken /*= TRUE*/)
{
	// Note:  normally tokens will be allowed to accumulate;
	//        If you do not want to accumulate, uncomment
	//        the following line

	//saTokens.RemoveAll();

	ASSERT(lpszString && (lpszString[0] != _T('\0')));
	if ((lpszString == NULL) || (lpszString[0] == _T('\0')))
		return 0;

	ASSERT(lpszDelimiters && (lpszDelimiters[0] != _T('\0')));
	if ((lpszDelimiters == NULL) || (lpszDelimiters[0] == _T('\0')))
		return 0;

	BOOL bEndedWithDelim = FALSE;
	const TCHAR * cp = lpszString;

	if (!bReturnEmptyToken)
		cp += _tcsspn(cp, lpszDelimiters);			// skip leading delimiters

	// loop to find all tokens

	while (*cp)
	{
		const TCHAR * pos = NULL;
		CString strToken = _T("");

		// skip leading whitespace;  normally this will not cause a problem,
		// since it is unlikely that you would want to return empty tokens
		// when whitespace is the delimiter
		while ((*cp == _T(' ')) || (*cp == _T('\t')))
			strToken += *cp++;

		BOOL bInQuote = FALSE;

		if (bEnableDoubleQuote && (*cp == _T('"')))
		{
			bInQuote = TRUE;
			++cp;							// skip " char
		}

		// loop to accumulate token
		for (pos = cp; *pos; pos++)
		{
			// check for end of token - either a quote or a delimiter
			if (bInQuote)
			{
				if (*pos == _T('"'))
					break;
			}
			else
			{
				if (_tcsspn(pos, lpszDelimiters) != 0)
					break;
			}

			if (bEnableEscapedChars && (*pos == _T('\\')))
			{
				pos++;						// skip '\'
				if (*pos == _T('\0'))
					break;
			}
			strToken += *pos;
		}
		cp = pos;

		// check if string ended with delimiter
		if (_tcsspn(cp, lpszDelimiters))
			bEndedWithDelim = TRUE;
		else
			bEndedWithDelim = FALSE;

		// skip ending delimiter/quote
		if (*cp)
			cp++;

		if (bInQuote)
		{
			// if quote ended with delimiter, skip it
			if (_tcsspn(cp, lpszDelimiters))
				cp++;
		}

		if (bTrimToken)
		{
			strToken.TrimLeft();
			strToken.TrimRight();
		}

		if ((!strToken.IsEmpty()) || bReturnEmptyToken)
		{
			// either token is not empty OR caller wants empty tokens

			if ((nMaxTokens > 0) && (saTokens.GetSize() >= nMaxTokens))
			{
				break;
			}

			saTokens.Add(strToken);
		}
	}

	// add empty token if caller wants it
	if (bReturnEmptyToken && bEndedWithDelim)
		saTokens.Add(_T(""));

	return saTokens.GetSize();
}


CString GetExcelDriver()
{
	char szBuf[2001];
	WORD cbBufMax = 2000;
	WORD cbBufOut;
	char *pszBuf = szBuf;
	CString sDriver;

	// Get the names of the installed drivers ("odbcinst.h" has to be included )
	if(!SQLGetInstalledDrivers(szBuf, cbBufMax, &cbBufOut))
		return _T("");

	// Search for the driver...
	do
	{
		if( strstr( pszBuf, _T("Excel") ) != 0 )
		{
			// Found !
			sDriver = CString( pszBuf );
			break;
		}
		pszBuf = strchr( pszBuf, '\0' ) + 1;
	}
	while( pszBuf[1] != '\0' );

	return sDriver;
}


void StatusBarMessage(char* fmt, ...)
{
	if (AfxGetApp() != NULL && AfxGetApp()->m_pMainWnd != NULL) {
		char buffer[256];
		CMFCStatusBar* pStatus = (CMFCStatusBar*) 
			AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
		va_list argptr;
		va_start(argptr, fmt);
		vsprintf_s(buffer, fmt, argptr);
		va_end(argptr);
		if (pStatus != NULL)
		{
			pStatus->SetPaneText(0, buffer);
			pStatus->UpdateWindow();
		}
	}
}

void StatusProgress(int nIndex, int nRange=-1)
{
	if (AfxGetApp() != NULL && AfxGetApp()->m_pMainWnd != NULL)
	{
		CMFCStatusBar* pStatus = (CMFCStatusBar*) 
			AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
		if (pStatus != NULL)
		{
			pStatus->EnablePaneProgressBar (nIndex, nRange, TRUE);
		}
	}
}

void StatusSetProgress(int nIndex, int nPos)
{
	if (AfxGetApp() != NULL && AfxGetApp()->m_pMainWnd != NULL)
	{
		CMFCStatusBar* pStatus = (CMFCStatusBar*) 
			AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
		if (pStatus != NULL)
		{
			pStatus->SetPaneProgress (nIndex, nPos);
		}
	}			
}

void SetStatusXY(double x, double y, LPCSTR lpStr)
{
	if (AfxGetApp() != NULL && AfxGetApp()->m_pMainWnd != NULL) {
		char buffer[256];
		CMFCStatusBar* pStatus = (CMFCStatusBar*) 
			AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
		if (pStatus != NULL)
		{
			sprintf_s(buffer,"%s %.2lf, %.2lf",lpStr, x,y);
			pStatus->SetPaneText(3, buffer);
			pStatus->UpdateWindow();
		}
	}
}

void SetStatusCenter(double x, double y, LPCSTR lpStr)
{
	if (AfxGetApp() != NULL && AfxGetApp()->m_pMainWnd != NULL) {
		char buffer[256];
		CMFCStatusBar* pStatus = (CMFCStatusBar*) 
			AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
		if (pStatus != NULL)
		{
			if(strlen(lpStr)>0)
				sprintf_s(buffer,"%s %.2lf, %.2lf",lpStr, x,y);
			else
				sprintf_s(buffer,"ͼ������: , ");
			pStatus->SetPaneText(2, buffer);
			pStatus->UpdateWindow();
		}
	}
}

AFX_STATIC_DATA const TCHAR *_afxFileSection[NUM_FILTER] = {_T("Recent File List"),_T("Recent PRJ File List")};//ע�����"���PRJ�ļ�"��"����ļ�"��������;
AFX_STATIC_DATA const TCHAR *_afxFileEntry[NUM_FILTER] = {_T("File%d"),_T("Prj%d")};//ע�����ÿһ�ļ���ļ���;

// CInterlayerApp

BEGIN_MESSAGE_MAP(CInterlayerApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CInterlayerApp::OnAppAbout)
	// �����ļ��ı�׼�ĵ�����
	// ��׼��ӡ��������
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
	ON_COMMAND_EX_RANGE(ID_FILE_MRU_PRJ1, ID_FILE_MRU_PRJ16, OnOpenRecentPrjFile)
	ON_UPDATE_COMMAND_UI(ID_FILE_MRU_PRJ1, OnUpdateRecentPrjFileMenu)
END_MESSAGE_MAP()


// CInterlayerApp ����

CInterlayerApp::CInterlayerApp()
{
	m_bHiColorIcons			= TRUE;
	//m_hImageSys				= NULL;

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��

	m_pWellInfoDocTempl		= NULL;
	m_pWellPathInfoDocTempl	= NULL;
	m_pWellLoggingInfoDocTempl = NULL;
	m_pWellPathDocTempl		= NULL;
	m_pFileDatDocTemplate	= NULL;
	m_pTxtDocTempl			= NULL;
	m_pPictrueDocTempl		= NULL;
	m_p2DModelDocTemplate	= NULL;
	m_pGeoMapDocTemplate	= NULL;
	m_p3DModelDocTemplate	= NULL;
	m_pLayerDocTemplate		= NULL;
	m_pVertModelDocTemplate	= NULL;

	for(int i=0;i<NUM_FILTER;i++)
		m_pRecentFileList[i]=NULL;
}

// Ψһ��һ�� CInterlayerApp ����

CInterlayerApp theApp;


// CInterlayerApp ��ʼ��

BOOL CInterlayerApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();
	
	// ��ʼ�� OLE ��
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("���������о�Ժ"));
	LoadStdProfileSettings(4);  // ���ر�׼ INI �ļ�ѡ��(���� MRU)

	InitContextMenuManager();
	InitKeyboardManager();	
	InitShellManager ();
	InitTooltipManager();
	
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// ע��Ӧ�ó�����ĵ�ģ�塣�ĵ�ģ��
	// �������ĵ�����ܴ��ں���ͼ֮�������
	/*CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_ProjectTYPE,
		RUNTIME_CLASS(CProjectDoc),
		RUNTIME_CLASS(CChildFrame), // �Զ��� MDI �ӿ��
		RUNTIME_CLASS(CProjectView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
*/
	m_p2DModelDocTemplate = new CMultiDocTemplate(IDR_2DViewTYPE,
		RUNTIME_CLASS(C2DModelDoc),
		RUNTIME_CLASS(CChildFrame), // �Զ��� MDI �ӿ��
		RUNTIME_CLASS(C2DModelView));
	if (!m_p2DModelDocTemplate)
		return FALSE;
	AddDocTemplate(m_p2DModelDocTemplate);

	m_pWellInfoDocTempl = new CMultiDocTemplate(IDR_2DViewTYPE,
		RUNTIME_CLASS(CWellInfoDoc),
		RUNTIME_CLASS(CChildFrame), // �Զ��� MDI �ӿ��
		RUNTIME_CLASS(CWellInfoView));
	if (!m_pWellInfoDocTempl)
		return FALSE;
	AddDocTemplate(m_pWellInfoDocTempl);

	m_pWellPathInfoDocTempl = new CMultiDocTemplate(IDR_2DViewTYPE,
		RUNTIME_CLASS(CWellPathInfoDoc),
		RUNTIME_CLASS(CChildFrame), // �Զ��� MDI �ӿ��
		RUNTIME_CLASS(CWellPathInfoView));
	if (!m_pWellPathInfoDocTempl)
		return FALSE;
	AddDocTemplate(m_pWellPathInfoDocTempl);

	m_pWellPathDocTempl = new CMultiDocTemplate(IDR_2DViewTYPE,
		RUNTIME_CLASS(CWellPathDoc),
		RUNTIME_CLASS(CChildFrame), // �Զ��� MDI �ӿ��
		RUNTIME_CLASS(CWellPathView));
	if (!m_pWellPathDocTempl)
		return FALSE;
	AddDocTemplate(m_pWellPathDocTempl);

	m_pWellLoggingInfoDocTempl = new CMultiDocTemplate(IDR_2DViewTYPE,
		RUNTIME_CLASS(CWellLoggingInfoDoc),
		RUNTIME_CLASS(CChildFrame), // �Զ��� MDI �ӿ��
		RUNTIME_CLASS(CWellLoggingInfoView));
	if (!m_pWellLoggingInfoDocTempl)
		return FALSE;
	AddDocTemplate(m_pWellLoggingInfoDocTempl);

	m_pLoggingDocTemplate = InitDispLogCurveDLL(IDR_2DViewTYPE);

	m_pFileDatDocTemplate = new CMultiDocTemplate(IDR_2DViewTYPE,
		RUNTIME_CLASS(CFileDatDoc),
		RUNTIME_CLASS(CChildFrame), // �Զ��� MDI �ӿ��
		RUNTIME_CLASS(CFileDatView));
	if (!m_pFileDatDocTemplate)
		return FALSE;
	AddDocTemplate(m_pFileDatDocTemplate);

	m_pTxtDocTempl = new CMultiDocTemplate(IDR_2DViewTYPE,
		RUNTIME_CLASS(CMyRichEditDoc),
		RUNTIME_CLASS(CChildFrame), // �Զ��� MDI �ӿ��
		RUNTIME_CLASS(CMyRichEditView));
	if (!m_pTxtDocTempl)
		return FALSE;
	AddDocTemplate(m_pTxtDocTempl);

	m_pPictrueDocTempl = new CMultiDocTemplate(IDR_2DViewTYPE,
		RUNTIME_CLASS(CPictrueDoc),
		RUNTIME_CLASS(CChildFrame), // �Զ��� MDI �ӿ��
		RUNTIME_CLASS(CPictrueView));
	if (!m_pPictrueDocTempl)
		return FALSE;
	AddDocTemplate(m_pPictrueDocTempl);

	m_pGeoMapDocTemplate = new CMultiDocTemplate(IDR_GeoMapTYPE,
		RUNTIME_CLASS(CGeoMapDoc),
		RUNTIME_CLASS(CChildFrame), // �Զ��� MDI �ӿ��
		RUNTIME_CLASS(CGeoMapView));
	if (!m_pGeoMapDocTemplate)
		return FALSE;
	AddDocTemplate(m_pGeoMapDocTemplate);

	m_p3DModelDocTemplate = new CMultiDocTemplate(IDR_3DViewTYPE,
		RUNTIME_CLASS(C3DModelDoc),
		RUNTIME_CLASS(CChildFrame), // �Զ��� MDI �ӿ��
		RUNTIME_CLASS(C3DModelView));
	if (!m_p3DModelDocTemplate)
		return FALSE;
	AddDocTemplate(m_p3DModelDocTemplate);
	
	m_pLayerDocTemplate = new CMultiDocTemplate(IDR_2DViewTYPE,
		RUNTIME_CLASS(CGenerateLayerDoc),
		RUNTIME_CLASS(CChildFrame), // �Զ��� MDI �ӿ��
		RUNTIME_CLASS(CFormGenerateLayer));
	if (!m_pLayerDocTemplate)
		return FALSE;
	AddDocTemplate(m_pLayerDocTemplate);

	m_pVertModelDocTemplate = new CMultiDocTemplate(IDR_2DViewTYPE,
		RUNTIME_CLASS(CVertModelDoc),
		RUNTIME_CLASS(CChildFrame), // �Զ��� MDI �ӿ��
		RUNTIME_CLASS(CVertModelView));
	if (!m_pVertModelDocTemplate)
		return FALSE;
	AddDocTemplate(m_pVertModelDocTemplate);

	// ������ MDI ��ܴ���
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
	// �������к�׺ʱ�ŵ��� DragAcceptFiles
	//  �� MDI Ӧ�ó����У���Ӧ������ m_pMainWnd ֮����������
	// ������/��
	m_pMainWnd->DragAcceptFiles();

	// ���á�DDE ִ�С�
	//EnableShellOpen();
	//RegisterShellFileTypes(TRUE);

	// ������׼������DDE�����ļ�������������
//	CCommandLineInfo cmdInfo;
//	ParseCommandLine(cmdInfo);


	// ��������������ָ����������
	// �� /RegServer��/Register��/Unregserver �� /Unregister ����Ӧ�ó����򷵻� FALSE��
//	if (!ProcessShellCommand(cmdInfo))
//		return FALSE;
	// �������ѳ�ʼ���������ʾ����������и���
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();	

	CString strProjectFile = GetProfileString ("Settings","Project File","");

	strProjectFile.Trim();
	if( !strProjectFile.IsEmpty() && PathFileExists(strProjectFile))
		pMainFrame->OpenProjectFile(strProjectFile);

	return TRUE;
}


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// �������жԻ����Ӧ�ó�������
void CInterlayerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CInterlayerApp �Զ������/���淽��

void CInterlayerApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_TREE);
}

void CInterlayerApp::LoadCustomState()
{
}

void CInterlayerApp::SaveCustomState()
{
}

// CInterlayerApp ��Ϣ�������

int CInterlayerApp::ExitInstance()
{	
	// TODO: �ڴ����ר�ô����/����û���
	//�����׼����(��Ҫ������ļ��б�);
	SaveStdProfileSettings();

	for(int i=0;i<NUM_FILTER;i++)
	{
		if(m_pRecentFileList[i])
			delete m_pRecentFileList[i];
	}

	return CWinAppEx::ExitInstance();
}

HIMAGELIST CInterlayerApp::GetSystemImageList(BOOL fSmall)
{	
	/*TCHAR szWinDir [MAX_PATH + 1];
	if (GetWindowsDirectory (szWinDir, MAX_PATH) == 0)
	{
		return NULL;
	}

	SHFILEINFO sfi;
	HIMAGELIST hImageList = (HIMAGELIST) SHGetFileInfo (szWinDir,
		0,
		&sfi, 
		sizeof (SHFILEINFO), 
		SHGFI_SYSICONINDEX | (fSmall ? SHGFI_SMALLICON : SHGFI_LARGEICON));
	return hImageList;
	*/

	HIMAGELIST  himl;
	SHFILEINFO  sfi;

	himl = (HIMAGELIST)SHGetFileInfo(TEXT("C:\\"), 0, &sfi,
		sizeof(SHFILEINFO), SHGFI_SYSICONINDEX |
		(fSmall ? SHGFI_SMALLICON : SHGFI_LARGEICON));

	/*
	Do a version check first because you only need to use this code on
	Windows NT version 4.0.
	*/ 
	OSVERSIONINFO vi;
	vi.dwOSVersionInfoSize = sizeof(vi);
	GetVersionEx(&vi);
	if(VER_PLATFORM_WIN32_WINDOWS == vi.dwPlatformId)
		return himl;

	/*
	You need to create a temporary, empty .lnk file that you can use to
	pass to IShellIconOverlay::GetOverlayIndex. You could just enumerate
	down from the Start Menu folder to find an existing .lnk file, but
	there is a very remote chance that you will not find one. By creating
	your own, you know this code will always work.
	*/ 
	HRESULT           hr;
	IShellFolder      *psfDesktop = NULL;
	IShellFolder      *psfTempDir = NULL;
	IMalloc           *pMalloc = NULL;
	LPITEMIDLIST      pidlTempDir = NULL;
	LPITEMIDLIST      pidlTempFile = NULL;
	TCHAR             szTempDir[MAX_PATH];
	TCHAR             szTempFile[MAX_PATH] = TEXT("");
	TCHAR             szFile[MAX_PATH];
	HANDLE            hFile;
	int               i;
	OLECHAR           szOle[MAX_PATH];
	DWORD             dwAttributes;
	DWORD             dwEaten;
	IShellIconOverlay *psio = NULL;
	int               nIndex;

	// Get the desktop folder.
	hr = SHGetDesktopFolder(&psfDesktop);
	if(FAILED(hr))
		goto exit;

	// Get the shell's allocator.
	hr = SHGetMalloc(&pMalloc);
	if(FAILED(hr))
		goto exit;

	// Get the TEMP directory.
	if(!GetTempPath(MAX_PATH, szTempDir))
	{
		/*
		There might not be a TEMP directory. If this is the case, use the
		Windows directory.
		*/ 
		if(!GetWindowsDirectory(szTempDir, MAX_PATH))
		{
			hr = E_FAIL;
			goto exit;
		}
	}

	// Create a temporary .lnk file.
	if(szTempDir[lstrlen(szTempDir) - 1] != '\\')
		lstrcat(szTempDir, TEXT("\\"));
	for(i = 0, hFile = INVALID_HANDLE_VALUE;
		INVALID_HANDLE_VALUE == hFile;
		i++)
	{
		lstrcpy(szTempFile, szTempDir);
		wsprintf(szFile, TEXT("temp%d.lnk"), i);
		lstrcat(szTempFile, szFile);

		hFile = CreateFile(  szTempFile,
			GENERIC_WRITE,
			0,
			NULL,
			CREATE_NEW,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		// Do not try this more than 100 times.
		if(i > 100)
		{
			hr = E_FAIL;
			goto exit;
		}
	}

	// Close the file you just created.
	CloseHandle(hFile);
	hFile = INVALID_HANDLE_VALUE;

	// Get the PIDL for the directory.
	LocalToWideChar(szOle, szTempDir, MAX_PATH);
	hr = psfDesktop->ParseDisplayName(  NULL,
		NULL,
		szOle,
		&dwEaten,
		&pidlTempDir,
		&dwAttributes);
	if(FAILED(hr))
		goto exit;

	// Get the IShellFolder for the TEMP directory.
	hr = psfDesktop->BindToObject(   pidlTempDir,
		NULL,
		IID_IShellFolder,
		(LPVOID*)&psfTempDir);
	if(FAILED(hr))
		goto exit;

	/*
	Get the IShellIconOverlay interface for this folder. If this fails,
	it could indicate that you are running on a pre-Internet Explorer 4.0
	shell, which doesn't support this interface. If this is the case, the
	overlay icons are already in the system image list.
	*/ 
	hr = psfTempDir->QueryInterface(IID_IShellIconOverlay, (LPVOID*)&psio);
	if(FAILED(hr))
		goto exit;

	// Get the PIDL for the temporary .lnk file.
	LocalToWideChar(szOle, szFile, MAX_PATH);
	hr = psfTempDir->ParseDisplayName(  NULL,
		NULL,
		szOle,
		&dwEaten,
		&pidlTempFile,
		&dwAttributes);
	if(FAILED(hr))
		goto exit;

	/*
	Get the overlay icon for the .lnk file. This causes the shell
	to put all of the standard overlay icons into your copy of the system
	image list.
	*/ 
	hr = psio->GetOverlayIndex(pidlTempFile, &nIndex);

exit:
	// Delete the temporary file.
	DeleteFile(szTempFile);

	if(psio)
		psio->Release();

	if(INVALID_HANDLE_VALUE != hFile)
		CloseHandle(hFile);

	if(psfTempDir)
		psfTempDir->Release();

	if(pMalloc)
	{
		if(pidlTempFile)
			pMalloc->Free(pidlTempFile);

		if(pidlTempDir)
			pMalloc->Free(pidlTempDir);

		pMalloc->Release();
	}

	if(psfDesktop)
		psfDesktop->Release();

	return himl;
}

int CInterlayerApp::LocalToWideChar(LPWSTR pWide, LPCTSTR pLocal, DWORD dwChars)
{
	*pWide = 0;

#ifdef UNICODE
	lstrcpyn(pWide, pLocal, dwChars);
#else
	MultiByteToWideChar( CP_ACP, 0, pLocal, -1, pWide, dwChars); 
#endif

	return lstrlenW(pWide);
}

/*********************************************************************
 * ˵��:����������ĵ�
 * ��ڲ���:
 * UINT nID -- ����򿪹����ĵ���ID
 * ����ֵ:
 * BOOL  -- 
 * ����: �ž�(ZhangJun)  
*********************************************************************/
BOOL CInterlayerApp::OnOpenRecentPrjFile(UINT nID)
{
	ASSERT_VALID(this);
	ASSERT(m_pRecentFileList[1] != NULL);

	ASSERT(nID >= ID_FILE_MRU_PRJ1);
	ASSERT(nID < ID_FILE_MRU_PRJ1 + (UINT)m_pRecentFileList[1]->GetSize());
	int nIndex = nID - ID_FILE_MRU_PRJ1;
	ASSERT((*m_pRecentFileList[1])[nIndex].GetLength() != 0);

	TRACE2("MRU: open file (%d) '%s'.\n", (nIndex) + 1,
			(LPCTSTR)(*m_pRecentFileList[1])[nIndex]);
    
	//����Ӧ�Ĺ����ļ�;
	//if (OpenDocumentFile((*m_pRecentFileList[1])[nIndex]) == NULL)
	CString strProjectFile = (*m_pRecentFileList[1])[nIndex];

	strProjectFile.Trim();
	if( !strProjectFile.IsEmpty() && PathFileExists(strProjectFile))
	{
		CMainFrame *pMainFrame = (CMainFrame *)AfxGetMainWnd();
		if( !pMainFrame->OpenProjectFile(strProjectFile) )
			m_pRecentFileList[1]->Remove(nIndex);
	}
	if( strProjectFile.IsEmpty() )
		m_pRecentFileList[1]->Remove(nIndex);

	return TRUE;
}

/*********************************************************************
 * ˵��:��������򿪹����ĵ��˵�
 * ��ڲ���:
 * CCmdUI* pCmdUI -- 
 * ����ֵ:
 * void  -- 
 * ����: �ž�(ZhangJun)
*********************************************************************/
void CInterlayerApp::OnUpdateRecentPrjFileMenu(CCmdUI* pCmdUI) 
{
	ASSERT_VALID(this);

	if(pCmdUI->m_pSubMenu!=NULL)//��ʱ����"File"�˵�����ʾ;
	{
		
		BOOL bEnable=FALSE;
		if (m_pRecentFileList[1] != NULL)
			{
				bEnable=TRUE;
			}
		 pCmdUI->m_pMenu->EnableMenuItem(pCmdUI->m_nIndex, MF_BYPOSITION | 
                (bEnable ? MF_ENABLED : (MF_DISABLED | MF_GRAYED)));      
         return;	
	
	}
	else    //��ʱ�����ļ��б�������ʾ;
		m_pRecentFileList[1]->UpdateMenu(pCmdUI);

}



/*********************************************************************
 * ˵��:װ�ر�׼����(��Ҫ������ļ��б�);
 * ��ڲ���:
 * UINT nMaxMRU -- 
 * ����ֵ:
 * void  -- 
 * ����: �ž�(ZhangJun)
*********************************************************************/
void CInterlayerApp::LoadStdProfileSettings(UINT nMaxMRU)
{
	ASSERT_VALID(this);
	
    for(int i=0;i<NUM_FILTER;i++)
		ASSERT(m_pRecentFileList[i]==NULL);
	
	if (nMaxMRU != 0)
	{
		// ��ÿһ����չ������CRecentFileList����,����ReadList����ԭ�ȵ�����ļ��б�;
		for(int i=0;i<NUM_FILTER;i++)
		{
			m_pRecentFileList[i] = new CRecentFileList(0, _afxFileSection[i], _afxFileEntry[i],
				nMaxMRU);
			m_pRecentFileList[i]->ReadList();
		}
		
	}
}

/*********************************************************************
 * ˵��:�����׼����(��Ҫ������ļ��б�);
 * ��ڲ���:
 * ����ֵ:
 * void  -- 
 * ����: �ž�(ZhangJun)
*********************************************************************/
void CInterlayerApp::SaveStdProfileSettings()
{
	ASSERT_VALID(this);
	
	for(int i=0;i<NUM_FILTER;i++)
	{
		if (m_pRecentFileList[i] != NULL)
			m_pRecentFileList[i]->WriteList();  //����ע�����;
	}
}


/*********************************************************************
 * ˵��:���������ļ����˵��б�
 * ��ڲ���:
 * LPCTSTR lpszPathName -- 
 * ����ֵ:
 * void  -- 
 * ����: �ž�(ZhangJun)
*********************************************************************/
void CInterlayerApp::AddToRecentFileList(LPCTSTR lpszPathName) 
{
	ASSERT_VALID(this);
	ASSERT(lpszPathName != NULL);
	ASSERT(AfxIsValidString(lpszPathName));
    
	CString strFilterExt;
    
	strFilterExt=_T(".Prj"); // ��չ��ΪPRj 

	LPCTSTR lpszDot=_tcsrchr(lpszPathName,'.');
	//��������ļ�����չ������strFilterExt,������빤���ļ�������ļ��б�;
	if (lpszDot != NULL && lstrcmpi(lpszDot, strFilterExt) == 0)
	{
		if (m_pRecentFileList[1] != NULL)
		{
			m_pRecentFileList[1]->Add(lpszPathName);
			
		}
	}
	else //��������ļ�����չ��������strFilterExt,�������ҳ���ļ�������ļ��б�;
	{
		CString tmp = lpszDot;
		tmp.MakeUpper();		
		if (m_pRecentFileList[0] != NULL)
		{
			m_pRecentFileList[0]->Add(lpszPathName);			
		}		
	}	
}
