#include "StdAfx.h"
#include "WrlReader.h"
#include "Material.h"
#include "Texture.h"
#include "Transform.h"
#include "Utils3d.h"

extern void StatusProgress(int nIndex, int nRange=-1);
extern void StatusSetProgress(int nIndex, int nPos);

CWrlReader::CWrlReader(void)
{
	m_IndexBuffer = 0;
	m_pBufferWord[0] = '\0';
	m_pBufferLine[0] = '\0';
	fileName.Empty();
}

CWrlReader::CWrlReader(const CString& fName)
{
	m_IndexBuffer = 0;
	m_pBufferWord[0] = '\0';
	m_pBufferLine[0] = '\0';
	fileName = fName;
	Read();
}

CWrlReader::~CWrlReader()
{
}

CWrlReader* CWrlReader::Copy()
{
	CWrlReader* R = new CWrlReader;
	R->fileName = fileName;
	if( m_pointList.empty() )
	{
		R->m_pointList.resize(m_pointList.size());
		std::copy(m_pointList.begin(), m_pointList.end(), R->m_pointList.begin());

		R->m_indexs.resize(m_indexs.size());
		std::copy(m_indexs.begin(), m_indexs.end(), R->m_indexs.begin());
	}
	return R;
}

bool CWrlReader::Read()
{
	char* pBuffer = NULL;
	pBuffer = ReadFile(m_SizeFile);
	if (m_SizeFile < 1)
		return false;

	if (!CheckVersion(pBuffer))
		return false;

	while (OffsetToStringBeginLine("DEF", pBuffer))
		ReadMesh(pBuffer);

	delete pBuffer;
	return true;
}

char* CWrlReader::ReadFile(UINT& size)
{
	if( fileName.IsEmpty() )
		return NULL;

	size = 0;
	CFile file;
	CFileException ex;

	char* pBuffer = NULL;

	// Opening	
	if (!file.Open(fileName, CFile::modeRead | CFile::typeBinary, &ex))
		return NULL;

	// Size file
	size = file.GetLength();

	if (size < 1)
		return NULL;

	pBuffer = new char[size];

	if (pBuffer == NULL)
	{
		size = 0;
		return NULL;
	}

	// Reading
	TRY
	{
		file.Read(pBuffer, size);
	}
	CATCH(CFileException, e)
	{
		file.Close();		
		delete pBuffer;

		size = 0;

		return NULL;
	}
	END_CATCH

	// Closing
	file.Close();

	return pBuffer;
}


//********************************************
// CheckVersion
//********************************************
bool CWrlReader::CheckVersion(char* pBuffer)
{
	ReadLine(pBuffer);
	TRACE("  check version (vrml 2.0)...");
	if (strstr(m_pBufferLine, "#VRML V2.0") != NULL)
	{
		TRACE("ok\n");
		return true;
	}
	TRACE("invalid\n");
	return false;
}


//********************************************
// ReadLine
// eol : '\n'
// eos : '\0'
//********************************************
bool CWrlReader::ReadLine(char* pBuffer)
{
	m_pBufferLine[0] = '\0';
	int i = 0;
	do
	{
		m_pBufferLine[i++] = pBuffer[m_IndexBuffer++];
	}
	while (pBuffer[m_IndexBuffer - 1] != '\n' && i < MAX_LINE_VRML && m_IndexBuffer < m_SizeFile);

	m_pBufferLine[i - 1] = '\0';

	return true;
}


//********************************************
// ReadLine
// eol : '\n'
// eos : '\0'
//********************************************
bool CWrlReader::ReadWord(char* pBuffer)
{
	m_pBufferWord[0] = '\0';
	int i = 0;

	// Jump to next valid character
	while ((pBuffer[m_IndexBuffer] == '\n' || pBuffer[m_IndexBuffer] == '\t' || pBuffer[m_IndexBuffer] == '\r' || pBuffer[m_IndexBuffer] == ' ') && m_IndexBuffer < m_SizeFile)
		m_IndexBuffer++;

	// Check eof
	if (m_IndexBuffer >= m_SizeFile)
		return false;
	do
	{
		m_pBufferWord[i++] = pBuffer[m_IndexBuffer++];
	}
	while (pBuffer[m_IndexBuffer - 1] != '\n' && pBuffer[m_IndexBuffer - 1] != '\t' && pBuffer[m_IndexBuffer - 1] != '\r' && pBuffer[m_IndexBuffer - 1] != ' ' && i < MAX_WORD_VRML && m_IndexBuffer < m_SizeFile);

	m_pBufferWord[i - 1] = '\0';

	//TRACE("  word : %s\n",m_pBufferWord);

	return true;
}

