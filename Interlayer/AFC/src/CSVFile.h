#pragma once
#include "afx.h"

class AFX_EXT_CLASS CCSVFile : public CStdioFile
{
public:
	enum Mode { modeRead, modeWrite };
	CCSVFile(LPCTSTR lpszFilename, Mode mode = modeRead);
	~CCSVFile(void);

	bool ReadData(CStringArray &arr, const TCHAR cSeq = ',');
	void WriteData(CStringArray &arr, const TCHAR cSeq = ',');
	
#ifdef _DEBUG
	Mode m_nMode;
#endif
};