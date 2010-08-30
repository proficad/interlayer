// DispLogCurveView.cpp : implementation file
//

#include "stdafx.h"
#include "DispLogCurveView.h"
#include "resource.h"
#include "DispLogCurveDoc.h"
#include "DlgDepthSetup.h"
#include "DlgAddLane.h"
#include "DlgAddCurve.h"
#include "DlgTemplEdit.h"
#include "DlgTemplSelect.h"
#include "DispLogCurveFrame.h"
#include <math.h>
#include <SYS\STAT.H>
#include "VisualCalc/VCalcParser.h"
#include "VisualCalc/VCalcParserException.h"
#include "../afc/src/SplitPath.h"
#include "../GridCtrl/miniexcel/mexcel.h"
using namespace miniexcel;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern AFX_EXTENSION_MODULE DispLogCurveDLL;

extern int nFormatClip_Curve;
extern int nFormatClip_Sand;


CString ExpandShortcut(CString &inFile)
{
	CString outFile = "";
	
    ASSERT(inFile != _T(""));
	
    IShellLink* psl;
    HRESULT hres;
    LPTSTR lpsz = inFile.GetBuffer(MAX_PATH);
	
    // Create instance for shell link
    hres = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
        IID_IShellLink, (LPVOID*) &psl);
    if (SUCCEEDED(hres))
    {
        // Get a pointer to the persist file interface
        IPersistFile* ppf;
        hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*) &ppf);
        if (SUCCEEDED(hres))
        {
            // Make sure it's ANSI
            WCHAR wsz[MAX_PATH];
            ::MultiByteToWideChar(CP_ACP, 0, lpsz, -1, wsz, MAX_PATH);
			
            // Load shortcut
            hres = ppf->Load(wsz, STGM_READ);
            if (SUCCEEDED(hres)) {
				WIN32_FIND_DATA wfd;
				// find the path from that
				HRESULT hres = psl->GetPath(outFile.GetBuffer(MAX_PATH), 
					MAX_PATH,
					&wfd, 
					SLGP_UNCPRIORITY);
				
				outFile.ReleaseBuffer();
            }
            ppf->Release();
        }
        psl->Release();
    }
	
	inFile.ReleaseBuffer();
	
    return outFile;
}


UINT GetMouseScrollLines()
{
    int nScrollLines = 3;            // reasonable default
	
#ifndef _WIN32_WCE
    // Do things the hard way in win95
    OSVERSIONINFO VersionInfo;
    VersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    if (!GetVersionEx(&VersionInfo) || 
		(VersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS && VersionInfo.dwMinorVersion == 0))
    {
        HKEY hKey;
        if (RegOpenKeyEx(HKEY_CURRENT_USER,  _T("Control Panel\\Desktop"),
            0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
        {
            TCHAR szData[128];
            DWORD dwKeyDataType;
            DWORD dwDataBufSize = sizeof(szData);
            
            if (RegQueryValueEx(hKey, _T("WheelScrollLines"), NULL, &dwKeyDataType,
				(LPBYTE) &szData, &dwDataBufSize) == ERROR_SUCCESS)
            {
                nScrollLines = _tcstoul(szData, NULL, 10);
            }
            RegCloseKey(hKey);
        }
    }
    // win98 or greater
    else
		SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &nScrollLines, 0);
#endif
	
    return nScrollLines;
}

CString GetLogComments(const CString &strCurLogName)
{
	CString strComments = _T("");
	CString strUNIT = _T("");
	
	CString strConnection;
	CWinAppEx * pApp = (CWinAppEx * )AfxGetApp();
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	
	_splitpath_s(pApp->m_pszHelpFilePath, drive, dir, fname, ext);
	
	CString strFilePath = drive;
	strFilePath += dir;
	
	strConnection = _T("Driver={Microsoft Access Driver (*.mdb)};Dbq=");
	strConnection += strFilePath;
	strConnection += "Legend.mdb;";
	strConnection += "Uid=;Pwd=;";
	
	CADODatabase m_pDb;
	if(!m_pDb.Open(strConnection))
		return strComments;
	
	CString strSQL = _T("select COMMENTS,UNIT from [标准测井曲线], [测井曲线对应表] ")
		_T("where [标准测井曲线].LOGINGNAME = [测井曲线对应表].LOGINGNAME ")
		_T("and CURNAME = '") + strCurLogName + _T("'");
	
	CADORecordset m_pRs = CADORecordset(&m_pDb);
	if (m_pRs.Open((LPCTSTR)strSQL))
	{
		if (!m_pRs.IsEOF())
		{				
			m_pRs.GetFieldValue(0,strComments);
			m_pRs.GetFieldValue(1,strUNIT);
			strUNIT.TrimRight();
			strUNIT.TrimLeft();
			if(!strUNIT.IsEmpty())
			{
				strComments += "(";
				strComments += strUNIT;
				strComments += ")";
			}				
		}
	}
	
	if( !strComments.IsEmpty() )
		return strComments;
	
	strSQL = _T("select COMMENTS,UNIT from [标准测井曲线] ")
		_T("where ")
		_T("LOGINGNAME = '") + strCurLogName + _T("'");
	
	if (m_pRs.Open((LPCTSTR)strSQL))
	{
		if (!m_pRs.IsEOF())
		{				
			m_pRs.GetFieldValue(0,strComments);
			m_pRs.GetFieldValue(1,strUNIT);
			strUNIT.TrimRight();
			strUNIT.TrimLeft();
			if(!strUNIT.IsEmpty())
			{
				strComments += "(";
				strComments += strUNIT;
				strComments += ")";
			}				
		}
	}
	
	return strComments;
}

CString GetLogName(const CString &strCurLogName)
{
	CString strLogName = _T("");
	
	CString strConnection;
	CWinAppEx * pApp = (CWinAppEx * )AfxGetApp();
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	
	_splitpath_s(pApp->m_pszHelpFilePath, drive, dir, fname, ext);
	
	CString strFilePath = drive;
	strFilePath += dir;
	
	strConnection = _T("Driver={Microsoft Access Driver (*.mdb)};Dbq=");
	strConnection += strFilePath;
	strConnection += "Legend.mdb;";
	strConnection += "Uid=;Pwd=;";
	
	CADODatabase m_pDb;
	if(!m_pDb.Open(strConnection))	
		return strLogName;
	
	CString strSQL = _T("select LOGINGNAME from [测井曲线对应表] where CURNAME = '") + strCurLogName + "'";
	CADORecordset m_pRs = CADORecordset(&m_pDb);
	if (m_pRs.Open((LPCTSTR)strSQL))
	{
		if (!m_pRs.IsEOF())
			m_pRs.GetFieldValue(0,strLogName);
	}
	
	if( !strLogName.IsEmpty() )
		return strLogName;
	strLogName = strCurLogName;
	
	return strLogName;
}

/////////////////////////////////////////////////////////////////////////////
// CDispLogCurveView

IMPLEMENT_DYNCREATE(CDispLogCurveView, CScrollView)

CDispLogCurveView::CDispLogCurveView()
{
	m_pDragImageList	= NULL;
	m_fZoomScale = 1.0;
	tipCount = MAX_TIP_COUNT;

	m_bLMouseDown = FALSE;
}

CDispLogCurveView::~CDispLogCurveView()
{
}


BEGIN_MESSAGE_MAP(CDispLogCurveView, CScrollView)
	ON_WM_CONTEXTMENU()
	//{{AFX_MSG_MAP(CDispLogCurveView)
	ON_COMMAND(ID_VIEW_SCALE, OnViewScale)
	ON_COMMAND(ID_EXPORT_DISPLOGCURVE, OnExportDisplogcurve)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_DISPLOGCURVE, OnUpdateExportDisplogcurve)
	ON_COMMAND(ID_ZOOM_IN, OnZoomIn)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_IN, OnUpdateZoomIn)
	ON_COMMAND(ID_ZOOM_11, OnZoom11)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_11, OnUpdateZoom11)
	ON_COMMAND(ID_ZOOM_OUT, OnZoomOut)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_OUT, OnUpdateZoomOut)
	ON_COMMAND(ID_DEPTH_SEGMENT, OnDepthSegment)
	ON_UPDATE_COMMAND_UI(ID_DEPTH_SEGMENT, OnUpdateDepthSegment)
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_VSCROLL()
	ON_COMMAND(ID_VIEW_DISPCAPTION, OnViewDispcaption)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DISPCAPTION, OnUpdateViewDispcaption)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_VIEW_LANE_PROP, OnViewLaneProp)
	ON_COMMAND(ID_INSERT_LANE, OnInsertLane)
	ON_COMMAND(ID_APPEND_LANE, OnAppendLane)
	ON_COMMAND(ID_ADD_CURVE, OnAddCurve)
	ON_UPDATE_COMMAND_UI(ID_INSERT_LANE, OnUpdateInsertLane)
	ON_UPDATE_COMMAND_UI(ID_APPEND_LANE, OnUpdateAppendLane)
	ON_UPDATE_COMMAND_UI(ID_ADD_CURVE, OnUpdateAddCurve)
	ON_COMMAND(ID_DEL_CURVE, OnDelCurve)
	ON_UPDATE_COMMAND_UI(ID_DEL_CURVE, OnUpdateDelCurve)
	ON_COMMAND(ID_DEL_LANE, OnDelLane)
	ON_UPDATE_COMMAND_UI(ID_DEL_LANE, OnUpdateDelLane)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_IMPORT_DAT, OnImportDat)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_UPDATE_COMMAND_UI(ID_IMPORT_DAT, OnUpdateImportDat)
	ON_WM_CREATE()
	ON_COMMAND(ID_DISP_TEMPL, OnDispTempl)
	ON_UPDATE_COMMAND_UI(ID_DISP_TEMPL, OnUpdateDispTempl)
	ON_COMMAND(ID_EDIT_DISP_TEMPL, OnEditDispTempl)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DISP_TEMPL, OnUpdateEditDispTempl)
	ON_WM_DROPFILES()

	ON_COMMAND(ID_EDIT_CUT_INTERLAYER, OnEditCutInterLayer)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT_INTERLAYER, OnUpdateEditCutInterLayer)
	ON_COMMAND(ID_EDIT_COPY_INTERLAYER, OnEditCopyInterLayer)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY_INTERLAYER, OnUpdateEditCopyInterLayer)
	ON_COMMAND(ID_EDIT_PASTE_INTERLAYER, OnEditPasteInterLayer)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_INTERLAYER, OnUpdateEditPasteInterLayer)

	//}}AFX_MSG_MAP
	ON_CBN_SELENDOK(ID_VIEW_SCALE, OnViewScale)
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
	ON_WM_GETDLGCODE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDispLogCurveView drawing

