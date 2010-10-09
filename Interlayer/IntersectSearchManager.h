#pragma once

#ifndef _INTER_SECT_SEARCH_
#define _INTER_SECT_SEARCH_

class CGridObject;
class CGLObject;

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

	void SetGridModel(CGridObject* model){ m_model = model; }
	void SetGridModelName(const std::string& filename){m_gridFilename=filename;}
	std::string GetGridModelName(){return m_gridFilename;}

	void AddLayerModel(CGLObject* layer);

	CGridObject* GetGrid();

	int	GetLayerNumber();
	CGLObject* GetLayer(int index);
	CGLObject* GetLayer(const std::string& name);
	
	void ReleaseAll();
protected:
	CIntersectSearchManager();
	~CIntersectSearchManager();
	void SearchALayer(CGLObject* gird);
	CGridObject* m_model;
	std::string		 m_gridFilename;
	std::vector<CGLObject*>   m_interlayers;
};

#endif