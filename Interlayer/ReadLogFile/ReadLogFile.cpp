// ReadLogFile.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxdllx.h>
#include "ReadLogFile.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static AFX_EXTENSION_MODULE ReadLogFileDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("READLOGFILE.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(ReadLogFileDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(ReadLogFileDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("READLOGFILE.DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(ReadLogFileDLL);
	}
	return 1;   // ok
}

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
int TokenString(LPCTSTR lpszString, 
				 LPCTSTR lpszDelimiters,
				 CStringArray& saTokens,
				 int nMaxTokens = 0,
				 BOOL bTrimToken = TRUE,
				 BOOL bEnableEscapedChars = TRUE,
				 BOOL bEnableDoubleQuote = FALSE,
				 BOOL bReturnEmptyToken = FALSE);

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
				 int nMaxTokens /*= 0*/,
				 BOOL bTrimToken /*= TRUE*/,
				 BOOL bEnableEscapedChars /*= TRUE*/,
				 BOOL bEnableDoubleQuote /*= TRUE*/,
				 BOOL bReturnEmptyToken /*= TRUE*/)
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

// 是否为716格式
// 是否按实际曲线条数保存的
BOOL Is716Format(LPCSTR name, BOOL &bIsBigEndian, BOOL &bCurves, long &lReadPos)
{
	BOOL bRet = TRUE;
	CFile file;
	if(file.Open(name,CFile::modeRead))
	{
		ULONGLONG nLen = file.GetLength();//文件长度
		
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
BOOL IsTextFile(LPCSTR lpstrFile)
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
		//::MessageBox(AfxGetMainWnd()->m_hWnd,"读文件错误！","提示信息",MB_OK|MB_ICONERROR);
		return FALSE;
	}
	END_CATCH
}