void CDispLogCurveView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	DragAcceptFiles(TRUE);

	m_nMapMode = MM_TEXT;
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	CSize sizeTotal = pDoc->m_chartLog.GetChartSize();
	pDoc->UpdateAllViews(NULL,HINT_UPDATE_LOADLOG);
	// TODO: calculate the total size of this view
	CClientDC dc(this);
	OnPrepareDC(&dc,NULL);
	sizeTotal.cx += pDoc->m_chartLog.m_iMargin*2;
	sizeTotal.cy += 100;
	dc.LPtoDP(&sizeTotal);
	SetScrollSizes(MM_TEXT, sizeTotal);

	EnableToolTips(TRUE);
}

void CDispLogCurveView::OnDraw(CDC* pDC)
{
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	CDC dc;
	CDC* pDrawDC = pDC;
	CBitmap bitmap;
	CBitmap* pOldBitmap;
	
	// only paint the rect that needs repainting
	CRect client;
	pDC->GetClipBox(client);
	
	CRect rect = client;
	pDC->LPtoDP(&rect);
	
	if (!pDC->IsPrinting())
	{
		// draw to offscreen bitmap for fast looking repaints
		if (dc.CreateCompatibleDC(pDC))
		{
			if (bitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height()))
			{
				OnPrepareDC(&dc, NULL);
				pDrawDC = &dc;
				
				// offset origin more because bitmap is just piece of the whole drawing
				dc.OffsetViewportOrg(-rect.left, -rect.top);
				pOldBitmap = dc.SelectObject(&bitmap);
				dc.SetBrushOrg(rect.left % 8, rect.top % 8);
				
				// might as well clip to the same rectangle
				dc.IntersectClipRect(client);
			}
		}
	}
	
	// paint background
	CBrush brush;
	if (!brush.CreateSolidBrush(RGB(255, 255, 255)))
		return;
	
	brush.UnrealizeObject();
	pDrawDC->FillRect(client, &brush);	
	
	pDoc->m_chartLog.Draw(pDrawDC, this);
	
	if (pDrawDC != pDC)
	{
		pDC->SetViewportOrg(0, 0);
		pDC->SetWindowOrg(0, 0);
		pDC->SetMapMode(MM_TEXT);
		dc.SetViewportOrg(0, 0);
		dc.SetWindowOrg(0, 0);
		dc.SetMapMode(MM_TEXT);
		pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(),
			&dc, 0, 0, SRCCOPY);
		dc.SelectObject(pOldBitmap);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDispLogCurveView diagnostics

#ifdef _DEBUG
void CDispLogCurveView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CDispLogCurveView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDispLogCurveView message handlers


void CDispLogCurveView::OnViewScale() 
{
	CMFCToolBarComboBoxButton* pSrcCombo = NULL;
	
	CObList listButtons;
	if (CMFCToolBar::GetCommandButtons (ID_VIEW_SCALE, listButtons) > 0)
	{
		for (POSITION posCombo = listButtons.GetHeadPosition (); 
		pSrcCombo == NULL && posCombo != NULL;)
		{
			CMFCToolBarComboBoxButton* pCombo = 
				DYNAMIC_DOWNCAST (CMFCToolBarComboBoxButton, listButtons.GetNext (posCombo));
			
			if (pCombo != NULL && 
				CMFCToolBar::IsLastCommandFromButton (pCombo))
			{
				pSrcCombo = pCombo;
			}
		}
	}
	
	if (pSrcCombo != NULL)
	{
		ASSERT_VALID (pSrcCombo);

		int i = pSrcCombo->GetCurSel();

		CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
		ASSERT_VALID(pDoc);

		pDoc->m_chartLog.m_iCurDispScale = i;
		pDoc->m_chartLog.ReCaleAllRect();

		pDoc->UpdateAllViews(NULL,HINT_UPDATE_CHANGEDSIZE);

		Invalidate(FALSE);

	}
	else
	{
		AfxMessageBox (_T("Show \"Set Active Configuration\" dialog...."));
	}
}

void CDispLogCurveView::OnExportDisplogcurve() 
{
	static char szFilter[] = "逗号分隔符文件 (*.csv)|*.csv|Microsoft Office Excel 工作薄 (*.xls)|*.xls|增强图元文件 (*.EMF)|*.EMF||"; 
	CString strTitle = GetDocument()->GetTitle();
	CFileDialog dlg(FALSE, "*.CSV", strTitle, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, NULL);
	//------用来检测目前的操作系统的版本信息	
	CString strFileName;

	if (dlg.DoModal() == IDOK)
		strFileName = dlg.GetPathName();
	else
		return;
	
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	
	_splitpath_s( strFileName, drive, dir, fname, ext );

	CString strExt = ext;
	strExt.MakeUpper();

	if( strExt == ".EMF")
	{
		CMetaFileDC file;
		CClientDC dc(this);
		dc.m_bPrinting = TRUE;
		OnPrepareDC(&dc);
		
		file.CreateEnhanced(&dc, strFileName, NULL , NULL);
		file.SetAttribDC(dc.m_hDC);
		file.m_bPrinting = TRUE;
		
		CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
		ASSERT_VALID(pDoc);
		pDoc->m_chartLog.Draw(&file, this);
		
		HENHMETAFILE hemf = file.CloseEnhanced();
		file.ReleaseAttribDC();
		::DeleteEnhMetaFile(hemf);
	}
	else if( strExt == ".XLS" )
	{
		CFile file;
		if( !file.Open( strFileName, CFile::modeCreate
			| CFile::modeWrite | CFile::typeBinary ) ) 
		{
			::MessageBox(NULL, "创建文件失败！","提示信息",MB_OK|MB_ICONINFORMATION);
			return;
		}
		
		CMiniExcel miniexcel;

		CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
		ASSERT_VALID(pDoc);

		//str;

		int nCols = pDoc->m_chartLog.m_logDat.GetLogCount();
		for ( int i=0;i<nCols;i++)
		{
			CString str = pDoc->m_chartLog.m_logDat.GetLogName(i);

			CString strTmp = GetLogName(str);
			if( !strTmp.IsEmpty() )
				str = strTmp;

			miniexcel(0,i) = str;
		}

		int nSamples = pDoc->m_chartLog.m_logDat.GetSamples();

		double v;
		for (int j=0; j<nSamples; j++)
		{
			for ( int i=0;i<nCols;i++)
			{
				v = pDoc->m_chartLog.m_logDat.GetValue(i,j);
				miniexcel(j+1,i) = v;
			}
		}

		miniexcel.Write(&file);
	}
	else if( strExt == ".CSV" )
	{
		CStdioFile file;
		CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
		ASSERT_VALID(pDoc);
		
		if( !file.Open( strFileName, CFile::modeCreate
			| CFile::modeWrite | CFile::typeText ) ) 
		{
			::MessageBox(NULL, "创建文件失败！","提示信息",MB_OK|MB_ICONINFORMATION);
			return;
		}

		CString str;
		
		int nCols = pDoc->m_chartLog.m_logDat.GetLogCount();
		for ( int i=0;i<nCols;i++)
		{
			str = pDoc->m_chartLog.m_logDat.GetLogName(i);
			
			CString strTmp = GetLogName(str);
			if( strTmp.IsEmpty() )
				file.WriteString(str);
			else
				file.WriteString(strTmp);

			if( i<nCols-1 )
				file.WriteString(",");
			else
				file.WriteString("\n");
		}
		
		int nSamples = pDoc->m_chartLog.m_logDat.GetSamples();
		
		double v;
		for (int j=0; j<nSamples; j++)
		{
			for ( int i=0;i<nCols;i++)
			{
				v = pDoc->m_chartLog.m_logDat.GetValue(i,j);
				str.Format("%g",v);	
				file.WriteString(str);
				if( i<nCols-1 )
					file.WriteString(",");
				else
					file.WriteString("\n");
			}
		}

		file.Close();
	}
	else
	{
		CStdioFile file;
		CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
		ASSERT_VALID(pDoc);
		
		if( !file.Open( strFileName, CFile::modeCreate
			| CFile::modeWrite | CFile::typeText ) ) 
		{
			::MessageBox(NULL, "创建文件失败！","提示信息",MB_OK|MB_ICONINFORMATION);
			return;
		}
		
		CString str;
		
		int nCols = pDoc->m_chartLog.m_logDat.GetLogCount();
		for ( int i=0;i<nCols;i++)
		{
			str = pDoc->m_chartLog.m_logDat.GetLogName(i);
			file.WriteString(str);
			if( i<nCols-1 )
				file.WriteString("	");
			else
				file.WriteString("\n");
		}
		
		int nSamples = pDoc->m_chartLog.m_logDat.GetSamples();
		
		double v;
		for (int j=0; j<nSamples; j++)
		{
			for ( int i=0;i<nCols;i++)
			{
				v = pDoc->m_chartLog.m_logDat.GetValue(i,j);
				str.Format("%g",v);	
				file.WriteString(str);
				if( i<nCols-1 )
					file.WriteString("	");
				else
					file.WriteString("\n");
			}
		}
		
		file.Close();
	}
	

	ShellExecute(NULL,"open",strFileName,NULL,NULL,SW_SHOW);
}

