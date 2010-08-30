#include "stdafx.h"
#include "graph_props.h"

/////////////////////////////////////////////////////////////////////////////
//class CPointsCollection
CGraphProps::CGraphProps(COLORREF newColor, const TCHAR* newTitle, BOOL VisibleFlag, BOOL _bResources, int _index/* = -1*/, BOOL b_sort_x/* = TRUE*/, BOOL b_keep_same_x/* = FALSE*/) : CPointsCollection(b_sort_x, b_keep_same_x)
{
	m_grcolor = newColor;
	m_grTitle = newTitle;
	m_bIsVisible = VisibleFlag;
	m_index = _index;
	bPrivateResources = _bResources;
	if (bPrivateResources)
	{
		pen = new CPen(PS_SOLID, 1, m_grcolor);
		brush = new CBrush(m_grcolor);
	}
	else
	{
		pen = NULL;
		brush = NULL;
	}

	p1.x = 0.0;
	p1.y = 1.0;

	p2.x = 0.5;
	p2.y = 0.5;

	m_bRulerIsVisible = TRUE;
	m_bRulerLinkage = FALSE;
	m_bAngleLinkage = FALSE;
	m_bAngleZipLinkage = FALSE;
	m_bLenLinkage = FALSE;

}

CGraphProps::CGraphProps() : CPointsCollection(TRUE, FALSE)
{
	p1.x = 0.0;
	p1.y = 1.0;

	p2.x = 0.5;
	p2.y = 0.5;

	m_bRulerIsVisible = TRUE;
	m_bRulerLinkage = FALSE;
	m_bAngleLinkage = FALSE;
	m_bAngleZipLinkage = FALSE;
	m_bLenLinkage = FALSE;
}

CGraphProps::~CGraphProps()
{
	if (pen != NULL)
		delete pen;
	if (brush != NULL)
		delete brush;
}

CGraphProps::CGraphProps(CGraphProps* grprop, BOOL bCopyPoints /*= TRUE*/)
{
	pen = NULL;
	brush = NULL;
	if (bCopyPoints)
		CPointsCollection(grprop);
	else
		CPointsCollection();
	SetGraphProps(grprop);

	p1.x = 0.0;
	p1.y = 1.0;

	p2.x = 0.5;
	p2.y = 0.5;

	m_bRulerIsVisible = TRUE;
	m_bRulerLinkage = FALSE;
	m_bAngleLinkage = FALSE;
	m_bAngleZipLinkage = FALSE;
	m_bLenLinkage = FALSE;
}

void CGraphProps::SetGraphProps(CGraphProps* grprop)
{
	bPrivateResources = grprop->bPrivateResources;
	SetGraphColor(grprop->m_grcolor);
	m_grTitle = grprop->m_grTitle;
	m_bIsVisible = grprop->m_bIsVisible;
	m_index = grprop->m_index;
}

void CGraphProps::SetGraphColor(COLORREF newColor)
{
	m_grcolor = newColor;
	//recreate pen and brush
	if (AreResourcesPrivate())
	{
		if (pen != NULL)
			delete pen;
		if (brush != NULL)
			delete brush;
		pen = new CPen(PS_SOLID, 1, m_grcolor);
		brush = new CBrush(m_grcolor);
	};
}

void CGraphProps::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << bPrivateResources;
		ar << (DWORD)m_grcolor;
		ar << m_grTitle;
		ar << m_bIsVisible;
		ar << m_index;

		ar << p1.x;
		ar << p1.y;

		ar << p2.x;
		ar << p2.y;

		ar << m_bRulerIsVisible;
		ar << m_bRulerLinkage;
		ar << m_bAngleLinkage;
		ar << m_bAngleZipLinkage;
		ar << m_bLenLinkage;
	}
	else
	{
		DWORD wd;

		ar >> bPrivateResources;
		ar >> wd;
		m_grcolor = (COLORREF)wd;
		ar >> m_grTitle;
		ar >> m_bIsVisible;
		ar >> m_index;

		ar >> p1.x;
		ar >> p1.y;

		ar >> p2.x;
		ar >> p2.y;

		ar >> m_bRulerIsVisible;
		ar >> m_bRulerLinkage;
		ar >> m_bAngleLinkage;
		ar >> m_bAngleZipLinkage;
		ar >> m_bLenLinkage;

		if (bPrivateResources)
		{
			pen = new CPen(PS_SOLID, 1, m_grcolor);
			brush = new CBrush(m_grcolor);
		}
		else
		{
			pen = NULL;
			brush = NULL;
		}
	}

	CPointsCollection::Serialize(ar);
}
//CAxisProps**************
CAxisProps::CAxisProps()
{
	formatStrings[0] = formatStrings[1] = formatStrings[2] = NULL;
	m_Title = ""; 
	m_UOM = ""; 
	m_Precision = 3;
	CalculateFormatStrings();
}

CAxisProps::~CAxisProps()
{
	for (int i = 0; i < 3; i++)
	{
		if (formatStrings[i] != NULL)
			delete formatStrings[i];
		formatStrings[i] = NULL;
	};
}

void CAxisProps::GetFormattedOutput(double value, int formatLevel, CString* str)
{
	if (formatLevel < 0 || formatLevel > 2)
	{
		*str = "";
	}
	else
	{
		str->Format(formatStrings[formatLevel], value);
	};
}

void CAxisProps::GetFullTitle(CString* str)
{
	*str = m_Title + ", " + m_UOM;
}

void CAxisProps::SetNewTitle(const TCHAR* title)
{
	m_Title = title;
	CalculateFormatStrings();
}

void CAxisProps::SetNewUOM(const TCHAR* newUOM)
{
	m_UOM = newUOM;
	CalculateFormatStrings();
}

void CAxisProps::SetNewPrecision(int newPrecision)
{
	m_Precision = newPrecision;
	CalculateFormatStrings();
}

void CAxisProps::CalculateFormatStrings()
{
	for (int i = 0; i < 3; i++)
	{
		if (formatStrings[i] != NULL)
			delete formatStrings[i];
		formatStrings[i] = NULL;
	};
	CString str;

	str.Format(_T("%%0.%df"), m_Precision);
	formatStrings[0] = new TCHAR[str.GetLength() + 1];
	//strcpy_s(formatStrings[0], str.GetLength(),(LPCTSTR)str);
	strcpy(formatStrings[0],(LPCTSTR)str);

	str.Format(_T("%%0.%df %s"), m_Precision, m_UOM);
	formatStrings[1] = new TCHAR[str.GetLength() + 1];
	//strcpy_s(formatStrings[1], str.GetLength(),(LPCTSTR)str);
	strcpy(formatStrings[1], (LPCTSTR)str);

	str.Format(_T("%s: %%0.%df %s"), m_Title, m_Precision, m_UOM);
	formatStrings[2] = new TCHAR[str.GetLength() + 1];
	//strcpy_s(formatStrings[2], str.GetLength(),(LPCTSTR)str);
	strcpy(formatStrings[2],(LPCTSTR)str);
}

