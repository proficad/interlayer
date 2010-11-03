#include "StdAfx.h"
#include "DrawPage.h"
#include "GeoMapDoc.h"
#include "GeoMapView.h"
#include "DlgPageSet.h"
#include "../CoordTrans.h"

extern const char* newGUID();

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL(CDrawPage, CRefObject, VERSIONABLE_SCHEMA|1)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDrawPage::CDrawPage()
{	
	m_pDocument			= NULL;
	m_bLocked			= FALSE;
	m_nHorzPages		= 1;	// 横向排页面总数 
	m_nVertPages		= 1;	// 纵向排页面总数
	m_strPageName		= _T("");				//页名
	m_strMark			= _T("");				//备注
	m_strID = newGUID();
	m_strCurLayerID = _T("");
	m_pCurDrawLayer = NULL;

	m_lScale			= 1;		// 1:50000 1cm=500米

	m_lZoom				= 1800;			// 屏幕放缩因子
	m_ptPosition.x		= -1;			// 屏幕位置	
	m_ptPosition.y		= -1;			// 屏幕位置	

	m_bPagePanel		= FALSE;
	m_iMarginWidth		= 100;			// 1cm

	m_ptCoordOrig.x		= 0;
	m_ptCoordOrig.x		= 0;
	m_ptCoordOrig.m_dValue	= 0;
	for(int i=0; i<6; i++)
	{
		m_ground2screen[i] = 0;
		m_screen2ground[i] = 0;
	}
	m_bParameter		= FALSE;
}

CDrawPage::CDrawPage(CGeoMapDoc* pDocument)
{
	m_pDocument			= pDocument;
	m_bLocked			= FALSE;
	m_nHorzPages		= 1;	// 横向排页面总数 
	m_nVertPages		= 1;	// 纵向排页面总数
	m_strPageName		= _T("");				//页名
	m_strMark			= _T("");				//备注
	m_strID = newGUID();
	m_strCurLayerID = _T("");
	m_pCurDrawLayer = NULL;

	m_lScale			= 1;		// 1:50000 1cm=500米

	m_lZoom				= 1800;			// 屏幕放缩因子
	m_ptPosition.x		= -1;			// 屏幕位置	
	m_ptPosition.y		= -1;			// 屏幕位置	

	m_bPagePanel		= FALSE;
	m_iMarginWidth		= 100;			// 1cm

	m_ptCoordOrig.x		= 0;
	m_ptCoordOrig.x		= 0;
	m_ptCoordOrig.m_dValue	= 0;
	for(int i=0; i<6; i++)
	{
		m_ground2screen[i] = 0;
		m_screen2ground[i] = 0;
	}
	m_bParameter		= FALSE;
}

CDrawPage::~CDrawPage()
{
	CDrawLayer *pDrawLayer;
	POSITION pos = m_DrawLayers.GetHeadPosition();
	while (pos != NULL)
	{
		pDrawLayer = m_DrawLayers.GetNext(pos);
		pDrawLayer->RemoveAll();		
	}
	m_DrawLayers.RemoveAll();
}

