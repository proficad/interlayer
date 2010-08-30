// GeoWellObj.cpp: implementation of the CGeoWellObj class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GeoWellObj.h"
#include "DrawLayer.h"
#include "GeoMapDoc.h"
#include "GeoMapView.h"
#include "../Resource.h"
#include <float.h>
#include "CmdGeoDrawObj.h"
#include "DlgWellSet.h"
#include <typeinfo>

#pragma warning(disable:4244)

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


extern void StatusBarMessage(char* fmt, ...);
extern void StatusProgress(int nIndex, int nRange=-1);
extern void StatusSetProgress(int nIndex, int nPos);


IMPLEMENT_SERIAL(CGeoWellObj, CGeoDrawObj, VERSIONABLE_SCHEMA|0)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeoWellObj::CGeoWellObj()
{
	SetObjName(_T("单井"));
}

CGeoWellObj::CGeoWellObj(CDrawLayer *pLayer)
:CGeoDrawObj(pLayer)
{
	m_strWellName	= _T("Well");		// 井号
	m_strWellType	= _T("11");			// 井别代码 
	m_strWellStyle	= _T("11");			// 井型代码
	m_ptWellName	= CPoint(0,0);		// 井名绘制位置
	m_iSymbolSize	= 20;			// 1mm;
	m_ptWellName.x	= 0;				// 井名绘制位置
	m_ptWellName.y  = m_iSymbolSize/2 + 5 + 13;
	
	// 井号字体
	memset(&m_lfFont, 0, sizeof(LOGFONT) );
	m_lfFont.lfHeight = -25;
	m_lfFont.lfCharSet = GB2312_CHARSET;
	strcpy_s(m_lfFont.lfFaceName, _T("宋体"));
	
	m_clrFont = RGB(0,0,0);				// 字体颜色
	m_clrSymbolPen = RGB(0,0,0);
	m_clrSymbolFill = RGB(255,255,255);
	m_dElevation = 0;

	SetObjName(_T("单井"));
}

CGeoWellObj::CGeoWellObj(const CPoint &pt, CDrawLayer *pLayer, LPCSTR strWellName)
:CGeoDrawObj(pLayer)
{
	m_ptSymbolWell		= pt;
	// 转换为大地坐标
	m_ptSymbolCoord		= CoordinateTransfer(m_ptSymbolWell);
	if( strWellName )
		m_strWellName = strWellName;
	else
		m_strWellName	= _T("Well");		// 井号
	m_strWellType	= _T("11");			// 井别代码 
	m_strWellStyle	= _T("11");			// 井型代码
	m_iSymbolSize	= 20;				// 1mm;
	m_ptWellName.x	= 0;				// 井名绘制位置
	m_ptWellName.y  = m_iSymbolSize/2 + 5 + 13;
	
	// 井号字体
	memset(&m_lfFont, 0, sizeof(LOGFONT) );
	m_lfFont.lfHeight = -25;
	m_lfFont.lfCharSet = ANSI_CHARSET;
	strcpy_s(m_lfFont.lfFaceName, _T("Arial"));

	m_clrFont = RGB(0,0,0);				// 字体颜色
	m_clrSymbolPen = RGB(0,0,0);
	m_clrSymbolFill = RGB(255,255,255);
	m_dElevation = 0;

	RecalcBounds(NULL);

	CPoint centerPoint = m_rtPosition.CenterPoint();
	m_geoCenterPoint = CoordinateTransfer(centerPoint);
	SetObjName(_T("单井"));
}

