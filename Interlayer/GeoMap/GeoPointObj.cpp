#include "StdAfx.h"
#include "GeoPointObj.h"
#include "DrawLayer.h"
#include "GeoMapDoc.h"
#include "GeoMapView.h"
#include "../Resource.h"
#include <float.h>
#include "CmdGeoDrawObj.h"
//#include "DlgPointSet.h"
#include <typeinfo>


IMPLEMENT_SERIAL(CGeoPointObj, CGeoDrawObj, VERSIONABLE_SCHEMA|0)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeoPointObj::CGeoPointObj()
{
	SetObjName(_T("��"));
}

CGeoPointObj::CGeoPointObj(CDrawLayer *pLayer)
:CGeoDrawObj(pLayer)
{
	m_strName	= _T("Point");
	m_ptName	= CPoint(0,0);
	m_iSymbolSize	= 20;			// 1mm;
	m_ptName.x	= 0;
	m_ptName.y  = m_iSymbolSize/2 + 5 + 13;

	memset(&m_lfFont, 0, sizeof(LOGFONT) );
	m_lfFont.lfHeight = -25;
	m_lfFont.lfCharSet = GB2312_CHARSET;
	strcpy_s(m_lfFont.lfFaceName, _T("����"));

	m_clrFont = RGB(0,0,0);				// ������ɫ
	m_clrSymbolPen = RGB(0,0,0);
	m_clrSymbolFill = RGB(255,255,255);

	SetObjName(_T("��"));
}

CGeoPointObj::CGeoPointObj(const CPoint &pt, CDrawLayer *pLayer, LPCSTR strName)
:CGeoDrawObj(pLayer)
{
	m_ptSymbolPoint		= pt;
	// ת��Ϊ�������
	m_ptSymbolCoord		= CoordinateTransfer(m_ptSymbolPoint);
	if( strName )
		m_strName = strName;
	else
		m_strName	= _T("Point");

	m_iSymbolSize	= 20;				// 1mm;
	m_ptName.x	= 0;
	m_ptName.y  = m_iSymbolSize/2 + 5 + 13;

	memset(&m_lfFont, 0, sizeof(LOGFONT) );
	m_lfFont.lfHeight = -25;
	m_lfFont.lfCharSet = ANSI_CHARSET;
	strcpy_s(m_lfFont.lfFaceName, _T("Arial"));

	m_clrFont = RGB(0,0,0);				// ������ɫ
	m_clrSymbolPen = RGB(0,0,0);
	m_clrSymbolFill = RGB(255,255,255);

	RecalcBounds(NULL);

	CPoint centerPoint = m_rtPosition.CenterPoint();
	m_geoCenterPoint = CoordinateTransfer(centerPoint);
	SetObjName(_T("��"));
}

CGeoPointObj::CGeoPointObj(const CPoint2D &pt, CDrawLayer *pLayer, LPCSTR strName)
:CGeoDrawObj(pLayer)
{
	m_ptSymbolCoord = pt;
	// ת��Ϊͼ������
	m_ptSymbolPoint = CoordinateTransfer(pt);

	if( strName )
		m_strName = strName;
	else
		m_strName = pt.szInfo;
	
	m_ptName	= CPoint(0,20);	
	m_iSymbolSize	= 20;				// 1mm;
	m_ptName.x	= 0;
	m_ptName.y  = m_iSymbolSize/2 + 5 + 13;

	memset(&m_lfFont, 0, sizeof(LOGFONT) );
	m_lfFont.lfHeight = -25;
	m_lfFont.lfCharSet = ANSI_CHARSET;
	strcpy_s(m_lfFont.lfFaceName, _T("Arial"));

	m_clrFont = RGB(0,0,0);				// ������ɫ
	m_clrSymbolPen = RGB(0,0,0);
	m_clrSymbolFill = RGB(255,255,255);

	RecalcBounds(NULL);

	CPoint centerPoint = m_rtPosition.CenterPoint();
	m_geoCenterPoint = CoordinateTransfer(centerPoint);
	SetObjName(_T("��"));
}

