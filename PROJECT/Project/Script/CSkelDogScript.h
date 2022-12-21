#pragma once
#include <Engine/CScript.h>

class CMonsterScript;
class CUnitScript;
class CPlayerScript;

class CSkelDogScript final
	: public CScript
{
public:
	CSkelDogScript();
	virtual ~CSkelDogScript();
	CLONE(CSkelDogScript);

public:
	void start() override;
	void update() override;

	void OnCollisionExit(CGameObject* _OtherObject) override;
private:
	CGameObject*   m_pPlayer;
	CPlayerScript* m_pPlayerScript;

	CUnitScript*    m_pUnitScript;
	CMonsterScript* m_pMonsterScript;

	float m_speed;
	float m_attackDelay;
	float m_accTime;
	float m_attackRange;
	float m_searchRange;
	bool  m_isAttacking;
	bool  m_attackOnce = false;
	Vec3 m_attackDirection;
};
