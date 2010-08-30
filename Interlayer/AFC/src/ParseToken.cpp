// ParseToken.cpp: implementation of the CSimuToken class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ParseToken.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// PBRS
CAnalysePbrsCommand::CAnalysePbrsCommand()
{
	Init();
}

void CAnalysePbrsCommand::Init()
{
	m_arCommand.Add(CPbrsCommandItem("NX","x����������",CPbrsCommandItem::NX));
	m_arCommand.Add(CPbrsCommandItem("NY","y����������",CPbrsCommandItem::NY));
	m_arCommand.Add(CPbrsCommandItem("NZ","z����������",CPbrsCommandItem::NZ));
	m_arCommand.Add(CPbrsCommandItem("DZ","Z������",CPbrsCommandItem::DZ,CPbrsCommandItem::SCENE));
	m_arCommand.Add(CPbrsCommandItem("DZNET","�����",CPbrsCommandItem::DZNET,CPbrsCommandItem::SCENE));
	m_arCommand.Add(CPbrsCommandItem("KX","�������������ĵ�X������͸��",CPbrsCommandItem::KX,CPbrsCommandItem::SCENE));
	m_arCommand.Add(CPbrsCommandItem("KY","�������������ĵ�Y������͸��",CPbrsCommandItem::KY,CPbrsCommandItem::SCENE));
	m_arCommand.Add(CPbrsCommandItem("KZ","�������������ĵ�Z������͸��",CPbrsCommandItem::KZ,CPbrsCommandItem::SCENE));
	m_arCommand.Add(CPbrsCommandItem("KXF","�������������ϵ�X������͸��",CPbrsCommandItem::KXF,CPbrsCommandItem::SCENE));
	m_arCommand.Add(CPbrsCommandItem("KYF","�������������ϵ�Y������͸��",CPbrsCommandItem::KYF,CPbrsCommandItem::SCENE));
	m_arCommand.Add(CPbrsCommandItem("KZF","�������������ϵ�Z������͸��",CPbrsCommandItem::KZF,CPbrsCommandItem::SCENE));
	m_arCommand.Add(CPbrsCommandItem("POR","��϶��",CPbrsCommandItem::POR,CPbrsCommandItem::SCENE));
	m_arCommand.Add(CPbrsCommandItem("SWC","��һ������ˮ���Ͷ�",CPbrsCommandItem::SWC,CPbrsCommandItem::SCENE));
	m_arCommand.Add(CPbrsCommandItem("SGC","��һ���ٽ������Ͷ�",CPbrsCommandItem::SGC,CPbrsCommandItem::SCENE));
	m_arCommand.Add(CPbrsCommandItem("SW","��ˮ���Ͷ�",CPbrsCommandItem::SW,CPbrsCommandItem::SCENE));
	m_arCommand.Add(CPbrsCommandItem("SO","���ͱ��Ͷ�",CPbrsCommandItem::SO,CPbrsCommandItem::SCENE));
	m_arCommand.Add(CPbrsCommandItem("SG","�������Ͷ�",CPbrsCommandItem::SG,CPbrsCommandItem::SCENE));
	m_arCommand.Add(CPbrsCommandItem("P","ÿ������Ԫ��ƽ������ѹ��",CPbrsCommandItem::P,CPbrsCommandItem::SCENE));
	m_arCommand.Add(CPbrsCommandItem("BP","ÿ������Ԫ���ݵ�ѹ��",CPbrsCommandItem::BP,CPbrsCommandItem::SCENE));
	m_arCommand.Add(CPbrsCommandItem("MULT","�˲������������������ݣ�",CPbrsCommandItem::MULT));
	m_arCommand.Add(CPbrsCommandItem("VMULT","�˲������������������ݣ�",CPbrsCommandItem::VMULT));
	m_arCommand.Add(CPbrsCommandItem("INCLUDE","�����ļ�",CPbrsCommandItem::INCLUDE));
	m_arCommand.Add(CPbrsCommandItem("DEPTH","�������",CPbrsCommandItem::DEPTH,CPbrsCommandItem::SCENE));
	m_arCommand.Add(CPbrsCommandItem("MOD","",CPbrsCommandItem::MOD));
	m_arCommand.Add(CPbrsCommandItem("VMOD","",CPbrsCommandItem::VMOD));
	m_arCommand.Add(CPbrsCommandItem("DX","",CPbrsCommandItem::DX));
	m_arCommand.Add(CPbrsCommandItem("DY","",CPbrsCommandItem::DY));
	m_arCommand.Add(CPbrsCommandItem("XVAR","",CPbrsCommandItem::XVAR));
	m_arCommand.Add(CPbrsCommandItem("YVAR","",CPbrsCommandItem::YVAR));
	m_arCommand.Add(CPbrsCommandItem("ZVAR","",CPbrsCommandItem::ZVAR));
	m_arCommand.Add(CPbrsCommandItem("VALUE","",CPbrsCommandItem::VALUE));
	m_arCommand.Add(CPbrsCommandItem("LAYER","",CPbrsCommandItem::LAYER));
	m_arCommand.Add(CPbrsCommandItem("SOM","�ɶ��ͱ��Ͷ�",CPbrsCommandItem::SOM,CPbrsCommandItem::SCENE));
	m_arCommand.Add(CPbrsCommandItem("SW","��ˮ���Ͷ�",CPbrsCommandItem::SW,CPbrsCommandItem::SCENE));
	m_arCommand.Add(CPbrsCommandItem("WELL","������",CPbrsCommandItem::WELL));
	m_arCommand.Add(CPbrsCommandItem("DATE","Ͷ������",CPbrsCommandItem::DATE));
	m_arCommand.Add(CPbrsCommandItem("TITLE1","����1",CPbrsCommandItem::TITLE1));
	m_arCommand.Add(CPbrsCommandItem("TITLE2","����2",CPbrsCommandItem::TITLE2));
	m_arCommand.Add(CPbrsCommandItem("TITLE3","����3",CPbrsCommandItem::TITLE3));
	m_arCommand.Add(CPbrsCommandItem("DIM","",CPbrsCommandItem::DIM));
	m_arCommand.Add(CPbrsCommandItem("IMPES","",CPbrsCommandItem::IMPES));
	m_arCommand.Add(CPbrsCommandItem("NREGMX","",CPbrsCommandItem::NREGMX));
	m_arCommand.Add(CPbrsCommandItem("NEQLMX","",CPbrsCommandItem::NEQLMX));  
	m_arCommand.Add(CPbrsCommandItem("NSATMX","",CPbrsCommandItem::NSATMX));  
	m_arCommand.Add(CPbrsCommandItem("NSATENT","",CPbrsCommandItem::NSATENT));
	m_arCommand.Add(CPbrsCommandItem("NTABP","",CPbrsCommandItem::NTABP));  
	m_arCommand.Add(CPbrsCommandItem("NWMAX","",CPbrsCommandItem::NWMAX));
	m_arCommand.Add(CPbrsCommandItem("NPRFMX","",CPbrsCommandItem::NPRFMX));
	m_arCommand.Add(CPbrsCommandItem("NPRFTOT","",CPbrsCommandItem::NPRFTOT));
	m_arCommand.Add(CPbrsCommandItem("NONEQ","",CPbrsCommandItem::NONEQ));
	m_arCommand.Add(CPbrsCommandItem("PRINT","",CPbrsCommandItem::PRINT));
	m_arCommand.Add(CPbrsCommandItem("EQUIL","",CPbrsCommandItem::EQUIL));
	m_arCommand.Add(CPbrsCommandItem("NONE","",CPbrsCommandItem::NONE));
	m_arCommand.Add(CPbrsCommandItem("TABLES","",CPbrsCommandItem::TABLES));
	m_arCommand.Add(CPbrsCommandItem("ARRAYS","",CPbrsCommandItem::ARRAYS));
	m_arCommand.Add(CPbrsCommandItem("COEFS","",CPbrsCommandItem::COEFS));
	m_arCommand.Add(CPbrsCommandItem("INFLUX","",CPbrsCommandItem::INFLUX));
	m_arCommand.Add(CPbrsCommandItem("INIT","",CPbrsCommandItem::INIT));
	m_arCommand.Add(CPbrsCommandItem("FAULTS","",CPbrsCommandItem::FAULTS));
	m_arCommand.Add(CPbrsCommandItem("DOB","",CPbrsCommandItem::DOB));
	m_arCommand.Add(CPbrsCommandItem("GRAV","",CPbrsCommandItem::GRAV));
	m_arCommand.Add(CPbrsCommandItem("DWB","",CPbrsCommandItem::DWB));
	m_arCommand.Add(CPbrsCommandItem("BWI","",CPbrsCommandItem::BWI));
	m_arCommand.Add(CPbrsCommandItem("VW","",CPbrsCommandItem::VW));
	m_arCommand.Add(CPbrsCommandItem("CW","",CPbrsCommandItem::CW));
	m_arCommand.Add(CPbrsCommandItem("CR","",CPbrsCommandItem::CR));
	m_arCommand.Add(CPbrsCommandItem("TRES","",CPbrsCommandItem::TRES));
	m_arCommand.Add(CPbrsCommandItem("TS","",CPbrsCommandItem::TS));
	m_arCommand.Add(CPbrsCommandItem("PS","",CPbrsCommandItem::PS));
	m_arCommand.Add(CPbrsCommandItem("SWT","",CPbrsCommandItem::SWT));
	m_arCommand.Add(CPbrsCommandItem("KROW","",CPbrsCommandItem::KROW));
	m_arCommand.Add(CPbrsCommandItem("KRW","",CPbrsCommandItem::KRW));
	m_arCommand.Add(CPbrsCommandItem("PCWO","",CPbrsCommandItem::PCWO));
	m_arCommand.Add(CPbrsCommandItem("SGT","",CPbrsCommandItem::SGT));
	m_arCommand.Add(CPbrsCommandItem("SG","",CPbrsCommandItem::SG));
	m_arCommand.Add(CPbrsCommandItem("KRG","",CPbrsCommandItem::KRG));
	m_arCommand.Add(CPbrsCommandItem("KROG","",CPbrsCommandItem::KROG));
	m_arCommand.Add(CPbrsCommandItem("PCGO","",CPbrsCommandItem::PCGO));
	m_arCommand.Add(CPbrsCommandItem("PVTO","",CPbrsCommandItem::PVTO));
	m_arCommand.Add(CPbrsCommandItem("RS","",CPbrsCommandItem::RS));
	m_arCommand.Add(CPbrsCommandItem("VO","",CPbrsCommandItem::VO));
	m_arCommand.Add(CPbrsCommandItem("CVO","",CPbrsCommandItem::CVO));
	m_arCommand.Add(CPbrsCommandItem("BO","",CPbrsCommandItem::BO));
	m_arCommand.Add(CPbrsCommandItem("CO","",CPbrsCommandItem::CO));
	m_arCommand.Add(CPbrsCommandItem("PVTG","",CPbrsCommandItem::PVTG));
	m_arCommand.Add(CPbrsCommandItem("VG","",CPbrsCommandItem::VG));
	m_arCommand.Add(CPbrsCommandItem("ZG","",CPbrsCommandItem::ZG));
	m_arCommand.Add(CPbrsCommandItem("PPPW","",CPbrsCommandItem::PPPW));
	m_arCommand.Add(CPbrsCommandItem("PPPG","",CPbrsCommandItem::PPPG));
	m_arCommand.Add(CPbrsCommandItem("PPPR","",CPbrsCommandItem::PPPR));
	m_arCommand.Add(CPbrsCommandItem("PPPF","",CPbrsCommandItem::PPPF));
	m_arCommand.Add(CPbrsCommandItem("MAP","",CPbrsCommandItem::MAP));
	m_arCommand.Add(CPbrsCommandItem("FORM","",CPbrsCommandItem::FORM));
	m_arCommand.Add(CPbrsCommandItem("WLOC","",CPbrsCommandItem::WLOC));
	m_arCommand.Add(CPbrsCommandItem("MMAP","",CPbrsCommandItem::MMAP));
}