// 判断文件是否为ASC文本文件
BOOL IsAscFile(LPCSTR lpstrFile)
{
	// 	茂X22-斜18	216                                                                  
	// 	icurve     sdep         edep          cyjj         cyds
	// 	12  1512.780029  1794.130127     0.050000    62.000000
	// 	DEP          CI        GR        LLD       LLS       HAC       RMG       RMN       CALC      MSFL      AC        R250      SP  
	// 	1512.780     6.926   114.814    11.780    11.670   406.242     4.452     3.711    22.628     0.000   415.522    10.226    34.770

	CStdioFile file;

	if( !file.Open(lpstrFile, CFile::modeRead|CFile::typeText) )
	{
		//CString msg;
		//msg.Format("打开 %s 文件失败!",lpstrFile);

		//::MessageBox(NULL, msg, "提示信息", MB_OK|MB_ICONSTOP);

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
BOOL IsLasFile(LPCSTR lpstrFile)
{
	CStdioFile file;
	
	if( !file.Open(lpstrFile, CFile::modeRead|CFile::typeText) )
	{
		CString msg;
		msg.Format("打开 %s 文件失败!",lpstrFile);
		
		//::MessageBox(NULL, msg, "提示信息", MB_OK|MB_ICONSTOP);
		
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

BOOL IsListFile(LPCSTR lpstrFile)
{
	CStdioFile file;
	
	if( !file.Open(lpstrFile, CFile::modeRead|CFile::typeText) )
	{
		CString msg;
		msg.Format("打开 %s 文件失败!",lpstrFile);
		
		//::MessageBox(NULL, msg, "提示信息", MB_OK|MB_ICONSTOP);
		
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

BOOL GetFieldsLoging(LPCSTR lpszFileName, CStringArray &arLogingName)
{
	if( IsTextFile (lpszFileName))
	{
		if( IsLasFile(lpszFileName) ) // 是LAS文件格式吗
		{
			return GetFieldsFromLasFile(lpszFileName, arLogingName);
		}
		
		if( IsListFile(lpszFileName) ) // 是LIST文件格式吗
		{
			return GetFieldsFromListFile(lpszFileName, arLogingName);
		}

		if( IsAscFile(lpszFileName) ) // 是ASC文件格式
		{
			return GetFieldsFromAscFile(lpszFileName, arLogingName);
		}

		return GetFieldsFromTxt(lpszFileName, arLogingName); // 三种格式
	}
	else
	{
		BOOL bBigEndian, bCurves;
		long lReadPos;
		if( !Is716Format(lpszFileName,bBigEndian,bCurves,lReadPos) )
		{
			return FALSE;
		}
		return GetFieldsFrom716File(lpszFileName,arLogingName, bBigEndian,bCurves,lReadPos);
	}

	return FALSE;
}

// List
BOOL GetFieldsFromListFile(LPCSTR lpszFileName, CStringArray &arLogingName)
{
	CStdioFile file;

	if( !file.Open(lpszFileName, CFile::modeRead|CFile::typeText) )
	{
		return FALSE;
	}

	CString strTmp;
	file.ReadString( strTmp );

	int nLen = strTmp.GetLength();
	nLen = nLen/10;

	for (int j=1; j<nLen; j++)
	{
		CString tmp = strTmp.Mid(j*10, 10);
		tmp.TrimRight();
		tmp.TrimLeft();
		arLogingName.Add(tmp);
	}

	file.Close();

	return TRUE;
}

// ASC
BOOL GetFieldsFromAscFile(LPCSTR lpszFileName, CStringArray &arLogingName)
{
	CStdioFile file;

	if( !file.Open(lpszFileName, CFile::modeRead|CFile::typeText) )
	{
		return FALSE;
	}

	CString strTmp;
	file.ReadString( strTmp );
	file.ReadString( strTmp );
	file.ReadString( strTmp );
	file.ReadString( strTmp );
	strTmp.Trim();

	CStringArray arStr;
	TokenString(strTmp," ", arStr);

	int nCols = arStr.GetSize();

	for (int i=1; i<nCols; i++)
		arLogingName.Add(arStr[i]);

	file.Close();

	return TRUE;
}



// 读LAS文件格式文件
BOOL GetFieldsFromLasFile(LPCSTR lpszFileName, CStringArray &arLogingName)
{
	CStdioFile file;

	if( !file.Open(lpszFileName, CFile::modeRead|CFile::typeText) )
	{
		return FALSE;
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
		return FALSE;
	}

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
				strTmp.Replace(_T("~A"),_T(""));
				strTmp.TrimLeft();
				strTmp.TrimRight();

				strTmp.Replace(_T("\t"),_T(" "));
				
				CStringArray arr;
				
				TokenString(strTmp," ",arr);
				
				int nIndex = arr.GetSize();
				
				CString strCurveName;
				for (int i=0;i<nIndex;i++)
				{
					strCurveName = arr[i];
					
					if( strCurveName=="DEPT" || strCurveName=="DEPTH")
						continue;
					else
						arLogingName.Add(strCurveName);
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
				
				int nIndex = arr.GetSize();
				
				CString strCurveName;
				for (int i=0;i<nIndex;i++)
				{
					strCurveName = arr[i];
					
					if( strCurveName=="DEPT" || strCurveName=="DEPTH")
						continue;
					else
						arLogingName.Add(strCurveName);
				}

				break;
			}
		}
	}

	file.Close();

	return TRUE;
}


BOOL GetFieldsFromTxt(LPCSTR lpszFileName, CStringArray &arLogingName)
{
	CStdioFile file;

	if( !file.Open(lpszFileName, CFile::modeRead|CFile::typeText) )
	{
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
			file.Close();
			return FALSE;
		}

		int nCols = arStr.GetSize();

		for (int i=1; i<nCols; i++)
			arLogingName.Add(arStr[i]);

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
				return FALSE;
			
			int nCols = arStr.GetSize();
			
			for (int i=1; i<nCols; i++)
				arLogingName.Add(arStr[i]);
			
						
			file.Close();

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
					file.Close();
					return TRUE;
				}
				
				int nCols = arStr.GetSize();
				
				for (int i=1; i<nCols; i++)
					arLogingName.Add(arStr[i]);			
							
				file.Close();
				bRet = TRUE; 
			}
			else
				bRet = FALSE;
		}
	}

	return bRet;
}


BOOL GetFieldsFrom716File(LPCSTR lpszFileName, CStringArray &arLogingName, BOOL bIsBigEndian, BOOL bCurves, long lReadPos)
{
	char buff[5]="";
	CFile file;
	int nCount; //曲线数
	CString tmp, sql;

	if(!file.Open(lpszFileName,CFile::modeRead|CFile::typeBinary))
		return FALSE;
	
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
			//::MessageBox(AfxGetMainWnd()->m_hWnd,"716文件格式有错误!","提示信息",MB_OK|MB_ICONERROR);
			return FALSE;
		}
		arLogingName.Add(tmp);//曲线名称
	}

	file.Close();

	return TRUE;
}


