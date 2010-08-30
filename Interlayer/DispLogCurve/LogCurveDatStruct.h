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
#define HINT_UPDATE_LOADLOG		2	// װ�ز⾮����		
typedef std::vector<double> CDoubleArray;
typedef std::map<std::string, int> name_list;


//////////////////////////////////////////////////////////////////////////

class CLogDats // ��������
{
public:
	CLogDats() { m_pLogDats = NULL;};
	~CLogDats() { RemoveAll(); };
	
private:
	name_list m_strLogNames;							// �������Ƽ���Ӧ��������
	CDoubleArray *m_pLogDats;							// ��������
public:
	int		GetLogCount();								// ������
	int		GetSamples();								// ������		
	int		GetLogIndex(LPCSTR lpsName);				// ��������
	CString GetLogName(const int &iIndex);				// ��������
	CString	AddLogName(LPCSTR lpsName, int iIndex);		// ����������Ŀ		
	BOOL	Add(int nCount,...);						// ������������
	double	GetValue(LPCSTR lpname ,int iRow);			// ��ȡĳ���ߵĵڼ�������
	double	GetValue(int iCol ,int iRow);				// ��ȡĳ���ߵĵڼ�������
	double	GetMinValue(LPCSTR lpname);					// ��ȡ��Сֵ
	double	GetMaxValue(LPCSTR lpname);					// ��ȡ���ֵ
	double	GetMinDepth();								// ��С����
	double	GetMaxDepth();								// �����	
	double	GetDepth(int iRow);							// ��ȡ����
	void	RemoveAll();								// �ͷ��ڴ�
	void	Serialize(CArchive& ar);					// ���л�
	
public:
	void	Add(double dVal,int iIndex)					// ����ĳ��������
	{
		int size = m_strLogNames.size(); // ������������
		if( size < 1 )					// û��
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
	
// ��������ʾ
#define GL_NONE                0L      // ����ʾ
#define GL_HORZ                1L      // ˮƽ��
#define GL_VERT                2L      // ��ֱ��
#define GL_BOTH                3L      // ʮ����
	
class CLogChart;
class CLogCurveLane    // �⾮����Ӿ��
{
public:
	enum LANETYPE
	{
		DEPTH = 0,				// ����
			LOGCURVE,				// �⾮����
			INTERLAYER,				// �в��־
	};
	
	struct strLOGINGPROP  //�⾮��������
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
			m_iFillStyle = 6; //��ʵ�����
			
			m_bSelected = FALSE;
			m_pParentLane = NULL;
		}
	public:
		//////////////////////////////////////////////////////////////////////////
		// �����
		CString		m_strTitle;			// �������Ƽӵ�λ ������ʾ
		CString		m_strLogName;		// �⾮��������
		COLORREF	m_clrLine;			// ������ɫ
		int			m_iLineStyle;		// ������
		int			m_iLineWidth;		// ���߿��
		BOOL		m_bFill;			// �Ƿ����
		COLORREF	m_clrFill;			// �����ɫ
		int			m_iFillStyle;		// �������			
		
		double		m_dCoordMin;		// ������Сֵ
		double		m_dCoordMax;		// �������ֵ
		int			m_iPart;			// ��ע�ȷ�
		int			m_iMarkPart;		// �̶ȵȷ�
		BOOL		m_bDirect;			// ���귽��TRUE������
		double		m_dMinVal;			// ������Сֵ
		double		m_dMaxVal;			// �������ֵ
		
		CDoubleArray m_arLogDat;		// ��������
		CDoubleArray m_arDepth;			// �������ݶ�Ӧ�ľ���
		
		//////////////////////////////////////////////////////////////////////////
		// ���ñ����

		BOOL		m_bSelected;		// ѡ��״̬
		CLogCurveLane *m_pParentLane;	// ����Ӿ��
	public:
		
		strLOGINGPROP& operator=( const strLOGINGPROP& varSrc );
		void Serialize(CArchive& ar);
		void OnEditProperties( CView *pView );	// �༭����
		void Add(double dDepth, double dVal)    // �������
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
		
	public:	// ����
		// ��������
		CRect						m_rectLane;			// Ӿ����С��Χ
		LANETYPE					m_nLaneType;		// Ӿ�����
		
		//////////////////////////////////////////////////////////////////////////
		// ���ߵ�ר������
		BOOL						m_bLogCoord;		// �Ƿ�Ϊ����
		BOOL						m_bSameCoord;		// �Ƿ�Ϊͬһ����ϵ
		//////////////////////////////////////////////////////////////////////////
		
		BOOL						m_bBorder;			// �Ƿ���ʾ�߿�
		int							m_iGridLine;		// ������(GL_NONE,...)
		BOOL						m_bFill;			// �Ƿ����
		COLORREF					m_clrFill;			// �����ɫ
		CString						m_strTitle;			// ����
		
		// �Ǳ�������

		// ����Ǳߵ���������Ի���ͽ����Ӿ��ֻ����һ�����ݼ�
		// ��������ж���⾮�������ݼ�
		CArray<strLOGINGPROP,strLOGINGPROP&>	m_arLoging;	// �������Ĳ⾮����
		
		BOOL							m_bSelected;		// ѡ��
	private:
		CLogChart						*m_pChartLog;		// �⾮����ͼ
		CRect							m_rtLane;			// ��ʱ�ķ�Χ
		
	public:	// ����

		CLogChart* GetLogChart() {return m_pChartLog;};
		void PasteDataFromClipboard(long lParam = 0);
		void CopyDataToClipboard(long lParam = 0);
		int GetSamples(int iIndex ) { return m_arLoging[iIndex].m_arLogDat.size(); }
		CLogCurveLane& operator=( const CLogCurveLane& varSrc );			
		void Serialize(CArchive& ar);
		void Draw(CDC *pDC, CView *pView, int iIndex);
		void DrawInterLayer(CDC *pDC, CView *pView);			// �в���
		void DrawTitle(CDC *pDC, CView *pView, int iIndex);			// ������
		void SetParentChartLog(CLogChart *pChartLog) { m_pChartLog = pChartLog; }
		int  GetCurveCount() { return m_arLoging.GetSize(); }
		BOOL Add(int iIndex, double dDepth, double dVal) // ������� indexָ���ǵڼ�������
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
		
		void DelCurve(strLOGINGPROP *pProp, CView *pView = NULL ) // ɾ������
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

		void DelCurve(const int& iIndex, CView *pView = NULL ) // ɾ������
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
		
		BOOL AddCurve(strLOGINGPROP &prop, CView *pView = NULL) // �������
		{
			if( (m_bSameCoord || m_bLogCoord) && m_arLoging.GetSize() > 0 ) // ͬһ����
			{
				prop.m_dCoordMin = m_arLoging[0].m_dCoordMin;
				prop.m_dCoordMax = m_arLoging[0].m_dCoordMax;					
			}
			
			if( m_bLogCoord ) // ��������
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
		
		BOOL SetLogCurveTitle(int iIndex, const CString &strTitle)// �������߱��� indexָ���ǵڼ�������
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

		BOOL SetLogName(int iIndex, const CString &strName)// ������������ indexָ���ǵڼ�������
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
		
		void OnEditProperties( CView *pView );	// �༭����
		
	private:
		
		void DrawLaneFill(CDC *pDC, CView *pView);		// ���������
		void DrawGrid(CDC *pDC, CView *pView);			// ������
		void DrawBorber(CDC *pDC, CView *pView);		// ���߿�
		
		void DrawLogCurve(CDC *pDC, CView *pView);		// ������
		void DrawLogCurveFill(CDC *pDC, CView *pView);	// �����ߵ����

		void DrawDepth(CDC *pDC, CView *pView);			// ����
};




	
//////////////////////////////////////////////////////////////////////////
class CLogCaption // �⾮����ͼ����
{
public:
	CString m_strTitle; // ����
	LOGFONT m_lfFont;   // ����
	COLORREF m_clrFont; // ������ɫ
	CRect	m_rectCaption;		// �����С��Χ
	CLogChart *m_pchartLog;
public:
	CLogCaption() // ���캯��
	{
		m_strTitle = _T("�⾮����ͼ");
		m_clrFont = RGB(0,0,0);
		CFont font;
		font.CreateFont(60, 0, 0, 0, 0, 0, 0, 0, GB2312_CHARSET, 0,0,0,0,"����_GB2312");
		font.GetLogFont(&m_lfFont);
	}
	
	void DrawCaption(CDC *pDC, CView *pView); // ������
	
	void Serialize(CArchive &ar) // ���л�
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
class CLogChart  // �⾮����ͼ
{
public:
	
public:
	// ͼ��
	CLogCaption		m_chartCaption;		// ͼ����ͷ
	CArray<DWORD, DWORD> m_chartLanes;	// Ӿ��
	
	// ����
	CLogDats		m_logDat;				// ��������
	CArray<double, double> m_arInterLayer;	// �в���
	
	// ����
	double			m_dMinDepth;	// ��ʾ���εĶ���
	double			m_dMaxDepth;	// ��ʾ���εĵ���
	
	CRect			m_rectLane;		// ��������Ӿ����С
	CRect			m_rectTitle;	// ���߱���Ӿ����С
	int 			m_iTitleHeight;	// ���߱���߶�
	int				m_iMargin;		// �߿�0.01cm
	
	double			m_dDispScale[11];	// ������ʾ����
	int				m_iCurDispScale;	// ��ǰ��ʾ��������
		
	BOOL			m_bDispCaption;		// ͼ�����Ƿ���ʾ
	
public:
	void ReCaleAllRect();
	CLogChart();
	~CLogChart();
	
	CSize GetChartSize()					// ͼ�ߴ��С
	{
		int width = m_rectLane.Width();		
		
		int height = m_rectLane.bottom;
		if( height < 0 )
			height = 0;
		
		return CSize(width, height);
	}

	int GetLogDatIndex(const CString &strLogName);
	
	CRect GetChartTitleRect()  // ���߱�����С
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
		size.cy = m_iTitleHeight + 50; // ���߱���߶� + 50
		
		if( m_bDispCaption )		// ͼ�����Ƿ���ʾ
			size.cy += m_chartCaption.m_rectCaption.Height();
		
		return size;
	}
	
	//
	// �Ƿ�Ϊ716��ʽ
	// �Ƿ�ʵ���������������
	BOOL Is716Format(LPCSTR name, BOOL &bIsBigEndian, BOOL &bCurves, long &lReadPos);
	// �ж��ļ��Ƿ�Ϊ�ı��ļ�
	BOOL IsTextFile(LPCSTR lpstrFile);	
	// �ж��ļ��Ƿ�ΪLAS�ļ�
	BOOL IsLasFile(LPCSTR lpstrFile);
	// �ж��ļ��Ƿ�ΪLIST�ļ�
	BOOL IsListFile(LPCSTR lpstrFile);
	// �ж��ļ��Ƿ�ΪASC��ʽ�ļ�
	BOOL IsAscFile(LPCSTR lpstrFile);

	BOOL Load716File(const CString &fileName, BOOL bIsBigEndian, BOOL bCurves, long lReadPos);	
	BOOL GetLogDatTempFileName (LPTSTR pszFileName);

	// װ�ز⾮����
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
			m_chartCaption.Serialize(ar);			// ͼ����ͷ
			
			m_logDat.Serialize(ar);				// ��������		

			int n = m_chartLanes.GetSize();
			ar <<	n;
			int i;
			for ( i=0;i<n;i++)
			{
				CLogCurveLane *pLane = (CLogCurveLane *)m_chartLanes.GetAt(i);
				pLane->Serialize(ar);
			}
			
			ar <<	m_dMinDepth;	// ��ʾ���εĶ���
			ar <<	m_dMaxDepth;	// ��ʾ���εĵ���
			
			ar <<	m_rectLane;		// ��������Ӿ����С
			ar <<	m_rectTitle;	// ���߱���Ӿ����С
			ar <<	m_iMargin;		// �߿�0.01cm
			ar <<	m_iTitleHeight;	// ���߱���߶�
			
			for ( i=0;i<11;i++)
				ar <<	m_dDispScale[i];	// ������ʾ����
			ar <<	m_iCurDispScale;	// ��ǰ��ʾ��������
			
			ar <<	m_bDispCaption;		// ͼ�����Ƿ���ʾ

			n = m_arInterLayer.GetSize();
			ar << n;
			for ( i=0;i<n;i++)
			{
				ar << m_arInterLayer[i];
			}
		}
		else
		{
			m_chartCaption.Serialize(ar);			// ͼ����ͷ			
			m_logDat.Serialize(ar);				// ��������
			
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
			
			ar >>	m_dMinDepth;	// ��ʾ���εĶ���
			ar >>	m_dMaxDepth;	// ��ʾ���εĵ���
			
			ar >>	m_rectLane;		//  ��������Ӿ����С
			ar >>	m_rectTitle;	//	���߱���Ӿ����С
			ar >>	m_iMargin;		//	�߿�0.01cm
			ar >>	m_iTitleHeight;	// ���߱���߶�
			
			for ( i=0;i<11;i++)
				ar >>	m_dDispScale[i];	// ������ʾ����	
			
			ar >>	m_iCurDispScale;	// ��ǰ��ʾ��������
			
			ar >>	m_bDispCaption;		// ͼ�����Ƿ���ʾ

			ar >> n;
			for ( i=0;i<n;i++)
			{
				double tmp; 
				ar >> tmp;
				m_arInterLayer.Add(tmp);
			}
		}
	}
	
