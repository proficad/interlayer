// DlgImportWellInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "Interlayer.h"
#include "DlgImportWellInfo.h"
#include "WellInfoDoc.h"
#include "WellInfoView.h"
#include <map>
#include <string>
#include "../ShapeDll/GridCellShape/GridCellShape.h"
// CDlgImportWellInfo 对话框

IMPLEMENT_DYNAMIC(CDlgImportWellInfo, CDialog)

CDlgImportWellInfo::CDlgImportWellInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgImportWellInfo::IDD, pParent)
	, m_strText(_T(""))
	, m_strSeq(_T(","))
	, m_nSeq(0)
	, m_bHeader(FALSE)
	, m_strLoadFileName(_T(""))
	, m_uLine(1)
	, m_iFileType(0)
{

}

CDlgImportWellInfo::~CDlgImportWellInfo()
{
}

void CDlgImportWellInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TEXT, m_strText);
	DDX_Text(pDX, IDC_EDIT_CH, m_strSeq);
	DDX_Radio(pDX, IDC_RADIO_COMMA, m_nSeq);
	DDX_Check(pDX, IDC_CHECK_FIRST_LINE, m_bHeader);
	DDX_Control(pDX, IDC_COMBO_SHEET, m_wndSheet);
	DDX_Text(pDX, IDC_EDIT_FILE, m_strLoadFileName);
	DDX_Text(pDX, IDC_EDIT_LINE, m_uLine);
	DDV_MinMaxUInt(pDX, m_uLine, 1, 200);
	DDX_Radio(pDX, IDC_TXT_FILE, m_iFileType);
	DDX_GridControl(pDX, IDC_GRID_FIELDS, m_wndGridFields);
}


BEGIN_MESSAGE_MAP(CDlgImportWellInfo, CDialog)
	ON_BN_CLICKED(IDC_BTN_FILE, &CDlgImportWellInfo::OnBnClickedBtnFile)
	ON_CBN_SELENDOK(IDC_COMBO_SHEET, OnSelendokComboSheet)
	ON_BN_CLICKED(IDC_CHECK_FIRST_LINE, OnCheckFirstLine)
	ON_EN_KILLFOCUS(IDC_EDIT_CH, OnKillfocusEditCh)
	ON_BN_CLICKED(IDC_RADIO_COMMA, OnRadioSeq)
	ON_BN_CLICKED(IDC_RADIO_TAB, OnRadioSeq)	
	ON_BN_CLICKED(IDC_RADIO_SPACE, OnRadioSeq)
	ON_BN_CLICKED(IDC_RADIO_OTHER, OnRadioSeq)
	ON_BN_CLICKED(IDC_TXT_FILE, &CDlgImportWellInfo::OnBnClickedFileType)
	ON_BN_CLICKED(IDC_EXCEL_FILE, &CDlgImportWellInfo::OnBnClickedFileType)
	ON_BN_CLICKED(IDC_DBF_FILE, &CDlgImportWellInfo::OnBnClickedFileType)
	ON_BN_CLICKED(IDC_MDB_FILE, &CDlgImportWellInfo::OnBnClickedFileType)
	ON_BN_CLICKED(IDOK, &CDlgImportWellInfo::OnBnClickedOk)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID_FIELDS, OnFieldGridEndEdit)
END_MESSAGE_MAP()


// CDlgImportWellInfo 消息处理程序
char* Utf8toAnsi( const char * utf8, int len )
{

	char *ansistr = NULL;
	int length = MultiByteToWideChar(CP_UTF8, 0, utf8, len, NULL, NULL );
	WCHAR *lpszW = NULL;

	lpszW = new WCHAR[length+1];
	ansistr = ( char * ) calloc ( sizeof(char), length+5 );

	//this step intended only to use WideCharToMultiByte
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, lpszW, length );

	//Conversion to ANSI (CP_ACP)
	WideCharToMultiByte(CP_ACP, 0, lpszW, -1, ansistr, length, NULL, NULL);

	ansistr[length] = 0;

	delete[] lpszW;

	return ansistr;
}

int fileUTF8toANSI(const CString &str1, const CString &str2)
{
	try
	{
		// open the UNICODE file
		CFile file (str1, CFile::modeRead);
		int iFileLen = file.GetLength ();
		PSTR buffer = new char[iFileLen];
		PSTR pMultiByteStr = new char[iFileLen];

		//memset(pMultiByteStr, '\0', iFileLen);
		BOOL bUsedDefaultChar;
		file.Read (buffer, iFileLen);
		// convert to ANSI string

		CString tmp = Utf8toAnsi((buffer+3), iFileLen*2-6);
		file.Close();
		// create the ANSI file
		file.Open( str2, CFile::modeCreate | CFile::modeWrite);

		file.Write(tmp.GetBuffer(), tmp.GetLength());
		file.Close();
		delete[] buffer;
		delete[] pMultiByteStr;
	}
	catch (CFileException* e)
	{
		TCHAR szCause[255];
		e->GetErrorMessage(szCause, 255);
		AfxMessageBox(szCause);
		e->ReportError ();
		e->Delete ();
	}
	return 0;
}
int fileUNICODEtoANSI(const CString &str1, const CString &str2)
{
	try
	{
		// open the UNICODE file
		CFile file (str1, CFile::modeRead);
		int iFileLen = file.GetLength ();
		PSTR buffer = new char[iFileLen];
		PSTR pMultiByteStr = new char[iFileLen];

		memset(pMultiByteStr, '\0', iFileLen);
		BOOL bUsedDefaultChar;
		file.Read (buffer, iFileLen);
		// convert to ANSI string
		
		WideCharToMultiByte(CP_ACP, 0, (LPCWSTR) (buffer+2), -1, pMultiByteStr, iFileLen, NULL, &bUsedDefaultChar);
		file.Close();
		// create the ANSI file
		file.Open( str2, CFile::modeCreate | CFile::modeWrite);
		int index =0;
		while(pMultiByteStr[index]!='?'&&pMultiByteStr[index]!='\0')
		{
			index++;
		}
		file.Write(pMultiByteStr, index);
		file.Close();
		delete[] buffer;
		delete[] pMultiByteStr;
	}
	catch (CFileException* e)
	{
		TCHAR szCause[255];
		e->GetErrorMessage(szCause, 255);
		AfxMessageBox(szCause);
		e->ReportError ();
		e->Delete ();
	}
	return 0;
}  

class CStdioFileEx: public CStdioFile
{
public:
	CStdioFileEx();
	CStdioFileEx( LPCTSTR lpszFileName, UINT nOpenFlags );

