// 2DModelView.cpp : 实现文件
//

#include "stdafx.h"
#include "Interlayer.h"
#include "2DModelView.h"
#include "MainFrm.h"
#include "DlgGridPropSetup.h"
#include "2DModelDoc.h"
// C2DModelView

#define IsSHIFTpressed() ( (GetKeyState(VK_SHIFT) & (1 << (sizeof(SHORT)*8-1))) != 0   )
#define IsCTRLpressed()  ( (GetKeyState(VK_CONTROL) & (1 << (sizeof(SHORT)*8-1))) != 0 )

const int MapMode = MM_LOMETRIC;

using namespace _2DView;

CHandTracker::CHandTracker(C2DModelView * pView)
{
	ASSERT(pView != NULL);
	m_pView = pView;
	m_sizeMin = CSize(1, 1);
}

void CHandTracker::DrawTrackerRect(LPCRECT lpRect, CWnd * pWndClipTo, CDC * pDC, CWnd * pWnd)
{
	// draw nothing
}

void CHandTracker::OnChangedRect(const CRect & rectOld)
{
	CPoint point;
	// call the view to update scroll position
	point.x = m_rect.Width() - rectOld.Width();
	point.y = m_rect.Height() - rectOld.Height();
	m_pView->UpdateScrollPosition(point);
}

/////////////////////////////////////////////////////////////////////////////
// C2DModelView

IMPLEMENT_DYNCREATE(C2DModelView, CBigScrollView)

C2DModelView::C2DModelView()
{
	m_bDrawFill			= TRUE;
	m_bGradientFill		= TRUE;
	m_bDrawBorder		= TRUE;
	m_bDrawText			= FALSE;

	CFont font;
	font.CreateFont(6,0,0,0,0,0,0,0,ANSI_CHARSET,0,0,0,0,"Arial");
	font.GetLogFont(&m_lgFont);


	m_sizeDoc = CSize(2100, 2960);
	// default to A4, MM_LOMETRIC
	m_Scale = m_Zoom = ZOOM_ONE;
	m_ZoomMin = 1;
	m_ZoomMax = LONG_MAX / 2960;
	m_x0 = 2100 / 2;
	m_y0 = 2960 / 2;
	m_hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	m_operation = opNone;

	m_ptCurPosition.x = 0;
	m_ptCurPosition.y = 0;

	m_Zoom = 1000;
	m_pCurCell = NULL;

	m_bDispTop = TRUE;
}

C2DModelView::~C2DModelView()
{
	if (m_hCursor != NULL)
		DestroyCursor(m_hCursor);
}

//功能:判断网格是否在选中列表中
//参数:
//返回:
BOOL C2DModelView::IsSelected(CObject* pCell) const
{
	return m_selection.Find(pCell) != NULL;	
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoMapView::SelectWithinRect
// 返回  : void
// 参数  : CRect rect
// 参数  : BOOL bAdd FALSE-- 移去原所有选中的网格后添加该网格到网格集中 
//					 TRUE -- 不移去原所有选中的网格并添加该网格到网格集中
//					缺省值是FALSE
// 说明  : 圈选网格
//
void C2DModelView::SelectWithinRect(CRect rect, BOOL bAdd)
{
	if (!bAdd)
		Select(NULL);

	if( rect.IsRectEmpty() )
		return;

	this->ClientToDoc(rect);

	C2DModelDoc *pDoc = GetDocument();

	for( int i = 0; i< pDoc->m_gridModel.m_nGridX; i++)
	{
		for( int j =0; j< pDoc->m_gridModel.m_nGridY;j++)
		{				
			CFieldCell * pCell = pDoc->GetCell(i, j);
			if (pCell->Intersects(rect))
				Select(pCell, TRUE);
		}
	}
}

//功能:把网格加入到选中列表中
//参数:如果 pObj == NULL则清空列表, 如果bAdd = FALSE清空列表后加入网格,否则加入网格
//返回:
void C2DModelView::Select(CFieldCell *pCell, BOOL bAdd)
{
	if (!bAdd)
	{
		OnUpdate(NULL, HINT_UPDATE_SELECTION, NULL);		
		m_selection.RemoveAll();
	}

	CFieldCell*p = m_pCurCell;
	m_pCurCell = pCell;
	if( p )
		InvalCell(p);

	if (pCell == NULL || IsSelected(pCell))
	{
		if (pCell)
			InvalCell(pCell);
		return;
	}

	m_selection.AddTail(pCell);
	InvalCell(pCell);
	UpdateDisplayInfo();	
}


//功能:取消选中网格
//参数:
//返回:
void C2DModelView::DeSelect(CFieldCell *pCell)
{
	POSITION pos = m_selection.Find(pCell);
	if (pos != NULL)
	{
		InvalCell(pCell);
		m_selection.RemoveAt(pos);
	}
}


void C2DModelView::InvalCell(CFieldCell *pCell)
{
	CRect rect = pCell->GetRect(); // 逻辑坐标
	DocToClient(rect); // 转换为设备坐标
	rect.DeflateRect(-1,-1,-1,-1);
	InvalidateRect(rect, FALSE);
}

BEGIN_MESSAGE_MAP(C2DModelView, CBigScrollView)
	//{{AFX_MSG_MAP(C2DModelView)
	ON_COMMAND(ID_VIEW_ZOOM_IN, OnViewZoomIn)
	ON_COMMAND(ID_VIEW_ZOOM_OUT, OnViewZoomOut)
	ON_COMMAND(ID_VIEW_ZOOM_DEFAULT, OnViewZoomDefault)
	ON_WM_MOUSEMOVE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_COMMAND(ID_VIEW_SET_SCALE_TO_FIT, OnViewSetScaleToFit)
	ON_COMMAND(ID_VIEW_RESIZE_PARENT_TO_FIT, OnViewResizeParentToFit)
	ON_COMMAND(ID_VIEW_UPPER_LEFT, OnViewUpperLeft)
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_TRACK_MODE, OnTrackMode)
	ON_UPDATE_COMMAND_UI(ID_TRACK_MODE, OnUpdateTrackMode)
	ON_COMMAND(ID_ZOOM_AREA, OnZoomArea)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_AREA, OnUpdateZoomArea)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_CONTEXTMENU()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
	ON_COMMAND(ID_DISP_FILL, OnDispFill)
	ON_UPDATE_COMMAND_UI(ID_DISP_FILL, OnUpdateDispFill)
	ON_COMMAND(ID_DISP_GRADFILL, OnDispGradfill)
	ON_UPDATE_COMMAND_UI(ID_DISP_GRADFILL, OnUpdateDispGradfill)
	ON_COMMAND(ID_DISP_GRID, OnDispGrid)
	ON_UPDATE_COMMAND_UI(ID_DISP_GRID, OnUpdateDispGrid)
	ON_COMMAND(ID_DISP_VALUES, OnDispValues)
	ON_UPDATE_COMMAND_UI(ID_DISP_VALUES, OnUpdateDispValues)
	ON_COMMAND(ID_DRAW_SELECT, OnDrawSelect)
	ON_UPDATE_COMMAND_UI(ID_DRAW_SELECT, OnUpdateDrawSelect)
	ON_COMMAND(ID_EDIT_CELL, OnEditCell)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CELL, OnUpdateEditCell)
	ON_COMMAND(ID_FUNTION, OnFuntion)
	ON_UPDATE_COMMAND_UI(ID_FUNTION, OnUpdateFuntion)
	ON_COMMAND(ID_DISP_TOP, OnDispTop)
	ON_UPDATE_COMMAND_UI(ID_DISP_TOP, OnUpdateDispTop)
	ON_COMMAND(ID_DISP_BOTTOM, OnDispBottom)
	ON_UPDATE_COMMAND_UI(ID_DISP_BOTTOM, OnUpdateDispBottom)
	ON_COMMAND(ID_EXPORT_GRAPH, OnExportGraph)
	ON_COMMAND(ID_EXPORT_DAT, OnExportDat)
	ON_COMMAND(ID_PROP_SETUP, OnPropSet)
	ON_COMMAND(ID_UP_LAYER, OnUpLayer)
	ON_UPDATE_COMMAND_UI(ID_UP_LAYER, OnUpdateUpLayer)
	ON_COMMAND(ID_DOWN_LAYER, OnDownLayer)
	ON_UPDATE_COMMAND_UI(ID_DOWN_LAYER, OnUpdateDownLayer)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CBigScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CBigScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CBigScrollView::OnFilePrintPreview)

	ON_WM_DESTROY()
	ON_WM_KEYUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// C2DModelView drawing

C2DModelDoc* C2DModelView::GetDocument()
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(C2DModelDoc)));
	return (C2DModelDoc*)m_pDocument;
}

