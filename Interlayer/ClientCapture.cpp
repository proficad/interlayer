#include "StdAfx.h"
#include "ClientCapture.h"

/* DIB Macros*/
#define RECTWIDTH(lpRect)     ((lpRect)->right - (lpRect)->left)
#define RECTHEIGHT(lpRect)    ((lpRect)->bottom - (lpRect)->top)


/////////////////////////////////////////////////////////////////////////////
// CClientCapture::CClientCapture - Constructor

CClientCapture::CClientCapture()
{
	m_pBMI = 0;
	m_pBits = 0;
	hDIB = 0;
}


/////////////////////////////////////////////////////////////////////////////
// CClientCapture::~CClientCapture - Destructor

CClientCapture::~CClientCapture()
{
	if (hDIB)
		GlobalFree(hDIB);

	hDIB = 0;
	m_pBMI = 0;
	m_pBits = 0;
}


void CClientCapture::Capture(CDC *dc, CRect rectDIB) 
{
	WriteWindowToDIB(dc, rectDIB);
}

BOOL CClientCapture::WriteWindowToDIB(CDC* dc, CRect rect)
{
	CBitmap 	bitmap;
	CDC 		memDC;

	memDC.CreateCompatibleDC(dc); 
	bitmap.CreateCompatibleBitmap(dc, rect.Width(), rect.Height());

	CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);
	memDC.BitBlt(0, 0, rect.Width(), rect.Height(), dc, 0, 0, SRCCOPY); 

	// 如果设备支持调色板的话则创建一个256色的逻辑调色板

	if (dc->GetDeviceCaps(RASTERCAPS) & RC_PALETTE)
	{
		UINT nSize = sizeof(LOGPALETTE) +(sizeof(PALETTEENTRY) * 256);
		LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];
		pLP->palVersion = 0x300;

		pLP->palNumEntries = 
			GetSystemPaletteEntries(*dc, 0, 255, pLP->palPalEntry);

		// 创建调色板
		pal.CreatePalette(pLP);

		delete[] pLP;
	}

	memDC.SelectObject(pOldBitmap);

	// 将 bitmap 转换为 DIB
	hDIB = DDBToDIB(bitmap, BI_RGB, &pal, dc);

	if (hDIB == NULL)
		return FALSE;

	return TRUE;
}


BOOL CClientCapture::Paint(HDC hDC, CPalette * m_pPalette, LPRECT lpDCRect, LPRECT lpDIBRect) const
{
	if (!m_pBMI)
		return FALSE;

	HPALETTE hPal = NULL;           // DIB 的调色盘
	HPALETTE hOldPal = NULL;        // 先前的调色盘

	// 获取DIB的调色盘，然后将它选入DC
	if (m_pPalette)
	{
		hPal = (HPALETTE) m_pPalette->m_hObject;

		// Select as background since we have
		// already realized in forground if needed
		hOldPal = ::SelectPalette(hDC, hPal, TRUE);
	}

	// 确定使用最适合彩色图像的stretching模式
	::SetStretchBltMode(hDC, COLORONCOLOR);

	// 判断是调用StretchDIBits() 还是 SetDIBitsToDevice()
	BOOL bSuccess = FALSE;
	if ((RECTWIDTH(lpDCRect)  == RECTWIDTH(lpDIBRect)) &&
		(RECTHEIGHT(lpDCRect) == RECTHEIGHT(lpDIBRect)))
		bSuccess = ::SetDIBitsToDevice(hDC,        // hDC
		lpDCRect->left,             // DestX
		lpDCRect->top,              // DestY
		RECTWIDTH(lpDCRect),        // nDestWidth
		RECTHEIGHT(lpDCRect),       // nDestHeight
		lpDIBRect->left,            // SrcX
		(int)Height() -
		lpDIBRect->top -
		RECTHEIGHT(lpDIBRect),     // SrcY
		0,                          // nStartScan
		(WORD)Height(),             // nNumScans
		m_pBits,                    // lpBits
		m_pBMI,                     // lpBitsInfo
		DIB_RGB_COLORS);            // wUsage
	else
		bSuccess = ::StretchDIBits(hDC,            // hDC
		lpDCRect->left,               // DestX
		lpDCRect->top,                // DestY
		RECTWIDTH(lpDCRect),          // nDestWidth
		RECTHEIGHT(lpDCRect),         // nDestHeight
		lpDIBRect->left,              // SrcX
		lpDIBRect->top,               // SrcY
		RECTWIDTH(lpDIBRect),         // wSrcWidth
		RECTHEIGHT(lpDIBRect),        // wSrcHeight
		m_pBits,                      // lpBits
		m_pBMI,                       // lpBitsInfo
		DIB_RGB_COLORS,               // wUsage
		SRCCOPY);                     // dwROP


	// 恢复调色盘
	if (hOldPal)
		::SelectPalette(hDC, hOldPal, TRUE);

	return bSuccess;
}

