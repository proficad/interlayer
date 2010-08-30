#if !defined _LOGCURVEDATSTRUCT_H
#define _LOGCURVEDATSTRUCT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning (disable:4786)

#include <afxtempl.h>
#include <map>
#include <string>
#include <vector>
#include <stdarg.h>
#include <math.h>
#include <float.h>

#define HINT_UPDATE_CHANGEDSIZE 1
#define HINT_UPDATE_LOADLOG		2	// 装载测井数据		
typedef std::vector<double> CDoubleArray;
typedef std::map<std::string, int> name_list;


//////////////////////////////////////////////////////////////////////////

class CLogDats // 曲线数据
{
public:
	CLogDats() { m_pLogDats = NULL;};
	~CLogDats() { RemoveAll(); };
	
private:
	name_list m_strLogNames;							// 曲线名称及对应的数据列
	CDoubleArray *m_pLogDats;							// 曲线数据
public:
	int		GetLogCount();								// 曲线数
	int		GetSamples();								// 数据数		
	int		GetLogIndex(LPCSTR lpsName);				// 曲线索引
	CString GetLogName(const int &iIndex);				// 曲线名称
	CString	AddLogName(LPCSTR lpsName, int iIndex);		// 增加曲线项目		
	BOOL	Add(int nCount,...);						// 增加曲线数据
	double	GetValue(LPCSTR lpname ,int iRow);			// 获取某曲线的第几个数据
	double	GetValue(int iCol ,int iRow);				// 获取某曲线的第几个数据
	double	GetMinValue(LPCSTR lpname);					// 获取最小值
	double	GetMaxValue(LPCSTR lpname);					// 获取最大值
	double	GetMinDepth();								// 最小井深
	double	GetMaxDepth();								// 最大井深	
	double	GetDepth(int iRow);							// 获取井深
	void	RemoveAll();								// 释放内存
	void	Serialize(CArchive& ar);					// 序列化
	
public:
	void	Add(double dVal,int iIndex)					// 增加某曲线数据
	{
		int size = m_strLogNames.size(); // 曲线数据列数
		if( size < 1 )					// 没有
			return ;
		
		if( m_pLogDats == NULL )
			m_pLogDats = new CDoubleArray[size];

		name_list::iterator theIterator = m_strLogNames.begin();

		int index = -1;

		for (; theIterator != m_strLogNames.end(); theIterator ++)
		{
			int idx = (*theIterator).second;
			if( iIndex == idx)
			{
				index = iIndex;
				break;
			}
		}
		
		if( index > -1) 
			m_pLogDats[index].push_back(dVal);
	}
};



//////////////////////////////////////////////////////////////////////////
	
// 网格线显示
#define GL_NONE                0L      // 不显示
#define GL_HORZ                1L      // 水平线
#define GL_VERT                2L      // 垂直线
#define GL_BOTH                3L      // 十字线
	
class CLogChart;
class CLogCurveLane    // 测井曲线泳道
{
public:
	enum LANETYPE
	{
		DEPTH = 0,				// 井深
			LOGCURVE,				// 测井曲线
			INTERLAYER,				// 夹层标志
	};
	
	struct strLOGINGPROP  //测井曲线属性
	{
		strLOGINGPROP()
		{
			m_strTitle = _T("");
			m_strLogName = _T("");
			m_iPart = 5;
			m_iMarkPart = 2;
			m_bDirect = FALSE;
			m_dCoordMin = DBL_MAX;
			m_dCoordMax = -DBL_MAX;
			m_dMinVal = DBL_MAX;
			m_dMaxVal = -DBL_MAX;
			m_clrLine = RGB(0,0,0);
			m_iLineStyle = PS_SOLID;
			m_iLineWidth = 3;
			m_bFill = FALSE;
			m_clrFill = RGB(128,0,0);
			m_iFillStyle = 6; //是实心填充
			
			m_bSelected = FALSE;
			m_pParentLane = NULL;
		}
	public:
		//////////////////////////////////////////////////////////////////////////
		// 保存的
		CString		m_strTitle;			// 中文名称加单位 用于显示
		CString		m_strLogName;		// 测井曲线名字
		COLORREF	m_clrLine;			// 曲线颜色
		int			m_iLineStyle;		// 曲线型
		int			m_iLineWidth;		// 曲线宽度
		BOOL		m_bFill;			// 是否填充
		COLORREF	m_clrFill;			// 填充颜色
		int			m_iFillStyle;		// 填充类型			
		
