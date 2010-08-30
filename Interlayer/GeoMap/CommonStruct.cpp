#include "stdafx.h"
#include "CommonStruct.h"
#include "../Resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CArchive& operator<<(CArchive& ar, const GRADIENTFILL& temp)
{
	ar << temp.m_crColour1;
	ar << temp.m_crColour2;
	ar << temp.m_nDirection;
	return ar;
}

CArchive& operator>>(CArchive& ar, GRADIENTFILL& temp)
{
	ar >> temp.m_crColour1;
	ar >> temp.m_crColour2;
	ar >> temp.m_nDirection;
	return ar;
}

CArchive& operator<<(CArchive& ar, const PICTUREFILL& temp)
{
	ar << temp.m_bTetxTrue;
	ar << temp.m_strFileName;	
	return ar;
}

CArchive& operator>>(CArchive& ar, PICTUREFILL& temp)
{
	ar >> temp.m_bTetxTrue;
	ar >> temp.m_strFileName;
	return ar;
}


CArchive& operator<<(CArchive& ar, const PATTERNFILL& temp)
{
	ar << temp.m_crColourBack;
	ar << temp.m_crColourFore;
	ar << temp.m_nCol;
	ar << temp.m_nRow;
	return ar;
}

CArchive& operator>>(CArchive& ar, PATTERNFILL& temp)
{
	ar >> temp.m_crColourBack;
	ar >> temp.m_crColourFore;
	ar >> temp.m_nCol;
	ar >> temp.m_nRow;
	return ar;
}

CArchive& operator<<(CArchive& ar, const PATTERNSTYLE& temp)
{
	ar << temp.m_nFill;
	
	switch(temp.m_nFill)
	{
	case 1:
		ar << temp.m_PatternFill;
		break;
	case 0:
		ar << temp.m_GradientFill;
		break;
	case 2:
		ar << temp.m_PictureFill;
		break;
	default:
		break;
		
	}
	return ar;
}

CArchive& operator>>(CArchive& ar, PATTERNSTYLE& temp)
{
	ar >> temp.m_nFill;
	
	switch(temp.m_nFill)
	{
	case 1:
		ar >> temp.m_PatternFill;
		break;
	case 0:
		ar >> temp.m_GradientFill;
		break;
	case 2:
		ar >> temp.m_PictureFill;
		break;
	default:
		break;
		
	}

	return ar;
}

CArchive& operator<<(CArchive& ar, const ELEMENTCAPTION& temp)
{
	ar << temp.m_bShow;
	ar << temp.m_crColour;
	ar << temp.m_strCaption;
	ar.Write( & (temp.logFont), sizeof(LOGFONT));
	return ar;
}

CArchive& operator>>(CArchive& ar, ELEMENTCAPTION& temp)
{
	ar >> temp.m_bShow;
	ar >> temp.m_crColour;
	ar >> temp.m_strCaption;
	ar.Read( & (temp.logFont), sizeof(LOGFONT));
	return ar;
}

CArchive& operator<<(CArchive& ar, const ELEMENTLINEFILL& temp)
{
	ar << temp.m_bShow;
	ar << temp.sFillStyle;
	ar << temp.m_uSpace;
	ar << temp.m_nPenStyle;
	ar << temp.m_bSquarePen;
	return ar;
}

CArchive& operator>>(CArchive& ar, ELEMENTLINEFILL& temp)
{
	ar >> temp.m_bShow;
	ar >> temp.sFillStyle;
	ar >> temp.m_uSpace;
	ar >> temp.m_nPenStyle;
	ar >> temp.m_bSquarePen;
	return ar;
}

CArchive& operator<<(CArchive& ar, const ELEMENTLINE& temp)
{
	ar << temp.m_bClose;
	ar << temp.m_bSquarePen;
	ar << temp.m_bShow;
	ar << temp.m_bSmooth;
	ar << temp.m_nLineType;
	ar << temp.m_nPenStyle;
	ar << temp.m_nPenWide;
	ar << temp.m_strLineName;
	ar << temp.s_LineStyle;
	ar << temp.m_uBeginPointMode;
	ar << temp.m_uBeginPointSize;
	ar << temp.m_uEndPointMode;
	ar << temp.m_uEndPointSize;
	ar << temp.eLineSpaceFill;
	return ar;
}

CArchive& operator>>(CArchive& ar, ELEMENTLINE& temp)
{
	ar >> temp.m_bClose;
	ar >> temp.m_bSquarePen;
	ar >> temp.m_bShow;
	ar >> temp.m_bSmooth;
	ar >> temp.m_nLineType;
	ar >> temp.m_nPenStyle;
	ar >> temp.m_nPenWide;
	ar >> temp.m_strLineName;
	ar >> temp.s_LineStyle;
	ar >> temp.m_uBeginPointMode;
	ar >> temp.m_uBeginPointSize;
	ar >> temp.m_uEndPointMode;
	ar >> temp.m_uEndPointSize;
	ar >> temp.eLineSpaceFill;
	return ar;
}

CArchive& operator<<(CArchive& ar, const ELEMENTFILL& temp)
{
	ar << temp.m_bShow;
	ar << temp.sFillStyle;
	ar.Write(&(temp.blendFunction), sizeof(BLENDFUNCTION));
	return ar;
}

