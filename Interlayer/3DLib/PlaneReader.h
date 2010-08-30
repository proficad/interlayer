#pragma once
#include "reader.h"

class CPlaneReader : public CReader
{
public:
	CPlaneReader(void);
	CPlaneReader(const CString& fName);
	~CPlaneReader(void);
	CPlaneReader* Copy();
	virtual void Serialize(CArchive& ar);
private:
	bool Read();
};