	virtual BOOL Open( LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError = NULL );
	virtual BOOL ReadString(CString& rString);
	BOOL ReadWideString(CStringW& rString);
	BOOL ReadAnsiString(CStringA& rString);
	virtual void WriteString(LPCTSTR lpsz);
	void WriteWideString(LPCWSTR lpsz);
	void WriteAnsiString(LPCSTR lpsz);
	bool IsUnicodeFormat() {return m_bIsUnicodeText;}
	unsigned long GetCharCount();

	// Additional flag to allow Unicode text format writing
	enum {modeWriteUnicode = 0x100000};

	static bool IsFileUnicode(const CString& sFilePath);
	static bool IsFileUTF8code(const CString& sFilePath);

protected:
	UINT PreprocessFlags(const CString& sFilePath, UINT& nOpenFlags);

	bool    m_bIsUnicodeText;
};
#define UNICODE_BOM        0xFEFF//Unicode "byte order mark" which goes at start of file

CStdioFileEx::CStdioFileEx(): CStdioFile()
{
	m_bIsUnicodeText = false;
}

CStdioFileEx::CStdioFileEx(LPCTSTR lpszFileName,UINT nOpenFlags)
:CStdioFile(lpszFileName, PreprocessFlags(lpszFileName, nOpenFlags))
{
}

BOOL CStdioFileEx::Open(LPCTSTR lpszFileName,UINT nOpenFlags,CFileException* pError /*=NULL*/)
{
	PreprocessFlags(lpszFileName, nOpenFlags);

	return CStdioFile::Open(lpszFileName, nOpenFlags, pError);
}

BOOL CStdioFileEx::ReadString(CString& rString)
{
#ifdef _UNICODE
	return ReadWideString(rString);
#else
	return ReadAnsiString(rString);
#endif
}

BOOL CStdioFileEx::ReadWideString(CStringW& rString)
{
	_ASSERTE(m_pStream);
	rString = L"";      // empty string without deallocating

	if(m_bIsUnicodeText)
	{
		// If at position 0, discard byte-order mark before reading
		if(GetPosition() == 0)
		{
			wchar_t bom;
			Read(&bom, sizeof(wchar_t));
		}
		const int nMaxSize = 128;
		LPWSTR lpsz = rString.GetBuffer(nMaxSize);
		LPWSTR lpszResult;
		int nLen = 0;
		for (;;)
		{
			lpszResult = fgetws(lpsz, nMaxSize+1, m_pStream);
			rString.ReleaseBuffer();

			// handle error/eof case
			if (lpszResult == NULL && !feof(m_pStream))
			{
				Afx_clearerr_s(m_pStream);
				AfxThrowFileException(CFileException::genericException, _doserrno,
					m_strFileName);
			}

			// if string is read completely or EOF
			if (lpszResult == NULL ||
				(nLen = (int)lstrlenW(lpsz)) < nMaxSize ||
				lpsz[nLen-1] == '\n')
				break;

			nLen = rString.GetLength();
			lpsz = rString.GetBuffer(nMaxSize + nLen) + nLen;
		}
		//remove crlf if exist.
		nLen = rString.GetLength();
		if (nLen > 1 && rString.Mid(nLen-2) == L"\r\n")
		{
			rString.GetBufferSetLength(nLen-2);
		}
		return rString.GetLength() > 0;
	}
	else
	{
		CStringA ansiString;
		BOOL bRetval = ReadAnsiString(ansiString);
		//setlocale(LC_ALL, "chs_chn.936");//no need
		rString = ansiString;
		return bRetval;
	}
}

BOOL CStdioFileEx::ReadAnsiString(CStringA& rString)
{
	_ASSERTE(m_pStream);
	rString = "";      // empty string without deallocating

	if(!m_bIsUnicodeText)
	{
		const int nMaxSize = 128;
		LPSTR lpsz = rString.GetBuffer(nMaxSize);
		LPSTR lpszResult;
		int nLen = 0;
		for (;;)
		{
			lpszResult = fgets(lpsz, nMaxSize+1, m_pStream);
			rString.ReleaseBuffer();

			// handle error/eof case
			if (lpszResult == NULL && !feof(m_pStream))
			{
				Afx_clearerr_s(m_pStream);
				AfxThrowFileException(CFileException::genericException, _doserrno,
					m_strFileName);
			}

			// if string is read completely or EOF
			if (lpszResult == NULL ||
				(nLen = (int)lstrlenA(lpsz)) < nMaxSize ||
				lpsz[nLen-1] == '\n')
				break;

			nLen = rString.GetLength();
			lpsz = rString.GetBuffer(nMaxSize + nLen) + nLen;
		}
		//remove crlf if exist.
		nLen = rString.GetLength();
		if (nLen > 1 && rString.Mid(nLen-2) == "\r\n")
		{
			rString.GetBufferSetLength(nLen-2);
		}
		return rString.GetLength() > 0;
	}
	else
	{
		CStringW wideString;
		BOOL bRetval = ReadWideString(wideString);
		//setlocale(LC_ALL, "chs_chn.936");//no need
		rString = wideString;
		return bRetval;
	}
}

// Purpose:    Writes string to file either in Unicode or multibyte, depending on whether the caller specified the
//       CStdioFileEx::modeWriteUnicode flag. Override of base class function.
void CStdioFileEx::WriteString(LPCTSTR lpsz)
{
#ifdef _UNICODE
	WriteWideString(lpsz);
#else
	WriteAnsiString(lpsz);
#endif
}

void CStdioFileEx::WriteWideString(LPCWSTR lpsz)
{
	ASSERT(lpsz != NULL);

	if (lpsz == NULL)
	{
		AfxThrowInvalidArgException();
	}
	if(m_bIsUnicodeText)
	{
		ASSERT(m_pStream != NULL);
		// If writing Unicode and at the start of the file, need to write byte mark
		if(GetPosition() == 0)
		{
			wchar_t cBOM = (wchar_t)UNICODE_BOM;
			CFile::Write(&cBOM, sizeof(wchar_t));
		}
		if (fputws(lpsz, m_pStream) == _TEOF)
			AfxThrowFileException(CFileException::diskFull, _doserrno, m_strFileName);
	}
	else
	{
		USES_CONVERSION;
		WriteAnsiString(CW2A(lpsz));
	}
}

void CStdioFileEx::WriteAnsiString(LPCSTR lpsz)
{
	ASSERT(lpsz != NULL);

	if (lpsz == NULL)
	{
		AfxThrowInvalidArgException();
	}
	if(!m_bIsUnicodeText)
	{
		ASSERT(m_pStream != NULL);
		if (fputs(lpsz, m_pStream) == _TEOF)
			AfxThrowFileException(CFileException::diskFull, _doserrno, m_strFileName);
	}
	else
	{
		USES_CONVERSION;
		WriteWideString(CA2W(lpsz));
	}
}