//********************************************
// OffsetToStringBeginLine
//********************************************
bool CWrlReader::OffsetToStringBeginLine(char* string, char* pBuffer)
{
	while (m_IndexBuffer < m_SizeFile)
	{
		ReadLine(pBuffer);
		if (strncmp(m_pBufferLine, string, strlen(string)) == 0)
		{
			m_IndexBuffer -= strlen(m_pBufferLine) + 1;
			return true;
		}
	}

	return false;
}

//********************************************
// OffsetToString
//********************************************
bool CWrlReader::OffsetToString(char* string, char* pBuffer)
{
	while (m_IndexBuffer < m_SizeFile)
	{
		ReadLine(pBuffer);
		char* adr = strstr(m_pBufferLine, string);
		if (strstr(m_pBufferLine, string) != NULL)
		{
			m_IndexBuffer = m_IndexBuffer - strlen(m_pBufferLine) - 1 + (adr - m_pBufferLine);			
			return true;
		}
	}

	return false;
}

//********************************************
// OffsetToString
//********************************************
bool CWrlReader::OffsetToStringBefore(char* string, char* before, char* pBuffer)
{
	while (m_IndexBuffer < m_SizeFile)
	{
		ReadLine(pBuffer);
		char* adr = strstr(m_pBufferLine, string);
		if (strstr(m_pBufferLine, before) != NULL)
			return false;
		if (strstr(m_pBufferLine, string) != NULL)
		{
			m_IndexBuffer = m_IndexBuffer - strlen(m_pBufferLine) - 1 + (adr - m_pBufferLine);			
			return true;
		}
	}

	return false;
}

//********************************************
// CheckMesh
//********************************************
bool CWrlReader::CheckMesh(char* pBuffer)
{
	// Find a possible mesh
	TRACE("  check mesh...");
	OffsetToStringBeginLine("DEF", pBuffer);
	int tmp = m_IndexBuffer;
	ReadLine(pBuffer);
	if (strstr(m_pBufferLine, "DEF") != NULL && strstr(m_pBufferLine, "Transform") != NULL)
	{
		m_IndexBuffer = tmp;
		if (OffsetToString("Transform", pBuffer) && OffsetToString("Material", pBuffer) && OffsetToString("IndexedFaceSet", pBuffer))
		{
			m_IndexBuffer = tmp;
			TRACE("ok\n");
			return true;
		}
	}
	//m_IndexBuffer = tmp;
	TRACE("not a mesh\n");
	return false;
}


