#include "stdafx.h"
#include "GraphPanel.h"
#include "f2doff.h"
#include "gruler.h"
#include "coords.h"
#include "graphwnd.h"
#include "../afc/src/Spline.h"
#include "VertModelDoc.h"
#include "VertModelView.h"
#include "../GeoMap/GeoMapView.h"
#include "../DlgRuler.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//bFlags Ϊtrue ��p1�����ӳ�
extern CPoint2D LineExtension(const CPoint2D &p1,const CPoint2D &p2,double dLength,bool bFlags);
/////////////////////////////////////////////////////////////////////////////
// CGraphPanel

CGraphPanel::CGraphPanel(FVRuler* ver_ruler, FHRuler* hor_ruler)
{
	offscreen = NULL;
	vruler = ver_ruler;
	hruler = hor_ruler;
	CurrentCoordsX = NULL;
	CurrentCoordsY = NULL;
	bkColor = RGB(230, 230, 230);
	m_grflags = (GRAPH_AUTOSCALE | GRAPH_SQUAREPOINTS | GRAPH_DRAW_AXIS);
	//zoom
	CRect r(10, 10, 50, 50);
	bZoomActive = FALSE;
	bCreateZoomRect = FALSE;
	zoomrect = new CRectTracker(&r, CRectTracker::solidLine | CRectTracker::resizeInside | CRectTracker::hatchInside);
	//panning
	m_bPanning = FALSE;
	m_PanStart.x = m_PanStart.y = 0L;
	m_PanStop.x = m_PanStop.y = 0L;

	//m_opPoint = NONE_POINT;
	m_opPoint = ADD_POINT;
	m_lCurrentGraphId = -1;
	m_iCurrentEditPt = -1;

	m_iRuler = -1;
}

CGraphPanel::~CGraphPanel()
{
	delete CurrentCoordsX;
	delete CurrentCoordsY;
	delete offscreen;
	delete zoomrect;
}


BEGIN_MESSAGE_MAP(CGraphPanel, CWnd)
	//{{AFX_MSG_MAP(CGraphPanel)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGraphPanel message handlers

BOOL CGraphPanel::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

void CGraphPanel::OnSize(UINT nType, int cx, int cy)
{
// 	if( cx>cy)
// 		SetWorldCoords(-0.1-(double)(cx-cy)/(double)cx/2.0,1.1+(double)(cx-cy)/(double)cx/2.0,-0.1,1.1,TRUE);
// 	else
// 		SetWorldCoords(-0.1,1.1,-0.1-(double)(cy-cx)/(double)cy,1.1+(double)(cy-cx)/(double)cy/2.0,TRUE);

	InitCoords(GetX1(), GetX2(), GetY1(), GetY2());

	CWnd::OnSize(nType, cx, cy);

	if (offscreen)
	{
		// update offscreen 
		CRect r;	
		GetClientRect(&r);
		CSize s = r.Size();
		offscreen->ResizeDib(s.cx, s.cy);
	}
}

int CGraphPanel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect r;
	GetClientRect(&r);
	offscreen = new FOffScreen(r.Width(), r.Height());

	EnableToolTips(TRUE);
	//create tool tip object
	tooltip.Create(this);
	tooltip.Activate(TRUE);
	tooltip.AddTool(this, (LPCTSTR)_T(""));
	tooltip.SendMessage(TTM_SETMAXTIPWIDTH, 0, SHRT_MAX);
	tooltip.SendMessage(TTM_SETDELAYTIME, TTDT_AUTOPOP, SHRT_MAX);
	tooltip.SendMessage(TTM_SETDELAYTIME, TTDT_INITIAL, 100);
	tooltip.SendMessage(TTM_SETDELAYTIME, TTDT_RESHOW, 100);

	return 0;
}

void CGraphPanel::DoRedraw(CDC* dc, LPCRECT r)
{
	if (!offscreen)
	{
		return;
	};

	CRect rect;
	if (r == NULL)
	{
		GetClientRect(&rect);
	}
	else
	{
		rect.CopyRect(r);
	};
	if (rect.IsRectEmpty())
		return;

	if (!offscreen->IsDIBReady())
	{
		UpdateGraphWindow(NULL);
	};

	offscreen->DrawDib(dc->m_hDC, &rect);
	//draw zoom rect
	if (bZoomActive || bCreateZoomRect)
		zoomrect->Draw(dc);
}

void CGraphPanel::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	PAINTSTRUCT* ps = &dc.m_ps;
	CRect rect(ps->rcPaint);

	DoRedraw(&dc, rect);
}

void CGraphPanel::DrawAxis(CDC* pDC, CRect& rect_to_draw)
{
	if (CurrentCoordsX == NULL || CurrentCoordsY == NULL)
		return;

	int i;

	//create new set of CCoordinates
	CCoordinates coords_x(rect_to_draw.left, rect_to_draw.right, CurrentCoordsX->v1(), CurrentCoordsX->v2());
	CCoordinates coords_y(rect_to_draw.bottom, rect_to_draw.top, CurrentCoordsY->v1(), CurrentCoordsY->v2());

	//draw axis and zero line first
	CPen BlackDashPen(PS_DOT, 1, RGB(255, 192, 128));
	
	CPen* OldPen = (CPen*)pDC->SelectObject(&BlackDashPen);
	pDC->SetBkMode(TRANSPARENT);
	//horizontal
	double Delta;
	double* wp;
	double* sp;
	int count, maxexp, NMax;
	NMax = hruler->GetNMax(pDC, rect_to_draw);
	coords_x.DivideInterval(NMax, &maxexp, &Delta, &count, &sp, &wp);
	for (i = 0; i < count; i++)
	{
		int x = (int)sp[i];
		pDC->MoveTo(x, rect_to_draw.bottom);
		pDC->LineTo(x, rect_to_draw.top);
	};
	if (sp != NULL)
		delete sp;
	if (wp != NULL)
		delete wp;
	//vertical
	NMax = vruler->GetNMax(pDC, rect_to_draw);
	coords_y.DivideInterval(NMax, &maxexp, &Delta, &count, &sp, &wp);
	for (i = 0; i < count; i++)
	{
		int x = (int)sp[i];
		pDC->MoveTo(rect_to_draw.right, x);
		pDC->LineTo(rect_to_draw.left, x);
	};
	if (sp != NULL)
		delete sp;
	if (wp != NULL)
		delete wp;

	pDC->SelectObject(OldPen);
}

void CGraphPanel::UpdateGraphWindow(LPCRECT rect)
{
	if (!offscreen)
	{
		return;
	}

	CRect r;
	if (rect == NULL)
		GetClientRect(&r);
	else
		r.CopyRect(rect);

	//do paint here
	CDC* dc = offscreen->GetDibCDC();
	if (dc != NULL)
	{
		CRect rect_to_draw;
		GetClientRect(rect_to_draw);
		
		DrawToDC(dc, rect_to_draw);
		offscreen->SetReady(TRUE);
	}

	CDC* cdc = GetDC();
	DoRedraw(cdc, &r);
	ReleaseDC(cdc);
}

void CGraphPanel::DrawToDC(CDC* dc_to_draw, CRect& rect_to_draw)
{
	//fill background
	CBrush bkbrush(bkColor);
	CBrush* oldbrush = (CBrush*)dc_to_draw->SelectObject(&bkbrush);

	CRect bkrect(rect_to_draw);
	bkrect.bottom += 1; bkrect.right += 1; bkrect.left -= 1; bkrect.top -= 1;
	dc_to_draw->Rectangle(bkrect);
	dc_to_draw->SelectObject(oldbrush);
	//axis
	if ((m_grflags & GRAPH_DRAW_AXIS) == GRAPH_DRAW_AXIS)
		DrawAxis(dc_to_draw, rect_to_draw);

	//draw points
	DrawPoints(dc_to_draw, rect_to_draw);

}

void CGraphPanel::InitCoords(double x1, double x2, double y1, double y2)
{
	if (CurrentCoordsX != NULL)
	{
		delete CurrentCoordsX;
		CurrentCoordsX = NULL;
	};
	if (CurrentCoordsY != NULL)
	{
		delete CurrentCoordsY;
		CurrentCoordsY = NULL;
	};

	CRect r;
	GetClientRect(&r);
	CurrentCoordsX = new CCoordinates(r.left, r.right, x1, x2);
	CurrentCoordsY = new CCoordinates(r.bottom, r.top, y1, y2);
}

