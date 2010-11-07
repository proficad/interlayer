//############################################################################

#if !defined(AFX_GLVIEW_H__859711F8_BE5E_4EFB_9EAB_84708FB5E343__INCLUDED_)
#define AFX_GLVIEW_H__859711F8_BE5E_4EFB_9EAB_84708FB5E343__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GLView.h : header file
//

#include "GLDisplayContext.h"
#include "GLObject.h"
#include "GLTrihedron.h"

#include "Vector3d.h"
#include "colorlookuptable.h"
#include "background.h"
#include "Vertex3D.h"
#include "GLDisplayContext.h"

#define NUM_COLORS (sizeof(m_colorv) / sizeof(m_colorv[0]))
#define FAR_CLIP 200000.0
/////////////////////////////////////////////////////////////////////////////
// CGLView view

//class CGLDisplayContext;
class CGLView
{
public:
	CGLView(CWnd* pWnd, CGLDisplayContext* aContext);
	~CGLView();

	// Attributes
public:
	HGLRC m_hRC;			// Rendering Context ���Ƴ���
	HDC m_hDC;				// Device Context �豸����
	CPalette m_GLPalette;	// Logical Palette
	HPALETTE m_hPalette, m_hPalette2;

	CGLTrihedron* m_myTrihedron;	// ������
	CGLEditAxis*	m_editAxis;				//�༭������
private:
	// Color Indexes
	struct colorIndexState
	{
		GLfloat amb[3];		/* ambient color / bottom of ramp */
		GLfloat diff[3];	/* diffuse color / middle of ramp */
		GLfloat spec[3];	/* specular color / top of ramp */
		GLfloat ratio;		/* ratio of diffuse to specular in ramp */
		GLint indexes[3];	/* where ramp was placed in palette */
	};

	colorIndexState m_colorv[3];		// ��ɫ��

	CPoint m_DownPoint;
	bool m_ZoomON;						// ����
	bool m_PanON;						// ƽ��
	bool m_RotON;						// ��ת

	//bool m_gridOn;					// ��ʾ����
	bool m_bSliceZ;						// ������Ƭ
	double m_dCurrZ;					// ��ǰZֵ

	bool m_bSliceXY;					// ������Ƭ
	double m_dCurrX1;					// ��ǰ����ֵ
	double m_dCurrY1;
	double m_dCurrX2;
	double m_dCurrY2;
	
	// Operations
public:
	void InitGL();
	//------------------------------------------------------------------------
	CGLDisplayContext* GetContext() { return m_pContext;}
	BOOL EnableRC(HDC m_hDC, HGLRC m_hRC, BOOL bEnable);
	void RenderScene(GLenum mode, bool bSlice = false);		// ��Ⱦ����
	void InitializePalette(void);

	void ZoomView(CPoint, CPoint);		// ����
	void PanView(CPoint, CPoint);		// ƽ��
	void RotateView(CPoint, CPoint);	// ��ת

	//////////////////////////////////////////////////////////////////////////
	double GetVertical() { return m_pContext->GetVertical(); }
	double GetVerticalOriginal() { return m_pContext->GetVerticalOriginal(); }
	void VerticalStretch() { m_pContext->SetVertical(m_pContext->m_dVertical + 0.1); Refresh(); }
	void VerticalShrink() { m_pContext->SetVertical(m_pContext->m_dVertical - 0.1); Refresh();}
	void VerticalOriginal() { m_pContext->SetVertical(m_pContext->GetVerticalOriginal()); Refresh();}
	//////////////////////////////////////////////////////////////////////////

	void SetSliceZ(bool bFlag){ m_bSliceZ = bFlag; }
	void SetCurrZ(double z) { m_dCurrZ = z; }
	void SetSliceXY(bool bFlag){ m_bSliceXY = bFlag; }
	void SetCurrXY(double x1, double y1, double x2, double y2)
	{ 
		m_dCurrX1 = x1; 
		m_dCurrY1 = y1; 
		m_dCurrX2 = x2; 
		m_dCurrY2 = y2; 
	}

	void GetCurrXY(double &x1, double &y1, double &x2, double &y2)
	{ 
		x1 = m_dCurrX1;
		y1 = m_dCurrY1;
		x2 = m_dCurrX2;
		y2 = m_dCurrY2;
	}

	void StopZooming()
	{
		m_ZoomON = false;
	} // ֹͣ����
	void StopPanning()
	{
		m_PanON = false;
	}	// ֹͣƽ��
	void StopRotation()
	{
		m_RotON = false;
	}	// ֹͣ��ת

	void TopView();					// ��ʾ����
	void BottomView();				// ��ʾ����
	void FrontView();				// ��ʾǰ��
	void BackView();				// ��ʾ����
	void LeftView();				// ��ʾ����
	void RightView();				// ��ʾ����
	void AxonView();				// ���ͶӰ

	void FitAll();					// ȫ���ʺ�
	void ZoomWindow(CRect zcRect);	// ��������
	void ResetView();				// �����趨

	void SetSmooth(bool bSmooth);
	bool IsSmooth()
	{
		return m_pContext->m_bSmooth;
	}
	void SetHLRemoved(bool b){m_pContext->SetHLRemoved(b);}; // ������
	bool IsHLRemoved(){return m_pContext->m_bHLRemoved;}

	void PointToScreen(const CVertex3D& P, int& x, int& y);
	double GetSliceHeight(const int& height, bool bVert = true);	// ��ȡ��Ƭ�ĺ�� height ���ص���
	void ScreenToPoint(const int& x, const int& y, CVertex3D& P);
	void FitView();
	void Fit3d(CRect zcRect);

	void ShowLegend(bool bLegend);
	bool IsShowLegend();
	void ShowAxis(bool bShow);
	bool IsShowAxis();
	void ShowGird(bool bShow);
	bool IsShowGird();

	void SetSmoothGround(bool bSmooth);
	bool GetSmoothGround();

	void SetBackgroundColor(GLfloat, GLfloat, GLfloat);
	void GetBackgroundColor(GLfloat&, GLfloat&, GLfloat&);

	void SetDisplayMode(const GLDisplayMode& aMode);
	bool IsShaded() { return m_pContext->GetDisplayMode()==GLSHADED; }

	void SetAntiAliasing(const bool& mode);

	void Select(const int& x, const int& y);		// ��ѡ
	void MultiSelect(const int& x, const int& y);	// ��ѡ
	void SweepSelect(const CRect& swRect);			// ɨ������ѡ��

	int HitSliceXY(const int& x, const int& y, const int& sensitivity = 20);
	int HitPoint(const int& x, const int& y, const int& sensitivity = 5);
	CGLObject* ProcessSelection(const int& x, const int& y, const int& sensitivity = 5);
	int ProcessSweepSelection(const CRect& rect, unsigned long int* id);
	int ProcessEditAxisSelection(const int& x, const int& y, const int& sensitivity = 5);

	virtual void Draw();			// ����
	void ReSize(int cx, int cy);	// �ı�ͼ�δ�С
	void Refresh();					// ˢ��

	BOOL QueryNewPalette();			// ��ѯ��ɫ��
	void PaletteChanged();			// �ı��ɫ��

	CWnd *GetWnd(){ return m_ptrWnd;}//����

	void ExportFile(LPCTSTR lpszFileName);
	

private:
	CWnd* m_ptrWnd;						// ���ھ��
	CGLDisplayContext* m_pContext;		// ��ʾ����	
public:
	CBackground m_Background;
};


#endif // !defined(AFX_GLVIEW_H__859711F8_BE5E_4EFB_9EAB_84708FB5E343__INCLUDED_)