void CDispLogCurveView::OnUpdateExportDisplogcurve(CCmdUI* pCmdUI) 
{
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable( pDoc->m_chartLog.m_logDat.GetLogCount() > 0 );
}

void CDispLogCurveView::OnZoomIn() 
{
	m_fZoomScale += 0.1f;
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();	
	pDoc->UpdateAllViews(NULL,HINT_UPDATE_CHANGEDSIZE);

	Invalidate(FALSE);
}

void CDispLogCurveView::OnUpdateZoomIn(CCmdUI* pCmdUI) 
{
	if (m_fZoomScale < 10)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CDispLogCurveView::OnZoom11() 
{
	m_fZoomScale = 1.0;
	
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();	
	pDoc->UpdateAllViews(NULL,HINT_UPDATE_CHANGEDSIZE);

	Invalidate(FALSE);
}

void CDispLogCurveView::OnUpdateZoom11(CCmdUI* pCmdUI) 
{
	if (m_fZoomScale != 1.0)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CDispLogCurveView::OnZoomOut() 
{
	m_fZoomScale -= 0.1f;
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();	
	pDoc->UpdateAllViews(NULL,HINT_UPDATE_CHANGEDSIZE);

	Invalidate(FALSE);
}

void CDispLogCurveView::OnUpdateZoomOut(CCmdUI* pCmdUI) 
{
	if (m_fZoomScale > 0.2)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CDispLogCurveView::OnDepthSegment() 
{
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);

	HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(DispLogCurveDLL.hModule);
	
	CDlgDepthSetup dlg;
	dlg.m_dMinVal = pDoc->m_chartLog.m_logDat.GetMinDepth();
	dlg.m_dMaxVal = pDoc->m_chartLog.m_logDat.GetMaxDepth();
	dlg.m_dMinDepth = pDoc->m_chartLog.m_dMinDepth;
	dlg.m_dMaxDepth = pDoc->m_chartLog.m_dMaxDepth;
	if( dlg.DoModal() == IDOK )
	{
		pDoc->m_chartLog.m_dMinDepth = min(dlg.m_dMinDepth,dlg.m_dMaxDepth);
		pDoc->m_chartLog.m_dMaxDepth = max(dlg.m_dMinDepth,dlg.m_dMaxDepth);
		pDoc->m_chartLog.ReCaleAllRect();
		pDoc->UpdateAllViews(NULL,HINT_UPDATE_CHANGEDSIZE);
		Invalidate(FALSE);
	}

	AfxSetResourceHandle(hInstOld);
}

void CDispLogCurveView::OnUpdateDepthSegment(CCmdUI* pCmdUI) 
{
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable( pDoc->m_chartLog.m_logDat.GetLogCount() > 0 );
}

BOOL CDispLogCurveView::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	cs.style |= WS_CLIPCHILDREN;
	return CScrollView::PreCreateWindow(cs);
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CDispLogCurveView::OnBeginDrag
// 返回  : void
// 参数  : BOOL bLane 是否是泳道或是曲线
// 参数  : int iLaneIndex 泳道索引
// 参数  : int iIndex   曲线索引
// 说明  : 
//
void CDispLogCurveView::OnBeginDrag(BOOL bLane, int iLaneIndex, int iIndex, CPoint point)
{
	m_pDragImageList = CreateDragImage(bLane, iLaneIndex, iIndex);
	
	m_pDragImageList->DragEnter(this, point);
	m_pDragImageList->BeginDrag(0, CPoint(0, 0));
}

CImageList* CDispLogCurveView::CreateDragImage(BOOL bLane, int iLaneIndex, int iIndex)
{	
	CClientDC dc(this);
	dc.SetMapMode(MM_ISOTROPIC);
	
	dc.SetWindowExt(254, 254);
	dc.SetViewportExt(100 * m_fZoomScale, 100 * m_fZoomScale);
	CDC* pDC = &dc;
    if (!pDC)
        return NULL;
	
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);

	CLogCurveLane *pLane = (CLogCurveLane *)pDoc->m_chartLog.m_chartLanes.GetAt(iLaneIndex);
	
	CRect rect;
	if( bLane )
	{
		rect = pLane->m_rectLane;
		rect.top = 0;
		rect.bottom = 30;
		pDC->LPtoDP(rect);
	}
	else
	{
		rect.SetRect(0,0,100,50);
	}
    
    // Create a new imagelist (the caller of this function has responsibility
    // for deleting this list)
    CImageList* pList = new CImageList;
    if (!pList || !pList->Create(rect.Width(), rect.Height(), ILC_MASK, 1, 1))
    {    
        if (pList)
            delete pList;
        return NULL;
    }
    
    // Create mem DC and bitmap
    CDC MemDC;
    CBitmap bm;
    MemDC.CreateCompatibleDC(pDC);
    bm.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
    CBitmap* pOldBitmap = MemDC.SelectObject(&bm);
	
	if( bLane )
	{
		MemDC.SetMapMode(MM_ISOTROPIC);
		
		MemDC.SetWindowExt(254, 254);
		MemDC.SetViewportExt(100 * m_fZoomScale, 100 * m_fZoomScale);
		MemDC.SetWindowOrg(0, 0);
		
		MemDC.DPtoLP(rect);
		MemDC.FillSolidRect(rect,::GetSysColor(COLOR_BTNFACE));
		CFont font;
		font.CreateFont(30,0,0,0,0,0,0,0,ANSI_CHARSET,0,0,0,0,"Arial");
		CFont *pOldFont = MemDC.SelectObject(&font);
		CString tmp;
		MemDC.SetBkMode(1);
		
		MemDC.LPtoDP(rect);
		rect.DeflateRect(0,0,1,0);
		MemDC.DPtoLP(rect);
		MemDC.DrawEdge(rect, EDGE_RAISED, BF_RECT);
		MemDC.LPtoDP(rect);
		rect.DeflateRect(0,0,-1,0);
		MemDC.DPtoLP(rect);
		tmp.Format("%d",iLaneIndex+1);
		MemDC.DrawText(tmp,rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		MemDC.SelectObject(pOldFont);
	}
	else
	{
		MemDC.FillSolidRect(rect,RGB(255,255,255));
		CFont font;
		font.CreateFont(15,0,0,0,0,0,0,0,ANSI_CHARSET,0,0,0,0,"Arial");
		CFont *pOldFont = MemDC.SelectObject(&font);
		MemDC.SetTextColor(pLane->m_arLoging[iIndex].m_clrLine);
		rect.DeflateRect(16,0,0,0);
		MemDC.DrawText(pLane->m_arLoging[iIndex].m_strTitle, rect,DT_TOP|DT_LEFT|DT_SINGLELINE);

		MemDC.DrawIcon(CPoint(11,16),AfxGetApp()->LoadIcon(IDI_ICONCURVE));

		MemDC.SelectObject(pOldFont);
	}
    
    // Clean up
    MemDC.SelectObject(pOldBitmap);
    ReleaseDC(pDC);
    
    // Add the bitmap we just drew to the image list.
    pList->Add(&bm, RGB(255,255,255));
    bm.DeleteObject();
	
    return pList;
}

void CDispLogCurveView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	switch (lHint)
	{
	case HINT_UPDATE_CHANGEDSIZE:
		{
			CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
			CSize sizeTotal = pDoc->m_chartLog.GetChartSize();
			// TODO: calculate the total size of this view
			CClientDC dc(this);
			OnPrepareDC(&dc,NULL);
			sizeTotal.cx += pDoc->m_chartLog.m_iMargin*2;
			sizeTotal.cy += 100;
			dc.LPtoDP(&sizeTotal);
			SetScrollSizes(MM_TEXT, sizeTotal);
		}
		break;
	case HINT_UPDATE_LOADLOG:
		{
			CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
			CDispLogCurveFrame *pFrame = (CDispLogCurveFrame*)GetParentFrame();
			pFrame->m_wndWatchBar.m_wndWatch.DeleteAllItems();
			int nCount = pDoc->m_chartLog.m_logDat.GetLogCount();
			for (int i=1;i<nCount;i++)
			{
				CString strTmp;
				strTmp.Format("%d.",i);
				pFrame->m_wndWatchBar.m_wndWatch.InsertItem (i-1, strTmp);
				strTmp = pDoc->m_chartLog.m_logDat.GetLogName(i);
				pFrame->m_wndWatchBar.m_wndWatch.SetItemText (i-1, 1, strTmp);
			}
			break;
		}
	default:
		Invalidate(FALSE);
		break;
	}	
}

BOOL CDispLogCurveView::VertScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll)
{
	// calc new y position
	int y = GetScrollPos(SB_VERT);
	int yOrig = y;
	
	switch (HIBYTE(nScrollCode))
	{
		case SB_TOP:
			y = 0;
			break;
		case SB_BOTTOM:
			y = INT_MAX;
			break;
		case SB_LINEUP:
			y -= m_lineDev.cy;
			break;
		case SB_LINEDOWN:
			y += m_lineDev.cy;
			break;
		case SB_PAGEUP:
			y -= m_pageDev.cy;
			break;
		case SB_PAGEDOWN:
			y += m_pageDev.cy;
			break;
		case SB_THUMBTRACK:
			y = nPos;
			break;
	}
	
	BOOL bResult = VertScrollBy(CSize(0, y - yOrig), bDoScroll);
	//if (bResult && bDoScroll)
	//	UpdateWindow();
	
	return bResult;
}