UINT CStdioFileEx::PreprocessFlags(const CString& sFilePath, UINT& nOpenFlags)
{
	m_bIsUnicodeText = false;

	// If we have writeUnicode we must have write or writeRead as well
	if (nOpenFlags & CStdioFileEx::modeWriteUnicode)
	{
		ASSERT(nOpenFlags & CFile::modeWrite || nOpenFlags & CFile::modeReadWrite);
		m_bIsUnicodeText = true;
	}
	// If reading in text mode and not creating...
	else if (nOpenFlags & CFile::typeText && !(nOpenFlags & CFile::modeCreate) && !(nOpenFlags & CFile::modeWrite ))
	{
		m_bIsUnicodeText = IsFileUnicode(sFilePath);
	}

	//如果要读写Unicode格式的文本文件, 必须切换到typeBinary方式, 因为这会影响fputws/fgetws的工作方式(具体情况参考MSDN)。
	if (m_bIsUnicodeText)
	{
		nOpenFlags &= ~(CFile::typeText);
		nOpenFlags |= CFile::typeBinary;
	}

	return nOpenFlags;
}

// Purpose:    Determines whether a file is Unicode by reading the first character and detecting
//       whether it's the Unicode byte marker.
bool CStdioFileEx::IsFileUnicode(const CString& sFilePath)
{
	CStdioFile      file;
	wchar_t     cFirstChar;
	CFileException exFile;

	bool      bIsUnicode = false;
	// Open file in binary mode and read first character
	if (file.Open(sFilePath, CFile::typeBinary | CFile::modeRead, &exFile))
	{
		// If byte is Unicode byte-order marker, let's say it's Unicode
		//if (file.Read(&cFirstChar, sizeof(wchar_t)) > 0 && cFirstChar == (wchar_t)UNICODE_BOM)
		if (file.Read(&cFirstChar, sizeof(wchar_t)) > 0 && cFirstChar == (wchar_t)UNICODE_BOM)
		{
			bIsUnicode = true;
		}

		file.Close();
	}
	else
	{
		// Handle error here if you like
	}

	return bIsUnicode;
}


unsigned long CStdioFileEx::GetCharCount()
{
	int      nCharSize;
	unsigned long nByteCount, nCharCount = 0;

	if (m_pStream)
	{
		// Get size of chars in file
		nCharSize = m_bIsUnicodeText ? sizeof(wchar_t): sizeof(char);

		// If Unicode, remove byte order mark from count
		nByteCount = (unsigned long)GetLength();

		if (m_bIsUnicodeText)
		{
			nByteCount = nByteCount - sizeof(wchar_t);
		}

		// Calc chars
		nCharCount = (nByteCount / nCharSize);
	}

	return nCharCount;
}

bool CStdioFileEx::IsFileUTF8code( const CString& sFilePath )
{
	CStdioFile      file;
	CString			 strTmp;
	CFileException exFile;

	bool      bIsUnicode = false;
	// Open file in binary mode and read first character
	if (file.Open(sFilePath, CFile::typeBinary | CFile::modeRead, &exFile))
	{
		// If byte is Unicode byte-order marker, let's say it's Unicode
		file.ReadString(strTmp);
		char * tmpLine = strTmp.GetBuffer();
		if(((char)0xEF==tmpLine[0])&&((char)0xBB==tmpLine[1])&&((char)0xBF==tmpLine[2]))
			bIsUnicode = true;
		file.Close();
	}
	else
	{
		// Handle error here if you like
	}

	return bIsUnicode;
}


bool ProsscessUTF(CString& stringLine)
{
	char * tmpLine = stringLine.GetBuffer();
	//UTF-16 的BOM 头 0xFF, 0xFE
	//UTF-8   的BOM 头 0xEF, 0xBB, 0xBF
	ASSERT(stringLine.GetLength()>=3);
	if( ((char)0xFF==tmpLine[0])&&((char)0xFE==tmpLine[1]) )
	{
		stringLine.Delete(0,2);
		//stringLine = UTF8toASCII(stringLine.GetBuffer()).c_str();
		return false;
	}
	if(((char)0xEF==tmpLine[0])&&((char)0xBB==tmpLine[1])&&((char)0xBF==tmpLine[2]))
	{
		stringLine.Delete(0,3);
		//stringLine = UTF16toASCII(stringLine.GetBuffer()).c_str();
		//char *rel  = new char[stringLine.GetLength()+1];
		//WideString ws(stringLine.GetBuffer(), stringLine.GetLength()+1);
		//ws.toUTF8String(rel);
		//stringLine = rel;
		return true;
	}
	return false;
}

