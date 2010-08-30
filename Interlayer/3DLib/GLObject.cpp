// GLObject.cpp: implementation of the CGLObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GLObject.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

template<class T>
void ___swap(T &t1, T &t2)
{
	T t=t1;
	t1=t2;
	t2=t;
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
int CGLObject::m_glObjCount = 0;

CGLObject::CGLObject()
{
	m_glObjType = GLNONE;
	m_strObjName = _T("base");
	m_ListDone = 0;
	m_Modified = 1;	
	m_Show = 1;

	m_displayMode = GLWIREFRAME;

	itsShadeRed = 155;
	itsShadeGreen = 155;
	itsShadeBlue = 155;

	//AddMaterial(GOLD);

	SetColor(32, 255, 0);

	m_itsBox.SetLimits(-1, 1, -1, 1, -1, 1);

	m_glObjCount++;

	GUID guid;
	if (S_OK == ::CoCreateGuid(&guid))
	{
		m_glObjID = guid.Data1;
	}
	else
		m_glObjID = (int)m_glObjType + CGLObject::GetCount();
}

void CGLObject::SetColor(const GLubyte&r, const GLubyte&g, const GLubyte&b)
{
	m_Color.Set(r, g, b, m_Color.GetA());
}
void CGLObject::SetColor(CColor& color)
{
	m_Color = color;
}

void CGLObject::GetColor(GLubyte*clr) const
{
	clr[0] = m_Color.GetR();
	clr[1] = m_Color.GetG();
	clr[2] = m_Color.GetB();
}

CColor* CGLObject::GetColor(void)
{
	return &m_Color;
}

CGLObject::~CGLObject()
{
	m_glObjCount--;
}

void CGLObject::SetGLObjType(const GLObjectType& type)
{
	m_glObjType = type;
}

GLObjectType CGLObject::GetGLObjType() const
{
	return m_glObjType;
}

bool CGLObject::IsOfType(const GLObjectType& type) const
{
	GLObjectType typ = type;
	bool b = (m_glObjType == typ);
	return b;
}

void CGLObject::SetDisplayMode(const GLDisplayMode& mode)
{
	m_displayMode = mode;
	DefineDisplay();
	m_Modified = 1;
}

GLDisplayMode CGLObject::GetDisplayMode() const
{
	return m_displayMode;
}

void CGLObject::SetBoundingBox(const CBoundingBox& B)
{
	m_itsBox = B;
}

CBoundingBox CGLObject::GetBoundingBox() const
{
	return m_itsBox;
}

void CGLObject::ApplyMaterial()
{
	glLightfv(GL_LIGHT0,GL_AMBIENT,m_Material.GetAmbient());
	glLightfv(GL_LIGHT0,GL_DIFFUSE,m_Material.GetDiffuse());
	glLightfv(GL_LIGHT0,GL_SPECULAR,m_Material.GetSpecular());
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_Material.GetAmbient());
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_Material.GetDiffuse());
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, m_Material.GetShininess(0));
	glEnable(GL_LIGHT0);
}

void CGLObject::SetMaterial(float*mat)
{
	m_Material.SetMaterial(mat);
}

