
#ifndef __COMMONSTRUCT_H__
#define __COMMONSTRUCT_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//笔的固定类型值，为以下几种，其它自定
// PS_SOLID            0
// PS_DASH             1       /* -------  */
// PS_DOT              2       /* .......  */
// PS_DASHDOT          3       /* _._._._  */
// PS_DASHDOTDOT       4       /* _.._.._  */
// PS_INSIDEFRAME      5
// PS_NULL             6
//填充的固定类型为以下几种, 其它自定
// SOLID			   -1	   /*实     */
// HS_HORIZONTAL       0       /* ----- */
// HS_VERTICAL         1       /* ||||| */
// HS_FDIAGONAL        2       /* \\\\\ */
// HS_BDIAGONAL        3       /* ///// */
// HS_CROSS            4       /* +++++ */
// HS_DIAGCROSS        5       /* xxxxx */


#include "wingdi.h"

/*#ifdef __cplusplus
extern "C" {
#endif
*/

HBITMAP MyCreateBitmap(
					   HDC hdc,
					   INT cx,
					   INT cy,
					   INT nColors);

typedef struct tagGRADIENTFILL	//过渡填充结构
{
	//*********属性***************

	COLORREF m_crColour1;//过渡颜色1
	COLORREF m_crColour2;//过渡颜色2
	int		 m_nDirection;//过渡方向
	
	//*********操作****************

	tagGRADIENTFILL()
	{
		m_crColour1 = RGB(0, 0, 0);
		m_crColour2 = RGB(255, 255, 255);
		m_nDirection = 0;
	};

	const tagGRADIENTFILL& operator=(const tagGRADIENTFILL& temp)
	{
		m_crColour1 = temp.m_crColour1;
		m_crColour2 = temp.m_crColour2;
		m_nDirection = temp.m_nDirection;
		return *this;
	}

	friend CArchive& operator<<(CArchive& ar, const tagGRADIENTFILL& temp);
	friend CArchive& operator>>(CArchive& ar, tagGRADIENTFILL& temp);

} GRADIENTFILL, far *LPGRADIENTFILL;


typedef struct tagPICTUREFILL	//图片填充结构
{
	//*********属性***************

	CString		m_strFileName;	//图片文件名
	BOOL		m_bTetxTrue;	//是否是纹理
	
	//*********操作****************

	tagPICTUREFILL()
	{
		m_strFileName = _T("");
		m_bTetxTrue   = TRUE;
	};

	const tagPICTUREFILL& operator=(const tagPICTUREFILL& temp)
	{
		m_strFileName = temp.m_strFileName;
		m_bTetxTrue   = temp.m_bTetxTrue;
		return *this;
	}

	friend CArchive& operator<<(CArchive& ar, const tagPICTUREFILL& temp);
	friend CArchive& operator>>(CArchive& ar, tagPICTUREFILL& temp);

} PICTUREFILL, far *LPPICTUREFILL;



typedef struct tagPATTERNFILL// 图案填充结构
{	
	//*********属性***************

	int m_nRow; //
	int m_nCol; //
	COLORREF m_crColourFore;//
	COLORREF m_crColourBack;//
	
	//*********操作****************

	tagPATTERNFILL(){
		m_crColourFore = RGB(0, 0, 0);
		m_crColourBack = RGB(255, 255, 255);
		m_nCol = -1;
		m_nRow = -1;
	};

	const tagPATTERNFILL& operator=(const tagPATTERNFILL& temp)
	{
		m_crColourFore = temp.m_crColourFore;
		m_crColourBack = temp.m_crColourBack;
		m_nCol = temp.m_nCol;
		m_nRow = temp.m_nRow;
		return *this;
	};

	void LoadBitmap(CBitmap *pBitmap);
	BOOL CreateBrush(CBrush *pBrush, CDC *pDC);

	friend CArchive& operator<<(CArchive& ar, const tagPATTERNFILL& temp);
	friend CArchive& operator>>(CArchive& ar, tagPATTERNFILL& temp);
} PATTERNFILL, far *LPPATTERNFILL;


