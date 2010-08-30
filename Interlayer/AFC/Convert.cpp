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
		//����
		if ((ch >= '0') && (ch <= '9'))
			NumCount ++;
		//С����
		else if (ch == '.')
		{
			PointPos= i;
			PointCount++;
		}
		//����
		else if (ch == '-')
		{
			SingPos= i;
			SingCount++;
		}
		//�����ַ�
		else
			return false;
	}

	//ע��:������ʽ������Ч�ĸ�����,����������StrToFloat()����һ�¾�֪����
	//////////////////////////////////////////////////////////////////////////////
	//.
	//.1
	//0.1
	//-.
	//-.1
	//-0.1
	//////////////////////////////////////////////////////////////////////////////

	if( PointCount > 1)//�������ϵ�С����,�϶�����
		return false; 

	if( SingCount > 1 )//�������ϵĸ���-,�϶�����
		return false; 
	
	//�з���
	if (SingCount > 0)
	{
		if(SingPos != 0) //�и���,�����Ų����ͷ,�϶�����
			return false;
		if ((NumCount == 0) 
			&& (PointCount == 0)) //ֻ��һ������,û������,Ҳû�е��,�϶�����.
			return false;
	}
	//
	return true;
}