		double		m_dCoordMin;		// 坐标最小值
		double		m_dCoordMax;		// 坐标最大值
		int			m_iPart;			// 标注等分
		int			m_iMarkPart;		// 刻度等分
		BOOL		m_bDirect;			// 坐标方向TRUE反方向
		double		m_dMinVal;			// 数据最小值
		double		m_dMaxVal;			// 数据最大值
		
		CDoubleArray m_arLogDat;		// 曲线数据
		CDoubleArray m_arDepth;			// 曲线数据对应的井深
		
		//////////////////////////////////////////////////////////////////////////
		// 不用保存的

		BOOL		m_bSelected;		// 选中状态
		CLogCurveLane *m_pParentLane;	// 所属泳道
	public:
		
		strLOGINGPROP& operator=( const strLOGINGPROP& varSrc );
		void Serialize(CArchive& ar);
		void OnEditProperties( CView *pView );	// 编辑属性
		void Add(double dDepth, double dVal)    // 添加数据
		{
			if( dVal > -1000.0 && dVal < 1000)
			{
				m_arDepth.push_back(dDepth);
				m_arLogDat.push_back(dVal);
				if( m_dMinVal > dVal )
				{
					m_dMinVal = dVal;
					m_dCoordMin = dVal;
				}
				if( m_dMaxVal < dVal )
				{
					m_dMaxVal = dVal;
					m_dCoordMax = dVal;
				}
			}				
		}
	};
	
	public:
		CLogCurveLane(LANETYPE	nLaneType, CLogChart* pLogBox, CRect rect);
		CLogCurveLane();
		~CLogCurveLane(){};
		
	public:	// 属性
		// 保存属性
		CRect						m_rectLane;			// 泳道大小范围
		LANETYPE					m_nLaneType;		// 泳道类别
		
		//////////////////////////////////////////////////////////////////////////
		// 曲线道专有属性
		BOOL						m_bLogCoord;		// 是否为对数
		BOOL						m_bSameCoord;		// 是否为同一坐标系
		//////////////////////////////////////////////////////////////////////////
		
		BOOL						m_bBorder;			// 是否显示边框
		int							m_iGridLine;		// 网格线(GL_NONE,...)
		BOOL						m_bFill;			// 是否填充
		COLORREF					m_clrFill;			// 填充颜色
		CString						m_strTitle;			// 标题
		
		// 非保存属性

		// 如果是边道、井深，岩性或解释结果此泳道只能有一个数据集
		// 否则可以有多个测井曲线数据集
		CArray<strLOGINGPROP,strLOGINGPROP&>	m_arLoging;	// 所包含的测井曲线
		
		BOOL							m_bSelected;		// 选中
	private:
		CLogChart						*m_pChartLog;		// 测井曲线图
		CRect							m_rtLane;			// 画时的范围
		
	public:	// 操作

		CLogChart* GetLogChart() {return m_pChartLog;};
		void PasteDataFromClipboard(long lParam = 0);
		void CopyDataToClipboard(long lParam = 0);
		int GetSamples(int iIndex ) { return m_arLoging[iIndex].m_arLogDat.size(); }
		CLogCurveLane& operator=( const CLogCurveLane& varSrc );			
		void Serialize(CArchive& ar);
		void Draw(CDC *pDC, CView *pView, int iIndex);
		void DrawInterLayer(CDC *pDC, CView *pView);			// 夹层标记
		void DrawTitle(CDC *pDC, CView *pView, int iIndex);			// 画标题
		void SetParentChartLog(CLogChart *pChartLog) { m_pChartLog = pChartLog; }
		int  GetCurveCount() { return m_arLoging.GetSize(); }
		BOOL Add(int iIndex, double dDepth, double dVal) // 添加数据 index指的是第几条曲线
		{
			if(iIndex > m_arLoging.GetSize() )
				return FALSE;
			
			if( iIndex == m_arLoging.GetSize() )
			{
				strLOGINGPROP prop;
				prop.m_pParentLane = this;
				m_arLoging.Add(prop);
			}
			
			m_arLoging[iIndex].Add(dDepth, dVal);
			
			return TRUE;
		}
		
