// PatternPage.cpp : implementation file
//

#include "stdafx.h"
#include "PatternPage.h"
#include "PatternSheet.h"
#include "ColourPicker.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CTransitionPage, CPropertyPage)
IMPLEMENT_DYNCREATE(CTexturePage, CPropertyPage)
IMPLEMENT_DYNCREATE(CPatternPage, CPropertyPage)
IMPLEMENT_DYNCREATE(CPicturePage, CPropertyPage)


/////////////////////////////////////////////////////////////////////////////
// CTransitionPage property page

CTransitionPage::CTransitionPage() : CPropertyPage(CTransitionPage::IDD)
{
	//{{AFX_DATA_INIT(CTransitionPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CTransitionPage::~CTransitionPage()
{
}

void CTransitionPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTransitionPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTransitionPage, CPropertyPage)
	//{{AFX_MSG_MAP(CTransitionPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTexturePage property page

CTexturePage::CTexturePage() : CPropertyPage(CTexturePage::IDD)
{
	//{{AFX_DATA_INIT(CTexturePage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CTexturePage::~CTexturePage()
{
}

void CTexturePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTexturePage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTexturePage, CPropertyPage)
	//{{AFX_MSG_MAP(CTexturePage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPatternPage property page

CPatternPage::CPatternPage() : CPropertyPage(CPatternPage::IDD)
{
	//{{AFX_DATA_INIT(CPatternPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	nRow = -1;
	nCol = -1;
	m_crColourBack = RGB(255, 255, 255);
	m_crColourFore = RGB(0, 0, 0);	
}

CPatternPage::~CPatternPage()
{	
}

void CPatternPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPatternPage)
	DDX_Control(pDX, IDC_FORECOLOR, m_ColourFore);
	DDX_Control(pDX, IDC_BACKCOLOR, m_ColourBack);
	//}}AFX_DATA_MAP
	DDX_ColourPicker(pDX, IDC_FORECOLOR, m_crColourFore);
	DDX_ColourPicker(pDX, IDC_BACKCOLOR, m_crColourBack);
}


BEGIN_MESSAGE_MAP(CPatternPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPatternPage)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_MESSAGE(CPN_SELENDOK,     OnSelEndOK)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPicturePage property page

CPicturePage::CPicturePage() : CPropertyPage(CPicturePage::IDD)
{
	//{{AFX_DATA_INIT(CPicturePage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPicturePage::~CPicturePage()
{
}

void CPicturePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPicturePage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPicturePage, CPropertyPage)
	//{{AFX_MSG_MAP(CPicturePage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



void CPatternPage::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect rect;
	int i, j;

	CBrush brush;
	CBitmap bit;

	for(i= 0; i<8; i++)
	{
		for( j = 0; j<6; j++)
		{
			rect.SetRect(30 + i*32, 30+j*22, 30+(i+1)*32, 30+(j+1)*22);
			rect.DeflateRect(1,1);
			if( nRow == i && nCol == j)
				dc.Draw3dRect(rect, RGB(255, 0, 0), RGB(255, 0, 0));				
			else
				dc.DrawEdge(rect, EDGE_SUNKEN, BF_RECT);

			dc.SetBkColor(m_crColourBack);
			dc.SetTextColor(m_crColourFore);
			
			bit.LoadBitmap(IDB_BITMAP_01+i*6+j);
						
			CDC MemDC, TransDC;
			MemDC.CreateCompatibleDC(&dc);
			TransDC.CreateCompatibleDC(&dc);
			
			CBitmap bmp;			
			bmp.CreateCompatibleBitmap(&MemDC, 8, 8);
			
			CBitmap *p1 = TransDC.SelectObject(&bit);			
			CBitmap *p2 = MemDC.SelectObject(&bmp);

			MemDC.BitBlt(0,0,8,8,&TransDC, 0, 0, SRCCOPY);

			TransDC.SelectObject(p1);
			TransDC.DeleteDC();
			
			brush.CreatePatternBrush(&bmp);
			bmp.DeleteObject();
			rect.DeflateRect(2, 2);
			
			dc.FillRect(rect, &brush);			
			
			brush.DeleteObject();
			bit.DeleteObject();			
			
			MemDC.SelectObject(p2);
			MemDC.DeleteDC();
		}
	}
	
	rect.SetRect(30, 34+6*22, 30+8*32, 34+7*22);	
	dc.FillSolidRect(rect,::GetSysColor(COLOR_3DFACE));
	dc.DrawEdge(rect, EDGE_SUNKEN, BF_RECT);

	CFont font;
	font.CreateFont(-12, 0, 0, 0, 0, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, "宋体");

	CFont *pOldFont = dc.SelectObject(&font);

	dc.SetBkMode(OPAQUE);
	dc.SetBkColor(::GetSysColor(COLOR_3DFACE) );
	dc.SetTextColor( RGB(0, 0, 0));
	
	if( nRow != -1 && nCol != -1)
		dc.DrawText(m_strPatternName[nCol + nRow*6],rect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);

	dc.SelectObject(pOldFont);
	font.DeleteObject();
}


