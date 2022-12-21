#pragma once
#include <Engine/CScript.h>

class CMonsterScript;
class CUnitScript;

class CSkelKnightScript final
	: public CScript
{
public:
	CSkelKnightScript();
	virtual ~CSkelKnightScript();
	CLONE(CSkelKnightScript);

public:
	void start() override;
	void update() override;

private:
	CMonsterScript* m_pMonsterScript;
	CUnitScript*    m_pUnitScript;

	CGameObject* m_pPlayer;
	CGameObject* m_pWeaponColliderObject;

	float m_attackRange;
	float m_accTime;
	float m_speed;
	bool  m_isAttacking;
	Vec3  m_attackDirection;
};