CGeoWellObj::CGeoWellObj(const CPoint2D &pt, CDrawLayer *pLayer, LPCSTR strWellName)
:CGeoDrawObj(pLayer)
{
	m_ptSymbolCoord = pt;
	// 转换为图形坐标
	m_ptSymbolWell = CoordinateTransfer(pt);

	if( strWellName )
		m_strWellName = strWellName;
	else
		m_strWellName	= _T("Well");		// 井号
	
	m_strWellType	= _T("11");			// 井别代码 
	m_strWellStyle	= _T("11");			// 井型代码
	m_ptWellName	= CPoint(0,20);		// 井名绘制位置
	m_iSymbolSize	= 20;				// 1mm;
	m_ptWellName.x	= 0;				// 井名绘制位置
	m_ptWellName.y  = m_iSymbolSize/2 + 5 + 13;
	
	// 井号字体
	memset(&m_lfFont, 0, sizeof(LOGFONT) );
	m_lfFont.lfHeight = -25;
	m_lfFont.lfCharSet = ANSI_CHARSET;
	strcpy_s(m_lfFont.lfFaceName, _T("Arial"));
	
	m_clrFont = RGB(0,0,0);				// 字体颜色
	m_clrSymbolPen = RGB(0,0,0);
	m_clrSymbolFill = RGB(255,255,255);
	m_dElevation = 0;
	
	RecalcBounds(NULL);

	CPoint centerPoint = m_rtPosition.CenterPoint();
	m_geoCenterPoint = CoordinateTransfer(centerPoint);
	SetObjName(_T("单井"));
}

CGeoWellObj::~CGeoWellObj()
{
}

void CGeoWellObj::Draw(CDC *pDC, CGeoMapView *pView)
{
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

	CGeoDrawObj::Draw(pDC, pView);
	
	int nSaved = pDC->SaveDC();
	pDC->SetBkMode(TRANSPARENT);          //设定显示方式

	pDC->SetTextColor(m_clrFont);
	CFont font;
	font.CreateFontIndirect(&m_lfFont);

// 	CBrush brush1(RGB(255, 0,0));
// 	CBrush brush2(RGB(0,255,0));
// 
// 	CBrush *pOldBrush;
// 	if( !m_strWellType.IsEmpty() )
// 	{
// 		if( m_strWellType[0]=='1' )
// 			pOldBrush = pDC->SelectObject(&brush1);
// 		else
// 			pOldBrush = pDC->SelectObject(&brush2);
// 	}

	CBrush *pOldBrush = (CBrush *)pDC->SelectStockObject(NULL_BRUSH);
	CPen *pOldPen;
	CPen pen(PS_SOLID,0,RGB(0,0,0));
	CPen pen1(PS_SOLID,0,RGB(255,0,0));
	CPen pen2(PS_SOLID,0,RGB(0,255,0));
	if( !m_strWellType.IsEmpty() )
	{
		if( m_strWellType[0]=='1' )
			pOldPen = pDC->SelectObject(&pen1);
		else
			pOldPen = pDC->SelectObject(&pen2);
	}
	else
		pOldPen = pDC->SelectObject(&pen);

	CFont *pOldFont = pDC->SelectObject(&font);

	CSize size(m_iSymbolSize/2,m_iSymbolSize/2), size1;
	pDC->LPtoDP(&size);
	size1 = size;
	size.cx +=1;
	size.cy +=1;

	pDC->DPtoLP(&size);
	pDC->DPtoLP(&size1);
	
	pDC->Ellipse(CRect(m_ptSymbolWell.x-size.cx, m_ptSymbolWell.y - size.cy,
		m_ptSymbolWell.x + size1.cx, m_ptSymbolWell.y + size1.cy));

	CPoint pt = m_ptSymbolWell+m_ptWellName;
	CRect rt(pt,CSize(1,1));
	pDC->DrawText(m_strWellName, rt, DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_NOCLIP);
	
	pDC->SelectObject(pOldFont);

	font.DeleteObject();

// 	if( !m_strWellType.IsEmpty() )
// 	{
// 		pDC->SelectObject(pOldBrush);
// 	}

	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);

	pDC->RestoreDC(nSaved);
}

void CGeoWellObj::DrawEditMode(CGeoMapView* pView)
{
	CGeoDrawObj::DrawEditMode(pView);
}

void CGeoWellObj::DrawMoveState(CGeoMapView* pView)
{
	CGeoDrawObj::DrawMoveState(pView);
}

void CGeoWellObj::DrawSizeState(CGeoMapView* pView)
{
	CGeoDrawObj::DrawSizeState(pView);
	
	// 	CClientDC dc(pView);
	// 	pView->OnPrepareDC(&dc);
	// 	int mode = dc.SetROP2(R2_XORPEN);
	// 	CRect rect = m_rtPosition;
	// 	CPen pen;
	// 	pen.CreatePen(PS_SOLID, 0, RGB(255, 0, 0));
	// 	CPen *pOldPen = dc.SelectObject(&pen);
	// 	CBrush *pOldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	// 	dc.SetROP2(R2_XORPEN);
	// 	
	// 	dc.Rectangle(rect);
	// 	
	// 	dc.SelectObject(pOldBrush);
	// 	dc.SelectObject(pOldPen);	
// 	dc.SetROP2(mode);	
}