// DDBToDIB		- Creates a DIB from a DDB
// bitmap		- Device dependent bitmap
// dwCompression	- Type of compression - see BITMAPINFOHEADER
// pPal			- Logical palette
HANDLE CClientCapture::DDBToDIB(CBitmap& bitmap, DWORD dwCompression, CPalette* pPal, 	CDC *dc) 
{
	BITMAP			bm;
	BITMAPINFOHEADER	bi;
	LPBITMAPINFOHEADER 	lpbi;
	DWORD			dwLen;
	// HANDLE			hDIB;
	HANDLE			handle;
	HDC 			hDC;
	HPALETTE		hPal;


	ASSERT(bitmap.GetSafeHandle());

	// The function has no arg for bitfields
	if (dwCompression == BI_BITFIELDS)
		return NULL;

	// 如果未提供调色板，则使用缺省调色板
	hPal = (HPALETTE) pPal->GetSafeHandle();
	if (hPal == NULL)
		hPal = (HPALETTE) GetStockObject(DEFAULT_PALETTE);

	// 取得位图信息
	bitmap.GetObject(sizeof(bm), (LPSTR) & bm);

	// 初始化BITMAPINFOHEADER结构
	bi.biSize           = sizeof(BITMAPINFOHEADER);
	bi.biWidth          = bm.bmWidth;
	bi.biHeight         = bm.bmHeight;
	bi.biPlanes         = 1;
	bi.biBitCount       = bm.bmPlanes * bm.bmBitsPixel;
	bi.biCompression    = dwCompression;
	bi.biSizeImage      = 0;
	bi.biXPelsPerMeter  = 0;
	bi.biYPelsPerMeter  = 0;
	bi.biClrUsed        = 0;
	bi.biClrImportant	= 0;

	// Compute the size of the  infoheader and the color table
	int nColors = (1 << bi.biBitCount);
	if (nColors > 256) 
		nColors = 0;
	dwLen  = bi.biSize + nColors * sizeof(RGBQUAD);

	// We need a device context to get the DIB from
	hDC = dc->GetSafeHdc();

	// hDC = GetDC(NULL);
	hPal = SelectPalette(hDC, hPal, FALSE);
	RealizePalette(hDC);


	// 分配内存以容纳BITMAPINFOHEADER结构和颜色表
	if (hDIB)
		GlobalFree(hDIB);

	hDIB = GlobalAlloc(GMEM_FIXED, dwLen);

	if (!hDIB)
	{
		SelectPalette(hDC, hPal, FALSE);
		return NULL;
	}

	lpbi = (LPBITMAPINFOHEADER)hDIB;

	*lpbi = bi;

	// Call GetDIBits with a NULL lpBits param, so the device driver 
	// will calculate the biSizeImage field 
	GetDIBits(hDC, (HBITMAP)bitmap.GetSafeHandle(), 0L, (DWORD)bi.biHeight,
		(LPBYTE)NULL, (LPBITMAPINFO)lpbi, (DWORD)DIB_RGB_COLORS);

	bi = *lpbi;

	// If the driver did not fill in the biSizeImage field, then compute it
	// Each scan line of the image is aligned on a DWORD (32bit) boundary
	if (bi.biSizeImage == 0)
	{
		bi.biSizeImage = ((((bi.biWidth * bi.biBitCount) + 31) & ~31) / 8) 
			* bi.biHeight;

		// If a compression scheme is used the result may infact be larger
		// Increase the size to account for this.
		if (dwCompression != BI_RGB)
			bi.biSizeImage = (bi.biSizeImage * 3) / 2;
	}

	// 重新分配内存以能容纳所用的位
	dwLen += bi.biSizeImage;
	if (handle = GlobalReAlloc(hDIB, dwLen, GMEM_MOVEABLE))
		hDIB = handle;
	else
	{
		GlobalFree(hDIB);
		hDIB = 0;
		// Reselect the original palette
		SelectPalette(hDC, hPal, FALSE);

		return NULL;
	}

	// 获取位图的各位
	lpbi = (LPBITMAPINFOHEADER)hDIB;
	m_pBMI = (LPBITMAPINFO)hDIB;
	m_pBits = (LPBYTE)hDIB +(bi.biSize + nColors * sizeof(RGBQUAD));

	// FINALLY get the DIB
	BOOL bGotBits = GetDIBits(hDC, (HBITMAP)bitmap.GetSafeHandle(),
		0L,				// Start scan line
		(DWORD)bi.biHeight,		// # of scan lines
		(LPBYTE) m_pBits,// lpbi 			// address for bitmap bits
		(LPBITMAPINFO)lpbi,		// address of bitmapinfo
		(DWORD)DIB_RGB_COLORS);		// Use RGB for color table

	if (!bGotBits)
	{
		GlobalFree(hDIB);
		hDIB = 0;
		SelectPalette(hDC, hPal, FALSE);

		return NULL;
	}

	SelectPalette(hDC, hPal, FALSE);

	return hDIB;
}


