// 3dLib\3DModelView.cpp : 实现文件
//

#include "stdafx.h"
#include "../Interlayer.h"
#include "3DModelView.h"
#include "../MainFrm.h"
#include "3DModelDoc.h"
#include "../Resource.h"
#include "Vertex3D.h"
#include "ClrFileDialog.h"
#include "C3DSlice.h"
#include "C3DDice.h"
#include "PsRenderer.h"
#include "3DObject.h"
// C3DModelView

#define IsSHIFTpressed() ( (GetKeyState(VK_SHIFT) & (1 << (sizeof(SHORT)*8-1))) != 0   )
#define IsCTRLpressed()  ( (GetKeyState(VK_CONTROL) & (1 << (sizeof(SHORT)*8-1))) != 0 )

extern int FindType(const CString& ext);
CString GetFileTypes(BOOL bOpenFileDialog);

IMPLEMENT_DYNCREATE(C3DModelView, CView)

C3DModelView::C3DModelView()
{
	m_lbutdown = false;
	m_mbutdown = false;
	m_rbutdown = false;
	m_bPanning = false;
	m_antialiased = true;
	m_Pen = new CPen(PS_SOLID, 1, RGB(0, 0, 0));

	m_myView = NULL;

	m_eOperate = NONE;

	m_posSliceXY = -1;
}

C3DModelView::~C3DModelView()
{
	delete m_Pen;

	if (m_myView)
		delete m_myView;
}

C3DModelDoc* C3DModelView::GetDocument()
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(C3DModelDoc)));
	return (C3DModelDoc *)m_pDocument;
}

BEGIN_MESSAGE_MAP(C3DModelView, CView)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CONTEXTMENU()
	
	ON_COMMAND(ID_TOPVIEW, OnTopview)
	ON_COMMAND(ID_BOTTOMVIEW, OnBottomview)
	ON_COMMAND(ID_FRONTVIEW, OnFrontview)
	ON_COMMAND(ID_REARVIEW, OnRearview)
	ON_COMMAND(ID_LEFTVIEW, OnLeftview)
	ON_COMMAND(ID_RIGHTVIEW, OnRightview)
	ON_COMMAND(ID_AXONVIEW, OnAxonview)
	ON_COMMAND(ID_FITALL, OnFitall)
	
	ON_COMMAND(ID_RESET, OnReset)
	ON_COMMAND(ID_DMODE, OnDisplaymode)
	ON_UPDATE_COMMAND_UI(ID_DMODE, OnUpdateDisplaymode)
	ON_COMMAND(ID_HLREMOVED, OnHlremoved)
	ON_UPDATE_COMMAND_UI(ID_HLREMOVED, OnUpdateHlremoved)
	ON_COMMAND(ID_ANTIAL, OnAntial)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_COORD, OnUpdateCoordViewer)
	ON_COMMAND(ID_SCREEN_BACKGROUNDCOLOR, OnScreenBackgroundcolor)	
	
	ON_COMMAND(ID_SMOOTH, OnSmooth)
	ON_UPDATE_COMMAND_UI(ID_SMOOTH, OnUpdateSmooth)
	ON_COMMAND(ID_VERTICAL_STRETCH, OnVerticalStretch)
	ON_UPDATE_COMMAND_UI(ID_VERTICAL_STRETCH, OnUpdateVerticalStretch)
	ON_COMMAND(ID_VERTICAL_ORIGINAL, OnVerticalOriginal)
	ON_UPDATE_COMMAND_UI(ID_VERTICAL_ORIGINAL, OnUpdateVerticalOriginal)
	ON_COMMAND(ID_VERTICAL_SHRINK, OnVerticalShrink)
	ON_UPDATE_COMMAND_UI(ID_VERTICAL_SHRINK, OnUpdateVerticalShrink)

	ON_COMMAND(ID_SELECT_OBJ, OnSelectObj)
	ON_UPDATE_COMMAND_UI(ID_SELECT_OBJ, OnUpdateSelectObj)

	ON_COMMAND(ID_MOVE_POINT,OnEditPoint)
	ON_UPDATE_COMMAND_UI(ID_MOVE_POINT,OnUpdateEditPoint)

	ON_COMMAND(ID_MOVE_EYE_HAND, OnMoveEye)
	ON_UPDATE_COMMAND_UI(ID_MOVE_EYE_HAND, OnUpdateMoveEye)

	ON_COMMAND(ID_ROTATE_EYE_HAND, OnRotateEye)
	ON_UPDATE_COMMAND_UI(ID_ROTATE_EYE_HAND, OnUpdateRotateEye)

	ON_COMMAND(ID_ZOOM_EYE_HAND, OnZoomEye)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_EYE_HAND, OnUpdateZoomEye)

	ON_COMMAND(ID_ZOOMWIN, OnZoomwin)
	ON_UPDATE_COMMAND_UI(ID_ZOOMWIN, OnUpdateZoomwin)

	ON_COMMAND(ID_SLICE_Z, OnSliceZ)
	ON_UPDATE_COMMAND_UI(ID_SLICE_Z, OnUpdateSliceZ)

	ON_COMMAND(ID_SLICE_X, OnSliceX)
	ON_UPDATE_COMMAND_UI(ID_SLICE_X, OnUpdateSliceX)

	ON_COMMAND(ID_SLICE_Y, OnSliceY)
	ON_UPDATE_COMMAND_UI(ID_SLICE_Y, OnUpdateSliceY)

	ON_COMMAND(ID_SLICE_XY, OnSliceXY)
	ON_UPDATE_COMMAND_UI(ID_SLICE_XY, OnUpdateSliceXY)

	ON_COMMAND(ID_SHOW_LEGEND, OnShowLegend)
	ON_UPDATE_COMMAND_UI(ID_SHOW_LEGEND, OnUpdateShowLegend)

	ON_COMMAND(ID_LEGEND_COLOR, OnSetLegendColor)

	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_GETDLGCODE()
	ON_WM_KEYUP()
	ON_COMMAND(ID_SMOOTH_GROUND, &C3DModelView::OnSmoothGround)
	ON_UPDATE_COMMAND_UI(ID_SMOOTH_GROUND, &C3DModelView::OnUpdateSmoothGround)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_EXPORT, &C3DModelView::OnExport)
	ON_UPDATE_COMMAND_UI(ID_EXPORT, &C3DModelView::OnUpdateExport)

	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
END_MESSAGE_MAP()

void C3DModelView::OnFilePrintPreview() 
{
	CRect rcDIB;
	GetClientRect( &rcDIB );
	OnDraw( GetDC() );
	m_ClientCapture.Capture( GetDC(), rcDIB );
	AFXPrintPreview (this);
}
// C3DModelView 绘图