CArchive& operator>>(CArchive& ar, ELEMENTFILL& temp)
{
	ar >> temp.m_bShow;
	ar >> temp.sFillStyle;
	ar.Read(&(temp.blendFunction), sizeof(BLENDFUNCTION));
	return ar;
}

CArchive& operator<<(CArchive& ar, const ELEMENTLABEL& temp)
{
	ar << temp.m_bShow;
	ar << temp.m_crColour;
	ar << temp.m_fSpace;
	ar << temp.m_fLabel;
	ar << temp.m_uDecimal;
	ar.Write(&(temp.logFont), sizeof(LOGFONT));
	return ar;
}

CArchive& operator>>(CArchive& ar, ELEMENTLABEL& temp)
{
	//long s;
	ar >> temp.m_bShow;
	ar >> temp.m_crColour;
	ar >> temp.m_fSpace;
	//ar >> s;
	//temp.m_fLabel = s;
	ar >> temp.m_fLabel;
	ar >> temp.m_uDecimal;
	ar.Read(&(temp.logFont), sizeof(LOGFONT));
	return ar;
}

CArchive& operator<<(CArchive& ar, const ELEMENTSTATE& temp)
{
	ar << temp.m_bActive;
	ar << temp.m_bLock;
	ar << temp.m_bNoMove;
	ar << temp.m_bShow;
	ar << temp.m_bEdit;
	return ar;
}

CArchive& operator>>(CArchive& ar, ELEMENTSTATE& temp)
{
	ar >> temp.m_bActive;
	ar >> temp.m_bLock;
	ar >> temp.m_bNoMove;
	ar >> temp.m_bShow;
	ar >> temp.m_bEdit;
	return ar;
}

CArchive& operator<<(CArchive& ar, const ELEMENTPROPERTY& temp)
{
	ar << temp.eCaption;
	ar << temp.eFill;
	ar << temp.eLabel;
	ar << temp.eLine;
	ar << temp.m_nMovingHandle;
	ar << temp.m_rtPosition;
	ar << temp.m_strElementName;
	return ar;
}

CArchive& operator>>(CArchive& ar, ELEMENTPROPERTY& temp)
{
	ar >> temp.eCaption;
	ar >> temp.eFill;
	ar >> temp.eLabel;
	ar >> temp.eLine;
	ar >> temp.m_nMovingHandle;
	ar >> temp.m_rtPosition;
	ar >> temp.m_strElementName;
	return ar;
}


void PATTERNFILL::LoadBitmap(CBitmap *pBitmap)
{	
	ASSERT( m_nRow != -1 || m_nCol != -1);
	pBitmap->LoadBitmap(IDB_BITMAP_01+m_nRow*6+m_nCol);
}

BOOL PATTERNFILL::CreateBrush(CBrush *pBrush, CDC *pDC)
{
	CBitmap MaskBmp;	
	
	LoadBitmap(&MaskBmp);

	pDC->SetBkColor(m_crColourBack);
	pDC->SetTextColor(m_crColourFore);
	
	CDC MemDC, MaskDC;
	MemDC.CreateCompatibleDC(pDC);
	MaskDC.CreateCompatibleDC(pDC);	
	
	CBitmap MemBmp;	
	
	HBITMAP hBitmap;
	hBitmap = MyCreateBitmap(MemDC.m_hDC, 8, 8, 32);
	
	MemBmp.Attach(hBitmap);
	
	CBitmap *pMaskBmp = MaskDC.SelectObject(&MaskBmp);			
	CBitmap *pMemBmp = MemDC.SelectObject(&MemBmp);
	
	MemDC.BitBlt(0,0,8,8,&MaskDC, 0, 0,  SRCCOPY);

	BOOL bResult = pBrush->CreatePatternBrush(&MemBmp);
	
	MaskDC.SelectObject(pMaskBmp);
	MaskDC.DeleteDC();	
	
	MemBmp.DeleteObject();		
	MaskBmp.DeleteObject();		
	MemDC.SelectObject(pMemBmp);
	MemDC.DeleteDC();

	return bResult;
}

BOOL PATTERNSTYLE::CreateBrush(CBrush *pBrush, CDC *pDC)
{
	if(m_nFill == 1)
		return m_PatternFill.CreateBrush(pBrush, pDC);
	else
		return FALSE;
}


HBITMAP MyCreateBitmap(
					   HDC hdc,
					   INT cx,
					   INT cy,
					   INT nColors)
{
	BITMAPINFOHEADER bmih;
    
	if (nColors == 2) {
		return CreateBitmap(cx, cy, 1, 1, NULL);
	}
	else {
		bmih.biSize = sizeof(BITMAPINFOHEADER);
		bmih.biWidth = cx;
		bmih.biHeight = cy;
		bmih.biPlanes = 1;              // 1 plane, 4 bpp is
		bmih.biBitCount = nColors;      //  colors.
		
		bmih.biCompression =
        bmih.biSizeImage =
        bmih.biXPelsPerMeter =
        bmih.biYPelsPerMeter =
        bmih.biClrUsed =
        bmih.biClrImportant = 0;
		
		return CreateDIBitmap(hdc, &bmih, 0L, NULL, NULL, 0);
	}
}