void CDrawPage::Serialize(CArchive &ar)
{
	CRefObject::Serialize(ar);
	ar.SerializeClass(RUNTIME_CLASS(CDrawPage));

	m_DrawLayers.Serialize(ar);

	if (ar.IsStoring())
	{
		ar << m_bLocked;
		ar << m_nHorzPages;	// 横向排页面总数 
		ar << m_nVertPages;	// 纵向排页面总数
		ar << m_strPageName;				//页名
		ar << m_strMark;				//备注	
		ar << m_strID;
		ar << m_strCurLayerID;

		ar << m_lScale;
		ar << m_ptCoordOrig;
		ar << m_bParameter;

		ar << m_lZoom;				// 屏幕放缩因子
		ar << m_ptPosition;			// 屏幕位置		
		ar << m_bPagePanel;
		ar << m_iMarginWidth;
		for(int i=0; i<6; i++)
		{
			ar << m_ground2screen[i];
			ar << m_screen2ground[i];
		}
	}
	else
	{
		int nVersion = ar.GetObjectSchema(); // 获取版本信息
		switch( nVersion )
		{
		case 1:
			{
				m_pDocument = (CGeoMapDoc*)ar.m_pDocument;
				ASSERT_VALID(m_pDocument);
				ASSERT_KINDOF(CGeoMapDoc, m_pDocument);
				ar >> m_bLocked;
				ar >> m_nHorzPages;	// 横向排页面总数 
				ar >> m_nVertPages;	// 纵向排页面总数
				ar >> m_strPageName;	// 页名
				ar >> m_strMark;	// 备注		
				ar >> m_strID;
				ar >> m_strCurLayerID;

				ar >> m_lScale;
				ar >> m_ptCoordOrig;
				ar >> m_bParameter;

				ar >> m_lZoom;				// 屏幕放缩因子
				ar >> m_ptPosition;			// 屏幕位置

				ar >> m_bPagePanel;
				ar >> m_iMarginWidth;

				for(int i=0; i<6; i++)
				{
					ar >> m_ground2screen[i];
					ar >> m_screen2ground[i];
				}

				POSITION pos = m_DrawLayers.GetHeadPosition();
				while (pos != NULL)
				{
					CDrawLayer* pObj = m_DrawLayers.GetNext(pos);
					pObj->m_pParentPage = this;	
				}

				if( m_strCurLayerID.IsEmpty() )
					m_pCurDrawLayer = m_DrawLayers.GetTail();
				else
					m_pCurDrawLayer = GetLayer(m_strCurLayerID);

				if( m_pCurDrawLayer == NULL )
					m_pCurDrawLayer = m_DrawLayers.GetTail();
			}
			break;
		default:
			break;
		}

	}
}

void CDrawPage::Draw(CDC *pDC, CGeoMapView *pView)
{
	POSITION pos = m_DrawLayers.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawLayer* pObj = m_DrawLayers.GetNext(pos);
		pObj->Draw(pDC, pView);		
	}

	pos = m_DrawLayers.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawLayer* pObj = m_DrawLayers.GetNext(pos);
		pObj->DrawTracker(pDC, pView);		
	}	
}

BOOL CDrawPage::Add(CDrawLayer *pObj)
{
	TRY
	{
		pObj->m_pDocument = m_pDocument;
		pObj->m_pParentPage = this;
		m_DrawLayers.AddTail(pObj);
		m_pCurDrawLayer = pObj;
		m_strCurLayerID = m_pCurDrawLayer->m_strID;
		if( m_pDocument )
			m_pDocument->SetModifiedFlag();
	}
	CATCH_ALL(e)
	{
		e->Delete();
		return FALSE;
	}
	END_CATCH_ALL

		return TRUE;
}

BOOL CDrawPage::Remove(CDrawLayer *pObj)
{
	pObj->Invalidate();
	if( m_DrawLayers.GetCount() == 1) // 如果是最后一层
	{
		m_DrawLayers.RemoveAll();

		CDrawLayer *pLayer = new CDrawLayer(m_pDocument);
		pLayer->m_strLayerName = "图层 1";
		Add(pLayer);

		return TRUE;
	}

	// Find and remove from document
	POSITION pos = m_DrawLayers.Find(pObj);	

	if( m_pCurDrawLayer == pObj )		
	{
		POSITION tmpPos = m_DrawLayers.GetTailPosition();
		if( tmpPos == pos )
		{
			m_DrawLayers.RemoveAt(pos);
			m_pCurDrawLayer = m_DrawLayers.GetTail();
			m_strCurLayerID = m_pCurDrawLayer->m_strID;
		}
		else
		{
			POSITION tmpPos = pos;
			m_DrawLayers.GetNext(pos);
			m_pCurDrawLayer = m_DrawLayers.GetNext(pos);
			m_DrawLayers.RemoveAt(tmpPos);
			m_strCurLayerID = m_pCurDrawLayer->m_strID;
		}

		m_pDocument->m_pCurDrawLayer = m_pCurDrawLayer;
	}
	else
		m_DrawLayers.RemoveAt(pos);

	m_pDocument->SetModifiedFlag();

	return TRUE;
}