//////////////////////////////////////////////////////////////////////////
// 逻辑坐标到设备坐标的一组互换函数
// 
void C2DModelView::ClientToDoc(CSize& size)
{
	CClientDC dc(this);
	OnPrepareDC(&dc, NULL);
	dc.DPtoLP(&size);
}

void C2DModelView::ClientToDoc(CPoint& point)
{
	CClientDC dc(this);
	OnPrepareDC(&dc, NULL);
	dc.DPtoLP(&point);
}

void C2DModelView::ClientToDoc(CRect& rect)
{
	CClientDC dc(this);
	OnPrepareDC(&dc, NULL);
	dc.DPtoLP(rect);
}

void C2DModelView::DocToClient(CSize& size)
{
	CClientDC dc(this);
	OnPrepareDC(&dc, NULL);
	dc.LPtoDP(&size);
}

void C2DModelView::DocToClient(CPoint& point)
{
	CClientDC dc(this);
	OnPrepareDC(&dc, NULL);
	dc.LPtoDP(&point);
}

void C2DModelView::DocToClient(CRect& rect)
{
	CClientDC dc(this);
	OnPrepareDC(&dc, NULL);
	dc.LPtoDP(rect);
	rect.NormalizeRect();
}

//////////////////////////////////////////////////////////////////////////

void C2DModelView::OnInitialUpdate()
{
	CBigScrollView::OnInitialUpdate();

	C2DModelDoc* pDoc = GetDocument();
	//pDoc->m_bAutoDelete = TRUE;

	Init();
}

void C2DModelView::OnDraw(CDC* pDC)
{
	C2DModelDoc* pDoc = GetDocument();
	// TODO: add draw code here

	// TODO: add draw code here
	CDC dc;
	CDC* pDrawDC = pDC;
	CBitmap bitmap;
	CBitmap* pOldBitmap;

	// only paint the rect that needs repainting
	CRect client;

	int nSaved = pDC->SaveDC();

	pDC->GetClipBox(client);

	client.NormalizeRect();

	if( !pDC->IsPrinting() ) 
		client.DeflateRect(-10,-10,-10,-10); // 消除白线

	CRect rect = client;

	pDC->LPtoDP(rect);

	if (!pDC->IsPrinting())
	{
		// draw to offscreen bitmap for fast looking repaints
		if (dc.CreateCompatibleDC(pDC))
		{
			if (bitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height()))
				//		if (bitmap.CreateBitmap(rect.Width(), rect.Height(),1,32,NULL))
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

	CFieldCell * pCell;
	if( pDoc->m_gridModel.m_bCornerPointGrid ) // 如果是角点网格，按深度进行显示
	{
		int nSize = pDoc->m_arDepth.GetSize();
		for (int i=nSize-1;i>=0;i--)
		{
			pCell = pDoc->GetCell(pDoc->m_arDepth[i].m_i, pDoc->m_arDepth[i].m_j);
			pCell->Draw(pDrawDC, this);
		}
	}
	else
	{
		for( int i = 0; i< pDoc->m_gridModel.m_nGridX; i++)
		{
			for( int j =0; j< pDoc->m_gridModel.m_nGridY;j++)
			{				
				pCell = pDoc->GetCell(i, j);

				pCell->Draw(pDrawDC, this);
			}
		}
	}

	if( !pDC->IsPrinting() )
	{
		POSITION pos = m_selection.GetHeadPosition();
		while (pos != NULL)
		{
			CFieldCell*pCell = (CFieldCell*)m_selection.GetNext(pos);
			pCell->DrawStateSelected(pDrawDC, this);
		}
		if( m_pCurCell )
			m_pCurCell->DrawCurCell(pDC, this);
	}

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

	pDC->RestoreDC(nSaved);


}

/////////////////////////////////////////////////////////////////////////////
// C2DModelView diagnostics

#ifdef _DEBUG
void C2DModelView::AssertValid() const
{
	CBigScrollView::AssertValid();
}

void C2DModelView::Dump(CDumpContext& dc) const
{
	CBigScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// C2DModelView message handlers

void C2DModelView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	CRect rect;
	CBigScrollView::OnPrepareDC(pDC, pInfo);    

	if (pDC->IsPrinting())
	{
		C2DModelDoc* pDoc = GetDocument();

		pDC->SetMapMode(MM_LOMETRIC);
		CSize size = pDoc->m_sizePicture;
		pDC->LPtoDP(&size);
		pDC->SetMapMode(MM_ANISOTROPIC);

		pDC->SetWindowExt(pDoc->m_sizePicture);
		pDC->SetViewportExt(size.cx, - size.cy);
		pDC->SetWindowOrg(
			- (pDoc->m_sizeGridMap.cx / 2 + 100),
			pDoc->m_sizeGridMap.cy / 2 + 100	
			);

		if (pInfo)
		{
			int hPages = pDoc->m_sizePicture.cx / pDoc->m_sizeDoc.cx 
				+ (pDoc->m_sizePicture.cx%pDoc->m_sizeDoc.cx ? 1:0);

			int vPages = pDoc->m_sizePicture.cy / pDoc->m_sizeDoc.cy 
				+ (pDoc->m_sizePicture.cy%pDoc->m_sizeDoc.cy ? 1:0);
			int hPage = (pInfo->m_nCurPage - 1)%hPages;
			int vPage = (pInfo->m_nCurPage - 1) / hPages;

			pDC->OffsetWindowOrg(hPage * pDoc->m_sizeDoc.cx, - pDoc->m_sizeDoc.cy * vPage);
		}
	}
	else 
	{
		C2DModelDoc* pDoc = GetDocument();

		pDC->SetMapMode(MM_ISOTROPIC);
		CSize size = m_sizeDoc;
		pDC->LPtoDP(&size);

		pDC->SetMapMode(MM_ANISOTROPIC);

		pDC->SetWindowExt(pDoc->m_sizePicture);
		pDC->SetViewportExt(MulDiv(size.cx, m_Zoom, ZOOM_ONE), - MulDiv(size.cy, m_Zoom, ZOOM_ONE));
		pDC->OffsetWindowOrg(
			- (pDoc->m_sizeGridMap.cx / 2 + 100),
			pDoc->m_sizeGridMap.cy / 2 + 100 
			);
	}
}

BOOL C2DModelView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	// TODO: call DoPreparePrinting to invoke the Print dialog box

	C2DModelDoc* pDoc = GetDocument();

	// 计算打印页数
	int hPages = pDoc->m_sizePicture.cx / pDoc->m_sizeDoc.cx 
		+ (pDoc->m_sizePicture.cx%pDoc->m_sizeDoc.cx ? 1:0);
	int vPages = pDoc->m_sizePicture.cy / pDoc->m_sizeDoc.cy 
		+ (pDoc->m_sizePicture.cy%pDoc->m_sizeDoc.cy ? 1:0);

	pInfo->SetMaxPage(hPages * vPages);

	return DoPreparePrinting(pInfo);
}

void C2DModelView::OnDispFill() 
{
	// TODO: Add your command handler code here
	if( m_bDrawFill )
		m_bDrawFill = FALSE;
	else
		m_bDrawFill = TRUE;

	Invalidate(TRUE);
}

void C2DModelView::OnUpdateDispFill(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetRadio(m_bDrawFill);
}

void C2DModelView::OnDispGradfill() 
{
	// TODO: Add your command handler code here
	if( m_bGradientFill )
		m_bGradientFill = FALSE;
	else
		m_bGradientFill = TRUE;

	Invalidate(TRUE);
}

void C2DModelView::OnUpdateDispGradfill(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_bDrawFill);
	pCmdUI->SetRadio(m_bGradientFill);
}

void C2DModelView::OnDispGrid() 
{
	// TODO: Add your command handler code here
	if( m_bDrawBorder )
		m_bDrawBorder = FALSE;
	else
		m_bDrawBorder = TRUE;

	Invalidate(TRUE);
}

void C2DModelView::OnUpdateDispGrid(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetRadio(m_bDrawBorder);
}

void C2DModelView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch(lHint)
	{
	case HINT_UPDATE_LAYER:
		Init();
		m_pCurCell = NULL;
		m_selection.RemoveAll();
		Invalidate(TRUE);
		return;
	case HINT_UPDATE_SELECTION: // 所有选中的网格发生改变（包括状态、大小、位置)
		{
			CObList* pList = pHint != NULL ?
				(CObList*)pHint : &m_selection;
			POSITION pos = pList->GetHeadPosition();
			while (pos != NULL)
				InvalCell((CFieldCell*)pList->GetNext(pos));
		}
		return;
	default:
		break;
	}

	C2DModelDoc* pDoc = GetDocument();
	m_sizeDoc = pDoc->m_sizePicture;

	CClientDC dc(this);
	CSize sizeScreen(1000, 1000);

	if (m_sizeDoc.cx > m_sizeDoc.cy)
		sizeScreen.cy = MulDiv(sizeScreen.cy, m_sizeDoc.cy, m_sizeDoc.cx);
	else 
		sizeScreen.cx = MulDiv(sizeScreen.cx, m_sizeDoc.cx, m_sizeDoc.cy);

	dc.SetMapMode(MapMode);
	dc.DPtoLP(&sizeScreen);
	m_x0 = sizeScreen.cx / 2;
	m_y0 = sizeScreen.cy / 2;
	m_sizeDoc = sizeScreen;
	SetScrollSizes(MapMode, sizeScreen);

	OnZoom(m_Zoom);
	ScrollToPosition(m_ptCurPosition);
}

