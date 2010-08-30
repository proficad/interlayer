// GLTypeEnums.h: Type Enumerations For Graphics.
//
//////////////////////////////////////////////////////////////////////
#ifndef __GLTYPEENUMS_H__
#define __GLTYPEENUMS_H__

//Every CGLObject created in the context must have a unique id;
enum GLObjectType
{
	GLNONE		= 0,
	GLDICE		= 1,						// 切块
	GLSLICE		= 2,						// 切片

	GLCURVE		= 123456,					// 曲线
	GLFONT		= 234561,					// 文字
	GLPLANE		= 345612,					// 平面
	GLPOINT		= 456123,					// 点
	GLSURFACE	= 561234,					// 表面
	GLTRIHEDRON	= 612345,					// 三面体
	GLPIPELINE,								// 三维管子

	GLONEAXIS	= 765432,
	GLAXIS		= 876543,					// 	
	GLUSERDEF	= 875643
};

//Somehow I dont like the GL_POINTS primitive of OpenGL
//So creating some markers for point display
enum GLPointMarker				// 点形状
{
	MARKER_POINT,				// .
	MARKER_PLUS,				// +
	MARKER_CROSS,				// x
	MARKER_STAR,				// *
	MARKER_CIRCLE				// o
};

enum GLLineStyle		// 线
{
	LINE_SOLID,			// _________
	LINE_DOTTED,		// .........
	LINE_DASHED,		// ---------
	LINE_DASHDOTTED		// -.-.-.-.-
};

enum GLDisplayMode
{
	GLSHADED,
	GLWIREFRAME	
};

enum GLSelectedMode
{
	GLNOSELECTED,
	GLSELECTED,
	GLEDIT
};

// Some Standard Materials
// P_ prefix denotes Polished
enum GLMaterial
{
	DEFAULT,
	ALUMINIUM,				// 铝
	BRASS,					// 黄铜
	BRONZE,					// 青铜
	P_BRONZE,				// 亮青铜
	CHROME,					// 铬
	COPPER,					// 铜
	P_COPPER,				// 亮铜
	GOLD,					// 金
	P_GOLD,					// 亮金
	PEWTER,					// 锡
	SILVER,					// 银
	P_SILVER,				// 亮银
	STEEL,					// 钢
	EMERALD,				// 绿宝石
	JADE,					// 翡翠
	OBSIDIAN,				// 黑曜石
	PEARL,					// 珍珠
	RUBY,					// 红宝石
	TURQUOISE,				// 绿松石
	PLASTIC,				// 塑料
	RUBBER,					// 橡胶
	NONE
};

// Normal binding
enum NORMALBINDING
{
	NORMAL_PER_VERTEX,		// 每个顶点法向量
	NORMAL_PER_FACE			// 每个面法向量
};

enum COLORBINDING		// 颜色模式
{
	COLOR_PER_VERTEX,	// 每个顶点颜色
	COLOR_PER_LINE,		// 每条线颜色
	COLOR_PER_FACE		// 每个面颜色
};

#endif