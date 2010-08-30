#include "stdafx.h"

#pragma warning (disable : 4786)

#include "LogCurveDatStruct.h"
#include "DlgLaneSetup.h"
#include "DlgCurveSetup.h"
#include "LogTemplSheet.h"
#include "DispLogCurveDoc.h"
#include "odbcinst.h"
#include <fstream>
#include "VisualCalc/VCalcParser.h"
#include "VisualCalc/VCalcParserException.h"
#include "../afc/src/SplitPath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern AFX_EXTENSION_MODULE DispLogCurveDLL;
extern CString GetLogName(const CString &strCurLogName);
extern CString GetLogComments(const CString &strCurLogName);

int nFormatClip_Curve = RegisterClipboardFormat(_T("DispLogCurve_测井曲线"));
//////////////////////////////////////////////////////////////////////////

int GetInt(CFile &file, BOOL bIsBigEndian)
{
	TOP_INT m_Int;
	if( bIsBigEndian )
	{
		file.Read(&m_Int.buf[1],1);
		file.Read(&m_Int.buf[0],1);	
	}
	else
	{
		file.Read(&m_Int.buf[0],1);
		file.Read(&m_Int.buf[1],1);	
	}
	
	return m_Int.val;
}

float GetFloat(CFile &file, BOOL bIsBigEndian)
{
	TOP_FLOAT m_Float;
	
	if( bIsBigEndian )
	{
		file.Read(&m_Float.buf[3],1);
		file.Read(&m_Float.buf[2],1);
		file.Read(&m_Float.buf[1],1);
		file.Read(&m_Float.buf[0],1);
	}
	else
	{
		file.Read(&m_Float.buf[0],1);
		file.Read(&m_Float.buf[1],1);
		file.Read(&m_Float.buf[2],1);
		file.Read(&m_Float.buf[3],1);
	}
	
	return m_Float.val;
}

int TokenString(LPCTSTR lpszString, 
				LPCTSTR lpszDelimiters,
				CStringArray& saTokens,
				int nMaxTokens = 0,
				BOOL bTrimToken = TRUE,
				BOOL bEnableEscapedChars = TRUE,
				BOOL bEnableDoubleQuote = FALSE,
				BOOL bReturnEmptyToken = FALSE)
{
	// Note:  normally tokens will be allowed to accumulate;
	//        If you do not want to accumulate, uncomment
	//        the following line

	//saTokens.RemoveAll();

	ASSERT(lpszString && (lpszString[0] != _T('\0')));
	if ((lpszString == NULL) || (lpszString[0] == _T('\0')))
		return 0;

	ASSERT(lpszDelimiters && (lpszDelimiters[0] != _T('\0')));
	if ((lpszDelimiters == NULL) || (lpszDelimiters[0] == _T('\0')))
		return 0;

	BOOL bEndedWithDelim = FALSE;
	const TCHAR * cp = lpszString;

	if (!bReturnEmptyToken)
		cp += _tcsspn(cp, lpszDelimiters);			// skip leading delimiters

	// loop to find all tokens

	while (*cp)
	{
		const TCHAR * pos = NULL;
		CString strToken = _T("");

		// skip leading whitespace;  normally this will not cause a problem,
		// since it is unlikely that you would want to return empty tokens
		// when whitespace is the delimiter
		while ((*cp == _T(' ')) || (*cp == _T('\t')))
			strToken += *cp++;

		BOOL bInQuote = FALSE;

		if (bEnableDoubleQuote && (*cp == _T('"')))
		{
			bInQuote = TRUE;
			++cp;							// skip " char
		}

		// loop to accumulate token
		for (pos = cp; *pos; pos++)
		{
			// check for end of token - either a quote or a delimiter
			if (bInQuote)
			{
				if (*pos == _T('"'))
					break;
			}
			else
			{
				if (_tcsspn(pos, lpszDelimiters) != 0)
					break;
			}

			if (bEnableEscapedChars && (*pos == _T('\\')))
			{
				pos++;						// skip '\'
				if (*pos == _T('\0'))
					break;
			}
			strToken += *pos;
		}
		cp = pos;

		// check if string ended with delimiter
		if (_tcsspn(cp, lpszDelimiters))
			bEndedWithDelim = TRUE;
		else
			bEndedWithDelim = FALSE;

		// skip ending delimiter/quote
		if (*cp)
			cp++;

		if (bInQuote)
		{
			// if quote ended with delimiter, skip it
			if (_tcsspn(cp, lpszDelimiters))
				cp++;
		}

		if (bTrimToken)
		{
			strToken.TrimLeft();
			strToken.TrimRight();
		}

		if ((!strToken.IsEmpty()) || bReturnEmptyToken)
		{
			// either token is not empty OR caller wants empty tokens

			if ((nMaxTokens > 0) && (saTokens.GetSize() >= nMaxTokens))
			{
				break;
			}

			saTokens.Add(strToken);
		}
	}

	// add empty token if caller wants it
	if (bReturnEmptyToken && bEndedWithDelim)
		saTokens.Add(_T(""));

	return saTokens.GetSize();
}

//////////////////////////////////////////////////////////////////////////
// CLogDats
//////////////////////////////////////////////////////////////////////////

int CLogDats::GetLogCount()     // 曲线数
{
	return m_strLogNames.size();
}

int CLogDats::GetSamples() // 数据数
{
	if( m_pLogDats == NULL )
		return 0;
	else
		return m_pLogDats[0].size();
	
}

int CLogDats::GetLogIndex(LPCSTR lpsName)  // 曲线索引
{
	typedef name_list::iterator nameIt ;
	nameIt It;
	if((It = m_strLogNames.find(lpsName)) != m_strLogNames.end())
		return (*It).second;
	else
		return -1;		
}

CString CLogDats::GetLogName(const int &iIndex)  // 曲线名称
{
	typedef name_list::iterator nameIt ;				
	
	nameIt start, end, it ;
	
	start = m_strLogNames.begin() ; 
	end = m_strLogNames.end() ; 
	
	CString tmp = "";
	for(it = start; it != end; it++)
	{
		
		if( iIndex == (*it).second )
		{
			tmp = (*it).first.c_str();
			break;
		}				
	}
	
	return tmp;
}

CString CLogDats::AddLogName(LPCSTR lpsName, int iIndex) // 增加曲线顶目
{
	//m_strLogNames[lpsName] = iIndex;
	name_list::iterator it = m_strLogNames.find(lpsName);
	if(it == m_strLogNames.end())
	{
		//没找到
		m_strLogNames[lpsName] = iIndex;
		return _T("");
	}
	else 
	{
		//找到
		CString str;
		int i=1;
		while (it != m_strLogNames.end())
		{
			str.Format (_T("%s_%d"),lpsName, i);
			it = m_strLogNames.find((LPCSTR)str);
			i++;
		}
		
		m_strLogNames[(LPCSTR)str] = iIndex;
		
		return str;
	}
}

BOOL CLogDats::Add(int nCount,...)  // 增加曲线数据
{
	int size = m_strLogNames.size(); // 曲线数据列数
	if( nCount != size )					// 曲线列不相等
		return FALSE;
	
	if( m_pLogDats == NULL )
		m_pLogDats = new CDoubleArray[size];
	
	int count = 0;
	double val;
	
	va_list marker;
	
	va_start( marker, nCount );
	while( count < nCount  )
	{				
		val = va_arg( marker, double);
		
		m_pLogDats[count].push_back(val);
		
		count++;
	}
	va_end( marker );
	
	return TRUE;
}

double CLogDats::GetValue(LPCSTR lpname ,int iRow) // 获取某曲线的第几个数据
{
	if( m_pLogDats == NULL )
		return -99999.0;
	
	long index;
	
	name_list::iterator theIterator;
	
	theIterator = m_strLogNames.find(lpname);
	
	if(theIterator != m_strLogNames.end() )
		index = (*theIterator).second;
	else 
		index = 0;

	int n = m_pLogDats[index].size();
	if( n <= iRow )
		return -99999.0;
	
	return m_pLogDats[index][iRow];
}

double CLogDats::GetValue(int iCol ,int iRow) // 获取某曲线的第几个数据
{
	if( m_pLogDats == NULL )
		return -99999.0;
	
	int n = m_pLogDats[iCol].size();
	if( n <= iRow )
		return -99999.0;
	
	return m_pLogDats[iCol][iRow];
}

double CLogDats::GetMinValue(LPCSTR lpname) // 获取最小值
{
	if( m_pLogDats == NULL )
		return -99999.0;
	
	long index;

	name_list::iterator theIterator;

	theIterator = m_strLogNames.find(lpname);
	
	if(theIterator != m_strLogNames.end() )
		index = (*theIterator).second;
	else 
		index = 0;	
	
	int n = m_pLogDats[index].size();
	
	if( n < 1 )
		return -99999.0;
	
	double tmp = m_pLogDats[index][0];
	
	for (int i=1; i<n;i++)
	{
		if( tmp > m_pLogDats[index][i])
			tmp = m_pLogDats[index][i];
	}
	
	return tmp;
}

double CLogDats::GetMaxValue(LPCSTR lpname) // 获取最大值
{
	if( m_pLogDats == NULL )
		return -99999.0;
	
	long index;
	
	name_list::iterator theIterator;
	
	theIterator = m_strLogNames.find(lpname);
	
	if(theIterator != m_strLogNames.end() )
		index = (*theIterator).second;
	else 
		index = 0;

	int n = m_pLogDats[index].size();
	if( n < 1 )
		return -99999.0;
	
	double tmp = m_pLogDats[index][0];
	
	for (int i=1; i<n;i++)
	{
		if( tmp < m_pLogDats[index][i])
			tmp = m_pLogDats[index][i];
	}
	
	return tmp;
}		

double CLogDats::GetMinDepth() // 最小井深
{
	return GetMinValue(_T("DEPTH"));
}

double CLogDats::GetMaxDepth()  // 最大井深
{
	return GetMaxValue(_T("DEPTH"));
}

double	CLogDats::GetDepth(int iRow)
{
	return GetValue(_T("DEPTH") ,iRow);
}

void CLogDats::RemoveAll() // 释放内存
{
	if( m_pLogDats != NULL )
		delete []m_pLogDats;
	
	m_pLogDats = NULL;
	
	m_strLogNames.erase(m_strLogNames.begin(), m_strLogNames.end());
}

void CLogDats::Serialize(CArchive& ar) // 序列化
{
	if( ar.IsStoring() ) // 保存
	{
		// 保存曲线名称及其数据列索引
		int nCols = m_strLogNames.size() ;
		
		ar << nCols;
		
		typedef name_list::iterator nameIt ;				
		
		nameIt start, end, it ;
		
		start = m_strLogNames.begin() ; 
		end = m_strLogNames.end() ; 
		
		CString tmp;
		int index;
		for(it = start; it != end; it++)
		{
			tmp = (*it).first.c_str();
			index = (*it).second;
			ar << tmp;
			ar << index;
		}
		
		// 保存曲线数据
		
		int nRows = 0; // 数据行数
		
		if( m_pLogDats != NULL )
		{
			nRows = m_pLogDats[0].size();
			for (int i = 0; i<nCols ; i++ )
			{
				if( nRows <m_pLogDats[i].size())
					nRows = m_pLogDats[i].size();
			}
		}
		
		ar << nRows;
		double dTmp;
		for (int i = 0; i<nCols ; i++ )
		{
			for (int j=0;j<nRows;j++)
			{
				dTmp = m_pLogDats[i][j];
				ar << dTmp;
			}
		}
	}
	else // 读取数据
	{
		int nCols, nRows;
		ar >> nCols; // 曲线数
		
		CString tmp;
		int iIndex;
		int i;

		for ( i=0; i< nCols; i++ )
		{
			ar >> tmp;
			ar >> iIndex;
			
			m_strLogNames[(LPCSTR)tmp] = iIndex;
		}
		
		ar >> nRows; // 数据行数
		
		if( nRows > 0 )
			m_pLogDats = new CDoubleArray[nCols];
		
		double dVal;
		for (i = 0 ; i<nCols; i++)
		{
			for ( int j=0;j<nRows;j++)
			{
				ar >> dVal;
				Add(dVal, i);
			}
		}
	}
}


void CLogChart::CutInterLayer()
{
	CopyInterLayer();
	m_arInterLayer.RemoveAll();
}

void CLogChart::CopyInterLayer()
{
	CWaitCursor wait;

	CSharedFile file (GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT);

	CString source;				

	int nSamples = m_arInterLayer.GetSize();

	for (int i=0; i<nSamples; i++)
	{
		source.Format("%g\n",m_arInterLayer[i]);
		file.Write (source, source.GetLength());
	}

	if(OpenClipboard(NULL))
	{
		//char * buffer;
		EmptyClipboard();
		HGLOBAL clipbuffer = file.Detach();
		GlobalUnlock(clipbuffer);
		SetClipboardData(CF_TEXT,clipbuffer);
		CloseClipboard();
	}
}

void CLogChart::PasteInterLayer()
{

	double dDepth;		// 深度

	CString strText;
	char * buffer;
	if (OpenClipboard(NULL))
	{
		buffer = (char*)GetClipboardData(CF_TEXT);
		strText = buffer;
	}

	CloseClipboard();

	strText.LockBuffer();
	CString strLine = strText;

	// Find the end of the first line
	int nIndex;
	do
	{					
		nIndex = strLine.Find(_T("\n"));

		// Store the remaining chars after the newline
		CString strNext = (nIndex < 0)? _T("")  : strLine.Mid(nIndex + 1);

		// Remove all chars after the newline
		if (nIndex >= 0)
			strLine = strLine.Left(nIndex);

		int nLineIndex = strLine.FindOneOf(_T("\t,"));
		CString strCellText = (nLineIndex >= 0)? strLine.Left(nLineIndex) : strLine;

		int nColumn = 0;
		while (!strLine.IsEmpty() && nColumn < 1 )
		{
			strCellText.TrimLeft();
			strCellText.TrimRight();

			// 设置值

			switch( nColumn )
			{
			case 0:
				dDepth = atof(strCellText);
				break;	
			default:
				break;
			}

			strLine = (nLineIndex >= 0)? strLine.Mid(nLineIndex + 1) : _T("");
			nLineIndex = strLine.FindOneOf(_T("\t,"));
			strCellText = (nLineIndex >= 0)? strLine.Left(nLineIndex) : strLine;

			nColumn++;
		}

		m_arInterLayer.Add(dDepth);

		strLine = strNext;

	} while (nIndex >= 0);

	strText.UnlockBuffer();
}


//////////////////////////////////////////////////////////////////////////
// 测井曲线泳道
//////////////////////////////////////////////////////////////////////////

CLogCurveLane::strLOGINGPROP& CLogCurveLane::strLOGINGPROP::operator=( const CLogCurveLane::strLOGINGPROP& varSrc )
{
	m_strTitle		= varSrc.m_strTitle;		// 中文名称加单位 用于显示
	m_strLogName	= varSrc.m_strLogName;		// 测井曲线名字
	m_clrLine		= varSrc.m_clrLine;			// 曲线颜色
	m_iLineStyle	= varSrc.m_iLineStyle;		// 曲线型
	m_iLineWidth	= varSrc.m_iLineWidth;		// 曲线宽度
	m_bFill			= varSrc.m_bFill	;		// 是否填充
	m_clrFill		= varSrc.m_clrFill;			// 填充颜色
	m_iFillStyle	= varSrc.m_iFillStyle;		// 填充类型
	
	m_dCoordMin		= varSrc.m_dCoordMin;		// 坐标最小值
	m_dCoordMax		= varSrc.m_dCoordMax;		// 坐标最大值
	m_iPart			= varSrc.m_iPart	;		// 标注等分
	m_iMarkPart		= varSrc.m_iMarkPart;		// 刻度等分
	m_bDirect		= varSrc.m_bDirect  ;		// 坐标方向TRUE反方向
	m_dMinVal		= varSrc.m_dMinVal;			// 数据最小值
	m_dMaxVal		= varSrc.m_dMaxVal;			// 数据最大值
	m_arLogDat		= varSrc.m_arLogDat;		// 数据
	m_arDepth		= varSrc.m_arDepth;			// 井深
	m_pParentLane	= varSrc.m_pParentLane;		// 所属泳道
	
	return *this;
}