void CGraphPanel::DrawPoints(CDC* dc, CRect& rect_to_draw)
{
	//draw all the points
	SSinglePoint ssp, ssp1;
	int x, y, x1, y1;
	if (CurrentCoordsX == NULL || CurrentCoordsY == NULL)
		return;
	CGraphWnd* main_wnd = get_main_graph_window();
	if (main_wnd == NULL)
		return;

	//save current state of device context
	int saved_dc = dc->SaveDC();

	//set new clipping region
	dc->IntersectClipRect(rect_to_draw);

	//create new set of CCoordinates
	CCoordinates coords_x(rect_to_draw.left, rect_to_draw.right, CurrentCoordsX->v1(), CurrentCoordsX->v2());
	CCoordinates coords_y(rect_to_draw.bottom, rect_to_draw.top, CurrentCoordsY->v1(), CurrentCoordsY->v2());

	if (m_lCurrentGraphId > -1)
	{
		CGraphProps* grprop = main_wnd->GetGraph(m_lCurrentGraphId);
		if (grprop != NULL)
		{
			if(grprop->m_bRulerIsVisible)
			{
				dc->SetBkMode(1);
				CPen pen(PS_SOLID, 1, RGB(255, 0, 255));
				CPen *pOldPen = dc->SelectObject(&pen);
				x = (int)coords_x.WtoX(grprop->p1.x);
				y = (int)coords_y.WtoX(grprop->p1.y);
				dc->MoveTo(rect_to_draw.left, y);
				dc->LineTo(rect_to_draw.right, y);				

				dc->MoveTo(x, y);
				x1 = (int)coords_x.WtoX(grprop->p2.x);
				y1 = (int)coords_y.WtoX(grprop->p2.y);
				dc->LineTo(x1, y1);
				
				Angle::AngleCalculator cal;
				// ֱ��1
				Angle::Line l1(grprop->p1.x, grprop->p1.y, grprop->p1.x+1.0,grprop->p1.y);
				// ֱ��2
				Angle::Line l2(grprop->p1.x, grprop->p1.y, grprop->p2.x,grprop->p2.y);

				// �Ƕ�
				double dAngle = cal.getDegree(l1,l2)*180.0/PI;

				dc->MoveTo(x, y);
				dc->AngleArc(x,y,40,0,-dAngle);

				dc->Ellipse(x-4,y-4,x+4,y+4);
				dc->FillSolidRect(x1-4,y1-4,8,8,RGB(0, 0, 255));

				dc->SelectObject(pOldPen);

				dc->SetTextAlign(TA_LEFT|TA_BOTTOM);
				dc->SetTextColor(RGB(0,0,255));
				CFont font;
				font.CreateFont(13,0,0,0,0,0,0,0,GB2312_CHARSET,0,0,0,0,_T("����"));
				CFont *pOldFont = dc->SelectObject(&font);
				CString msg;
				msg.Format(_T(" �Ƕ�: %.2lf��"),dAngle);
				dc->TextOut(x, y-4, msg);
				dc->SelectObject(pOldFont);
			}

			if (grprop->GetSize() > 0 && grprop->IsVisible())
			{
				//set color
				CPen* pen = grprop->GetPen();
				CBrush* brush = grprop->GetBrush();
				CPen* oldpen = (CPen*)dc->SelectObject(pen);
				CBrush* oldbrush = (CBrush*)dc->SelectObject(brush);

				CArray<POINT> pts;

				grprop->GetPoint(0, &ssp);
				x = (int)coords_x.WtoX(ssp.x);
				y = (int)coords_y.WtoX(ssp.y);
				if ((long)(m_grflags & GRAPH_SQUAREPOINTS) != 0)
					DrawSquarePoint(dc, x, y);
				POINT pt;
				pt.x = x;
				pt.y = y;
				pts.Add(pt);

				for (int i = 1; i< grprop->GetSize(); i++)
				{
					grprop->GetPoint(i, &ssp);
					x1 = (int)coords_x.WtoX(ssp.x);
					y1 = (int)coords_y.WtoX(ssp.y);
					if (x != x1 || y != y1)
					{
						if ((m_grflags & GRAPH_SQUAREPOINTS) != 0)
							DrawSquarePoint(dc, x1, y1);

						x = x1; y = y1;

						pt.x = x;
						pt.y = y;
						pts.Add(pt);
					};
				};

				if ((m_grflags & GRAPH_GRAPH_SCATTER) == 0 && pts.GetSize() > 1)
				{
					if (pts.GetSize() > 2)
					{
						SPLINE::CSpline sp(pts.GetData(), pts.GetSize());
						sp.Generate();
						sp.draw(dc->m_hDC);
						//PolySpline3(dc->m_hDC, pts.GetData(), pts.GetSize());
					}
					else
					{
						dc->Polyline(pts.GetData(), pts.GetSize());
					}
				}

				dc->SelectObject(oldpen);
				dc->SelectObject(oldbrush);
				grprop->ReleasePen(pen);
				grprop->ReleaseBrush(brush);
			}
		}
	}
	dc->RestoreDC(saved_dc);
}

int CGraphPanel::GetSquareSide(CDC* dest_dc)
{
	const double square_size = 1 / 35.0;

	int log_pix = GetDeviceCaps(dest_dc->m_hAttribDC, LOGPIXELSY);
	return (int)(square_size * log_pix);
}

void CGraphPanel::DrawSquarePoint(CDC* dc, int x, int y)
{
	int square_pix_size = GetSquareSide(dc);

	dc->Rectangle(x - square_pix_size, y - square_pix_size, x + square_pix_size + 1, y + square_pix_size + 1);
}

void CGraphPanel::SetMinX(double x, BOOL bRedraw)
{
	if (__min(GetX1(), GetX2()) == GetX1())
	{
		SetWorldCoords(x, GetX2(), GetY1(), GetY2(), bRedraw);
	}
	else
	{
		SetWorldCoords(GetX1(), x, GetY1(), GetY2(), bRedraw);
	};
}

void CGraphPanel::SetMaxX(double x, BOOL bRedraw)
{
	if (__max(GetX1(), GetX2()) == GetX1())
	{
		SetWorldCoords(x, GetX2(), GetY1(), GetY2(), bRedraw);
	}
	else
	{
		SetWorldCoords(GetX1(), x, GetY1(), GetY2(), bRedraw);
	};
}

void CGraphPanel::SetMinY(double y, BOOL bRedraw)
{
	if (__min(GetY1(), GetY2()) == GetY1())
	{
		SetWorldCoords(GetX1(), GetX2(), y, GetY2(), bRedraw);
	}
	else
	{
		SetWorldCoords(GetX1(), GetX2(), GetY1(), y, bRedraw);
	};
}

void CGraphPanel::SetMaxY(double y, BOOL bRedraw)
{
	if (__max(GetY1(), GetY2()) == GetY1())
	{
		SetWorldCoords(GetX1(), GetX2(), y, GetY2(), bRedraw);
	}
	else
	{
		SetWorldCoords(GetX1(), GetX2(), GetY1(), y, bRedraw);
	};
}

void CGraphPanel::SetWorldCoords(double _x1, double _x2, double _y1, double _y2, BOOL bRedraw)
{
	if (_x1 != GetX1() || _x2 != GetX2() || _y1 != GetY1() || _y2 != GetY2())
	{
		InitCoords(_x1, _x2, _y1, _y2);
		if (vruler != NULL)
		{
			vruler->SetMinMax(_y1, _y2, FALSE);
			if (bRedraw)
			{
				vruler->RedrawWindow();
			};
		};
		if (hruler != NULL)
		{
			hruler->SetMinMax(_x1, _x2, FALSE);
			if (bRedraw)
			{
				hruler->RedrawWindow();
			};
		};
		if (bRedraw)
		{
			UpdateGraphWindow(NULL);
		};
	};
}

double CGraphPanel::GetX1()
{
	return CurrentCoordsX == NULL ? 0 : CurrentCoordsX->v1();
}

