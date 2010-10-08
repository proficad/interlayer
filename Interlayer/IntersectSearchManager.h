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
	void SetGridModel(CGridObject* model){ m_model = model; }
	void AddLayerModel(CGLObject* layer);

	CGridObject* GetGrid();

	int	GetLayerNumber();
	CGLObject* GetLayer(int index);
	CGLObject* GetLayer(const std::string& name);
	
	void ReleaseAll();
protected:
	CIntersectSearchManager();
	~CIntersectSearchManager();
	CGridObject* m_model;
	std::vector<CGLObject*>   m_interlayers;
};

#endif