CAnalysePbrsCommand::~CAnalysePbrsCommand()
{
}

CPbrsCommandItem CAnalysePbrsCommand::SerchCommand(LPCSTR lpszToken)
{
	CPbrsCommandItem commItem;
	commItem.m_nCommID = CPbrsCommandItem::UNKNOWN;
	CString strToken = lpszToken;
	
	int nCount = m_arCommand.GetSize();
	for (int i = 0; i < nCount; i++)
	{		
		if (m_arCommand[i].m_strCommand == strToken)
		{
			commItem = m_arCommand[i];
			return commItem;
		}
	}

	return commItem;
}


//////////////////////////////////////////////////////////////////////////

CParsePbrsFile::CParsePbrsFile()
{
	m_nBufferCurPos = 0;			// �ڻ������ĵ�ǰλ��
	m_nBufferLen = 0;				// ��������С

	m_bIsOpen = FALSE;
}

CParsePbrsFile::~CParsePbrsFile()
{
	CloseFile();
}

// �Ƕ������
BOOL CParsePbrsFile::IsDelim(char c)
{
	if(strchr(" ;,+-<>/*%^=()",c) || c==9 || c=='\r' || c==0)
		return TRUE;
	else
		return FALSE;
}

// �ǿո���Ʊ����
BOOL CParsePbrsFile::IsWhite(char c)
{
	if(c==' ' || c=='\t')
		return TRUE;
	else 
		return FALSE;
}

