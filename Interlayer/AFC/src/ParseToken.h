// ParseToken.h: interface for the CParseToken class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARSETOKEN_H__6D77E333_3BE5_4DAE_AF6B_76B121C29613__INCLUDED_)
#define AFX_PARSETOKEN_H__6D77E333_3BE5_4DAE_AF6B_76B121C29613__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class AFX_EXT_CLASS CSimuToken  // ��ģ�ļ�����
{
public:
	enum KEY_WORD
	{
		UNKNOWN,		// δ֪��
		END,			// ����
		COMMAND,		// ����
			NUMBER,			// ����
			QUOTE,			// ����
			STRING,			// �ַ���
	};
public:
	CSimuToken()
	{
		m_bEndLine = FALSE; 
	};

	virtual ~CSimuToken(){};
public:
	KEY_WORD token_type;
	CString m_strToken;
	BOOL	m_bEndLine;				// �н�����־
public:
	CSimuToken(const CSimuToken& varSrc) 
	{
		token_type = varSrc.token_type;
		m_strToken = varSrc.m_strToken;
		m_bEndLine = varSrc.m_bEndLine;
	};

	CSimuToken& operator=( const CSimuToken& varSrc )
	{
		token_type = varSrc.token_type;
		m_strToken = varSrc.m_strToken;
		m_bEndLine = varSrc.m_bEndLine;
		return *this;
	}
};

//////////////////////////////////////////////////////////////////////////
// PBRS �ļ�
class AFX_EXT_CLASS CPbrsCommandItem				// PBRS����
{
public:
	enum COMMAND_PBRS			// 
	{
		UNKNOWN,		// δ֪��
			NX,			// X����������
			NY,			// Y����������
			NZ,			// Z����������
			DX,			// X������������,��ˮƽ��Ϊ׼���ӳ���
			DY,			// Y������������
			DZ,			// Z������������
			DXB,		// X������������,�Թ�����Ϊ׼���ӳ���
			DYB,		// Y������������
			DZB,		// Z������������
			KX,			// X�����϶��
			KY,			// Y�����϶��
			KZ,			// Z�����϶��
			TX,			// X������͸�ʻ򴫵���
			TY,			// Y������͸�ʻ򴫵���
			TZ,			// Z������͸�ʻ򴫵���
			KXF,
			KYF,
			KZF,
			DZNET,		// ��Ч���
			
			// ��������
			CON,		// ����
			XVAR,		// X�����ֵ
			YVAR,		// Y�����ֵ
			ZVAR,		// Z�����ֵ
			VALUE,		// ���ڵ㸳ֵ
			LAYER,		// NX*NY��ֵ��ֻ����DEPTH��MDEPTH
			DIP,		// ��Ǹ�ֵ��ֻ����DEPTH��MDEPTH
			
			POR,
			SWC,
			SGC,
			SW,
			SO,
			SG,
			P,
			BP,
			MULT,
			VMULT,
			INCLUDE,
			DEPTH,				// ����Ԫ�������
			MDEPTH,				// ����Ԫ�������
			MOD,
			VMOD,
			SOM,
			WELL,
			DATE,				// ģ������
			SWT,				// ����(ˮ)
			SGT,				// ����(��)
			PVTO,				// ��ѹ����(��)
			PVTG,				// ��ѹ����(��)
			COMPACT,			// ѹʵ��������ѹ���ı仯
			TITLE1,
			TITLE2,
			TITLE3,
			DIM,
			IMPES,
			NREGMX,
			NEQLMX,
			NSATMX,
			NSATENT,
			NTABP,
			NWMAX,
			NPRFMX,
			NPRFTOT,
			NONEQ,
			PRINT,
			EQUIL,
			NONE,
			TABLES,
			ARRAYS,
			COEFS,
			INFLUX,
			INIT,
			FAULTS,
			
			DOB,
			GRAV,
			DWB,
			BWI,
			VW,
			CW,
			CR,
			TRES,
			TS,
			PS,
			KROW,
			KRW,
			KRG,
			KROG,
			PCWO,
			PCGO,
			RS,
			VO,
			CVO,
			BO,
			CO,
			VG,
			ZG,
			PPPW,		// ��������
			PPPG,		// ��������
			PPPR,		// ����
			PPPF,		// ȫ��
			MAP,		// 
			FORM,		//
			WLOC,
			MMAP, 
	};
	
	CPbrsCommandItem()
	{ 
		m_strCommand	= "";
		m_strNode		= "";
		m_nCommID		= UNKNOWN; 
		m_nType			= OTHER; 
	};

	CPbrsCommandItem(const CPbrsCommandItem &item)
	{
		m_strCommand	= item.m_strCommand;
		m_strNode		= item.m_strNode;
		m_nCommID		= item.m_nCommID;
		m_nType			= item.m_nType;
	};
	
	CPbrsCommandItem&  operator =(const CPbrsCommandItem &item)
	{
		m_strCommand	= item.m_strCommand;
		m_strNode		= item.m_strNode;
		m_nCommID		= item.m_nCommID;
		m_nType			= item.m_nType;
		return *this;
	}
	