CGeoPointObj::~CGeoPointObj()
{
}

void CGeoPointObj::Draw(CDC *pDC, CGeoMapView *pView)
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

	CBrush brush(RGB(255, 235,200));

	CBrush *pOldBrush;
	pOldBrush = pDC->SelectObject(&brush);

	CFont *pOldFont = pDC->SelectObject(&font);

	CSize size(m_iSymbolSize/2,m_iSymbolSize/2), size1;
	pDC->LPtoDP(&size);
	size1 = size;
	size.cx +=1;
	size.cy +=1;

	pDC->DPtoLP(&size);
	pDC->DPtoLP(&size1);

	pDC->Ellipse(CRect(m_ptSymbolPoint.x-size.cx, m_ptSymbolPoint.y - size.cy,
		m_ptSymbolPoint.x + size1.cx, m_ptSymbolPoint.y + size1.cy));

	CPoint pt = m_ptSymbolPoint+m_ptName;
	CRect rt(pt,CSize(1,1));
	pDC->DrawText(m_strName, rt, DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_NOCLIP);

	pDC->SelectObject(pOldFont);

	font.DeleteObject();

	pDC->SelectObject(pOldBrush);

	pDC->RestoreDC(nSaved);
}

void CGeoPointObj::DrawEditMode(CGeoMapView* pView)
{
	CGeoDrawObj::DrawEditMode(pView);
}

void CGeoPointObj::DrawMoveState(CGeoMapView* pView)
{
	CGeoDrawObj::DrawMoveState(pView);
}

void CGeoPointObj::DrawSizeState(CGeoMapView* pView)
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