void CGLObject::SetMaterial(const GLMaterial&mat)
{
	switch (mat)
	{
		//Default Setting
	case DEFAULT:
		SetMaterial(DefaultM);
		break;
		//Non Metals
	case EMERALD:
		SetMaterial(Emerald);
		break;
	case JADE:
		SetMaterial(Jade);
		break;
	case OBSIDIAN:
		SetMaterial(Obsidian);
		break;
	case PEARL:
		SetMaterial(Pearl);
		break;
	case PEWTER:
		SetMaterial(Pewter);
		break;
	case RUBY:
		SetMaterial(Ruby);
		break;
	case TURQUOISE:
		SetMaterial(Turquoise);
		break;
	case PLASTIC:
		SetMaterial(Plastic);
		break;
	case RUBBER:
		SetMaterial(Rubber);
		break;
		//Metals
	case ALUMINIUM:
		SetMaterial(Aluminium);
		break;
	case BRASS:
		SetMaterial(Brass);
		break;
	case BRONZE:
		SetMaterial(Bronze);
		break;
	case P_BRONZE:
		SetMaterial(Polished_Bronze);
		break;
	case CHROME:
		SetMaterial(Chrome);
		break;
	case COPPER:
		SetMaterial(Copper);
		break;
	case P_COPPER:
		SetMaterial(Polished_Copper);
		break;
	case GOLD:
		SetMaterial(Gold);
		break;
	case P_GOLD:
		SetMaterial(Polished_Gold);
		break;
	case SILVER:
		SetMaterial(Silver);
		break;
	case P_SILVER:
		SetMaterial(Polished_Silver);
		break;
	case STEEL:
		SetMaterial(Steel);
		break;
	case NONE:
		SetMaterial(None);
		break;
	default:
		SetMaterial(DefaultM);
		break;
	}
}
void CGLObject::SetMaterial(CMaterial* pMat)
{
	m_Material = (*pMat);
}
void CGLObject::SetMaterial(CMaterial& mat)
{
	m_Material = mat;
}

//********************************************
// SetColorBinding
//********************************************
void CGLObject::SetColorBinding(COLORBINDING type)
{
	m_ColorBinding = type;
	m_Modified = 1;
}
//********************************************
// GetColorBinding
//********************************************
COLORBINDING CGLObject::GetColorBinding(void)
{
	return m_ColorBinding;
}


void CGLObject::DrawText(LPCTSTR string)
{
	LPCTSTR p = NULL;

	for (p = string; *p; p++)
	{
		if(*p>=0)
			glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12, *p);
		else
		{
			GLubyte bytes[32]={0};

 			get_hz(p,bytes);
 
  			for(int i=0;i<8;i++)
			{
				___swap(bytes[i*2], bytes[(15-i)*2]);
				___swap(bytes[i*2+1], bytes[(15-i)*2+1]);
			}

			GLubyte b[64];
			for(int i=0;i<16;i++)
			{
				b[i*4] = bytes[2*i];
				b[i*4+1] = bytes[2*i+1];

				b[i*4+2] = bytes[2*i];
				b[i*4+3] = bytes[2*i+1];
			}

			glBitmap(16,16, 0, 0, 16, 0, b);

			++p;
		}
	}
}

void CGLObject::get_hz(LPCTSTR incode,GLubyte *bytes)
{
	TCHAR szCurPath[MAX_PATH];  
	memset(szCurPath,   0,   MAX_PATH);  
	GetModuleFileName(NULL,   szCurPath,   sizeof(szCurPath)/sizeof(TCHAR));   
	CString strFile = szCurPath;
	strFile = strFile.Left(strFile.ReverseFind('\\')+1);
	strFile += _T("hzk16");

	CFile file;
	if( file.Open(strFile, CFile::modeRead|CFile::typeBinary))
	{
		unsigned char qh,wh;
		unsigned long offset;
		qh=incode[0]-0xa0;					// 得到区号
		wh=incode[1]-0xa0;					// 得到位号
		offset=(94*(qh-1)+(wh-1))*32L;		// 得到偏移位置
		file.Seek(offset,CFile::begin);		// 移文件指针到要读取的汉字字模处
		file.Read(bytes,32);				// *读取32个汉字的汉字字模
		file.Close();
	}
}

void CGLObject::print (float x, float y, float z, char* format, ...)
{
	va_list args;   //  Variable argument list
	int len;        // String length
	char * text;    // Text

	//  Initialize a variable argument list
	va_start(args, format);

	//  Return the number of characters in the string referenced the list of arguments.
	// _vscprintf doesn't count terminating '\0' (that's why +1)
	len = _vscprintf(format, args) + 1;

	//  Allocate memory for a string of the specified size
	text = (char*)malloc(len * sizeof(char));

	//  Write formatted output using a pointer to the list of arguments
	vsprintf_s(text, len, format, args);

	//  End using variable argument list
	va_end(args);

	//  Specify the raster position for pixel operations.
	glRasterPos3f (x, y, z);

	//  Draw the characters one by one
	DrawText(text);

	//  Free the allocated memory for the string
	free(text);
}
