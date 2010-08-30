#pragma once
#include "globject.h"
#include "Vertex3D.h"

class C3DSlice : public CGLObject
{
public:
	C3DSlice(void);
	~C3DSlice(void);
	//Ax+By+Cz+D
	double A, B, C, D;			// 平面裁剪,这四个系数分别是裁剪平面Ax+By+Cz+D=0的A、B、C、D值
	double m_dHeight;			// 切片厚度
	bool m_bSpecified;			// 是否按指定厚度
	bool m_bShowSlicePlane;		// 是否显示切面

	bool IsVertSlice() { return (C==0.0);}  // 是否为纵切片
	virtual CGLObject* Copy();
	virtual void DefineDisplay(){};
	virtual void Display(const GLDisplayMode& = GLWIREFRAME, bool bForce = false){};
	virtual void DisplaySelected(const GLSelectedMode& = GLSELECTED){};
	virtual void Hilight(const GLSelectedMode&){};
	virtual bool BuildList(){ return true;};

	// 获取此剪裁面在坐标框上的交点
	void GetCoordinate(double *eq, CVertex3D *pCoord);	

	// 保存
	virtual void Serialize(CArchive& ar);
};
