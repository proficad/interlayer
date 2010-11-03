#include "StdAfx.h"
#include "CoordTrans.h"
#include <math.h>

CCoordConverter::CCoordConverter(void)
{	
}

CCoordConverter::~CCoordConverter(void)
{
	
}

//���溯��
void CCoordConverter::det(double **p,double **b,double **c,int n)
{
	int i,j,k;
	double m;
	//��չΪ�������
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
	//����벿�ֻ�Ϊ��λ����
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

//ת�ú���
void CCoordConverter::transform(double **a,double **b,int m,int n)
{
	int i,j;
	for(i=0;i<m;i++)
		for(j=0;j<n;j++)
		{
			b[j][i]=a[i][j];
		}
}

//�˷�����
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

//����ӷ�����
void CCoordConverter::add(double **a,double **b,double **ab,int m,int n,int sign)
{
	for(int i=0;i<m;i++)
	{
		for(int j=0;j<n;j++)
			ab[i][j]=a[i][j]+pow(-1.0,sign)*b[i][j];     //signΪż���Ǳ�ʾ������ӣ�������ʾ���
	}
}

//void CCoordConverter::Init()
//{
//	//�ͷ��ڴ�
//	m_vecOldPt.erase(m_vecOldPt.begin(), m_vecOldPt.end());
//	m_vecNewPt.erase(m_vecNewPt.begin(), m_vecNewPt.end());
//}
//
//void CCoordConverter::DeleteAllPoint()
//{
//	//�ͷŶ�̬������ڴ�
//	
//	m_vecOldPoint.erase(m_vecOldPoint.begin(), m_vecOldPoint.end());
//	m_vecNewPoint.erase(m_vecNewPoint.begin(), m_vecNewPoint.end());
//}


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
	double **oldp,**newp1;        //������ ������۲�ֵ ������ƽ��ֵ
	
	N = m_vecOldPt.size();
	if( N<3)
		return false;


	oldp=new double*[N];				//�غϵ��ھ�����ϵ�е�����
	for(i=0;i<N;i++)
		oldp[i]=new double[2];
	newp1=new double*[N];				//�غϵ����������ϵ�е�����
	for(i=0;i<N;i++)
		newp1[i]=new double[2];

	double **coneB;						//ϵ������B
	coneB=new double*[2*N];
	for(i=0;i<2*N;i++)
		coneB[i]=new double[4];
	double **TconeB;					//ϵ������B��ת��
	TconeB=new double*[4];
	for(i=0;i<4;i++)
		TconeB[i]=new double[2*N];
	double **consL;						//���������L
	consL=new double*[2*N];
	for(i=0;i<2*N;i++)
		consL[i]=new double[1];
	double **Nbb;						//������ϵ������Nbb
	Nbb=new double*[4];
	for(i=0;i<4;i++)
		Nbb[i]=new double[4];
	double **DNbb;						//������ϵ������Nbb�������
	DNbb=new double*[4];
	for(i=0;i<4;i++)
		DNbb[i]=new double[4];
	double **ENbb;						//������ϵ������Nbb����չ����
	ENbb=new double*[4];
	for(i=0;i<4;i++)
		ENbb[i]=new double[8];
	double **W;							//���ڱ�ʾNbb*TconeB
	W=new double*[4];
	for(i=0;i<4;i++)
		W[i]=new double[2*N];
	double **parameter;					//ƽ�����
	parameter=new double*[4];
	for(i=0;i<4;i++)
		parameter[i]=new double[1];
	

	//����׼������
	for(i=0;i<N;i++)					//�����ı��еľ�����ֵ
	{
		oldp[i][0] = m_vecOldPt[i].x;
		oldp[i][1] = m_vecOldPt[i].y;
	}
	for(i=0;i<2*N;i++)					//д������ϵ������B
	{
		for(j=0;j<4;j++)
		{
			coneB[i][0]=(0.5+pow(-1.,i)*0.5);
			coneB[i][1]=(0.5-pow(-1.,i)*0.5);
			coneB[i][2]=oldp[i/2][0]*(0.5+pow(-1.,i)*0.5)+oldp[i/2][1]*(0.5-pow(-1.,i)*0.5);
			coneB[i][3]=oldp[i/2][0]*(0.5-pow(-1.,i)*0.5)+oldp[i/2][1]*(-0.5-pow(-1.,i)*0.5);
		}
	}

	
	for(i=0;i<N;i++)//�����ı��е�������Ĺ۲�ֵ
	{
		newp1[i][0] = m_vecNewPt[i].x;
		newp1[i][1] = m_vecNewPt[i].y;
	}

	for(i=0;i<2*N;i++)						//д�����������L
	{
		for(j=0;j<1;j++)
			consL[i][j]=newp1[i/2][0]*(0.5+pow(-1.,i)*0.5)+newp1[i/2][1]*(0.5-pow(-1.,i)*0.5);
	}

	//�������ò���
	transform(coneB,TconeB,2*N,4);					//����ת�ú���
	multiplication(TconeB,coneB,Nbb,4,2*N,4);		//���ó˷��������㷨����ϵ��
	det(Nbb,ENbb,DNbb,4);							//�������溯�������㷨����ϵ��������
	multiplication(TconeB,consL,W,4,2*N,1);			//���ó˷����������㳣����W
	multiplication(DNbb,W,parameter,4,4,1);			//���ó˷�����,����ת��ϵ��a,b,c,d
	//
	// KΪ�߶����ӣ���Ϊ��ת�Ƕȣ�Lx\Ly���Ӧ��ƽ�Ʋ���
	//
	// X=Lx + X1*Kcos��- Y1*Ksin��
	// Y=LY + Y1*Kcos��+ X1*Ksin��
	//
	// ת��Ϊ
	//
	// X=a+X1*c-Y1*d
	// Y=b+Y1*c+X1*d
	// 

	m_parameter[0] = parameter[0][0];
	m_parameter[1] = parameter[1][0];
	m_parameter[2] = parameter[2][0];
	m_parameter[3] = parameter[3][0];
	

	for(i=0;i<N;i++)					//�غϵ��ھ�����ϵ�е�����
		delete []oldp[i];
	delete []oldp;

	for(i=0;i<N;i++)					//�غϵ����������ϵ�е�����
		delete []newp1[i];
	delete []newp1;

	for(i=0;i<2*N;i++)					//ϵ������B
		delete []coneB[i];
	delete []coneB;

	for(i=0;i<4;i++)					//ϵ������B��ת��
		delete []TconeB[i];
	delete []TconeB;

	for(i=0;i<2*N;i++)					//���������L
		delete []consL[i];
	delete []consL;

	for(i=0;i<4;i++)					//������ϵ������Nbb
		delete []Nbb[i];

	for(i=0;i<4;i++)					//������ϵ������Nbb�������
		delete []DNbb[i];
	delete []DNbb;

	for(i=0;i<4;i++)					//������ϵ������Nbb����չ����
		delete []ENbb[i];
	delete []ENbb;

	for(i=0;i<4;i++)					//���ڱ�ʾNbb*TconeB
		delete []W[i];
	delete []W;

	for(i=0;i<4;i++)					//ƽ�����
		delete []parameter[i];
	delete []parameter;

	return true;
}

