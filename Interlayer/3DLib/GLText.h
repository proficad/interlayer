#pragma once

#include <string>   

using namespace std;   

/*

Ӧ�÷�ʽ���£�

CGLText   text;
CGLText   text2;

text2.m_dX=0;
text2.m_dY=200;
glLoadIdentity();
glClear(GL_COLOR_BUFFER_BIT);
glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
text.Draw("����");
text2.Draw("����_GB2312");
glFlush();

*/
class CGLText
{
public:
	//��������
	CGLText();
	virtual ~CGLText();

	//�����ƶ���������֣�����ֻ�ڵ�һ�λ���ʱ�������ã�֮����Դ����ֵ
	void Draw(char* strFontName);

	//�ͷ�������ռ�ռ�
	void Free();
	
	//�����ַ���
	string m_str;   

	//�ַ����߶�
	double m_dHeight;   

	//�ַ���λ��
	double m_dX;
	double m_dY;
	double m_dZ;

protected:
	BOOL GenList();
	BOOL GenCharsLists(char* strFontName);
	int m_iDisplayList;
};   

