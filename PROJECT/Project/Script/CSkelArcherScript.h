#pragma once
#include <Engine/CScript.h>

class CUnitScript;
class CMonsterScript;

class CSkelArcherScript final
	: public CScript
{
public:
	CSkelArcherScript();
	virtual ~CSkelArcherScript();
	CLONE(CSkelArcherScript);

public:
	void start() override;
	void update() override;

private:
	CMonsterScript* m_pMonsterScript;
	CUnitScript*    m_pUnitScript;

	CGameObject* m_pBow;
	CGameObject* m_pPlayer;
	float        m_accTime;
	float        m_attackDelay;
	float        m_attackRange;
};