BOOL CDispLogCurveView::VertScrollBy(CSize sizeScroll, BOOL bDoScroll)
{
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);

	int yOrig, y;
	
	// don't scroll if there is no valid scroll range (ie. no scroll bar)
	CScrollBar* pBar;
	DWORD dwStyle = GetStyle();
	pBar = GetScrollBarCtrl(SB_VERT);
	if ((pBar != NULL && !pBar->IsWindowEnabled()) ||
		(pBar == NULL && !(dwStyle & WS_VSCROLL)))
	{
		// vertical scroll bar not enabled
		sizeScroll.cy = 0;
	}
	
	// adjust current y position
	yOrig = y = GetScrollPos(SB_VERT);
	int yMax = GetScrollLimit(SB_VERT);
	y += sizeScroll.cy;
	if (y < 0)
		y = 0;
	else if (y > yMax)
		y = yMax;
	
	// did anything change?
	if (y == yOrig)
		return FALSE;
	
	if (bDoScroll)
	{
		// do scroll and update scroll positions	
		
		CRect cliRect;
		GetClientRect(cliRect);		
		CClientDC dc(this);
		OnPrepareDC(&dc);
		dc.DPtoLP(&cliRect);
		cliRect.top += pDoc->m_chartLog.GetChartTitleSize().cy + 30;
		dc.LPtoDP(&cliRect);
		cliRect.top += 2;
		
		ScrollWindow(0,  - (y - yOrig), cliRect, cliRect);
		if (y != yOrig)
			SetScrollPos(SB_VERT, y);
	}
	return TRUE;
}

BOOL CDispLogCurveView::DoMouseWheel(UINT fFlags, short zDelta, CPoint point)
{
	UNUSED_ALWAYS(point);
	UNUSED_ALWAYS(fFlags);
	
	DWORD dwStyle = GetStyle();
	CScrollBar* pBar = GetScrollBarCtrl(SB_VERT);
	BOOL bHasVertBar = ((pBar != NULL) && pBar->IsWindowEnabled()) ||
		(dwStyle & WS_VSCROLL);
	
	pBar = GetScrollBarCtrl(SB_HORZ);
	BOOL bHasHorzBar = ((pBar != NULL) && pBar->IsWindowEnabled()) ||
		(dwStyle & WS_HSCROLL);
	
	if (!bHasVertBar && !bHasHorzBar)
		return FALSE;
	
	BOOL bResult = FALSE;
	UINT uWheelScrollLines = GetMouseScrollLines();
	int nToScroll;
	int nDisplacement;
	
	if (bHasVertBar)
	{
		nToScroll = ::MulDiv(-zDelta, uWheelScrollLines, WHEEL_DELTA);
		if (nToScroll == -1 || uWheelScrollLines == WHEEL_PAGESCROLL)
		{
			nDisplacement = m_pageDev.cy;
			if (zDelta > 0)
				nDisplacement = -nDisplacement;
		}
		else
		{
			nDisplacement = nToScroll * m_lineDev.cy;
			nDisplacement = min(nDisplacement, m_pageDev.cy);
		}
		bResult = VertScrollBy(CSize(0, nDisplacement), TRUE);
	}
	else if (bHasHorzBar)
	{
		nToScroll = ::MulDiv(-zDelta, uWheelScrollLines, WHEEL_DELTA);
		if (nToScroll == -1 || uWheelScrollLines == WHEEL_PAGESCROLL)
		{
			nDisplacement = m_pageDev.cx;
			if (zDelta > 0)
				nDisplacement = -nDisplacement;
		}
		else
		{
			nDisplacement = nToScroll * m_lineDev.cx;
			nDisplacement = min(nDisplacement, m_pageDev.cx);
		}
		bResult = OnScrollBy(CSize(nDisplacement, 0), TRUE);
	}
	
//	if (bResult)
//		UpdateWindow();
	
	return bResult;
}

void CDispLogCurveView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	
	pDoc->m_chartLogTool.OnKeyDown(this, nChar, nRepCnt, nFlags);
	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CDispLogCurveView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	
	pDoc->m_chartLogTool.OnLButtonDblClk(this, nFlags, point);
	
	CScrollView::OnLButtonDblClk(nFlags, point);
}

void CDispLogCurveView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	
	pDoc->m_chartLogTool.OnLButtonDown(this, nFlags, point);
	CScrollView::OnLButtonDown(nFlags, point);

	CRect cliRect;
	GetClientRect(cliRect);		
	CClientDC dc(this);
	OnPrepareDC(&dc);
	dc.DPtoLP(&cliRect);
	cliRect.top += pDoc->m_chartLog.GetChartTitleSize().cy + 30;
	dc.LPtoDP(&cliRect);
	cliRect.top += 2;

	if( cliRect.PtInRect(point))
	{
		SetCapture();
		m_bLMouseDown = TRUE;
		m_StartPoint = point;
		m_EndPoint = point;
		
		CClientDC dc(this); //新的DC
		CRect rect = cliRect;		
		int oldmode = dc.SetROP2(R2_NOTXORPEN);
		CBrush brush, * oldbrush;
		brush.CreateSolidBrush(RGB(194, 194, 255));
		oldbrush = dc.SelectObject(&brush);
		rect.top = m_StartPoint.y;
		rect.bottom = point.y;
		rect.NormalizeRect();
		dc.Rectangle(rect);
		dc.SelectObject(oldbrush);
		brush.DeleteObject();
		dc.SetROP2(oldmode);

		long nYPos;
		CPoint pt = point;
		OnPrepareDC(&dc);
		dc.DPtoLP(&pt);
		
		nYPos = pt.y;		
		m_dFirstDepth = pDoc->m_chartLog.GetDepth(nYPos);
	}
	
}

