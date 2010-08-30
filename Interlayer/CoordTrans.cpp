#include "StdAfx.h"
#include "CoordTrans.h"
#include <math.h>

CCoordConverter::CCoordConverter(void)
{	
}

CCoordConverter::~CCoordConverter(void)
{
	
}

//求逆函数
void CCoordConverter::det(double **p,double **b,double **c,int n)
{
	int i,j,k;
	double m;
	//扩展为增广矩阵
	for(i=0;i<n;i++)              
	{
		for(j=0;j<n;j++)
		{
			b[i][j]=p[i][j];
		}
		for(j=n;j<2*n;j++)
		{
			if(i+n==j)
				b[i][j]=1;
			else
				b[i][j]=0;
		}
	}
	//将左半部分化为单位矩阵
	for(i=0;i<n;i++)
	{ 
		m=b[i][i];
		for(j=i;j<n*2;j++)
			b[i][j]/=m;
		for(j=i+1;j<n;j++)
		{ 
			m=b[j][i];
			for(k=i;k<n*2;k++)
				b[j][k]=b[j][k]-m*b[i][k];
		}
	}
	for(i=n-1;i>=0;i--)
	{ 
		m=b[i][i];
		for(j=i-1;j>=0;j--)
		{ 
			m=b[j][i];
			for(k=i;k<n*2;k++)
				b[j][k]=b[j][k]-m*b[i][k];
		}
	}
	for(i=0;i<n;i++)
	{  
		for(j=n;j<2*n;j++)
		{
			c[i][j-n]=b[i][j];
		}
	}
}

//转置函数
void CCoordConverter::transform(double **a,double **b,int m,int n)
{
	int i,j;
	for(i=0;i<m;i++)
		for(j=0;j<n;j++)
		{
			b[j][i]=a[i][j];
		}
}

//乘法函数
void CCoordConverter::multiplication(double **a,double **b,double **c,int m,int s,int n)
{
	int i,j,k;
	double sum;
	for(i=0;i<m;i++)
	{
		for(j=0;j<n;j++)
		{
			sum=0.0;
			for(k=0;k<s;k++)
				sum+=a[i][k]*b[k][j];
			c[i][j]=sum;
		}
	}
}

//矩阵加法函数
void CCoordConverter::add(double **a,double **b,double **ab,int m,int n,int sign)
{
	for(int i=0;i<m;i++)
	{
		for(int j=0;j<n;j++)
			ab[i][j]=a[i][j]+pow(-1.0,sign)*b[i][j];     //sign为偶数是表示矩阵相加，奇数表示相减
	}
}

void CCoordConverter::Init()
{
	//释放内存
	m_vecOldPt.erase(m_vecOldPt.begin(), m_vecOldPt.end());
	m_vecNewPt.erase(m_vecNewPt.begin(), m_vecNewPt.end());
}

void CCoordConverter::DeleteAllPoint()
{
	//释放动态分配的内存
	
	m_vecOldPoint.erase(m_vecOldPoint.begin(), m_vecOldPoint.end());
	m_vecNewPoint.erase(m_vecNewPoint.begin(), m_vecNewPoint.end());
}


void CCoordConverter::AddCoincidence(const CPoint2D &oldPt, const CPoint2D &newPt)
{
	CPoint2D pt = oldPt;
	m_vecOldPt.push_back(pt);
	pt = newPt;
	m_vecNewPt.push_back(pt);
}

