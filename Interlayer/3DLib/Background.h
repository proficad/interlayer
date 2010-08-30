#pragma once
class CGLView;

struct SCOLOR4F
{
	float r, g, b, a;
};

enum MIRROR_TYPE	{ MIRROR00, MIRROR10, MIRROR01, MIRROR11 };


struct SBACKGROUND_PARAMS
{
	int row_num;
	int col_num;
	SCOLOR4F corner [4];
	MIRROR_TYPE mirror_type;
	CString texture_file;
};

class CBackground
{
private:
	float m_depth;
	int m_row_num;
	int m_col_num;
	bool m_bSmooth;

	SCOLOR4F m_corners[4];
	MIRROR_TYPE m_mirror_type;
	CString m_texture_file;
	GLuint m_textureID;

	unsigned int m_ListOpenGL;			// 显示列表
	unsigned int m_ListDone;			// 列表是否已经完成
	int m_Modified;						// 是否修改了
	int m_Show;							// 是否显示

	bool CreateTexture();
	unsigned char *LoadFromBmp(char *filename, int* bmpWidth, int* bmpHeight);

	bool BuildList();
	void DefineDisplay();

public:
	CBackground();
	~CBackground();
	void SetParams(SBACKGROUND_PARAMS *params);
	void GetParams(SBACKGROUND_PARAMS *params);
	void Display(CGLView *pGLView = NULL);
	void SetDepth(float depth);

	void SetBackgroundColor(GLfloat r, GLfloat g, GLfloat b, bool bFar = true);

	void SetSmoothGround(bool bSmooth) { m_bSmooth = bSmooth; }
	bool GetSmoothGround() { return m_bSmooth; }

	// show
	void Show(int flag)
	{
		m_Show = flag;
	}
	bool isShow()
	{
		return (m_Show != 0);
	}
	// Modif
	void SetModified(int flag = 1)
	{
		m_Modified = flag;
		//m_ListDone = 0;
	}
	int GetModified()
	{
		return m_Modified;
	}
};