void CDrawPage::RemoveAll()
{
	CDrawLayer *pDrawLayer;
	POSITION pos = m_DrawLayers.GetHeadPosition();
	while (pos != NULL)
	{
		pDrawLayer = m_DrawLayers.GetNext(pos);
		pDrawLayer->RemoveAll();		
	}
	m_DrawLayers.RemoveAll();

	m_pDocument->SetModifiedFlag();
}

CSize CDrawPage::GetPageSize()
{
	CRect rect = GetPageRect();
	CSize m_size(rect.Width(), rect.Height());
	return m_size;
}

CRect CDrawPage::GetPageRect()
{
	CRect rect(0, 0, 0, 0);
	POSITION pos = m_DrawLayers.GetHeadPosition();
	CDrawLayer * pObj;
	while (pos != NULL)
	{		
		pObj = m_DrawLayers.GetNext(pos);
		CRect rc = pObj->GetLayerRect();
		rc.NormalizeRect();
		rect |= rc;
	}

	rect.NormalizeRect();	

	return rect;
}

CDrawLayer* CDrawPage::GetLayer(const int& iIndex)
{
	POSITION pos = m_DrawLayers.FindIndex(iIndex);
	return m_DrawLayers.GetAt(pos);
}

CDrawLayer* CDrawPage::GetLayer(const CString &strID)
{
	CDrawLayer *pDrawLayer = NULL;
	POSITION pos = m_DrawLayers.GetHeadPosition();
	while (pos != NULL)
	{
		pDrawLayer = m_DrawLayers.GetNext(pos);
		if( pDrawLayer->m_strID == strID)
			return pDrawLayer;
	}

	return NULL;
}

CString CDrawPage::GetLayerID(const int& iIndex)
{
	POSITION pos = m_DrawLayers.FindIndex(iIndex);
	CDrawLayer *pDrawLayer = m_DrawLayers.GetAt(pos);

	if( pDrawLayer != NULL)
		return pDrawLayer->m_strID;
	else
		return "";
}

CString CDrawPage::GetLayerID(const CDrawLayer* pLayer)
{
	if( pLayer != NULL)
		return pLayer->m_strID;
	else
		return "";
}

// 设置当前图层
void CDrawPage::SetCurLayer(CDrawLayer *pLayer)
{
	m_pCurDrawLayer = pLayer;
	m_strCurLayerID = pLayer->m_strID;

	m_pDocument->UpdateAllViews(NULL, HINT_UPDATE_LAYERBAR, m_pCurDrawLayer);
	m_pDocument->SetModifiedFlag();
}

// 获取当前图层
CDrawLayer *CDrawPage::GetCurLayer()
{
	return m_pCurDrawLayer;
}

//功能:图层移到最上一层
//参数:
//返回:
BOOL CDrawPage::MoveToFront(CDrawLayer *pLayer)
{
	if(Remove(pLayer))
		return Add(pLayer);		
	else
		return FALSE;	
}

//功能:图层移到最底一层
//参数:
//返回:
BOOL CDrawPage::MoveToBack(CDrawLayer *pLayer)
{
	if(Remove(pLayer))
	{
		m_DrawLayers.AddHead(pLayer);
		return TRUE;
	}
	else
		return FALSE;	
}

//功能:图层上移一层
//参数:
//返回:
BOOL CDrawPage::MoveForward(CDrawLayer *pLayer)
{
	POSITION pos = m_DrawLayers.Find(pLayer);
	ASSERT(pos != NULL);
	if (pos != m_DrawLayers.GetTailPosition())
	{
		POSITION posNext = pos;
		m_DrawLayers.GetNext(posNext);
		Remove(pLayer);
		m_DrawLayers.InsertAfter(posNext, pLayer);
		return TRUE;
	}
	return FALSE;
}

//功能:图层下移一层
//参数:
//返回:
BOOL CDrawPage::MoveBack(CDrawLayer *pLayer)
{
	POSITION pos = m_DrawLayers.Find(pLayer);
	ASSERT(pos != NULL);
	if (pos != m_DrawLayers.GetHeadPosition())
	{
		POSITION posPrev = pos;
		m_DrawLayers.GetPrev(posPrev);
		Remove(pLayer);
		m_DrawLayers.InsertBefore(posPrev, pLayer);	
		return TRUE;
	}
	return FALSE;
}