		void DelCurve(strLOGINGPROP *pProp, CView *pView = NULL ) // 删除曲线
		{
			int nSize = m_arLoging.GetSize();

			for (int i=0; i<nSize; i++)
			{
				strLOGINGPROP *p = &m_arLoging[i];
				if( p== pProp)
				{
					m_arLoging.RemoveAt(i);
					if( pView )
					{
						pView->Invalidate(FALSE);
						pView->GetDocument()->SetModifiedFlag();
					}
					break;
				}
			}			
		}

		void DelCurve(const int& iIndex, CView *pView = NULL ) // 删除曲线
		{
			m_arLoging.RemoveAt(iIndex);
			if( pView )
			{
				pView->Invalidate(FALSE);
				pView->GetDocument()->SetModifiedFlag();
			}
		}

		void DelAllCurve(CView *pView = NULL)
		{
			m_arLoging.RemoveAll();
			if( pView )
			{
				pView->Invalidate(FALSE);
				pView->GetDocument()->SetModifiedFlag();
			}
		}
		
		BOOL AddCurve(strLOGINGPROP &prop, CView *pView = NULL) // 添加曲线
		{
			if( (m_bSameCoord || m_bLogCoord) && m_arLoging.GetSize() > 0 ) // 同一坐标
			{
				prop.m_dCoordMin = m_arLoging[0].m_dCoordMin;
				prop.m_dCoordMax = m_arLoging[0].m_dCoordMax;					
			}
			
			if( m_bLogCoord ) // 对数坐标
			{
				if( prop.m_dMinVal < 0.0
					|| prop.m_dMaxVal < 0.0)
					m_bLogCoord = FALSE;
			}
			
			prop.m_pParentLane = this;
			m_arLoging.Add(prop);
			
			if( pView )
			{
				pView->Invalidate(FALSE);
				pView->GetDocument()->SetModifiedFlag();
			}
			
			return TRUE;
		}
		
		BOOL SetLogCurveTitle(int iIndex, const CString &strTitle)// 设置曲线标题 index指的是第几条曲线
		{
			if(iIndex > m_arLoging.GetSize() )
				return FALSE;
			
			if( iIndex == m_arLoging.GetSize() )
			{
				strLOGINGPROP prop;
				prop.m_pParentLane = this;
				m_arLoging.Add(prop);
			}
			
			m_arLoging[iIndex].m_strTitle = strTitle;
			
			return TRUE;
		}

		BOOL SetLogName(int iIndex, const CString &strName)// 设置曲线名字 index指的是第几条曲线
		{
			if(iIndex > m_arLoging.GetSize() )
				return FALSE;
			
			if( iIndex == m_arLoging.GetSize() )
			{
				strLOGINGPROP prop;
				prop.m_pParentLane = this;
				m_arLoging.Add(prop);
			}
			
			m_arLoging[iIndex].m_strLogName = strName;
			
			return TRUE;
		}
		
		void OnEditProperties( CView *pView );	// 编辑属性
		
	private:
		
		void DrawLaneFill(CDC *pDC, CView *pView);		// 画道的填充
		void DrawGrid(CDC *pDC, CView *pView);			// 画网格
		void DrawBorber(CDC *pDC, CView *pView);		// 画边框
		
		void DrawLogCurve(CDC *pDC, CView *pView);		// 画曲线
		void DrawLogCurveFill(CDC *pDC, CView *pView);	// 画曲线的填充

		void DrawDepth(CDC *pDC, CView *pView);			// 井深
};




	
//////////////////////////////////////////////////////////////////////////
class CLogCaption // 测井曲线图标题
{
public:
	CString m_strTitle; // 标题
	LOGFONT m_lfFont;   // 字体
	COLORREF m_clrFont; // 字体颜色
	CRect	m_rectCaption;		// 标题大小范围
	CLogChart *m_pchartLog;
public:
	CLogCaption() // 构造函数
	{
		m_strTitle = _T("测井曲线图");
		m_clrFont = RGB(0,0,0);
		CFont font;
		font.CreateFont(60, 0, 0, 0, 0, 0, 0, 0, GB2312_CHARSET, 0,0,0,0,"仿宋_GB2312");
		font.GetLogFont(&m_lfFont);
	}
	