DWORD CClientCapture::Height() const
{
	if (!m_pBMI)
		return 0;

	/* return the DIB height */
	return m_pBMI->bmiHeader.biHeight;
}

void CClientCapture::OnDraw(HDC hDC, CRect rcRect, CRect rect)
{
	Paint(hDC, &pal, rcRect, rect);
}

// WriteDIB		- Writes a DIB to file
// Returns		- TRUE on success
// szFile		- Name of file to write to
// hDIB			- Handle of the DIB
BOOL CClientCapture::WriteDIB(CString csFile)
{
	BITMAPFILEHEADER	hdr;
	LPBITMAPINFOHEADER	lpbi;

	if (!hDIB)
		return FALSE;

	CFile file;
	if (!file.Open(csFile, CFile::modeWrite | CFile::modeCreate))
		return FALSE;

	lpbi = (LPBITMAPINFOHEADER)hDIB;

	int nColors = 1 << lpbi->biBitCount;
	if (nColors > 256) 
		nColors = 0;

	// Fill in the fields of the file header 
	hdr.bfType          = ((WORD)('M' << 8) | 'B');	// is always "BM"
	hdr.bfSize          = GlobalSize(hDIB) + sizeof(hdr);
	hdr.bfReserved1     = 0;
	hdr.bfReserved2     = 0;
	hdr.bfOffBits		= (DWORD)(sizeof(hdr) + lpbi->biSize +
		nColors * sizeof(RGBQUAD));

	// Write the file header 
	file.Write(&hdr, sizeof(hdr));

	// Write the DIB header and the bits 
	file.Write(lpbi, GlobalSize(hDIB));

	return TRUE;
}

void CClientCapture::Release()
{
	if (hDIB)
		GlobalFree(hDIB);

	hDIB = 0;
	m_pBMI = 0;
	m_pBits = 0;
}