void C3DModelView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
	if( pDC->IsPrinting() )
	{
		CRect rcDIB;
		GetClientRect( &rcDIB );

		rcDIB.right = rcDIB.Width();
		rcDIB.bottom = rcDIB.Height();
	
		// 取打印机页面大小（像素）
		int cxPage = pDC->GetDeviceCaps(HORZRES);
		int cyPage = pDC->GetDeviceCaps(VERTRES);
		// 取打印机每英寸像素
		int cxInch = pDC->GetDeviceCaps(LOGPIXELSX);
		int cyInch = pDC->GetDeviceCaps(LOGPIXELSY);
/*
		pDC->SetMapMode(MM_ANISOTROPIC);
		pDC->SetWindowExt( 360, 360 );
		pDC->SetViewportExt( 360.0/cyInch*360.0, 360.0/cyInch*360.0 );
*/
		CRect rcDest;
		rcDest.top = rcDest.left = 0;
		rcDest.bottom = (int)(((double)rcDIB.Height() * cxPage * cyInch)
				/ ((double)rcDIB.Width() * cxInch));
		rcDest.right = cxPage;

		m_ClientCapture.OnDraw(pDC->m_hDC, &rcDest, &rcDIB);
	}
	else
	{
		if( m_eOperate == SLICE_Z )
			m_myView->SetSliceZ(true);
		else
			m_myView->SetSliceZ(false);

		if( m_eOperate == SLICE_XY || m_eOperate == SLICE_Y || m_eOperate == SLICE_X)
			m_myView->SetSliceXY(true);
		else
			m_myView->SetSliceXY(false);

		m_myView->Draw();
	}
}


// C3DModelView 诊断

#ifdef _DEBUG
void C3DModelView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void C3DModelView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// C3DModelView 消息处理程序

void C3DModelView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
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

	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void C3DModelView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	GLfloat r, g, b;
	m_myView->GetBackgroundColor(r,g,b);
	m_myView->m_Background.SetBackgroundColor(r, g, b);
	m_myView->m_Background.SetSmoothGround(m_myView->GetSmoothGround());
	m_myView->m_myTrihedron->ShowGird(m_myView->IsShowGird());
	m_myView->m_myTrihedron->ShowLegend(m_myView->IsShowLegend());
	m_myView->m_myTrihedron->ShowAxis(m_myView->IsShowAxis());

	// TODO: 在此添加专用代码和/或调用基类
}

BOOL C3DModelView::OnPreparePrinting(CPrintInfo* pInfo)
{

	// TODO:  调用 DoPreparePrinting 以调用“打印”对话框

	return DoPreparePrinting(pInfo);
}

void C3DModelView::OnSize(UINT nType, int cx, int cy)
{
	if (m_myView)
	{
		m_myView->ReSize(cx, cy);
		m_myView->m_myTrihedron->SetModified(1);
	}
	CView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
}

int C3DModelView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	m_myView = new CGLView(this, GetDocument()->GetContext());

	return 0;
}

void C3DModelView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: 在此处添加消息处理程序代码

	if (m_myView)
	{
		delete m_myView;
		m_myView = NULL;
	}
}

BOOL C3DModelView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return TRUE;
}

void C3DModelView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	C3DModelDoc* pDoc = GetDocument();

	// ask window to give us events even if outside the view
	SetCapture();

	//  save the current mouse coordinate in min 
	m_myXmin = point.x;  m_myYmin = point.y;
	m_myXmax = point.x;  m_myYmax = point.y;

	m_lbutdown = true;
	m_lDownPnt = point;

	CVertex3D C;
	m_myView->ScreenToPoint(point.x, point.y, C);

	switch(m_eOperate)
	{
	case NONE:
		InvalidateRect(NULL, FALSE);
		break;
	case SELECT:
		{
			// 如果没有按SHIFT或CTRL 单选择
 			if (!(nFlags & MK_SHIFT) && !(nFlags & MK_CONTROL))
			{
				if( pDoc->GetContext()->m_selectedMode != GLEDIT )
					m_myView->Select(point.x, point.y);
				else
				{
					int n = m_myView->HitPoint(point.x, point.y,10);
					std::vector<CGLObject*>::iterator listIterSel = pDoc->GetContext()->m_listSelect->begin();

					C3DObject *pObj = (C3DObject *)(*listIterSel);
					if( pObj!=NULL)
					{
						pObj->m_iSelectedPoint = n;
						pObj->SetModified();
						int size = pObj->m_ArrayVertex.GetSize();
						if(n>=0&&n<size)
						{
							CVertex3D pos(pObj->GetPosX(),pObj->GetPosY(),pObj->GetPosZ());
							CVertex3D* pVert = pObj->m_ArrayVertex.GetAt(pObj->m_iSelectedPoint);
							pos.SetX(pos.GetX()+pVert->GetX());
							pos.SetY(pos.GetY()+pVert->GetY());
							pos.SetZ(pos.GetZ()+pVert->GetZ());
							m_myView->m_editAxis->SetPosition(CVector3DF(pos.x, pos.y, pos.z+2));
							m_myView->m_editAxis->Show(true);
							m_myView->m_editAxis->SetModified();
						}
					}
				}
				InvalidateRect(NULL, FALSE);
			}
			// 如果只按下MK_SHIFT 多项选择
			else if ((nFlags & MK_SHIFT) && !(nFlags & MK_CONTROL))
			{
				if( pDoc->GetContext()->m_selectedMode != GLEDIT )
					m_myView->MultiSelect(point.x, point.y);
				InvalidateRect(NULL, FALSE);
			}
			else
				InvalidateRect(NULL, FALSE);
		}
		break;
	case MOVE:
		{

		}
		break;
	case ROTATE:
		{

		}
		break;
	case ZOOM:
		{

		}
		break;
	case WINZOOM:
		{

		}
		break;
	case SLICE_Z:
		break;
	case SLICE_XY:
		{			
			GLuint id = m_myView->HitSliceXY(point.x,point.y);

			if( id == 102 )
				m_posSliceXY = 0; // 中间
			else if(id == 103)
				m_posSliceXY = 1; // 第一点
			else if(id == 104)
				m_posSliceXY = 2; // 第二点
			else 
				m_posSliceXY = -1; // 没有

		}
		break;
	case SLICE_X:
	case SLICE_Y:
		{			
			GLuint id = m_myView->HitSliceXY(point.x,point.y);

			if( id == 102 )
				m_posSliceXY = 0; // 中间			
			else 
				m_posSliceXY = -1; // 没有

		}
		break;
	default:
		break;
	}
	CView::OnLButtonDown(nFlags, point);
}

