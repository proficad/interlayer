#pragma once

//////////////////////////////////////////////////////////////////////////
// |---------------------------------------------------------------------  
// |  ʹ�÷�����
// |  CIni ini("c:\\a.ini");
// |  int n;
// 
// |  ���ֵ
// |  TRACE("%s",ini.GetValue("��1","��1"));
// |  
// |  
// |  ���ֵ
// |  ini.SetValue("�Զ����","��1","ֵ");
// |  ini.SetValue("�Զ����2","��1","ֵ",false);
// |  
// |  
// |  ö��ȫ������
// |  CStringArray arrSection;
// |  n=ini.GetSections(arrSection);
// |  for(int i=0;i<n;i++)
// |  TRACE("%s\n",arrSection[i]);
// |  
// |  
// |  ö��ȫ��������ֵ
// |  CStringArray arrKey,arrValue;
// |  n=ini.GetKeyValues(arrKey,arrValue,"��1");
// |  for(int i=0;i<n;i++)
// |  TRACE("����%s\nֵ��%s\n",arrKey[i],arrValue[i]);
// |  
// |  
// |  ɾ����ֵ
// |  ini.DelKey("��1","��1");
// |  
// |  
// |  ɾ����
// |  ini.DelSection("��1");
// |  
// |  
// |  ɾ��ȫ��
// |  ini.DelAllSections();
// |---------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////

class AFX_EXT_CLASS CIni
{
private:
	CString m_strFileName;
public:
	CIni(CString strFileName):m_strFileName(strFileName){}
	CIni(){}
public:
	//һ���Բ�����
	BOOL SetFileName(LPCTSTR lpFileName); //�����ļ���
	CString GetFileName(void); //����ļ���
	BOOL SetValue(LPCTSTR lpSection, LPCTSTR lpKey, LPCTSTR lpValue,bool bCreate=true); //���ü�ֵ��bCreate��ָ����������δ����ʱ���Ƿ񴴽���
	CString GetValueString(LPCTSTR lpSection, LPCTSTR lpKey); //�õ���ֵ.
	UINT GetValueInt(LPCTSTR lpSection, LPCTSTR lpKey);
	BOOL DelSection(LPCTSTR strSection); //ɾ������
	BOOL DelKey(LPCTSTR lpSection, LPCTSTR lpKey); //ɾ������

public:
	//�߼�������
	int GetSections(CStringArray& arrSection); //ö�ٳ�ȫ���Ķ���
	int GetKeyValues(CStringArray& arrKey,CStringArray& arrValue,LPCTSTR lpSection); //ö�ٳ�һ���ڵ�ȫ��������ֵ	
	BOOL DelAllSections();	
}; 