void CLogCurveLane::strLOGINGPROP::Serialize(CArchive& ar)
{
	if( ar.IsStoring() )
	{
		ar << m_strTitle;
		ar << m_strLogName;
		ar << m_clrLine;
		ar << m_iLineStyle;
		ar << m_iLineWidth;
		ar << m_bFill		;
		ar << m_clrFill;
		ar << m_iFillStyle;
		
		ar << m_dCoordMin	;
		ar << m_dCoordMax	;
		ar << m_iPart		;
		ar << m_iMarkPart	;
		ar << m_bDirect	;
		ar << m_dMinVal;
		ar << m_dMaxVal;
		
		size_t sz = m_arLogDat.size();
		ar << sz;
		for (int i=0;i<sz;i++)
		{
			ar << m_arDepth[i];
			ar << m_arLogDat[i];
		}
	}
	else
	{
		ar >> m_strTitle;
		ar >> m_strLogName;
		ar >> m_clrLine;
		ar >> m_iLineStyle;
		ar >> m_iLineWidth;
		ar >> m_bFill		;
		ar >> m_clrFill;
		ar >> m_iFillStyle;
		
		ar >> m_dCoordMin	;
		ar >> m_dCoordMax	;
		ar >> m_iPart		;
		ar >> m_iMarkPart	;
		ar >> m_bDirect	;
		ar >> m_dMinVal;
		ar >> m_dMaxVal;
		
		size_t sz ;
		ar >> sz;
		double tmp;
		for (int i=0;i<sz;i++)
		{
			ar >> tmp;
			m_arDepth.push_back(tmp);
			ar >> tmp;
			m_arLogDat.push_back(tmp);
		}
	}		
}
void CLogCurveLane::strLOGINGPROP::OnEditProperties( CView *pView )	// 编辑属性
{
	HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(DispLogCurveDLL.hModule);
	
	CDlgCurveSetup dlg;
	dlg.m_strCurveTitle = m_strTitle;
	dlg.m_clrLine		= m_clrLine;
	dlg.m_dCoordMin		= m_dCoordMin;
	dlg.m_dCoordMax		= m_dCoordMax;
	dlg.m_dMinVal		= m_dMinVal;
	dlg.m_dMaxVal		= m_dMaxVal;
	dlg.m_iLineStyle	= m_iLineStyle;
	dlg.m_uPenWidth		= m_iLineWidth;
	dlg.m_bFill			= m_bFill;
	dlg.m_clrFill		= m_clrFill;
	dlg.m_iFillStyle	= m_iFillStyle;
	dlg.m_iPart			= m_iPart ;
	dlg.m_iMarkPart		= m_iMarkPart;
	dlg.m_bDirect		= m_bDirect;
	dlg.m_bSameCoord	= m_pParentLane->m_bSameCoord;
	dlg.m_bLogCoord		= m_pParentLane->m_bLogCoord;

	if( dlg.DoModal() == IDOK )
	{
		m_strTitle		= dlg.m_strCurveTitle;
		m_clrLine		= dlg.m_clrLine;
		m_dCoordMin		= min(dlg.m_dCoordMin,dlg.m_dCoordMax);
		m_dCoordMax		= max(dlg.m_dCoordMin,dlg.m_dCoordMax);
		m_dMinVal		= dlg.m_dMinVal;
		m_dMaxVal		= dlg.m_dMaxVal;
		m_iLineStyle	= dlg.m_iLineStyle;
		m_iLineWidth	= dlg.m_uPenWidth;
		m_bFill			= dlg.m_bFill;
		m_clrFill		= dlg.m_clrFill;
		m_iFillStyle	= dlg.m_iFillStyle;
		m_iPart			= dlg.m_iPart ;
		m_iMarkPart		= dlg.m_iMarkPart;
		m_bDirect		= dlg.m_bDirect;
		
		m_pParentLane->m_bSameCoord	= dlg.m_bSameCoord;
		m_pParentLane->m_bLogCoord	= dlg.m_bLogCoord;
		
		if( m_pParentLane->m_bSameCoord || m_pParentLane->m_bLogCoord )
		{
			int n = m_pParentLane->m_arLoging.GetSize();
			for (int i=0;i<n;i++)
			{
				m_pParentLane->m_arLoging[i].m_dCoordMin = min(dlg.m_dCoordMin,dlg.m_dCoordMax);
				m_pParentLane->m_arLoging[i].m_dCoordMax = max(dlg.m_dCoordMin,dlg.m_dCoordMax);
				if( m_pParentLane->m_arLoging[i].m_dMinVal < 0 
					|| m_pParentLane->m_arLoging[i].m_dCoordMin < 0)
					
					m_pParentLane->m_bLogCoord = FALSE;
			}
		}
		
		CString strDepFormula = dlg.m_strDepthFormula;

		strDepFormula.TrimLeft();
		strDepFormula.TrimRight();
		strDepFormula.MakeUpper();

		if(! strDepFormula.IsEmpty() )
		{
			int nSamples = m_arDepth.size();

			for (int i=0;i<nSamples;i++)
			{
				CVCalcParser m_Parser;
				CString strVar, strDest;
				VARIANT valResult;
				try
				{					
					strVar.Format("DEPTH=%g",m_arDepth[i]);
					m_Parser.Evaluate((LPCTSTR)strVar);
					
					valResult = m_Parser.Evaluate((LPCTSTR)strDepFormula);			
				
					m_arDepth[i] = valResult.dblVal;
				}
				
				catch (CSyntaxException& ex) {
					strDest.Format("Syntax error %d : %s", ex.GetExceptionNumber(), ex.GetMessage().c_str());
					::MessageBox(NULL, strDest, _T("错误提示"),MB_OK|MB_ICONERROR);
					return;
				}
				catch (CMathematicException& ex) {
					strDest.Format("Mathematic error %d : %s", ex.GetExceptionNumber(), ex.GetMessage().c_str());
					::MessageBox(NULL, strDest, _T("错误提示"),MB_OK|MB_ICONERROR);
					return;
				}
				catch (CFunctionException& ex) {
					strDest.Format("Function error %d : %s", ex.GetExceptionNumber(), ex.GetMessage().c_str());
					::MessageBox(NULL, strDest, _T("错误提示"),MB_OK|MB_ICONERROR);
					return;
				}
				catch (CParameterException& ex) {
					strDest.Format("Parameter error %d : %s", ex.GetExceptionNumber(), ex.GetMessage().c_str());
					::MessageBox(NULL, strDest, _T("错误提示"),MB_OK|MB_ICONERROR);
					return;
				}
				catch (CVariableException& ex) {
					strDest.Format("Variable error %d : %s", ex.GetExceptionNumber(), ex.GetMessage().c_str());
					::MessageBox(NULL, strDest, _T("错误提示"),MB_OK|MB_ICONERROR);
					return;
				}
				catch (CDomainException& ex) {
					strDest.Format("Domain error %d : %s", ex.GetExceptionNumber(), ex.GetMessage().c_str());
					::MessageBox(NULL, strDest, _T("错误提示"),MB_OK|MB_ICONERROR);
					return;
				}
				catch (CParserException& ex) {
					strDest.Format("Parser error %d : %s", ex.GetExceptionNumber(), ex.GetMessage().c_str());
					::MessageBox(NULL, strDest, _T("错误提示"),MB_OK|MB_ICONERROR);
					return;
				}
				catch (...) {
					strDest.Format("Unknown parser internal error");
					::MessageBox(NULL, strDest, _T("错误提示"),MB_OK|MB_ICONERROR);
					return;
				}
			}
		}

		CString strVarFormula = dlg.m_strVarFormula;
		
		strVarFormula.TrimLeft();
		strVarFormula.TrimRight();
		strVarFormula.MakeUpper();
		
		if(! strVarFormula.IsEmpty() )
		{
			int nSamples = m_arLogDat.size();
			
			for (int i=0;i<nSamples;i++)
			{
				CVCalcParser m_Parser;
				CString strVar, strDest;
				VARIANT valResult;
				try
				{					
					strVar.Format("VAR=%g",m_arLogDat[i]);
					m_Parser.Evaluate((LPCTSTR)strVar);
					
					valResult = m_Parser.Evaluate((LPCTSTR)strVarFormula);			
					
					m_arLogDat[i] = valResult.dblVal;
				}
				
				catch (CSyntaxException& ex) {
					strDest.Format("Syntax error %d : %s", ex.GetExceptionNumber(), ex.GetMessage().c_str());
					::MessageBox(NULL, strDest, _T("错误提示"),MB_OK|MB_ICONERROR);
					return;
				}
				catch (CMathematicException& ex) {
					strDest.Format("Mathematic error %d : %s", ex.GetExceptionNumber(), ex.GetMessage().c_str());
					::MessageBox(NULL, strDest, _T("错误提示"),MB_OK|MB_ICONERROR);
					return;
				}
				catch (CFunctionException& ex) {
					strDest.Format("Function error %d : %s", ex.GetExceptionNumber(), ex.GetMessage().c_str());
					::MessageBox(NULL, strDest, _T("错误提示"),MB_OK|MB_ICONERROR);
					return;
				}
				catch (CParameterException& ex) {
					strDest.Format("Parameter error %d : %s", ex.GetExceptionNumber(), ex.GetMessage().c_str());
					::MessageBox(NULL, strDest, _T("错误提示"),MB_OK|MB_ICONERROR);
					return;
				}
				catch (CVariableException& ex) {
					strDest.Format("Variable error %d : %s", ex.GetExceptionNumber(), ex.GetMessage().c_str());
					::MessageBox(NULL, strDest, _T("错误提示"),MB_OK|MB_ICONERROR);
					return;
				}
				catch (CDomainException& ex) {
					strDest.Format("Domain error %d : %s", ex.GetExceptionNumber(), ex.GetMessage().c_str());
					::MessageBox(NULL, strDest, _T("错误提示"),MB_OK|MB_ICONERROR);
					return;
				}
				catch (CParserException& ex) {
					strDest.Format("Parser error %d : %s", ex.GetExceptionNumber(), ex.GetMessage().c_str());
					::MessageBox(NULL, strDest, _T("错误提示"),MB_OK|MB_ICONERROR);
					return;
				}
				catch (...) {
					strDest.Format("Unknown parser internal error");
					::MessageBox(NULL, strDest, _T("错误提示"),MB_OK|MB_ICONERROR);
					return;
				}
			}
		}
		
		pView->Invalidate(FALSE);

		pView->GetDocument()->SetModifiedFlag();
	}
	
	AfxSetResourceHandle(hInstOld);
}






//////////////////////////////////////////////////////////////////////////
// 图标题
//////////////////////////////////////////////////////////////////////////

void CLogCaption::DrawCaption(CDC *pDC, CView *pView) // 画标题
{
	int nSaved = pDC->SaveDC();
	CSize size = m_pchartLog->GetChartSize();
	CRect rect = m_rectCaption;
	rect.OffsetRect(m_pchartLog->m_iMargin - (rect.Width() - size.cx)/2,0);
	
	rect.DeflateRect(0,30,0,-30);
	
	if (!pDC->IsPrinting())
	{
		int nXPos = pView->GetScrollPos(SB_HORZ);
		int nYPos = pView->GetScrollPos(SB_VERT);
		size.cx = nXPos;
		size.cy = nYPos;
		
		pDC->DPtoLP(&size);			
		
		rect.top += size.cy;
		rect.bottom += size.cy;
	}
	
	CFont font;
	font.CreateFontIndirect(&m_lfFont);
	pDC->SetBkMode(1);
	CFont *pOldFont = pDC->SelectObject(&font);
	pDC->SetTextColor(m_clrFont);
	pDC->DrawText(m_strTitle, rect, DT_VCENTER | DT_CENTER |DT_SINGLELINE | DT_NOCLIP);
	pDC->SelectObject(pOldFont);
	pDC->RestoreDC(nSaved);
}



//////////////////////////////////////////////////////////////////////////
// 测井曲线泳道
//////////////////////////////////////////////////////////////////////////

CLogCurveLane::CLogCurveLane(LANETYPE	nLaneType, CLogChart *pChartLog, CRect rect)
:m_nLaneType(nLaneType), m_pChartLog(pChartLog), m_rectLane(rect)
{	
	switch(m_nLaneType)
	{
	case LOGCURVE:				//测井曲线
		break;
	case DEPTH:					//井深
		{
			m_strTitle = _T("井\r\n深\r\n(m)");
		}
		break;

	case INTERLAYER:				//夹层标记
		{
			m_strTitle = _T("夹\r\n层\r\n标\r\n记");
		}
		break;
	default:
		break;
	}
	
	m_bLogCoord	 = FALSE;			// 是否为对数
	m_bSameCoord = FALSE;			// 是否为同一坐标系
	
	m_bBorder	 = TRUE;			// 是否显示边框
	m_iGridLine	 = GL_BOTH;			// 网格线(GL_NONE,...)
	m_bFill		 = FALSE;			// 是否填充
	m_clrFill	 = RGB(255,255,255);// 填充颜色
	
	m_bSelected = FALSE;
}
	
CLogCurveLane::CLogCurveLane()
{	
	m_bLogCoord	 = FALSE;			// 是否为对数
	m_bSameCoord = FALSE;			// 是否为同一坐标系
	
	m_bBorder	 = TRUE;			// 是否显示边框
	m_iGridLine	 = GL_BOTH;			// 网格线(GL_NONE,...)
	m_bFill		 = FALSE;			// 是否填充
	m_clrFill	 = RGB(255,255,255);// 填充颜色
	
	m_bSelected = FALSE;
}

CLogCurveLane& CLogCurveLane::operator=( const CLogCurveLane& varSrc )
{
	m_strTitle		= varSrc.m_strTitle;
	m_rectLane		= varSrc.m_rectLane	;		// 泳道大小范围
	m_nLaneType		= varSrc.m_nLaneType	;		// 泳道类别
	m_bLogCoord		= varSrc.m_bLogCoord	;		// 是否为对数
	m_bSameCoord	= varSrc.m_bSameCoord;		// 是否为同一坐标系
	
	m_bBorder		= varSrc.m_bBorder	;			// 是否显示边框
	m_iGridLine		= varSrc.m_iGridLine	;		// 网格线(GL_NONE,...)
	m_bFill			= varSrc.m_bFill		;			// 是否填充
	m_clrFill		= varSrc.m_clrFill	;			// 填充颜色
	m_arLoging.Copy(varSrc.m_arLoging);
	m_pChartLog		= varSrc.m_pChartLog;				// 
	return *this;
}

void CLogCurveLane::Serialize(CArchive& ar)
{
	if( ar.IsStoring() )
	{
		ar << m_strTitle;
		WORD tmp = (WORD)m_nLaneType;
		ar << m_rectLane	;
		ar << tmp;
		ar << m_bLogCoord	;
		ar << m_bSameCoord;
		
		ar << m_bBorder	;
		ar << m_iGridLine;	
		ar << m_bFill	;	
		ar << m_clrFill	;

		int n = m_arLoging.GetSize();
		ar << n;
		for (int i=0;i<n;i++)
		{
			m_arLoging[i].Serialize(ar);
		}
	}
	else
	{
		ar >> m_strTitle;
		WORD tmp;
		ar >> m_rectLane	;
		ar >> tmp; m_nLaneType = (LANETYPE)tmp;
		ar >> m_bLogCoord	;
		ar >> m_bSameCoord;
		
		ar >> m_bBorder	;
		ar >> m_iGridLine;	
		ar >> m_bFill	;	
		ar >> m_clrFill	;

		int n;
		ar >> n;
		for (int i=0;i<n;i++)
		{
			CLogCurveLane::strLOGINGPROP prop;
			prop.Serialize(ar);
			prop.m_pParentLane = this;
			m_arLoging.Add(prop);
		}
	}
}

void CLogCurveLane::Draw(CDC *pDC, CView *pView, int iIndex)
{	
	int nSavedDC = pDC->SaveDC();
	m_rtLane = m_pChartLog->GetChartLaneRect();		
	m_rtLane.left += m_pChartLog->GetCurLanePos(iIndex);
	m_rtLane.right = m_rtLane.left + m_rectLane.Width();
	m_rtLane.DeflateRect(0,30,0,-30);
	
	DrawLaneFill(pDC, pView);		// 画道的填充
	
	if( m_nLaneType == LOGCURVE )	// 只画测井曲线道
		DrawGrid(pDC, pView);	    // 画网格
	
	if( m_bBorder )
		DrawBorber(pDC, pView);		// 画边框
	
	switch(m_nLaneType)
	{
	case DEPTH:						//井深
		DrawDepth(pDC, pView);
		break;
	case LOGCURVE:					//测井曲线
		DrawLogCurveFill(pDC, pView);
		DrawLogCurve(pDC, pView);
		break;
	case INTERLAYER:
		DrawInterLayer(pDC, pView);
		break;
	default:
		break;
	}
	pDC->RestoreDC(nSavedDC);

}

void CLogCurveLane::DrawInterLayer(CDC *pDC, CView *pView)			// 夹层标记
{
	int nCount = m_pChartLog->m_arInterLayer.GetSize();
	if( nCount < 1 )
		return;

	int nSavedDC = pDC->SaveDC();

	pDC->SetBkMode(1);

	CRect rectClip = m_rtLane;
	rectClip.NormalizeRect();
	pDC->IntersectClipRect(rectClip);
	CRect rect = m_rtLane;

	double dFirstDepth = m_pChartLog->m_dMinDepth;
	double dEndDepth = m_pChartLog->m_dMaxDepth;	


	double dDepth;
	CRect rt;
	CFont font;

	font.CreateFont(30, 0, 0, 0, 0, 0, 0, 0, ANSI_CHARSET, 0, 0, 0, 0, "Arial");

	CFont *pOldFont = pDC->SelectObject(&font);

	pDC->SetTextAlign(TA_CENTER|TA_TOP);
	pDC->SetTextColor(RGB(194,0,0));

	CPen pen(PS_SOLID,3,RGB(194,0,0));
	CPen *pOldPen = pDC->SelectObject(&pen);
	CBrush brush(RGB(194,0,0));
	CBrush *pOldBrush = pDC->SelectObject(&brush);

	CPoint pt[3];
	CString str;

	for (int i=0;i<nCount;i++)
	{		
		dDepth = m_pChartLog->m_arInterLayer[i];
		long y = rect.top  + (dDepth - dFirstDepth) * m_pChartLog->m_dDispScale[m_pChartLog->m_iCurDispScale];

		pDC->MoveTo(rect.left, y);
		pDC->LineTo(rect.right, y);

		pt[2].x = rect.right;
		pt[2].y = y;

		if( !pDC->IsPrinting()) 
		{
			pDC->LPtoDP(&pt[2]);
			pt[0].x = pt[2].x - 3;
			pt[0].y = pt[2].y - 3;

			pt[1].x = pt[2].x - 3;
			pt[1].y = pt[2].y + 3;

			pDC->DPtoLP(&pt[0]);
			pDC->DPtoLP(&pt[1]);
			pDC->DPtoLP(&pt[2]);
		}
		else
		{
			pt[0].x = pt[2].x - 15;
			pt[0].y = pt[2].y - 15;

			pt[1].x = pt[2].x - 15;
			pt[1].y = pt[2].y + 15;
		}

		pDC->Polygon(pt,3);
		
		pt[2].x = rect.left;
		pt[2].y = y;

		if( !pDC->IsPrinting()) 
		{
			pDC->LPtoDP(&pt[2]);
			pt[0].x = pt[2].x + 3;
			pt[0].y = pt[2].y - 3;

			pt[1].x = pt[2].x + 3;
			pt[1].y = pt[2].y + 3;

			pDC->DPtoLP(&pt[0]);
			pDC->DPtoLP(&pt[1]);
			pDC->DPtoLP(&pt[2]);
		}
		else
		{
			pt[0].x = pt[2].x + 15;
			pt[0].y = pt[2].y - 15;

			pt[1].x = pt[2].x + 15;
			pt[1].y = pt[2].y + 15;
		}

		pDC->Polygon(pt,3);

		str.Format("%.2lf",dDepth);
		pDC->TextOut(rect.left+rect.Width()/2, y-35,str);
	}

	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldFont);

	pDC->RestoreDC(nSavedDC);
}