void C3DModelView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_lbutdown = false;

	C3DModelDoc* pDoc = GetDocument();

	CVertex3D C;
	m_myView->ScreenToPoint(point.x, point.y, C);

	switch(m_eOperate)
	{
	case NONE:
		m_myView->StopRotation();
		break;
	case SELECT:
		{
			m_myView->StopRotation();
 			if (!(nFlags & MK_SHIFT) && !(nFlags & MK_CONTROL))
 			{
 				DrawRectangle(false);
 				CRect rect(m_lDownPnt, point);
 				rect.NormalizeRect();
 
 				if (rect.Width() && rect.Height())
 					m_myView->SweepSelect(rect);
 
 				InvalidateRect(NULL, FALSE);
 			}			
		}
		break;
	case MOVE:
		{
			m_myView->StopPanning();
		}
		break;
	case ROTATE:
		{
			m_myView->StopRotation();
		}
		break;
	case ZOOM:
		{
			m_myView->StopZooming();
		}
		break;
	case WINZOOM:
		{
			m_myXmax = point.x;  m_myYmax = point.y;
			DrawRectangle(false);
			CRect myZoomRect(m_myXmin, m_myYmax, m_myXmax, m_myYmin);
			if ((abs(m_myXmin - m_myXmax) > 1) || (abs(m_myYmin - m_myYmax) > 1))
				// Test if the zoom window size is not null
			{
				m_myView->ZoomWindow(myZoomRect);
			}
		}
		break;
	case SLICE_Z:
		{
			CMainFrame *pMF = (CMainFrame *)AfxGetMainWnd();

			C3DObjBar*pBar = pMF->Get3DBar();
			pBar->m_wndTree.GetRootItem();
			HTREEITEM hSrc = pBar->m_wndTree.GetChildNode(TVI_ROOT, _T("图形切片"));
			HTREEITEM hSrc1 = pBar->m_wndTree.GetChildNode(hSrc, _T("横切片"));

			CString strName;
			int num = 0;
			do 
			{
				if(num>0)
					strName.Format(_T("新切片 %d"),num);
				else
					strName = _T("新切片");
				num++;
			} while(pBar->m_wndTree.IsExistChildNode(hSrc1,strName));

			C3DSlice *pSlice = new C3DSlice;
			pSlice->A = 0.0;
			pSlice->B = 0.0;
			pSlice->C = 1.0;
			pSlice->D = -C.GetZ();
			pSlice->m_bSpecified = false;
			pSlice->m_dHeight = 1.0;
			pSlice->SetObjName(strName);
			pSlice->Show(true);

			pDoc->GetContext()->AddGLObj(pSlice);

			//delete pSlice;
		}
		break;
	case SLICE_X:
	case SLICE_Y:
	case SLICE_XY:
		{
			m_myView->StopRotation();
			m_posSliceXY = -1;
		}
		break;
	default:
		break;
	}

	ReleaseCapture();

	CView::OnLButtonUp(nFlags, point);
}

void C3DModelView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_mbutdown = true;
	m_mDownPnt = point;
	SetCapture();
	CView::OnMButtonDown(nFlags, point);
}

void C3DModelView::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_mbutdown = false;
	ReleaseCapture();
	m_myView->StopPanning();
	CView::OnMButtonUp(nFlags, point);
}

void C3DModelView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_rbutdown = true;
	m_rDownPnt = point;
	SetCapture();
	CView::OnRButtonDown(nFlags, point);
}

void C3DModelView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_rbutdown = false;
	ReleaseCapture();
	m_myView->StopRotation();
	if( (m_eOperate == NONE || m_eOperate == SELECT) && m_bPanning )
	{
		m_bPanning = false;
		return;
	}
	CView::OnRButtonUp(nFlags, point);
}

void C3DModelView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CVertex3D C;
	m_myView->ScreenToPoint(point.x, point.y, C);

	switch(m_eOperate)
	{
	case NONE:
		{
			if (m_lbutdown)
			{
				m_myView->RotateView(m_lDownPnt, point);
				Invalidate(FALSE);
			}
			if (m_mbutdown)
			{				
				if( m_mDownPnt != point )
				{
					m_myView->PanView(m_mDownPnt, point);
					Invalidate(FALSE);
					m_bPanning = true;
				}
			}
		}
		break;
	case SELECT:
		{
			if (m_mbutdown)
			{
				if( m_mDownPnt != point )
				{
					m_myView->PanView(m_mDownPnt, point);
					Invalidate(FALSE);
					m_bPanning = true;
				}
			}
			if (m_lbutdown && (nFlags & MK_CONTROL))
			{
				m_myView->RotateView(m_lDownPnt, point);
				Invalidate(FALSE);
			}
			
//  			if (m_lbutdown && !(nFlags & MK_CONTROL) && !(nFlags & MK_SHIFT))
//  			{
//  				m_myXmax = point.x; m_myYmax = point.y;	
//  				DrawRectangle(true, LongDash);
//  			}
		}
		break;
	case MOVE:
		{
			if (m_lbutdown)
			{
				m_myView->PanView(m_lDownPnt, point);
				Invalidate(FALSE);
			}
		}
		break;
	case ROTATE:
		{
			if (m_lbutdown)
			{
				m_myView->RotateView(m_lDownPnt, point);
				Invalidate(FALSE);
			}
		}
		break;
	case ZOOM:
		{
			if (m_lbutdown )
			{
				m_myView->ZoomView(m_lDownPnt, point);
				Invalidate(FALSE);
			}
		}
		break;
	case WINZOOM:
		{
			if (m_lbutdown && !(nFlags & MK_CONTROL) && !(nFlags & MK_SHIFT))
			{
				m_myXmax = point.x; m_myYmax = point.y;	
				DrawRectangle(true, LongDash);
			}
		}
		break;
	case SLICE_Z:
		{	
			if( C.GetZ() > 1.0)
				m_myView->SetCurrZ(1.0);
			else if( C.GetZ() < -1.0 )
				m_myView->SetCurrZ( -1.0 );
			else
				m_myView->SetCurrZ(C.GetZ());

			Invalidate(FALSE);
		}
		break;
	case SLICE_X:
	case SLICE_Y:
	case SLICE_XY:
		{
			if( m_lbutdown && m_posSliceXY > -1 )
			{
				double x1, y1, x2, y2;
				m_myView->GetCurrXY(x1, y1, x2, y2);

				//   A = ( y2 - y1 )
				//   B = ( x1 - x2 )
				//  CC = - ( y1*B + A * x1 );
				
				if( m_posSliceXY == 0 ) // 中间
				{
					double x, y;
					x = (x2+x1)/2.0;
					y = (y2+y1)/2.0;

					double A, B, CC;

					A = ( y2 - y1 );
					B = ( x1 - x2 );

					CC = - ( C.GetY()*B + A * C.GetX() );

					if( fabs(A) < 0.000000001)
					{
						x1 = -1.0;
						y1 = -(A*x1+CC)/B;
						x2 = 1.0;
						y2 = -(A*x2+CC)/B;
						m_myView->SetCurrXY(x1, y1, x2, y2);
					}
					else if( fabs(B)<0.0000000001)
					{
						y1 = -1.0;
						x1 = -(B*y1+CC)/A;
						y2 = 1.0;
						x2 = -(B*y2+CC)/A;
						m_myView->SetCurrXY(x1, y1, x2, y2);
					}
					else
					{
						x1 = -1.0;
						y1 = -(A*x1+CC)/B;
						
						if(fabs(y1)>1.0)
						{
							y1 = -1.0;
							x1 = -(B*y1+CC)/A;
						}
						
						x2 = 1.0;
						y2 = -(A*x2+CC)/B;

						if(fabs(y2)>1.0)
						{
							y2 = 1.0;
							x2 = -(B*y2+CC)/A;
						}
						m_myView->SetCurrXY(x1, y1, x2, y2);
					}
				}
				else if( m_posSliceXY == 1 ) // 第一点
				{					
					double A, B, CC;

					double x=x2, y=y2;

					A = ( y2 - C.GetY() );
					B = ( C.GetX() - x2 );

					CC = - ( C.GetY()*B + A * C.GetX() );

					if( fabs(A) < 0.000000001)
					{
						x1 = -1.0;
						y1 = -(A*x1+CC)/B;
						x2 = 1.0;
						y2 = -(A*x2+CC)/B;						
					}
					else if( fabs(B)<0.0000000001)
					{
						y1 = -1.0;
						x1 = -(B*y1+CC)/A;
						y2 = 1.0;
						x2 = -(B*y2+CC)/A;
					}
					else
					{
						double xx[4],yy[4];
						xx[0] = -1.0;
						yy[0] = -(A*xx[0]+CC)/B;

						xx[1] = 1.0;
						yy[1] = -(A*xx[1]+CC)/B;

						yy[2] = 1.0;
						xx[2] = -(B*yy[2]+CC)/A;

						yy[3] = -1.0;
						xx[3] = -(B*yy[3]+CC)/A;

						bool b = false;
						for (int i=0;i<4;i++)
						{
							if( fabs(xx[i])<=1.0 && fabs(yy[i])<=1.0)
							{
								if( !b )
								{
									x1 = xx[i];
									y1 = yy[i];

									b = true;

								}
								else
								{
									x2 = xx[i];
									y2 = yy[i];
								}
							}
						}
					}

					if( fabs(x-x1)<0.00000001&&fabs(y-y1)<0.00000001)
						m_myView->SetCurrXY(x2, y2, x1, y1);
					else
						m_myView->SetCurrXY(x1, y1, x2, y2);

				}
				else if( m_posSliceXY == 2)
				{
					double A, B, CC;

					double x=x1, y=y1;
					A = ( C.GetY() - y1 );
					B = ( x1 - C.GetX());

					CC = - ( C.GetY()*B + A * C.GetX() );

					if( fabs(A) < 0.000000001)
					{
						x1 = -1.0;
						y1 = -(A*x1+CC)/B;
						x2 = 1.0;
						y2 = -(A*x2+CC)/B;
						
					}
					else if( fabs(B)<0.0000000001)
					{
						y1 = -1.0;
						x1 = -(B*y1+CC)/A;
						y2 = 1.0;
						x2 = -(B*y2+CC)/A;
						
					}
					else
					{
						double xx[4],yy[4];
						xx[0] = -1.0;
						yy[0] = -(A*xx[0]+CC)/B;

						xx[1] = 1.0;
						yy[1] = -(A*xx[1]+CC)/B;

						yy[2] = 1.0;
						xx[2] = -(B*yy[2]+CC)/A;

						yy[3] = -1.0;
						xx[3] = -(B*yy[3]+CC)/A;

						bool b = false;
						for (int i=0;i<4;i++)
						{
							if( fabs(xx[i])<=1.0 && fabs(yy[i])<=1.0)
							{
								if( !b )
								{
									x1 = xx[i];
									y1 = yy[i];

									b = true;

								}
								else
								{
									x2 = xx[i];
									y2 = yy[i];
								}
							}
						}
					}

					if( fabs(x-x2)<0.00000001&&fabs(y-y2)<0.00000001)
						m_myView->SetCurrXY(x2, y2, x1, y1);
					else
						m_myView->SetCurrXY(x1, y1, x2, y2);
				}
				Invalidate(FALSE);
			}
			else if (m_lbutdown)
			{
				m_myView->RotateView(m_lDownPnt, point);
				Invalidate(FALSE);
			}
		}
		break;
	default:
		break;
	}

	if( m_myView->GetContext()->m_listDisplay->size()> 0 )
	{
		CBoundingBox box = m_myView->GetContext()->GetBoundingBox();
		double xRange = box.XMax()-box.XMin();
		double yRange = box.YMax()-box.YMin();
		double zRange = box.ZMax()-box.ZMin();

		double range = (xRange>yRange?xRange:yRange);

		coord.Format("X %0.2lf  Y %0.2lf  Z%0.2lf", 
			box.XMin() + (1.0+C.GetX())/2.0*range, 
			box.YMin() + (1.0+C.GetY())/2.0*range, 
			box.ZMin() + (1.0+C.GetZ())/2.0*zRange);
	}
	else
		coord.Format("X %0.2lf  Y %0.2lf  Z%0.2lf", 
		C.GetX(), 
		C.GetY(), 
		C.GetZ());

	CView::OnMouseMove(nFlags, point);
}