BOOL CDlgImportWellInfo::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	//GetDlgItem(IDOK)->EnableWindow(FALSE);
	// TODO:  在此添加额外的初始化

	CString strFieldName[7] = 
	{
		_T("井号"),
		_T("X"),
		_T("Y"),
		_T("补芯海拔"),
		_T("油层顶深"),
		_T("油层底深"),
		_T("目前井别")
	};
	
	// TODO: Add extra initialization here
	m_wndGridFields.SetRowCount(8);
	m_wndGridFields.SetColumnCount(3);
	m_wndGridFields.SetFixedColumnCount(1);
	m_wndGridFields.SetFixedRowCount(1);	

	m_wndGridFields.SetItemText(0,1,_T("数据项"));
	m_wndGridFields.SetItemText(0,2,_T("对应字段"));

	
	for (int j=1; j<8; j++)
	{
		m_wndGridFields.SetItemText(j,1,strFieldName[j-1]);
	}

	for (int i=1;i<8;i++)
	{
		//m_wndGridFields.SetItemState(i, 2, m_wndGridFields.GetItemState(i, 2) | GVNI_READONLY);
		m_wndGridFields.SetItemState(i, 1, m_wndGridFields.GetItemState(i, 1) | GVNI_READONLY);
	}

	m_wndGridFields.SetColumnWidth(0,15);

	m_wndGridFields.EnableTitleTips(FALSE);
	m_wndGridFields.SetRowResize(FALSE);
	m_wndGridFields.EnableDragAndDrop(FALSE);

	GetDlgItem(IDC_COMBO_SHEET)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_FIRST_LINE)->EnableWindow(TRUE);

	GetDlgItem(IDC_RADIO_COMMA)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_TAB)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_SPACE)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_OTHER)->EnableWindow(TRUE);
	//GetDlgItem(IDOK)->EnableWindow(FALSE);
	if( m_nSeq == 3 )
		GetDlgItem(IDC_EDIT_CH)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_EDIT_CH)->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgImportWellInfo::OnBnClickedBtnFile()
{
	// TODO: 在此添加控件通知处理程序代码
	CString szFilter;

	switch(m_iFileType)
	{
	case 0: // 文本文件
		szFilter = _T("文本文件 (*.txt)|*.TXT|逗号分隔符文件 (*.csv)|*.CSV|所有文件 (*.*)|*.*||");
		GetDlgItem(IDC_COMBO_SHEET)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_FIRST_LINE)->EnableWindow(TRUE);

		GetDlgItem(IDC_RADIO_COMMA)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_TAB)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_SPACE)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_OTHER)->EnableWindow(TRUE);
		if( m_nSeq == 3 )
			GetDlgItem(IDC_EDIT_CH)->EnableWindow(TRUE);
		else
			GetDlgItem(IDC_EDIT_CH)->EnableWindow(FALSE);
		break;
	case 1: // excel
		szFilter = _T("Microsoft Office Excel 工作薄 (*.xls)|*.xls|所有文件 (*.*)|*.*||");
		GetDlgItem(IDC_COMBO_SHEET)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_FIRST_LINE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_COMMA)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_TAB)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_SPACE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_OTHER)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_CH)->EnableWindow(FALSE);
		break;
	case 2:// dbf
		szFilter = _T("dBase, FoxFro, xBase 文件 (*.dbf)|*.dbf|所有文件 (*.*)|*.*||"); 
		GetDlgItem(IDC_COMBO_SHEET)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_FIRST_LINE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_COMMA)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_TAB)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_SPACE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_OTHER)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_CH)->EnableWindow(FALSE);
		break;
	case 3:// mdb
		szFilter = _T("MS Access 文件 (*.mdb)|*.mdb|所有文件 (*.*)|*.*||"); 
		GetDlgItem(IDC_COMBO_SHEET)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_FIRST_LINE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_COMMA)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_TAB)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_SPACE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_OTHER)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_CH)->EnableWindow(FALSE);
		break;
	default:
		break;
	}
	
	CFileDialog dlg(TRUE,0,0,4|2,szFilter);
	if( dlg.DoModal() != IDOK )
		return;
	
	m_strLoadFileName = dlg.GetPathName();
	UpdateData(FALSE);
	if( m_iFileType == 0)
	{
		CStdioFileEx cfex;
		if(cfex.IsFileUnicode(m_strLoadFileName))
		{
			fileUNICODEtoANSI(m_strLoadFileName, m_strLoadFileName);
		}
		if(cfex.IsFileUTF8code(m_strLoadFileName))
		{
			fileUTF8toANSI(m_strLoadFileName, m_strLoadFileName);
		}
		CStdioFile file;
		if( file.Open(m_strLoadFileName, CFile::modeRead | CFile::typeText))
		{
			CString strTmp;
			int i = 0;
			bool bUTF8;
			while( file.ReadString(strTmp) )
			{
				if(0==i)
				{
					bUTF8 = ProsscessUTF(strTmp);
					//char * tmp = new char[strTmp.GetLength()+1];
					//UnicodeToAnsi((LPCWSTR)(strTmp.GetBuffer()),(LPSTR*)&tmp);
					//strTmp = tmp;
				}
				strTmp.Replace(char(9),char(32));
				int spaceIndex = strTmp.Find("  ");
				while(spaceIndex!=-1)
				{
					strTmp.Replace("  "," ");
					spaceIndex = strTmp.Find("  ");
				}
				strTmp.Replace(char(32),char(9));
				if(bUTF8)
				{
					strTmp = Utf8toAnsi(strTmp.GetBuffer(), strTmp.GetLength());
				}
				//if(bUnicode)
				//{
				//	LPSTR tmp;
				//	UnicodeToAnsi((LPCOLESTR)strTmp.GetBuffer(), &tmp);
				//	strTmp = tmp;
				//}
				m_strText += strTmp;
				m_strText += "\r\n";				
				i++;
				if( i>=20 )
					break;
			}
			
			file.Close();
			UpdateData(FALSE);
		}
	}

	Init();
}

void CDlgImportWellInfo::OnSelendokComboSheet() 
{
	// TODO: Add your control notification handler code here
	m_wndGridFields.EndEditing();

	UpdateData();
	try
	{
		int index = m_wndSheet.GetCurSel();
		CString strSheet;
		strSheet.Empty();

		CStringArray options;

		if( index != -1 )
		{
			m_wndSheet.GetLBText(index, strSheet);

			CString strFileName = m_strLoadFileName;

			CString strConnection;
			if( m_iFileType == 3 )
			{
				CString fmt = _T("Driver={Microsoft Access Driver (*.mdb)};Dbq=%s;"
					"Uid=%s;Pwd=%s;");
				strConnection.Format(fmt, strFileName, _T(""), _T(""));
			}
			else			
			{
				CString sDriver = GetExcelDriver();
				if( sDriver.IsEmpty() )
				{
					AfxMessageBox("没有发现 Excel ODBC 驱动程序!");
					return;
				}

				strConnection.Format("DRIVER={%s};DSN='';FIRSTROWHASNAMES=1;READONLY=FALSE;CREATE_DB=\"%s\";DBQ=%s",
					sDriver, strFileName, strFileName);
			}

			CADODatabase m_pDb;

			if(!m_pDb.Open(strConnection))
				return;

			CADORecordset m_pRs = CADORecordset(&m_pDb);	

			CString strTmp;
			if (m_pRs.GetColumns(strSheet)) // 查找Excel中的表名
			{
				while (!m_pRs.IsEOF())
				{				
					m_pRs.GetFieldValue(3,strTmp);
					strTmp.Trim();
					options.Add(strTmp);
					m_pRs.MoveNext();			
				}
			}
			m_pRs.Close();
		}

		if( options.GetSize() < 1)
			options.Add("");

		int nRow = m_wndGridFields.GetRowCount();
		for (int i=1;i<nRow;i++)
		{		
			CGridCellCombo *pCell = (CGridCellCombo*) m_wndGridFields.GetCell(i,2);
			pCell->SetOptions(options);
			pCell->SetStyle(CBS_DROPDOWN); //CBS_DROPDOWN, CBS_DROPDOWNLIST, CBS_SIMPLE		
		}

	}
	catch (const CADOException&e)
	{
		::MessageBox(AfxGetMainWnd()->m_hWnd, e.GetErrorMessage(), "提示信息", MB_OK|MB_ICONERROR);
	}
	catch(...)
	{
		::MessageBox(AfxGetMainWnd()->m_hWnd, _T("无法预料的错误！"), "提示信息", MB_OK|MB_ICONERROR);
	}
}

