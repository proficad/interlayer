// GeoPictrueObj.cpp: implementation of the CGeoPictrueObj class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GeoPictrueObj.h"
#include "GeoMapView.h"
#include "DlgRectSet.h"
#include "CmdGeoDrawObj.h"
#include "RgnClipping.h"
#include "../MainFrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



DocType doctypes[CMAX_IMAGE_FORMATS] =
{
	{ -1, TRUE, FALSE, "所有支持图像文件 (*.bmp;*.gif;*.jpg;*.jpeg;*.png;*.ico;*.tif;*.tiff;*.tga;*.pcx;*.wbmp;*.wmf;*.emf;*.j2k;*.jp2;*.jbg;*.j2c;*.jpc;*.pgx;*.pnm;*.pgm;*.ppm;*.ras)", "*.bmp;*.gif;*.jpg;*.jpeg;*.png;*.ico;*.tif;*.tiff;*.tga;*.pcx;*.wbmp;*.wmf;*.emf;*.j2k;*.jp2;*.jbg;*.j2c;*.jpc;*.pgx;*.pnm;*.pgm;*.ppm;*.ras" },
#if CXIMAGE_SUPPORT_BMP
	{ CXIMAGE_FORMAT_BMP, TRUE, TRUE, "BMP 文件(*.bmp)", "*.bmp" },
#endif
#if CXIMAGE_SUPPORT_GIF
	{ CXIMAGE_FORMAT_GIF, TRUE, TRUE, "GIF 文件(*.gif)", "*.gif" },
#endif
#if CXIMAGE_SUPPORT_JPG
	{ CXIMAGE_FORMAT_JPG, TRUE, TRUE, "JPG 文件(*.jpg;*.jpeg)", "*.jpg;*.jpeg" },
#endif
#if CXIMAGE_SUPPORT_PNG
	{ CXIMAGE_FORMAT_PNG, TRUE, TRUE, "PNG 文件(*.png)", "*.png" },
#endif
#if CXIMAGE_SUPPORT_MNG
	{ CXIMAGE_FORMAT_MNG, TRUE, TRUE, "MNG 文件(*.mng;*.jng;*.png)", "*.mng;*.jng;*.png" },
#endif
#if CXIMAGE_SUPPORT_ICO
	{ CXIMAGE_FORMAT_ICO, TRUE, TRUE, "ICO CUR 文件(*.ico;*.cur)", "*.ico;*.cur" },
#endif
#if CXIMAGE_SUPPORT_TIF
	{ CXIMAGE_FORMAT_TIF, TRUE, TRUE, "TIF 文件(*.tif;*.tiff)", "*.tif;*.tiff" },
#endif
#if CXIMAGE_SUPPORT_TGA
	{ CXIMAGE_FORMAT_TGA, TRUE, TRUE, "TGA 文件(*.tga)", "*.tga" },
#endif
#if CXIMAGE_SUPPORT_PCX
	{ CXIMAGE_FORMAT_PCX, TRUE, TRUE, "PCX 文件(*.pcx)", "*.pcx" },
#endif
#if CXIMAGE_SUPPORT_WBMP
	{ CXIMAGE_FORMAT_WBMP, TRUE, TRUE, "WBMP 文件(*.wbmp)", "*.wbmp" },
#endif
#if CXIMAGE_SUPPORT_WMF
	{ CXIMAGE_FORMAT_WMF, TRUE, FALSE, "WMF EMF 文件(*.wmf;*.emf)", "*.wmf;*.emf" },
#endif
#if CXIMAGE_SUPPORT_J2K
	{ CXIMAGE_FORMAT_J2K, TRUE, TRUE, "J2K 文件(*.j2k;*.jp2)", "*.j2k;*.jp2" },
#endif
#if CXIMAGE_SUPPORT_JBG
	{ CXIMAGE_FORMAT_JBG, TRUE, TRUE, "JBG 文件(*.jbg)", "*.jbg" },
#endif
#if CXIMAGE_SUPPORT_JP2
	{ CXIMAGE_FORMAT_JP2, TRUE, TRUE, "JP2 文件(*.j2k;*.jp2)", "*.j2k;*.jp2" },
#endif
#if CXIMAGE_SUPPORT_JPC
	{ CXIMAGE_FORMAT_JPC, TRUE, TRUE, "JPC 文件(*.j2c;*.jpc)", "*.j2c;*.jpc" },
#endif
#if CXIMAGE_SUPPORT_PGX
	{ CXIMAGE_FORMAT_PGX, TRUE, TRUE, "PGX 文件(*.pgx)", "*.pgx" },
#endif
#if CXIMAGE_SUPPORT_RAS
	{ CXIMAGE_FORMAT_RAS, TRUE, TRUE, "RAS 文件(*.ras)", "*.ras" },
#endif
#if CXIMAGE_SUPPORT_PNM
	{ CXIMAGE_FORMAT_PNM, TRUE, TRUE, "PNM 文件(*.pnm;*.pgm;*.ppm)", "*.pnm;*.pgm;*.ppm" }
#endif
	
};