BOOL LoadDatTxt(const CString &strFileName, CStringArray &arDats)
{
	CStdioFile file;

	if( !file.Open(strFileName, CFile::modeRead|CFile::typeText) )
	{
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
			return FALSE;
		}

		int nCols = arStr.GetSize();

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
// 				double val = atof(arr[j]);
// 				
// 				if((val > 9998.0 || val <-9998.0))
// 					val = -99999.0;;
				
				arDats.Add(arr[j]);
			}
			for (j=nCount;j<nCols;j++) //列数不够时
				arDats.Add("");
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
				return FALSE;
			}
			
			int nCols = arStr.GetSize();
			
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
// 					double val = atof(arr[j]);
// 					
// 					if((val > 9998.0 || val <-9998.0))
// 						val = -99999.0;;
					
					arDats.Add(arr[j]);
				}
				for (j=nCount;j<nCols;j++) //列数不够时
					arDats.Add("");
			}			
			file.Close();

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
					return FALSE;
				}
				
				int nCols = arStr.GetSize();				
				
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
// 						double val = atof(arr[j]);
// 						
// 						if((val > 9998.0 || val <-9998.0))
// 							val = -99999.0;;
						
						arDats.Add(arr[j]);
					}
					for (j=nCount;j<nCols;j++) //列数不够时
						arDats.Add("");
				}			
				file.Close();
				bRet = TRUE; 
			}
			else
				bRet = FALSE;
		}
	}

	return bRet;
}

// 读LAS文件格式文件
BOOL LoadLasFile(const CString &strFileName, CStringArray &arDats)
{
	CStdioFile file;

	if( !file.Open(strFileName, CFile::modeRead|CFile::typeText) )
	{
		return FALSE;
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
		return FALSE;
	}

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
				bStart = TRUE;
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
				break;
			}
		}
	}

	if( !bStart )
	{
		file.Close();		
		return FALSE;
	}

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
// 			double val = atof(arr[j]);
// 			
// 			if((val > 9998.0 || val <-9998.0))
// 				val = -99999.0;;
			
			arDats.Add(arr[j]);
		}
	}
	
	file.Close();

	return TRUE;
}

// 读LIST文件格式文件
BOOL LoadListFile( const CString &strFileName, CStringArray &arDats)
{
	CStdioFile file;

	if( !file.Open(strFileName, CFile::modeRead|CFile::typeText) )
	{
		return FALSE;
	}

	CString strTmp;
	file.ReadString( strTmp );

	file.ReadString(strTmp);
	
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
		for ( j= 0; j<nCount; j++)
		{
// 			double val = atof(arr[j]);
// 			
// 			if((val > 9998.0 || val <-9998.0))
// 				val = -99999.0;;
			
			arDats.Add(arr[j]);
		}
	}
	
	file.Close();

	return TRUE;
}


// 读ASc文件格式文件
BOOL LoadAscFile( const CString &strFileName, CStringArray &arDats)
{
	CStdioFile file;

	if( !file.Open(strFileName, CFile::modeRead|CFile::typeText) )
	{
		return FALSE;
	}

	CString strTmp;
	file.ReadString( strTmp );
	file.ReadString( strTmp );
	file.ReadString( strTmp );
	file.ReadString( strTmp );

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
		for ( j= 0; j<nCount; j++)
		{
			// 			double val = atof(arr[j]);
			// 			
			// 			if((val > 9998.0 || val <-9998.0))
			// 				val = -99999.0;;

			arDats.Add(arr[j]);
		}
	}

	file.Close();

	return TRUE;
}

BOOL Load716File(const CString &fileName,CStringArray &arDats, BOOL bIsBigEndian, BOOL bCurves, long lReadPos)
{
	char buff[5]="";
	CFile file;
	int nCount; //曲线数
	CString tmp;
	if(!file.Open(fileName,CFile::modeRead|CFile::typeBinary))
		return FALSE;
	
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
			return FALSE;
		}
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
			return FALSE;
		}
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
	
	int nSampls = ceilf((endp - stdp)/relv) + 1;//总样点数
	GetInt(file, bIsBigEndian);
	GetInt(file, bIsBigEndian);
	
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

	double depth = stdp;
	CString strTmp;
	for(i=0;i<nSampls; i++)
	{				
		depth += relv;
		strTmp.Format("%.3lf",depth);
		arDats.Add(strTmp);

		for(int j=0; j < nCount; j++)
		{
			strTmp.Format("%.3lf",arValues[j][i]);
			arDats.Add(strTmp);
		}		
	}	
	
	return TRUE;
}

