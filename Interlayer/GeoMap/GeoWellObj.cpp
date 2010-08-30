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
	SetObjName(_T("����"));
}

CGeoWellObj::CGeoWellObj(CDrawLayer *pLayer)
:CGeoDrawObj(pLayer)
{
	m_strWellName	= _T("Well");		// ����
	m_strWellType	= _T("11");			// ������� 
	m_strWellStyle	= _T("11");			// ���ʹ���
	m_ptWellName	= CPoint(0,0);		// ��������λ��
	m_iSymbolSize	= 20;			// 1mm;
	m_ptWellName.x	= 0;				// ��������λ��
	m_ptWellName.y  = m_iSymbolSize/2 + 5 + 13;
	
	// ��������
	memset(&m_lfFont, 0, sizeof(LOGFONT) );
	m_lfFont.lfHeight = -25;
	m_lfFont.lfCharSet = GB2312_CHARSET;
	strcpy_s(m_lfFont.lfFaceName, _T("����"));
	
	m_clrFont = RGB(0,0,0);				// ������ɫ
	m_clrSymbolPen = RGB(0,0,0);
	m_clrSymbolFill = RGB(255,255,255);
	m_dElevation = 0;

	SetObjName(_T("����"));
}

CGeoWellObj::CGeoWellObj(const CPoint &pt, CDrawLayer *pLayer, LPCSTR strWellName)
:CGeoDrawObj(pLayer)
{
	m_ptSymbolWell		= pt;
	// ת��Ϊ�������
	m_ptSymbolCoord		= CoordinateTransfer(m_ptSymbolWell);
	if( strWellName )
		m_strWellName = strWellName;
	else
		m_strWellName	= _T("Well");		// ����
	m_strWellType	= _T("11");			// ������� 
	m_strWellStyle	= _T("11");			// ���ʹ���
	m_iSymbolSize	= 20;				// 1mm;
	m_ptWellName.x	= 0;				// ��������λ��
	m_ptWellName.y  = m_iSymbolSize/2 + 5 + 13;
	
	// ��������
	memset(&m_lfFont, 0, sizeof(LOGFONT) );
	m_lfFont.lfHeight = -25;
	m_lfFont.lfCharSet = ANSI_CHARSET;
	strcpy_s(m_lfFont.lfFaceName, _T("Arial"));

	m_clrFont = RGB(0,0,0);				// ������ɫ
	m_clrSymbolPen = RGB(0,0,0);
	m_clrSymbolFill = RGB(255,255,255);
	m_dElevation = 0;

	RecalcBounds(NULL);

	CPoint centerPoint = m_rtPosition.CenterPoint();
	m_geoCenterPoint = CoordinateTransfer(centerPoint);
	SetObjName(_T("����"));
}

