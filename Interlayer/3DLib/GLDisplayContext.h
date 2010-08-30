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

class CGLDisplayContext  // ��ʾ����
{
public:
	CGLDisplayContext();
	CGLDisplayContext(CDocument* pDoc);
	virtual ~CGLDisplayContext();
public:
	// Data
	CColorLookUpTable* GetColorTable(){return &m_ColorTable;}		// ͼ����ɫ
	//
	std::vector<CGLObject*>* m_listDisplay;							// ��ʾ����
	std::vector<CGLObject*>* m_listSelect;							// ѡ������

	std::vector<CGLObject*>* m_listDice;							// �п�����
	std::vector<CGLObject*>* m_listSlice;							// ��Ƭ����

	double m_dSliceWidth;											// ��Ƭ��� ����ʱ
	double m_dSliceHeight;											// ��Ƭ��� ����ʱ

	//------------------------------------------------------------------------
	bool IsShowDice();												// �Ƿ���ʾ�п�
	bool IsShowSlice();												// �Ƿ���ʾ��Ƭ

	//------------------------------------------------------------------------
	bool m_bHLRemoved;												// ������
	void SetHLRemoved(bool b)
	{
		m_bHLRemoved = b;
		SetModifiedFlag();
	}

	//------------------------------------------------------------------------
	bool m_bSmooth;													// ������ʾ
	void SetSmooth(bool bSmooth)
	{
		m_bSmooth = bSmooth;
		SetModifiedFlag();
	}

	//------------------------------------------------------------------------
	GLSelectedMode m_selectedMode;									// ѡ����ʾģʽ
	GLSelectedMode GetSelectedMode(){return m_selectedMode;}
	void SetSelectedMode(const GLSelectedMode& aMode)
	{
		m_selectedMode = aMode;
		SetModifiedFlag();
	}

	//------------------------------------------------------------------------
	GLDisplayMode m_displayMode;									// ��ʾģʽ
	GLDisplayMode GetDisplayMode(){return m_displayMode;}
	void SetDisplayMode(const GLDisplayMode& aMode)
	{
		m_displayMode = aMode;
		SetModifiedFlag();
	}

	//------------------------------------------------------------------------
	bool m_antialias;												// ������
	void SetAntiAliasing(const bool& mode)
	{
		m_antialias = mode;
		SetModifiedFlag();
	}
	//------------------------------------------------------------------------
	double m_dVertical;												// �����С
	double GetVertical() { return m_dVertical;}
	double GetVerticalOriginal();
	void SetVertical(double v) 
	{ 
		m_dVertical = v; 
		m_ptrDoc->SetModifiedFlag();
	}

	//------------------------------------------------------------------------
	bool	m_bSmoothGround;										// �Ƿ������ʾ
	GLfloat m_bkRed;												// ������ɫ
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
	bool m_bLegend;						// ��ʾͼ��
	bool m_bShowAxis;					// ��ʾ�����
	bool m_bShowGrid;					// ��ʾ��������
	void ShowLegend(bool bLegend) {m_bLegend = bLegend;}
	bool IsShowLegend() {return m_bLegend;}
	void ShowAxis(bool bShow) { m_bShowAxis = bShow;}
	bool IsShowAxis() {return m_bShowAxis;}
	void ShowGird(bool bShow) { m_bShowGrid = bShow;}
	bool IsShowGird() {return m_bShowGrid;}
	//------------------------------------------------------------------------
	GLdouble m_dRange;												// ��Χ	
	GLdouble GetRange() { return m_dRange;}
	void SetRange(double range) { m_dRange = range; m_ptrDoc->SetModifiedFlag();}

	//------------------------------------------------------------------------
	GLdouble m_xRot;												// ��ת�Ƕ�
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
	GLdouble m_xTrans;												// ת��
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
	void AddGLObj(CGLObject*);										// ��ʾ������ӵ���ʾ������

	void DisplayAll();												// ��ʾ���ж���
	void DisplaySelected();											// ��ʾѡ��Ķ���

	void Erase(CGLObject*);											// ����ʾ�������Ƴ�����--��ʱ	
	void EraseDice(CGLObject*);										// ���п��������Ƴ�����--��ʱ
	void EraseSlice(CGLObject*);									// ����Ƭ�������Ƴ�����--��ʱ
	void EraseAll();												// �Ƴ����ж���--��ʱ
	void EraseSelected();											// �Ƴ�ѡ��Ķ���--��ʱ
	
	bool IsErased(const CGLObject*) const;
	
	bool IsAllErasedDice() const;
	bool IsAllErasedSlice() const;
	bool IsAllErased() const;

	void DeleteAll();												// ɾ�����ж���--����
	void DeleteSelected();											// ɾ��ѡ��Ķ���--����
	
	void Display(const GLDisplayMode& = GLWIREFRAME);				// ��Ⱦ
	void DisplaySelected(const GLSelectedMode& = GLSELECTED);		// ѡ����Ⱦ

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
	std::vector<CGLObject*>* m_listErase;				// �Ƴ�����

	std::vector<CGLObject*>::iterator m_selListIter;

	CDocument* m_ptrDoc;
	CBoundingBox m_viewBox;
	int m_mouseSensitivity; //for selection;
};

#endif // !defined(AFX_GLDISPLAYCONTEXT_H__D04FD53F_4852_4D6C_957A_44E8AE0C54DB__INCLUDED_)