void CLogCurveLane::DrawTitle(CDC *pDC, CView *pView, int iIndex)			// 画标题
{
	CRect rect = m_pChartLog->GetChartTitleRect();
	int nSavedDC = pDC->SaveDC();

	pDC->SetBkMode(1);

	int nXPos = pView->GetScrollPos(SB_HORZ);
	int nYPos = pView->GetScrollPos(SB_VERT);
	CSize size;
	size.cx = nXPos;
	size.cy = nYPos;
	
	pDC->DPtoLP(&size);
	
	if (!pDC->IsPrinting())
	{
		rect.top += size.cy;
		rect.bottom += size.cy;
	}
	
	rect.left += m_pChartLog->GetCurLanePos(iIndex);
	rect.right = rect.left + m_rectLane.Width();
	rect.DeflateRect(0,30,0,-30);	
	
	CPen pen( PS_SOLID | PS_JOIN_MITER | PS_ENDCAP_FLAT , 3 , RGB(0, 0, 0) );
	CPen *pOldPen = pDC->SelectObject(&pen);
	pDC->MoveTo(rect.left, rect.top);
	pDC->LineTo(rect.left, rect.bottom);
	pDC->LineTo(rect.right, rect.bottom);
	pDC->LineTo(rect.right, rect.top);
	pDC->SelectObject(pOldPen);	
	
	if( m_bSelected && !pDC->IsPrinting() )	
		pDC->FillSolidRect(rect,::GetSysColor(COLOR_HIGHLIGHT));
	
	CRect rectClip = rect;
	rectClip.NormalizeRect();
	pDC->IntersectClipRect(rectClip);
	
	if (m_nLaneType != LOGCURVE )
	{
		int height = rect.Height();
		CFont font;
		
		font.CreateFont(35, 0, 0, 0, 0, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, "宋体");
		CFont *pOldFont = pDC->SelectObject(&font);
		pDC->SetTextColor(RGB(0, 0, 0));
		CRect calcRect(0,0,0,0);
		pDC->DrawText(m_strTitle, calcRect, DT_CALCRECT );
		rect.DeflateRect(0,(height-calcRect.Height())/2,0,0);
		pDC->DrawText(m_strTitle, rect, DT_CENTER | DT_WORDBREAK );
		pDC->SelectObject(pOldFont);
	}
	else
	{
		CFont font;
		font.CreateFont(30, 0, 0, 0, 0, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, "宋体");
		CFont *pOldFont = pDC->SelectObject(&font);
		int nLogings = m_arLoging.GetSize(); //测井曲线总数
		int height;
		
		if( nLogings )
			height = (rect.Height()-10) / nLogings ;
		
		int i;
		
		CString tmp1, tmp2;
		CRect rt = rect;
		for (i = 0; i < nLogings; i++)
		{		
			
			tmp1.Format(" %g ", m_arLoging[i].m_dCoordMin);
			tmp2.Format(" %g ", m_arLoging[i].m_dCoordMax);
			
			CPen pen;
			LOGBRUSH logBrush;
			logBrush.lbStyle = BS_SOLID; 
			logBrush.lbColor = m_arLoging[i].m_clrLine; 

			switch(m_arLoging[i].m_iLineStyle)
			{
			case PS_SOLID:
				pen.CreatePen( PS_JOIN_MITER | PS_ENDCAP_FLAT | PS_SOLID, m_arLoging[i].m_iLineWidth, m_arLoging[i].m_clrLine);
				break;
			case PS_DASH: /* -------  */
				{
					DWORD dashsizes[4];
					
					dashsizes[0] = 2*m_arLoging[i].m_iLineWidth;
					dashsizes[1] = m_arLoging[i].m_iLineWidth;
					dashsizes[2] = 2*m_arLoging[i].m_iLineWidth;
					dashsizes[3] = m_arLoging[i].m_iLineWidth;
					
					pen.CreatePen(PS_GEOMETRIC | PS_JOIN_MITER | PS_ENDCAP_FLAT | PS_USERSTYLE , 
						m_arLoging[i].m_iLineWidth,
						&logBrush, 4, dashsizes);
				}
				break;
			case PS_DOT:/* .......  */
				{
					DWORD dashsizes[4];
					
					dashsizes[0] = m_arLoging[i].m_iLineWidth;
					dashsizes[1] = m_arLoging[i].m_iLineWidth;
					dashsizes[2] = m_arLoging[i].m_iLineWidth;
					dashsizes[3] = m_arLoging[i].m_iLineWidth;
					
					pen.CreatePen(PS_GEOMETRIC | PS_JOIN_MITER | PS_ENDCAP_FLAT | PS_USERSTYLE , 
						m_arLoging[i].m_iLineWidth,
						&logBrush, 4, dashsizes);
				}
				
				break;
			case PS_DASHDOT:/* _._._._  */
				{
					DWORD dashsizes[4];
					
					dashsizes[0] = 2*m_arLoging[i].m_iLineWidth;
					dashsizes[1] = m_arLoging[i].m_iLineWidth;
					dashsizes[2] = m_arLoging[i].m_iLineWidth;
					dashsizes[3] = m_arLoging[i].m_iLineWidth;
					
					pen.CreatePen(PS_GEOMETRIC | PS_JOIN_MITER | PS_ENDCAP_FLAT | PS_USERSTYLE , 
						m_arLoging[i].m_iLineWidth,
						&logBrush, 4, dashsizes);
				}
				break;
			case PS_DASHDOTDOT: /* _.._.._  */
				{
					DWORD dashsizes[6];
					
					dashsizes[0] = 2*m_arLoging[i].m_iLineWidth;
					dashsizes[1] = m_arLoging[i].m_iLineWidth;
					dashsizes[2] = m_arLoging[i].m_iLineWidth;
					dashsizes[3] = m_arLoging[i].m_iLineWidth;
					dashsizes[4] = m_arLoging[i].m_iLineWidth;
					dashsizes[5] = m_arLoging[i].m_iLineWidth;
					
					pen.CreatePen(PS_GEOMETRIC | PS_JOIN_MITER | PS_ENDCAP_FLAT | PS_USERSTYLE , 
						m_arLoging[i].m_iLineWidth,
						&logBrush, 6, dashsizes);
				}
				break;
			default:
				pen.CreatePen( PS_JOIN_MITER | PS_ENDCAP_FLAT | PS_SOLID, m_arLoging[i].m_iLineWidth, m_arLoging[i].m_clrLine);
				break;
			}
			
			CPen *pOldPen = pDC->SelectObject(&pen);
			
			pDC->SetTextColor(m_arLoging[i].m_clrLine);
			
			pDC->MoveTo(rect.left, rect.top+height*(i+1));
			pDC->LineTo(rect.right, rect.top+height*(i+1));
			
			rt.top = rect.top+height*i;
			rt.bottom = rect.top+height*(i+1);
			
			pDC->DrawText(m_arLoging[i].m_strTitle, rt, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
			
			if (m_arLoging[i].m_bDirect) //反向
			{
				pDC->DrawText(tmp2, rt, DT_BOTTOM | DT_LEFT | DT_SINGLELINE);
				pDC->DrawText(tmp1, rt, DT_BOTTOM | DT_RIGHT | DT_SINGLELINE);
			}
			else
			{
				pDC->DrawText(tmp1, rt, DT_BOTTOM | DT_LEFT | DT_SINGLELINE);
				pDC->DrawText(tmp2, rt, DT_BOTTOM | DT_RIGHT | DT_SINGLELINE);
			}
			
			pDC->SelectObject(pOldPen);
			
			if (m_arLoging[i].m_bSelected && !pDC->IsPrinting()) //画选中状态
			{
				LOGBRUSH LogBrush;
				
				CRect rc(0,0,1,1);
				pDC->DrawText(m_arLoging[i].m_strTitle, rc, DT_VCENTER | DT_CENTER | DT_SINGLELINE|DT_CALCRECT);
				LogBrush.lbColor = RGB(255,0,0);				
				LogBrush.lbStyle = PS_SOLID;
				CPen pen ;
				pen.CreatePen( PS_COSMETIC | PS_ALTERNATE , 1, &LogBrush, 0, NULL );
				CPoint pt = rt.CenterPoint();
				CSize size = rc.Size();
				rc.SetRect(pt.x - size.cx/2, pt.y-size.cy/2,pt.x + size.cx/2, pt.y+size.cy/2);
				rc.DeflateRect(-15,-15,-15,-15);
				CPen *pOldPen = pDC->SelectObject(&pen);
				pDC->MoveTo(rc.left, rc.top);
				pDC->LineTo(rc.left, rc.bottom);
				pDC->LineTo(rc.right,rc.bottom);
				pDC->LineTo(rc.right,rc.top);
				pDC->LineTo(rc.left,rc.top);
				pDC->SelectObject(pOldPen);	
			}
			
			rect.bottom -= 40;
		}
		pDC->SelectObject(pOldFont);
	}	
	
	
	pDC->RestoreDC(nSavedDC);
}

void CLogCurveLane::DrawLaneFill(CDC *pDC, CView *pView)		// 画道的填充
{
	if ( m_bFill )
	{
		int nSavedDC = pDC->SaveDC();
		CRect rect = m_rtLane;
		pDC->FillSolidRect(rect,m_clrFill);

		pDC->RestoreDC(nSavedDC);
	}		
}

void CLogCurveLane::DrawGrid(CDC *pDC, CView *pView)			// 画网格
{
	if(m_arLoging.GetSize() < 1 )
		return;
	
	if( m_iGridLine == GL_NONE )
		return;
	
	int nSavedDC = pDC->SaveDC();

	if( m_iGridLine & GL_HORZ ) // 横线
	{
		CRect rect = m_rtLane;
		

		CPen pen1;
		CPen pen2;
		CPen pen3;
		CPen pen4;

		if( !pDC->IsPrinting() )
		{
			pen1.CreatePen(PS_SOLID, 0, RGB(191, 191, 191));
			pen2.CreatePen(PS_SOLID, 0, RGB(230, 230, 230));
			pen3.CreatePen(PS_SOLID, 0, RGB(128, 128, 128));
			pen4.CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
		}
		else
		{
			pen1.CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
			pen2.CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
			pen3.CreatePen(PS_SOLID, 3, RGB(94, 94, 94));
			pen4.CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
		}
		
		CPen *pOldPen; 
		pOldPen = pDC->SelectObject(&pen2);
		
		double dFirstDepth = (long)(floor(m_pChartLog->m_dMinDepth));
		
		if (dFirstDepth > 0)
			dFirstDepth = (long)(dFirstDepth) / 5 * 5;
		else
			dFirstDepth = ((long)(dFirstDepth) / 5 + ((long)dFirstDepth%5?-1:0)) * 5;
		
		double dEndDepth = ceil(m_pChartLog->m_dMaxDepth);
		dEndDepth = ((long)(dEndDepth) / 5 + ((long)dEndDepth%5? 1:0)) * 5;
		
		double dDispMin = m_pChartLog->m_dMinDepth;	
		
		double tmp = dFirstDepth;
		
		long step = m_pChartLog->m_dDispScale[m_pChartLog->m_iCurDispScale] / 10.0;
		long first = rect.top  + (tmp - dDispMin) * m_pChartLog->m_dDispScale[m_pChartLog->m_iCurDispScale];
		CSize size(0, step);
		if( !pDC->IsPrinting() )
		{
			pDC->LPtoDP(&size);
			if (size.cy >= 3)
			{
				while (tmp < dEndDepth)
				{
					pDC->MoveTo(rect.right, first);
					pDC->LineTo(rect.left, first);
					tmp += 0.1;
					first += step;		
				}
			}
			
			tmp = dFirstDepth;
			step = m_pChartLog->m_dDispScale[m_pChartLog->m_iCurDispScale] / 5.0;
			first = rect.top  + (tmp - dDispMin) * m_pChartLog->m_dDispScale[m_pChartLog->m_iCurDispScale];
			size.cx = 0;
			size.cy = step;
			pDC->LPtoDP(&size);
			if (size.cy >= 3)
			{
				while (tmp < dEndDepth)
				{
					pDC->MoveTo(rect.right, first);
					pDC->LineTo(rect.left, first);
					tmp += 0.2;
					first += step;		
				}
			}
		}
		
		tmp = dFirstDepth;
		step = m_pChartLog->m_dDispScale[m_pChartLog->m_iCurDispScale];
		first = rect.top  + (tmp - dDispMin) * m_pChartLog->m_dDispScale[m_pChartLog->m_iCurDispScale];
		size.cx = 0;
		size.cy = step;
		pDC->LPtoDP(&size);
		while (tmp < dEndDepth)
		{
			//if (size.cy >= 3)
			{
				pOldPen = pDC->SelectObject(&pen1);
				pDC->MoveTo(rect.right, first);
				pDC->LineTo(rect.left, first);
				pDC->SelectObject(pOldPen);				
			}
			
			if ((long)tmp%5 == 0  && first > rect.top)
			{
				pOldPen = pDC->SelectObject(&pen3);
				pDC->MoveTo(rect.right, first);
				pDC->LineTo(rect.left, first);
				pDC->SelectObject(pOldPen);				
			}
			
			if ((long)tmp%10 == 0  && first > rect.top)
			{
				pOldPen = pDC->SelectObject(&pen4);
				pDC->MoveTo(rect.right, first);
				pDC->LineTo(rect.left, first);
				pDC->SelectObject(pOldPen);				
			}
			tmp += 1.0;
			first += step;		
		}
		
		pDC->SelectObject(pOldPen);
	}
	
	if( m_iGridLine & GL_VERT ) // 纵线
	{
		CRect rect = m_rtLane;
		
		int width = rect.Width(); //总宽度
		if(m_arLoging.GetSize()<1)
			return;
		CLogCurveLane::strLOGINGPROP LogingProp;
		LogingProp = m_arLoging[0];
		if (m_bLogCoord)
		{
			double vMin =  log10(LogingProp.m_dCoordMin); // 
			double vMax =  log10(LogingProp.m_dCoordMax);  // 
			double xStep = (double)width / (vMax - vMin); //
			if (LogingProp.m_bDirect) // 反向
			{
				double vV = ceil(vMax);
				double s = pow(10, vV);
				
				do
				{
					for (int nnn = 0; nnn < 9; nnn++)
					{
						double vStep = (pow(10, vV) - pow(10, vV - 1)) / 9.0;
						s -= vStep;
						if (s < LogingProp.m_dCoordMin)
							break;
						if (s > LogingProp.m_dCoordMax)
							continue;
						
						CPen pen;

						if( pDC->IsPrinting() )
							pen.CreatePen(PS_SOLID,0,RGB(0,0,0));
						else
							pen.CreatePen(PS_SOLID, 0, RGB(230, 230, 230));

						CPen *pOldPen = pDC->SelectObject(&pen);
						
						pDC->MoveTo(rect.left + (vMax - log10(s)) * xStep, rect.top);
						pDC->LineTo(rect.left + (vMax - log10(s)) * xStep, rect.bottom);
						
						pDC->SelectObject(pOldPen);
					}									
					
					vV = vV - 1.0;
					
					if (s < LogingProp.m_dCoordMin)
						break;
				}while (1);
				
				vV = ceil(vMax);
				vV -= 1.0;
				while (vV > vMin)
				{
					CPen pen;

					if( pDC->IsPrinting() )
						pen.CreatePen(PS_SOLID,0,RGB(0,0,0));
					else
						pen.CreatePen(PS_SOLID, 0, RGB(200, 200, 200));

					CPen *pOldPen = pDC->SelectObject(&pen);
					
					pDC->MoveTo(rect.left + (vMax - vV) * xStep, rect.top);
					pDC->LineTo(rect.left + (vMax - vV) * xStep, rect.bottom);
					
					pDC->SelectObject(pOldPen);
					
					vV = vV - 1.0;
				}
			}
			else
			{
				double vV = floor(vMin);
				double s = pow(10, vV);
				
				do
				{
					for (int nnn = 0; nnn < 9; nnn++)
					{
						double vStep = (pow(10, vV + 1) - pow(10, vV)) / 9.0;
						s += vStep;
						if (s < LogingProp.m_dCoordMin)
							continue;
						if (s > LogingProp.m_dCoordMax)
							break;
						
						CPen pen;
						if( pDC->IsPrinting() )
							pen.CreatePen(PS_SOLID,0,RGB(0,0,0));
						else
							pen.CreatePen(PS_SOLID, 0, RGB(230, 230, 230));

						CPen *pOldPen = pDC->SelectObject(&pen);						
						
						pDC->MoveTo(rect.left + (log10(s) - vMin) * xStep, rect.top);
						pDC->LineTo(rect.left + (log10(s) - vMin) * xStep, rect.bottom);
						
						pDC->SelectObject(pOldPen);
					}									
					
					vV = vV + 1.0;
					
					if (s > LogingProp.m_dCoordMax)
						break;
				}while (1);
				
				vV = floor(vMin);
				vV += 1.0;
				while (vV < vMax)
				{
					CPen pen;

					if( pDC->IsPrinting() )
						pen.CreatePen(PS_SOLID,0,RGB(0,0,0));
					else
						pen.CreatePen(PS_SOLID, 0, RGB(200, 200, 200));

					CPen *pOldPen = pDC->SelectObject(&pen);
					
					pDC->MoveTo(rect.left + (vV - vMin) * xStep, rect.top);
					pDC->LineTo(rect.left + (vV - vMin) * xStep, rect.bottom);
					
					pDC->SelectObject(pOldPen);
					
					vV = vV + 1.0;
				}
			}
		}
		else
		{
			double xStep;
			
			xStep = (double) width  / (LogingProp.m_iPart * LogingProp.m_iMarkPart);
			
			CPen pen1;
			CPen pen2;

			if( pDC->IsPrinting() )
			{
				pen1.CreatePen(PS_SOLID,3,RGB(94,94,94));
				pen2.CreatePen(PS_SOLID,0,RGB(0,0,0));
			}
			else
			{
				pen1.CreatePen(PS_SOLID, 0, RGB(200, 200, 200));
				pen2.CreatePen(PS_SOLID, 0, RGB(230, 230, 230));
			}

			CPen *pOldPen;
			
			long xPos;
			int i;
			
			pOldPen = pDC->SelectObject(&pen2);
			for (i = 0; i < (LogingProp.m_iPart * LogingProp.m_iMarkPart); i++)
			{			
				xPos = rect.left + xStep * i + 0.5;
				pDC->MoveTo(xPos, rect.top);
				pDC->LineTo(xPos, rect.bottom);
			}
			
			xStep = (double) width / LogingProp.m_iPart;			
			
			pDC->SelectObject(&pen1);
			
			for (i = 0; i < LogingProp.m_iPart; i++)
			{			
				xPos = rect.left + xStep * i + 0.5;
				pDC->MoveTo(xPos, rect.top);
				pDC->LineTo(xPos, rect.bottom);
			}
			
			pDC->SelectObject(pOldPen);
		}
	}

	pDC->RestoreDC(nSavedDC);
}

void CLogCurveLane::DrawBorber(CDC *pDC, CView *pView)		// 画边框
{
	int nSavedDC = pDC->SaveDC();

	CRect rect = m_rtLane;
	
	CPen pen( PS_SOLID | PS_JOIN_MITER | PS_ENDCAP_FLAT , 3 , RGB(0, 0, 0) );
	CPen *pOldPen = pDC->SelectObject(&pen);
	pDC->MoveTo(rect.left, rect.top);
	pDC->LineTo(rect.left, rect.bottom);
	pDC->MoveTo(rect.right, rect.top);
	pDC->LineTo(rect.right, rect.bottom);
	pDC->SelectObject(pOldPen);	
	
	pDC->RestoreDC(nSavedDC);
}

void CLogCurveLane::DrawLogCurve(CDC *pDC, CView *pView)		// 画曲线
{
	int nSavedDC = pDC->SaveDC();	
	
	pDC->SetBkMode(1);

	BOOL bRM = FALSE; // 是微电极曲线道吗？

	CRect rectClip = m_rtLane;
	rectClip.NormalizeRect();
	pDC->IntersectClipRect(rectClip);
	CRect rect = m_rtLane;
	
	int width = rect.Width(); //总宽度
	int nLogings = m_arLoging.GetSize(); //测井曲线总数
	int i, j;
		
	long xPos, yPos;	
	
	double dFirstDepth = m_pChartLog->m_dMinDepth;
	double dEndDepth = m_pChartLog->m_dMaxDepth;
	
	CRect cliRect;
	pView->GetClientRect(cliRect);
	CRect rt=cliRect;
	pDC->DPtoLP(&cliRect);

	for (i = 0; i < nLogings; i++)
	{
		// 判断是否是微电极
		if( m_arLoging[i].m_strTitle.Find("微梯度") != -1 
			|| m_arLoging[i].m_strTitle.Find("微电位") != -1  
			|| m_arLoging[i].m_strTitle.Find("RMN") != -1 
			|| m_arLoging[i].m_strTitle.Find("RMG") != -1)		
			bRM = TRUE;
		else
			bRM = FALSE;

		CPen pen;
		LOGBRUSH logBrush;
		logBrush.lbStyle = BS_SOLID; 
		logBrush.lbColor = m_arLoging[i].m_clrLine;

		switch(m_arLoging[i].m_iLineStyle)
		{
		case PS_SOLID:
			pen.CreatePen( PS_JOIN_MITER | PS_ENDCAP_FLAT | PS_SOLID, m_arLoging[i].m_iLineWidth, m_arLoging[i].m_clrLine);
			break;
		case PS_DASH: /* -------  */
			{
				DWORD dashsizes[4];
				
				dashsizes[0] = 2*m_arLoging[i].m_iLineWidth;
				dashsizes[1] = m_arLoging[i].m_iLineWidth;
				dashsizes[2] = 2*m_arLoging[i].m_iLineWidth;
				dashsizes[3] = m_arLoging[i].m_iLineWidth;
				
				pen.CreatePen(PS_GEOMETRIC | PS_JOIN_MITER | PS_ENDCAP_FLAT | PS_USERSTYLE , 
					m_arLoging[i].m_iLineWidth,
					&logBrush, 4, dashsizes);
			}
			break;
		case PS_DOT:/* .......  */
			{
				DWORD dashsizes[4];

				dashsizes[0] = m_arLoging[i].m_iLineWidth;
				dashsizes[1] = m_arLoging[i].m_iLineWidth;
				dashsizes[2] = m_arLoging[i].m_iLineWidth;
				dashsizes[3] = m_arLoging[i].m_iLineWidth;

				pen.CreatePen(PS_GEOMETRIC | PS_JOIN_MITER | PS_ENDCAP_FLAT | PS_USERSTYLE , 
					m_arLoging[i].m_iLineWidth,
					&logBrush, 4, dashsizes);
			}

			break;
		case PS_DASHDOT:/* _._._._  */
			{
				DWORD dashsizes[4];
				
				dashsizes[0] = 2*m_arLoging[i].m_iLineWidth;
				dashsizes[1] = m_arLoging[i].m_iLineWidth;
				dashsizes[2] = m_arLoging[i].m_iLineWidth;
				dashsizes[3] = m_arLoging[i].m_iLineWidth;
				
				pen.CreatePen(PS_GEOMETRIC | PS_JOIN_MITER | PS_ENDCAP_FLAT | PS_USERSTYLE , 
					m_arLoging[i].m_iLineWidth,
					&logBrush, 4, dashsizes);
			}
			break;
		case PS_DASHDOTDOT: /* _.._.._  */
			{
				DWORD dashsizes[6];
				
				dashsizes[0] = 2*m_arLoging[i].m_iLineWidth;
				dashsizes[1] = m_arLoging[i].m_iLineWidth;
				dashsizes[2] = m_arLoging[i].m_iLineWidth;
				dashsizes[3] = m_arLoging[i].m_iLineWidth;
				dashsizes[4] = m_arLoging[i].m_iLineWidth;
				dashsizes[5] = m_arLoging[i].m_iLineWidth;
				
				pen.CreatePen(PS_GEOMETRIC | PS_JOIN_MITER | PS_ENDCAP_FLAT | PS_USERSTYLE , 
					m_arLoging[i].m_iLineWidth,
					&logBrush, 6, dashsizes);
			}
			break;
		default:
			pen.CreatePen( PS_JOIN_MITER | PS_ENDCAP_FLAT | PS_SOLID, m_arLoging[i].m_iLineWidth, m_arLoging[i].m_clrLine);
			break;
		}
		
		CPen *pOldPen = pDC->SelectObject(&pen);
		
		
		double vMaxValue;  // 曲线最大值
		double vMinValue;  // 曲线最大值
		
		vMaxValue = m_arLoging[i].m_dMaxVal;
		vMinValue = m_arLoging[i].m_dMinVal;

		double vMin;
		double vMax;
		
		if (m_bLogCoord)
		{
			vMin = log10(m_arLoging[i].m_dCoordMin); // 
			vMax = log10(m_arLoging[i].m_dCoordMax);  // 
		}
		else
		{
			vMin = m_arLoging[i].m_dCoordMin; // 
			vMax = m_arLoging[i].m_dCoordMax;  // 
		}

		double dSpace = vMax - vMin;
		double xStep = width / dSpace; //

		if( pDC->IsPrinting() )
		{
			while( vMin > vMinValue)
			{
				vMax = vMin;
				vMin = vMin - dSpace;
			}
		}
				

		int nSamples = GetSamples(i);
		int zj = 0;

		while(vMin < vMaxValue )
		{			
			BOOL bStart = FALSE;

			for (j = 0; j < nSamples; j++)
			{				
				double dDepth = m_arLoging[i].m_arDepth[j];
				
				if (dDepth < dFirstDepth)
					continue;
				if (dDepth > dEndDepth)
					break;
				
				double vVaule = m_arLoging[i].m_arLogDat[j];
				
				if ( vVaule < -9999 || vVaule > 9999 )
					continue;

				yPos = rect.top  + (dDepth - dFirstDepth) * m_pChartLog->m_dDispScale[m_pChartLog->m_iCurDispScale];
				
				if( !pDC->IsPrinting() )
				{
					if( yPos<=cliRect.top )
						continue;
					if( yPos>=cliRect.bottom)
						break;
				}
				
				if (m_bLogCoord)
				{
					if (m_arLoging[i].m_bDirect)
						xPos = (vMax - log10(vVaule)) * xStep + rect.left;//计算横向位置
					else
						xPos = (log10(vVaule) - vMin) * xStep + rect.left;//计算横向位置
				}
				else
				{
					if (m_arLoging[i].m_bDirect)
						xPos = (vMax - vVaule) * xStep + rect.left;//计算横向位置
					else
						xPos = (vVaule - vMin) * xStep + rect.left;//计算横向位置
				}
				
				if (!bStart)
					pDC->MoveTo(xPos, yPos);
				else
					pDC->LineTo(xPos, yPos);
				
				bStart = TRUE;
			}

			vMin = vMax;
			vMax = vMin + dSpace;

			zj ++;

			if( !pDC->IsPrinting() )
			{
				if( zj > 1 )
					break;
			}
		};
		
		pDC->SelectObject(pOldPen);
		pen.DeleteObject();
	}
	
	pDC->RestoreDC(nSavedDC);
}

void CLogCurveLane::DrawLogCurveFill(CDC *pDC, CView *pView)	// 画曲线的填充
{
	int nSavedDC = pDC->SaveDC();
	
	pDC->SetBkMode(1);

	CRect rectClip = m_rtLane;
	rectClip.NormalizeRect();
	pDC->IntersectClipRect(rectClip);
	CRect rect = m_rtLane;
	
	int width = rect.Width(); //总宽度
	int nLogings = m_arLoging.GetSize(); //测井曲线总数
	int i, j;
	
	long xPos, yPos;
	
	double dFirstDepth = m_pChartLog->m_dMinDepth;
	double dEndDepth = m_pChartLog->m_dMaxDepth;		
	
	for (i = 0; i < nLogings; i++)
	{
		
		if (!m_arLoging[i].m_bFill)
			continue;
		
		CArray <POINT, POINT> m_CurvePoints;
		POINT pt;
		
		BOOL bStart = FALSE;
		
		double vMin;
		double vMax;
		
		if (m_bLogCoord)
		{
			vMin = log10(m_arLoging[i].m_dCoordMin); // 
			vMax = log10(m_arLoging[i].m_dCoordMax);  // 
		}
		else
		{
			vMin = m_arLoging[i].m_dCoordMin; // 
			vMax = m_arLoging[i].m_dCoordMax;  // 
		}
		
		double xStep = width / (vMax - vMin); //
		int nSamples = GetSamples(i);
		for (j = 0; j < nSamples; j++)
		{
			double vVaule = m_arLoging[i].m_arLogDat[j];
			
			if ( vVaule < -9999  || vVaule > 9999 )
				continue;
			
			double dDepth = m_arLoging[i].m_arDepth[j];
			
			yPos = rect.top  + (dDepth - dFirstDepth) * m_pChartLog->m_dDispScale[m_pChartLog->m_iCurDispScale];
			
			if (m_bLogCoord)
			{
				if (m_arLoging[i].m_bDirect)
					xPos = (vMax - log10(vVaule)) * xStep + rect.left;//计算横向位置
				else
					xPos = (log10(vVaule) - vMin) * xStep + rect.left;//计算横向位置
			}
			else
			{
				if (m_arLoging[i].m_bDirect)
					xPos = (vMax - vVaule) * xStep + rect.left;//计算横向位置
				else
					xPos = (vVaule - vMin) * xStep + rect.left;//计算横向位置
			}
			
			if (!bStart)
			{	
				pt.x = rect.left;
				pt.y = yPos;
				m_CurvePoints.Add(pt);
				pt.x = xPos;
				pt.y = yPos;
				m_CurvePoints.Add(pt);
			}
			else
			{
				pt.x = xPos;
				pt.y = yPos;
				m_CurvePoints.Add(pt);
			}
			bStart = TRUE;
		}
		
		int nn = m_CurvePoints.GetSize();
		pt.x = rect.left;
		pt.y = m_CurvePoints[nn - 1].y;
		m_CurvePoints.Add(pt);
		
		CBrush brush;
		CPen pen(PS_NULL, 0, RGB(0, 0, 0));
		CPen *pOldPen = pDC->SelectObject(&pen);
		if (m_arLoging[i].m_iFillStyle == 6)
			brush.CreateSolidBrush(m_arLoging[i].m_clrFill);
		else
			brush.CreateHatchBrush(m_arLoging[i].m_iFillStyle, m_arLoging[i].m_clrFill);
		CBrush *pOldBrush = pDC->SelectObject(&brush);
		int oldmode = pDC->SetROP2(R2_MASKPEN);
		pDC->Polygon(m_CurvePoints.GetData(), m_CurvePoints.GetSize());
		pDC->SetROP2(oldmode);
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);		
		m_CurvePoints.RemoveAll();
	}
	
	pDC->RestoreDC(nSavedDC);
}