bool CCoordConverter::Prepare()
{
	int i,j,N;
	double **oldp,**newp1;        //旧坐标 新坐标观测值 新坐标平差值
	
	N = m_vecOldPt.size();
	if( N<3)
		return false;


	oldp=new double*[N];				//重合点在旧坐标系中的坐标
	for(i=0;i<N;i++)
		oldp[i]=new double[2];
	newp1=new double*[N];				//重合点的在新坐标系中的坐标
	for(i=0;i<N;i++)
		newp1[i]=new double[2];

	double **coneB;						//系数矩阵B
	coneB=new double*[2*N];
	for(i=0;i<2*N;i++)
		coneB[i]=new double[4];
	double **TconeB;					//系数矩阵B的转置
	TconeB=new double*[4];
	for(i=0;i<4;i++)
		TconeB[i]=new double[2*N];
	double **consL;						//常数项矩阵L
	consL=new double*[2*N];
	for(i=0;i<2*N;i++)
		consL[i]=new double[1];
	double **Nbb;						//法方程系数矩阵Nbb
	Nbb=new double*[4];
	for(i=0;i<4;i++)
		Nbb[i]=new double[4];
	double **DNbb;						//法方程系数矩阵Nbb的逆矩阵
	DNbb=new double*[4];
	for(i=0;i<4;i++)
		DNbb[i]=new double[4];
	double **ENbb;						//法方程系数矩阵Nbb的扩展矩阵
	ENbb=new double*[4];
	for(i=0;i<4;i++)
		ENbb[i]=new double[8];
	double **W;							//用于表示Nbb*TconeB
	W=new double*[4];
	for(i=0;i<4;i++)
		W[i]=new double[2*N];
	double **parameter;					//平差参数
	parameter=new double*[4];
	for(i=0;i<4;i++)
		parameter[i]=new double[1];
	

	//数据准备部分
	for(i=0;i<N;i++)					//读入文本中的旧坐标值
	{
		oldp[i][0] = m_vecOldPt[i].x;
		oldp[i][1] = m_vecOldPt[i].y;
	}
	for(i=0;i<2*N;i++)					//写出方程系数矩阵B
	{
		for(j=0;j<4;j++)
		{
			coneB[i][0]=(0.5+pow(-1.,i)*0.5);
			coneB[i][1]=(0.5-pow(-1.,i)*0.5);
			coneB[i][2]=oldp[i/2][0]*(0.5+pow(-1.,i)*0.5)+oldp[i/2][1]*(0.5-pow(-1.,i)*0.5);
			coneB[i][3]=oldp[i/2][0]*(0.5-pow(-1.,i)*0.5)+oldp[i/2][1]*(-0.5-pow(-1.,i)*0.5);
		}
	}

	
	for(i=0;i<N;i++)//读入文本中的新坐标的观测值
	{
		newp1[i][0] = m_vecNewPt[i].x;
		newp1[i][1] = m_vecNewPt[i].y;
	}

	for(i=0;i<2*N;i++)						//写出常数项矩阵L
	{
		for(j=0;j<1;j++)
			consL[i][j]=newp1[i/2][0]*(0.5+pow(-1.,i)*0.5)+newp1[i/2][1]*(0.5-pow(-1.,i)*0.5);
	}

	//函数调用部分
	transform(coneB,TconeB,2*N,4);					//调用转置函数
	multiplication(TconeB,coneB,Nbb,4,2*N,4);		//调用乘法函数计算法方程系数
	det(Nbb,ENbb,DNbb,4);							//调用求逆函数，计算法方程系数的逆阵
	multiplication(TconeB,consL,W,4,2*N,1);			//调用乘法函数，计算常数项W
	multiplication(DNbb,W,parameter,4,4,1);			//调用乘法函数,解算转换系数a,b,c,d
	//
	// K为尺度因子，ａ为旋转角度，Lx\Ly相对应的平移参数
	//
	// X=Lx + X1*Kcosａ- Y1*Ksinａ
	// Y=LY + Y1*Kcosａ+ X1*Ksinａ
	//
	// 转化为
	//
	// X=a+X1*c-Y1*d
	// Y=b+Y1*c+X1*d
	// 

	m_parameter[0] = parameter[0][0];
	m_parameter[1] = parameter[1][0];
	m_parameter[2] = parameter[2][0];
	m_parameter[3] = parameter[3][0];
	

	for(i=0;i<N;i++)					//重合点在旧坐标系中的坐标
		delete []oldp[i];
	delete []oldp;

	for(i=0;i<N;i++)					//重合点的在新坐标系中的坐标
		delete []newp1[i];
	delete []newp1;

	for(i=0;i<2*N;i++)					//系数矩阵B
		delete []coneB[i];
	delete []coneB;

	for(i=0;i<4;i++)					//系数矩阵B的转置
		delete []TconeB[i];
	delete []TconeB;

	for(i=0;i<2*N;i++)					//常数项矩阵L
		delete []consL[i];
	delete []consL;

	for(i=0;i<4;i++)					//法方程系数矩阵Nbb
		delete []Nbb[i];

	for(i=0;i<4;i++)					//法方程系数矩阵Nbb的逆矩阵
		delete []DNbb[i];
	delete []DNbb;

	for(i=0;i<4;i++)					//法方程系数矩阵Nbb的扩展矩阵
		delete []ENbb[i];
	delete []ENbb;

	for(i=0;i<4;i++)					//用于表示Nbb*TconeB
		delete []W[i];
	delete []W;

	for(i=0;i<4;i++)					//平差参数
		delete []parameter[i];
	delete []parameter;

	return true;
}

void CCoordConverter::AddTransPoint(const CPoint2D &pt) // 添加需要转换的旧坐标
{
	CPoint2D point = pt;
	m_vecOldPoint.push_back(point);
}

bool CCoordConverter::Transform()						// 转换坐标,返回成功标志
{
	int M = m_vecOldPoint.size();
	if( M<1)
		return false;

	m_vecNewPoint.erase(m_vecNewPoint.begin(), m_vecNewPoint.end());

	for(int i=0;i<M;i++)
	{	
		CPoint2D pt;
		pt.x = m_parameter[0]+m_vecOldPoint[i].x*m_parameter[2]-m_vecOldPoint[i].y*m_parameter[3];
		pt.y = m_parameter[1]+m_vecOldPoint[i].y*m_parameter[2]+m_vecOldPoint[i].x*m_parameter[3];
		m_vecNewPoint.push_back(pt);
	}

	return true;
}

int CCoordConverter::GetPointCount()					// 需要转换的旧坐标的点数
{
	return m_vecOldPoint.size();
}

CPoint2D CCoordConverter::GetOldPoint(int index)		// 需要转换的旧坐标
{
	CPoint2D pt = m_vecOldPoint[index];
	return pt;
}

CPoint2D CCoordConverter::GetNewPoint(int index)		// 旧坐标经转换后的新坐标
{
	CPoint2D pt = m_vecNewPoint[index];
	return pt;
}