void C3DModelView::OnTopview()
{
	// TODO: Add your command handler code here
	m_myView->TopView();
}

void C3DModelView::OnBottomview()
{
	// TODO: Add your command handler code here
	m_myView->BottomView();
}

void C3DModelView::OnFrontview()
{
	// TODO: Add your command handler code here
	m_myView->FrontView();
}

void C3DModelView::OnRearview()
{
	// TODO: Add your command handler code here
	m_myView->BackView();
}

void C3DModelView::OnLeftview()
{
	// TODO: Add your command handler code here
	m_myView->LeftView();
}

void C3DModelView::OnRightview()
{
	// TODO: Add your command handler code here
	m_myView->RightView();
}


void C3DModelView::OnAxonview()
{
	// TODO: Add your command handler code here
	m_myView->AxonView();
}

void C3DModelView::OnFitall()
{
	// TODO: Add your command handler code here
	m_myView->FitAll();
}

void C3DModelView::OnZoomwin()
{
	// TODO: Add your command handler code here
	if( m_eOperate == WINZOOM)
		m_eOperate = NONE;
	else
		m_eOperate = WINZOOM;

	C3DModelDoc* pDoc = GetDocument();
	pDoc->GetContext()->m_selectedMode = GLNOSELECTED;
}

void C3DModelView::OnUpdateZoomwin(CCmdUI *pCmdUI)
{
	pCmdUI->SetRadio(m_eOperate == WINZOOM);
}

void C3DModelView::OnReset()
{
	// TODO: Add your command handler code here
	m_myView->ResetView();
}

void C3DModelView::OnDisplaymode()
{
	// TODO: Add your command handler code here
	if (!m_myView->IsShaded())
		m_myView->SetDisplayMode(GLSHADED);
	else
		m_myView->SetDisplayMode(GLWIREFRAME);
}

void C3DModelView::OnUpdateDisplaymode(CCmdUI *pCmdUI)
{
	pCmdUI->SetRadio(m_myView->IsShaded());
}
void C3DModelView::OnHlremoved()
{
	// TODO: Add your command handler code here
	if( m_myView->IsHLRemoved())
		m_myView->SetHLRemoved(false);
	else
		m_myView->SetHLRemoved(true);
	this->Invalidate();
}

void C3DModelView::OnUpdateHlremoved(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_myView->IsHLRemoved());
}

void C3DModelView::OnAntial()
{
	// TODO: Add your command handler code here
	m_antialiased = !m_antialiased;
	m_myView->SetAntiAliasing(m_antialiased);
}

void C3DModelView::OnUpdateCoordViewer(CCmdUI* pCmdUI)
{
	pCmdUI->SetText((LPCTSTR)coord);
}