double CGraphPanel::GetX2()
{
	return CurrentCoordsX == NULL ? 10 : CurrentCoordsX->v2();
}

double CGraphPanel::GetY1()
{
	return CurrentCoordsY == NULL ? 0 : CurrentCoordsY->v1();
}

double CGraphPanel::GetY2()
{
	return CurrentCoordsY == NULL ? 10 : CurrentCoordsY->v2();
}

void CGraphPanel::GetWorldCoords(double* x1, double* x2, double* y1, double* y2)
{
	*x1 = GetX1();
	*x2 = GetX2();
	*y1 = GetY1();
	*y2 = GetY2();
}

LRESULT CGraphPanel::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_COMMAND && (wParam > GRAPH_RBMC_FIRST && wParam < GRAPH_RBMC_LAST))
	{
		//this is right button menu commands
		EnumerateParentWindows(OnRBMenuCommandForAllParents, (void*)wParam);
	};

	if (message == CM_GRAPH_NOTIFICATION)
	{
		switch (wParam)
		{
		case GRAPH_GRAPH_CLEARED:
		case GRAPH_GRAPH_REMOVED:
		case GRAPH_POINT_REMOVED:
		case GRAPH_POINT_CHANGED:
			{
				SGraphChange* sgc = (SGraphChange*)lParam;
				if (sgc->bRedraw)
				{
					UpdateGraphWindow(NULL);
				};
			}; break;
		case GRAPH_POINT_ADDED:
			{
				AddPoint((SGraphChange *)lParam);
				return NULL;
			}; break;
		case GRAPH_GRAPH_ADDED:
			{
			}; break;
		case GRAPH_GRAPHIX_PROP:
			{
				SGraphixProps* sgp = (SGraphixProps*)lParam;
				SetGraphixFlags(sgp->flags, sgp->bRedraw);
				SetWorldCoords(sgp->x1, sgp->x2, sgp->y1, sgp->y2, sgp->bRedraw);
			}; break;
		case GRAPH_GRAPHIX_AXISPROPS:
			{
				SAxisPropsChange* sapc = (SAxisPropsChange*)lParam;
				UpdateTitles(sapc->bXAxis, sapc->bRedraw);
			}; break;
		case GRAPH_GRAPHIX_UPDATE:
			{
				if ((lParam & GRAPH_WUV_GRAPH) != 0)
				{
					UpdateGraphWindow(NULL);
				};
				if ((lParam & GRAPH_WUV_RULERS) != 0)
				{
					hruler->RedrawWindow();
					vruler->RedrawWindow();
				};
			}; break;
		};
	};	
	return CWnd::WindowProc(message, wParam, lParam);
}

void CGraphPanel::AddPoint(SGraphChange* sgc)
{
	//get main view pointer
	if (sgc->main_wnd_ptr == NULL)
		return;

	CView *pView = get_view();
	if( pView )
	{
		CDocument*pDoc = pView->GetDocument();
		pDoc->SetModifiedFlag();
	}

	SSinglePoint ssp;
	CGraphProps* sgp = sgc->main_wnd_ptr->GetGraph(sgc->graphnum);
	if (!sgp->IsVisible())
	{
		//if graph is not visible - do not do anything here
		return;
	}
	sgp->GetPoint(sgc->index, &ssp);
	BOOL bNeedRedraw = FALSE;
	if ((unsigned long)(m_grflags & GRAPH_AUTOSCALE))
	{
		//if grafix is autoscaled - just recalc coordinates and redraw the window
		if (ssp.x < __min(GetX1(), GetX2()))
		{
			SetMinX(ssp.x, FALSE);
			bNeedRedraw = TRUE;
		}
		if (ssp.x > __max(GetX1(), GetX2()))
		{
			SetMaxX(ssp.x, FALSE);
			bNeedRedraw = TRUE;
		}
		if (ssp.y < __min(GetY1(), GetY2()))
		{
			SetMinY(ssp.y, FALSE);
			bNeedRedraw = TRUE;
		}
		if (ssp.y > __max(GetY1(), GetY2()))
		{
			SetMaxY(ssp.y, FALSE);
			bNeedRedraw = TRUE;
		}
	}

	UpdateGraphWindow(NULL);
	return;
}

DWORD CGraphPanel::GetGraphixFlags()
{
	return m_grflags;
}

DWORD CGraphPanel::SetGraphixFlags(DWORD new_flags, BOOL bRedraw)
{
	DWORD old_flags = m_grflags;
	if (old_flags != new_flags)
	{
		m_grflags = new_flags;
		if (bRedraw)
		{
			UpdateGraphWindow(NULL);
		}
	}

	return old_flags;
}

void CGraphPanel::UpdateTitles(BOOL bXAxis, BOOL bRedraw)
{
	CString str;
	CGraphWnd* grview = get_main_graph_window();

	if (grview == NULL)
		return;

	CView *pView = get_view();
	if( pView )
	{
		CDocument*pDoc = pView->GetDocument();
		pDoc->SetModifiedFlag();
	}

	if (bXAxis)
	{
		grview->GetAxisProps(GRAPH_X_AXIS)->GetFullTitle(&str);
		hruler->SetNewTitles((char*)(LPCTSTR)grview->GetAxisProps(GRAPH_X_AXIS)->GetUOM(), (char*)(LPCTSTR)str);
		if (bRedraw)
			hruler->RedrawWindow();
	}
	else
	{
		grview->GetAxisProps(GRAPH_Y_AXIS)->GetFullTitle(&str);
		vruler->SetNewTitles((char*)(LPCTSTR)grview->GetAxisProps(GRAPH_Y_AXIS)->GetUOM(), (char*)(LPCTSTR)str);
		if (bRedraw)
			vruler->RedrawWindow();
	}
}

