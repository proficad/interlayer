// GLDisplayContext.cpp: implementation of the CGLDisplayContext class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GLDisplayContext.h"
#include "3DObject.h"
#include "GLView.h"
#include "../MainFrm.h"
#include "../3DLib/3DObjBar.h"
#include "../3DLib/C3DDice.h"
#include "../3DLib/C3DSlice.h"
#include "../3DLib/GridObject.h"
#include "PipeLine.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

extern void StatusBarMessage(char* fmt, ...);

extern void StatusProgress(int nIndex, int nRange=-1);
extern void StatusSetProgress(int nIndex, int nPos);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGLDisplayContext::CGLDisplayContext()
{
	m_bSmooth = true;
	m_displayMode = GLSHADED;
	m_selectedMode = GLNOSELECTED;
	m_antialias = true;
	m_bHLRemoved = true;

	m_bkRed = 0.2f;
	m_bkGreen = 0.2f;
	m_bkBlue = 0.6f;

	m_dVertical = 0.6;

	m_dRange = 2;

	m_xRot = -75.0f;
	m_zRot = -45.0f;
	m_yRot = 0;
	m_xTrans = m_yTrans = m_zTrans = 0.0f;

	m_bSmoothGround = true;

	m_bLegend = false;					// 显示图例
	m_bShowAxis = true;					// 显示坐标框
	m_bShowGrid = true;					// 显示坐标网格

	m_dSliceWidth = 0.004;				// 切片厚度 纵切时
	m_dSliceHeight = 0.004;				// 切片厚度 横切时

	m_listDisplay = new std::vector<CGLObject*>;

	m_listDice = new std::vector<CGLObject*>;
	m_listSlice = new std::vector<CGLObject*>;

	m_listErase = new std::vector<CGLObject*>;
	m_listSelect = new std::vector<CGLObject*>;
	m_selListIter = m_listSelect->begin();

	m_viewBox.SetLimits(DBL_MAX, -DBL_MAX, DBL_MAX, -DBL_MAX, DBL_MAX, -DBL_MAX);

	TCHAR szCurPath[MAX_PATH];  
	memset(szCurPath,   0,   MAX_PATH);  
	GetModuleFileName(NULL,   szCurPath,   sizeof(szCurPath)/sizeof(TCHAR));   
	CString strMoudlePath = szCurPath;
	strMoudlePath = strMoudlePath.Left(strMoudlePath.ReverseFind('\\')+1);
	CString strFileName = strMoudlePath;
	strFileName += _T("Config\\Rainbow3.CLR");
	m_ColorTable.LoadColorTable(strFileName);
}

CGLDisplayContext::CGLDisplayContext(CDocument* pDoc)
{
	m_bSmooth = true;
	m_displayMode = GLSHADED;
	m_antialias = true;
	m_bHLRemoved = true;

	m_bkRed = 0.2f;
	m_bkGreen = 0.2f;
	m_bkBlue = 0.6f;

	m_dVertical = 0.6;

	m_dRange = 2.0;
	m_xRot = -75.0f;
	m_zRot = -45.0f;
	m_yRot = 0;
	m_xTrans = m_yTrans = m_zTrans = 0.0f;

	m_bSmoothGround = true;

	m_bLegend = false;					// 显示图例
	m_bShowAxis = true;					// 显示坐标框
	m_bShowGrid = true;					// 显示坐标网格

	m_dSliceWidth = 0.004;				// 切片厚度 纵切时
	m_dSliceHeight = 0.004;				// 切片厚度 横切时

	m_listDisplay = new std::vector<CGLObject*>;
	m_listErase = new std::vector<CGLObject*>;
	m_listSelect = new std::vector<CGLObject*>;
	
	m_listDice = new std::vector<CGLObject*>;
	m_listSlice = new std::vector<CGLObject*>;

	m_selListIter = m_listSelect->begin();
	m_ptrDoc = pDoc;
	m_viewBox.SetLimits(DBL_MAX, -DBL_MAX, DBL_MAX, -DBL_MAX, DBL_MAX, -DBL_MAX);

	TCHAR szCurPath[MAX_PATH];  
	memset(szCurPath,   0,   MAX_PATH);  
	GetModuleFileName(NULL,   szCurPath,   sizeof(szCurPath)/sizeof(TCHAR));   
	CString strMoudlePath = szCurPath;
	strMoudlePath = strMoudlePath.Left(strMoudlePath.ReverseFind('\\')+1);
	CString strFileName = strMoudlePath;
	strFileName += _T("Config\\Rainbow3.CLR");
	m_ColorTable.LoadColorTable(strFileName);

	m_mouseSensitivity = 5; //5 pixels sensitivity

	/*C3DDice *pSli = new C3DDice;
	pSli->SetContext(this);
	CVertex3D v;
	
	v.x = 0.2;
	v.y = 0.2;
	v.z = -1.0;
	pSli->m_pointList->push_back(v);

	v.x = 0.2;
	v.y = -0.2;
	v.z = -1.0;
	pSli->m_pointList->push_back(v);

	v.x = 0.2;
	v.y = -0.2;
	v.z = 1.0;
	pSli->m_pointList->push_back(v);

	v.x = 0.2;
	v.y = 0.2;
	v.z = 1.0;
	pSli->m_pointList->push_back(v);

//------------------------------------------------------------------------
	v.x = 0.2;
	v.y = 0.2;
	v.z = 1.0;
	pSli->m_pointList->push_back(v);

	v.x = -0.2;
	v.y = 0.2;
	v.z = 1.0;
	pSli->m_pointList->push_back(v);

	v.x = -0.2;
	v.y = 0.2;
	v.z = -1.0;
	pSli->m_pointList->push_back(v);

	v.x = 0.2;
	v.y = 0.2;
	v.z = -1.0;
	pSli->m_pointList->push_back(v);
	
//------------------------------------------------------------------------

	/*v.x = 0.2;
	v.y = -0.2;
	v.z = 1.0;
	pSli->m_pointList->push_back(v);

	v.x = 0.2;
	v.y = -0.2;
	v.z = -1.0;
	pSli->m_pointList->push_back(v);

	v.x = -0.2;
	v.y = 0.2;
	v.z = -1.0;
	pSli->m_pointList->push_back(v);

	v.x = -0.2;
	v.y = 0.2;
	v.z = 1.0;
	pSli->m_pointList->push_back(v);

	m_listDice->push_back(pSli);
*/
}

