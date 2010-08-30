#pragma once
#include "globject.h"
#include "Vertex3D.h"

class C3DSlice : public CGLObject
{
public:
	C3DSlice(void);
	~C3DSlice(void);
	//Ax+By+Cz+D
	double A, B, C, D;			// ƽ��ü�,���ĸ�ϵ���ֱ��ǲü�ƽ��Ax+By+Cz+D=0��A��B��C��Dֵ
	double m_dHeight;			// ��Ƭ���
	bool m_bSpecified;			// �Ƿ�ָ�����
	bool m_bShowSlicePlane;		// �Ƿ���ʾ����

	bool IsVertSlice() { return (C==0.0);}  // �Ƿ�Ϊ����Ƭ
	virtual CGLObject* Copy();
	virtual void DefineDisplay(){};
	virtual void Display(const GLDisplayMode& = GLWIREFRAME, bool bForce = false){};
	virtual void DisplaySelected(const GLSelectedMode& = GLSELECTED){};
	virtual void Hilight(const GLSelectedMode&){};
	virtual bool BuildList(){ return true;};

	// ��ȡ�˼�������������ϵĽ���
	void GetCoordinate(double *eq, CVertex3D *pCoord);	

	// ����
	virtual void Serialize(CArchive& ar);
};
