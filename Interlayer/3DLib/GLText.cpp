#include "stdafx.h"
#include "GLText.h"

CGLText::CGLText()
{
	m_dX = 0;
	m_dY = 0;
	m_str = "abc中国";
	m_dHeight = 100;
	m_iDisplayList = 0;
}   

CGLText::~CGLText()
{
	Free();
}   

BOOL CGLText::GenCharsLists(char* strFontName)
{
	HDC hdc;
	const   char* str = m_str.c_str();
	hdc = CreateDC("DISPLAY", "", "", NULL);
	
	int iNum = _mbslen((const unsigned char*)str);
	m_iDisplayList = glGenLists(iNum + 1);
	
	HFONT hNewCFont;
	
	LOGFONT CLogFont;   //存储当前字体参数
	
	//初始化字体参数
	memset(&CLogFont, 0, sizeof(LOGFONT));

	CLogFont.lfEscapement = CLogFont.lfOrientation = 0;
	CLogFont.lfWeight = FW_NORMAL;
	CLogFont.lfCharSet = GB2312_CHARSET;
	CLogFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	CLogFont.lfQuality = DEFAULT_QUALITY;
	CLogFont.lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
	strcpy_s(CLogFont.lfFaceName, LPCTSTR(strFontName));
	CLogFont.lfHeight = -10;
	CLogFont.lfWidth = 0;

	hNewCFont = CreateFontIndirect(&CLogFont);

	HFONT hOldFont = (HFONT)SelectObject(hdc, hNewCFont);
	int i = 0,j = 0,iTotal = strlen(str);
	UINT nChar = 0;
	while (i < iTotal)
	{
		j++;   

		if (IsDBCSLeadByte(str[i]))
		{
			nChar = ((unsigned char)str[i]) * 0x100 + (unsigned   char)str[i + 1];
			i += 2;
		}
		else
		{
			nChar = str[i];
			i++;
		}

		GLYPHMETRICSFLOAT agmf[1];
		BOOL bOK = wglUseFontOutlines(hdc, nChar, 1, m_iDisplayList + j, 0.002f, 0, WGL_FONT_POLYGONS, agmf);
	}

	SelectObject(hdc, hOldFont);
	DeleteObject(hNewCFont);
	DeleteDC(hdc);

	return   TRUE;
}


BOOL CGLText::GenList()
{
	int iNum = _mbslen((const   unsigned   char*)m_str.c_str());

	glNewList(m_iDisplayList, GL_COMPILE);

	for (int i = 1; i <= iNum; i++)
	{
		glPushMatrix();

		//此处修改文字间距
		glTranslated(m_dX + m_dHeight * (i - 1), m_dY, 0);
		glScaled(m_dHeight, m_dHeight, 1);
		glCallList(m_iDisplayList + i);
		glPopMatrix();
	}
	glEndList();

	return TRUE;
}

void CGLText::Free()
{
	if (glIsList(m_iDisplayList))
		glDeleteLists(m_iDisplayList, _mbslen((const   unsigned   char*)m_str.c_str()) + 1);

	m_iDisplayList = 0;
}

void CGLText::Draw(char* strFontName)
{
	if (!glIsList(m_iDisplayList))
	{
		GenCharsLists(strFontName);
		GenList();
	}
	
	glCallList(m_iDisplayList);
}