void CGeoPointObj::MoveTo(const CRect &position, CGeoMapView *pView)
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
		m_ptSymbolPoint += delta;
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
		m_ptSymbolPoint += delta;
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

	m_ptSymbolCoord = CoordinateTransfer(m_ptSymbolPoint);

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
void CGeoPointObj::MoveHandleTo(int nHandle, CPoint point, CGeoMapView *pView)
{
	ASSERT_VALID(this);

	if( m_bLocked )
		return;

	CPoint pt = m_ptSymbolPoint + m_ptName;
	if ( point == pt )
	{
		return;
	}
	m_ptName = point-m_ptSymbolPoint;

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
BOOL CGeoPointObj::OnEditProperties(CGeoMapView *pView)
{
	ASSERT_VALID(this);
	if( m_bLocked )
		return FALSE;

	return FALSE;
}


////////////////////////////////////////////////////////////////////////////////
//
// ����  : CGeoDrawObj::Clone
// ����  : CGeoDrawObj*
// ����  : CGeoMapDoc *pDoc
// ˵��  : ��¡ͼ�ز���ͼ�ؼ��뵽�ĵ���
//
CGeoDrawObj* CGeoPointObj::Clone(CDrawLayer *pLayer)
{
	ASSERT_VALID(this);

	CGeoPointObj* pClone = new CGeoPointObj(pLayer);
	pClone->m_rtPosition	=	m_rtPosition;
	pClone->m_bLocked		=	m_bLocked;			// ����ͼ��
	pClone->m_bShow			=	m_bShow;			// �Ƿ���ʾ
	pClone->m_strID			=	newGUID();			// �µ�ID
	pClone->m_ptAnchors.Copy(m_ptAnchors);			// ê��

	pClone->m_pDocument		=	m_pDocument;		// �ĵ�

	//////////////////////////////////////////////////////////////////////////
	pClone->m_ptSymbolPoint		= m_ptSymbolPoint;
	pClone->m_ptSymbolCoord		= m_ptSymbolCoord;

	pClone->m_strName	= m_strName;
	pClone->m_ptName	= m_ptName;	
	pClone->m_iSymbolSize		= m_iSymbolSize;				// 1mm;

	memcpy(&pClone->m_lfFont, &m_lfFont, sizeof(LOGFONT) );	
	pClone->m_clrFont = m_clrFont;				// ������ɫ
	pClone->m_clrSymbolPen = m_clrSymbolPen;
	pClone->m_clrSymbolFill = m_clrSymbolFill;

	ASSERT_VALID(pClone);

	if (pClone != NULL)
	{
		m_pDocument->Do(new CCmdAddDrawObj(m_pDocument, pClone), TRUE);
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
void CGeoPointObj::Serialize(CArchive &ar)
{
	CGeoDrawObj::Serialize(ar);

	ar.SerializeClass(RUNTIME_CLASS(CGeoPointObj));

	if (ar.IsStoring())
	{
		ar << m_ptSymbolPoint;
		ar << m_ptSymbolCoord;

		ar << m_strName;
		ar << m_ptName;	
		ar << m_iSymbolSize;				// 1mm;

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

				ar >> m_ptSymbolPoint;
				ar >> m_ptSymbolCoord;

				ar >> m_strName;
				ar >> m_ptName;	
				ar >> m_iSymbolSize;				// 1mm;
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
int CGeoPointObj::GetHandleCount()
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
CPoint CGeoPointObj::GetHandle(int nHandle)
{
	CPoint pt = m_ptSymbolPoint + m_ptName;

	return pt;
}


////////////////////////////////////////////////////////////////////////////////
//
// ����  : CGeoDrawObj::GetHandleCursor
// ����  : HCURSOR
// ����  : int nHandle
// ˵��  : ��ȡĳ�ֱ������ָ��
//
HCURSOR CGeoPointObj::GetHandleCursor(int nHandle)
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
int CGeoPointObj::HitTestHandle(CPoint point, CGeoMapView *pView, BOOL bSelected)
{
	return CGeoDrawObj::HitTestHandle(point, pView, bSelected);
}

// �������߷����ı�ʱ
void CGeoPointObj::OnScaleChanged()
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

	m_ptSymbolPoint = CoordinateTransfer(m_ptSymbolCoord);

	RecalcBounds(NULL);
}

// ��������תʱ
void CGeoPointObj::OnAngleChanged()
{

}

// ����������ԭ���иı�ʱ
void CGeoPointObj::OnCoordOrigChanged()
{

}


BOOL CGeoPointObj::RecalcBounds(CGeoMapView* pView)
{
	CRect rect;
	CPoint pt1 = m_ptSymbolPoint;
	CPoint pt2 = m_ptName+m_ptSymbolPoint;
	int nWidth = m_strName.GetLength() * labs(m_lfFont.lfHeight)/2 + 20;	
	rect.left = min(pt1.x-m_iSymbolSize/2-10, pt2.x-nWidth/2);
	rect.right = max(pt1.x+m_iSymbolSize/2+10, pt2.x+nWidth/2);
	rect.top = min(pt1.y-m_iSymbolSize/2-10,pt2.y - labs(m_lfFont.lfHeight)/2-10);
	rect.bottom = max(pt1.y+m_iSymbolSize/2+10,pt2.y + labs(m_lfFont.lfHeight)/2+10);

	m_rtPosition = rect;

	return CGeoDrawObj::RecalcBounds(pView);
}


void CGeoPointObj::CopyAttribute(CGeoDrawObj *pSrcObj)
{
	CGeoDrawObj::CopyAttribute(pSrcObj);

	CGeoPointObj *pSrcWell = (CGeoPointObj*)pSrcObj;

	m_clrSymbolFill = pSrcWell->m_clrSymbolFill;
	m_clrSymbolPen = pSrcWell->m_clrSymbolPen;

	m_clrFont = pSrcWell->m_clrFont;
	memcpy(&m_lfFont, &pSrcWell->m_lfFont, sizeof(LOGFONT) );
	m_iSymbolSize = pSrcWell->m_iSymbolSize;
	m_ptName = pSrcWell->m_ptName;
}