CString FindExtension(const CString& name)
{
	int len = name.GetLength();
	int i;
	for (i = len-1; i >= 0; i--)
	{
		if (name[i] == '.')
		{
			return name.Mid(i+1);
		}
	}
	return CString("");
}

//////////////////////////////////////////////////////////////////////////////
int FindType(const CString& ext)
{
	int type = 0;
	if (ext == "bmp")					type = CXIMAGE_FORMAT_BMP;
#if CXIMAGE_SUPPORT_JPG
	else if (ext=="jpg"||ext=="jpeg")	type = CXIMAGE_FORMAT_JPG;
#endif
#if CXIMAGE_SUPPORT_GIF
	else if (ext == "gif")				type = CXIMAGE_FORMAT_GIF;
#endif
#if CXIMAGE_SUPPORT_PNG
	else if (ext == "png")				type = CXIMAGE_FORMAT_PNG;
#endif
#if CXIMAGE_SUPPORT_MNG
	else if (ext=="mng"||ext=="jng")	type = CXIMAGE_FORMAT_MNG;
#endif
#if CXIMAGE_SUPPORT_ICO
	else if (ext == "ico")				type = CXIMAGE_FORMAT_ICO;
#endif
#if CXIMAGE_SUPPORT_TIF
	else if (ext=="tiff"||ext=="tif")	type = CXIMAGE_FORMAT_TIF;
#endif
#if CXIMAGE_SUPPORT_TGA
	else if (ext=="tga")				type = CXIMAGE_FORMAT_TGA;
#endif
#if CXIMAGE_SUPPORT_PCX
	else if (ext=="pcx")				type = CXIMAGE_FORMAT_PCX;
#endif
#if CXIMAGE_SUPPORT_WBMP
	else if (ext=="wbmp")				type = CXIMAGE_FORMAT_WBMP;
#endif
#if CXIMAGE_SUPPORT_WMF
	else if (ext=="wmf"||ext=="emf")	type = CXIMAGE_FORMAT_WMF;
#endif
#if CXIMAGE_SUPPORT_J2K
	else if (ext=="j2k"||ext=="jp2")	type = CXIMAGE_FORMAT_J2K;
#endif
#if CXIMAGE_SUPPORT_JBG
	else if (ext=="jbg")				type = CXIMAGE_FORMAT_JBG;
#endif
#if CXIMAGE_SUPPORT_JP2
	else if (ext=="jp2"||ext=="j2k")	type = CXIMAGE_FORMAT_JP2;
#endif
#if CXIMAGE_SUPPORT_JPC
	else if (ext=="jpc"||ext=="j2c")	type = CXIMAGE_FORMAT_JPC;
#endif
#if CXIMAGE_SUPPORT_PGX
	else if (ext=="pgx")				type = CXIMAGE_FORMAT_PGX;
#endif
#if CXIMAGE_SUPPORT_RAS
	else if (ext=="ras")				type = CXIMAGE_FORMAT_RAS;
#endif
#if CXIMAGE_SUPPORT_PNM
	else if (ext=="pnm"||ext=="pgm"||ext=="ppm") type = CXIMAGE_FORMAT_PNM;
#endif
	else type = CXIMAGE_FORMAT_UNKNOWN;
	
	return type;
}

