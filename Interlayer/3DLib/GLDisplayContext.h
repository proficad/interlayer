//############################################################################
//
// GLDisplayContext.h: interface for the CGLDisplayContext class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLDISPLAYCONTEXT_H__D04FD53F_4852_4D6C_957A_44E8AE0C54DB__INCLUDED_)
#define AFX_GLDISPLAYCONTEXT_H__D04FD53F_4852_4D6C_957A_44E8AE0C54DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "GLObject.h"
#include "BoundingBox.h"
#include "colorlookuptable.h"

class CGLView;

class CGLDisplayContext  // 显示场景
{
public:
	CGLDisplayContext();
	CGLDisplayContext(CDocument* pDoc);
	virtual ~CGLDisplayContext();
public:
	// Data
	CColorLookUpTable* GetColorTable(){return &m_ColorTable;}		// 图例颜色
	//
	std::vector<CGLObject*>* m_listDisplay;							// 显示链表
	std::vector<CGLObject*>* m_listSelect;							// 选择链表

	std::vector<CGLObject*>* m_listDice;							// 切块链表
	std::vector<CGLObject*>* m_listSlice;							// 切片链表

	double m_dSliceWidth;											// 切片厚度 纵切时
	double m_dSliceHeight;											// 切片厚度 横切时

	//------------------------------------------------------------------------
	bool IsShowDice();												// 是否显示切块
	bool IsShowSlice();												// 是否显示切片

	//------------------------------------------------------------------------
	bool m_bHLRemoved;												// 消隐线
	void SetHLRemoved(bool b)
	{
		m_bHLRemoved = b;
		SetModifiedFlag();
	}

	//------------------------------------------------------------------------
	bool m_bSmooth;													// 过渡显示
	void SetSmooth(bool bSmooth)
	{
		m_bSmooth = bSmooth;
		SetModifiedFlag();
	}

	//------------------------------------------------------------------------
	GLSelectedMode m_selectedMode;									// 选择显示模式
	GLSelectedMode GetSelectedMode(){return m_selectedMode;}
	void SetSelectedMode(const GLSelectedMode& aMode)
	{
		m_selectedMode = aMode;
		SetModifiedFlag();
	}

	//------------------------------------------------------------------------
	GLDisplayMode m_displayMode;									// 显示模式
	GLDisplayMode GetDisplayMode(){return m_displayMode;}
	void SetDisplayMode(const GLDisplayMode& aMode)
	{
		m_displayMode = aMode;
		SetModifiedFlag();
	}

	//------------------------------------------------------------------------
	bool m_antialias;												// 反走样
	void SetAntiAliasing(const bool& mode)
	{
		m_antialias = mode;
		SetModifiedFlag();
	}
	//------------------------------------------------------------------------
	double m_dVertical;												// 垂向大小
	double GetVertical() { return m_dVertical;}
	double GetVerticalOriginal();
	void SetVertical(double v) 
	{ 
		m_dVertical = v; 
		m_ptrDoc->SetModifiedFlag();
	}

	//------------------------------------------------------------------------
	bool	m_bSmoothGround;										// 是否过渡显示
	GLfloat m_bkRed;												// 背景颜色
	GLfloat m_bkGreen;
	GLfloat m_bkBlue;

	void SetSmoothGround(bool bSmooth) { m_bSmoothGround = bSmooth; }
	bool GetSmoothGround() { return m_bSmoothGround; }

	void GetBackgroundColor(GLfloat& r, GLfloat& g, GLfloat& b)
	{
		r = m_bkRed;
		g = m_bkGreen;
		b = m_bkBlue;
	}

	void SetBackgroundColor(GLfloat r, GLfloat g, GLfloat b)
	{
		m_bkRed = r;
		m_bkGreen = g;
		m_bkBlue = b;
		m_ptrDoc->SetModifiedFlag();
	}

	//------------------------------------------------------------------------
	bool m_bLegend;						// 显示图例
	bool m_bShowAxis;					// 显示坐标框
	bool m_bShowGrid;					// 显示坐标网格
	void ShowLegend(bool bLegend) {m_bLegend = bLegend;}
	bool IsShowLegend() {return m_bLegend;}
	void ShowAxis(bool bShow) { m_bShowAxis = bShow;}
	bool IsShowAxis() {return m_bShowAxis;}
	void ShowGird(bool bShow) { m_bShowGrid = bShow;}
	bool IsShowGird() {return m_bShowGrid;}
	//------------------------------------------------------------------------
	GLdouble m_dRange;												// 范围	
	GLdouble GetRange() { return m_dRange;}
	void SetRange(double range) { m_dRange = range; m_ptrDoc->SetModifiedFlag();}

