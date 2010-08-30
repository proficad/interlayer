// SetWidthView.cpp : implementation file
//

#include "stdafx.h"
#include "SetWidthView.h"
#include "DlgSetWidth.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetWidthView

#define WM_SETWIDTH WM_USER+1

IMPLEMENT_DYNCREATE(CSetWidthView, CScrollView)

CSetWidthView::CSetWidthView()
{
	m_font.CreateFont(12,0,0,0,0,0,0,0,GB2312_CHARSET,0,0,0,0,"宋体");
}

CSetWidthView::~CSetWidthView()
{

	m_font.DeleteObject();
}


BEGIN_MESSAGE_MAP(CSetWidthView, CScrollView)
	//{{AFX_MSG_MAP(CSetWidthView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETWIDTH,OnSetWidth)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetWidthView drawing

void CSetWidthView::OnInitialUpdate()
{
	CDlgSetWidth *pDlgWnd = (CDlgSetWidth *)GetParentFrame()->GetParent();

	pDlgWnd->m_pView = this;
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;

	sizeTotal.cx = 100;
	sizeTotal.cy = 100;

	SetScrollSizes(MM_TEXT, sizeTotal);
}

void CSetWidthView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here

	CRect rect;
	GetClientRect(rect);
	pDC->FillSolidRect(rect,RGB(255,255,255));
	CFont *pOldFont = pDC->SelectObject(&m_font);

	int n=m_arrStr.GetSize();
	int i;
	for ( i =1 ; i < n; i++)
	{
		CString tmp = m_arrStr[i];
		pDC->TextOut(0,(i-1)*14+22,tmp);
	}

	CBrush brush(RGB(255,0,0));
	CBrush*pOldBrush = pDC->SelectObject(&brush);
	CPen pen(PS_SOLID,0,RGB(255,0,0));

	CPen *pOldPen = pDC->SelectObject(&pen);

	CPoint pt[3];
	n = m_arWidth.GetSize();
	for (i=0;i<n;i++)
	{
		int t = m_arWidth[i];
		pt[0].x=t*6-3;
		pt[0].y=20;
		pt[1].x=t*6+3;
		pt[1].y=20;

		pt[2].x=t*6;
		pt[2].y=13;

		pDC->Polygon(pt,3);
	}

	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);	
		
	pDC->SelectObject(pOldFont);

	if( n > 0)		
	{
		int w = (n>1)?m_arWidth[n-1]-m_arWidth[n-2]:m_arWidth[n-1];
		int t = m_arWidth[n-1];
		while (i<100)
		{
			t += w;
			pt[0].x=t*6-3;
			pt[0].y=20;
			pt[1].x=t*6+3;
			pt[1].y=20;
			
			pt[2].x=t*6;
			pt[2].y=13;
			
			pDC->Polygon(pt,3);
			i++;
		}
	}

	
}

/////////////////////////////////////////////////////////////////////////////
// CSetWidthView message handlers

int CSetWidthView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	DWORD dw = WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL | ES_READONLY &~WS_BORDER;
	m_Edit.Create(dw,CRect(0,0,0,0),this,1);
	m_Edit.SetFont(&m_font);
	
	return 0;
}
void CSetWidthView::OnSize(UINT nType, int cx, int cy) 
{
	CScrollView::OnSize(nType, cx, cy);
	
}

BOOL CSetWidthView::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class
	cs.style |= WS_CLIPCHILDREN;
	return CScrollView::PreCreateWindow(cs);
}

int CSetWidthView::GetSeqCounts()
{
	return m_arWidth.GetSize();
}

void CSetWidthView::GetSeqWidth(int*iSeqsWidth)
{
	int n = m_arWidth.GetSize();
	for (int i=0;i<n;i++)
		iSeqsWidth[i] = m_arWidth[i];
}

void CSetWidthView::AddStr(CString str)
{
	m_arrStr.Add(str);
}

void CSetWidthView::UpdateWidth()
{
	CSize sizeTotal;
	// TODO: calculate the total size of this view
	int n = m_arrStr.GetSize();

	int nLen=0;
	for (int i=0;i<n;i++)
	{
		CString tmp = m_arrStr[i];
		int t = tmp.GetLength();
		if( nLen < t)
			nLen = t;
	}
	sizeTotal.cx = nLen*6+10;
	sizeTotal.cy = 100;

//	CString tt = m_arrStr[0]; 
//	tt.Replace("	"," ");
	m_Edit.SetWindowText(m_arrStr[0]);

	SetScrollSizes(MM_TEXT, sizeTotal);
	int xMin,xMax;
	GetScrollRange(SB_HORZ,&xMin,&xMax);
	m_Edit.MoveWindow(0,0,xMax+xMin+1,13);
}

LRESULT CSetWidthView::OnSetWidth(WPARAM wp, LPARAM lp)
{
	int w = (int)lp;
	int n = m_arWidth.GetSize();
	for (int i=0;i<n;i++)
	{
		int t = m_arWidth[i];
		if( t == w )
		{
			m_arWidth.RemoveAt(i);
			Invalidate();
			return -1;
		}
	}
	
	if(n==0)
		m_arWidth.Add(lp);
	else if( n==1)
	{
		if( w > m_arWidth[0])
			m_arWidth.Add(lp);
		else
			m_arWidth.InsertAt(0,w);
	}
	else
	{
		if( w < m_arWidth[0]) //如果小于第一个则插在第一个
		{
			m_arWidth.InsertAt(0,w);
			Invalidate();
			return -1;
		}

		for (int i=0;i<n-1;i++)
		{
			
			if( w > m_arWidth[i] && w < m_arWidth[i+1] )
			{
				m_arWidth.InsertAt(i+1,w);
				Invalidate();
				return -1;
			}
			
		}
		m_arWidth.Add(lp);
	}
	
	Invalidate();
	return 0;
}