void CGraphPanel::OnMouseMove(UINT nFlags, CPoint point)
{
	//tooltips
	tipCount -= 1;
	if (tipCount <= 0)
	{
		tooltip.Activate((m_grflags & GRAPH_SHOW_TOOLTIP) == GRAPH_SHOW_TOOLTIP);
		if ((m_grflags & GRAPH_SHOW_TOOLTIP) == GRAPH_SHOW_TOOLTIP)
		{
			CString str;
			GetToolTipString(point, str);
			tooltip.UpdateTipText(str, this);
		};
		tipCount = MAX_TIP_COUNT;
	};
	//panning
	if (m_bPanning)
	{
		m_PanStart = m_PanStop;
		m_PanStop = point;
		double dx = CurrentCoordsX->XtoW(m_PanStart.x) - CurrentCoordsX->XtoW(m_PanStop.x);
		double dy = CurrentCoordsY->XtoW(m_PanStart.y) - CurrentCoordsY->XtoW(m_PanStop.y); 

		SetWorldCoords(GetX1() + dx, GetX2() + dx, GetY1() + dy, GetY2() + dy, TRUE);
	}
	else
	{
		CGraphWnd* main_wnd = get_main_graph_window();
		CGraphProps* grprop = main_wnd->GetGraph(m_lCurrentGraphId);
		CRect rect_to_draw;
		GetClientRect(rect_to_draw);
		CCoordinates coords_x(rect_to_draw.left, rect_to_draw.right, CurrentCoordsX->v1(), CurrentCoordsX->v2());
		CCoordinates coords_y(rect_to_draw.bottom, rect_to_draw.top, CurrentCoordsY->v1(), CurrentCoordsY->v2());

		if( m_iRuler >-1)
		{
			if( m_iRuler == 0 )
			{
				SSinglePoint pt;
				pt.x = grprop->p1.x;
				pt.y = grprop->p1.y;

				if( !grprop->m_bRulerLinkage )
				{
					for (int i = 0; i< grprop->GetSize(); i++)
					{
						SSinglePoint sp;
						grprop->GetPoint(i, &sp);

						CPoint pp;
						pp.x = coords_x.WtoX(sp.x);
						pp.y = coords_y.WtoX(sp.y);

						if( point.x >= pp.x - 8 && point.x<=pp.x+8 
							&& point.y >= pp.y -8 && point.y<=pp.y+8 )
						{
							point = pp;
							break;
						}					
					}
				}
				grprop->p1.x = coords_x.XtoW(point.x);
				grprop->p1.y = coords_y.XtoW(point.y);
				
				grprop->p2.x += grprop->p1.x-pt.x;
				grprop->p2.y += grprop->p1.y-pt.y;

				if( grprop->m_bRulerLinkage )
				{
					for (int i = 0; i< grprop->GetSize(); i++)
					{
						SSinglePoint ssp;
						grprop->GetPoint(i, &ssp);

						ssp.x += grprop->p1.x-pt.x;
						ssp.y += grprop->p1.y-pt.y;

						grprop->EditPoint(i,ssp.x, ssp.y,TRUE);
					}
				}
			}
			else
			{
				double dAngle1 = 0, dAngle2 = 0;
				double dis1 = 0, dis2 = 0;
				if( grprop->m_bAngleLinkage || grprop->m_bAngleZipLinkage || grprop->m_bLenLinkage )
				{
					Angle::AngleCalculator cal;
					// ֱ��1
					Angle::Line l1(grprop->p1.x, grprop->p1.y, grprop->p1.x+1.0,grprop->p1.y);
					// ֱ��2
					Angle::Line l2(grprop->p1.x, grprop->p1.y, grprop->p2.x,grprop->p2.y);

					// �Ƕ�
					dAngle1 = cal.getDegree(l1,l2);
					dis1 = sqrt((grprop->p2.x-grprop->p1.x)*(grprop->p2.x-grprop->p1.x)+(grprop->p2.y-grprop->p1.y)*(grprop->p2.y-grprop->p1.y));
				}

				grprop->p2.x = coords_x.XtoW(point.x);
				grprop->p2.y = coords_y.XtoW(point.y);

				if( grprop->m_bAngleLinkage || grprop->m_bAngleZipLinkage || grprop->m_bLenLinkage )
				{
					Angle::AngleCalculator cal;
					// ֱ��1
					Angle::Line l1(grprop->p1.x, grprop->p1.y, grprop->p1.x+1.0,grprop->p1.y);
					// ֱ��2
					Angle::Line l2(grprop->p1.x, grprop->p1.y, grprop->p2.x,grprop->p2.y);

					// �Ƕ�
					double dAngle2 = cal.getDegree(l1,l2);

					CPoint2D po;
					if( grprop->GetSize() > 0 )
					{
						SSinglePoint pp;
						grprop->GetPoint(0, &pp);
						po.x = pp.x;
						po.y = pp.y;
					}

					dis2 = sqrt((grprop->p2.x-grprop->p1.x)*(grprop->p2.x-grprop->p1.x)+(grprop->p2.y-grprop->p1.y)*(grprop->p2.y-grprop->p1.y));

					double dScale = dis2/dis1;

					double dScaleAngle = dAngle2/dAngle1;

					for (int i = 1; i< grprop->GetSize(); i++)
					{
						SSinglePoint ssp;
						SSinglePoint sp;
						grprop->GetPoint(i, &sp);

						if( fabs(sp.x - po.x)<0.00001 && fabs(sp.y - po.y)<0.00001)
							continue;

						CPoint2D p;
						if( grprop->m_bLenLinkage )
						{
							CPoint2D sp1;
							sp1.x = sp.x;
							sp1.y = sp.y;

							double d = GetPointDistance(po, sp1)*dScale;

							p = LineExtension(po, sp1, d, FALSE);
						}
						else
						{
							p.x = sp.x;
							p.y = sp.y;
						}

						p.x-=po.x;
						p.y-=po.y;

						if( grprop->m_bAngleLinkage )
						{
							ssp.x = p.x*cos(dAngle1-dAngle2) - p.y*sin(dAngle1-dAngle2) + po.x;
							ssp.y = p.x*sin(dAngle1-dAngle2) + p.y*cos(dAngle1-dAngle2) + po.y;
						}
						else if( grprop->m_bAngleZipLinkage )
						{
							Angle::AngleCalculator cal;
							// ֱ��1
							Angle::Line l1(po.x, po.y, po.x+1.0,po.y);
							// ֱ��2
							Angle::Line l2(po.x, po.y, sp.x,sp.y);
							double dA1 = cal.getDegree(l1,l2);
							double dA2 = dA1 * dScaleAngle;

							ssp.x = p.x*cos(dA1-dA2) - p.y*sin(dA1-dA2) + po.x;
							ssp.y = p.x*sin(dA1-dA2) + p.y*cos(dA1-dA2) + po.y;
						}
						else
						{
							ssp.x = p.x + po.x;
							ssp.y = p.y + po.y;
						}

						grprop->EditPoint(i,ssp.x, ssp.y,TRUE);
					}
				}
			}
			m_ldStartPoint = point;
			UpdateGraphWindow(NULL);
			main_wnd->GetPointViewBar()->InitPointList(NULL);
			CWnd::OnMouseMove(nFlags, point);
			return;
		}
		else
		{
			int x1 = (int)coords_x.WtoX(grprop->p1.x);
			int y1 = (int)coords_y.WtoX(grprop->p1.y);

			int x2 = (int)coords_x.WtoX(grprop->p2.x);
			int y2 = (int)coords_y.WtoX(grprop->p2.y);

			CRect rect1(x1 - 5, y1 - 5, x1 + 5, y1 + 5);
			CRect rect2(x2 - 5, y2 - 5, x2 + 5, y2 + 5);
			if (rect1.PtInRect(point))
			{
				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));
				if( nFlags & MK_LBUTTON )
				{
					grprop->p1.x = coords_x.XtoW(point.x);
					grprop->p1.y = coords_y.XtoW(point.y);
					UpdateGraphWindow(NULL);
				}
				CWnd::OnMouseMove(nFlags, point);
				return;
			}
			else if (rect2.PtInRect(point))
			{
				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));
				if( nFlags & MK_LBUTTON )
				{

				}
				CWnd::OnMouseMove(nFlags, point);
				return;
			}
		}

		if (m_opPoint != NONE_POINT)
		{
			if (m_opPoint == ADD_POINT)
			{	
				int index = -1;

				if (grprop != NULL)
				{
					if (grprop->GetSize() > 0 && grprop->IsVisible())
					{
						SSinglePoint ssp;

						for (int i = 0; i< grprop->GetSize(); i++)
						{
							grprop->GetPoint(i, &ssp);
							int x = (int)coords_x.WtoX(ssp.x);
							int y = (int)coords_y.WtoX(ssp.y);

							CRect rect(x - 5, y - 5, x + 5, y + 5);
							if (rect.PtInRect(point))
							{
								index = i;
								break;
							}
							else
							{
								index = -1;
							}
						}
					}
				}

				if( index == -1 )
					::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_ADD_PT));
				else
					::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_MOVE_PT));
			}

			if ((m_opPoint == MOVE_POINT|| m_opPoint == ADD_POINT) && (nFlags & MK_LBUTTON) && m_iCurrentEditPt > -1)
			{

				if (grprop->IsVisible())
				{
					SSinglePoint ssp;
					CRect rect_to_draw;
					GetClientRect(rect_to_draw);
					CCoordinates coords_x(rect_to_draw.left, rect_to_draw.right, CurrentCoordsX->v1(), CurrentCoordsX->v2());
					CCoordinates coords_y(rect_to_draw.bottom, rect_to_draw.top, CurrentCoordsY->v1(), CurrentCoordsY->v2());

					CPoint pt = point;

					if (grprop->GetSize() > 1)
					{
						int x1 = 0;
						int x2 = 0;
						if (m_iCurrentEditPt == 0)
						{
							grprop->GetPoint(m_iCurrentEditPt + 1, &ssp);
							x2 = (int)coords_x.WtoX(ssp.x);
							if (pt.x >= (x2 - 1))
								pt.x = x2 - 1;
						}
						else if ((grprop->GetSize() - 1) == m_iCurrentEditPt)
						{
							grprop->GetPoint(m_iCurrentEditPt - 1, &ssp);
							x1 = (int)coords_x.WtoX(ssp.x);
							if (pt.x <= (x1 + 1))
								pt.x = x1 + 1;
						}
						else
						{
							grprop->GetPoint(m_iCurrentEditPt - 1, &ssp);
							x1 = (int)coords_x.WtoX(ssp.x);
							if (pt.x <= (x1 + 1))
								pt.x = x1 + 1;

							grprop->GetPoint(m_iCurrentEditPt + 1, &ssp);
							x2 = (int)coords_x.WtoX(ssp.x);
							if (pt.x >= (x2 - 1))
								pt.x = x2 - 1;
						}
					}

					double x = coords_x.XtoW(pt.x);
					double y = coords_y.XtoW(pt.y);

					main_wnd->EditPoint(m_lCurrentGraphId, m_iCurrentEditPt, x, y);
				}
			}
		}
		else
		{
			//if on-the-fly zoom is active - redraw new rectangle
// 			if (bCreateZoomRect)
// 			{
// 				CRect old_rect(new_zoom_1.x, new_zoom_1.y, new_zoom_2.x, new_zoom_2.y);
// 				old_rect.NormalizeRect();
// 
// 				new_zoom_2 = point;
// 				CRect rect(new_zoom_1.x, new_zoom_1.y, new_zoom_2.x, new_zoom_2.y);
// 				rect.NormalizeRect();
// 
// 				zoomrect->m_rect = rect;
// 
// 				old_rect.UnionRect(&old_rect, &rect);
// 				old_rect.InflateRect(2, 2);
// 				InvalidateRect(&old_rect, FALSE);
// 			};
		}
	};
	CWnd::OnMouseMove(nFlags, point);
}