void CDispLogCurveView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);

	pDoc->m_chartLogTool.OnLButtonUp(this, nFlags, point);

	CScrollView::OnLButtonUp(nFlags, point);
	if( m_bLMouseDown )
	{
		ReleaseCapture();
		CClientDC dc(this);
		CRect rect;		
		int oldmode = dc.SetROP2(R2_NOTXORPEN);
		CBrush brush, * oldbrush;
		brush.CreateSolidBrush(RGB(194, 194, 255));
		oldbrush = dc.SelectObject(&brush);
		GetClientRect(rect);
		rect.top = point.y;
		rect.bottom = point.y;
		rect.NormalizeRect();
		dc.Rectangle(rect);
		dc.SelectObject(oldbrush);
		brush.DeleteObject();
		dc.SetROP2(oldmode);
		m_bLMouseDown = FALSE;
		Invalidate(FALSE);	
		
		long nYPos;
		CPoint pt = point;
		OnPrepareDC(&dc);
		dc.DPtoLP(&pt);
		
		nYPos = pt.y;		
		m_dEndDepth = pDoc->m_chartLog.GetDepth(nYPos);

		CDispLogCurveFrame *pFrame = (CDispLogCurveFrame*)GetParentFrame();
		int nCount = pDoc->m_chartLog.m_logDat.GetLogCount();
		for (int i=1;i<nCount;i++)
		{
			CString strTmp;
			if( m_dFirstDepth > m_dEndDepth )
				strTmp.Format("%.2lf - %.2lf",m_dEndDepth, m_dFirstDepth);
			else
				strTmp.Format("%.2lf - %.2lf",m_dFirstDepth,m_dEndDepth );
			pFrame->m_wndWatchBar.m_wndWatch.SetItemText (i-1, 4, strTmp);

			strTmp.Format("%.2lf",pDoc->m_chartLog.GetLogMinValue(i,m_dFirstDepth,m_dEndDepth));
			pFrame->m_wndWatchBar.m_wndWatch.SetItemText (i-1, 5, strTmp);

			strTmp.Format("%.2lf",pDoc->m_chartLog.GetLogMaxValue(i,m_dFirstDepth,m_dEndDepth));
			pFrame->m_wndWatchBar.m_wndWatch.SetItemText (i-1, 6, strTmp);

			strTmp.Format("%.2lf",pDoc->m_chartLog.GetLogAvgValue(i,m_dFirstDepth,m_dEndDepth));
			pFrame->m_wndWatchBar.m_wndWatch.SetItemText (i-1, 7, strTmp);

			strTmp.Format("%.2lf",pDoc->m_chartLog.GetLogAreaValue(i,m_dFirstDepth,m_dEndDepth));
			pFrame->m_wndWatchBar.m_wndWatch.SetItemText (i-1, 8, strTmp);
		}		
	}

}

void CDispLogCurveView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	
	pDoc->m_chartLogTool.OnMouseMove(this, nFlags, point);

	tipCount -= 1;

	if (tipCount <= 0)
	{		
		CRect cliRect;
		GetClientRect(cliRect);
		CClientDC dc(this);
		OnPrepareDC(&dc);
		dc.DPtoLP(&cliRect);
		cliRect.top += pDoc->m_chartLog.GetChartTitleSize().cy + 30;
		dc.LPtoDP(&cliRect);
		cliRect.top += 2;
		
		//tooltip.Activate( cliRect.PtInRect(point) && pDoc->m_chartLog.m_logDat.GetLogCount() > 0 );
		
		long nYPos;
		CPoint pt = point;
		dc.DPtoLP(&pt);
		
		nYPos = pt.y;		
		double dDepth = pDoc->m_chartLog.GetDepth(nYPos);
		
		//CString str;
		//str.Format("井深: %.2lf 米",dDepth);
		//tooltip.UpdateTipText(str, this);
		
		CDispLogCurveFrame *pFrame = (CDispLogCurveFrame*)GetParentFrame();
		int nCount = pDoc->m_chartLog.m_logDat.GetLogCount();
		for (int i=1;i<nCount;i++)
		{
			CString strTmp;
			strTmp.Format("%.2lf",dDepth);
			pFrame->m_wndWatchBar.m_wndWatch.SetItemText (i-1, 2, strTmp);
			strTmp.Format("%.2lf",pDoc->m_chartLog.GetLogValue(i,dDepth));
			pFrame->m_wndWatchBar.m_wndWatch.SetItemText (i-1, 3, strTmp);			
		}

		tipCount = MAX_TIP_COUNT;
	}

	if( m_bLMouseDown )
	{
		CClientDC dc(this);
		int oldmode = dc.SetROP2(R2_NOTXORPEN);
		CBrush brush, * oldbrush;
		CRect rect, rc1, rc2;
		GetClientRect(rect);
		
		rc1 = rect;
		rc2 = rect;
		
		rc1.top = m_StartPoint.y;
		rc1.bottom = m_EndPoint.y;
		
		brush.CreateSolidBrush(RGB(194, 194, 255));
		oldbrush = dc.SelectObject(&brush);
		//抹掉以前的拾取矩形
		dc.Rectangle(rc1);
		rc2.top = m_StartPoint.y;
		rc2.bottom = point.y;
		//绘制当前的拾取矩形
		dc.Rectangle(rc2);
		dc.SelectObject(oldbrush);
		brush.DeleteObject();
		dc.SetROP2(oldmode);
		
		m_EndPoint = point;	
	}
	
	CScrollView::OnMouseMove(nFlags, point);
}

BOOL CDispLogCurveView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	
	return DoMouseWheel(nFlags, zDelta, pt);
}

void CDispLogCurveView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (pScrollBar != NULL && pScrollBar->SendChildNotifyLastMsg())
		return;     // eat it
	
	// ignore scroll bar msgs from other controls
	if (pScrollBar != GetScrollBarCtrl(SB_VERT))
		return;
	
	VertScroll(MAKEWORD(-1, nSBCode), nPos, TRUE);
}

void CDispLogCurveView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (pDC->IsPrinting())
	{
		pDC->SetMapMode(MM_ANISOTROPIC);
		CSize size;
		size.cx = pDC->GetDeviceCaps(LOGPIXELSX);
		size.cy = pDC->GetDeviceCaps(LOGPIXELSY);
		
		pDC->SetWindowExt(254, 254);
		pDC->SetViewportExt(size);
		pDC->SetWindowOrg(0, 0);
		
		if (pInfo )
		{
			CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
			ASSERT_VALID(pDoc);

			CSize sizeTotal = pDoc->m_chartLog.GetChartSize();
			
			sizeTotal.cx += pDoc->m_chartLog.m_iMargin*2;
			sizeTotal.cy += 100;
			
			int hPages = sizeTotal.cx / pDoc->m_sizeDoc.cx  + (sizeTotal.cx%pDoc->m_sizeDoc.cx ? 1:0);
			int vPages = sizeTotal.cy / pDoc->m_sizeDoc.cy  + (sizeTotal.cy%pDoc->m_sizeDoc.cy ? 1:0);
			
			int hPage = (pInfo->m_nCurPage - 1) / vPages;
			int vPage = (pInfo->m_nCurPage - 1)%vPages;
			
			pDC->OffsetWindowOrg(hPage * pDoc->m_sizeDoc.cx, pDoc->m_sizeDoc.cy * vPage);
		}		
	}
	else
	{
		pDC->SetMapMode(MM_ISOTROPIC);
		
		pDC->SetWindowExt(254, 254);
		pDC->SetViewportExt(100 * m_fZoomScale, 100 * m_fZoomScale);
		
		CPoint point = this->GetScrollPosition();
		pDC->OffsetViewportOrg(-point.x, -point.y);
	}
}

BOOL CDispLogCurveView::OnPreparePrinting(CPrintInfo* pInfo) 
{	
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);	
	
	CSize sizeTotal = pDoc->m_chartLog.GetChartSize();
	
	sizeTotal.cx += pDoc->m_chartLog.m_iMargin*2;
	sizeTotal.cy += 100;
	
	int hPages = sizeTotal.cx / pDoc->m_sizeDoc.cx  + (sizeTotal.cx%pDoc->m_sizeDoc.cx ? 1:0);
	int vPages = sizeTotal.cy / pDoc->m_sizeDoc.cy  + (sizeTotal.cy%pDoc->m_sizeDoc.cy ? 1:0);
	
	pInfo->SetMaxPage(hPages * vPages);	

	return DoPreparePrinting(pInfo);
}

void CDispLogCurveView::OnViewDispcaption() 
{
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	
	if(pDoc->m_chartLog.m_bDispCaption )
		pDoc->m_chartLog.m_bDispCaption = FALSE;
	else
		pDoc->m_chartLog.m_bDispCaption = TRUE;
	pDoc->m_chartLogTool.EndEdit();
	pDoc->m_chartLog.ReCaleAllRect();
	pDoc->UpdateAllViews(NULL,HINT_UPDATE_CHANGEDSIZE);
	Invalidate(FALSE);
}

void CDispLogCurveView::OnUpdateViewDispcaption(CCmdUI* pCmdUI) 
{
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	
	pCmdUI->SetRadio(pDoc->m_chartLog.m_bDispCaption);
}

void CDispLogCurveView::OnFilePrintPreview() 
{
	AFXPrintPreview (this);
}

void CDispLogCurveView::OnContextMenu(CWnd*, CPoint point)
{
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	
	if(pDoc->m_chartLogTool.m_pLaneSelected != NULL)
	{		
		if (point.x == -1 && point.y == -1)
		{
			//keystroke invocation
			CRect rect;
			GetClientRect(rect);
			ClientToScreen(rect);

			point = rect.TopLeft();
			point.Offset(5, 5);
		}

		HINSTANCE hInstOld = AfxGetResourceHandle();
		AfxSetResourceHandle(DispLogCurveDLL.hModule);

		CMenu menu;
		VERIFY(menu.LoadMenu(CG_IDR_POPUP_DISP_LOG_CURVE_VIEW));

		AfxSetResourceHandle(hInstOld);

		CMenu* pPopup = menu.GetSubMenu(0);
		ASSERT(pPopup != NULL);
		
		//////////////////////////////////////////////////////////////////////////
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;
		pPopupMenu->SetAutoDestroy (FALSE);
		pPopupMenu->Create (this, point.x, point.y, pPopup->GetSafeHmenu ());
	}
}

