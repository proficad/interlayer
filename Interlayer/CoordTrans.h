#pragma once
#include <vector>

class CCoordConverter // 坐标转换器
{
public:
	CCoordConverter(void);
	~CCoordConverter(void);
public:
	// 按以下顺序调用
	void Init();		// 初始化转换器
	void AddCoincidence(const CPoint2D &oldPt, const CPoint2D &newPt); // 添加重合点坐标
	bool Prepare();		// 准备转换 
	
	//------------------------------------------------------------------------
	// 
	//void DeleteAllPoint();					// 删除需要转换的旧坐标及转换后新坐标
	//void AddTransPoint(const CPoint2D &pt); // 添加需要转换的旧坐标
	//bool Transform();						// 转换坐标,返回成功标志

	//int GetPointCount();					// 需要转换的旧坐标的点数
	//CPoint2D GetOldPoint(int index);		// 需要转换的旧坐标
	//CPoint2D GetNewPoint(int index);		// 旧坐标经转换后的新坐标
	double GetParameter(int index) { return m_parameter[index]; }
private:
	std::vector<CPoint2D> m_vecOldPt;		//重合点在旧坐标系中的坐标
	std::vector<CPoint2D> m_vecNewPt;		//重合点的在新坐标系中的坐标
	double m_parameter[4];					//平差四参数
	//------------------------------------------------------------------------
	//std::vector<CPoint2D> m_vecOldPoint;	//用于存储需要转换的旧坐标
	//std::vector<CPoint2D> m_vecNewPoint;	//用于存储旧坐标经转换后的新坐标
private:
	void det(double **p,double **b,double **c,int n);							//求逆函数
	void transform(double **a,double **b,int m,int n);							//求转置的函数
	void multiplication(double **a,double **b,double **c,int m,int s,int n);	//矩阵相乘函数
	void add(double **a,double **b,double **ab,int m,int n,int sign);			//矩阵加、减函数
};


class CCoordConverterNew
{
public:
	CCoordConverterNew(void);
	CCoordConverterNew(double *screen2ground, double *ground2screen);
	~CCoordConverterNew(void);
public:
	void AddCoincidence(const CPoint2D &ground, const CPoint2D &screen);	// 添加重合点坐标
	bool Prepare();																							// 准备转换
	CPoint2D Ground2Screen(const CPoint2D& ground);
	CPoint2D Screen2Ground(const CPoint2D& screen);

	double GetG2SParameter(int index) { return m_ground2screen[index]; }		//获得第n个参数 ground大地 到 screen屏幕
	double GetS2GParameter(int index) { return m_screen2ground[index]; }		//获得第n个参数 screen屏幕 到 ground大地
protected:
	void hdai(double a[][6],double *b, double *m_bhcsh);//回代求解 
	void lzyxq(double a[][6],double *b);//高斯变换
	void lzy(double a[][6],double *b,int m);//行变换 

	std::vector<CPoint2D> m_vecGroundPt;															//重合点在旧坐标系中的坐标
	std::vector<CPoint2D> m_vecScreenPt;														//重合点的在新坐标系中的坐标

	double							m_ground2screen[6];												//大地到屏幕
	double							m_screen2ground[6];												//屏幕到大地
};