	CPbrsCommandItem(LPCSTR lpszCommand,LPCSTR lpszNote, COMMAND_PBRS nCommID, int type = OTHER)
	{
		m_strCommand	= lpszCommand;
		m_strNode		= lpszNote;
		m_nCommID		= nCommID;
		m_nType			= type;
	}

public:
	enum {SCENE, OTHER};
	CString m_strCommand;			// �������
	CString m_strNode;				// �������˵��
	COMMAND_PBRS m_nCommID;			// ����ID
	int m_nType;
};

class AFX_EXT_CLASS CAnalysePbrsCommand			// ����PBRS���� 
{
public:
	CPbrsCommandItem SerchCommand(LPCSTR lpszToken);
	CAnalysePbrsCommand();
	void Init();
	virtual ~CAnalysePbrsCommand();
private:
	CArray<CPbrsCommandItem,CPbrsCommandItem&> m_arCommand;
};


class AFX_EXT_CLASS CParsePbrsFile		// ����PBRS�ļ���
{
public:
	CParsePbrsFile();
	virtual ~CParsePbrsFile();
public:
	BOOL OpenFile(LPCTSTR lpszFileName);	
	CSimuToken GetToken( CPbrsCommandItem &commItem );
	void CloseFile();
private:
	BOOL IsDelim(char c);
	BOOL IsWhite(char c);
	BOOL ReadFile(CString &strBuff);	
private:
	CStdioFile m_fileOpen;			// �򿪵��ļ�
	BOOL m_bIsOpen;

	CString m_strBuffer;			// ������
	int m_nBufferCurPos;			// �ڻ������ĵ�ǰλ��
	int m_nBufferLen;				// ��������С

	CAnalysePbrsCommand m_parsePbrsToken;
	
public:

};

//////////////////////////////////////////////////////////////////////////
//
// Eclipse�ļ�
//

class AFX_EXT_CLASS CEclipseCommandItem				// Eclipse����
{
public:
	enum COMMAND_ECLIPSE			// 
	{
		UNKNOWN,		// δ֪��
			SPECGRID,	// ��������
			DX,			// X���������С�ߴ� nx*ny*nz
			DY,			// Y���������С�ߴ� nx*ny*nz
			DZ,			// Z���������С�ߴ� nx*ny*nz
			TOPS,		// ����������� nx*ny
			COORD,		// ֱ�� (nx+1)(ny+1)
			ZCORN,		// �ǵ���� nx*nY*nz
			PORO,
			GENERAL,
			NTG,
			PERMX,
			PERMY,
			SWAT,
	};
	
	CEclipseCommandItem()
	{ 
		m_strCommand	= "";
		m_strNode		= "";
		m_nCommID		= UNKNOWN; 
		m_nType			= OTHER; 
	};

	CEclipseCommandItem(const CEclipseCommandItem &item)
	{
		m_strCommand	= item.m_strCommand;
		m_strNode		= item.m_strNode;
		m_nCommID		= item.m_nCommID;
		m_nType			= item.m_nType;
	};
	
	CEclipseCommandItem&  operator =(const CEclipseCommandItem &item)
	{
		m_strCommand	= item.m_strCommand;
		m_strNode		= item.m_strNode;
		m_nCommID		= item.m_nCommID;
		m_nType			= item.m_nType;
		return *this;
	}
	
	CEclipseCommandItem(LPCSTR lpszCommand,LPCSTR lpszNote, COMMAND_ECLIPSE nCommID, int type = OTHER)
	{
		m_strCommand	= lpszCommand;
		m_strNode		= lpszNote;
		m_nCommID		= nCommID;
		m_nType			= type;
	}

public:
	enum {SCENE, OTHER};
	CString m_strCommand;			// �������
	CString m_strNode;				// �������˵��
	COMMAND_ECLIPSE m_nCommID;		// ����ID
	int m_nType;
};

class AFX_EXT_CLASS CAnalyseEclipseCommand			// ����ECLI���� 
{
public:
	CEclipseCommandItem SerchCommand(LPCSTR lpszToken);
	CAnalyseEclipseCommand();
	void Init();
	virtual ~CAnalyseEclipseCommand();
private:
	CArray<CEclipseCommandItem,CEclipseCommandItem&> m_arCommand;
};


class AFX_EXT_CLASS CParseEclipseFile		// ����Eclipse�ļ���
{
public:
	CParseEclipseFile();
	virtual ~CParseEclipseFile();
public:
	BOOL OpenFile(LPCTSTR lpszFileName);	
	CSimuToken GetToken( CEclipseCommandItem &commItem );
	void CloseFile();
private:
	BOOL IsDelim(char c);
	BOOL IsWhite(char c);
	BOOL ReadFile(CString &strBuff);	
private:
	CStdioFile m_fileOpen;			// �򿪵��ļ�
	BOOL m_bIsOpen;

	CString m_strBuffer;			// ������
	int m_nBufferCurPos;			// �ڻ������ĵ�ǰλ��
	int m_nBufferLen;				// ��������С

	CAnalyseEclipseCommand m_parseEclipseToken;
	
public:

};

#endif // !defined(AFX_PARSETOKEN_H__6D77E333_3BE5_4DAE_AF6B_76B121C29613__INCLUDED_)