void CGraphPanel::OnLButtonUp(UINT nFlags, CPoint point)
{
	::ReleaseCapture();
	//panning
	if (m_bPanning)
	{
		m_bPanning = FALSE;
		m_PanStop = point;
		double dx = CurrentCoordsX->XtoW(m_PanStart.x) - CurrentCoordsX->XtoW(m_PanStop.x);
		double dy = CurrentCoordsY->XtoW(m_PanStart.y) - CurrentCoordsY->XtoW(m_PanStop.y); 
		SetWorldCoords(GetX1() + dx, GetX2() + dx, GetY1() + dy, GetY2() + dy, TRUE);
	}
	else
	{
		CGraphWnd* main_wnd = get_main_graph_window();
		CGraphProps* grprop = main_wnd->GetGraph(m_lCurrentGraphId);
		CRect rect_to_draw;
		GetClientRect(rect_to_draw);
		CCoordinates coords_x(rect_to_draw.left, rect_to_draw.right, CurrentCoordsX->v1(), CurrentCoordsX->v2());
		CCoordinates coords_y(rect_to_draw.bottom, rect_to_draw.top, CurrentCoordsY->v1(), CurrentCoordsY->v2());

		if( m_iRuler >-1)
		{
			if( m_iRuler == 0 )
			{
				SSinglePoint pt;
				pt.x = grprop->p1.x;
				pt.y = grprop->p1.y;

				if( !grprop->m_bRulerLinkage )
				{
					for (int i = 0; i< grprop->GetSize(); i++)
					{
						SSinglePoint sp;
						grprop->GetPoint(i, &sp);

						CPoint pp;
						pp.x = coords_x.WtoX(sp.x);
						pp.y = coords_y.WtoX(sp.y);

						if( point.x >= pp.x -8 && point.x<=pp.x+8 
							&& point.y >= pp.y -8 && point.y<=pp.y+8 )
						{
							point = pp;
							break;
						}					
					}
				}

				grprop->p1.x = coords_x.XtoW(point.x);
				grprop->p1.y = coords_y.XtoW(point.y);

				grprop->p2.x += grprop->p1.x-pt.x;
				grprop->p2.y += grprop->p1.y-pt.y;

				if( grprop->m_bRulerLinkage )				
				{
					for (int i = 0; i< grprop->GetSize(); i++)
					{
						SSinglePoint ssp;
						grprop->GetPoint(i, &ssp);

						ssp.x += grprop->p1.x-pt.x;
						ssp.y += grprop->p1.y-pt.y;

						grprop->EditPoint(i,ssp.x, ssp.y,TRUE);
					}
				}
			}
			else
			{
				double dAngle1 = 0, dAngle2 = 0;
				double dis1 = 0, dis2 = 0;
				if( grprop->m_bAngleLinkage || grprop->m_bAngleZipLinkage || grprop->m_bLenLinkage )
				{
					Angle::AngleCalculator cal;
					// ֱ��1
					Angle::Line l1(grprop->p1.x, grprop->p1.y, grprop->p1.x+1.0,grprop->p1.y);
					// ֱ��2
					Angle::Line l2(grprop->p1.x, grprop->p1.y, grprop->p2.x,grprop->p2.y);

					// �Ƕ�
					dAngle1 = cal.getDegree(l1,l2);

					dis1 = sqrt((grprop->p2.x-grprop->p1.x)*(grprop->p2.x-grprop->p1.x)+(grprop->p2.y-grprop->p1.y)*(grprop->p2.y-grprop->p1.y));
				}

				grprop->p2.x = coords_x.XtoW(point.x);
				grprop->p2.y = coords_y.XtoW(point.y);

				if( grprop->m_bAngleLinkage || grprop->m_bAngleZipLinkage || grprop->m_bLenLinkage )
				{
					Angle::AngleCalculator cal;
					// ֱ��1
					Angle::Line l1(grprop->p1.x, grprop->p1.y, grprop->p1.x+1.0,grprop->p1.y);
					// ֱ��2
					Angle::Line l2(grprop->p1.x, grprop->p1.y, grprop->p2.x,grprop->p2.y);

					// �Ƕ�
					double dAngle2 = cal.getDegree(l1,l2);

					CPoint2D po;
					if( grprop->GetSize() > 0 )
					{
						SSinglePoint pp;
						grprop->GetPoint(0, &pp);
						po.x = pp.x;
						po.y = pp.y;
					}

					dis2 = sqrt((grprop->p2.x-grprop->p1.x)*(grprop->p2.x-grprop->p1.x)+(grprop->p2.y-grprop->p1.y)*(grprop->p2.y-grprop->p1.y));

					double dScale = dis2/dis1;

					double dScaleAngle = dAngle2/dAngle1;

					for (int i = 1; i< grprop->GetSize(); i++)
					{
						SSinglePoint ssp;
						SSinglePoint sp;
						grprop->GetPoint(i, &sp);

						if( fabs(sp.x - po.x)<0.00001 && fabs(sp.y - po.y)<0.00001)
							continue;
						
						CPoint2D p;
						if( grprop->m_bLenLinkage )
						{
							CPoint2D sp1;
							sp1.x = sp.x;
							sp1.y = sp.y;

							double d = GetPointDistance(po, sp1)*dScale;

							p = LineExtension(po, sp1, d, FALSE);
						}
						else
						{
							p.x = sp.x;
							p.y = sp.y;
						}

						p.x-=po.x;
						p.y-=po.y;
						
						if( grprop->m_bAngleLinkage )
						{
							ssp.x = p.x*cos(dAngle1-dAngle2) - p.y*sin(dAngle1-dAngle2) + po.x;
							ssp.y = p.x*sin(dAngle1-dAngle2) + p.y*cos(dAngle1-dAngle2) + po.y;
						}
						else if( grprop->m_bAngleZipLinkage )
						{
							Angle::AngleCalculator cal;
							// ֱ��1
							Angle::Line l1(po.x, po.y, po.x+1.0,po.y);
							// ֱ��2
							Angle::Line l2(po.x, po.y, sp.x,sp.y);
							double dA1 = cal.getDegree(l1,l2);
							double dA2 = dA1 * dScaleAngle;

							ssp.x = p.x*cos(dA1-dA2) - p.y*sin(dA1-dA2) + po.x;
							ssp.y = p.x*sin(dA1-dA2) + p.y*cos(dA1-dA2) + po.y;
						}
						else
						{
							ssp.x = p.x + po.x;
							ssp.y = p.y + po.y;
						}

						grprop->EditPoint(i,ssp.x, ssp.y,TRUE);						
					}
				}
			}
			UpdateGraphWindow(NULL);
			main_wnd->GetPointViewBar()->InitPointList(NULL);
			CWnd::OnLButtonUp(nFlags, point);
			m_iRuler = -1;
			
			CView *pView = get_view();
			if( pView )
			{
				CDocument*pDoc = pView->GetDocument();
				pDoc->SetModifiedFlag();
			}
			
			return;
		}
		if (m_opPoint != NONE_POINT)
		{
			//if (m_opPoint == MOVE_POINT || m_opPoint == ADD_POINT)
			//	::ReleaseCapture();

			if (m_opPoint == ADD_POINT && m_iCurrentEditPt == -1)
			{
				if (grprop->IsVisible())
				{
					SSinglePoint ssp;
					CRect rect_to_draw;
					GetClientRect(rect_to_draw);
					CCoordinates coords_x(rect_to_draw.left, rect_to_draw.right, CurrentCoordsX->v1(), CurrentCoordsX->v2());
					CCoordinates coords_y(rect_to_draw.bottom, rect_to_draw.top, CurrentCoordsY->v1(), CurrentCoordsY->v2());

					double x = coords_x.XtoW(point.x);
					double y = coords_y.XtoW(point.y);

					if (grprop->GetSize() > 1)
					{
						grprop->GetPoint(0, &ssp);
						int x1 = (int)coords_x.WtoX(ssp.x);
						if (point.x < x1)
							main_wnd->AddPoint(m_lCurrentGraphId, x, y, TRUE, 0);
						else
						{
							grprop->GetPoint(grprop->GetSize() - 1, &ssp);

							int x2 = (int)coords_x.WtoX(ssp.x);

							if (point.x > x2)
							{
								main_wnd->AddPoint(m_lCurrentGraphId, x, y);
							}
							else
							{
								for (int i = 1; i< grprop->GetSize(); i++)
								{
									grprop->GetPoint(i, &ssp);
									x2 = (int)coords_x.WtoX(ssp.x);

									if (point.x > x1 && point.x < x2)
									{
										main_wnd->AddPoint(m_lCurrentGraphId, x, y, TRUE, i);
										break;
									}
									else
									{
										x1 = x2;
									}
								}
							}
						}
					}
					else
						main_wnd->AddPoint(m_lCurrentGraphId, x, y, TRUE, 0);
				}
			}

			m_iCurrentEditPt = -1;			
		}
		else
		{
			//zoom
// 			if (bCreateZoomRect)
// 			{
// 				CRect old_rect(new_zoom_1.x, new_zoom_1.y, new_zoom_2.x, new_zoom_2.y);
// 				old_rect.NormalizeRect();
// 
// 				new_zoom_2 = point;
// 				bCreateZoomRect = FALSE;
// 
// 				CRect rect(new_zoom_1.x, new_zoom_1.y, new_zoom_2.x, new_zoom_2.y);
// 				rect.NormalizeRect();
// 				zoomrect->m_rect = rect;
// 
// 				if (new_zoom_1 != new_zoom_2)
// 				{
// 					bZoomActive = TRUE;
// 				};
// 
// 				old_rect.UnionRect(&old_rect, &rect);
// 				old_rect.InflateRect(2, 2);
// 				InvalidateRect(&old_rect, FALSE);
// 			};
		}
	};
	CWnd::OnLButtonUp(nFlags, point);
}