void CLogCurveLane::DrawDepth(CDC *pDC, CView *pView)			// 井深
{
	int nSavedDC = pDC->SaveDC();		
	CRect rectClip = m_rtLane;
	rectClip.NormalizeRect();
	pDC->IntersectClipRect(rectClip);
	CRect rect = m_rtLane;
	pDC->SetBkMode(1);

	CFont font;
	font.CreateFont(35, 0, 0, 0, 0, 0, 0, 0, ANSI_CHARSET, 0, 0, 0, 0, "Arial");
	CFont *pOldFont = pDC->SelectObject(&font);
	
	CPen pen(PS_SOLID, 0, RGB(0, 0, 0));
	CPen *pOldPen; 
	pOldPen = pDC->SelectObject(&pen);
	
	double dFirstDepth = (long)(floor(m_pChartLog->m_dMinDepth));
	
	if (dFirstDepth > 0)
		dFirstDepth = (long)(dFirstDepth) / 5 * 5;
	else
		dFirstDepth = ((long)(dFirstDepth) / 5 + ((long)dFirstDepth%5?-1:0)) * 5;
	
	double dEndDepth = ceil(m_pChartLog->m_dMaxDepth);
	dEndDepth = ((long)(dEndDepth) / 5 + ((long)dEndDepth%5? 1:0)) * 5;
	
	double dDispMin = m_pChartLog->m_dMinDepth;	
	
	double tmp = dFirstDepth;
	
	long step = m_pChartLog->m_dDispScale[m_pChartLog->m_iCurDispScale] / 10.0;
	long first = rect.top  + (tmp - dDispMin) * m_pChartLog->m_dDispScale[m_pChartLog->m_iCurDispScale];
	if( step >= 10 )
	{
		while (tmp < dEndDepth)
		{
			pDC->MoveTo(rect.right, first);
			pDC->LineTo(rect.right - 7, first);
			pDC->MoveTo(rect.left, first);
			pDC->LineTo(rect.left + 7, first);
			tmp += 0.1;
			first += step;		
		}
	}
	
	tmp = dFirstDepth;
	step = m_pChartLog->m_dDispScale[m_pChartLog->m_iCurDispScale] / 5.0;
	first = rect.top  + (tmp - dDispMin) * m_pChartLog->m_dDispScale[m_pChartLog->m_iCurDispScale];
	if( step >= 10 )
	{
		while (tmp < dEndDepth)
		{
			pDC->MoveTo(rect.right, first);
			pDC->LineTo(rect.right - 7, first);
			
			pDC->MoveTo(rect.left, first);
			pDC->LineTo(rect.left + 7, first);
			tmp += 0.2;
			first += step;		
		}
	}			
	
	tmp = dFirstDepth;
	step = m_pChartLog->m_dDispScale[m_pChartLog->m_iCurDispScale];
	first = rect.top  + (tmp - dDispMin) * m_pChartLog->m_dDispScale[m_pChartLog->m_iCurDispScale];
	
	while (tmp < dEndDepth)
	{
		if ((long)tmp%5 == 0  && first > rect.top)
		{
			pDC->MoveTo(rect.right, first);
			pDC->LineTo(rect.right - 15, first);
			
			pDC->MoveTo(rect.left, first);
			pDC->LineTo(rect.left + 15, first);
			CString strTmp;
			strTmp.Format("%.0f", tmp);
			CRect rc(rect.left, first - 1, rect.right, first + 1);
			pDC->DrawText(strTmp, rc, DT_VCENTER | DT_CENTER | DT_SINGLELINE | DT_NOCLIP);
		}
		
		if( step >= 10 )
		{
			pDC->MoveTo(rect.right, first);
			pDC->LineTo(rect.right - 10, first);
			pDC->MoveTo(rect.left, first);
			pDC->LineTo(rect.left + 10, first);
			if (step >= 100)
			{
				if( (long)tmp%5 )
				{
					CString strTmp;
					strTmp.Format("%d", (long)tmp%10);
					CRect rc(rect.left, first - 1, rect.right, first + 1);
					pDC->DrawText(strTmp, rc, DT_VCENTER | DT_CENTER | DT_SINGLELINE | DT_NOCLIP);
				}
				
			}
		}
		tmp += 1.0;
		first += step;		
	}
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldFont);
	
	pDC->RestoreDC(nSavedDC);
}

void CLogCurveLane::OnEditProperties( CView *pView )	// 编辑属性
{
	HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(DispLogCurveDLL.hModule);
	
	CDlgLaneSetup dlg;
	dlg.m_bFill = m_bFill;
	dlg.m_clrFill = m_clrFill;
	dlg.m_bBorder = m_bBorder;
	
	if( m_iGridLine & GL_HORZ ) // 横线
		dlg.m_bHorzLine = TRUE;
	else
		dlg.m_bHorzLine = FALSE;
	if( m_iGridLine & GL_VERT ) // 纵线
		dlg.m_bVertLine = TRUE;
	else
		dlg.m_bVertLine = FALSE;
	
	dlg.m_dWidth = m_rectLane.Width()/100.0;
	
	dlg.m_strTitle = m_strTitle;

	if( dlg.DoModal() == IDOK )
	{
		if( dlg.m_bApplyAll ) // 如果应用于所有
		{
			int nCount = m_pChartLog->m_chartLanes.GetSize();
			for ( int i = 0; i<nCount;i++)
			{
				CLogCurveLane *pLane = (CLogCurveLane *)m_pChartLog->m_chartLanes[i];
				pLane->m_bFill = dlg.m_bFill;
				pLane->m_clrFill = dlg.m_clrFill;
				pLane->m_bBorder = dlg.m_bBorder;
				
				if( dlg.m_bHorzLine ) // 横线
					pLane->m_iGridLine |= GL_HORZ;
				else
					pLane->m_iGridLine &= ~GL_HORZ;
				if( dlg.m_bVertLine ) // 纵线
					pLane->m_iGridLine |= GL_VERT;
				else
					pLane->m_iGridLine &= ~GL_VERT;
			}
		}
		else
		{
			m_bFill = dlg.m_bFill;
			m_clrFill = dlg.m_clrFill;
			m_bBorder = dlg.m_bBorder;
			
			if( dlg.m_bHorzLine ) // 横线
				m_iGridLine |= GL_HORZ;
			else
				m_iGridLine &= ~GL_HORZ;
			if( dlg.m_bVertLine ) // 纵线
				m_iGridLine |= GL_VERT;
			else
				m_iGridLine &= ~GL_VERT;		
		}
		
		m_rectLane.right = dlg.m_dWidth * 100.0;
		
		m_strTitle = dlg.m_strTitle;

		m_pChartLog->ReCaleAllRect();

		
		pView->GetDocument()->UpdateAllViews(NULL, HINT_UPDATE_CHANGEDSIZE);
		
		pView->Invalidate(FALSE);

		pView->GetDocument()->SetModifiedFlag();
	}
	
	AfxSetResourceHandle(hInstOld);
}




