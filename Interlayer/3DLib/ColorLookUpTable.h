#pragma once


/*==============================================*
//
//        『颜色查找表』类
//
*===============================================*/
struct CColorNode
{
public:
	//颜色表中的颜色节点
	double pos;        // 0~1 ,归一化坐标位置
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
	virtual void Serialize(CArchive& ar) // 序列化
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

	COLORREF LookUpColor(double v) const;		// 在颜色表中查询v值的颜色
	void AddColor(double v, COLORREF newClr);	// 向颜色表中加入颜色

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

	int InsertColorIntoTable(double pos, COLORREF newClr);	// 向颜色表中加入颜色节点(插入后保证pos从小到大排序)
	BOOL RemoveColorFromTable(double pos);					// 删除颜色表中的颜色节点
	void DrawColorSpectrum(CDC* pDC, const CRect& drawRect, BOOL bHorz = TRUE);//绘制颜色表的颜色谱

	BOOL LoadColorTable(const CString& filePath );
	BOOL SaveColorTable(const CString& filePath );
	virtual void Serialize(CArchive& ar); // 序列化
private:
	void GradientRectangle(CDC* pDC, const CRect& drawRect,COLORREF clrStart,COLORREF clrEnd, BOOL bHorz = TRUE);
	
private:
	CArray<CColorNode,CColorNode&> m_ColorTable;	// 颜色表(0~1内的位置--->RGB颜色)
	double m_ValueMin, m_ValueMax;					// 数值的范围,该范围内的数值和颜色表内的颜色相互对应


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