//********************************************
// ReadMesh
//********************************************
bool CWrlReader::ReadMesh(char* pBuffer)
{
	// Check
	if (!CheckMesh(pBuffer))
		return false;

	int tmp = m_IndexBuffer;
	ReadLine(pBuffer);
	ASSERT(strstr(m_pBufferLine, "DEF") != NULL);
	ASSERT(strstr(m_pBufferLine, "Transform") != NULL);
	// DEF [name] Transform {
	if (sscanf_s(m_pBufferLine, "DEF %s Transform", m_pBufferWord) != 1)
	{
		TRACE("  invalid syntax (BufferLine : %s BufferWord : %s\n", m_pBufferLine, m_pBufferWord);
		return false;
	}

	TRACE("  start reading mesh %s\n", m_pBufferWord);

	m_IndexBuffer = tmp;

	int IndexTexture = -1;

	// Transform
	//********************************************
	// Syntax :
	// Transform {
	// translation -360.7 1370 3471
	// rotation 0.3236 -0.3236 -0.8891 -1.688
	// scale -49.36 -49.36 -49.36
	// scaleOrientation -0.689 0.4766 -0.546 -0.6007

	OffsetToString("Transform", pBuffer);
	ReadLine(pBuffer); // Transform
	CTransform transform;
	transform.Clear();

	// Translation
	tmp = m_IndexBuffer;
	ReadLine(pBuffer);
	if (strstr(m_pBufferLine, "translation") != NULL)
	{
		// Come back
		m_IndexBuffer = tmp;
		// Jump after "translation"
		ReadWord(pBuffer); 

		double x, y, z;
		ReadWord(pBuffer);
		int success = sscanf_s(m_pBufferWord, "%f", &x);
		ReadWord(pBuffer);
		success &= sscanf_s(m_pBufferWord, "%f", &y);
		ReadWord(pBuffer);
		success &= sscanf_s(m_pBufferWord, "%f", &z);
		if (success)
		{
			transform.SetTranslation(CVector3D(x, y, z));
			TRACE("    translation : %g %g %g\n", x, y, z);
		}
		ReadLine(pBuffer);
		tmp = m_IndexBuffer;
		ReadLine(pBuffer);
	}

	// Rotation
	if (strstr(m_pBufferLine, "rotation") != NULL)
	{
		// Come back
		m_IndexBuffer = tmp;
		// Jump after "rotation"
		ReadWord(pBuffer); 

		double x, y, z, val;
		ReadWord(pBuffer);
		int success = sscanf_s(m_pBufferWord, "%lf", &x);
		ReadWord(pBuffer);
		success &= sscanf_s(m_pBufferWord, "%lf", &y);
		ReadWord(pBuffer);
		success &= sscanf_s(m_pBufferWord, "%lf", &z);
		ReadWord(pBuffer);
		success &= sscanf_s(m_pBufferWord, "%lf", &val);
		if (success)
		{
			transform.SetRotation(CVector3D(x, y, z));
			transform.SetValueRotation(val / 3.1415926f * 180.0f);
			TRACE("    rotation : %g %g %g %g\n", x, y, z, val);
		}
		ReadLine(pBuffer);
		tmp = m_IndexBuffer;
		ReadLine(pBuffer);
	}

	// Scale
	if (strstr(m_pBufferLine, "scale") != NULL)
	{
		// Come back
		m_IndexBuffer = tmp;
		// Jump after "scale"
		ReadWord(pBuffer); 

		double x, y, z;
		ReadWord(pBuffer);
		int success = sscanf_s(m_pBufferWord, "%f", &x);
		ReadWord(pBuffer);
		success &= sscanf_s(m_pBufferWord, "%f", &y);
		ReadWord(pBuffer);
		success &= sscanf_s(m_pBufferWord, "%f", &z);
		if (success)
		{
			transform.SetScale(CVector3D(x, y, z));
			TRACE("    scale : %g %g %g\n", x, y, z);
		}
		ReadLine(pBuffer);
		tmp = m_IndexBuffer;
		ReadLine(pBuffer);
	}

	// ScaleOrientation
	if (strstr(m_pBufferLine, "scaleOrientation") != NULL)
	{
		// Come back
		m_IndexBuffer = tmp;
		// Jump after "scaleOrientation"
		ReadWord(pBuffer); 

		double x, y, z, val;
		ReadWord(pBuffer);
		int success = sscanf_s(m_pBufferWord, "%lf", &x);
		ReadWord(pBuffer);
		success &= sscanf_s(m_pBufferWord, "%lf", &y);
		ReadWord(pBuffer);
		success &= sscanf_s(m_pBufferWord, "%lf", &z);
		ReadWord(pBuffer);
		success &= sscanf_s(m_pBufferWord, "%lf", &val);
		if (success)
		{
			//transform.SetScale(CVector3D(x,y,z));
			TRACE("    scaleOrientation : %g %g %g %g\n", x, y, z, val);
		}
		ReadLine(pBuffer);
	}

	// Material
	//********************************************
	// appearance Appearance {
	// material Material {
	// diffuseColor 0.5686 0.1098 0.6941
	CMaterial material;
	if (OffsetToString("Material", pBuffer))
	{
		ReadLine(pBuffer);

		tmp = m_IndexBuffer;

		// Diffuse color
		ReadLine(pBuffer); 
		if (strstr(m_pBufferLine, "diffuseColor") != NULL)
		{
			// Come back
			m_IndexBuffer = tmp;

			// Jump
			ReadWord(pBuffer);

			float r, g, b;
			ReadWord(pBuffer);
			int success = sscanf_s(m_pBufferWord, "%f", &r);
			ReadWord(pBuffer);
			success &= sscanf_s(m_pBufferWord, "%f", &g);
			ReadWord(pBuffer);
			success &= sscanf_s(m_pBufferWord, "%f", &b);
			if (success)
			{
				material.SetDiffuse(r, g, b, 1.0);
				TRACE("    diffuseColor : %g %g %g\n", r, g, b);
			}
		}
	}

	// Texture
	//********************************************
	int texture = 0;
	if (OffsetToStringBefore("texture ImageTexture", "geometry", pBuffer))
	{
		texture = 1;
		ReadLine(pBuffer);
		tmp = m_IndexBuffer;

		ReadLine(pBuffer); 
		if (strstr(m_pBufferLine, "url") != NULL)
		{
			// Come back
			m_IndexBuffer = tmp;

			// Jump
			ReadWord(pBuffer);
			char string[MAX_PATH];
			ReadWord(pBuffer);
			sscanf_s(m_pBufferWord, "%s", string);

			// Remove ""
			CString TextureName = string;
			TextureName = TextureName.Mid(1, TextureName.GetLength() - 2);
			TRACE("    texture : %s\n", TextureName);

			// Ask SceneGraph to add texture, if needed
			char* name = TextureName.GetBuffer(MAX_PATH);
			/*if (!pSceneGraph->HasTexture(name, &IndexTexture))
					{
						CTexture* pTexture = new CTexture;
						pTexture->ReadFile(name);
						IndexTexture = pSceneGraph->AddTexture(pTexture);
					}
					*/
			TextureName.ReleaseBuffer();
		}
	}
	else // come back
		m_IndexBuffer = tmp;

	// Mesh
	//********************************************
	int NbVertex, NbFace, NbTextureCoordinate;	
	// Count size (do not offset in file)
	if (!SizeMesh(pBuffer, &NbVertex, &NbFace, texture, &NbTextureCoordinate))
		return false;

	CArray3d<CVertex3D> ArrayVertex;
	ArrayVertex.SetSize(NbVertex);
	StoreMesh(&ArrayVertex, 0, NULL, NULL, pBuffer);
	ArrayVertex.Free();
	/*
		// Add mesh
		CMesh3d* pMesh = new CMesh3d;
		pSceneGraph->Add(pMesh);
		// Set Size (faster)
		pMesh->m_ArrayVertex.SetSize(NbVertex);
		pMesh->m_ArrayFace.SetSize(NbFace);
		if (texture)
		{
			pMesh->m_pTextureCoordinate = new double[NbTextureCoordinate * 2]; // x y 
			pMesh->m_pTextureCoordinateIndex = new int[NbFace * 3]; 		  // triangular faces
			pMesh->m_IndexTexture = IndexTexture;
		}
		// Store mesh (offset in file)
		CArray3d<CVertex3D> ArrayVertex;
		ArrayVertex.SetSize(NbVertex);
		StoreMesh(&ArrayVertex, texture, pMesh->m_pTextureCoordinate, pMesh->m_pTextureCoordinateIndex, pBuffer);
		// Transform & material
		pMesh->SetTransform(transform);
		pMesh->SetMaterial(&material);
		TRACE("  end reading mesh\n");
	*/
	return true;
}