void CGraphPanel::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	//panning
	if (GetKeyState(VK_SHIFT) < 0)
	{
		m_bPanning = TRUE;
		m_PanStart = point;
		m_PanStop = point;
	}
	else
	{
		CGraphWnd* main_wnd = get_main_graph_window();
		CGraphProps* grprop = main_wnd->GetGraph(m_lCurrentGraphId);
		CRect rect_to_draw;
		GetClientRect(rect_to_draw);
		CCoordinates coords_x(rect_to_draw.left, rect_to_draw.right, CurrentCoordsX->v1(), CurrentCoordsX->v2());
		CCoordinates coords_y(rect_to_draw.bottom, rect_to_draw.top, CurrentCoordsY->v1(), CurrentCoordsY->v2());

		{
			int x1 = (int)coords_x.WtoX(grprop->p1.x);
			int y1 = (int)coords_y.WtoX(grprop->p1.y);

			int x2 = (int)coords_x.WtoX(grprop->p2.x);
			int y2 = (int)coords_y.WtoX(grprop->p2.y);

			CRect rect1(x1 - 5, y1 - 5, x1 + 5, y1 + 5);
			CRect rect2(x2 - 5, y2 - 5, x2 + 5, y2 + 5);
			if (rect1.PtInRect(point))
			{
				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));
				m_iRuler = 0;
				m_ldStartPoint = point;
				CWnd::OnLButtonDown(nFlags, point);
				return;
			}
			else if (rect2.PtInRect(point))
			{
				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));
				m_iRuler = 1;				
				m_ldStartPoint = point;
				CWnd::OnLButtonDown(nFlags, point);
				return;
			}
		}

		if (m_opPoint != NONE_POINT)
		{
			if (m_opPoint == MOVE_POINT || m_opPoint == ADD_POINT)
			{
				if (grprop->GetSize() > 0 && grprop->IsVisible())
				{
					SSinglePoint ssp;
					CRect rect_to_draw;
					GetClientRect(rect_to_draw);
					CCoordinates coords_x(rect_to_draw.left, rect_to_draw.right, CurrentCoordsX->v1(), CurrentCoordsX->v2());
					CCoordinates coords_y(rect_to_draw.bottom, rect_to_draw.top, CurrentCoordsY->v1(), CurrentCoordsY->v2());

					for (int i = 0; i< grprop->GetSize(); i++)
					{
						grprop->GetPoint(i, &ssp);
						int x = (int)coords_x.WtoX(ssp.x);
						int y = (int)coords_y.WtoX(ssp.y);

						CRect rect(x - 5, y - 5, x + 5, y + 5);
						if (rect.PtInRect(point))
						{
							m_iCurrentEditPt = i;
							break;
						}
						else
						{
							m_iCurrentEditPt = -1;
						}
					}
				}
			}
			else if (m_opPoint == DEL_POINT)
			{
				if (grprop->GetSize() > 0 && grprop->IsVisible())
				{
					SSinglePoint ssp;
					CRect rect_to_draw;
					GetClientRect(rect_to_draw);
					CCoordinates coords_x(rect_to_draw.left, rect_to_draw.right, CurrentCoordsX->v1(), CurrentCoordsX->v2());
					CCoordinates coords_y(rect_to_draw.bottom, rect_to_draw.top, CurrentCoordsY->v1(), CurrentCoordsY->v2());

					for (int i = 1; i< grprop->GetSize()-1; i++)
					{
						grprop->GetPoint(i, &ssp);
						int x = (int)coords_x.WtoX(ssp.x);
						int y = (int)coords_y.WtoX(ssp.y);

						CRect rect(x - 5, y - 5, x + 5, y + 5);
						if (rect.PtInRect(point))
						{
							main_wnd->RemovePoint(m_lCurrentGraphId, i, TRUE);
							break;
						}
					}
				}
			}
		}
		else
		{
			//zoom
// 			if (!bZoomActive)
// 			{
// 				bCreateZoomRect = TRUE;
// 				new_zoom_1 = new_zoom_2 = point;
// 			}
// 
// 			if (bZoomActive)
// 			{
// 				CRect r1, r2;
// 				zoomrect->GetTrueRect(&r1);
// 				if (zoomrect->Track(this, point))
// 				{
// 					zoomrect->GetTrueRect(&r2);
// 					InvalidateRect(&r1, FALSE);
// 					InvalidateRect(&r2, FALSE);
// 				}
// 			}
		}
	}

	CWnd::OnLButtonDown(nFlags, point);
}

BOOL CGraphPanel::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (m_opPoint == ADD_POINT)
	{		
		if( m_iCurrentEditPt > -1 )
			::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_MOVE_PT));
		else
			::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_ADD_PT));
		return TRUE;
	}
	if (m_opPoint == DEL_POINT)
	{
		::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_DEL_PT));
		return TRUE;
	}
	if (m_opPoint == MOVE_POINT)
	{
		::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_MOVE_PT));
		return TRUE;
	}
	if (bZoomActive)
	{
		if (zoomrect->SetCursor(this, nHitTest))
		{
			return TRUE;
		}
	}

	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CGraphPanel::ApplyZoom()
{
	if (!bZoomActive)
		return;
	bZoomActive = FALSE;
	m_opPoint = NONE_POINT;
	CRect r;
	zoomrect->GetTrueRect(&r);
	//convert this rectangle to world coords
	double x1 = CurrentCoordsX->XtoW(r.left);
	double x2 = CurrentCoordsX->XtoW(r.right);
	double y1 = CurrentCoordsY->XtoW(r.bottom);
	double y2 = CurrentCoordsY->XtoW(r.top);

	SetWorldCoords(x1, x2, y1, y2, TRUE);
}