//////////////////////////////////////////////////////////////////////////
// 测井曲线图
//////////////////////////////////////////////////////////////////////////

CLogChart::CLogChart()
{	
	m_dDispScale[0] = 5.0;  // 1:2000
	m_dDispScale[1] = 10.0; // 1:1000
	m_dDispScale[2] = 12.5; // 1:800
	m_dDispScale[3] = 20.0;  // 1:500
	m_dDispScale[4] = 50.0;  // 1:200
	m_dDispScale[5] = 100.0; // 1:100
	m_dDispScale[6] = 200.0;  // 1:50
	m_dDispScale[7] = 400.0;  // 1:25
	m_dDispScale[8] = 500.0;  // 1:20
	m_dDispScale[9] = 1000.0;  // 1:10
	m_dDispScale[10] = 2000.0;  // 1:5
	
	m_iCurDispScale = 4;
	
	m_iMargin = 100; // 1cm
	m_iTitleHeight = 400;// 4cm
	
	m_chartCaption.m_pchartLog = this;
	
	m_chartCaption.m_rectCaption = CRect(0,0,100,150);
	
	m_bDispCaption	= TRUE;	// 图标题是否显示
}

CLogChart::~CLogChart()
{
	int nCount = m_chartLanes.GetSize();
	
	for ( int i=0;i<nCount; i++)
	{
		CLogCurveLane *pLane = (CLogCurveLane *)m_chartLanes.GetAt(i);
		delete pLane;
	}
}

void CLogChart::DrawTitle(CDC *pDC, CView *pView)
{
	if( m_logDat.GetLogCount() < 1 )
		return;
	
	int nSavedDC = pDC->SaveDC();

	CRect rect = m_rectTitle;
	CSize size = GetChartSize();
	rect.OffsetRect(m_iMargin - (rect.Width() - size.cx)/2,0);
	
	int nXPos = pView->GetScrollPos(SB_HORZ);
	int nYPos = pView->GetScrollPos(SB_VERT);
	size.cx = nXPos;
	size.cy = nYPos;
	
	pDC->DPtoLP(&size);
	
	if (!pDC->IsPrinting())
	{
		rect.top += size.cy;
		rect.bottom += size.cy;
	}
	
	rect.DeflateRect(0,30,0,-30);
	
	int nCount = m_chartLanes.GetSize();
	
	for (int i=0;i<nCount;i++)
	{
		CLogCurveLane *pLane = (CLogCurveLane *)m_chartLanes.GetAt(i);
		pLane->DrawTitle(pDC, pView, i);
	}		
	
	CPen pen( PS_SOLID | PS_JOIN_MITER | PS_ENDCAP_FLAT , 5 , RGB(0, 0, 0) );
	CPen *pOldPen = pDC->SelectObject(&pen);
	pDC->MoveTo(rect.left, rect.top);
	pDC->LineTo(rect.left, rect.bottom);
	pDC->LineTo(rect.right,rect.bottom);
	pDC->LineTo(rect.right, rect.top);
	pDC->LineTo(rect.left, rect.top);
	pDC->SelectObject(pOldPen);	
	
	pDC->RestoreDC(nSavedDC);
}

void CLogChart::DrawLane(CDC *pDC, CView *pView)
{
	if( m_logDat.GetLogCount() < 1 )
		return;
	
	int nSavedDC = pDC->SaveDC();

	int nCount = m_chartLanes.GetSize();
	
	for (int i=0;i<nCount;i++)
	{
		CLogCurveLane *pLane = (CLogCurveLane *)m_chartLanes.GetAt(i);
		pLane->Draw(pDC, pView, i);
	}
	
	CRect rect = m_rectLane;
	CSize size = GetChartSize();
	rect.OffsetRect(m_iMargin - (rect.Width() - size.cx)/2,0);
	
	rect.DeflateRect(0,30,0,-30);
	
	CPen pen( PS_SOLID | PS_JOIN_MITER | PS_ENDCAP_FLAT , 5 , RGB(0, 0, 0) );
	CPen *pOldPen = pDC->SelectObject(&pen);
	pDC->MoveTo(rect.left, rect.top);
	pDC->LineTo(rect.left, rect.bottom);
	pDC->LineTo(rect.right,rect.bottom);
	pDC->LineTo(rect.right, rect.top);
	pDC->LineTo(rect.left, rect.top);
	pDC->SelectObject(pOldPen);	

	pDC->RestoreDC(nSavedDC);
}

double CLogChart::GetDepth(long yPos)
{
	CRect rect = m_rectLane;

	double dFirstDepth = (long)(floor(m_dMinDepth));
	
	if (dFirstDepth > 0)
		dFirstDepth = (long)(dFirstDepth) / 5 * 5;
	else
		dFirstDepth = ((long)(dFirstDepth) / 5 + ((long)dFirstDepth%5?-1:0)) * 5;
	
	double dDepth = dFirstDepth + (yPos-rect.top-30) * 1.0 / m_dDispScale[m_iCurDispScale];
	return dDepth;
}

double CLogChart::GetLogValue(int iItem, double dDepth)
{
	int nCount = m_logDat.GetLogCount();
	
	if( iItem>=nCount)
		return -99999.0;

	
	int nSamples = m_logDat.GetSamples();

	double dTop = 0, dBottom = 0;
	double value1=-99999.0,value2=-99999.0;

	for ( int i=0;i<nSamples-1;i++)
	{
		if( dDepth>=m_logDat.GetValue(0 ,i)
			&& dDepth< m_logDat.GetValue(0 ,i+1) )
		{
			dTop = m_logDat.GetValue(0 ,i);
			dBottom = m_logDat.GetValue(0 ,i+1);
			value1 = m_logDat.GetValue(iItem ,i);
			value2 = m_logDat.GetValue(iItem ,i+1);
			break;
		}
	}
	
	//
	if( dTop == dBottom)
		return -99999.0;

	// 两点式
	// (y-y1)/(x-x1)=(y2-y1)/(x2-x1) 
	// y = (y2-y1)/(x2-x1)*(x-x1)+y1
	//
	double val = (value2-value1)/(dBottom-dTop)*(dDepth-dTop) + value1;

	return val;	
}

double CLogChart::GetLogMinValue(int iItem, double dFirstDepth, double dSecondDepth)
{
	int nCount = m_logDat.GetLogCount();
	
	if( iItem>=nCount)
		return -99999.0;
	
	
	int nSamples = m_logDat.GetSamples();
	
	double val = 99999.0;
	
	for ( int i=0;i<nSamples;i++)
	{
		if( (dFirstDepth>=m_logDat.GetValue(0 ,i)
			&& dSecondDepth<= m_logDat.GetValue(0 ,i+1))
			||
			( dSecondDepth>=m_logDat.GetValue(0 ,i)
			&& dFirstDepth<= m_logDat.GetValue(0 ,i+1)))
		{
			if(val>m_logDat.GetValue(iItem ,i))
				val = m_logDat.GetValue(iItem,i);
		}
	}
		
	return val;	
}


double CLogChart::GetLogMaxValue(int iItem,  double dFirstDepth, double dSecondDepth)
{
	int nCount = m_logDat.GetLogCount();
	
	if( iItem>=nCount)
		return -99999.0;
	
	
	int nSamples = m_logDat.GetSamples();
	
	double val = -99999.0;
	
	for ( int i=0;i<nSamples;i++)
	{
		if( (dFirstDepth>=m_logDat.GetValue(0 ,i)
			&& dSecondDepth<= m_logDat.GetValue(0 ,i+1))
			||
			( dSecondDepth>=m_logDat.GetValue(0 ,i)
			&& dFirstDepth<= m_logDat.GetValue(0 ,i+1)))
		{
			if(val<m_logDat.GetValue(iItem ,i))
				val = m_logDat.GetValue(iItem,i);
		}
	}
	
	return val;	
}


double CLogChart::GetLogAvgValue(int iItem,  double dFirstDepth, double dSecondDepth)
{
	int nCount = m_logDat.GetLogCount();
	
	if( iItem>=nCount)
		return 0.0;
	
	
	int nSamples = m_logDat.GetSamples();
	
	double val = 0.0;
	int n = 0;
	for ( int i=0;i<nSamples;i++)
	{
		if( (dFirstDepth>=m_logDat.GetValue(0 ,i)
			&& dSecondDepth<= m_logDat.GetValue(0 ,i+1))
			||
			( dSecondDepth>=m_logDat.GetValue(0 ,i)
			&& dFirstDepth<= m_logDat.GetValue(0 ,i+1)))
		{
			val += m_logDat.GetValue(iItem ,i);
			n++;
		}
	}
	
	val /= n;

	return val;	
}


double CLogChart::GetLogAreaValue(int iItem,  double dFirstDepth, double dSecondDepth)
{
	int nCount = m_logDat.GetLogCount();
	
	if( iItem>=nCount)
		return 0.0;
	
	double val;

	double v1 = GetLogValue(iItem, dFirstDepth);
	double v2 = GetLogValue(iItem, dSecondDepth);

	val = (v1+v2)*fabs(dSecondDepth-dFirstDepth)/2.0;

	
	return val;	
}

void CLogChart::Add(CLogCurveLane *pLane, CView *pView/* = NULL*/) // 添加泳道
{
	m_chartLanes.Add((DWORD)pLane);
	int width = pLane->m_rectLane.Width();
	
	m_rectLane.DeflateRect(0,0,-width,0);
	m_rectTitle.DeflateRect(0,0,-width,0);		
	
	if( pView )
		pView->Invalidate(FALSE);
}

void CLogChart::InsertOf(CLogCurveLane *pLane, CLogCurveLane *pNewLane, CView *pView/* = NULL*/) // 添加泳道
{
	int iIndex;
	int n = m_chartLanes.GetSize();
	for (int i=0;i<n;i++)
	{
		CLogCurveLane *p = (CLogCurveLane *)m_chartLanes.GetAt(i);
		if( p==pLane )
		{
			iIndex = i;
			break;
		}
	}
	
	m_chartLanes.InsertAt(iIndex, (DWORD)pNewLane);
	
	int width = pNewLane->m_rectLane.Width();
	
	m_rectLane.DeflateRect(0,0,-width,0);
	m_rectTitle.DeflateRect(0,0,-width,0);		
	
	if( pView )
		pView->Invalidate(FALSE);
}

void CLogChart::AppendTo(CLogCurveLane *pLane, CLogCurveLane *pNewLane, CView *pView/* = NULL*/) // 添加泳道
{
	int iIndex;
	int n = m_chartLanes.GetSize();
	for (int i=0;i<n;i++)
	{
		CLogCurveLane *p = (CLogCurveLane *)m_chartLanes.GetAt(i);
		if( p==pLane )
		{
			iIndex = i;
			break;
		}
	}
	
	m_chartLanes.InsertAt(iIndex+1, (DWORD)pNewLane);
	
	int width = pNewLane->m_rectLane.Width();
	
	m_rectLane.DeflateRect(0,0,-width,0);
	m_rectTitle.DeflateRect(0,0,-width,0);		
	
	if( pView )
		pView->Invalidate(FALSE);
}

void CLogChart::Del(CLogCurveLane *pLane, CView *pView/* = NULL*/) // 删除泳道
{
	int nCount = m_chartLanes.GetSize();
	
	for (int i=0;i<nCount;i++)
	{
		CLogCurveLane *p = (CLogCurveLane *)m_chartLanes.GetAt(i);
		if( p == pLane )
		{
			Del(i, pView);
			break;
		}
	}
}

int CLogChart::GetLogDatIndex(const CString &strLogName)
{
	int iItem = -1;
	
	int nCount = m_logDat.GetLogCount();
	for ( int i=0;i<nCount;i++)
	{
		CString strCurName = m_logDat.GetLogName(i);
		CString str = GetLogName(strCurName);
		if ( str == strLogName )
		{
			iItem = i;
			break;
		}
	}
	
	return iItem;	
}

void CLogChart::Del(int &iIndex, CView *pView/* = NULL*/) // 删除泳道
{
	CLogCurveLane *pLane = (CLogCurveLane *)m_chartLanes.GetAt(iIndex);
	
	int width = pLane->m_rectLane.Width();
	m_rectLane.DeflateRect(0,0,width,0);
	m_rectTitle.DeflateRect(0,0,width,0);
	
	m_chartLanes.RemoveAt(iIndex);
	delete pLane;
	
	if( pView )
		pView->Invalidate(FALSE);
}

void CLogChart::ReCaleAllRect()
{
	// 计算高度
	double dFirstDepth = (long)(floor(m_dMinDepth));
	
	if (dFirstDepth > 0)
		dFirstDepth = (long)(dFirstDepth) / 5 * 5;
	else
		dFirstDepth = ((long)(dFirstDepth) / 5 + ((long)dFirstDepth%5?-1:0)) * 5;
	
	double dEndDepth = ceil(m_dMaxDepth);
	
	dEndDepth = ((long)(dEndDepth) / 5 + ((long)dEndDepth%5? 1:0)) * 5;
	
	m_rectTitle.top = 50;
	if( m_bDispCaption )		// 图标题是否显示
		m_rectTitle.top += m_chartCaption.m_rectCaption.Height();
	
	m_rectTitle.bottom = m_rectTitle.top + m_iTitleHeight;
	
	m_rectLane.top = m_rectTitle.bottom;
	m_rectLane.bottom = m_rectLane.top + (dEndDepth-dFirstDepth) * m_dDispScale[m_iCurDispScale];
	
	// 计算宽度
	int nCount = m_chartLanes.GetSize();
	
	int nWidth = 0;
	
	for ( int i=0;i<nCount; i++)
	{
		CLogCurveLane *pLane = (CLogCurveLane *)m_chartLanes.GetAt(i);
		nWidth += pLane->m_rectLane.Width();
	}
	
	m_rectTitle.right = m_rectTitle.left + nWidth;
	m_rectLane.right = m_rectLane.left + nWidth;
}

void CLogCurveLane::CopyDataToClipboard(long lParam)
{
	switch(m_nLaneType)
	{
	case LOGCURVE:				//测井曲线
		{
			strLOGINGPROP *pCurveSelected = (strLOGINGPROP *)lParam;

			if( pCurveSelected != NULL )
			{
				CWaitCursor wait;
				
				CSharedFile file (GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT);
				
				CString source;
				
				source = "井深	";
				source += pCurveSelected->m_strTitle;
				source += "\n";
				
				
				file.Write (source, source.GetLength());
				
				int nSamples = pCurveSelected->m_arDepth.size();
				
				for (int i=0; i<nSamples; i++)
				{
					source.Format("%g	%g\n",pCurveSelected->m_arDepth[i],pCurveSelected->m_arLogDat[i]);
					file.Write (source, source.GetLength());
				}
				
				CSharedFile file1 (GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT);
				
				CArchive ar(&file1, CArchive::store);
				
				ar << 1;
				pCurveSelected->Serialize(ar);
				ar.Close();

				if(OpenClipboard(NULL))
				{
					//char * buffer;
					EmptyClipboard();
					HGLOBAL clipbuffer = file.Detach();
					HGLOBAL clipbuffer1 = file1.Detach();
					GlobalUnlock(clipbuffer);
					GlobalUnlock(clipbuffer1);
					SetClipboardData(CF_TEXT,clipbuffer);
					SetClipboardData(nFormatClip_Curve,clipbuffer1);
					CloseClipboard();
				}
			}
			else
			{
				CWaitCursor wait;
				
				CSharedFile file (GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT);
				
				CArchive ar(&file, CArchive::store);
				
				int n = m_arLoging.GetSize();
				ar << n;
				for (int i=0;i<n;i++)
				{
					m_arLoging[i].Serialize(ar);
				}
				
				ar.Close();
				if(OpenClipboard(NULL))
				{
					//char * buffer;
					EmptyClipboard();
					HGLOBAL clipbuffer = file.Detach();
					GlobalUnlock(clipbuffer);
					SetClipboardData(nFormatClip_Curve,clipbuffer);
					CloseClipboard();
				}
			}
		}
		break;
	case DEPTH:					//井深  拷贝曲线数据
		{
			CWaitCursor wait;

			CSharedFile file (GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT);

			CString source;
			CString str;

			int nCols = m_pChartLog->m_logDat.GetLogCount();
			for ( int i=0;i<nCols;i++)
			{
				str = m_pChartLog->m_logDat.GetLogName(i);
				source += str;
				if( i<nCols-1)
					source += "	";
				else
					source += "\n";
			}

			file.Write (source, source.GetLength());

			int nSamples = m_pChartLog->m_logDat.GetSamples();
			
			for (int j=0; j<nSamples; j++)
			{
				source = "";
				for ( int i=0;i<nCols;i++)
				{
					str.Format("%g",m_pChartLog->m_logDat.GetValue(i,j));
					source += str;
					if( i<nCols-1)
						source += "	";
					else
						source += "\n";
				}

				file.Write (source, source.GetLength());
			}

			if(OpenClipboard(NULL))
			{
				//char * buffer;
				EmptyClipboard();
				HGLOBAL clipbuffer = file.Detach();
				GlobalUnlock(clipbuffer);
				SetClipboardData(CF_TEXT,clipbuffer);
				CloseClipboard();
			}
		}
		break;
	default:
		break;
	}
}

void CLogCurveLane::PasteDataFromClipboard(long lParam)
{
	switch(m_nLaneType)
	{
	case LOGCURVE:				//测井曲线
		{
			if(IsClipboardFormatAvailable(nFormatClip_Curve))
			{
				CSharedFile file (GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT);
				
				CArchive ar((CFile* )&file,CArchive::load);
				
				if (OpenClipboard(NULL))
				{				
					HGLOBAL hData =GetClipboardData(nFormatClip_Curve);				
					file.SetHandle(hData);
					GlobalUnlock(hData);
					CloseClipboard();
				}
				
				int n;
				ar >> n;
				for (int i=0;i<n;i++)
				{
					CLogCurveLane::strLOGINGPROP prop;
					prop.Serialize(ar);
					prop.m_pParentLane = this;
					prop.m_bSelected = FALSE;
					m_arLoging.Add(prop);
				}

				ar.Close();
			}			
		}
		break;
	default:
		break;
	}
}