void CDlgImportWellInfo::OnBnClickedFileType()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_strLoadFileName = _T("");
	UpdateData(FALSE);
	
	if( m_iFileType == 3 || m_iFileType == 1 ) // 从Access数据库加载
	{
		GetDlgItem(IDC_COMBO_SHEET)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_FIRST_LINE)->EnableWindow(FALSE);

		GetDlgItem(IDC_RADIO_COMMA)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_TAB)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_SPACE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_OTHER)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_CH)->EnableWindow(FALSE);
	}
	else if( m_iFileType == 2 )
	{
		GetDlgItem(IDC_COMBO_SHEET)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_FIRST_LINE)->EnableWindow(FALSE);

		GetDlgItem(IDC_RADIO_COMMA)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_TAB)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_SPACE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_OTHER)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_CH)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_COMBO_SHEET)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_FIRST_LINE)->EnableWindow(TRUE);

		GetDlgItem(IDC_RADIO_COMMA)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_TAB)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_SPACE)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_OTHER)->EnableWindow(TRUE);
		if( m_nSeq == 3 )
			GetDlgItem(IDC_EDIT_CH)->EnableWindow(TRUE);
		else
			GetDlgItem(IDC_EDIT_CH)->EnableWindow(FALSE);
	}
}


void CDlgImportWellInfo::OnKillfocusEditCh() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	Init();
}


void CDlgImportWellInfo::OnCheckFirstLine() 
{
	// TODO: Add your control notification handler code here
	m_wndGridFields.EndEditing();
	UpdateData();
	Init();
}


void CDlgImportWellInfo::OnRadioSeq() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	switch(m_nSeq)
	{
	case 0:			// ","
		m_strSeq = _T(",");
		break;
	case 1:			// tab
		m_strSeq = _T(";");
		break;
	case 2:			// " "
		m_strSeq = _T(" ");
		break;
	case 3:			// 其它
		break;
	default:
		break;
	}
	if( m_nSeq == 3 )
		GetDlgItem(IDC_EDIT_CH)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_EDIT_CH)->EnableWindow(FALSE);
	UpdateData(FALSE);
	Init();
}