void CDispLogCurveView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	
	pDoc->m_chartLogTool.OnRButtonDown(this, nFlags, point);
	CScrollView::OnRButtonDown(nFlags, point);
}

void CDispLogCurveView::OnViewLaneProp() 
{
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->m_chartLogTool.m_pLaneSelected->OnEditProperties(this);
}

void CDispLogCurveView::OnInsertLane() 
{
	HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(DispLogCurveDLL.hModule);

	CDlgAddLane dlg;
	if ( dlg.DoModal() == IDOK )
	{
		if( dlg.m_iSelected == -1 )
			return;
		
		CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
		ASSERT_VALID(pDoc);
		CRect rect = pDoc->m_chartLog.m_rectLane;
		rect.left = 0;
		
		if (dlg.m_iSelected == CLogCurveLane::LOGCURVE)
			rect.right = 500;
// 		else if(dlg.m_iSelected == CLogCurveLane::INTERLAYER)
// 			rect.right = 100;
		else
			rect.right = 100;

		CLogCurveLane *pLane = new CLogCurveLane((CLogCurveLane::LANETYPE)dlg.m_iSelected,&pDoc->m_chartLog,rect);
		pDoc->m_chartLog.InsertOf(pDoc->m_chartLogTool.m_pLaneSelected, pLane, this);

		pDoc->m_chartLog.ReCaleAllRect();
		pDoc->UpdateAllViews(NULL,HINT_UPDATE_CHANGEDSIZE);
	}

	AfxSetResourceHandle(hInstOld);
}

void CDispLogCurveView::OnAppendLane() 
{
	HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(DispLogCurveDLL.hModule);
	
	CDlgAddLane dlg;
	if ( dlg.DoModal() == IDOK )
	{
		if( dlg.m_iSelected == -1 )
			return;
		
		CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
		ASSERT_VALID(pDoc);
		CRect rect = pDoc->m_chartLog.m_rectLane;
		rect.left = 0;
		
		if (dlg.m_iSelected == CLogCurveLane::LOGCURVE )
			rect.right = 500;
// 		else if(dlg.m_iSelected == CLogCurveLane::INTERLAYER)
// 			rect.right = 100;
		else
			rect.right = 100;
		
		CLogCurveLane *pLane = new CLogCurveLane((CLogCurveLane::LANETYPE)dlg.m_iSelected,&pDoc->m_chartLog,rect);
		if( pDoc->m_chartLogTool.m_pLaneSelected != NULL )
			pDoc->m_chartLog.AppendTo(pDoc->m_chartLogTool.m_pLaneSelected, pLane, this);
		else
			pDoc->m_chartLog.Add( pLane, this );
		
		pDoc->m_chartLog.ReCaleAllRect();
		pDoc->UpdateAllViews(NULL,HINT_UPDATE_CHANGEDSIZE);
	}
	
	AfxSetResourceHandle(hInstOld);
}

void CDispLogCurveView::OnAddCurve() 
{
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);

	HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(DispLogCurveDLL.hModule);

	CDlgAddCurve dlg;
	dlg.m_pChartLog = &pDoc->m_chartLog;
	if ( dlg.DoModal() == IDOK )
	{
		int nLog = pDoc->m_chartLog.m_logDat.GetLogCount();		
		for (int i = 0; i < nLog; i++)
		{
			int iIndex = pDoc->m_chartLogTool.m_pLaneSelected->GetCurveCount();
			if( dlg.m_bLogCurve[i] )
			{
				int nSamples = pDoc->m_chartLog.m_logDat.GetSamples();
				CString str = pDoc->m_chartLog.m_logDat.GetLogName(i);
				for (int j=0; j<nSamples; j++)
					pDoc->m_chartLogTool.m_pLaneSelected->Add(iIndex,
					pDoc->m_chartLog.m_logDat.GetDepth(j),
					pDoc->m_chartLog.m_logDat.GetValue(str,j));

				pDoc->m_chartLogTool.m_pLaneSelected->m_arLoging[iIndex].m_dCoordMin 
					= floor(pDoc->m_chartLogTool.m_pLaneSelected->m_arLoging[iIndex].m_dCoordMin);

				pDoc->m_chartLogTool.m_pLaneSelected->m_arLoging[iIndex].m_dCoordMax 
					= ceil(pDoc->m_chartLogTool.m_pLaneSelected->m_arLoging[iIndex].m_dCoordMax);

				long ltmp = pDoc->m_chartLogTool.m_pLaneSelected->m_arLoging[iIndex].m_dCoordMin;

				pDoc->m_chartLogTool.m_pLaneSelected->m_arLoging[iIndex].m_dCoordMin = ltmp/5*5;

				ltmp = pDoc->m_chartLogTool.m_pLaneSelected->m_arLoging[iIndex].m_dCoordMax ;
				pDoc->m_chartLogTool.m_pLaneSelected->m_arLoging[iIndex].m_dCoordMax = (ltmp/5+(ltmp%5?1:0))*5; 

				CString title="";
				
				title = GetLogComments(str);

				if( title.IsEmpty() )
					title = str;
				
				pDoc->m_chartLogTool.m_pLaneSelected->SetLogCurveTitle(iIndex,title);

				pDoc->m_chartLogTool.m_pLaneSelected->SetLogName(iIndex,str);
			}
		}
		if( dlg.m_bCustomCurve ) // 有自定义曲线
		{
			CWaitCursor wait;
			CString strSource = dlg.m_strFormula;
			int nSamples = pDoc->m_chartLog.m_logDat.GetSamples();
			int nLog = pDoc->m_chartLog.m_logDat.GetLogCount();
			int iIndex = pDoc->m_chartLogTool.m_pLaneSelected->GetCurveCount();
			
			for (int i=0; i<nSamples; i++)
			{
				CVCalcParser m_Parser;
				CString strVar, strDest;
				VARIANT valResult;
				BOOL b = FALSE;
				
				try
				{
					for(int j=0; j<nLog; j++ )
					{
						if( pDoc->m_chartLog.m_logDat.GetValue(j,i) < -9999.0 )
						{
							b = TRUE;
							break;
						}
						strVar.Format("var%02d=%g",j,pDoc->m_chartLog.m_logDat.GetValue(j,i));
						m_Parser.Evaluate((LPCTSTR)strVar);
					}
					
					if( b )
						continue;
					
					valResult = m_Parser.Evaluate((LPCTSTR)strSource);			
					pDoc->m_chartLogTool.m_pLaneSelected->Add(iIndex, pDoc->m_chartLog.m_logDat.GetDepth(i), valResult.dblVal);
				}
				
				catch (CSyntaxException& ex) {
					strDest.Format("Syntax error %d : %s", ex.GetExceptionNumber(), ex.GetMessage().c_str());
					::MessageBox(NULL, strDest, _T("错误提示"),MB_OK|MB_ICONERROR);
					return;
				}
				catch (CMathematicException& ex) {
					strDest.Format("Mathematic error %d : %s", ex.GetExceptionNumber(), ex.GetMessage().c_str());
					::MessageBox(NULL, strDest, _T("错误提示"),MB_OK|MB_ICONERROR);
					return;
				}
				catch (CFunctionException& ex) {
					strDest.Format("Function error %d : %s", ex.GetExceptionNumber(), ex.GetMessage().c_str());
					::MessageBox(NULL, strDest, _T("错误提示"),MB_OK|MB_ICONERROR);
					return;
				}
				catch (CParameterException& ex) {
					strDest.Format("Parameter error %d : %s", ex.GetExceptionNumber(), ex.GetMessage().c_str());
					::MessageBox(NULL, strDest, _T("错误提示"),MB_OK|MB_ICONERROR);
					return;
				}
				catch (CVariableException& ex) {
					strDest.Format("Variable error %d : %s", ex.GetExceptionNumber(), ex.GetMessage().c_str());
					::MessageBox(NULL, strDest, _T("错误提示"),MB_OK|MB_ICONERROR);
					return;
				}
				catch (CDomainException& ex) {
					strDest.Format("Domain error %d : %s", ex.GetExceptionNumber(), ex.GetMessage().c_str());
					::MessageBox(NULL, strDest, _T("错误提示"),MB_OK|MB_ICONERROR);
					return;
				}
				catch (CParserException& ex) {
					strDest.Format("Parser error %d : %s", ex.GetExceptionNumber(), ex.GetMessage().c_str());
					::MessageBox(NULL, strDest, _T("错误提示"),MB_OK|MB_ICONERROR);
					return;
				}
				catch (...) {
					strDest.Format("Unknown parser internal error");
					::MessageBox(NULL, strDest, _T("错误提示"),MB_OK|MB_ICONERROR);
					return;
				}
			}
			
			pDoc->m_chartLogTool.m_pLaneSelected->SetLogCurveTitle(iIndex, dlg.m_strCurveName);
			pDoc->m_chartLogTool.m_pLaneSelected->SetLogName(iIndex,"");
		}
		

		Invalidate(FALSE);		
	}

	AfxSetResourceHandle(hInstOld);
}