void C2DModelView::Init()
{
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	CString strMsg;
	strMsg = "正在计算网格模型数据，请稍等...";
	pMF->GetStatusBar().SetPaneText(0, strMsg);	

	C2DModelDoc* pDoc = GetDocument();

	double dRange = pDoc->m_dDispMaxValue - pDoc->m_dDispMinValue;

	double dRStep = ( GetRValue(pDoc->m_clrMax) - GetRValue(pDoc->m_clrMin) ) / dRange ;
	double dGStep = ( GetGValue(pDoc->m_clrMax) - GetGValue(pDoc->m_clrMin) ) / dRange ;
	double dBStep = ( GetBValue(pDoc->m_clrMax) - GetBValue(pDoc->m_clrMin) ) / dRange ;

	int R, G, B;

	// 计算网格中心颜色值

	double xRange = pDoc->m_gridModel.m_dMaxX - pDoc->m_gridModel.m_dMinX;
	double yRange = pDoc->m_gridModel.m_dMaxY - pDoc->m_gridModel.m_dMinY;

	for( int i = 0; i< pDoc->m_gridModel.m_nGridX; i++)
	{
		for( int j =0; j< pDoc->m_gridModel.m_nGridY;j++)
		{				
			CFieldCell * pCell = pDoc->GetCell(i, j);

			if( pCell->m_dValue < pDoc->m_dDispMinValue )
				pCell->m_clrCenter = RGB(255, 255, 255);
			else if( pCell->m_dValue > pDoc->m_dDispMaxValue )
				pCell->m_clrCenter = RGB(255, 255, 255);
			else
			{
				R = (pCell->m_dValue - pDoc->m_dDispMinValue) * dRStep + GetRValue(pDoc->m_clrMin);
				G = (pCell->m_dValue - pDoc->m_dDispMinValue) * dGStep + GetGValue(pDoc->m_clrMin);
				B = (pCell->m_dValue - pDoc->m_dDispMinValue) * dBStep + GetBValue(pDoc->m_clrMin);

				pCell->m_clrCenter = RGB(R, G, B);
			}

			pCell->m_point[0].x = pDoc->RealToLogicX(pCell->m_cornerPoint[0].x);
			pCell->m_point[0].y = pDoc->RealToLogicY(pCell->m_cornerPoint[0].y);
			pCell->m_point[1].x = pDoc->RealToLogicX(pCell->m_cornerPoint[1].x);
			pCell->m_point[1].y = pDoc->RealToLogicY(pCell->m_cornerPoint[1].y);
			pCell->m_point[2].x = pDoc->RealToLogicX(pCell->m_cornerPoint[2].x);
			pCell->m_point[2].y = pDoc->RealToLogicY(pCell->m_cornerPoint[2].y);
			pCell->m_point[3].x = pDoc->RealToLogicX(pCell->m_cornerPoint[3].x);
			pCell->m_point[3].y = pDoc->RealToLogicY(pCell->m_cornerPoint[3].y);

			pCell->m_point[4].x = pDoc->RealToLogicX(pCell->m_cornerPoint[4].x);
			pCell->m_point[4].y = pDoc->RealToLogicY(pCell->m_cornerPoint[4].y);
			pCell->m_point[5].x = pDoc->RealToLogicX(pCell->m_cornerPoint[5].x);
			pCell->m_point[5].y = pDoc->RealToLogicY(pCell->m_cornerPoint[5].y);
			pCell->m_point[6].x = pDoc->RealToLogicX(pCell->m_cornerPoint[6].x);
			pCell->m_point[6].y = pDoc->RealToLogicY(pCell->m_cornerPoint[6].y);
			pCell->m_point[7].x = pDoc->RealToLogicX(pCell->m_cornerPoint[7].x);
			pCell->m_point[7].y = pDoc->RealToLogicY(pCell->m_cornerPoint[7].y);
		}
	}

	// 计算网格角点颜色值
	for( int i = 0; i< pDoc->m_gridModel.m_nGridX; i++)
	{
		for( int j = 0; j< pDoc->m_gridModel.m_nGridY;j++)
		{				
			CFieldCell * pCell = pDoc->GetCell(i, j);

			int nSC, nEC;
			int nSR, nER;

			if( i > 0 )
				nSR = i-1;
			else
				nSR = 0;

			if( i < pDoc->m_gridModel.m_nGridX-1 )
				nER = i+1;
			else
				nER = i;

			if( j > 0 )
				nSC = j-1;
			else
				nSC = 0;

			if( j < pDoc->m_gridModel.m_nGridY-1 )
				nEC = j+1;
			else
				nEC = j;

			Interpolater* pInterpolater = NULL;
			pInterpolater = new InverseDist(200, 4);

			vector<CVertex3D> inputR;
			vector<CVertex3D> inputG;
			vector<CVertex3D> inputB;

			int m, n;

			inputR.erase(inputR.begin(), inputR.end());
			inputG.erase(inputG.begin(), inputG.end());
			inputB.erase(inputB.begin(), inputB.end());

			for ( m=nSR; m<=i; m++)
			{
				for ( n=nSC; n<=j; n++)
				{
					CFieldCell * p = pDoc->GetCell(m, n);
					CVertex3D pt;

					pt.x = (p->m_centerPoint[0].x - pDoc->m_gridModel.m_dMinX) / xRange; // 归一化
					pt.y = (p->m_centerPoint[0].y - pDoc->m_gridModel.m_dMinY) / yRange;

					pt.z = GetRValue(p->m_clrCenter);
					inputR.push_back(pt);

					pt.z = GetGValue(p->m_clrCenter);
					inputG.push_back(pt);

					pt.z = GetBValue(p->m_clrCenter);
					inputB.push_back(pt);
				}
			}

			if( inputB.size() < 2)
				pCell->m_clrCorner[0] = pCell->m_clrCenter;
			else
			{
				R = pInterpolater->GetInterpolatedZ((pCell->m_cornerPoint[0].x - pDoc->m_gridModel.m_dMinX) / xRange, (pCell->m_cornerPoint[0].y - pDoc->m_gridModel.m_dMinY) / yRange, inputR.begin(), inputR.end());
				G = pInterpolater->GetInterpolatedZ((pCell->m_cornerPoint[0].x - pDoc->m_gridModel.m_dMinX) / xRange, (pCell->m_cornerPoint[0].y - pDoc->m_gridModel.m_dMinY) / yRange, inputG.begin(), inputG.end());
				B = pInterpolater->GetInterpolatedZ((pCell->m_cornerPoint[0].x - pDoc->m_gridModel.m_dMinX) / xRange, (pCell->m_cornerPoint[0].y - pDoc->m_gridModel.m_dMinY) / yRange, inputB.begin(), inputB.end());
				pCell->m_clrCorner[0] = RGB(R,G,B);
			}

			inputR.erase(inputR.begin(), inputR.end());
			inputG.erase(inputG.begin(), inputG.end());
			inputB.erase(inputB.begin(), inputB.end());

			for ( m=i; m<=nER; m++)
			{
				for ( n=nSC; n<=j; n++)
				{
					CFieldCell * p = pDoc->GetCell(m, n);
					CVertex3D pt;
					pt.x = (p->m_centerPoint[0].x - pDoc->m_gridModel.m_dMinX) / xRange; // 归一化
					pt.y = (p->m_centerPoint[0].y - pDoc->m_gridModel.m_dMinY) / yRange;

					pt.z = GetRValue(p->m_clrCenter);
					inputR.push_back(pt);

					pt.z = GetGValue(p->m_clrCenter);
					inputG.push_back(pt);

					pt.z = GetBValue(p->m_clrCenter);
					inputB.push_back(pt);
				}
			}

			if( inputB.size() < 2)
				pCell->m_clrCorner[1] = pCell->m_clrCenter;
			else
			{
				R = pInterpolater->GetInterpolatedZ((pCell->m_cornerPoint[1].x - pDoc->m_gridModel.m_dMinX) / xRange, (pCell->m_cornerPoint[1].y - pDoc->m_gridModel.m_dMinY) / yRange, inputR.begin(), inputR.end());
				G = pInterpolater->GetInterpolatedZ((pCell->m_cornerPoint[1].x - pDoc->m_gridModel.m_dMinX) / xRange, (pCell->m_cornerPoint[1].y - pDoc->m_gridModel.m_dMinY) / yRange, inputG.begin(), inputG.end());
				B = pInterpolater->GetInterpolatedZ((pCell->m_cornerPoint[1].x - pDoc->m_gridModel.m_dMinX) / xRange, (pCell->m_cornerPoint[1].y - pDoc->m_gridModel.m_dMinY) / yRange, inputB.begin(), inputB.end());
				pCell->m_clrCorner[1] = RGB(R,G,B);
			}

			inputR.erase(inputR.begin(), inputR.end());
			inputG.erase(inputG.begin(), inputG.end());
			inputB.erase(inputB.begin(), inputB.end());

			for ( m=i; m<=nER; m++)
			{
				for ( n=j; n<=nEC; n++)
				{
					CFieldCell * p = pDoc->GetCell(m, n);
					CVertex3D pt;
					pt.x = (p->m_centerPoint[0].x - pDoc->m_gridModel.m_dMinX) / xRange; // 归一化
					pt.y = (p->m_centerPoint[0].y - pDoc->m_gridModel.m_dMinY) / yRange;

					pt.z = GetRValue(p->m_clrCenter);
					inputR.push_back(pt);

					pt.z = GetGValue(p->m_clrCenter);
					inputG.push_back(pt);

					pt.z = GetBValue(p->m_clrCenter);
					inputB.push_back(pt);
				}
			}

			if( inputB.size() < 2)
				pCell->m_clrCorner[2] = pCell->m_clrCenter;
			else
			{
				R = pInterpolater->GetInterpolatedZ((pCell->m_cornerPoint[2].x - pDoc->m_gridModel.m_dMinX) / xRange, (pCell->m_cornerPoint[2].y - pDoc->m_gridModel.m_dMinY) / yRange, inputR.begin(), inputR.end());
				G = pInterpolater->GetInterpolatedZ((pCell->m_cornerPoint[2].x - pDoc->m_gridModel.m_dMinX) / xRange, (pCell->m_cornerPoint[2].y - pDoc->m_gridModel.m_dMinY) / yRange, inputG.begin(), inputG.end());
				B = pInterpolater->GetInterpolatedZ((pCell->m_cornerPoint[2].x - pDoc->m_gridModel.m_dMinX) / xRange, (pCell->m_cornerPoint[2].y - pDoc->m_gridModel.m_dMinY) / yRange, inputB.begin(), inputB.end());
				pCell->m_clrCorner[2] = RGB(R,G,B);
			}

			inputR.erase(inputR.begin(), inputR.end());
			inputG.erase(inputG.begin(), inputG.end());
			inputB.erase(inputB.begin(), inputB.end());

			for ( m=nSR; m<=i; m++)
			{
				for ( n=j; n<=nEC; n++)
				{
					CFieldCell * p = pDoc->GetCell(m, n);
					CVertex3D pt;
					pt.x = (p->m_centerPoint[0].x - pDoc->m_gridModel.m_dMinX) / xRange; // 归一化
					pt.y = (p->m_centerPoint[0].y - pDoc->m_gridModel.m_dMinY) / yRange;

					pt.z = GetRValue(p->m_clrCenter);
					inputR.push_back(pt);

					pt.z = GetGValue(p->m_clrCenter);
					inputG.push_back(pt);

					pt.z = GetBValue(p->m_clrCenter);
					inputB.push_back(pt);
				}
			}

			if( inputB.size() < 2)
				pCell->m_clrCorner[3] = pCell->m_clrCenter;
			else
			{
				R = pInterpolater->GetInterpolatedZ((pCell->m_cornerPoint[3].x - pDoc->m_gridModel.m_dMinX) / xRange, (pCell->m_cornerPoint[3].y - pDoc->m_gridModel.m_dMinY) / yRange, inputR.begin(), inputR.end());
				G = pInterpolater->GetInterpolatedZ((pCell->m_cornerPoint[3].x - pDoc->m_gridModel.m_dMinX) / xRange, (pCell->m_cornerPoint[3].y - pDoc->m_gridModel.m_dMinY) / yRange, inputG.begin(), inputG.end());
				B = pInterpolater->GetInterpolatedZ((pCell->m_cornerPoint[3].x - pDoc->m_gridModel.m_dMinX) / xRange, (pCell->m_cornerPoint[3].y - pDoc->m_gridModel.m_dMinY) / yRange, inputB.begin(), inputB.end());
				pCell->m_clrCorner[3] = RGB(R,G,B);
			}

			inputR.erase(inputR.begin(), inputR.end());
			inputG.erase(inputG.begin(), inputG.end());
			inputB.erase(inputB.begin(), inputB.end());

			for ( m=nSR; m<=i; m++)
			{
				for ( n=nSC; n<=j; n++)
				{
					CFieldCell * p = pDoc->GetCell(m, n);
					CVertex3D pt;

					pt.x = (p->m_centerPoint[1].x - pDoc->m_gridModel.m_dMinX) / xRange; // 归一化
					pt.y = (p->m_centerPoint[1].y - pDoc->m_gridModel.m_dMinY) / yRange;

					pt.z = GetRValue(p->m_clrCenter);
					inputR.push_back(pt);

					pt.z = GetGValue(p->m_clrCenter);
					inputG.push_back(pt);

					pt.z = GetBValue(p->m_clrCenter);
					inputB.push_back(pt);
				}
			}

			if( inputB.size() < 2)
				pCell->m_clrCorner[4] = pCell->m_clrCenter;
			else
			{
				R = pInterpolater->GetInterpolatedZ((pCell->m_cornerPoint[4].x - pDoc->m_gridModel.m_dMinX) / xRange, (pCell->m_cornerPoint[4].y - pDoc->m_gridModel.m_dMinY) / yRange, inputR.begin(), inputR.end());
				G = pInterpolater->GetInterpolatedZ((pCell->m_cornerPoint[4].x - pDoc->m_gridModel.m_dMinX) / xRange, (pCell->m_cornerPoint[4].y - pDoc->m_gridModel.m_dMinY) / yRange, inputG.begin(), inputG.end());
				B = pInterpolater->GetInterpolatedZ((pCell->m_cornerPoint[4].x - pDoc->m_gridModel.m_dMinX) / xRange, (pCell->m_cornerPoint[4].y - pDoc->m_gridModel.m_dMinY) / yRange, inputB.begin(), inputB.end());
				pCell->m_clrCorner[4] = RGB(R,G,B);
			}

			inputR.erase(inputR.begin(), inputR.end());
			inputG.erase(inputG.begin(), inputG.end());
			inputB.erase(inputB.begin(), inputB.end());

			for ( m=i; m<=nER; m++)
			{
				for ( n=nSC; n<=j; n++)
				{
					CFieldCell * p = pDoc->GetCell(m, n);
					CVertex3D pt;
					pt.x = (p->m_centerPoint[1].x - pDoc->m_gridModel.m_dMinX) / xRange; // 归一化
					pt.y = (p->m_centerPoint[1].y - pDoc->m_gridModel.m_dMinY) / yRange;

					pt.z = GetRValue(p->m_clrCenter);
					inputR.push_back(pt);

					pt.z = GetGValue(p->m_clrCenter);
					inputG.push_back(pt);

					pt.z = GetBValue(p->m_clrCenter);
					inputB.push_back(pt);
				}
			}

			if( inputB.size() < 2)
				pCell->m_clrCorner[5] = pCell->m_clrCenter;
			else
			{
				R = pInterpolater->GetInterpolatedZ((pCell->m_cornerPoint[5].x - pDoc->m_gridModel.m_dMinX) / xRange, (pCell->m_cornerPoint[5].y - pDoc->m_gridModel.m_dMinY) / yRange, inputR.begin(), inputR.end());
				G = pInterpolater->GetInterpolatedZ((pCell->m_cornerPoint[5].x - pDoc->m_gridModel.m_dMinX) / xRange, (pCell->m_cornerPoint[5].y - pDoc->m_gridModel.m_dMinY) / yRange, inputG.begin(), inputG.end());
				B = pInterpolater->GetInterpolatedZ((pCell->m_cornerPoint[5].x - pDoc->m_gridModel.m_dMinX) / xRange, (pCell->m_cornerPoint[5].y - pDoc->m_gridModel.m_dMinY) / yRange, inputB.begin(), inputB.end());
				pCell->m_clrCorner[5] = RGB(R,G,B);
			}

			inputR.erase(inputR.begin(), inputR.end());
			inputG.erase(inputG.begin(), inputG.end());
			inputB.erase(inputB.begin(), inputB.end());

			for ( m=i; m<=nER; m++)
			{
				for ( n=j; n<=nEC; n++)
				{
					CFieldCell * p = pDoc->GetCell(m, n);
					CVertex3D pt;
					pt.x = (p->m_centerPoint[1].x - pDoc->m_gridModel.m_dMinX) / xRange; // 归一化
					pt.y = (p->m_centerPoint[1].y - pDoc->m_gridModel.m_dMinY) / yRange;

					pt.z = GetRValue(p->m_clrCenter);
					inputR.push_back(pt);

					pt.z = GetGValue(p->m_clrCenter);
					inputG.push_back(pt);

					pt.z = GetBValue(p->m_clrCenter);
					inputB.push_back(pt);
				}
			}

			if( inputB.size() < 2)
				pCell->m_clrCorner[6] = pCell->m_clrCenter;
			else
			{
				R = pInterpolater->GetInterpolatedZ((pCell->m_cornerPoint[6].x - pDoc->m_gridModel.m_dMinX) / xRange, (pCell->m_cornerPoint[6].y - pDoc->m_gridModel.m_dMinY) / yRange, inputR.begin(), inputR.end());
				G = pInterpolater->GetInterpolatedZ((pCell->m_cornerPoint[6].x - pDoc->m_gridModel.m_dMinX) / xRange, (pCell->m_cornerPoint[6].y - pDoc->m_gridModel.m_dMinY) / yRange, inputG.begin(), inputG.end());
				B = pInterpolater->GetInterpolatedZ((pCell->m_cornerPoint[6].x - pDoc->m_gridModel.m_dMinX) / xRange, (pCell->m_cornerPoint[6].y - pDoc->m_gridModel.m_dMinY) / yRange, inputB.begin(), inputB.end());
				pCell->m_clrCorner[6] = RGB(R,G,B);
			}

			inputR.erase(inputR.begin(), inputR.end());
			inputG.erase(inputG.begin(), inputG.end());
			inputB.erase(inputB.begin(), inputB.end());

			for ( m=nSR; m<=i; m++)
			{
				for ( n=j; n<=nEC; n++)
				{
					CFieldCell * p = pDoc->GetCell(m, n);
					CVertex3D pt;
					pt.x = (p->m_centerPoint[1].x - pDoc->m_gridModel.m_dMinX) / xRange; // 归一化
					pt.y = (p->m_centerPoint[1].y - pDoc->m_gridModel.m_dMinY) / yRange;

					pt.z = GetRValue(p->m_clrCenter);
					inputR.push_back(pt);

					pt.z = GetGValue(p->m_clrCenter);
					inputG.push_back(pt);

					pt.z = GetBValue(p->m_clrCenter);
					inputB.push_back(pt);
				}
			}

			if( inputB.size() < 2)
				pCell->m_clrCorner[7] = pCell->m_clrCenter;
			else
			{
				R = pInterpolater->GetInterpolatedZ((pCell->m_cornerPoint[7].x - pDoc->m_gridModel.m_dMinX) / xRange, (pCell->m_cornerPoint[7].y - pDoc->m_gridModel.m_dMinY) / yRange, inputR.begin(), inputR.end());
				G = pInterpolater->GetInterpolatedZ((pCell->m_cornerPoint[7].x - pDoc->m_gridModel.m_dMinX) / xRange, (pCell->m_cornerPoint[7].y - pDoc->m_gridModel.m_dMinY) / yRange, inputG.begin(), inputG.end());
				B = pInterpolater->GetInterpolatedZ((pCell->m_cornerPoint[7].x - pDoc->m_gridModel.m_dMinX) / xRange, (pCell->m_cornerPoint[7].y - pDoc->m_gridModel.m_dMinY) / yRange, inputB.begin(), inputB.end());
				pCell->m_clrCorner[7] = RGB(R,G,B);
			}

			delete pInterpolater;
		}
	}

	strMsg = "计算完成!";

	pMF->GetStatusBar().SetPaneText(0, strMsg);	

	//////////////////////////////////////////////////////////////////////////	
}