CGLDisplayContext::~CGLDisplayContext()
{
	std::vector<CGLObject*>::iterator listIter;
	CGLObject *pObj = NULL;

	if (!m_listDisplay->empty())
	{
		for (listIter = m_listDisplay->begin(); listIter != m_listDisplay->end(); listIter++)
		{
			pObj = (*listIter);
			delete pObj;
		}

		m_listDisplay->clear();
	}

	if (!m_listDice->empty())
	{
		for (listIter = m_listDice->begin(); listIter != m_listDice->end(); listIter++)
		{
			pObj = (*listIter);
			delete pObj;
		}

		m_listDice->clear();
	}

	if (!m_listSlice->empty())
	{
		for (listIter = m_listSlice->begin(); listIter != m_listSlice->end(); listIter++)
		{
			pObj = (*listIter);
			delete pObj;
		}

		m_listSlice->clear();
	}


	if (!m_listErase->empty())
	{
		for (listIter = m_listErase->begin(); listIter != m_listErase->end(); listIter++)
		{
			pObj = (*listIter);
			delete pObj;
		}
		m_listErase->clear();
	}

	if (!m_listSelect->empty())
		m_listSelect->clear();

	delete m_listDisplay;
	delete m_listDice;
	delete m_listSlice;

	delete m_listErase;
	delete m_listSelect;
}

double CGLDisplayContext::GetVerticalOriginal()
{
	double xRange = m_viewBox.XMax()-m_viewBox.XMin();
	double yRange = m_viewBox.YMax()-m_viewBox.YMin();
	double zRange = m_viewBox.ZMax()-m_viewBox.ZMin();

	double range = (xRange>yRange?xRange:yRange);

	if(range<0.0000000001 || zRange < 0.000000001 )
		return 0.6;
	else
		return zRange / range;
}

void CGLDisplayContext::SetModifiedFlag(bool bFlag)
{
	m_ptrDoc->SetModifiedFlag(bFlag);
	CGLObject* pObj = NULL;
	std::vector<CGLObject*>::iterator listIter = m_listDisplay->begin();
	for (; listIter != m_listDisplay->end(); listIter++)
	{
		pObj = (*listIter);
		pObj->SetModified(bFlag);
	}

	listIter = m_listDice->begin();
	for (; listIter != m_listDice->end(); listIter++)
	{
		pObj = (*listIter);
		pObj->SetModified(bFlag);
	}

	listIter = m_listSlice->begin();
	for (; listIter != m_listSlice->end(); listIter++)
	{
		pObj = (*listIter);
		pObj->SetModified(bFlag);
	}
}