//********************************************
// SizeMesh
//********************************************
bool CWrlReader::SizeMesh(char* pBuffer, int* pNbVertex, int* pNbFace, int HasTexture, int* pNbTextureCoordinate /* = NULL */)
{
	TRACE("    size mesh...");
	int tmp = m_IndexBuffer;

	ASSERT(pNbVertex != NULL);
	ASSERT(pNbFace != NULL);

	if (!OffsetToString("IndexedFaceSet", pBuffer))
	{
		TRACE("invalid mesh\n");
		return false;
	}

	// Count points
	//***********************************************
	if (!OffsetToString("Coordinate { point [", pBuffer))
	{
		TRACE("invalid mesh\n");
		return false;
	}

	m_IndexBuffer += strlen("Coordinate { point [") + 1;

	// Cur : x y z,
	// End : x y z]
	int NbVertex = 0;
	int success;
	do
	{
		double x, y, z;
		ReadWord(pBuffer);
		success = sscanf_s(m_pBufferWord, "%f", &x);
		ReadWord(pBuffer);
		success &= sscanf_s(m_pBufferWord, "%f", &y);
		ReadWord(pBuffer);
		success &= sscanf_s(m_pBufferWord, "%f", &z);
		NbVertex += success;
		//TRACE("\n (%g %g %g) ",x,y,z);
	}
	while (success);
	TRACE(" %d points,", NbVertex);

	if (NbVertex <= 0)
		return 0;

	// Count texture coordinates, if needed 
	//***********************************************
	int NbTextureCoordinate = 0;
	if (HasTexture)
	{
		if (!OffsetToString("TextureCoordinate { point [", pBuffer))
		{
			TRACE("invalid texture coordinates\n");
			return 0;
		}

		m_IndexBuffer += strlen("TextureCoordinate { point [") + 1;

		// Cur : x y,
		// End : x y]
		int success;
		do
		{
			double x, y;
			ReadWord(pBuffer);
			success = sscanf_s(m_pBufferWord, "%f", &x);
			ReadWord(pBuffer);
			success &= sscanf_s(m_pBufferWord, "%f", &y);
			NbTextureCoordinate += success;
			//TRACE("\n (%g %g %g) ",x,y,z);
		}
		while (success);
		TRACE(" %d texture coordinates,", NbTextureCoordinate);

		if (NbTextureCoordinate <= 0)
			return 0;
	}


	// Count faces, accept only triangles
	//***********************************************
	m_IndexBuffer = tmp;
	if (!OffsetToString("coordIndex [", pBuffer))
	{
		TRACE("invalid mesh\n");
		return 0;
	}
	m_IndexBuffer += strlen("coordIndex [") + 1;

	// Cur : int, int, int, -1,
	// End : int, int, int, -1]
	int NbFace = 0;
	do
	{
		int v1, v2, v3;
		ReadWord(pBuffer);
		success = sscanf_s(m_pBufferWord, "%d,", &v1);
		ReadWord(pBuffer);
		success &= sscanf_s(m_pBufferWord, "%d,", &v2);
		ReadWord(pBuffer);
		success &= sscanf_s(m_pBufferWord, "%d,", &v3);
		NbFace += success;

		ASSERT(v1 >= 0);
		ASSERT(v2 >= 0);
		ASSERT(v3 >= 0);

		int test;
		ReadWord(pBuffer);
		sscanf_s(m_pBufferWord, "%d", &test);
		if (strstr(m_pBufferWord, "]") != NULL)
			success = 0;
	}
	while (success);
	TRACE(" %d faces,", NbFace);

	if (NbFace <= 0)
		return 0;

	// Count texture coordinate index 
	//***********************************************
	if (HasTexture)
	{
		m_IndexBuffer = tmp;
		if (!OffsetToString("texCoordIndex [", pBuffer))
		{
			TRACE("invalid texture coordinate index\n");
			return 0;
		}
		m_IndexBuffer += strlen("texCoordIndex [") + 1;

		// Cur : int, int, int, -1,
		// End : int, int, int, -1]
		int NbCoordIndex = 0;
		do
		{
			int v1, v2, v3;
			ReadWord(pBuffer);
			success = sscanf_s(m_pBufferWord, "%d,", &v1);
			ReadWord(pBuffer);
			success &= sscanf_s(m_pBufferWord, "%d,", &v2);
			ReadWord(pBuffer);
			success &= sscanf_s(m_pBufferWord, "%d,", &v3);
			NbCoordIndex += success;

			ASSERT(v1 >= 0);
			ASSERT(v2 >= 0);
			ASSERT(v3 >= 0);

			int test;
			ReadWord(pBuffer);
			sscanf_s(m_pBufferWord, "%d", &test);
			if (strstr(m_pBufferWord, "]") != NULL)
				success = 0;
		}
		while (success);
		TRACE(" %d coordinate index\n", NbCoordIndex);

		if (NbFace != NbCoordIndex)
		{
			TRACE(" different values for coord index and faces\n");
			return false;
		}
	}

	// Store result
	*pNbVertex = NbVertex;
	*pNbFace = NbFace;
	if (HasTexture)
		*pNbTextureCoordinate = NbTextureCoordinate;

	m_IndexBuffer = tmp;

	return true;
}