void C2DModelView::OnDispValues() 
{
	// TODO: Add your command handler code here
	if( m_bDrawText )
		m_bDrawText = FALSE;
	else
		m_bDrawText = TRUE;

	Invalidate(TRUE);
}

void C2DModelView::OnUpdateDispValues(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetRadio(m_bDrawText);
}


/////////////////////////////////////////////////////////////////////////////
// C2DModelView message handlers


void C2DModelView::UpdateDisplayInfo()const 
{
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	if( m_selection.IsEmpty() )
		return;

	CString strMsg;

	CFieldCell *pCell = (CFieldCell *)m_selection.GetTail();
	strMsg.Format("x,y,z - (%3d,%3d,%3d)",pCell->m_i+1, pCell->m_j+1, pCell->m_k+1);
	pMF->GetStatusBar().SetPaneText(1, strMsg);	

	CMFCToolBarEditBoxButton* pSrcEdit = NULL;

	CObList listButtons;
	if (CMFCToolBar::GetCommandButtons (ID_EDIT_CELL, listButtons) > 0)
	{
		for (POSITION pos = listButtons.GetHeadPosition (); 
			pSrcEdit == NULL && pos != NULL;)
		{
			CMFCToolBarEditBoxButton* pEdit = 
				DYNAMIC_DOWNCAST (CMFCToolBarEditBoxButton, listButtons.GetNext (pos));

			if (pEdit != NULL )
			{
				pSrcEdit = pEdit;
			}
		}
	}

	if (pSrcEdit != NULL && pSrcEdit->GetEditBox () != NULL &&
		(pSrcEdit->GetEditBox ()->GetStyle () & WS_VISIBLE))
	{
		CString strTmp;
		strTmp.Format("%lf",pCell->m_dValue);
		pSrcEdit->GetEditBox ()->SetWindowText (strTmp);
		pSrcEdit->GetEditBox ()->SetFocus();
		pSrcEdit->GetEditBox ()->SetSel (0,-1);
	}
}

