// StlObject.h: interface for the C3DObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STLOBJECT_H__0B30ABE7_430E_41A7_80B1_41426F4C6C87__INCLUDED_)
#define AFX_STLOBJECT_H__0B30ABE7_430E_41A7_80B1_41426F4C6C87__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GLObject.h"
#include "StLReader.h"
#include "Reader.h"
#include "Vector3d.h"
#include "Array3d.h"
#include "Material.h"
#include "Texture.h"
#include "FaceTriangles.h"

class C3DObject : public CGLObject
{
public:
	C3DObject();
	C3DObject(CReader*);
	virtual ~C3DObject();
	virtual CGLObject* Copy();
	virtual bool BuildList();
	virtual void DefineDisplay();
	virtual void Display(const GLDisplayMode& = GLWIREFRAME, bool bForce = false);
	virtual void DisplaySelected(const GLSelectedMode& = GLSELECTED);
	virtual void Hilight(const GLSelectedMode&);
	void ReversePoints();
	void SaveSurface(const std::string& filename);
	void SaveDivideSurface( const std::string& filename, const std::string& newfilename, const CVector3D& size, int index );

	int m_iSelectedPoint;
	std::vector<CVertex3D>* GetPointList() { return &m_pointList; }

	std::vector<CVertex3D> m_pointList;
	std::vector<int> m_indexs;

	//-----------------------------------------------------------------------
	CArray3d<CVertex3D> m_ArrayVertex;
	CArray3d<CFaceTriangles> m_ArrayFace;

private:
	void DrawWired(const GLSelectedMode& dMode = GLNOSELECTED);
	void DrawShaded();
	void ComputePoints(CReader* r);

	void ComputeBoundLimits();
private:	

	// 外貌
	NORMALBINDING m_NormalBinding;				// 定义顶点或面的法向量? (Gouraud)着色
	
	// 纹理
	int m_IndexTexture;					// 纹理? (-1 : 无纹理)
	double* m_pTextureCoordinate;		// 纹理坐标
	int* m_pTextureCoordinateIndex;		// 纹理索引

public:
	// Datas
	void Free();
	int IsValid();	
	
	// Faces
	int NbFace();
	CFaceTriangles* GetFace(int index);
	void AddFace(CFaceTriangles* pFace);
	void SetNbFace(int NbFace);
	CArray3d<CFaceTriangles>* GetArrayFace();
	int Has(CFaceTriangles* pFace);
	int FindFace(CFaceTriangles* pFace); // even in neighborign faces

	// Vertices
	int NbVertex();
	void SetNbVertex(int NbVertex);
	CArray3d<CVertex3D>* GetArrayVertex();
	void AddVertex(CVertex3D* pVertex);
	int DeleteVertex(CVertex3D* pVertex);
	int DeleteVertex(int index);
	int DeleteFaceNbNeighbors(int NbNeighbor);
	CVertex3D* GetVertex(int index);
	int Has(CVertex3D* pVertex);

	// Flags
	void SetFlagOnFaces(int flag);
	void SetFlagOnVertices(int flag);
	CVertex3D* GetFirstVertexWithFlag(int flag);
	int GetMaxFlagOnFaces();
	int FindSmallestFlagOnVerticesDiffThan(int flag, int* pFounded);
	int FindFlagOnVerticesDiffThan(int flag);
	CVertex3D* GetAnyVertexWithFlagRootPrefered(int flag);

	// Edges 边
	double GetMeanLengthEdge();
	
	// Range 范围
	void Range(int coord, double* min, double* max); 
	void Range(int coord, double min, double max); 
	void Offset(int coord, double offset);
	void Scale(int coord, double scale);
	void Move(double dx, double dy, double dz);

	// Predefined 预先定义
	int GenerateMap(int line, int col, double min, double max);
	int GenerateBox(double dx, double dy, double dz);
	int GenerateMap(CTexture* pTexture, int width, int height, int FlagColor = 1);	

	// Adjacency  邻接
	int BuildAdjacency();		// 建立邻接
	void Rebuild();				// 重新创建

	// Processing
	int IndexFrom(CFaceTriangles* pFace);		// 查找索引值
	int IndexFrom(CVertex3D* pVertex);			// 查找索引值

	// Modif 修改
	void SetModified(int flag = 1);
	int GetModified();

	// Search 搜索
	int FindVertex(CVertex3D* pVertex);

	// Normals 法线
	void SetNormalBinding(NORMALBINDING type);
	NORMALBINDING GetNormalBinding(void);

	int CalculateNormalPerVertex(void);
	int CalculateNormalPerFace(void);	

	// Sharp edges 锐利边缘
	int ColorSharpEdge(double threshold, CColor& color);

	// Color 颜色
// 	virtual void SetColor(const GLubyte&, const GLubyte&, const GLubyte&);
// 	virtual void SetColor(CColor& color);
// 	virtual void GetColor(GLubyte*) const;
// 	virtual CColor* GetColor(void);	

	// Texture		纹理
	int GetTextureIndex();

	// Subdivision  细分
	int SubdivisionLoop();
	int Subdivision(void); 
	double Alpha(int n);

	// Smoothing 平滑
	int Smooth(int MoveOnBundary = 1);

	// Coloring (curvature, compacity, etc...)
	//void ColorCurvature(CColorRamp* pRamp);			//  曲率
	//void ColorNormalSpace(CColorRamp* pRamp);
	//void ColorCompacity(CColorRamp* pRamp);
	//void ColorHeight(CColorRamp* pRamp);
	
	// Intersection 插值
	int NearestIntersectionWithLine(CVertex3D* pV0, CVertex3D* pV1, CVertex3D* pVertexResult, CFaceTriangles** ppFaceResult, int* pNbFaceVisited);
	
	// Area 面积
	double GetMinArea(CFaceTriangles** ppFace = NULL);
	double GetMeanArea();

	// 保存
	virtual void Serialize(CArchive& ar);
};

#endif // !defined(AFX_STLOBJECT_H__0B30ABE7_430E_41A7_80B1_41426F4C6C87__INCLUDED_)
