#pragma once
#include <Engine/CScript.h>

class CUnitScript;
class CMonsterScript;
class CPlayerScript;

class CGhostScript final
	: public CScript
{
public:
	CGhostScript();
	virtual ~CGhostScript();
	CLONE(CGhostScript);

public:
	void start() override;
	void update() override;

	void OnCollisionEnter(CGameObject* _OtherObject) override;
	void OnCollision(CGameObject* _OtherObject) override;
private:
	CGameObject*   m_pPlayer;
	CPlayerScript* m_pPlayerScript;

	CUnitScript*    m_pUnitScript;
	CMonsterScript* m_pMonsterScript;

	float m_speed;
	float m_attackDelay;
	float m_accTime;
	float m_attackTime;
	float m_attackRange;

	Vec3 m_attackDirection;
};