void CDrawPage::OnScaleChanged()
{
	POSITION pos = m_DrawLayers.GetHeadPosition();
	CDrawLayer * pObj;
	while (pos != NULL)
	{		
		pObj = m_DrawLayers.GetNext(pos);
		pObj->OnScaleChanged();
	}
}

void CDrawPage::OnEditProperties(CGeoMapView* pView )
{	
	CDlgPageSet dlg;
	dlg.m_strPageName		= m_strPageName;
	dlg.m_nHorzPages		= m_nHorzPages;
	dlg.m_nVertPages		= m_nVertPages;
	dlg.m_strMark			= m_strMark;
	dlg.m_dAngle			= m_ptCoordOrig.m_dValue;
	dlg.m_lScale			= m_lScale;
	dlg.m_iMarginWidth		= m_iMarginWidth;
	dlg.m_x					= m_ptCoordOrig.x;
	dlg.m_y					= m_ptCoordOrig.y;

	double lScale;
	double x,y,dAngle;
	lScale = m_lScale;
	x = m_ptCoordOrig.x;
	y = m_ptCoordOrig.y;
	dAngle = m_ptCoordOrig.m_dValue;

	if( dlg.DoModal() == IDOK )
	{
		m_strPageName			= dlg.m_strPageName;
		m_nHorzPages			= dlg.m_nHorzPages;
		m_nVertPages			= dlg.m_nVertPages;
		m_strMark				= dlg.m_strMark;
		m_ptCoordOrig.m_dValue	= dlg.m_dAngle;		
		m_lScale				= dlg.m_lScale;
		m_iMarginWidth			= dlg.m_iMarginWidth;
		m_ptCoordOrig.x			= dlg.m_x;
		m_ptCoordOrig.y			= dlg.m_y;

		if( lScale != m_lScale
			|| x != m_ptCoordOrig.x
			|| y != m_ptCoordOrig.y
			|| dAngle != m_ptCoordOrig.m_dValue)
		{
			OnScaleChanged();
		}

		if( pView)
			pView->Invalidate(FALSE);
		m_pDocument->SetModifiedFlag();
	}
	
}

// 坐标转换器
// 转换成大地坐标
CPoint2D CDrawPage::CoordinateTransfer(const CPoint &pt)
{
	// 100 代表1cm
	// pt.x/10000.0 化为米

	CPoint2D point;

	// a = Lx, b = Ly, c = Kcosａ, d = Ksinａ;
	double a, b, c, d;

	a = m_ptCoordOrig.x;
	b = m_ptCoordOrig.y;

	c = 1.0 / m_lScale * cos(this->GetAngle()/180.0*PI);
	d = 1.0 / m_lScale * sin(this->GetAngle()/180.0*PI);

	// 因为
	// 
	// X=a+X1*c-Y1*d
	// Y=b+Y1*c+X1*d
	//
	// 所以
	//
	// X1 = (x*c+y*d-(a*c+b*d))/(c*c+d*d)
	// Y1 = ((y*c-x*d)-(b*c-a*d))/(c*c+d*d)
	// 

	point.x = (pt.x*c+pt.y*d-(a*c+b*d))/(c*c+d*d);
	point.y = ((pt.y*c-pt.x*d)-(b*c-a*d))/(c*c+d*d);

	return point;
}

CPoint CDrawPage::CoordinateTransfer(const CPoint2D &pt)
{
	// 100 代表1cm
	// pt.x/10000.0 化为米

	CPoint2D point;

	// a = Lx, b = Ly, c = Kcosａ, d = Ksinａ;
	double a, b, c, d;

	a = m_ptCoordOrig.x;
	b = m_ptCoordOrig.y;

	c = 1.0 / m_lScale * cos(this->GetAngle()/180.0*PI);
	d = 1.0 / m_lScale * sin(this->GetAngle()/180.0*PI);

	// X=a+X1*c-Y1*d
	// Y=b+Y1*c+X1*d
	//
	point.x = a + pt.x*c - pt.y * d;
	point.y = b + pt.y*c + pt.x * d;

	CPoint retpt(point.x, point.y);

	return retpt;
}

