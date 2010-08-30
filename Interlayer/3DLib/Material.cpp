#include "stdafx.h"
#include "Material.h"
#include "GLMaterials.h"

CMaterial::CMaterial()
{
	// Default
	SetAmbient(0.0f, 0.33f, 0.5f, 1.0f);
	SetDiffuse(0.5f, 0.5f, 0.5f, 1.0f);
	SetSpecular(1.0f, 1.0f, 1.0f, 1.0f);
	SetShininess(84.0f);
	SetEmission(0.0f, 0.0f, 0.0f, 1.0f);
}

CMaterial::CMaterial(CMaterial* pMaterial)
{
	Copy(pMaterial);
}
//////////////////////////////////////////////
// DATA SETTINGS
//////////////////////////////////////////////

//********************************************
// SetAmbient
//********************************************
void CMaterial::SetAmbient(float r, float g, float b, float a)
{
	m_pAmbient[0] = r;
	m_pAmbient[1] = g;
	m_pAmbient[2] = b;
	m_pAmbient[3] = a;
}

//********************************************
// SetDiffuse
//********************************************
void CMaterial::SetDiffuse(float r, float g, float b, float a)
{
	m_pDiffuse[0] = r;
	m_pDiffuse[1] = g;
	m_pDiffuse[2] = b;
	m_pDiffuse[3] = a;
}

//********************************************
// SetSpecular
//********************************************
void CMaterial::SetSpecular(float r, float g, float b, float a)
{
	m_pSpecular[0] = r;
	m_pSpecular[1] = g;
	m_pSpecular[2] = b;
	m_pSpecular[3] = a;
}

//********************************************
// SetEmission
//********************************************
void CMaterial::SetEmission(float r, float g, float b, float a)
{
	m_pEmission[0] = r;
	m_pEmission[1] = g;
	m_pEmission[2] = b;
	m_pEmission[3] = a;
}


//********************************************
// SetShininess
//********************************************
void CMaterial::SetShininess(float value)
{
	m_pShininess[0] = value;
}

//********************************************
// Copy
//********************************************
void CMaterial::Copy(CMaterial* pMaterial)
{
	SetSpecular(pMaterial->GetSpecular()[0], pMaterial->GetSpecular()[1], pMaterial->GetSpecular()[2], pMaterial->GetSpecular()[3]);
	SetAmbient(pMaterial->GetAmbient()[0], pMaterial->GetAmbient()[1], pMaterial->GetAmbient()[2], pMaterial->GetAmbient()[3]);
	SetDiffuse(pMaterial->GetDiffuse()[0], pMaterial->GetDiffuse()[1], pMaterial->GetDiffuse()[2], pMaterial->GetDiffuse()[3]);
	SetEmission(pMaterial->GetEmission()[0], pMaterial->GetEmission()[1], pMaterial->GetEmission()[2], pMaterial->GetEmission()[3]);
	SetShininess(pMaterial->GetShininess()[0]);
}

void CMaterial::SetMaterial(float* matArray)
{
	m_pAmbient[0] = matArray[0];
	m_pAmbient[1] = matArray[1];
	m_pAmbient[2] = matArray[2];
	m_pAmbient[3] = matArray[3];

	m_pDiffuse[0] = matArray[4];
	m_pDiffuse[1] = matArray[5];
	m_pDiffuse[2] = matArray[6];
	m_pDiffuse[3] = matArray[7];

	m_pSpecular[0] = matArray[8];
	m_pSpecular[1] = matArray[9];
	m_pSpecular[2] = matArray[10];
	m_pSpecular[3] = matArray[11];

	m_pEmission[0] = 0.0f;
	m_pEmission[1] = 0.0f;
	m_pEmission[2] = 0.0f;
	m_pEmission[3] = 0.0f;

	m_pShininess[0] = matArray[12];
}

void CMaterial::SetMaterial(const GLMaterial&mat)
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
void CMaterial::SetMaterial(CMaterial* pMat)
{
	Copy(pMat);
}
void CMaterial::SetMaterial(CMaterial& mat)
{
	(*this) = mat;
}

void CMaterial::GetMaterial(float* matArray)
{
	matArray[0] = m_pAmbient[0];
	matArray[1] = m_pAmbient[1];
	matArray[2] = m_pAmbient[2];
	matArray[3] = m_pAmbient[3];

	matArray[4] = m_pDiffuse[0];
	matArray[5] = m_pDiffuse[1];
	matArray[6] = m_pDiffuse[2];
	matArray[7] = m_pDiffuse[3];

	matArray[8] = m_pSpecular[0];
	matArray[9] = m_pSpecular[1];
	matArray[10] = m_pSpecular[2];
	matArray[11] = m_pSpecular[3];

	matArray[12] = m_pShininess[0];
}

CMaterial CMaterial::operator=(const CMaterial& Material)
{
	this->SetSpecular(Material.GetSpecular(0), Material.GetSpecular(1), Material.GetSpecular(2), Material.GetSpecular(3));
	this->SetAmbient(Material.GetAmbient(0), Material.GetAmbient(1), Material.GetAmbient(2), Material.GetAmbient(3));
	this->SetDiffuse(Material.GetDiffuse(0), Material.GetDiffuse(1), Material.GetDiffuse(2), Material.GetDiffuse(3));
	this->SetEmission(Material.GetEmission(0), Material.GetEmission(1), Material.GetEmission(2), Material.GetEmission(3));
	this->SetShininess(Material.GetShininess(0));

	return (*this);
}

void CMaterial::ApplyMaterial()
{
	//glLightfv(GL_LIGHT0,GL_AMBIENT,GetAmbient());
	//glLightfv(GL_LIGHT0,GL_DIFFUSE,GetDiffuse());
	//glLightfv(GL_LIGHT0,GL_SPECULAR,GetSpecular());
	GLfloat light_ambient [] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat light_diffuse [] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT , light_ambient );
	glLightfv(GL_LIGHT0, GL_DIFFUSE , light_diffuse );
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, GetAmbient());
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, GetDiffuse());
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, GetShininess(0));
	glEnable(GL_LIGHT0);
}