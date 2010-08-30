// ParseToken.h: interface for the CParseToken class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARSETOKEN_H__6D77E333_3BE5_4DAE_AF6B_76B121C29613__INCLUDED_)
#define AFX_PARSETOKEN_H__6D77E333_3BE5_4DAE_AF6B_76B121C29613__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class AFX_EXT_CLASS CSimuToken  // 数模文件符号
{
public:
	enum KEY_WORD
	{
		UNKNOWN,		// 未知的
		END,			// 结束
		COMMAND,		// 命令
			NUMBER,			// 数字
			QUOTE,			// 引用
			STRING,			// 字符串
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
	BOOL	m_bEndLine;				// 行结束标志
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
// PBRS 文件
class AFX_EXT_CLASS CPbrsCommandItem				// PBRS命令
{
public:
	enum COMMAND_PBRS			// 
	{
		UNKNOWN,		// 未知的
			NX,			// X方向网格数
			NY,			// Y方向网格数
			NZ,			// Z方向网格数
			DX,			// X方向网格数组,以水平面为准增加长度
			DY,			// Y方向网格数组
			DZ,			// Z方向网格数组
			DXB,		// X方向网格数组,以构造面为准增加长度
			DYB,		// Y方向网格数组
			DZB,		// Z方向网格数组
			KX,			// X方向孔隙度
			KY,			// Y方向孔隙度
			KZ,			// Z方向孔隙度
			TX,			// X方向渗透率或传导率
			TY,			// Y方向渗透率或传导率
			TZ,			// Z方向渗透率或传导率
			KXF,
			KYF,
			KZF,
			DZNET,		// 有效厚度
			
			// 数据输入
			CON,		// 常数
			XVAR,		// X方向变值
			YVAR,		// Y方向变值
			ZVAR,		// Z方向变值
			VALUE,		// 按节点赋值
			LAYER,		// NX*NY个值，只用于DEPTH和MDEPTH
			DIP,		// 倾角赋值，只用于DEPTH或MDEPTH
			
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
			DEPTH,				// 网格单元顶部深度
			MDEPTH,				// 网格单元中心深度
			MOD,
			VMOD,
			SOM,
			WELL,
			DATE,				// 模拟日期
			SWT,				// 相渗(水)
			SGT,				// 相渗(气)
			PVTO,				// 高压物性(油)
			PVTG,				// 高压物性(气)
			COMPACT,			// 压实表，孔渗随压力的变化
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
			PPPW,		// 单井曲线
			PPPG,		// 集输中心
			PPPR,		// 区域
			PPPF,		// 全区
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
	CString m_strCommand;			// 命令符号
	CString m_strNode;				// 命令符号说明
	COMMAND_PBRS m_nCommID;			// 命令ID
	int m_nType;
};

class AFX_EXT_CLASS CAnalysePbrsCommand			// 分析PBRS符号 
{
public:
	CPbrsCommandItem SerchCommand(LPCSTR lpszToken);
	CAnalysePbrsCommand();
	void Init();
	virtual ~CAnalysePbrsCommand();
private:
	CArray<CPbrsCommandItem,CPbrsCommandItem&> m_arCommand;
};


class AFX_EXT_CLASS CParsePbrsFile		// 分析PBRS文件类
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
	CStdioFile m_fileOpen;			// 打开的文件
	BOOL m_bIsOpen;

	CString m_strBuffer;			// 缓冲区
	int m_nBufferCurPos;			// 在缓冲区的当前位置
	int m_nBufferLen;				// 缓冲区大小

	CAnalysePbrsCommand m_parsePbrsToken;
	
public:

};

//////////////////////////////////////////////////////////////////////////
//
// Eclipse文件
//

class AFX_EXT_CLASS CEclipseCommandItem				// Eclipse命令
{
public:
	enum COMMAND_ECLIPSE			// 
	{
		UNKNOWN,		// 未知的
			SPECGRID,	// 网格特征
			DX,			// X方向网格大小尺寸 nx*ny*nz
			DY,			// Y方向网格大小尺寸 nx*ny*nz
			DZ,			// Z方向网格大小尺寸 nx*ny*nz
			TOPS,		// 网格顶中心深度 nx*ny
			COORD,		// 直线 (nx+1)(ny+1)
			ZCORN,		// 角点深度 nx*nY*nz
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
	CString m_strCommand;			// 命令符号
	CString m_strNode;				// 命令符号说明
	COMMAND_ECLIPSE m_nCommID;		// 命令ID
	int m_nType;
};

class AFX_EXT_CLASS CAnalyseEclipseCommand			// 分析ECLI符号 
{
public:
	CEclipseCommandItem SerchCommand(LPCSTR lpszToken);
	CAnalyseEclipseCommand();
	void Init();
	virtual ~CAnalyseEclipseCommand();
private:
	CArray<CEclipseCommandItem,CEclipseCommandItem&> m_arCommand;
};


class AFX_EXT_CLASS CParseEclipseFile		// 分析Eclipse文件类
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
	CStdioFile m_fileOpen;			// 打开的文件
	BOOL m_bIsOpen;

	CString m_strBuffer;			// 缓冲区
	int m_nBufferCurPos;			// 在缓冲区的当前位置
	int m_nBufferLen;				// 缓冲区大小

	CAnalyseEclipseCommand m_parseEclipseToken;
	
public:

};

#endif // !defined(AFX_PARSETOKEN_H__6D77E333_3BE5_4DAE_AF6B_76B121C29613__INCLUDED_)
