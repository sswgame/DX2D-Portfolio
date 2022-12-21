#pragma once
#include <Engine/CScript.h>

#include "NO_SCRIPT/Define.h"

class CWeaponScript;
class CMonsterHPBarScript;
class CUnitScript;

class CMonsterScript final
	: public CScript
{
public:
	CMonsterScript();
	virtual ~CMonsterScript();
	CLONE(CMonsterScript);

public:
	void start() override;
	void update() override;
	void UpdateData() override;;

	void OnCollisionEnter(CGameObject* _OtherObject) override;

public:
	void         SetMonsterType(MONSTER_TYPE type) { m_monsterType = type; }
	MONSTER_TYPE GetMonsterType() const { return m_monsterType; }

private:
	UINT GetTotalHP();

private:
	std::wstring m_keyPrefabSummon;


	std::wstring m_collisionEffectKey;
	std::wstring m_damageUIKey;

	CUnitScript*         m_pUnitScript;
	CMonsterHPBarScript* m_pMonsterHPScript;
	CGameObject*         m_pHPBar;

	std::wstring m_keyPrefabHPBar;

	MONSTER_TYPE m_monsterType;
	bool         m_isAttacked;
	float        m_accTime;

	CGameObject*   m_pPlayer;
	CWeaponScript* m_pWeaponScript;
};
