
#ifndef __COMMONSTRUCT_H__
#define __COMMONSTRUCT_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//�ʵĹ̶�����ֵ��Ϊ���¼��֣������Զ�
// PS_SOLID            0
// PS_DASH             1       /* -------  */
// PS_DOT              2       /* .......  */
// PS_DASHDOT          3       /* _._._._  */
// PS_DASHDOTDOT       4       /* _.._.._  */
// PS_INSIDEFRAME      5
// PS_NULL             6
//���Ĺ̶�����Ϊ���¼���, �����Զ�
// SOLID			   -1	   /*ʵ     */
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

typedef struct tagGRADIENTFILL	//�������ṹ
{
	//*********����***************

	COLORREF m_crColour1;//������ɫ1
	COLORREF m_crColour2;//������ɫ2
	int		 m_nDirection;//���ɷ���
	
	//*********����****************

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


typedef struct tagPICTUREFILL	//ͼƬ���ṹ
{
	//*********����***************

	CString		m_strFileName;	//ͼƬ�ļ���
	BOOL		m_bTetxTrue;	//�Ƿ�������
	
	//*********����****************

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



typedef struct tagPATTERNFILL// ͼ�����ṹ
{	
	//*********����***************

	int m_nRow; //
	int m_nCol; //
	COLORREF m_crColourFore;//
	COLORREF m_crColourBack;//
	
	//*********����****************

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


typedef struct tagPATTERNSTYLE// ͼ�����ͽṹ
{	
	//*********����***************

	GRADIENTFILL m_GradientFill; // ����
	PICTUREFILL  m_PictureFill;  // ͼƬ������
	PATTERNFILL	 m_PatternFill;  // ����ͼ��
	int          m_nFill;		 // �Ǻ������
	
	//*********����****************

	tagPATTERNSTYLE()
	{
		m_nFill = 1;//����ͼ��
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


typedef struct tagELEMENTCAPTION	// �������Խṹ
{	
	//*********����***************

	CString		m_strCaption;	// ����
	BOOL		m_bShow;		// �Ƿ���ʾ
	COLORREF	m_crColour;		// ������ɫ
	LOGFONT		logFont;		// ��������
	
	//*********����****************

	tagELEMENTCAPTION()
	{
		m_bShow = FALSE;
		m_crColour = RGB(0, 0, 0);
		m_strCaption = _T("");
		memset(&logFont, 0, sizeof(LOGFONT) );
		logFont.lfHeight = -50;
		logFont.lfCharSet = GB2312_CHARSET;
		strcpy_s(logFont.lfFaceName, "����");
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

typedef struct tagELEMENTLINEFILL		// �߼����
{
	//*********����***************

	PATTERNSTYLE	sFillStyle;   // ���ͼ��
	UINT			m_uSpace;     // �߼���뵥λ0.1mm
	int				m_nPenStyle;  // ������ʵ
	BOOL			m_bSquarePen; // �Ƿ�Ϊ����	
	BOOL			m_bShow;	  // �Ƿ���ʾ���Ƿ����
	
	//*********����****************

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

typedef struct tagELEMENTLINE		// �߽ṹ
{	
	//*********����***************

	int				m_nPenWide;	// �����߼����
	int				m_nPenStyle;// ������ʵ
	PATTERNSTYLE	s_LineStyle;// �ߵ���ɫ�����ͼ��
	int				m_nLineType;// �ߵ�����
	BOOL			m_bShow;	// �Ƿ���ʾ
	BOOL			m_bSmooth;	// �Ƿ�⻬
	BOOL			m_bClose;	// �Ƿ���
	BOOL			m_bSquarePen; // �Ƿ�Ϊ����
	UINT			m_uBeginPointMode;// ʼ����ʽ
	UINT			m_uBeginPointSize;// ʼ�˴�С
	UINT			m_uEndPointMode;// ĩ����ʽ
	UINT			m_uEndPointSize;// ĩ�˴�С
	ELEMENTLINEFILL eLineSpaceFill; // �߼����
	CString			m_strLineName;// �ߵ�������֣��磺��ֵ�ߣ��ϲ��

	//*********����****************

	tagELEMENTLINE(){		
		m_bClose = FALSE;
		m_bSquarePen = TRUE;//ȱʡΪ����
		m_bSmooth = FALSE;
		m_bShow = TRUE;
		m_nPenWide = 1;
		m_nPenStyle = 0;
		m_strLineName = _T("��");
		m_nLineType = 0;
		m_uBeginPointMode = 0;//ʼ����ʽ
		m_uBeginPointSize = 30;//ʼ�˴�С
		m_uEndPointMode = 0;//ĩ����ʽ
		m_uEndPointSize = 30;//ĩ�˴�С
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
		m_uBeginPointMode = temp.m_uBeginPointMode;//ʼ����ʽ
		m_uBeginPointSize = temp.m_uBeginPointSize;//ʼ�˴�С
		m_uEndPointMode = temp.m_uEndPointMode;//ĩ����ʽ
		m_uEndPointSize = temp.m_uEndPointSize;//ĩ�˴�С
		eLineSpaceFill = temp.eLineSpaceFill;
		return *this;
	};
public:
	
} ELEMENTLINE, far *LPELEMENTLINE;

CArchive& operator<<(CArchive& ar, const tagELEMENTLINE& temp);
CArchive& operator>>(CArchive& ar, tagELEMENTLINE& temp);

typedef struct tagELEMENTFILL		// ���ṹ
{	
	//*********����***************

	PATTERNSTYLE	sFillStyle;		// �ߵ���ɫ�����ͼ��
	BOOL			m_bShow;		// �Ƿ���ʾ
	BLENDFUNCTION	blendFunction;	// ͸����
	
	//*********����****************

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

typedef struct tagELEMENTLABEL	// ��ע�ṹ
{
	//*********����***************

	//LONG		m_lLabel;		// ��עֵ
	float		m_fLabel;		// ��עֵ modified by luoqin in 2002.6.28
	float		m_fSpace;		// ��ע���(cm)
	BOOL		m_bShow;		// �Ƿ���ʾ
	LOGFONT		logFont;		// ��ע����
	COLORREF	m_crColour;		// ��ע��ɫ
	UINT        m_uDecimal;     // ��עС����λ��
	
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

typedef struct tagELEMENTSTATE	// ͼ��״̬�ṹ
{
	//*********����***************

	BOOL m_bActive;	// ͼ���Ƿ񼤶������ڳ���״̬
	BOOL m_bNoMove;	// ͼ���Ƿ��ƶ�
	BOOL m_bLock;	// ͼ���Ƿ�����
	BOOL m_bShow;	// �Ƿ���ʾ
	BOOL m_bEdit;	// �Ƿ��ڱ༭״̬
	
	//*********����****************

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

typedef struct tagELEMENTPROPERTY		// ͼ������
{
	//*********����***************

	ELEMENTCAPTION	eCaption;			// ͼ�ر���
	ELEMENTLINE		eLine;				// ͼ��������
	ELEMENTFILL		eFill;				// ͼ���������
	ELEMENTLABEL	eLabel;				// ͼ�ر�ע����
	CRect			m_rtPosition;		// ͼ�ص���ʾ���������߼���Χ
	int				m_nMovingHandle;	// �����ƶ����ֱ�
	CString			m_strElementName;	// ͼ��������֣��羮��ֱ�ߵ�
	
	//*********����****************

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
