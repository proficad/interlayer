//############################################################################
//
// GeomException.h: interface for the CGeomException class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GEOMEXCEPTION_H__83974345_743E_4440_A01B_6B925B6997E3__INCLUDED_)
#define AFX_GEOMEXCEPTION_H__83974345_743E_4440_A01B_6B925B6997E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum ErrorType
{
	DivideByZero,
	ConstructionFailure,
	MaxDegree
};

class CGeomException
{
public:
	CGeomException();
	CGeomException(ErrorType type);
	virtual ~CGeomException();
	char* ErrorMessage() const;
private:
	char* msg;
};

#endif // !defined(AFX_GEOMEXCEPTION_H__83974345_743E_4440_A01B_6B925B6997E3__INCLUDED_)