	void DrawCaption(CDC *pDC, CView *pView); // 画标题
	
	void Serialize(CArchive &ar) // 序列化
	{
		if( ar.IsStoring() )
		{
			ar << m_strTitle;
			ar << m_clrFont;
			ar << m_rectCaption;
			ar.Write(&m_lfFont, sizeof(LOGFONT));
		}
		else
		{
			ar >> m_strTitle;
			ar >> m_clrFont;
			ar >> m_rectCaption;
			ar.Read(&m_lfFont, sizeof(LOGFONT));
		}
	}
	
};


union TOP_INT {
	unsigned char buf[2];
	short val;
};
union TOP_FLOAT {
	unsigned char buf[4];
	float val;
};

int GetInt(CFile &file, BOOL bIsBigEndian = TRUE);
float GetFloat(CFile &file, BOOL bIsBigEndian = TRUE);

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
class CLogChart  // 测井曲线图
{
public:
	
public:
	// 图素
	CLogCaption		m_chartCaption;		// 图标题头
	CArray<DWORD, DWORD> m_chartLanes;	// 泳道
	
	// 数据
	CLogDats		m_logDat;				// 曲线数据
	CArray<double, double> m_arInterLayer;	// 夹层标记
	
	// 属性
	double			m_dMinDepth;	// 显示井段的顶深
	double			m_dMaxDepth;	// 显示井段的底深
	
	CRect			m_rectLane;		// 整个曲线泳道大小
	CRect			m_rectTitle;	// 曲线标题泳道大小
	int 			m_iTitleHeight;	// 曲线标题高度
	int				m_iMargin;		// 边宽0.01cm
	
	double			m_dDispScale[11];	// 井深显示比例
	int				m_iCurDispScale;	// 当前显示比例索引
		
	BOOL			m_bDispCaption;		// 图标题是否显示
	
public:
	void ReCaleAllRect();
	CLogChart();
	~CLogChart();
	
	CSize GetChartSize()					// 图尺寸大小
	{
		int width = m_rectLane.Width();		
		
		int height = m_rectLane.bottom;
		if( height < 0 )
			height = 0;
		
		return CSize(width, height);
	}

	int GetLogDatIndex(const CString &strLogName);
	
	CRect GetChartTitleRect()  // 曲线标题框大小
	{
		CRect rect = m_rectTitle;
		CSize size = GetChartSize();
		rect.OffsetRect(m_iMargin - (rect.Width() - size.cx)/2,0);
		return rect;
	}
	
	CRect GetChartLaneRect()
	{
		CRect rect = m_rectLane;
		CSize size = GetChartSize();
		rect.OffsetRect(m_iMargin - (rect.Width() - size.cx)/2,0);
		return rect;
	}
	
	int GetCurLanePos(int iIndex)
	{
		int width = 0;
		
		for (int i=0;i<iIndex;i++)
		{
			CLogCurveLane *pLane = (CLogCurveLane *)m_chartLanes.GetAt(i);
			width += pLane->m_rectLane.Width();
		}
		
		return width;
	}
	
	CSize GetChartTitleSize()
	{
		CSize size = GetChartSize();
		size.cy = m_iTitleHeight + 50; // 曲线标题高度 + 50
		
		if( m_bDispCaption )		// 图标题是否显示
			size.cy += m_chartCaption.m_rectCaption.Height();
		
		return size;
	}
	
	//
	// 是否为716格式
	// 是否按实际曲线条数保存的
	BOOL Is716Format(LPCSTR name, BOOL &bIsBigEndian, BOOL &bCurves, long &lReadPos);
	// 判断文件是否为文本文件
	BOOL IsTextFile(LPCSTR lpstrFile);	
	// 判断文件是否为LAS文件
	BOOL IsLasFile(LPCSTR lpstrFile);
	// 判断文件是否为LIST文件
	BOOL IsListFile(LPCSTR lpstrFile);
	// 判断文件是否为ASC格式文件
	BOOL IsAscFile(LPCSTR lpstrFile);

	BOOL Load716File(const CString &fileName, BOOL bIsBigEndian, BOOL bCurves, long lReadPos);	
	BOOL GetLogDatTempFileName (LPTSTR pszFileName);

