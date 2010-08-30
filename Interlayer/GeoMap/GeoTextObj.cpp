#include "stdafx.h"
#include "GeoTextObj.h"

#include "GeoMapView.h"
#include "CmdGeoDrawObj.h"

#include "RgnClipping.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CGeoTextObj, CGeoDrawObj, VERSIONABLE_SCHEMA|0)

CGeoTextObj::CGeoTextObj(void)
{
	m_strText		= _T("�ı�");
	m_clrText		= RGB(0,0,0);			// ������ɫ
	m_nFormat		= DT_CENTER;			// ��ʽ

	memset(&m_lfFont, 0, sizeof(LOGFONT) );	// ����
	m_lfFont.lfHeight = 40;
	m_lfFont.lfCharSet = GB2312_CHARSET;
	strcpy_s(m_lfFont.lfFaceName, "����");
}

CGeoTextObj::CGeoTextObj(CDrawLayer* pLayer)
:CGeoDrawObj(pLayer)
{
	m_strText		= _T("�ı�");
	m_clrText		= RGB(0,0,0);			// ������ɫ
	m_nFormat		= DT_CENTER;			// ��ʽ

	memset(&m_lfFont, 0, sizeof(LOGFONT) );	// ����
	m_lfFont.lfHeight = 80;
	m_lfFont.lfCharSet = GB2312_CHARSET;
	strcpy_s(m_lfFont.lfFaceName, "����");
}

CGeoTextObj::CGeoTextObj(const CRect& position, CDrawLayer* pLayer)
:CGeoDrawObj(position,pLayer)
{
	m_strText		= _T("�ı�");
	m_clrText		= RGB(0,0,0);			// ������ɫ
	m_nFormat		= DT_CENTER;			// ��ʽ

	memset(&m_lfFont, 0, sizeof(LOGFONT) );	// ����
	m_lfFont.lfHeight = 80;
	m_lfFont.lfCharSet = GB2312_CHARSET;
	strcpy_s(m_lfFont.lfFaceName, "����");
}

CGeoTextObj::~CGeoTextObj(void)
{
}


void CGeoTextObj::Draw(CDC *pDC, CGeoMapView *pView)
{
	ASSERT_VALID(this);

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

	int nSaved = pDC->SaveDC();
	pDC->SetBkMode(TRANSPARENT);          //�趨��ʾ��ʽ

	// TODO:

	CRect rect = m_rtPosition;
	rect.NormalizeRect();

	pDC->IntersectClipRect(rect);

	CFont font;
	font.CreateFontIndirect(&m_lfFont);
	CFont *pOldFont = pDC->SelectObject(&font);

	pDC->SetTextColor(m_clrText);
	CPoint pt = rect.CenterPoint();
	CRect rc(0,0,1,1);
	pDC->DrawText(m_strText, rc, DT_CALCRECT);
	int w = rc.Width();
	int h = rc.Height();	
	pt.x -= w/2;
	pt.y -= h/2;

	rc.OffsetRect(pt);
	if( w < rect.Width() )
	{
		rc.left = rect.left;
		rc.right = rect.right;
	}

	pDC->DrawText(m_strText, rc, m_nFormat| DT_TOP | DT_WORDBREAK);
	pDC->SelectObject(pOldFont);

	pDC->RestoreDC(nSaved);
}


////////////////////////////////////////////////////////////////////////////////
//
// ����  : CGeoTextObj::OnEditProperties
// ����  : void
// ����  : CGeoMapView *pView
// ˵��  : �༭ͼ������
//
BOOL CGeoTextObj::OnEditProperties(CGeoMapView *pView)
{
	ASSERT_VALID(this);
	if( m_bLocked )
		return FALSE;

	m_bEditing = TRUE;
	Invalidate();

	pView->m_pDrawTextObj = this;

	CEditEx *pEdit;

	if(m_nFormat == DT_LEFT)
		pEdit = &(pView->m_LeftEdit);
	if(m_nFormat == DT_CENTER)
		pEdit = &(pView->m_CenterEdit);
	if(m_nFormat == DT_RIGHT)
		pEdit = &(pView->m_RightEdit);	

	CRect rect = m_rtPosition;
	pView->DocToClient(rect);
	rect.DeflateRect(4, 4, 4, 4);

	pEdit->MoveWindow(rect);

	LOGFONT font;

	CSize size;
	size.cx = m_lfFont.lfWidth;
	size.cy = m_lfFont.lfHeight;

	CClientDC dc(pView);
	pView->OnPrepareDC(&dc, NULL);
	dc.LPtoDP(&size);

	memcpy(&font, &m_lfFont, sizeof(LOGFONT));

	font.lfHeight = size.cy;
	font.lfWidth = size.cx;

	pEdit->setFont(&font);	
	pEdit->SetWindowText(m_strText);
	pEdit->SetSel(0, -1);

	pEdit->bkColor(RGB(255, 255, 255));	
	pEdit->textColor(m_clrText);

	pEdit->SetCapture();
	pEdit->SetFocus();
	
	return FALSE;
}