//void CCoordConverter::AddTransPoint(const CPoint2D &pt) // �����Ҫת���ľ�����
//{
//	CPoint2D point = pt;
//	m_vecOldPoint.push_back(point);
//}
//
//bool CCoordConverter::Transform()						// ת������,���سɹ���־
//{
//	int M = m_vecOldPoint.size();
//	if( M<1)
//		return false;
//
//	m_vecNewPoint.erase(m_vecNewPoint.begin(), m_vecNewPoint.end());
//
//	for(int i=0;i<M;i++)
//	{	
//		CPoint2D pt;
//		pt.x = m_parameter[0]+m_vecOldPoint[i].x*m_parameter[2]-m_vecOldPoint[i].y*m_parameter[3];
//		pt.y = m_parameter[1]+m_vecOldPoint[i].y*m_parameter[2]+m_vecOldPoint[i].x*m_parameter[3];
//		m_vecNewPoint.push_back(pt);
//	}
//
//	return true;
//}
//
//int CCoordConverter::GetPointCount()					// ��Ҫת���ľ�����ĵ���
//{
//	return m_vecOldPoint.size();
//}
//
//CPoint2D CCoordConverter::GetOldPoint(int index)		// ��Ҫת���ľ�����
//{
//	CPoint2D pt = m_vecOldPoint[index];
//	return pt;
//}
//
//CPoint2D CCoordConverter::GetNewPoint(int index)		// �����꾭ת�����������
//{
//	CPoint2D pt = m_vecNewPoint[index];
//	return pt;
//}