// 用户手工导入数据
void CLogChart::LoadLogDat(CView *pView)
{
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"所有文件 *.*|*.*||");
	//------用来检测目前的操作系统的版本信息	
	int nStructSize = 0;	
	DWORD dwVersion, dwWindowsMajorVersion, dwWindwosMinorVersion;	
	dwVersion = GetVersion();	
	dwWindowsMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));	
	dwWindwosMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));	
	if(dwVersion < 0x80000000)
	{		
        nStructSize = 88; //2k,xp下显示新的版本		
	}	
	else		
	{		
        nStructSize = 76; //98下显示老版本		
	}	
	dlg.m_ofn.lStructSize = nStructSize; //------用来检测目前的操作系统的版本信息
	
	if ( dlg.DoModal() == IDOK )
	{
		CWaitCursor wait;
		CString fileName = dlg.GetPathName();
		m_chartCaption.m_strTitle = dlg.GetFileTitle();
		m_chartCaption.m_strTitle += _T("测井曲线图");
		LoadLogDat(pView, fileName);
	}
}

void CLogChart::LoadLogDat(CView *pView, const CString&strFileName)
{
	CWaitCursor wait;
	if( IsTextFile (strFileName))
	{
		if( IsLasFile(strFileName) ) // 是LAS文件格式吗
		{
			LoadLogLasFile(pView, strFileName);
			return;
		}
		
		if( IsListFile(strFileName) ) // 是LIST文件格式吗
		{
			LoadLogListFile(pView, strFileName);
			return;
		}

		if( IsAscFile(strFileName) ) // 是ASC文件格式
		{
			LoadLogAscFile(pView, strFileName);
			return;
		}

		if( LoadLogDatTxt(pView, strFileName) ) // 三种格式
			return;

		HINSTANCE hInstOld = AfxGetResourceHandle();
		AfxSetResourceHandle(DispLogCurveDLL.hModule);
		
		CLogTemplSheet propSheet;
		propSheet.m_strPath = strFileName;
		if( propSheet.DoModal() == ID_WIZFINISH)
		{
			CLogTemplPage0 *pPage0 = (CLogTemplPage0 *)propSheet.GetPage(0);				
			CLogTemplPage1 *pPage1 = (CLogTemplPage1 *)propSheet.GetPage(1);
			
			CString tmp,strTemp;
			char seps[5] = " ,	;";
			char sep[2] = "\0";
			if (pPage1->m_iChar < 4)
				sep[0] = seps[pPage1->m_iChar];
			else
				sep[0] = pPage1->m_char[0];
			
			int nCols = pPage1->m_Grid.GetColumnCount()-1;
			
			for (int j=0; j<nCols; j++)
			{						
				if( (j+1) == pPage0->m_nDepthCol )
					tmp = "DEPTH";
				else
					tmp = pPage1->m_Grid.GetItemText(1,j+1);
				
				m_logDat.AddLogName(tmp,j);
			}
			

			CStdioFile file;

			if( !file.Open( strFileName, CFile::modeRead | CFile::typeText ) )
			{
				::MessageBox(NULL, "打开文件失败！","提示信息",MB_OK|MB_ICONERROR);
				return;
			}
			
			int nLine = 0;

			while (file.ReadString(strTemp))
			{
				nLine ++;
				strTemp.TrimLeft();
				strTemp.TrimRight();
				if (strTemp.IsEmpty())
					continue;
				if(nLine >= pPage0->m_iFirstRow )
				{								
					if( pPage0->m_option == 0 )// 分隔符
					{
						CStringArray arr;
						
						TokenString(strTemp,sep,arr);
						
						int nCount = arr.GetSize();
						if(nCount>nCols)
						{
							nCount = nCols;
						}
						int j;
						for ( j= 0; j<nCount; j++)
						{
							double val = atof(arr[j]);
							
							if((val > 9998.0 || val <-9998.0)
								&& j!=pPage0->m_nDepthCol-1 )
								val = -99999.0;;
							
							m_logDat.Add(val,j);
						}
						for (j=nCount;j<nCols;j++) //列数不够时
							m_logDat.Add(0.0,j);
					}
					else // 固定宽度
					{
						tmp = strTemp;
						
						int first=0, count=0;
						int nTitle = pPage1->m_arTitleWidth.GetSize();
						for( int j= 0;j<nCols;j++)
						{	
							if( j==0)
							{
								first = 0;
								count = pPage1->m_arTitleWidth[j];
							}
							else if(j < nTitle)
							{
								first += pPage1->m_arTitleWidth[j];
								count = pPage1->m_arTitleWidth[j];
							}
							else
							{
								first += pPage1->m_arTitleWidth[nTitle-1];
								count = pPage1->m_arTitleWidth[nTitle-1];
							}
							
							if( first >= tmp.GetLength())
								strTemp = "";
							else
								strTemp = tmp.Mid(first,count);
							
							double val = atof(strTemp);
							
							if((val > 9998.0 || val <-9998.0)
								&& j!=pPage0->m_nDepthCol-1 )
								val = -99999.0;;
							
							m_logDat.Add(val,j);
						}
					}
				}
				else
					continue;
			}

			file.Close();
			
			
			if( m_logDat.GetSamples() > 1 )
			{
				m_dMinDepth = m_logDat.GetMinDepth();
				m_dMaxDepth = m_logDat.GetMaxDepth();
				
				double dFirstDepth = (long)(floor(m_dMinDepth));
				
				if (dFirstDepth > 0)
					dFirstDepth = (long)(dFirstDepth) / 5 * 5;
				else
					dFirstDepth = ((long)(dFirstDepth) / 5 + ((long)dFirstDepth%5?-1:0)) * 5;
				
				double dEndDepth = ceil(m_dMaxDepth);
				
				dEndDepth = ((long)(dEndDepth) / 5 + ((long)dEndDepth%5? 1:0)) * 5;
				
				m_dMinDepth = dFirstDepth;
				m_dMaxDepth = dEndDepth;
				
				m_rectTitle.left = 0;
				m_rectTitle.right = m_rectTitle.left + 100;
				
				m_rectTitle.top = 50;
				if( m_bDispCaption )		// 图标题是否显示
					m_rectTitle.top += m_chartCaption.m_rectCaption.Height();
				
				m_rectTitle.bottom = m_rectTitle.top + m_iTitleHeight;
				
				m_rectLane.left = 0;
				m_rectLane.right = m_rectLane.left + 100;
				m_rectLane.top = m_rectTitle.bottom;
				m_rectLane.bottom = m_rectLane.top + (dEndDepth-dFirstDepth) * m_dDispScale[m_iCurDispScale];
				
				CRect rect = m_rectLane;
				CLogCurveLane *pLane = new CLogCurveLane(CLogCurveLane::DEPTH,this,rect);
				m_chartLanes.Add((DWORD)pLane);
				
				ReCaleAllRect();
			}
			else
				m_logDat.RemoveAll();
			
			}
			
			AfxSetResourceHandle(hInstOld);
		}
		else
		{
			BOOL bBigEndian, bCurves;
			long lReadPos;
			if( !Is716Format(strFileName,bBigEndian,bCurves,lReadPos) )
			{
				::MessageBox(AfxGetMainWnd()->m_hWnd,"系统不能识别此二进制文件格式!","提示信息",MB_OK|MB_ICONERROR);
				return;
			}
			Load716File(strFileName,bBigEndian,bCurves,lReadPos);
		}
		
		if( pView )
		{
			pView->GetDocument()->UpdateAllViews(NULL, HINT_UPDATE_CHANGEDSIZE);
			
			pView->Invalidate(FALSE);
			
			pView->GetDocument()->SetModifiedFlag();

			CSplitPath path(strFileName);
			
			pView->GetDocument()->SetTitle(path.GetFileName());
		}
}

//////////////////////////////////////////////////////////////////////////

// 是否为716格式
// 是否按实际曲线条数保存的
BOOL CLogChart::Is716Format(LPCSTR name, BOOL &bIsBigEndian, BOOL &bCurves, long &lReadPos)
{
	BOOL bRet = TRUE;
	CFile file;
	if(file.Open(name,CFile::modeRead))
	{
		int nLen = file.GetLength();//文件长度
		
		// 文件头有496个字节 或 512
		if( nLen <= 0x1F0 )//文件长度不能少于496个字节
		{
			file.Close();
			return FALSE;
		}
		
		bIsBigEndian = TRUE;
		file.Seek(0XA4,CFile::begin);		
		int nCount=GetInt(file,true);
		if( nCount > 40 )//不能超过40条曲线
		{
			file.Seek(0XA4,CFile::begin);		
			nCount=GetInt(file,false);
			
			if( nCount > 40 )
				bRet = FALSE;
			
			bIsBigEndian = FALSE;
		}
		
		if( bRet )
		{			
			file.Seek(0XF8,CFile::begin);
			
			float stdp=GetFloat(file,bIsBigEndian);
			float endp=GetFloat(file,bIsBigEndian);
			float relv=GetFloat(file,bIsBigEndian);
			
			if( stdp < -10.0 
				|| stdp > 9999.0 
				|| endp < -10.0 
				|| endp > 9999.0
				|| relv <= 0.0 
				|| relv >= 500.0)
				bRet = FALSE;
			
			if( bRet )
			{
				int nSampls=ceilf(( stdp>endp ? (stdp-endp):(endp-stdp))/relv) + 1;//总样点数
				GetInt(file,bIsBigEndian);
				GetInt(file,bIsBigEndian);
				
				int nNum = (int)GetFloat(file,bIsBigEndian);//每个记录的样点数
				
				if( nNum > 0 )
				{
					// 计算记录数
					int nRecord;
					nRecord = nSampls/nNum;
					if(nSampls%nNum)
						nRecord = nRecord + 1;
					
					// 数据的总字节数
					long nDataLen = nRecord * 40 * nNum * 4;
					
					if( nDataLen > nLen )
					{
						nDataLen = nRecord * nCount * nNum * 4;
						
						lReadPos = nLen - nDataLen;
						if( lReadPos > 0X200 )
							bCurves = FALSE;
						else
							bCurves = TRUE;
					}
					else
					{
						lReadPos = nLen - nDataLen;
						bCurves = FALSE;
					}
					
					if( lReadPos < 276 )
					{
						file.Close();
						return FALSE;
					}

					long pos = lReadPos - 0X1F0;
					
					if( pos >= 0 )
					{
						float fCheck = (float)pos/(float)0X1F0;
						if( ceilf(fCheck) == floorf(fCheck) )
							lReadPos = 0X1F0;
						else
						{
							pos = lReadPos - 0X200;
							fCheck = (float)pos/(float)0X200;
							if( ceilf(fCheck) == floorf(fCheck) )
								lReadPos = 0X200;
							else
								lReadPos = 0X1F0;
						}
					}				
					
				}
				else
					bRet = FALSE;
			}					
		}
		
		file.Close();
		
		return bRet;
	}
	return FALSE;
}


// 判断文件是否为文本文件
BOOL CLogChart::IsTextFile(LPCSTR lpstrFile)
{
	TRY
	{
		CFile file( lpstrFile, CFile::modeRead|CFile::typeBinary);
		BYTE buf;
		int i=0;
		while (file.Read(&buf,1)) 
		{
			int x=iscntrl(buf);
			
			if(iscntrl(buf)&&buf!=0x0d&&buf!=0x0a&&buf!=0x09&&buf!=0x00)
			{
				file.Close();
				return FALSE;
			}
			
			i++;
			
			if( i > 1024 )
				break;
		}
		
		file.Close();
		return TRUE;
	}
	CATCH( CFileException, e )
	{
		::MessageBox(AfxGetMainWnd()->m_hWnd,"读文件错误！","提示信息",MB_OK|MB_ICONERROR);
		return FALSE;
	}
	END_CATCH
}

// 判断文件是否为ASC文本文件
BOOL CLogChart::IsAscFile(LPCSTR lpstrFile)
{
// 	茂X22-斜18	216                                                                  
// 	icurve     sdep         edep          cyjj         cyds
// 	12  1512.780029  1794.130127     0.050000    62.000000
// 	DEP          CI        GR        LLD       LLS       HAC       RMG       RMN       CALC      MSFL      AC        R250      SP  
// 	1512.780     6.926   114.814    11.780    11.670   406.242     4.452     3.711    22.628     0.000   415.522    10.226    34.770

	CStdioFile file;

	if( !file.Open(lpstrFile, CFile::modeRead|CFile::typeText) )
	{
		CString msg;
		msg.Format("打开 %s 文件失败!",lpstrFile);

		::MessageBox(NULL, msg, "提示信息", MB_OK|MB_ICONSTOP);

		return FALSE;
	}

	CString strTmp;
	file.ReadString( strTmp );
	file.ReadString( strTmp ); // 第二行
	file.Close();

	int nLen = strTmp.GetLength(); // 文件第二行长度
	if( nLen < 7 )
		return FALSE;

	strTmp.Trim();

	strTmp.MakeLower();
	CStringArray arr;
	if( TokenString(strTmp, _T(" "), arr) != 5 )
		return FALSE;

	if( arr[0] != _T("icurve")
		|| arr[1] != _T("sdep")
		|| arr[2] != _T("edep")
		|| arr[3] != _T("cyjj")
		|| arr[4] != _T("cyds"))
		return FALSE;

	return TRUE;
}

// 判断文件是否为LAS文件
BOOL CLogChart::IsLasFile(LPCSTR lpstrFile)
{
	CStdioFile file;
	
	if( !file.Open(lpstrFile, CFile::modeRead|CFile::typeText) )
	{
		CString msg;
		msg.Format("打开 %s 文件失败!",lpstrFile);
		
		::MessageBox(NULL, msg, "提示信息", MB_OK|MB_ICONSTOP);
		
		return FALSE;
	}
	
	BOOL bRet = FALSE;
	
	CString strTmp;
	
	int nCountLine = 0;
	int num = 0;
	
	while (file.ReadString( strTmp ))
	{
		strTmp.TrimRight();
		strTmp.TrimLeft();
		if(strTmp.IsEmpty())
			continue;
		if( strTmp[0]=='#' )
			continue;
		strTmp.MakeUpper();

		if( strTmp.Left(5) == "~WELL" )
			num ++;
		else if( strTmp.Left(8) == "~VERSION")
			num ++;		
		else if( strTmp.Left(6) == "~CURVE")
			num ++;
		else if(strTmp.Left(2) == "~A" )
			num++;

		nCountLine ++;

		if( num == 4)
		{
			bRet = TRUE;
			break;
		}
		if( nCountLine >= 200 )
			break;
	}

	file.Close();

	return bRet;
}


// 判断文件是否为LIST文件
// 第一行，曲线名称，每个名称10字符
// 第二行，--------  --------  
// 第三行，空
// 第四行，数据开始，以空格分隔

BOOL CLogChart::IsListFile(LPCSTR lpstrFile)
{
	CStdioFile file;
	
	if( !file.Open(lpstrFile, CFile::modeRead|CFile::typeText) )
	{
		CString msg;
		msg.Format("打开 %s 文件失败!",lpstrFile);
		
		::MessageBox(NULL, msg, "提示信息", MB_OK|MB_ICONSTOP);
		
		return FALSE;
	}
	
	BOOL bRet = TRUE;
	
	CString strTmp;
	file.ReadString( strTmp );
	
	int nLen = strTmp.GetLength(); // 文件第一行长度
	if( nLen < 10 )
		return FALSE;
	
	int nMod = nLen%10; // 必须为10的倍数
	nLen = nLen/10;		// 必须大于1个10的倍数

	if( nMod != 0 || nLen < 2)
		return FALSE;

	for (int i=0;i<nLen;i++)
	{
		CString tmp = strTmp.Mid(i*10,10);
		
		tmp.TrimLeft();
		tmp.TrimRight();

		if( tmp.IsEmpty() )
			return FALSE;

		BOOL bCNchar = FALSE;
		UINT cn;
		BOOL bEn = TRUE;
		int n = tmp.GetLength();
		for(int j=0;j<n;j++)  // 如果有汉字名称，判断是否有半个汉字
		{
			cn = tmp[j];
			if(cn>0xa0 && bCNchar == FALSE)
			{
				bCNchar = TRUE;
				bEn = FALSE;
			}
			else
			{
				if( bCNchar )
				{
					if( cn>0xa0 )
						bCNchar = FALSE;
					else
						return FALSE;
				}
			}
		}
		
		if( bEn ) // 如果全是英文，英文中不能有空格、制表键、逗号、|等字符
		{			
			if( tmp.Find(" ") != -1 ) // 测井曲线名称中有空格
				return FALSE;
			
			if( tmp.Find("	") != -1 ) // 测井曲线名称中有制表键
				return FALSE;
			
			if( tmp.Find(",") != -1 ) // 测井曲线名称中有逗号
				return FALSE;
			
			if( tmp.Find("|") != -1 ) // 测井曲线名称中有"|"
				return FALSE;
		}
	}

	int nCountLine = 0;
	
	while (file.ReadString( strTmp ))
	{
		strTmp.TrimRight();
		strTmp.TrimLeft();
		if(strTmp.IsEmpty())
			continue;
		nCountLine ++;
		CStringArray arr;
		TokenString(strTmp, " ", arr);
		
		if( arr.GetSize() != nLen )	// 不等于曲线总数返回
		{
			bRet = FALSE;
			break;
		}

		if( nCountLine > 20 )
			break;
	}
	
	file.Close();
	
	return bRet;
}

