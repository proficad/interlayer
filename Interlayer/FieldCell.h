#pragma once
#include "./3Dlib/Vertex3D.h"

class C2DModelView;

// CFieldCell ����Ŀ��

class CFieldCell : public CObject
{
	DECLARE_SERIAL(CFieldCell)

	CFieldCell();           // protected constructor used by dynamic creation

	virtual ~CFieldCell();
	// Attributes
public:	
	CVertex3D m_cornerPoint[8];	// �ǵ�ʵ������
	double	 m_dValue;			// �����������
	int      m_i, m_j, m_k;		// ��ǰ��������
	int		 m_nX, m_nY, m_nZ;	// �������������

	BOOL	m_bDispTop;			// ��ʾ����

	// ������ ���¼���
	CPoint m_point[8];			// �ǵ���Ļ����
	CVertex3D m_centerPoint[2];	// ������������()�ı�����������
	COLORREF m_clrCenter;		// ���ĵ���ɫ
	COLORREF m_clrCorner[8];	// �ǵ���ɫ

	// Operations
public:
	CRect GetRect ();
	CVertex3D PointOfIntersection(); // �ı��ζԽ��߽���
	void GradientFill(CDC *pDC, C2DModelView *pView);
	void DrawFill(CDC *pDC, C2DModelView *pView);
	void DrawBorder(CDC *pDC, C2DModelView *pView);
	void DrawText(CDC *pDC, C2DModelView *pView);
	void Draw(CDC *pDC,  C2DModelView *pView);
	void DrawStateSelected(CDC *pDC, C2DModelView *pView);
	void DrawCurCell(CDC *pDC, C2DModelView *pView);

	virtual void Serialize(CArchive& ar);
	BOOL Intersects(CRect &rect);
	BOOL PtInRegion(const CPoint &point);

	CPoint2D intersect(CPoint2D p1[2], CPoint2D p2[2]);
};


