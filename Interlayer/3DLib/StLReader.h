#pragma once
#include "reader.h"

class CStLReader :	public CReader
{
public:
	CStLReader();
	CStLReader(const CString& fName);
	virtual ~CStLReader();
	virtual CStLReader* Copy();
	virtual void Serialize(CArchive& ar);
private:
	bool Read();
};