//
void C3DModelView::DrawRectangle(const bool  bDraw, const LineStyle aLineStyle)
{
	static		int m_DrawMode = R2_MERGEPENNOT;
	static		int StoredMinX, StoredMaxX, StoredMinY, StoredMaxY;
	static		bool m_IsVisible = false;
	static		LineStyle StoredLineStyle = aLineStyle;

	CPen* aOldPen;
	CClientDC clientDC(this);
	if (m_Pen)
		aOldPen = clientDC.SelectObject(m_Pen);

	clientDC.SetROP2(m_DrawMode);

	if (m_IsVisible) //  erase at the old position
	{
		clientDC.MoveTo(StoredMinX, StoredMinY); clientDC.LineTo(StoredMinX, StoredMaxY); 
		clientDC.LineTo(StoredMaxX, StoredMaxY); 
		clientDC.LineTo(StoredMaxX, StoredMinY); clientDC.LineTo(StoredMinX, StoredMinY);
		m_IsVisible = false;
	}

	StoredMinX = min(m_myXmin, m_myXmax);    
	StoredMinY = min(m_myYmin, m_myYmax);
	StoredMaxX = max(m_myXmin, m_myXmax);
	StoredMaxY = max(m_myYmin, m_myYmax);

	if (bDraw)
	{
		// move : draw
		if (StoredLineStyle != aLineStyle)
		{
			delete m_Pen;
			//  		   if  (aLineStyle ==Solid )
			//  			   {m_Pen = new CPen(PS_SOLID, 2, RGB(0,0,0)); m_DrawMode = R2_MERGEPENNOT;}
			//  		   else if (aLineStyle ==Dot )
			//  			   {m_Pen = new CPen(PS_DOT, 1, RGB(0,0,0));   m_DrawMode = R2_XORPEN;}
			//  		   else if (aLineStyle == ShortDash)
			//  			   {m_Pen = new CPen(PS_DASH, 1, RGB(255,0,0));	m_DrawMode = R2_XORPEN;}
			//  		   else if (aLineStyle == LongDash)
			//  			   {m_Pen = new CPen(PS_DASH, 1, RGB(0,0,0));	m_DrawMode = R2_NOTXORPEN;}
			//  		   clientDC.SelectObject(m_Pen);

			if (aLineStyle == Solid)
			{
				m_Pen = new CPen(PS_SOLID, 0, RGB(0, 0, 0)); m_DrawMode = R2_MERGEPENNOT;
			}
			else if (aLineStyle == Dot)
			{
				m_Pen = new CPen(PS_DOT, 0, RGB(0, 0, 0));   m_DrawMode = R2_XORPEN;
			}
			else if (aLineStyle == ShortDash)
			{
				m_Pen = new CPen(PS_DASH, 0, RGB(255, 0, 0));	m_DrawMode = R2_XORPEN;
			}
			else if (aLineStyle == LongDash)
			{
				m_Pen = new CPen(PS_DASH, 0, RGB(0, 0, 0));	m_DrawMode = R2_NOTXORPEN;
			}
			clientDC.SelectObject(m_Pen);
		}

		clientDC.SetROP2(m_DrawMode);
		clientDC.MoveTo(StoredMinX, StoredMinY);
		clientDC.LineTo(StoredMinX, StoredMaxY); 
		clientDC.LineTo(StoredMaxX, StoredMaxY); 
		clientDC.LineTo(StoredMaxX, StoredMinY);
		clientDC.LineTo(StoredMinX, StoredMinY);
		m_IsVisible = true;
	}

	if (m_Pen)
		clientDC.SelectObject(aOldPen);
}


void C3DModelView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: Add your message handler code here
	//return;
	if (GetAsyncKeyState(VK_CONTROL))
		return;
	CRect rcClient;
	GetClientRect(&rcClient);

	int cx = rcClient.Width() / 2;
	int cy = rcClient.Height() / 2;
	CRect rcShape(cx - 45, cy - 45, cx + 45, cy + 45);

	CPoint pos = point;
	ScreenToClient(&pos);

	CGLDisplayContext* ctx = GetDocument()->GetContext();
// 	if (ctx->HasSelected())
// 	{
//  		CMenu menu;
//  		menu.LoadMenu(IDR_POPUP_3DVIEW);
//  		CMenu* pContextMenu = menu.GetSubMenu(0);
//  		SetMenuDefaultItem(pContextMenu->m_hMenu, 0, TRUE);
//  		//pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
// 		theApp.GetContextMenuManager()->ShowPopupMenu(pContextMenu->m_hMenu, point.x, point.y, this, TRUE);
//  		return;
// 	}
// 	else
	{
 		CMenu menu;
 		menu.LoadMenu(IDR_POPUP_3DVIEW);
 		CMenu* pContextMenu = menu.GetSubMenu(1);
 		SetMenuDefaultItem(pContextMenu->m_hMenu, 0, TRUE);
 		//pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
		theApp.GetContextMenuManager()->ShowPopupMenu(pContextMenu->m_hMenu, point.x, point.y, this, TRUE);
 		return;
	}

	CWnd::OnContextMenu(pWnd, point);
}


void C3DModelView::OnScreenBackgroundcolor()
{
	// TODO: Add your command handler code here
	CMFCColorDialog dlg;
	GLfloat m_Red;
	GLfloat m_Green;
	GLfloat m_Blue;
	m_myView->GetBackgroundColor(m_Red, m_Green, m_Blue);
	dlg.SetCurrentColor(RGB(m_Red*255, m_Green*255, m_Blue*255));
	if (dlg.DoModal() == IDOK)
	{
		COLORREF color = dlg.GetColor();
		GLfloat m_Red = (GLfloat)GetRValue(color);
		GLfloat m_Green = (GLfloat)GetGValue(color);
		GLfloat m_Blue = (GLfloat)GetBValue(color);
		m_myView->SetBackgroundColor(m_Red / 255, m_Green / 255, m_Blue / 255);
	}
}

void C3DModelView::OnSmooth()
{
	// TODO: 在此添加命令处理程序代码
	if( m_myView->IsSmooth())
		m_myView->SetSmooth(false);
	else
		m_myView->SetSmooth(true);
	this->Invalidate();
}

void C3DModelView::OnUpdateSmooth(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_myView->IsSmooth());
}

void C3DModelView::OnVerticalStretch()
{
	// TODO: 在此添加命令处理程序代码
	m_myView->VerticalStretch();
}

void C3DModelView::OnUpdateVerticalStretch(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(m_myView->GetVertical()<100.);
}

void C3DModelView::OnVerticalOriginal()
{
	// TODO: 在此添加命令处理程序代码
	m_myView->VerticalOriginal();
}

void C3DModelView::OnUpdateVerticalOriginal(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(fabs(m_myView->GetVertical()-m_myView->GetVerticalOriginal())>0.0000001);
}

void C3DModelView::OnVerticalShrink()
{
	// TODO: 在此添加命令处理程序代码
	m_myView->VerticalShrink();
}

void C3DModelView::OnUpdateVerticalShrink(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码

	pCmdUI->Enable(m_myView->GetVertical()>0.1);

}

void C3DModelView::OnSelectObj()
{
	m_eOperate = SELECT;

	C3DModelDoc* pDoc = GetDocument();
	pDoc->GetContext()->m_selectedMode = GLSELECTED;	
}

void C3DModelView::OnUpdateSelectObj(CCmdUI *pCmdUI)
{
	pCmdUI->SetRadio(m_eOperate == SELECT);
}

void C3DModelView::OnEditPoint()
{
	C3DModelDoc* pDoc = GetDocument();
	
	if( pDoc->GetContext()->m_selectedMode == GLEDIT)
		pDoc->GetContext()->m_selectedMode = GLSELECTED;
	else
	{
		pDoc->GetContext()->m_selectedMode = GLEDIT;
		
		std::vector<CGLObject*>::iterator listIter;
		CGLObject *pObj = NULL;

		for (listIter = pDoc->GetContext()->m_listDisplay->begin(); listIter != pDoc->GetContext()->m_listDisplay->end(); listIter++)
		{
			pObj = (*listIter);
			pObj->SetModified();
		}			
	}

	Invalidate(FALSE);
}