// ��ȡ��ģ�ļ�����
CSimuToken CParsePbrsFile::GetToken(CPbrsCommandItem &commItem)
{
	CSimuToken token;
	
	if( m_strBuffer.GetLength() <=0 || m_nBufferCurPos >= m_nBufferLen) 
	{ 		
		m_nBufferCurPos = 0;
		if ( !ReadFile(m_strBuffer) )				// �ļ�����
		{
			token.token_type = CSimuToken::END;		// ���ؽ�����־
			return token;
		}
		m_nBufferCurPos = 0;						// �ڻ������ĵ�ǰλ��
		m_nBufferLen = m_strBuffer.GetLength();		// ��������С		
	}
	
	int ch = m_strBuffer.GetAt(m_nBufferCurPos);
	
	// �����ո���Ʊ��
	while ( IsWhite(ch) && m_nBufferCurPos < m_nBufferLen) 
	{
		/* skip over white space */
		ch = m_strBuffer.GetAt(++m_nBufferCurPos);
	}

	// ����
	// ˫�����ڵ�����
	if (ch == '"')
	{
		ch = m_strBuffer.GetAt(++m_nBufferCurPos);
		while (ch != '"' && ch != '\r' && m_nBufferCurPos < m_nBufferLen) 
		{
			token.m_strToken += m_strBuffer.GetAt(m_nBufferCurPos);
			ch = m_strBuffer.GetAt(m_nBufferCurPos);
			m_nBufferCurPos++;
		}

		token.m_strToken = token.m_strToken.Mid(0, token.m_strToken.GetLength() - 1);
		token.token_type = CSimuToken::QUOTE;

		if( m_nBufferCurPos >= m_nBufferLen)
			token.m_bEndLine = TRUE;

		return (token);
	}

	// ������Ϊͷ�Ķ���Ϊ������
	// 
	// ��: 2*1.3 0.2E-1 0.2F2��
	if (isdigit(ch) 
		|| (ch == '.'&& isdigit(m_strBuffer[m_nBufferCurPos+1]) ) 
		|| (ch == '-' && isdigit(m_strBuffer[m_nBufferCurPos+1])))
	{
		do{
			token.m_strToken += (TCHAR)ch;
			
			if (m_nBufferCurPos < m_nBufferLen - 1)
				ch = m_strBuffer.GetAt(++m_nBufferCurPos);
			else 
			{
				m_nBufferCurPos++;
				break;
			}
		}while ( !IsDelim(ch) || ch == '+' || ch == '-');

		token.token_type = CSimuToken::NUMBER;

		if( m_nBufferCurPos >= m_nBufferLen)
			token.m_bEndLine = TRUE;

		return (token);
	}

	// �ַ���
	// 
	// *1.5  =0.22 -0.3 +0.6 ��Ϊ����
	// 

	if (isprint(ch))
	{ 
		while (!IsWhite(ch)) 
		{
			if (m_nBufferLen < m_nBufferCurPos)
				break;

			token.m_strToken += (TCHAR)ch;
			if (++m_nBufferCurPos < m_nBufferLen)
			{
				ch = m_strBuffer.GetAt(m_nBufferCurPos);
			}
			else 
				break;
		}

		token.token_type = CSimuToken::STRING; // ������ַ�������һ���ж��Ƿ�ΪPBRS����

		commItem = m_parsePbrsToken.SerchCommand((LPCSTR)token.m_strToken);
		if (commItem.m_nCommID != CPbrsCommandItem::UNKNOWN )
		{
			token.token_type = CSimuToken::COMMAND;

			if( m_nBufferCurPos >= m_nBufferLen)
				token.m_bEndLine = TRUE;

			return token;
		}

	}

	if( m_nBufferCurPos >= m_nBufferLen)
			token.m_bEndLine = TRUE;

	return token;
}