//********************************************
// StoreMesh
//********************************************
bool CWrlReader::StoreMesh(CArray3d<CVertex3D>* pArrayVertex, int HasTexture, double* pTextureCoordinate, int* pTextureCoordinateIndex, char* pBuffer)
{
	TRACE("    store mesh...");
	int tmp = m_IndexBuffer;

	if (!OffsetToString("IndexedFaceSet", pBuffer))
	{
		TRACE("invalid mesh\n");
		return 0;
	}

	// Store vertices
	//***********************************************
	if (!OffsetToString("Coordinate { point [", pBuffer))
	{
		TRACE("invalid mesh\n");
		return 0;
	}
	m_IndexBuffer += strlen("Coordinate { point [") + 1;
	// Cur : x y z,
	// End : x y z]
	int success;
	int NbVertex = 0;

	double m_dMinX = DBL_MAX, m_dMaxX = -DBL_MAX;
	double m_dMinY = DBL_MAX, m_dMaxY = -DBL_MAX;
	double m_dMinZ = DBL_MAX, m_dMaxZ = -DBL_MAX;

	do
	{
		double x, y, z;
		ReadWord(pBuffer);
		success = sscanf_s(m_pBufferWord, "%lf", &x);
		ReadWord(pBuffer);
		success &= sscanf_s(m_pBufferWord, "%lf", &y);
		ReadWord(pBuffer);
		success &= sscanf_s(m_pBufferWord, "%lf", &z);
		if (success)
			pArrayVertex->SetAt(NbVertex++, new CVertex3D(x, y, z));

		if( m_dMinX > x )
			m_dMinX = x;
		if( m_dMinY > y )
			m_dMinY = y;
		if( m_dMinZ > z )
			m_dMinZ = z;
		if( m_dMaxX < x )
			m_dMaxX = x;
		if( m_dMaxY < y )
			m_dMaxY = y;
		if( m_dMaxZ < z )
			m_dMaxZ = z;
	}
	while (success);

	// Store texture coordinates (if needed)
	//***********************************************
	if (HasTexture)
	{
		if (!OffsetToString("TextureCoordinate { point [", pBuffer))
		{
			TRACE("invalid texture coordinate\n");
			return 0;
		}
		m_IndexBuffer += strlen("TextureCoordinate { point [") + 1;
		// Cur : x y,
		// End : x y
		int success;
		int NbTextureCoordinate = 0;
		do
		{
			double x, y;
			ReadWord(pBuffer);
			success = sscanf_s(m_pBufferWord, "%lf", &x);
			ReadWord(pBuffer);
			success &= sscanf_s(m_pBufferWord, "%lf", &y);
			if (success)
			{
				pTextureCoordinate[2 * NbTextureCoordinate] = x;
				pTextureCoordinate[2 * NbTextureCoordinate + 1] = y;
				NbTextureCoordinate++;
			}
			//TRACE("\n (%g %g) ",x,y);
		}
		while (success);
		TRACE(" added %d texture coordinates,", NbTextureCoordinate);
	}

	// Store faces, accept only triangles
	//***********************************************
	m_IndexBuffer = tmp;
	if (!OffsetToString("coordIndex [", pBuffer))
	{
		TRACE("invalid mesh\n");
		return 0;
	}
	m_IndexBuffer += strlen("coordIndex [") + 1;

	// Cur : int, int, int, -1,
	// End : int, int, int, -1]
	int NbFace = 0;
	do
	{
		int v1, v2, v3;
		ReadWord(pBuffer);
		success = sscanf_s(m_pBufferWord, "%d,", &v1);
		ReadWord(pBuffer);
		success &= sscanf_s(m_pBufferWord, "%d,", &v2);
		ReadWord(pBuffer);
		success &= sscanf_s(m_pBufferWord, "%d,", &v3);

		ASSERT(v1 >= 0);
		ASSERT(v2 >= 0);
		ASSERT(v3 >= 0);

		if (success && v1 >= 0 && v2 >= 0 && v3 >= 0)
		{
			//CFace3d* pFace = new CFace3d(pArrayVertex->GetAt(v1), pArrayVertex->GetAt(v2), pArrayVertex->GetAt(v3));
			//pArrayFace->SetAt(NbFace++, pFace);

			CVertex3D pt1, pt2, pt3;
			CVertex3D p1, p2, p3;

			pt1.SetParam(pArrayVertex->GetAt(v1)->GetX(), pArrayVertex->GetAt(v1)->GetY(), pArrayVertex->GetAt(v1)->GetZ());
			pt2.SetParam(pArrayVertex->GetAt(v2)->GetX(), pArrayVertex->GetAt(v2)->GetY(), pArrayVertex->GetAt(v2)->GetZ());
			pt3.SetParam(pArrayVertex->GetAt(v3)->GetX(), pArrayVertex->GetAt(v3)->GetY(), pArrayVertex->GetAt(v3)->GetZ());

			p1= pt1;
			p2= pt2;
			p3= pt3;

			pt1.x = 2*(pt1.x - m_dMinX)/(m_dMaxX - m_dMinX)-1;
			pt1.y = 2*(pt1.y - m_dMinY)/(m_dMaxY - m_dMinY)-1;
			pt1.z = 2*(pt1.z - m_dMinZ)/(m_dMaxZ - m_dMinZ)-1;

			pt2.x = 2*(pt2.x - m_dMinX)/(m_dMaxX - m_dMinX)-1;
			pt2.y = 2*(pt2.y - m_dMinY)/(m_dMaxY - m_dMinY)-1;
			pt2.z = 2*(pt2.z - m_dMinZ)/(m_dMaxZ - m_dMinZ)-1;

			pt3.x = 2*(pt3.x - m_dMinX)/(m_dMaxX - m_dMinX)-1;
			pt3.y = 2*(pt3.y - m_dMinY)/(m_dMaxY - m_dMinY)-1;
			pt3.z = 2*(pt3.z - m_dMinZ)/(m_dMaxZ - m_dMinZ)-1;


			CVector3D vec1(pt1, pt2);
			CVector3D vec2(pt1, pt3);
			CVector3D normal = CalculateNormal(vec1, vec2);
			CVertex3D pt;
			pt.SetParam(normal.GetX(), normal.GetY(), normal.GetZ());
// 			pointList->push_back(pt);
// 			pointList->push_back(p1);
// 			pointList->push_back(p2);
// 			pointList->push_back(p3);
		}

		int test;
		ReadWord(pBuffer);
		sscanf_s(m_pBufferWord, "%d", &test);
		if (strstr(m_pBufferWord, "]") != NULL)
			success = 0;
	}
	while (success);
	TRACE(" added %d faces\n", NbFace);

	// Store texture coord index
	//***********************************************
	if (HasTexture)
	{
		m_IndexBuffer = tmp;
		if (!OffsetToString("texCoordIndex [", pBuffer))
		{
			TRACE("invalid mesh\n");
			return false;
		}
		m_IndexBuffer += strlen("texCoordIndex [") + 1;

		// Cur : int, int, int, -1,
		// End : int, int, int, -1]
		int NbTexCoordIndex = 0;
		do
		{
			int v1, v2, v3;
			ReadWord(pBuffer);
			success = sscanf_s(m_pBufferWord, "%d,", &v1);
			ReadWord(pBuffer);
			success &= sscanf_s(m_pBufferWord, "%d,", &v2);
			ReadWord(pBuffer);
			success &= sscanf_s(m_pBufferWord, "%d,", &v3);

			ASSERT(v1 >= 0);
			ASSERT(v2 >= 0);
			ASSERT(v3 >= 0);

			if (success && v1 >= 0 && v2 >= 0 && v3 >= 0)
			{
				pTextureCoordinateIndex[3 * NbTexCoordIndex] = v1;
				pTextureCoordinateIndex[3 * NbTexCoordIndex + 1] = v2;
				pTextureCoordinateIndex[3 * NbTexCoordIndex + 2] = v3;
				NbTexCoordIndex++;
			}

			int test;
			ReadWord(pBuffer);
			sscanf_s(m_pBufferWord, "%d", &test);
			if (strstr(m_pBufferWord, "]") != NULL)
				success = 0;
		}
		while (success);
		TRACE(" added %d texture coordinate index\n", NbTexCoordIndex);
	}

	return true;
}

void CWrlReader::Serialize(CArchive& ar)
{
	CReader::Serialize(ar);
}