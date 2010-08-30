#pragma once
#include "Array3d.h"
#include "reader.h"

#define MAX_LINE_VRML 10000
#define MAX_WORD_VRML 1000

class CWrlReader : public CReader
{
public:
	CWrlReader(void);
	CWrlReader(const CString& fName);
	~CWrlReader(void);
	CWrlReader* Copy();
	virtual void Serialize(CArchive& ar);
private:
	bool Read();
	char* ReadFile(UINT& size);

	bool CheckVersion(char* pBuffer);
	bool ReadLine(char* pBuffer);
	bool ReadWord(char* pBuffer);

	bool OffsetToString(char* string, char* pBuffer);
	bool OffsetToStringBefore(char* string, char* before, char* pBuffer);
	bool OffsetToStringBeginLine(char* string, char* pBuffer);
	bool CheckMesh(char* pBuffer);

	bool ReadMesh(char* pBuffer);
	bool StoreMesh(CArray3d<CVertex3D>* pArrayVertex, int HasTexture, double* pTextureCoordinate, int* pTextureCoordinateIndex, char* pBuffer);
	bool SizeMesh(char* pBuffer, int* pNbVertex, int* pNbFace, int HasTexture, int* pNbTextureCoordinate = NULL);

private:	
	char m_pBufferLine[MAX_LINE_VRML];
	char m_pBufferWord[MAX_WORD_VRML];

	UINT m_IndexBuffer;
	UINT m_SizeFile;
};