typedef struct tagPATTERNSTYLE// 图案类型结构
{	
	//*********属性***************

	GRADIENTFILL m_GradientFill; // 过渡
	PICTUREFILL  m_PictureFill;  // 图片或纹理
	PATTERNFILL	 m_PatternFill;  // 定制图案
	int          m_nFill;		 // 是何种填充
	
	//*********操作****************

	tagPATTERNSTYLE()
	{
		m_nFill = 1;//定制图案
	};

	const tagPATTERNSTYLE& operator=(const tagPATTERNSTYLE& temp)
	{
		m_nFill = temp.m_nFill;
		m_GradientFill = temp.m_GradientFill;
		m_PatternFill = temp.m_PatternFill;
		m_PictureFill = temp.m_PictureFill;
		return *this;
	};

	BOOL CreateBrush(CBrush *pBrush, CDC *pDC);

	friend CArchive& operator<<(CArchive& ar, const tagPATTERNSTYLE& temp);
	friend CArchive& operator>>(CArchive& ar, tagPATTERNSTYLE& temp);
} PATTERNSTYLE, far *LPPATTERNSTYLE;


typedef struct tagELEMENTCAPTION	// 标题属性结构
{	
	//*********属性***************

	CString		m_strCaption;	// 标题
	BOOL		m_bShow;		// 是否显示
	COLORREF	m_crColour;		// 标题颜色
	LOGFONT		logFont;		// 标题字体
	
	//*********操作****************

	tagELEMENTCAPTION()
	{
		m_bShow = FALSE;
		m_crColour = RGB(0, 0, 0);
		m_strCaption = _T("");
		memset(&logFont, 0, sizeof(LOGFONT) );
		logFont.lfHeight = -50;
		logFont.lfCharSet = GB2312_CHARSET;
		strcpy_s(logFont.lfFaceName, "宋体");
	};

	const tagELEMENTCAPTION& operator=(const tagELEMENTCAPTION& temp)
	{
		m_bShow = temp.m_bShow;
		m_crColour = temp.m_crColour;
		m_strCaption = temp.m_strCaption;
		memcpy(&logFont, &(temp.logFont), sizeof(LOGFONT) );
		return *this;
	};

	friend CArchive& operator<<(CArchive& ar, const tagELEMENTCAPTION& temp);
	friend CArchive& operator>>(CArchive& ar, tagELEMENTCAPTION& temp);
} ELEMENTCAPTION, far *LPELEMENTCAPTION;

typedef struct tagELEMENTLINEFILL		// 线间填充
{
	//*********属性***************

	PATTERNSTYLE	sFillStyle;   // 填充图案
	UINT			m_uSpace;     // 线间距离单位0.1mm
	int				m_nPenStyle;  // 画笔虚实
	BOOL			m_bSquarePen; // 是否为方笔	
	BOOL			m_bShow;	  // 是否显示即是否填充
	
	//*********操作****************

	tagELEMENTLINEFILL()
	{
		m_bShow = FALSE;
		m_uSpace = 30;
		m_nPenStyle = 0;
		m_bSquarePen = TRUE;
		sFillStyle.m_PatternFill.m_crColourFore = RGB(255, 255, 255);
		sFillStyle.m_PatternFill.m_crColourBack = RGB(255, 255, 255);
		sFillStyle.m_PatternFill.m_nCol = -1;
		sFillStyle.m_PatternFill.m_nRow = -1;		
	};

	const tagELEMENTLINEFILL& operator=(const tagELEMENTLINEFILL& temp)
	{
		m_bShow = temp.m_bShow;
		sFillStyle = temp.sFillStyle;
		m_uSpace = temp.m_uSpace;
		m_nPenStyle = temp.m_nPenStyle;
		m_bSquarePen = temp.m_bSquarePen;
		return *this;
	};

	friend CArchive& operator<<(CArchive& ar, const tagELEMENTLINEFILL& temp);
	friend CArchive& operator>>(CArchive& ar, tagELEMENTLINEFILL& temp);
}ELEMENTLINEFILL, far * LPELEMENTLINEFILL;

