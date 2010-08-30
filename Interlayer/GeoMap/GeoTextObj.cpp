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
	m_strText		= _T("文本");
	m_clrText		= RGB(0,0,0);			// 字体颜色
	m_nFormat		= DT_CENTER;			// 格式

	memset(&m_lfFont, 0, sizeof(LOGFONT) );	// 字体
	m_lfFont.lfHeight = 40;
	m_lfFont.lfCharSet = GB2312_CHARSET;
	strcpy_s(m_lfFont.lfFaceName, "宋体");
}

CGeoTextObj::CGeoTextObj(CDrawLayer* pLayer)
:CGeoDrawObj(pLayer)
{
	m_strText		= _T("文本");
	m_clrText		= RGB(0,0,0);			// 字体颜色
	m_nFormat		= DT_CENTER;			// 格式

	memset(&m_lfFont, 0, sizeof(LOGFONT) );	// 字体
	m_lfFont.lfHeight = 80;
	m_lfFont.lfCharSet = GB2312_CHARSET;
	strcpy_s(m_lfFont.lfFaceName, "宋体");
}

CGeoTextObj::CGeoTextObj(const CRect& position, CDrawLayer* pLayer)
:CGeoDrawObj(position,pLayer)
{
	m_strText		= _T("文本");
	m_clrText		= RGB(0,0,0);			// 字体颜色
	m_nFormat		= DT_CENTER;			// 格式

	memset(&m_lfFont, 0, sizeof(LOGFONT) );	// 字体
	m_lfFont.lfHeight = 80;
	m_lfFont.lfCharSet = GB2312_CHARSET;
	strcpy_s(m_lfFont.lfFaceName, "宋体");
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
		if( !Intersects(cliRect) ) //如果该图素不在当前窗口中则不显示
			return ;
	}

	int nSaved = pDC->SaveDC();
	pDC->SetBkMode(TRANSPARENT);          //设定显示方式

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
// 函数  : CGeoTextObj::OnEditProperties
// 返回  : void
// 参数  : CGeoMapView *pView
// 说明  : 编辑图素属性
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
// 函数  : CGeoDrawObj::Clone
// 返回  : CGeoDrawObj*
// 参数  : CGeoMapDoc *pDoc
// 说明  : 克隆图素并将图素加入到文档中
//
CGeoDrawObj* CGeoTextObj::Clone(CDrawLayer *pLayer)
{
	ASSERT_VALID(this);

	CGeoTextObj* pClone = new CGeoTextObj(m_rtPosition,pLayer);
	pClone->m_rtPosition	=	m_rtPosition;
	pClone->m_bLocked		=	m_bLocked;			// 锁定图素
	pClone->m_bShow			=	m_bShow;			// 是否显示
	pClone->m_strID			=	newGUID();			// 新的ID
	pClone->m_ptAnchors.Copy(m_ptAnchors);			// 锚点

	pClone->m_pDocument		=	m_pDocument;		// 文档
	pClone->m_geoCenterPoint		=	m_geoCenterPoint;

	pClone->m_strText			= m_strText;			// 文本
	pClone->m_clrText			= m_clrText;			// 字体颜色
	pClone->m_nFormat			= m_nFormat;			// 格式
	memcpy(&pClone->m_lfFont,&m_lfFont,sizeof(LOGFONT));			// 字体

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
void CGeoTextObj::Serialize(CArchive &ar)
{
	CGeoDrawObj::Serialize(ar);

	ar.SerializeClass(RUNTIME_CLASS(CGeoTextObj));

	if (ar.IsStoring())
	{
		ar << m_strText;						// 文本
		ar << m_clrText;						// 字体颜色
		ar << m_nFormat;						// 格式
		ar.Write(&m_lfFont,sizeof(LOGFONT));	// 字体
	}
	else
	{
		// get the document back pointer from the archive

		int nVersion = ar.GetObjectSchema(); // 获取版本信息
		switch( nVersion )
		{
		case 0:
			{				
				ar >> m_strText;						// 文本
				ar >> m_clrText;						// 字体颜色
				ar >> m_nFormat;						// 格式
				ar.Read(&m_lfFont,sizeof(LOGFONT));		// 字体
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
// 函数  : CGeoTextObj::HitTestHanle
// 返回  : int
// 参数  : CPoint point是逻辑坐标
// 参数  : CGeoMapView *pView
// 参数  : BOOL bSelected 如果已经选中，返回是否点中某个手柄
// 说明  : 否则，如果返回1选中该图素，否则没有点中该图素
//
int CGeoTextObj::HitTestHandle(CPoint point, CGeoMapView *pView, BOOL bSelected)
{
	ASSERT_VALID(this);
	ASSERT(pView != NULL);

	if (bSelected) // 如果该图素已经被选中了
	{
		int nHandleCount = GetHandleCount(); // 手柄总数
		for (int nHandle = 1; nHandle <= nHandleCount; nHandle += 1)
		{
			// 返回某手柄的逻辑范围
			CRect rc = GetHandleRect(nHandle,pView);
			rc.NormalizeRect();
			if (point.x >= rc.left && point.x <= rc.right &&
				point.y >= rc.top && point.y <= rc.bottom)
				return nHandle;
		}
	}
	else // 否则, 测试是否落在图素范围内了
	{
		if(IsPoint(point,pView))
			return 1;
	}
	return 0;
}

// 当比例尺发生改变时
void CGeoTextObj::OnScaleChanged()
{
	CGeoDrawObj::OnScaleChanged();
}

// 当发生旋转时
void CGeoTextObj::OnAngleChanged()
{
	Invalidate();
}

// 当发生坐标原点有改变时
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

	m_clrText = pObj->m_clrText;				// 字体颜色
	m_nFormat = pObj->m_nFormat;				// 格式
	memcpy(&m_lfFont, &pObj->m_lfFont,sizeof(LOGFONT));	// 字体	

	CGeoDrawObj::CopyAttribute(pSrcObj);
}