void CDispLogCurveView::OnUpdateInsertLane(CCmdUI* pCmdUI) 
{
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable(pDoc->m_chartLogTool.m_pLaneSelected != NULL );
}

void CDispLogCurveView::OnUpdateAppendLane(CCmdUI* pCmdUI) 
{
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable(pDoc->m_chartLog.m_logDat.GetSamples() > 0 );
}

void CDispLogCurveView::OnUpdateAddCurve(CCmdUI* pCmdUI) 
{
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable(pDoc->m_chartLogTool.m_pLaneSelected != NULL && pDoc->m_chartLogTool.m_pLaneSelected->m_nLaneType == CLogCurveLane::LOGCURVE);
}

void CDispLogCurveView::OnDelCurve()
{
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	CLogCurveLane *pLane = (CLogCurveLane *)pDoc->m_chartLogTool.m_pLogChart->m_chartLanes.GetAt(pDoc->m_chartLogTool.m_iLaneDrag);
	if( pLane )
		pLane->DelCurve(pDoc->m_chartLogTool.m_pCurveSelected, this);
	
	pDoc->m_chartLogTool.m_pCurveSelected = NULL;
}

void CDispLogCurveView::OnUpdateDelCurve(CCmdUI* pCmdUI)
{
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable(pDoc->m_chartLogTool.m_pCurveSelected != NULL);	
}

void CDispLogCurveView::OnDelLane() 
{
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->m_chartLog.Del(pDoc->m_chartLogTool.m_pLaneSelected, this);
	pDoc->m_chartLogTool.m_pLaneSelected = NULL;
}

void CDispLogCurveView::OnUpdateDelLane(CCmdUI* pCmdUI) 
{
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable(pDoc->m_chartLogTool.m_pLaneSelected != NULL );
}

void CDispLogCurveView::OnEditCopy() 
{
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	if( pDoc->m_chartLogTool.m_pLaneSelected != NULL )
		pDoc->m_chartLogTool.m_pLaneSelected->CopyDataToClipboard((long)pDoc->m_chartLogTool.m_pCurveSelected);
	else
	{
		CMetaFileDC file;
		CClientDC dc(this);
		CSize size;
		size.cx = 2540;
		size.cy = 2540;
		dc.SetMapMode(MM_ISOTROPIC);
		
		dc.HIMETRICtoDP(&size);
		dc.SetWindowExt(2540, -2540);
		dc.SetViewportExt(size);
		dc.SetWindowOrg(0, 0);
		dc.m_bPrinting = TRUE;
		
		file.CreateEnhanced(&dc, NULL, NULL , "ImageName\0Copy\0\0");
		file.SetAttribDC(dc.m_hDC);
		size.cx = 2540;
		size.cy = 2540;
		file.HIMETRICtoDP(&size);
		file.SetMapMode(MM_ISOTROPIC);
		file.SetWindowExt(2540, -2540);
		file.SetViewportExt(size.cx, size.cy);
		file.m_bPrinting = TRUE;
		
		CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
		ASSERT_VALID(pDoc);
		pDoc->m_chartLog.Draw(&file, this);
		
		HENHMETAFILE hemf = file.CloseEnhanced();
		OpenClipboard();
		EmptyClipboard();
		::SetClipboardData(CF_ENHMETAFILE,hemf);
		CloseClipboard();
		file.ReleaseAttribDC();
		::DeleteEnhMetaFile(hemf);		
	}
}

void CDispLogCurveView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	if(pDoc->m_chartLogTool.m_pLaneSelected != NULL )
	{
		switch(pDoc->m_chartLogTool.m_pLaneSelected->m_nLaneType)
		{	
		case CLogCurveLane::DEPTH:					//井深
		case CLogCurveLane::LOGCURVE:					//测井曲线
			pCmdUI->Enable(TRUE);
			break;
		default:
			pCmdUI->Enable(FALSE);
			break;
		}
	}	
	else
		pCmdUI->Enable(TRUE);
}

void CDispLogCurveView::OnEditPaste() 
{
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->m_chartLogTool.m_pLaneSelected->PasteDataFromClipboard();
	Invalidate(FALSE);
	pDoc->SetModifiedFlag();
}

void CDispLogCurveView::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	if(pDoc->m_chartLogTool.m_pLaneSelected != NULL
		&& CountClipboardFormats() > 0 )
	{
		switch(pDoc->m_chartLogTool.m_pLaneSelected->m_nLaneType)
		{	
		case CLogCurveLane::LOGCURVE:					//测井曲线
			if( IsClipboardFormatAvailable(nFormatClip_Curve) )
				pCmdUI->Enable(TRUE);
			else
				pCmdUI->Enable(FALSE);
			break;
		default:
			pCmdUI->Enable(FALSE);
			break;
		}
	}	
	else
		pCmdUI->Enable(FALSE);
}


void CDispLogCurveView::OnEditCut() 
{
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->m_chartLogTool.m_pLaneSelected->CopyDataToClipboard((long)pDoc->m_chartLogTool.m_pCurveSelected);
	switch(pDoc->m_chartLogTool.m_pLaneSelected->m_nLaneType)
	{	
	case CLogCurveLane::LOGCURVE:					//测井曲线
		if(pDoc->m_chartLogTool.m_pCurveSelected!=NULL)
		{
			pDoc->m_chartLogTool.m_pLaneSelected->m_bSelected = FALSE;
			pDoc->m_chartLogTool.m_pCurveSelected->m_bSelected = FALSE;
			pDoc->m_chartLogTool.m_pLaneSelected->DelCurve(pDoc->m_chartLogTool.m_iCurveDrag, this);
			pDoc->m_chartLogTool.m_pCurveSelected = NULL;
			pDoc->m_chartLogTool.m_pLaneSelected = NULL;
		}
		else
		{
			pDoc->m_chartLogTool.m_pLaneSelected->m_bSelected = FALSE;
			pDoc->m_chartLogTool.m_pLaneSelected->DelAllCurve(this);
			pDoc->m_chartLogTool.m_pLaneSelected = NULL;
		}
		
		break;
	default:
		break;
	}

	Invalidate(FALSE);
	pDoc->SetModifiedFlag();
}

void CDispLogCurveView::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	if(pDoc->m_chartLogTool.m_pLaneSelected != NULL )
	{
		switch(pDoc->m_chartLogTool.m_pLaneSelected->m_nLaneType)
		{	
		case CLogCurveLane::LOGCURVE:					//测井曲线
			pCmdUI->Enable(TRUE);
			break;
		default:
			pCmdUI->Enable(FALSE);
			break;
		}
	}
	else
		pCmdUI->Enable(FALSE);
}

void CDispLogCurveView::OnImportDat() 
{
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	pDoc->m_chartLog.LoadLogDat(this);
	ApplyTemplate(_T("Default"), pDoc);
	pDoc->UpdateAllViews(NULL,HINT_UPDATE_LOADLOG);
}

void CDispLogCurveView::OnUpdateImportDat(CCmdUI* pCmdUI) 
{
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable(pDoc->m_chartLog.m_logDat.GetSamples() < 1 );
}

int CDispLogCurveView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	EnableToolTips(TRUE);
    tooltip.Create(this);
    tooltip.Activate(TRUE);
    tooltip.AddTool(this, (LPCTSTR)"");
	
	return 0;
}

BOOL CDispLogCurveView::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	tooltip.RelayEvent(pMsg);
	return CScrollView::PreTranslateMessage(pMsg);
}

void CDispLogCurveView::OnDispTempl() 
{
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	
	HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(DispLogCurveDLL.hModule);
	
	CDlgTemplSelect dlg;
	if( dlg.DoModal() != IDOK )
	{
		AfxSetResourceHandle(hInstOld);
		return;
	}

	CString strTemplName = dlg.m_strTemplName;
	
	CString strSQL;
	CString strConnection;
	CString strTemplate = _T("");
	
	CWinAppEx * pApp = (CWinAppEx * )AfxGetApp();
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	
	_splitpath_s(pApp->m_pszHelpFilePath, drive, dir, fname, ext);
	
	CString strFilePath = drive;
	strFilePath += dir;
	
	strConnection = _T("Driver={Microsoft Access Driver (*.mdb)};Dbq=");
	strConnection += strFilePath;
	strConnection += "Legend.mdb;";
	strConnection += "Uid=;Pwd=;";
	
	CADODatabase m_pDb;
	if(m_pDb.Open(strConnection))
	{	
		CString strSQL = _T("select TEMPLATE from [显示模板] where LB = 1 and MC = '");
		strSQL += strTemplName;
		strSQL += "'";
		
		CADORecordset m_pRs = CADORecordset(&m_pDb);
		if (m_pRs.Open((LPCTSTR)strSQL))
		{
			while (!m_pRs.IsEOF())
			{	
				CString str;
				m_pRs.GetFieldValue(0,strTemplate);
				m_pRs.MoveNext();
			}
		}
	}
	
	if( !strTemplate.IsEmpty() ) // 应用模板
	{
		pDoc->m_chartLog.ApplyTemplate(strTemplate, this);
	}

	AfxSetResourceHandle(hInstOld);	
}