CRectEx CDrawPage::CoordinateTransfer(const CRect &rect)
{
	CDrawPage *pPage = this;

	// a = Lx, b = Ly, c = Kcosａ, d = Ksinａ;
	double a, b, c, d;

	a = m_ptCoordOrig.x;
	b = m_ptCoordOrig.y;

	c = 1.0 / m_lScale * cos(this->GetAngle()/180.0*PI);
	d = 1.0 / m_lScale * sin(this->GetAngle()/180.0*PI);

	// 因为
	// 
	// X=a+X1*c-Y1*d
	// Y=b+Y1*c+X1*d
	//
	// 所以
	//
	// X1 = (x*c+y*d-(a*c+b*d))/(c*c+d*d)
	// Y1 = ((y*c-x*d)-(b*c-a*d))/(c*c+d*d)
	// 

	CPoint pt[4];
	pt[0].x =  rect.left;
	pt[0].y =  rect.top;

	pt[1].x =  rect.right;
	pt[1].y =  rect.top;

	pt[2].x =  rect.right;
	pt[2].y =  rect.bottom;

	pt[3].x =  rect.left;
	pt[3].y =  rect.bottom;

	double xMin = DBL_MAX, xMax = -DBL_MAX, yMin = DBL_MAX, yMax = -DBL_MAX; 
	for (int i=0; i<4; i++)
	{
		CPoint2D point = CoordinateTransfer(pt[i]);
		if(point.x < xMin)
			xMin = point.x;
		if(point.x > xMax)
			xMax = point.x;
		if(point.y < yMin)
			yMin = point.y;
		if(point.y > yMax)
			yMax = point.x;
	}

	CRectEx retRect;

	retRect.left = xMin;
	retRect.top = yMin;
	retRect.right = xMax;
	retRect.bottom = yMax;

	return retRect;
}

CRect CDrawPage::CoordinateTransfer(const CRectEx &rect)
{
	CDrawPage *pPage = this;

	// a = Lx, b = Ly, c = Kcosａ, d = Ksinａ;
	double a, b, c, d;

	a = m_ptCoordOrig.x;
	b = m_ptCoordOrig.y;

	c = 1.0 / m_lScale * cos(this->GetAngle()/180.0*PI);
	d = 1.0 / m_lScale * sin(this->GetAngle()/180.0*PI);

	// X=a+X1*c-Y1*d
	// Y=b+Y1*c+X1*d
	CPoint2D pt[4];
	pt[0].x =  rect.left;
	pt[0].y =  rect.top;

	pt[1].x =  rect.right;
	pt[1].y =  rect.top;

	pt[2].x =  rect.right;
	pt[2].y =  rect.bottom;

	pt[3].x =  rect.left;
	pt[3].y =  rect.bottom;

	long xMin = LONG_MAX, xMax = LONG_MIN, yMin = LONG_MAX, yMax = LONG_MIN; 
	for (int i=0; i<4; i++)
	{
		CPoint point = CoordinateTransfer(pt[i]);
		if(point.x < xMin)
			xMin = point.x;
		if(point.x > xMax)
			xMax = point.x;
		if(point.y < yMin)
			yMin = point.y;
		if(point.y > yMax)
			yMax = point.x;
	}

	CRect retRect;
	retRect.left = xMin;
	retRect.top = yMin;
	retRect.right = xMax;
	retRect.bottom = yMax;

	return retRect;
}

void CDrawPage::ClaimChildren()
{
	POSITION pos = m_DrawLayers.GetHeadPosition();
	while (pos != NULL)
	{
		CDrawLayer* pObj = m_DrawLayers.GetNext(pos);
		pObj->m_pParentPage = this;
		pObj->ClaimChildren();
	}
}