void CGLDisplayContext::AddGLObj(CGLObject* aObj)
{
	if (aObj == NULL)
		return;

	CGLObject* pObj = aObj;

	CMainFrame *pMF = (CMainFrame *)AfxGetMainWnd();

	C3DObjBar*pBar = pMF->Get3DBar();
	CLayerIntersectSearch *pSearchbar = pMF->GetSearchBar();
	//CGLObject* pObj = aObj->Copy();

	switch(pObj->GetGLObjType())
	{
	case GLDICE:
		{
			m_listDice->push_back(pObj);				// 添加到切块链表
		}
		break;
	case GLSLICE:
		{
			m_listSlice->push_back(pObj);				// 添加到切片链表
		}
		break;
	case GLSURFACE:
		{
			bool bOri = (fabs(m_dVertical - GetVerticalOriginal())<0.0000001);

			if (m_listDisplay->empty())
				m_viewBox = pObj->GetBoundingBox();
			else
				m_viewBox.AddBox(pObj->GetBoundingBox());

			if( m_dVertical == 0.6 || bOri )
				m_dVertical = GetVerticalOriginal();

			m_listDisplay->push_back(pObj);				// 添加到显示链表
			pSearchbar->m_wndTree.SetModel(pObj);
		}
		break;
	case GLINTERLAYERCELL:
		{
			bool bOri = (fabs(m_dVertical - GetVerticalOriginal())<0.0000001);

			if (m_listDisplay->empty())
				m_viewBox = pObj->GetBoundingBox();
			else
				m_viewBox.AddBox(pObj->GetBoundingBox());

			if( m_dVertical == 0.6 || bOri )
				m_dVertical = GetVerticalOriginal();

			m_listDisplay->push_back(pObj);				// 添加到显示链表
			//pSearchbar->m_wndTree.SetModel(pObj);
		}
		break;
	case GLPLANE:
		{
			bool bOri = (fabs(m_dVertical - GetVerticalOriginal())<0.0000001);

			if (m_listDisplay->empty())
				m_viewBox = pObj->GetBoundingBox();
			else
				m_viewBox.AddBox(pObj->GetBoundingBox());

			if( m_dVertical == 0.6 || bOri )
				m_dVertical = GetVerticalOriginal();

			int nSize = m_listDisplay->size();

			int j = nSize%21+1;

			GLMaterial mat = (GLMaterial)j;

			pObj->SetMaterial(mat);

			m_listDisplay->push_back(pObj);				// 添加到显示链表
			pSearchbar->m_wndTree.AddLayer(pObj);
		}
		break;
	default:
		{
			bool bOri = (fabs(m_dVertical - GetVerticalOriginal())<0.0000001);

			if (m_listDisplay->empty())
				m_viewBox = pObj->GetBoundingBox();
			else
				m_viewBox.AddBox(pObj->GetBoundingBox());

			if( m_dVertical == 0.6 || bOri )
				m_dVertical = GetVerticalOriginal();

			int nSize = m_listDisplay->size();

			int j = nSize%21+1;

			GLMaterial mat = (GLMaterial)j;

			pObj->SetMaterial(mat);

			m_listDisplay->push_back(pObj);				// 添加到显示链表
		}
		break;

	}	
	
	pBar->m_wndTree.AddObj(pObj);

	SetModifiedFlag(true);
}

void CGLDisplayContext::Erase(CGLObject* aO) //
{
	m_listErase->push_back(aO);

	std::vector<CGLObject*>::iterator listIter;
	CGLObject *pObj = NULL;
	for (listIter = m_listDisplay->begin(); listIter != m_listDisplay->end(); listIter++)
	{
		pObj = (*listIter);
		if( pObj == aO)
		{
			m_listDisplay->erase(listIter);
			break;
		}
	}

	if( m_listDisplay->empty() )
		m_viewBox.SetLimits(DBL_MAX, -DBL_MAX, DBL_MAX, -DBL_MAX, DBL_MAX, -DBL_MAX);
}

void CGLDisplayContext::EraseDice(CGLObject* aO) //
{
	m_listErase->push_back(aO);

	std::vector<CGLObject*>::iterator listIter;
	CGLObject *pObj = NULL;
	for (listIter = m_listDice->begin(); listIter != m_listDice->end(); listIter++)
	{
		pObj = (*listIter);
		if( pObj == aO)
		{
			m_listDice->erase(listIter);
			break;
		}
	}
}

void CGLDisplayContext::EraseSlice(CGLObject* aO) //
{
	m_listErase->push_back(aO);

	std::vector<CGLObject*>::iterator listIter;
	CGLObject *pObj = NULL;
	for (listIter = m_listSlice->begin(); listIter != m_listSlice->end(); listIter++)
	{
		pObj = (*listIter);
		if( pObj == aO)
		{
			m_listSlice->erase(listIter);
			break;
		}
	}
}

void CGLDisplayContext::DisplayAll()
{	
	std::vector<CGLObject*>::iterator listIter;

	for (listIter = m_listErase->begin(); listIter != m_listErase->end(); listIter++)
	{
		CGLObject* eObj = (*listIter);

		if( eObj->GetGLObjType() == GLDICE)
		{
			m_listDice->push_back(eObj);
			m_listDice->erase(listIter);
		}
	}

	for (listIter = m_listErase->begin(); listIter != m_listErase->end(); listIter++)
	{
		CGLObject* eObj = (*listIter);
		if( eObj->GetGLObjType() == GLSLICE)
		{
			m_listSlice->push_back(eObj);
			m_listSlice->erase(listIter);
		}
	}

	for (listIter = m_listErase->begin(); listIter != m_listErase->end(); listIter++)
	{
		CGLObject* eObj = (*listIter);
		if (m_listDisplay->empty())
			m_viewBox = eObj->GetBoundingBox();
		else
			m_viewBox.AddBox(eObj->GetBoundingBox());
		m_listDisplay->push_back(eObj);
	}
	m_listErase->clear();

	SetModifiedFlag(true);

	CMainFrame *pMF = (CMainFrame *)AfxGetMainWnd();

	C3DObjBar*pBar = pMF->Get3DBar();
	pBar->m_wndTree.FillTreeCtrl();

	m_ptrDoc->UpdateAllViews(NULL);
}

