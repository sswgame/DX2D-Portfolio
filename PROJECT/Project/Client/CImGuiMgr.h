#pragma once

#include "UI.h"

struct tUIDelegate
{
	std::function<void(void*)> pFunc;
	void*                      pParameter;
};


class CImGuiMgr final
	: public CSingleton<CImGuiMgr>
{
	SINGLE(CImGuiMgr)
private:
	map<string, UI*>    m_mapUI;
	vector<tUIDelegate> m_vecDelegate;

	HANDLE m_hNotify;

public:
	void init(HWND hwnd);

	void progress();
	void render();
	void clear();

private:
	void CreateUI();
	void ObserveContent();

public:
	UI*  FindUI(const string& key);
	void AddDelegate(const tUIDelegate delegate) { m_vecDelegate.push_back(delegate); }
};
