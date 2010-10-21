#pragma once

#ifndef _INTER_SECT_SEARCH_
#define _INTER_SECT_SEARCH_

class CGridObject;
class CGLObject;
class CVector3DF;

//WinExec(_T("f:\\testcreate.exe 实验"), SW_SHOW);

class CIntersectSearchManager
{
public:
	static CIntersectSearchManager* Instance()
	{
		static CIntersectSearchManager instance;
		return &instance;
	}

	bool LoadGridModel(const std::string& filename,const std::string& outfilename);
	bool SearchInterSect();

	//void SetGridModel(CGridObject* model){ m_model = model; }
	void SetGridModelName(const std::string& filename){m_gridFilename=filename;}
	std::string GetGridModelName(){return m_gridFilename;}

	void AddLayerModel(CGLObject* layer);

	CGridObject* GetGrid();

	int	GetLayerNumber();
	CGLObject* GetLayer(int index);
	CGLObject* GetLayer(const std::string& name);

	//设置物理参数
	void			SetPhyParaMin(double minpara){m_phyparaMin = minpara;}
	void			SetPhyParaMax(double maxpara){m_phyparaMax = maxpara;}
	double		GetPhyParaMin(){return m_phyparaMin;}
	double		GetPhyParaMax(){return m_phyparaMax;}
	//参数的计算
	bool			ComputePara2(const std::string& file1, const std::string& file2, const std::string& savefile,
											const std::string& op1, const std::string& op2, const std::string& op,
											float para1, float para2);
	bool			ComputePara1(const std::string& file, float para, const std::string& savefile, const std::string& op);
	//导出加密三角形
	bool			ExportTriangle(const std::string& input, const std::string& output, int size, int index);
	
	void			ReleaseAll();
protected:
	CIntersectSearchManager();
	~CIntersectSearchManager();
	void SearchALayer(CGLObject* gird, int index);
	CGridObject* m_model;
	std::string		 m_gridFilename;
	std::vector<std::string>  	 m_interlayerNames;
	std::vector<CGLObject*>   m_interlayers;

	double			m_phyparaMin;
	double			m_phyparaMax;
};

#endif