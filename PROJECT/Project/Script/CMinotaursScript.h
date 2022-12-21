#pragma once
#include <Engine/CScript.h>


class CUnitScript;
class CMonsterScript;

class CMinotaursScript final
	: public CScript
{
public:
	CMinotaursScript();
	virtual ~CMinotaursScript();
	CLONE(CMinotaursScript);

public:
	void start() override;
	void update() override;

	void OnCollisionEnter(CGameObject* _OtherObject) override;
	void OnCollision(CGameObject* _OtherObject) override;
	void OnCollisionExit(CGameObject* _OtherObject) override;

private:
	CMonsterScript* m_pMonsterScript;
	CUnitScript*    m_pUnitScript;

	CGameObject* m_pPlayer;
	CGameObject* m_pWeaponColliderObject;

	float m_speed;
	float m_searchRange;
	float m_attackRange;
	float m_accTime;
	float m_attackDelay;
	bool  m_isSkillUsed;

	Vec3 m_attackDirection;
};