void C3DModelView::OnUpdateEditPoint(CCmdUI *pCmdUI)
{
	if( m_eOperate == SELECT)
	{
		C3DModelDoc* pDoc = GetDocument();
		if( pDoc->GetContext()->m_listSelect->size() > 0 )
		{
			pCmdUI->Enable(TRUE);
			pCmdUI->SetRadio(pDoc->GetContext()->m_selectedMode == GLEDIT);
		}
		else
			pCmdUI->Enable(FALSE);
	}
	else
		pCmdUI->Enable(FALSE);	
}


void C3DModelView::OnMoveEye()
{
	if( m_eOperate != MOVE)
		m_eOperate = MOVE;
	else
		m_eOperate = NONE;

	C3DModelDoc* pDoc = GetDocument();
	pDoc->GetContext()->m_selectedMode = GLNOSELECTED;
}
void C3DModelView::OnUpdateMoveEye(CCmdUI *pCmdUI)
{
	pCmdUI->SetRadio(m_eOperate == MOVE);
}

void C3DModelView::OnRotateEye()
{
	if( m_eOperate != ROTATE)
		m_eOperate = ROTATE;
	else
		m_eOperate = NONE;

	C3DModelDoc* pDoc = GetDocument();
	pDoc->GetContext()->m_selectedMode = GLNOSELECTED;
}
void C3DModelView::OnUpdateRotateEye(CCmdUI *pCmdUI)
{
	pCmdUI->SetRadio(m_eOperate == ROTATE);
}

void C3DModelView::OnZoomEye()
{
	if( m_eOperate != ZOOM)
		m_eOperate = ZOOM;
	else
		m_eOperate = NONE;

	C3DModelDoc* pDoc = GetDocument();
	pDoc->GetContext()->m_selectedMode = GLNOSELECTED;
}
void C3DModelView::OnUpdateZoomEye(CCmdUI *pCmdUI)
{
	pCmdUI->SetRadio(m_eOperate == ZOOM);
}

void C3DModelView::OnSliceZ()
{
	if( m_eOperate != SLICE_Z)
		m_eOperate = SLICE_Z;
	else
		m_eOperate = NONE;

	C3DModelDoc* pDoc = GetDocument();
	pDoc->GetContext()->m_selectedMode = GLNOSELECTED;

	Invalidate(FALSE);
}

void C3DModelView::OnUpdateSliceZ(CCmdUI *pCmdUI)
{
	pCmdUI->SetRadio(m_eOperate == SLICE_Z);
}


void C3DModelView::OnSliceX()
{
	if( m_eOperate != SLICE_X)
	{
		m_myView->SetCurrXY(0.0, -1.0, 0.0, 1.0);
		m_eOperate = SLICE_X;
	}
	else
		m_eOperate = NONE;

	C3DModelDoc* pDoc = GetDocument();
	pDoc->GetContext()->m_selectedMode = GLNOSELECTED;

	Invalidate(FALSE);
}

void C3DModelView::OnUpdateSliceX(CCmdUI *pCmdUI)
{
	pCmdUI->SetRadio(m_eOperate == SLICE_X);
}

void C3DModelView::OnSliceY()
{
	if( m_eOperate != SLICE_Y)
	{
		m_myView->SetCurrXY(-1.0, 0.0, 1.0, 0.0);
		m_eOperate = SLICE_Y;
	}
	else
		m_eOperate = NONE;

	C3DModelDoc* pDoc = GetDocument();
	pDoc->GetContext()->m_selectedMode = GLNOSELECTED;

	Invalidate(FALSE);
}

void C3DModelView::OnUpdateSliceY(CCmdUI *pCmdUI)
{
	pCmdUI->SetRadio(m_eOperate == SLICE_Y);
}

void C3DModelView::OnSliceXY()
{
	if( m_eOperate != SLICE_XY)
		m_eOperate = SLICE_XY;
	else
		m_eOperate = NONE;

	C3DModelDoc* pDoc = GetDocument();
	pDoc->GetContext()->m_selectedMode = GLNOSELECTED;

	Invalidate(FALSE);
}

void C3DModelView::OnUpdateSliceXY(CCmdUI *pCmdUI)
{
	pCmdUI->SetRadio(m_eOperate == SLICE_XY);
}

void C3DModelView::OnShowLegend()
{
	if( m_myView->m_myTrihedron->IsShowLegend() )
		m_myView->m_myTrihedron->ShowLegend(false);
	else
		m_myView->m_myTrihedron->ShowLegend(true);

	Invalidate(FALSE);
}

void C3DModelView::OnUpdateShowLegend(CCmdUI *pCmdUI)
{
	pCmdUI->SetRadio(m_myView->m_myTrihedron->IsShowLegend());
}

void C3DModelView::OnSetLegendColor()
{
	TCHAR szCurPath[MAX_PATH];  
	memset(szCurPath,   0,   MAX_PATH);  
	GetModuleFileName(NULL,   szCurPath,   sizeof(szCurPath)/sizeof(TCHAR));   
	CString strMoudlePath = szCurPath;
	strMoudlePath = strMoudlePath.Left(strMoudlePath.ReverseFind('\\')+1);	
	strMoudlePath += _T("Config");

	CClrFileDialog Dlg(
		TRUE, // TRUE for FileOpen, FALSE for FileSaveAs
		_T("clr"),
		NULL,	//LPCTSTR lpszFileName = NULL,
		OFN_HIDEREADONLY  | OFN_OVERWRITEPROMPT | OFN_ENABLETEMPLATE,
		_T("颜色定义 (*.clr) |*.clr||"),
		NULL);


	Dlg.m_ofn.lpstrInitialDir = strMoudlePath;

	if(Dlg.DoModal()!=IDOK)
		return;
	
	m_myView->GetContext()->GetColorTable()->LoadColorTable(Dlg.GetPathName());

	C3DModelDoc* pDoc = GetDocument();

	std::vector<CGLObject*>::iterator listIter;
	CGLObject *pObj = NULL;

	if (!pDoc->GetContext()->m_listDisplay->empty())
	{
		for (listIter = pDoc->GetContext()->m_listDisplay->begin(); listIter != pDoc->GetContext()->m_listDisplay->end(); listIter++)
		{
			pObj = (*listIter);
			if( pObj->GetGLObjType() == GLSURFACE )
				pObj->SetModified();
		}
	}	

	pDoc->SetModifiedFlag();

	Invalidate(FALSE);
}