CString GetFileTypes(BOOL bOpenFileDialog)
{
	CString str;
	for (int i=0;i<CMAX_IMAGE_FORMATS;i++){
		if (bOpenFileDialog && doctypes[i].bRead){
			str += doctypes[i].description;
			str += (TCHAR)NULL;
			str += doctypes[i].ext;
			str += (TCHAR)NULL;
		} else if (!bOpenFileDialog && doctypes[i].bWrite) {
			str += doctypes[i].description;
			str += (TCHAR)NULL;
			str += doctypes[i].ext;
			str += (TCHAR)NULL;
		}
	}
	return str;
}

IMPLEMENT_SERIAL(CGeoPictrueObj, CGeoDrawObj, VERSIONABLE_SCHEMA|0)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeoPictrueObj::CGeoPictrueObj()
{
	m_strFileName = _T("");
	pImage = NULL;
	SetObjName(_T("图片"));
	m_bLockScale = FALSE;
	m_bEditPoint = FALSE;
	m_dAngle = 0.0;
}

CGeoPictrueObj::CGeoPictrueObj(CDrawLayer* pLayer)
:CGeoDrawObj(pLayer)
{
	m_strFileName = _T("");
	pImage = NULL;
	SetObjName(_T("图片"));
	m_bLockScale = FALSE;
	m_bEditPoint = FALSE;
	m_dAngle = 0.0;
}

CGeoPictrueObj::CGeoPictrueObj(const CRect& position, CDrawLayer* pLayer)
:CGeoDrawObj(position,pLayer)
{
	m_strFileName = _T("");
	pImage = NULL;
	SetObjName(_T("图片"));
	m_bLockScale = FALSE;
	m_bEditPoint = FALSE;
	m_dAngle = 0.0;
}

CGeoPictrueObj::~CGeoPictrueObj()
{
	delete pImage;
	pImage = NULL;
}