	void DrawFixedRow(CDC *pDC, CView *pView) // ��������ȵ���
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
		
		if( m_bDispCaption )		// ͼ�����Ƿ���ʾ
			m_chartCaption.DrawCaption(pDC, pView);
		
		DrawTitle(pDC, pView);
	}

	const CString CreateTemplate();
	void	ApplyTemplate(const CString &strTemplate, CView* pView);
	double GetDepth(long yPos);					// ��ȡ��ǰλ�õľ���
	double GetLogValue(int iItem, double dDepth);// ��ȡ��ǰ����Ĳ⾮����
	double GetLogMinValue(int iItem,  double dFirstDepth, double dSecondDepth);
	double GetLogMaxValue(int iItem,  double dFirstDepth, double dSecondDepth);
	double GetLogAvgValue(int iItem,  double dFirstDepth, double dSecondDepth);
	double GetLogAreaValue(int iItem,  double dFirstDepth, double dSecondDepth);

	void DrawLane(CDC *pDC, CView *pView);		// ��Ӿ��
	void DrawTitle(CDC *pDC, CView *pView);		// ������
	void Add(CLogCurveLane *pLane, CView *pView = NULL); // �����Ӿ��
	void InsertOf(CLogCurveLane *pLane, CLogCurveLane *pNewLane, CView *pView/* = NULL*/); // ����Ӿ��
	void AppendTo(CLogCurveLane *pLane, CLogCurveLane *pNewLane, CView *pView/* = NULL*/); // ���Ӿ��
	
	void Del(CLogCurveLane *pLane, CView *pView = NULL); // ɾ��Ӿ��
	void Del(int &iIndex, CView *pView = NULL); // ɾ��Ӿ��
	void ChangeLaneWidth(const int& iIndex, const int &nWidth, CView *pView)	// �ı�Ӿ�����
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