///������ת����
CCoordConverterNew::CCoordConverterNew( void )
{

}

CCoordConverterNew::CCoordConverterNew(double *screen2ground, double *ground2screen )
{
	m_ground2screen[0] = ground2screen[0];
	m_ground2screen[1] = ground2screen[1];
	m_ground2screen[2] = ground2screen[2];

	m_ground2screen[3] = ground2screen[3];
	m_ground2screen[4] = ground2screen[4];
	m_ground2screen[5] = ground2screen[5];

	m_screen2ground[0] = screen2ground[0];
	m_screen2ground[1] = screen2ground[1];
	m_screen2ground[2] = screen2ground[2];

	m_screen2ground[3] = screen2ground[3];
	m_screen2ground[4] = screen2ground[4];
	m_screen2ground[5] = screen2ground[5];
}

CCoordConverterNew::~CCoordConverterNew( void )
{

}

void CCoordConverterNew::AddCoincidence( const CPoint2D &ground, const CPoint2D &screen )
{
	CPoint2D pt = ground;
	m_vecGroundPt.push_back(pt);
	pt = screen;
	m_vecScreenPt.push_back(pt);
}

bool CCoordConverterNew::Prepare()
{
	int n = m_vecGroundPt.size();	
	if( n<3)
		return false;
	double **wt = new double*[n];
	double **shl = new double*[n];
	for(int i=0; i<n; i++)
	{
		wt[i] = new double[2];
		shl[i] = new double[2];
		
		wt[i][0] = m_vecScreenPt[i].x;
		wt[i][1] = m_vecScreenPt[i].y;

		shl[i][0] = m_vecGroundPt[i].x;
		shl[i][1] = m_vecGroundPt[i].y;
	}


	double xs[100][6],chsh[100],zhgxs[6][6],zhgchsh[6];

	for(int i=0;i<n;i++)
	{ 				
		xs[2*i][0]=wt[i][0];
		xs[2*i][1]=0;
		xs[2*i][2]=wt[i][1];
		xs[2*i][3]=0;
		xs[2*i][4]=1;
		xs[2*i][5]=0;

		chsh[2*i]=shl[i][0];

		xs[2*i+1][0]=0;
		xs[2*i+1][1]=wt[i][0];
		xs[2*i+1][2]=0;
		xs[2*i+1][3]=wt[i][1];
		xs[2*i+1][4]=0;
		xs[2*i+1][5]=1;

		chsh[2*i+1]=shl[i][1];

	}	

	//��ֵ�����淽�����ϵ������ͳ�������	
	for(int k=0;k<6;k++)
	{
		for(int j=0;j<6;j++)
		{
			zhgxs[k][j]=0;
			for(int s=0;s<2*n;s++)
			{			  
				zhgxs[k][j]+=xs[s][k]*xs[s][j];
			}
		}

		zhgchsh[k]=0;
		for(int s=0;s<2*n;s++)
		{
			zhgchsh[k]+=xs[s][k]*chsh[s];
		}
	}   

	//��Ļ�����
	//��6Ԫ���淽����
	lzyxq(zhgxs, zhgchsh); 
	hdai(zhgxs, zhgchsh, m_screen2ground);

	delete [] wt;
	delete [] shl;

	wt = new double*[n];
	shl = new double*[n];
	for(int i=0; i<n; i++)
	{
		wt[i] = new double[2];
		shl[i] = new double[2];

		shl[i][0] = m_vecScreenPt[i].x;
		shl[i][1] = m_vecScreenPt[i].y;

		wt[i][0] = m_vecGroundPt[i].x;
		wt[i][1] = m_vecGroundPt[i].y;
	}

	for(int i=0;i<n;i++)
	{ 				
		xs[2*i][0]=wt[i][0];
		xs[2*i][1]=0;
		xs[2*i][2]=wt[i][1];
		xs[2*i][3]=0;
		xs[2*i][4]=1;
		xs[2*i][5]=0;

		chsh[2*i]=shl[i][0];

		xs[2*i+1][0]=0;
		xs[2*i+1][1]=wt[i][0];
		xs[2*i+1][2]=0;
		xs[2*i+1][3]=wt[i][1];
		xs[2*i+1][4]=0;
		xs[2*i+1][5]=1;

		chsh[2*i+1]=shl[i][1];

	}	

	//��ֵ�����淽�����ϵ������ͳ�������	
	for(int k=0;k<6;k++)
	{
		for(int j=0;j<6;j++)
		{
			zhgxs[k][j]=0;
			for(int s=0;s<2*n;s++)
			{			  
				zhgxs[k][j]+=xs[s][k]*xs[s][j];
			}

		}

		zhgchsh[k]=0;
		for(int s=0;s<2*n;s++)
		{
			zhgchsh[k]+=xs[s][k]*chsh[s];
		}
	}   

	//��Ļ�����
	//��6Ԫ���淽����
	lzyxq(zhgxs, zhgchsh); 
	hdai(zhgxs, zhgchsh, m_ground2screen);

	delete [] wt;
	delete [] shl;

	return true;
}

