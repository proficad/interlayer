// GLTypeEnums.h: Type Enumerations For Graphics.
//
//////////////////////////////////////////////////////////////////////
#ifndef __GLTYPEENUMS_H__
#define __GLTYPEENUMS_H__

//Every CGLObject created in the context must have a unique id;
enum GLObjectType
{
	GLNONE		= 0,
	GLDICE		= 1,						// �п�
	GLSLICE		= 2,						// ��Ƭ

	GLCURVE		= 123456,					// ����
	GLFONT		= 234561,					// ����
	GLPLANE		= 345612,					// ƽ��
	GLPOINT		= 456123,					// ��
	GLSURFACE	= 561234,					// ����
	GLTRIHEDRON	= 612345,					// ������
	GLPIPELINE,								// ��ά����

	GLONEAXIS	= 765432,
	GLAXIS		= 876543,					// 	
	GLUSERDEF	= 875643
};

//Somehow I dont like the GL_POINTS primitive of OpenGL
//So creating some markers for point display
enum GLPointMarker				// ����״
{
	MARKER_POINT,				// .
	MARKER_PLUS,				// +
	MARKER_CROSS,				// x
	MARKER_STAR,				// *
	MARKER_CIRCLE				// o
};

enum GLLineStyle		// ��
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
	ALUMINIUM,				// ��
	BRASS,					// ��ͭ
	BRONZE,					// ��ͭ
	P_BRONZE,				// ����ͭ
	CHROME,					// ��
	COPPER,					// ͭ
	P_COPPER,				// ��ͭ
	GOLD,					// ��
	P_GOLD,					// ����
	PEWTER,					// ��
	SILVER,					// ��
	P_SILVER,				// ����
	STEEL,					// ��
	EMERALD,				// �̱�ʯ
	JADE,					// ���
	OBSIDIAN,				// ����ʯ
	PEARL,					// ����
	RUBY,					// �챦ʯ
	TURQUOISE,				// ����ʯ
	PLASTIC,				// ����
	RUBBER,					// ��
	NONE
};

// Normal binding
enum NORMALBINDING
{
	NORMAL_PER_VERTEX,		// ÿ�����㷨����
	NORMAL_PER_FACE			// ÿ���淨����
};

enum COLORBINDING		// ��ɫģʽ
{
	COLOR_PER_VERTEX,	// ÿ��������ɫ
	COLOR_PER_LINE,		// ÿ������ɫ
	COLOR_PER_FACE		// ÿ������ɫ
};

#endif