void CGLDisplayContext::DisplaySelected()
{
	if (!HasSelected())
		return;

	EraseAll();

	std::vector<CGLObject*>::iterator listIterD = m_listErase->begin();
	std::vector<CGLObject*>::iterator listIterSel = m_listSelect->begin();
	CGLObject* eObj, * sObj;
	for (; listIterSel != m_listSelect->end(); listIterSel++)
	{
		sObj = (*listIterSel);
		for (listIterD = m_listErase->begin(); listIterD != m_listErase->end(); listIterD++)
		{
			eObj = (*listIterD);
			if (eObj->GetObjID() == sObj->GetObjID())
				break;
		}
		if (m_listDisplay->empty())
			m_viewBox = eObj->GetBoundingBox();
		else
			m_viewBox.AddBox(eObj->GetBoundingBox());

		m_listDisplay->push_back(eObj);

		m_listErase->erase(listIterD);
	}
	EmptySelection();
}

void CGLDisplayContext::EraseAll()
{
	std::vector<CGLObject*>::iterator listIter;

	for (listIter = m_listDisplay->begin(); listIter != m_listDisplay->end(); listIter++)
	{
		m_listErase->push_back((*listIter));
	}
	m_listDisplay->clear();

	for (listIter = m_listDice->begin(); listIter != m_listDice->end(); listIter++)
	{
		m_listErase->push_back((*listIter));
	}
	m_listDice->clear();

	for (listIter = m_listSlice->begin(); listIter != m_listSlice->end(); listIter++)
	{
		m_listErase->push_back((*listIter));
	}
	m_listSlice->clear();	
}

void CGLDisplayContext::EraseSelected()
{
	if (!HasSelected())
		return;

	std::vector<CGLObject*>::iterator listIterD;
	std::vector<CGLObject*>::iterator listIterSel = m_listSelect->begin();
	CGLObject* eObj, * sObj;
	for (; listIterSel != m_listSelect->end(); listIterSel++)
	{
		sObj = (*listIterSel);

		if( sObj->GetGLObjType() == GLDICE )
		{
			for (listIterD = m_listDice->begin(); listIterD != m_listDice->end(); listIterD++)
			{
				eObj = (*listIterD);
				if (eObj->GetObjID() == sObj->GetObjID())
					break;
			}
			EraseDice(eObj);
		}
		else if( sObj->GetGLObjType() == GLSLICE )
		{
			for (listIterD = m_listSlice->begin(); listIterD != m_listSlice->end(); listIterD++)
			{
				eObj = (*listIterD);
				if (eObj->GetObjID() == sObj->GetObjID())
					break;
			}
			EraseSlice(eObj);
		}
		else
		{
			for (listIterD = m_listDisplay->begin(); listIterD != m_listDisplay->end(); listIterD++)
			{
				eObj = (*listIterD);
				if (eObj->GetObjID() == sObj->GetObjID())
					break;
			}
			Erase(eObj);
		}
		
	}
	EmptySelection();
	m_viewBox.SetLimits(DBL_MAX, -DBL_MAX, DBL_MAX, -DBL_MAX, DBL_MAX, -DBL_MAX);
	for (listIterD = m_listDisplay->begin(); listIterD != m_listDisplay->end(); listIterD++)
		m_viewBox.AddBox((*listIterD)->GetBoundingBox());
}

void CGLDisplayContext::DeleteAll()
{
	std::vector<CGLObject*>::iterator listIter;
	CGLObject *pObj = NULL;

	if (!m_listDisplay->empty())
	{
		for (listIter = m_listDisplay->begin(); listIter != m_listDisplay->end(); listIter++)
		{
			pObj = (*listIter);
			delete pObj;
		}

		m_listDisplay->clear();
	}

	if (!m_listDice->empty())
	{
		for (listIter = m_listDice->begin(); listIter != m_listDice->end(); listIter++)
		{
			pObj = (*listIter);
			delete pObj;
		}

		m_listDice->clear();
	}

	if (!m_listSlice->empty())
	{
		for (listIter = m_listSlice->begin(); listIter != m_listSlice->end(); listIter++)
		{
			pObj = (*listIter);
			delete pObj;
		}

		m_listSlice->clear();
	}

	if (!m_listErase->empty())
	{
		for (listIter = m_listErase->begin(); listIter != m_listErase->end(); listIter++)
		{
			pObj = (*listIter);
			delete pObj;
		}
		m_listErase->clear();
	}

	EmptySelection();

	m_viewBox.SetLimits(DBL_MAX, -DBL_MAX, DBL_MAX, -DBL_MAX, DBL_MAX, -DBL_MAX);
}

void CGLDisplayContext::DeleteSelected()
{
	if (!HasSelected())
		return;

	std::vector<CGLObject*>::iterator listIterD;
	std::vector<CGLObject*>::iterator listIterSel;
	
	CGLObject* eObj, * sObj;
	
	for (listIterSel = m_listSelect->begin(); listIterSel != m_listSelect->end(); listIterSel++)
	{
		sObj = (*listIterSel);
		for (listIterD = m_listDisplay->begin(); listIterD != m_listDisplay->end(); listIterD++)
		{
			eObj = (*listIterD);
			if (eObj->GetObjID() == sObj->GetObjID())
				break;
		}

		m_listDisplay->erase(listIterD);

		delete eObj;
	}

	for (listIterSel = m_listSelect->begin(); listIterSel != m_listSelect->end(); listIterSel++)
	{
		sObj = (*listIterSel);
		for (listIterD = m_listDice->begin(); listIterD != m_listDice->end(); listIterD++)
		{
			eObj = (*listIterD);
			if (eObj->GetObjID() == sObj->GetObjID())
				break;
		}

		m_listDice->erase(listIterD);

		delete eObj;
	}

	for (listIterSel = m_listSelect->begin(); listIterSel != m_listSelect->end(); listIterSel++)
	{
		sObj = (*listIterSel);
		for (listIterD = m_listSlice->begin(); listIterD != m_listSlice->end(); listIterD++)
		{
			eObj = (*listIterD);
			if (eObj->GetObjID() == sObj->GetObjID())
				break;
		}

		m_listSlice->erase(listIterD);

		delete eObj;
	}

	EmptySelection();

	m_viewBox.SetLimits(DBL_MAX, -DBL_MAX, DBL_MAX, -DBL_MAX, DBL_MAX, -DBL_MAX);

	for (listIterD = m_listDisplay->begin(); listIterD != m_listDisplay->end(); listIterD++)
		m_viewBox.AddBox((*listIterD)->GetBoundingBox());
}