void C2DModelView::OnZoom(long Zoom)
{
	if (Zoom < m_ZoomMin)
	{
		MessageBeep(MB_ICONEXCLAMATION);
		Zoom = m_ZoomMin;
	}
	if (Zoom > m_ZoomMax)
	{
		MessageBeep(MB_ICONEXCLAMATION);
		Zoom = m_ZoomMax;
	}
	if (Zoom != m_Zoom)
	{
		m_Zoom = Zoom;
		CSize sizeTotal = m_sizeDoc;
		sizeTotal.cx = MulDiv(sizeTotal.cx, m_Zoom, ZOOM_ONE);
		sizeTotal.cy = MulDiv(sizeTotal.cy, m_Zoom, ZOOM_ONE);
		SetScrollSizes(MapMode, sizeTotal);
		Invalidate(FALSE);
	}	
}

void C2DModelView::OnViewZoomIn()
{
	OnZoom(m_Zoom * 2);
}

void C2DModelView::OnViewZoomOut()
{
	OnZoom(m_Zoom / 2);
}

void C2DModelView::OnViewZoomDefault()
{
	// get client center in document units
	CRect rectClient;
	GetLogClientRect(&rectClient);
	CPoint pointCenter = rectClient.CenterPoint();
	pointCenter.x = MulDiv(pointCenter.x, ZOOM_ONE, m_Zoom);
	pointCenter.y = MulDiv(pointCenter.y, ZOOM_ONE, m_Zoom);

	// change zoom
	OnZoom(ZOOM_ONE);

	// calculate new center point with zoom
	pointCenter.x = MulDiv(pointCenter.x, m_Zoom, ZOOM_ONE);
	pointCenter.y = MulDiv(pointCenter.y, m_Zoom, ZOOM_ONE);

	// center on new point
	CenterOnPoint(pointCenter);
}

void C2DModelView::OnMouseMove(UINT nFlags, CPoint point)
{
	//	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	//	CPoint devPoint = GetLogPosition(point);
	//	pMF->m_xv = devPoint.x; pMF->m_yv = devPoint.y;
	//	devPoint.x = PosX(devPoint.x);
	//	devPoint.y = PosY(devPoint.y);
	//	pMF->m_xl = devPoint.x; pMF->m_yl = devPoint.y;
	switch(m_operation)
	{
	case opSelectCell:
		{

		}
		break;
	default:
		break;
	}
	CBigScrollView::OnMouseMove(nFlags, point);
}

void C2DModelView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar * pScrollBar)
{
	// only to update display infos
	CBigScrollView::OnHScroll(nSBCode, nPos, pScrollBar);
	m_ptCurPosition = GetScrollPosition();
}

void C2DModelView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar * pScrollBar)
{
	// only to update display infos
	CBigScrollView::OnVScroll(nSBCode, nPos, pScrollBar);
	m_ptCurPosition = GetScrollPosition();
}

void C2DModelView::OnSize(UINT nType, int cx, int cy)
{
	// only to update display infos
	CBigScrollView::OnSize(nType, cx, cy);
}

