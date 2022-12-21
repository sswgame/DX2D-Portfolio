#pragma once
#include <Engine/CScript.h>

class CPlayerScript;

class CDashUIScript final
	: public CScript
{
public:
	CDashUIScript();
	virtual ~CDashUIScript();
	CLONE(CDashUIScript);

public:
	void start() override;

public:
	void SetDashCount(int count);
	void SetMaxDashCount(int count);

private:
	void UpdatePosition();
private:
	CPlayerScript* m_pPlayerScript;

	std::vector<CGameObject*> m_vecOuterDashCount;
	std::vector<CGameObject*> m_vecInnerDashCount;

	int m_prevMaxDashCount;
	int m_prevDashCount;
};