void CGLDisplayContext::DisplaySelected(const GLSelectedMode& mode)		// 选择渲染
{
	if (IsEmpty())
		return;

	CGLObject* pObj = NULL;
	
	std::vector<CGLObject*>::iterator listIter = m_listSelect->begin();			// 显示选择链表中的图形

	for (; listIter != m_listSelect->end(); listIter++)
	{
		pObj = (*listIter);
		if (pObj && pObj->isShow())
			pObj->DisplaySelected(mode);
	}
}

void CGLDisplayContext::Display(const GLDisplayMode& mode)
{
	if (IsEmpty())
		return;

	CGLObject* pObj = NULL;

	// Initialize the names stack
	glInitNames();
	glPushName(-1);

	bool bSlice = IsShowSlice(); // 是否显示切片
	bool bDice = IsShowDice();	//  是否显示切块
	std::vector<CGLObject*>::iterator listIter = m_listDisplay->begin();
	for (; listIter != m_listDisplay->end(); listIter++)
	{
		pObj = (*listIter);
		if (pObj)
		{
			if(m_selectedMode != GLEDIT && pObj->GetGLObjType() == GLPLANE )
				glLoadName(reinterpret_cast<GLuint>(pObj));
			pObj->Display(mode);
		}
	}

	//------------------------------------------------------------------------
	if( bSlice )
	{
		std::vector<CGLObject*>::iterator listSliceIter;
		
		double xRange = m_viewBox.XMax()-m_viewBox.XMin();
		double yRange = m_viewBox.YMax()-m_viewBox.YMin();
		double zRange = m_viewBox.ZMax()-m_viewBox.ZMin();

		double range = (xRange>yRange?xRange:yRange);

		C3DSlice* pSlice = NULL;
		for (listSliceIter = m_listSlice->begin(); listSliceIter != m_listSlice->end(); listSliceIter++)
		{
			pSlice = (C3DSlice*)(*listSliceIter);
			if( pSlice->isShow() )
			{
				GLdouble eqn1[4], eqn2[4];
				eqn1[0] = pSlice->A;
				eqn1[1] = pSlice->B;
				eqn1[2] = pSlice->C;
				eqn1[3] = pSlice->D;

				eqn2[0] = -pSlice->A;
				eqn2[1] = -pSlice->B;
				eqn2[2] = -pSlice->C;
				eqn2[3] = -pSlice->D;

				if( pSlice->IsVertSlice() )
				{
					if( pSlice->m_bSpecified )
					{
						eqn1[3] += pSlice->m_dHeight/range*2;
						eqn2[3] += pSlice->m_dHeight/range*2;
					}
					else
					{
						eqn1[3] += m_dSliceWidth;
						eqn2[3] += m_dSliceWidth;
					}
				}
				else
				{
					if( pSlice->m_bSpecified )
					{
						eqn1[3] += pSlice->m_dHeight/zRange;
						eqn2[3] += pSlice->m_dHeight/zRange;
					}
					else
					{
						eqn1[3] += m_dSliceHeight/2;
						eqn2[3] += m_dSliceHeight/2;
					}
				}			

				glClipPlane(GL_CLIP_PLANE0,eqn1);	//平面裁剪,这四个系数分别是裁剪平面Ax+By+Cz+D=0的A、B、C、D值
				glClipPlane(GL_CLIP_PLANE1,eqn2);

				//________________________________________________________________________
				//
				listIter = m_listDisplay->begin();
				for (; listIter != m_listDisplay->end(); listIter++)
				{
					pObj = (*listIter);
					if (pObj->GetGLObjType() != GLSURFACE )
					{
						glEnable(GL_CLIP_PLANE0);			//使得当前所定义的裁剪平面有效				
						glEnable(GL_CLIP_PLANE1);			//参数plane是GL_CLIP_PLANEi(i=0,1,...)，指定裁剪面号

						if(m_selectedMode != GLEDIT)
							glLoadName(reinterpret_cast<GLuint>(pObj));
						pObj->Display(mode, true);

						glDisable(GL_CLIP_PLANE0);
						glDisable(GL_CLIP_PLANE1);						
					}
					else
					{		

						/*glEnable(GL_CLIP_PLANE0);
						glEnable(GL_CLIP_PLANE1);			//参数plane是GL_CLIP_PLANEi(i=0,1,...)，指定裁剪面号

						glClear(GL_STENCIL_BUFFER_BIT);
						// multipass rendering
						glDisable(GL_DEPTH_TEST);
						glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
						glEnable(GL_STENCIL_TEST);

						// first pass: back face increment
						glStencilFunc(GL_ALWAYS, 0, 0);
						glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
						glCullFace(GL_FRONT);
						
						if(m_selectedMode != GLEDIT)
						glLoadName(reinterpret_cast<GLuint>(pObj));
						pObj->Display(mode, true);

						// second pass: front face decrement
						glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
						glCullFace(GL_BACK);
						
						if(m_selectedMode != GLEDIT)
						glLoadName(reinterpret_cast<GLuint>(pObj));
						pObj->Display(mode, true);

						// drawing clip planes masked by stencil buffer content
						glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
						glEnable(GL_DEPTH_TEST);
						glDisable(GL_CLIP_PLANE0);
						glDisable(GL_CLIP_PLANE1);

						CMaterial*pMat = pObj->GetMaterial();
						pMat->ApplyMaterial();

						glStencilFunc(GL_NOTEQUAL, 0, ~0);

						// rendering clip edges to frame buffer
						{
							glBegin(GL_QUADS);
							CVertex3D verts[4];
							CVector3D nrml;
							pSlice->GetCoordinate(eqn1, verts);
							nrml = CVector3D(verts[2], verts[1]).CrossProduct(CVector3D(verts[1], verts[0])); 
							nrml.Normalize();
							glNormal3f(nrml.GetX(), nrml.GetY(), nrml.GetZ()); //We want plane to be lit properly, thus we need to specify it's normal.
							for(int j=3; j>=0; j--)
								glVertex3f(verts[j].x, verts[j].y, verts[j].z);
							glEnd();
						}

						{
							glBegin(GL_QUADS);
							CVertex3D verts[4];
							CVector3D nrml;
							pSlice->GetCoordinate(eqn2, verts);
							nrml = CVector3D(verts[2], verts[1]).CrossProduct(CVector3D(verts[1], verts[0])); 
							nrml.Normalize();
							glNormal3f(nrml.GetX(), nrml.GetY(), nrml.GetZ()); //We want plane to be lit properly, thus we need to specify it's normal.
							for(int j=3; j>=0; j--)
								glVertex3f(verts[j].x, verts[j].y, verts[j].z);
							glEnd();
						}
						

						// rendering geometry to frame buffer			
						glDisable(GL_STENCIL_TEST);

						
*/

						glEnable(GL_CLIP_PLANE0);
						glEnable(GL_CLIP_PLANE1);
						
						if(m_selectedMode != GLEDIT)
							glLoadName(reinterpret_cast<GLuint>(pObj));
						pObj->Display(mode, true);

						glDisable(GL_CLIP_PLANE0);
						glDisable(GL_CLIP_PLANE1);
					}
				}
				//________________________________________________________________________
				//	

				if( pSlice->m_bShowSlicePlane )
				{
					CMaterial mat;
					mat.SetMaterial(None);
					mat.ApplyMaterial();
					if( pSlice->IsVertSlice() )
					{
						if( pSlice->m_bSpecified )
							eqn1[3] -= pSlice->m_dHeight/range*2;
						else
							eqn1[3] -= m_dSliceWidth;
					}
					else
					{
						if( pSlice->m_bSpecified )
							eqn1[3] -= pSlice->m_dHeight/zRange;
						else
							eqn1[3] -= m_dSliceHeight/2;
					}

					glEnable(GL_BLEND);
					glColor4d(0.1,0.1,0.1,0.1);

					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);    

					glBegin(GL_QUADS);
					CVertex3D verts[4];
					CVector3D nrml;
					pSlice->GetCoordinate(eqn1, verts);
					nrml = CVector3D(verts[2], verts[1]).CrossProduct(CVector3D(verts[1], verts[0])); 
					nrml.Normalize();
					glNormal3f(nrml.GetX(), nrml.GetY(), nrml.GetZ()); //We want plane to be lit properly, thus we need to specify it's normal.
					for(int j=3; j>=0; j--)
						glVertex3f(verts[j].x, verts[j].y, verts[j].z);
					glEnd();
					glDisable(GL_BLEND);
				}
			}
		}
	}
}

