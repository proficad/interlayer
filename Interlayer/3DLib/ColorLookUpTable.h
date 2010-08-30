#pragma once


/*==============================================*
//
//        ����ɫ���ұ���
//
*===============================================*/
struct CColorNode
{
public:
	//��ɫ���е���ɫ�ڵ�
	double pos;        // 0~1 ,��һ������λ��
	COLORREF color;
public:
	CColorNode(double p=0.f,COLORREF clr=0)
		:pos(p),color(clr)
	{
	}
	CColorNode(const CColorNode& node)
	{
		pos = node.pos;
		color = node.color;
	}
	CColorNode& operator=( const CColorNode& node )
	{
		pos = node.pos;
		color = node.color;
		return *this;
	}
public:
	virtual void Serialize(CArchive& ar) // ���л�
	{
		if( ar.IsStoring() )
		{
			ar << pos;
			ar << color;
		}
		else
		{
			ar >> pos;
			ar >> color;
		}
	}

	friend CArchive& operator<<(CArchive& ar, CColorNode& dat);
	friend CArchive& operator>>(CArchive& ar, CColorNode& dat);
};

CArchive& operator<<(CArchive& ar, CColorNode& dat);
CArchive& operator>>(CArchive& ar, CColorNode& dat);

class CColorLookUpTable  
{
public:
	CColorLookUpTable();
	~CColorLookUpTable();

	COLORREF LookUpColor(double v) const;		// ����ɫ���в�ѯvֵ����ɫ
	void AddColor(double v, COLORREF newClr);	// ����ɫ���м�����ɫ

	int GetColorTableCount() { return m_ColorTable.GetSize(); };
	double GetValue(int iIndex) { return m_ValueMin + (m_ColorTable[iIndex].pos*(m_ValueMax-m_ValueMin)); };

	void SetValueRange(double min, double max);
	void GetValueRange(double &min, double &max);

	double GetMaxValue(){return m_ValueMax;}
	double GetMinValue(){return m_ValueMin;}

	COLORREF GetColor(double pos)const;
	void SetColor(double pos, COLORREF newClr);	

	void SetColor(int iIndex, COLORREF newClr)
	{
		m_ColorTable[iIndex].color = newClr;
	}

	int InsertColorIntoTable(double pos, COLORREF newClr);	// ����ɫ���м�����ɫ�ڵ�(�����֤pos��С��������)
	BOOL RemoveColorFromTable(double pos);					// ɾ����ɫ���е���ɫ�ڵ�
	void DrawColorSpectrum(CDC* pDC, const CRect& drawRect, BOOL bHorz = TRUE);//������ɫ�����ɫ��

	BOOL LoadColorTable(const CString& filePath );
	BOOL SaveColorTable(const CString& filePath );
	virtual void Serialize(CArchive& ar); // ���л�
private:
	void GradientRectangle(CDC* pDC, const CRect& drawRect,COLORREF clrStart,COLORREF clrEnd, BOOL bHorz = TRUE);
	
private:
	CArray<CColorNode,CColorNode&> m_ColorTable;	// ��ɫ��(0~1�ڵ�λ��--->RGB��ɫ)
	double m_ValueMin, m_ValueMax;					// ��ֵ�ķ�Χ,�÷�Χ�ڵ���ֵ����ɫ���ڵ���ɫ�໥��Ӧ


public:
	CColorLookUpTable& operator=(const CColorLookUpTable&);
	CColorLookUpTable(const CColorLookUpTable&);
private:
	friend CArchive& operator<<(CArchive& ar, CColorLookUpTable& dat);
	friend CArchive& operator>>(CArchive& ar, CColorLookUpTable& dat);

	CColorLookUpTable *m_pColorTable;
};

CArchive& operator<<(CArchive& ar, CColorLookUpTable& dat);
CArchive& operator>>(CArchive& ar, CColorLookUpTable& dat);
