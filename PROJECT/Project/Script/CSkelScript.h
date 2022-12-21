#pragma once
#include <Engine/CScript.h>

class CUnitScript;
class CMonsterScript;

class CSkelScript final
	: public CScript
{
public:
	CSkelScript();
	virtual ~CSkelScript();
	CLONE(CSkelScript);

public:
	void start() override;
	void update() override;
private:
	CGameObject* m_pWeapon;
	std::wstring m_keyPrefabSkelSword;

	CUnitScript*    m_pUnitScript;
	CMonsterScript* m_pMonsterScript;
	CGameObject*    m_pPlayer;

	float m_searchRange;
	float m_attackRange;

	float m_speed;
	float m_attackDelay;
	float m_accTime;
};
