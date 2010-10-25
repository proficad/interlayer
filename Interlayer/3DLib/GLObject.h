// GLObject.h: interface for the CGLObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLOBJECT_H__D32E3D3F_1394_41B3_ADD0_59A1B08F9FA4__INCLUDED_)
#define AFX_GLOBJECT_H__D32E3D3F_1394_41B3_ADD0_59A1B08F9FA4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GLTypeEnums.h"
#include "GLMaterials.h"
#include "BoundingBox.h"
#include "Geometry.h"
#include "Material.h"
#include "Color.h"

class CGLTrihedron;
class CGLDisplayContext;

class CGLObject
{
	friend class CGLTrihedron;
public:
	CGLObject();
	virtual ~CGLObject();

	virtual CGLObject* Copy() = 0;
	virtual void DefineDisplay() = 0;
	virtual void Display(const GLDisplayMode& = GLWIREFRAME, bool bForce = false) = 0;
	virtual void DisplaySelected(const GLSelectedMode& = GLSELECTED) = 0;
	virtual void Hilight(const GLSelectedMode&) = 0;
	virtual bool BuildList() = 0;
	
	virtual void DrawText(LPCTSTR string);
	void get_hz(LPCTSTR incode,GLubyte *bytes);	

	void print (float x, float y, float z, char* format, ...);

	int GetCount() const
	{
		return m_glObjCount;
	} // 图素计数
	
	unsigned long int GetObjID() const
	{
		return m_glObjID;
	}// 图素ID

	void SetDisplayMode(const GLDisplayMode&);
	GLDisplayMode GetDisplayMode() const;

	void SetObjName(LPCTSTR lpszName)
	{
		m_strObjName = lpszName;
	}

	LPCTSTR GetObjName() 
	{
		return m_strObjName;
	}

	//////////////////////////////////////////////////////////////////////////
	// 材质
	virtual void ApplyMaterial();
	virtual void SetMaterial(float*mat);
	virtual void SetMaterial(const GLMaterial&mat);
	virtual void SetMaterial(CMaterial* pMat);
	virtual void SetMaterial(CMaterial& mat);
	CMaterial* GetMaterial(){return &m_Material;}

	// Color
	virtual void SetColorBinding(COLORBINDING type);
	virtual COLORBINDING GetColorBinding(void);

	virtual void SetColor(const GLubyte&, const GLubyte&, const GLubyte&);
	virtual void SetColor(CColor& color);
	virtual void GetColor(GLubyte*) const;
	virtual CColor* GetColor(void);

	void SetBoundingBox(const CBoundingBox&);
	CBoundingBox GetBoundingBox() const;

	virtual void SetGLObjType(const GLObjectType& type);
	virtual GLObjectType GetGLObjType() const;
	bool IsOfType(const GLObjectType&) const;

	void SetContext(CGLDisplayContext* pContext) { m_pDisplayContext = pContext; }
	CGLDisplayContext* GetContext( ) { return m_pDisplayContext; }

	virtual CGeometry* Geometry() const
	{
		return 0;
	}

	// show
	void Show(int flag)
	{
		m_Show = flag;
	}
	bool isShow()
	{
		return (m_Show != 0);
	}
	// Modif
	void SetModified(int flag = 1)
	{
		m_Modified = flag;
	}
	int GetModified()
	{
		return m_Modified;
	}	
	virtual void Serialize(CArchive& ar)
	{
		m_itsBox.Serialize(ar);
		if( ar.IsStoring() )
		{
			ar << (WORD)m_displayMode;
			ar << m_strObjName;
			ar << m_Show;
			ar << m_Color;
			ar << m_Material;
		}
		else
		{
			WORD wd;
			ar >> wd;
			m_displayMode = (GLDisplayMode)wd;
			ar >> m_strObjName;
			ar >> m_Show;
			ar >> m_Color;
			ar >> m_Material;
		}
	}

	GLubyte itsShadeRed;
	GLubyte itsShadeGreen;
	GLubyte itsShadeBlue;

	CString							m_strGUID;
protected:
	GLObjectType		m_glObjType;		// 图素类型
	static int			m_glObjCount;		// 图素计数
	unsigned long int	m_glObjID;			// 图素ID
	
	COLORBINDING		m_ColorBinding;		// 定义顶点或面的颜色 ? (Gouraud)着色  颜色模式

	CBoundingBox		m_itsBox;			// 范围

	unsigned int		m_ListOpenGL;		// 显示列表
	unsigned int		m_ListDone;			// 列表是否已经完成
	int					m_Modified;			// 是否修改了

	GLDisplayMode		m_displayMode;		// 显示模式
	CString				m_strObjName;		// 图素名称
	int					m_Show;				// 是否显示

	CGLDisplayContext* m_pDisplayContext;	//

	CColor				m_Color;			// 颜色
	CMaterial			m_Material;			// 材质
};

#endif // !defined(AFX_GLOBJECT_H__D32E3D3F_1394_41B3_ADD0_59A1B08F9FA4__INCLUDED_)
