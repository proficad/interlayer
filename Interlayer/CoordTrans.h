#pragma once
#include <vector>

class CCoordConverter // ����ת����
{
public:
	CCoordConverter(void);
	~CCoordConverter(void);
public:
	// ������˳�����
	void Init();		// ��ʼ��ת����
	void AddCoincidence(const CPoint2D &oldPt, const CPoint2D &newPt); // ����غϵ�����
	bool Prepare();		// ׼��ת�� 
	
	//------------------------------------------------------------------------
	// 
	//void DeleteAllPoint();					// ɾ����Ҫת���ľ����꼰ת����������
	//void AddTransPoint(const CPoint2D &pt); // �����Ҫת���ľ�����
	//bool Transform();						// ת������,���سɹ���־

	//int GetPointCount();					// ��Ҫת���ľ�����ĵ���
	//CPoint2D GetOldPoint(int index);		// ��Ҫת���ľ�����
	//CPoint2D GetNewPoint(int index);		// �����꾭ת�����������
	double GetParameter(int index) { return m_parameter[index]; }
private:
	std::vector<CPoint2D> m_vecOldPt;		//�غϵ��ھ�����ϵ�е�����
	std::vector<CPoint2D> m_vecNewPt;		//�غϵ����������ϵ�е�����
	double m_parameter[4];					//ƽ���Ĳ���
	//------------------------------------------------------------------------
	//std::vector<CPoint2D> m_vecOldPoint;	//���ڴ洢��Ҫת���ľ�����
	//std::vector<CPoint2D> m_vecNewPoint;	//���ڴ洢�����꾭ת�����������
private:
	void det(double **p,double **b,double **c,int n);							//���溯��
	void transform(double **a,double **b,int m,int n);							//��ת�õĺ���
	void multiplication(double **a,double **b,double **c,int m,int s,int n);	//������˺���
	void add(double **a,double **b,double **ab,int m,int n,int sign);			//����ӡ�������
};


class CCoordConverterNew
{
public:
	CCoordConverterNew(void);
	CCoordConverterNew(double *screen2ground, double *ground2screen);
	~CCoordConverterNew(void);
public:
	void AddCoincidence(const CPoint2D &ground, const CPoint2D &screen);	// ����غϵ�����
	bool Prepare();																							// ׼��ת��
	CPoint2D Ground2Screen(const CPoint2D& ground);
	CPoint2D Screen2Ground(const CPoint2D& screen);

	double GetG2SParameter(int index) { return m_ground2screen[index]; }		//��õ�n������ ground��� �� screen��Ļ
	double GetS2GParameter(int index) { return m_screen2ground[index]; }		//��õ�n������ screen��Ļ �� ground���
protected:
	void hdai(double a[][6],double *b, double *m_bhcsh);//�ش���� 
	void lzyxq(double a[][6],double *b);//��˹�任
	void lzy(double a[][6],double *b,int m);//�б任 

	std::vector<CPoint2D> m_vecGroundPt;															//�غϵ��ھ�����ϵ�е�����
	std::vector<CPoint2D> m_vecScreenPt;														//�غϵ����������ϵ�е�����

	double							m_ground2screen[6];												//��ص���Ļ
	double							m_screen2ground[6];												//��Ļ�����
};