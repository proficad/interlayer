#include "StdAfx.h"
#include "C3DDice.h"

C3DDice::C3DDice(void)
{
	m_strObjName = _T("切块");
	m_pointList = NULL;
	m_ListDone = 0;
	m_Modified = 1;
	m_ColorBinding = COLOR_PER_VERTEX;

	m_glObjType = GLDICE;

	GUID guid;
	if (S_OK == ::CoCreateGuid(&guid))
	{
		m_glObjID = guid.Data1;
	}
	else
		m_glObjID = (int)m_glObjType + CGLObject::GetCount();

	m_pointList = new std::vector<CVertex3D>;
}

C3DDice::~C3DDice(void)
{
	if (m_pointList)
	{
		m_pointList->clear();
		delete m_pointList;
	}
}

void C3DDice::Serialize(CArchive &ar)
{
	CGLObject::Serialize(ar);

	if( ar.IsStoring() )
	{
		int size = m_pointList->size();
		ar << size;

		std::vector<CVertex3D>::iterator iTerator = m_pointList->begin();		
		CVertex3D point;
		for (; iTerator != m_pointList->end(); iTerator++)
		{
			point = (*iTerator);
			point.Serialize(ar);
		}
	}
	else
	{
		if (m_pointList)
		{
			m_pointList->clear();
			delete m_pointList;
			m_pointList = NULL;
		}

		int size;
		ar >> size;

		m_pointList = new std::vector<CVertex3D>;

		for (int i=0; i<size; i++)
		{
			CVertex3D point;

			ar >> point;

			m_pointList->push_back(point);
		}

	}
}

CGLObject* C3DDice::Copy()
{
	C3DDice* O = new C3DDice();
	O->SetContext(m_pDisplayContext);
	O->m_glObjID = m_glObjID;
	O->m_strObjName = m_strObjName;
	O->m_glObjType = m_glObjType;
	if( m_pointList->size()>0)
	{
		O->m_pointList->resize(m_pointList->size());
		std::copy(m_pointList->begin(), m_pointList->end(), O->m_pointList->begin());
	}

	return O;
}


void C3DDice::DefineDisplay()
{	
	CWaitCursor wait;
	//gluLookAt(1.0,1.0,1.0,0.0,0.0,0.0,0.0,1.0,0.0); 
	glutSolidCube(0.5); 

	//return;
// 	if( m_pointList->size() < 12 )		// 小于三个面
// 		return;

	/*CVertex3D curP;

	std::vector<CVertex3D>::iterator myListIter = m_pointList->begin();	
	
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBegin(GL_QUADS);
	//glBegin(GL_LINE_LOOP);
	
	for (; myListIter!=m_pointList->end(); myListIter++)
	{
		curP = (*myListIter);
		glVertex3d(curP.GetX(), curP.GetY(),curP.GetZ());
	}

	glEnd();
	*/
}

void C3DDice::Display(const GLDisplayMode& dMode, bool bForce)
{
	m_displayMode = dMode;
	if (!m_Show)
		return;

	// Build list at first
	if (!m_ListDone || m_Modified)
	{
		if (!BuildList())
			return;
	}

	// Search for a new list
	if (::glIsList(m_ListOpenGL) == GL_TRUE)
	{
		::glCallList(m_ListOpenGL);
		return;
	}
	else
	{
		TRACE("unable to draw list %d\n", m_ListOpenGL);
		return;
	}
}

bool C3DDice::BuildList()
{
	if (!m_Modified && m_ListDone)
		return false;

	// Erase last list
	::glDeleteLists(m_ListOpenGL, 1);

	// Search for a new list
	m_ListOpenGL = ::glGenLists(1);
	if (m_ListOpenGL == 0)
	{
		TRACE("CGLObject::BuildList : unable to build DrawList\n");
		return false;
	}

	// Start list
	::glNewList(m_ListOpenGL, GL_COMPILE_AND_EXECUTE);

	DefineDisplay(); // 定义显示

	::glEndList();

	// List is done now
	m_ListDone = 1;
	m_Modified = 0;

	return true;
}

void C3DDice::DisplaySelected(const GLSelectedMode& dMode)
{
	Hilight(dMode);
}

void C3DDice::Hilight(const GLSelectedMode& dMode)
{
}