	// 装载测井数据
	void LoadLogDat(CView *pView);
	void LoadLogDat(CView *pView, const CString&strFileName);				
	BOOL LoadLogDatTxt(CView *pView, const CString &strFileName);
	void LoadLogLasFile(CView *pView, const CString &strFileName);
	void LoadLogListFile(CView *pView, const CString &strFileName);
	void LoadLogAscFile(CView *pView, const CString &strFileName);

	void Serialize(CArchive &ar)
	{
		if( ar.IsStoring() )
		{
			m_chartCaption.Serialize(ar);			// 图标题头
			
			m_logDat.Serialize(ar);				// 曲线数据		

			int n = m_chartLanes.GetSize();
			ar <<	n;
			int i;
			for ( i=0;i<n;i++)
			{
				CLogCurveLane *pLane = (CLogCurveLane *)m_chartLanes.GetAt(i);
				pLane->Serialize(ar);
			}
			
			ar <<	m_dMinDepth;	// 显示井段的顶深
			ar <<	m_dMaxDepth;	// 显示井段的底深
			
			ar <<	m_rectLane;		// 整个曲线泳道大小
			ar <<	m_rectTitle;	// 曲线标题泳道大小
			ar <<	m_iMargin;		// 边宽0.01cm
			ar <<	m_iTitleHeight;	// 曲线标题高度
			
			for ( i=0;i<11;i++)
				ar <<	m_dDispScale[i];	// 井深显示比例
			ar <<	m_iCurDispScale;	// 当前显示比例索引
			
			ar <<	m_bDispCaption;		// 图标题是否显示

			n = m_arInterLayer.GetSize();
			ar << n;
			for ( i=0;i<n;i++)
			{
				ar << m_arInterLayer[i];
			}
		}
		else
		{
			m_chartCaption.Serialize(ar);			// 图标题头			
			m_logDat.Serialize(ar);				// 曲线数据
			
			int n;
			int i;
			ar >>	n;
			for ( i=0;i<n;i++)
			{
				CLogCurveLane *pLane = new CLogCurveLane;
				pLane->Serialize(ar);
				
				pLane->SetParentChartLog(this);

				m_chartLanes.Add((DWORD)pLane);
				
			}
			
			ar >>	m_dMinDepth;	// 显示井段的顶深
			ar >>	m_dMaxDepth;	// 显示井段的底深
			
			ar >>	m_rectLane;		//  整个曲线泳道大小
			ar >>	m_rectTitle;	//	曲线标题泳道大小
			ar >>	m_iMargin;		//	边宽0.01cm
			ar >>	m_iTitleHeight;	// 曲线标题高度
			
			for ( i=0;i<11;i++)
				ar >>	m_dDispScale[i];	// 井深显示比例	
			
			ar >>	m_iCurDispScale;	// 当前显示比例索引
			
			ar >>	m_bDispCaption;		// 图标题是否显示

			ar >> n;
			for ( i=0;i<n;i++)
			{
				double tmp; 
				ar >> tmp;
				m_arInterLayer.Add(tmp);
			}
		}
	}
	