void CGeoWellObj::MoveTo(const CRect &position, CGeoMapView *pView)
{
	ASSERT_VALID(this);
	
	if( m_bLocked )
		return;
	
	if (position == m_rtPosition)
		return;
	
	CPoint pt = m_rtPosition.CenterPoint();
	CPoint delta = position.CenterPoint();
	delta -= pt;
	
	if (pView == NULL)
	{
		Invalidate();
		m_rtPosition = position;
		m_ptSymbolWell += delta;
		Invalidate();
	}
	else
	{
		if( m_bEditing )
			DrawEditMode( pView );
		else if( m_bSizeChanging )
			DrawSizeState( pView );
		else if( m_bMoving )
			DrawMoveState( pView );
		else
			pView->InvalObj(this);
		
		m_rtPosition = position;		
		m_ptSymbolWell += delta;
		if( m_bEditing )
			DrawEditMode( pView );
		else if( m_bSizeChanging )
			DrawSizeState( pView );
		else if( m_bMoving )
			DrawMoveState( pView );
		else
			pView->InvalObj(this);
	}
	
	pt = m_rtPosition.CenterPoint();
	
	// 转换为大地坐标
	m_geoCenterPoint = CoordinateTransfer(pt);
	m_geoPosition = CoordinateTransfer(m_rtPosition);

	m_ptSymbolCoord = CoordinateTransfer(m_ptSymbolWell);
	
	m_pDocument->SetModifiedFlag();
	
	RecalcBounds(pView);
}
////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::MoveHandleTo
// 返回  : void
// 参数  : int nHandle 手柄索引
// 参数  : CPoint point 目前逻辑位置
// 参数  : CGeoMapView *pView
// 说明  : 移动某手柄
//
void CGeoWellObj::MoveHandleTo(int nHandle, CPoint point, CGeoMapView *pView)
{
	ASSERT_VALID(this);
	
	if( m_bLocked )
		return;

	CPoint pt = m_ptSymbolWell + m_ptWellName;
	if ( point == pt )
	{
		return;
	}
	m_ptWellName = point-m_ptSymbolWell;

	Invalidate(pView);

	RecalcBounds(pView);

	Invalidate(pView);

	m_pDocument->SetModifiedFlag();
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::OnEditProperties
// 返回  : void
// 参数  : CGeoMapView *pView
// 说明  : 编辑图素属性
//
BOOL CGeoWellObj::OnEditProperties(CGeoMapView *pView)
{
	ASSERT_VALID(this);
	if( m_bLocked )
		return FALSE;
	
	CString strJBDM[19] = {
		_T("11"),_T("21"),_T("31"),_T("41"),_T("51"),
		_T("61"),_T("71"),_T("81"),_T("A1"),_T("90"),
		_T("B0"),_T("C0"),_T("X1"),_T("DZ"),_T("QT"),
		_T("YT"),_T("PJ"),_T("GC"),_T("SW")
	};

	CString strJXDM[11] = {
		_T("11"),_T("12"),_T("21"),_T("22"),_T("23"),
		_T("24"),_T("25"),_T("26"),_T("27"),_T("28"),
		_T("90")
	};

	CDlgWellSet dlg;
	dlg.m_pWellObj = this;
	dlg.m_clrFill = m_clrSymbolFill;
	dlg.m_clrPen = m_clrSymbolPen;
	dlg.m_clrFont = m_clrFont;
	memcpy(&dlg.m_lfFont, &m_lfFont, sizeof(LOGFONT) );
	dlg.m_nSymbolSize = m_iSymbolSize;

	dlg.m_strWellName = m_strWellName;
	dlg.m_dBXHB = m_dElevation;
	dlg.m_dZZBX = m_ptSymbolCoord.y;
	dlg.m_dHZBY = m_ptSymbolCoord.x;

	int i;
	for( i=0;i<19;i++)
	{
		if( m_strWellType == strJBDM[i])
		{
			dlg.m_iWellType = i;
			break;
		}
	}

	for( i=0;i<19;i++)
	{
		if( m_strWellStyle == strJXDM[i])
		{
			dlg.m_iWellStyle = i;
			break;
		}
	}
	
	if( dlg.DoModal() == IDOK )
	{
		Invalidate(pView);
		m_clrSymbolFill = dlg.m_clrFill;
		m_clrSymbolPen = dlg.m_clrPen;
		m_clrFont = dlg.m_clrFont;
		memcpy(&m_lfFont, &dlg.m_lfFont, sizeof(LOGFONT) );
		m_iSymbolSize = dlg.m_nSymbolSize;

		m_dElevation = dlg.m_dBXHB;
		if( m_ptSymbolCoord.x != dlg.m_dHZBY
			|| m_ptSymbolCoord.y != dlg.m_dZZBX)
		{
			m_ptSymbolCoord.x = dlg.m_dHZBY;
			m_ptSymbolCoord.y = dlg.m_dZZBX;
			m_ptSymbolWell = CoordinateTransfer(m_ptSymbolCoord);
		}
		
		if( dlg.m_iWellType!=-1)
			m_strWellType = strJBDM[dlg.m_iWellType];
		if( dlg.m_iWellStyle!=-1)
			m_strWellStyle = strJXDM[dlg.m_iWellStyle];

		m_strWellName = dlg.m_strWellName;

		RecalcBounds(pView);
		Invalidate(pView);
		m_pDocument->SetModifiedFlag();

		if( dlg.m_iApplyWell<3 )
		{
			if( dlg.m_iApply == 0 ) // 只应用当前层
			{
				CDrawLayer* pLayer = this->m_pParentLayer;
				POSITION position = pLayer->m_DrawObjects.GetHeadPosition();
				while(position!=NULL)
				{
					CGeoDrawObj *pObj = pLayer->m_DrawObjects.GetNext(position);
					if(pObj->IsKindOf(RUNTIME_CLASS(CGeoWellObj)) && pObj != this )
					{
						CGeoWellObj*pWell = (CGeoWellObj*)pObj;
						
						if( dlg.m_iApplyWell== 0  // 应用同类型
							&& pWell->m_strWellType == m_strWellType )
						{		
							pWell->Invalidate(pView);
							pWell->CopyAttribute(this);
							pWell->RecalcBounds(pView);
							pWell->Invalidate(pView);
						}
						else if( dlg.m_iApplyWell== 1 // 应用同井型的井
							&& pWell->m_strWellStyle == m_strWellStyle )
						{
							pWell->Invalidate(pView);
							pWell->CopyAttribute(this);
							pWell->RecalcBounds(pView);
							pWell->Invalidate(pView);
						}
						else if( dlg.m_iApplyWell== 2 ) // 应用所有井
						{
							pWell->Invalidate(pView);
							pWell->CopyAttribute(this);
							pWell->RecalcBounds(pView);
							pWell->Invalidate(pView);
						}
					}
				}

			}

			//////////////////////////////////////////////////////////////////////////
			if( dlg.m_iApply == 1 ) // 只应用当前页
			{
				CDrawPage *pPage = m_pParentLayer->GetParentPage();
				POSITION pos = pPage->m_DrawLayers.GetHeadPosition();
				while (pos != NULL)
				{
					CDrawLayer* pLayer = pPage->m_DrawLayers.GetNext(pos);
					POSITION position = pLayer->m_DrawObjects.GetHeadPosition();
					while(position!=NULL)
					{
						CGeoDrawObj *pObj = pLayer->m_DrawObjects.GetNext(position);
						if(pObj->IsKindOf(RUNTIME_CLASS(CGeoWellObj)) && pObj != this )
						{
							CGeoWellObj*pWell = (CGeoWellObj*)pObj;
							
							if( dlg.m_iApplyWell== 0  // 应用同类型
								&& pWell->m_strWellType == m_strWellType )
							{		
								pWell->Invalidate(pView);
								pWell->CopyAttribute(this);
								pWell->RecalcBounds(pView);
								pWell->Invalidate(pView);
							}
							else if( dlg.m_iApplyWell== 1 // 应用同井型的井
								&& pWell->m_strWellStyle == m_strWellStyle )
							{
								pWell->Invalidate(pView);
								pWell->CopyAttribute(this);
								pWell->RecalcBounds(pView);
								pWell->Invalidate(pView);
							}
							else if( dlg.m_iApplyWell== 2 ) // 应用所有井
							{
								pWell->Invalidate(pView);
								pWell->CopyAttribute(this);
								pWell->RecalcBounds(pView);
								pWell->Invalidate(pView);
							}
						} // if
					} // while
				}// while
			}// if			
		}
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
CGeoDrawObj* CGeoWellObj::Clone(CDrawLayer *pLayer)
{
	ASSERT_VALID(this);
	
	CGeoWellObj* pClone = new CGeoWellObj(pLayer);
	pClone->m_rtPosition	=	m_rtPosition;
	pClone->m_bLocked		=	m_bLocked;			// 锁定图素
	pClone->m_bShow			=	m_bShow;			// 是否显示
	pClone->m_strID			=	newGUID();			// 新的ID
	pClone->m_ptAnchors.Copy(m_ptAnchors);			// 锚点

	pClone->m_pDocument		=	m_pDocument;		// 文档

	//////////////////////////////////////////////////////////////////////////
	pClone->m_ptSymbolWell		= m_ptSymbolWell;
	pClone->m_ptSymbolCoord		= m_ptSymbolCoord;
	pClone->m_dElevation	= m_dElevation;
	
	pClone->m_strWellName	= m_strWellName;		// 井号
	pClone->m_strWellType	= m_strWellType;			// 井别代码 
	pClone->m_strWellStyle	= m_strWellStyle;			// 井型代码
	pClone->m_ptWellName	= m_ptWellName;		// 井名绘制位置(相对)
	pClone->m_iSymbolSize		= m_iSymbolSize;				// 1mm;
	
	// 井号字体
	memcpy(&pClone->m_lfFont, &m_lfFont, sizeof(LOGFONT) );	
	pClone->m_clrFont = m_clrFont;				// 字体颜色
	pClone->m_clrSymbolPen = m_clrSymbolPen;
	pClone->m_clrSymbolFill = m_clrSymbolFill;

	ASSERT_VALID(pClone);
	
	if (pClone != NULL)
	{
		m_pDocument->Do(new CCmdAddDrawObj(m_pDocument, pClone), TRUE);
		//pLayer->Add(pClone);
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
void CGeoWellObj::Serialize(CArchive &ar)
{
	CGeoDrawObj::Serialize(ar);
	
	ar.SerializeClass(RUNTIME_CLASS(CGeoWellObj));

	if (ar.IsStoring())
	{
		ar << m_ptSymbolWell;
		ar << m_ptSymbolCoord;
		ar << m_dElevation;
		
		ar << m_strWellName;			// 井号
		ar << m_strWellType;			// 井别代码 
		ar << m_strWellStyle;			// 井型代码
		ar << m_ptWellName;		// 井名绘制位置(相对)
		ar << m_iSymbolSize;				// 1mm;
		
		// 井号字体
		ar.Write(&m_lfFont, sizeof(LOGFONT) );	
		ar << m_clrFont;				// 字体颜色	
		ar << m_clrSymbolPen;
		ar << m_clrSymbolFill;
	}
	else
	{
		// get the document back pointer from the archive

		int nVersion = ar.GetObjectSchema(); // 获取版本信息
		switch( nVersion )
		{
		case 0:
			{
				m_pDocument = (CGeoMapDoc*)ar.m_pDocument;
				ASSERT_VALID(m_pDocument);
				ASSERT_KINDOF(CGeoMapDoc, m_pDocument);
				
				ar >> m_ptSymbolWell;
				ar >> m_ptSymbolCoord;
				ar >> m_dElevation;
				
				ar >> m_strWellName;		// 井号
				ar >> m_strWellType;			// 井别代码 
				ar >> m_strWellStyle;			// 井型代码
				ar >> m_ptWellName;		// 井名绘制位置(相对)
				ar >> m_iSymbolSize;				// 1mm;
				
				// 井号字体
				ar.Read(&m_lfFont, sizeof(LOGFONT) );	
				ar >> m_clrFont;				// 字体颜色
				ar >> m_clrSymbolPen;
				ar >> m_clrSymbolFill;
			}
			break;
		default:
			break;
		}		
	}
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::GetHandleCount
// 返回  : int
// 说明  : 获取手柄总数
//
int CGeoWellObj::GetHandleCount()
{
	ASSERT_VALID(this);
	return 1;
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::GetHandle
// 返回  : CPoint
// 参数  : int nHandle
// 说明  : 获取某手柄的中心逻辑坐标
//
CPoint CGeoWellObj::GetHandle(int nHandle)
{
	CPoint pt = m_ptSymbolWell + m_ptWellName;
	
	return pt;
}


////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::GetHandleCursor
// 返回  : HCURSOR
// 参数  : int nHandle
// 说明  : 获取某手柄的鼠标指针
//
HCURSOR CGeoWellObj::GetHandleCursor(int nHandle)
{	
	return AfxGetApp()->LoadCursor(IDC_CURSOR_MOVE_PT);
}

////////////////////////////////////////////////////////////////////////////////
//
// 函数  : CGeoDrawObj::HitTestHanle
// 返回  : int
// 参数  : CPoint point是逻辑坐标
// 参数  : CGeoMapView *pView
// 参数  : BOOL bSelected 如果已经选中，返回是否点中某个手柄
// 说明  : 否则，如果返回1选中该图素，否则没有点中该图素
//
int CGeoWellObj::HitTestHandle(CPoint point, CGeoMapView *pView, BOOL bSelected)
{
	return CGeoDrawObj::HitTestHandle(point, pView, bSelected);
}

// 当比例尺发生改变时
void CGeoWellObj::OnScaleChanged()
{
	// 转换为屏幕坐标
	CPoint point = CoordinateTransfer(m_geoCenterPoint);
	
	int iWidth = m_rtPosition.Width();
	int iHeight = m_rtPosition.Height();
	
	m_rtPosition.SetRect(
		point.x - iWidth/2 - iWidth%2, 
		point.y - iHeight/2 - iHeight%2,
		point.x + iWidth/2,
		point.y + iHeight/2);

	m_ptSymbolWell = CoordinateTransfer(m_ptSymbolCoord);

	RecalcBounds(NULL);
}

// 当发生旋转时
void CGeoWellObj::OnAngleChanged()
{

}

// 当发生坐标原点有改变时
void CGeoWellObj::OnCoordOrigChanged()
{

}


BOOL CGeoWellObj::RecalcBounds(CGeoMapView* pView)
{
	CRect rect;
	CPoint pt1 = m_ptSymbolWell;
	CPoint pt2 = m_ptWellName+m_ptSymbolWell;
	int nWidth = m_strWellName.GetLength() * labs(m_lfFont.lfHeight)/2 + 20;	
	rect.left = min(pt1.x-m_iSymbolSize/2-10, pt2.x-nWidth/2);
	rect.right = max(pt1.x+m_iSymbolSize/2+10, pt2.x+nWidth/2);
	rect.top = min(pt1.y-m_iSymbolSize/2-10,pt2.y - labs(m_lfFont.lfHeight)/2-10);
	rect.bottom = max(pt1.y+m_iSymbolSize/2+10,pt2.y + labs(m_lfFont.lfHeight)/2+10);

	m_rtPosition = rect;

	return CGeoDrawObj::RecalcBounds(pView);
}


void CGeoWellObj::CopyAttribute(CGeoDrawObj *pSrcObj)
{
	CGeoDrawObj::CopyAttribute(pSrcObj);
	
	CGeoWellObj *pSrcWell = (CGeoWellObj*)pSrcObj;

	if( m_strWellType == pSrcWell->m_strWellType )
	{
		m_clrSymbolFill = pSrcWell->m_clrSymbolFill;
		m_clrSymbolPen = pSrcWell->m_clrSymbolPen;
	}
	
	m_clrFont = pSrcWell->m_clrFont;
	memcpy(&m_lfFont, &pSrcWell->m_lfFont, sizeof(LOGFONT) );
	m_iSymbolSize = pSrcWell->m_iSymbolSize;
	m_ptWellName = pSrcWell->m_ptWellName;	// 井名绘制位置(相对)
}

