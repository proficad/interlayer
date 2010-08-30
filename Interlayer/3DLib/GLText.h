#pragma once

#include <string>   

using namespace std;   

/*

应用方式如下：

CGLText   text;
CGLText   text2;

text2.m_dX=0;
text2.m_dY=200;
glLoadIdentity();
glClear(GL_COLOR_BUFFER_BIT);
glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
text.Draw("宋体");
text2.Draw("楷体_GB2312");
glFlush();

*/
class CGLText
{
public:
	//构造文字
	CGLText();
	virtual ~CGLText();

	//绘制制定字体的文字，字体只在第一次绘制时进行设置，之后可以传入空值
	void Draw(char* strFontName);

	//释放文字所占空间
	void Free();
	
	//文字字符串
	string m_str;   

	//字符串高度
	double m_dHeight;   

	//字符串位置
	double m_dX;
	double m_dY;
	double m_dZ;

protected:
	BOOL GenList();
	BOOL GenCharsLists(char* strFontName);
	int m_iDisplayList;
};   