BOOL GetDatFromFile(LPCSTR lpszFileName, CStringArray &arDats)
{
	if( IsTextFile (lpszFileName))
	{
		if( IsLasFile(lpszFileName) ) // 是LAS文件格式吗
		{
			return LoadLasFile(lpszFileName, arDats);
		}
		
		if( IsListFile(lpszFileName) ) // 是LIST文件格式吗
		{
			return LoadListFile(lpszFileName, arDats);
		}

		if( IsAscFile(lpszFileName) ) // 是Asc文件格式吗
		{
			return LoadAscFile(lpszFileName, arDats);
		}

		return LoadDatTxt(lpszFileName, arDats); // 三种格式
	}
	else
	{
		BOOL bBigEndian, bCurves;
		long lReadPos;
		if( !Is716Format(lpszFileName,bBigEndian,bCurves,lReadPos) )
		{
			return FALSE;
		}
		return Load716File(lpszFileName,arDats, bBigEndian,bCurves,lReadPos);
	}

	return FALSE;
}

//------------------------------------------------------------------------
BOOL LoadDatTxt(const CString &strFileName, CArray<double, double> &arDats)
{
	CStdioFile file;

	if( !file.Open(strFileName, CFile::modeRead|CFile::typeText) )
	{
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
			return FALSE;
		}

		int nCols = arStr.GetSize();

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

				arDats.Add(val);
			}
			for (j=nCount;j<nCols;j++) //列数不够时
				arDats.Add(-99999.0);
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
				return FALSE;
			}

			int nCols = arStr.GetSize();

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

					arDats.Add(val);
				}
				for (j=nCount;j<nCols;j++) //列数不够时
					arDats.Add(-99999.0);
			}			
			file.Close();

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
					return FALSE;
				}

				int nCols = arStr.GetSize();				

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

						arDats.Add(val);
					}
					for (j=nCount;j<nCols;j++) //列数不够时
						arDats.Add(-99999.0);
				}			
				file.Close();
				bRet = TRUE; 
			}
			else
				bRet = FALSE;
		}
	}

	return bRet;
}

// 读LAS文件格式文件
BOOL LoadLasFile(const CString &strFileName, CArray<double, double> &arDats)
{
	CStdioFile file;

	if( !file.Open(strFileName, CFile::modeRead|CFile::typeText) )
	{
		return FALSE;
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
		return FALSE;
	}

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
				bStart = TRUE;
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
				break;
			}
		}
	}

	if( !bStart )
	{
		file.Close();		
		return FALSE;
	}

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

			arDats.Add(val);
		}
	}

	file.Close();

	return TRUE;
}

// 读LIST文件格式文件
BOOL LoadListFile( const CString &strFileName, CArray<double, double> &arDats)
{
	CStdioFile file;

	if( !file.Open(strFileName, CFile::modeRead|CFile::typeText) )
	{
		return FALSE;
	}

	CString strTmp;
	file.ReadString( strTmp );

	file.ReadString(strTmp);

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
		for ( j= 0; j<nCount; j++)
		{
			double val = atof(arr[j]);
					
			if((val > 9998.0 || val <-9998.0))
				val = -99999.0;;

			arDats.Add(val);
		}
	}

	file.Close();

	return TRUE;
}


// 读ASc文件格式文件
BOOL LoadAscFile( const CString &strFileName, CArray<double, double> &arDats)
{
	CStdioFile file;

	if( !file.Open(strFileName, CFile::modeRead|CFile::typeText) )
	{
		return FALSE;
	}

	CString strTmp;
	file.ReadString( strTmp );
	file.ReadString( strTmp );
	file.ReadString( strTmp );
	file.ReadString( strTmp );

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
		for ( j= 0; j<nCount; j++)
		{
			double val = atof(arr[j]);
				
			if((val > 9998.0 || val <-9998.0))
				val = -99999.0;;

			arDats.Add(val);
		}
	}

	file.Close();

	return TRUE;
}

BOOL Load716File(const CString &fileName, CArray<double, double> &arDats, BOOL bIsBigEndian, BOOL bCurves, long lReadPos)
{
	char buff[5]="";
	CFile file;
	int nCount; //曲线数
	CString tmp;
	if(!file.Open(fileName,CFile::modeRead|CFile::typeBinary))
		return FALSE;

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
			return FALSE;
		}
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
			return FALSE;
		}
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

	int nSampls = ceilf((endp - stdp)/relv) + 1;//总样点数
	GetInt(file, bIsBigEndian);
	GetInt(file, bIsBigEndian);

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

	double depth = stdp;
	for(i=0;i<nSampls; i++)
	{				
		depth += relv;
		arDats.Add(depth);

		for(int j=0; j < nCount; j++)
			arDats.Add(arValues[j][i]);
	}	

	return TRUE;
}