BOOL CParsePbrsFile::OpenFile(LPCTSTR lpszFileName)
{
	if( m_bIsOpen )
		CloseFile();

	if( m_fileOpen.Open(lpszFileName, CFile::modeRead | CFile::typeText) )
		m_bIsOpen = TRUE;
	else
		m_bIsOpen = FALSE;
	
	return m_bIsOpen;
}

void CParsePbrsFile::CloseFile()
{
	if( m_bIsOpen )
	{
		m_fileOpen.Close();
		m_bIsOpen = FALSE;
	}
}

BOOL CParsePbrsFile::ReadFile(CString &strBuff)
{
	if( !m_bIsOpen )
		return FALSE;
	
	CString strTmp;
	
	do
	{
		if( m_fileOpen.ReadString(strTmp) )
		{
			strTmp.TrimLeft();	// ȥ��ǰ��Ŀո�
			strTmp.TrimRight();	// ȥ������Ŀո�
		}
		else
			return FALSE;
	}
	while( strTmp.IsEmpty() || // ȥ������
		( strTmp.GetLength() >= 2 && strTmp.Mid(0,2) == "C ") // ȥ��ע����
		|| ( strTmp.GetLength() == 1 && strTmp[0] == 'C')
		);
	
	strBuff = strTmp;
	
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// Eclipse
CAnalyseEclipseCommand::CAnalyseEclipseCommand()
{
	Init();
}

void CAnalyseEclipseCommand::Init()
{
	m_arCommand.Add(CEclipseCommandItem("SPECGRID","��������",CEclipseCommandItem::SPECGRID,CEclipseCommandItem::SCENE));
	m_arCommand.Add(CEclipseCommandItem("DX","X���������С�ߴ�",CEclipseCommandItem::DX,CEclipseCommandItem::SCENE));
	m_arCommand.Add(CEclipseCommandItem("DY","Y���������С�ߴ�",CEclipseCommandItem::DY,CEclipseCommandItem::SCENE));
	m_arCommand.Add(CEclipseCommandItem("DZ","Z���������С�ߴ�",CEclipseCommandItem::DZ,CEclipseCommandItem::SCENE));
	m_arCommand.Add(CEclipseCommandItem("TOPS","�����������",CEclipseCommandItem::TOPS,CEclipseCommandItem::SCENE));
	m_arCommand.Add(CEclipseCommandItem("COORD","ֱ��",CEclipseCommandItem::COORD,CEclipseCommandItem::SCENE));
	m_arCommand.Add(CEclipseCommandItem("ZCORN","�ǵ����",CEclipseCommandItem::ZCORN,CEclipseCommandItem::SCENE));
	m_arCommand.Add(CEclipseCommandItem("PORO","",CEclipseCommandItem::PORO,CEclipseCommandItem::SCENE));
	m_arCommand.Add(CEclipseCommandItem("GENERAL","",CEclipseCommandItem::GENERAL,CEclipseCommandItem::SCENE));
	m_arCommand.Add(CEclipseCommandItem("NTG","",CEclipseCommandItem::NTG,CEclipseCommandItem::SCENE));
	m_arCommand.Add(CEclipseCommandItem("PERMX","",CEclipseCommandItem::PERMX,CEclipseCommandItem::SCENE));
	m_arCommand.Add(CEclipseCommandItem("PERMY","",CEclipseCommandItem::PERMY,CEclipseCommandItem::SCENE));
	m_arCommand.Add(CEclipseCommandItem("SWAT","",CEclipseCommandItem::SWAT,CEclipseCommandItem::SCENE));
}

CAnalyseEclipseCommand::~CAnalyseEclipseCommand()
{
}

CEclipseCommandItem CAnalyseEclipseCommand::SerchCommand(LPCSTR lpszToken)
{
	CEclipseCommandItem commItem;
	commItem.m_nCommID = CEclipseCommandItem::UNKNOWN;
	CString strToken = lpszToken;
	
	int nCount = m_arCommand.GetSize();
	for (int i = 0; i < nCount; i++)
	{		
		if (m_arCommand[i].m_strCommand == strToken)
		{
			commItem = m_arCommand[i];
			return commItem;
		}
	}

	return commItem;
}


//////////////////////////////////////////////////////////////////////////

CParseEclipseFile::CParseEclipseFile()
{
	m_nBufferCurPos = 0;			// �ڻ������ĵ�ǰλ��
	m_nBufferLen = 0;				// ��������С

	m_bIsOpen = FALSE;
}

CParseEclipseFile::~CParseEclipseFile()
{
	CloseFile();
}

// �Ƕ������
BOOL CParseEclipseFile::IsDelim(char c)
{
	if(strchr(" ;,+-<>/*%^=()",c) || c==9 || c=='\r' || c==0)
		return TRUE;
	else
		return FALSE;
}

// �ǿո���Ʊ����
BOOL CParseEclipseFile::IsWhite(char c)
{
	if(c==' ' || c=='\t')
		return TRUE;
	else 
		return FALSE;
}

// ��ȡ��ģ�ļ�����
CSimuToken CParseEclipseFile::GetToken(CEclipseCommandItem &commItem)
{
	CSimuToken token;
	
	if( m_strBuffer.GetLength() <=0 || m_nBufferCurPos >= m_nBufferLen) 
	{ 		
		m_nBufferCurPos = 0;
		if ( !ReadFile(m_strBuffer) )				// �ļ�����
		{
			token.token_type = CSimuToken::END;		// ���ؽ�����־
			return token;
		}
		m_nBufferCurPos = 0;						// �ڻ������ĵ�ǰλ��
		m_nBufferLen = m_strBuffer.GetLength();		// ��������С
		if( m_nBufferLen > 130 )					// 130���Ժ���ַ�������ע��
			m_nBufferLen = 130;
		int index = m_strBuffer.Find("/");
		if( index != -1 )					// ��/���Ժ��Ϊע��
			m_nBufferLen = index+1;
	}
	
	int ch = m_strBuffer.GetAt(m_nBufferCurPos);
	
	// �����ո���Ʊ��
	while ( IsWhite(ch) && m_nBufferCurPos < m_nBufferLen) 
	{
		/* skip over white space */
		ch = m_strBuffer.GetAt(++m_nBufferCurPos);
	}

	// ����
	// ˫�����ڵ�����
	if (ch == '\'')
	{
		ch = m_strBuffer.GetAt(++m_nBufferCurPos);
		while (ch != '\'' && ch != '\r' && m_nBufferCurPos < m_nBufferLen) 
		{
			token.m_strToken += m_strBuffer.GetAt(m_nBufferCurPos);
			ch = m_strBuffer.GetAt(m_nBufferCurPos);
			m_nBufferCurPos++;
		}

		token.m_strToken = token.m_strToken.Mid(0, token.m_strToken.GetLength() - 1);
		token.token_type = CSimuToken::QUOTE;

		if( m_nBufferCurPos >= m_nBufferLen)
			token.m_bEndLine = TRUE;

		return (token);
	}

	// ������Ϊͷ�Ķ���Ϊ������
	// 
	// ��: 2*1.3 0.2E-1 0.2F2��
	if (isdigit(ch) 
		|| (ch == '.'&& isdigit(m_strBuffer[m_nBufferCurPos+1]) ) 
		|| (ch == '-' && isdigit(m_strBuffer[m_nBufferCurPos+1])))
	{
		do{
			token.m_strToken += (TCHAR)ch;
			
			if (m_nBufferCurPos < m_nBufferLen - 1)
				ch = m_strBuffer.GetAt(++m_nBufferCurPos);
			else 
			{
				m_nBufferCurPos++;
				break;
			}
		}while ( !IsDelim(ch) || ch == '+' || ch == '-');

		token.token_type = CSimuToken::NUMBER;

		if( m_nBufferCurPos >= m_nBufferLen)
			token.m_bEndLine = TRUE;

		return (token);
	}

	// �ַ���
	// 
	// *1.5  =0.22 -0.3 +0.6 ��Ϊ����
	// 

	if (isprint(ch))
	{ 
		while (!IsWhite(ch)) 
		{
			if (m_nBufferLen < m_nBufferCurPos)
				break;

			token.m_strToken += (TCHAR)ch;
			if (++m_nBufferCurPos < m_nBufferLen)
			{
				ch = m_strBuffer.GetAt(m_nBufferCurPos);
			}
			else 
				break;
		}

		token.token_type = CSimuToken::STRING; // ������ַ�������һ���ж��Ƿ�ΪEclipse����


		CString strComm = token.m_strToken;
		
		if( strComm.GetLength() > 8 ) // �ؼ������Ϊ8���ַ�
			strComm = strComm.Left(8);
		
		commItem = m_parseEclipseToken.SerchCommand((LPCSTR)strComm);
		
		if (commItem.m_nCommID != CEclipseCommandItem::UNKNOWN ) // ����ǹؼ���
		{
			token.token_type = CSimuToken::COMMAND;	

			if( m_nBufferCurPos >= m_nBufferLen)
				token.m_bEndLine = TRUE;

			m_nBufferCurPos = m_nBufferLen; // ����8���ַ��Ժ���ַ�

			return token;
		}

	}

	if( m_nBufferCurPos >= m_nBufferLen)
			token.m_bEndLine = TRUE;

	return token;
}

BOOL CParseEclipseFile::OpenFile(LPCTSTR lpszFileName)
{
	if( m_bIsOpen )
		CloseFile();

	if( m_fileOpen.Open(lpszFileName, CFile::modeRead | CFile::typeText) )
		m_bIsOpen = TRUE;
	else
		m_bIsOpen = FALSE;

	if( m_fileOpen.GetLength() < 1 ) // ����ļ�����Ϊ0
	{
		m_fileOpen.Close();
		m_bIsOpen = FALSE;
	}
	
	return m_bIsOpen;
}

void CParseEclipseFile::CloseFile()
{
	if( m_bIsOpen )
	{
		m_fileOpen.Close();
		m_bIsOpen = FALSE;
	}
}

BOOL CParseEclipseFile::ReadFile(CString &strBuff)
{
	if( !m_bIsOpen )
		return FALSE;
	
	CString strTmp;
	
	do
	{
		if( m_fileOpen.ReadString(strTmp) )
		{
			strTmp.TrimLeft();	// ȥ��ǰ��Ŀո�
			strTmp.TrimRight();	// ȥ������Ŀո�
		}
		else
			return FALSE;
	}
	while( strTmp.IsEmpty() || // ȥ������
		( strTmp.GetLength() >= 2 && strTmp.Mid(0,2) == "--") // ȥ��ע����
		|| ( strTmp.GetLength() == 1 && strTmp[0] == '/')
		);
	
	strBuff = strTmp;
	
	return TRUE;
}