void CGraphPanel::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	//if (bZoomActive)
	//	ApplyZoom();

	CGraphWnd* main_wnd = get_main_graph_window();
	CGraphProps* grprop = main_wnd->GetGraph(m_lCurrentGraphId);
	CRect rect_to_draw;
	GetClientRect(rect_to_draw);
	CCoordinates coords_x(rect_to_draw.left, rect_to_draw.right, CurrentCoordsX->v1(), CurrentCoordsX->v2());
	CCoordinates coords_y(rect_to_draw.bottom, rect_to_draw.top, CurrentCoordsY->v1(), CurrentCoordsY->v2());

	{
		int x1 = (int)coords_x.WtoX(grprop->p1.x);
		int y1 = (int)coords_y.WtoX(grprop->p1.y);

		int x2 = (int)coords_x.WtoX(grprop->p2.x);
		int y2 = (int)coords_y.WtoX(grprop->p2.y);

		CRect rect1(x1 - 5, y1 - 5, x1 + 5, y1 + 5);
		CRect rect2(x2 - 5, y2 - 5, x2 + 5, y2 + 5);
		if (rect1.PtInRect(point)|| rect2.PtInRect(point))
		{
			Angle::AngleCalculator cal;
			// ֱ��1
			Angle::Line l1(grprop->p1.x, grprop->p1.y, grprop->p1.x+1.0,grprop->p1.y);
			// ֱ��2
			Angle::Line l2(grprop->p1.x, grprop->p1.y, grprop->p2.x,grprop->p2.y);

			// �Ƕ�
			double dAngle = cal.getDegree(l1,l2)*180.0/PI;
			CDlgRuler dlg;
			dlg.m_bRulerIsVisable = grprop->m_bRulerIsVisible;
			dlg.m_dAngle = dAngle;
			if( dlg.DoModal() == IDOK)
			{
				grprop->m_bRulerIsVisible = dlg.m_bRulerIsVisable;
				if( dlg.m_dAngle == 90.0 )
				{
					grprop->p2.x = grprop->p1.x;

				}
				else if( dlg.m_dAngle == 180.0)
				{
					grprop->p2.y = grprop->p1.y;
					grprop->p2.x *= -1;
				}
				else if( dlg.m_dAngle == 0.0)
				{
					grprop->p2.y = grprop->p1.y;
				}
				else
				{
					grprop->p2.y = grprop->p1.y - (grprop->p2.x-grprop->p1.x)*tan(dlg.m_dAngle/180.0*PI);
				}
			}
			UpdateGraphWindow(NULL);
		}
	}

	CWnd::OnLButtonDblClk(nFlags, point);
}

BOOL CGraphPanel::PreTranslateMessage(MSG* pMsg)
{
	tooltip.RelayEvent(pMsg);
	return CWnd::PreTranslateMessage(pMsg);
}

void CGraphPanel::GetToolTipString(CPoint pt, CString& tip_str)
{
	if (CurrentCoordsX == NULL || CurrentCoordsY == NULL)
		return;
	CGraphWnd* main_wnd = get_main_graph_window();
	if (main_wnd == NULL)
		return;

	double v1, v2;
	v1 = CurrentCoordsX->XtoW(pt.x);
	v2 = CurrentCoordsY->XtoW(pt.y);

	CString str;
	main_wnd->FormatAxisOutput(v1, TRUE, 1, str);
	CString str1;
	main_wnd->FormatAxisOutput(v2, FALSE, 1, str1);

	tip_str.Format(_T("x=%s, y=%s"), str, str1);
}

void CGraphPanel::OnRButtonUp(UINT nFlags, CPoint point)
{
	CWnd::OnRButtonUp(nFlags, point);

	ClientToScreen(&point);

	CMenu* menu = new CMenu();
	menu->CreatePopupMenu();

	//append common menu items from parent windows
	EnumerateParentWindows(AppendMenuForAllParents, (void*)menu);

	menu->TrackPopupMenu(TPM_CENTERALIGN, point.x, point.y, this);

	delete menu;
}

void CGraphPanel::AppendMenuItems(CMenu* menu)
{
	//Point marks
	UINT menu_flags = MF_STRING | MF_ENABLED;
	if ((m_grflags & GRAPH_SQUAREPOINTS) == GRAPH_SQUAREPOINTS)
	{
		menu_flags |= MF_CHECKED;
	};
	menu->AppendMenu(menu_flags, GRAPH_RBMC_TOGGLE_POINT_MARKS, _T("���ݵ�"));

	//Scatter graph
	menu_flags = MF_STRING | MF_ENABLED;
	if ((m_grflags & GRAPH_GRAPH_SCATTER) == GRAPH_GRAPH_SCATTER)
	{
		menu_flags |= MF_CHECKED;
	};
	menu->AppendMenu(menu_flags, GRAPH_RBMC_TOGGLE_SCATTER_MODE, _T("������"));

	//Point marks
	menu_flags = MF_STRING | MF_ENABLED;
	if ((m_grflags & GRAPH_SHOW_TOOLTIP) == GRAPH_SHOW_TOOLTIP)
	{
		menu_flags |= MF_CHECKED;
	};
	menu->AppendMenu(menu_flags, GRAPH_RBMC_TRACE_MOUSE, _T("�������"));

	//Point marks
	menu_flags = MF_STRING | MF_ENABLED;
	if ((m_grflags & GRAPH_DRAW_AXIS) == GRAPH_DRAW_AXIS)
	{
		menu_flags |= MF_CHECKED;
	};
	menu->AppendMenu(menu_flags, GRAPH_RBMC_SHOW_AXIS, _T("������"));

	//separator
	menu->AppendMenu(MF_SEPARATOR, 0, _T(""));

	//Zoom
	menu->AppendMenu(MF_STRING | MF_ENABLED, GRAPH_RBMC_ZOOM_RESET, _T("��ԭ"));
	CMenu sub_menu;
	sub_menu.CreateMenu();
	menu->AppendMenu(MF_POPUP | MF_ENABLED | MF_STRING, (UINT)sub_menu.m_hMenu, _T("����..."));
	//Zoom tool
	menu_flags = MF_STRING | MF_ENABLED;
	if (bZoomActive)
	{
		menu_flags |= MF_CHECKED;
	};
	sub_menu.AppendMenu(menu_flags, GRAPH_RBMC_ZOOM_TOOL, _T("��������"));

	//Apply zoom
	menu_flags = MF_STRING;
	if (bZoomActive)
	{
		menu_flags |= MF_ENABLED;
	}
	else
	{
		menu_flags |= MF_GRAYED;
	};
	sub_menu.AppendMenu(menu_flags, GRAPH_RBMC_APPLY_ZOOM, _T("Ӧ��"));

	sub_menu.AppendMenu(MF_SEPARATOR, 0, _T(""));

	sub_menu.AppendMenu(MF_STRING | MF_ENABLED, GRAPH_RBMC_FIT_WIDTH, _T("�����"));
	sub_menu.AppendMenu(MF_STRING | MF_ENABLED, GRAPH_RBMC_FIT_HEIGHT, _T("���߶�"));
	sub_menu.AppendMenu(MF_STRING | MF_ENABLED, GRAPH_RBMC_FIT_PAGE, _T("ȫ��"));

	sub_menu.Detach();

	//Fit
// 	sub_menu.CreateMenu();
// 	menu->AppendMenu(MF_POPUP | MF_ENABLED | MF_STRING, (UINT)sub_menu.m_hMenu, _T("����..."));
// 	sub_menu.AppendMenu(MF_STRING | MF_ENABLED, GRAPH_RBMC_FIT_WIDTH, _T("�����"));
// 	sub_menu.AppendMenu(MF_STRING | MF_ENABLED, GRAPH_RBMC_FIT_HEIGHT, _T("���߶�"));
// 	sub_menu.AppendMenu(MF_STRING | MF_ENABLED, GRAPH_RBMC_FIT_PAGE, _T("ȫ��"));
// 	sub_menu.Detach();

	//separator
	menu->AppendMenu(MF_SEPARATOR, 0, _T(""));
	//properties
	menu->AppendMenu(MF_STRING | MF_ENABLED, GRAPH_RBMC_VIEW_PROPERTIES, _T("����"));
}