BOOL GetDatFromFile(LPCSTR lpszFileName,  CArray<double, double> &arDats)
{
	if( IsTextFile (lpszFileName))
	{
		if( IsLasFile(lpszFileName) ) // 是LAS文件格式吗
		{
			return LoadLasFile(lpszFileName, arDats);
		}

		if( IsListFile(lpszFileName) ) // 是LIST文件格式吗
		{
			return LoadListFile(lpszFileName, arDats);
		}

		if( IsAscFile(lpszFileName) ) // 是Asc文件格式吗
		{
			return LoadAscFile(lpszFileName, arDats);
		}

		return LoadDatTxt(lpszFileName, arDats); // 三种格式
	}
	else
	{
		BOOL bBigEndian, bCurves;
		long lReadPos;
		if( !Is716Format(lpszFileName,bBigEndian,bCurves,lReadPos) )
		{
			return FALSE;
		}
		return Load716File(lpszFileName,arDats, bBigEndian,bCurves,lReadPos);
	}

	return FALSE;
}

//------------------------------------------------------------------------
BOOL WriteLogCSV(LPCSTR lpszFileName, const CStringArray &arLogingName, const CStringArray &arDats)
{
	CStdioFile file;
	
	if( !file.Open(lpszFileName, CFile::modeWrite|CFile::modeCreate|CFile::typeText) )
	{
		return FALSE;
	}
	int n = arLogingName.GetSize();
	int i;
	file.WriteString(_T("DEPTH"));
	for( i=0;i<n;i++)
	{
		file.WriteString(_T(","));
		file.WriteString(arLogingName[i]);
	}
	file.WriteString(_T("\n"));

	int m = arDats.GetSize();

	for( i=0;i<m;i+=(n+1))
	{
		for (int j=0;j<= n; j++)
		{
			file.WriteString(arDats[i+j]);
			if( j<n)
				file.WriteString(_T(","));
			else
				file.WriteString(_T("\n"));
		}		
	}
	
	file.Close();

	return TRUE;
}


BOOL WriteLogWIS(LPCSTR lpszFileName, const CStringArray &arLogingName, const CStringArray &arDats)
{
	CStdioFile file;
	
	if( !file.Open(lpszFileName, CFile::modeWrite|CFile::modeCreate|CFile::typeText) )
	{
		return FALSE;
	}

	file.WriteString(_T("FORWARD_TEXT_FORMAT_1.0\n"));
	CString strTmp;
	strTmp.Format(_T("STDEP = %10s\n"),arDats[0]);
	file.WriteString(strTmp);
	strTmp.Format(_T("ENDEP = %10s\n"),arDats[arDats.GetSize()-arLogingName.GetSize()-1]);
	file.WriteString(strTmp);

	strTmp.Format(_T("RLEV  = %10.3f\n"),atof(arDats[0])-atof(arDats[arLogingName.GetSize()+1]));;
	file.WriteString(strTmp);

	int n = arLogingName.GetSize();
	int i;
	file.WriteString(_T("CURVENAME =  "));
	for( i=0;i<n;i++)
	{
		if( i>0)
			file.WriteString(_T(","));
		file.WriteString(arLogingName[i]);
	}
	file.WriteString(_T("\n"));

	file.WriteString(_T("END\n"));

	file.WriteString(_T("#DEPTH    "));
	for( i=0;i<n;i++)
	{
		strTmp.Format(_T(" %10s"),arLogingName[i]);;
		file.WriteString(strTmp);
	}

	file.WriteString(_T("\n"));

	
	int m = arDats.GetSize();
	
	for( i=0;i<m;i+=(n+1))
	{
		for (int j=0;j<= n; j++)
		{
			if( j<n)
				strTmp.Format(_T("%10s "),arDats[i+j]);
			else
				strTmp.Format(_T("%10s\n"),arDats[i+j]);;
			file.WriteString(strTmp);
		}		
	}
	
	file.Close();
	
	return TRUE;
}


BOOL WriteLogLAS(LPCSTR lpszFileName, const CStringArray &arLogingName, const CStringArray &arDats)
{
	CStdioFile file;
	
	if( !file.Open(lpszFileName, CFile::modeWrite|CFile::modeCreate|CFile::typeText) )
	{
		return FALSE;
	}
	int n = arLogingName.GetSize();
	int i;
	file.WriteString(_T("DEPTH"));
	for( i=0;i<n;i++)
	{
		file.WriteString(_T(","));
		file.WriteString(arLogingName[i]);
	}
	file.WriteString(_T("\n"));
	
	int m = arDats.GetSize();
	
	for( i=0;i<m;i+=(n+1))
	{
		for (int j=0;j<= n; j++)
		{
			file.WriteString(arDats[i+j]);
			if( j<n)
				file.WriteString(_T(","));
			else
				file.WriteString(_T("\n"));
		}		
	}
	
	file.Close();
	
	return TRUE;
}