void CDlgImportWellInfo::Init()
{
	m_wndGridFields.EndEditing();

	if( m_strLoadFileName.IsEmpty() )
		return;	

	m_strText = "";

	try
	{	
		CStringArray options;
		CString strFileName = m_strLoadFileName;
		CSplitPath path(strFileName);

		if( m_iFileType == 3 ) // 从Access数据库加载
		{
			GetDlgItem(IDC_COMBO_SHEET)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_FIRST_LINE)->EnableWindow(FALSE);

			GetDlgItem(IDC_RADIO_COMMA)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_TAB)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_SPACE)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_OTHER)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_CH)->EnableWindow(FALSE);

			CWinAppEx *pApp = (CWinAppEx*)AfxGetApp();

			CADODatabase m_pDb;

			CString fmt = _T("Driver={Microsoft Access Driver (*.mdb)};Dbq=%s;"
				"Uid=%s;Pwd=%s;");

			CString strConnection;
			
			strConnection.Format(fmt, strFileName, _T(""), _T(""));

			if(!m_pDb.Open(strConnection))
				return;
			m_wndSheet.ResetContent();

			CADORecordset m_pRs = CADORecordset(&m_pDb);

			CString strTmp;
			if (m_pRs.OpenSchema(adSchemaTables)) // 查找表名
			{
				while (!m_pRs.IsEOF())
				{				
					m_pRs.GetFieldValue(2,strTmp);
					strTmp.Trim();
					m_wndSheet.AddString(strTmp);
					m_pRs.MoveNext();			
				}
			}
			m_pRs.Close();

			if( m_wndSheet.GetCount() < 0 )
				return;

			m_wndSheet.SetCurSel(0);

			CString strTable;
			m_wndSheet.GetLBText(0, strTable);

			{		
				CADORecordset m_pRs = CADORecordset(&m_pDb);

				CString strTmp;
				if (m_pRs.GetColumns(strTable))
				{
					while (!m_pRs.IsEOF())
					{	
						m_pRs.GetFieldValue(3,strTmp);
						strTmp.Trim();
						options.Add(strTmp);
						m_pRs.MoveNext();			
					}
				}
				m_pRs.Close();
			}
		}
		else if( m_iFileType == 1 ) // Excel
		{
			GetDlgItem(IDC_COMBO_SHEET)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_FIRST_LINE)->EnableWindow(FALSE);

			GetDlgItem(IDC_RADIO_COMMA)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_TAB)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_SPACE)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_OTHER)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_CH)->EnableWindow(FALSE);

			CString sDsn;

			CString sDriver = GetExcelDriver();
			if( sDriver.IsEmpty() )
			{
				AfxMessageBox(_T("没有发现 Excel ODBC 驱动程序!"));
				return;
			}

			sDsn.Format(_T("DRIVER={%s};DSN='';FIRSTROWHASNAMES=1;READONLY=FALSE;CREATE_DB=\"%s\";DBQ=%s"),
				sDriver, strFileName, strFileName);

			CADODatabase m_pDb;

			if(!m_pDb.Open(sDsn))
				return;

			m_wndSheet.ResetContent();

			CADORecordset m_pRs = CADORecordset(&m_pDb);

			CString strTmp;
			if (m_pRs.OpenSchema(adSchemaTables)) // 查找Excel中的表名
			{
				while (!m_pRs.IsEOF())
				{				
					m_pRs.GetFieldValue(2,strTmp);
					strTmp.Trim();
					m_wndSheet.AddString(strTmp);
					m_pRs.MoveNext();			
				}
			}
			m_pRs.Close();

			if( m_wndSheet.GetCount() < 0 )
				return;

			m_wndSheet.SetCurSel(0);

			CString strSheet;
			m_wndSheet.GetLBText(0, strSheet);

			if (m_pRs.GetColumns(strSheet)) // 查找Excel中的表名
			{
				while (!m_pRs.IsEOF())
				{				
					m_pRs.GetFieldValue(3,strTmp);
					strTmp.Trim();
					options.Add(strTmp);
					m_pRs.MoveNext();			
				}
			}

			m_pRs.Close();
		}
		else if( m_iFileType == 2 ) // DBF
		{
			GetDlgItem(IDC_COMBO_SHEET)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_FIRST_LINE)->EnableWindow(FALSE);

			GetDlgItem(IDC_RADIO_COMMA)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_TAB)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_SPACE)->EnableWindow(FALSE);
			GetDlgItem(IDC_RADIO_OTHER)->EnableWindow(FALSE);
			GetDlgItem(IDC_EDIT_CH)->EnableWindow(FALSE);

			CString sDsn;
			sDsn.Format(_T("Provider=VFPOLEDB.1;Data Source=%s;Password=;Collating Sequence=MACHINE;"), 
				strFileName);

			CADODatabase m_pDb;

			if(!m_pDb.Open(sDsn))
				return;

			CADORecordset m_pRs = CADORecordset(&m_pDb);

			CString strTable = path.GetFileName();

			CString strTmp;
			if (m_pRs.GetColumns(strTable)) // 查找Excel中的表名
			{
				while (!m_pRs.IsEOF())
				{				
					m_pRs.GetFieldValue(3,strTmp);
					strTmp.Trim();
					options.Add(strTmp);
					m_pRs.MoveNext();
				}
			}
			m_pRs.Close();

		}
		else // TXT
		{
			GetDlgItem(IDC_COMBO_SHEET)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_FIRST_LINE)->EnableWindow(TRUE);

			GetDlgItem(IDC_RADIO_COMMA)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_TAB)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_SPACE)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_OTHER)->EnableWindow(TRUE);
			if( m_nSeq == 3 )
				GetDlgItem(IDC_EDIT_CH)->EnableWindow(TRUE);
			else
				GetDlgItem(IDC_EDIT_CH)->EnableWindow(FALSE);

			
			CStdioFileEx cfex;
			if(cfex.IsFileUnicode(strFileName))
			{
				fileUNICODEtoANSI(strFileName, strFileName);
			}
			if(cfex.IsFileUTF8code(m_strLoadFileName))
			{
				fileUTF8toANSI(m_strLoadFileName, m_strLoadFileName);
			}
			CStdioFile file;
			if( file.Open(strFileName, CFile::modeRead | CFile::typeText ))
			{
				
				CString strTmp;
				int i = 0;
				bool bUTF8;
				while( file.ReadString(strTmp) )
				{
					if(0==i)
					{
						bUTF8 = ProsscessUTF(strTmp);
						//char * tmp = new char[strTmp.GetLength()+1];
						//UnicodeToAnsi((LPCWSTR)(strTmp.GetBuffer()),(LPSTR*)&tmp);
						//strTmp = tmp;
					}
					//把空格和tab替换为tab
					strTmp.Replace(char(9),char(32));
					int spaceIndex = strTmp.Find("  ");
					while(spaceIndex!=-1)
					{
						strTmp.Replace("  "," ");
						spaceIndex = strTmp.Find("  ");
					}
					strTmp.Replace((char)32,(char)9);
					if(bUTF8)
					{
						strTmp = Utf8toAnsi(strTmp.GetBuffer(), strTmp.GetLength());
					}
					//if(bUnicode)
					//{
					//	LPSTR tmp;
					//	UnicodeToAnsi((LPCOLESTR)strTmp.GetBuffer(), &tmp);
					//	strTmp = tmp;
					//}
					m_strText += strTmp;
					m_strText += "\r\n";
					i++;
					if( i>=20)
						break;
				}
				file.Close();
				UpdateData(FALSE);
			}

			CCSVFile txtFile(strFileName);
			if( m_bHeader )
			{
				for (UINT i=0; i<m_uLine-1; i++)
				{
					CString strTmp;
					txtFile.ReadString(strTmp);
				}
				txtFile.ReadData(options, m_strSeq[0]);
			}
			else
			{
				CStringArray arr;
				if( txtFile.ReadData(arr, m_strSeq[0]) )
				{
					int size = arr.GetSize();
					CString strTmp;
					
					for(int i=0; i<size; i++)
					{
						strTmp.Format(_T("Col %d"), i+1);
						options.Add(strTmp);
					}
					options.Add("");
				}
			}
			txtFile.Close();
		}

		//if( options.GetSize() < 1)
		//	options.Add("");

		int nRow = m_wndGridFields.GetRowCount();

		for (int i=1;i<nRow;i++)
		{
			m_wndGridFields.SetCellType(i,2, RUNTIME_CLASS(CGridCellCombo));

			CGridCellCombo *pCell = (CGridCellCombo*) m_wndGridFields.GetCell(i,2);
			pCell->SetOptions(options);
			pCell->SetStyle(CBS_DROPDOWN); //CBS_DROPDOWN, CBS_DROPDOWNLIST, CBS_SIMPLE
		}
	}
	catch (const CADOException&e)
	{
		::MessageBox(AfxGetMainWnd()->m_hWnd, e.GetErrorMessage(), _T("提示信息"), MB_OK|MB_ICONERROR);
	}
	catch(...)
	{
		::MessageBox(AfxGetMainWnd()->m_hWnd, _T("无法预料的错误！"), _T("提示信息"), MB_OK|MB_ICONERROR);
	}
}