bool CGLDisplayContext::IsShowDice()
{
	std::vector<CGLObject*>::iterator listIter;

	CGLObject* pObj;
	for (listIter = m_listDice->begin(); listIter != m_listDice->end(); listIter++)
	{
		pObj = (*listIter);
		if (pObj->isShow() )
			return true;
	}

	return false;
}

bool CGLDisplayContext::IsShowSlice()
{
	std::vector<CGLObject*>::iterator listIter;

	CGLObject* pObj;
	for (listIter = m_listSlice->begin(); listIter != m_listSlice->end(); listIter++)
	{
		pObj = (*listIter);
		if (pObj->isShow() )
			return true;
	}

	return false;
}


bool CGLDisplayContext::Select(CGLView* aView, const int& x, const int& y)
{
	if( m_listDisplay->empty() )
		return false;

	bool res = false;
	CGLObject* pObj = NULL;
	pObj = aView->ProcessSelection(x, y, m_mouseSensitivity);

	if (pObj!=NULL)
		res = true;
	else
	{
		EmptySelection();
		return false;
	}

	if (!IsSelected(pObj))
	{
		m_listSelect->clear();
		AddToSelection(pObj);
	}
	else
		RemoveSelection(pObj);

	return res;
}

bool CGLDisplayContext::MultiSelect(CGLView* aView, const int& x, const int& y)
{
	if( m_listDisplay->empty() )
		return false;

	bool res = false;
	CGLObject* pObj = NULL;
	pObj = aView->ProcessSelection(x, y, m_mouseSensitivity);
	if (pObj!=NULL)
		res = true;
	
	if (!IsSelected(pObj))
	{
		m_listSelect->clear();
		AddToSelection(pObj);
	}
	else
		RemoveSelection(pObj);

	return res;
}

