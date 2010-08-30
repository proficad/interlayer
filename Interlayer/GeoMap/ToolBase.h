// ToolBase.h: interface for the CToolBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLBASE_H__7B8D4E16_F0D9_447F_B725_6AB2B2D247B8__INCLUDED_)
#define AFX_TOOLBASE_H__7B8D4E16_F0D9_447F_B725_6AB2B2D247B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGeoMapView;

class CToolBase  
{
public:
	enum DrawShape
	{
		selection,				// ѡ����󹤾�
			
			rectempty,			// ���ľ��ι���
			roundrectempty,		// ����Բ�Ǿ��ι���
			ellipseempty,		// ������Բ����
			rect,				// ���ι���
			roundrect,			// Բ�Ǿ��ι���
			ellipse,			// ��Բ����
			solidrect,			// ���ι���
			solidroundrect,		// Բ�Ǿ��ι���
			solidellipse,		// ��Բ����

			line,				// ֱ��
			polyline,			// ����ն����
			pline,				// �⻬����ն����
			polygonempty,		// ���ķ�ն����	
			plinepolygonempty,	// ���Ĺ⻬��ն����
			polygon,			// ��ն����	
			plinepolygon,		// �⻬��ն����
			poly,				// �ǹ⻬����
			smoothfill,			// �⻬����

			movePoint,			// �ƶ��ڵ�
			delPoint,			// ɾ���ڵ�
			addPoint,			// �������
			dividePoly,			// �ָ��߶�
			joinPoly,			// �ս��߶�

			region,				// �������
			newFill,			// �����µ�����
			PaintBox,			// ����Ͱ

			clippingRgn,		// ���ü�������

			horn_lable,			// ���α�ע
			line_lable,			// ���Ա�ע
			text,				// �ı�

			pictrue,			// ͼƬ

			well,				// �����¾�
			moveWellpoleFD,		// �ƶ����� -- դ״ͼ
			linkWellpoleFD,		// �������� -- դ״ͼ
			scaleRuler,			// ������

			normal,				// ����
			interlayer,			// �в���

			pageMove,			// �ƶ���ҳ
			layerMove,			// �ƶ�������

			attriBrush			// ��ʽˢ
	};
	
	enum SelectMode				// ѡ��ģʽ
	{
		none,					// ��
			netSelect,			// ��ѡ����
			moveObj,			// �ƶ�����
			sizeObj,			// �ı����ߴ��С
			editObj, 			// �༭����
	};
public:
	CToolBase(DrawShape nDrawShape);
	
	// Overridables
	virtual void OnKeyDown(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnKeyUp(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnChar(CGeoMapView* pView, UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnContextMenu(CGeoMapView* pView, CWnd* pWnd, CPoint point);
	virtual void OnRButtonDown(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonDown(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonDblClk(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnLButtonUp(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnMouseMove(CGeoMapView* pView, UINT nFlags, const CPoint& point);
	virtual void OnEditProperties(CGeoMapView* pView);
	virtual void OnCancel(CGeoMapView* pView);

public:	
	// Attributes
	DrawShape m_drawShape;
	
	static CToolBase* FindTool(DrawShape drawShape);
	static CPtrList c_tools;

	static CPoint c_down;
	static UINT c_nDownFlags;
	static CPoint c_last;
	static DrawShape c_drawShape;
};

#endif // !defined(AFX_TOOLBASE_H__7B8D4E16_F0D9_447F_B725_6AB2B2D247B8__INCLUDED_)