////////////////////////////////////////////////////////////////////////////////
//
// ����  : CGeoDrawObj::Clone
// ����  : CGeoDrawObj*
// ����  : CGeoMapDoc *pDoc
// ˵��  : ��¡ͼ�ز���ͼ�ؼ��뵽�ĵ���
//
CGeoDrawObj* CGeoTextObj::Clone(CDrawLayer *pLayer)
{
	ASSERT_VALID(this);

	CGeoTextObj* pClone = new CGeoTextObj(m_rtPosition,pLayer);
	pClone->m_rtPosition	=	m_rtPosition;
	pClone->m_bLocked		=	m_bLocked;			// ����ͼ��
	pClone->m_bShow			=	m_bShow;			// �Ƿ���ʾ
	pClone->m_strID			=	newGUID();			// �µ�ID
	pClone->m_ptAnchors.Copy(m_ptAnchors);			// ê��

	pClone->m_pDocument		=	m_pDocument;		// �ĵ�
	pClone->m_geoCenterPoint		=	m_geoCenterPoint;

	pClone->m_strText			= m_strText;			// �ı�
	pClone->m_clrText			= m_clrText;			// ������ɫ
	pClone->m_nFormat			= m_nFormat;			// ��ʽ
	memcpy(&pClone->m_lfFont,&m_lfFont,sizeof(LOGFONT));			// ����

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
void CGeoTextObj::Serialize(CArchive &ar)
{
	CGeoDrawObj::Serialize(ar);

	ar.SerializeClass(RUNTIME_CLASS(CGeoTextObj));

	if (ar.IsStoring())
	{
		ar << m_strText;						// �ı�
		ar << m_clrText;						// ������ɫ
		ar << m_nFormat;						// ��ʽ
		ar.Write(&m_lfFont,sizeof(LOGFONT));	// ����
	}
	else
	{
		// get the document back pointer from the archive

		int nVersion = ar.GetObjectSchema(); // ��ȡ�汾��Ϣ
		switch( nVersion )
		{
		case 0:
			{				
				ar >> m_strText;						// �ı�
				ar >> m_clrText;						// ������ɫ
				ar >> m_nFormat;						// ��ʽ
				ar.Read(&m_lfFont,sizeof(LOGFONT));		// ����
			}
			break;
		case 1:
			break;
		case 2:
			break;
		default:
			break;
		}

	}
}


////////////////////////////////////////////////////////////////////////////////
//
// ����  : CGeoTextObj::HitTestHanle
// ����  : int
// ����  : CPoint point���߼�����
// ����  : CGeoMapView *pView
// ����  : BOOL bSelected ����Ѿ�ѡ�У������Ƿ����ĳ���ֱ�
// ˵��  : �����������1ѡ�и�ͼ�أ�����û�е��и�ͼ��
//
int CGeoTextObj::HitTestHandle(CPoint point, CGeoMapView *pView, BOOL bSelected)
{
	ASSERT_VALID(this);
	ASSERT(pView != NULL);

	if (bSelected) // �����ͼ���Ѿ���ѡ����
	{
		int nHandleCount = GetHandleCount(); // �ֱ�����
		for (int nHandle = 1; nHandle <= nHandleCount; nHandle += 1)
		{
			// ����ĳ�ֱ����߼���Χ
			CRect rc = GetHandleRect(nHandle,pView);
			rc.NormalizeRect();
			if (point.x >= rc.left && point.x <= rc.right &&
				point.y >= rc.top && point.y <= rc.bottom)
				return nHandle;
		}
	}
	else // ����, �����Ƿ�����ͼ�ط�Χ����
	{
		if(IsPoint(point,pView))
			return 1;
	}
	return 0;
}

// �������߷����ı�ʱ
void CGeoTextObj::OnScaleChanged()
{
	CGeoDrawObj::OnScaleChanged();
}

// ��������תʱ
void CGeoTextObj::OnAngleChanged()
{
	Invalidate();
}

// ����������ԭ���иı�ʱ
void CGeoTextObj::OnCoordOrigChanged()
{
	Invalidate();
}

BOOL CGeoTextObj::IsPoint(CPoint point, CGeoMapView *pView)
{
	if(!m_bShow)
		return FALSE;

	if( m_pParentLayer->m_rgnClipping.m_nPoints > 2
		&& m_pParentLayer->m_bClipping)		
	{
		CRect rt = m_pParentLayer->m_rgnClipping.m_rtPosition;
		rt.NormalizeRect();
		if(!rt.PtInRect(point))
			return FALSE;
	}

	CClientDC dc(pView);
	pView->OnPrepareDC(&dc);

	CSize size(20,20);
	dc.DPtoLP(&size);
	dc.LPtoDP(&point);

	CRect rect = m_rtPosition;
	rect.NormalizeRect();

	CPen pen1(PS_GEOMETRIC | PS_SOLID | PS_JOIN_MITER | PS_ENDCAP_FLAT , size.cx , RGB(0, 0, 0) );

	CPen *pOldPen = dc.SelectObject(&pen1);	

	dc.BeginPath();	

	dc.Rectangle(rect);

	dc.EndPath();

	dc.SelectObject(pOldPen);
	pen1.DeleteObject();

	if( m_rgn.m_hObject )
		m_rgn.DeleteObject();

	m_rgn.CreateFromPath(&dc);

	if( !m_rgn.m_hObject )
		return FALSE;

	if( m_rgn.PtInRegion(point) )
		return TRUE;
	else
		return FALSE;
}

void CGeoTextObj::SelectClipRgn(CDC *pDC)
{
	CRect rect = m_rtPosition;
	rect.NormalizeRect();

	pDC->BeginPath();	

	pDC->Rectangle(rect);

	pDC->EndPath();

	pDC->SelectClipPath(RGN_COPY);

	return;
}

void CGeoTextObj::CopyAttribute(CGeoDrawObj *pSrcObj)
{	
	CGeoTextObj *pObj = (CGeoTextObj*)pSrcObj;

	m_clrText = pObj->m_clrText;				// ������ɫ
	m_nFormat = pObj->m_nFormat;				// ��ʽ
	memcpy(&m_lfFont, &pObj->m_lfFont,sizeof(LOGFONT));	// ����	

	CGeoDrawObj::CopyAttribute(pSrcObj);
}