void C2DModelView::OnViewSetScaleToFit()
{
	CSize sizeTotal = m_TotalSize;
	// this function change the scale
	SetScaleToFitSize(m_TotalSize);
	// read the scale back
	OnZoom(MulDiv(m_Zoom, m_TotalSize.cx, sizeTotal.cx));
}

void C2DModelView::OnViewResizeParentToFit()
{
	CRect rect;
	GetLogClientRect(&rect);
	OnZoom(MulDiv(rect.Width(), ZOOM_ONE, m_sizeDoc.cx));
}

void C2DModelView::OnViewUpperLeft()
{
	C2DModelDoc* pDoc = GetDocument();
	m_ptCurPosition = CPoint(0,0);
	ScrollToPosition(CPoint(0, 0));
}

void C2DModelView::UpdateScrollPosition(CPoint point)
{
	CPoint pointCenter = GetLogClientCenter();
	if (m_nMapMode != MM_TEXT)
	{
		CWindowDC dc(NULL);
		dc.SetMapMode(m_nMapMode);
		dc.DPtoLP(&point);
	}
	pointCenter -= point;
	CenterOnPoint(pointCenter);

	m_ptCurPosition = GetScrollPosition();
}

void C2DModelView::OnLButtonDown(UINT nFlags, CPoint point)
{
	switch(m_operation)
	{
	case opMoveView:
		{
			CHandTracker trackHand(this);
			CPoint saveCenter = m_Center;
			trackHand.TrackRubberBand(this, point);
			Invalidate(FALSE);			
		}
		break;
	case opZoomRect:
		{
			CRectTracker tracker;
			if (tracker.TrackRubberBand(this, point))
			{
				CRect rectTracker = tracker.m_rect;
				rectTracker.NormalizeRect();
				if (rectTracker.Width() >= 8 * m_Delta)
				{
					long Zoom = m_Zoom;
					long saveZoom = m_Zoom;

					// get logical position from device position
					CPoint p1 = GetLogPosition(rectTracker.TopLeft());
					CPoint p2 = GetLogPosition(rectTracker.BottomRight());

					// get size of tracker rectangle
					rectTracker = CRect(p1, p2);
					CSize sizeTracker(abs(rectTracker.Width()), abs(rectTracker.Height()));

					// get center of tracker rectangle without zoom
					CPoint pointCenter = rectTracker.CenterPoint();
					pointCenter.x = MulDiv(pointCenter.x, ZOOM_ONE, m_Zoom);
					pointCenter.y = MulDiv(pointCenter.y, ZOOM_ONE, m_Zoom);

					// get client size
					CRect rectClient;
					GetLogClientRect(&rectClient);
					CSize sizeClient(rectClient.Width(), abs(rectClient.Height()));

					// calculate new zoom
					if (sizeClient.cy < MulDiv(sizeTracker.cy, sizeClient.cx, sizeTracker.cx))
						Zoom = MulDiv(Zoom, sizeClient.cy, sizeTracker.cy);
					else 
						Zoom = MulDiv(Zoom, sizeClient.cx, sizeTracker.cx);

					// change zoom
					OnZoom(Zoom);

					// calculate new center point with new zoom
					if (m_Zoom != saveZoom)
					{
						pointCenter.x = MulDiv(pointCenter.x, m_Zoom, ZOOM_ONE);
						pointCenter.y = MulDiv(pointCenter.y, m_Zoom, ZOOM_ONE);
						// center on new point
						CenterOnPoint(pointCenter);
					}
				}
			}
		}
		break;

	case opNone:
		{
			CRectTracker tracker;
			if (tracker.TrackRubberBand(this, point))
			{
				CRect rectTracker = tracker.m_rect;
				rectTracker.NormalizeRect();
				if( nFlags & MK_CONTROL )
					SelectWithinRect(rectTracker, TRUE);
				else
					SelectWithinRect(rectTracker, FALSE);
			}
			else
			{
				CFieldCell *pCell = HitCell(point);
				if( nFlags & MK_CONTROL )
				{
					if( IsSelected(pCell) && pCell != m_pCurCell )
						DeSelect(pCell);
					else
						Select(pCell, TRUE);
				}
				else
					Select(pCell, FALSE);
			}			
		}
		break;
	default:
		break;
	}

	CBigScrollView::OnLButtonDown(nFlags, point);
}

BOOL C2DModelView::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message)
{
	if (m_operation != opNone && nHitTest == HTCLIENT)
	{
		::SetCursor(m_hCursor);
		return 0;
	}

	return CBigScrollView::OnSetCursor(pWnd, nHitTest, message);
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数  : C2DModelView::OnTrackMode
// 返回  : void
// 说明  : 点击移动图形方式
//
void C2DModelView::OnTrackMode()
{
	m_operation = opMoveView;
	m_hCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_HAND);
}

void C2DModelView::OnUpdateTrackMode(CCmdUI * pCmdUI)
{
	pCmdUI->SetRadio(m_operation == opMoveView);
}


// 测试当前是否点中网格了
CFieldCell* C2DModelView::HitCell(const CPoint &point)
{
	CPoint pt = point;
	CFieldCell *pCell = NULL;

	C2DModelDoc* pDoc = GetDocument();

	ClientToDoc(pt);

	for( int i = 0; i< pDoc->m_gridModel.m_nGridX; i++)
	{
		for( int j =0; j< pDoc->m_gridModel.m_nGridY;j++)
		{				
			pCell = pDoc->GetCell(i, j);

			if (pCell->PtInRegion(pt))
				return pCell;
		}
	}    

	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数  : C2DModelView::OnZoomArea
// 返回  : void
// 说明  : 区域放大
//
void C2DModelView::OnZoomArea()
{
	m_operation = opZoomRect;
	m_hCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_ZOOM_RECT);
}

void C2DModelView::OnUpdateZoomArea(CCmdUI * pCmdUI)
{
	pCmdUI->SetCheck(m_operation == opZoomRect);
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数  : C2DModelView::OnEditCopy
// 返回  : void
// 说明  : 拷贝图形
//
void C2DModelView::OnEditCopy()
{
	HENHMETAFILE hemfcopy;

	if (!OpenClipboard()) return;
	if (!::EmptyClipboard()) return;

	CClientDC dcRef(this);
	CMetaFileDC dcMeta;
	dcMeta.CreateEnhanced(&dcRef, NULL, NULL, "ImageName\0Copy\0\0");
	dcMeta.SetAttribDC(dcRef.m_hDC);
	dcMeta.m_bPrinting = TRUE;
	OnPrepareDC(&dcMeta);
	OnDraw(&dcMeta);
	HENHMETAFILE hemf = dcMeta.CloseEnhanced();	

	UINT uiEnhSize = ::GetEnhMetaFileBits(hemf, 0, NULL);
	LPBYTE lpEMF = (LPBYTE)malloc(uiEnhSize);

	if (lpEMF && GetEnhMetaFileBits(hemf, uiEnhSize, lpEMF)) {
		hemfcopy = SetEnhMetaFileBits(uiEnhSize, lpEMF);
		SetClipboardData(CF_ENHMETAFILE, hemfcopy);
	}
	CloseClipboard();

	if (lpEMF)
		free(lpEMF);

	dcMeta.DeleteDC();
	::DeleteEnhMetaFile(hemf);
}

void C2DModelView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	switch(m_operation)
	{
	default:
		break;
	}
	CBigScrollView::OnLButtonUp(nFlags, point);
}


BOOL C2DModelView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default

	CRect rect;
	pDC->GetClipBox(rect);
	pDC->FillSolidRect(rect, RGB(255,255,255));
	return TRUE;
}

void C2DModelView::OnContextMenu(CWnd* pWnd, CPoint point) 
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

	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_POPUP_MENU));
	int nCount = menu.GetMenuItemCount();
	CMenu* pPopup = NULL;
	//
	//	GetMenuString
	CString strMenuName;
	for (int i=0;i<nCount;i++)
	{
		menu.GetMenuString(i,strMenuName,MF_BYPOSITION);
		if( strMenuName == "_2D_网格模型_" )
		{
			pPopup = menu.GetSubMenu(i);
			break;
		}
	}

	if( pPopup == NULL )
		return;

	ASSERT(pPopup != NULL);

	CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;
	pPopupMenu->SetAutoDestroy (TRUE);

	pPopupMenu->Create (this, point.x, point.y, pPopup->m_hMenu, FALSE, TRUE);		
}


BOOL C2DModelView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	//if (m_operation == opMoveView)
	{
		if(zDelta < 0)
			OnZoom(m_Zoom + 100);
		else
			OnZoom(m_Zoom - 100);
		return TRUE;
	}

	BOOL bRet = CBigScrollView::OnMouseWheel(nFlags, zDelta, pt);

	m_ptCurPosition = GetScrollPosition();

	return bRet;
}