typedef struct tagELEMENTLINE		// 线结构
{	
	//*********属性***************

	int				m_nPenWide;	// 画笔逻辑宽度
	int				m_nPenStyle;// 画笔虚实
	PATTERNSTYLE	s_LineStyle;// 线的颜色及填充图案
	int				m_nLineType;// 线的线型
	BOOL			m_bShow;	// 是否显示
	BOOL			m_bSmooth;	// 是否光滑
	BOOL			m_bClose;	// 是否封闭
	BOOL			m_bSquarePen; // 是否为方笔
	UINT			m_uBeginPointMode;// 始端样式
	UINT			m_uBeginPointSize;// 始端大小
	UINT			m_uEndPointMode;// 末端样式
	UINT			m_uEndPointSize;// 末端大小
	ELEMENTLINEFILL eLineSpaceFill; // 线间填充
	CString			m_strLineName;// 线的类别名字，如：等值线，断层等

	//*********操作****************

	tagELEMENTLINE(){		
		m_bClose = FALSE;
		m_bSquarePen = TRUE;//缺省为方笔
		m_bSmooth = FALSE;
		m_bShow = TRUE;
		m_nPenWide = 1;
		m_nPenStyle = 0;
		m_strLineName = _T("无");
		m_nLineType = 0;
		m_uBeginPointMode = 0;//始端样式
		m_uBeginPointSize = 30;//始端大小
		m_uEndPointMode = 0;//末端样式
		m_uEndPointSize = 30;//末端大小
	};

	const tagELEMENTLINE& operator=(const tagELEMENTLINE& temp)
	{
		m_bSquarePen = temp.m_bSquarePen;
		m_bClose = temp.m_bClose;
		m_bSmooth = temp.m_bSmooth;
		m_bShow = temp.m_bShow;
		m_nPenWide = temp.m_nPenWide;
		m_nPenStyle = temp.m_nPenStyle;
		m_strLineName = temp.m_strLineName;
		m_nLineType = temp.m_nLineType;
		s_LineStyle = temp.s_LineStyle;
		m_uBeginPointMode = temp.m_uBeginPointMode;//始端样式
		m_uBeginPointSize = temp.m_uBeginPointSize;//始端大小
		m_uEndPointMode = temp.m_uEndPointMode;//末端样式
		m_uEndPointSize = temp.m_uEndPointSize;//末端大小
		eLineSpaceFill = temp.eLineSpaceFill;
		return *this;
	};
public:
	
} ELEMENTLINE, far *LPELEMENTLINE;

CArchive& operator<<(CArchive& ar, const tagELEMENTLINE& temp);
CArchive& operator>>(CArchive& ar, tagELEMENTLINE& temp);

typedef struct tagELEMENTFILL		// 填充结构
{	
	//*********属性***************

	PATTERNSTYLE	sFillStyle;		// 线的颜色及填充图案
	BOOL			m_bShow;		// 是否显示
	BLENDFUNCTION	blendFunction;	// 透明度
	
	//*********操作****************

	tagELEMENTFILL()
	{
		m_bShow = FALSE;
		sFillStyle.m_PatternFill.m_crColourFore = RGB(191, 191, 191);
		sFillStyle.m_PatternFill.m_crColourBack = RGB(255, 255, 255);
		sFillStyle.m_PatternFill.m_nCol = -1;
		sFillStyle.m_PatternFill.m_nRow = -1;
		blendFunction.BlendOp = AC_SRC_OVER;
		blendFunction.BlendFlags = 0;
		blendFunction.AlphaFormat = 0;
		blendFunction.SourceConstantAlpha = 255;
	};

	const tagELEMENTFILL& operator=(const tagELEMENTFILL& temp)
	{
		m_bShow = temp.m_bShow;
		sFillStyle = temp.sFillStyle;
		memcpy(&blendFunction, &temp.blendFunction, sizeof(BLENDFUNCTION) );		
		return *this;
	};

	
} ELEMENTFILL, far *LPELEMENTFILL;

CArchive& operator<<(CArchive& ar, const tagELEMENTFILL& temp);
CArchive& operator>>(CArchive& ar, tagELEMENTFILL& temp);

