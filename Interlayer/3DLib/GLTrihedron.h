// GLTrihedron.h: interface for the CGLTrihedron class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLTRIHEDRON_H__F1DE074D_EC62_4F52_9890_7612816C931B__INCLUDED_)
#define AFX_GLTRIHEDRON_H__F1DE074D_EC62_4F52_9890_7612816C931B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GLObject.h"
#include "background.h"
#include "ColorLookUpTable.h"
#include "../Vector3df.h"
class CGLView;

class CGLTrihedron : public CGLObject  // 三面体
{
public:
	CGLTrihedron();
	CGLTrihedron(GLfloat size);
	virtual ~CGLTrihedron();
	virtual CGLObject* Copy();
	virtual bool BuildList();
	virtual void DefineDisplay();						// 定义显示
	virtual void Display(const GLDisplayMode& = GLWIREFRAME, bool bForce = false);
	virtual void DisplaySelected(const GLSelectedMode& = GLSELECTED);
	virtual void Hilight(const GLSelectedMode&);
	void SetSize(GLfloat size);

	// Color
	virtual void SetColor(const GLubyte&, const GLubyte&, const GLubyte&);
	virtual void SetColor(CColor& color);
	virtual void GetColor(GLubyte*) const;
	virtual CColor* GetColor(void);

	// 画图例
	void ShowLegend(bool bLegend);
	bool IsShowLegend();

	void DrawLegend();
	void DrawMark();
	void SetGLView(CGLView* pGLView);

	// 坐标轴
	//
	void ShowAxis(bool bShow) { m_Show = m_bShowAxis = bShow;}
	bool IsShowAxis() {return m_bShowAxis;}
	void ShowGird(bool bShow) { m_bShowGrid = bShow;}
	bool IsShowGird() {return m_bShowGrid;}

	void DrawAxis();
	void DrawGrid();

private:
	GLfloat csSize;
	GLfloat m_dDepth;
	//
	bool m_bLegend;
	CGLView *m_pGLView;
	//
	bool m_bShowAxis;
	bool m_bShowGrid;
	//
	void ComputeBoundLimits();
public:
	CBackground m_background;
};

//class CVector3DF;
class CGLEditAxis : public CGLObject
{
public:
	CGLEditAxis();
	CGLEditAxis(GLfloat size);
	~CGLEditAxis();

	virtual CGLObject* Copy();
	virtual bool BuildList();
	virtual void DefineDisplay();						// 定义显示
	virtual void Display(const GLDisplayMode& = GLWIREFRAME, bool bForce = false);
	virtual void DisplaySelected(const GLSelectedMode& = GLSELECTED);
	virtual void Hilight(const GLSelectedMode&);
	void	DrawShaded();
	void SetSize(GLfloat size);

	// Color
	virtual void SetColor(const GLubyte&, const GLubyte&, const GLubyte&);
	virtual void SetColor(CColor& color);
	virtual void GetColor(GLubyte*) const;
	virtual CColor* GetColor(void);

	//position
	void	SetPosition(const CVector3DF& position);
	CVector3DF GetPosition();

	void SetGLView(CGLView* pGLView);
protected:
	bool				m_show;
	CVector3DF	m_v3Position;
	float			m_size;
	CGLView		*m_pGLView;
	bool				m_bShowAxis;
};
#endif // !defined(AFX_GLTRIHEDRON_H__F1DE074D_EC62_4F52_9890_7612816C931B__INCLUDED_)
