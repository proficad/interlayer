#include "stdafx.h"
#include "Transform.h"

CTransform::CTransform()
{
	SetScale(CVector3D(1.0, 1.0, 1.0));
	SetTranslation(CVector3D(0.0f,0.0f,0.0f));
	SetValueRotation(0.0f);
}

//********************************************
// Constructor
//********************************************
void CTransform::Clear(void)
{
	SetScale(CVector3D(1.0, 1.0, 1.0));
	SetTranslation(CVector3D(0.0, 0.0, 0.0));
	SetValueRotation(0.0f);
}

//********************************************
// Copy
//********************************************
void CTransform::Copy(CTransform& transform)
{
	SetScale(transform.GetScale());
	SetTranslation(transform.GetTranslation());
	SetRotation(transform.GetRotation());
	SetValueRotation(transform.GetValueRotation());
}

//********************************************
// Copy
//********************************************
void CTransform::Copy(CTransform* pTransform)
{
	SetScale(pTransform->GetScale());
	SetTranslation(pTransform->GetTranslation());
	SetRotation(pTransform->GetRotation());
	SetValueRotation(pTransform->GetValueRotation());
}

