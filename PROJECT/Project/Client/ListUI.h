#pragma once
#include "UI.h"

class ListUI :
	public UI
{
private:
	vector<string> m_vecList;

	// 더블클릭 이벤트   
	std::function<void(void* pData)> m_DBCEvent;

public:
	void AddList(const string& _str) { m_vecList.push_back(_str); }
	void AddList(vector<string>& _vecStr) { m_vecList.insert(m_vecList.cend(), _vecStr.begin(), _vecStr.end()); }
	void Clear() { m_vecList.clear(); }

	void SetDBCEvent(std::function<void(void* pData)> _func) { m_DBCEvent = _func; }


public:
	void render_update() override;
	void Activate() override;

public:
	ListUI();
	virtual ~ListUI();
};
