#include "stdafx.h"
#include "ColorLookUpTable.h"
#include <math.h>

CArchive& operator<<(CArchive& ar, CColorNode& dat)
{
	dat.Serialize(ar);
	return ar;
}

CArchive& operator>>(CArchive& ar, CColorNode& dat)
{
	dat.Serialize(ar);
	return ar;
}

CArchive& operator<<(CArchive& ar, CColorLookUpTable& dat)
{
	dat.Serialize(ar);
	return ar;
}

CArchive& operator>>(CArchive& ar, CColorLookUpTable& dat)
{
	dat.Serialize(ar);
	return ar;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CColorLookUpTable::CColorLookUpTable()
: m_pColorTable(NULL)
{
	m_ValueMin = 0.0;
	m_ValueMax = 500.0;

	//缺省颜色表为RainBow颜色表
	m_ColorTable.Add( CColorNode(0.f,RGB(153,102,255)) );
	m_ColorTable.Add( CColorNode(0.2f,0x0000FF) );
	m_ColorTable.Add( CColorNode(0.4f,0x00FF00) );
	m_ColorTable.Add( CColorNode(0.6f,0xFFFF00) );
	m_ColorTable.Add( CColorNode(0.8f,RGB(255,102,0)) );
	m_ColorTable.Add( CColorNode(1.f,0xFF0000) );
}

CColorLookUpTable::~CColorLookUpTable()
{

}

CColorLookUpTable& CColorLookUpTable::operator=(const CColorLookUpTable& table)
{
	m_ColorTable.Copy(table.m_ColorTable);
	m_ValueMin = table.m_ValueMin;
	m_ValueMax = table.m_ValueMax;

	return *this;
}

CColorLookUpTable::CColorLookUpTable(const CColorLookUpTable&table)
{
	m_ColorTable.Copy(table.m_ColorTable);
	m_ValueMin = table.m_ValueMin;
	m_ValueMax = table.m_ValueMax;
}

//根据pos的位置找出在ColorTable中对应的颜色,
//pos为0～1的值
COLORREF CColorLookUpTable::GetColor(double pos) const
{

	if( pos<0.f )
		pos = 0;

	//return RGB(0,0,0);
	if( pos>1.0f )
		pos = 1.0;
	//return RGB(255,255,255);

	if(pos<=m_ColorTable.GetAt(0).pos)
		return m_ColorTable.GetAt(0).color;

	int count=m_ColorTable.GetSize();
	
	double ps = 0;

	for(int i=0; i<count; i++)
	{
		ps = m_ColorTable[i].pos;

		if( pos == ps)
		{
			return m_ColorTable[i].color;
		}
		else if( pos > ps )
		{
			continue;
		}
		else if( pos < ps )
		{ 
			double ratio = (pos - m_ColorTable[i-1].pos )
				/ ( m_ColorTable[i].pos - m_ColorTable[i-1].pos );

			COLORREF color = m_ColorTable[i-1].color;
			BYTE s_r = GetRValue(color);
			BYTE s_g = GetGValue(color);
			BYTE s_b = GetBValue(color);

			color = m_ColorTable[i].color;
			BYTE e_r = GetRValue(color);
			BYTE e_g = GetGValue(color);
			BYTE e_b = GetBValue(color);

			return RGB(s_r+(e_r-s_r)*ratio,
				s_g+(e_g-s_g)*ratio,
				s_b+(e_b-s_b)*ratio);
		}
	}

	return m_ColorTable.GetAt(count-1).color;
}

void CColorLookUpTable::SetColor(double pos, COLORREF newClr)
{
	ASSERT( pos>=0.f && pos<=1.0f );

	InsertColorIntoTable(pos,newClr);
}

//在颜色表中查询v值的颜色
COLORREF CColorLookUpTable::LookUpColor(double v) const
{
	ASSERT( v>=m_ValueMin && v<=m_ValueMax );

	return GetColor( (v-m_ValueMin)/(m_ValueMax-m_ValueMin) );
}

// 向颜色表中加入颜色
void CColorLookUpTable::AddColor(double v, COLORREF newClr)
{
	ASSERT( v>=m_ValueMin && v<=m_ValueMax );
	SetColor((v-m_ValueMin)/(m_ValueMax-m_ValueMin), newClr);
}

//向颜色表中加入颜色节点,根据pos从小到大顺序插入,同时返回插入后所在的位置
int CColorLookUpTable::InsertColorIntoTable(double pos, COLORREF newClr)
{
	ASSERT( pos>=0.f && pos<=1.0f );

	int count = m_ColorTable.GetSize();

	double p = 0.f;
	for(int i=0; i<count; i++)
	{
		p = m_ColorTable.GetAt(i).pos;
		if( pos > p)
		{			
			continue;
		}
		else if( pos == p )
		{
			m_ColorTable[i].color = newClr;  //修改
			return i;
		}
		else if( pos < p  )
		{
			m_ColorTable.InsertAt(i,CColorNode(pos,newClr));
			return i;
		}
	}

	return m_ColorTable.Add(CColorNode(pos,newClr));
}

//删除颜色表中的颜色节点
BOOL CColorLookUpTable::RemoveColorFromTable(double pos)
{
	int nSize = m_ColorTable.GetSize();

	for(int i=0; i<nSize; i++)
	{
		if( fabs(m_ColorTable.GetAt(i).pos - pos) < 0.0000001 )
		{
			m_ColorTable.RemoveAt(i);
			return TRUE;
		}
	}

	return FALSE;

}

void CColorLookUpTable::GradientRectangle(CDC* pDC, const CRect& drawRect,
										  COLORREF clrStart,COLORREF clrEnd, 
										  BOOL bHorz)
{
	//从clrStart开始到clrEnd渐变填充矩形drawRect


	int width  = drawRect.Width();
	int height = drawRect.Height();

	CPoint pt0 = drawRect.TopLeft(); //左上角
	CPoint pt1 = drawRect.BottomRight(); //右下角

	BYTE r,g,b;

	//按像素填充颜色
	COLORREF fillClr;

	if( bHorz )
	{
		for(int i=pt0.x; i<pt1.x; i++)
		{
			//线性插值
			r = i*( GetRValue(clrEnd) - GetRValue(clrStart) ) / width + ( pt1.x*GetRValue(clrStart) - pt0.x*GetRValue(clrEnd) )/width;
			g = i*( GetGValue(clrEnd) - GetGValue(clrStart) ) / width + ( pt1.x*GetGValue(clrStart) - pt0.x*GetGValue(clrEnd) )/width;
			b = i*( GetBValue(clrEnd) - GetBValue(clrStart) ) / width + ( pt1.x*GetBValue(clrStart) - pt0.x*GetBValue(clrEnd) )/width;

			fillClr = RGB(r,g,b);

			for(int j=pt0.y; j<pt1.y; j++)
			{
				//MemDC.SetPixelV(i,j,RGB(r,g,b));
				pDC->SetPixelV(i,j,fillClr);
			}
		}
	}
	else
	{
		for(int j=pt0.y; j<pt1.y; j++)
		{
			//线性插值
			r = j*( GetRValue(clrEnd) - GetRValue(clrStart) ) / height + ( pt1.y*GetRValue(clrStart) - pt0.y*GetRValue(clrEnd) )/height;
			g = j*( GetGValue(clrEnd) - GetGValue(clrStart) ) / height + ( pt1.y*GetGValue(clrStart) - pt0.y*GetGValue(clrEnd) )/height;
			b = j*( GetBValue(clrEnd) - GetBValue(clrStart) ) / height + ( pt1.y*GetBValue(clrStart) - pt0.y*GetBValue(clrEnd) )/height;

			fillClr = RGB(r,g,b);
			for(int i=pt0.x; i<pt1.x; i++)
			{

				//MemDC.SetPixelV(i,j,RGB(r,g,b));
				pDC->SetPixelV(i,j,fillClr);
			}
		}
	}

}

//绘制颜色表的颜色谱
void CColorLookUpTable::DrawColorSpectrum(CDC* pDC, const CRect& drawRect, BOOL bHorz)
{	
	CRect rect(drawRect);
	rect.NormalizeRect();

	pDC->LPtoDP(&rect);
	rect.NormalizeRect();

	if( bHorz )
	{
		CDC memDC;
		memDC.CreateCompatibleDC(pDC);
		CBitmap bitmap;
		if (bitmap.CreateCompatibleBitmap(pDC, rect.Width(), 1))
		{
			CBitmap *pBit = memDC.SelectObject(&bitmap);
			double pos;
			COLORREF clr;
			int Width = rect.Width();
			for (int i=0;i<Width;i++)
			{
				pos = double(i)/Width;
				clr = GetColor(pos);
				memDC.SetPixelV(i,0,clr);
			}

			pDC->StretchBlt(drawRect.left,drawRect.top,drawRect.Width(),
				drawRect.Height(),&memDC,0,0,rect.Width(),1,SRCCOPY);

			memDC.SelectObject(pBit);
		}
	}
	else
	{
		CDC memDC;
		memDC.CreateCompatibleDC(pDC);
		CBitmap bitmap;
		if (bitmap.CreateCompatibleBitmap(pDC, 1, rect.Height()))
		{
			CBitmap *pBit = memDC.SelectObject(&bitmap);
			double pos;
			COLORREF clr;
			int Height = rect.Height();
			for (int i=0;i<Height;i++)
			{
				pos = double(i)/Height;
				clr = GetColor(pos);
				memDC.SetPixelV(0,i,clr);
			}

			pDC->StretchBlt(drawRect.left,drawRect.top,drawRect.Width(),
				drawRect.Height(),&memDC,0,0,1,rect.Height(),SRCCOPY);

			memDC.SelectObject(pBit);
		}
	}	
}

BOOL CColorLookUpTable::LoadColorTable(const CString& filePath )
{
	FILE* fp;
	errno_t err;

	// Open for read (will fail if file "crt_fopen_s.c" does not exist)
	err  = fopen_s( &fp, (LPCTSTR)filePath,_T("rt") );
	if( err == 0 )
	{
		return FALSE;
	}

	char head[13]="";
	int headlen=strlen("ColorMap 1 1");
	fread( head, sizeof( char ), headlen, fp );
	if(strcmp(head,"ColorMap 1 1")!=0)
	{
		AfxMessageBox("文件格式有误，不能打开文件.");
		fclose(fp);
		return FALSE;
	}

	//-------开始读取颜色表------------------
	m_ColorTable.RemoveAll();

	UINT r,g,b;
	double pos;
	while(!feof(fp))
	{
		fscanf_s(fp,"%lf%d%d%d",&pos, &r, &g, &b);
		m_ColorTable.Add( CColorNode(pos/100.f,RGB(r,g,b)) );
		if(pos>=100.f)
			break;
	}

	fclose(fp);

	return TRUE;
}

BOOL CColorLookUpTable::SaveColorTable(const CString& filePath )
{

	FILE* fp;

	errno_t err;

	err  = fopen_s( &fp, filePath, "wt" );
	if( err == 0 )
		return FALSE;


	fprintf(fp, "ColorMap 1 1\n");
	int nCount = m_ColorTable.GetSize();
	for (int i=0;i<nCount;i++)
	{
		fprintf(fp, "%.2lf %d %d %d\n", 
			m_ColorTable[i].pos*100, 
			GetRValue(m_ColorTable[i].color),
			GetGValue(m_ColorTable[i].color),
			GetBValue(m_ColorTable[i].color));
	}

	fclose(fp);

	return TRUE; 
}


void CColorLookUpTable::Serialize(CArchive& ar) // 序列化
{
	if( ar.IsStoring() )
	{
		int size = m_ColorTable.GetSize();
		ar << size;
		for (int i=0;i<size;i++)
		{
			ar << m_ColorTable[i];
		}
		ar << m_ValueMin;
		ar << m_ValueMax;       //数值的范围,该范围内的数值和颜色表内的颜色相互对应
	}
	else
	{
		m_ColorTable.RemoveAll();
		int size;
		ar >> size;
		for (int i=0;i<size;i++)
		{
			CColorNode node;
			ar >> node;
			m_ColorTable.Add(node);
		}
		ar >> m_ValueMin;
		ar >> m_ValueMax;       //数值的范围,该范围内的数值和颜色表内的颜色相互对应
	}
}

void CColorLookUpTable::SetValueRange(double min, double max)
{
	ASSERT( min < max );
	m_ValueMin = min;
	m_ValueMax = max;
}

void CColorLookUpTable::GetValueRange(double &min, double &max)
{
	min = m_ValueMin;
	max = m_ValueMax;
}