#pragma once

#ifndef _INTER_SECT_SEARCH_
#define _INTER_SECT_SEARCH_

class CGridObject;
class CGLObject;
class CVector3DF;
class CModelView;
//WinExec(_T("f:\\testcreate.exe ʵ��"), SW_SHOW);

class CIntersectSearchManager
{
public:
	static CIntersectSearchManager* Instance()
	{
		static CIntersectSearchManager instance;
		return &instance;
	}

	bool LoadGridModel(const std::string& filename,const std::string& outfilename);
	UINT SearchInterSect(LPVOID pParam);

	//void SetGridModel(CGridObject* model){ m_model = model; }
	void SetGridModelName(const std::string& filename,const std::string& guid, CString pathname){m_gridFilename=filename;m_gridModelTreeItemGuid=guid; m_pathname=pathname;}
	std::string GetGridModelName(){return m_gridFilename;}

	void AddLayerModel(CGLObject* layer);

	CGridObject* GetGrid();

	int	GetLayerNumber();
	CGLObject* GetLayer(int index);
	CGLObject* GetLayer(const std::string& name);

	//�����������
	void			SetPhyParaMin(double minpara){m_phyparaMin = minpara;}
	void			SetPhyParaMax(double maxpara){m_phyparaMax = maxpara;}
	double		GetPhyParaMin(){return m_phyparaMin;}
	double		GetPhyParaMax(){return m_phyparaMax;}
	//�����ļ���
	bool			ComputePara2(const std::string& file1, const std::string& file2, const std::string& savefile,
											const std::string& op1, const std::string& op2, const std::string& op,
											float para1, float para2);
	bool			ComputePara1(const std::string& file, float para, const std::string& savefile, const std::string& op);
	//�����ữ
	bool			AveragePara(const std::string& filename, const std::string& savename, int type);
	//��������������
	bool			ExportTriangle(const std::string& input, const std::string& output, int size, int index);
	//������
	void			RunCommond(const std::string& command);

	void			ReleaseAll();
protected:
	CIntersectSearchManager();
	~CIntersectSearchManager();
	void SearchALayer(CGLObject* gird);
	CGridObject* m_model;
	CString			m_pathname;
	std::string		 m_gridFilename;
	std::string		 m_gridModelTreeItemGuid;
	std::vector<std::string>  	 m_interlayerNames;
	std::vector<CGLObject*>   m_interlayers;
	std::vector<std::string>		m_tempTriangleFiles;

	double			m_phyparaMin;
	double			m_phyparaMax;
};


class CExportManager
{
public:
	static CExportManager* Instance()
	{
		static CExportManager instance;
		return &instance;
	}
	//����
	bool SetExportGridFileName(const std::string& filename);

	bool SetRedefine(bool isRedefine);
	bool SetWriteProperty(bool isWrite);
	bool SetRedefineProperty(bool isWrite);

	bool AddProName(const std::string& filename);
	bool	ClearProNames();
	int	GetProNameNums();
	bool	RemoveProNameAt(int index);

	bool AddTrackName(const std::string& filename);
	bool	ClearTrackNames();
	int	GetTrackNameNums();
	bool	RemoveTrackNameAt(int index);

	bool	WriteExport(const std::string& filename);
	bool WriteCenterPoints(const std::string& filename, const std::string& saveFileName, bool isWriteTick, int number);

protected:
	CExportManager(){}
	~CExportManager(){}
};
#endif