bool CGLDisplayContext::SweepSelect(CGLView* aView, const CRect& swRect)
{
	if( m_listDisplay->empty() )
		return false;

	bool res = false;
	CGLObject* pObj = 0;
	unsigned long int* id = new unsigned long int[100];
	int no_of_objs = aView->ProcessSweepSelection(swRect, id);
	if (no_of_objs)
		res = true;
	std::vector<CGLObject*>::iterator listIter = m_listDisplay->begin();
	for (; listIter != m_listDisplay->end(); listIter++)
	{
		pObj = (*listIter);
		if (pObj)
		{
			for (int i = 0; i < no_of_objs; i++)
			{
				if (id[i] == pObj->GetObjID())
					AddToSelection(pObj);
			}
		}
	}
	delete[] id;
	return res;
}

bool CGLDisplayContext::IsEmpty() const
{
	return (m_listDisplay->empty() && m_listErase->empty());
}

bool CGLDisplayContext::IsDisplayed(const CGLObject* pObj) const
{
	if (pObj == NULL)
		return false;
	std::vector<CGLObject*>::iterator li = m_listDisplay->begin();

	CGLObject* pO = 0;
	for (; li != m_listDisplay->end(); li++)
	{
		pO = (*li);
		if( pO == pObj )
			return true;
	}

	return false;
}

bool CGLDisplayContext::IsErased(const CGLObject* pObj) const
{
	if (pObj == NULL)
		return false;

	std::vector<CGLObject*>::iterator li = m_listErase->begin();

	CGLObject* pO = 0;
	for (; li != m_listErase->end(); li++)
	{
		pO = (*li);
		if( pO == pObj )
			return true;
	}

	return false;
}

bool CGLDisplayContext::IsSelected(const CGLObject* pObj) const
{
	bool b = false;
	if (pObj == NULL)
		return false;
	std::vector<CGLObject*>::iterator li = m_listSelect->begin();
	for (; li != m_listSelect->end(); li++)
	{
		if (pObj->GetObjID() == (*li)->GetObjID())
		{
			b = true;
			break;
		}
	}
	return b;
}

bool CGLDisplayContext::IsAllDisplayed() const
{
	return m_listErase->empty();
}

bool CGLDisplayContext::IsAllErased() const
{
	return m_listDisplay->empty();
}

bool CGLDisplayContext::IsAllErasedDice() const
{
	return m_listDice->empty();
}

bool CGLDisplayContext::IsAllErasedSlice() const
{
	return m_listSlice->empty();
}


bool CGLDisplayContext::HasSelected() const
{
	return !m_listSelect->empty();
}

void CGLDisplayContext::InitSelected()
{
	m_selListIter = m_listSelect->begin();
}

bool CGLDisplayContext::MoreSelected() const
{
	return m_selListIter != m_listSelect->end();
}

void CGLDisplayContext::NextSelected()
{
	m_selListIter++;
}

CGLObject* CGLDisplayContext::CurrentSelected() const
{
	CGLObject* sObj = (*m_selListIter);
	CGLObject* pObj = 0;
	std::vector<CGLObject*>::iterator listIter = m_listDisplay->begin();
	for (; listIter != m_listDisplay->end(); listIter++)
	{
		if (sObj->GetObjID() == (*listIter)->GetObjID())
		{
			pObj = (*listIter);
			break;
		}
	}

	return pObj;
}

//Private
void CGLDisplayContext::AddToSelection(CGLObject* pObj)
{
	m_listSelect->push_back(pObj);
}

void CGLDisplayContext::RemoveSelection(CGLObject* pObj)
{
	std::vector<CGLObject*>::iterator listIter = m_listSelect->begin();

	for (; listIter != m_listSelect->end(); listIter++)
	{
		if (pObj->GetObjID() == (*listIter)->GetObjID())
		{
			m_listSelect->erase(listIter);
			break;
		}
	}
}

void CGLDisplayContext::EmptySelection()
{
	if (!m_listSelect->empty())
		m_listSelect->clear();
}