void CDispLogCurveView::OnUpdateDispTempl(CCmdUI* pCmdUI) 
{
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable(pDoc->m_chartLog.m_logDat.GetSamples() > 0 );
}

void CDispLogCurveView::OnEditDispTempl() 
{
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	
	HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(DispLogCurveDLL.hModule);
	
	CDlgTemplEdit dlg;
	if( dlg.DoModal() == IDOK )
	{
		CString strSQL;
		
		CString strConnection;
		
		CWinAppEx * pApp = (CWinAppEx * )AfxGetApp();
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];
		
		_splitpath_s(pApp->m_pszHelpFilePath, drive, dir, fname, ext);
		
		CString strFilePath = drive;
		strFilePath += dir;
		
		strConnection = _T("Driver={Microsoft Access Driver (*.mdb)};Dbq=");
		strConnection += strFilePath;
		strConnection += "Legend.mdb;";
		strConnection += "Uid=;Pwd=;";
		
		CADODatabase m_pDb;
		if(m_pDb.Open(strConnection))
		{	
			if( dlg.m_bIsExist )
			{
				strSQL = "delete from [显示模板] where LB = 1 and mc ='";
				strSQL += dlg.m_strTemplName;
				strSQL += "'";
				m_pDb.Execute(strSQL);
			}
			
			strSQL = "insert into [显示模板] values(1,'";
			strSQL += dlg.m_strTemplName;
			strSQL += "','";
			strSQL += pDoc->m_chartLog.CreateTemplate();
			strSQL += "')";
			m_pDb.Execute(strSQL);
		}
	}
	
	AfxSetResourceHandle(hInstOld);
}

void CDispLogCurveView::OnUpdateEditDispTempl(CCmdUI* pCmdUI) 
{
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->Enable(pDoc->m_chartLog.m_logDat.GetSamples() > 0 );
}


void CDispLogCurveView::OnDropFiles(HDROP hDropInfo) 
{
	// TODO: Add your message handler code here and/or call default
	// 被拖动的项目数
	WORD wNumFilesDropped = DragQueryFile(hDropInfo, -1, NULL, 0);
	

	CString strFile="";
	
	int nFiles = 0;
	// 得到所有项目
	for (WORD x = 0 ; x < wNumFilesDropped; x++)
	{
		
		// 获取字节大小
		WORD wPathnameSize = DragQueryFile(hDropInfo, x, NULL, 0);
		
		// 申请内存
		char * npszFile = (char *) LocalAlloc(LPTR, wPathnameSize += 1);
		
		// 内存不够
		if (npszFile == NULL) continue;
		
		// 拷贝 pathname到缓冲区
		DragQueryFile(hDropInfo, x, npszFile, wPathnameSize);
		
		strFile = npszFile;
		
		// 删除缓存
		LocalFree(npszFile);
		
		// 如果是快捷方式则要获取其完整的路径
		CString expandedFile = ExpandShortcut(strFile);
		
		// 得到一个实际的文件路径
		if (expandedFile!="") 
			strFile=expandedFile;		
		
		struct _stat buf;
		// 获取文件信息
		int result = _stat( strFile, &buf );
		if( result == 0 )
		{
			
			// 如果是子目录
			if ((buf.st_mode & _S_IFDIR) == _S_IFDIR) 
			{
				::MessageBox(NULL, "不能拖动目录!", "提示信息", MB_OK|MB_ICONSTOP);
				return;
			} 
			else if ((buf.st_mode & _S_IFREG) == _S_IFREG)//如果是文件
			{
				CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
				CWaitCursor wait;
				
				if( nFiles == 0 && pDoc->m_chartLog.m_logDat.GetSamples() < 1 )
				{
					CSplitPath path(strFile);
					pDoc->m_chartLog.m_chartCaption.m_strTitle = path.GetFileName();
					pDoc->m_chartLog.m_chartCaption.m_strTitle += _T("测井曲线图");					
					
					pDoc->m_chartLog.LoadLogDat(this, strFile );

					ApplyTemplate(_T("Default"), pDoc);

					pDoc->UpdateAllViews(NULL,HINT_UPDATE_LOADLOG);
				}

				else
				{
					CSplitPath path(strFile);

					CString strTitle = path.GetFileName();
					CWnd *pWnd = AfxGetMainWnd();
					TCHAR strFileName[1024];
					pWnd->SendMessage(WM_CREATE_FILE_LOG,(WPARAM)(LPCTSTR)strTitle, (LPARAM)(LPCTSTR)strFileName);
					
					CDocTemplate* pDocTempl = pDoc->GetDocTemplate();
					CDispLogCurveDoc* pNewDoc = (CDispLogCurveDoc*)pDocTempl->OpenDocumentFile(strFileName);

					if( pNewDoc != NULL )
					{
						POSITION pos = pNewDoc->GetFirstViewPosition();
						CView *pView = pNewDoc->GetNextView(pos);
						
						pNewDoc->m_chartLog.m_chartCaption.m_strTitle = path.GetFileName();
						pNewDoc->m_chartLog.m_chartCaption.m_strTitle += _T("测井曲线图");
						
						pNewDoc->m_chartLog.LoadLogDat(pView, strFile );
						ApplyTemplate(_T("Default"), pNewDoc);
						pNewDoc->UpdateAllViews(NULL,HINT_UPDATE_LOADLOG);
					}					
				}

				nFiles ++;
			}
		}
	}
	
	// Free the memory block containing the dropped-file information
	DragFinish(hDropInfo);

	CScrollView::OnDropFiles(hDropInfo);
}

void CDispLogCurveView::ApplyTemplate(LPCTSTR lpszTempl, CDispLogCurveDoc* pDoc)
{
	CString strTemplName = lpszTempl;

	CString strSQL;
	CString strConnection;
	CString strTemplate = _T("");

	CWinAppEx * pApp = (CWinAppEx * )AfxGetApp();
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath_s(pApp->m_pszHelpFilePath, drive, dir, fname, ext);

	CString strFilePath = drive;
	strFilePath += dir;

	strConnection = _T("Driver={Microsoft Access Driver (*.mdb)};Dbq=");
	strConnection += strFilePath;
	strConnection += "Legend.mdb;";
	strConnection += "Uid=;Pwd=;";

	CADODatabase m_pDb;
	if(m_pDb.Open(strConnection))
	{	
		CString strSQL = _T("select TEMPLATE from [显示模板] where LB = 1 and MC = '");
		strSQL += strTemplName;
		strSQL += "'";

		CADORecordset m_pRs = CADORecordset(&m_pDb);
		if (m_pRs.Open((LPCTSTR)strSQL))
		{
			while (!m_pRs.IsEOF())
			{	
				CString str;
				m_pRs.GetFieldValue(0,strTemplate);
				m_pRs.MoveNext();
			}
		}
	}

	if( !strTemplate.IsEmpty() ) // 应用模板
	{
		pDoc->m_chartLog.ApplyTemplate(strTemplate, this);
	}
}

UINT CDispLogCurveView::OnGetDlgCode()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	UINT nCode = DLGC_WANTALLKEYS; //

	return nCode;
}


void CDispLogCurveView::OnEditCutInterLayer() 
{
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	pDoc->m_chartLog.CutInterLayer();
}

void CDispLogCurveView::OnUpdateEditCutInterLayer(CCmdUI* pCmdUI) 
{
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	if(pDoc->m_chartLog.m_arInterLayer.GetSize()<1 )
	{
		pCmdUI->Enable(FALSE);
	}	
	else
		pCmdUI->Enable(TRUE);
}

void CDispLogCurveView::OnEditCopyInterLayer() 
{
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	pDoc->m_chartLog.CopyInterLayer();
}

void CDispLogCurveView::OnUpdateEditCopyInterLayer(CCmdUI* pCmdUI) 
{
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	ASSERT_VALID(pDoc);
	if(pDoc->m_chartLog.m_arInterLayer.GetSize()<1 )
	{
		pCmdUI->Enable(FALSE);
	}	
	else
		pCmdUI->Enable(TRUE);
}

void CDispLogCurveView::OnEditPasteInterLayer() 
{
	CDispLogCurveDoc* pDoc = (CDispLogCurveDoc*)GetDocument();
	pDoc->m_chartLog.PasteInterLayer();
}

void CDispLogCurveView::OnUpdateEditPasteInterLayer(CCmdUI* pCmdUI) 
{
	if( IsClipboardFormatAvailable(CF_TEXT) )
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}