void CGeoPictrueObj::Draw(CDC *pDC, CGeoMapView *pView)
{
	ASSERT_VALID(this);
	
	if( !m_bShow )
		return;
	
	if( !pDC->IsPrinting() )
	{
		CRect cliRect;
		pView->GetClientRect(cliRect);
		pView->ClientToDoc(cliRect);
		if( !Intersects(cliRect) ) //如果该图素不在当前窗口中则不显示
			return ;
	}
	
	int nSaved = pDC->SaveDC();
	pDC->SetBkMode(TRANSPARENT);          //设定显示方式
	
	CRect rect = m_rtPosition;
	//rect.NormalizeRect();

	rect.top = m_rtPosition.top<m_rtPosition.bottom?m_rtPosition.bottom:m_rtPosition.top;
	rect.bottom = m_rtPosition.top>m_rtPosition.bottom?m_rtPosition.bottom:m_rtPosition.top;

	if( pImage )
	{
		int nWidth = pImage->GetWidth();
		int nHeight = pImage->GetHeight();

		double dScale = (nWidth>nHeight?(double)nWidth/nHeight:(double)nHeight/nWidth);
		
		nWidth = rect.Width();
		nHeight = rect.Height();
		
		if( pImage->GetWidth() > pImage->GetHeight() )
			rect.right = rect.left - nHeight * dScale;
		else
			rect.top = rect.bottom + nWidth * dScale;

		m_rtPosition = rect;

		pImage->Stretch(pDC->m_hDC, rect);
	}
	else
	{
		if( !pDC->IsPrinting() )
		{
			CFont font;
			font.CreateFont(0,0,0,0,0,0,0,0,0,0,0,0,0,"宋体");
			CFont *pOldFont = pDC->SelectObject(&font);
			pDC->SetTextColor(RGB(94,94,94));
			pDC->DrawText("无图片!",rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_NOCLIP);
			pDC->SelectObject(pOldFont);
		}
	}

	if( m_bEditPoint )
	{
		CPoint pt;
		int n = m_ptCtrl.GetSize();
		CSize size1(10, 10);
		CSize size2(7, 7);
		pDC->DPtoLP(&size1);
		pDC->DPtoLP(&size2);
		pDC->SelectStockObject( NULL_BRUSH );
		CPen pen(PS_SOLID, 0 ,RGB(255, 0, 255));
		CPen *pOldPen = pDC->SelectObject(&pen);
		for (int i=0; i<n; i++)
		{
			pt = m_ptCtrl[i];
			pDC->Ellipse(pt.x-size1.cx,pt.y-size1.cy,pt.x+size1.cx,pt.y+size1.cy);
			pDC->MoveTo(pt.x-size2.cx,	pt.y-size2.cy);
			pDC->LineTo(pt.x+size2.cx,pt.y+size2.cy);
			pDC->MoveTo(pt.x+size2.cx,pt.y-size2.cy);
			pDC->LineTo(pt.x-size2.cx,pt.y+size2.cy);
		}
		pDC->SelectObject(pOldPen);

		if( pView->m_nCtrlPointIndex > -1 && pView->m_nCtrlPointIndex<n)
		{
			pt = m_ptCtrl[pView->m_nCtrlPointIndex];
			CPen pen(PS_SOLID, 0 ,RGB(0, 255, 255));
			pOldPen = pDC->SelectObject(&pen);
			pDC->Ellipse(pt.x-size1.cx,pt.y-size1.cy,pt.x+size1.cx,pt.y+size1.cy);
			pDC->MoveTo(pt.x-size2.cx,	pt.y-size2.cy);
			pDC->LineTo(pt.x+size2.cx,pt.y+size2.cy);
			pDC->MoveTo(pt.x+size2.cx,pt.y-size2.cy);
			pDC->LineTo(pt.x-size2.cx,pt.y+size2.cy);
			pDC->SelectObject(pOldPen);
		}
	}
	
	pDC->RestoreDC(nSaved);
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoPictrueObj::OnEditProperties
// 返回  : void
// 参数  : CGeoMapView *pView
// 说明  : 编辑图素属性
//
BOOL CGeoPictrueObj::OnEditProperties(CGeoMapView *pView)
{
	ASSERT_VALID(this);
	if( m_bLocked )
		return FALSE;

	CFileDialog dlg(TRUE,"BMP",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT);
	
	CString strFilter = GetFileTypes(TRUE);
	dlg.m_ofn.lpstrFilter = strFilter;
	
	if ( dlg.DoModal() == IDOK )
	{
		CWaitCursor wait;
		CString m_strSourceFileName = dlg.GetPathName();
		m_strFileName = dlg.GetFileName();
		
		CMainFrame *pMF = (CMainFrame *)AfxGetMainWnd();
		CString filename = pMF->GetProjectDatPath();
		filename += _T("\\other files\\");
		filename += m_strFileName;

		CopyFile(m_strSourceFileName, filename, FALSE);

		m_dAngle = 0.0;

		LoadPictrue();

		m_pDocument->SetModifiedFlag();
	}
	return FALSE;
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::Clone
// 返回  : CGeoDrawObj*
// 参数  : CGeoMapDoc *pDoc
// 说明  : 克隆图素并将图素加入到文档中
//
CGeoDrawObj* CGeoPictrueObj::Clone(CDrawLayer *pLayer)
{
	ASSERT_VALID(this);
	
	CGeoPictrueObj* pClone = new CGeoPictrueObj(m_rtPosition,pLayer);
	pClone->m_rtPosition	=	m_rtPosition;
	pClone->m_bLocked		=	m_bLocked;			// 锁定图素
	pClone->m_bShow			=	m_bShow;			// 是否显示
	pClone->m_strID			=	newGUID();			// 新的ID
	pClone->m_ptAnchors.Copy(m_ptAnchors);			// 锚点

	pClone->m_pDocument		=	m_pDocument;		// 文档
	pClone->m_geoCenterPoint		=	m_geoCenterPoint;


	pClone->m_strFileName = m_strFileName;
	pClone->m_bLockScale = m_bLockScale;

	pClone->m_dAngle = m_dAngle;

	ASSERT_VALID(pClone);	
	
	if (pClone != NULL)
	{
		m_pDocument->Do(new CCmdAddDrawObj(m_pDocument, pClone), TRUE);
	}
	
	return pClone;
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::Serialize
// 返回  : void
// 参数  : CArchive &ar
// 说明  : 串行化图素
//
void CGeoPictrueObj::Serialize(CArchive &ar)
{
	CGeoDrawObj::Serialize(ar);
	
	ar.SerializeClass(RUNTIME_CLASS(CGeoPictrueObj));

	if (ar.IsStoring())
	{
		ar << m_strFileName;
		ar << m_bLockScale;
		ar << m_dAngle;
		int n = m_ptCtrl.GetSize();
		ar << n;
		for (int i=0; i<n; i++)
		{
			ar << m_ptCtrl[i];
			ar << m_ptGeoCtrl[i];
			ar << m_vecWellName[i];
		}		
	}
	else
	{
		// get the document back pointer from the archive

		int nVersion = ar.GetObjectSchema(); // 获取版本信息
		switch( nVersion )
		{
		case 0:
			{				
				ar >> m_strFileName;
				ar >> m_bLockScale;
				ar >> m_dAngle;
				int n;
				ar >> n;
				CPoint pt;
				CPoint2D geoPt;
				CString strTmp;
				for (int i=0; i<n; i++)
				{
					ar >> pt;
					ar >> geoPt;
					ar >> strTmp;
					m_ptCtrl.Add(pt);
					m_ptGeoCtrl.Add(geoPt);
					m_vecWellName.Add(strTmp);
				}
			}
			break;
		case 1:
			break;
		case 2:
			break;
		default:
			break;
		}
		
		LoadPictrue();
	}
}

// 当比例尺发生改变时
void CGeoPictrueObj::OnScaleChanged()
{
	CGeoDrawObj::OnScaleChanged();
}

// 当发生旋转时
void CGeoPictrueObj::OnAngleChanged()
{
	Invalidate();
}

// 当发生坐标原点有改变时
void CGeoPictrueObj::OnCoordOrigChanged()
{
	Invalidate();
}

void CGeoPictrueObj::LoadPictrue()
{
	CString filename;
	CMainFrame *pMF = (CMainFrame *)AfxGetMainWnd();
	filename = pMF->GetProjectDatPath();
	filename += _T("\\other files\\");
	filename += m_strFileName;

	CString ext(FindExtension(filename));
	ext.MakeLower();
	if (ext != "")
	{
		int type = FindType(ext);
		
		if(pImage != NULL )
			delete pImage;
		
		pImage = new CxImage(filename, type);
		
		if (!pImage->IsValid())
		{
			AfxMessageBox(pImage->GetLastError());
			delete pImage;
			pImage = NULL;
		}
		else
			pImage->Rotate2(m_dAngle);
	}
}


void CGeoPictrueObj::MoveTo(const CRect &position, CGeoMapView *pView)
{
	CRect oldRect = m_rtPosition;
	CGeoDrawObj::MoveTo(position, pView);
	CRect newRect = position;

	oldRect.NormalizeRect();
	newRect.NormalizeRect();
// 	CPoint pt = newRect.CenterPoint() - oldRect.CenterPoint();
// 	CPoint pt1 = oldRect.CenterPoint();
// 	CPoint pt2 = newRect.CenterPoint();

	double w1 = oldRect.Width();
	double w2 = newRect.Width();
	double h1 = oldRect.Height();
	double h2 = newRect.Height();

// 	w1 = w1*0.5;
// 	w2 = w2*0.5;
// 	h1 = h1*0.5;
// 	h2 = h2*0.5;

	int n = m_ptCtrl.GetSize();
	for (int i=0; i<n; i++)
	{
		double xSC = (double)(m_ptCtrl[i].x - oldRect.left)/w1;
		double ySC = (double)(m_ptCtrl[i].y - oldRect.top)/h1;

		m_ptCtrl[i].x = newRect.left + xSC*w2;
		m_ptCtrl[i].y = newRect.top + ySC*h2;
	}
}