// 坐标转换器
// 转换成大地坐标
CPoint2D CDrawPage::CoordinateTransferNew(const CPoint &pt)
{
	// 100 代表1cm
	// pt.x/10000.0 化为米

	CPoint2D point;
	point.x = pt.x;
	point.y = pt.y;
	CCoordConverterNew *ct = new CCoordConverterNew(m_screen2ground, m_ground2screen);
	point = ct->Screen2Ground(point);
	return point;
}

CPoint CDrawPage::CoordinateTransferNew(const CPoint2D &pt)
{
	// 100 代表1cm
	// pt.x/10000.0 化为米

	CPoint point;
	CPoint2D tp;
	CCoordConverterNew *ct = new CCoordConverterNew(m_screen2ground, m_ground2screen);
	tp = ct->Ground2Screen(pt);
	point.x = tp.x;
	point.y = tp.y;
	return point;

}

CRectEx CDrawPage::CoordinateTransferNew(const CRect &rect)
{
	CDrawPage *pPage = this;

	// a = Lx, b = Ly, c = Kcosａ, d = Ksinａ;
	double a, b, c, d;

	a = m_ptCoordOrig.x;
	b = m_ptCoordOrig.y;

	c = 1.0 / m_lScale * cos(this->GetAngle()/180.0*PI);
	d = 1.0 / m_lScale * sin(this->GetAngle()/180.0*PI);

	// 因为
	// 
	// X=a+X1*c-Y1*d
	// Y=b+Y1*c+X1*d
	//
	// 所以
	//
	// X1 = (x*c+y*d-(a*c+b*d))/(c*c+d*d)
	// Y1 = ((y*c-x*d)-(b*c-a*d))/(c*c+d*d)
	// 

	CPoint pt[4];
	pt[0].x =  rect.left;
	pt[0].y =  rect.top;

	pt[1].x =  rect.right;
	pt[1].y =  rect.top;

	pt[2].x =  rect.right;
	pt[2].y =  rect.bottom;

	pt[3].x =  rect.left;
	pt[3].y =  rect.bottom;

	double xMin = DBL_MAX, xMax = -DBL_MAX, yMin = DBL_MAX, yMax = -DBL_MAX; 
	for (int i=0; i<4; i++)
	{
		CPoint2D point = CoordinateTransfer(pt[i]);
		if(point.x < xMin)
			xMin = point.x;
		if(point.x > xMax)
			xMax = point.x;
		if(point.y < yMin)
			yMin = point.y;
		if(point.y > yMax)
			yMax = point.x;
	}

	CRectEx retRect;

	retRect.left = xMin;
	retRect.top = yMin;
	retRect.right = xMax;
	retRect.bottom = yMax;

	return retRect;
}

CRect CDrawPage::CoordinateTransferNew(const CRectEx &rect)
{
	CDrawPage *pPage = this;

	// a = Lx, b = Ly, c = Kcosａ, d = Ksinａ;
	double a, b, c, d;

	a = m_ptCoordOrig.x;
	b = m_ptCoordOrig.y;

	c = 1.0 / m_lScale * cos(this->GetAngle()/180.0*PI);
	d = 1.0 / m_lScale * sin(this->GetAngle()/180.0*PI);

	// X=a+X1*c-Y1*d
	// Y=b+Y1*c+X1*d
	CPoint2D pt[4];
	pt[0].x =  rect.left;
	pt[0].y =  rect.top;

	pt[1].x =  rect.right;
	pt[1].y =  rect.top;

	pt[2].x =  rect.right;
	pt[2].y =  rect.bottom;

	pt[3].x =  rect.left;
	pt[3].y =  rect.bottom;

	long xMin = LONG_MAX, xMax = LONG_MIN, yMin = LONG_MAX, yMax = LONG_MIN; 
	for (int i=0; i<4; i++)
	{
		CPoint point = CoordinateTransfer(pt[i]);
		if(point.x < xMin)
			xMin = point.x;
		if(point.x > xMax)
			xMax = point.x;
		if(point.y < yMin)
			yMin = point.y;
		if(point.y > yMax)
			yMax = point.x;
	}

	CRect retRect;
	retRect.left = xMin;
	retRect.top = yMin;
	retRect.right = xMax;
	retRect.bottom = yMax;

	return retRect;
}