BOOL CLogChart::Load716File(const CString &fileName, BOOL bIsBigEndian, BOOL bCurves, long lReadPos)
{
	char buff[5]="";
	CFile file;
	int nCount; //曲线数
	CString tmp;
	CStringArray arFields;
	if(file.Open(fileName,CFile::modeRead|CFile::typeBinary))
	{
		file.Seek(0XA4,CFile::begin);
		nCount=GetInt(file, bIsBigEndian);
		file.Seek(0XA8,CFile::begin);
		int i;
		int nFirstCount = (nCount>20?20:nCount);
		for ( i=0; i<nFirstCount; i++)
		{
			file.Read(buff,4);
			tmp = buff;
			tmp.TrimRight();
			tmp.TrimLeft();
			if( tmp.IsEmpty())
			{
				file.Close();
				::MessageBox(AfxGetMainWnd()->m_hWnd,"716文件格式有错误!","提示信息",MB_OK|MB_ICONERROR);
				return FALSE;
			}
			arFields.Add(tmp);//曲线名称
		}
		
		file.Seek(0X114,CFile::begin);
		for ( i=20; i<nCount; i++)
		{
			file.Read(buff,4);
			tmp = buff;
			tmp.TrimRight();
			tmp.TrimLeft();
			if( tmp.IsEmpty())
			{
				file.Close();
				::MessageBox(AfxGetMainWnd()->m_hWnd,"716文件格式有错误!","提示信息",MB_OK|MB_ICONERROR);
				return FALSE;
			}
			arFields.Add(tmp);//曲线名称
		}

		file.Seek(0XF8,CFile::begin);

		float stdp=GetFloat(file,bIsBigEndian);
		float endp=GetFloat(file,bIsBigEndian);
		float relv=GetFloat(file,bIsBigEndian);

		if( stdp>endp )
		{
			float dTmp;
			dTmp = stdp;
			stdp = endp;
			endp = dTmp;
		}

		int nSampls=ceilf((endp-stdp)/relv)+1;//总样点数
		GetInt(file,bIsBigEndian);
		GetInt(file,bIsBigEndian);
		
		int nNum = (int)GetFloat(file,bIsBigEndian);//每个记录的样点数

		// 计算记录数
		int nRecord;
		nRecord = nSampls/nNum;
		if(nSampls%nNum)
			nRecord = nRecord + 1;

		CArray<float, float> arValues[40]; //样点数据

		int nCurves;

		if( bCurves )
			nCurves = nCount;
		else
			nCurves = 40;		
		
		file.Seek(lReadPos,CFile::begin);	

		int index=0;
		for (i=0;i<nRecord; i++)
		{
			for (int j=0; j<nCurves; j++) //40为允许最大曲线数
			{
				for (int m =0; m<nNum; m++)
				{
					
					if( j < nCount && index < nSampls)
						arValues[j].Add(GetFloat(file,bIsBigEndian));
					else
						GetFloat(file);

					if(j==0&&m==0)
						index++;
				}
			}
		}
		
		file.Close();
		
		
		//////////////////////////////////////////////////////////////////////////
		// 删除以前的数据
		m_logDat.RemoveAll();
		nCount = m_chartLanes.GetSize();
		
		for ( i=0;i<nCount; i++)
		{
			CLogCurveLane *pLane = (CLogCurveLane *)m_chartLanes.GetAt(i);
			delete pLane;
		}
		
		m_chartLanes.RemoveAll();	
		
		// 装入数据
		{
			float depth=stdp;
			
			int nFields = arFields.GetSize();
			
			m_logDat.AddLogName("DEPTH",0);
			for(i=0;i<nFields;i++)
			{
				m_logDat.AddLogName(arFields[i],i+1);
			}
			
			for(i=0;i<nSampls; i++)
			{				
				m_logDat.Add(depth,0);
				depth += relv;
				for(int j=0;j<nFields;j++)
				{
					m_logDat.Add(arValues[j][i],j+1);
				}
				
			}			
		}

		m_dMinDepth = m_logDat.GetMinDepth();
		m_dMaxDepth = m_logDat.GetMaxDepth();
		
		double dFirstDepth = (long)(floor(m_dMinDepth));
		
		if (dFirstDepth > 0)
			dFirstDepth = (long)(dFirstDepth) / 5 * 5;
		else
			dFirstDepth = ((long)(dFirstDepth) / 5 + ((long)dFirstDepth%5?-1:0)) * 5;
		
		double dEndDepth = ceil(m_dMaxDepth);
		
		dEndDepth = ((long)(dEndDepth) / 5 + ((long)dEndDepth%5? 1:0)) * 5;
		
		m_dMinDepth = dFirstDepth;
		m_dMaxDepth = dEndDepth;
		
		m_rectTitle.left = 0;
		m_rectTitle.right = m_rectTitle.left + 100;
		
		m_rectTitle.top = 50;
		if( m_bDispCaption )		// 图标题是否显示
			m_rectTitle.top += m_chartCaption.m_rectCaption.Height();
		
		m_rectTitle.bottom = m_rectTitle.top + m_iTitleHeight;
		
		m_rectLane.left = 0;
		m_rectLane.right = m_rectLane.left + 100;
		m_rectLane.top = m_rectTitle.bottom;
		m_rectLane.bottom = m_rectLane.top + (dEndDepth-dFirstDepth) * m_dDispScale[m_iCurDispScale];
		
		CRect rect = m_rectLane;
		CLogCurveLane *pLane = new CLogCurveLane(CLogCurveLane::DEPTH,this,rect);
		m_chartLanes.Add((DWORD)pLane);
		
		ReCaleAllRect();		
	}
	else
	{
		::MessageBox(AfxGetMainWnd()->m_hWnd,"读文件错误！","提示信息",MB_OK|MB_ICONERROR);
		return FALSE;
	}
	return TRUE;
}

BOOL CLogChart::GetLogDatTempFileName (LPTSTR pszFileName)
{   
	TCHAR szTmpDir[_MAX_PATH];     // Ask the system for the TEMP directory  
	DWORD dwChars = GetTempPath (_MAX_PATH, szTmpDir);
	if (dwChars)   
	{     
		dwChars = ::GetTempFileName
			(szTmpDir,         // Call the Win32 API    
			"CSV",			   // Our transport's fixed prefix for temp files 
			0,                     // Use the time to create a pseudo-random number 
			pszFileName);          // Destination buffer
	}
	return (0 != dwChars ? TRUE : FALSE);
} 



void CLogChart::ApplyTemplate(const CString &strTemplate, CView* pView)
{
	if( strTemplate.IsEmpty() )
	{
		::MessageBox(NULL, "显示模板为空!","提示信息",MB_OK|MB_ICONWARNING);
		return;
	}
	
	CWaitCursor wait;

	// 删除所有道
	int nCount = m_chartLanes.GetSize();
	
	for ( int i=0;i<nCount; i++)
	{
		CLogCurveLane *pLane = (CLogCurveLane *)m_chartLanes.GetAt(i);
		delete pLane;
	}
	
	m_chartLanes.RemoveAll();
	
	ReCaleAllRect();
	
	CString strLaneType[21] = 
	{
		_T("井深"),
			_T("测井曲线"),
			_T("夹层标记"),
			_T("地层分层"),
			_T("油层组"),
			_T("油层"),
			_T("层号"),
			_T("射孔井段"),
			_T("砂岩厚度"),
			_T("有效厚度"),
			_T("渗透率"),
			_T("孔隙度"),
			_T("含油饱和度"),
			_T("含气饱和度"),	
			_T("含水饱和度"),
			_T("束缚水饱和度"),
			_T("解释结果"),
			_T("电测解释结果"),					
			_T("岩性"),
			_T("图片"),
			_T("文字说明")
	};
	
	CStringArray arrTemplate;

	TokenString(strTemplate,"\n",arrTemplate);
	
	CString strTmp;
	nCount = arrTemplate.GetSize();
	for (int k=0;k<nCount;k++)
	{
		strTmp = arrTemplate[k];
		strTmp.TrimLeft();
		strTmp.TrimRight();
	
		CStringArray arr;
		CString str;
		
		TokenString(strTmp,",",arr);
		
		int n = arr.GetSize();
		if( n < 1 )
			continue;
		
		str = arr[0]; // 道类别
		str.TrimLeft();
		str.TrimRight();
		if( str.IsEmpty() )
			continue;
		
		CLogCurveLane::LANETYPE nType;
		
		for ( int i=0;i<20;i++)
		{
			if(str == strLaneType[i] )
			{
				nType = (CLogCurveLane::LANETYPE) i;
				break;
			}
		}
	
		if( n == 1 )
		{	
			CRect rect(0,0,0,0);
			rect.left = 0;
			
			if (nType == CLogCurveLane::LOGCURVE)
				rect.right = 500;
// 			else if(nType == CLogCurveLane::INTERLAYER)
// 				rect.right = 100;
			else
				rect.right = 100;
			
			CLogCurveLane *pLane = new CLogCurveLane(nType,this,rect);			
			Add( pLane, pView );
			
			continue;
		}
		
		str = arr[1]; // 道宽
		
		CRect rect(0,0,0,0);
		rect.left = 0;
		
		int iWidth = atoi(str);
		
		rect.right = iWidth;
		
		CLogCurveLane *pLane = new CLogCurveLane(nType,this,rect);			
		Add( pLane, pView );

		pLane->m_bBorder	 = atoi(arr[2]);	// 边框
		pLane->m_bFill		 = atoi(arr[3]);	// 是否填充
		pLane->m_clrFill	 = atol(arr[4]);	// 填充颜色
		pLane->m_bLogCoord	 = atoi(arr[5]);	// 对数坐标
		pLane->m_bSameCoord	 = atoi(arr[6]);	// 统一坐标			
		pLane->m_iGridLine	 = atoi(arr[7]);	// 网格线
		
		if(nType == CLogCurveLane::LOGCURVE)
		{
			int nCurve = (n - 8) / 12;
			for ( int j=0; j<nCurve; j++)
			{
				str = arr[8+j*12+0];
				
				str.MakeUpper();
				str.TrimLeft();
				str.TrimRight();
				if( str.IsEmpty() )
					continue;
				
				CString strTitle = str;
				int iItem = GetLogDatIndex(str);
				
				if( iItem == -1 )
					continue;
				
				int iPart;
				int iMarkPart;
				BOOL bDirect;
				double dCoordMin;
				double dCoordMax; 
				COLORREF clrLine;
				int	iLineStyle;
				int	iLineWidth;
				BOOL bFill;
				COLORREF clrFill;
				int iFillStyle; //是实心填充 

				str = arr[8+j*12+1];
				iPart = atoi(str);
				str = arr[8+j*12+2];
				iMarkPart = atoi(str);
				str = arr[8+j*12+3];
				bDirect = atoi(str);
				str = arr[8+j*12+4];
				dCoordMin = atof(str);
				str = arr[8+j*12+5];
				dCoordMax = atof(str);
				str = arr[8+j*12+6];
				clrLine = atol(str);
				str = arr[8+j*12+7];
				iLineStyle = atoi(str);
				str = arr[8+j*12+8];
				iLineWidth = atoi(str);
				str = arr[8+j*12+9];
				bFill = atoi(str);
				str = arr[8+j*12+10];
				clrFill = atol(str);
				str = arr[8+j*12+11];
				iFillStyle = atoi(str);
				
				int nLog = m_logDat.GetLogCount();		
				for (int l = 0; l < nLog; l++)
				{
					int iIndex = pLane->GetCurveCount();
					if( iItem == l )
					{
						int nSamples = m_logDat.GetSamples();
						CString str = m_logDat.GetLogName(l);
						for (int m=0; m<nSamples; m++)
							pLane->Add(iIndex,
							m_logDat.GetDepth(m),
							m_logDat.GetValue(str,m));
						
						pLane->m_arLoging[iIndex].m_iPart		 = iPart;
						pLane->m_arLoging[iIndex].m_iMarkPart	 = iMarkPart;
						pLane->m_arLoging[iIndex].m_bDirect		 = bDirect;
						pLane->m_arLoging[iIndex].m_dCoordMin	 = dCoordMin;
						pLane->m_arLoging[iIndex].m_dCoordMax	 = dCoordMax;
						pLane->m_arLoging[iIndex].m_clrLine		 = clrLine;
						pLane->m_arLoging[iIndex].m_iLineStyle	 = iLineStyle;
						pLane->m_arLoging[iIndex].m_iLineWidth	 = iLineWidth;
						pLane->m_arLoging[iIndex].m_bFill		 = bFill;
						pLane->m_arLoging[iIndex].m_clrFill		 = clrFill;
						pLane->m_arLoging[iIndex].m_iFillStyle	 = iFillStyle; //是实心填充   

						CString title="";
						
						// 获取曲线中文名称
						title = GetLogComments(strTitle);
						if( title.IsEmpty() )
							title = strTitle;
						
						pLane->SetLogCurveTitle(iIndex,title);
						
						pLane->SetLogName(iIndex,strTitle);
					}
				}
			}
		}
	}
	pView->GetDocument()->UpdateAllViews(NULL, HINT_UPDATE_CHANGEDSIZE);
	pView->Invalidate(TRUE);
}

const CString CLogChart::CreateTemplate()
{
	CString strTemplate;
	int nCount = m_chartLanes.GetSize();
	for (int i=0;i<nCount; i++)
	{
		CLogCurveLane *pLane = (CLogCurveLane *)m_chartLanes[i];

		CString strTmp;

		switch(pLane->m_nLaneType)
		{
		case CLogCurveLane::DEPTH:					// 井深
			strTemplate += _T("井深");
			break;
		case CLogCurveLane::LOGCURVE:				// 测井曲线
			strTemplate += _T("测井曲线");
			break;
		case CLogCurveLane::INTERLAYER:				// 夹层标记
			strTemplate += _T("夹层标记");
			break;
		default:
			break;
		}
		strTmp.Format(",%d,%d,%d,%ld,%d,%d,%d",
			pLane->m_rectLane.Width(),	// 宽度
			pLane->m_bBorder,			// 边框
			pLane->m_bFill,				// 是否填充
			pLane->m_clrFill,			// 填充颜色
			pLane->m_bLogCoord,			// 对数坐标
			pLane->m_bSameCoord,		// 统一坐标
			pLane->m_iGridLine			// 网格线
		);

		strTemplate += strTmp;

		if( pLane->m_nLaneType == CLogCurveLane::LOGCURVE)
		{
			int n = pLane->m_arLoging.GetSize();
			for (int j=0; j <n; j++)
			{				
				CString strLogName = GetLogName( pLane->m_arLoging[j].m_strLogName );

				strTmp.Format(",%s,%d,%d,%d,%g,%g,%ld,%d,%d,%d,%ld,%d",
					strLogName,
					pLane->m_arLoging[j].m_iPart,
					pLane->m_arLoging[j].m_iMarkPart,
					pLane->m_arLoging[j].m_bDirect,
					pLane->m_arLoging[j].m_dCoordMin,
					pLane->m_arLoging[j].m_dCoordMax,
					pLane->m_arLoging[j].m_clrLine,
					pLane->m_arLoging[j].m_iLineStyle,
					pLane->m_arLoging[j].m_iLineWidth,
					pLane->m_arLoging[j].m_bFill,
					pLane->m_arLoging[j].m_clrFill,
					pLane->m_arLoging[j].m_iFillStyle //是实心填充
					);
				strTemplate += strTmp;
			}

			strTemplate += "\n";
		}
		else
			strTemplate += "\n";
	}
	return strTemplate;
}

//////////////////////////////////////////////////////////////////////////

// 读取文本测井曲线数据文件
BOOL CLogChart::LoadLogDatTxt(CView *pView, const CString &strFileName)
{
	CStdioFile file;

	if( !file.Open(strFileName, CFile::modeRead|CFile::typeText) )
	{
		CString msg;
		msg.Format("打开 %s 文件失败!",strFileName);

		::MessageBox(NULL, msg, "提示信息", MB_OK|MB_ICONSTOP);

		return FALSE;
	}

	BOOL bRet = FALSE;

	CString strTmp;

	file.ReadString( strTmp );
	
	if( strTmp == "FORWARD_TEXT_FORMAT_1.0") //
	{
		file.ReadString(strTmp);
		file.ReadString(strTmp);
		file.ReadString(strTmp);
		file.ReadString(strTmp);
		file.ReadString(strTmp);
		
		file.ReadString(strTmp);

		CStringArray arStr;
		TokenString(strTmp," ", arStr);

		if( arStr.GetSize() < 2 )
		{
			::MessageBox(NULL, "没有数据！", "提示信息", MB_OK|MB_ICONSTOP);
			file.Close();
			return TRUE;
		}

		int nCols = arStr.GetSize();

		for (int i=0; i<nCols; i++)
		{
			if( i== 0 )
				m_logDat.AddLogName("DEPTH",0);
			else
				m_logDat.AddLogName(arStr[i],i);
		}

		while (file.ReadString(strTmp))
		{
			strTmp.TrimLeft();
			strTmp.TrimRight();
			if (strTmp.IsEmpty())
				continue;

			CStringArray arr;
			
			TokenString(strTmp," ",arr);
			
			int nCount = arr.GetSize();
			
			if(nCount>nCols)
			{
				nCount = nCols;
			}

			int j;
			for ( j= 0; j<nCount; j++)
			{
				double val = atof(arr[j]);
				
				if((val > 9998.0 || val <-9998.0))
					val = -99999.0;;
				
				m_logDat.Add(val,j);
			}
			for (j=nCount;j<nCols;j++) //列数不够时
				m_logDat.Add(0.0,j);
		}

		file.Close();
		bRet = TRUE;
	}
	else
	{
		strTmp.TrimLeft();
		strTmp.TrimRight();
		strTmp.MakeUpper();
		if( strTmp.Left(15) == _T("WELL NAME IS : ")) // Well name is : 敖240-42	201
		{
			strTmp.Replace(_T("WELL NAME IS : "),"");
			CString  strWellName = strTmp;
			strWellName = strWellName.Left(strWellName.Find("\t"));
			file.ReadString(strTmp);
			strTmp.TrimLeft();
			strTmp.TrimRight();
			strTmp.MakeUpper();
			if( strTmp.Left(6) != _T("NUMLOG") ) //numlog : 14	Form 1279.899048 to 1370.599121	rlev : 0.050000
			{
				file.Close();
				return FALSE;
			}

			file.ReadString(strTmp);
			file.ReadString(strTmp);

			CStringArray arStr;
			TokenString(strTmp," ", arStr);
			
			if( arStr.GetSize() < 2 )
			{
				::MessageBox(NULL, "没有数据！", "提示信息", MB_OK|MB_ICONSTOP);
				file.Close();
				return TRUE;
			}
			
			int nCols = arStr.GetSize();
			
			for (int i=0; i<nCols; i++)
			{
				if( i== 0 )
					m_logDat.AddLogName("DEPTH",0);
				else
					m_logDat.AddLogName(arStr[i],i);
			}
			
			while (file.ReadString(strTmp))
			{
				strTmp.TrimLeft();
				strTmp.TrimRight();
				if (strTmp.IsEmpty())
					continue;
				
				CStringArray arr;
				
				TokenString(strTmp," ",arr);
				
				int nCount = arr.GetSize();
				
				if(nCount>nCols)
				{
					nCount = nCols;
				}
				
				int j;
				for ( j= 0; j<nCount; j++)
				{
					double val = atof(arr[j]);
					
					if((val > 9998.0 || val <-9998.0))
						val = -99999.0;;
					
					m_logDat.Add(val,j);
				}
				for (j=nCount;j<nCols;j++) //列数不够时
					m_logDat.Add(0.0,j);
			}			
			file.Close();

			m_chartCaption.m_strTitle = strWellName + "测井曲线图";

			bRet = TRUE;
		}
		
		else // 如果不是以上两种格式
		{
			CString sExt = file.GetFileName();
			
			sExt = sExt.Right(sExt.GetLength()-sExt.Find("."));
			sExt.MakeUpper();
			
			if( sExt==".CSV" ) // 是CSV格式
			{
				CStringArray arStr;
				TokenString(strTmp,",", arStr);
				
				if( arStr.GetSize() < 2 )
				{
					::MessageBox(NULL, "没有数据！", "提示信息", MB_OK|MB_ICONSTOP);
					file.Close();
					return TRUE;
				}
				
				int nCols = arStr.GetSize();
				
				for (int i=0; i<nCols; i++)
				{
					if( i== 0 )
						m_logDat.AddLogName("DEPTH",0);
					else
						m_logDat.AddLogName(arStr[i],i);
				}
				
				while (file.ReadString(strTmp))
				{
					strTmp.TrimLeft();
					strTmp.TrimRight();
					if (strTmp.IsEmpty())
						continue;
					
					CStringArray arr;
					
					TokenString(strTmp,",",arr);
					
					int nCount = arr.GetSize();
					
					if(nCount>nCols)
					{
						nCount = nCols;
					}
					
					int j;
					for ( j= 0; j<nCount; j++)
					{
						double val = atof(arr[j]);
						
						if((val > 9998.0 || val <-9998.0))
							val = -99999.0;;
						
						m_logDat.Add(val,j);
					}
					for (j=nCount;j<nCols;j++) //列数不够时
						m_logDat.Add(0.0,j);
				}			
				file.Close();
				bRet = TRUE; 
			}
			else
				bRet = FALSE;
		}
	}

	if( m_logDat.GetSamples() > 1 )
	{
		m_dMinDepth = m_logDat.GetMinDepth();
		m_dMaxDepth = m_logDat.GetMaxDepth();
		
		double dFirstDepth = (long)(floor(m_dMinDepth));
		
		if (dFirstDepth > 0)
			dFirstDepth = (long)(dFirstDepth) / 5 * 5;
		else
			dFirstDepth = ((long)(dFirstDepth) / 5 + ((long)dFirstDepth%5?-1:0)) * 5;
		
		double dEndDepth = ceil(m_dMaxDepth);
		
		dEndDepth = ((long)(dEndDepth) / 5 + ((long)dEndDepth%5? 1:0)) * 5;
		
		m_dMinDepth = dFirstDepth;
		m_dMaxDepth = dEndDepth;
		
		m_rectTitle.left = 0;
		m_rectTitle.right = m_rectTitle.left + 100;
		
		m_rectTitle.top = 50;
		if( m_bDispCaption )		// 图标题是否显示
			m_rectTitle.top += m_chartCaption.m_rectCaption.Height();
		
		m_rectTitle.bottom = m_rectTitle.top + m_iTitleHeight;
		
		m_rectLane.left = 0;
		m_rectLane.right = m_rectLane.left + 100;
		m_rectLane.top = m_rectTitle.bottom;
		m_rectLane.bottom = m_rectLane.top + (dEndDepth-dFirstDepth) * m_dDispScale[m_iCurDispScale];
		
		CRect rect = m_rectLane;
		CLogCurveLane *pLane = new CLogCurveLane(CLogCurveLane::DEPTH,this,rect);
		m_chartLanes.Add((DWORD)pLane);
		
		ReCaleAllRect();
	}
	else
		m_logDat.RemoveAll();

	if( pView != NULL )
	{
		pView->GetDocument()->UpdateAllViews(NULL, HINT_UPDATE_CHANGEDSIZE);
		
		pView->Invalidate(FALSE);
		
		pView->GetDocument()->SetModifiedFlag();
		
		CSplitPath path(strFileName);
		
		pView->GetDocument()->SetTitle(path.GetFileName());
	}

	return bRet;
}