	void DrawFixedRow(CDC *pDC, CView *pView) // 画调整宽度的条
	{
		if( m_logDat.GetLogCount() < 1 )
			return;
		
		int nSaved = pDC->SaveDC();
		CRect rect = m_rectTitle;
		rect.top = 0;
		rect.bottom = 30;
		CSize size = GetChartSize();
		rect.OffsetRect(m_iMargin - (rect.Width() - size.cx)/2,0);
		CSize sz;
		int nXPos = pView->GetScrollPos(SB_HORZ);
		int nYPos = pView->GetScrollPos(SB_VERT);
		size.cx = nXPos;
		size.cy = nYPos;
		
		pDC->DPtoLP(&size);
		
		rect.top += size.cy;
		rect.bottom += size.cy;
		CRect rt = rect;
		rt.left = 0;
		rt.right = rt.left + GetChartSize().cx + m_iMargin*2;
		pDC->FillSolidRect(rt,::GetSysColor(COLOR_BTNFACE));
		int nCount = m_chartLanes.GetSize();
		rt = rect;
		rt.right = rt.left;
		CFont font;
		font.CreateFont(30,0,0,0,0,0,0,0,ANSI_CHARSET,0,0,0,0,"Arial");
		CFont *pOldFont = pDC->SelectObject(&font);
		CString tmp;
		pDC->SetBkMode(1);
		for (int i=0;i<nCount;i++)
		{
			CLogCurveLane *pLane = (CLogCurveLane *)m_chartLanes.GetAt(i);
			int width = pLane->m_rectLane.Width();
			rt.left = rt.right; 
			rt.right = rt.left + width;
			CRect rr = rt;
			pDC->LPtoDP(rr);
			rr.DeflateRect(1,0,0,0);
			pDC->DPtoLP(rr);
			pDC->DrawEdge(rr, EDGE_RAISED , BF_RECT);
			pDC->LPtoDP(rr);
			rr.DeflateRect(-1,0,0,0);
			pDC->DPtoLP(rr);
			tmp.Format("%d",i+1);
			pDC->DrawText(tmp,rr,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		}
		pDC->SelectObject(pOldFont);
		pDC->RestoreDC(nSaved);
	}
	
	void Draw(CDC *pDC, CView *pView)
	{				
		DrawLane(pDC, pView);
		
		if (!pDC->IsPrinting())
		{
			CSize size, sz;
			int nXPos = pView->GetScrollPos(SB_HORZ);
			int nYPos = pView->GetScrollPos(SB_VERT);
			size.cx = nXPos;
			size.cy = nYPos;
			
			pDC->DPtoLP(&size);
			sz = GetChartTitleSize();
			CRect rect(0, 30, sz.cx + m_iMargin + m_iMargin, sz.cy );
			rect.DeflateRect(0,0,0,-30);
			rect.top += size.cy;
			rect.bottom += size.cy;
			pDC->FillSolidRect(rect,RGB(255,255,255));
		}		
		
		if (!pDC->IsPrinting())
			DrawFixedRow(pDC, pView);
		
		if( m_bDispCaption )		// 图标题是否显示
			m_chartCaption.DrawCaption(pDC, pView);
		
		DrawTitle(pDC, pView);
	}

	const CString CreateTemplate();
	void	ApplyTemplate(const CString &strTemplate, CView* pView);
	double GetDepth(long yPos);					// 获取当前位置的井深
	double GetLogValue(int iItem, double dDepth);// 获取当前井深的测井数据
	double GetLogMinValue(int iItem,  double dFirstDepth, double dSecondDepth);
	double GetLogMaxValue(int iItem,  double dFirstDepth, double dSecondDepth);
	double GetLogAvgValue(int iItem,  double dFirstDepth, double dSecondDepth);
	double GetLogAreaValue(int iItem,  double dFirstDepth, double dSecondDepth);

	void DrawLane(CDC *pDC, CView *pView);		// 画泳道
	void DrawTitle(CDC *pDC, CView *pView);		// 画标题
	void Add(CLogCurveLane *pLane, CView *pView = NULL); // 后添加泳道
	void InsertOf(CLogCurveLane *pLane, CLogCurveLane *pNewLane, CView *pView/* = NULL*/); // 插入泳道
	void AppendTo(CLogCurveLane *pLane, CLogCurveLane *pNewLane, CView *pView/* = NULL*/); // 添加泳道
	
	void Del(CLogCurveLane *pLane, CView *pView = NULL); // 删除泳道
	void Del(int &iIndex, CView *pView = NULL); // 删除泳道
	void ChangeLaneWidth(const int& iIndex, const int &nWidth, CView *pView)	// 改变泳道宽度
	{
		CLogCurveLane *pLane = (CLogCurveLane *)m_chartLanes.GetAt(iIndex);
		pLane->m_rectLane.right += nWidth;
		if( pLane->m_rectLane.right < 4 )
			pLane->m_rectLane.right = 4;
		int nCount = m_chartLanes.GetSize();
		m_rectTitle.right = 0;
		m_rectLane.right = 0;
		for (int i=0;i<nCount; i++)
		{
			pLane = (CLogCurveLane *)m_chartLanes.GetAt(i);
			m_rectTitle.right += pLane->m_rectLane.Width();
			m_rectLane.right += pLane->m_rectLane.Width();
		}			
	}

	//////////////////////////////////////////////////////////////////////////
	void CutInterLayer();
	void CopyInterLayer();
	void PasteInterLayer();
};

#endif