BOOL CPatternPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CString str[48] =
	{
		_T("5%"),			_T("10%"),			_T("20%"),			_T("25%"),			_T("30%"),			_T("40%"), 
		_T("50%"),			_T("60%"),			_T("70%"),			_T("75%"),			_T("80%"),			_T("90%"),
		_T("浅色下对角线"),	_T("浅色上对角线"),	_T("深色下对角线"),	_T("深色上对角线"),	_T("宽下对角线"),	_T("宽上对角线"), 
		_T("浅色竖线"),		_T("浅色横线"),		_T("窄竖线"),		_T("窄横线"),		_T("深色竖线"),		_T("深色横线"), 
		_T("下对角虚线"),	_T("上对角虚线"),	_T("横虚线"),		_T("竖虚线"),		_T("小纸屑"),		_T("大纸屑"), 
		_T("之字形"),		_T("波浪线"),		_T("对角砖形"),		_T("横向砖形"),		_T("编织物"),		_T("苏格兰方格呢"), 
		_T("草皮"),			_T("虚线网格"),		_T("点式菱形"),		_T("瓦形"),			_T("棚架"),			_T("球体"), 
		_T("小网格"),		_T(" 大网格"),		_T("小棋盘"),		_T("大棋盘"),		_T("轮廓式菱形"),	_T("实心菱形")
	};

	m_ColourFore.SetDefaultText(NULL);
	//m_ColourFore.SetCustomText(NULL);
	m_ColourFore.SetPatternText(NULL);

	m_ColourBack.SetDefaultText(NULL);
	//m_ColourBack.SetCustomText(NULL);
	m_ColourBack.SetPatternText(NULL);

	for(int i=0; i<48; i++)
		m_strPatternName[i] = str[i];	

	Invalidate();
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPatternPage::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRect rect;
	int i, j;
	
	for(i= 0; i<8; i++)
	{
		for( j = 0; j<6; j++)
		{
			rect.SetRect(30 + i*32, 30+j*22, 30+(i+1)*32, 30+(j+1)*22);
			rect.DeflateRect(3,3);
			
			if(rect.PtInRect(point))
			{
				nRow = i;
				nCol = j;				
				break;
			}
		}
	}
	CPatternSheet *pWnd = (CPatternSheet *)GetParent();
	pWnd->m_wndPreview.nCol = nCol;
	pWnd->m_wndPreview.nRow = nRow;
	pWnd->m_wndPreview.m_crColourFore = m_crColourFore;
	pWnd->m_wndPreview.m_crColourBack = m_crColourBack;
	pWnd->m_wndPreview.Invalidate(FALSE);
	Invalidate(FALSE);
	
	CPropertyPage::OnLButtonDown(nFlags, point);
}

LONG CPatternPage::OnSelEndOK(WPARAM wParam, LPARAM lParam)
{
	this->UpdateData();
	this->Invalidate();
	CPatternSheet *pWnd = (CPatternSheet *)GetParent();
	pWnd->m_wndPreview.m_crColourFore = m_crColourFore;
	pWnd->m_wndPreview.m_crColourBack = m_crColourBack;
	pWnd->m_wndPreview.Invalidate(FALSE);
	Invalidate(FALSE);

	return TRUE;
}

void CPatternPage::OnOK() 
{
	this->UpdateData();
	CPatternSheet *pWnd = (CPatternSheet *)GetParent();
	pWnd->patternStyle.m_nFill = 1;	
	pWnd->patternStyle.m_PatternFill.m_nCol = nCol;
	pWnd->patternStyle.m_PatternFill.m_nRow = nRow;
	pWnd->patternStyle.m_PatternFill.m_crColourBack = m_crColourBack;
	pWnd->patternStyle.m_PatternFill.m_crColourFore = m_crColourFore;	
	CPropertyPage::OnOK();
}
