#pragma once

class CGameObject;

class CEventMgr
	: public CSingleton<CEventMgr>
{
	SINGLE(CEventMgr)
private:
	vector<tEventInfo>   m_vecEvent;
	vector<CGameObject*> m_vecDead;

	bool m_bObjEvn;
	bool m_change;
	tEventInfo m_sceneChangeInfo;
public:
	void update();

public:
	void AddEvent(const tEventInfo& _event)
	{
		m_vecEvent.push_back(_event);
	}
	bool HasOccurObjEvent() const { return m_bObjEvn; }
};
