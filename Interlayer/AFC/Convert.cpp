#include "StdAfx.h"
#include "Convert.h"

extern "C" 
bool StrIsValidInteger(LPCTSTR lpStr)
{	
	//
	if( strlen(lpStr) < 1)
		return false;

	TCHAR ch = lpStr[0];
	if (((ch <= '0') || (ch >= '9')) && (ch!='-') )
		return false;
	
	if( (strlen(lpStr) == 1) && (ch == '-'))
		return false;

	for (int i=1; i<strlen(lpStr); i++)
	{
		ch = lpStr[i];
		if ((ch <= '0') || (ch >= '9'))
			return false;
	}

	return true;
}

extern "C" 
bool StrIsValidFloat(LPCTSTR lpStr)
{
	int i, NumCount, PointCount, SingCount, PointPos, SingPos;
	if( strlen(lpStr) < 1)
		return false;

	NumCount = 0;
	PointCount = 0;
	SingCount = 0;

	PointPos = -1;
	SingPos = -1;

	TCHAR ch;

	for(i= 0; i < strlen(lpStr); i++)
	{
		ch = lpStr[i];
		//数字
		if ((ch >= '0') && (ch <= '9'))
			NumCount ++;
		//小数点
		else if (ch == '.')
		{
			PointPos= i;
			PointCount++;
		}
		//负号
		else if (ch == '-')
		{
			SingPos= i;
			SingCount++;
		}
		//其它字符
		else
			return false;
	}

	//注意:以下形式都是有效的浮点数,可以自行用StrToFloat()测试一下就知道了
	//////////////////////////////////////////////////////////////////////////////
	//.
	//.1
	//0.1
	//-.
	//-.1
	//-0.1
	//////////////////////////////////////////////////////////////////////////////

	if( PointCount > 1)//两个以上的小数点,肯定错误
		return false; 

	if( SingCount > 1 )//两个以上的负号-,肯定错误
		return false; 
	
	//有符号
	if (SingCount > 0)
	{
		if(SingPos != 0) //有负号,但负号不是最开头,肯定错误
			return false;
		if ((NumCount == 0) 
			&& (PointCount == 0)) //只有一个负号,没有数字,也没有点号,肯定错误.
			return false;
	}
	//
	return true;
}