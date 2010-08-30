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
	void DeleteAllPoint();					// ɾ����Ҫת���ľ����꼰ת����������
	void AddTransPoint(const CPoint2D &pt); // �����Ҫת���ľ�����
	bool Transform();						// ת������,���سɹ���־

	int GetPointCount();					// ��Ҫת���ľ�����ĵ���
	CPoint2D GetOldPoint(int index);		// ��Ҫת���ľ�����
	CPoint2D GetNewPoint(int index);		// �����꾭ת�����������
	double GetParameter(int index) { return m_parameter[index]; }
private:
	std::vector<CPoint2D> m_vecOldPt;		//�غϵ��ھ�����ϵ�е�����
	std::vector<CPoint2D> m_vecNewPt;		//�غϵ����������ϵ�е�����
	double m_parameter[4];					//ƽ���Ĳ���
	//------------------------------------------------------------------------
	std::vector<CPoint2D> m_vecOldPoint;	//���ڴ洢��Ҫת���ľ�����
	std::vector<CPoint2D> m_vecNewPoint;	//���ڴ洢�����꾭ת�����������
private:
	void det(double **p,double **b,double **c,int n);							//���溯��
	void transform(double **a,double **b,int m,int n);							//��ת�õĺ���
	void multiplication(double **a,double **b,double **c,int m,int s,int n);	//������˺���
	void add(double **a,double **b,double **ab,int m,int n,int sign);			//����ӡ�������
};