void CGLDisplayContext::Serialize(CArchive& ar)
{
	m_viewBox.Serialize(ar);

	if (ar.IsStoring())
	{
		StatusBarMessage(_T("正在写入文件数据，请稍等..."));

		ar << m_ColorTable;
		ar << m_bSmooth;
		ar << m_bHLRemoved;
		ar << (WORD)m_displayMode;
		ar << m_antialias;

		ar << m_bkRed;
		ar << m_bkGreen;
		ar << m_bkBlue;

		ar << m_dVertical;

		ar << m_dRange;

		ar << m_xRot;
		ar << m_zRot;
		ar << m_yRot;
		ar << m_xTrans;
		ar << m_yTrans;
		ar << m_zTrans;

		ar << m_bSmoothGround;
		ar << m_bLegend;					// 显示图例
		ar << m_bShowAxis;					// 显示坐标框
		ar << m_bShowGrid;					// 显示坐标网格
		
		int size = m_listDisplay->size();
		ar << size;

		StatusProgress(1,size);
		CGLObject* pObj = 0;
		int i = 0;
		std::vector<CGLObject*>::iterator listIter = m_listDisplay->begin();
		for (; listIter != m_listDisplay->end(); listIter++)
		{
			pObj = (*listIter);
			GLObjectType type = pObj->GetGLObjType();
			DWORD dw = type;
			ar << dw;			
			pObj->Serialize(ar);

			i++;
			StatusSetProgress(1,i);
		}


		size = m_listDice->size();
		ar << size;

		for (listIter = m_listDice->begin(); listIter != m_listDice->end(); listIter++)
		{
			pObj = (*listIter);	
			pObj->Serialize(ar);
		}

		size = m_listSlice->size();
		ar << size;

		for (listIter = m_listSlice->begin(); listIter != m_listSlice->end(); listIter++)
		{
			pObj = (*listIter);	
			pObj->Serialize(ar);
		}


		size = m_listErase->size();
		ar << size;

		for (listIter = m_listErase->begin(); listIter != m_listErase->end(); listIter++)
		{
			pObj = (*listIter);
			GLObjectType type = pObj->GetGLObjType();
			DWORD dw = type;
			ar << dw;			
			pObj->Serialize(ar);
		}

		StatusProgress(1);

		StatusBarMessage(_T("写入文件完成."));
	}
	else
	{
		StatusBarMessage(_T("正在读取文件数据，请稍等..."));

		ar >> m_ColorTable;

		WORD wd;
		ar >> m_bSmooth;
		ar >> m_bHLRemoved;
		ar >> wd;
		m_displayMode = (GLDisplayMode)wd;
		ar >> m_antialias;

		ar >> m_bkRed;
		ar >> m_bkGreen;
		ar >> m_bkBlue;

		ar >> m_dVertical;

		ar >> m_dRange;

		ar >> m_xRot;
		ar >> m_zRot;
		ar >> m_yRot;
		ar >> m_xTrans;
		ar >> m_yTrans;
		ar >> m_zTrans;

		ar >> m_bSmoothGround;
		ar >> m_bLegend;					// 显示图例
		ar >> m_bShowAxis;					// 显示坐标框
		ar >> m_bShowGrid;					// 显示坐标网格
		
		int size;
		ar >> size;

		StatusProgress(1,size);

		for (int i=0; i<size; i++)
		{
			DWORD dw;
			ar >> dw;
			GLObjectType type = (GLObjectType)dw;

			switch(type)
			{
			case GLSURFACE:
				{
					CGridObject *pObj = new CGridObject;
					pObj->SetGLObjType(GLSURFACE);
					pObj->SetContext(this);
					pObj->Serialize(ar);
					m_listDisplay->push_back(pObj);
				}
				break;
			case GLPLANE:
				{
					C3DObject *pObj = new C3DObject;
					pObj->SetGLObjType(GLPLANE);
					pObj->SetContext(this);
					pObj->Serialize(ar);
					m_listDisplay->push_back(pObj);
				}
				break;
			case GLPIPELINE:
				{
					CPipeLine *pObj = new CPipeLine;
					pObj->SetGLObjType(GLPIPELINE);
					pObj->SetContext(this);
					pObj->Serialize(ar);
					m_listDisplay->push_back(pObj);
				}
				break;
			case GLINTERLAYERCELL:
				{
					InterLayerGridObject *pObj = new InterLayerGridObject;
					pObj->SetGLObjType(GLINTERLAYERCELL);
					pObj->SetContext(this);
					pObj->Serialize(ar);
					m_listDisplay->push_back(pObj);
				}
				break;
			default:
				break;
			}

			StatusSetProgress(1,i+1);
		}

		ar >> size;

		for (int i=0; i<size; i++)
		{
			C3DDice *pObj = new C3DDice;
			pObj->SetContext(this);
			pObj->Serialize(ar);
			m_listDice->push_back(pObj);
		}

		ar >> size;

		for (int i=0; i<size; i++)
		{
			C3DSlice *pObj = new C3DSlice;
			pObj->SetContext(this);
			pObj->Serialize(ar);
			m_listSlice->push_back(pObj);
		}

		ar >> size;

		for (int i=0; i<size; i++)
		{
			DWORD dw;
			ar >> dw;
			GLObjectType type = (GLObjectType)dw;

			switch(type)
			{
			case GLPLANE:
				{
					C3DObject *pObj = new C3DObject;
					pObj->SetGLObjType(GLPLANE);
					pObj->SetContext(this);
					pObj->Serialize(ar);
					m_listErase->push_back(pObj);
				}
				break;
			default:
				break;
			}
		}

		StatusProgress(1);
	}
}