// 读LAS文件格式文件
void CLogChart::LoadLogLasFile(CView *pView, const CString &strFileName)
{
	CStdioFile file;

	if( !file.Open(strFileName, CFile::modeRead|CFile::typeText) )
	{
		CString msg;
		msg.Format("打开 %s 文件失败!",strFileName);

		::MessageBox(NULL, msg, "提示信息", MB_OK|MB_ICONSTOP);

		return;
	}

	CString strTmp;

	BOOL bIsExist = FALSE;
	while (file.ReadString( strTmp ))
	{
		strTmp.TrimRight();
		strTmp.TrimLeft();
		if(strTmp.IsEmpty())
			continue;
		if( strTmp[0]=='#' )
			continue;
		strTmp.MakeUpper();
		
		if( strTmp.Left(6) == "~CURVE")
		{
			bIsExist = TRUE;
			break;
		}
	}
	
	if( !bIsExist )
	{
		file.Close();

		::MessageBox(NULL, "文件中没有曲线！", "提示信息", MB_OK|MB_ICONSTOP);
		
		return;
	}

	int nIndex = 0;
	BOOL bStart = FALSE; // 数据开始
	while (file.ReadString( strTmp ))
	{
		strTmp.TrimRight();
		strTmp.TrimLeft();
		if(strTmp.IsEmpty())
			continue;
		if( strTmp[0]=='#' )
			continue;
		strTmp.MakeUpper();
		
		if(strTmp.Left(1) == "~" )
		{
			if(strTmp.Left(2) == "~A" ) // 如果是~A
			{
				bStart = TRUE;
				strTmp.Replace(_T("~A"),_T(""));
				strTmp.TrimLeft();
				strTmp.TrimRight();

				strTmp.Replace(_T("\t"),_T(" "));
				
				CStringArray arr;
				
				TokenString(strTmp," ",arr);
				
				nIndex = arr.GetSize();
				
				CString strCurveName;
				for (int i=0;i<nIndex;i++)
				{
					strCurveName = arr[i];
					
					if( strCurveName=="DEPT" || strCurveName=="DEPTH")
						m_logDat.AddLogName("DEPTH",i);
					else
					{
						CString str = m_logDat.AddLogName(strCurveName,i);
						if(!str.IsEmpty ())
						{
							CString msg;
							msg.Format(_T("曲线[ %s ]已经存在，系统已经将它改为[ %s ] !"),strCurveName, str);
							::MessageBox(AfxGetMainWnd()->m_hWnd,msg, _T("提示信息"),MB_OK|MB_ICONWARNING);
						}
					}
				}
			}
			break;
		}
	}	

	if( !bStart )
	{
		while (file.ReadString( strTmp ))
		{
			strTmp.TrimRight();
			strTmp.TrimLeft();
			if(strTmp.IsEmpty())
				continue;
			if( strTmp[0]=='#' )
				continue;
			strTmp.MakeUpper();
			
			if(strTmp.Left(2) == "~A" )
			{
				bStart = TRUE;

				strTmp.Replace(_T("~A"),_T(""));
				strTmp.TrimLeft();
				strTmp.TrimRight();
				strTmp.Replace(_T("\t"),_T(" "));
				
				CStringArray arr;
				
				TokenString(strTmp," ",arr);
				
				nIndex = arr.GetSize();
				
				CString strCurveName;
				for (int i=0;i<nIndex;i++)
				{
					strCurveName = arr[i];
					
					if( strCurveName=="DEPT" || strCurveName=="DEPTH")
						m_logDat.AddLogName("DEPTH",i);
					else
					{
						CString str = m_logDat.AddLogName(strCurveName,i);
						if(!str.IsEmpty ())
						{
							CString msg;
							msg.Format(_T("曲线[ %s ]已经存在，系统已经将它改为[ %s ] !"),strCurveName, str);
							::MessageBox(AfxGetMainWnd()->m_hWnd,msg, _T("提示信息"),MB_OK|MB_ICONWARNING);
						}
					}
				}
				break;
			}
		}
	}

	if( !bStart )
	{
		file.Close();
		
		::MessageBox(NULL, "文件中没有曲线数据！", "提示信息", MB_OK|MB_ICONSTOP);
		
		return;
	}

	int nCols = nIndex;
	nIndex = 0;
	while (file.ReadString(strTmp))
	{
		strTmp.TrimLeft();
		strTmp.TrimRight();
		if (strTmp.IsEmpty())
			continue;
		
		strTmp.Replace(_T("\t"),_T(" "));

		CStringArray arr;
		
		TokenString(strTmp," ",arr);
		
		int nCount = arr.GetSize();
		
		int j;
		for ( j= 0; j<nCount; j++)
		{
			double val = atof(arr[j]);
			
			if((val > 9998.0 || val <-9998.0))
				val = -99999.0;;
			
			m_logDat.Add(val,nIndex);
			nIndex ++;
			if( nIndex == nCols )
				nIndex = 0;
		}
	}
	
	file.Close();

	if( m_logDat.GetSamples() > 1 )
	{
		m_dMinDepth = m_logDat.GetMinDepth();
		m_dMaxDepth = m_logDat.GetMaxDepth();
		
		double dFirstDepth = (long)(floor(m_dMinDepth));
		
		if (dFirstDepth > 0)
			dFirstDepth = (long)(dFirstDepth) / 5 * 5;
		else
			dFirstDepth = ((long)(dFirstDepth) / 5 + ((long)dFirstDepth%5?-1:0)) * 5;
		
		double dEndDepth = ceil(m_dMaxDepth);
		
		dEndDepth = ((long)(dEndDepth) / 5 + ((long)dEndDepth%5? 1:0)) * 5;
		
		m_dMinDepth = dFirstDepth;
		m_dMaxDepth = dEndDepth;
		
		m_rectTitle.left = 0;
		m_rectTitle.right = m_rectTitle.left + 100;
		
		m_rectTitle.top = 50;
		if( m_bDispCaption )		// 图标题是否显示
			m_rectTitle.top += m_chartCaption.m_rectCaption.Height();
		
		m_rectTitle.bottom = m_rectTitle.top + m_iTitleHeight;
		
		m_rectLane.left = 0;
		m_rectLane.right = m_rectLane.left + 100;
		m_rectLane.top = m_rectTitle.bottom;
		m_rectLane.bottom = m_rectLane.top + (dEndDepth-dFirstDepth) * m_dDispScale[m_iCurDispScale];
		
		CRect rect = m_rectLane;
		CLogCurveLane *pLane = new CLogCurveLane(CLogCurveLane::DEPTH,this,rect);
		m_chartLanes.Add((DWORD)pLane);
		
		ReCaleAllRect();
	}
	else
		m_logDat.RemoveAll();

	if( pView != NULL )
	{
		pView->GetDocument()->UpdateAllViews(NULL, HINT_UPDATE_CHANGEDSIZE);
		
		pView->Invalidate(FALSE);
		
		pView->GetDocument()->SetModifiedFlag();

		CSplitPath path(strFileName);
		
		pView->GetDocument()->SetTitle(path.GetFileName());
	}

	return ;
}

// 读LIST文件格式文件
void CLogChart::LoadLogListFile(CView *pView, const CString &strFileName)
{
	CStdioFile file;

	if( !file.Open(strFileName, CFile::modeRead|CFile::typeText) )
	{
		CString msg;
		msg.Format("打开 %s 文件失败!",strFileName);

		::MessageBox(NULL, msg, "提示信息", MB_OK|MB_ICONSTOP);

		return;
	}

	CString strTmp;
	file.ReadString( strTmp );

	int nLen = strTmp.GetLength();
	nLen = nLen/10;

	int nJC = -1;			// 夹层标记

	m_logDat.AddLogName("DEPTH",0);
	for (int j=1; j<nLen; j++)
	{
		CString tmp = strTmp.Mid(j*10, 10);
		tmp.TrimRight();
		tmp.TrimLeft();
		
		CString str = tmp;
		str.MakeUpper();
		CString strJC = GetLogName(str);

		if( strJC == _T("INTERLAYER"))  // 如果是夹层
		{
			nJC = j;
		}
		else
			m_logDat.AddLogName(tmp,j);
	}

	file.ReadString(strTmp);
	
	int nCols = nLen;
	double dDepth = 0.0;
	
	while (file.ReadString(strTmp))
	{
		strTmp.TrimLeft();
		strTmp.TrimRight();
		if (strTmp.IsEmpty())
			continue;
		
		CStringArray arr;
		
		TokenString(strTmp," ",arr);
		
		int nCount = arr.GetSize();
		
		int j;
		int nIndex = 0;
		for ( j= 0; j<nCount; j++)
		{
			double val = atof(arr[j]);
			
			if((val > 9998.0 || val <-9998.0))
				val = -99999.0;;

			if( j == 0 )
				dDepth = val;

			if( j==nJC && fabs(val)>0.5 )
				m_arInterLayer.Add(dDepth);
			else
				m_logDat.Add(val,nIndex);

			nIndex ++;
			if( nIndex == nCols )
				break;
		}
	}
	
	file.Close();

	if( m_logDat.GetSamples() > 1 )
	{
		m_dMinDepth = m_logDat.GetMinDepth();
		m_dMaxDepth = m_logDat.GetMaxDepth();
		
		double dFirstDepth = (long)(floor(m_dMinDepth));
		
		if (dFirstDepth > 0)
			dFirstDepth = (long)(dFirstDepth) / 5 * 5;
		else
			dFirstDepth = ((long)(dFirstDepth) / 5 + ((long)dFirstDepth%5?-1:0)) * 5;
		
		double dEndDepth = ceil(m_dMaxDepth);
		
		dEndDepth = ((long)(dEndDepth) / 5 + ((long)dEndDepth%5? 1:0)) * 5;
		
		m_dMinDepth = dFirstDepth;
		m_dMaxDepth = dEndDepth;
		
		m_rectTitle.left = 0;
		m_rectTitle.right = m_rectTitle.left + 100;
		
		m_rectTitle.top = 50;
		if( m_bDispCaption )		// 图标题是否显示
			m_rectTitle.top += m_chartCaption.m_rectCaption.Height();
		
		m_rectTitle.bottom = m_rectTitle.top + m_iTitleHeight;
		
		m_rectLane.left = 0;
		m_rectLane.right = m_rectLane.left + 100;
		m_rectLane.top = m_rectTitle.bottom;
		m_rectLane.bottom = m_rectLane.top + (dEndDepth-dFirstDepth) * m_dDispScale[m_iCurDispScale];
		
		CRect rect = m_rectLane;
		CLogCurveLane *pLane = new CLogCurveLane(CLogCurveLane::DEPTH,this,rect);
		m_chartLanes.Add((DWORD)pLane);
		
		ReCaleAllRect();
	}
	else
		m_logDat.RemoveAll();

	if( pView != NULL )
	{
		pView->GetDocument()->UpdateAllViews(NULL, HINT_UPDATE_CHANGEDSIZE);
		
		pView->Invalidate(FALSE);
		
		pView->GetDocument()->SetModifiedFlag();

		CSplitPath path(strFileName);
		
		pView->GetDocument()->SetTitle(path.GetFileName());
	}

	return ;
}


// 读Asc文件格式文件
void CLogChart::LoadLogAscFile(CView *pView, const CString &strFileName)
{
	CStdioFile file;

	if( !file.Open(strFileName, CFile::modeRead|CFile::typeText) )
	{
		CString msg;
		msg.Format("打开 %s 文件失败!",strFileName);

		::MessageBox(NULL, msg, "提示信息", MB_OK|MB_ICONSTOP);

		return;
	}

	CString strTmp;
	file.ReadString( strTmp );
	file.ReadString( strTmp );
	file.ReadString( strTmp );
	file.ReadString( strTmp );
	strTmp.Trim();

	CStringArray arStr;
	TokenString(strTmp," ", arStr);

	if( arStr.GetSize() < 2 )
	{
		::MessageBox(NULL, "没有数据！", "提示信息", MB_OK|MB_ICONSTOP);
		file.Close();
		return ;
	}

	int nCols = arStr.GetSize();

	int nJC = -1;

	for (int i=0; i<nCols; i++)
	{
		if( i== 0 )
			m_logDat.AddLogName("DEPTH",0);
		else
		{
			CString tmp = arStr[i];
			tmp.Trim();

			CString str = tmp;
			str.MakeUpper();
			CString strJC = GetLogName(str);

			if( strJC == _T("INTERLAYER"))  // 如果是夹层
			{
				nJC = i;
			}
			else
				m_logDat.AddLogName(tmp,i);
		}
	}

	
	while (file.ReadString(strTmp))
	{
		strTmp.TrimLeft();
		strTmp.TrimRight();
		if (strTmp.IsEmpty())
			continue;

		CStringArray arr;

		TokenString(strTmp," ",arr);

		int nCount = arr.GetSize();

		int j;
		int nIndex = 0;
		double dDepth = 0.0;
		for ( j= 0; j<nCount; j++)
		{
			double val = atof(arr[j]);

			if((val > 9998.0 || val <-9998.0))
				val = -99999.0;;

			if( j == 0 )
				dDepth = val;

			if( j==nJC && fabs(val)>0.5 )
				m_arInterLayer.Add(dDepth);
			else
				m_logDat.Add(val,nIndex);

			nIndex ++;
			if( nIndex == nCols )
				break;
		}
	}

	file.Close();

	if( m_logDat.GetSamples() > 1 )
	{
		m_dMinDepth = m_logDat.GetMinDepth();
		m_dMaxDepth = m_logDat.GetMaxDepth();

		double dFirstDepth = (long)(floor(m_dMinDepth));

		if (dFirstDepth > 0)
			dFirstDepth = (long)(dFirstDepth) / 5 * 5;
		else
			dFirstDepth = ((long)(dFirstDepth) / 5 + ((long)dFirstDepth%5?-1:0)) * 5;

		double dEndDepth = ceil(m_dMaxDepth);

		dEndDepth = ((long)(dEndDepth) / 5 + ((long)dEndDepth%5? 1:0)) * 5;

		m_dMinDepth = dFirstDepth;
		m_dMaxDepth = dEndDepth;

		m_rectTitle.left = 0;
		m_rectTitle.right = m_rectTitle.left + 100;

		m_rectTitle.top = 50;
		if( m_bDispCaption )		// 图标题是否显示
			m_rectTitle.top += m_chartCaption.m_rectCaption.Height();

		m_rectTitle.bottom = m_rectTitle.top + m_iTitleHeight;

		m_rectLane.left = 0;
		m_rectLane.right = m_rectLane.left + 100;
		m_rectLane.top = m_rectTitle.bottom;
		m_rectLane.bottom = m_rectLane.top + (dEndDepth-dFirstDepth) * m_dDispScale[m_iCurDispScale];

		CRect rect = m_rectLane;
		CLogCurveLane *pLane = new CLogCurveLane(CLogCurveLane::DEPTH,this,rect);
		m_chartLanes.Add((DWORD)pLane);

		ReCaleAllRect();
	}
	else
		m_logDat.RemoveAll();

	if( pView != NULL )
	{
		pView->GetDocument()->UpdateAllViews(NULL, HINT_UPDATE_CHANGEDSIZE);

		pView->Invalidate(FALSE);

		pView->GetDocument()->SetModifiedFlag();

		CSplitPath path(strFileName);

		pView->GetDocument()->SetTitle(path.GetFileName());
	}

	return ;
}
