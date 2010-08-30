// DispLogCurve.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxdllx.h>

#include "Resource.h"
#include "DispLogCurveFrame.h"
#include "DispLogCurveView.h"
#include "DispLogCurveDoc.h"
#include "DispLogCurve.h"

#include "../afc/src/SplitPath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

AFX_EXTENSION_MODULE DispLogCurveDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("DISPLOGCURVE.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(DispLogCurveDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(DispLogCurveDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("DISPLOGCURVE.DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(DispLogCurveDLL);
	}
	return 1;   // ok
}

extern "C" CMultiDocTemplate* WINAPI InitDispLogCurveDLL(UINT nIDResource)
{
	CWinApp* pApp = AfxGetApp();
	ASSERT(pApp != NULL);

	CMultiDocTemplate *pTemplate = new CMultiDocTemplate
		(nIDResource,
		RUNTIME_CLASS(CDispLogCurveDoc),
		RUNTIME_CLASS(CDispLogCurveFrame), // custom MDI child frame
		RUNTIME_CLASS(CDispLogCurveView));

	pApp->AddDocTemplate(pTemplate);

	return pTemplate;
}

extern "C" void LoadLogFile(CDocument *pDoc, LPCTSTR lpszLogDatFile)
{
	POSITION pos = pDoc->GetFirstViewPosition();
	CDispLogCurveDoc *p = (CDispLogCurveDoc *)pDoc;
	CDispLogCurveView *pView = (CDispLogCurveView *)pDoc->GetNextView(pos);
	CString strFileName = lpszLogDatFile;
	CSplitPath path(strFileName);
	p->m_chartLog.m_chartCaption.m_strTitle = path.GetFileName();
	p->m_chartLog.m_chartCaption.m_strTitle += _T("²â¾®ÇúÏßÍ¼");
	p->m_chartLog.LoadLogDat(pView, strFileName);
	pView->ApplyTemplate(_T("Default"), p);
	pDoc->UpdateAllViews(NULL,HINT_UPDATE_LOADLOG);
}