BOOL C3DModelView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//if( m_eOperate == NONE || m_eOperate == SELECT )
	{
		CPoint point = pt;
		pt.y += zDelta/10.0;
		m_myView->ZoomView(pt, point);
		Invalidate(FALSE);
		m_myView->StopZooming();
	}
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void C3DModelView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	C3DModelDoc *pDoc = GetDocument();
	switch(nChar)
	{
	case VK_LEFT:
		{
			if(m_eOperate == SELECT && pDoc->GetContext()->m_selectedMode == GLEDIT)
			{
				CBoundingBox box = pDoc->GetContext()->GetBoundingBox();

				double xRange = box.XMax()-box.XMin();
				double yRange = box.YMax()-box.YMin();

				double range = (xRange>yRange?xRange:yRange)*0.001;

				std::vector<CGLObject*>::iterator listIterSel = pDoc->GetContext()->m_listSelect->begin();

				C3DObject *pObj = (C3DObject *)(*listIterSel);
				if( pObj!=NULL)
				{
					if(IsCTRLpressed())
					{
						if(pObj->m_iSelectedPoint>-1)
						{
							CVertex3D* pVert = pObj->m_ArrayVertex.GetAt(pObj->m_iSelectedPoint);
							pVert->x = pVert->x - range;
							pObj->Rebuild();
						}
					}
					else
					{
						if(pObj->m_iSelectedPoint>-1)
						{
							CVertex3D* pVert = pObj->m_ArrayVertex.GetAt(pObj->m_iSelectedPoint);
							pVert->y = pVert->y - range;
							pObj->Rebuild();
						}
					}

					pObj->SetModified();
				}
			}
			else
			{
				CPoint pt[2];
				pt[0].x = 0;
				pt[0].y = 0;
				pt[1].x = -1;
				pt[1].y = 0;

				if(IsCTRLpressed())
				{
					m_myView->PanView(pt[0], pt[1]);
					m_myView->StopPanning();
				}
				else
				{
					m_myView->RotateView(pt[0], pt[1]);
					m_myView->StopRotation();
				}
			}

			Invalidate(FALSE);
		}
		break;
	case VK_RIGHT:
		{
			if(m_eOperate == SELECT && pDoc->GetContext()->m_selectedMode == GLEDIT)
			{
				CBoundingBox box = pDoc->GetContext()->GetBoundingBox();

				double xRange = box.XMax()-box.XMin();
				double yRange = box.YMax()-box.YMin();

				double range = (xRange>yRange?xRange:yRange)*0.001;

				std::vector<CGLObject*>::iterator listIterSel = pDoc->GetContext()->m_listSelect->begin();
				C3DObject *pObj = (C3DObject *)(*listIterSel);
				if( pObj!=NULL)
				{
					std::vector<CVertex3D>::iterator myListIter;
					if(IsCTRLpressed())
					{
						if(pObj->m_iSelectedPoint>-1)
						{
							CVertex3D* pVert = pObj->m_ArrayVertex.GetAt(pObj->m_iSelectedPoint);
							pVert->x = pVert->x + range;
							pObj->Rebuild();
						}
					}
					else
					{
						if(pObj->m_iSelectedPoint>-1)
						{
							CVertex3D* pVert = pObj->m_ArrayVertex.GetAt(pObj->m_iSelectedPoint);
							pVert->y = pVert->y + range;
							pObj->Rebuild();
						}
					}

					pObj->SetModified();
				}
			}
			else
			{
				CPoint pt[2];
				pt[0].x = 0;
				pt[0].y = 0;
				pt[1].x = 1;
				pt[1].y = 0;

				if(IsCTRLpressed())
				{
					m_myView->PanView(pt[0], pt[1]);
					m_myView->StopPanning();
				}
				else
				{
					m_myView->RotateView(pt[0], pt[1]);
					m_myView->StopRotation();
				}
			}

			Invalidate(FALSE);
		}
		break;
	case VK_UP:
		{
			if(m_eOperate == SELECT && pDoc->GetContext()->m_selectedMode == GLEDIT)
			{
				CBoundingBox box = pDoc->GetContext()->GetBoundingBox();

				double xRange = box.XMax()-box.XMin();
				double yRange = box.YMax()-box.YMin();
				double zRange = (box.ZMax()-box.ZMin());

				double range = (xRange>yRange?xRange:yRange);

				std::vector<CGLObject*>::iterator listIterSel = pDoc->GetContext()->m_listSelect->begin();
				C3DObject *pObj = (C3DObject *)(*listIterSel);
				if( pObj!=NULL)
				{
					if(IsCTRLpressed())
					{
						// x = x0 + mt
						// y = y0 + nt
						// z = z0 + pt

						if(pObj->m_iSelectedPoint>-1)
						{
							CVertex3D* pVert = pObj->m_ArrayVertex.GetAt(pObj->m_iSelectedPoint);
							double m = pVert->GetNormal()->GetX();
							double n = pVert->GetNormal()->GetY();
							double p = pVert->GetNormal()->GetZ();

							pVert->x = pVert->x + m*0.001*range;
							pVert->y = pVert->y + n*0.001*range;
							pVert->z = pVert->z + p*0.001*zRange;

							pObj->Rebuild();
						}
					}
					else
					{
						if(pObj->m_iSelectedPoint>-1)
						{
							CVertex3D* pVert = pObj->m_ArrayVertex.GetAt(pObj->m_iSelectedPoint);
							pVert->z = pVert->z + zRange*0.001;
							pObj->Rebuild();
						}
					}

					pObj->SetModified();
				}
			}
			else
			{
				CPoint pt[2];
				pt[0].x = 0;
				pt[0].y = 0;
				pt[1].x = 0;
				pt[1].y = -1;

				if(IsCTRLpressed())
				{
					m_myView->PanView(pt[0], pt[1]);
					m_myView->StopPanning();
				}
				else
				{
					m_myView->RotateView(pt[0], pt[1]);
					m_myView->StopRotation();
				}
			}
			Invalidate(FALSE);
		}
		break;
	case VK_DOWN:
		{
			if(m_eOperate == SELECT && pDoc->GetContext()->m_selectedMode == GLEDIT)
			{
				CBoundingBox box = pDoc->GetContext()->GetBoundingBox();

				double xRange = box.XMax()-box.XMin();
				double yRange = box.YMax()-box.YMin();
				double zRange = (box.ZMax()-box.ZMin());

				double range = (xRange>yRange?xRange:yRange);

				std::vector<CGLObject*>::iterator listIterSel = pDoc->GetContext()->m_listSelect->begin();
				C3DObject *pObj = (C3DObject *)(*listIterSel);
				if( pObj!=NULL)
				{
					std::vector<CVertex3D>::iterator myListIter;
					if(IsCTRLpressed())
					{
						if(pObj->m_iSelectedPoint>-1)
						{
							CVertex3D* pVert = pObj->m_ArrayVertex.GetAt(pObj->m_iSelectedPoint);
							double m = pVert->GetNormal()->GetX();
							double n = pVert->GetNormal()->GetY();
							double p = pVert->GetNormal()->GetZ();

							pVert->x = pVert->x - m*0.001*range;
							pVert->y = pVert->y - n*0.001*range;
							pVert->z = pVert->z - p*0.001*zRange;

							pObj->Rebuild();
						}
					}
					else
					{
						if(pObj->m_iSelectedPoint>-1)
						{
							CVertex3D* pVert = pObj->m_ArrayVertex.GetAt(pObj->m_iSelectedPoint);
							pVert->z = pVert->z - zRange*0.001;
							pObj->Rebuild();
						}
					}

					pObj->SetModified();
				}
			}
			else
			{
				CPoint pt[2];
				pt[0].x = 0;
				pt[0].y = 0;
				pt[1].x = 0;
				pt[1].y = 1;

				if(IsCTRLpressed())
				{
					m_myView->PanView(pt[0], pt[1]);
					m_myView->StopPanning();
				}
				else
				{
					m_myView->RotateView(pt[0], pt[1]);
					m_myView->StopRotation();
				}
			}

			Invalidate(FALSE);
		}
		break;
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void C3DModelView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CView::OnChar(nChar, nRepCnt, nFlags);
}

UINT C3DModelView::OnGetDlgCode()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return DLGC_WANTALLKEYS;
}

