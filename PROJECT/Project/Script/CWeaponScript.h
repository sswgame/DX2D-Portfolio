#pragma once
#include <Engine/CScript.h>

#include "CAttackScript.h"


class CItemScript;

class CWeaponScript final
	: public CScript
{
public:
	CWeaponScript();
	CWeaponScript(const CWeaponScript& other);
	virtual ~CWeaponScript();
	CLONE(CWeaponScript);

public:
	void start() override;
	void update() override;

	void SaveToScene(FILE* _pFile) override;
	void LoadFromScene(FILE* _pFile) override;
public:
	void SetDamage(UINT damage) { m_damage = damage; }
	UINT GetDamage() const { return m_damage + rand() % 10; }

	void  SetAttackSpeed(float speed) { m_attackSpeed = speed; }
	float GetAttackSpeed() const { return m_attackSpeed; }

	float GetAttackDelay() const { return m_attackDelay; }

	void  SetDelay(float delay) { m_attackDelay = delay; }
	float GetDelay() const { return m_attackDelay; }

	void  SetSkillCoolTime(float delay) { m_skillCoolTime = delay; }
	float GetSkillCoolTime() const { return m_skillCoolTime; }

	bool CanAttack() const { return m_canAttack; }
	void SetCanAttack(bool enable) { m_canAttack = enable; }

	void InstanciateEffect(const Vec3& position, const Vec3& rotation);
	bool IsEffectCreated() const { return m_pAttackObject != nullptr; }

	const std::wstring& GetIconName() const { return m_keyPrefabIcon; }
private:
	std::wstring   m_keyPrefabEffect;
	CGameObject*   m_pAttackObject;
	CAttackScript* m_pAttackScript;


	CItemScript* m_pItemScript;

	std::wstring m_keyPrefabIcon;
	std::wstring m_keySound;

	UINT  m_damage;
	float m_attackSpeed;
	float m_attackDelay;
	float m_skillCoolTime;

	bool m_canAttack;
};