	//------------------------------------------------------------------------
	GLdouble m_xRot;												// 旋转角度
	GLdouble m_yRot;
	GLdouble m_zRot;
	void RotateXY(double x, double y) 
	{
		m_xRot = x;
		m_yRot = y;
		m_ptrDoc->SetModifiedFlag();
	}
	void RotateXZ(double x, double z) 
	{
		m_xRot = x;
		m_zRot = z;
		m_ptrDoc->SetModifiedFlag();
	}
	void RotateYZ(double y, double z) 
	{
		m_yRot = y;
		m_zRot = z;
		m_ptrDoc->SetModifiedFlag();
	}
	void RotateXYZ(double x, double y, double z) 
	{
		m_xRot = x;
		m_yRot = y;
		m_zRot = z;
		m_ptrDoc->SetModifiedFlag();
	}
	//------------------------------------------------------------------------
	GLdouble m_xTrans;												// 转换
	GLdouble m_yTrans;
	GLdouble m_zTrans;
	void TransformXY(double x, double y) 
	{
		m_xTrans = x;
		m_yTrans = y;
		m_ptrDoc->SetModifiedFlag();
	}
	void TransformXZ(double x, double z) 
	{
		m_xTrans = x;
		m_zTrans = z;
		m_ptrDoc->SetModifiedFlag();
	}
	void TransformYZ(double y, double z) 
	{
		m_yTrans = y;
		m_zTrans = z;
		m_ptrDoc->SetModifiedFlag();
	}
	void TransformXYZ(double x, double y, double z) 
	{
		m_xTrans = x;
		m_yTrans = y;
		m_zTrans = z;
		m_ptrDoc->SetModifiedFlag();
	}
	

public:	
	void AddGLObj(CGLObject*);										// 显示对象并添加到显示链表中

	void DisplayAll();												// 显示所有对象
	void DisplaySelected();											// 显示选择的对象

	void Erase(CGLObject*);											// 从显示链表中移除对象--暂时	
	void EraseDice(CGLObject*);										// 从切块链表中移除对象--暂时
	void EraseSlice(CGLObject*);									// 从切片链表中移除对象--暂时
	void EraseAll();												// 移除所有对象--暂时
	void EraseSelected();											// 移除选择的对象--暂时
	
	bool IsErased(const CGLObject*) const;
	
	bool IsAllErasedDice() const;
	bool IsAllErasedSlice() const;
	bool IsAllErased() const;

	void DeleteAll();												// 删除所有对象--永久
	void DeleteSelected();											// 删除选择的对象--永久
	
	void Display(const GLDisplayMode& = GLWIREFRAME);				// 渲染
	void DisplaySelected(const GLSelectedMode& = GLSELECTED);		// 选择渲染

	void SetModifiedFlag(bool bFlag = true);
	
	bool Select(CGLView* aView, const int& x, const int& y);
	bool MultiSelect(CGLView* aView, const int& x, const int& y);
	bool SweepSelect(CGLView* aView, const CRect& swRect);
	bool IsEmpty() const;
	bool IsDisplayed(const CGLObject*) const;
	
	bool IsSelected(const CGLObject*) const;
	bool IsAllDisplayed() const;	
	
	bool HasSelected() const;
	void InitSelected();
	bool MoreSelected() const;
	void NextSelected();

	CGLObject* CurrentSelected() const;
	CBoundingBox GetBoundingBox() const
	{
		return m_viewBox;
	}

	void SetBoundingBox(const CBoundingBox &box)
	{
		m_viewBox = box;
	}

	void AddBox(const CBoundingBox&box)
	{
		m_viewBox.AddBox(box);
	}

	void SetSensitivity(const int& s)
	{
		m_mouseSensitivity = s;
	}
	void Serialize(CArchive& ar);
private:
	void AddToSelection(CGLObject* O);
	void RemoveSelection(CGLObject* O);
	void EmptySelection();
private:
	CColorLookUpTable m_ColorTable;
	std::vector<CGLObject*>* m_listErase;				// 移除链表

	std::vector<CGLObject*>::iterator m_selListIter;

	CDocument* m_ptrDoc;
	CBoundingBox m_viewBox;
	int m_mouseSensitivity; //for selection;
};

#endif // !defined(AFX_GLDISPLAYCONTEXT_H__D04FD53F_4852_4D6C_957A_44E8AE0C54DB__INCLUDED_)