void C3DModelView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	C3DModelDoc* pDoc = GetDocument();

	switch(nChar)
	{
	case VK_ESCAPE:
		m_eOperate = NONE;
		pDoc->GetContext()->m_selectedMode = GLNOSELECTED;
		break;
	case VK_RETURN:
		if( m_eOperate == SLICE_X
			|| m_eOperate == SLICE_Y
			|| m_eOperate == SLICE_XY)
		{
			double x1, y1, x2, y2;
			m_myView->GetCurrXY(x1, y1, x2, y2);

			CMainFrame *pMF = (CMainFrame *)AfxGetMainWnd();

			C3DObjBar*pBar = pMF->Get3DBar();
			pBar->m_wndTree.GetRootItem();
			HTREEITEM hSrc = pBar->m_wndTree.GetChildNode(TVI_ROOT, _T("图形切片"));
			HTREEITEM hSrc1 = pBar->m_wndTree.GetChildNode(hSrc, _T("纵切片"));

			CString strName;
			int num = 0;
			do 
			{
				if(num>0)
					strName.Format(_T("新切片 %d"),num);
				else
					strName = _T("新切片");
				num++;
			} while(pBar->m_wndTree.IsExistChildNode(hSrc1,strName));

			C3DSlice *pSlice = new C3DSlice;
			pSlice->A = y2-y1;
			pSlice->B = x1-x2;
			pSlice->C = 0.0;
			pSlice->D = - ( y1*pSlice->B + pSlice->A * x1 );
			pSlice->m_bSpecified = false;
			pSlice->m_dHeight = 1.0;
			pSlice->SetObjName(strName);
			pSlice->Show(true);

			pDoc->GetContext()->AddGLObj(pSlice);

			//delete pSlice;

			pDoc->UpdateAllViews(NULL);
		}
		break;
	}
	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void C3DModelView::OnSmoothGround()
{
	// TODO: 在此添加命令处理程序代码

	if( m_myView->GetSmoothGround() )
		m_myView->SetSmoothGround(false);
	else
		m_myView->SetSmoothGround(true);
}

void C3DModelView::OnUpdateSmoothGround(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码

	pCmdUI->SetRadio(m_myView->GetSmoothGround());
}

void C3DModelView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	C3DModelDoc* pDoc = GetDocument();

	switch(m_eOperate)
	{
	case SLICE_X:
	case SLICE_Y:
	case SLICE_XY:
		{
			double x1, y1, x2, y2;
			m_myView->GetCurrXY(x1, y1, x2, y2);

			CMainFrame *pMF = (CMainFrame *)AfxGetMainWnd();

			C3DObjBar*pBar = pMF->Get3DBar();
			pBar->m_wndTree.GetRootItem();
			HTREEITEM hSrc = pBar->m_wndTree.GetChildNode(TVI_ROOT, _T("图形切片"));
			HTREEITEM hSrc1 = pBar->m_wndTree.GetChildNode(hSrc, _T("纵切片"));

			CString strName;
			int num = 0;
			do 
			{
				if(num>0)
					strName.Format(_T("新切片 %d"),num);
				else
					strName = _T("新切片");
				num++;
			} while(pBar->m_wndTree.IsExistChildNode(hSrc1,strName));

			C3DSlice *pSlice = new C3DSlice;
			pSlice->A = y2-y1;
			pSlice->B = x1-x2;
			pSlice->C = 0.0;
			pSlice->D = - ( y1*pSlice->B + pSlice->A * x1 );
			pSlice->m_bSpecified = false;
			pSlice->m_dHeight = 1.0;
			pSlice->SetObjName(strName);
			pSlice->Show(true);

			pDoc->GetContext()->AddGLObj(pSlice);

			//delete pSlice;

			pDoc->UpdateAllViews(NULL);
		}
		break;
	default:
		break;
	}
	CView::OnLButtonDblClk(nFlags, point);
}

void C3DModelView::OnExport()
{
	// TODO: 在此添加命令处理程序代码

	C3DModelDoc* pDoc = GetDocument();

	CString strFilter = GetFileTypes(FALSE);
	
	CString filename = pDoc->GetTitle();

	CString ext = "";

	if(filename.IsEmpty())
		filename = "NoName";

	CFileDialog dlg(false, _T("jpg"), filename, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT);

	dlg.m_ofn.lpstrFilter = strFilter;

	if(dlg.DoModal() == IDOK)
	{
		ext = dlg.GetFileExt();
		filename = dlg.GetPathName();

		wglMakeCurrent(m_myView->m_hDC,m_myView->m_hRC);
		m_myView->RenderScene(GL_RENDER);
		::SwapBuffers(m_myView->m_hDC);

		CRect rect;
		GetClientRect(rect);
		int width = rect.Width();
		int height = rect.Height();

		int expand = 0;
		if((width *3)%4)
			expand = 4 - (width*3)%4;	//保证位图宽度能被4整除

		int mapSize = (width*3 +expand) * (height);

		if(mapSize == 0)
			return;

		unsigned char * pixel = new BYTE[mapSize]; 

		if(!pixel)
			return ;
		
		// 读取屏幕像素 
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 
		glReadPixels(0, 0, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixel);

		// BMP信息头 
		CxImage image;
		image.CreateFromArray(pixel,width,height,24,width*3 + expand,false);

		image.SetJpegQuality(98); //指定JPG文件的质量（0-100）

		int type = FindType(ext);
		image.Save(filename,type);		
		delete[] pixel;

		wglMakeCurrent(NULL, NULL);
	}
}

void C3DModelView::OnUpdateExport(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	C3DModelDoc* pDoc = GetDocument();
	pCmdUI->Enable(pDoc->GetContext()->m_listDisplay->size()>0);
}


unsigned char *C3DModelView::SnapClient(CSize *pSize)
{
	BeginWaitCursor();

	// Client zone
	CRect rect;
	GetClientRect(&rect);
	//CSize size(CTexture::LowerPowerOfTwo(rect.Width()),rect.Height());
	CSize size(rect.Width(),rect.Height());
	*pSize = size;

	ASSERT(size.cx > 0);
	ASSERT(size.cy > 0);

	// Alloc
	unsigned char *pixel = new unsigned char[3*size.cx*size.cy];
	ASSERT(pixel != NULL);

	// Capture frame buffer
	TRACE("Start reading client...\n");
	TRACE("Client : (%d,%d)\n",size.cx,size.cy);

	CRect ClientRect,MainRect;
	this->GetWindowRect(&ClientRect);
	CWnd *pMain = AfxGetApp()->m_pMainWnd;
	CWindowDC dc(pMain);
	pMain->GetWindowRect(&MainRect);
	int xOffset = ClientRect.left - MainRect.left;
	int yOffset = ClientRect.top - MainRect.top;
	for(int j=0;j<size.cy;j++)
	{
		for(int i=0;i<size.cx;i++)
		{
			COLORREF color = dc.GetPixel(i+xOffset,j+yOffset);
			pixel[3*(size.cx*(size.cy-1-j)+i)] = (BYTE)GetBValue(color);
			pixel[3*(size.cx*(size.cy-1-j)+i)+1] = (BYTE)GetGValue(color);
			pixel[3*(size.cx*(size.cy-1-j)+i)+2] = (BYTE)GetRValue(color);
		}
	}
	EndWaitCursor();
	return pixel;
}