void CCoordConverterNew::lzyxq(double a[][6],double *b) 
{ 
	int i,j,m; 
	double l; 
	for(m=0;m<6-1;m++) 
	{ 
		lzy(a,b,m); 
		for(i=m+1;i<6;i++) 
		{ 
			l=a[i][m]/a[m][m]; 
			b[i]-=l*b[m]; 
			for(j=m;j<6;j++) 
			{ 
				a[i][j]-=l*a[m][j]; 
			} 
		}
	} 
}

void CCoordConverterNew::lzy(double a[][6],double *b,int m) 
{ 
	int i,j,t; 
	float tempa,tempb; 
	t=m; 
	tempa=a[t][m]; 

	for(i=m+1;i<6;i++) 
	{ 
		if(fabs(tempa)<fabs(a[i][m])) 
		{ 
			tempa=a[i][m]; 
			t=i; 
		} 
	} 

	if(t!=m) 
	{ 
		for(j=0;j<6;j++) 
		{ 
			tempa=a[m][j]; 
			a[m][j]=a[t][j]; 
			a[t][j]=tempa; 
		} 

		tempb=b[m]; 
		b[m]=b[t]; 
		b[t]=tempb; 
	} 
}

void CCoordConverterNew::hdai(double a[][6], double *b, double *bhcsh)
{  
	int i,j; 
	double sum; 
	bhcsh[5]=b[5]/a[5][5]; 
	for(i=6-2;i>=0;i--) 
	{ 
		sum=0; 
		for(j=i+1;j<6;j++) 
		{
			sum+=a[i][j]*bhcsh[j]; 

		}
		bhcsh[i]=(b[i]-sum)/a[i][i]; 
	} 
}

CPoint2D CCoordConverterNew::Ground2Screen( const CPoint2D& ground )
{
	CPoint2D screen;
	screen.x = ground.x * m_ground2screen[0] + ground.y * m_ground2screen[2] + m_ground2screen[4];
	screen.y = ground.x * m_ground2screen[1] + ground.y * m_ground2screen[3] + m_ground2screen[5];
	return screen;
}

CPoint2D CCoordConverterNew::Screen2Ground( const CPoint2D& screen )
{
	CPoint2D ground;
	ground.x = screen.x * m_screen2ground[0] + screen.y * m_screen2ground[2] + m_screen2ground[4];
	ground.y = screen.x * m_screen2ground[1] + screen.y * m_screen2ground[3] + m_screen2ground[5];
	return ground;
}