void CDlgImportWellInfo::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	try
	{
		CWaitCursor wait;

		CWellInfoDoc *pDoc = m_pView->GetDocument();
		pDoc->SetModifiedFlag(TRUE);
		m_wndGridFields.EndEditing();

		if(m_pView->m_wndGrid.GetRowCount()>1)
		if(MessageBox("是否保留原有记录","",MB_YESNO )==IDNO)
		{
			//m_strSeq.Empty();
			for(int i=m_pView->m_wndGrid.GetRowCount();i>0;i--)
			{
				m_pView->m_wndGrid.DeleteRow(i);
			}
			m_pView->m_wndGrid.Invalidate();
			m_pView->GetDocument()->SetModifiedFlag(TRUE);
			m_pView->GetDocument()->DoFileSave();
		}
		if( m_iFileType == 3 ) // 从Access数据库中加载
		{
			CString strTable;

			int index = m_wndSheet.GetCurSel();
			if( index < 0 )
				return;

			m_wndSheet.GetLBText(index, strTable);
			CWinAppEx *pApp = (CWinAppEx*)AfxGetApp();

			CString strFileName = m_strLoadFileName;

			CString strConnection;

			CString fmt = _T("Driver={Microsoft Access Driver (*.mdb)};Dbq=%s;"
				"Uid=%s;Pwd=%s;");

			strConnection.Format(fmt, strFileName, _T(""), _T(""));

			CADODatabase m_pDb;

			if(m_pDb.Open(strConnection))
			{		
				CADORecordset m_pRs = CADORecordset(&m_pDb);
				CString strSQL;
				CString strTmp;

				int nRow = m_wndGridFields.GetRowCount();

				BOOL bFieldEmpty = TRUE;
				for ( int i=1;i<nRow;i++)
				{
					strTmp = m_wndGridFields.GetItemText(i,2);
					strTmp.TrimLeft();
					strTmp.TrimRight();
					if( !strTmp.IsEmpty() )
					{
						bFieldEmpty = FALSE;
						break;
					}
				}

				if( bFieldEmpty )
					strSQL = "select * from [" + strTable + "]";
				else
				{
					strSQL = "select ";
					for ( int i=1;i<nRow;i++)
					{
						strTmp = m_wndGridFields.GetItemText(i,2);
						strTmp.TrimLeft();
						strTmp.TrimRight();
						if( strTmp.IsEmpty() )
						{
							strTmp = _T("''");
						}
						else
						{
							if( i != nRow-1 )
							{
								strSQL += strTmp;
								strSQL += ",";
							}
							else
							{
								strSQL += strTmp;
								strSQL += " ";
							}
						}
					}

					strSQL += " from ";
					strSQL += strTable;
				}

				if (m_pRs.Open((LPCSTR)strSQL))
				{
					int nFields = m_pRs.GetFieldCount();
					if( nFields >= nRow )
						nFields = nRow-1;
					while (!m_pRs.IsEOF())
					{	
						int index = m_pView->m_wndGrid.InsertRow("");
						for ( int i=0;i<nFields;i++)
						{
							m_pRs.GetFieldValue(i,strTmp);
							strTmp.Trim();
							m_pView->m_wndGrid.SetItemText(index,i+1,strTmp);
						}

						m_pRs.MoveNext();			
					}
				}
				m_pRs.Close();
				m_pView->m_wndGrid.Invalidate();
			}
		}
		else if( m_iFileType == 1 ) // excel
		{

			CString strSQL, strTmp;
			CString sDsn;

			CString sDriver = GetExcelDriver();
			if( sDriver.IsEmpty() )
			{
				AfxMessageBox("没有发现 Excel ODBC 驱动程序!");
				return;
			}

			sDsn.Format("DRIVER={%s};DSN='';FIRSTROWHASNAMES=1;READONLY=FALSE;CREATE_DB=\"%s\";DBQ=%s",
				sDriver, m_strLoadFileName, m_strLoadFileName);

			CADODatabase m_pDb;

			if(!m_pDb.Open(sDsn))
				return;

			CADORecordset m_pRs = CADORecordset(&m_pDb);

			int index = m_wndSheet.GetCurSel();
			CString strSheet;

			if( index < 0 )
			{
				::MessageBox(m_hWnd, "请选择工作表！", "提示信息", MB_OK|MB_ICONWARNING);
				return;
			}

			m_wndSheet.GetLBText(index, strSheet);

			int nRow = m_wndGridFields.GetRowCount();

			BOOL bFieldEmpty = TRUE;
			for ( int i=1;i<nRow;i++)
			{
				strTmp = m_wndGridFields.GetItemText(i,2);
				strTmp.TrimLeft();
				strTmp.TrimRight();
				if( !strTmp.IsEmpty() )
				{
					bFieldEmpty = FALSE;
					break;
				}
			}

			if( bFieldEmpty )
				strSQL = "select * from [" + strSheet + "]";
			else
			{
				strSQL = "select ";
				for ( int i=1;i<nRow;i++)
				{
					strTmp = m_wndGridFields.GetItemText(i,2);
					strTmp.TrimLeft();
					strTmp.TrimRight();
					if( strTmp.IsEmpty() )
					{
						//::MessageBox(m_hWnd, "数据项中有没有对应的字段！", "提示信息", MB_OK|MB_ICONWARNING);
						//return;
						strTmp = _T("''");
					}
					else
					{
						if( i != nRow-1 )
						{
							strSQL += "[";
							strSQL += strTmp;
							strSQL += "]";
							strSQL += ",";
						}
						else
						{
							strSQL += "[";
							strSQL += strTmp;
							strSQL += "]";
							strSQL += " ";
						}
					}
				}

				strSQL += " from [";
				strSQL += strSheet;
				strSQL += "]";
			}

			if (m_pRs.Open((LPCSTR)strSQL))
			{
				int nFields = m_pRs.GetFieldCount();
				if( nFields >= nRow )
					nFields = nRow-1;
				while (!m_pRs.IsEOF())
				{	
					int index = m_pView->m_wndGrid.InsertRow("");
					for ( int i=0;i<nFields;i++)
					{
						m_pRs.GetFieldValue(i,strTmp);
						strTmp.Trim();
						m_pView->m_wndGrid.SetItemText(index,i+1,strTmp);
					}

					m_pRs.MoveNext();			
				}
			}
			m_pRs.Close();
			m_pView->m_wndGrid.Invalidate();
		}
		else if( m_iFileType == 2 ) // dbf
		{
			CSplitPath path(m_strLoadFileName);
			CString strSQL;
			CString sDsn;
			sDsn.Format(_T("Provider=VFPOLEDB.1;Data Source=%s;Password=;Collating Sequence=MACHINE;"), 
				m_strLoadFileName);

			CADODatabase m_pDb;

			if(!m_pDb.Open(sDsn))
				return;

			CADORecordset m_pRs = CADORecordset(&m_pDb);

			CString strTable = path.GetFileName() + path.GetExtension();

			CString strTmp;	

			int nRow = m_wndGridFields.GetRowCount();

			BOOL bFieldEmpty = TRUE;
			for ( int i=1;i<nRow;i++)
			{
				strTmp = m_wndGridFields.GetItemText(i,2);
				strTmp.TrimLeft();
				strTmp.TrimRight();
				if( !strTmp.IsEmpty() )
				{
					bFieldEmpty = FALSE;
					break;
				}
			}

			if( bFieldEmpty )
				strSQL = "select * from [" + strTable + "]";
			else
			{
				strSQL = "select ";
				for ( int i=1;i<nRow;i++)
				{
					strTmp = m_wndGridFields.GetItemText(i,2);
					strTmp.TrimLeft();
					strTmp.TrimRight();
					if( strTmp.IsEmpty() )
					{
						//::MessageBox(m_hWnd, "数据项中有没有对应的字段！", "提示信息", MB_OK|MB_ICONWARNING);
						//return;
						strTmp = _T("''");
					}
					else
					{
						if( i != nRow-1 )
						{
							//strSQL += "[";
							strSQL += strTmp;
							//strSQL += "]";
							strSQL += ",";
						}
						else
						{
							//strSQL += "[";
							strSQL += strTmp;
							strSQL += " ";
							//strSQL += "]";
						}
					}
				}

				strSQL += " from ";
				strSQL += "[";
				strSQL += strTable;
				strSQL += "]";
			}

			m_pRs.m_pRecordset->CursorLocation = ADODB::adUseServer;

			if (m_pRs.Open((LPCSTR)strSQL))
			{
				TRACE(strSQL);
				TRACE("\n");
				int nFields = m_pRs.GetFieldCount();
				if( nFields >= nRow )
					nFields = nRow-1;
				while (!m_pRs.IsEOF())
				{	
					int index = m_pView->m_wndGrid.InsertRow("");
					for ( int i=0;i<nFields;i++)
					{
						if( !m_pRs.GetFieldValue(i,strTmp) )
							break;
						strTmp.Trim();
						m_pView->m_wndGrid.SetItemText(index,i+1,strTmp);
						TRACE(strTmp);
						TRACE("	");
					}
					TRACE("\n");

					if( !m_pRs.MoveNext() )
						break;
				}
			}
			m_pRs.Close();
			m_pView->m_wndGrid.Invalidate();
		}
		else // 文本文件
		{
			std::map<std::string, int> fieldIndex;
			std::map<std::string, int>::const_iterator iIter;
			
			//CStdioFileEx cfex;
			//if(cfex.IsFileUnicode(m_strLoadFileName))
			//{
			//	fileUNICODEtoANSI(m_strLoadFileName, m_strLoadFileName);
			//}
			CCSVFile txtFile(m_strLoadFileName);
			if( m_bHeader )
			{
				for (UINT i=0; i<m_uLine-1; i++)
				{
					CString strTmp;
					txtFile.ReadString(strTmp);
				}

				CStringArray arr;
				txtFile.ReadData(arr, m_strSeq[0]);
				int size = arr.GetSize();
				for (int i=0; i< size; i++)
				{
					CString strTmp = arr[i];
					fieldIndex[(LPCSTR)strTmp] = i;
				}

				int nRow = m_wndGridFields.GetRowCount();

				CArray<int, int> indexs;
				for ( int i=1;i<nRow;i++)
				{
					CString strTmp = m_wndGridFields.GetItemText(i,2);
					strTmp.TrimLeft();
					strTmp.TrimRight();
					
					if( strTmp.IsEmpty() )
						indexs.Add(-1);
					else
					{
						iIter = fieldIndex.find((LPCSTR)strTmp);
						if( iIter != fieldIndex.end() )
							indexs.Add(iIter->second);
						else
							indexs.Add(-1);
					}
				}

				arr.RemoveAll();

				while(txtFile.ReadData(arr, m_strSeq[0]))
				{
					int index = m_pView->m_wndGrid.InsertRow("");
					int size = arr.GetSize();
					int nFields = (nRow-1)>size?size:(nRow-1);

					for ( int i=0;i<nFields;i++)
					{
						CString strTmp;

						if( indexs[i] == -1 )
							strTmp = _T("");
						else
							strTmp = arr[indexs[i]];

						m_pView->m_wndGrid.SetItemText(index,i+1,strTmp);
					}

					arr.RemoveAll();
				}
			}
			else
			{
				CStringArray arr;
				if( txtFile.ReadData(arr, m_strSeq[0]) )
				{
					int size = arr.GetSize();
					if(size>0)
					{
						ProsscessUTF(arr[0]);
					}
					CString strTmp;
					for(int i=0; i<size; i++)
					{
						strTmp.Format(_T("Col %d"), i+1);
						fieldIndex[(LPCSTR)strTmp] = i;
					}

					int nRow = m_wndGridFields.GetRowCount();

					CArray<int, int> indexs;
					for ( int i=1;i<nRow;i++)
					{
						CString strTmp = m_wndGridFields.GetItemText(i,2);
						strTmp.TrimLeft();
						strTmp.TrimRight();
						if( strTmp.IsEmpty() )
							indexs.Add(-1);
						else
						{
							iIter = fieldIndex.find((LPCSTR)strTmp);
							if( iIter != fieldIndex.end() )
								indexs.Add(iIter->second);
							else
								indexs.Add(-1);
						}
					}

					if((indexs[0]==-1)||(indexs[1]==-1)||(indexs[2]==-1))
					{
						if(indexs[0]==-1)
							MessageBox("井名字段不能为空");
						if(indexs[1]==-1)
							MessageBox("x坐标字段不能为空");
						if(indexs[2]==-1)
							MessageBox("y坐标字段不能为空");
						return;
					}

					pDoc->SetModifiedFlag(TRUE);
					do{
						int index = m_pView->m_wndGrid.InsertRow("");
						int size = arr.GetSize();
						int nFields = (nRow-1)>size?size:(nRow-1);

						for ( int i=0;i<nFields;i++)
						{							
							CString strTmp;
						
							if( indexs[i] == -1 )
								strTmp = _T("");
							else
								strTmp = arr[indexs[i]];

							m_pView->m_wndGrid.SetItemText(index,i+1,strTmp);
						}

						arr.RemoveAll();
					}while(txtFile.ReadData(arr, m_strSeq[0]));
				}
			}
			txtFile.Close();
			
			m_pView->m_wndGrid.Invalidate();
		}

		int nRow = m_pView->m_wndGrid.GetRowCount();
		if( nRow < 2 )
			m_pView->GetDlgItem(IDC_BTN_DEL_ROW)->EnableWindow(FALSE);
		else
			m_pView->GetDlgItem(IDC_BTN_DEL_ROW)->EnableWindow(TRUE);

		pDoc->DoFileSave();
		pDoc->SetTitle(_T("井基础信息"));

	}
	catch (const CADOException&e)
	{
		::MessageBox(AfxGetMainWnd()->m_hWnd, e.GetErrorMessage(), _T("提示信息"), MB_OK|MB_ICONERROR);
	}
	catch(...)
	{
		::MessageBox(AfxGetMainWnd()->m_hWnd, _T("无法预料的错误！"), _T("提示信息"), MB_OK|MB_ICONERROR);
	}	
}

void CDlgImportWellInfo::OnFieldGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
	
	/*int nRows = m_wndGridFields.GetRowCount();
	BOOL bFlag = TRUE;
 	for (int i=1;i<nRows;i++)
 	{
 		CString strTmp = m_wndGridFields.GetItemText(i,2);
 		strTmp.TrimLeft();
 		strTmp.TrimRight();
 		if( strTmp.IsEmpty() )
 		{
 			bFlag = FALSE;
 			break;
 		}
 	}

	GetDlgItem(IDOK)->EnableWindow(	bFlag);
	*/

	*pResult = 0;
}