CGeoWellObj::CGeoWellObj(const CPoint2D &pt, CDrawLayer *pLayer, LPCSTR strWellName)
:CGeoDrawObj(pLayer)
{
	m_ptSymbolCoord = pt;
	// ת��Ϊͼ������
	m_ptSymbolWell = CoordinateTransfer(pt);

	if( strWellName )
		m_strWellName = strWellName;
	else
		m_strWellName	= _T("Well");		// ����
	
	m_strWellType	= _T("11");			// ������� 
	m_strWellStyle	= _T("11");			// ���ʹ���
	m_ptWellName	= CPoint(0,20);		// ��������λ��
	m_iSymbolSize	= 20;				// 1mm;
	m_ptWellName.x	= 0;				// ��������λ��
	m_ptWellName.y  = m_iSymbolSize/2 + 5 + 13;
	
	// ��������
	memset(&m_lfFont, 0, sizeof(LOGFONT) );
	m_lfFont.lfHeight = -25;
	m_lfFont.lfCharSet = ANSI_CHARSET;
	strcpy_s(m_lfFont.lfFaceName, _T("Arial"));
	
	m_clrFont = RGB(0,0,0);				// ������ɫ
	m_clrSymbolPen = RGB(0,0,0);
	m_clrSymbolFill = RGB(255,255,255);
	m_dElevation = 0;
	
	RecalcBounds(NULL);

	CPoint centerPoint = m_rtPosition.CenterPoint();
	m_geoCenterPoint = CoordinateTransfer(centerPoint);
	SetObjName(_T("����"));
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
 		if( !Intersects(cliRect) ) //�����ͼ�ز��ڵ�ǰ����������ʾ
 			return ;
 	}

	CGeoDrawObj::Draw(pDC, pView);
	
	int nSaved = pDC->SaveDC();
	pDC->SetBkMode(TRANSPARENT);          //�趨��ʾ��ʽ

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
	
	// ת��Ϊ�������
	m_geoCenterPoint = CoordinateTransfer(pt);
	m_geoPosition = CoordinateTransfer(m_rtPosition);

	m_ptSymbolCoord = CoordinateTransfer(m_ptSymbolWell);
	
	m_pDocument->SetModifiedFlag();
	
	RecalcBounds(pView);
}
////////////////////////////////////////////////////////////////////////////////
//
// ����  : CGeoDrawObj::MoveHandleTo
// ����  : void
// ����  : int nHandle �ֱ�����
// ����  : CPoint point Ŀǰ�߼�λ��
// ����  : CGeoMapView *pView
// ˵��  : �ƶ�ĳ�ֱ�
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
// ����  : CGeoDrawObj::OnEditProperties
// ����  : void
// ����  : CGeoMapView *pView
// ˵��  : �༭ͼ������
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
			if( dlg.m_iApply == 0 ) // ֻӦ�õ�ǰ��
			{
				CDrawLayer* pLayer = this->m_pParentLayer;
				POSITION position = pLayer->m_DrawObjects.GetHeadPosition();
				while(position!=NULL)
				{
					CGeoDrawObj *pObj = pLayer->m_DrawObjects.GetNext(position);
					if(pObj->IsKindOf(RUNTIME_CLASS(CGeoWellObj)) && pObj != this )
					{
						CGeoWellObj*pWell = (CGeoWellObj*)pObj;
						
						if( dlg.m_iApplyWell== 0  // Ӧ��ͬ����
							&& pWell->m_strWellType == m_strWellType )
						{		
							pWell->Invalidate(pView);
							pWell->CopyAttribute(this);
							pWell->RecalcBounds(pView);
							pWell->Invalidate(pView);
						}
						else if( dlg.m_iApplyWell== 1 // Ӧ��ͬ���͵ľ�
							&& pWell->m_strWellStyle == m_strWellStyle )
						{
							pWell->Invalidate(pView);
							pWell->CopyAttribute(this);
							pWell->RecalcBounds(pView);
							pWell->Invalidate(pView);
						}
						else if( dlg.m_iApplyWell== 2 ) // Ӧ�����о�
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
			if( dlg.m_iApply == 1 ) // ֻӦ�õ�ǰҳ
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
							
							if( dlg.m_iApplyWell== 0  // Ӧ��ͬ����
								&& pWell->m_strWellType == m_strWellType )
							{		
								pWell->Invalidate(pView);
								pWell->CopyAttribute(this);
								pWell->RecalcBounds(pView);
								pWell->Invalidate(pView);
							}
							else if( dlg.m_iApplyWell== 1 // Ӧ��ͬ���͵ľ�
								&& pWell->m_strWellStyle == m_strWellStyle )
							{
								pWell->Invalidate(pView);
								pWell->CopyAttribute(this);
								pWell->RecalcBounds(pView);
								pWell->Invalidate(pView);
							}
							else if( dlg.m_iApplyWell== 2 ) // Ӧ�����о�
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
// ����  : CGeoDrawObj::Clone
// ����  : CGeoDrawObj*
// ����  : CGeoMapDoc *pDoc
// ˵��  : ��¡ͼ�ز���ͼ�ؼ��뵽�ĵ���
//
CGeoDrawObj* CGeoWellObj::Clone(CDrawLayer *pLayer)
{
	ASSERT_VALID(this);
	
	CGeoWellObj* pClone = new CGeoWellObj(pLayer);
	pClone->m_rtPosition	=	m_rtPosition;
	pClone->m_bLocked		=	m_bLocked;			// ����ͼ��
	pClone->m_bShow			=	m_bShow;			// �Ƿ���ʾ
	pClone->m_strID			=	newGUID();			// �µ�ID
	pClone->m_ptAnchors.Copy(m_ptAnchors);			// ê��

	pClone->m_pDocument		=	m_pDocument;		// �ĵ�

	//////////////////////////////////////////////////////////////////////////
	pClone->m_ptSymbolWell		= m_ptSymbolWell;
	pClone->m_ptSymbolCoord		= m_ptSymbolCoord;
	pClone->m_dElevation	= m_dElevation;
	
	pClone->m_strWellName	= m_strWellName;		// ����
	pClone->m_strWellType	= m_strWellType;			// ������� 
	pClone->m_strWellStyle	= m_strWellStyle;			// ���ʹ���
	pClone->m_ptWellName	= m_ptWellName;		// ��������λ��(���)
	pClone->m_iSymbolSize		= m_iSymbolSize;				// 1mm;
	
	// ��������
	memcpy(&pClone->m_lfFont, &m_lfFont, sizeof(LOGFONT) );	
	pClone->m_clrFont = m_clrFont;				// ������ɫ
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
// ����  : CGeoDrawObj::Serialize
// ����  : void
// ����  : CArchive &ar
// ˵��  : ���л�ͼ��
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
		
		ar << m_strWellName;			// ����
		ar << m_strWellType;			// ������� 
		ar << m_strWellStyle;			// ���ʹ���
		ar << m_ptWellName;		// ��������λ��(���)
		ar << m_iSymbolSize;				// 1mm;
		
		// ��������
		ar.Write(&m_lfFont, sizeof(LOGFONT) );	
		ar << m_clrFont;				// ������ɫ	
		ar << m_clrSymbolPen;
		ar << m_clrSymbolFill;
	}
	else
	{
		// get the document back pointer from the archive

		int nVersion = ar.GetObjectSchema(); // ��ȡ�汾��Ϣ
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
				
				ar >> m_strWellName;		// ����
				ar >> m_strWellType;			// ������� 
				ar >> m_strWellStyle;			// ���ʹ���
				ar >> m_ptWellName;		// ��������λ��(���)
				ar >> m_iSymbolSize;				// 1mm;
				
				// ��������
				ar.Read(&m_lfFont, sizeof(LOGFONT) );	
				ar >> m_clrFont;				// ������ɫ
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
// ����  : CGeoDrawObj::GetHandleCount
// ����  : int
// ˵��  : ��ȡ�ֱ�����
//
int CGeoWellObj::GetHandleCount()
{
	ASSERT_VALID(this);
	return 1;
}


////////////////////////////////////////////////////////////////////////////////
//
// ����  : CGeoDrawObj::GetHandle
// ����  : CPoint
// ����  : int nHandle
// ˵��  : ��ȡĳ�ֱ��������߼�����
//
CPoint CGeoWellObj::GetHandle(int nHandle)
{
	CPoint pt = m_ptSymbolWell + m_ptWellName;
	
	return pt;
}


////////////////////////////////////////////////////////////////////////////////
//
// ����  : CGeoDrawObj::GetHandleCursor
// ����  : HCURSOR
// ����  : int nHandle
// ˵��  : ��ȡĳ�ֱ������ָ��
//
HCURSOR CGeoWellObj::GetHandleCursor(int nHandle)
{	
	return AfxGetApp()->LoadCursor(IDC_CURSOR_MOVE_PT);
}

////////////////////////////////////////////////////////////////////////////////
//
// ����  : CGeoDrawObj::HitTestHanle
// ����  : int
// ����  : CPoint point���߼�����
// ����  : CGeoMapView *pView
// ����  : BOOL bSelected ����Ѿ�ѡ�У������Ƿ����ĳ���ֱ�
// ˵��  : �����������1ѡ�и�ͼ�أ�����û�е��и�ͼ��
//
int CGeoWellObj::HitTestHandle(CPoint point, CGeoMapView *pView, BOOL bSelected)
{
	return CGeoDrawObj::HitTestHandle(point, pView, bSelected);
}

// �������߷����ı�ʱ
void CGeoWellObj::OnScaleChanged()
{
	// ת��Ϊ��Ļ����
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

// ��������תʱ
void CGeoWellObj::OnAngleChanged()
{

}

// ����������ԭ���иı�ʱ
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
	m_ptWellName = pSrcWell->m_ptWellName;	// ��������λ��(���)
}