void C2DModelView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	CBigScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void C2DModelView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if( !HitCell(point) )
	{
		CBigScrollView::OnLButtonDblClk(nFlags, point);
		return;
	}

	switch(m_operation)
	{
	case opNone:
	case opMoveView:		// 移动视窗
		{
		}
		break;
	default:
		break;
	}

	CBigScrollView::OnLButtonDblClk(nFlags, point);
}

void C2DModelView::OnFilePrintPreview() 
{
	// TODO: Add your command handler code here
	AFXPrintPreview (this);
}

void C2DModelView::OnDrawSelect() 
{
	// TODO: Add your command handler code here
	m_operation = opNone;
	m_hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
}

void C2DModelView::OnUpdateDrawSelect(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_operation == opNone);
}

void C2DModelView::OnEditCell() 
{
	// TODO: Add your command handler code here
	CMFCToolBarEditBoxButton* pSrcEdit = NULL;

	CObList listButtons;
	if (CMFCToolBar::GetCommandButtons (ID_EDIT_CELL, listButtons) > 0)
	{
		for (POSITION pos = listButtons.GetHeadPosition (); 
			pSrcEdit == NULL && pos != NULL;)
		{
			CMFCToolBarEditBoxButton* pEdit = 
				DYNAMIC_DOWNCAST (CMFCToolBarEditBoxButton, listButtons.GetNext (pos));

			if (pEdit != NULL && ::GetFocus () == pEdit->GetHwnd ())
			{
				pSrcEdit = pEdit;
			}
		}
	}

	if (pSrcEdit != NULL && pSrcEdit->GetEditBox () != NULL &&
		(pSrcEdit->GetEditBox ()->GetStyle () & WS_VISIBLE))
	{
		C2DModelDoc* pDoc = GetDocument();

		CString strCell;
		pSrcEdit->GetEditBox ()->GetWindowText (strCell);
		strCell.TrimRight();
		strCell.TrimLeft();
		if(!strCell.IsEmpty() && strCell[0]=='=')
		{
			strCell.Replace("=","");
			if(!strCell.IsEmpty())
			{
				switch(strCell[0])
				{
				case '*':
					{
						CString strTmp = strCell.Right(strCell.GetLength()-1);
						if( strTmp.IsEmpty() )
						{
							::MessageBox(AfxGetMainWnd()->m_hWnd,"缺少数据！","提示信息",MB_OK|MB_ICONINFORMATION);
							return;
						}

						double v = atof(strTmp);
						POSITION pos = m_selection.GetHeadPosition();
						while (pos != NULL)
						{
							CFieldCell*pCell = (CFieldCell*)m_selection.GetNext(pos);
							pCell->m_dValue *= v;
							pDoc->SetCellValue(pCell->m_i,pCell->m_j,pCell->m_k, pCell->m_dValue);
						}
					}
					break;
				case '/':
					{
						CString strTmp = strCell.Right(strCell.GetLength()-1);
						if( strTmp.IsEmpty() )
						{
							::MessageBox(AfxGetMainWnd()->m_hWnd,"缺少数据！","提示信息",MB_OK|MB_ICONINFORMATION);
							return;
						}
						double v = atof(strTmp);
						POSITION pos = m_selection.GetHeadPosition();
						while (pos != NULL)
						{
							CFieldCell*pCell = (CFieldCell*)m_selection.GetNext(pos);
							pCell->m_dValue /= v;
							pDoc->SetCellValue(pCell->m_i,pCell->m_j,pCell->m_k, pCell->m_dValue);
						}
					}
					break;
				case '+':
					{
						CString strTmp = strCell.Right(strCell.GetLength()-1);
						if( strTmp.IsEmpty() )
						{
							::MessageBox(AfxGetMainWnd()->m_hWnd,"缺少数据！","提示信息",MB_OK|MB_ICONINFORMATION);
							return;
						}
						double v = atof(strTmp);
						POSITION pos = m_selection.GetHeadPosition();
						while (pos != NULL)
						{
							CFieldCell*pCell = (CFieldCell*)m_selection.GetNext(pos);
							pCell->m_dValue += v;
							pDoc->SetCellValue(pCell->m_i,pCell->m_j,pCell->m_k, pCell->m_dValue);
						}
					}
					break;
				case '-':
					{
						CString strTmp = strCell.Right(strCell.GetLength()-1);
						if( strTmp.IsEmpty() )
						{
							::MessageBox(AfxGetMainWnd()->m_hWnd,"缺少数据！","提示信息",MB_OK|MB_ICONINFORMATION);
							return;
						}
						double v = atof(strTmp);
						POSITION pos = m_selection.GetHeadPosition();
						while (pos != NULL)
						{
							CFieldCell*pCell = (CFieldCell*)m_selection.GetNext(pos);
							pCell->m_dValue -= v;
							pDoc->SetCellValue(pCell->m_i,pCell->m_j,pCell->m_k, pCell->m_dValue);
						}						
					}
					break;
				default:
					{
						double v = atof(strCell);
						POSITION pos = m_selection.GetHeadPosition();
						while (pos != NULL)
						{
							CFieldCell*pCell = (CFieldCell*)m_selection.GetNext(pos);
							pCell->m_dValue = v;
							pDoc->SetCellValue(pCell->m_i,pCell->m_j,pCell->m_k, v);
						}
					}
					break;
				}
			}
		}
		else
		{
			double v = atof(strCell);
			POSITION pos = m_selection.GetHeadPosition();
			while (pos != NULL)
			{
				CFieldCell*pCell = (CFieldCell*)m_selection.GetNext(pos);
				pCell->m_dValue = v;
				pDoc->SetCellValue(pCell->m_i,pCell->m_j,pCell->m_k, v);
			}
		}

		Init();

		Invalidate(FALSE);

		pDoc->SetModifiedFlag();
	}
}

void C2DModelView::OnUpdateEditCell(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(!m_selection.IsEmpty());
}

void C2DModelView::OnFuntion() 
{
	// TODO: Add your command handler code here
	CMFCToolBarEditBoxButton* pSrcEdit = NULL;

	CObList listButtons;
	if (CMFCToolBar::GetCommandButtons (ID_EDIT_CELL, listButtons) > 0)
	{
		for (POSITION pos = listButtons.GetHeadPosition (); 
			pSrcEdit == NULL && pos != NULL;)
		{
			CMFCToolBarEditBoxButton* pEdit = 
				DYNAMIC_DOWNCAST (CMFCToolBarEditBoxButton, listButtons.GetNext (pos));

			if (pEdit != NULL )
			{
				pSrcEdit = pEdit;
			}
		}
	}

	if (pSrcEdit != NULL && pSrcEdit->GetEditBox () != NULL &&
		(pSrcEdit->GetEditBox ()->GetStyle () & WS_VISIBLE))
	{
		CString strTmp;
		pSrcEdit->GetEditBox ()->GetWindowText (strTmp);
		strTmp.TrimLeft();
		strTmp.TrimRight();
		if( !strTmp.IsEmpty() && strTmp[0] != '=' )			
			pSrcEdit->GetEditBox ()->SetWindowText ("="+strTmp);
		else if( strTmp.IsEmpty())
			pSrcEdit->GetEditBox ()->SetWindowText ("=");
		else
			pSrcEdit->GetEditBox ()->SetWindowText (strTmp);
	}
}

void C2DModelView::OnUpdateFuntion(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(!m_selection.IsEmpty());
}

void C2DModelView::OnDispTop() 
{
	// TODO: Add your command handler code here
	if( !m_bDispTop )
	{
		m_bDispTop = TRUE;

		C2DModelDoc* pDoc = GetDocument();		

		for( int i = 0; i< pDoc->m_gridModel.m_nGridX; i++)
		{
			for( int j =0; j< pDoc->m_gridModel.m_nGridY;j++)
			{				
				CFieldCell *pCell = pDoc->GetCell(i, j);

				pCell->m_bDispTop = TRUE;
			}
		}

		Invalidate(TRUE);
	}

}

void C2DModelView::OnUpdateDispTop(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetRadio(m_bDispTop);
}

void C2DModelView::OnDispBottom() 
{
	// TODO: Add your command handler code here
	if( m_bDispTop )
	{
		m_bDispTop = FALSE;
		C2DModelDoc* pDoc = GetDocument();

		for( int i = 0; i< pDoc->m_gridModel.m_nGridX; i++)
		{
			for( int j =0; j< pDoc->m_gridModel.m_nGridY;j++)
			{				
				CFieldCell *pCell = pDoc->GetCell(i, j);

				pCell->m_bDispTop = FALSE;
			}
		}

		Invalidate(TRUE);
	}
}

void C2DModelView::OnUpdateDispBottom(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetRadio(!m_bDispTop);
}