void CGraphPanel::ToggleGraphFlag(unsigned long graph_flag, BOOL bUpdate)
{
	DWORD win_flags = GetGraphixFlags();
	if ((win_flags & graph_flag) == graph_flag)
	{
		win_flags = win_flags & (~graph_flag);
	}
	else
	{
		win_flags = win_flags | graph_flag;
	};
	SetGraphixFlags(win_flags, bUpdate);
}

void CGraphPanel::OnRBMenuCommand(UINT command_id)
{
	switch (command_id)
	{
	case GRAPH_RBMC_TOGGLE_POINT_MARKS:
		{
			ToggleGraphFlag(GRAPH_SQUAREPOINTS, TRUE);
		}; break;
	case GRAPH_RBMC_TRACE_MOUSE:
		{
			ToggleGraphFlag(GRAPH_SHOW_TOOLTIP, FALSE);
		}; break;
	case GRAPH_RBMC_SHOW_AXIS:
		{
			ToggleGraphFlag(GRAPH_DRAW_AXIS, TRUE);
		}; break;
	case GRAPH_RBMC_TOGGLE_SCATTER_MODE:
		{
			ToggleGraphFlag(GRAPH_GRAPH_SCATTER, TRUE);
		}; break;
	case GRAPH_RBMC_ZOOM_TOOL:
		{
			if (!bZoomActive)
			{
				bZoomActive = TRUE;
				zoomrect->m_rect.left = 5;
				zoomrect->m_rect.top = 5;
				zoomrect->m_rect.right = 50;
				zoomrect->m_rect.bottom = 50;
			}
			else
			{
				bZoomActive = FALSE;
				m_opPoint = NONE_POINT;
			};
			CRect r;
			zoomrect->GetTrueRect(&r);
			InvalidateRect(&r, FALSE);
		}; break;
	case GRAPH_RBMC_ZOOM_RESET:
		{
			InitCoords(GetX1(), GetX2(), GetY1(), GetY2());
			CRect rect;
			GetClientRect(rect);

// 			int x = rect.Width();
// 			int y = rect.Height();
// 
// 			if( x>y)
// 				SetWorldCoords(-0.1-(double)(x-y)/(double)x/1.0,1.1+(double)(x-y)/(double)x/1.0,-0.1,1.1,TRUE);
// 			else
// 				SetWorldCoords(-0.1,1.1,-0.1-(double)(y-x)/(double)y/1.0,1.1+(double)(y-x)/(double)y/1.0,TRUE);

			InvalidateRect(&rect, FALSE);

		};break;
	case GRAPH_RBMC_APPLY_ZOOM:
		{
			ApplyZoom();
		}; break;
	case GRAPH_RBMC_FIT_WIDTH:
	case GRAPH_RBMC_FIT_HEIGHT:
	case GRAPH_RBMC_FIT_PAGE:
		{
			DoFit(command_id);
		}; break;
	case GRAPH_RBMC_VIEW_PROPERTIES:
		{
			CPropertySheet prop_sheet(_T("����"), this);
			EnumerateParentWindows(AppendPropertyPageForAllParents, (void*)&prop_sheet);

			UINT result = prop_sheet.DoModal();

			EnumerateParentWindows(ReleasePropertyPageForAllParents, (void*)result);

			if (result == IDOK)
			{
				CGraphWnd* main_wnd = get_main_graph_window();
				main_wnd->UpdateWindows(GRAPH_WUV_ALL);
			};
		}; break;
	};
}

void CGraphPanel::DoFit(UINT fitType)
{
	CGraphWnd* main_wnd = get_main_graph_window();
	if (main_wnd == NULL)
		return;
	double x1, x2, y1, y2;
	if (!main_wnd->GetBoundRect(&x1, &x2, &y1, &y2))
		return;
	if (x1 == x2)
		x1 = x2 = x1 * 0.9;
	if (y1 == y2)
		y1 = y2 = y1 * 0.9;
	switch (fitType)
	{
	case GRAPH_RBMC_FIT_WIDTH:
		{
			y1 = GetY1(); y2 = GetY2();
		}; break;
	case GRAPH_RBMC_FIT_HEIGHT:
		{
			x1 = GetX1(); x2 = GetX2();
		}; break;
	case GRAPH_RBMC_FIT_PAGE:
		{
		}; break;
	};
	SetWorldCoords(x1, x2, y1, y2, TRUE);
}

void CGraphPanel::AppendPropertyPage(CPropertySheet* prop_sheet)
{
	graph_prop_page = new CGraphPropertyPage();

	graph_prop_page->m_x1 = GetX1();
	graph_prop_page->m_x2 = GetX2();
	graph_prop_page->m_y1 = GetY1();
	graph_prop_page->m_y2 = GetY2();
	graph_prop_page->m_PointMarks = (GetGraphixFlags() & GRAPH_SQUAREPOINTS) != 0;
	graph_prop_page->m_MouseCoords = (GetGraphixFlags() & GRAPH_SHOW_TOOLTIP) != 0;
	graph_prop_page->m_ShowAxis = (GetGraphixFlags() & GRAPH_DRAW_AXIS) != 0;

	prop_sheet->AddPage(graph_prop_page);
}

void CGraphPanel::ReleasePropertyPage(UINT dialog_status)
{
	if (dialog_status == IDOK)
	{
		//apply settings
		SetWorldCoords(graph_prop_page->m_x1, graph_prop_page->m_x2, graph_prop_page->m_y1, graph_prop_page->m_y2, FALSE);
		DWORD flags = GetGraphixFlags();
		if (graph_prop_page->m_PointMarks)
		{
			flags |= GRAPH_SQUAREPOINTS;
		}
		else
		{
			flags &= ~GRAPH_SQUAREPOINTS;
		};

		if (graph_prop_page->m_MouseCoords)
		{
			flags |= GRAPH_SHOW_TOOLTIP;
		}
		else
		{
			flags &= ~GRAPH_SHOW_TOOLTIP;
		};

		if (graph_prop_page->m_ShowAxis)
		{
			flags |= GRAPH_DRAW_AXIS;
		}
		else
		{
			flags &= ~GRAPH_DRAW_AXIS;
		};
		SetGraphixFlags(flags, FALSE);
	};
	delete graph_prop_page;
}

/////////////////////////////////////////////////////////////////////////////
// CGraphPropertyPage property page

IMPLEMENT_DYNCREATE(CGraphPropertyPage, CPropertyPage)

CGraphPropertyPage::CGraphPropertyPage() : CPropertyPage(CGraphPropertyPage::IDD)
{
	//{{AFX_DATA_INIT(CGraphPropertyPage)
	m_x1 = 0.0;
	m_x2 = 0.0;
	m_y1 = 0.0;
	m_y2 = 0.0;
	m_PointMarks = FALSE;
	m_MouseCoords = FALSE;
	m_ShowAxis = FALSE;
	//}}AFX_DATA_INIT

}

CGraphPropertyPage::~CGraphPropertyPage()
{
}

void CGraphPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGraphPropertyPage)
	DDX_Text(pDX, IDC_X1, m_x1);
	DDX_Text(pDX, IDC_X2, m_x2);
	DDX_Text(pDX, IDC_Y1, m_y1);
	DDX_Text(pDX, IDC_Y2, m_y2);
	DDX_Check(pDX, IDC_GRAPH_POINTMARKS, m_PointMarks);
	DDX_Check(pDX, IDC_GRAPH_MOUSECOORDS, m_MouseCoords);
	DDX_Check(pDX, IDC_GEN_GRAPH_PROPS_AXIS, m_ShowAxis);
	//}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CGraphPropertyPage, CPropertyPage)
	//{{AFX_MSG_MAP(CGraphPropertyPage)
	// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphPropertyPage message handlers