typedef struct tagELEMENTLABEL	// 标注结构
{
	//*********属性***************

	//LONG		m_lLabel;		// 标注值
	float		m_fLabel;		// 标注值 modified by luoqin in 2002.6.28
	float		m_fSpace;		// 标注间隔(cm)
	BOOL		m_bShow;		// 是否显示
	LOGFONT		logFont;		// 标注字体
	COLORREF	m_crColour;		// 标注颜色
	UINT        m_uDecimal;     // 标注小数点位数
	
	tagELEMENTLABEL(){
		m_fSpace = 5.0f;
		m_fLabel = 0;
		m_uDecimal = 0;
		m_bShow = FALSE;
		m_crColour = RGB(0, 0, 0);
		memset(&logFont, 0, sizeof(LOGFONT) );
		logFont.lfHeight = -35;
		logFont.lfWeight = 900;
		logFont.lfCharSet = ANSI_CHARSET;
		strcpy_s(logFont.lfFaceName, "Arial");		
	};

	const tagELEMENTLABEL& operator=(const tagELEMENTLABEL& temp)
	{
		m_fSpace = temp.m_fSpace;
		m_fLabel = temp.m_fLabel;
		m_uDecimal = temp.m_uDecimal;
		m_bShow = temp.m_bShow;
		m_crColour = temp.m_crColour;
		memcpy(&logFont, &(temp.logFont), sizeof(LOGFONT) );
		
		return *this;
	};

	friend CArchive& operator<<(CArchive& ar, const tagELEMENTLABEL& temp);
	friend CArchive& operator>>(CArchive& ar, tagELEMENTLABEL& temp);	

} ELEMENTLABEL, far *LPELEMENTLABEL;

typedef struct tagELEMENTSTATE	// 图素状态结构
{
	//*********属性***************

	BOOL m_bActive;	// 图素是否激动即处于初建状态
	BOOL m_bNoMove;	// 图素是否移动
	BOOL m_bLock;	// 图素是否锁定
	BOOL m_bShow;	// 是否显示
	BOOL m_bEdit;	// 是否处于编辑状态
	
	//*********操作****************

	tagELEMENTSTATE(){
		m_bActive = FALSE;
		m_bNoMove = FALSE;
		m_bLock   = FALSE;
		m_bShow   = TRUE;
		m_bEdit	  = FALSE;
	};

	const tagELEMENTSTATE& operator=(const tagELEMENTSTATE& temp)
	{
		m_bActive = temp.m_bActive;
		m_bNoMove = temp.m_bLock;
		m_bLock   = temp.m_bNoMove;
		m_bShow   = temp.m_bShow;
		m_bEdit   = temp.m_bEdit;		
		return *this;
	}

	friend CArchive& operator<<(CArchive& ar, const tagELEMENTSTATE& temp);
	friend CArchive& operator>>(CArchive& ar, tagELEMENTSTATE& temp);	

} ELEMENTSTATE, far *LPELEMENTSTATE;

typedef struct tagELEMENTPROPERTY		// 图素属性
{
	//*********属性***************

	ELEMENTCAPTION	eCaption;			// 图素标题
	ELEMENTLINE		eLine;				// 图素线属性
	ELEMENTFILL		eFill;				// 图素填充属性
	ELEMENTLABEL	eLabel;				// 图素标注属性
	CRect			m_rtPosition;		// 图素的显示的最大矩形逻辑范围
	int				m_nMovingHandle;	// 正在移动的手柄
	CString			m_strElementName;	// 图素类别名字，如井、直线等
	
	//*********操作****************

	tagELEMENTPROPERTY()
	{
		m_rtPosition.left = 0;
		m_rtPosition.top = 0;
		m_rtPosition.right = 0;
		m_rtPosition.bottom = 0;	
		m_nMovingHandle = -1;
		m_strElementName = _T("");	
	};

	const tagELEMENTPROPERTY& operator=(const tagELEMENTPROPERTY& temp)
	{
		m_nMovingHandle = -1;
		m_strElementName = temp.m_strElementName;	
		eCaption = temp.eCaption;
		eLine = temp.eLine;
		eFill = temp.eFill;
		eLabel = temp.eLabel;
		m_rtPosition = temp.m_rtPosition;
		return *this;
	};

	friend CArchive& operator<<(CArchive& ar, const tagELEMENTPROPERTY& temp);
	friend CArchive& operator>>(CArchive& ar, tagELEMENTPROPERTY& temp);	

} ELEMENTPROPERTY, far *LPELEMENTPROPERTY;


/*#ifdef __cplusplus
}
#endif*/
#endif  // __COMMONSTRUCT_H__