void C2DModelView::OnExportGraph() 
{
	// TODO: Add your command handler code here
	static char szFilter[] = "增强图元文件 (*.EMF)|*.EMF||";
	CString str;

	CFileDialog dlg(FALSE, "*.WMF", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, NULL);
	//------用来检测目前的操作系统的版本信息	
	int nStructSize = 0;
	DWORD dwVersion, dwWindowsMajorVersion, dwWindwosMinorVersion;
	dwVersion = GetVersion();
	dwWindowsMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
	dwWindwosMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));
	if (dwVersion < 0x80000000)
	{
		nStructSize = 88;
		// 2k,xp下显示新的版本
	}
	else 
	{
		nStructSize = 76;
		// 98下显示老版本		
	}
	dlg.m_ofn.lStructSize = nStructSize;
	//------用来检测目前的操作系统的版本信息
	if (dlg.DoModal() == IDOK)
		str = dlg.GetPathName();
	else 
		return;

	CMetaFileDC file;
	CClientDC dc(this);

	if (file.CreateEnhanced(&dc, str, NULL, NULL))
	{
		file.SetAttribDC(dc.m_hDC);	
		file.m_bPrinting = TRUE;
		OnPrepareDC(&file);		
		OnDraw(&file);
		file.ReleaseAttribDC();
		HENHMETAFILE hemf = file.CloseEnhanced();
		file.DeleteDC();
		::DeleteEnhMetaFile(hemf);
	}
}

void C2DModelView::OnExportDat() 
{
	// TODO: Add your command handler code here

	static char szFilter[] = "文本文件 (*.TXT)|*.TXT||";
	CString strFileName;

	CFileDialog dlg(FALSE, "*.TXT", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, NULL);
	//------用来检测目前的操作系统的版本信息	
	int nStructSize = 0;
	DWORD dwVersion, dwWindowsMajorVersion, dwWindwosMinorVersion;
	dwVersion = GetVersion();
	dwWindowsMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
	dwWindwosMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));
	if (dwVersion < 0x80000000)
	{
		nStructSize = 88;
		// 2k,xp下显示新的版本
	}
	else 
	{
		nStructSize = 76;
		// 98下显示老版本		
	}
	dlg.m_ofn.lStructSize = nStructSize;
	//------用来检测目前的操作系统的版本信息
	if (dlg.DoModal() == IDOK)
		strFileName = dlg.GetPathName();
	else 
		return;

	C2DModelDoc* pDoc = GetDocument();

	CStdioFile file;
	if(!file.Open(strFileName, CFile::modeCreate|CFile::modeWrite|CFile::typeText))
		return;

	int nSize = pDoc->m_values.size();

	CString strTmp;
	for (int i=0;i<nSize;i++)
	{
		strTmp.Format("%lf ",pDoc->m_values[i]);
		file.WriteString(strTmp);
		if( (i+1)%6 == 0 )
			file.WriteString("\n");
	}
	file.Close();
}

void C2DModelView::OnPropSet() 
{
	// TODO: Add your command handler code here
	C2DModelDoc* pDoc = GetDocument();
	CDlgGridPropSetup dlg;
	dlg.m_clrMinColor = pDoc->m_clrMin;
	dlg.m_clrMaxColor = pDoc->m_clrMax;
	dlg.m_dDispMinValue = pDoc->m_dDispMinValue;
	dlg.m_dDispMaxValue = pDoc->m_dDispMaxValue;
	dlg.m_dMinValue = pDoc->m_dMinValue;
	dlg.m_dMaxValue = pDoc->m_dMaxValue;
	CString strTmp;
	strTmp.Format("%d*%d*%d",pDoc->m_gridModel.m_nGridX, pDoc->m_gridModel.m_nGridY,pDoc->m_gridModel.m_nGridZ);
	dlg.m_strGridInfo = strTmp;
	if( dlg.DoModal() == IDOK )
	{
		pDoc->m_clrMin = dlg.m_clrMinColor;
		pDoc->m_clrMax = dlg.m_clrMaxColor;
		pDoc->m_dDispMinValue = dlg.m_dDispMinValue;
		pDoc->m_dDispMaxValue = dlg.m_dDispMaxValue;

		Init();
		pDoc->SetModifiedFlag();
		Invalidate(TRUE);
	}
}

void C2DModelView::OnUpLayer() 
{
	// TODO: Add your command handler code here
	CWaitCursor wait;
	C2DModelDoc* pDoc = GetDocument();

	CString strTitle = pDoc->GetTitle();
	int index = strTitle.Find("-");	
	int nLayer = pDoc->m_nCurLayer-1;
	strTitle = strTitle.Left(index);
	CString str;
	str.Format("- %d",nLayer+1);
	strTitle += str;
	pDoc->SetTitle(strTitle);
	pDoc->CreateGrid(nLayer);
	pDoc->UpdateAllViews(NULL, HINT_UPDATE_LAYER, NULL);
}

void C2DModelView::OnUpdateUpLayer(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	C2DModelDoc* pDoc = GetDocument();
	pCmdUI->Enable(pDoc->m_nCurLayer>0);
}

void C2DModelView::OnDownLayer() 
{
	// TODO: Add your command handler code here
	CWaitCursor wait;
	C2DModelDoc* pDoc = GetDocument();

	CString strTitle = pDoc->GetTitle();
	int index = strTitle.Find("-");	
	int nLayer = pDoc->m_nCurLayer+1;
	strTitle = strTitle.Left(index);
	CString str;
	str.Format("- %d",nLayer+1);
	strTitle += str;
	pDoc->SetTitle(strTitle);
	pDoc->CreateGrid(nLayer);
	pDoc->UpdateAllViews(NULL, HINT_UPDATE_LAYER, NULL);
}

void C2DModelView::OnUpdateDownLayer(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	C2DModelDoc* pDoc = GetDocument();
	pCmdUI->Enable(pDoc->m_nCurLayer<pDoc->m_gridModel.m_nGridZ-1);
}

void C2DModelView::OnDestroy()
{
	C2DModelDoc* pDoc = GetDocument();
	pDoc->DeleteGrid();
	CBigScrollView::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}

void C2DModelView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	// TODO: 在此添加专用代码和/或调用基类

	if( bActivate )
	{
		CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
		pFrame->ActiveView(pActivateView);
	}
	else
	{
		CMainFrame *pFrame = (CMainFrame *)AfxGetMainWnd();
		pFrame->DeactiveView(pActivateView);
	}

	CBigScrollView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void C2DModelView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	C2DModelDoc* pDoc = GetDocument();
	switch( nChar)
	{
	case VK_LEFT:
		{
			if( m_pCurCell )
			{
				int i = m_pCurCell->m_i;					
				int j = m_pCurCell->m_j;
				if( i > 0 )
				{
					CFieldCell *pCell = pDoc->GetCell(i-1,j);
					if( IsCTRLpressed() )
					{
						Select(pCell,TRUE);
					}
					else
						Select(pCell,FALSE);
				}
			}
		}
		break;
	case VK_RIGHT:
		{
			if( m_pCurCell )
			{
				int i = m_pCurCell->m_i;					
				int j = m_pCurCell->m_j;
				if( i < pDoc->m_gridModel.m_nGridX-1 )
				{
					CFieldCell *pCell = pDoc->GetCell(i+1,j);
					if( IsCTRLpressed() )
					{
						Select(pCell,TRUE);
					}
					else
						Select(pCell,FALSE);
				}
			}
		}
		break;
	case VK_UP:
		{
			if( m_pCurCell )
			{
				int i = m_pCurCell->m_i;
				int j = m_pCurCell->m_j;
				if( j < pDoc->m_gridModel.m_nGridY-1 )
				{
					CFieldCell *pCell = pDoc->GetCell(i,j+1);
					if( IsCTRLpressed() )
					{
						Select(pCell,TRUE);
					}
					else
						Select(pCell,FALSE);
				}
			}
		}
		break;
	case VK_DOWN:
		{
			if( m_pCurCell )
			{
				int i = m_pCurCell->m_i;
				int j = m_pCurCell->m_j;
				if( j > 0 )
				{
					CFieldCell *pCell = pDoc->GetCell(i,j-1);
					if( IsCTRLpressed() )
					{
						Select(pCell,TRUE);
					}
					else
						Select(pCell,FALSE);
				}
			}
		}
		break;
	case VK_ESCAPE:
		{
			m_operation = opNone;
			m_hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
			SetCursor(m_hCursor);
		}
		break;
	default:
		break;
	}

	CBigScrollView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void C2DModelView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CHandTracker trackHand(this);
	//CPoint saveCenter = m_Center;
	trackHand.TrackRubberBand(this, point);
	Invalidate(FALSE);
	//CBigScrollView::OnMButtonDown(nFlags, point);
}

void C2DModelView::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	ReleaseCapture();
	CBigScrollView::OnMButtonUp(nFlags, point);
}
