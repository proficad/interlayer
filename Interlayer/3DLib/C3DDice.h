#pragma once
#include "globject.h"
#include "Vertex3D.h"
#include <vector>

class C3DDice :	public CGLObject
{
public:
	C3DDice(void);
	~C3DDice(void);
	std::vector<CVertex3D>* m_pointList;
	virtual CGLObject* Copy();
	virtual void DefineDisplay();
	virtual void Display(const GLDisplayMode& = GLWIREFRAME, bool bForce = false);
	virtual void DisplaySelected(const GLSelectedMode& = GLSELECTED);
	virtual void Hilight(const GLSelectedMode&);
	virtual bool BuildList();

	// ±£´æ
	virtual void Serialize(CArchive& ar);
};
